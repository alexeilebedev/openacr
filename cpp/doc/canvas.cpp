// Copyright (C) 2026 AlgoRND
//
// License: GPL
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
// Target: doc (exe) -- Render a markdown document to the terminal
// Exceptions: yes
// Source: cpp/doc/canvas.cpp
//
// The page as a grid of cells, and the one place that turns cells into escapes.
// A rendered document is a rectangle of cells, each carrying a character, a
// style, a colour and the link it belongs to.  Nothing downstream of this file
// reasons about escape sequences: the painter walks a row and emits a sequence
// only where the style changes, so a style that opens inside another one cannot
// close it by accident, and no code has to remember which off-code cancels
// which.
// Two things fall out of the grid that a string of text cannot give.  A column
// is a cell, so every width is exact by counting rather than by measuring a
// string with escapes and utf-8 in it.  And an overlay -- the selected link,
// the hits of a search -- is a style applied at paint time to whichever cells
// qualify, computed on the way to the screen and stored nowhere.

#include "include/algo.h"
#include "include/doc.h"

// Drop the page: its cells, its rows and its links.
void doc::CanvasClear() {
    doc::cell_RemoveAll();
    doc::row_RemoveAll();
    doc::link_RemoveAll();
    doc::anchor_RemoveAll();
    doc::_db.rowbegin = 0;
    doc::_db.listtop = -1;
    doc::_db.listbase = 1;
}

// Append one cell to the row being built.
void doc::PutCell(u32 ch, doc::Attr attr, doc::Color color, int link) {
    doc::Cell &cell = doc::cell_Alloc();
    cell.ch = ch;
    cell.attr = attr;
    cell.color = color;
    cell.link = link;
}

// Close the row being built and start the next one.
//
// Trailing blanks are dropped as the row closes.  Padding is emitted freely
// while a table row is laid out, and a row that kept it would end every line
// with spaces that show up as trailing whitespace in a capture and as a wide
// selection on the screen.
void doc::EndRow() {
    while (doc::cell_N() > doc::_db.rowbegin && doc::cell_qFind(doc::cell_N() - 1).ch == ' ') {
        doc::cell_RemoveLast();
    }
    doc::Row &row = doc::row_Alloc();
    row.begin = doc::_db.rowbegin;
    row.n = i32(doc::cell_N() - doc::_db.rowbegin);
    doc::_db.rowbegin = i32(doc::cell_N());
}

// Return the number of cells already in the row being built.
int doc::RowWidth() {
    return i32(doc::cell_N() - doc::_db.rowbegin);
}

// Append N blank cells to the row being built.
void doc::PutSpace(int n) {
    frep_(i, n) {
        doc::PutCell(' ', doc::Attr(doc_Attr_plain), doc::Color(doc_Color_default), 0);
    }
}

// Append blanks until the row being built is COL cells wide.
void doc::PadTo(int col) {
    doc::PutSpace(i32_Max(0, col - doc::RowWidth()));
}

// Decode the utf-8 sequence of TEXT at POS into CH and return the offset just
// past it.  A byte that starts no valid sequence is passed through as itself,
// so malformed input costs one garbled character rather than a desynchronized
// scan.
int doc::Utf8Get(algo::strptr text, int pos, u32 &ch) {
    int len = algo::Utf8SeqLen(text, pos);
    int ret = pos + i32_Max(1, len);
    ch = u8(text.elems[pos]);
    if (len == 2) {
        ch = ((ch & 0x1f) << 6) | (u8(text.elems[pos + 1]) & 0x3f);
    } else if (len == 3) {
        ch = ((ch & 0x0f) << 12) | ((u8(text.elems[pos + 1]) & 0x3f) << 6) | (u8(text.elems[pos + 2]) & 0x3f);
    } else if (len == 4) {
        ch = ((ch & 0x07) << 18) | ((u8(text.elems[pos + 1]) & 0x3f) << 12)
            | ((u8(text.elems[pos + 2]) & 0x3f) << 6) | (u8(text.elems[pos + 3]) & 0x3f);
    }
    return i32_Min(ret, ch_N(text));
}

// Append the characters of TEXT to RUN, all carrying the same style.
void doc::PutRunText(doc::FRun &run, algo::strptr text, doc::Attr attr, doc::Color color, int link) {
    int i = 0;
    while (i < ch_N(text)) {
        u32 ch = 0;
        i = doc::Utf8Get(text, i, ch);
        doc::Cell &cell = ary_Alloc(run);
        cell.ch = ch;
        cell.attr = attr;
        cell.color = color;
        cell.link = link;
    }
}

// Append cells BEGIN through BEGIN+N of RUN to the row being built.
void doc::PutRun(doc::FRun &run, int begin, int n) {
    frep_(i, n) {
        doc::Cell &src = ary_qFind(run, begin + i);
        doc::PutCell(src.ch, src.attr, src.color, src.link);
    }
}

// Encode the code point CH as utf-8 and append it to OUT.
void doc::PrintUtf8(u32 ch, algo::cstring &out) {
    if (ch < 0x80) {
        out << char(ch);
    } else if (ch < 0x800) {
        out << char(0xc0 | (ch >> 6)) << char(0x80 | (ch & 0x3f));
    } else if (ch < 0x10000) {
        out << char(0xe0 | (ch >> 12)) << char(0x80 | ((ch >> 6) & 0x3f)) << char(0x80 | (ch & 0x3f));
    } else {
        out << char(0xf0 | (ch >> 18)) << char(0x80 | ((ch >> 12) & 0x3f));
        out << char(0x80 | ((ch >> 6) & 0x3f)) << char(0x80 | (ch & 0x3f));
    }
}

// Append to OUT the escape sequence that puts a terminal into style ATTR and
// colour COLOR, having been in no particular style before.
//
// The sequence always opens with a reset, so what it selects does not depend on
// what was selected before it.  That is what lets the painter emit one of these
// wherever two adjacent cells differ and be certain the result is right,
// without tracking which of several overlapping spans a cell belongs to.
//
// Emphasis is painted dim, and the italic code is never emitted.  SGR 3 is italic
// on a terminal that has the face and reverse video on one that does not, and a
// terminal cannot be asked which it is doing.  Reverse video is loud enough that a
// reader takes an emphasized word for an error, so the quieter of the two faces is
// the one worth having: dim differs from the body text without shouting, and an
// emphasis lost among the dim of a comment costs nothing a reader was reading for.
//
// The colour is written as an index into the 256-colour palette rather than as
// one of the eight codes from 30 to 37.  The first eight indices name the same
// eight colours, so nothing that used them changed; what the wider form buys is
// the two hundred and forty above them, which is where a page finds a pale blue
// that is not the blue beside it.  Eight foreground colours are not enough to
// give a record's type tag, its attribute names and its values each a colour a
// reader can tell apart at a glance.
static void PrintStyle(doc::Attr attr, doc::Color color, algo::cstring &out) {
    bool faint = (attr.value & (doc_Attr_dim | doc_Attr_italic)) != 0;
    out << "\033[0";
    out << (attr.value & doc_Attr_bold ? ";1" : "");
    out << (faint ? ";2" : "");
    out << (attr.value & doc_Attr_underline ? ";4" : "");
    out << (attr.value & doc_Attr_reverse ? ";7" : "");
    if (color.value != doc_Color_default) {
        out << ";38;5;" << i32(color.value);
    }
    out << "m";
}

// Append the characters of TEXT to the row being built, all carrying the same
// style and all belonging to LINK, which defaults to no link at all.
void doc::PutText(algo::strptr text, doc::Attr attr, doc::Color color, int link DFLTVAL(0)) {
    int i = 0;
    while (i < ch_N(text)) {
        u32 ch = 0;
        i = doc::Utf8Get(text, i, ch);
        doc::PutCell(ch, attr, color, link);
    }
}

// Record where each link's first cell landed, so the reader can be taken to a
// link without the page being searched for it.
//
// Position is not known while a link is rendered.  Wrapping moves text after
// the fact, and half the links in this corpus sit in table cells, which are
// wrapped one at a time and then printed column by column -- so the order links
// reach the page is not the order they were rendered in.  Reading the finished
// page settles it for both cases at once.
void doc::LocateLink() {
    frep_(irow, doc::row_N()) {
        doc::Row &row = doc::row_qFind(irow);
        frep_(icol, row.n) {
            doc::Cell &cell = doc::cell_qFind(row.begin + icol);
            bool first = cell.link > 0 && doc::link_qFind(cell.link - 1).row == 0 && doc::link_qFind(cell.link - 1).col == 0;
            if (first) {
                doc::link_qFind(cell.link - 1).row = i32(irow);
                doc::link_qFind(cell.link - 1).col = i32(icol);
            }
        }
    }
}

// Return true when an occurrence of the text being searched for begins exactly
// at column ICOL of row IROW.
//
// This is the test the reader steps by, where HitQ is the test the painter
// highlights by: stepping wants one position per occurrence, while painting
// wants every cell the occurrence covers.
bool doc::HitStartQ(int irow, int icol) {
    algo::strptr find = doc::_db.find;
    doc::Row &row = doc::row_qFind(irow);
    bool ret = ch_N(find) > 0 && icol + ch_N(find) <= row.n;
    for (int i = 0; i < ch_N(find) && ret; i++) {
        ret = doc::cell_qFind(row.begin + icol + i).ch == u32(u8(find.elems[i]));
    }
    return ret;
}

// Return true when the cell at column ICOL of row IROW falls inside an
// occurrence of the text the reader is searching for.
//
// The comparison is over the row's characters rather than over the painted
// output, so a hit is found the same way whether or not the run it lands in is
// coloured, and a match that straddles two styles is still one match.
bool doc::HitQ(int irow, int icol) {
    algo::strptr find = doc::_db.find;
    doc::Row &row = doc::row_qFind(irow);
    bool ret = false;
    int begin = i32_Max(0, icol - ch_N(find) + 1);
    for (int at = begin; at <= icol && !ret; at++) {
        bool fits = ch_N(find) > 0 && at + ch_N(find) <= row.n;
        bool same = fits;
        for (int i = 0; i < ch_N(find) && same; i++) {
            same = doc::cell_qFind(row.begin + at + i).ch == u32(u8(find.elems[i]));
        }
        ret = same;
    }
    return ret;
}

// Paint row IROW into OUT, giving every cell whose link is SELECT, and every
// cell inside a hit recorded by the search, the reverse style on the way past.
// NCOL is the number of cells there is room for, or zero for all of them.
//
// SELECT is the one-based index of the selected link, or zero when nothing is
// selected.  Every hit of the search is drawn in reverse and the one the reader
// is standing on is drawn bold as well, since a screen with four identical
// highlights on it does not say which one the arrows will move away from.  The
// overlay is applied here and nowhere else: no cell is modified, so moving the
// selection is a repaint rather than a re-render.
//
// NCOL is what stops a row wider than the window from wrapping onto a second
// display line.  A record copied from acr runs to a hundred and forty columns,
// and a screenful of those on an eighty-column terminal asks it to draw
// thirty-one lines in a twenty-four line window: the terminal scrolls seven
// lines, every frame, and the reader sees the whole page jump.  Cutting each row
// at the edge of the window is what a pager does with long lines, and it makes
// the frame exactly as tall as the screen.  The plain-text path passes zero,
// since its output is somebody's file rather than a window.
void doc::PaintRow(int irow, int select, algo::cstring &out, int ncol DFLTVAL(0)) {
    doc::Row &row = doc::row_qFind(irow);
    doc::Attr attr(doc_Attr_plain);
    doc::Color color(doc_Color_default);
    bool styled = false;
    frep_(i, ncol > 0 ? i32_Min(row.n, ncol) : row.n) {
        doc::Cell &cell = doc::cell_qFind(row.begin + i);
        doc::Attr want = cell.attr;
        want.value |= (select > 0 && cell.link == select) ? doc_Attr_reverse : 0;
        bool hit = doc::HitQ(irow, i);
        bool here = hit && irow == doc::_db.hitrow && i >= doc::_db.hitcol && i < doc::_db.hitcol + ch_N(doc::_db.find);
        want.value |= hit ? doc_Attr_reverse : 0;
        want.value |= here ? doc_Attr_bold : 0;
        bool change = !styled || want != attr || cell.color != color;
        if (doc::_db.color && change) {
            attr = want;
            color = cell.color;
            styled = true;
            PrintStyle(attr, color, out);
        }
        doc::PrintUtf8(cell.ch, out);
    }
    if (doc::_db.color && styled) {
        out << "\033[0m";
    }
}

// Paint every row of the page into OUT, one line each.
void doc::PaintPage(algo::cstring &out) {
    frep_(irow, doc::row_N()) {
        doc::PaintRow(i32(irow), 0, out);
        out << eol;
    }
}

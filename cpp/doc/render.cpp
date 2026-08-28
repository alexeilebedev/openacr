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
// Source: cpp/doc/render.cpp
//
// Turn the parsed blocks into cells on the page.
// The layout follows a man page, because a man page is what a reader already
// knows how to look at: headings sit at the left margin, body text is indented
// under them, and the measure stays narrow enough to read in one eye movement
// however wide the window is.  Where it goes further is colour, which roff
// cannot carry -- a code span, a link and a listing each get their own.
// Everything here works in cells rather than in characters of a string, so a
// column is a cell and every width is a count.  Wrapping a run therefore needs
// no notion of which bytes are escapes and which are the tail of a utf-8
// sequence; those questions belong to canvas.cpp and stop at its edge.

#include "include/algo.h"
#include "include/doc.h"

// Return the substring of TEXT beginning at byte POS.
algo::strptr doc::RestFrom(algo::strptr text, int pos) {
    return algo::strptr(text.elems + pos, i32_Max(0, ch_N(text) - pos));
}

// Return the byte offset of the next MARK in IN at or after POS, or the length
// of IN when there is none.  An unterminated span then renders as plain text
// to the end of the block rather than swallowing the rest of the document.
int doc::FindClose(algo::strptr in, int pos, algo::strptr mark) {
    int ret = ch_N(in);
    int i = pos;
    while (i + ch_N(mark) <= ch_N(in) && ret == ch_N(in)) {
        if (algo::strptr(in.elems + i, ch_N(mark)) == mark) {
            ret = i;
        }
        i++;
    }
    return ret;
}

// Record a link on TARGET and return its one-based index, or zero when the
// target is one a reader at a terminal cannot be taken to.
//
// An http target is not recorded.  Following it would mean launching a browser
// from a documentation reader, and a link that highlights and then refuses to
// open is worse than one that never offered.
int doc::NewLink(algo::strptr target) {
    bool external = StartsWithQ(target, "http:") || StartsWithQ(target, "https:") || StartsWithQ(target, "mailto:");
    int ret = 0;
    if (!external && ch_N(target) > 0) {
        doc::FLink &link = doc::link_Alloc();
        link.target = target;
        ret = i32(doc::link_N());
    }
    return ret;
}

// Render the markdown inline markup of IN into RUN, every cell carrying ATTR, COLOR and
// LINK unless the markup changes them.
//
// The markup is read into spans first, which is where the syntax lives, so what is left
// here is the terminal's own vocabulary: bold and emphasis are attributes, a code span is
// cyan, and a link is underlined text pointing at a record the reader can follow.
//
// A link becomes one record however many spans its text turned out to be, since the
// reader selects and follows a link rather than a run of cells.  The span's own number
// says when a new link has started, which two adjacent links to the same target would
// not.
void doc::RenderInline(algo::strptr in, doc::Attr attr, doc::Color color, int link, doc::FRun &run) {
    doc::ReadInline(in);
    int ilink = 0;
    int idx = link;
    ind_beg(doc::_db_span_curs, span, doc::_db) {
        if (span.ilink != ilink) {
            ilink = span.ilink;
            idx = ilink > 0 ? doc::NewLink(span.target) : link;
        }
        doc::Attr spanattr(attr);
        spanattr.value |= span.bold ? doc_Attr_bold : 0;
        spanattr.value |= span.italic ? doc_Attr_italic : 0;
        spanattr.value |= ilink > 0 ? doc_Attr_underline : 0;
        doc::Color spancolor(span.code ? doc::Color(doc_Color_cyan) : color);
        doc::PutRunText(run, span.br ? algo::strptr("\n") : algo::strptr(span.text), spanattr, spancolor, idx);
    }ind_end;
}

// Work out where RUN breaks into lines of at most WIDTH cells, recording each
// line's first cell in RUN's break array.
//
// A newline cell is a break the author asked for and is always taken.  Other
// breaks go at a blank, and a word longer than the measure -- a path, a long
// option -- is left to overhang rather than being cut, because a cut path is
// no longer a path a reader can copy.
void doc::WrapCompute(doc::FRun &run, int width) {
    brk_RemoveAll(run);
    brk_Alloc(run) = 0;
    int col = 0;
    int word = -1;
    frep_(i, ary_N(run)) {
        doc::Cell &cell = ary_qFind(run, i);
        bool blank = cell.ch == ' ';
        bool brk = cell.ch == '\n';
        word = blank || brk ? -1 : (word < 0 ? i32(i) : word);
        bool over = col >= width && !blank && !brk;
        bool cutable = over && word > brk_qFind(run, brk_N(run) - 1);
        if (brk) {
            brk_Alloc(run) = i32(i) + 1;
            col = 0;
            word = -1;
        } else if (cutable) {
            brk_Alloc(run) = word;
            col = i32(i) - word + 1;
        } else {
            col++;
        }
    }
}

// Return the number of cells on line ILINE of RUN.
static int LineN(doc::FRun &run, int iline) {
    i64 end = iline + 1 < brk_N(run) ? brk_qFind(run, iline + 1) : ary_N(run);
    return i32(end - brk_qFind(run, iline));
}

// Put line ILINE of RUN onto the page at column INDENT, without closing the row.
// Leading blanks of a wrapped line are dropped, since they are the space the
// line was broken at.
static void PutLine(doc::FRun &run, int iline, int indent) {
    int begin = brk_qFind(run, iline);
    int n = LineN(run, iline);
    while (n > 0 && (ary_qFind(run, begin).ch == ' ' || ary_qFind(run, begin).ch == '\n')) {
        begin++;
        n--;
    }
    while (n > 0 && ary_qFind(run, begin + n - 1).ch == '\n') {
        n--;
    }
    doc::PadTo(indent);
    doc::PutRun(run, begin, n);
}

// Lay RUN onto the page wrapped to WIDTH, with MARKER at INDENT on the first
// line and the lines after it starting under the marker's text.  With REPEAT
// the marker is drawn on every line instead.
//
// The two forms are the two things a marker can mean.  A list bullet names the
// item once and its continuation lines hang under the item's words, which is
// what keeps a wrapped item reading as one item.  A quotation's bar is not a
// name but an edge, and an edge that stopped after the first line would leave
// the rest of the quotation looking like the prose around it.
void doc::WrapRun(doc::FRun &run, int indent, algo::strptr marker, int width, bool repeat DFLTVAL(false)) {
    doc::WrapCompute(run, width);
    frep_(iline, brk_N(run)) {
        doc::PadTo(indent);
        if (iline == 0 || repeat) {
            doc::PutRunOwn(marker);
        } else {
            doc::PutSpace(doc::MarkerWidth(marker));
        }
        PutLine(run, i32(iline), doc::RowWidth());
        doc::EndRow();
    }
}

// Put the plain text MARKER onto the page as unstyled cells.
void doc::PutRunOwn(algo::strptr marker) {
    int i = 0;
    while (i < ch_N(marker)) {
        u32 ch = 0;
        i = doc::Utf8Get(marker, i, ch);
        doc::PutCell(ch, doc::Attr(doc_Attr_plain), doc::Color(doc_Color_default), 0);
    }
}

// Return the number of cells the plain text MARKER occupies.
int doc::MarkerWidth(algo::strptr marker) {
    int ret = 0;
    int i = 0;
    while (i < ch_N(marker)) {
        u32 ch = 0;
        i = doc::Utf8Get(marker, i, ch);
        ret++;
    }
    return ret;
}

// Lay RUN onto the page at column INDENT, breaking only where the run itself
// carries a newline.  A code listing is poured rather than wrapped: its lines
// were laid out by whoever wrote them, and rewrapping them would change what
// the code means.
// Return the target of the heading of this page anchored at NAME, and the empty string
// when no heading of it is.
//
// The blocks of the page being drawn are what is asked, so this answers about the document
// a listing sits in rather than about the tree.  A page that documents its flags under
// headings anchored by the flag therefore links its usage block to them, and a page that
// does not says nothing about a word that merely looks like one.
tempstr doc::Headanchor(algo::strptr name) {
    bool found = false;
    ind_beg(doc::_db_block_curs, block, doc::_db) {
        found = found || (block.blocktype == doc_Blocktype_heading && block.anchor == name);
    }ind_end;
    tempstr ret;
    if (found) {
        ret << "#" << name;
    }
    return ret;
}

void doc::PourRun(doc::FRun &run, int indent) {
    doc::PadTo(indent);
    frep_(i, ary_N(run)) {
        doc::Cell &cell = ary_qFind(run, i);
        if (cell.ch == '\n') {
            doc::EndRow();
            doc::PadTo(indent);
        } else {
            doc::PutCell(cell.ch, cell.attr, cell.color, cell.link);
        }
    }
    doc::EndRow();
}

// Drop the empty rows at the end of the page.
static void Trim() {
    while (doc::row_N() > 0 && doc::row_qFind(doc::row_N() - 1).n == 0) {
        doc::row_RemoveLast();
    }
}

// Leave exactly one empty row at the end of the page, and none while the page
// is still empty.
//
// Blocks are separated by one blank line and each block emits none of its own,
// so the separation is decided in one place.  Two headings in a row is the case
// that shows why it has to be: a document whose title is followed straight away
// by its contents heading otherwise takes a blank line from the block above and
// another from the block below, and the gap reads as a missing section.
static void Gap() {
    Trim();
    if (doc::row_N() > 0) {
        doc::EndRow();
    }
}

// Return true when CELL is a column separator of a pipe table: dashes, with an
// optional colon at either end to state the column's justification.
static bool SepCellQ(algo::strptr cell) {
    algo::strptr trim = algo::Trimmed(cell);
    int ndash = 0;
    frep_(i, ch_N(trim)) {
        ndash += trim.elems[i] == '-' ? 1 : 0;
    }
    bool colon = ch_N(trim) > 0 && (trim.elems[0] == ':' || trim.elems[ch_N(trim) - 1] == ':');
    return ndash > 0 && ndash + (colon ? 1 : 0) >= ch_N(trim);
}

// Return the byte offset just past the pipe table row LINE's leading delimiter.
// Return the byte offset of the first cell of the table row LINE, past the pipe
// that opens it when the author wrote one.
int doc::RowBegin(algo::strptr line) {
    return ch_N(line) > 0 && line.elems[0] == '|' ? 1 : 0;
}

// Return the byte offset of the pipe table row LINE's trailing delimiter.
// Return the byte offset just past the last cell of the table row LINE, before the
// pipe that closes it when the author wrote one.
int doc::RowEnd(algo::strptr line) {
    return ch_N(line) > 0 && line.elems[ch_N(line) - 1] == '|' ? ch_N(line) - 1 : ch_N(line);
}

// Return true when every cell of the pipe table row LINE is a separator, which
// is how markdown marks the row above as the header.
// Return true when LINE is the separator row markdown uses to mark the row above
// it as the header.  It carries no content, so it is dropped once it has served
// that purpose -- on the terminal and in the browser alike.
bool doc::SepRowQ(algo::strptr line) {
    int i = doc::RowBegin(line);
    int cell = i;
    bool ret = doc::RowEnd(line) > i;
    while (i <= doc::RowEnd(line)) {
        bool split = i == doc::RowEnd(line) || line.elems[i] == '|';
        if (split) {
            ret = ret && SepCellQ(algo::strptr(line.elems + cell, i - cell));
            cell = i + 1;
        }
        i++;
    }
    return ret;
}

// Render every cell of the pipe table row LINE into its own run, tagged with its
// row and column.  The header row sets NCOL; every row after it is cut to that
// many cells.
//
// The header decides how wide a table is, and a later row may disagree with it.  A
// generated table declared four columns -- ctype, ssimfile, create, access -- and then
// gave a ctype with several access paths a cell for each, so one row ran to nine.
// Taking the widest row instead would spread a hundred columns of
// measure across nine of them, and the four that were named come out four
// characters wide with their contents run together: the table stops being
// readable to save cells that have no heading over them.  A markdown renderer
// for the web makes the same choice, so the page and the file agree about what
// the table says.
static void RenderRow(algo::strptr line, int irow, int &ncol) {
    int i = doc::RowBegin(line);
    int cell = i;
    int icol = 0;
    doc::Attr attr(irow == 0 ? doc_Attr_bold : doc_Attr_plain);
    while (i <= doc::RowEnd(line)) {
        bool escaped = i > 0 && line.elems[i - 1] == '\\';
        bool split = (i == doc::RowEnd(line) || line.elems[i] == '|') && !escaped;
        if (split) {
            if (irow == 0 || icol < ncol) {
                doc::FRun &run = doc::run_Alloc();
                run.irow = irow;
                run.icol = icol;
                doc::RenderInline(algo::Trimmed(algo::strptr(line.elems + cell, i - cell)), attr, doc::Color(doc_Color_default), 0, run);
            }
            cell = i + 1;
            icol++;
        }
        i++;
    }
    ncol = irow == 0 ? icol : ncol;
}

// Choose how wide each column of the rendered table prints, so the table as a
// whole fits AVAIL cells, and record the answer on every run of that column.
//
// The natural width of a column is its widest cell, and a table whose columns
// all fit is printed at those widths.  The corpus under txt/ does not
// cooperate: the median table row is a hundred columns wide and the widest is
// eight hundred, so most tables have to give ground somewhere.  Taking it from
// the widest column each time keeps a narrow column of flags at its natural
// width while a column of prose wraps, which is the split a reader wants --
// shrinking every column by the same fraction would wrap the flags too, and a
// wrapped flag is unreadable.
static void FitColumn(int ncol, int avail, doc::WidthAry &width) {
    frep_(icol, ncol) {
        ary_Alloc(width) = 1;
    }
    ind_beg(doc::_db_run_curs, run, doc::_db) {
        i32 &want = ary_qFind(width, run.icol);
        want = i32_Max(want, i32(ary_N(run)));
    }ind_end;
    int budget = avail - 3 * (ncol - 1);
    int total = 0;
    frep_(icol, ncol) {
        total += ary_qFind(width, icol);
    }
    int floor_w = i32_Min(8, i32_Max(1, budget / i32_Max(1, ncol)));
    bool progress = true;
    while (total > budget && progress) {
        int worst = -1;
        frep_(icol, ncol) {
            bool better = ary_qFind(width, icol) > floor_w && (worst < 0 || ary_qFind(width, icol) > ary_qFind(width, worst));
            worst = better ? i32(icol) : worst;
        }
        progress = worst >= 0;
        if (progress) {
            ary_qFind(width, worst) -= 1;
            total -= 1;
        }
    }
    ind_beg(doc::_db_run_curs, run, doc::_db) {
        run.width = ary_qFind(width, run.icol);
    }ind_end;
}

// Lay the pipe table TEXT onto the page, indented by INDENT and fitted to the
// resolved measure.  The separator row that markdown uses to mark the header
// carries no content, so it is dropped once it has served that purpose.
void doc::RenderTable(algo::strptr text, int indent) {
    doc::run_RemoveAll();
    int ncol = 0;
    int nrow = 0;
    ind_beg(algo::Line_curs, line, text) {
        algo::strptr body = algo::Trimmed(line);
        if (!doc::SepRowQ(body)) {
            RenderRow(body, nrow, ncol);
            nrow++;
        }
    }ind_end;
    doc::WidthAry width;
    if (ncol > 0 && nrow > 0) {
        FitColumn(ncol, doc::_db.width - indent, width);
        frep_(irow, nrow) {
            int depth = 1;
            ind_beg(doc::_db_run_curs, run, doc::_db) if (run.irow == irow) {
                doc::WrapCompute(run, run.width);
                depth = i32_Max(depth, i32(brk_N(run)));
            }ind_end;
            frep_(iline, depth) {
                int col = indent;
                frep_(icol, ncol) {
                    ind_beg(doc::_db_run_curs, run, doc::_db) if (run.irow == irow && run.icol == icol) {
                        if (iline < brk_N(run)) {
                            PutLine(run, i32(iline), col);
                        }
                    }ind_end;
                    col += ary_qFind(width, icol) + 3;
                }
                doc::EndRow();
            }
            if (irow == 0) {
                int col = indent;
                frep_(icol, ncol) {
                    doc::PadTo(col);
                    frep_(i, ary_qFind(width, icol)) {
                        doc::PutCell(0x2500, doc::Attr(doc_Attr_dim), doc::Color(doc_Color_default), 0);
                    }
                    col += ary_qFind(width, icol) + 3;
                }
                doc::EndRow();
            }
        }
    }
    doc::run_RemoveAll();
}

// Lay every parsed block onto the page.
//
// The indentation follows a man page.  A top-level heading sits at the left
// margin where the eye finds it while scrolling, a deeper heading is inset by
// two, and body text is inset by three under all of them.  A listing goes one
// step further in, which is what separates it from the prose around it even
// with colour turned off.
//
// Ordered items are numbered from the running count of adjacent items at the
// same depth rather than from the digits the author typed.  Markdown files are
// edited by inserting lines, so those digits are wrong as often as not.
void doc::RenderDoc() {
    algo_lib::Regx section;
    algo_lib::Regx_ReadSql(section, doc::_db.cmdline.section, true);
    bool filter = ch_N(doc::_db.cmdline.section) > 0;
    bool show = !filter;
    int number = 0;
    bool listed = false;
    ind_beg(doc::_db_block_curs, block, doc::_db) {
        int level = block.level;
        bool heading = block.blocktype == doc_Blocktype_heading;
        bool item = block.blocktype == doc_Blocktype_bullet || block.blocktype == doc_Blocktype_number;
        if (heading && filter) {
            doc::FRun &plain = doc::run_Alloc();
            doc::RenderInline(block.text, doc::Attr(doc_Attr_plain), doc::Color(doc_Color_default), 0, plain);
            algo::tempstr flat;
            frep_(i, ary_N(plain)) {
                flat << char(ary_qFind(plain, i).ch);
            }
            show = algo_lib::Regx_Match(section, flat);
            doc::run_RemoveAll();
        }
        number = block.blocktype == doc_Blocktype_number ? number + 1 : 0;
        if (show) {
            int indent = 3 + 2 * level;
            if (item && listed) {
                Trim();
            } else {
                Gap();
            }
            listed = item;
            if (heading) {
                doc::FRun &run = doc::run_Alloc();
                doc::Attr attr(doc_Attr_bold);
                if (ch_N(block.anchor) > 0) {
                    doc::PutAnchor(block.anchor);
                }
                doc::RenderInline(block.text, attr, doc::Color(doc_Color_amber), 0, run);
                doc::WrapRun(run, level <= 2 ? 0 : 2 * (level - 2), "", doc::_db.width);
            } else if (block.blocktype == doc_Blocktype_code) {
                if (ch_N(block.cmd) > 0) {
                    doc::PutNote(doc::Querytext(block.cmd));
                }
                doc::FRun &run = doc::run_Alloc();
                doc::Highlight(block.lang, block.text, run);
                doc::_db.listtop = doc::block_N() == 1 ? i32(doc::row_N()) : doc::_db.listtop;
                doc::PourRun(run, 6);
            } else if (block.blocktype == doc_Blocktype_table) {
                doc::RenderTable(block.text, 3);
            } else if (block.blocktype == doc_Blocktype_rule) {
                doc::PadTo(3);
                frep_(i, i32_Max(1, doc::_db.width - 6)) {
                    doc::PutCell(0x2500, doc::Attr(doc_Attr_dim), doc::Color(doc_Color_default), 0);
                }
                doc::EndRow();
            } else if (block.blocktype == doc_Blocktype_quote) {
                doc::FRun &run = doc::run_Alloc();
                doc::Attr attr(doc_Attr_dim);
                doc::RenderInline(block.text, attr, doc::Color(doc_Color_default), 0, run);
                doc::WrapRun(run, 3, "│ ", doc::_db.width - 5, true);
            } else if (item) {
                doc::FRun &run = doc::run_Alloc();
                algo::tempstr marker;
                if (block.blocktype == doc_Blocktype_number) {
                    marker << number << ". ";
                } else {
                    marker << "•  ";
                }
                doc::RenderInline(block.text, doc::Attr(doc_Attr_plain), doc::Color(doc_Color_default), 0, run);
                doc::WrapRun(run, indent, marker, doc::_db.width - indent - doc::MarkerWidth(marker));
            } else {
                doc::FRun &run = doc::run_Alloc();
                doc::RenderInline(block.text, doc::Attr(doc_Attr_plain), doc::Color(doc_Color_default), 0, run);
                doc::WrapRun(run, 3, "", doc::_db.width - 3);
            }
            doc::run_RemoveAll();
        }
    }ind_end;
    Trim();
}

// Return the line NAME names, or zero when it names none.
//
// A line anchor is spelled `L` and the number, which is what an element of an HTML
// page can carry as an id and what a browser will scroll to.  One spelling serves both
// devices: this resolves it to a row, and a browser resolves it itself.  The letter is
// also what keeps a heading anchored `2-the-thing` from being read as a line.
static int NumberOf(algo::strptr name) {
    bool tagged = ch_N(name) > 1 && name.elems[0] == 'L';
    algo::strptr digit = tagged ? doc::RestFrom(name, 1) : algo::strptr();
    bool digits = ch_N(digit) > 0;
    frep_(i, ch_N(digit)) {
        digits = digits && digit.elems[i] >= '0' && digit.elems[i] <= '9';
    }
    return digits ? algo::ParseI32(digit, 0) : 0;
}

// Return the row the target NAME leads to, or -1 when this page carries no such
// row.
//
// The anchors were recorded as the page was laid out, from the markers abt_md
// plants under its headings, so this is an exact match against the same string a
// link's target names.  Searching the page text for the name instead can never
// work: a target is a slug -- `reading-a-document` -- while what is printed is
// the heading it was made from.
//
// A target spelling `L` and a number names a line of a file rather than a heading,
// which is how a function link reaches the line its prototype was declared on.  A page
// that is one file listing shows one line per row, so the row follows from the row the
// listing began at and the file line that row shows; a page that is not one listing has
// no lines to name and answers -1.  A whole file begins at line one and a function's
// source begins wherever the function does, which is what `listbase` carries.
// Record the anchor SLUG at the row about to be laid out, so that a link naming it can be
// followed to here.
//
// A heading of a document carries the anchor abt_md planted under it; a section doc lays
// out itself carries the one the pane spells.  Both arrive here, so an entry of the pane
// leads somewhere on either kind of page.
void doc::PutAnchor(algo::strptr slug) {
    doc::FAnchor &anchor = doc::anchor_Alloc();
    anchor.anchor = slug;
    anchor.row = i32(doc::row_N());
}

// Return the row the anchor NAME was laid out on, or -1 when the page carries no such
// anchor.
int doc::AnchorRow(algo::strptr name) {
    int line = NumberOf(name);
    int at = doc::_db.listtop + line - i32(doc::_db.listbase);
    bool listed = line > 0 && doc::_db.listtop >= 0 && at < i32(doc::row_N());
    int ret = listed ? at : -1;
    if (line == 0) {
        ind_beg(doc::_db_anchor_curs, anchor, doc::_db) {
            if (anchor.anchor == name) {
                ret = anchor.row;
            }
        }ind_end;
    }
    return ret;
}

// Lay the pane of the page at LOC onto the page, above the body.
//
// A browser puts the pane in a column beside the page; here it goes above the text, which
// is the same map in the place a terminal has for one.
//
// The pane is drawn before the body, so what it can hold is what is known before the body
// runs -- the headings of a document just parsed, the sections of a page of records.  A
// browser builds its pane after the body and carries more in it: the functions the code
// section named.  That is one text asked for at two moments rather than two panes.
static void RenderNav(algo::strptr loc) {
    tempstr nav(doc::NavText(loc));
    if (ch_N(nav) > 0) {
        doc::ParseText(nav);
        doc::RenderDoc();
        doc::EndRow();
    }
}

// Lay the markdown document at PATH onto the page, naming the file on the first
// row.
//
// The pane goes above the text, and a document about a namespace carries a link to that
// namespace's internals under its title.  Both are generated from what was parsed, so the
// document is parsed twice: once to learn its headings, and once to lay it out.
//
// A README gets the listing of its own directory under it.  What a directory is about
// is one thing and what it holds is another, and the README is where a reader arrives
// looking for both -- so the page that answers the first ends with the second, read off
// dev.gitfile at the moment it is asked for rather than written into the file.

//
// The name is there because a key does not say what it resolved to.  `doc tut`
// answers with a page of tutorial titles and, without the header, no statement
// of which of the tree's twelve hundred documents produced them -- so a reader
// who wants to edit what they are reading, or cite it, has to go and find it.
//
// When the key the reader typed matched more than one document, the others are
// named at the foot of the page.  A rule file and a tool README describe the
// same namespace from two sides, and a reader who found one usually wants to
// know the other is there.  Each is a link from the top of the repository, so
// the reader opens one with Enter instead of leaving to type the path again.
static void RenderFile(algo::strptr path) {
    doc::PutText(path, doc::Attr(doc_Attr_dim), doc::Color(doc_Color_default));
    doc::EndRow();
    doc::ParseFile(path);
    RenderNav(path);
    tempstr intro(doc::IntroText(path));
    doc::ParseFile(path);
    if (ch_N(intro) > 0) {
        doc::ParseText(doc::Withintro(algo::FileToString(path, algo::FileFlags()), intro));
    }
    doc::RenderDoc();
    if (doc::FootdirQ(path)) {
        doc::RenderDir(tempstr() << Pathcomp(path, "/RL") << "/", path);
    }
}

// Lay the foot of the page about the document at PATH onto it: the link to the internals
// of the namespace it is about, and the other documents the key matched.
//
// It comes after the sections rather than before them, because both of these lead the
// reader somewhere else and the sections are still the page they are on.
static void RenderFoot(algo::strptr path) {
    tempstr foot(doc::InternalsText(path));
    if (ch_N(foot) > 0) {
        doc::ParseText(foot);
        doc::RenderDoc();
    }
    bool head = doc::c_match_N() > 1 && path == algo::strptr(doc::c_match_qFind(0).gitfile);
    if (head) {
        int i = 0;
        ind_beg(doc::_db_c_match_curs, readmefile, doc::_db) {
            if (i == 1) {
                doc::EndRow();
                doc::PutText("   see also", doc::Attr(doc_Attr_dim), doc::Color(doc_Color_default));
                doc::EndRow();
            }
            if (i > 0) {
                tempstr target;
                target << "/" << readmefile.gitfile;
                doc::PutText("      ", doc::Attr(doc_Attr_plain), doc::Color(doc_Color_default));
                doc::PutText(readmefile.gitfile, doc::Attr(doc_Attr_underline), doc::Color(doc_Color_default), doc::NewLink(target));
                doc::EndRow();
            }
            i++;
        }ind_end;
    }
}

// Lay the page of what the key KEY could have meant onto the page, and record the key as
// the location the reader is on.
//
// It is a page like any other once it is drawn, which is what lets a key that resolved to
// nothing be read on the screen, paged, searched and followed out of -- the reader picks
// one of the candidates with Enter instead of typing a name a second time.
void doc::RenderMatch(algo::strptr key) {
    tempstr page(key);
    doc::CanvasClear();
    ch_RemoveAll(doc::_db.navfunc);
    doc::_db.page = page;
    doc::_db.p_out = NULL;
    doc::ParseText(doc::MatchText(page));
    doc::RenderDoc();
    doc::LocateLink();
}

// Lay the location LOC onto the page, and record it as the one the reader is on.
//
// A location is either a page this tool generates, a directory of the tree, or a file --
// and dev.docpage is the list of the first kind, each row bound to the function that draws
// it.  So the dispatch here is a lookup: the word before the colon names the page, and what
// it names is a row rather than a case.  Access paths are the one thing added to a
// page rather than laid out as one: they are drawn about whatever records the page
// selects, so the drawing follows every body that selects any, in one place.  What the two share is this frame: the
// old page goes, the location is recorded so that the reader has a name to come
// back to, and the links are located once the page stands still.  Where the
// links are cannot be known before that, because wrapping and table layout both
// move text after it has been rendered.
void doc::RenderPage(algo::strptr loc) {
    tempstr page(loc);
    doc::CanvasClear();
    doc::_db.page = page;
    doc::FDocpage *docpage = doc::Docpage(page);
    doc::_db.p_out = NULL;
    bool file = !docpage && !doc::DirQ(page);
    if (doc::RecpageQ(page)) {
        RenderNav(page);
    }
    if (docpage) {
        (*docpage->step)();
    } else if (doc::DirQ(page)) {
        doc::RenderDir(page, "");
    } else {
        RenderFile(page);
    }
    if (doc::FoldQ(page)) {
        doc::RenderSection(page);
    }
    if (file) {
        RenderFoot(page);
    }
    doc::LocateLink();
}

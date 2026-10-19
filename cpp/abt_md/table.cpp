// Copyright (C) 2026 AlgoX2 Corp
//
// License: Apache
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Target: abt_md (exe) -- Tool to generate markdown documentation
// Exceptions: yes
// Source: cpp/abt_md/table.cpp
//
// Align the pipe tables of a readme, so a table reads as a table in the markdown
// source as it does rendered.  A table is read into an algo_lib.FTxttbl and drawn
// again by FTxttbl_Markdown, which pads each cell to its column's width.  The cells
// are cut where GitHub's table grammar cuts them, so the rendered table does not
// change.

#include "include/algo.h"
#include "include/abt_md.h"

// Return the indent of LINE when LINE is a row of a pipe table, a line whose first
// character after its leading blanks is a pipe, and -1 when it is not.
static int RowIndent(algo::strptr line) {
    int i = 0;
    while (i < line.n_elems && line.elems[i] == ' ') {
        i++;
    }
    return i < line.n_elems && line.elems[i] == '|' ? i : -1;
}

// TRUE when LINE opens or closes a fenced block, so the lines up to the next such
// line are code and hold no table.
static bool FenceQ(algo::strptr line) {
    algo::strptr text = Trimmed(line);
    return StartsWithQ(text, "```") || StartsWithQ(text, "~~~");
}

// Set CELLS to the cells of LINE, a table row whose first pipe is at INDENT: the text
// between pipes, trimmed.  A pipe after a backslash belongs to its cell, as GitHub's
// table grammar reads it, and a row that ends without a pipe ends its last cell at the
// end of the line.
static void RowCells(algo::strptr line, int indent, algo::StringAry &cells) {
    ary_RemoveAll(cells);
    int beg = indent + 1;
    int i = beg;
    while (i < line.n_elems) {
        char c = line.elems[i];
        if (c == '\\' && i + 1 < line.n_elems) {
            i += 2;
        } else if (c == '|') {
            ary_Alloc(cells) = Trimmed(algo::strptr(line.elems + beg, i - beg));
            i++;
            beg = i;
        } else {
            i++;
        }
    }
    algo::strptr rest = Trimmed(algo::strptr(line.elems + beg, line.n_elems - beg));
    if (ch_N(rest) > 0) {
        ary_Alloc(cells) = rest;
    }
}

// Set JUST to the justification the delimiter cell CELL gives its column -- right for
// a colon on the right, center for colons on both ends, and left otherwise -- and
// return whether CELL is a delimiter cell, dashes with a colon at either end.
static bool DelimJustMaybe(algo::strptr cell, algo_TextJustEnum &just) {
    bool left = cell.n_elems > 0 && cell.elems[0] == ':';
    bool right = cell.n_elems > 1 && cell.elems[cell.n_elems - 1] == ':';
    int beg = left ? 1 : 0;
    int end = right ? cell.n_elems - 1 : cell.n_elems;
    bool ret = end > beg;
    for (int i = beg; i < end; i++) {
        ret = ret && cell.elems[i] == '-';
    }
    just = left && right ? algo_TextJust_j_center : right ? algo_TextJust_j_right : algo_TextJust_j_left;
    return ret;
}

// TRUE when LINE, a row at INDENT, is the delimiter row of a table: every cell is a
// delimiter cell.
static bool DelimRowQ(algo::strptr line, int indent) {
    algo::StringAry cells;
    RowCells(line, indent, cells);
    bool ret = ary_N(cells) > 0;
    ind_beg(algo::StringAry_ary_curs, cell, cells) {
        algo_TextJustEnum just = algo_TextJust_j_left;
        ret = ret && DelimJustMaybe(cell, just);
    }ind_end;
    return ret;
}

// Append to OUT the table whose rows are the lines LINE[BEG] up to LINE[END], each
// beginning with a pipe at INDENT, the second its delimiter row, drawn again by
// FTxttbl_Markdown with each column justified as the delimiter row says.
static void PrintTable(algo::cstring &out, algo::StringAry &line, int beg, int end, int indent) {
    algo::StringAry cells;
    RowCells(ary_qFind(line, beg + 1), indent, cells);
    algo::U64Ary just;
    ind_beg(algo::StringAry_ary_curs, cell, cells) {
        algo_TextJustEnum colj = algo_TextJust_j_left;
        (void)DelimJustMaybe(cell, colj);
        ary_Alloc(just) = u64(colj + 1);
    }ind_end;
    algo_lib::FTxttbl txttbl;
    for (int i = beg; i < end; i++) {
        if (i != beg + 1) {
            RowCells(ary_qFind(line, i), indent, cells);
            algo_lib::FTxtrow &txtrow = algo_lib::AddRow(txttbl);
            ind_beg(algo::StringAry_ary_curs, cell, cells) {
                int col = ind_curs(cell).index;
                int colj = col < ary_N(just) ? int(ary_qFind(just, col)) - 1 : algo_TextJust_j_left;
                (void)algo_lib::AddCell(txtrow, cell, algo_TextJustEnum(colj));
            }ind_end;
        }
    }
    tempstr table;
    algo_lib::FTxttbl_Markdown(txttbl, table);
    ind_beg(algo::Line_curs, row, table) {
        char_PrintNTimes(' ', out, indent);
        out << row << eol;
    }ind_end;
}

// Return TEXT, a readme, with every pipe table aligned (PrintTable).  A table is a run
// of rows at one indent whose second row is a delimiter row; a run of pipe lines with
// no delimiter row is left as it stands, and so is everything inside a fenced block.
// Every line of the result ends with a newline.
tempstr abt_md::AlignTables(algo::strptr text) {
    algo::StringAry line;
    ind_beg(algo::Line_curs, text_line, text) {
        ary_Alloc(line) = text_line;
    }ind_end;
    tempstr ret;
    bool fenced = false;
    int i = 0;
    while (i < ary_N(line)) {
        algo::strptr cur = ary_qFind(line, i);
        int indent = fenced ? -1 : RowIndent(cur);
        bool table = indent >= 0 && i + 1 < ary_N(line) && RowIndent(ary_qFind(line, i + 1)) == indent
            && DelimRowQ(ary_qFind(line, i + 1), indent);
        int end = i + 1;
        if (table) {
            end = i + 2;
            while (end < ary_N(line) && RowIndent(ary_qFind(line, end)) == indent) {
                end++;
            }
            PrintTable(ret, line, i, end, indent);
        } else {
            fenced = fenced != FenceQ(cur);
            ret << cur << eol;
        }
        i = end;
    }
    return ret;
}

// Return the number of characters in LINE, counting a multi-byte UTF-8 character once,
// which is the unit FTxttbl_Markdown pads a cell in.
static int CharN(algo::strptr line) {
    int ret = 0;
    for (int i = 0; i < line.n_elems; i++) {
        ret += (u8(line.elems[i]) & 0xC0) != 0x80;
    }
    return ret;
}

// Report every pipe table of the readme GITFILE whose widest row is longer than
// LIMIT characters, naming the table's first line and its width, and fail the run
// once for each.
//
// A readme is read in a terminal and in a diff as often as it is rendered.  A table
// row there that runs to several hundred characters reads as one line nobody can
// follow, and once one cell grows, alignment pads every other row to match.  So a
// readme keeps a table's cells short and puts the long text in bullets beneath the
// table.  The width is read off the file as written, after alignment, since padding
// is what makes a narrow column wide.
static void CheckTableWidth(algo::strptr gitfile, int limit) {
    algo::StringAry line;
    ind_beg(algo::FileLine_curs, file_line, gitfile) {
        ary_Alloc(line) = file_line;
    }ind_end;
    bool fenced = false;
    int i = 0;
    while (i < ary_N(line)) {
        algo::strptr cur = ary_qFind(line, i);
        int indent = fenced ? -1 : RowIndent(cur);
        bool table = indent >= 0 && i + 1 < ary_N(line) && RowIndent(ary_qFind(line, i + 1)) == indent
            && DelimRowQ(ary_qFind(line, i + 1), indent);
        int end = table ? i : i + 1;
        int width = 0;
        while (table && end < ary_N(line) && RowIndent(ary_qFind(line, end)) == indent) {
            width = i32_Max(width, CharN(ary_qFind(line, end)));
            end++;
        }
        if (!table) {
            fenced = fenced != FenceQ(cur);
        }
        if (width > limit) {
            prlog(gitfile << ":" << i + 1 << ": abt_md.wide_table"
                  << Keyval("width", width)
                  << Keyval("limit", limit)
                  << Keyval("comment", "keep a table's cells short and move the long text to bullets beneath it"));
            algo_lib::_db.exit_code++;
        }
        i = end;
    }
}

// Check every readme marked narrow for tables wider than 120 characters
// (CheckTableWidth).  Rule 29 of the writing style asks this of every readme.  A
// readme is marked narrow once its tables hold to it, which the rule files do, so
// the rest of the tree joins the check one file at a time as its tables are brought
// to it.
void abt_md::Main_CheckTableWidth() {
    ind_beg(_db_readmefile_curs, readmefile, _db) if (readmefile.narrow) {
        CheckTableWidth(readmefile.gitfile, 120);
    }ind_end;
}

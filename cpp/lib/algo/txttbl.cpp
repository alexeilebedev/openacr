// Copyright (C) 2025-2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
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
// Contacting ICE: <https://www.theice.com/contact>
// Target: algo_lib (lib) -- Support library for all executables
// Exceptions: NO
// Source: cpp/lib/algo/txttbl.cpp -- Ascii table
//

#include "include/algo.h"

// Look up cell with specific coordinates.
algo_lib::FTxtcell *algo_lib::FindCell(algo_lib::FTxttbl &txttbl, int row, int col) {
    algo_lib::FTxtcell *retval=NULL;
    if (algo_lib::FTxtrow *txtrow = c_txtrow_Find(txttbl, row)) {
        retval = c_txtcell_Find(*txtrow, col);
    }
    return retval;
}

algo_TextJustEnum algo_lib::EvalJust(algo_lib::FTxttbl &txttbl, u32 col, algo_TextJustEnum just) {
    algo_TextJustEnum ret = just;
    if (just == algo_TextJust_j_auto) {
        if (algo_lib::FTxtcell *cell = FindCell(txttbl, 0, col)) {
            ret = cell->justify;
        } else {
            ret = algo_TextJust_j_left;
        }
    }
    return ret;
}

// Add new cell to the given row.
// TEXT specifies the text for the cell
algo_lib::FTxtcell &algo_lib::AddCell(algo_lib::FTxtrow &txtrow, algo::strptr text, algo_TextJustEnum justify DFLTVAL(algo_TextJust_j_auto)) {
    algo_lib::FTxtcell &txtcell = algo_lib::txtcell_Alloc();
    txtcell.p_txtrow = &txtrow;
    txtcell.justify = EvalJust(*txtrow.p_txttbl, c_txtcell_N(txtrow)-1, justify);
    txtcell.text    = text;
    txtcell.rsep    = "  ";
    (void)txtcell_XrefMaybe(txtcell);
    return txtcell;
}

algo::cstring &algo_lib::AddCell(algo_lib::FTxttbl &txttbl) {
    return AddCellEx(txttbl).text;
}

// Add new row to the table and return a reference to it
algo_lib::FTxtrow &algo_lib::AddRow(algo_lib::FTxttbl &txttbl) {
    algo_lib::FTxtrow &txtrow = txtrow_Alloc();
    txtrow.p_txttbl = &txttbl;
    (void)txtrow_XrefMaybe(txtrow);
    return txtrow;
}

// Add column to the last row of table and return reference to it
// The justification for the column is copied from the header row
algo_lib::FTxtcell &algo_lib::AddCellEx(algo_lib::FTxttbl &txttbl) {
    if (!c_txtrow_N(txttbl)) {
        AddRow(txttbl);
    }
    algo_lib::FTxtrow  &txtrow = *c_txtrow_Find(txttbl, c_txtrow_N(txttbl)-1);
    algo_lib::FTxtcell &txtcell = algo_lib::txtcell_Alloc();
    txtcell.p_txtrow = &txtrow;
    txtcell.justify  = EvalJust(txttbl, c_txtcell_N(txtrow) - 1, algo_TextJust_j_auto);
    tempstr t;
    char_PrintNTimes(' ', t,  txttbl.col_space);
    txtcell.rsep = t;
    (void)txtcell_XrefMaybe(txtcell);
    return txtcell;
}

// Add a column with specified text TEXT to the last row of the table
// If optional JUSTIFY is provided, the justification is set. Otherwise, justification is automatically
// determined from the header cell.
void algo_lib::AddCol(algo_lib::FTxttbl &txttbl, algo::strptr text, algo_TextJustEnum justify DFLTVAL(algo_TextJust_j_auto)) {
    algo_lib::FTxtcell &txtcell = AddCellEx(txttbl);
    txtcell.text    = text;
    txtcell.justify = EvalJust(txttbl, c_txtcell_N(*txtcell.p_txtrow)-1, justify);
}

// Add a comma-separated list of columns to the table
void algo_lib::AddCols(algo_lib::FTxttbl &txttbl, algo::strptr csv, algo_TextJustEnum justify DFLTVAL(algo_TextJust_j_left)) {
    ind_beg(algo::Sep_curs,token,csv,','){
        AddCol(txttbl, token, justify);
    }ind_end;
}

bool algo::SaneTerminalQ() {
    strptr termtype = getenv(algo_lib::dev_envvar_TERM);
    return isatty(STDOUT_FILENO) && termtype != strptr("dumb");
}

// -----------------------------------------------------------------------------

static void PrintRow(cstring &str, algo_lib::FTxtrow &row, bool use_style) {
    ind_beg(algo_lib::txtrow_c_txtcell_curs, txtcell, row) {
        int extra = txtcell.width - ch_N(txtcell.rsep) - ch_N(txtcell.text);
        int l = txtcell.justify>0 ? extra
            : txtcell.justify<0 ? 0
            : extra/2;
        char_PrintNTimes(' ', str,  l);
        algo::TermStyle style = use_style ? txtcell.style : algo::TermStyle(algo_TermStyle_default);
        if (style != algo_TermStyle_default) {
            if (style & algo_TermStyle_bold) {
                // do it
            }
            if (style & algo_TermStyle_red) {
                str << "\033[91m";
            }
            if (style & algo_TermStyle_green) {
                str << "\033[92m";
            }
            if (style & algo_TermStyle_blue) {
                str << "\033[94m";
            }
        }
        str << txtcell.text;
        if (style) {
            strptr_Print("\033[0m", str);
        }
        char_PrintNTimes(' ', str,  extra-l);
        str << txtcell.rsep;
    }ind_end;
    // trim whitespace on the right
    while (ch_N(str) && ch_qLast(str) == ' ') {
        str.ch_n--;
    }
    str << eol;
}

static void Normalize(algo_lib::FTxttbl &txttbl) {
    algo::U64Ary norm;
    ary_Alloc(norm) = 0;
    int maxspan = 0;
    ind_beg(algo_lib::txttbl_c_txtrow_curs,txtrow,txttbl) if (txtrow.select) {
        ind_beg(algo_lib::txtrow_c_txtcell_curs, txtcell, txtrow) {
            maxspan = i32_Max(maxspan, txtcell.span);
        }ind_end;
    }ind_end;
    frep_(span,maxspan+1) {
        ind_beg(algo_lib::txttbl_c_txtrow_curs,txtrow,txttbl) if (txtrow.select) {
            int i = 0;
            ind_beg(algo_lib::txtrow_c_txtcell_curs, txtcell, txtrow) {
                if (txtcell.span == span) {
                    int w = i32_Max(ch_N(txtcell.text) + ch_N(txtcell.rsep), txtcell.width);
                    // apply width
                    int r = i + txtcell.span;
                    while (r >= ary_N(norm)) {
                        int last = ary_qLast(norm);
                        ary_Alloc(norm) = last;
                    }
                    int extra = w - (ary_qFind(norm, r) - ary_qFind(norm, i));
                    if (extra > 0) {
                        for (int j = r; j < ary_N(norm); j++) {
                            ary_qFind(norm, j) += extra;
                        }
                    }
                }
                i += txtcell.span;
            }ind_end;
        }ind_end;
    }
    ind_beg(algo_lib::txttbl_c_txtrow_curs,txtrow,txttbl) if (txtrow.select) {
        int i = 0;
        ind_beg(algo_lib::txtrow_c_txtcell_curs, txtcell, txtrow) {
            txtcell.width = ary_qFind(norm, i+txtcell.span) - ary_qFind(norm, i);
            i += txtcell.span;
        }ind_end;;
    }ind_end;;
}

// -----------------------------------------------------------------------------

void algo_lib::FTxttbl_Print(algo_lib::FTxttbl &txttbl, algo::cstring &str) {
    bool use_style = txttbl.style || algo::SaneTerminalQ();
    if (bool_Update(txttbl.normalized,true)) {
        Normalize(txttbl);
    }
    ind_beg(algo_lib::txttbl_c_txtrow_curs,txtrow,txttbl) if (txtrow.select) {
        PrintRow(str,txtrow,use_style);
    }ind_end;
}

static tempstr NewlineToBr(strptr s) {
    tempstr ret(s);
    Replace(ret,"\n","<br>");
    return ret;
}

// -----------------------------------------------------------------------------

// Return how many characters the bytes S spell, a UTF-8 sequence counting as one,
// so an em dash pads like a hyphen.
static int TextWidth(algo::strptr s) {
    int ret = 0;
    for (int i = 0; i < s.n_elems; i++) {
        ret += (u8(s.elems[i]) & 0xc0) != 0x80 ? 1 : 0;
    }
    return ret;
}

// Print table TXTTBL using markdown, appending to string STR, aligned so that the
// table reads as a table in the markdown source as it does rendered.  The first row
// is the header, and the delimiter row after it gives each column the justification
// of its header cell: plain dashes for left, a colon on the right for right, and
// colons on both ends for center.  Every cell is padded to the width of its column in
// characters, at least three for the delimiter's dashes, on the side its justification
// says.  A left-justified last column is the exception: its text ends where it ends,
// since a column holding prose would otherwise pad every row to its longest paragraph.
// Newlines in cells are converted to '<br>'.
void algo_lib::FTxttbl_Markdown(algo_lib::FTxttbl &txttbl, algo::cstring &str) {
    algo::U64Ary width;
    ind_beg(algo_lib::txttbl_c_txtrow_curs,txtrow,txttbl) if (txtrow.select) {
        ind_beg(algo_lib::txtrow_c_txtcell_curs, txtcell, txtrow) {
            int col = ind_curs(txtcell).index;
            while (ary_N(width) <= col) {
                ary_Alloc(width) = 3;
            }
            ary_qFind(width, col) = u64_Max(ary_qFind(width, col), TextWidth(NewlineToBr(txtcell.text)));
        }ind_end;
    }ind_end;
    ind_beg(algo_lib::txttbl_c_txtrow_curs,txtrow,txttbl) if (txtrow.select && c_txtcell_N(txtrow)) {
        str<<"|";
        ind_beg(algo_lib::txtrow_c_txtcell_curs, txtcell, txtrow) {
            // markdown is thrown away by \n in the cells, so it has to be replaced with <br>
            tempstr text = NewlineToBr(txtcell.text);
            int col = ind_curs(txtcell).index;
            bool ragged = col == ary_N(width) - 1 && txtcell.justify == algo_TextJust_j_left;
            int extra = ragged ? 0 : int(ary_qFind(width, col)) - TextWidth(text);
            int l = txtcell.justify>0 ? extra
                : txtcell.justify<0 ? 0
                : extra/2;
            str<<" ";
            char_PrintNTimes(' ', str, l);
            str<<text;
            char_PrintNTimes(' ', str, extra-l);
            str<<" |";
        }ind_end;
        str<<eol;
        if (ind_curs(txtrow).index==0) {
            str<<"|";
            ind_beg(algo_lib::txtrow_c_txtcell_curs, txtcell, txtrow) {
                int col = ind_curs(txtcell).index;
                bool ragged = col == ary_N(width) - 1 && txtcell.justify == algo_TextJust_j_left;
                int ndash = ragged ? i32_Max(3, TextWidth(NewlineToBr(txtcell.text))) : int(ary_qFind(width, col));
                str<<" "<<(txtcell.justify==algo_TextJust_j_center ? ':' : '-');
                char_PrintNTimes('-', str, ndash - 2);
                str<<(txtcell.justify==algo_TextJust_j_left ? '-' : ':')<<" |";
            }ind_end;
            str<<eol;
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

// Add a header cell to TXTTBL naming ATTR in upper case, or nothing when ATTR has no name.
static void AddHeaderCell(algo_lib::FTxttbl &txttbl, algo::Attr &attr) {
    if (attr.name != "") {
        tempstr name(attr.name);
        ind_beg(algo::cstring_ch_curs,ch,name) {
            ch = algo::ToUpper(ch);
        }ind_end;
        algo_lib::AddCol(txttbl,name);
    }
}

// Add a cell to the last row of TXTTBL holding the value of ATTR, or EMPTY when the value
// is empty; nothing when ATTR has no name.
static void AddValueCell(algo_lib::FTxttbl &txttbl, algo::Attr &attr, algo::strptr empty) {
    if (attr.name != "") {
        algo_lib::AddCell(txttbl) = attr.value == "" ? empty : algo::strptr(attr.value);
    }
}

// Return true when tuple NEXT cannot join the table whose first row is tuple FIRST, so
// that a new table starts at NEXT.  A table has one column per attr of its first row, and
// a row of another type, or with another number of attrs, would put its values under the
// wrong headers.  The type of a tuple is its head's name when the head has one, as in
// `key:value` form, and the head's value otherwise.
bool algo_lib::NewTableQ(algo::Tuple &first, algo::Tuple &next) {
    bool ret = first.head.name == ""
        ? first.head.value != next.head.value
        : first.head.name != next.head.name;
    return ret || attrs_N(first) != attrs_N(next);
}

// Add TUPLE to table TXTTBL as one row with a cell per named attr.  The first tuple of an
// empty table also adds the header row, which names each attr in upper case.  The head is a
// column only when it has a name, since a bare type tag is the same on every row.  A cell
// whose value is empty holds EMPTY instead, so that a reader who splits a line on blanks
// finds every column where the header says it is.
void algo_lib::FTxttbl_AddTuple(algo_lib::FTxttbl &txttbl, algo::Tuple &tuple, algo::strptr empty) {
    if (c_txtrow_N(txttbl) == 0) {
        AddHeaderCell(txttbl, tuple.head);
        ind_beg(algo::Tuple_attrs_curs,attr,tuple) {
            AddHeaderCell(txttbl, attr);
        }ind_end;
    }
    AddRow(txttbl);
    AddValueCell(txttbl, tuple.head, empty);
    ind_beg(algo::Tuple_attrs_curs,attr,tuple) {
        AddValueCell(txttbl, attr, empty);
    }ind_end;
}

// Append table TXTTBL to OUT, aligned, with a blank line after it, and empty the table so the
// next one starts from a header row.
static void FlushTable(algo_lib::FTxttbl &txttbl, algo::cstring &out) {
    algo_lib::FTxttbl_Print(txttbl, out);
    out << eol;
    algo_lib::c_txtrow_Cascdel(txttbl);
    txttbl.normalized = false;
}

// Append to OUT the ssim tuples of TEXT, one per line, as aligned text tables.  A table
// starts at the first tuple and at every tuple that NewTableQ separates from the first row
// of the table in progress, and a blank line ends each table.  A line that does not read
// as a tuple with at least one attr is skipped.  An empty value prints as EMPTY.
void algo_lib::SsimToTable(algo::strptr text, algo::strptr empty, algo::cstring &out) {
    algo_lib::FTxttbl txttbl;
    algo::Tuple first;
    ind_beg(algo::Line_curs,line,text) {
        algo::Tuple tuple;
        if (Tuple_ReadStrptr(tuple,line,false) && attrs_N(tuple) > 0) {
            if (c_txtrow_N(txttbl) > 0 && NewTableQ(first, tuple)) {
                FlushTable(txttbl, out);
            }
            if (c_txtrow_N(txttbl) == 0) {
                first = tuple;
            }
            FTxttbl_AddTuple(txttbl, tuple, empty);
        }
    }ind_end;
    FlushTable(txttbl, out);
}

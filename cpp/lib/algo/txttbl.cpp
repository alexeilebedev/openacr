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

// Print table TXTTBL using markdown, appending to string STR.
// First row of the table is assumed to be the header.
// Newlines in cells are converted to '<br>'.
void algo_lib::FTxttbl_Markdown(algo_lib::FTxttbl &txttbl, algo::cstring &str) {
    ind_beg(algo_lib::txttbl_c_txtrow_curs,txtrow,txttbl) if (txtrow.select && c_txtcell_N(txtrow)) {
        str<<"|";
        ind_beg(algo_lib::txtrow_c_txtcell_curs, txtcell, txtrow) {
            // markdown is thrown away by \n in the cells, so it has to be replaced with <br>
            str<<NewlineToBr(txtcell.text)<<"|";
        }ind_end;
        str<<eol;
        if (ind_curs(txtrow).index==0) {
            frep_(i,c_txtcell_N(txtrow)) {
                str<<"|---";
            };
            str<<"|"<<eol;
        }
    }ind_end;
}

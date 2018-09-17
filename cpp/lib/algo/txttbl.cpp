// (C) 2013-2019 NYSE | Intercontinental Exchange
//
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
// Contacting ICE: <https://www.theice.com/contact>
//
// Target: algo_lib (lib) -- Support library for all executables
// Exceptions: NO
// Source: cpp/lib/algo/txttbl.cpp -- Ascii table
//
// Created By: alexei.lebedev shreejith.lokegowda
// Authors: alexei.lebedev
// Recent Changes: alexei.lebedev shreejith.lokegowda hayk.mkrtchyan
//

static algo_TextJustEnum DefaultJust(algo_lib::FTxttbl &txttbl, u32 col) {
    algo_TextJustEnum retval = algo_TextJust_j_left;
    u32 row = txttbl.hdr_row;
    if (algo_lib::FTxtrow *txtrow = c_txtrow_Find(txttbl, row)) {
        u32 ncols = c_txtcell_N(*txtrow);
        if (col < ncols) {
            retval = c_txtcell_Find(*txtrow, col)->justify;
        }
    }
    return retval;
}

// Add new cell to the given row.
algo_lib::FTxtcell &algo_lib::AddCell(algo_lib::FTxtrow &txtrow, strptr title, algo_TextJustEnum justify, int span) {
    algo_lib::FTxtcell &txtcell = algo_lib::txtcell_Alloc();
    txtcell.p_txtrow = &txtrow;
    txtcell.justify = justify;
    txtcell.text    = title;
    txtcell.span    = span;
    txtcell.rsep    = "  ";
    (void)txtcell_XrefMaybe(txtcell);
    return txtcell;
}

algo_lib::FTxtcell &algo_lib::AddCell(algo_lib::FTxtrow &txtrow, strptr title, algo_TextJustEnum justify) {
    return AddCell(txtrow, title, justify, 1);
}

// Add new cell to the last row of of table and return references to its text
algo_lib::FTxtrow &algo_lib::AddRow(algo_lib::FTxttbl &txttbl) {
    algo_lib::FTxtrow &txtrow = txtrow_Alloc();
    txtrow.p_txttbl = &txttbl;
    (void)txtrow_XrefMaybe(txtrow);
    return txtrow;
}

// Add column to the last row of table and return reference to it
algo_lib::FTxtcell &algo_lib::AddCellEx(algo_lib::FTxttbl &txttbl) {
    if (!c_txtrow_N(txttbl)) {
        AddRow(txttbl);
    }
    algo_lib::FTxtrow  &txtrow = *c_txtrow_Find(txttbl, c_txtrow_N(txttbl)-1);
    algo_lib::FTxtcell &txtcell = algo_lib::txtcell_Alloc();
    txtcell.p_txtrow = &txtrow;
    txtcell.justify    = DefaultJust(txttbl, c_txtcell_N(txtrow) - 1);
    tempstr t;
    char_PrintNTimes(' ', t,  txttbl.col_space);
    txtcell.rsep = t;
    (void)txtcell_XrefMaybe(txtcell);
    return txtcell;
}

// Text table.
// Text table is a table with rows; Each row has an array of cells.
// Each call has text, col span, text justification, and optional style (color).
// Add column to the last row of table.
void algo_lib::AddCol(algo_lib::FTxttbl &txttbl, strptr col, algo_TextJustEnum justify) {
    algo_lib::FTxtcell &txtcell = AddCellEx(txttbl);
    txtcell.text    = col;
    txtcell.justify = justify;
}

void algo_lib::AddCol(algo_lib::FTxttbl &txttbl, strptr col) {
    AddCol(txttbl, col, algo_TextJust_j_left);
}

cstring &algo_lib::AddCell(algo_lib::FTxttbl &txttbl) {
    return AddCellEx(txttbl).text;
}

// Use prlog(txttbl) to print.
void algo_lib::TxttblCsv(algo_lib::FTxttbl &tbl) {
    ind_beg(algo_lib::txttbl_c_txtrow_curs, tblrow, tbl) {
        u32 n = c_txtcell_N(tblrow);
        u32 i=0;
        ind_beg(algo_lib::txtrow_c_txtcell_curs, cell, tblrow) {
            if (++i < n) {
                cell.text << ", ";
            }
        }ind_end;
    }ind_end;
}


bool algo::SaneTerminalQ() {
    strptr termtype = getenv("TERM");
    return isatty(STDOUT_FILENO) && termtype != strptr("dumb");
}

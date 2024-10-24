// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
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
// Contacting ICE: <https://www.theice.com/contact>
// Target: atf_unit (exe) -- Unit tests (see unittest table)
// Exceptions: yes
// Source: cpp/atf_unit/algo_txttbl.cpp
//

#include "include/atf_unit.h"
// -----------------------------------------------------------------------------
void atf_unit::unittest_algo_lib_Txttbl(){
    algo_lib::FTxttbl tbl;
    AddCol(tbl,"col1...",algo_TextJust_j_right);
    AddCol(tbl,"col2");
    AddCol(tbl,"col3\n");
    AddRow(tbl);
    AddCol(tbl,"-",algo_TextJust_j_right);
    AddCol(tbl,"-",algo_TextJust_j_center);
    AddCol(tbl,"-",algo_TextJust_j_left);
    AddRow(tbl);
    AddCell(tbl)="val3";
    AddCell(tbl)="val2";
    AddCell(tbl)="val1";

    cstring out;
    FTxttbl_Markdown(tbl,out);
    vrfyeq_(out,
            "|col1...|col2|col3<br>|\n"
            "|---|---|---|\n"
            "|-|-|-|\n"
            "|val3|val2|val1|\n"
            );

    Refurbish(out);
    FTxttbl_Print(tbl,out);
    vrfyeq_(out,
            "col1...  col2  col3\n\n"
            "      -   -    -\n"
            "val3     val2  val1\n"
            );

    // Check AddCols
    algo::Refurbish(tbl);
    AddCols(tbl,"aaa,bbb,ccc");
    AddRow(tbl);
    AddCols(tbl,"*,*,*",algo_TextJust_j_right);
    AddRow(tbl);
    AddCols(tbl,"*,*,*",algo_TextJust_j_center);
    AddRow(tbl);
    AddCols(tbl,"*,*,*",algo_TextJust_j_left);
    Refurbish(out);
    FTxttbl_Print(tbl,out);
    vrfyeq_(out,
            "aaa  bbb  ccc\n"
            "  *    *    *\n"
            " *    *    *\n"
            "*    *    *\n"
            );
}

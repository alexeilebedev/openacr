// Copyright (C) 2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
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
// Target: atf_unit (exe) -- Unit tests (see unittest table)
// Exceptions: yes
// Source: cpp/atf_unit/algo_txttbl.cpp
//

#include "include/atf_unit.h"

// -----------------------------------------------------------------------------

static void Check(algo::strptr a, algo::strptr b) {
    if (a!=b) {
        prlog("=result=");
        prlog(a);
        prlog("=should be=");
        prlog(b);
        vrfy(0,"mismatch");
    }
}

void atf_unit::unittest_algo_lib_Txttbl(){
    algo_lib::FTxttbl tbl;
    AddCol(tbl,"col1...",algo_TextJust_j_right);
    AddCol(tbl,"col2",algo_TextJust_j_left);
    AddCol(tbl,"col3\n",algo_TextJust_j_left);
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
    Check(out,
          "|col1...|col2|col3<br>|\n"
          "|---|---|---|\n"
          "|-|-|-|\n"
          "|val3|val2|val1|\n"
          );

    Refurbish(out);
    FTxttbl_Print(tbl,out);
    Check(out,
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
    Check(out,
          "aaa  bbb  ccc\n"
          "  *    *    *\n"
          " *    *    *\n"
          "*    *    *\n"
          );
}

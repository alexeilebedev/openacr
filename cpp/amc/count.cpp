// Copyright (C) 2008-2012 AlgoEngineering LLC
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2023 AlgoRND
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
// Target: amc (exe) -- Algo Model Compiler: generate code under include/gen and cpp/gen
// Exceptions: NO
// Source: cpp/amc/count.cpp
//

#include "include/amc.h"

void amc::tclass_Count() {
}

void amc::tfunc_Count_Insert() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FFunc& insert = amc::CreateCurFunc();
    Ins(&R, insert.comment, "Count row. If row is already counted, do nothing");
    Ins(&R, insert.ret  , "void", false);
    Ins(&R, insert.proto, "$name_Insert($Parent, $Ctype& row)", false);
    Ins(&R, insert.body, "$parname.$name_n += row.$name_value == false;");
    Ins(&R, insert.body, "row.$name_value = true;");
}

void amc::tfunc_Count_Remove() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FFunc& remove = amc::CreateCurFunc();
    Ins(&R, remove.comment, "Uncount row");
    Ins(&R, remove.ret  , "void", false);
    Ins(&R, remove.proto, "$name_Remove($Parent, $Ctype& row)", false);
    Ins(&R, remove.body, "if($parname.$name_n > 0) {");
    Ins(&R, remove.body, "    $parname.$name_n -= row.$name_value;");
    Ins(&R, remove.body, "}");
    Ins(&R, remove.body, "row.$name_value = false;");
}

void amc::tfunc_Count_N() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FFunc& n = amc::CreateCurFunc();
    Ins(&R, n.comment, "Count # of elements in the set");
    Ins(&R, n.ret  , "u32", false);
    Ins(&R, n.proto, "$name_N($Parent)", false);
    Ins(&R, n.body, "return $parname.$name_n;");
}

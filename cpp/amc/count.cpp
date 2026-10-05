// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2008-2012 AlgoEngineering LLC
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
// Target: amc (exe) -- Algo Model Compiler: generate code under include/gen and cpp/gen
// Exceptions: yes
// Source: cpp/amc/count.cpp -- Count reftype
//

#include "include/amc.h"

void amc::tclass_Count() {
}

void amc::tfunc_Count_Insert() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FFunc& insert = amc::CreateCurFunc();
    Ins(&R, insert.comment, "Count row. If row is already counted, do nothing");
    Ins(&R, insert.ret  , "void", false);
    Ins(&R, insert.proto, "$name_Insert($Parent, $Ctype& row)", false);
    Ins(&R, insert.body, "$parname.$name_n += row.$name_value == false;");
    Ins(&R, insert.body, "row.$name_value = true;");
}

void amc::tfunc_Count_Remove() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
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
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FFunc& n = amc::CreateCurFunc();
    Ins(&R, n.comment, "Count # of elements in the set");
    Ins(&R, n.ret  , "u32", false);
    Ins(&R, n.proto, "$name_N($Parent)", false);
    Ins(&R, n.body, "return $parname.$name_n;");
}

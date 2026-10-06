// Copyright (C) 2025-2026 AlgoX2 Corp
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
// Source: cpp/amc/cppfunc.cpp -- Cppfunc reftype
//

#include "include/amc.h"

void amc::tclass_Cppfunc() {
}

void amc::tfunc_Cppfunc_Get() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FFunc& get = amc::CreateCurFunc();
    Set(R, "$Fldtype", field.cpp_type);
    Ins(&R, get.ret  , "$Fldtype", false);
    Ins(&R, get.proto, "$name_Get($Parent)", false);
    Ins(&R, get.body, "(void)$parname;");
    if (field.c_cppfunc && ch_N(field.c_cppfunc->expr.value)) {
        if (Subst(R,"$parname")!="parent") {
            Ins(&R, get.body, "$Partype &parent = $parname; // needed in case 'expr' refers to 'parent'");
            Ins(&R, get.body, "(void)parent;");
        }
        if (field.reftype==dmmeta_Reftype_reftype_Ptr || field.reftype==dmmeta_Reftype_reftype_Upptr) {
            Set(R, "$Fldtype", "($Fldtype)");
        }
        get.inl = true;
        Ins(&R, get.body, tempstr()<<"return $Fldtype("<<field.c_cppfunc->expr<<");");
    } else {
        // external fldfunc
        get.extrn = true;
    }
}

// C++ already constructs the field; no extra Init needed.
void amc::tfunc_Cppfunc_Init() {
}

void amc::tfunc_Cppfunc_Set() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    if (field.c_cppfunc && field.c_cppfunc->set) {
        Set(R, "$Fldargtype", Argtype(field));
        amc::FFunc& set = amc::CreateCurFunc();
        set.acrkey << "cppfunc:"<<field.field;
        set.extrn = true;
        Ins(&R, set.ret  , "void", false);
        Ins(&R, set.proto, "$name_Set($Parent, $Fldargtype rhs)", false);
    }
}

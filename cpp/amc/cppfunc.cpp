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
// Source: cpp/amc/cppfunc.cpp
//

#include "include/amc.h"

void amc::tclass_Cppfunc() {
}

void amc::tfunc_Cppfunc_Get() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
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

void amc::tfunc_Cppfunc_Init() {
    // #AL# C++ already constructs field
}

void amc::tfunc_Cppfunc_Set() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    if (field.c_cppfunc && field.c_cppfunc->set) {
        Set(R, "$Fldargtype", Argtype(field));
        amc::FFunc& set = amc::CreateCurFunc();
        set.extrn = true;
        Ins(&R, set.ret  , "void", false);
        Ins(&R, set.proto, "$name_Set($Parent, $Fldargtype rhs)", false);
    }
}

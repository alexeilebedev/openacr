// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
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
// Source: cpp/amc/delptr.cpp
//

#include "include/amc.h"


void amc::tclass_Delptr() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    vrfy(!FldfuncQ(field), "fldfunc for delptr not supported");
    vrfy(DefaultPool(*field.p_ctype->p_ns), "namespace needs a default pool");
    Set(R, "$name"   , name_Get(field));

    InsVar(R, field.p_ctype, "$Cpptype*", "$name", "", "Private pointer to value");
}

void amc::tfunc_Delptr_Init() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FFunc& init = amc::CreateCurFunc();
    init.inl = true;
    Ins(&R, init.body, "$parname.$name = NULL;");
}

void amc::tfunc_Delptr_Uninit() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FFunc& func = amc::CreateCurFunc();
    func.comment << "Delete value, if necessary";
    Ins(&R, func.body, "$name_Delete($pararg);");
}

void amc::tfunc_Delptr_Delete() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    amc::FFunc& func = amc::CreateCurFunc(true); {
        AddRetval(func, "void", "", "");
        Ins(&R, func.comment, "Delete value.");
        Ins(&R, func.body, "if ($parname.$name) {");
        if (!field.p_arg->c_cpptype || field.p_arg->c_cpptype->dtor) {
            Ins(&R, func.body, "$parname.$name->~$Ctype();");
        }
        Ins(&R, func.body, "$basepool_FreeMem($parname.$name, sizeof($Cpptype));");
        Ins(&R, func.body, "$parname.$name = NULL;");
        Ins(&R, func.body, "}");
    }
}

void amc::tfunc_Delptr_Access() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    amc::FFunc& func = amc::CreateCurFunc(true); {
        AddRetval(func, Subst(R,"$Cpptype&"), "", "");
        Set(R, "$dflt",field.dflt.value);
        Ins(&R, func.comment, "Access value, creating it if necessary. Process dies if not successful.");
        Ins(&R, func.body, "$Cpptype *ret=$parname.$name;");
        Ins(&R, func.body, "if (!ret) {");
        Ins(&R, func.body, "    ret = ($Cpptype*)$basepool_AllocMem(sizeof($Cpptype));");
        Ins(&R, func.body, "    if (!ret) {");
        Ins(&R, func.body, "        FatalErrorExit(\"out of memory allocating $Cpptype (in $Partype.$name)\");");
        Ins(&R, func.body, "    }");
        Set(R, "$dflt", field.dflt.value);
        Ins(&R, func.body, "    new(ret) $Cpptype($dflt);");
        Ins(&R, func.body, "    $parname.$name = ret;");
        Ins(&R, func.body, "}");
        Ins(&R, func.body, "return *ret;");
    }
}

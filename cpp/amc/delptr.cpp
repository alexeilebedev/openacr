// Copyright (C) 2025-2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
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
// Target: amc (exe) -- Algo Model Compiler: generate code under include/gen and cpp/gen
// Exceptions: yes
// Source: cpp/amc/delptr.cpp -- Delptr reftype
//

#include "include/amc.h"


void amc::tclass_Delptr() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    vrfy(!FldfuncQ(field), "fldfunc for delptr not supported");
    vrfy(DefaultPool(*field.p_ctype->p_ns), "namespace needs a default pool");
    Set(R, "$name"   , name_Get(field));

    InsVar(R, field.p_ctype, "$Cpptype*", "$name", "", "Private pointer to value");
}

void amc::tfunc_Delptr_Init() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FFunc& init = amc::CreateCurFunc();
    init.inl = true;
    Ins(&R, init.body, "$parname.$name = NULL;");
}

void amc::tfunc_Delptr_Uninit() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FFunc& func = amc::CreateCurFunc();
    func.comment << "Delete value, if necessary";
    Ins(&R, func.body, "$name_Delete($pararg);");
}

void amc::tfunc_Delptr_Delete() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

    amc::FFunc& func = amc::CreateCurFunc(true); {
        AddRetval(func, "void", "", "");
        Ins(&R, func.comment, "Delete value.");
        Ins(&R, func.body, "if ($parname.$name) {");
        if (HasDtorQ(*field.p_arg)) {
            Ins(&R, func.body, "$parname.$name->~$Ctype();");
        }
        Ins(&R, func.body, "$basepool_FreeMem($parname.$name, sizeof($Cpptype));");
        Ins(&R, func.body, "$parname.$name = NULL;");
        Ins(&R, func.body, "}");
    }
}

void amc::tfunc_Delptr_Access() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;

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

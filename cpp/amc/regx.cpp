// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2008-2013 AlgoEngineering LLC
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
// Source: cpp/amc/regx.cpp -- Small strings
//

#include "include/amc.h"

void amc::tclass_RegxSql() {
    tclass_Regx();
    vrfy(amc::_db.genctx.p_field->c_fregx, "missing fregx");
}

void amc::tfunc_RegxSql_ReadStrptrMaybe() {
    tfunc_Regx_ReadStrptrMaybe();
}

void amc::tfunc_RegxSql_Print() {
    tfunc_Regx_Print();
}

void amc::tfunc_RegxSql_Init() {
    tfunc_Regx_Init();
}

// -----------------------------------------------------------------------------

void amc::tclass_Regx() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FFregx *fregx = field.c_fregx;
    Set(R, "$Regxtype", fregx ? strptr(fregx->regxtype) : strptr("Sql"));
    Set(R, "$dflt"   , strptr(field.dflt.value));

    // Provide a comment such as "Regx Sql of dev::Target
    // Or just "Regx Acr"
    tempstr comment("$Regxtype Regx");
    if (RelationalQ(*field.p_arg)) {
        comment << " of $Cpptype";
    }

    InsVar(R, field.p_ctype, "algo_lib::Regx", "$name", field.dflt.value, comment);
}

void amc::tfunc_Regx_ReadStrptrMaybe() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FFregx *fregx = field.c_fregx;
    if (HasReadQ(*field.p_ctype)) {
        Set(R, "$full", (fregx && fregx->partial ? "false" : "true"));
        amc::FFunc& read = amc::CreateCurFunc(true);
        Ins(&R, read.comment, "Convert string to field. Return success value");
        AddProtoArg(read, "algo::strptr", "in");
        AddRetval(read, "bool", "retval", "true");
        Ins(&R, read.body, "Regx_Read$Regxtype($parname.$name, in, $full);");
        SetPresent(read,Subst(R,"$parname"),field);
    }
}

void amc::tfunc_Regx_Print() {
    algo_lib::Replscope &R = amc::_db.genctx.R;

    amc::FFunc& print = amc::CreateCurFunc(true);
    AddRetval(print, "void", "", "");
    AddProtoArg(print, "algo::cstring &", "out");
    Ins(&R, print.body, "Regx_Print($parname.$name, out);");
}

void amc::tfunc_Regx_Init() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FFregx *fregx = field.c_fregx;
    Set(R, "$full", (fregx && fregx->partial ? "false" : "true"));

    if (ch_N(field.dflt.value)) {
        amc::FFunc& init = amc::CreateCurFunc();
        Ins(&R, init.body, "Regx_Read$Regxtype($parname.$name, $dflt, $full);");
    }
}

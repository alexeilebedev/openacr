// Copyright (C) 2008-2013 AlgoEngineering LLC
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2020-2021 Astra
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
// Source: cpp/amc/regx.cpp -- Small strings
//

#include "include/amc.h"

void amc::tclass_RegxSql() {
    tclass_Regx();
    vrfy(amc::_db.genfield.p_field->c_fregx, "missing fregx");
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
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
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
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FFregx *fregx = field.c_fregx;
    if (HasReadQ(*field.p_ctype)) {
        Set(R, "$full", (fregx && fregx->partial ? "false" : "true"));
        amc::FFunc& read = amc::CreateCurFunc();
        Ins(&R, read.comment, "Convert string to field. Return success value");
        Ins(&R, read.ret  , "bool", false);
        Ins(&R, read.proto, "$name_ReadStrptrMaybe($Parent, algo::strptr in)", false);
        Ins(&R, read.body, "Regx_Read$Regxtype($parname.$name, in, $full);");
        Ins(&R, read.body, "bool retval = true;// !$parname.$name.parseerror; -- TODO: uncomment");
        SetPresent(read,Subst(R,"$parname"),field);
        Ins(&R, read.body, "return retval;");
    }
}

void amc::tfunc_Regx_Print() {
    algo_lib::Replscope &R = amc::_db.genfield.R;

    amc::FFunc& print = amc::CreateCurFunc();
    Ins(&R, print.ret  , "void", false);
    Ins(&R, print.proto, "$name_Print($Parent, algo::cstring &out)", false);
    Ins(&R, print.body, "Regx_Print($parname.$name, out);");
}

void amc::tfunc_Regx_Init() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    if (ch_N(field.dflt.value)) {
        amc::FFunc& init = amc::CreateCurFunc();
        Ins(&R, init.body, "Regx_Read$Regxtype($parname.$name, $dflt, true);");
    }
}

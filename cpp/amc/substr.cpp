// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2008-2012 AlgoEngineering LLC
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
// Source: cpp/amc/substr.cpp
//

#include "include/amc.h"

void amc::tclass_Substr() {
}

void amc::tfunc_Substr_Get() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FSubstr *substr = field.c_substr;
    if (substr) {
        Set(R, "$Fldtype", field.cpp_type);
        Set(R, "$name"   , name_Get(field));

        amc::FFunc& get = amc::CreateCurFunc();
        Ins(&R, get.ret  , "$Fldtype", false);
        Ins(&R, get.proto, "$name_Get($Parent)", false);

        Set(R, "$substrexpr", substr->expr.value);
        Set(R, "$pathexpr", tempstr()<<"algo::Pathcomp("<<FieldvalExpr(field.p_ctype, *substr->p_srcfield,"$parname")<<", \"$substrexpr\")");

        if (ConstructFromStringQ(*field.p_arg)) {
            Ins(&R, get.body    , "$Fldtype ret($pathexpr);");
        } else {
            Set(R, "$dflt"   , DfltExprVal(field));
            Ins(&R, get.body    , "$Fldtype ret;");
            if (Subst(R,"$dflt") != "") {
                Ins(&R, get.body, "ret = $dflt; // default value");// hack -- compatibility with bool_ReadStrptr which doesn't throw
            }
            Ins(&R, get.body    , "(void)$Cpptype_ReadStrptrMaybe(ret, $pathexpr);");
        }
        Ins(&R, get.body        , "return ret;");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Substr_Get2() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FSubstr *substr = field.c_substr;
    bool ssimdb = field.p_ctype->p_ns->nstype == dmmeta_Nstype_nstype_ssimdb;
    if (substr && ssimdb) {
        Set(R, "$Fldtype", field.cpp_type);
        Set(R, "$name"   , name_Get(field));
        Set(R, "$Substrprefix", name_Get(*field.p_ctype));

        amc::FFunc& get = amc::CreateCurFunc();
        Ins(&R, get.ret  , "$Fldtype", false);
        Ins(&R, get.proto, "$Substrprefix_$name_Get(algo::strptr arg)", false);

        Set(R, "$substrexpr", substr->expr.value);
        Set(R, "$pathexpr", tempstr()<<"algo::Pathcomp(arg, \"$substrexpr\")");

        if (amc::ConstructFromStringQ(*field.p_arg)) {
            Ins(&R, get.body    , "$Fldtype ret($pathexpr);");
        } else {
            Set(R, "$dflt"   , DfltExprVal(field));
            Ins(&R, get.body    , "$Fldtype ret;");
            if (Subst(R,"$dflt") != "") {
                Ins(&R, get.body, "ret = $dflt; // default value");// hack -- compatibility with bool_ReadStrptr which doesn't throw
            }
            Ins(&R, get.body    , "(void)$Cpptype_ReadStrptrMaybe(ret, $pathexpr);");
        }
        Ins(&R, get.body        , "return ret;");
    }
}

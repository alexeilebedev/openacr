// Copyright (C) 2025-2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2021 Astra
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
// Source: cpp/amc/substr.cpp -- Substr fields
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

        // A string-typed substring is returned as a view (strptr) into the
        // source field's storage: copying it into a $Fldtype temporary invites
        // the caller to bind a strptr to that temporary, which dangles.
        bool retview = ConstructFromStringQ(*field.p_arg);
        amc::FFunc& get = amc::CreateCurFunc();
        Ins(&R, get.ret  , retview ? strptr("algo::strptr") : strptr("$Fldtype"), false);
        Ins(&R, get.proto, "$name_Get($Parent)", false);

        Set(R, "$substrexpr", substr->expr.value);
        Set(R, "$pathexpr", tempstr()<<"algo::Pathcomp("<<FieldvalExpr(field.p_ctype, *substr->p_srcfield,"$parname")<<", \"$substrexpr\")");

        if (retview) {
            Ins(&R, get.body    , "return $pathexpr;");
        } else {
            Set(R, "$dflt"   , DfltExprVal(field));
            Ins(&R, get.body    , "$Fldtype ret;");
            if (Subst(R,"$dflt") != "") {
                Ins(&R, get.body, "ret = $dflt; // default value");// hack -- compatibility with bool_ReadStrptr which doesn't throw
            }
            Ins(&R, get.body    , "(void)$Cpptype_ReadStrptrMaybe(ret, $pathexpr);");
            Ins(&R, get.body    , "return ret;");
        }
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

        // Same rule as tfunc_Substr_Get: a string-typed substring is a view,
        // here into the caller's arg, which the caller owns.
        bool retview = ConstructFromStringQ(*field.p_arg);
        amc::FFunc& get = amc::CreateCurFunc();
        Ins(&R, get.ret  , retview ? strptr("algo::strptr") : strptr("$Fldtype"), false);
        Ins(&R, get.proto, "$Substrprefix_$name_Get(algo::strptr arg)", false);

        Set(R, "$substrexpr", substr->expr.value);
        Set(R, "$pathexpr", tempstr()<<"algo::Pathcomp(arg, \"$substrexpr\")");

        if (retview) {
            Ins(&R, get.body    , "return $pathexpr;");
        } else {
            Set(R, "$dflt"   , DfltExprVal(field));
            Ins(&R, get.body    , "$Fldtype ret;");
            if (Subst(R,"$dflt") != "") {
                Ins(&R, get.body, "ret = $dflt; // default value");// hack -- compatibility with bool_ReadStrptr which doesn't throw
            }
            Ins(&R, get.body    , "(void)$Cpptype_ReadStrptrMaybe(ret, $pathexpr);");
            Ins(&R, get.body    , "return ret;");
        }
    }
}

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
// Source: cpp/amc/fcast.cpp -- Implicit casts
//

#include "include/amc.h"

void amc::tclass_Fcast() {
}

void amc::tfunc_Fcast_Cast() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FCtype &valtype = *field.p_arg;
    amc::FFcast &fcast = *field.c_fcast;

    tempstr casttype;
    if (ch_N(fcast.expr)) {
        casttype = fcast.expr;
    } else if (field.reftype == dmmeta_Reftype_reftype_Smallstr) {
        casttype = "algo::aryptr<char>";
    } else if (field.reftype == dmmeta_Reftype_reftype_Tary || field.reftype == dmmeta_Reftype_reftype_Inlary) {
        casttype = Subst(R,"algo::aryptr<$Cpptype>");
    }  else if (c_field_N(valtype)>0 && c_field_Find(valtype,0)->c_fcast) {
        casttype = c_field_Find(valtype,0)->c_fcast->expr;
    } else if (c_fconst_N(field) && !FieldStringQ(field)) {
        casttype = Enumtype(field);
    } else {
        casttype = valtype.cpp_type;
    }

    Set(R, "$casttype", casttype);
    Set(R, "$Get", FieldvalExpr(field.p_ctype, field, "(*this)"));
    amc::FFunc& get = amc::CreateCurFunc();
    get.inl = true;
    get.member = true;
    //get.ret = casttype;
    Ins(&R, get.proto, "operator $casttype() const", false);
    if (field.reftype == dmmeta_Reftype_reftype_Smallstr
        || field.reftype == dmmeta_Reftype_reftype_Tary
        || field.reftype == dmmeta_Reftype_reftype_Inlary) {
        Ins(&R, get.body, "return $name_Getary(*this);");
    } else {
        Ins(&R, get.body, "return $casttype($Get);");
    }
    //InsStruct(R, field.p_ctype, "inline operator $casttype() const;");// hack?
}

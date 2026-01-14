// Copyright (C) 2023-2026 AlgoRND
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

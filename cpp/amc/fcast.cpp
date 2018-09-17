// (C) AlgoEngineering LLC 2008-2012
// (C) 2013-2019 NYSE | Intercontinental Exchange
//
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
//
// Target: amc (exe) -- Algo Model Compiler: generate code under include/gen and cpp/gen
// Exceptions: NO
// Source: cpp/amc/fcast.cpp
//
// Created By: alexei.lebedev hayk.mkrtchyan
// Authors: alexei.lebedev
// Recent Changes: alexei.lebedev hayk.mkrtchyan
//

#include "include/amc.h"

void amc::tclass_Fcast() {
}

void amc::tfunc_Fcast_Cast() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FCtype &valtype = *field.p_arg;
    amc::FFcast &fcast = *field.c_fcast;

    tempstr casttype;
    if (field.reftype == dmmeta_Reftype_reftype_Smallstr) {
        casttype = "algo::strptr";
    } else if (ch_N(fcast.expr)) {
        casttype = fcast.expr;
    } else if (c_field_N(valtype)>0 && c_field_Find(valtype,0)->c_fcast) {
        casttype = c_field_Find(valtype,0)->c_fcast->expr;
    } else if (c_fconst_N(field) && !FieldStringQ(field)) {
        casttype = Enumtype(field);
    } else {
        casttype = valtype.cpp_type;
    }
    // #AL# todo: fix me!
    bool docast = !(field.field == "algo.cstring.ch");

    if (docast) {
        Set(R, "$casttype", casttype);
        Set(R, "$Get", FieldvalExpr(field.p_ctype, field, "(*this)"));
        amc::FFunc& get = amc::CreateCurFunc();
        get.oper = true;
        get.inl = true;
        get.ret = casttype;
        Ins(&R, get.proto, "() const", false);
        if (field.reftype == dmmeta_Reftype_reftype_Smallstr) {
            Ins(&R, get.body, "return $name_Getary(*this);");
        } else {
            Ins(&R, get.body, "return $casttype($Get);");
        }
        InsStruct(R, field.p_ctype, "inline operator $casttype() const;");// hack?
    }
}

// Copyright (C) 2025-2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
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
// Target: amc (exe) -- Algo Model Compiler: generate code under include/gen and cpp/gen
// Exceptions: yes
// Source: cpp/amc/alias.cpp -- Alias field type
//

#include "include/amc.h"

// True if FIELD is an alias of a field of another ctype: a global field
// naming another namespace's list so that a step can be declared on it.
// Such an alias has no value to get, set or read.
bool amc::ListAliasQ(amc::FField &field) {
    return field.c_falias && field.c_falias->p_srcfield->p_ctype != field.p_ctype;
}

void amc::tclass_Alias() {
}

void amc::tfunc_Alias_Get() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    if (!ListAliasQ(field)) {
        amc::FFunc& func = amc::CreateCurFunc();
        Set(R, "$basename", name_Get(*field.c_falias->p_srcfield));
        Ins(&R, func.comment, "Alias: value is retrieved from $basename");
        Ins(&R, func.ret  , "$Cpptype", false);
        Ins(&R, func.proto, "$name_Get($Cparent)", false);
        Set(R, "$FieldExpr", FieldvalExpr(field.p_ctype,*field.c_falias->p_srcfield,"parent"));
        func.inl = true;
        Ins(&R, func.body, "return $FieldExpr;");
    }
}

void amc::tfunc_Alias_Set() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FField &srcfield = *field.c_falias->p_srcfield;
    if (!ListAliasQ(field)) {
        amc::FFunc& func = amc::CreateCurFunc();
        Set(R, "$Fldargtype", Argtype(field));
        Set(R, "$basename", name_Get(srcfield));
        Ins(&R, func.comment, "Alias: value is assigned to $basename");
        AddRetval(func,"void","","");
        Ins(&R, func.proto, "$name_Set($Parent, $Fldargtype rhs)", false);
        Ins(&R, func.body, tempstr()<<AssignExpr(srcfield, Subst(R,"parent"), "rhs", false)<<";");
    }
}

void amc::tfunc_Alias_ReadStrptrMaybe() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FField &field = *amc::_db.genctx.p_field;
    amc::FField &srcfield = *field.c_falias->p_srcfield;
    if (!ListAliasQ(field)) {
        amc::FFunc& func = amc::CreateCurFunc();
        Ins(&R, func.comment, "Alias: value is read into $basename");
        Ins(&R, func.proto  , "$name_ReadStrptrMaybe()",false);
        AddProtoArg(func, Subst(R,"$Partype&"), "parent", true);
        AddProtoArg(func, "algo::strptr", "in_str", true);
        Set(R,"$ReadExpr",ReadFieldExpr(srcfield,"$parname","in_str"));
        AddRetval(func,"bool","retval",Subst(R,"$ReadExpr"));
    }
}

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
// Target: amc (exe) -- Algo Model Compiler: generate code under include/gen and cpp/gen
// Exceptions: NO
// Source: cpp/amc/alias.cpp
//

#include "include/amc.h"

void amc::tclass_Alias() {
}

void amc::tfunc_Alias_Get() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FFunc& func = amc::CreateCurFunc();
    Set(R, "$basename", name_Get(*field.c_falias->p_basefield));
    Ins(&R, func.comment, "Alias: value is retrieved from $basename");
    Ins(&R, func.ret  , "$Cpptype", false);
    Ins(&R, func.proto, "$name_Get($Cparent)", false);
    Set(R, "$FieldExpr", FieldvalExpr(field.p_ctype,*field.c_falias->p_basefield,"parent"));
    func.inl = true;
    Ins(&R, func.body, "return $FieldExpr;");
}

void amc::tfunc_Alias_Set() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FField &basefield = *field.c_falias->p_basefield;
    amc::FFunc& func = amc::CreateCurFunc();

    Set(R, "$Fldargtype", Argtype(field));
    Set(R, "$basename", name_Get(basefield));
    Ins(&R, func.comment, "Alias: value is assigned to $basename");
    AddRetval(func,"void","","");
    Ins(&R, func.proto, "$name_Set($Parent, $Fldargtype rhs)", false);
    Ins(&R, func.body, tempstr()<<AssignExpr(basefield, Subst(R,"parent"), "rhs", false)<<";");
}

void amc::tfunc_Alias_ReadStrptrMaybe() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FField &basefield = *field.c_falias->p_basefield;
    amc::FFunc& func = amc::CreateCurFunc();
    Ins(&R, func.comment, "Alias: value is read into $basename");
    Ins(&R, func.proto  , "$name_ReadStrptrMaybe()",false);
    AddProtoArg(func, Subst(R,"$Partype&"), "parent", true);
    AddProtoArg(func, "algo::strptr", "in_str", true);
    Set(R,"$ReadExpr",ReadFieldExpr(basefield,"$parname","in_str"));
    AddRetval(func,"bool","retval",Subst(R,"$ReadExpr"));
}

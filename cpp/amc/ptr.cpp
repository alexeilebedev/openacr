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
// Source: cpp/amc/ptr.cpp
//
// Created By: alexei.lebedev
// Authors: alexei.lebedev
// Recent Changes: alexei.lebedev hayk.mkrtchyan
//

#include "include/amc.h"

void amc::tclass_Ptr() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    Set(R, "$dflt"   , field.dflt.value);
    Set(R, "$Fldtype", field.cpp_type);
    Set(R, "$name"   , name_Get(field));
    Set(R, "$comment", field.comment);
    Set(R, "$getfld", FieldvalExpr(field.p_ctype, field, "$parname"));
    //Set(R, "$getfld", FieldvalExpr(field.p_ctype, field, GlobalQ(*field.p_ctype) ? "_db" : "row"));

    vrfy(!ch_N(field.dflt.value)
         , tempstr()<<"amc.ptr_dflt"
         <<Keyval("field",field.field)
         <<Keyval("dflt",field.dflt.value)
         <<Keyval("comment","Default value for Ptr is not supported (it's always NULL)"));

    if (!FldfuncQ(field)) {
        InsVar(R, field.p_ctype, "$Cpptype*", "$name", "", "$comment. optional pointer");
    }
}

void amc::tfunc_Ptr_Init() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    if (!FldfuncQ(field)) {
        amc::FFunc& init = amc::CreateCurFunc();
        init.inl = true;
        Ins(&R, init.body, "$parname.$name = NULL;");
    }
}

void amc::tfunc_Ptr_InsertMaybe() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    if (!FldfuncQ(field) && field.c_xref) {// xref functions -- the pointer acts like an index.
        amc::FFunc& ins = amc::CreateCurFunc();
        ins.inl = true;
        Ins(&R, ins.comment, "Insert row into pointer index. Return final membership status.");
        Ins(&R, ins.ret  , "bool", false);
        Ins(&R, ins.proto, "$name_InsertMaybe($Parent, $Cpptype& row)", false);
        Ins(&R, ins.body, "$Cpptype* ptr = $getfld;");
        Ins(&R, ins.body, "bool retval = (ptr == NULL) | (ptr == &row);");
        Ins(&R, ins.body, "if (retval) {");
        Ins(&R, ins.body, "    $parname.$name = &row;");
        Ins(&R, ins.body, "}");
        Ins(&R, ins.body, "return retval;");
    }
}

void amc::tfunc_Ptr_Remove() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    if (!FldfuncQ(field) && field.c_xref) {
        amc::FFunc& remove = amc::CreateCurFunc();
        Ins(&R, remove.ret  , "void", false);
        Ins(&R, remove.proto, "$name_Remove($Parent, $Cpptype& row)", false);
        Ins(&R, remove.body, "$Cpptype *ptr = $getfld;");
        Ins(&R, remove.body, "if (LIKELY(ptr == &row)) {");
        Ins(&R, remove.body, "    $parname.$name = NULL;");
        Ins(&R, remove.body, "}");
    }

}

void amc::tfunc_Ptr_Cascdel() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    if (!FldfuncQ(field) && field.c_cascdel) {
        amc::FFunc& cascdel = amc::CreateCurFunc();
        Ins(&R, cascdel.comment, "Deleted pointed-to item.");
        Ins(&R, cascdel.body, "$Cpptype *ptr = $getfld;");
        Ins(&R, cascdel.body, "if (ptr) {");
        Ins(&R, cascdel.body, DeleteExpr(field,"$pararg","*ptr")<<";");
        Ins(&R, cascdel.body, "    $parname.$name = NULL;");
        Ins(&R, cascdel.body, "}");
    }

}

void amc::tfunc_Ptr_Print() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    if (!FldfuncQ(field)) {
        amc::FFunc& print = amc::CreateCurFunc();
        Ins(&R, print.comment, "Default format");
        Ins(&R, print.ret  , "void", false);
        // this is a terrible hack. the print function is inlined into the ctype
        // print function. I know, having looked at the code, that ctype
        // print uses words 'row' and 'temp'.
        Set(R, "$xgetfld", FieldvalExpr(field.p_ctype, field, GlobalQ(*field.p_ctype) ? "_db" : "row"));
        Ins(&R, print.proto, "$name_Print($Cpptype &row, cstring &temp)", false);
        Ins(&R, print.body, "u64_PrintHex(u64((const $Cpptype*)$xgetfld), temp, 8, true);");
    }
}

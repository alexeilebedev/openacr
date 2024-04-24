// Copyright (C) 2023-2024 AlgoRND
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
// Source: cpp/amc/opt.cpp
//

#include "include/amc.h"

void amc::tclass_Opt() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    InsStruct(R, field.p_ctype, "// $Cpptype    \t$name[]; \t \t \toptional field");
    Set(R, "$lenexpr", LengthExpr(*field.p_ctype, Subst(R,"$parname")));
    EnsureVarlenPool(*field.p_ctype);
}

void amc::tfunc_Opt_Get() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    // access opt option
    amc::FFunc& get = amc::CreateCurFunc();
    // if child has a length field, can't evaluate this expression inline
    get.inl=!field.p_arg->c_lenfld
        || HdrIncludesQ(*field.p_ctype->p_ns, *field.p_arg->p_ns);
    if (!get.inl) {
        verblog("amc.opt_noninline"
                <<Keyval("field",field.field)
                <<Keyval("comment","could not inline this getter"));
    }
    Set(R, "$childsize", SizeEnum(*field.p_arg));
    Ins(&R, get.comment, "Return pointer to optional last element (NULL if none)");
    Ins(&R, get.comment, "The parent's length field is assumed to have already been checked for validity.");
    Ins(&R, get.comment, "If the parent's lengthis too short to allow a full-sized $name, return NULL.");
    if (field.p_arg->c_lenfld) {
        Ins(&R, get.comment, "Since the last element is itself variable-length, two more checks are applied:");
        Ins(&R, get.comment, "If $name's length field value is too short, return NULL.");
        Ins(&R, get.comment, "If $name's length field value extends past parent's allowed length, return NULL.");
    }
    Ins(&R, get.ret  , "$Cpptype*",false);
    Ins(&R, get.proto, "$name_Get($Parent)",false);
    Ins(&R, get.body    , "i32 totlen = $lenexpr;");
    Ins(&R, get.body    , "u8 *end = (u8*)&$parname + sizeof($Partype);");
    Ins(&R, get.body    , "$Cpptype *ptr = ($Cpptype*)end;");
    Ins(&R, get.body    , "bool exists = ssizeof($Partype) + $childsize <= totlen;");
    // check if child type has a lenfld; if so, validate length
    // can't evaluate child's length if it doesn't fit -- so the above check still holds!
    if (field.p_arg->c_lenfld) {
        Set(R, "$childlenexpr", LengthExpr(*field.p_arg, "(*ptr)"));
        //Ins(&R, get.body, "i32 childlen = $childlenexpr;");
        Ins(&R, get.body, "exists = exists && $childlenexpr >= $childsize && ssizeof($Partype) + $childlenexpr <= totlen;");
    }
    Ins(&R, get.body    , "return exists ? ptr : NULL;");
}

void amc::tfunc_Opt_Getary() {
    algo_lib::Replscope &R = amc::_db.genfield.R;

    amc::FFunc& getary = amc::CreateCurFunc();
    Ins(&R, getary.ret  , "algo::aryptr<u8>", false);
    Ins(&R, getary.proto, "$name_Getary($Parent)", false);
    Ins(&R, getary.body , "u8 *end = (u8*)&$parname + sizeof($Partype);");
    Ins(&R, getary.body , "return algo::aryptr<u8>(end, $lenexpr - ssizeof($Partype));");
}

void amc::tfunc_Opt_Print() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    // Provide a print function for the field
    if (HasStringPrintQ(*field.p_ctype) && HasStringPrintQ(*field.p_arg)) {
        amc::FFunc& print = amc::CreateCurFunc();
        Ins(&R, print.ret     , "void", false);
        Ins(&R, print.proto   , "$name_Print($Parent, cstring &out)", false);
        Ins(&R, print.body    , "(void)out;");
        Ins(&R, print.body    , "(void)$pararg;");
        Ins(&R, print.body    , "if ($Cpptype *$name = $name_Get($pararg)) {");
        if (field.p_arg->c_typefld) {
            Set(R, "$Fldhdrtype", field.p_arg->c_typefld->p_ctype->cpp_type);
            Ins(&R, print.body, "    $FldhdrtypeMsgs_Print(out, *$name, INT_MAX);");
        } else {
            Ins(&R, print.body, "    out << *$name;");
        }
        Ins(&R, print.body    , "}");
    }
}

void amc::tfunc_Opt_ReadStrptrMaybe() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    if (HasReadQ(*field.p_ctype)) {
        amc::FFunc& doread = amc::CreateCurFunc();
        Ins(&R, doread.comment, "Convert string to field. Return success value");
        Ins(&R, doread.ret  , "bool", false);
        Ins(&R, doread.proto, "$name_ReadStrptrMaybe($Partype &parent, algo::strptr in_str)", false);
        Ins(&R, doread.body, "bool retval = false;");
        if (field.p_arg->c_typefld) {
            Set(R, "$Fldhdrtype", field.p_arg->c_typefld->p_ctype->cpp_type);
            Ins(&R, doread.body, "if (algo_lib::_db.varlenbuf) {");
            Ins(&R, doread.body, "    algo::ByteAry temp;");
            Ins(&R, doread.body, "    retval = $FldhdrtypeMsgs_ReadStrptrMaybe(in_str, temp); // read any of several message types here");
            Ins(&R, doread.body, "    ary_Setary(*algo_lib::_db.varlenbuf, ary_Getary(temp)); // return it");
            Ins(&R, doread.body, "}");
            SetPresent(doread,Subst(R,"$parname"),field);
        } else if (FindStringRead(*field.p_arg) && field.p_arg->c_lenfld) {
            // try to use read function
            amc::FLenfld& lenfld = *field.p_arg->c_lenfld;
            Set(R, "$Msgname", name_Get(*field.p_arg));
            Set(R, "$Ctype", amc::NsToCpp(field.p_arg->ctype));
            cstring lenassign("len");
            if (lenfld.extra > 0) {
                lenassign << "+" << lenfld.extra;
            } else if (lenfld.extra < 0) {
                lenassign << lenfld.extra;
            }
            Set(R, "$assignlen", AssignExpr(*lenfld.p_field, "*ctype", lenassign, true));
            Ins(&R, doread.body, "if (algo::ByteAry* varlenbuf_save = algo_lib::_db.varlenbuf) {");
            Ins(&R, doread.body, "    int len = sizeof($Ctype);");
            Ins(&R, doread.body, "    $Ctype *ctype = new(ary_AllocN(*varlenbuf_save, len).elems) $Ctype; // default values");
            Ins(&R, doread.body, "    algo::ByteAry varlenbuf; // use for varlen msgs");
            Ins(&R, doread.body, "    algo_lib::_db.varlenbuf = &varlenbuf;");
            Ins(&R, doread.body, "    retval = $Msgname_ReadStrptrMaybe(*ctype,in_str); // read the type");
            Ins(&R, doread.body, "    len += ary_N(varlenbuf);");
            Ins(&R, doread.body, "    $assignlen;");
            Ins(&R, doread.body, "    ary_Addary(*varlenbuf_save, ary_Getary(varlenbuf));");
            Ins(&R, doread.body, "    algo_lib::_db.varlenbuf = varlenbuf_save;");
            Ins(&R, doread.body, "}");
            SetPresent(doread,Subst(R,"$parname"),field);
        } else {
            Ins(&R, doread.body, "// field cannot be read");
        }
        MaybeUnused(doread, "parent");
        MaybeUnused(doread, "in_str");
        Ins(&R, doread.body, "return retval;");
    }
}

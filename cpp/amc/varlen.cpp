// Copyright (C) 2008-2012 AlgoEngineering LLC
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
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
// Source: cpp/amc/varlen.cpp
//

#include "include/amc.h"

void amc::tclass_Varlen() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    InsStruct(R, field.p_ctype, "// var-length field $field starts here. access it with $name_Addr");
    Set(R, "$lenexpr", LengthExpr(*field.p_ctype, Subst(R,"$parname")));
    Set(R, "$curslenexpr", LengthExpr(*field.p_ctype, Subst(R,"parent")));
    Set(R, "$rettype",field.p_arg->c_lenfld ? "u8" : "$Cpptype");

    // check that the variable-length portion
    // doesn't declare a destructor
    if (field.p_arg->c_cpptype && field.p_arg->c_cpptype->dtor) {
        prerr("amc.varlen_pool"
              <<Keyval("field",field.field)
              <<Keyval("arg",field.arg)
              <<Keyval("comment","Variable-length portion must be of DTOR:N type"));
        algo_lib::_db.exit_code=1;
    }
    EnsureVarlenPool(*field.p_ctype);

    // Ensure that this field is last
    bool seen = false;
    ind_beg(amc::ctype_c_field_curs, _field,*field.p_ctype) {
        if (seen) {
            prerr("amc.varlen_last"
                  <<Keyval("field",field.field)
                  <<Keyval("comment","Variable-length field must be last"));
            algo_lib::_db.exit_code=1;
        }
        seen = seen || &field == &_field;
    }ind_end;

}

void amc::tfunc_Varlen_Addr() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FFunc& addr = amc::CreateCurFunc();
    Ins(&R, addr.ret  , "$rettype*", false);
    Ins(&R, addr.proto, "$name_Addr($Parent)", false);
    Ins(&R, addr.body, "return ($rettype*)((u8*)&$parname + sizeof($Partype)); // address of varlen portion");
}

void amc::tfunc_Varlen_Getary() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    if (field.p_ctype->c_lenfld) {
        // withhold defining Getary
        // because it breaks the print function on message types where length is not known.
        //
        amc::FFunc& get = amc::CreateCurFunc();
        Ins(&R, get.ret  , "algo::aryptr<$rettype>", false);
        Ins(&R, get.proto, "$name_Getary($Parent)", false);
        Ins(&R, get.body, "return algo::aryptr<$rettype>($name_Addr($pararg), $name_N($pararg));");
    }
}

void amc::tfunc_Varlen_N() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    // N: number of elements in varlen portion
    amc::FFunc& get_n = amc::CreateCurFunc();
    Ins(&R, get_n.ret  , "u32",false);
    Ins(&R, get_n.proto, "$name_N($Cparent)",false);
    if (field.p_ctype->c_lenfld) {
        Set(R, "$lenexpr", LengthExpr(*field.p_ctype, "(($Partype&)$parname)"));
        Ins(&R, get_n.body, "u32 length = $lenexpr;");
        Ins(&R, get_n.body, "u32 extra_bytes = u32_Max(length,sizeof($Partype)) - sizeof($Partype); // avoid unsigned subtraction underflow");
        Ins(&R, get_n.body, "return u32(extra_bytes / sizeof($rettype));");
    } else {
        get_n.extrn=true;
    }
}

void amc::tfunc_Varlen_ReadStrptrMaybe() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    if (HasReadQ(*field.p_ctype)) {
        amc::FFunc& rd = amc::CreateCurFunc();
        Ins(&R, rd.comment, "Convert string to field. Return success value");
        Ins(&R, rd.ret  , "bool", false);
        Ins(&R, rd.proto, "$name_ReadStrptrMaybe($Parent, algo::strptr in_str)", false);
        Ins(&R, rd.body, "bool retval = true;");
        Ins(&R, rd.body, "if (algo_lib::_db.varlenbuf) {");
        if ((field.arg == "char" || field.arg == "u8")) {
            Ins(&R, rd.body , "    ary_Addary(*algo_lib::_db.varlenbuf, strptr_ToMemptr(in_str));");
        } else if (field.p_arg->c_typefld) {
            Set(R, "$Fldhdrtype", field.p_arg->c_typefld->p_ctype->cpp_type);
            Ins(&R, rd.body, "    algo::ByteAry temp;");
            Ins(&R, rd.body, "    retval = $FldhdrtypeMsgs_ReadStrptrMaybe(in_str, temp); // read any of several message types here");
            Ins(&R, rd.body, "    ary_Addary(*algo_lib::_db.varlenbuf, ary_Getary(temp)); // return it");
        } else {
            Set(R, "$Fldcpptype", field.cpp_type);
            Ins(&R, rd.body , "    $Fldcpptype *$name_tmp = new(ary_AllocN(*algo_lib::_db.varlenbuf, sizeof($Fldcpptype)).elems) $Fldcpptype;");
            Ins(&R, rd.body , "    retval = $Cpptype_ReadStrptrMaybe(*$name_tmp, in_str);");
        }
        Ins(&R, rd.body, "}");
        SetPresent(rd,Subst(R,"$parname"),field);
        MaybeUnused(rd,Subst(R,"$parname"));
        MaybeUnused(rd,"in_str");
        Ins(&R, rd.body, "return retval;");
    }
}

void amc::tfunc_Varlen_curs() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FNs &ns = *amc::_db.genfield.p_field->p_ctype->p_ns;
    if (field.p_ctype->c_lenfld) {

        Ins(&R, ns.curstext, "");
        Ins(&R, ns.curstext, "struct $Parname_$name_curs {// cursor");
        Ins(&R, ns.curstext, "    typedef $Cpptype ChildType;");
        Ins(&R, ns.curstext, "    u8 *ptr;");
        Ins(&R, ns.curstext, "    int length;");
        Ins(&R, ns.curstext, "    int index;"); // index for printing
        Ins(&R, ns.curstext, "    $Parname_$name_curs() { ptr=NULL; length=0; index=0; }");
        Ins(&R, ns.curstext, "};");
        Ins(&R, ns.curstext, "");

        {
            amc::FFunc& curs_reset = amc::CreateInlineFunc(Subst(R,"$field_curs.Reset"));
            Ins(&R, curs_reset.ret  , "void", false);
            Ins(&R, curs_reset.proto, "$Parname_$name_curs_Reset($Parname_$name_curs &curs, $Partype &parent)", false);
            Ins(&R, curs_reset.body, "curs.ptr = (u8*)&parent + sizeof($Partype);");
            Ins(&R, curs_reset.body, "curs.length = $curslenexpr - sizeof($Partype);");
            Ins(&R, curs_reset.body, "curs.index = 0;");
        }

        {
            amc::FFunc& curs_validq = amc::CreateInlineFunc(Subst(R,"$field_curs.ValidQ"));
            Ins(&R, curs_validq.comment, "cursor points to valid item");
            Ins(&R, curs_validq.ret  , "bool", false);
            Ins(&R, curs_validq.proto, "$Parname_$name_curs_ValidQ($Parname_$name_curs &curs)", false);
            Ins(&R, curs_validq.body, "bool valid = ssizeof($Cpptype) <= curs.length;");
            if (field.p_arg->c_lenfld) {
                Set(R, "$childlenexpr", LengthExpr(*field.p_arg, "(*($Cpptype*)curs.ptr)"));
                // check that len is >= ssizeof($Cppexpr) && <= curs.length (remaining length)
                // using unsigned comparison trick
                Ins(&R, curs_validq.body, "valid = valid && unsigned($childlenexpr-ssizeof($Cpptype)) <= curs.length-ssizeof($Cpptype);");
            }
            Ins(&R, curs_validq.body, "return valid;");
        }

        {
            amc::FFunc& curs_next = amc::CreateInlineFunc(Subst(R,"$field_curs.Next"));
            Ins(&R, curs_next.comment, "proceed to next item");
            Ins(&R, curs_next.ret  , "void", false);
            Ins(&R, curs_next.proto, "$Parname_$name_curs_Next($Parname_$name_curs &curs)", false);
            Set(R, "$childlenexpr", LengthExpr(*field.p_arg, "(*($Cpptype*)curs.ptr)"));
            Ins(&R, curs_next.body, "i32 len = $childlenexpr;");
            Ins(&R, curs_next.body, "curs.ptr += len;");
            Ins(&R, curs_next.body, "curs.length -= len;");
            Ins(&R, curs_next.body, "++curs.index;");
        }

        {
            amc::FFunc& curs_access = amc::CreateInlineFunc(Subst(R,"$field_curs.Access"));
            Ins(&R, curs_access.comment, "item access");
            Ins(&R, curs_access.ret  , "$Cpptype&", false);
            Ins(&R, curs_access.proto, "$Parname_$name_curs_Access($Parname_$name_curs &curs)", false);
            Ins(&R, curs_access.body, "return *($Cpptype*)curs.ptr;");
        }
    }
}

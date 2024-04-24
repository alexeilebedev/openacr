// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2019 NYSE | Intercontinental Exchange
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
// Source: cpp/amc/msgcurs.cpp -- Message scanning cursor
//

#include "include/amc.h"

// -----------------------------------------------------------------------------

static void CursLookup(algo_lib::Replscope &R, amc::FField &lenfld, amc::FFunc &func) {
    Set(R, "$getlength", amc::LengthExpr(*lenfld.p_ctype, "(*ptr)"));
    Ins(&R, func.body, "$Cpptype *msg = NULL;");
    Ins(&R, func.body, "int msglen = 0;");
    Ins(&R, func.body, "if (curs.limit >= ssizeof($Cpptype)) {");
    Ins(&R, func.body, "    $Cpptype *ptr = ($Cpptype*)curs.bytes;");
    Ins(&R, func.body, "    msglen = $getlength;");
    Ins(&R, func.body, "    if (msglen >= ssizeof($Cpptype) && curs.limit >= msglen) {");
    Ins(&R, func.body, "        msg = ptr;");
    Ins(&R, func.body, "    }");
    Ins(&R, func.body, " }");
    Ins(&R, func.body, "curs.msg = msg;");
    Ins(&R, func.body, "curs.msglen = msglen;");
}

static amc::FFunc &CreateCursFunc(amc::FCtype &ctype, strptr name) {
    amc::FFunc &func = amc::ind_func_GetOrCreate(tempstr()<<ctype.ctype<<".."<<name);
    func.nothrow=true;
    func.glob=true;
    func.inl=true;
    func.proto<<name_Get(ctype)<<"_"<<name<<"()";
    return func;
}

// -----------------------------------------------------------------------------

// // Cursor for byte-array of xyz messages
// // Usage:
// // ind_beg(xyz::MsgHeader_curs,msghdr,byteary) {
// //     < do something >
// // }ind_end;
// struct MsgHeader_curs {
//     typedef $Cpptype *ChildType;
//     $Cpptype *msg;
//     u8 *bytes;
//     int limit;
//     int msglen;
// };
static void GenMsgcurs(amc::FCtype &fctype, amc::FCtype &parent) {
    amc::FField &lenfld = *parent.c_lenfld->p_field;
    algo_lib::Replscope R;
    Set(R,"$Cpptype",amc::NsToCpp(parent.ctype));
    Ins(&R, fctype.body, "typedef $Cpptype *ChildType;");// add a typedef
    amc::InsField(dmmeta::Field(tempstr()<<fctype.ctype<<".msg"
                                , parent.ctype
                                , dmmeta_Reftype_reftype_Ptr
                                , algo::CppExpr("")
                                , algo::Comment("Pointer to current message")));
    amc::InsField(dmmeta::Field(tempstr()<<fctype.ctype<<".bytes"
                                , "u8"
                                , dmmeta_Reftype_reftype_Ptr
                                , algo::CppExpr("")
                                , algo::Comment("Beginning of region")));
    amc::InsField(dmmeta::Field(tempstr()<<fctype.ctype<<".limit"
                                , "i32"
                                , dmmeta_Reftype_reftype_Val
                                , algo::CppExpr("")
                                , algo::Comment("# Of bytes in the region")));
    amc::InsField(dmmeta::Field(tempstr()<<fctype.ctype<<".msglen"
                                , "i32"
                                , dmmeta_Reftype_reftype_Val
                                , algo::CppExpr("")
                                , algo::Comment("Length of current message (if any)")));
    amc::pack_InsertMaybe(dmmeta::Pack(fctype.ctype, algo::Comment()));
    {
        amc::FFunc &func = CreateCursFunc(fctype, "ValidQ");
        AddProtoArg(func, Subst(R,"$Cpptype_curs&"), "curs");
        amc::AddRetval(func, "bool", "", "");
        Ins(&R, func.body, "return curs.msg != NULL;");
    }
    {
        amc::FFunc &func = CreateCursFunc(fctype, "Reset");
        AddProtoArg(func, Subst(R,"$Cpptype_curs&"), "curs");
        AddProtoArg(func, "algo::memptr", "buf");
        amc::AddRetval(func, "void", "", "");
        Ins(&R, func.body, "curs.bytes = buf.elems;");
        Ins(&R, func.body, "curs.limit = buf.n_elems;");
        CursLookup(R, lenfld, func);
    }
    {
        amc::FFunc &func = CreateCursFunc(fctype, "Access");
        AddProtoArg(func, Subst(R,"$Cpptype_curs&"), "curs");
        amc::AddRetval(func, Subst(R,"$Cpptype*&"), "", "");
        Ins(&R, func.body, "return curs.msg;");
    }
    {
        amc::FFunc &func = CreateCursFunc(fctype, "Next");
        AddProtoArg(func, Subst(R,"$Cpptype_curs&"), "curs");
        amc::AddRetval(func, "void", "", "");
        Ins(&R, func.body, "curs.bytes += curs.msglen;");
        Ins(&R, func.body, "curs.limit -= curs.msglen;");
        CursLookup(R, lenfld, func);
    }
}

// -----------------------------------------------------------------------------

// Generate a message-scanning cursor for each message header with length
void amc::gen_msgcurs() {
    ind_beg(amc::_db_ctype_curs,ctype,amc::_db) if (ctype.c_lenfld) {
        amc::FCtype *fctype
            = ctype_InsertMaybe(dmmeta::Ctype(tempstr() << ctype.ctype << "_curs"
                                              , algo::Comment("Cursor for scanning messages in a memptr")));
        if (fctype) {
            GenMsgcurs(*fctype, ctype);
        }
    }ind_end;
}

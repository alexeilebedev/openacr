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
// Source: cpp/amc/bitfld.cpp
//

#include "include/amc.h"

void amc::tclass_Bitfld() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;

    Set(R, "$Fldtype", field.cpp_type);
    Set(R, "$name"   , name_Get(field));
    amc::FBitfld &bitfld = *field.c_bitfld;

    vrfy(field.c_bitfld, tempstr()<<"amc.where_is_the_bitfld  field:"<<field.field);
    vrfy(bitfld.width <= 64, tempstr()<<"amc.bitfield_width  field:"<<field.field<<"  comment:'max bitfield width is 64 bits'");

    u64 mask = bitfld.width==64 ? u64(0xffffffffffffffff) : (u64(1) << bitfld.width)-1;
    tempstr hexval;
    u64_PrintHex(mask, hexval, 2, true, false);// 0xffffffff for a 32-bit wide field, or 0x3ff for 9-bit wide field

    amc::FField &srcfield = *bitfld.p_srcfield;

    Set(R, "$srcfldname"  , name_Get(srcfield));// name of source field
    Set(R, "$offset"  , tempstr()<<bitfld.offset);
    Set(R, "$width"   , tempstr()<<bitfld.width);
    // type with which mask operations are performed -- same as field type
    Set(R, "$valtype" , amc::StripWrappers(*srcfield.p_arg,srcfield.p_arg)->ctype);
    Set(R, "$mask"    , hexval);
    Set(R, "$srcfldval" , FieldvalExpr(field.p_ctype, *bitfld.p_srcfield, "$parname"));
}

void amc::tfunc_Bitfld_Init() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    Set(R, "$dflt", DfltExprBitfld(field));
    if (ch_N(Subst(R,"$dflt")) > 0) {
        amc::FFunc& init = amc::CreateCurFunc();
        Ins(&R, init.body, "$name_Set($parname, $dflt); // default value");
    }
}

void amc::tfunc_Bitfld_Get() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FBitfld &bitfld = *field.c_bitfld;
    amc::FField &srcfield = *bitfld.p_srcfield;

    amc::FFunc& get = amc::CreateCurFunc();
    Ins(&R, get.comment, "Retrieve bitfield from value of field $srcfldname");
    Ins(&R, get.comment, "   $width bits starting at bit $offset.");
    if (srcfield.c_fbigend) {
        Ins(&R, get.comment, "   NOTE: bits correspond to the the integer value of the field.");
        Ins(&R, get.comment, "   The value is obtained by reading bytes from memory and swapping them.");
    }
    Ins(&R, get.ret  , "$Fldtype", false);
    Ins(&R, get.proto, "$name_Get(const $Parent)", false);
    Ins(&R, get.body, "return $Fldtype(($srcfldval >> $offset) & $mask);");
}

void amc::tfunc_Bitfld_Set() {
    algo_lib::Replscope &R = amc::_db.genfield.R;
    amc::FField &field = *amc::_db.genfield.p_field;
    amc::FBitfld &bitfld = *field.c_bitfld;
    amc::FField &srcfield = *bitfld.p_srcfield;

    amc::FFunc& set = amc::CreateCurFunc();
    Ins(&R, set.ret  , "void", false);
    Ins(&R, set.proto, "$name_Set($Parent, $Fldtype rhs)", false);

    Ins(&R, set.comment, "Set bitfield in value of field '$srcfldname'");
    Ins(&R, set.comment, "   $width bits starting at bit $offset.");
    if (srcfield.c_fbigend) {
        Ins(&R, set.comment, "   NOTE: bits correspond to the integer value of '$srcfldname'.");
        Ins(&R, set.comment, "   $srcfldname is stored in memory as big-endian.");
        Ins(&R, set.comment, "   The value is obtained by reading bytes from memory and swapping them.");
    }
    Ins(&R, set.body,     "$valtype t1    = $valtype($mask) << $offset;");
    if (c_datafld_N(*field.p_arg) == 1) {
        Set(R, "$singlefldval", FieldvalExpr(field.p_arg, *c_datafld_Find(*field.p_arg,0), "rhs"));
        // for case where rhs does not have implicit cast to integer
        // -- what if single field also does not?
        Ins(&R, set.body, "$valtype t2    = ($valtype($singlefldval) & $mask) << $offset;");
    } else {
        Ins(&R, set.body, "$valtype t2    = ($valtype(rhs) & $mask) << $offset;");
    }
    Ins(&R, set.body, AssignExpr(*bitfld.p_srcfield, "$parname", "($srcfldval & ~t1) | t2", true)<<";");
}

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
// Source: cpp/amc/bitset.cpp
//
// Created By: alexei.lebedev hayk.mkrtchyan
// Authors: alexei.lebedev
// Recent Changes: alexei.lebedev hayk.mkrtchyan jeffrey.wang
//

#include "include/amc.h"

void amc::tclass_Bitset() {
    algo_lib::Replscope &R        = amc::_db.genfield.R;
    amc::FField         &field    = *amc::_db.genfield.p_field;
    amc::FFbitset &fbitset = *field.c_fbitset;
    amc::FCtype &elemtype = *field.p_arg;

    u32 elem_bits = elemtype.c_csize ? elemtype.c_csize->size*8 :0;
    if (elem_bits==0) {
        prerr("amc.GenBitset"
              <<Keyval("fbitset",fbitset.field)
              <<Keyval("comment","Cannot determine size of underlying field. Use u8,u16,u32, or u64"));
        algo_lib::_db.exit_code++;
    }

    Set(R, "$idxshift"  , tempstr()<<algo::FloorLog2(elem_bits));
    Set(R, "$shiftmask" , tempstr()<<(1 << algo::FloorLog2(elem_bits))-1);
    Set(R, "$elembits"  , tempstr()<<elem_bits);
}

// -----------------------------------------------------------------------------

void amc::tfunc_Bitset_N() {
    algo_lib::Replscope &R        = amc::_db.genfield.R;
    amc::FField         &field    = *amc::_db.genfield.p_field;
    bool singleval = !field.c_inlary && !field.c_tary;
    // support single-element bitset
    if (singleval) {
        amc::FFunc &n = amc::CreateCurFunc();
        Ins(&R, n.comment, "Return constant 1");
        Ins(&R, n.ret  , "int",false);
        Ins(&R, n.proto, "$name_N($Parent)",false);
        Ins(&R, n.body, "(void)$parname;");
        Ins(&R, n.body, "return 1;");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Bitset_qFind() {
    algo_lib::Replscope &R        = amc::_db.genfield.R;
    amc::FField         &field    = *amc::_db.genfield.p_field;
    bool singleval = !field.c_inlary && !field.c_tary;
    if (singleval) {
        // qfind -- always return value itself
        amc::FFunc& qfind = amc::CreateCurFunc();
        Ins(&R, qfind.comment, "Access value");
        Ins(&R, qfind.ret  , "$Cpptype&", false);
        Ins(&R, qfind.proto, "$name_qFind($Parent, int)", false);
        Ins(&R, qfind.body, "return $parname.$name;");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Bitset_NBits() {
    algo_lib::Replscope &R        = amc::_db.genfield.R;
    amc::FFunc &nbits = amc::CreateCurFunc();
    Ins(&R, nbits.comment, "Return max. number of bits supported by array");
    Ins(&R, nbits.ret  , "int",false);
    Ins(&R, nbits.proto, "$name_Nbits($Parent)",false);
    Ins(&R, nbits.body, "return $name_N($pararg) * $elembits;");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Bitset_qGetBit() {
    algo_lib::Replscope &R        = amc::_db.genfield.R;
    amc::FFunc &qgetbit = amc::CreateCurFunc();
    Ins(&R, qgetbit.comment, "Retrieve value of bit #BIT_IDX in bit set. No bounds checking");
    Ins(&R, qgetbit.ret  , "bool",false);
    Ins(&R, qgetbit.proto, "$name_qGetBit($Parent, u32 bit_idx)",false);
    Ins(&R, qgetbit.body, "u64 elem_idx = bit_idx >> $idxshift;");
    Ins(&R, qgetbit.body, "u64 shift = bit_idx & $shiftmask;");
    Ins(&R, qgetbit.body, "$Cpptype &elem = $name_qFind($pararg, elem_idx); // fetch element");
    Ins(&R, qgetbit.body, "return bool((elem >> shift) & 1); // extract bit");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Bitset_GetBit() {
    algo_lib::Replscope &R        = amc::_db.genfield.R;
    amc::FFunc &getbit = amc::CreateCurFunc();
    Ins(&R, getbit.comment, "Retrieve value of bit #BIT_IDX in bit set. If bit index is out of bounds, return 0.");
    Ins(&R, getbit.ret  , "bool",false);
    Ins(&R, getbit.proto, "$name_GetBit($Parent, u32 bit_idx)",false);
    Ins(&R, getbit.body, "u64 elem_idx = bit_idx >> $idxshift;");
    Ins(&R, getbit.body, "u64 shift = bit_idx & $shiftmask;");
    Ins(&R, getbit.body, "bool ret = false;");
    Ins(&R, getbit.body, "u64 lim = $name_N($parname);");
    Ins(&R, getbit.body, "if (elem_idx < lim) {");
    Ins(&R, getbit.body, "    $Cpptype &elem = $name_qFind($pararg, elem_idx); // fetch element");
    Ins(&R, getbit.body, "    ret = (elem >> shift) & 1;                 // extract bit");
    Ins(&R, getbit.body, "}");
    Ins(&R, getbit.body, "return ret;");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Bitset_Sum1s() {
    algo_lib::Replscope &R       = amc::_db.genfield.R;
    amc::FFunc          &sumones = amc::CreateCurFunc();
    amc::FField         &field   = *amc::_db.genfield.p_field;

    Set(R, "$Fldtype", field.p_arg->cpp_type);
    Ins(&R, sumones.ret  , "u64",false);
    Ins(&R, sumones.proto, "$name_Sum1s($Parent)",false);
    Ins(&R, sumones.body, "u64 sum = 0;");
    Ins(&R, sumones.body, "u64 n = $name_N($pararg);");
    Ins(&R, sumones.body, "for (u64 i = 0; i < n; i++) {");
    Ins(&R, sumones.body, "    sum += algo::$Fldtype_Count1s($name_qFind($pararg, i));");
    Ins(&R, sumones.body, "}");
    Ins(&R, sumones.body, "return sum;");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Bitset_BitsEmptyQ() {
    algo_lib::Replscope &R       = amc::_db.genfield.R;
    amc::FFunc          &emptyq = amc::CreateCurFunc();
    amc::FField         &field   = *amc::_db.genfield.p_field;

    Set(R, "$Fldtype", field.p_arg->cpp_type);
    Ins(&R, emptyq.ret  , "bool",false);
    Ins(&R, emptyq.proto, "$name_BitsEmptyQ($Parent)",false);
    Ins(&R, emptyq.body, "bool retval = true;");
    Ins(&R, emptyq.body, "u64 n = $name_N($pararg);");
    Ins(&R, emptyq.body, "for (u64 i = 0; i < n; i++) {");
    Ins(&R, emptyq.body, "    if ($name_qFind($pararg,i) != 0) {");
    Ins(&R, emptyq.body, "        retval = false;");
    Ins(&R, emptyq.body, "        break;");
    Ins(&R, emptyq.body, "    }");
    Ins(&R, emptyq.body, "}");
    Ins(&R, emptyq.body, "return retval;");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Bitset_qClearBit() {
    algo_lib::Replscope &R        = amc::_db.genfield.R;
    amc::FFunc &qclearbit = amc::CreateCurFunc();
    Ins(&R, qclearbit.comment, "Clear bit # BIT_IDX in bit set. No bounds checking");
    Ins(&R, qclearbit.ret  , "void",false);
    Ins(&R, qclearbit.proto, "$name_qClearBit($Parent, u32 bit_idx)",false);
    Ins(&R, qclearbit.body, "u64 elem_idx = bit_idx >> $idxshift;");
    Ins(&R, qclearbit.body, "u64 shift = bit_idx & $shiftmask;");
    Ins(&R, qclearbit.body, "$Cpptype &elem = $name_qFind($pararg, elem_idx); // fetch");
    Ins(&R, qclearbit.body, "elem = elem & ~($Cpptype(1) << shift); // clear bit");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Bitset_ClearBit() {
    algo_lib::Replscope &R        = amc::_db.genfield.R;
    amc::FFunc &clearbit = amc::CreateCurFunc();
    Ins(&R, clearbit.comment, "Clear bit # BIT_IDX in bit set. If bit index is out of bounds, do nothing");
    Ins(&R, clearbit.ret  , "void",false);
    Ins(&R, clearbit.proto, "$name_ClearBit($Parent, u32 bit_idx)",false);
    Ins(&R, clearbit.body, "u64 elem_idx = bit_idx >> $idxshift;");
    Ins(&R, clearbit.body, "u64 shift = bit_idx & $shiftmask;");
    Ins(&R, clearbit.body, "u64 lim = $name_N($parname);");
    Ins(&R, clearbit.body, "if (elem_idx < lim) {");
    Ins(&R, clearbit.body, "    $Cpptype &elem = $name_qFind($pararg, elem_idx); // fetch");
    Ins(&R, clearbit.body, "    elem = elem & ~($Cpptype(1) << shift); // clear bit");
    Ins(&R, clearbit.body, "}");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Bitset_qSetBit() {
    algo_lib::Replscope &R        = amc::_db.genfield.R;
    amc::FFunc &qsetbit = amc::CreateCurFunc();
    Ins(&R, qsetbit.ret  , "void",false);
    Ins(&R, qsetbit.proto, "$name_qSetBit($Parent, u32 bit_idx)",false);
    Ins(&R, qsetbit.body, "u64 elem_idx = bit_idx >> $idxshift;");
    Ins(&R, qsetbit.body, "u64 shift = bit_idx & $shiftmask;");
    Ins(&R, qsetbit.body, "$Cpptype &elem = $name_qFind($pararg, elem_idx); // fetch");
    Ins(&R, qsetbit.body, "elem = elem | ($Cpptype(1) << shift); // set bit");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Bitset_SetBit() {
    algo_lib::Replscope &R        = amc::_db.genfield.R;
    amc::FFunc &setbit = amc::CreateCurFunc();
    Ins(&R, setbit.ret  , "void",false);
    Ins(&R, setbit.proto, "$name_SetBit($Parent, u32 bit_idx)",false);
    Ins(&R, setbit.body, "u64 elem_idx = bit_idx >> $idxshift;");
    Ins(&R, setbit.body, "u64 shift = bit_idx & $shiftmask;");
    Ins(&R, setbit.body, "u64 lim = $name_N($parname);");
    Ins(&R, setbit.body, "if (elem_idx < lim) {");
    Ins(&R, setbit.body, "    $Cpptype &elem = $name_qFind($pararg, elem_idx); // fetch");
    Ins(&R, setbit.body, "    elem = elem | ($Cpptype(1) << shift); // set bit");
    Ins(&R, setbit.body, "}");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Bitset_qSetBitVal() {
    algo_lib::Replscope &R        = amc::_db.genfield.R;
    amc::FFunc &setbitval = amc::CreateCurFunc();
    Ins(&R, setbitval.ret  , "void",false);
    Ins(&R, setbitval.proto, "$name_qSetBitVal($Parent, u32 bit_idx, bool val)",false);
    Ins(&R, setbitval.body, "u64 elem_idx = bit_idx >> $idxshift;");
    Ins(&R, setbitval.body, "u64 shift = bit_idx & $shiftmask;");
    Ins(&R, setbitval.body, "$Cpptype &elem = $name_qFind($pararg, elem_idx); // fetch");
    Ins(&R, setbitval.body, "elem = (elem & ~($Cpptype(1) << shift)) | ($Cpptype(val) << shift); // insert new value");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Bitset_qOrBitVal() {
    algo_lib::Replscope &R        = amc::_db.genfield.R;
    amc::FFunc &orbitval = amc::CreateCurFunc();
    Ins(&R, orbitval.ret  , "void",false);
    Ins(&R, orbitval.proto, "$name_qOrBitVal($Parent, u32 bit_idx, bool val)",false);
    Ins(&R, orbitval.body, "u64 elem_idx = bit_idx >> $idxshift;");
    Ins(&R, orbitval.body, "u64 shift = bit_idx & $shiftmask;");
    Ins(&R, orbitval.body, "$Cpptype &elem = $name_qFind($pararg, elem_idx); // fetch");
    Ins(&R, orbitval.body, "elem = elem | ($Cpptype(val) << shift); // Or in val into elem");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Bitset_ClearBitsAll() {
    algo_lib::Replscope &R        = amc::_db.genfield.R;
    amc::FFunc &clearall = amc::CreateCurFunc();
    Ins(&R, clearall.comment, "Set all bits of array to zero.");
    Ins(&R, clearall.comment, "Note: this does not change what NBits will return.");
    Ins(&R, clearall.ret  , "void",false);
    Ins(&R, clearall.proto, "$name_ClearBitsAll($Parent)",false);
    Ins(&R, clearall.body, "u64 n = $name_N($pararg);");
    Ins(&R, clearall.body, "for (u64 i = 0; i < n; i++) {");
    Ins(&R, clearall.body, "    $name_qFind($pararg, i) = 0;");
    Ins(&R, clearall.body, "}");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Bitset_ClearBits() {
    algo_lib::Replscope &R        = amc::_db.genfield.R;
    amc::FFunc &qclearbits = amc::CreateCurFunc();
    Ins(&R, qclearbits.ret  , "void",false);
    Ins(&R, qclearbits.proto, "$name_ClearBits($Parent, $Partype &rhs)",false);
    Ins(&R, qclearbits.body, "u64 n = u64_Min($name_N($pararg), $name_N(rhs));");
    Ins(&R, qclearbits.body, "for (u64 i = 0; i < n; i++) {");
    Ins(&R, qclearbits.body, "    $name_qFind($pararg, i) &= ~$name_qFind(rhs, i);");
    Ins(&R, qclearbits.body, "}");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Bitset_OrBits() {
    algo_lib::Replscope &R        = amc::_db.genfield.R;
    amc::FFunc &qsetbits = amc::CreateCurFunc();
    Ins(&R, qsetbits.comment, "Set PARENT to union of two bitsets.");
    Ins(&R, qsetbits.comment, "(This function is not named Set.. to avoid triple entendre).");
    Ins(&R, qsetbits.ret  , "void",false);
    Ins(&R, qsetbits.proto, "$name_OrBits($Parent, $Partype &rhs)",false);
    Ins(&R, qsetbits.body, "u64 n = u64_Min($name_N($pararg), $name_N(rhs));");
    Ins(&R, qsetbits.body, "for (u64 i = 0; i < n; i++) {");
    Ins(&R, qsetbits.body, "    $name_qFind($pararg, i) |= $name_qFind(rhs, i);");
    Ins(&R, qsetbits.body, "}");
}

// -----------------------------------------------------------------------------

void amc::tfunc_Bitset_ExpandBits() {
    algo_lib::Replscope &R        = amc::_db.genfield.R;
    amc::FField         &field    = *amc::_db.genfield.p_field;
    bool singleval = !field.c_inlary && !field.c_tary;
    bool expandable = (!field.c_inlary || !FixaryQ(field)) && !singleval;
    // todo: support varlen and other array types
    if (expandable) {
        amc::FFunc &expandbits = amc::CreateCurFunc();
        Ins(&R, expandbits.comment, "Make sure the array supports at least NBITS bits.");
        Ins(&R, expandbits.comment, "This function is the same as AllocBit(NBITS - 1) assuming NBITS > 0;");
        Ins(&R, expandbits.ret  , "void",false);
        Ins(&R, expandbits.proto, "$name_ExpandBits($Parent, u32 n_bits)",false);
        Ins(&R, expandbits.body, "u64 n_elems = (n_bits+$shiftmask) >> $idxshift;");
        Ins(&R, expandbits.body, "while (true) {");
        Ins(&R, expandbits.body, "    u64 n = $name_N($pararg);");
        Ins(&R, expandbits.body, "    if (n_elems <= n) break;");
        Ins(&R, expandbits.body, "    $name_Alloc($pararg) = 0; // keep stretching");
        Ins(&R, expandbits.body, "}");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Bitset_AllocBit() {
    algo_lib::Replscope &R        = amc::_db.genfield.R;
    amc::FField         &field    = *amc::_db.genfield.p_field;
    bool singleval = !field.c_inlary && !field.c_tary;
    bool expandable = (!field.c_inlary || !FixaryQ(field)) && !singleval;
    if (expandable) {
        amc::FFunc &allocbit = amc::CreateCurFunc();
        Ins(&R, allocbit.comment, "Make sure all bits up to and including #BIT_IDX exist");
        Ins(&R, allocbit.comment, "New values are initialized with zero.");
        Ins(&R, allocbit.ret  , "void",false);
        Ins(&R, allocbit.proto, "$name_AllocBit($Parent, u32 bit_idx)",false);
        Ins(&R, allocbit.body, "$name_ExpandBits($parname, bit_idx + 1);");
    }
}

// -----------------------------------------------------------------------------

void amc::tfunc_Bitset_Sup() {
    algo_lib::Replscope &R        = amc::_db.genfield.R;
    amc::FField         &field    = *amc::_db.genfield.p_field;
    amc::FCtype &elemtype = *field.p_arg;
    u32 elem_bits = elemtype.c_csize ? elemtype.c_csize->size*8 :0;
    // sup function
    if (elem_bits <= 64) {
        amc::FFunc &sup = amc::CreateCurFunc();
        Ins(&R, sup.ret  , "i32",false);
        Ins(&R, sup.proto, "$name_Sup($Parent)",false);
        Ins(&R, sup.body, "u64 lim = $name_N($pararg);");
        Ins(&R, sup.body, "i32 ret = 0;");
        Ins(&R, sup.body, "for (int i = lim-1; i >= 0; i--) {");
        Ins(&R, sup.body, "    $Cpptype &val = $name_qFind($pararg, i);");
        Ins(&R, sup.body, "    if (val) {");
        Ins(&R, sup.body, "        u32 bitidx = algo::u64_BitScanReverse(val) + 1;");
        Ins(&R, sup.body, "        ret = i * $elembits + bitidx;");
        Ins(&R, sup.body, "        break;");
        Ins(&R, sup.body, "    }");
        Ins(&R, sup.body, "}");
        Ins(&R, sup.body, "return ret;");
    }
}

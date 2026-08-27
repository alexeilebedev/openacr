// Copyright (C) 2023-2026 AlgoRND
// Copyright (C) 2020-2021 Astra
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
// Exceptions: yes
// Source: cpp/amc/bitset.cpp -- Bit sets
//

#include "include/amc.h"

// Check that the bitset element is an unsigned integer builtin of a width the
// accessors can index, and set up the substitution variables shared by the
// bitset accessors: element width, the index shift and mask splitting a bit
// index, and the bit-scan width.
void amc::tclass_Bitset() {
    algo_lib::Replscope &R        = amc::_db.genctx.R;
    amc::FField         &field    = *amc::_db.genctx.p_field;
    amc::FFbitset &fbitset = *field.c_fbitset;
    amc::FCtype &elemtype = *field.p_arg;

    i64 elem_bits = Ctype_Nbit(elemtype);
    // The generated accessors read bits back with plain shifts, which
    // sign-extend on a signed element (an i8 element compiles but Sup returns
    // wrong values), so the element type must be one of the unsigned integer
    // builtins. Each of those names a width -- and the width the accessors
    // actually index by is a second fact, taken from the element's
    // dmmeta.csize row, which a universe can state as anything. The two must
    // agree: a csize row wider than the name makes the accessors shift past
    // the end of the member the struct declares, and a narrower or unaligned
    // one makes the index split, which is exact only for a power-of-two
    // width, land on the wrong element. Comparing the stated width against
    // the name's own width settles both, and it also keeps the width inside
    // the reach of the bit-scan helpers, which stop at 128 bits.
    amc::BltinId bltin_id(amc_BltinIdEnum(0));
    value_SetStrptrMaybe(bltin_id, elemtype.ctype);
    i64 name_bits = 0;
    switch (bltin_id.value) {
    case amc_BltinId_u8:   name_bits = 8;   break;
    case amc_BltinId_u16:  name_bits = 16;  break;
    case amc_BltinId_u32:  name_bits = 32;  break;
    case amc_BltinId_u64:  name_bits = 64;  break;
    case amc_BltinId_u128: name_bits = 128; break;
    default:                                break;
    }
    if (name_bits == 0) {
        prerr("amc.bitset_elem"
              <<Keyval("fbitset",fbitset.field)
              <<Keyval("arg",field.arg)
              <<Keyval("comment","bitset element must be an unsigned builtin. Use u8,u16,u32,u64, or u128"));
        algo_lib::_db.exit_code++;
    } else if (elem_bits==0) {
        // the element is one of the unsigned builtins, so the only way its
        // width can be unknown is a universe lacking the type's csize row
        prerr("amc.bitset_elem_size"
              <<Keyval("fbitset",fbitset.field)
              <<Keyval("arg",field.arg)
              <<Keyval("comment","element size unknown; add a dmmeta.csize row for the element type"));
        algo_lib::_db.exit_code++;
    } else if (elem_bits != name_bits) {
        // the csize row disagrees with the width the element type's own name
        // states, and the accessors are generated from the csize row
        prerr("amc.bitset_elem_width"
              <<Keyval("fbitset",fbitset.field)
              <<Keyval("arg",field.arg)
              <<Keyval("elem_bits",elem_bits)
              <<Keyval("name_bits",name_bits)
              <<Keyval("comment","element width must be the width its type name states; check the dmmeta.csize row"));
        algo_lib::_db.exit_code++;
    }

    Set(R, "$idxshift"  , tempstr()<<algo::FloorLog2(u32(elem_bits)));
    Set(R, "$shiftmask" , tempstr()<<(1 << algo::FloorLog2(u32(elem_bits)))-1);
    Set(R, "$elembits"  , tempstr()<<elem_bits);
    // width of the bit-scan helper (BitScanForward/Reverse): elements up to
    // 64 bits promote to u64; a u128 element uses the u128 helper
    Set(R, "$scantype"  , elem_bits > 64 ? "u128" : "u64");
}

// -----------------------------------------------------------------------------

// Generate the element-count function for a single-element bitset (the
// constant 1); an array-backed bitset takes it from the array reftype.
void amc::tfunc_Bitset_N() {
    algo_lib::Replscope &R        = amc::_db.genctx.R;
    amc::FField         &field    = *amc::_db.genctx.p_field;
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

// Generate the element accessor for a single-element bitset: return the
// value itself, ignoring the index; arrays take it from the array reftype.
void amc::tfunc_Bitset_qFind() {
    algo_lib::Replscope &R        = amc::_db.genctx.R;
    amc::FField         &field    = *amc::_db.genctx.p_field;
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

// Generate the capacity function: number of bits the bitset holds
// (element count times element width).
void amc::tfunc_Bitset_NBits() {
    algo_lib::Replscope &R        = amc::_db.genctx.R;
    amc::FFunc &nbits = amc::CreateCurFunc();
    Ins(&R, nbits.comment, "Return max. number of bits supported by array");
    Ins(&R, nbits.ret  , "int",false);
    Ins(&R, nbits.proto, "$name_Nbits($Parent)",false);
    Ins(&R, nbits.body, "return $name_N($pararg) * $elembits;");
}

// -----------------------------------------------------------------------------

// Generate the unchecked bit read: fetch one bit without bounds checking.
void amc::tfunc_Bitset_qGetBit() {
    algo_lib::Replscope &R        = amc::_db.genctx.R;
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

// Generate the checked bit read: fetch one bit, returning false when the
// bit index is out of bounds.
void amc::tfunc_Bitset_GetBit() {
    algo_lib::Replscope &R        = amc::_db.genctx.R;
    amc::FFunc &getbit = amc::CreateCurFunc();
    Ins(&R, getbit.comment, "Retrieve value of bit #BIT_IDX in bit set. If bit index is out of bounds, return 0.");
    Ins(&R, getbit.ret  , "bool",false);
    Ins(&R, getbit.proto, "$name_GetBit($Parent, u32 bit_idx)",false);
    Ins(&R, getbit.body, "u64 elem_idx = bit_idx >> $idxshift;");
    Ins(&R, getbit.body, "u64 shift = bit_idx & $shiftmask;");
    Ins(&R, getbit.body, "bool ret = false;");
    Ins(&R, getbit.body, "u64 lim = $name_N($pararg);");
    Ins(&R, getbit.body, "if (elem_idx < lim) {");
    Ins(&R, getbit.body, "    $Cpptype &elem = $name_qFind($pararg, elem_idx); // fetch element");
    Ins(&R, getbit.body, "    ret = (elem >> shift) & 1;                 // extract bit");
    Ins(&R, getbit.body, "}");
    Ins(&R, getbit.body, "return ret;");
}

// -----------------------------------------------------------------------------

// Generate the population count: total number of set bits over all elements.
void amc::tfunc_Bitset_Sum1s() {
    algo_lib::Replscope &R       = amc::_db.genctx.R;
    amc::FFunc          &sumones = amc::CreateCurFunc();
    amc::FField         &field   = *amc::_db.genctx.p_field;

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

// Generate the emptiness predicate: true when every bit is zero.
void amc::tfunc_Bitset_BitsEmptyQ() {
    algo_lib::Replscope &R       = amc::_db.genctx.R;
    amc::FFunc          &emptyq = amc::CreateCurFunc();
    amc::FField         &field   = *amc::_db.genctx.p_field;

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

// Generate the unchecked bit clear: zero one bit without bounds checking.
void amc::tfunc_Bitset_qClearBit() {
    algo_lib::Replscope &R        = amc::_db.genctx.R;
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

// Generate the checked bit clear: zero one bit, a no-op when the bit
// index is out of bounds.
void amc::tfunc_Bitset_ClearBit() {
    algo_lib::Replscope &R        = amc::_db.genctx.R;
    amc::FFunc &clearbit = amc::CreateCurFunc();
    Ins(&R, clearbit.comment, "Clear bit # BIT_IDX in bit set. If bit index is out of bounds, do nothing");
    Ins(&R, clearbit.ret  , "void",false);
    Ins(&R, clearbit.proto, "$name_ClearBit($Parent, u32 bit_idx)",false);
    Ins(&R, clearbit.body, "u64 elem_idx = bit_idx >> $idxshift;");
    Ins(&R, clearbit.body, "u64 shift = bit_idx & $shiftmask;");
    Ins(&R, clearbit.body, "u64 lim = $name_N($pararg);");
    Ins(&R, clearbit.body, "if (elem_idx < lim) {");
    Ins(&R, clearbit.body, "    $Cpptype &elem = $name_qFind($pararg, elem_idx); // fetch");
    Ins(&R, clearbit.body, "    elem = elem & ~($Cpptype(1) << shift); // clear bit");
    Ins(&R, clearbit.body, "}");
}

// -----------------------------------------------------------------------------

// Generate the unchecked bit set: set one bit without bounds checking.
void amc::tfunc_Bitset_qSetBit() {
    algo_lib::Replscope &R        = amc::_db.genctx.R;
    amc::FFunc &qsetbit = amc::CreateCurFunc();
    Ins(&R, qsetbit.ret  , "void",false);
    Ins(&R, qsetbit.proto, "$name_qSetBit($Parent, u32 bit_idx)",false);
    Ins(&R, qsetbit.body, "u64 elem_idx = bit_idx >> $idxshift;");
    Ins(&R, qsetbit.body, "u64 shift = bit_idx & $shiftmask;");
    Ins(&R, qsetbit.body, "$Cpptype &elem = $name_qFind($pararg, elem_idx); // fetch");
    Ins(&R, qsetbit.body, "elem = elem | ($Cpptype(1) << shift); // set bit");
}

// -----------------------------------------------------------------------------

// Generate the checked bit set: set one bit, a no-op when the bit index
// is out of bounds.
void amc::tfunc_Bitset_SetBit() {
    algo_lib::Replscope &R        = amc::_db.genctx.R;
    amc::FFunc &setbit = amc::CreateCurFunc();
    Ins(&R, setbit.ret  , "void",false);
    Ins(&R, setbit.proto, "$name_SetBit($Parent, u32 bit_idx)",false);
    Ins(&R, setbit.body, "u64 elem_idx = bit_idx >> $idxshift;");
    Ins(&R, setbit.body, "u64 shift = bit_idx & $shiftmask;");
    Ins(&R, setbit.body, "u64 lim = $name_N($pararg);");
    Ins(&R, setbit.body, "if (elem_idx < lim) {");
    Ins(&R, setbit.body, "    $Cpptype &elem = $name_qFind($pararg, elem_idx); // fetch");
    Ins(&R, setbit.body, "    elem = elem | ($Cpptype(1) << shift); // set bit");
    Ins(&R, setbit.body, "}");
}

// -----------------------------------------------------------------------------

// Generate the unchecked bit write: overwrite one bit with VAL without
// bounds checking.
void amc::tfunc_Bitset_qSetBitVal() {
    algo_lib::Replscope &R        = amc::_db.genctx.R;
    amc::FFunc &setbitval = amc::CreateCurFunc();
    Ins(&R, setbitval.ret  , "void",false);
    Ins(&R, setbitval.proto, "$name_qSetBitVal($Parent, u32 bit_idx, bool val)",false);
    Ins(&R, setbitval.body, "u64 elem_idx = bit_idx >> $idxshift;");
    Ins(&R, setbitval.body, "u64 shift = bit_idx & $shiftmask;");
    Ins(&R, setbitval.body, "$Cpptype &elem = $name_qFind($pararg, elem_idx); // fetch");
    Ins(&R, setbitval.body, "elem = (elem & ~($Cpptype(1) << shift)) | ($Cpptype(val) << shift); // insert new value");
}

// -----------------------------------------------------------------------------

// Generate the unchecked bit or: or VAL into one bit without bounds checking.
void amc::tfunc_Bitset_qOrBitVal() {
    algo_lib::Replscope &R        = amc::_db.genctx.R;
    amc::FFunc &orbitval = amc::CreateCurFunc();
    Ins(&R, orbitval.ret  , "void",false);
    Ins(&R, orbitval.proto, "$name_qOrBitVal($Parent, u32 bit_idx, bool val)",false);
    Ins(&R, orbitval.body, "u64 elem_idx = bit_idx >> $idxshift;");
    Ins(&R, orbitval.body, "u64 shift = bit_idx & $shiftmask;");
    Ins(&R, orbitval.body, "$Cpptype &elem = $name_qFind($pararg, elem_idx); // fetch");
    Ins(&R, orbitval.body, "elem = elem | ($Cpptype(val) << shift); // Or in val into elem");
}

// -----------------------------------------------------------------------------

// Generate the whole-set clear: zero every element; the capacity is
// unchanged.
void amc::tfunc_Bitset_ClearBitsAll() {
    algo_lib::Replscope &R        = amc::_db.genctx.R;
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

// Generate the set difference: clear the bits of PARENT that are set in
// RHS. Skipped for a global ctype, which has no second instance.
void amc::tfunc_Bitset_ClearBits() {
    algo_lib::Replscope &R        = amc::_db.genctx.R;
    amc::FField         &field    = *amc::_db.genctx.p_field;
    if (GlobalQ(*field.p_ctype)) {
        // skip for global - no second instance to compare against
    } else {
        amc::FFunc &qclearbits = amc::CreateCurFunc();
        Ins(&R, qclearbits.ret  , "void",false);
        Ins(&R, qclearbits.proto, "$name_ClearBits($Parent, $Partype &rhs)",false);
        Ins(&R, qclearbits.body, "u64 n = u64_Min($name_N($pararg), $name_N(rhs));");
        Ins(&R, qclearbits.body, "for (u64 i = 0; i < n; i++) {");
        Ins(&R, qclearbits.body, "    $name_qFind($pararg, i) &= ~$name_qFind(rhs, i);");
        Ins(&R, qclearbits.body, "}");
    }
}

// -----------------------------------------------------------------------------

// Generate the set union: or RHS's bits into PARENT. Skipped for a global
// ctype, which has no second instance.
void amc::tfunc_Bitset_OrBits() {
    algo_lib::Replscope &R        = amc::_db.genctx.R;
    amc::FField         &field    = *amc::_db.genctx.p_field;
    if (GlobalQ(*field.p_ctype)) {
        // skip for global - no second instance to compare against
    } else {
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
}

// -----------------------------------------------------------------------------

// Generate the capacity grower for an expandable array: allocate zeroed
// elements until at least N_BITS bits exist.
void amc::tfunc_Bitset_ExpandBits() {
    algo_lib::Replscope &R        = amc::_db.genctx.R;
    amc::FField         &field    = *amc::_db.genctx.p_field;
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

// Generate the single-bit grower for an expandable array: ensure bits up
// to and including BIT_IDX exist, initialized to zero.
void amc::tfunc_Bitset_AllocBit() {
    algo_lib::Replscope &R        = amc::_db.genctx.R;
    amc::FField         &field    = *amc::_db.genctx.p_field;
    bool singleval = !field.c_inlary && !field.c_tary;
    bool expandable = (!field.c_inlary || !FixaryQ(field)) && !singleval;
    if (expandable) {
        amc::FFunc &allocbit = amc::CreateCurFunc();
        Ins(&R, allocbit.comment, "Make sure all bits up to and including #BIT_IDX exist");
        Ins(&R, allocbit.comment, "New values are initialized with zero.");
        Ins(&R, allocbit.ret  , "void",false);
        Ins(&R, allocbit.proto, "$name_AllocBit($Parent, u32 bit_idx)",false);
        Ins(&R, allocbit.body, "$name_ExpandBits($pararg, bit_idx + 1);");
    }
}

// -----------------------------------------------------------------------------

// Generate the supremum function: 1 plus the index of the highest set
// bit, 0 when no bit is set.
void amc::tfunc_Bitset_Sup() {
    algo_lib::Replscope &R        = amc::_db.genctx.R;
    // sup function
    amc::FFunc &sup = amc::CreateCurFunc();
    Ins(&R, sup.ret  , "i32",false);
    Ins(&R, sup.proto, "$name_Sup($Parent)",false);
    Ins(&R, sup.body, "u64 lim = $name_N($pararg);");
    Ins(&R, sup.body, "i32 ret = 0;");
    Ins(&R, sup.body, "for (int i = lim-1; i >= 0; i--) {");
    Ins(&R, sup.body, "    $Cpptype &val = $name_qFind($pararg, i);");
    Ins(&R, sup.body, "    if (val) {");
    Ins(&R, sup.body, "        u32 bitidx = algo::$scantype_BitScanReverse(val) + 1;");
    Ins(&R, sup.body, "        ret = i * $elembits + bitidx;");
    Ins(&R, sup.body, "        break;");
    Ins(&R, sup.body, "    }");
    Ins(&R, sup.body, "}");
    Ins(&R, sup.body, "return ret;");
}

// -----------------------------------------------------------------------------

// Generate the bitcurs cursor (struct plus Reset/ValidQ/Access/Next):
// iterate the indexes of the set bits in ascending order, skipping zero
// elements with a bit scan.
void amc::tfunc_Bitset_bitcurs() {
    algo_lib::Replscope &R = amc::_db.genctx.R;
    amc::FNs &ns = *amc::_db.genctx.p_field->p_ctype->p_ns;
    amc::FFunc& curs_next = amc::ind_func_GetOrCreate(Subst(R,"$field_bitcurs.Next"));
    Ins(&R, curs_next.comment, "proceed to next item");
    Ins(&R, curs_next.ret  , "void", false);
    Ins(&R, curs_next.proto, "$Parname_$name_bitcurs_Next($Parname_$name_bitcurs &curs)", false);
    Ins(&R, curs_next.body, "++curs.bit;");
    Ins(&R, curs_next.body, "int index = curs.bit / $elembits;");
    Ins(&R, curs_next.body, "int offset = curs.bit % $elembits;");
    Ins(&R, curs_next.body, "for (; index < curs.n_elems; ++index, offset = 0) {");
    Ins(&R, curs_next.body, "    $scantype rest = curs.elems[index] >> offset;");
    Ins(&R, curs_next.body, "    if (rest) {");
    Ins(&R, curs_next.body, "         offset += algo::$scantype_BitScanForward(rest);");
    Ins(&R, curs_next.body, "         break;");
    Ins(&R, curs_next.body, "    }");
    Ins(&R, curs_next.body, "}");
    Ins(&R, curs_next.body, "curs.bit = index * $elembits + offset;");
    Ins(&R, ns.curstext, "");
    Ins(&R, ns.curstext, "struct $Parname_$name_bitcurs {// cursor");
    Ins(&R, ns.curstext, "    typedef int& ChildType;");
    Ins(&R, ns.curstext, "    $Cpptype* elems;");
    Ins(&R, ns.curstext, "    int n_elems;");
    Ins(&R, ns.curstext, "    int bit;");
    Ins(&R, ns.curstext, "    $Parname_$name_bitcurs() : elems(0), n_elems(0), bit(0) {}");
    Ins(&R, ns.curstext, "};");
    Ins(&R, ns.curstext, "");

    {
        amc::FFunc& curs_reset = amc::CreateInlineFunc(Subst(R,"$field_bitcurs.Reset"));
        Ins(&R, curs_reset.ret  , "void", false);
        Ins(&R, curs_reset.proto, "$Parname_$name_bitcurs_Reset($Parname_$name_bitcurs &curs, $Partype &parent)", false);
        Ins(&R, curs_reset.body, "curs.elems = &$name_qFind(parent,0);");
        Ins(&R, curs_reset.body, "curs.n_elems = $name_N(parent);");
        Ins(&R, curs_reset.body, "curs.bit = -1;");
        Ins(&R, curs_reset.body, "$Parname_$name_bitcurs_Next(curs);");
    }

    {
        amc::FFunc& curs_validq = amc::CreateInlineFunc(Subst(R,"$field_bitcurs.ValidQ"));
        Ins(&R, curs_validq.comment, "cursor points to valid item");
        Ins(&R, curs_validq.ret  , "bool", false);
        Ins(&R, curs_validq.proto, "$Parname_$name_bitcurs_ValidQ($Parname_$name_bitcurs &curs)", false);
        Ins(&R, curs_validq.body, "return curs.bit < curs.n_elems*$elembits;");
    }

    {
        amc::FFunc& curs_access = amc::CreateInlineFunc(Subst(R,"$field_bitcurs.Access"));
        Ins(&R, curs_access.comment, "item access");
        Ins(&R, curs_access.ret  , "int&", false);
        Ins(&R, curs_access.proto, "$Parname_$name_bitcurs_Access($Parname_$name_bitcurs &curs)", false);
        Ins(&R, curs_access.body, "return curs.bit;");
    }
}

// Copyright (C) 2023-2024,2026 AlgoRND
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
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
// Target: atf_amc (exe) -- Unit tests for amc (see amctest table)
// Exceptions: yes
// Source: cpp/atf_amc/varlen.cpp
//

#include "include/atf_amc.h"

void atf_amc::amctest_VarlenExternLength() {
    // Allocate
    u32 ary[]={1,2,3,4,5,6,7,8,9,10};
    algo::aryptr<u32> ary1=algo::aryptr<u32>(ary,10);
    atf_amc::VarlenExtern &vl = atf_amc::varlen_extern_AllocVarlen(ary1);
    algo::aryptr<u32> ary2=varlen_Getary(vl);
    vrfyeq_(elems_N(ary1),elems_N(ary2)); // test code goes here
    for (int i=0; i<ary1.n_elems; i++) {
        vrfyeq_(ary1[i], ary2[i]);
    }
    varlen_extern_Delete(vl);// get rid of it
}

void atf_amc::length_Set(atf_amc::VarlenExtern &vl, u32 n) {
    vl.n_elems = (n-sizeof(atf_amc::VarlenExtern &))/sizeof(u32);
}

u32 atf_amc::length_Get(atf_amc::VarlenExtern &vl) {
    return sizeof(atf_amc::VarlenExtern)+sizeof(u32)*vl.n_elems;
}

// -----------------------------------------------------------------------------

static void CheckVarlen(atf_amc::VarlenAlloc *rec, int n) {
    vrfy_(rec && rec->length == sizeof(atf_amc::VarlenAlloc) + n*sizeof(int));
    for (int i=0; i<n; i++) {
        vrfy_(elem_Getary(*rec)[i] == i+1);
    }
}

void atf_amc::amctest_VarlenAlloc() {
    int temp[256];
    for (int i=0; i<256; i++) {
        temp[i]=i+1;
    }
    // alloc using Alloc
    {
        atf_amc::VarlenAlloc *rec = atf_amc::varlenalloc_AllocMaybe(256);
        memcpy(elem_Getary(*rec).elems,temp,sizeof(temp));
        CheckVarlen(rec,256);
        varlenalloc_Delete(*rec);
    }
    // alloc using Extra
    {
        atf_amc::VarlenAlloc *rec = &atf_amc::varlenalloc_AllocExtra(temp,sizeof(temp));
        CheckVarlen(rec,256);
        varlenalloc_Delete(*rec);
    }
    // alloc using Varlen
    {
        atf_amc::VarlenAlloc *rec = &atf_amc::varlenalloc_AllocVarlen(algo::aryptr<int>(temp,256));
        CheckVarlen(rec,256);
        varlenalloc_Delete(*rec);
    }
    // alloc using Extra
    {
        atf_amc::VarlenAlloc *rec = atf_amc::varlenalloc_AllocExtraMaybe(temp,sizeof(temp));
        vrfy_(rec && rec->length == sizeof(atf_amc::VarlenAlloc) + sizeof(temp));
        CheckVarlen(rec,256);
        varlenalloc_Delete(*rec);
    }
}

// Pool alloc must store the exact inverse of the lenfld read formula.
// VarlenAllocScale's lenfld has scale:4 extra:-4, so the reader computes
// total = length*4 + 4, and the alloc must store length = (total - 4)/4,
// i.e. the number of 4-byte varlen words.
void atf_amc::amctest_PoolLenfldScale() {
    // AllocExtra path: total = sizeof(header) + extra bytes
    {
        i32 temp[8];
        for (int i=0; i<8; i++) {
            temp[i]=i+1;
        }
        atf_amc::VarlenAllocScale *rec = atf_amc::varlenallocscale_AllocExtraMaybe(temp,sizeof(temp));
        vrfy_(rec != NULL);
        vrfyeq_(i32(rec->length)*4 + 4, i32(sizeof(atf_amc::VarlenAllocScale) + sizeof(temp)));
        vrfyeq_(elem_Getary(*rec).n_elems, 8);
        for (int i=0; i<8; i++) {
            vrfyeq_(elem_Getary(*rec)[i], i+1);// the payload bytes round-trip
        }
        varlenallocscale_Delete(*rec);
    }
    // AllocExtraMaybe with a byte count that is not a multiple of scale:
    // no length word can represent the total, so the alloc fails (NULL)
    // like any other alloc failure, instead of storing a truncated length
    // the reader would reconstruct short
    {
        i32 temp[2];
        temp[0]=1;
        temp[1]=2;
        atf_amc::VarlenAllocScale *rec = atf_amc::varlenallocscale_AllocExtraMaybe(temp,6);
        vrfy_(rec == NULL);
    }
    // plain Alloc path: MsgHdrLTScale's lenfld has scale:4 extra:-2,
    // so the reader computes total = len*4 + 2, which must equal sizeof (2)
    {
        atf_amc::MsgHdrLTScale &hdr = atf_amc::msghdrltscale_Alloc();
        vrfyeq_(GetMsgLength(hdr), i32(sizeof(atf_amc::MsgHdrLTScale)));
        msghdrltscale_Delete(hdr);
    }
}

// A negative Opt byte count -- passed by the caller, or read off a corrupt
// payload's own length word -- underallocates the fixed portion (the header
// stores already overflow the buffer), and the Opt memcpy converts the count
// to a huge size_t. Construction must refuse the count (NULL) before any
// buffer space is taken: the caller-passed arm (OptG) and the payload-derived
// arm (OptOptG, whose length word 0xFFFFFFFC reads as -4).
void atf_amc::amctest_PnewOptNegative() {
    {
        u8 store[64];
        memset(store,0,sizeof(store));
        algo::memptr buf(store, 64);
        atf_amc::TypeG typeg;
        typeg.typeg = 1;
        atf_amc::OptG *msg = atf_amc::OptG_FmtMemptr(buf, &typeg, -2);
        vrfy_(msg == NULL);
    }
    {
        u8 store[64];
        memset(store,0,sizeof(store));
        algo::memptr buf(store, 64);
        atf_amc::OptG optg;
        optg.length = 0xFFFFFFFC;
        atf_amc::OptOptG *msg = atf_amc::OptOptG_FmtMemptr(buf, &optg);
        vrfy_(msg == NULL);
    }
}

// A message constructor over a scaled lenfld: VarlenB counts 4-byte words
// past the first (scale:4 extra:-4) over a byte-granular payload, so only
// a total landing on a scale multiple has a representable length word.
// FmtByteAry with a 4-byte payload round-trips; a 3-byte payload must fail
// (NULL) before allocation, rather than store a truncated length word the
// reader would reconstruct short.
void atf_amc::amctest_PnewScaleGuard() {
    algo::ByteAry buf;
    atf_amc::VarlenB *msg = atf_amc::VarlenB_FmtByteAry(buf, "abcd");
    vrfy_(msg != NULL);
    vrfyeq_(c_Getary(*msg), "abcd");
    vrfyeq_(msg->length, u32(1));
    atf_amc::VarlenB *msg2 = atf_amc::VarlenB_FmtByteAry(buf, "abc");
    vrfy_(msg2 == NULL);
}

// A message constructor whose lenfld extra exceeds the ctype's fixed size:
// VarlenLow's u8 length word carries the total minus 8 (extra:-8) over a
// 1-byte fixed part, so a total below 8 has no representable length word --
// the store would go negative and wrap through the unsigned word, and the
// reader would reconstruct a frame 256 bytes longer than was written. An
// 8-byte total (7 payload bytes) round-trips; a 1-byte total (empty payload)
// must fail (NULL) before allocation.
void atf_amc::amctest_PnewLowGuard() {
    algo::ByteAry buf;
    atf_amc::VarlenLow *msg = atf_amc::VarlenLow_FmtByteAry(buf, "abcdefg");
    vrfy_(msg != NULL);
    vrfyeq_(c_Getary(*msg), "abcdefg");
    vrfyeq_(msg->length, u8(0));
    vrfyeq_(c_N(*msg), u32(7));
    atf_amc::VarlenLow *msg2 = atf_amc::VarlenLow_FmtByteAry(buf, "");
    vrfy_(msg2 == NULL);
}

// Pnew length arithmetic carries the buffer capacity and the varlen byte
// count in u64, so neither wraps at 4GiB: a capacity beyond 4GiB is compared
// at its true value rather than mod 2^32, and a varlen portion beyond 4GiB
// is compared at its true value rather than framing a truncated message.
// A total the frame length cannot express is refused outright: every buffer
// takes its size as an i32 (algo::Alloc and lib_ams::BeginWrite narrow len
// to int, and the reader reconstructs the total as an i32), so a total above
// i32 max would allocate the low bits while the payload copy moves the whole
// count. The first case is the accepted control; the accepted edge itself, a
// total of exactly i32 max, is pinned in the emitted text (comptest
// amc.LenfldNarrow) because accepting it would move 2GiB of payload.
// None of these cases needs real gigabytes: capacity and element count only
// feed comparisons, and bytes move only after the checks pass.
void atf_amc::amctest_PnewWideLen() {
    // capacity beyond 4GiB whose low 32 bits are small, small message:
    // must construct, not refuse
    {
        u8 store[64];
        memset(store,0,sizeof(store));
        u32 elem[2] = {1,2};
        algo::memptr buf(store, i64(1ULL<<32) + 8);
        atf_amc::VarlenK *msg = atf_amc::VarlenK_FmtMemptr(buf, algo::aryptr<u32>(elem,2));
        vrfy_(msg != NULL);
        vrfyeq_(i_N(*msg), u32(2));
    }
    // varlen portion beyond 4GiB whose byte count wraps to 4, small buffer:
    // must refuse (NULL), not construct a 4-byte portion
    {
        u8 store[64];
        memset(store,0,sizeof(store));
        u32 elem[2] = {1,2};
        algo::memptr buf(store, 64);
        atf_amc::VarlenK *msg = atf_amc::VarlenK_FmtMemptr(buf, algo::aryptr<u32>(elem, i64(1ULL<<30) + 1));
        vrfy_(msg == NULL);
    }
    // total one byte past i32 max, buffer roomy enough to admit it:
    // must refuse (NULL) before the payload copy, because the buffer size
    // argument would carry only the low 32 bits of the total
    {
        u8 store[64];
        memset(store,0,sizeof(store));
        u32 elem[2] = {1,2};
        algo::memptr buf(store, i64(1ULL<<33));
        atf_amc::VarlenK *msg = atf_amc::VarlenK_FmtMemptr(buf, algo::aryptr<u32>(elem, i64(1) << 29));
        vrfy_(msg == NULL);
    }
}

// An Opt element with a scaled lenfld: OptBMsg's optional trailing element
// is a VarlenB, and reading the message from a string stores the element's
// length through the element's own lenfld formula. A 4-byte payload
// round-trips; a 3-byte payload has no representable length word -- the
// read must fail rather than store a truncated word that makes b_Get
// reconstruct a short element.
void atf_amc::amctest_OptScaleGuard() {
    algo::ByteAry buf;
    bool ok = atf_amc::MsgHeaderMsgs_ReadStrptrMaybe("atf_amc.OptBMsg  b:'atf_amc.VarlenB  c:abcd'", buf);
    vrfy_(ok);
    atf_amc::OptBMsg *msg = (atf_amc::OptBMsg*)buf.ary_elems;
    atf_amc::VarlenB *b = b_Get(*msg);
    vrfy_(b != NULL);
    vrfyeq_(c_Getary(*b), "abcd");
    algo::ByteAry buf2;
    bool ok2 = atf_amc::MsgHeaderMsgs_ReadStrptrMaybe("atf_amc.OptBMsg  b:'atf_amc.VarlenB  c:abc'", buf2);
    vrfy_(!ok2);
}

// Ascii form of a message MSGTYPE whose varlen field FIELD carries NCHAR
// payload bytes.
static tempstr LongTuple(algo::strptr msgtype, algo::strptr field, int nchar) {
    tempstr ret;
    ret << msgtype << "  " << field << ":";
    for (int i = 0; i < nchar; i++) {
        ch_Alloc(ret) = 'a';
    }
    return ret;
}

// Reading a message from its ascii form appends the varlen tail to the buffer
// and stores the resulting total through the message's length field, and that
// total is a runtime one: nothing about the tuple bounds it to what the length
// word represents. Text's word is a u16 counting the total, so a total of
// 65535 is the largest that round-trips and 65536 stores 0 -- a length word
// that frames the payload as the next message header. MsgLTScaleV's word is a
// u8 counting (total - 2) / 4, which adds two more ways to miss: a total that
// is not 2 above a multiple of 4 truncates in the division, and a total past
// 1022 exceeds the word. Each accepted edge is read back through the length
// field to confirm the stored word reconstructs the total; each rejected one
// must fail the read rather than store a word the reader misinterprets.
// (A total below the word's low end is the one shape this path cannot reach:
// no in-tree dispatch-read header subtracts more than its own fixed size,
// and the low-end term comes from the same expression amctest PnewLowGuard
// pins.)
void atf_amc::amctest_DispReadLenfldGuard() {
    // u16 word counting the total: the largest total it stores, and one past
    {
        algo::ByteAry buf;
        tempstr str = LongTuple("atf_amc.Text", "text", 65531);
        vrfy_(atf_amc::MsgHeaderMsgs_ReadStrptrMaybe(str, buf));
        atf_amc::Text *msg = (atf_amc::Text*)buf.ary_elems;
        vrfyeq_(i32(msg->length), 65535);
        vrfyeq_(text_N(*msg), u32(65531));
    }
    {
        algo::ByteAry buf;
        tempstr str = LongTuple("atf_amc.Text", "text", 65532);
        vrfy_(!atf_amc::MsgHeaderMsgs_ReadStrptrMaybe(str, buf));
    }
    // u8 word counting (total-2)/4: a total on the scale, and one off it
    {
        algo::ByteAry buf;
        vrfy_(atf_amc::MsgHdrLTScaleMsgs_ReadStrptrMaybe("atf_amc.MsgLTScaleV  v:abcd", buf));
        atf_amc::MsgLTScaleV *msg = (atf_amc::MsgLTScaleV*)buf.ary_elems;
        vrfyeq_(u32(msg->len), u32(1));
        vrfyeq_(v_N(*msg), u32(4));
    }
    {
        algo::ByteAry buf;
        vrfy_(!atf_amc::MsgHdrLTScaleMsgs_ReadStrptrMaybe("atf_amc.MsgLTScaleV  v:abc", buf));
    }
    // same word: the largest total it stores, and the next total on the scale
    {
        algo::ByteAry buf;
        tempstr str = LongTuple("atf_amc.MsgLTScaleV", "v", 1020);
        vrfy_(atf_amc::MsgHdrLTScaleMsgs_ReadStrptrMaybe(str, buf));
        atf_amc::MsgLTScaleV *msg = (atf_amc::MsgLTScaleV*)buf.ary_elems;
        vrfyeq_(u32(msg->len), u32(255));
        vrfyeq_(v_N(*msg), u32(1020));
    }
    {
        algo::ByteAry buf;
        tempstr str = LongTuple("atf_amc.MsgLTScaleV", "v", 1024);
        vrfy_(!atf_amc::MsgHdrLTScaleMsgs_ReadStrptrMaybe(str, buf));
    }
}

// AllocExtraMaybe takes the varlen byte count as a signed i32, and a caller
// can arrive at a negative one: InsertMaybe computes the count as the total
// length minus the fixed size, so a corrupt length word smaller than the
// fixed size goes negative. An unguarded negative count underallocates the
// fixed portion, and the extra-bytes memcpy converts it to a huge size_t.
// The alloc must refuse the count (NULL) like any other alloc failure --
// on the unscaled path (VarlenAlloc) and on the scaled path
// (VarlenAllocScale, where -8 passes the multiple-of-scale test).
void atf_amc::amctest_PoolAllocExtraNegative() {
    {
        atf_amc::VarlenAlloc *rec = atf_amc::varlenalloc_AllocExtraMaybe(NULL, -4);
        vrfy_(rec == NULL);
    }
    {
        atf_amc::VarlenAllocScale *rec = atf_amc::varlenallocscale_AllocExtraMaybe(NULL, -8);
        vrfy_(rec == NULL);
    }
}

// InsertMaybe computes the varlen byte count as the inserted value's length
// word minus the fixed size, and hands that count to the allocator as an
// i32. A word smaller than the fixed size (a zeroed struct, a corrupt wire
// message) makes the count negative, and one past 2^31-1 makes it exceed
// what the allocator's argument holds; both fail as NULL per the function's
// contract, rather than dying inside the die-on-fail AllocExtra with a
// diagnostic blaming memory for an input error.  Out-of-memory keeps dying.
// A word equal to the fixed size is a record with no trailing element and
// is accepted, as is any word between that and 2^31-1.
//
// The length word is a u32, so the accept/reject table runs: 0 and every
// word below the fixed size reject, the fixed size itself and larger words
// accept, and words from 0x80000000 up reject. The accepted upper edge --
// the word that makes the count exactly 2^31-1 minus the fixed size -- is
// not exercised here, because reaching it means asking the allocator for
// two gigabytes.
void atf_amc::amctest_PoolInsertMaybeBound() {
    atf_amc::OptG value;
    // below the fixed size: a zeroed word, and the word one short of it
    value.length = 0;
    vrfy_(atf_amc::optg_InsertMaybe(value) == NULL);
    value.length = u32(sizeof(atf_amc::OptG)) - 1;
    vrfy_(atf_amc::optg_InsertMaybe(value) == NULL);
    // exactly the fixed size: no trailing element, and the row is stored
    value.length = u32(sizeof(atf_amc::OptG));
    atf_amc::FOptG *row = atf_amc::optg_InsertMaybe(value);
    vrfy_(row != NULL);
    vrfyeq_(row->length, u32(sizeof(atf_amc::FOptG)));
    atf_amc::optg_Delete(*row);
    // past what the allocator's i32 byte count holds
    value.length = 0x80000000;
    vrfy_(atf_amc::optg_InsertMaybe(value) == NULL);
    value.length = 0xffffffff;
    vrfy_(atf_amc::optg_InsertMaybe(value) == NULL);
}

// A length word whose range runs past u32 cannot be expanded to a byte
// total first: the scaled multiply, and at scale 1 the extra adjustment
// after the u64->i64 wrap, would overflow inside the very expression meant
// to keep the corrupt word visible. InsertMaybe therefore bounds the raw
// word by a generation-time constant -- the largest value whose expanded
// total still fits the i32 frame-length domain -- and refuses anything
// above it. OptWide is the unsigned scaled case: a u64 word, scale 2 and
// extra -8 put that constant at 1073741819. OptSigned is the signed case,
// where the bound is what rejects a negative stored length, through the
// u64 conversion in the emitted test; the addon-count test below it never
// sees the value. Each ctype's smallest frame -- the word whose total is
// exactly the fixed size -- is the control that still inserts.
void atf_amc::amctest_PoolInsertMaybeWideWord() {
    {
        atf_amc::OptWide value;
        value.length = 0;// smallest frame: total is exactly the fixed size
        atf_amc::FOptWide *row = atf_amc::optwide_InsertMaybe(value);
        vrfy_(row != NULL);
        atf_amc::optwide_Delete(*row);
        value.length = 1073741820;// one past the bound
        vrfy_(atf_amc::optwide_InsertMaybe(value) == NULL);
        value.length = 0x8000000000000000;// the scaled multiply would overflow
        vrfy_(atf_amc::optwide_InsertMaybe(value) == NULL);
    }
    {
        atf_amc::OptSigned value;
        value.length = ssizeof(atf_amc::OptSigned);// smallest frame
        atf_amc::FOptSigned *row = atf_amc::optsigned_InsertMaybe(value);
        vrfy_(row != NULL);
        atf_amc::optsigned_Delete(*row);
        value.length = 2147483648;// one past the bound
        vrfy_(atf_amc::optsigned_InsertMaybe(value) == NULL);
        value.length = -1;// negative: rejected by the u64 conversion
        vrfy_(atf_amc::optsigned_InsertMaybe(value) == NULL);
        value.length = -0x7fffffffffffffff - 1;// the fixed-size subtraction would underflow
        vrfy_(atf_amc::optsigned_InsertMaybe(value) == NULL);
    }
}

// template is needed for string literal in order
// to get correct length for NUL char inside
template <typename T> static void Check(T &bin_literal, strptr str) {
    strptr bin(bin_literal, sizeof bin_literal-1);
    // print bin and compare with str
    {
        auto &hdr = *(atf_amc::MsgHdrLT*)bin.elems;
        vrfyeq(hdr.len,ch_N(bin)-2,bin);
        tempstr out;
        atf_amc::MsgHdrLTMsgs_Print(out,hdr,sizeof bin);
        verblog(out);
        vrfyeq_(out,str);
    }

    // read str and compare with bin
    {
        algo::ByteAry buf;
        vrfy_(atf_amc::MsgHdrLTMsgs_ReadStrptrMaybe(str,buf));
        algo::memptr out(ary_Getary(buf));
        verblog(ToStrPtr(out));
        vrfyeq_(ToStrPtr(out),bin);
    }
}

void atf_amc::amctest_VarlenMsgs() {
    Check("\x04" "A" "1234"      ,"atf_amc.MsgLTA  a:1234");
    Check("\x06" "B" "123456"    ,"atf_amc.MsgLTB  b:123456");
    Check("\x00" "O"             ,"atf_amc.MsgLTO");
    Check("\x00" "V"             ,"atf_amc.MsgLTV");
    Check("\x06" "O"
          "\x04" "A" "1234"      ,"atf_amc.MsgLTO  o:\"atf_amc.MsgLTA  a:1234\"");
    Check("\x08" "O"
          "\x06" "B" "123456"    ,"atf_amc.MsgLTO  o:\"atf_amc.MsgLTB  b:123456\"");
    Check("\x08" "O"
          "\x06" "O"
          "\x04" "A" "1234"      ,"atf_amc.MsgLTO  o:'atf_amc.MsgLTO  o:\"atf_amc.MsgLTA  a:1234\"'");
    Check("\x0a" "O"
          "\x08" "O"
          "\x06" "B" "123456"    ,"atf_amc.MsgLTO  o:'atf_amc.MsgLTO  o:\"atf_amc.MsgLTB  b:123456\"'");
    Check("\x06" "V"
          "\x04" "A" "1234"      ,"atf_amc.MsgLTV  v.0:\"atf_amc.MsgLTA  a:1234\"");
    Check("\x08" "V"
          "\x06" "B" "123456"    ,"atf_amc.MsgLTV  v.0:\"atf_amc.MsgLTB  b:123456\"");
    Check("\x08" "V"
          "\x06" "O"
          "\x04" "A" "1234"      ,"atf_amc.MsgLTV  v.0:'atf_amc.MsgLTO  o:\"atf_amc.MsgLTA  a:1234\"'");
    Check("\x0a" "V"
          "\x08" "O"
          "\x06" "B" "123456"    ,"atf_amc.MsgLTV  v.0:'atf_amc.MsgLTO  o:\"atf_amc.MsgLTB  b:123456\"'");
    Check("\x08" "V"
          "\x06" "V"
          "\x04" "A" "1234"      ,"atf_amc.MsgLTV  v.0:'atf_amc.MsgLTV  v.0:\"atf_amc.MsgLTA  a:1234\"'");
    Check("\x0a" "V"
          "\x08" "V"
          "\x06" "B" "123456"    ,"atf_amc.MsgLTV  v.0:'atf_amc.MsgLTV  v.0:\"atf_amc.MsgLTB  b:123456\"'");
    Check("\x10" "V"
          "\x00" "O"
          "\x04" "A" "1234"
          "\x06" "B" "123456"    ,"atf_amc.MsgLTV  v.0:atf_amc.MsgLTO  v.1:\"atf_amc.MsgLTA  a:1234\"  v.2:\"atf_amc.MsgLTB  b:123456\"");
    Check("\x10" "V"
          "\x06" "O"
          "\x04" "A" "1234"
          "\x06" "B" "123456"    ,"atf_amc.MsgLTV  v.0:'atf_amc.MsgLTO  o:\"atf_amc.MsgLTA  a:1234\"'  v.1:\"atf_amc.MsgLTB  b:123456\"");
}

template<class T> inline algo::memptr ToBytes(const T &t) {
    return algo::memptr(reinterpret_cast<u8*>(const_cast<T*>(&t)),sizeof(t));
}

void atf_amc::amctest_VarlenMsgsPnew() {
    algo::ByteAry vbuf;
    ary_Addary(vbuf,ToBytes(atf_amc::MsgLTA("ab")));
    ary_Addary(vbuf,ToBytes(atf_amc::MsgLTB("cd")));
    algo::ByteAry msgvbuf;
    atf_amc::MsgLTV *msgv = atf_amc::MsgLTV_FmtByteAry(msgvbuf,ary_Getary(vbuf));
    vrfy_(msgv);
    vrfyeq_(msgv->type,atf_amc_MsgHdrLT_type_atf_amc_MsgLTV);
    vrfyeq_(msgv->len,ssizeof(atf_amc::MsgLTA)+ssizeof(atf_amc::MsgLTB)+ssizeof(atf_amc::MsgLTV)-2);

    atf_amc::MsgLTV_v_curs vi;
    MsgLTV_v_curs_Reset(vi,*msgv);
    vrfy_(MsgLTV_v_curs_ValidQ(vi));
    atf_amc::MsgHdrLT &ha = MsgLTV_v_curs_Access(vi);
    atf_amc::MsgLTA *a = MsgLTA_Castdown(ha);
    vrfy_(a);
    vrfyeq_(a->a,"ab");

    MsgLTV_v_curs_Next(vi);
    vrfy_(MsgLTV_v_curs_ValidQ(vi));
    atf_amc::MsgHdrLT &hb = MsgLTV_v_curs_Access(vi);
    atf_amc::MsgLTB *b = MsgLTB_Castdown(hb);
    vrfy_(b);
    vrfyeq_(b->b,"cd");

    MsgLTV_v_curs_Next(vi);
    vrfy_(!MsgLTV_v_curs_ValidQ(vi));
}

template <typename T> strptr atf_amc::Bytes(T &arg) {
    return strptr(arg, sizeof(T)-1);
}

void atf_amc::amctest_Varlen2() {
    // pnew
    algo::ByteAry buf;
    Varlen2Msg *msg = Varlen2Msg_FmtByteAry(buf,"Hello","My","World!");
    vrfyeq_(s1_Getary(*msg),"Hello");
    vrfyeq_(s2_Getary(*msg),"My");
    vrfyeq_(s3_Getary(*msg),"World!");

    // print
    vrfyeq_(tempstr()<<*msg, "atf_amc.Varlen2Msg  s1:Hello  s2:My  s3:World!");

    // read
    algo::Refurbish(buf);
    atf_amc::MsgHeaderMsgs_ReadStrptr("atf_amc.Varlen2Msg  s1:Salve  s2:Mi  s3:Munde!", buf);
    msg = (Varlen2Msg *)buf.ary_elems;
    vrfyeq_(s1_Getary(*msg),"Salve");
    vrfyeq_(s2_Getary(*msg),"Mi");
    vrfyeq_(s3_Getary(*msg),"Munde!");
    vrfyeq_(ToStrPtr(ary_Getary(buf)),Bytes("\001\020\025\000\005\000\007\000SalveMiMunde!"));

    // read in reverse order
    algo::Refurbish(buf);
    atf_amc::MsgHeaderMsgs_ReadStrptr("atf_amc.Varlen2Msg  s3:Welt!  s2:Mein s1:Hallo", buf);
    msg = (Varlen2Msg *)buf.ary_elems;
    vrfyeq_(s1_Getary(*msg),"Hallo");
    vrfyeq_(s2_Getary(*msg),"Mein");
    vrfyeq_(s3_Getary(*msg),"Welt!");
    vrfyeq_(ToStrPtr(ary_Getary(buf)),Bytes("\001\020\026\000\005\000\011\000HalloMeinWelt!"));

    // cursors
    {
        const char *s1="Hallo";
        int i1(0);
        ind_beg(Varlen2Msg_s1_curs,c,*msg) {
            vrfyeq_(c,s1[i1]);
            ++i1;
        }ind_end;
        vrfyeq_(i1,(int)strlen(s1));
    }
    {
        const char *s2="Mein";
        int i2(0);
        ind_beg(Varlen2Msg_s2_curs,c,*msg) {
            vrfyeq_(c,s2[i2]);
            ++i2;
        }ind_end;
        vrfyeq_(i2,(int)strlen(s2));
    }
    {
        const char *s3="Welt!";
        int i3(0);
        ind_beg(Varlen2Msg_s3_curs,c,*msg) {
            vrfyeq_(c,s3[i3]);
            ++i3;
        }ind_end;
        vrfyeq_(i3,(int)strlen(s3));
    }
}

bool atf_amc::Arycmp(algo::aryptr<u32> a, algo::aryptr<u32> b) {
    bool ret = elems_N(a)==elems_N(b);
    for (int i=0; ret && i<elems_N(a); ++i) {
        ret = a[i]==b[i];
    }
    if (!ret) {
        cstring out;
        algo::ListSep ls1,ls2;
        out << "Array mismatch, got {";
        frep_(i,elems_N(a)) {
            out << ls1 << a[i];
        }
        out << "}, but shall be {";
        frep_(i,elems_N(b)) {
            out << ls2 << b[i];
        }
        out << "}";
        prlog(out);
    }
    return ret;
}

void atf_amc::amctest_Varlen2a() {
    const u32 a1[] = {1,2,3};
    const u32 a2[] = {4,5,6};
    const u32 a3[] = {7,8,9};

    algo::aryptr<u32> pa1(a1,_array_count(a1));
    algo::aryptr<u32> pa2(a2,_array_count(a2));
    algo::aryptr<u32> pa3(a3,_array_count(a3));

    // pnew
    algo::ByteAry buf;
    Varlen2aMsg *msg = Varlen2aMsg_FmtByteAry(buf,pa1,pa2,pa3);

    vrfy_(Arycmp(u1_Getary(*msg),pa1));
    vrfy_(Arycmp(u2_Getary(*msg),pa2));
    vrfy_(Arycmp(u3_Getary(*msg),pa3));

    vrfyeq_(tempstr()<<*msg, "atf_amc.Varlen2aMsg  u1.0:1  u1.1:2  u1.2:3  u2.0:4  u2.1:5  u2.2:6  u3.0:7  u3.1:8  u3.2:9");

    // read
    algo::Refurbish(buf);
    atf_amc::MsgHeaderMsgs_ReadStrptr("atf_amc.Varlen2aMsg u3.0:7  u2.0:4  u1.0:1  u3.1:8  u2.1:5  u1.1:2  u3.2:9  u2.2:6  u1.2:3", buf);
    msg = (Varlen2aMsg *)buf.ary_elems;
    vrfy_(Arycmp(u1_Getary(*msg),pa1));
    vrfy_(Arycmp(u2_Getary(*msg),pa2));
    vrfy_(Arycmp(u3_Getary(*msg),pa3));
    vrfyeq_(ToStrPtr(ary_Getary(buf)), Bytes("\002\020,\000"
                                             "\014\000"
                                             "\030\000"
                                             "\001\000\000\000"
                                             "\002\000\000\000"
                                             "\003\000\000\000"
                                             "\004\000\000\000"
                                             "\005\000\000\000"
                                             "\006\000\000\000"
                                             "\007\000\000\000"
                                             "\010\000\000\000"
                                             "\011\000\000\000"));
}

void atf_amc::amctest_Varlen2m() {
    algo::ByteAry buf;
    const char *str = "atf_amc.Varlen2mMsg"
        "  m3:'atf_amc.Varlen2Msg  s1:ii  s2:1  s3:9'"
        "  m2:'atf_amc.Varlen2Msg  s1:h  s2:2  s3:8'"
        "  m1:'atf_amc.Varlen2Msg  s1:gg  s2:3  s3:7'"
        "  m3:'atf_amc.Varlen2Msg  s1:fff  s2:4  s3:6'"
        "  m2:'atf_amc.Varlen2Msg  s1:eee  s2:5  s3:5'"
        "  m1:'atf_amc.Varlen2Msg  s1:dddd  s2:6  s3:4'"
        "  m3:'atf_amc.Varlen2Msg  s1:ccc  s2:7  s3:3'"
        "  m2:'atf_amc.Varlen2Msg  s1:bc  s2:8  s3:2'"
        "  m1:'atf_amc.Varlen2Msg  s1:a  s2:9  s3:1'";
    atf_amc::MsgHeaderMsgs_ReadStrptr(str,buf);
    Varlen2mMsg *msg = (Varlen2mMsg*)buf.ary_elems;

    const char *str1 =
        "atf_amc.Varlen2mMsg"
        "  m1.0:\"atf_amc.Varlen2Msg  s1:gg  s2:3  s3:7\""
        "  m1.1:\"atf_amc.Varlen2Msg  s1:dddd  s2:6  s3:4\""
        "  m1.2:\"atf_amc.Varlen2Msg  s1:a  s2:9  s3:1\""
        "  m2.0:\"atf_amc.Varlen2Msg  s1:h  s2:2  s3:8\""
        "  m2.1:\"atf_amc.Varlen2Msg  s1:eee  s2:5  s3:5\""
        "  m2.2:\"atf_amc.Varlen2Msg  s1:bc  s2:8  s3:2\""
        "  m3.0:\"atf_amc.Varlen2Msg  s1:ii  s2:1  s3:9\""
        "  m3.1:\"atf_amc.Varlen2Msg  s1:fff  s2:4  s3:6\""
        "  m3.2:\"atf_amc.Varlen2Msg  s1:ccc  s2:7  s3:3\"";
    vrfyeq_(tempstr()<<*msg, str1);

    const char str2[] = "\004\020w\000%\000I\000"
        "\001\020\014\000\002\000\003\000gg37"
        "\001\020\016\000\004\000\005\000dddd64"
        "\001\020\013\000\001\000\002\000a91"
        "\001\020\013\000\001\000\002\000h28"
        "\001\020\r\000\003\000\004\000eee55"
        "\001\020\014\000\002\000\003\000bc82"
        "\001\020\014\000\002\000\003\000ii19"
        "\001\020\r\000\003\000\004\000fff46"
        "\001\020\r\000\003\000\004\000ccc73";
    vrfyeq_(ToStrPtr(ary_Getary(buf)), Bytes(str2));
}

void atf_amc::amctest_Varlen2v() {
    algo::ByteAry buf;
    const char *str = "atf_amc.Varlen2vMsg"
        "  v1:'atf_amc.VarlenK  i:1'"
        "  v1:'atf_amc.VarlenK  i:2  i:3'"
        "  v1:'atf_amc.VarlenK  i:4'"

        "  v2:'atf_amc.VarlenK  i:5  i:6'"
        "  v2:'atf_amc.VarlenK  i:7'"
        "  v2:'atf_amc.VarlenK  i:8  i:9'"

        "  v3:'atf_amc.VarlenK  i:10'"
        "  v3:'atf_amc.VarlenK  i:11  i:12'"
        "  v3:'atf_amc.VarlenK  i:13  i:14  i:15'";
    atf_amc::MsgHeaderMsgs_ReadStrptr(str,buf);
    Varlen2vMsg *msg = (Varlen2vMsg*)buf.ary_elems;

    const char *str1 = "atf_amc.Varlen2vMsg"
        "  v1.0:\"atf_amc.VarlenK  i.0:1\""
        "  v1.1:\"atf_amc.VarlenK  i.0:2  i.1:3\""
        "  v1.2:\"atf_amc.VarlenK  i.0:4\""
        "  v2.0:\"atf_amc.VarlenK  i.0:5  i.1:6\""
        "  v2.1:\"atf_amc.VarlenK  i.0:7\""
        "  v2.2:\"atf_amc.VarlenK  i.0:8  i.1:9\""
        "  v3.0:\"atf_amc.VarlenK  i.0:10\""
        "  v3.1:\"atf_amc.VarlenK  i.0:11  i.1:12\""
        "  v3.2:\"atf_amc.VarlenK  i.0:13  i.1:14  i.2:15\"";
    vrfyeq_(tempstr()<<*msg, str1);

    const char str2[] = "\003\020h\000\034\000<\000"
        "\010\000\000\000\001\000\000\000"
        "\014\000\000\000\002\000\000\000\003\000\000\000"
        "\010\000\000\000\004\000\000\000"
        "\014\000\000\000\005\000\000\000\006\000\000\000"
        "\010\000\000\000\007\000\000\000"
        "\014\000\000\000\010\000\000\000\t\000\000\000"
        "\010\000\000\000\012\000\000\000"
        "\014\000\000\000\013\000\000\000\014\000\000\000"
        "\020\000\000\000\015\000\000\000\016\000\000\000\017\000\000\000";
    vrfyeq_(ToStrPtr(ary_Getary(buf)), Bytes(str2));
}

// Fixture: VarlenWMsg's header counts total bytes (MsgHeader.length, scale:1),
// but each VarlenW element counts 4-byte words past the first (scale:4
// extra:-4). Reading an element from a string must store the element's length
// through the element's own lenfld formula -- an element of 12 bytes stores
// length 2, not 12. A raw byte count would make the element cursor stride
// past the element and misread everything that follows.
void atf_amc::amctest_VarlenNestScale() {
    algo::ByteAry buf;
    const char *str = "atf_amc.VarlenWMsg"
        "  word:'atf_amc.VarlenW  i:1  i:2'"
        "  word:'atf_amc.VarlenW  i:3'";
    atf_amc::MsgHeaderMsgs_ReadStrptr(str,buf);
    VarlenWMsg *msg = (VarlenWMsg*)buf.ary_elems;

    const char *str1 = "atf_amc.VarlenWMsg"
        "  word.0:\"atf_amc.VarlenW  i.0:1  i.1:2\""
        "  word.1:\"atf_amc.VarlenW  i.0:3\"";
    vrfyeq_(tempstr()<<*msg, str1);

    const char str2[] = "\005\020\030\000"
        "\002\000\000\000\001\000\000\000\002\000\000\000"
        "\001\000\000\000\003\000\000\000";
    vrfyeq_(ToStrPtr(ary_Getary(buf)), Bytes(str2));
}

// A varlen element with a scaled lenfld over a byte-granular payload: only
// a byte count that lands on a scale multiple has a representable length
// word. VarlenB counts 4-byte words past the first (scale:4 extra:-4), and
// its payload is a char array, so any payload length is expressible in the
// input. A 4-byte payload round-trips; a 3-byte payload has no length word
// that reconstructs it -- the read must fail rather than store a truncated
// word that makes the element cursor stride into the element's middle.
void atf_amc::amctest_VarlenNestScaleGuard() {
    algo::ByteAry buf;
    const char *str = "atf_amc.VarlenBMsg"
        "  b:'atf_amc.VarlenB  c:abcd'";
    atf_amc::MsgHeaderMsgs_ReadStrptr(str,buf);
    VarlenBMsg *msg = (VarlenBMsg*)buf.ary_elems;
    vrfyeq_(tempstr()<<*msg, "atf_amc.VarlenBMsg  b.0:\"atf_amc.VarlenB  c:abcd\"");

    algo::ByteAry buf2;
    bool ok = atf_amc::MsgHeaderMsgs_ReadStrptrMaybe("atf_amc.VarlenBMsg  b:'atf_amc.VarlenB  c:abc'", buf2);
    vrfy_(!ok);
}

// A varlen field with a one-letter name: the field name becomes the element
// ctype's reference name, so the generated readers take a parameter named w;
// where the parameter is unused, the (void) suppression must still be
// emitted even though the body contains w inside another token (new()).
void atf_amc::amctest_VarlenShortName() {
    algo::ByteAry buf;
    const char *str = "atf_amc.VarlenVMsg"
        "  w:'atf_amc.VarlenW  i:7'";
    atf_amc::MsgHeaderMsgs_ReadStrptr(str,buf);
    VarlenVMsg *msg = (VarlenVMsg*)buf.ary_elems;
    vrfyeq_(tempstr()<<*msg, "atf_amc.VarlenVMsg  w.0:\"atf_amc.VarlenW  i.0:7\"");
}

// Fixture: a wire frame combining fbigend storage, bitfield views (a typefld
// enum + a lenfld), Base inheritance, a Varlen array of a ctype, and a String.
// The shape pins three generator obligations:
//  - Base CopyOut/CopyIn reference an fbigend field by its _be member
//  - GetEnum on a bitfield field reads via the _Get accessor, not parent.<f>
//  - the lenfld default (ssizeof) uses the parent arg, not *this, in the
//    free _Init function generated for a bitfield lenfld.
void atf_amc::amctest_NetFrameVarlen() {
    // header word is big-endian: kind = SETTINGS(4) in low 8 bits, len = 12 in
    // the high 24 bits -> word 0x00000c04 -> bytes 00 00 0c 04. Then two 6-byte
    // big-endian (id,val) entries (12 bytes of payload, matching len).
    u8 wire[] = {
        0x00,0x00,0x0c,0x04,                // len=12, kind=4 (SETTINGS)
        0x01,0x02, 0x03,0x04,0x05,0x06,     // entry0: id=0x0102 val=0x03040506
        0x11,0x12, 0x13,0x14,0x15,0x16      // entry1: id=0x1112 val=0x13141516
    };
    atf_amc::NetFrame &frame = *(atf_amc::NetFrame*)wire;

    // fbigend word decoded, then bitfield views extracted
    vrfyeq_(kind_Get(frame), atf_amc_NetFrameHdr_kind_SETTINGS);
    vrfyeq_(len_Get(frame), u32(12));

    // generated _N from the lenfld: 12 payload bytes / 6 = 2 entries
    vrfyeq_(entry_N(frame), u32(2));

    // cursor over a Varlen-of-ctype; each entry's fields are big-endian
    int i = 0;
    ind_beg(atf_amc::NetFrame_entry_curs, e, frame) {
        if (i == 0) {
            vrfyeq_(id_Get(e), u16(0x0102));
            vrfyeq_(val_Get(e), u32(0x03040506));
        } else {
            vrfyeq_(id_Get(e), u16(0x1112));
            vrfyeq_(val_Get(e), u32(0x13141516));
        }
        ++i;
    }ind_end;
    vrfyeq_(i, 2);

    // Base CopyOut copies the fbigend storage word by its _be member
    atf_amc::NetFrameHdr hdr;
    net_frame_CopyOut(frame, hdr);
    vrfyeq_(kind_Get(hdr), atf_amc_NetFrameHdr_kind_SETTINGS);
    vrfyeq_(len_Get(hdr), u32(12));

    // GetEnum on the bitfield typefld reads through kind_Get
    vrfyeq_(int(kind_GetEnum(hdr)), int(atf_amc_NetFrameHdr_kind_SETTINGS));

    // default-constructed frame: msgtype default kind=SETTINGS, lenfld default len=0
    // (ssizeof(NetFrame) - 4). This is the path whose Init used *this.
    atf_amc::NetFrame fresh;
    vrfyeq_(kind_Get(fresh), atf_amc_NetFrameHdr_kind_SETTINGS);
    vrfyeq_(len_Get(fresh), u32(0));
    vrfyeq_(entry_N(fresh), u32(0));

    // String formatting. The NetFrame Tuple printer walks the Varlen cursor and
    // prints each NetEntry; the typefld (kind) and lenfld (len) are framing, not
    // printed. Entry values are the big-endian-decoded fields.
    cstring framestr;
    atf_amc::NetFrame_Print(frame, framestr);
    vrfyeq_(framestr,
            "atf_amc.NetFrame"
            "  entry.0:\"atf_amc.NetEntry  id:258  val:50595078\""
            "  entry.1:\"atf_amc.NetEntry  id:4370  val:320083222\"");

    // Read the whole frame back from its string form. A NetFrame is variable-size,
    // so the read goes through the msgtype dispatch reader into a ByteAry: it sets
    // kind from the "atf_amc.NetFrame" type tag, parses each entry, and recomputes
    // the bitfield lenfld. The reconstructed bytes must equal the original wire frame.
    algo::ByteAry rbuf;
    atf_amc::NetFrameHdrMsgsCase rcase = atf_amc::NetFrameHdrMsgs_ReadStrptr(framestr, rbuf);
    vrfyeq_(int(rcase), int(atf_amc_NetFrameHdrMsgsCase_atf_amc_NetFrame));
    atf_amc::NetFrame &rd = *(atf_amc::NetFrame*)rbuf.ary_elems;
    vrfyeq_(kind_Get(rd), atf_amc_NetFrameHdr_kind_SETTINGS);
    vrfyeq_(len_Get(rd), u32(12));
    vrfyeq_(entry_N(rd), u32(2));
    vrfyeq_(id_Get(entry_Getary(rd)[0]),  u16(0x0102));
    vrfyeq_(val_Get(entry_Getary(rd)[0]), u32(0x03040506));
    vrfyeq_(id_Get(entry_Getary(rd)[1]),  u16(0x1112));
    vrfyeq_(val_Get(entry_Getary(rd)[1]), u32(0x13141516));
    vrfyeq_(ToStrPtr(ary_Getary(rbuf)), algo::strptr((char*)wire, sizeof(wire)));

    // NetEntry print + read round-trip (read:Y print:Y on an fbigend ctype):
    // printing decodes big-endian, reading re-encodes it, so the storage bytes match.
    atf_amc::NetEntry e0;
    id_Set(e0, u16(0x0102));
    val_Set(e0, u32(0x03040506));
    vrfyeq_(tempstr() << e0, "atf_amc.NetEntry  id:258  val:50595078");

    atf_amc::NetEntry e1;
    vrfy_(atf_amc::NetEntry_ReadStrptrMaybe(e1, "atf_amc.NetEntry  id:258  val:50595078"));
    vrfyeq_(id_Get(e1), u16(0x0102));
    vrfyeq_(val_Get(e1), u32(0x03040506));
    vrfyeq_(e1.id_be, e0.id_be);
    vrfyeq_(e1.val_be, e0.val_be);
}

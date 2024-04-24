// Copyright (C) 2023-2024 AlgoRND
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
// Exceptions: NO
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

// template is needed for string literal in order
// to get correct length for NUL char inside
template <typename T>
static void Check(T &bin_literal, strptr str) {
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

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

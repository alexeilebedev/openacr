// (C) 2017-2019 NYSE | Intercontinental Exchange
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
// Target: atf_amc (exe) -- Unit tests for amc (see amctest table)
// Exceptions: NO
// Source: cpp/atf/amc/main.cpp
//
// Created By: alexei.lebedev
// Recent Changes: alexei.lebedev
//

#include "include/atf_amc.h"
#include <algorithm>


// -----------------------------------------------------------------------------

void atf_amc::amctest_PrintBase36() {
    {
        algo::LnumStr7_U32_Base36 str;
        ch_SetnumMaybe(str, 10);
        vrfyeq_(str, "A");
    }
    {
        algo::LnumStr7_U32_Base36 str;
        ch_SetnumMaybe(str, 35);
        vrfyeq_(str, "Z");
    }
    {
        algo::LnumStr7_U32_Base36 str;
        ch_SetnumMaybe(str, 9);
        vrfyeq_(str, "9");
    }
    {
        algo::LnumStr7_U32_Base36 str;
        ch_SetnumMaybe(str, 36);
        vrfyeq_(str, "10");
    }
}

void atf_amc::amctest_SetGetnumBase10() {
    {
        // length <= 4
        u32 num = 987;
        algo::LnumStr4_U32 str;
        // Test Setnum
        ch_SetnumMaybe(str, num);
        vrfy_(memcmp(str.ch, "0987", 4) == 0);
        // Test Getnum
        u32 val = ch_GetnumDflt(str,0);
        vrfyeq_(val, num);
    }
    {
        // length <= 8
        u32 num = 98765;
        algo::LnumStr8_U32 str;
        // Test Setnum
        ch_SetnumMaybe(str, num);
        vrfy_(memcmp(str.ch, "00098765", 8) == 0);
        // Test Getnum
        u32 val = ch_GetnumDflt(str,0);
        vrfyeq_(val, num);
    }
    {
        // length = 9
        u32 num = 987654321;
        algo::LnumStr9_U32 str;
        // Test Setnum
        ch_SetnumMaybe(str, num);
        vrfy_(memcmp(str.ch, "987654321", 9) == 0);
        // Test Getnum
        u32 val = ch_GetnumDflt(str,0);
        vrfyeq_(val, num);
    }
    {
        // length > 9
        u64 num = 987654321012;
        algo::LnumStr12_U64 str;
        // Test Setnum
        ch_SetnumMaybe(str, num);
        vrfy_(memcmp(str.ch, "987654321012", 12) == 0);
        // Test Getnum
        u64 val = ch_GetnumDflt(str,0);
        vrfyeq_(val, num);
    }
}



// not actually a test but scratch area
void atf_amc::amctest_Scratch() {
}


void atf_amc::amctest_CastUp() {
    atf_amc::Seqmsg seqmsg;
    const atf_amc::MsgHeader &hdr = Castbase(seqmsg);
    vrfyeq_((u64)&hdr, (u64)&seqmsg);
}

void atf_amc::amctest_CastDown() {
    atf_amc::Seqmsg seqmsg;
    const atf_amc::MsgHeader &hdr = Castbase(seqmsg);
    atf_amc::Seqmsg *seqmsg2 = Seqmsg_Castdown((atf_amc::MsgHeader&)hdr);
    vrfyeq_((u64)&seqmsg, (u64)seqmsg2);
}

void atf_amc::amctest_CastDownFail() {
    atf_amc::Seqmsg seqmsg;
    const atf_amc::MsgHeader &hdr = Castbase(seqmsg);
    atf_amc::Text *streamhb = Text_Castdown((atf_amc::MsgHeader&)hdr);
    vrfyeq_((u64)streamhb, (u64)0);
}

void atf_amc::amctest_CastDownTooShort() {
    atf_amc::Seqmsg seqmsg;
    seqmsg.length.value = sizeof(atf_amc::Seqmsg)-1;
    const atf_amc::MsgHeader &hdr = Castbase(seqmsg);
    atf_amc::Seqmsg *seqmsg2 = Seqmsg_Castdown((atf_amc::MsgHeader&)hdr);
    vrfyeq_((u64)seqmsg2, (u64)0);
}

void atf_amc::amctest_CopyOut1() {
    atf_amc::Text text;
    vrfyeq_(text.type, atf_amc::MsgType(atf_amc_MsgType_atf_amc_Text));
    vrfyeq_(text.length, sizeof(atf_amc::Text));
}

void atf_amc::amctest_CopyOut2() {
}

void atf_amc::amctest_CopyOut3() {
}

void atf_amc::amctest_TestInsertX2() {
    atf_amc::InsertStrptrMaybe("atf_amc.TypeS  types:33");
    atf_amc::FTypeS *types = atf_amc::ind_types_Find(33);// check that it's there
    vrfy_(types);
    vrfyeq_(atf_amc::types_N(), 1);
    vrfyeq_(atf_amc::typet_N(), 0);
    vrfyeq_(atf_amc::zdl_typet_N(*types), 0);

    atf_amc::InsertStrptrMaybe("atf_amc.TypeT  types:33  j:44");
    vrfyeq_(atf_amc::types_N(), 1);
    vrfyeq_(atf_amc::typet_N(), 1);
    vrfyeq_(atf_amc::zdl_typet_N(*types), 1);
    // cleanup
    atf_amc::types_RemoveAll();
    atf_amc::typet_RemoveAll();
}

void atf_amc::amctest_TestInsertX3() {
    atf_amc::InsertStrptrMaybe("atf_amc.TypeS  types:33");
    atf_amc::FTypeS *types = atf_amc::ind_types_Find(33);// check that it's there
    vrfy_(types);
    vrfyeq_(atf_amc::types_N(), 1);
    vrfyeq_(atf_amc::typet_N(), 0);
    vrfyeq_(atf_amc::zdl_typet_N(*types), 0);
    atf_amc::InsertStrptrMaybe("atf_amc.TypeT  types:44  j:44");
    vrfyeq_(atf_amc::types_N(), 1);
    vrfyeq_(atf_amc::typet_N(), 0);

    // cleanup
    atf_amc::types_RemoveAll();
    atf_amc::typet_RemoveAll();
}

void atf_amc::amctest_TestCstring1() {
    algo::cstring str;
    vrfyeq_(strptr(str), strptr(""));
    str="test";// check assignment
    vrfyeq_(strptr(str), strptr("test"));
    algo::cstring str2(str);// check copy constructor
    vrfyeq_(strptr(str), strptr(str2));// check strptr equality
    vrfyeq_(str, str2);// check string equality
    algo::cstring str3;
    str3=str;// copy assignment
    vrfyeq_(str, strptr("test"));// check string equality
    vrfyeq_(str3, strptr("test"));// check string equality
    str3=str;// copy assignment (#2)
    vrfyeq_(str, strptr("test"));// check string equality
    vrfyeq_(str3, strptr("test"));// check string equality
}

void atf_amc::amctest_TestCstring2() {
    atf_amc::Cstring str;
    str.str = "abcde";
    vrfyeq_(strptr(str.str), strptr("abcde"));
    vrfy(atf_amc::cstring_InsertMaybe(str), algo_lib::_db.errtext);
    vrfyeq_(atf_amc::ind_cstring_Find("abcde") != NULL, true);
    atf_amc::FCstring *fcstring = atf_amc::ind_cstring_Find("abcde");
    vrfy_(fcstring);
    cstring_CopyOut(*fcstring, str);
    vrfyeq_(strptr(fcstring->str), strptr("abcde"));
    atf_amc::cstring_RemoveAll();
}

void atf_amc::amctest_TestSep1() {
    atf_amc::Sep1 sep1;
    sep1.val1 = 1;
    sep1.val2 = 2;
    sep1.val3 = 3;
    tempstr out;
    out << sep1;
    vrfyeq_(strptr(out), strptr("1-2/3"));
}

void atf_amc::amctest_TestSep2() {
    atf_amc::Sep1 sep1;
    Sep1_ReadStrptrMaybe(sep1, strptr("1-2/3"));
    vrfyeq_(sep1.val1, (u32)1);
    vrfyeq_(sep1.val2, (u32)2);
    vrfyeq_(sep1.val3, (u32)3);
}

void atf_amc::amctest_TestRegx1() {
    atf_amc::TestRegx1 test_regx1;
    strptr expr("atf_amc.TestRegx1  val:aabb%dd");// regx is read from string
    atf_amc::TestRegx1_ReadStrptrMaybe(test_regx1, expr);
    vrfyeq_(Regx_Match(test_regx1.val, "aabbEEEdd"), true);
    vrfyeq_(Regx_Match(test_regx1.val, "asdf"), false);

    cstring out;
    out << test_regx1;
    vrfyeq_(out, expr);
}

// --------------------------------------------------------------------------------

static void ClearStack() {
    memset(alloca(1024),0xff,1024);
}

void atf_amc::amctest_SubstrDfltval() {
    atf_amc::AmcSubstr1 substr1;

    substr1.strval = "abcde"; // invalid string for extracting 'b'
    ClearStack();// clear the stack here.
    vrfyeq_(boolval_Get(substr1),false); // default value
    vrfyeq_(intval_Get(substr1),0); // default value

    substr1.strval = "abcde.false";
    ClearStack();// clear the stack here.
    vrfyeq_(boolval_Get(substr1),false);
    vrfyeq_(intval_Get(substr1),0); // default value

    substr1.strval = "12345.67890";
    ClearStack();// clear the stack here.
    vrfyeq_(boolval_Get(substr1),false); // default value
    vrfyeq_(intval_Get(substr1),67890);
    vrfyeq_(0,0); // test code goes here
}

// --------------------------------------------------------------------------------

void atf_amc::amctest_Minmax() {
    vrfyeq_(i32_Min(1,2), 1);
    vrfyeq_(i32_Min(2,1), 1);

    vrfyeq_(i32_Max(1,2), 2);
    vrfyeq_(i32_Max(2,1), 2);

    i32 x=0;
    vrfyeq_(i32_UpdateMax(x,2), true);
    vrfyeq_(i32_UpdateMax(x,2), false);
    vrfyeq_(i32_UpdateMin(x,1), true);
    vrfyeq_(i32_UpdateMin(x,1), false);
}
// --------------------------------------------------------------------------------

void atf_amc::bh_typec_Step() {
}

void atf_amc::zsl_h_typec_Step() {
}

void atf_amc::zs_t_typec_Step() {
}

void atf_amc::csl_h_typec_Step() {
}

void atf_amc::cs_t_typec_Step() {
}

void atf_amc::amctest_ImdXref() {
    int n = 100000;
    // reserve room in index
    atf_amc::ind_typea_Reserve(n);

    // insert N items into table A
    u64 c=get_cycles();
    frep_(i,n) {
        atf_amc::TypeA typea;
        typea.typea = i;
        vrfy(atf_amc::typea_InsertMaybe(typea), algo_lib::_db.errtext);
    }

    c=get_cycles()-c;
    verblog("tpool/hash insert "<<n<<" items: "<<c/n<<" cycles/item");

    // look up items
    frep_(i,n) {
        vrfy_(atf_amc::ind_typea_Find(i));
    }

    vrfy_(atf_amc::ind_typea_Find(1));// check that it exists

    // insert child item
    atf_amc::TypeB typeb;
    vrfy(atf_amc::typeb_InsertMaybe(typeb), algo_lib::_db.errtext);

    atf_amc::typea_RemoveAll();
    vrfyeq_(atf_amc::typea_N(), 0);
}

// -----------------------------------------------------------------------------

static void _StripTypeTag(strptr s, strptr w, bool cmp, strptr rest) {
    vrfy_(algo::StripTypeTag(s,w)==cmp);
    vrfy_(s==rest);
}

// -----------------------------------------------------------------------------

void atf_amc::amctest_Typetag() {
    vrfy_(GetTypeTag("a b") == "a");
    vrfy_(GetTypeTag(" a b") == "a");
    vrfy_(GetTypeTag(" a") == "a");

    _StripTypeTag("a b","a",true, "b");
    _StripTypeTag(" a b","a",true, "b");
    _StripTypeTag(" a","a",true, "");
    _StripTypeTag(" a        x","a",true, "x");
    _StripTypeTag("a b","x",false, "a b");
}

// -----------------------------------------------------------------------------

// Check that gconst field within tuple is printed as raw
void atf_amc::amctest_PrintRawGconst() {
}

// -----------------------------------------------------------------------------

void atf_amc::amctest_MsgLength() {
    ByteAry buf;
    cstring temp;
    // try a bunch of sizes
    for (int i=0; i<100; i++) {
        temp << char(i+1);
        atf_amc::Text *msg = atf_amc::Text_FmtByteAry(buf, temp);
        vrfy_(GetMsgLength(*msg) == msg->length);
        vrfy_(memptr_ToStrptr(GetMsgMemptr(*msg)) == strptr((char*)msg,msg->length));
        vrfy_(text_Getary(*msg) == temp);
    }
}

// -----------------------------------------------------------------------------

void atf_amc::Main() {
    int nmatch=0;
    ind_beg(atf_amc::_db_amctest_curs,amctest, atf_amc::_db) {
        amctest.select = Regx_Match(atf_amc::_db.cmdline.amctest, amctest.amctest);
        nmatch += amctest.select;
    }ind_end;
    ind_beg(atf_amc::_db_amctest_curs,amctest, atf_amc::_db) if (amctest.select) {
        bool dofork=nmatch>1;
#ifdef _DEBUG
        dofork=false;
#endif
        bool success=false;
        int child_pid=0;
        try {
            if (dofork) {
                child_pid = fork();
                if (child_pid==0) {
                    alarm(900);
                }
            }
            errno_vrfy(child_pid!=-1, "fork");
            if (child_pid==0) {
                algo_lib::DetachBadTags();
                (*amctest.step)();
                success=true;
            }
            if (child_pid != 0 && dofork) {
                int status = 0;
                int ret    = waitpid(0,&status,0);
                vrfy(ret == child_pid, "waitpid");
                success = status==0;
            }
            if (child_pid == 0 && dofork) {
                _exit(0);
            }
        }catch(algo_lib::ErrorX &x) {
            prerr("atf_amc.error"
                  <<Keyval("amctest",amctest.amctest)
                  <<Keyval("comment",x.str));
            if (child_pid==0) {
                _exit(1);// child
            }
            success=false;
        }
        algo_lib::_db.exit_code += !success;
        prlog("atf_amc.run"
              <<Keyval("amctest",amctest.amctest)
              <<Keyval("success",Bool(success))
              <<Keyval("comment",amctest.comment));
    }ind_end;
}

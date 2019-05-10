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
// Target: atf_unit (exe) -- Algo Test Framework: unit tests
// Exceptions: yes
// Source: cpp/atf/unit/algo_lib.cpp
//
// Created By: alexei.lebedev vladimir.parizhsky
// Authors: alexei.lebedev
// Recent Changes: alexei.lebedev hayk.mkrtchyan ezequiel.fernandes
//

#include "include/atf_unit.h"
#include <algorithm>


void atf_unit::unittest_algo_lib_PopCnt1() {
    u32 t;// 32-bit
    vrfy_(algo::u32_Count1s(u32(0)) == 0);
    u32 N = sizeof(t) * 8;
    rep_(i,int(N-1)) {
        vrfy_(algo::u32_Count1s(   u32(1) << i    ) == u32(1  ));
        vrfy_(algo::u32_Count1s(~( u32(1) << i   )) ==     N-1);
        vrfy_(algo::u32_Count1s(  (u32(1) << i)-1 ) == u32(i  ));
        vrfy_(algo::u32_Count1s(~((u32(1) << i)-1)) ==     N-i);
    }
}

void atf_unit::unittest_algo_lib_PopCnt2() {
    u64 t;// 64-bit
    vrfy_(algo::u32_Count1s(u32(0)) == 0);
    u32 N = sizeof(t) * 8;
    rep_(i,int(N-1)) {
        vrfy_(algo::u64_Count1s(   u64(1) << i    ) == u32(1  ));
        vrfy_(algo::u64_Count1s(~( u64(1) << i   )) ==     N-1);
        vrfy_(algo::u64_Count1s(  (u64(1) << i)-1 ) == u32(i  ));
        vrfy_(algo::u64_Count1s(~((u64(1) << i)-1)) ==     N-i);
    }
}

void atf_unit::unittest_algo_lib_TestBitSet() {
    algo_lib::Bitset B;
    ary_ExpandBits(B, 100);
    vrfy_(ary_N(B) == 2);
    ary_qSetBit(B, 0);
    ary_qSetBit(B, 4);
    ary_qSetBit(B, 63);
    vrfy_(ary_Sum1s(B) == 3);
    vrfy_( ary_qGetBit(B, 0));
    vrfy_( ary_qGetBit(B, 4));
    vrfy_( ary_qGetBit(B, 63));
    vrfy_(!ary_qGetBit(B, 64));
    vrfy_(!ary_qGetBit(B, 1));

    ary_qClearBit(B, 0);
    vrfy_(ary_Sum1s(B) == 2);
    vrfy_(!ary_qGetBit(B, 0));

    ary_ClearBitsAll(B);
    vrfy_(ary_Sum1s(B)==0);

    ary_AllocBit(B, 1000);
    ary_qSetBit(B, 1000);
    vrfy_(ary_qGetBit(B, 1000));
}

void atf_unit::unittest_algo_lib_TestFbitset() {
    atf_unit::Bitset frame;
    vrfyeq_(atf_unit::fld1_Sum1s(frame), u64(0));
    vrfyeq_(atf_unit::fld1_BitsEmptyQ(frame), true);
    Fill(atf_unit::fld1_Getary(frame), (short unsigned int)-1);
    vrfyeq_(atf_unit::fld1_Sum1s(frame), u64(64));
    vrfyeq_(atf_unit::fld1_BitsEmptyQ(frame), false);

    atf_unit::fld1_qClearBit(frame, 4);
    vrfyeq_(atf_unit::fld1_qGetBit(frame, 4), false);
    vrfyeq_(atf_unit::fld1_Sum1s(frame), u64(63));
    vrfyeq_(atf_unit::fld1_BitsEmptyQ(frame), false);

    atf_unit::fld1_ClearBit(frame, 64); // out of range
    vrfyeq_(atf_unit::fld1_Sum1s(frame), u64(63));
    vrfyeq_(atf_unit::fld1_BitsEmptyQ(frame), false);

    atf_unit::fld1_ClearBit(frame, 63);
    vrfyeq_(fld1_Sup(frame),63);

    atf_unit::fld1_qClearBit(frame, 1);

    vrfy_( atf_unit::fld1_qGetBit(frame, 0));
    vrfy_(!atf_unit::fld1_qGetBit(frame, 1));
    vrfy_( atf_unit::fld1_qGetBit(frame, 2));
    vrfy_( atf_unit::fld1_qGetBit(frame, 3));
    vrfy_(!atf_unit::fld1_qGetBit(frame, 4));
    vrfy_(!atf_unit::fld1_qGetBit(frame, 63));
    vrfyeq_(atf_unit::fld1_Sum1s(frame), u64(61));
    vrfyeq_(atf_unit::fld1_BitsEmptyQ(frame), false);
    atf_unit::fld1_ClearBitsAll(frame);
    vrfyeq_(atf_unit::fld1_Sum1s(frame), u64(0));
    vrfyeq_(atf_unit::fld1_BitsEmptyQ(frame), true);
}

void atf_unit::unittest_algo_lib_TestFbitset2() {
    atf_unit::Bitset frame;
    vrfyeq_(frame.fld8, u8(0));

    // set all 8 bits one by one
    for(int i=0; i<8; i++) {
        atf_unit::fld8_qSetBit(frame, i);
        vrfyeq_(frame.fld8, (i==7 ? u8(-1) : (u8(1)<<(i+1))-1));
        vrfyeq_(fld8_Sum1s(frame), u8(i+1));
        vrfyeq_(fld8_Sup(frame), i+1);
    }
    // clear all 8 bits one by one
    for (int i=7; i>=0; i--) {
        atf_unit::fld8_qClearBit(frame, i);
        vrfyeq_(frame.fld8, (u8(1)<<i)-1);
        vrfyeq_(fld8_Sum1s(frame), u8(i));
        vrfyeq_(fld8_Sup(frame), i);
    }
}

void atf_unit::unittest_algo_lib_TestFbitset3() {
    atf_unit::Bitset frame;
    vrfyeq_(frame.fld64, u64(0));

    // set all 64 bits one by one
    for(int i=0; i<64; i++) {
        atf_unit::fld64_qSetBit(frame, i);
        vrfyeq_(frame.fld64, (i==63 ? u64(-1) : (u64(1)<<(i+1))-1));
        vrfyeq_(fld64_Sum1s(frame), u64(i+1));
        vrfyeq_(fld64_Sup(frame), i+1);
        vrfyeq_(fld64_BitsEmptyQ(frame), false);
    }
    // clear all 64 bits one by one
    for (int i=63; i>=0; i--) {
        atf_unit::fld64_qClearBit(frame, i);
        vrfyeq_(frame.fld64, (u64(1)<<i)-1);
        vrfyeq_(fld64_Sum1s(frame), u64(i));
        vrfyeq_(fld64_Sup(frame), i);
        vrfyeq_(fld64_BitsEmptyQ(frame), i==0);
    }
}

void atf_unit::unittest_algo_lib_TestFbitset4() {
    atf_unit::Bitset frame;
    vrfyeq_(frame.fld128, u128(0));

    // set all 128 bits one by one
    for(int i=0; i<128; i++) {
        atf_unit::fld128_qSetBit(frame, i);
        vrfyeq_(frame.fld128, (i==127 ? u128(-1) : (u128(1)<<(i+1))-1));
        vrfyeq_(fld128_Sum1s(frame), u128(i+1));
    }
    // clear all 128 bits one by one
    for (int i=127; i>=0; i--) {
        atf_unit::fld128_qClearBit(frame, i);
        vrfyeq_(frame.fld128, (u128(1)<<i)-1);
        vrfyeq_(fld128_Sum1s(frame), u128(i));
    }
}

struct RandomTypeName {
    char c[18];
    RandomTypeName(){memset(this,0,sizeof(*this));}
};

// -----------------------------------------------------------------------------

static void DoTest(double d) {
    vrfyeq_(  DFloor(d),floor(d));
    vrfyeq_(DCeiling(d),ceil(d));
    vrfyeq_(  DTrunc(d),d-fmod(d,1));
}

void atf_unit::unittest_algo_lib_DoTestRounding() {
    DoTest(0.5);
    DoTest(1.5);
    DoTest(1.0);
    DoTest(0.999999999);
    DoTest(0.111111111);
    DoTest(1000000.01);
    DoTest(1000000.9999);
    DoTest(10000000000.9999);
    DoTest(1000000000000.9999);
    DoTest(-0.00000001);
    DoTest(-0.5);
    DoTest(-0.50000001);
    DoTest(-0.999999);
}

// CPU should only use the lower 5 bits for shifting
void atf_unit::unittest_algo_lib_CheckShiftMask() {
    static volatile int _g;
    rep_(i,32) {
        // gcc bug: gcc folds (1000000>>32) to zero
        _g = (i+32);
        vrfy_((1000000 >> i) == (1000000 >> _g));
        _g = (i+64);
        vrfy_((1000000 >> i) == (1000000 >> _g));
        _g = (i+128);
        vrfy_((1000000 >> i) == (1000000 >> _g));
    }
}

// -----------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_Interlocked() {
    int x=0;
    vrfy_(IncrMT(&x,100) == 100);
    vrfy_(IncrMT(&x,100) == 200);
    vrfy_(IncrMT(&x,-100) == 100);
    vrfy_(IncrMT(&x,-100) == 0);

    int y=0;
    vrfy_(algo::IncrMT(&y)==1);
    vrfy_(algo::DecrMT(&y)==0);

    {
        void *A = (void*)0x1234;
        void *B = (void*)0x5678;
        algo::SwapMT(A,B);
        vrfy_(A == (void*)0x5678);
        vrfy_(B == (void*)0x1234);
    }
}

// -----------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_ReadLine() {
    LineBuf acc;
    strptr str("abcde\n\n");
    memptr data=strptr_ToMemptr(str);
    strptr line;
    LinebufBegin(acc,data,false);
    vrfy_(LinebufNext(acc,line) && line == "abcde");
    vrfy_(LinebufNext(acc,line) && !elems_N(line));
    vrfy_(!LinebufNext(acc,line));

    str = strptr("fgh");
    data=strptr_ToMemptr(str);
    LinebufBegin(acc,data,false);
    vrfy_(!LinebufNext(acc,line));
    str = strptr("ijk\n");
    data=strptr_ToMemptr(str);
    LinebufBegin(acc,data,false);
    vrfy_(LinebufNext(acc,line) && line == "fghijk");

    str = strptr("abc\ndef\n");
    data=strptr_ToMemptr(str);
    LinebufBegin(acc,data,false);
    vrfy_(LinebufNext(acc,line) && line == "abc");
    vrfy_(LinebufNext(acc,line) && line == "def");

    str = strptr("abc\ndef");
    data=strptr_ToMemptr(str);
    LinebufBegin(acc,data,false);
    vrfy_(LinebufNext(acc,line) && line == "abc");
    vrfy_(!LinebufNext(acc,line));
    acc.implied_eof = true;
    vrfy_(LinebufNext(acc,line) && line == "def");

    str = strptr("abc\ndef");
    data=strptr_ToMemptr(str);
    LinebufBegin(acc,data,true);
    vrfy_(LinebufNext(acc,line) && line == "abc");
    vrfy_(LinebufNext(acc,line) && line == "def");

}

// -----------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_Ceiling() {
    vrfyeq_(CeilingLog2(u32(0)), 0);
    vrfyeq_(CeilingLog2(u32(1)), 0);
    vrfyeq_(CeilingLog2(u32(2)), 1);
    vrfyeq_(CeilingLog2(u32(4)), 2);
    vrfyeq_(CeilingLog2(u32(8)), 3);
    vrfyeq_(CeilingLog2(u32(16)), 4);
    vrfyeq_(CeilingLog2(u32(32)), 5);
    vrfyeq_(CeilingLog2(u32(64)), 6);

    //vrfyeq_(CeilingLog2(u32(0)), 0);
    vrfyeq_(CeilingLog2(u32(1)), 0);
    vrfyeq_(CeilingLog2(u32(3)), 2);
    vrfyeq_(CeilingLog2(u32(7)), 3);
    vrfyeq_(CeilingLog2(u32(15)), 4);
    vrfyeq_(CeilingLog2(u32(31)), 5);
    vrfyeq_(CeilingLog2(u32(63)), 6);

    vrfyeq_(u32_CeilPow2((u32)4 ,(u32)8), 8);
    vrfyeq_(u32_CeilPow2((u32)4, (u32)16), 16);
    vrfyeq_(u32_CeilPow2((u32)4, (u32)64), 64);

    vrfyeq_(u32_CeilPow2((u32)0 ,(u32)8), 0);
    vrfyeq_(u32_CeilPow2((u32)0, (u32)16), 0);
    vrfyeq_(u32_CeilPow2((u32)0, (u32)64), 0);

    vrfyeq_(u32_CeilPow2((u32)7  ,(u32)8), 8);
    vrfyeq_(u32_CeilPow2((u32)15, (u32)16), 16);
    vrfyeq_(u32_CeilPow2((u32)63, (u32)64), 64);
}


// -----------------------------------------------------------------------------
//
// GetCSVToken, GetCSVTokens
//
//
//
//

void atf_unit::unittest_algo_lib_CSVTokens() {
    {
        algo_lib::CsvParse parsecsv;
        parsecsv.input = "a, ,   'c', \"d\"    ,,";
        RunCsvParse(parsecsv);

        vrfyeq_(ary_tok_N(parsecsv),6);
        vrfy_(ary_tok_qFind(parsecsv, 0)=="a");
        vrfy_(ary_tok_qFind(parsecsv, 1)=="");
        vrfy_(ary_tok_qFind(parsecsv, 2)=="c");
        vrfy_(ary_tok_qFind(parsecsv, 3)=="d");
        vrfy_(ary_tok_qFind(parsecsv, 4)=="");
        vrfy_(ary_tok_qFind(parsecsv, 5)=="");
    }
    {
        algo_lib::CsvParse parsecsv;
        parsecsv.input = strptr("");
        RunCsvParse(parsecsv);

        vrfy_(ary_tok_N(parsecsv)==0);
    }

    {
        algo_lib::CsvParse parsecsv;
        parsecsv.input = "a,,b,";
        RunCsvParse(parsecsv);

        vrfyeq_(ary_tok_N(parsecsv),4);
        vrfyeq_(ary_tok_qFind(parsecsv, 0),"a");
        vrfyeq_(ary_tok_qFind(parsecsv, 1),"");
        vrfyeq_(ary_tok_qFind(parsecsv, 2),"b");
        vrfyeq_(ary_tok_qFind(parsecsv, 3),"");
    }

    {
        algo_lib::CsvParse parsecsv;
        parsecsv.input = "a,b\"c\",d'e'";
        RunCsvParse(parsecsv);

        vrfyeq_(ary_tok_N(parsecsv),3);
        vrfyeq_(ary_tok_qFind(parsecsv, 0),"a");
        vrfyeq_(ary_tok_qFind(parsecsv, 1),"b\"c\"");
        vrfyeq_(ary_tok_qFind(parsecsv, 2),"d'e'");
    }

    {
        algo_lib::CsvParse parsecsv;
        parsecsv.input = "a,b\"\"c\"\",d''e'',,, ";
        RunCsvParse(parsecsv);

        vrfyeq_(ary_tok_N(parsecsv),6);
        vrfyeq_(ary_tok_qFind(parsecsv, 0),"a");
        vrfyeq_(ary_tok_qFind(parsecsv, 1),"b\"\"c\"\"");
        vrfyeq_(ary_tok_qFind(parsecsv, 2),"d''e''");
        vrfyeq_(ary_tok_qFind(parsecsv, 3),"");
        vrfyeq_(ary_tok_qFind(parsecsv, 4),"");
        vrfyeq_(ary_tok_qFind(parsecsv, 5),"");
    }

    {   // from adrian
        algo_lib::CsvParse parsecsv;
        parsecsv.input = "IT0004105653,CONAFI PRESTITO',,19286,,,,,19286,,100.00%,,,,";
        RunCsvParse(parsecsv);

        vrfyeq_(ary_tok_N(parsecsv),15);
        vrfyeq_(ary_tok_qFind(parsecsv, 0),"IT0004105653");
        vrfyeq_(ary_tok_qFind(parsecsv, 1),"CONAFI PRESTITO'");
        vrfyeq_(ary_tok_qFind(parsecsv, 2),"");
        vrfyeq_(ary_tok_qFind(parsecsv, 3),"19286");
        vrfyeq_(ary_tok_qFind(parsecsv, 4),"");
        vrfyeq_(ary_tok_qFind(parsecsv, 5),"");
        vrfyeq_(ary_tok_qFind(parsecsv, 6),"");
        vrfyeq_(ary_tok_qFind(parsecsv, 7),"");
        vrfyeq_(ary_tok_qFind(parsecsv, 8),"19286");
        vrfyeq_(ary_tok_qFind(parsecsv, 9),"");
        vrfyeq_(ary_tok_qFind(parsecsv, 10),"100.00%");
        vrfyeq_(ary_tok_qFind(parsecsv, 11),"");
        vrfyeq_(ary_tok_qFind(parsecsv, 12),"");
        vrfyeq_(ary_tok_qFind(parsecsv, 13),"");
        vrfyeq_(ary_tok_qFind(parsecsv, 14),"");
    }
}

void atf_unit::unittest_algo_lib_Strfind() {
    strptr s("abcde");

    vrfyeq_(substr_FindFirst(s, strptr("a"))        ,i32_Range(0,1));
    vrfyeq_(substr_FindFirst(s, strptr(""))         ,i32_Range(0,0));
    vrfyeq_(substr_FindFirst(s, strptr("test"))     ,i32_Range(elems_N(s),elems_N(s)));
    vrfyeq_(substr_FindFirst(s, strptr("abcde"))    ,i32_Range(0,5));


    vrfyeq_(substr_FindLast(s, strptr("a"))     ,i32_Range(0,1));
    vrfyeq_(substr_FindLast(s, strptr(""))      ,i32_Range(5,5));
    vrfyeq_(substr_FindLast(s, strptr("test"))  ,i32_Range(0,0));
    vrfyeq_(substr_FindLast(s, strptr("abcde")) ,i32_Range(0,5));

    s = "b,c,\nd\n";

    s = "12345";
    vrfy_(TRevFind(s, '6') == i32_Range(0,0));
    vrfy_(TFind(s, '6') == i32_Range(elems_N(s),elems_N(s)));

    frep_(i,elems_N(s)) {
        vrfy_(TFind(s,s[i]) == i32_Range(i,i+1));
        vrfy_(TRevFind(s,s[i]) == i32_Range(i,i+1));
    }
}

// -----------------------------------------------------------------------------
// Print

template<class T> static void TestPrint(T x, strptr result) {
    cstring s;
    s<<x;
    vrfy_(s == result);
}
static void CheckPos(i64 N, strptr result) {
    TestPrint<i32>(N,result);
    TestPrint<i64>(N,result);
    if (N>0) {
        TestPrint<u32>(N,result);
        TestPrint<u64>(N,result);
    }
}

void atf_unit::unittest_algo_lib_PrintSsim() {
    CheckPos(0,"0");
    CheckPos(1,"1");
    CheckPos(1000000000,"1000000000");
    CheckPos(-1,"-1");
}

// -----------------------------------------------------------------------------
//
// Min, Max, Floor, Round, Ceiling, UpdateMin, UpdateMax
//
//
//
//

void atf_unit::unittest_algo_lib_MinMax() {
    vrfy_(be16toh(0xff00) == 0x00ff);
    vrfy_(be32toh(0xaabbccdd) == 0xddccbbaa);
    vrfy_(be64toh(0x1122334455667788) == 0x8877665544332211);
    vrfy_(be64toh(0x1122334455667788) == 0x8877665544332211);

    vrfy_(i32_Min(0,1)==0);
    vrfy_(i32_Max(0,1)==1);
    vrfy_(i32_Min(-1,1)==-1);
    vrfy_(i32_Max(-1,1)==1);
    vrfy_(i32_Min(i32(0x80000000),i32(0x7fffffff))==i32(0x80000000));
    vrfy_(i32_Max(i32(0x80000000),i32(0x7fffffff))==i32(0x7fffffff));

    vrfy_(i32_Max( 0x7fffffff,-0x7fffffff) ==  0x7fffffff);
    vrfy_(i32_Max( 0x7fffffff, 0x7fffffff) ==  0x7fffffff);
    vrfy_(i32_Max(-0x7fffffff,-0x7fffffff) == -0x7fffffff);
    vrfy_(i32_Max(-0x7fffffff, 0x7fffffff) ==  0x7fffffff);
    vrfy_(i32_Max( 10, 20) == 20);
    vrfy_(i32_Max(  0,  0) == 0);
    vrfy_(i32_Max(-10,-20) == -10);
    vrfy_(i32_Max(  1, -1) == 1);
    vrfy_(i32_Max( -1,  1) == 1);

    vrfy_(i64_Max( 0x7fffffff,-0x7fffffff) ==  0x7fffffff);
    vrfy_(i64_Max( 0x7fffffff, 0x7fffffff) ==  0x7fffffff);
    vrfy_(i64_Max(-0x7fffffff,-0x7fffffff) == -0x7fffffff);
    vrfy_(i64_Max(-0x7fffffff, 0x7fffffff) ==  0x7fffffff);
    vrfy_(i64_Max( 10, 20) == 20);
    vrfy_(i64_Max(  0,  0) == 0);
    vrfy_(i64_Max(-10,-20) == -10);
    vrfy_(i64_Max(  1, -1) == 1);
    vrfy_(i64_Max( -1,  1) == 1);

    vrfy_(i32_Min(0x7fffffff,-0x7fffffff) == -0x7fffffff);
    vrfy_(i32_Min(0x7fffffff,0x7fffffff) == 0x7fffffff);
    vrfy_(i32_Min(-0x7fffffff,-0x7fffffff) == -0x7fffffff);
    vrfy_(i32_Min(-0x7fffffff,0x7fffffff) == -0x7fffffff);
    vrfy_(i32_Min(10,20) == 10);
    vrfy_(i32_Min(0,0) == 0);
    vrfy_(i32_Min(-10,-20) == -20);
    vrfy_(i32_Min(1,-1) == -1);
    vrfy_(i32_Min(-1,1) == -1);

    vrfy_(i64_Min(0x7fffffff,-0x7fffffff) == -0x7fffffff);
    vrfy_(i64_Min(0x7fffffff,0x7fffffff) == 0x7fffffff);
    vrfy_(i64_Min(-0x7fffffff,-0x7fffffff) == -0x7fffffff);
    vrfy_(i64_Min(-0x7fffffff,0x7fffffff) == -0x7fffffff);
    vrfy_(i64_Min(10,20) == 10);
    vrfy_(i64_Min(0,0) == 0);
    vrfy_(i64_Min(-10,-20) == -20);
    vrfy_(i64_Min(1,-1) == -1);
    vrfy_(i64_Min(-1,1) == -1);

    vrfy_(u64_Min(0,ULLONG_MAX) == 0);
    vrfy_(u64_Min(2,ULLONG_MAX) == 2);
    vrfy_(u64_Max(0,ULLONG_MAX) == ULLONG_MAX);
    vrfy_(u64_Min(0,1) == 0);
    vrfy_(u64_Max(0,1) == 1);

    vrfy_(u32_Min(0,UINT_MAX) == 0);
    vrfy_(u32_Max(0,UINT_MAX) == UINT_MAX);
    vrfy_(u32_Min(0,1) == 0);
    vrfy_(u32_Max(0,1) == 1);

    int a=0;
    a = i32_Max(a,2);
    vrfy_(a==2);
    a = i32_Max(a,1);
    vrfy_(a==2);
    a = i32_Min(a,1);
    vrfy_(a==1);
    a = i32_Min(a,2);
    vrfy_(a==1);
}

void atf_unit::unittest_algo_lib_NToh() {
    vrfy_(be16toh(0xff00) == 0x00ff);
    vrfy_(be32toh(0xaabbccdd) == 0xddccbbaa);
    vrfy_(be64toh(0x1122334455667788LL) == 0x8877665544332211LL);
}

// quite simple and straightforward conversion function

static u64 test_atoi(const u8 *str, u32 len) {
    u64 res = 0;
    u64 mult = 1;
    for (; len; --len, mult *= 10) {
        res += (str[len-1] - '0') * mult;
    }
    //prlog(res);
    return res;
}

// for reverse string

static u64 test_atoi_be(const u8 *str, u32 len) {
    u64 res = 0;
    u64 mult = 1;
    u32 i;
    for (i=0; i<len; ++i, mult *= 10) {
        res += (str[i] - '0') * mult;
    }
    //    prlog(res);
    return res;
}

// Template function to facilitate all test cases

template <u32 N, typename argtype, typename rettype>
static void TestStrNumConv(
                           rettype (*func) (argtype arg, u32 &ok)
                           ,rettype (*afunc)(argtype arg, u32 len, u32 &ok)
                           ,rettype (*sfunc)(strptr  arg, u32 &ok)) {
    u8 buf[N];

    // nominal cases for one running digit on the background of all other digits are the same
    {
        u32 i;
        for (i='0';i<='9';++i) {            // background
            frep_(j,N) {                    // place
                memset(buf,i,N);
                int k;
                for (k='0';k<='9';++k) {    // digit
                    u32 ok;
                    buf[j] = u8(k);
                    if (func) {
                        ok=0;
                        argtype arg;
                        memcpy(&arg, buf, sizeof(arg));
                        vrfyeq_(test_atoi_be(buf,N),(*func)(arg,ok));
                        vrfyeq_(1,ok);
                    }
                    if (afunc || sfunc) {
                        u32 len;
                        for (len = 1; len < N; ++len) {
                            if (afunc) {
                                ok=0;
                                argtype arg;
                                memcpy(&arg, buf, sizeof(arg));
                                vrfyeq_(test_atoi_be(buf+N-len,len),(*afunc)(arg,len,ok));
                                vrfyeq_(1,ok);
                            }
                            if (sfunc) {
                                ok=0;
                                vrfyeq_(test_atoi(buf,len),(*sfunc)(strptr((char*)buf,len),ok));
                                vrfyeq_(1,ok);
                            }
                        }
                    }
                }
            }
        }
    }

    // error cases for one running char on the background of all other chars are the same
    {
        frep_(i,255) {                      // background
            frep_(j,N) {                    // place
                memset(buf,i,N);
                frep_(k,256) {              // char
                    bool i_ok = (i >= '0' && i <= '9');
                    bool k_ok = (k >= '0' && k <= '9');
                    bool both_nok = !i_ok && !k_ok;
                    if (i_ok && k_ok) { continue; }
                    u32 ok;
                    buf[j] = u8(k);
                    if (func) {
                        memset(&ok,0xff,sizeof(ok));
                        argtype arg;
                        memcpy(&arg, buf, sizeof(arg));
                        (*func)(arg,ok);
                        vrfyeq_(0,ok);
                    }
                    if (afunc || sfunc) {
                        u32 len;
                        for (len = 0; len < N; ++len) {
                            if (afunc && ((len > u32(N-j)) || both_nok)) {
                                memset(&ok,0xff,sizeof(ok));
                                argtype arg;
                                memcpy(&arg, buf, sizeof(arg));
                                (*afunc)(arg,len,ok);
                                vrfyeq_(0,ok);
                            }
                            if (sfunc && ((len > u32(j+1)) || both_nok)) {
                                memset(&ok,0xff,sizeof(ok));
                                (*sfunc)(strptr((char*)buf,len),ok);
                                vrfyeq_(0,ok);
                            }
                        }
                    }
                }
            }
        }
    }

    // it looks like it's enough..
}

// -----------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_ParseNum() {
    TestStrNumConv<8,u64,u32>(ParseNum8,0,0);
    TestStrNumConv<4,u32,u32>(ParseNum4,0,0);
    TestStrNumConv<8,u64,u32>(0,ParseNum8,0);
    TestStrNumConv<4,u32,u32>(0,ParseNum4,0);
    TestStrNumConv<8,u64,u32>(0,0,aParseNum8);
    TestStrNumConv<4,u32,u32>(0,0,aParseNum4);
    TestStrNumConv<16,int,u64>(0,0,aParseNum16); // 2nd arg is fake
}

// -----------------------------------------------------------------------------
//
// ParseNum4 / ParseNum8 / ParseNum16 / aParseNum8 / aParseNum16
//
//
//

static void TestNumber(u64 n) {
    tempstr s;
    s<<n;
    u32 ok;
    if (ch_N(s) <= 8) {
        vrfyeq_(aParseNum8(s, ok), n);
        vrfyeq_(ok, true);
    } else {
        aParseNum8(s, ok);
        vrfyeq_(ok, false);
    }
    if (ch_N(s) <= 4) {
        vrfyeq_(aParseNum4(s, ok), n);
        vrfyeq_(ok, true);
    } else {
        aParseNum4(s, ok);
        vrfyeq_(ok, false);
    }
    if (ch_N(s) <= 16) {
        vrfyeq_(aParseNum16(s, ok), n);
        vrfyeq_(ok, true);
    } else {
        aParseNum16(s, ok);
        vrfyeq_(ok, false);
    }
    {
        char c = char(i32_WeakRandom(256));
        if (c>='0' && c<='9') {
            c += 10;
        }
        tempstr t;
        t=s;
        ch_qFind(t, i32_WeakRandom(ch_N(s))) = c;
        if (ch_N(t) <= 16) {
            aParseNum16(t, ok);
            vrfyeq_(ok, false);
        }
        if (ch_N(t) <= 8) {
            aParseNum8(t, ok);
            vrfyeq_(ok, false);
        }
    }
}

void atf_unit::unittest_algo_lib_ParseNumber_Empty1() {
    u32 ok=1;
    aParseNum8(strptr("ParseNumber_"), ok);
    vrfyeq_(ok, false);
}

void atf_unit::unittest_algo_lib_ParseNumber_Empty2() {
    u32 ok=1;
    aParseNum16(strptr("ParseNumber_"), ok);
    vrfyeq_(ok, false);
}

void atf_unit::unittest_algo_lib_ParseNumber_Empty3() {
    u32 ok=1;
    ParseNum8(0, 0, ok);
    vrfyeq_(ok, false);
}

void atf_unit::unittest_algo_lib_ParseNumber_Empty4() {
    u32 ok=1;
    ParseNum4(0, 0, ok);
    vrfyeq_(ok, false);
}

void atf_unit::unittest_algo_lib_ParseNumber_Overflow1() {
    algo::LnumStr7_U32_Base36 x;
    ch_SetStrptr(x, "1234XYZ");
    u32 val = ch_GetnumDflt(x,0);
    vrfyeq_(val, 2302984187);

    ch_SetStrptr(x, "XYZ1234");
    val = ch_GetnumDflt(x,0);
    vrfyeq_(val,u32(0));
}

void atf_unit::unittest_algo_lib_ParseNumber_Overflow2() {
    algo::LnumStr20_U64 x;
    ch_SetStrptr(x, "1234567891234567890");
    u64 val = ch_GetnumDflt(x,0);
    vrfyeq_(val, u64(1234567891234567890));

    ch_SetStrptr(x, "98765432109876543210");
    val = ch_GetnumDflt(x,0);
    vrfyeq_(val, u64(0));
}

void atf_unit::unittest_algo_lib_ParseNumber_Overflow3() {
    algo::LnumStr22_U64 x;
    ch_SetStrptr(x, "1234567891234567890");
    u64 val = ch_GetnumDflt(x,0);
    vrfyeq_(val, u64(1234567891234567890));

    ch_SetStrptr(x, "98765432109876543210");
    val = ch_GetnumDflt(x,0);
    vrfyeq_(val, u64(0));
}

void atf_unit::unittest_algo_lib_ParseNumber() {
    frep_(i,10) {
        TestNumber(i);
    }

    for (u64 x=1; x<=1000000000000000000; x*=10) {
        TestNumber(x);
    }

    rep_(i,64) {
        TestNumber(u64(1)<<i);
    }

    rep_(i,10000) {
        TestNumber(i32_WeakRandom(INT_MAX) & ((1<<i32_WeakRandom(32))-1));
    }
}

static void TestString(strptr before, strptr printed) {
    tempstr in;
    tempstr out;
    out << MaybeSpace; PrintAttr(out, "", before);
    vrfy_(out == printed);
    StringIter iter(out);
    cstring_ReadCmdarg(in, iter, false);
}

void atf_unit::unittest_algo_lib_PrintDoubleWithCommas1() {
    tempstr t;
    double_PrintWithCommas(0.1, t, 1);
    vrfyeq_(t, "0.1");
}

void atf_unit::unittest_algo_lib_PrintDoubleWithCommas2() {
    tempstr t;
    double_PrintWithCommas(0, t, 0);
    vrfyeq_(t, "0");
}

void atf_unit::unittest_algo_lib_PrintDoubleWithCommas3() {
    tempstr t;
    double_PrintWithCommas(12, t, 5);
    vrfyeq_(t, "12");
}

void atf_unit::unittest_algo_lib_PrintDoubleWithCommas4() {
    tempstr t;
    double_PrintWithCommas(101, t, 5);
    vrfyeq_(t, "101");
}

void atf_unit::unittest_algo_lib_PrintDoubleWithCommas5() {
    tempstr t;
    double_PrintWithCommas(1001, t, 5);
    vrfyeq_(t, "1,001");
}

void atf_unit::unittest_algo_lib_PrintDoubleWithCommas6() {
    tempstr t;
    double_PrintWithCommas(10001, t, 5);
    vrfyeq_(t, "10,001");
}

void atf_unit::unittest_algo_lib_PrintDoubleWithCommas7() {
    tempstr t;
    double_PrintWithCommas(100001, t, 5);
    vrfyeq_(t, "100,001");
}

void atf_unit::unittest_algo_lib_PrintDoubleWithCommas8() {
    tempstr t;
    double_PrintWithCommas(12341234, t, 5);
    vrfyeq_(t, "12,341,234");
}

// -----------------------------------------------------------------------------

static void CheckCppStr(strptr orig, strptr printed, char quote_char) {
    cstring s;
    strptr_PrintCppQuoted(orig,s,quote_char);
    vrfyeq_(s,printed);
}

void atf_unit::unittest_algo_lib_PrintCppQuoted() {
    CheckCppStr("", "\"\"", '"');// empty string, double quote
    CheckCppStr("", "''", '\'');// empty string, single quote
    CheckCppStr("", "!!", '!');// empyy string, bang quote
    CheckCppStr("!", "!\\!!", '!');// bang quote escape
    CheckCppStr(".", "\".\"", '"');// just a dot
    CheckCppStr("\n", "\"\\n\"", '"');// newline
    CheckCppStr("\r", "\"\\r\"", '"');// carriage return
    CheckCppStr("\t", "\"\\t\"", '"');// tab
    CheckCppStr("\"", "\"\\\"\"", '"');// backslash
    CheckCppStr("11111111111111111111111111111111111111111111111111111111111 "
                "2222222222222222222222222222222222222222222 "
                "33333333333333333333333333333333333333"
                ,"\"11111111111111111111111111111111111111111111111111111111111 "
                "2222222222222222222222222222222222222222222 "
                "33333333333333333333333333333333333333\"", '"');// longish string
}

// -----------------------------------------------------------------------------

static void TestPrintPadLeft(i64 value, int nplace, strptr result) {
    cstring str;
    i64_PrintPadLeft(value,str, nplace);
    vrfyeq_(str, result);
}

static void TestPrintPadRight(strptr in, int nplace, char padchar, strptr result) {
    cstring str;
    strptr_PrintPadRight(in, str, nplace, padchar);
    vrfyeq_(str, result);
}

static void TestPrintPadLeft(strptr in, int nplace, char padchar, strptr result) {
    cstring str;
    strptr_PrintPadLeft(in, str, nplace, padchar);
    vrfyeq_(str, result);
}

void atf_unit::unittest_algo_lib_PrintPad() {
    TestPrintPadLeft(10, 3, "010");
    TestPrintPadLeft(412, 3, "412");
    TestPrintPadLeft(2222, 3, "2222");
    TestPrintPadRight("test",  10, ' ', "test      ");
    TestPrintPadRight("test",  1, ' ', "test");
    TestPrintPadLeft("test", 1, ' ', "test");
    TestPrintPadLeft("test", 10, ' ', "      test");
}

// -----------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_PrintHex() {
    cstring str;
    u64_PrintHex(0xffffffffffffffffULL,str, 16,true,false);
    vrfyeq_(str,"0xffffffffffffffff");
}

void atf_unit::unittest_algo_lib_TestString() {
    // test other strings -- non-printable characteres, apostrophe, etc.
    TestString(""         , "\"\""           );
    TestString("abcde"    , "abcde"          );
    TestString("ab'cde"   , "\"ab'cde\""     );
    TestString("ab\"cde"  , "'ab\"cde'"      );
    TestString("ab\" cde" , "'ab\" cde'"     );
    TestString("ab\"' cde", "\"ab\\\"' cde\"");
}

void atf_unit::unittest_algo_lib_TestStringFmt() {
    cstring s;
    u128 val = u128(1) * 1000 * 1000 * 1000 * 1000; // 12 zeros
    val = val * val;//24 zeros
    u128_Print(val,s);
    vrfyeq_(s, "1000000000000000000000000");
}

void atf_unit::unittest_algo_lib_TestStringFmt2() {
    cstring s;
    u128 val = 0;
    u128_Print(val,s);
    vrfyeq_(s, "0");
}

void atf_unit::unittest_algo_lib_TestStringFmt3() {
    cstring s;
    // gcc doesn't support 128-bit constants
    u128 val = u128(12345678901234567890ULL) * 1000000000000000000ULL;
    u128_Print(val,s);
    vrfyeq_(s, "12345678901234567890000000000000000000");
}

//*****------------------------------------------------ Oct 8 1999 ------*****

static bool RegxMatch(strptr expr, strptr str) {
    algo_lib::Regx regx;
    Regx_ReadDflt(regx,expr);
    return Regx_Match(regx,str);
}
static bool RegxMatchShell(strptr expr, strptr str) {
    algo_lib::Regx regx;
    Regx_ReadShell(regx,expr,true);
    return Regx_Match(regx,str);
}
static bool RegxMatchSql(strptr expr, strptr str) {
    algo_lib::Regx regx;
    Regx_ReadSql(regx,expr,true);
    return Regx_Match(regx,str);
}

// --------------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_Regx() {
    vrfyeq_(RegxMatch("", ""), true);// empty regx matches empty string
    vrfyeq_(RegxMatch("", "x"), false);// empty regx matches empty string only
    vrfyeq_(RegxMatch(".*", "abcd")                     , true);
    vrfyeq_(RegxMatch("a.*", "abcd")                     , true);
    vrfyeq_(RegxMatch("ab.*", "abcd")                     , true);
    vrfyeq_(RegxMatch("abc.*", "abcd")                     , true);
    vrfyeq_(RegxMatch("abcd.*", "abcd")                     , true);
    vrfyeq_(RegxMatch("abcd.*", "abcde")                     , true);
    vrfyeq_(RegxMatch(".+", "abcd")                     , true);
    vrfyeq_(RegxMatch(".+", ""    )                     , false);
    vrfyeq_(RegxMatch("ab+c", "abc")                    , true);
    vrfyeq_(RegxMatch("ab+c", "abbbbbbbbbbbbbbbbbbbbc") , true);
    vrfyeq_(RegxMatch("abcd", "abcd")                   , true);
    vrfyeq_(RegxMatch("a(b|c)d", "acd")                 , true);
    vrfyeq_(RegxMatch("a(b|c)d", "abd")                 , true);
    vrfyeq_(RegxMatch("abcd", "abc")                    , false);
    vrfyeq_(RegxMatch(".*", "")                 , true);
    vrfyeq_(RegxMatch(".*", ".*")                 , true);
    vrfyeq_(RegxMatch(".*", "\x80\xff")                 , true);
    vrfyeq_(RegxMatch("\x82\x83", "\x82\x83")           , true);
    vrfyeq_(RegxMatch("\x82", "\x81")                   , false);
    vrfyeq_(RegxMatch("[\x81-\x85]+", "\x82\x83")       , true);
    vrfyeq_(RegxMatch("[\x81-\x85]", "\x80")            , false);

    vrfyeq_(RegxMatch("(", ""), true);// bad regx -- but must match
    vrfyeq_(RegxMatch("(((", ""), true);// bad regx -- but must match
    vrfyeq_(RegxMatch(")", ")"), true);// bad regx -- but must match
    vrfyeq_(RegxMatch("[", "["), true);// bad regx -- but must match
    vrfyeq_(RegxMatch("]", "]"), true);// bad regx -- but must match
    vrfyeq_(RegxMatch("]](", "]]"), true);// bad regx -- but must match
    vrfyeq_(RegxMatch("[a-b", "[a-b"), true);// bad regx -- but must match
    vrfyeq_(RegxMatch("", ""), true);// bad regx -- but must match

    vrfyeq_(RegxMatchShell("*makefile*", "temp_makefile"), true);
    vrfyeq_(RegxMatchShell("*makefile*", "makefile"), true);
    vrfyeq_(RegxMatchShell("*make.ile", "makefile"), false);// should not match -- . is not special
    vrfyeq_(RegxMatchShell("%", "makefile"), false);// should not match -- . is not special

    vrfyeq_(RegxMatchSql("", ""), true);// empty regx matches empty string
    vrfyeq_(RegxMatchSql("", "x"), false);// empty regx matches empty string only
    vrfyeq_(RegxMatchSql("_", "a"), true);// any char
    vrfyeq_(RegxMatchSql("\\_", "a"), false);// escaped underscore -> real char
    vrfyeq_(RegxMatchSql("%", ""), true);
    vrfyeq_(RegxMatchSql("%", "a"), true);
    vrfyeq_(RegxMatchSql("%%%%%%", "a"), true);
    vrfyeq_(RegxMatchSql("asdf%f", "asdfasdfasdf"), true);// wildcard
    vrfyeq_(RegxMatchSql("asdf\\%f", "asdfasdfasdf"), false);// escaped wildcard
    vrfyeq_(RegxMatchSql("asdf\\%f", "asdf%f"), true);// should be ok
    vrfyeq_(RegxMatchSql("(a|b)", "a"), true);// should be ok
    vrfyeq_(RegxMatchSql("(a|b)", "b"), true);// should be ok
    vrfyeq_(RegxMatchSql("(a|b)", "c"), false);// should be ok
    vrfyeq_(RegxMatchSql("(a|b)(d|e)", "ae"), true);// should be ok
    vrfyeq_(RegxMatchSql("(a|b)(d|e)", "bd"), true);// should be ok
    vrfyeq_(RegxMatchSql("(a|b)(d|e)", "bd"), true);// should be ok
}

// -----------------------------------------------------------------------------

static void ShortCircuitMatch(strptr regx_str, strptr str, int njunk, strptr junkstr, bool expect, int maxcycles) {
    algo_lib::Regx regx;
    Regx_ReadDflt(regx,regx_str);
    // a megabyte of junk
    cstring test(str);
    for (int i=0; i<njunk; i++) {
        test << junkstr;
    }
    bool good =false;
    for (int iter=0; iter<100; iter++) {
        u64 start=get_cycles();
        bool result=Regx_Match(regx,test);
        u64 end=get_cycles();
        TESTCMP(result,expect);
        if (start + maxcycles > end) {
            good=true;
            break;
        }
    }
    TESTCMP(good,true);
}

// --------------------------------------------------------------------------------

// Test that matching a huge string with a regex that
// ends in .* is fast.
void atf_unit::unittest_algo_lib_RegxShortCircuit() {
    ShortCircuitMatch("abcd.*", "abcde", 1000000, "x", true, 10000);
    ShortCircuitMatch("abcd.*.*", "abcdef", 1000000, "x", true, 10000);
    ShortCircuitMatch(".*", "abcde", 1000000, "x", true, 10000);
    ShortCircuitMatch("", "abcde", 1000000, "x", false, 10000);// must quickly NOT match this
}

// -----------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_Smallstr() {
    {
        algo::RspaceStr4 a;
        algo::RspaceStr4 b;
        a=algo::RspaceStr4(strptr("A"));
        b=algo::RspaceStr4(strptr("A"));
        vrfy_(algo::ch_N(a) == 1);
        vrfy_(algo::ch_N(b) == 1);
        vrfy_(strptr(a)==strptr(b));
        b=algo::RspaceStr4(strptr("A "));
        vrfy_(strptr(a)==strptr(b));
        vrfy_(algo::ch_N(strptr(a)) == 1);
        vrfy_(strptr(a)==strptr(b));
    }
    {
        LspaceStr4 a;
        LspaceStr4 b;
        a=LspaceStr4(strptr("A"));
        b=LspaceStr4(strptr("A"));
        vrfy_(ch_N(a) == 1);
        vrfy_(ch_N(b) == 1);
        vrfy_(strptr(a)==strptr(b));
        b=LspaceStr4(strptr(" A"));
        vrfy_(ch_N(b) == 1);
        vrfy_(strptr(a)==strptr(b));
        vrfy_(ch_N(strptr(a)) == 1);
        vrfy_(strptr(a)==strptr(b));
    }

    vrfy_(algo::Smallstr10("ABCD") == "ABCD");

    {
        algo::RspaceStr4 a;
        algo::RspaceStr4 b;
        a=algo::RspaceStr4(strptr("A"));
        b=algo::RspaceStr4(strptr("A"));
        b=algo::RspaceStr4(strptr("A "));
        vrfy_(algo::ch_N(strptr(a)) == 1);
        vrfy_(strptr(a)==strptr(b));
    }
}

// -----------------------------------------------------------------------------

static void CheckWordIter(strptr a, strptr b) {// Rewrite string using Word_curs
    tempstr out;
    ind_beg(Word_curs,word,a) {
        out<<" "<<word;
    }ind_end;
    vrfyeq_(out,b);
}

// -----------------------------------------------------------------------------

static void CheckWordcharf(strptr a, strptr b) {// Rewrite string using GetWordCharf
    tempstr out;
    StringIter iter(a);
    while (true) {
        strptr word = GetWordCharf(iter);
        if (word != "") {
            out << " "<< word;
        } else {
            break;
        }
    }
    vrfyeq_(out,b);
}

static void TestParseDouble(strptr str, bool ok, double result) {
    StringIter iter(str);
    double d;
    vrfy_(TryParseDouble(iter, d)==ok && (!ok || d==result));
}

static void TestParseDigits(strptr str, bool ok, double result) {
    StringIter iter(str);
    double d;
    vrfy(TryParseDigits(iter, d)==ok && (!ok || d==result), tempstr()<<str<<" "<<ok<<" "<<result);
}

static void TestParseI64(strptr str, bool ok, i64 result) {
    StringIter iter(str);
    i64 d;
    vrfy_(TryParseI64(iter, d)==ok && (!ok || d==result));
}

static void TestParseI32(strptr str, bool ok, i32 result) {
    StringIter iter(str);
    i32 val;
    vrfy_(TryParseI32(iter,val)==ok && (!ok || val==result));
}

static void TestGetLine() {
    StringIter iter;
    iter = StringIter("\nxx\n\n");
    vrfy_(GetLine(iter) == strptr());
    iter = StringIter("\nxx\n\n");
    GetLine(iter);
    vrfy_(GetLine(iter) == "xx");
    iter =StringIter("xyz\r\n\n");
    vrfy_(GetLine(iter) == "xyz");
}

void atf_unit::unittest_algo_lib_StringIter() {
    TestParseDouble("1", true, 1);
    TestParseDouble("+1", true, 1);
    TestParseDouble("-1", true, -1);
    TestParseDouble(".1", true, .1);
    TestParseDouble("+.1", true, .1);
    TestParseDouble("-.1", true, -.1);
    TestParseDouble("+.1e5", true, 1e4);
    TestParseDouble("1.1", true, 1.1);
    TestParseDouble("1.11", true, 1.11);
    TestParseDouble("11.11", true, 11.11);
    TestParseDouble("111.11", true, 111.11);
    TestParseDouble("111.111", true, 111.111);
    TestParseDigits("502394875029348", true, 502394875029348LL);
    TestParseDouble("502394875029348", true, 502394875029348LL);
    TestParseDouble(".502394875029348", true, .502394875029348);
    TestParseDigits("12345", true, 12345);
    TestParseDouble("12345", true, 12345);
    TestParseDigits("", false, 0);
    TestParseDouble("", false, 0);
    TestParseDigits("y", false, 0);
    TestParseDouble("y", false, 0);
    TestParseDigits(".", false, 0);
    TestParseDouble(".", false, 0);
    TestParseDigits(".e", false, 0);
    TestParseDouble(".e", false, 0);
    TestParseDouble("1e", false, 0);
    TestParseDouble("1e+", false, 0);
    TestParseI64("123456789123456789", true, 123456789123456789LL);
    TestParseI64("123456789123456789123456776545", true, LLONG_MAX);
    TestParseI32("  0234 ", true, 234);
    TestParseI32("  234", true, 234);
    TestParseI32("\r\n\t -12222", true, -12222);
    TestParseI32("3333333333333333333", true, INT_MAX);
    TestParseI32("-3333333333333333333", true, -INT_MAX);
    TestParseI32("2147483647", true, INT_MAX);
    TestParseI32("2147483648", true, INT_MAX);
    TestParseI32("2147483649", true, INT_MAX);
    TestParseI32("3147483648", true, INT_MAX);
    TestParseI32("4147483649", true, INT_MAX);
    TestParseI32("12147483649", true, INT_MAX);
    TestParseI32("0x10", true, 16);
    TestParseI32("0x10000", true, 65536);
    TestParseI32("0x", false, 0);
    TestParseI32("xxx", false, 0);
    TestParseI32("0", true, 0);
    TestParseI32("1", true, 1);
    TestParseI32("-1", true, -1);
    TestParseI32("123x", true, 123);

    TestGetLine();

    CheckWordcharf("abc def", " abc def");
    CheckWordcharf("   abc  def ", " abc def");
    CheckWordcharf("", "");
    CheckWordcharf("a", " a");
    CheckWordcharf("aaa bbb d ", " aaa bbb d");
    CheckWordcharf("   ", "");
    CheckWordIter("   abc  def ", " abc def");
    CheckWordIter("", "");
    CheckWordIter("a", " a");
    CheckWordIter("aaa bbb d ", " aaa bbb d");
    CheckWordIter("   ", "");
}

// -----------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_test_strptr() {
    vrfy_( StartsWithQ(strptr("testabab"), "test"));
    vrfy_(!StartsWithQ(strptr("testabab"), "testababc"));
    vrfy_( StartsWithQ(strptr("testabab"), "testabab"));

    vrfy_( StartsWithQ(strptr("testabab"), "test",false));
    vrfy_(!StartsWithQ(strptr("testabab"), "testAbAbc",false));
    vrfy_( StartsWithQ(strptr("testabab"), "testabAb",false));

    vrfy_( EndsWithQ(strptr("ababtest"), "test"));
    vrfy_(!EndsWithQ(strptr("ababtest"), "testx"));
    vrfy_( EndsWithQ(strptr("ababtest"), "ababtest"));

    union { char c[5]; u32 i;} c1234 ={"1234"};
    char *c = c1234.c;
    vrfy_(c1234.i == __builtin_bswap32(u32(MULTICHAR_CONST4('1','2','3','4'))));
    vrfy_((c[0]<<24|c[1]<<16|c[2]<<8|c[3]) == MULTICHAR_CONST4('1','2','3','4'));

    vrfy_(strptr("abc") == strptr("abc"));
    vrfy_(!(strptr("abc") == strptr("abd")));
    vrfy_(!(strptr("abc") == strptr("abcd")));

    vrfy_(strptr("abc") != strptr("abd"));
    vrfy_(strptr("abc") != strptr("abcd"));

    vrfy_(strptr("ab") < strptr("abc"));
    vrfy_(strptr("aba") < strptr("abc"));

    vrfy_(strptr("abc") == "abc");
    vrfy_(!(strptr("abc") == "abcd"));
    vrfy_(!(strptr("abc") == "ab"));

    vrfy_(strptr("abc") != "abb");

    vrfy_(CompareNoCase(strptr("abc"), "aBc")==0);
    vrfy_(CompareNoCase(strptr("abc"), "Abd")<0);
    vrfy_(CompareNoCase(strptr("abc"), "aBB")>0);

    vrfy_(CompareNoCase(strptr("abc"), "ab")>0);
    vrfy_(CompareNoCase(strptr("ab"), "abc")<0);

    vrfy_(FindStr(strptr("/Default"), strptr("/")) == 0);

    vrfy_( StrEqual(strptr("abc"), "aBc" ,false));
    vrfy_(!StrEqual(strptr("abc"), "Abd" ,false));
    vrfy_(!StrEqual(strptr("abc"), "abCd",false));

    vrfy_(CompareNoCase("a","b")<0);
    vrfy_(CompareNoCase("a","A")==0);
    vrfy_(CompareNoCase("a","aa")<0);
    vrfy_(CompareNoCase("a","AA")<0);

    strptr s("abc");
    s = RestFrom(s, 1);
    vrfy_(s == "bc");
    s = RestFrom(s, 2);
    vrfy_(s == "");
}


// The reasons why not to use library calls to setup fixtures -
// 1) Test condition should be "clean" as much as possible and
// 2) library under test may interfere test results
void atf_unit::unittest_algo_lib_ParseOct1() {
    // be exhaustive ... (this will catch "shift" error)
    frep_(i,255) {
        u8 res  = 0xff;
        // .. and straightforward (this will catch "encoding" error)
        switch (i) {
        case '0': vrfyeq_(ParseOct1(i,res),1); vrfyeq_(res,0); break;
        case '1': vrfyeq_(ParseOct1(i,res),1); vrfyeq_(res,1); break;
        case '2': vrfyeq_(ParseOct1(i,res),1); vrfyeq_(res,2); break;
        case '3': vrfyeq_(ParseOct1(i,res),1); vrfyeq_(res,3); break;
        case '4': vrfyeq_(ParseOct1(i,res),1); vrfyeq_(res,4); break;
        case '5': vrfyeq_(ParseOct1(i,res),1); vrfyeq_(res,5); break;
        case '6': vrfyeq_(ParseOct1(i,res),1); vrfyeq_(res,6); break;
        case '7': vrfyeq_(ParseOct1(i,res),1); vrfyeq_(res,7); break;
        default : vrfyeq_(ParseOct1(i,res),0);                 break;
        }
    }
}

void atf_unit::unittest_algo_lib_ParseHex1() {
    // be exhaustive ... (this will catch "shift" error)
    frep_(i,256) {
        u8 res  = 0xff;
        // .. and straightforward (this will catch "encoding" error)
        switch (i) {
        case '0': vrfyeq_(ParseHex1(i,res),1); vrfyeq_(res,0);  break;
        case '1': vrfyeq_(ParseHex1(i,res),1); vrfyeq_(res,1);  break;
        case '2': vrfyeq_(ParseHex1(i,res),1); vrfyeq_(res,2);  break;
        case '3': vrfyeq_(ParseHex1(i,res),1); vrfyeq_(res,3);  break;
        case '4': vrfyeq_(ParseHex1(i,res),1); vrfyeq_(res,4);  break;
        case '5': vrfyeq_(ParseHex1(i,res),1); vrfyeq_(res,5);  break;
        case '6': vrfyeq_(ParseHex1(i,res),1); vrfyeq_(res,6);  break;
        case '7': vrfyeq_(ParseHex1(i,res),1); vrfyeq_(res,7);  break;
        case '8': vrfyeq_(ParseHex1(i,res),1); vrfyeq_(res,8);  break;
        case '9': vrfyeq_(ParseHex1(i,res),1); vrfyeq_(res,9);  break;
        case 'a':
        case 'A': vrfyeq_(ParseHex1(i,res),1); vrfyeq_(res,10); break;
        case 'b':
        case 'B': vrfyeq_(ParseHex1(i,res),1); vrfyeq_(res,11); break;
        case 'c':
        case 'C': vrfyeq_(ParseHex1(i,res),1); vrfyeq_(res,12); break;
        case 'd':
        case 'D': vrfyeq_(ParseHex1(i,res),1); vrfyeq_(res,13); break;
        case 'e':
        case 'E': vrfyeq_(ParseHex1(i,res),1); vrfyeq_(res,14); break;
        case 'f':
        case 'F': vrfyeq_(ParseHex1(i,res),1); vrfyeq_(res,15); break;
        default : vrfyeq_(ParseHex1(i,res),0);
        }
    }
}

// -----------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_ParseOct3() {
    u8 res;

    // nominal cases for 1..3 digits

    res = 0xff;
    vrfyeq_(1,ParseOct3(*(const u32 *)("0000"),1,res));
    vrfyeq_(0,res);

    res = 0xff;
    vrfyeq_(1,ParseOct3(*(const u32 *)("0\0\0\0"),4,res));
    vrfyeq_(0,res);

    res = 0xff;
    vrfyeq_(2,ParseOct3(*(const u32 *)("0000"),2,res));
    vrfyeq_(0,res);

    res = 0xff;
    vrfyeq_(2,ParseOct3(*(const u32 *)("00\0\0"),4,res));
    vrfyeq_(0,res);

    res = 0xff;
    vrfyeq_(3,ParseOct3(*(const u32 *)("0000"),3,res));
    vrfyeq_(0,res);

    res = 0xff;
    vrfyeq_(3,ParseOct3(*(const u32 *)("000\0"),4,res));
    vrfyeq_(0,res);

    // check that 4th digit is not taken

    res = 0xff;
    vrfyeq_(3,ParseOct3(*(const u32 *)("0001"),4,res));
    vrfyeq_(0,res);

    // verify range

    res = 0;
    vrfyeq_(3,ParseOct3(*(const u32 *)("3770"),4,res));
    vrfyeq_(0377,res);

    // verify order

    res = 0;
    vrfyeq_(1,ParseOct3(*(const u32 *)("1\0\0\0"),4,res));
    vrfyeq_(1,res);

    res = 0;
    vrfyeq_(2,ParseOct3(*(const u32 *)("13\0\0"),4,res));
    vrfyeq_(013,res);

    res = 0;
    vrfyeq_(3,ParseOct3(*(const u32 *)("135\0\0"),4,res));
    vrfyeq_(0135,res);

    res = 0;
    vrfyeq_(3,ParseOct3(*(const u32 *)("1350"),4,res));
    vrfyeq_(0135,res);

    // verify hole

    res = 0;
    vrfyeq_(1,ParseOct3(*(const u32 *)("1\350"),4,res));
    vrfyeq_(1,res);

    vrfyeq_(0,ParseOct3(*(const u32 *)("\1350"),4,res));

    // verify non-octal digit

    vrfyeq_(0,ParseOct3(*(const u32 *)("8000"),4,res));
    vrfyeq_(1,ParseOct3(*(const u32 *)("0800"),4,res));
    vrfyeq_(2,ParseOct3(*(const u32 *)("0080"),4,res));
    vrfyeq_(3,ParseOct3(*(const u32 *)("0008"),4,res));
}


void atf_unit::unittest_algo_lib_ParseHex2() {
    u8 res;

    // nominal cases for 1..2 digits

    res = 0xff;
    vrfyeq_(1,ParseHex2(*(const u32 *)("0000"),1,res));
    vrfyeq_(0,res);

    res = 0xff;
    vrfyeq_(1,ParseHex2(*(const u32 *)("0\0\0\0"),4,res));
    vrfyeq_(0,res);

    res = 0xff;
    vrfyeq_(2,ParseHex2(*(const u32 *)("0000"),2,res));
    vrfyeq_(0,res);

    res = 0xff;
    vrfyeq_(2,ParseHex2(*(const u32 *)("00\0\0"),4,res));
    vrfyeq_(0,res);

    // check that 3rd and 4th digits are not taken

    res = 0xff;
    vrfyeq_(2,ParseHex2(*(const u32 *)("001\0"),4,res));
    vrfyeq_(0,res);

    res = 0xff;
    vrfyeq_(2,ParseHex2(*(const u32 *)("0012"),4,res));
    vrfyeq_(0,res);

    // verify range

    res = 0;
    vrfyeq_(2,ParseHex2(*(const u32 *)("fF\0\0"),4,res));
    vrfyeq_(0xff,res);

    // some interesting nominal cases

    res = 0;
    vrfyeq_(2,ParseHex2(*(const u32 *)("9A\0\0"),4,res));
    vrfyeq_(0x9a,res);

    res = 0;
    vrfyeq_(2,ParseHex2(*(const u32 *)("a1\0\0"),4,res));
    vrfyeq_(0xa1,res);


    // verify order

    res = 0;
    vrfyeq_(1,ParseHex2(*(const u32 *)("1\0\0\0"),4,res));
    vrfyeq_(1,res);

    res = 0;
    vrfyeq_(2,ParseHex2(*(const u32 *)("13\0\0"),4,res));
    vrfyeq_(0x13,res);

    // verify hole

    vrfyeq_(0,ParseHex2(*(const u32 *)("\13\0\0"),4,res));

    // verify non-hexadecimal

    vrfyeq_(0,ParseOct3(*(const u32 *)("/000"),4,res));
    vrfyeq_(0,ParseOct3(*(const u32 *)(":000"),4,res));
    vrfyeq_(0,ParseOct3(*(const u32 *)("@000"),4,res));
    vrfyeq_(0,ParseOct3(*(const u32 *)("G000"),4,res));
    vrfyeq_(0,ParseOct3(*(const u32 *)("`000"),4,res));
    vrfyeq_(0,ParseOct3(*(const u32 *)("g000"),4,res));

    vrfyeq_(1,ParseOct3(*(const u32 *)("0/00"),4,res));
    vrfyeq_(2,ParseOct3(*(const u32 *)("00:0"),4,res));

}

struct AliasedU32 {
    u32 val;
} __attribute__((__may_alias__));

void atf_unit::unittest_algo_lib_UnescapeC() {
    u8 res;

    // again be exhaustive and paranoic
    frep_(i,256) {
        u8 buf[4];
        AliasedU32 *buf_val = (AliasedU32*)buf;
        memset(buf,0xff,sizeof(buf));
        bool check = true;
        u8 chk_val=0;
        switch(i) {
        case 'n' : chk_val = '\n' ; break;
        case 't' : chk_val = '\t' ; break;
        case 'v' : chk_val = '\v' ; break;
        case 'b' : chk_val = '\b' ; break;
        case 'r' : chk_val = '\r' ; break;
        case 'f' : chk_val = '\f' ; break;
        case 'a' : chk_val = '\a' ; break;
        case '\\': chk_val = '\\' ; break;
        case '\?': chk_val = '\?' ; break;
        case '\'': chk_val = '\'' ; break;
        case '\"': chk_val = '\"' ; break;
        case '\0': chk_val = '\0' ; break;
        case '0' :
        case '1' :
        case '2' :
        case '3' :
        case '4' :
        case '5' :
        case '6' :
        case '7' :
        case 'X' :
        case 'x' : check = false  ; break;
        default  : chk_val = u8(i); break;
        }
        if (check) {
            memset(buf,0xff,sizeof(buf));
            buf[0] = u8(i);
            res = 0xff;
            vrfyeq_(1,UnescapeC(buf_val->val,4,res));
            vrfyeq_(chk_val,res);
        }
        // octal
        buf[0] = u8(((i >> 6) & 3) + '0');
        buf[1] = u8(((i >> 3) & 7) + '0');
        buf[2] = u8(((i     ) & 7) + '0');
        buf[3] = u8(0xff);

        res = 0xff;
        vrfyeq_(3,UnescapeC(buf_val->val,4,res));
        vrfyeq_(i,res);

        // w/o leading zeroes
        int cnt=3;
        while(buf[0] == '0' && cnt > 1) {
            memmove(&buf[0],&buf[1],3);
            --cnt;
        }
        res = 0xff;
        vrfyeq_(cnt,UnescapeC(buf_val->val,4,res));
        vrfyeq_(i,res);

        // hexadecimal
        buf[0] = 'x';
        buf[1] = u8(((i >> 4) & 0x0f));
        if (buf[1] <= 9) { buf[1] += '0'; } else { buf[1] += 'A' - 10; }
        buf[2] = u8(i & 0x0f);
        if (buf[2] <= 9) { buf[2] += '0'; } else { buf[2] += 'a' - 10; }
        buf[3] = 0xff;

        res = 0xff;
        vrfyeq_(3,UnescapeC(buf_val->val,4,res));
        vrfyeq_(i,res);

        buf[0] = 'X';
        res = 0xff;
        vrfyeq_(3,UnescapeC(buf_val->val,4,res));
        vrfyeq_(i,res);

        // w/o leading zero
        if (buf[1]==0) {
            buf[0] = 'x';
            memmove(&buf[1],&buf[2],2);
            res = 0xff;
            vrfyeq_(2,UnescapeC(buf_val->val,4,res));
            vrfyeq_(i,res);
            buf[0] = 'X';
            res = 0xff;
            vrfyeq_(2,UnescapeC(buf_val->val,4,res));
            vrfyeq_(i,res);
        }
    }
}

// -----------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_ParseURL1() {
    {
        URL str;
        vrfy(URL_ReadStrptrMaybe(str, "http://x:y@a:1<-0.0.0.0/x"), algo_lib::_db.errtext);
        vrfyeq_(str.username, "x");
        vrfyeq_(str.password, "y");
        vrfyeq_(str.server, "a");
        vrfyeq_(str.port, 1);
        vrfyeq_(str.protocol, "http");
        vrfyeq_(str.host, "0.0.0.0");
        vrfyeq_(str.dir, "/x");
        vrfy(URL_ReadStrptrMaybe(str, "(10.0.10.0,224.0.0.0)"), algo_lib::_db.errtext);
        vrfyeq_(str.source_addr_host, u64(10<<24|0<<16|10<<8|0));
        vrfyeq_(str.server, "224.0.0.0");
        vrfy(URL_ReadStrptrMaybe(str, "<-0.0.0.0"), algo_lib::_db.errtext);
        vrfyeq_(str.host, "0.0.0.0");
    }
    {
        URL test;
        vrfy(URL_ReadStrptrMaybe(test, "ftp://a@b@c.com/dir"), algo_lib::_db.errtext);

        vrfy(URL_ReadStrptrMaybe(test, "a://b:c@d.e:11111~-1.2.3.4/g"), algo_lib::_db.errtext);
        vrfy_(test.protocol == "a");
        vrfy_(test.username == "b");
        vrfy_(test.password == "c");
        vrfy_(test.server == "d.e");
        vrfy_(test.port == 11111);
        vrfy_(test.dir == "/g");
        vrfy_(test.host == "1.2.3.4");
    }
}

// -----------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_PerfParseNum() {
    algo::RnullStr24 str;
    // NUL-terminated by construction
    str = algo::strptr("12345");
    {
        prlog("clear caches...");
        ByteAry xxx;
        Fill(ary_AllocN(xxx, 20*1024*1024), (unsigned char)0xff);
        u64 m = 0;
        frep_(i,ary_N(xxx)) {
            m+=ary_qFind(xxx, i);
        }
        tempstr t;
        t<<m;
    }
    rep_(i,2) {
        u32 len = algo::ch_N(str);

        if (len<=8) {
            u64 c = rdtscp();
            u32 ok=false;
            u64 result=atoi((char*)str.ch);
            c = rdtscp() -c;
            prlog(result<<": "<<c<<" cycles (atoi). ok:"<<ok);
        }

        if (len<=8) {
            u64 c = rdtscp();
            u32 ok;
            u64 result=ParseNum8(ReadBE64(str.ch),len,ok);
            c = rdtscp() -c;
            prlog(result<<": "<<c<<" cycles (ParseNum8). ok:"<<ok);
        }

        {
            u64 c = rdtscp();
            u32 ok;
            u64 result=aParseNum16(str,ok);
            c = rdtscp() -c;
            prlog(result<<": "<<c<<" cycles (aParseNum16). ok:"<<ok);
        }

        if (len<=4) {
            u64 c = rdtscp();
            u32 ok;
            u64 result=ParseNum4(ReadBE32(str.ch),len,ok);
            c = rdtscp() -c;
            prlog(result<<": "<<c<<" cycles (ParseNum4). ok:"<<ok);
        }
    }
}

// -----------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_DirBeg() {
    RemDirRecurse(".testdirbeg", true);
    errno_vrfy(0==mkdir(".testdirbeg", 0755), "mkdir");
    prlog("Iterate over empty directory (must not pick up ., ..)");
    {
        cstring check;
        ind_beg(Dir_curs,E,".testdirbeg/*") {
            check<<E.filename<<";";
        }ind_end;
        vrfyeq_(check,"");
    }

    StringToFile("something",".testdirbeg/.file");

    prlog("Directory with .-file (must see .-file)");
    {
        cstring check;
        ind_beg(Dir_curs,E,".testdirbeg/*.*") {
            check<<E.filename<<";";
        }ind_end;
        vrfyeq_(check,".file;");
    }

    StringToFile("something",".testdirbeg/file2");

    prlog("Directory with regular file as well");
    {
        cstring check;
        ind_beg(Dir_curs,E,".testdirbeg/*") {
            check<<E.filename<<";";
        }ind_end;
        vrfy_(check == "file2;.file;" || check == ".file;file2;");
    }

    errno_vrfy(chdir(".testdirbeg")==0, "chdir");

    prlog("No directory name (assume ./)");
    {
        cstring check;
        ind_beg(Dir_curs,E,"*") {
            check<<E.filename<<";";
        }ind_end;
        vrfy_(check == "file2;.file;" || check == ".file;file2;");
    }

    errno_vrfy(chdir("..")==0, "chdir");
    RemDirRecurse(".testdirbeg", true);
}

void atf_unit::unittest_algo_lib_RemDirRecurse() {
    CreateDirRecurse("temp");
    StringToFile("test", "temp/RemDirRecurse_unrelated");
    vrfyeq_(FileQ("temp/RemDirRecurse_unrelated"), true);

    RemDirRecurse("temp/RemDirRecurse_unrelated", true);//

    vrfyeq_(FileQ("temp/RemDirRecurse_unrelated"), true);//file must exist
    vrfyeq_(unlink("temp/RemDirRecurse_unrelated"), 0);// delete the file
}

void atf_unit::unittest_algo_lib_RemDirRecurse1() {
    CreateDirRecurse("temp");
    RemDirRecurse("temp/RemDirRecurse", true);
    CreateDirRecurse("temp/RemDirRecurse");
    vrfyeq_(DirectoryQ("temp/RemDirRecurse"), true);

    RemDirRecurse("temp/RemDirRecurse", true);

    vrfyeq_(DirectoryQ("temp/RemDirRecurse"), false);
}

void atf_unit::unittest_algo_lib_RemDirRecurse2() {
    CreateDirRecurse("temp");
    CreateDirRecurse("temp/RemDirRecurse");
    StringToFile("test"        , "temp/RemDirRecurse/unrelated");// this file should not get deleted

    RemDirRecurse("temp/RemDirRecurse", false);// do not remove topmost -- returns true

    vrfyeq_(DirectoryQ("temp/RemDirRecurse"), true);// dir must exist
    vrfyeq_(FileQ("temp/RemDirRecurse/unrelated"), false);// file must not exist
    vrfyeq_(rmdir("temp/RemDirRecurse"), 0);// wipe the dir
}

void atf_unit::unittest_algo_lib_RemDirRecurse3() {
    CreateDirRecurse("temp");
    RemDirRecurse("temp/RemDirRecurse", true);
    CreateDirRecurse("temp/RemDirRecurse/");
    CreateDirRecurse("temp/RemDirRecurse2");

    StringToFile("Test 1", "temp/RemDirRecurse/file1");
    StringToFile("Test 2", "temp/RemDirRecurse2/file2");
    vrfyeq_(symlink("../RemDirRecurse2", "temp/RemDirRecurse/dir2"), 0);

    RemDirRecurse("temp/RemDirRecurse", true);

    vrfyeq_(DirectoryQ("temp/RemDirRecurse"), false);
    vrfyeq_(DirectoryQ("temp/RemDirRecurse2"), true); // must exist!

    RemDirRecurse("temp/RemDirRecurse2", true); // delete it too

    vrfyeq_(DirectoryQ("temp/RemDirRecurse2"), false);// must not exist now
}

void atf_unit::unittest_algo_lib_RemDirRecurse4() {
    CreateDirRecurse("temp");
    RemDirRecurse("temp/RemDirRecurse", true);
    CreateDirRecurse("temp/RemDirRecurse/subdir/level3");

    StringToFile("Level 1 file", "temp/RemDirRecurse/file1");
    StringToFile("Level 2 file", "temp/RemDirRecurse/subdir/plain-l2");
    StringToFile("Level 3 file", "temp/RemDirRecurse/subdir/level3/plain-l3");
    StringToFile("test"        , "temp/RemDirRecurse_unrelated");// this file should not get deleted

    vrfyeq_(link("temp/RemDirRecurse/subdir/plain-l2", "temp/RemDirRecurse/hardlink1"), 0);
    vrfyeq_(link("temp/RemDirRecurse/file1"          , "temp/RemDirRecurse/subdir/hardlink2"), 0);
    vrfyeq_(symlink("../level3/plain-l3"             , "temp/RemDirRecurse/subdir/softlink2"), 0);
    vrfyeq_(symlink("../subdir/plain-l2"             , "temp/RemDirRecurse/subdir/level3/softlink3"), 0);

    // check that unrelated file was not deleted!
    RemDirRecurse("temp/RemDirRecurse", true);

    vrfyeq_(DirectoryQ("temp/RemDirRecurse"), false);
    vrfyeq_(FileToString("temp/RemDirRecurse_unrelated"), "test");
    DeleteFile("temp/RemDirRecurse_unrelated");
}

// --------------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_ReadModuleId() {
}

// --------------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_Tempfile() {
    cstring fname;
    {
        algo_lib::FTempfile tempfile;
        TempfileInitX(tempfile,"unit");
        fname = tempfile.filename;
        vrfyeq_(FileQ(fname), true);
    }

    vrfyeq_(FileQ(fname), false);// check that tempfile was deleted
}

// --------------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_RegxReadTwice() {
    // Read a regx twice. It should continue to work
    algo_lib::Regx regx;
    Regx_ReadSql(regx, "a", true);
    vrfyeq_(Regx_Match(regx, "a"), true);
    vrfyeq_(Regx_Match(regx, "b"), false);

    Regx_ReadSql(regx, "b", true);
    vrfyeq_(Regx_Match(regx, "a"), false);
    vrfyeq_(Regx_Match(regx, "b"), true);
}

// --------------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_RegxReadTwice2() {
    // Read a regx twice. It should continue to work
    algo_lib::Regx regx;
    Regx_ReadSql(regx, "%", false);
    vrfyeq_(Regx_Match(regx, "aasdfasdf"), true);
    vrfyeq_(Regx_Match(regx, "asdfasdf"), true);

    Regx_ReadSql(regx, "b", true);
    vrfyeq_(Regx_Match(regx, "a"), false);
    vrfyeq_(Regx_Match(regx, "b"), true);
}

// --------------------------------------------------------------------------------

static void TestNextSep(strptr before, char sep, strptr after, strptr left) {
    strptr actual_after=before,actual_left;
    algo::NextSep(actual_after,sep,actual_left);
    vrfyeq_(actual_left,left);
    vrfyeq_(actual_after,after);
}

void atf_unit::unittest_algo_lib_NextSep() {
    TestNextSep("abc.def" , '.', "def", "abc");
    TestNextSep("abc-def" , '.', "", "abc-def");
    TestNextSep(""        , '.', "", "");
    TestNextSep("."       , '.', "", "");
    TestNextSep(".."      , '.', ".", "");
    TestNextSep("-"       , '.', "", "-");
}

// --------------------------------------------------------------------------------

static void TestI32Dec3Uns(i32 value, strptr str) {
    algo::I32Dec3 num;
    num.value = value;
    cstring out;
    I32Dec3_Print(num,out);
    vrfyeq_(out,str);// prints correctly

    algo::I32Dec3 num2;
    vrfy(I32Dec3_ReadStrptrMaybe(num2,out), algo_lib::_db.errtext);
    vrfyeq_(num,num2);// round trips correctly
}

static void TestI32Dec3(i32 value, strptr str) {
    TestI32Dec3Uns(value,str);
    if (value > 0) {
        TestI32Dec3Uns(-value,tempstr()<<"-"<<str);
    }
}

void atf_unit::unittest_algo_lib_I32Dec3Fmt() {
    TestI32Dec3(0, "0.000");
    TestI32Dec3(1, "0.001");
    TestI32Dec3(10, "0.010");
    TestI32Dec3(100, "0.100");
    TestI32Dec3(999, "0.999");
    TestI32Dec3(1000, "1.000");
    TestI32Dec3(10000, "10.000");
}

// -----------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_OrderID() {
    u64 x = 0x1122334455667788;
    u64 y = be64toh(x);
    vrfy_(y == 0x8877665544332211);
}

// -----------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_IntPrice() {
    // start with integer
    algo::I64Dec4 price4 = algo::I64Dec4(i64(50000));
    vrfy_(price4.value == 50000);
    vrfy_(value_GetDouble(price4) == 5);

    // same, with double as the source
    algo::I64Dec4 price4_2;
    value_qSetDouble(price4_2, 5.0);
    vrfy_(value_GetDouble(price4_2) == 5);

    // have to get the same result
    vrfy_(price4==price4_2);
}

// -----------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_Keyval() {
    cstring str;
    str<<Keyval("a",0);      vrfyeq_(str,"a:0");// integer
    str<<Keyval("b","blah"); vrfyeq_(str,"a:0  b:blah");// space + string
    str<<Keyval("c","");     vrfyeq_(str,"a:0  b:blah  c:\"\"");// empty string
    str<<Keyval("","d");     vrfyeq_(str,"a:0  b:blah  c:\"\"  d");// no name
    str<<Keyval("e"," ");    vrfyeq_(str,"a:0  b:blah  c:\"\"  d  e:\" \"");// quoted
}

// -----------------------------------------------------------------------------

static i64 GetInode(strptr fname) {
    struct stat st;
    ZeroBytes(st);
    (void)stat(Zeroterm(tempstr()<<fname),&st);
    return st.st_ino;
}

// -----------------------------------------------------------------------------

static i64 GetMode(strptr fname) {
    struct stat st;
    ZeroBytes(st);
    (void)stat(Zeroterm(tempstr()<<fname),&st);
    return st.st_mode;
}

// --------------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_StringToFile() {
    cstring fname;
    fname << "temp/tempfile.1";
    cstring str;
    str<<"test";
    // write string to file
    {
        StringToFile(str, fname, algo_FileFlags_write, false);
        vrfyeq_(FileToString(fname), str);
    }
    // append string to file
    {
        StringToFile(str, fname, algo_FileFlags_append, false);
        vrfyeq_(FileToString(fname), "testtest");
    }
    // use tempfile
    {
        i64 inode=GetInode(fname);
        (void)chmod(Zeroterm(fname),0642);// change mode
        bool ok =SafeStringToFile(str, fname);// file had 'testtest', replacing it with 'test'

        vrfyeq_(ok,true);
        vrfyeq_(FileToString(fname), "test");// yep, it's there
        i64 inode2=GetInode(fname);
        vrfyeq_(GetMode(fname) & 0x1ff,0642);// mode should have been preserved
        vrfyeq_(inode==inode2, false);// inode changed -- tempfile was used


        ok =SafeStringToFile(str, fname);// save same string to file
        vrfyeq_(FileToString(fname), "test");// yep, it's there
        i64 inode3=GetInode(fname);
        vrfyeq_(ok,true);
        vrfyeq_(inode2==inode3, true);// inode should not have changed because no save occured
    }
}

// --------------------------------------------------------------------------------

static void TestU128PrintHex(u128 num, bool prefix, strptr result) {
    cstring str;
    u128_PrintHex(num, str, prefix);
    vrfyeq_(str, result);
}

void atf_unit::unittest_algo_lib_U128PrintHex() {
    TestU128PrintHex(0, true, "0X0");
    TestU128PrintHex(0, false, "0");
    TestU128PrintHex(-1, false, "FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF");
    TestU128PrintHex(0x123456789ABCDEF, false, "123456789ABCDEF");
    TestU128PrintHex(0x123456789ABCDEF0, false, "123456789ABCDEF0");
    TestU128PrintHex(1, false, "1");
    TestU128PrintHex(0x10000000000000, false, "10000000000000");
}

// --------------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_FileToString() {
    // check that FileToString can read /proc/cpuinfo
    vrfyeq_(FileToString("/proc/iomem")
            ,SysEval("cat /proc/iomem",FailokQ(true),1024*1024*10));
}

// --------------------------------------------------------------------------------

static void TestIpmask(strptr str, bool valid) {
    vrfyeq_(algo_lib::IpmaskValidQ(str),valid);
}

void atf_unit::unittest_algo_lib_CheckIpmask() {
    TestIpmask("170.19.56.1/32", true);
    TestIpmask("0.0.0.0/0", true);
    TestIpmask("10.148.115.64/27", true);
    TestIpmask("10.148.114.0/23", true);
    TestIpmask("10.148.115.0/23", false);
    TestIpmask("170.19.56.0/0", false);
    TestIpmask("170.19.56.0/1", false);
    TestIpmask("170.19.56.0/10", false);
    TestIpmask("170.19.56.0/", false);
    TestIpmask("170.19.56.0/33", false);
    TestIpmask("10.148.115.68/27", false);
}

// --------------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_TimeConstants() {
    vrfyeq_(algo::SECS_PER_DAY * 1000, algo::MSECS_PER_DAY);
    vrfyeq_(algo::SECS_PER_HOUR * 24, algo::SECS_PER_DAY);
    vrfyeq_(algo::SECS_PER_MIN * 60, algo::SECS_PER_HOUR);
    vrfyeq_(algo::UNTIME_PER_MSEC * 1000, algo::UNTIME_PER_SEC);
}

// --------------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_Datecache() {
    UnTime from;
    UnTime to;
    vrfy_(UnTime_ReadStrptrMaybe(from,"2016/01/01 00:00:00"));
    vrfy_(UnTime_ReadStrptrMaybe(to,"2016/31/31 23:59:59"));
    algo::DateCache cache;
    while (from < to) {
        vrfy_(DateCache_LocalDate(cache,from) == LocalDate(from));
        from.value += UNTIME_PER_SEC * 60 * 15; // 15 minutes at a time.
    }
    u64 c=0;
    DO_PERF_TEST("LocalDate",c+=LocalDate(from).value);
    DO_PERF_TEST("DateCache_LocalDate",c+=DateCache_LocalDate(cache,from).value);
    prlog(c);
}

// --------------------------------------------------------------------------------

template<class T> inline void TestCmp(i32 (*fcn)(T,T)) {
    vrfy_(fcn(0,1) < 0);
    vrfy_(fcn(0,0) == 0);
    vrfy_(fcn(1,0) > 0);

    vrfy_(fcn(10,11) < 0);
    vrfy_(fcn(10,10) == 0);
    vrfy_(fcn(11,10) > 0);
}

void atf_unit::unittest_algo_lib_Cmp() {
    TestCmp<u8>(u8_Cmp);
    TestCmp<u16>(u16_Cmp);
    TestCmp<u32>(u32_Cmp);
    TestCmp<u64>(u64_Cmp);
    TestCmp<u128>(u128_Cmp);
    TestCmp<i8>(i8_Cmp);
    TestCmp<i16>(i16_Cmp);
    TestCmp<i32>(i32_Cmp);
    TestCmp<i64>(i64_Cmp);
    TestCmp<char>(char_Cmp);
    TestCmp<float>(float_Cmp);
    TestCmp<double>(double_Cmp);

    vrfy_(bool_Cmp(false,true)==-1);
    vrfy_(bool_Cmp(true,false)==1);
    vrfy_(bool_Cmp(false,false)==0);
    vrfy_(bool_Cmp(true,true)==0);
}

// -----------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_SchedTime() {
    // TODO
}
// --------------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_StringSubrange() {
    cstring s("tempstr");
    vrfy_(ch_FirstN(s,1) == "t");
    vrfy_(ch_FirstN(s,4) == "temp");
    vrfy_(ch_FirstN(s,100) == "tempstr");

    vrfy_(ch_LastN(s,1) == "r");
    vrfy_(ch_LastN(s,4) == "pstr");
    vrfy_(ch_LastN(s,100) == "tempstr");

    vrfy_(ch_RestFrom(s,0) == "tempstr");
    vrfy_(ch_RestFrom(s,4) == "str");
    vrfy_(ch_RestFrom(s,100) == "");

    vrfy_(ch_GetRegion(s,0,1) == "t");
    vrfy_(ch_GetRegion(s,0,4) == "temp");
    vrfy_(ch_GetRegion(s,0,100) == "tempstr");
    vrfy_(ch_GetRegion(s,1,1) == "e");
    vrfy_(ch_GetRegion(s,1,10) == "empstr");
    vrfy_(ch_GetRegion(s,100,100) == "");
    vrfy_(ch_GetRegion(s,-1,-100) == "");
}

// --------------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_Clipped() {
    vrfy_(Clipped(0,0,1) == 0);
    vrfy_(Clipped(-1,0,1) == 0);
    vrfy_(Clipped(1,0,1) == 0);

    vrfy_(Clipped(0,0,1) == 0);
    vrfy_(Clipped(-1,0,1) == 0);
    vrfy_(Clipped(1,0,1) == 0);

    vrfy_(Clipped(1,10) == 1);
    vrfy_(Clipped(10,10) == 9);

    vrfy_(Clippedf(-0.1, 0.0, 1.0) == 0.0);
    vrfy_(Clippedf(0.0, 0.0, 1.0) == 0.0);
    vrfy_(Clippedf(0.1, 0.0, 1.0) == 0.1);
    vrfy_(Clippedf(1.0, 0.0, 1.0) == 1.0);
    vrfy_(Clippedf(1.1, 0.0, 1.0) == 1.0);

    vrfy_(u16_SubClip(1,0)==1);
    vrfy_(u16_SubClip(1,1)==0);
    vrfy_(u16_SubClip(1,2)==0);

    vrfy_(u32_SubClip(1,0)==1);
    vrfy_(u32_SubClip(1,1)==0);
    vrfy_(u32_SubClip(1,2)==0);

    vrfy_(u64_SubClip(1,0)==1);
    vrfy_(u64_SubClip(1,1)==0);
    vrfy_(u64_SubClip(1,2)==0);
}

// --------------------------------------------------------------------------------

template<class T> void TestAbs(T t, T result) {
    vrfy_(Abs(t)==result);
}
template<class T> void TestAbs2() {
    TestAbs<T>(0,0);
    TestAbs<T>(-1,1);
    TestAbs<T>(1,1);
}

void atf_unit::unittest_algo_lib_Abs() {
    TestAbs2<i8>();
    TestAbs2<i16>();
    TestAbs2<i32>();
    TestAbs2<i64>();
    TestAbs2<float>();
    TestAbs2<double>();
}

// -----------------------------------------------------------------------------


void atf_unit::unittest_algo_lib_PerfMinMaxAvg() {
}

void atf_unit::unittest_algo_lib_PerfIntrinsics() {
    int temp = 50;
    int accumulator=0;
    prlog(temp);
    DO_PERF_TEST("signed division",accumulator += temp/4);
    DO_PERF_TEST("unsigned division",accumulator += unsigned(temp)/4);
    prlog(accumulator);
}

void atf_unit::unittest_algo_lib_PerfTruncVsFtol() {
    int i=0;
    double x=double(time(NULL));
    prlog(int(double(INT_MAX)));
    prlog(int(double(INT_MAX)+1));
    DO_PERF_TEST("ftol",{i+=int(x); x++;});
    prlog(i);
}

void atf_unit::unittest_algo_lib_PerfParseDouble() {
    {
        strptr data("123.1432");
        double d;
        StringIter iter(data);
        DO_PERF_TEST("ParseDouble 123.1432",
                     vrfy(double_ReadStrptrMaybe(d, data), algo_lib::_db.errtext)
                     );
        DO_PERF_TEST("atof",d=atof("123.1432"));
    }
    {
        strptr data("12345");
        int d;
        DO_PERF_TEST("ParseI32 12345",
                     vrfy(i32_ReadStrptrMaybe(d, data), algo_lib::_db.errtext)
                     );
        DO_PERF_TEST("atoi",d=atoi("12345"));
    }
    {
        strptr data("0.1");
        double d;
        StringIter iter(data);
        DO_PERF_TEST("ParseDouble 0.1",
                     vrfy(double_ReadStrptrMaybe(d, data), algo_lib::_db.errtext)
                     );
        DO_PERF_TEST("atof",d=atof("0.1"));
    }
}

namespace atf_unit {
    inline bool operator >(const atf_unit::Dbl &a, const atf_unit::Dbl &b) {
        return a.val > b.val;
    }
    inline bool operator >(const atf_unit::Cstr &a, const atf_unit::Cstr &b) {
        return ::operator <(a.val, b.val);
    }
}

void atf_unit::unittest_algo_lib_PerfSort() {
    atf_unit::FPerfSort frame;
    frep_(input,5) {
        int N = 100000;
        const char *method = "";
        frep_(i,N) {
            atf_unit::Dbl &elem = orig_Alloc(frame);
            switch(input) {
            case 0: elem.val = double_WeakRandom(1.0); method="random double"; break;
            case 1: elem.val = i; method = "ascending double"; break;
            case 2: elem.val = -i; method = "descending double"; break;
            case 3: elem.val = 0; method="zeros"; break;
            case 4: elem.val = i32_WeakRandom(2); method="zeros and ones"; break;
            }
        }
        frep_(iter,2) {
            sorted_RemoveAll(frame);
            sorted_Addary(frame, orig_Getary(frame));
            u64 c = get_cycles();
            std::sort(frame.sorted_elems, frame.sorted_elems + frame.sorted_n);
            u64 stl_cycles_elem = (get_cycles() - c)/N;

            sorted_RemoveAll(frame);
            sorted_Addary(frame, orig_Getary(frame));
            c = get_cycles();
            sorted_QuickSort(frame);
            u64 amc_cycles_elem = (get_cycles() - c)/N;

            prlog("atf_unit.PerfSort"
                  <<Keyval("method",method)
                  <<Keyval("N",N)
                  <<Keyval("iteration",iter)
                  <<Keyval("stl_cycles_elem",stl_cycles_elem)
                  <<Keyval("amc_cycles_elem",amc_cycles_elem));
        }
    }
}
// --------------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_Replscope() {
    algo_lib::Replscope R;
    R.fatal=false;
    R.eatcomma=true;
    Set(R,"$xyz","y");
    vrfy_(Subst(R, "") == "");    // empty substitution
    vrfy_(Subst(R, "[$xyz]") == "[y]");    // simple substitution
    Set(R, "$abc", "$xyz");
    vrfy_(Subst(R,"$abc") == "y");    // Set with evaluation

    Set(R, "$abc", "$xyz", false);
    vrfy_(Subst(R,"$abc") == "$xyz");    // Set without evaluation

    Set(R,"$testvar","z");
    // back-to-back substitutions
    // end-of-string substitution
    vrfy_(Subst(R,"$xyz$testvar$xyz") == "yzy");
    vrfy_(Subst(R,"$xyz$bad$xyz") == "y$bady");    // failed substitution -- variable is left in place
    SetStrictMode(R,true);
    Set(R, "$empty", "");
    vrfy_(Subst(R,"$empty, blah") == "blah");    // comma-eating
    SetEatComma(R,false);
    vrfy_(Subst(R,"$empty, blah") == ", blah");    // no comma-eating

}

#include <set>


typedef std::multiset<int> Numset;


static void InsertNumber(i32 num){
    atf_unit::FNumber& fnum = atf_unit::number_Alloc();
    fnum.num = num;
    number_XrefMaybe(fnum);
}

static void IncrementingInsert(int num){
    for(int i = 0; i < num; ++i){
        InsertNumber(i);
    }
}

static void IncrementingInsertSet(int n, Numset& numset){
    for(int i = 0; i < n; ++i){
        //atf_unit::atf_unit::number_Alloc();
        numset.insert(i);
    }
}

static i32 IterateAvl(){
    i32 res = 0;
    ind_beg(atf_unit::_db_tr_number_curs, num, atf_unit::_db){
        res += num.num;
    }ind_end;
    return res;
}

static i32 IterateSet(Numset& numset){
    i32 res = 0;
    Numset::const_iterator it = numset.begin();
    Numset::const_iterator end = numset.end();
    for(; it != end; ++it){
        res += *it;
    }
    return res;
}


static int FindAvl(){
    int res = 0;
    for(int i = 0; i < 10000; ++i){
        res += atf_unit::tr_number_FirstGe(i) == NULL ? 0 : 1;
    }
    return res;
}

static int FindSet(Numset& numset){
    Numset::const_iterator it;;
    Numset::const_iterator end = numset.end();
    int res = 0;
    for(int i = 0; i < 10000; ++i){
        it = numset.find(i);
        res += it == end ? 0 : 1;
    }
    return res;
}

static void RandomInsertAvl(int n){
    for(int i = 0 ; i < n; ++i){
        InsertNumber(i32_WeakRandom(n));
    }
}

static void RandomInsertSet(int n, Numset& numset){
    for(int i = 0 ; i < n; ++i){
        numset.insert(i32_WeakRandom(n));
    }
}

static void RemoveSet(int n, Numset& numset){
    for(int i = 0 ; i < n; ++i){
        numset.erase(numset.begin());
    }
}

static void RemoveAvl(int n){
    for(int i = 0 ; i < n; ++i){
        atf_unit::tr_number_Remove(*atf_unit::tr_number_First());
    }
}

void atf_unit::unittest_algo_lib_AvlvsMap(){
    //number_Reserve(10000);
    int n = 10000;
    i64 c = get_cycles();
    IncrementingInsert(n);
    c = get_cycles() - c;
    prlog("AvlInsert"<< Keyval("n", n)<< Keyval("t(Sec)",algo::ToSecs(SchedTime(c))));
    Numset numset;
    c = get_cycles();
    IncrementingInsertSet(n, numset);
    c = get_cycles() - c;
    prlog("SetInsert"<< Keyval("n", n)<< Keyval("t(Sec)",algo::ToSecs(SchedTime(c))));
    u64 res=0;
    DO_PERF_TEST("AvlIterate",res+=IterateAvl());
    DO_PERF_TEST("SetIterate",res+=IterateSet(numset));
    DO_PERF_TEST("AvlFind",res+=FindAvl());
    DO_PERF_TEST("SetFind",res+=FindSet(numset));

    c = get_cycles();
    RandomInsertAvl(n);
    c = get_cycles() - c;
    prlog("AvlInsertRandom"<< Keyval("n", n)<< Keyval("t(Sec)",algo::ToSecs(SchedTime(c))));
    c = get_cycles();
    RandomInsertSet(n, numset);
    c = get_cycles() - c;
    prlog("SetInsertRandom"<< Keyval("n", n)<< Keyval("t(Sec)",algo::ToSecs(SchedTime(c))));
    DO_PERF_TEST("AvlIterate",res+=IterateAvl());
    DO_PERF_TEST("SetIterate",res+=IterateSet(numset));
    DO_PERF_TEST("AvlFind",res+=FindAvl());
    DO_PERF_TEST("SetFind",res+=FindSet(numset));

    c = get_cycles();
    RemoveAvl(n);
    c = get_cycles() - c;
    prlog("AvlRemove"<< Keyval("n", n)<< Keyval("t(Sec)",algo::ToSecs(SchedTime(c))));
    c = get_cycles();
    RemoveSet(n, numset);
    c = get_cycles() - c;
    prlog("SetRemove"<< Keyval("n", n)<< Keyval("t(Sec)",algo::ToSecs(SchedTime(c))));
}
// --------------------------------------------------------------------------------

static void CheckSleep(double sec) {
    bool success=false;
    for (int i=0; i<10 && !success; i++,sleep(1)) {
        SchedTime c(get_cycles());
        algo::SleepMsec(sec*1000.0);
        SchedTime c2(get_cycles());
        double elapsed = ElapsedSecs(c,c2);
        //some servers only give you 10% accuracy in sleep
        success = elapsed >= sec*0.75 && elapsed <= sec*1.25;
        verblog("atf_unit.check_sleep"
                <<Keyval("want",sec)
                <<Keyval("elapsed",elapsed)
                <<Keyval("success",success));
    }
    vrfy(success,"SleepMsec test failed");
}

void atf_unit::unittest_algo_lib_Sleep() {
    CheckSleep(0.01);
    CheckSleep(1.0);
    CheckSleep(2.9);
}

// --------------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_strptr_Eq() {
    for (int len = 0; len < 200; len++) {
        tempstr a;
        tempstr b;
        for (int i=0; i<len; i++) {
            a << 'x';
            b << 'x';
        }
        for (int i=0; i<len; i++) {
            b.ch_elems[i]='y';
            vrfy_(!strptr_Eq(a,b));
            vrfy_(!StrEqual(a,b,true));
            vrfy_(!StrEqual(a,b,false));

            b.ch_elems[i]='X';
            vrfy_(!strptr_Eq(a,b));
            vrfy_(!StrEqual(a,b,true));
            vrfy_(StrEqual(a,b,false));

            b.ch_elems[i]='x';
            vrfy_(strptr_Eq(a,b));
            vrfy_(StrEqual(a,b,true));
            vrfy_(StrEqual(a,b,false));
        }
    }
}

// -----------------------------------------------------------------------------

static void CheckSysEval(strptr cmd, strptr expect_output, int limit, bool expect_success) {
    bool test_ok=true;
    cstring out;
    // test command
    try {
        out=SysEval(cmd,FailokQ(true),limit);
        test_ok = out==expect_output;
        out="";
    } catch(algo_lib::ErrorX &x) {
        test_ok=expect_success==false;
    }
    // re-test same command with FailokQ(false)
    if (test_ok && !expect_success) {
        try {
            out=SysEval(cmd,FailokQ(false),limit);
            test_ok = false;// must have thrown!!
        } catch(algo_lib::ErrorX &x) {
            test_ok = true;
        }
    }
    vrfy(test_ok,
         tempstr("syseval_fail")
         <<Keyval("cmd",cmd)
         <<Keyval("expect_output",expect_output)
         <<Keyval("output",out)
         <<Keyval("limit",limit));
}

void atf_unit::unittest_algo_lib_SysEval() {
    CheckSysEval("echo -n blah","blah",10,true);
    CheckSysEval("echo -n longstring","",1,false);// will fail because of output limit
    CheckSysEval("cat /dev/zero","",100000,false);// will fail because of output limit
    CheckSysEval("echo blah","blah\n",5,true);// check newline
}

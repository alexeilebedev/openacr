// Copyright (C) 2023-2024,2026 AlgoRND
// Copyright (C) 2020-2023 Astra
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
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
// Target: atf_unit (exe) -- Unit tests (see unittest table)
// Exceptions: yes
// Source: cpp/atf_unit/algo_lib.cpp
//

#include "include/atf_unit.h"
#include <algorithm>
#ifndef WIN32
#include <unistd.h>
#endif


void atf_unit::unittest_algo_lib_PopCnt1() {
    vrfy_(algo::u32_Count1s(u32(0)) == 0);
    u32 N = sizeof(u32) * 8;
    rep_(i,int(N-1)) {
        vrfy_(algo::u32_Count1s(   u32(1) << i    ) == u32(1  ));
        vrfy_(algo::u32_Count1s(~( u32(1) << i   )) ==     N-1);
        vrfy_(algo::u32_Count1s(  (u32(1) << i)-1 ) == u32(i  ));
        vrfy_(algo::u32_Count1s(~((u32(1) << i)-1)) ==     N-i);
    }
}

void atf_unit::unittest_algo_lib_PopCnt2() {
    vrfy_(algo::u32_Count1s(u32(0)) == 0);
    u32 N = sizeof(u64) * 8;
    rep_(i,int(N-1)) {
        vrfy_(algo::u64_Count1s(   u64(1) << i    ) == u32(1  ));
        vrfy_(algo::u64_Count1s(~( u64(1) << i   )) ==     N-1);
        vrfy_(algo::u64_Count1s(  (u64(1) << i)-1 ) == u32(i  ));
        vrfy_(algo::u64_Count1s(~((u64(1) << i)-1)) ==     N-i);
    }
}

struct RandomTypeName { // ignore:struct_in_src
    char c[18];
    RandomTypeName(){memset(this,0,sizeof(*this));}
};

// -----------------------------------------------------------------------------

static void DoTest(double d) {
    vrfyeq_(  algo::DFloor(d),floor(d));
    vrfyeq_(algo::DCeiling(d),ceil(d));
    vrfyeq_(  algo::DTrunc(d),d-fmod(d,1));
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

void atf_unit::unittest_algo_lib_ReadLine() {
    LineBuf acc;
    strptr str("abcde\n\n");
    algo::memptr data=strptr_ToMemptr(str);
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
    vrfyeq_(algo::CeilingLog2(u32(0)), 0);
    vrfyeq_(algo::CeilingLog2(u32(1)), 0);
    vrfyeq_(algo::CeilingLog2(u32(2)), 1);
    vrfyeq_(algo::CeilingLog2(u32(4)), 2);
    vrfyeq_(algo::CeilingLog2(u32(8)), 3);
    vrfyeq_(algo::CeilingLog2(u32(16)), 4);
    vrfyeq_(algo::CeilingLog2(u32(32)), 5);
    vrfyeq_(algo::CeilingLog2(u32(64)), 6);

    //vrfyeq_(algo::CeilingLog2(u32(0)), 0);
    vrfyeq_(algo::CeilingLog2(u32(1)), 0);
    vrfyeq_(algo::CeilingLog2(u32(3)), 2);
    vrfyeq_(algo::CeilingLog2(u32(7)), 3);
    vrfyeq_(algo::CeilingLog2(u32(15)), 4);
    vrfyeq_(algo::CeilingLog2(u32(31)), 5);
    vrfyeq_(algo::CeilingLog2(u32(63)), 6);

    vrfyeq_(algo::u32_CeilPow2((u32)4 ,(u32)8), 8);
    vrfyeq_(algo::u32_CeilPow2((u32)4, (u32)16), 16);
    vrfyeq_(algo::u32_CeilPow2((u32)4, (u32)64), 64);

    vrfyeq_(algo::u32_CeilPow2((u32)0 ,(u32)8), 0);
    vrfyeq_(algo::u32_CeilPow2((u32)0, (u32)16), 0);
    vrfyeq_(algo::u32_CeilPow2((u32)0, (u32)64), 0);

    vrfyeq_(algo::u32_CeilPow2((u32)7  ,(u32)8), 8);
    vrfyeq_(algo::u32_CeilPow2((u32)15, (u32)16), 16);
    vrfyeq_(algo::u32_CeilPow2((u32)63, (u32)64), 64);
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

    vrfyeq_(substr_FindFirst(s, strptr("a"))        ,algo::i32_Range(0,1));
    vrfyeq_(substr_FindFirst(s, strptr(""))         ,algo::i32_Range(0,0));
    vrfyeq_(substr_FindFirst(s, strptr("test"))     ,algo::i32_Range(elems_N(s),elems_N(s)));
    vrfyeq_(substr_FindFirst(s, strptr("abcde"))    ,algo::i32_Range(0,5));


    vrfyeq_(substr_FindLast(s, strptr("a"))     ,algo::i32_Range(0,1));
    vrfyeq_(substr_FindLast(s, strptr(""))      ,algo::i32_Range(5,5));
    vrfyeq_(substr_FindLast(s, strptr("test"))  ,algo::i32_Range(0,0));
    vrfyeq_(substr_FindLast(s, strptr("abcde")) ,algo::i32_Range(0,5));

    s = "b,c,\nd\n";

    s = "12345";
    vrfy_(TRevFind(s, '6') == algo::i32_Range(0,0));
    vrfy_(TFind(s, '6') == algo::i32_Range(elems_N(s),elems_N(s)));

    frep_(i,elems_N(s)) {
        vrfy_(TFind(s,s[i]) == algo::i32_Range(i,i+1));
        vrfy_(TRevFind(s,s[i]) == algo::i32_Range(i,i+1));
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

void atf_unit::unittest_algo_lib_ParseNumber_Overflow1() {
    algo::LnumStr7_U32_Base36 x;
    ch_SetStrptr(x, "1234XYZ");
    u32 val = ch_GetnumDflt(x,0);
    vrfyeq_(val, u32(2302984187));

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

// -----------------------------------------------------------------------------

static void TestDoubleWithCommas(double d, int prec, strptr result) {
    tempstr t;
    double_PrintWithCommas(d,t,prec);
    vrfyeq_(t,result);
}

void atf_unit::unittest_algo_lib_PrintDoubleWithCommas() {
    // test integers
    for (int prec=-1; prec<20; prec++) {
        TestDoubleWithCommas(0, prec, "0");
        TestDoubleWithCommas(1, prec, "1");
        TestDoubleWithCommas(10, prec, "10");
        TestDoubleWithCommas(12, prec, "12");
        TestDoubleWithCommas(100  , prec, "100");
        TestDoubleWithCommas(1000 , prec, "1,000");
        TestDoubleWithCommas(10000, prec, "10,000");
        TestDoubleWithCommas(100000, prec, "100,000");
        TestDoubleWithCommas(101, prec, "101");
        TestDoubleWithCommas(-101, prec, "-101");
        TestDoubleWithCommas(-1001, prec, "-1,001");
        TestDoubleWithCommas(1001, prec, "1,001");
        TestDoubleWithCommas(10001, prec, "10,001");
        TestDoubleWithCommas(100001, prec, "100,001");
        TestDoubleWithCommas(12341234, prec, "12,341,234");
    }

    TestDoubleWithCommas(0.1, 0, "0");
    TestDoubleWithCommas(0.1, 1, "0.1");
    TestDoubleWithCommas(0.1, 2, "0.1");
    TestDoubleWithCommas(0.1, 3, "0.1");

    TestDoubleWithCommas(-0.000001, 0, "-0");
    TestDoubleWithCommas(-0.000001, 1, "-0");
    TestDoubleWithCommas(-0.000001, 2, "-0");
    TestDoubleWithCommas(-0.000001, 3, "-0");
    TestDoubleWithCommas(-0.000001, 4, "-0");
    TestDoubleWithCommas(-0.000001, 5, "-0");
    TestDoubleWithCommas(-0.000001, 6, "-0.000001");

    TestDoubleWithCommas(12.0001, 0, "12");
    TestDoubleWithCommas(12.0001, 1, "12");
    TestDoubleWithCommas(12.0001, 2, "12");
    TestDoubleWithCommas(12.0001, 3, "12");
    TestDoubleWithCommas(12.0001, 4, "12.0001");

    TestDoubleWithCommas(12341234.333, 1, "12,341,234.3");
    TestDoubleWithCommas(12341234.333, 5, "12,341,234.333");

    TestDoubleWithCommas(1000.0*1000*1000*1000*1000, 0, "1,000,000,000,000,000");
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
    // check octal char printing
    u8 buf[] = {255};
    CheckCppStr(strptr((char*)buf,1), "\'\\377\'", '\'');
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

// -----------------------------------------------------------------------------

static void TestString(strptr before, strptr printed) {
    tempstr in;
    tempstr out;
    out << MaybeSpace;
    PrintAttr(out, "", before);
    vrfy_(out == printed);
    algo::StringIter iter(out);
    cstring_ReadCmdarg(in, iter, false);
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
    u128 val = u128(1) * u32(1000) * u32(1000) * u32(1000) * u32(1000); // 12 zeros
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
    algo::StringIter iter(a);
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
    algo::StringIter iter(str);
    double d;
    vrfy_(TryParseDouble(iter, d)==ok && (!ok || d==result));
}

static void TestParseDigits(strptr str, bool ok, double result) {
    algo::StringIter iter(str);
    double d;
    vrfy(TryParseDigits(iter, d)==ok && (!ok || d==result), tempstr()<<str<<" "<<ok<<" "<<result);
}

// -----------------------------------------------------------------------------

static void TestGetLine() {
    algo::StringIter iter;
    iter = algo::StringIter("\nxx\n\n");
    vrfy_(GetLine(iter) == strptr());
    iter = algo::StringIter("\nxx\n\n");
    GetLine(iter);
    vrfy_(GetLine(iter) == "xx");
    iter =algo::StringIter("xyz\r\n\n");
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
    vrfy_(c1234.i == u32(MULTICHAR_CONST4('4','3','2','1')));
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

    vrfy_(algo::CompareNoCase("a","b")<0);
    vrfy_(algo::CompareNoCase("a","A")==0);
    vrfy_(algo::CompareNoCase("a","aa")<0);
    vrfy_(algo::CompareNoCase("a","AA")<0);

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
        case '0': vrfyeq_(algo::ParseOct1(i,res),1); vrfyeq_(res,0); break;
        case '1': vrfyeq_(algo::ParseOct1(i,res),1); vrfyeq_(res,1); break;
        case '2': vrfyeq_(algo::ParseOct1(i,res),1); vrfyeq_(res,2); break;
        case '3': vrfyeq_(algo::ParseOct1(i,res),1); vrfyeq_(res,3); break;
        case '4': vrfyeq_(algo::ParseOct1(i,res),1); vrfyeq_(res,4); break;
        case '5': vrfyeq_(algo::ParseOct1(i,res),1); vrfyeq_(res,5); break;
        case '6': vrfyeq_(algo::ParseOct1(i,res),1); vrfyeq_(res,6); break;
        case '7': vrfyeq_(algo::ParseOct1(i,res),1); vrfyeq_(res,7); break;
        default : vrfyeq_(algo::ParseOct1(i,res),0);                 break;
        }
    }
}

void atf_unit::unittest_algo_lib_ParseHex1() {
    // be exhaustive ... (this will catch "shift" error)
    frep_(i,256) {
        u8 res  = 0xff;
        // .. and straightforward (this will catch "encoding" error)
        switch (i) {
        case '0': vrfyeq_(algo::ParseHex1(i,res),1); vrfyeq_(res,0);  break;
        case '1': vrfyeq_(algo::ParseHex1(i,res),1); vrfyeq_(res,1);  break;
        case '2': vrfyeq_(algo::ParseHex1(i,res),1); vrfyeq_(res,2);  break;
        case '3': vrfyeq_(algo::ParseHex1(i,res),1); vrfyeq_(res,3);  break;
        case '4': vrfyeq_(algo::ParseHex1(i,res),1); vrfyeq_(res,4);  break;
        case '5': vrfyeq_(algo::ParseHex1(i,res),1); vrfyeq_(res,5);  break;
        case '6': vrfyeq_(algo::ParseHex1(i,res),1); vrfyeq_(res,6);  break;
        case '7': vrfyeq_(algo::ParseHex1(i,res),1); vrfyeq_(res,7);  break;
        case '8': vrfyeq_(algo::ParseHex1(i,res),1); vrfyeq_(res,8);  break;
        case '9': vrfyeq_(algo::ParseHex1(i,res),1); vrfyeq_(res,9);  break;
        case 'a':
        case 'A': vrfyeq_(algo::ParseHex1(i,res),1); vrfyeq_(res,10); break;
        case 'b':
        case 'B': vrfyeq_(algo::ParseHex1(i,res),1); vrfyeq_(res,11); break;
        case 'c':
        case 'C': vrfyeq_(algo::ParseHex1(i,res),1); vrfyeq_(res,12); break;
        case 'd':
        case 'D': vrfyeq_(algo::ParseHex1(i,res),1); vrfyeq_(res,13); break;
        case 'e':
        case 'E': vrfyeq_(algo::ParseHex1(i,res),1); vrfyeq_(res,14); break;
        case 'f':
        case 'F': vrfyeq_(algo::ParseHex1(i,res),1); vrfyeq_(res,15); break;
        default : vrfyeq_(algo::ParseHex1(i,res),0);
        }
    }
}

// -----------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_ParseOct3() {
    u8 res;

    // nominal cases for 1..3 digits

    res = 0xff;
    vrfyeq_(1,algo::ParseOct3(*(const u32 *)("0000"),1,res));
    vrfyeq_(0,res);

    res = 0xff;
    vrfyeq_(1,algo::ParseOct3(*(const u32 *)("0\0\0\0"),4,res));
    vrfyeq_(0,res);

    res = 0xff;
    vrfyeq_(2,algo::ParseOct3(*(const u32 *)("0000"),2,res));
    vrfyeq_(0,res);

    res = 0xff;
    vrfyeq_(2,algo::ParseOct3(*(const u32 *)("00\0\0"),4,res));
    vrfyeq_(0,res);

    res = 0xff;
    vrfyeq_(3,algo::ParseOct3(*(const u32 *)("0000"),3,res));
    vrfyeq_(0,res);

    res = 0xff;
    vrfyeq_(3,algo::ParseOct3(*(const u32 *)("000\0"),4,res));
    vrfyeq_(0,res);

    // check that 4th digit is not taken

    res = 0xff;
    vrfyeq_(3,algo::ParseOct3(*(const u32 *)("0001"),4,res));
    vrfyeq_(0,res);

    // verify range

    res = 0;
    vrfyeq_(3,algo::ParseOct3(*(const u32 *)("3770"),4,res));
    vrfyeq_(0377,res);

    // verify order

    res = 0;
    vrfyeq_(1,algo::ParseOct3(*(const u32 *)("1\0\0\0"),4,res));
    vrfyeq_(1,res);

    res = 0;
    vrfyeq_(2,algo::ParseOct3(*(const u32 *)("13\0\0"),4,res));
    vrfyeq_(013,res);

    res = 0;
    vrfyeq_(3,algo::ParseOct3(*(const u32 *)("135\0\0"),4,res));
    vrfyeq_(0135,res);

    res = 0;
    vrfyeq_(3,algo::ParseOct3(*(const u32 *)("1350"),4,res));
    vrfyeq_(0135,res);

    // verify hole

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Warray-bounds"
    res = 0;
    vrfyeq_(1,algo::ParseOct3(*(const u32 *)("1\35000"),4,res));
    vrfyeq_(1,res);

    vrfyeq_(0,algo::ParseOct3(*(const u32 *)("\135000"),4,res));
#ifdef WIN32
#pragma warning(pop)
#endif
#pragma GCC diagnostic pop

    // verify non-octal digit

    vrfyeq_(0,algo::ParseOct3(*(const u32 *)("8000"),4,res));
    vrfyeq_(1,algo::ParseOct3(*(const u32 *)("0800"),4,res));
    vrfyeq_(2,algo::ParseOct3(*(const u32 *)("0080"),4,res));
    vrfyeq_(3,algo::ParseOct3(*(const u32 *)("0008"),4,res));
}


void atf_unit::unittest_algo_lib_ParseHex2() {
    u8 res;

    // nominal cases for 1..2 digits

    res = 0xff;
    vrfyeq_(1,algo::ParseHex2(*(const u32 *)("0000"),1,res));
    vrfyeq_(0,res);

    res = 0xff;
    vrfyeq_(1,algo::ParseHex2(*(const u32 *)("0\0\0\0"),4,res));
    vrfyeq_(0,res);

    res = 0xff;
    vrfyeq_(2,algo::ParseHex2(*(const u32 *)("0000"),2,res));
    vrfyeq_(0,res);

    res = 0xff;
    vrfyeq_(2,algo::ParseHex2(*(const u32 *)("00\0\0"),4,res));
    vrfyeq_(0,res);

    // check that 3rd and 4th digits are not taken

    res = 0xff;
    vrfyeq_(2,algo::ParseHex2(*(const u32 *)("001\0"),4,res));
    vrfyeq_(0,res);

    res = 0xff;
    vrfyeq_(2,algo::ParseHex2(*(const u32 *)("0012"),4,res));
    vrfyeq_(0,res);

    // verify range

    res = 0;
    vrfyeq_(2,algo::ParseHex2(*(const u32 *)("fF\0\0"),4,res));
    vrfyeq_(0xff,res);

    // some interesting nominal cases

    res = 0;
    vrfyeq_(2,algo::ParseHex2(*(const u32 *)("9A\0\0"),4,res));
    vrfyeq_(0x9a,res);

    res = 0;
    vrfyeq_(2,algo::ParseHex2(*(const u32 *)("a1\0\0"),4,res));
    vrfyeq_(0xa1,res);


    // verify order

    res = 0;
    vrfyeq_(1,algo::ParseHex2(*(const u32 *)("1\0\0\0"),4,res));
    vrfyeq_(1,res);

    res = 0;
    vrfyeq_(2,algo::ParseHex2(*(const u32 *)("13\0\0"),4,res));
    vrfyeq_(0x13,res);

    // verify hole

    vrfyeq_(0,algo::ParseHex2(*(const u32 *)("\13\0\0"),4,res));

    // verify non-hexadecimal

    vrfyeq_(0,algo::ParseOct3(*(const u32 *)("/000"),4,res));
    vrfyeq_(0,algo::ParseOct3(*(const u32 *)(":000"),4,res));
    vrfyeq_(0,algo::ParseOct3(*(const u32 *)("@000"),4,res));
    vrfyeq_(0,algo::ParseOct3(*(const u32 *)("G000"),4,res));
    vrfyeq_(0,algo::ParseOct3(*(const u32 *)("`000"),4,res));
    vrfyeq_(0,algo::ParseOct3(*(const u32 *)("g000"),4,res));

    vrfyeq_(1,algo::ParseOct3(*(const u32 *)("0/00"),4,res));
    vrfyeq_(2,algo::ParseOct3(*(const u32 *)("00:0"),4,res));

}

struct AliasedU32 { // ignore:struct_in_src
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
            vrfyeq_(1,algo::UnescapeC(buf_val->val,4,res));
            vrfyeq_(chk_val,res);
        }
        // octal
        buf[0] = u8(((i >> 6) & 3) + '0');
        buf[1] = u8(((i >> 3) & 7) + '0');
        buf[2] = u8(((i     ) & 7) + '0');
        buf[3] = u8(0xff);

        res = 0xff;
        vrfyeq_(3,algo::UnescapeC(buf_val->val,4,res));
        vrfyeq_(i,res);

        // w/o leading zeroes
        int cnt=3;
        while(buf[0] == '0' && cnt > 1) {
            memmove(&buf[0],&buf[1],3);
            --cnt;
        }
        res = 0xff;
        vrfyeq_(cnt,algo::UnescapeC(buf_val->val,4,res));
        vrfyeq_(i,res);

        // hexadecimal
        buf[0] = 'x';
        buf[1] = u8(((i >> 4) & 0x0f));
        if (buf[1] <= 9) { buf[1] += '0'; } else { buf[1] += 'A' - 10; }
        buf[2] = u8(i & 0x0f);
        if (buf[2] <= 9) { buf[2] += '0'; } else { buf[2] += 'a' - 10; }
        buf[3] = 0xff;

        res = 0xff;
        vrfyeq_(3,algo::UnescapeC(buf_val->val,4,res));
        vrfyeq_(i,res);

        buf[0] = 'X';
        res = 0xff;
        vrfyeq_(3,algo::UnescapeC(buf_val->val,4,res));
        vrfyeq_(i,res);

        // w/o leading zero
        if (buf[1]==0) {
            buf[0] = 'x';
            memmove(&buf[1],&buf[2],2);
            res = 0xff;
            vrfyeq_(2,algo::UnescapeC(buf_val->val,4,res));
            vrfyeq_(i,res);
            buf[0] = 'X';
            res = 0xff;
            vrfyeq_(2,algo::UnescapeC(buf_val->val,4,res));
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
        algo::ByteAry xxx;
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
            u64 c = algo::rdtscp();
            u32 ok=false;
            u64 result=atoi((char*)str.ch);
            c = algo::rdtscp() -c;
            prlog(result<<": "<<c<<" cycles (atoi). ok:"<<ok);
        }

        if (len<=8) {
            u64 c = algo::rdtscp();
            u32 ok;
            u64 result=algo::ParseNum8(algo::ReadBE64(str.ch),len,ok);
            c = algo::rdtscp() -c;
            prlog(result<<": "<<c<<" cycles (ParseNum8). ok:"<<ok);
        }

        {
            u64 c = algo::rdtscp();
            u32 ok;
            u64 result=algo::aParseNum16(str,ok);
            c = algo::rdtscp() -c;
            prlog(result<<": "<<c<<" cycles (aParseNum16). ok:"<<ok);
        }

        if (len<=4) {
            u64 c = algo::rdtscp();
            u32 ok;
            u64 result=algo::ParseNum4(algo::ReadBE32(str.ch),len,ok);
            c = algo::rdtscp() -c;
            prlog(result<<": "<<c<<" cycles (ParseNum4). ok:"<<ok);
        }
    }
}

// -----------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_DirBeg() {
    algo::RemDirRecurse("temp/testdirbeg", true);
    errno_vrfy(0==mkdir("temp/testdirbeg", 0755), "mkdir");
    prlog("Iterate over empty directory (must not pick up ., ..)");
    {
        cstring check;
        ind_beg(algo::Dir_curs,E,"temp/testdirbeg/*") {
            check<<E.filename<<";";
        }ind_end;
        vrfyeq_(check,"");
    }

    algo::StringToFile("something","temp/testdirbeg/.file");

    prlog("Directory with .-file (must see .-file)");
    {
        cstring check;
        ind_beg(algo::Dir_curs,E,"temp/testdirbeg/*.*") {
            check<<E.filename<<";";
        }ind_end;
        vrfyeq_(check,".file;");
    }

    algo::StringToFile("something","temp/testdirbeg/file2");

    prlog("Directory with regular file as well");
    {
        cstring check;
        ind_beg(algo::Dir_curs,E,"temp/testdirbeg/*") {
            check<<E.filename<<";";
        }ind_end;
        vrfy_(check == "file2;.file;" || check == ".file;file2;");
    }

    errno_vrfy(chdir("temp/testdirbeg")==0, "chdir");

    prlog("No directory name (assume ./)");
    {
        cstring check;
        ind_beg(algo::Dir_curs,E,"*") {
            check<<E.filename<<";";
        }ind_end;
        vrfy_(check == "file2;.file;" || check == ".file;file2;");
    }

    errno_vrfy(chdir("..")==0, "chdir");
    algo::RemDirRecurse("temp/testdirbeg", true);
}

void atf_unit::unittest_algo_lib_RemDirRecurse() {
    {
        algo::CreateDirRecurse("temp");
        algo::StringToFile("test", "temp/RemDirRecurse_unrelated");
        vrfyeq_(algo::FileQ("temp/RemDirRecurse_unrelated"), true);

        algo::RemDirRecurse("temp/RemDirRecurse_unrelated", true);//

        vrfyeq_(algo::FileQ("temp/RemDirRecurse_unrelated"), true);//file must exist
        vrfyeq_(unlink("temp/RemDirRecurse_unrelated"), 0);// delete the file
    }

    {
        algo::CreateDirRecurse("temp");
        algo::RemDirRecurse("temp/RemDirRecurse", true);
        algo::CreateDirRecurse("temp/RemDirRecurse");
        vrfyeq_(algo::DirectoryQ("temp/RemDirRecurse"), true);

        algo::RemDirRecurse("temp/RemDirRecurse", true);

        vrfyeq_(algo::DirectoryQ("temp/RemDirRecurse"), false);
    }

    {
        algo::CreateDirRecurse("temp");
        algo::CreateDirRecurse("temp/RemDirRecurse");
        algo::StringToFile("test"        , "temp/RemDirRecurse/unrelated");// this file should not get deleted

        algo::RemDirRecurse("temp/RemDirRecurse", false);// do not remove topmost -- returns true

        vrfyeq_(algo::DirectoryQ("temp/RemDirRecurse"), true);// dir must exist
        vrfyeq_(algo::FileQ("temp/RemDirRecurse/unrelated"), false);// file must not exist
        vrfyeq_(rmdir("temp/RemDirRecurse"), 0);// wipe the dir
    }

    {
        algo::CreateDirRecurse("temp");
        algo::RemDirRecurse("temp/RemDirRecurse", true);
        algo::CreateDirRecurse("temp/RemDirRecurse/");
        algo::CreateDirRecurse("temp/RemDirRecurse2");

        algo::StringToFile("Test 1", "temp/RemDirRecurse/file1");
        algo::StringToFile("Test 2", "temp/RemDirRecurse2/file2");
        vrfyeq_(symlink("../RemDirRecurse2", "temp/RemDirRecurse/dir2"), 0);

        algo::RemDirRecurse("temp/RemDirRecurse", true);

        vrfyeq_(algo::DirectoryQ("temp/RemDirRecurse"), false);
        vrfyeq_(algo::DirectoryQ("temp/RemDirRecurse2"), true); // must exist!

        algo::RemDirRecurse("temp/RemDirRecurse2", true); // delete it too

        vrfyeq_(algo::DirectoryQ("temp/RemDirRecurse2"), false);// must not exist now
    }

    {
        algo::CreateDirRecurse("temp");
        algo::RemDirRecurse("temp/RemDirRecurse", true);
        algo::CreateDirRecurse("temp/RemDirRecurse/subdir/level3");

        algo::StringToFile("Level 1 file", "temp/RemDirRecurse/file1");
        algo::StringToFile("Level 2 file", "temp/RemDirRecurse/subdir/plain-l2");
        algo::StringToFile("Level 3 file", "temp/RemDirRecurse/subdir/level3/plain-l3");
        algo::StringToFile("test"        , "temp/RemDirRecurse_unrelated");// this file should not get deleted

        vrfyeq_(link("temp/RemDirRecurse/subdir/plain-l2", "temp/RemDirRecurse/hardlink1"), 0);
        vrfyeq_(link("temp/RemDirRecurse/file1"          , "temp/RemDirRecurse/subdir/hardlink2"), 0);
        vrfyeq_(symlink("../level3/plain-l3"             , "temp/RemDirRecurse/subdir/softlink2"), 0);
        vrfyeq_(symlink("../subdir/plain-l2"             , "temp/RemDirRecurse/subdir/level3/softlink3"), 0);

        // check that unrelated file was not deleted!
        algo::RemDirRecurse("temp/RemDirRecurse", true);

        vrfyeq_(algo::DirectoryQ("temp/RemDirRecurse"), false);
        vrfyeq_(algo::FileToString("temp/RemDirRecurse_unrelated"), "test");
        algo::DeleteFile("temp/RemDirRecurse_unrelated");
    }
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

// DecodeNBytes and DecodeNChars take a length that always comes from the wire:
// a protobuf field header carrying the varint 0xFFFFFFFF, a kafka record whose
// signed varint length decodes to -1. Such a length must be refused, and the
// refusal has to hold for every integer type a caller can hand over -- the
// callers spell the length u16, u32, i32 and i64, and each of those spans a
// different set of values that no buffer can satisfy.
//
// The length is therefore compared as a signed 64-bit count, which every
// caller's type converts into without changing value, and the accepted range is
// stated on both ends: at least zero and at most the number of bytes left in
// the buffer. A negative length has no reading as a byte count, and a length
// above the remaining size cannot be served. A rejected call leaves the buffer
// where it was and the result empty, so a caller that ignores the return value
// still sees no fabricated payload.
void atf_unit::unittest_algo_lib_DecodeNRange() {
    u8 wire[] = {'a','b','c'};
    algo::memptr result;
    strptr chars;
    // zero bytes: accepted, buffer untouched
    algo::memptr buf(wire,sizeof(wire));
    vrfyeq_(algo::DecodeNBytes(buf,0,result),true);
    vrfyeq_(elems_N(result),0);
    vrfyeq_(elems_N(buf),3);
    // a length inside the buffer, then the whole remainder
    vrfyeq_(algo::DecodeNBytes(buf,1,result),true);
    vrfyeq_(ToStrPtr(result),strptr("a"));
    vrfyeq_(algo::DecodeNBytes(buf,2,result),true);
    vrfyeq_(ToStrPtr(result),strptr("bc"));
    vrfyeq_(elems_N(buf),0);
    // one byte past the end
    buf = algo::memptr(wire,sizeof(wire));
    vrfyeq_(algo::DecodeNBytes(buf,4,result),false);
    vrfyeq_(elems_N(result),0);
    vrfyeq_(elems_N(buf),3);
    // i32 lengths that a signed varint can produce
    vrfyeq_(algo::DecodeNBytes(buf,i32(-1),result),false);
    vrfyeq_(algo::DecodeNBytes(buf,i32(-2147483647-1),result),false);
    // u32 lengths that an unsigned varint can produce
    vrfyeq_(algo::DecodeNBytes(buf,u32(0x80000000),result),false);
    vrfyeq_(algo::DecodeNBytes(buf,u32(0xFFFFFFFF),result),false);
    // the widest lengths a caller can spell
    u64 u64max = ~u64(0);
    vrfyeq_(algo::DecodeNBytes(buf,u64max,result),false);
    vrfyeq_(algo::DecodeNBytes(buf,i64(-1),result),false);
    vrfyeq_(elems_N(buf),3);
    // DecodeNChars answers the same table
    vrfyeq_(algo::DecodeNChars(buf,0,chars),true);
    vrfyeq_(algo::DecodeNChars(buf,3,chars),true);
    vrfyeq_(chars,strptr("abc"));
    buf = algo::memptr(wire,sizeof(wire));
    vrfyeq_(algo::DecodeNChars(buf,4,chars),false);
    vrfyeq_(algo::DecodeNChars(buf,i32(-1),chars),false);
    vrfyeq_(algo::DecodeNChars(buf,u32(0xFFFFFFFF),chars),false);
    vrfyeq_(algo::DecodeNChars(buf,u64max,chars),false);
    vrfyeq_(elems_N(chars),0);
    vrfyeq_(elems_N(buf),3);
}

// -----------------------------------------------------------------------------

// Decode WIRE as a u32 varint, reporting the value and the number of bytes the
// decode took out of the buffer.
static bool DecodeVlcU32(algo::memptr wire, u32 &result, int &nread) {
    algo::memptr buf = wire;
    bool ret = algo::DecodeVLCLEU32(buf,result);
    nread = elems_N(wire) - elems_N(buf);
    return ret;
}

// Decode WIRE as a u64 varint, reporting the value and the number of bytes the
// decode took out of the buffer.
static bool DecodeVlcU64(algo::memptr wire, u64 &result, int &nread) {
    algo::memptr buf = wire;
    bool ret = algo::DecodeVLCLEU64(buf,result);
    nread = elems_N(wire) - elems_N(buf);
    return ret;
}

// A varint arrives as a sequence of seven-bit groups and is accumulated into a
// fixed-width integer, so the decoder has to answer what happens when the
// encoding names a value the accumulator cannot hold. A protobuf length field
// carrying the five bytes 83 80 80 80 10 names 4294967299 bytes of payload;
// keeping only the low 32 bits of that leaves 3, and a decode reported as
// successful then hands the caller the three-byte payload of a message that
// declared four gigabytes.
//
// The general condition is that a group's bits have to survive the shift that
// places them. A group shifted by the accumulator's width or more contributes
// nothing, and a group whose top bits fall off the end contributes only part of
// itself; in both cases the encoding names a value the accumulator does not
// hold, and the decode of the whole varint is refused.
//
// Each decoder therefore walks groups only while the shift stays inside its own
// width -- five bytes for u32, ten for u64 -- and only while the group under
// that shift keeps all its bits. A varint that runs past the width, and one the
// buffer never terminates, both leave the buffer where it was and the result
// zero. The zigzag decoders read through the same two accumulators and answer
// the same table.
void atf_unit::unittest_algo_lib_DecodeVLCLERange() {
    u32 v32(0);
    int nread(0);
    // one group per byte; four groups always fit u32
    u8 one32[] = {0x7f};
    vrfyeq_(DecodeVlcU32(algo::memptr(one32,sizeof(one32)),v32,nread),true);
    vrfyeq_(v32,u32(0x7f));
    vrfyeq_(nread,1);
    u8 four32[] = {0xff,0xff,0xff,0x7f};
    vrfyeq_(DecodeVlcU32(algo::memptr(four32,sizeof(four32)),v32,nread),true);
    vrfyeq_(v32,u32(0x0fffffff));
    vrfyeq_(nread,4);
    // the fifth group fits when it is 0x0f or less, which spans u32 to its top
    u8 max32[] = {0xff,0xff,0xff,0xff,0x0f};
    vrfyeq_(DecodeVlcU32(algo::memptr(max32,sizeof(max32)),v32,nread),true);
    vrfyeq_(v32,u32(0xffffffff));
    vrfyeq_(nread,5);
    // a fifth group of zero pads the encoding without naming a wider value
    u8 pad32[] = {0x80,0x80,0x80,0x80,0x00};
    vrfyeq_(DecodeVlcU32(algo::memptr(pad32,sizeof(pad32)),v32,nread),true);
    vrfyeq_(v32,u32(0));
    vrfyeq_(nread,5);
    // 0x10 is the smallest fifth group that does not fit: 2^32 and 2^32+3
    u8 over32[] = {0x80,0x80,0x80,0x80,0x10};
    vrfyeq_(DecodeVlcU32(algo::memptr(over32,sizeof(over32)),v32,nread),false);
    vrfyeq_(v32,u32(0));
    vrfyeq_(nread,0);
    u8 wide32[] = {0x83,0x80,0x80,0x80,0x10};
    vrfyeq_(DecodeVlcU32(algo::memptr(wide32,sizeof(wide32)),v32,nread),false);
    vrfyeq_(v32,u32(0));
    vrfyeq_(nread,0);
    // a sixth group is past the width whatever it holds
    u8 six32[] = {0x80,0x80,0x80,0x80,0x80,0x01};
    vrfyeq_(DecodeVlcU32(algo::memptr(six32,sizeof(six32)),v32,nread),false);
    vrfyeq_(nread,0);
    // ten and eleven groups are refused for the same reason, small value or not
    u8 ten32[] = {0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x01};
    vrfyeq_(DecodeVlcU32(algo::memptr(ten32,sizeof(ten32)),v32,nread),false);
    vrfyeq_(nread,0);
    u8 eleven32[] = {0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x01};
    vrfyeq_(DecodeVlcU32(algo::memptr(eleven32,sizeof(eleven32)),v32,nread),false);
    vrfyeq_(nread,0);
    // a varint the buffer never terminates
    u8 cut32[] = {0x80,0x80};
    vrfyeq_(DecodeVlcU32(algo::memptr(cut32,sizeof(cut32)),v32,nread),false);
    vrfyeq_(nread,0);
    // the u32 zigzag decoder answers the same table and consumes nothing
    algo::memptr zbuf(wide32,sizeof(wide32));
    i32 z32(1);
    vrfyeq_(algo::DecodeVLCLEI32Z(zbuf,z32),false);
    vrfyeq_(z32,0);
    vrfyeq_(elems_N(zbuf),5);
    // nine groups always fit u64
    u64 v64(0);
    u8 one64[] = {0x01};
    vrfyeq_(DecodeVlcU64(algo::memptr(one64,sizeof(one64)),v64,nread),true);
    vrfyeq_(v64,u64(1));
    vrfyeq_(nread,1);
    u8 nine64[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x7f};
    vrfyeq_(DecodeVlcU64(algo::memptr(nine64,sizeof(nine64)),v64,nread),true);
    vrfyeq_(v64,u64(0x7fffffffffffffff));
    vrfyeq_(nread,9);
    // the tenth group fits when it is 0x01, which spans u64 to its top
    u8 max64[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x01};
    vrfyeq_(DecodeVlcU64(algo::memptr(max64,sizeof(max64)),v64,nread),true);
    vrfyeq_(v64,~u64(0));
    vrfyeq_(nread,10);
    // 0x02 is the smallest tenth group that does not fit
    u8 over64[] = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x02};
    vrfyeq_(DecodeVlcU64(algo::memptr(over64,sizeof(over64)),v64,nread),false);
    vrfyeq_(v64,u64(0));
    vrfyeq_(nread,0);
    // an eleventh group is past the width, and shifting by it is not defined
    u8 eleven64[] = {0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x80,0x01};
    vrfyeq_(DecodeVlcU64(algo::memptr(eleven64,sizeof(eleven64)),v64,nread),false);
    vrfyeq_(v64,u64(0));
    vrfyeq_(nread,0);
    // a varint the buffer never terminates
    u8 cut64[] = {0x80};
    vrfyeq_(DecodeVlcU64(algo::memptr(cut64,sizeof(cut64)),v64,nread),false);
    vrfyeq_(nread,0);
    // the u64 zigzag decoder answers the same table and consumes nothing
    algo::memptr zbuf64(eleven64,sizeof(eleven64));
    i64 z64(1);
    vrfyeq_(algo::DecodeVLCLEI64Z(zbuf64,z64),false);
    vrfyeq_(z64,0);
    vrfyeq_(elems_N(zbuf64),11);
}

// -----------------------------------------------------------------------------

// The Dec reader accumulates the digits into a u64 and then stores the
// result in the field's own type. A string whose scaled value exceeds that
// type must be refused: 700.00 in a u16-backed field with two implied
// places scales to 70000, and storing it would wrap to 4464 -- a read that
// returns true after silently changing the value. A field as wide as the
// accumulator is exposed to the same failure one level up: the scaled value
// of 495765611597143987 with two implied places is 49576561159714398700,
// which does not fit a u64, and the top of the range must still read. The
// signed field is bounded by the same magnitude, so its own maximum reads
// and one unit past it is refused.
//
// A signed field's negative range reaches one unit further than its positive
// range, and the reader admits that unit: value_Print of an i32-backed field
// with one implied place at the type minimum emits -214748364.8, and reading
// that string back has to return the same value, or a record that printed
// cannot be loaded. Digits past the field's scale are dropped rather than
// rounded, so -214748364.85 reads as the minimum too, while a magnitude one
// unit further down is refused. An unsigned field has no negative range at
// all and refuses any leading minus.
void atf_unit::unittest_algo_lib_DecReadRange() {
    algo::U16Dec2 dec16;
    vrfyeq_(value_ReadStrptrMaybe(dec16,"655.35"),true);
    vrfyeq_(dec16.value,65535);
    vrfyeq_(value_ReadStrptrMaybe(dec16,"700.00"),false);
    algo::U32Dec1 dec32;
    vrfyeq_(value_ReadStrptrMaybe(dec32,"429496729.5"),true);
    vrfyeq_(dec32.value,4294967295);
    vrfyeq_(value_ReadStrptrMaybe(dec32,"429496729.6"),false);
    algo::U64Dec2 dec64;
    vrfyeq_(value_ReadStrptrMaybe(dec64,"184467440737095516.15"),true);
    vrfyeq_(dec64.value,u64(18446744073709551615ULL));
    vrfyeq_(value_ReadStrptrMaybe(dec64,"184467440737095516.16"),false);
    vrfyeq_(value_ReadStrptrMaybe(dec64,"495765611597143987"),false);
    vrfyeq_(value_ReadStrptrMaybe(dec64,"276701161105643274.00"),false);
    vrfyeq_(dec64.value,u64(18446744073709551615ULL));
    algo::I64Dec2 dec64s;
    vrfyeq_(value_ReadStrptrMaybe(dec64s,"92233720368547758.07"),true);
    vrfyeq_(dec64s.value,i64(9223372036854775807LL));
    vrfyeq_(value_ReadStrptrMaybe(dec64s,"92233720368547758.08"),false);
    vrfyeq_(value_ReadStrptrMaybe(dec64s,"431955459045440910"),false);
    vrfyeq_(value_ReadStrptrMaybe(dec64s,"-431955459045440910"),false);
    vrfyeq_(dec64s.value,i64(9223372036854775807LL));
    vrfyeq_(value_ReadStrptrMaybe(dec64s,"-92233720368547758.08"),true);
    vrfyeq_(dec64s.value,i64(-9223372036854775807LL-1));
    vrfyeq_(value_ReadStrptrMaybe(dec64s,"-92233720368547758.09"),false);
    vrfyeq_(dec64s.value,i64(-9223372036854775807LL-1));
    algo::I32Dec1 dec32s;
    vrfyeq_(value_ReadStrptrMaybe(dec32s,"214748364.7"),true);
    vrfyeq_(dec32s.value,2147483647);
    vrfyeq_(value_ReadStrptrMaybe(dec32s,"214748364.8"),false);
    vrfyeq_(value_ReadStrptrMaybe(dec32s,"-214748364.8"),true);
    vrfyeq_(dec32s.value,i32(-2147483647-1));
    vrfyeq_(value_ReadStrptrMaybe(dec32s,"-214748364.85"),true);
    vrfyeq_(dec32s.value,i32(-2147483647-1));
    vrfyeq_(value_ReadStrptrMaybe(dec32s,"-214748364.9"),false);
    vrfyeq_(value_ReadStrptrMaybe(dec32s,"-2147483648"),false);
    vrfyeq_(dec32s.value,i32(-2147483647-1));
    // the printed minimum reads back as the same value
    tempstr out;
    value_Print(dec32s,out);
    vrfyeq_(out,strptr("-214748364.8"));
    algo::I32Dec1 dec32back;
    vrfyeq_(value_ReadStrptrMaybe(dec32back,out),true);
    vrfyeq_(dec32back.value,dec32s.value);
    // an unsigned field refuses a leading minus
    vrfyeq_(value_ReadStrptrMaybe(dec16,"-1.00"),false);
    vrfyeq_(value_ReadStrptrMaybe(dec32,"-1.0"),false);
}

// -----------------------------------------------------------------------------

// SetDoubleMaybe stores the value rounded to the nearest scaled integer, so
// the bound it checks is the range the rounded value must land in, half a
// unit wider than the field's own range at each end: 214748364.7 scales to
// exactly 2147483647, the top of an i32-backed field with one implied place,
// and must store, while a value half a unit further up rounds past the top
// and is refused. The bottom of the range grants the same allowance, so a
// value that rounds up to the field's minimum stores and one that rounds
// below it is refused. A field whose type is as wide as the double's own
// integer reach is refused at the point where the scaled value reaches the
// type's magnitude, so no out-of-range double is ever stored.
void atf_unit::unittest_algo_lib_DecSetDoubleRange() {
    algo::I32Dec1 dec32;
    vrfyeq_(value_SetDoubleMaybe(dec32,214748364.7),true);
    vrfyeq_(dec32.value,2147483647);
    vrfyeq_(value_SetDoubleMaybe(dec32,214748364.74),true);
    vrfyeq_(dec32.value,2147483647);
    vrfyeq_(value_SetDoubleMaybe(dec32,-214748364.8),true);
    vrfyeq_(dec32.value,-2147483648);
    vrfyeq_(value_SetDoubleMaybe(dec32,-214748364.84),true);
    vrfyeq_(dec32.value,-2147483648);
    vrfyeq_(value_SetDoubleMaybe(dec32,-214748364.9),false);
    vrfyeq_(value_SetDoubleMaybe(dec32,-300000000.0),false);
    algo::U16Dec2 dec16;
    vrfyeq_(value_SetDoubleMaybe(dec16,655.35),true);
    vrfyeq_(dec16.value,65535);
    vrfyeq_(value_SetDoubleMaybe(dec16,655.4),false);
    vrfyeq_(value_SetDoubleMaybe(dec16,-0.004),true);
    vrfyeq_(dec16.value,0);
    vrfyeq_(value_SetDoubleMaybe(dec16,-0.01),false);
    algo::I64Dec1 dec64;
    vrfyeq_(value_SetDoubleMaybe(dec64,922337203685477580.8),false);
    vrfyeq_(value_SetDoubleMaybe(dec64,-922337203685477580.8),false);
    vrfyeq_(value_SetDoubleMaybe(dec64,1e30),false);
    vrfyeq_(value_SetDoubleMaybe(dec64,-1e30),false);
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
    algo::ZeroBytes(st);
    (void)stat(Zeroterm(tempstr()<<fname),&st);
    return st.st_ino;
}

// -----------------------------------------------------------------------------

static i64 GetMode(strptr fname) {
    struct stat st;
    algo::ZeroBytes(st);
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

        ok = SafeStringToFile("test2",fname); // new string to file
        vrfyeq_(FileToString(fname),"test2");// must be there
    }
}

// --------------------------------------------------------------------------------

// SafeStringToFile stages the new contents in a mkstemp tempfile beside the
// target, then renames it over the target. A failed rename -- forced here by
// making the target an existing directory -- must not leave the tempfile
// behind: each failed save would otherwise leak one <target>-XXXXXX file
// beside the target. Verify the call reports failure and removes the tempfile.
void atf_unit::unittest_algo_lib_SsfRenameFail() {
    strptr target("temp/algo_lib_ssf_renamefail");
    // the tempfile is created as <target>-XXXXXX, so both scans below derive
    // their glob from the target: the leak check cannot drift onto a stale
    // stem and pass vacuously if the target path changes
    tempstr pattern;
    pattern << target << "-*";
    // clean droppings from a previous failed run so the check below is
    // fresh -- collecting first and deleting after the walk: Dir_curs wraps
    // readdir() live, and an unlink mid-walk can perturb the enumeration so
    // a second stale dropping is skipped, survives the pre-clean, and trips
    // the leak check below as a false failure
    cstring stale;
    ind_beg(algo::Dir_curs,entry,pattern) {
        stale << entry.pathname << eol;
    }ind_end;
    ind_beg(algo::Line_curs,line,stale) {
        (void)DeleteFile(line);
    }ind_end;
    CreateDirRecurse(target);
    bool ok = SafeStringToFile("test",target);
    vrfyeq_(ok,false);
    bool leaked = false;
    ind_beg(algo::Dir_curs,entry,pattern) {
        (void)entry;
        leaked = true;
    }ind_end;
    vrfyeq_(leaked,false);
    (void)RemDirRecurse(target,true);
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
    algo::CreateDirRecurse("temp");

    // test basic operation
    {
        algo_lib::FTempfile tempfile;
        TempfileInitX(tempfile,"x");

        cstring str("blah\n");
        StringToFile(str,tempfile.filename);
        vrfy_(FileToString(tempfile.filename) == str);
    }

#ifdef __linux__
    // check that FileToString can read /proc/cpuinfo
    // mac doesn't have these files.
    vrfyeq_(algo::FileToString("/proc/iomem")
            ,SysEval("cat /proc/iomem",FailokQ(true),1024*1024*10));
#endif
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
    algo::UnTime from;
    algo::UnTime to;
    vrfy_(UnTime_ReadStrptrMaybe(from,"2016/01/01 00:00:00"));
    vrfy_(UnTime_ReadStrptrMaybe(to,"2016/31/31 23:59:59"));
    algo::DateCache cache;
    while (from < to) {
        vrfy_(DateCache_LocalDate(cache,from) == LocalDate(from));
        from.value += algo::UNTIME_PER_SEC * 60 * 15; // 15 minutes at a time.
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
#ifdef WIN32
#pragma warning(push)
#pragma warning(disable:4245) // conversion from int to u32, signed/unsigned mismatch
#endif
    vrfy_(ch_GetRegion(s,-1,-100) == "");
#ifdef WIN32
#pragma warning(pop)
#endif
}

// --------------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_Clipped() {
    vrfy_(algo::Clipped(0,0,1) == 0);
    vrfy_(algo::Clipped(-1,0,1) == 0);
    vrfy_(algo::Clipped(1,0,1) == 0);

    vrfy_(algo::Clipped(0,0,1) == 0);
    vrfy_(algo::Clipped(-1,0,1) == 0);
    vrfy_(algo::Clipped(1,0,1) == 0);

    vrfy_(algo::Clipped(1,10) == 1);
    vrfy_(algo::Clipped(10,10) == 9);

    vrfy_(algo::Clippedf(-0.1, 0.0, 1.0) == 0.0);
    vrfy_(algo::Clippedf(0.0, 0.0, 1.0) == 0.0);
    vrfy_(algo::Clippedf(0.1, 0.0, 1.0) == 0.1);
    vrfy_(algo::Clippedf(1.0, 0.0, 1.0) == 1.0);
    vrfy_(algo::Clippedf(1.1, 0.0, 1.0) == 1.0);

    vrfy_(algo::u16_SubClip(1,0)==1);
    vrfy_(algo::u16_SubClip(1,1)==0);
    vrfy_(algo::u16_SubClip(1,2)==0);

    vrfy_(algo::u32_SubClip(1,0)==1);
    vrfy_(algo::u32_SubClip(1,1)==0);
    vrfy_(algo::u32_SubClip(1,2)==0);
    vrfy_(algo::u32_SubClip(1516,~0LL)==0);
    vrfy_(algo::u32_SubClip(UINT_MAX,UINT_MAX)==0);
    vrfy_(algo::u32_SubClip(UINT_MAX,UINT_MAX-1)==1);

    vrfy_(algo::u64_SubClip(1,0)==1);
    vrfy_(algo::u64_SubClip(1,1)==0);
    vrfy_(algo::u64_SubClip(1,2)==0);
    vrfy_(algo::u64_SubClip(1516,ULLONG_MAX)==0);
}

// --------------------------------------------------------------------------------

template<class T> void TestAbs(T t, T result) {
    vrfy_(algo::Abs(t)==result);
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
        strptr data("");
        u32 d;
        prlog("input: "<<data);
        DO_PERF_TEST("u32_ReadStrPtrMaybe",
                     vrfy(u32_ReadStrptrMaybe(d, data), algo_lib::_db.errtext)
                     );
        vrfy_(d==0);
    }
    {
        strptr data("12345678");
        u32 d;
        prlog("input: "<<data);
        DO_PERF_TEST("u32_ReadStrPtrMaybe",
                     vrfy(u32_ReadStrptrMaybe(d, data), algo_lib::_db.errtext)
                     );
        vrfy_(d==12345678);
        char *endptr = data.elems  + data.n_elems;
        DO_PERF_TEST("strtoull",d=strtoull(data.elems,&endptr,10));
        vrfy_(d==12345678);
    }
    {
        strptr data("1234567812345678");
        u64 d;
        prlog("input: "<<data);
        DO_PERF_TEST("u64_ReadStrPtrMaybe",
                     vrfy(u64_ReadStrptrMaybe(d, data), algo_lib::_db.errtext)
                     );
        vrfy_(d==1234567812345678);
        char *endptr = data.elems  + data.n_elems;
        DO_PERF_TEST("strtoull",d=strtoull(data.elems,&endptr,10));
        vrfy_(d==1234567812345678);
    }
    {
        strptr data("1234567812345678");
        u128 d;
        prlog("input: "<<data);
        DO_PERF_TEST("u128_ReadStrPtrMaybe",
                     vrfy(u128_ReadStrptrMaybe(d, data), algo_lib::_db.errtext)
                     );
        vrfy_(d==1234567812345678);
    }
    {
        strptr data("12345678123456781234567812345678");
        u128 d;
        prlog("input: "<<data);
        DO_PERF_TEST("u128_ReadStrPtrMaybe",
                     vrfy(u128_ReadStrptrMaybe(d, data), algo_lib::_db.errtext)
                     );
        vrfy_(d==u128(1234567812345678)*10000000000000000+1234567812345678);
    }
    {
        strptr data("123.1432");
        double d;
        algo::StringIter iter(data);
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
        algo::StringIter iter(data);
        DO_PERF_TEST("ParseDouble 0.1",
                     vrfy(double_ReadStrptrMaybe(d, data), algo_lib::_db.errtext)
                     );
        DO_PERF_TEST("atof",d=atof("0.1"));
    }
}

namespace atf_unit { // ignore:namespace_block
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
            case 0: elem.val = algo::double_WeakRandom(1.0); method="random double"; break;
            case 1: elem.val = i; method = "ascending double"; break;
            case 2: elem.val = -i; method = "descending double"; break;
            case 3: elem.val = 0; method="zeros"; break;
            case 4: elem.val = algo::i32_WeakRandom(2); method="zeros and ones"; break;
            }
        }
        frep_(iter,2) {
            sorted_RemoveAll(frame);
            sorted_Addary(frame, orig_Getary(frame));
            u64 c = algo::get_cycles();
            std::sort(frame.sorted_elems, frame.sorted_elems + frame.sorted_n);
            u64 stl_cycles_elem = (algo::get_cycles() - c)/N;

            sorted_RemoveAll(frame);
            sorted_Addary(frame, orig_Getary(frame));
            c = algo::get_cycles();
            sorted_QuickSort(frame);
            u64 amc_cycles_elem = (algo::get_cycles() - c)/N;

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
    {
        algo_lib::Replscope R;
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
        vrfyeq_(Subst(R,"$xyz$testvar$xyz"), "yzy");
        vrfyeq_(Subst(R,"$xyz$bad$xyz"), "y$bady");    // failed substitution -- variable is left in place
        R.strict=2;
        Set(R, "$empty", "");
        vrfyeq_(Subst(R,"$empty, blah"), "blah");    // comma-eating
        eatcomma_Set(R,false);
        vrfyeq_(Subst(R,"$empty, blah"), ", blah");    // no comma-eating
    }

    // Check $$
    {
        algo_lib::Replscope R;
        R.eatcomma=false;
        Set(R, "$$", "$");
        vrfyeq_(Subst(R,"$$HOME"), "$HOME");// dollar-substitution
    }

    // Other cases
    {
        algo_lib::Replscope R;
        Set(R, "$var1", "blah");
        Set(R, "$var12", "foo");
        vrfyeq_(Subst(R,"$var12"), "blah2");// minimal match
        vrfyeq_(Subst(R,"$var"), "$var");// no match
        vrfyeq_(Subst(R,"$var$var1"), "$varblah");// first substitution fails, second succeeds
        vrfyeq_(Subst(R,"${var12}"), "foo");// curly syntax near end of string
        vrfyeq_(Subst(R,"${var12}x"), "foox");// curly syntax inside string
        vrfyeq_(Subst(R,"${var12"), "${var12");// unclosed curly
    }
}

// -----------------------------------------------------------------------------


void atf_unit::unittest_algo_lib_ReplscopeSharedPrefix() {
    // Check that adding a prefix of another variable fails
    {
        algo_lib::Replscope R;
        R.strict=2;
        Set(R,"$xyz","");
        bool good=false;
        try {
            Set(R,"$xy","");
        }  catch (...) {
            good=true;
        }
        vrfy(good, "new prefix was not detected");
    }

    // Check that adding a variable whose prefix is already added, fails
    {
        algo_lib::Replscope R;
        R.strict=2;
        Set(R,"$xy","");
        bool good=false;
        try {
            Set(R,"$xyz","");
        }  catch (...) {
            good=true;
        }
        vrfy(good, "old prefix was not detected");
    }
}

// -----------------------------------------------------------------------------

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
        InsertNumber(algo::i32_WeakRandom(n));
    }
}

static void RandomInsertSet(int n, Numset& numset){
    for(int i = 0 ; i < n; ++i){
        numset.insert(algo::i32_WeakRandom(n));
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
    i64 c = algo::get_cycles();
    IncrementingInsert(n);
    c = algo::get_cycles() - c;
    prlog("AvlInsert"<< Keyval("n", n)<< Keyval("t(Sec)",algo::ToSecs(algo::SchedTime(c))));
    Numset numset;
    c = algo::get_cycles();
    IncrementingInsertSet(n, numset);
    c = algo::get_cycles() - c;
    prlog("SetInsert"<< Keyval("n", n)<< Keyval("t(Sec)",algo::ToSecs(algo::SchedTime(c))));
    u64 res=0;
    DO_PERF_TEST("AvlIterate",res+=IterateAvl());
    DO_PERF_TEST("SetIterate",res+=IterateSet(numset));
    DO_PERF_TEST("AvlFind",res+=FindAvl());
    DO_PERF_TEST("SetFind",res+=FindSet(numset));

    c = algo::get_cycles();
    RandomInsertAvl(n);
    c = algo::get_cycles() - c;
    prlog("AvlInsertRandom"<< Keyval("n", n)<< Keyval("t(Sec)",algo::ToSecs(algo::SchedTime(c))));
    c = algo::get_cycles();
    RandomInsertSet(n, numset);
    c = algo::get_cycles() - c;
    prlog("SetInsertRandom"<< Keyval("n", n)<< Keyval("t(Sec)",algo::ToSecs(algo::SchedTime(c))));
    DO_PERF_TEST("AvlIterate",res+=IterateAvl());
    DO_PERF_TEST("SetIterate",res+=IterateSet(numset));
    DO_PERF_TEST("AvlFind",res+=FindAvl());
    DO_PERF_TEST("SetFind",res+=FindSet(numset));

    c = algo::get_cycles();
    RemoveAvl(n);
    c = algo::get_cycles() - c;
    prlog("AvlRemove"<< Keyval("n", n)<< Keyval("t(Sec)",algo::ToSecs(algo::SchedTime(c))));
    c = algo::get_cycles();
    RemoveSet(n, numset);
    c = algo::get_cycles() - c;
    prlog("SetRemove"<< Keyval("n", n)<< Keyval("t(Sec)",algo::ToSecs(algo::SchedTime(c))));
}
// --------------------------------------------------------------------------------

#ifndef __CYGWIN__
static void CheckSleep(double sec) {
    bool success=false;
    // try for up to 1 minute (in case of machine load)
    for (int i=0; i<60 && !success; i++,sleep(1)) {
        algo::SchedTime c(algo::get_cycles());
        algo::SleepMsec(sec*1000.0);
        algo::SchedTime c2(algo::get_cycles());
        double elapsed = algo::ElapsedSecs(c,c2);
        //some servers only give you 10% accuracy in sleep
        success = elapsed >= sec*0.5 && elapsed <= sec*1.5;
        verblog("atf_unit.check_sleep"
                <<Keyval("want",sec)
                <<Keyval("elapsed",elapsed)
                <<Keyval("success",success));
    }
    vrfy(success,"SleepMsec test failed");
}
#endif

void atf_unit::unittest_algo_lib_Sleep() {
#ifdef __CYGWIN__
    prlog("atf_unit: skipping Sleep test as it sometimes fails under Cygwin");
#else
    CheckSleep(0.01);
    CheckSleep(1.0);
    CheckSleep(2.9);
#endif
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
    } catch(algo_lib::ErrorX &) {
        test_ok=expect_success==false;
    }
    // re-test same command with FailokQ(false)
    if (test_ok && !expect_success) {
        try {
            out=SysEval(cmd,FailokQ(false),limit);
            test_ok = false;// must have thrown!!
        } catch(algo_lib::ErrorX &) {
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
    CheckSysEval("printf %s blah","blah",10,true);
    CheckSysEval("printf %s longstring","",1,false);// will fail because of output limit
    CheckSysEval("cat /dev/zero","",100000,false);// will fail because of output limit
    CheckSysEval("false","",100000,false);// will fail
    CheckSysEval("echo blah","blah\n",5,true);// check newline
}

// --------------------------------------------------------------------------------

static void TestTrimZeros(strptr const_str, strptr result) {
    tempstr str(const_str);
    strptr buf(str);
    algo::strptr_TrimZerosRight(buf);
    vrfy_(buf==result);
}

void atf_unit::unittest_algo_lib_TrimZerosRight() {
    TestTrimZeros(""          , "");
    TestTrimZeros("0"         , "0");
    TestTrimZeros("0.1"       , "0.1");
    TestTrimZeros("0.0"       , "0");
    TestTrimZeros("12345.000" , "12345");
    TestTrimZeros("-0"        , "-0");// not touched
    TestTrimZeros(".0"        , "0");
    TestTrimZeros("-.0"       , "0");
    TestTrimZeros("-0.0"      , "-0");
    TestTrimZeros("-10.0"     , "-10");
}

// --------------------------------------------------------------------------------

static void TestPrintWithCommas(strptr str, strptr result) {
    tempstr out;
    algo::strptr_PrintWithCommas(str,out);
    vrfy_(out==result);
}

void atf_unit::unittest_algo_lib_PrintWithCommas() {
    TestPrintWithCommas("0","0");
    TestPrintWithCommas("11","11");
    TestPrintWithCommas("222","222");
    TestPrintWithCommas("3333","3,333");
    TestPrintWithCommas("4567.3","4,567.3");
    TestPrintWithCommas("4567.3","4,567.3");
    TestPrintWithCommas("12341234123412341234123412341234","12,341,234,123,412,341,234,123,412,341,234");
    TestPrintWithCommas("2341234123412341234123412341234","2,341,234,123,412,341,234,123,412,341,234");
    TestPrintWithCommas("1.11111","1.11111");
}

// -----------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_FTruncate() {
    cstring s("1234567890");
    strptr fname("temp/truncated");
    DeleteFile(fname);
    StringToFile(s,fname);
    {
        algo::Fildes fildes=OpenWrite(fname,algo_FileFlags_append);
        TruncateFile(fildes,4);
        close(fildes.value);
        vrfy_(FileToString(fname) == "1234");
    }
}

// --------------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_GetCpuHz() {
    prlog("cpu hz: "<<algo::get_cpu_hz_int());
}

// --------------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_flock() {
    algo::Fildes fd1= algo::OpenWrite("lock/file1");
    vrfy_(ValidQ(fd1));
    algo::Fildes fd2= algo::OpenWrite("lock/file1");
    vrfy_(ValidQ(fd1));
    vrfy_(fd1.value != fd2.value);

    vrfy_(flock(fd1.value,LOCK_NB|LOCK_EX)==0);

    // exclusive lock should fail on the second file
    vrfy_(flock(fd2.value,LOCK_NB|LOCK_EX)==-1);

    // unlock via fd1
    vrfy_(flock(fd1.value,LOCK_UN)==0);

    // lock via fd2 -- must succeed
    vrfy_(flock(fd2.value,LOCK_NB|LOCK_EX)==0);

    close(fd1.value);
    close(fd2.value);
}
// --------------------------------------------------------------------------------

static void ShowVal(i64 val, U128 u128val) {
    (void)val;
    (void)u128val;
    // TODO
    // prerr(Keyval("val",val)
    //       <<Keyval("u128val",u128val));
}
#define vrfy_x(a,b) if (!(a)) { ShowVal(b,U128(b)); vrfy(0,#a); }

// Test that construging is the same as assigning
// And that resulting value matches
static void U128TestConstruct(i64 val) {
    if (val >= 0 && val <= 0x7fffffff) {
        U128 x = U128(u32(val));
        U128 y;
        y=val;
        vrfy_(u32(x)==val);
        vrfy_(u32(y)==val);
    }
    if (val > -0x7fffffff && val < 0x7fffffff) {
        U128 x = U128(i32(val));
        U128 y;
        y=val;
        vrfy_(i32(x)==val);
        vrfy_(i32(y)==val);
    }
    if (val > -0x7fffffffffffffff && val < 0x7fffffffffffffff) {
        U128 x = U128(i64(val));
        U128 y;
        y=val;
        vrfy_(u64(x)==u64(val));
        vrfy_(u64(y)==u64(val));
    }
}

static void u128_TestSmallIntConvert(i64 val) {
    // true for all numbers
    vrfy_x(U128(val) == U128(val), val);
    vrfy_x(U128(val) != U128(val+1), val);
    vrfy_x(U128(val) != u32(val+1), val);// always differs
    vrfy_x(U128(val) != u64(val+1), val);// always differs
    vrfy_x(u32(val+1) != U128(val), val);// always differs
    vrfy_x(u32(val+1) != U128(val), val);// always differs
    vrfy_x(U128(val) == U128(val), val);
    vrfy_x(U128(val) == ~~U128(val), val);

    if (val>=0) {
        // initialize to a value and read that value back
        vrfy_x(U128(u32(val)) == u32(val), val);
        vrfy_x(U128(u64(val)) == u64(val), val);
        // test with u128 on the right
        vrfy_x(u32(val) == U128(u32(val)), val);
        vrfy_x(u64(val) == U128(u64(val)), val);
    }
    if (val < 0) {
        // construct u128, convert back to unsigne,d test
        vrfy_x(-U128(i32(val)) == u32(-val), val);
        vrfy_x(-U128(i64(val)) == u64(-val), val);
    }
    U128TestConstruct(val);
    if (val > -0x7fffffff && val < 0x7fffffff) {
        // construct from i32 same as from i64
        vrfy_x(U128(i32(val)) == U128(i64(val)), val);
    }
    if (val >= 0 && val < 0x7fffffffffffffff) {
        for (int shift=0; shift<64; shift++) {
            vrfy_x((U128(val)>>shift)  == (u64(val)>>shift),val); // shifted right
        }
        vrfy_x(U128(val)   <  U128(val+1), val); // u128 vs u128
        vrfy_x(U128(val+1) >  U128(val), val);   // u128 vs u128
        vrfy_x(u64(U128(val))   <  u64(val+1), val);             // u128 vs i64
        // not supported
        vrfy_x(i64(val+1)             >  i64(u64(U128(val))), val);   // i64 vs u128

        vrfy_x(!(U128(val) <  U128(val)), val);  // number against itself
        vrfy_x(!(U128(val) >  U128(val)), val);  // same

        vrfy_x(U128(val)     == U128(val), val);  // number equals itself
        vrfy_x(!(U128(val)   != U128(val)), val); // doesn't differ from itself

        vrfy_x(!(U128(val)   == U128(val+1)), val); // different numbers don't equal
        vrfy_x(!(U128(val+1) == U128(val)), val);   // same

        U128 a = U128(val);
        U128 b = a;
        vrfy_x(a == b,val);
    }
    if (val >= 0 && val < 0x7fffffffffffffff/2) {
        // basic addition
        vrfy_x(U128(val) + U128(val) == U128(val+val), val);
    }
    if (val >= 0 && val < 0x7fffffffffffffff/2) {
        // #AL# TODO
        // division by numberes higher than u32 doesn't work
        for (u64 arg=1; arg <= 0x7fffffffffff; arg = arg*2) {
            // multiply and divide by same number, should get same number back
            vrfy_(U128(val)*arg / arg == U128(val));
            // add and sutract same number - should get the same result
            vrfy_x(U128(val) + U128(arg) - U128(arg) == U128(val), val);
        }
    }
    // conversion to bool
    vrfy_x(bool(U128(val)) == (val!=0), val);
}

static U128 U128Rand() {
    U128 ret;
    for (int i=0; i<8; i++) {
        ret  = (ret<<u32(16)) | rand();
    }
    return ret;
}

void atf_unit::unittest_algo_lib_u128() {
    u128_TestSmallIntConvert(0);
    u128_TestSmallIntConvert(-1);
    u128_TestSmallIntConvert(1);
    u128_TestSmallIntConvert(0x7fffffff);
    u128_TestSmallIntConvert(0xffffffffffffffff);
    u128_TestSmallIntConvert(0x8000000000000000);
    u128_TestSmallIntConvert(0x8000000000000000);

    {
        // set and clear all bits 1 through 127
        U128 a(1);
        for (int i=1; i<=127; i++) {
            U128 b = U128(i64(1)) << i;
            //verblog(Keyval("i",i)<<Keyval("a",a)<<Keyval("b",b));
            vrfy_((a | b)  != a);
            vrfy_((a & ~b) == a);
            vrfy_((a & b) == U128(i32(0)));
            vrfy_(((a | b) & ~b) == a);
            vrfy_((b << 0) == b);
            vrfy_((b >> 0) == b);

            // add = shift left by 1
            vrfy_((U128(1)<<(i-1)) + (U128(1)<<(i-1)) == (U128(1)<<i));
            // so does multiply by 2
            {
                U128 s1_0 = U128(1)<<(i-1);
                U128 s1 = s1_0*u32(2);
                U128 s2 = (U128(1)<<i);
                // verblog("i:"<<i
                //         <<"  s1_0:"<<s1_0
                //         <<"  s1:"<<s1
                //         <<"  s2:"<<s2);
                vrfy_(s1 == s2);
            }
            // non-zero
            vrfy_(bool(U128(1)<<i) == true);
        }
        vrfy_(bool(U128(0)) == false);
    }

    for (int i=0; i<10000; i++) {
        U128 a = U128Rand();
        U128 b = U128Rand();
        // test symmetry of bitwise operators
        vrfy_((a & b) == (b & a));
        vrfy_((a | b) == (b | a));

        vrfy_(U128(u64(a))*U128(u64(b)) / U128(u64(b)) == U128(u64(a)));

        // test that in-place operators match value operators
        {
            U128 c = a;
            c &= b;
            vrfy_(c == (a & b));
        }
        {
            U128 c = a;
            c |= b;
            vrfy_(c == (a | b));
        }
        // shift number left/right, should stay the same
        {
            U128 c(a);
            c<<=1;
            c>>=2;
            c<<=1;
            U128 d(a);
            d = d & ~U128(1);
            d = d & ~(U128(1)<<127);
            vrfy_(c==d);
        }

        // test that shift in-place matches shift by value
        for (int shift=0; shift<128; shift++) {
            U128 c = a;
            c>>=shift;
            vrfy_((a>>i32(shift)) == c);
            vrfy_((a>>u32(shift)) == c);
            vrfy_((a>>u64(shift)) == c);
            c = a;
            c<<=shift;
            vrfy_((a<<i32(shift)) == c);
            vrfy_((a<<u32(shift)) == c);
            vrfy_((a<<u64(shift)) == c);
        }
    }

}
// --------------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_Mmap() {
    strptr fname("temp/mmapfile");
    // initial state
    DeleteFile(fname);

    // write string to file
    StringToFile("sample text",fname);
    algo_lib::MmapFile mmapfile;
    algo_lib::FFildes readfd;
    readfd.fd = OpenRead(fname);
    vrfy_(MmapFile_LoadFd(mmapfile,readfd.fd));

    // check that file view sees the string
    vrfy_(mmapfile.text == "sample text");

    // write to file via OS file api
    {
        algo_lib::FFildes writefd;
        writefd.fd = OpenWrite(fname,algo_FileFlags_write|algo_FileFlags_read);
        lseek(writefd.fd.value,0,SEEK_SET);
        ssize_t ret_val = write(writefd.fd.value, "123456", 6);
        (void) ret_val;
    }

    // file mapping should have been updated
    vrfy_(mmapfile.text == "123456 text");
}
// --------------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_FileQ() {
    strptr fname("temp/linktest");
    // clear state
    DeleteFile(fname);
    prlog("phase 1");
    // file must not exist
    vrfy_(!FileQ(fname));
    vrfy_(!FileObjectExistsQ(fname));

    prlog("phase 2");
    // create a regular file
    StringToFile("x",fname);
    // check that it exists
    vrfy_(FileQ(fname));
    vrfy_(FileObjectExistsQ(fname));

    // clear again
    algo::DeleteFile(fname);
    algo::DeleteFile("temp/non-existent");
    prlog("phase 3");
    // create a symlink to a non-existent entity
    vrfy_(0==symlink(Zeroterm(tempstr()<<"non-existent"),Zeroterm(tempstr()<<fname)));
    vrfy_(FileObjectExistsQ(fname));
    // unfortunate platform difference
#ifdef WIN32
    vrfy_(FileQ(fname));
#else
    vrfy_(!FileQ(fname));
#endif
    vrfy_(algo::FileToString(algo::GetFullPath(fname),algo::FileFlags())=="");

    DeleteFile(fname);
    prlog("phase 4");
    algo::StringToFile("x","temp/target");
    vrfy_(0==symlink("target",Zeroterm(tempstr()<<fname)));
    vrfy_(FileObjectExistsQ(fname));
    vrfy_(FileQ(fname));
    prlog(fname);
    // under cygwin, FileToString(<symlink>) would return
    // the contents of the symlink file
    vrfy_(algo::FileToString(algo::GetFullPath(fname),algo::FileFlags())=="x");
}
// --------------------------------------------------------------------------------

static void CheckExitCode(algo::strptr cmd, int code, int exit_code, int exit_signal) {
    command::bash_proc bash;
    bash.cmd.c = cmd;
    int rc = bash_Exec(bash);
    vrfyeq_(rc,code);
    algo_lib::ExportWaitStatus(rc);
    vrfyeq_(algo_lib::_db.exit_code,exit_code);
    vrfyeq_(algo_lib::_db.exit_signal,exit_signal);
}

// _Exec returns the raw wait status; ExportWaitStatus decomposes it into the
// process's exit facts: exit_code carries the shell-convention code (exit N
// -> N, death by signal -> 128+signal), exit_signal carries the terminating
// signal alone, 0 when the child exited.
void atf_unit::unittest_algo_lib_ExitCode() {
    int save_code = algo_lib::_db.exit_code;
    int save_signal = algo_lib::_db.exit_signal;
    CheckExitCode("true", 0, 0, 0);
    CheckExitCode("false", 256, 1, 0);
    CheckExitCode("exit 3", 3*256, 3, 0);
    CheckExitCode("kill $$", 15, 143, 15);
    algo_lib::_db.exit_code = save_code;
    algo_lib::_db.exit_signal = save_signal;
}

// --------------------------------------------------------------------------------

static void CreateProcessTreeRecurse(int width, int levels, int sleep_sec) {
    if (levels>0) {
        frep_(i,width) {
            if (!fork()) {
                CreateProcessTreeRecurse(width,levels-1,sleep_sec);
                sleep(sleep_sec);
                exit(0);
            }
        }
    }
}

void atf_unit::unittest_algo_lib_KillRecurse() {
#ifdef __linux__
    CreateProcessTreeRecurse(2,3,5);
    sleep(1); // give some time to establish process tree
    // the walk is what the assertion measures, so it runs once, outside the
    // macro that re-evaluates its arguments to build the failure message
    int nproc_probe = algo_lib::KillRecurse(getpid(),0,true);
    vrfyeq_(15,nproc_probe);
    int nproc_kill = algo_lib::KillRecurse(getpid(),SIGKILL,false);
    vrfyeq_(14,nproc_kill);
#else
    (void)CreateProcessTreeRecurse;
#endif
}
// --------------------------------------------------------------------------------

// check that all characters print from memptr, and get parsed
// back as a string
void atf_unit::unittest_algo_lib_PrintMemptr() {
    algo::ByteAry bytes;
    for (int i=0; i<256; i++) {
        ary_Alloc(bytes)=i;
    }
    tempstr out;
    algo::strptr_PrintSsim(memptr_ToStrptr(ary_Getary(bytes)), out);
    prlog("out: "<<out);
    cstring in;
    algo::StringIter iter(out);
    vrfy_(cstring_ReadCmdarg(in,iter,true));
    prlog("in: "<<strptr_ToSsim(in));
    vrfy_(in == strptr((char*)bytes.ary_elems,bytes.ary_n));
}

bool atf_unit::Smallstr150_Eq(const algo::Smallstr150 & lhs,const algo::Smallstr150 & rhs) {
    bool retval = true;
    retval = algo::strptr_Eq(ch_Getary(lhs), ch_Getary(rhs));
    return retval;
}

// --------------------------------------------------------------------------------

static void TestStringEq(algo::strptr a, algo::strptr b, bool eq) {
    if (ch_N(a)<=2 && ch_N(b)<=2) vrfyeq_(algo::RspaceStr2(a) == algo::RspaceStr2(b), eq);
    if (ch_N(a)<=7 && ch_N(b)<=7) vrfyeq_(algo::RspaceStr7(a) == algo::RspaceStr7(b), eq);

    if (ch_N(a)<=3 && ch_N(b)<=3) vrfyeq_(algo::LspaceStr3(a) == algo::LspaceStr3(b), eq);
    if (ch_N(a)<=4 && ch_N(b)<=4) vrfyeq_(algo::LspaceStr4(a) == algo::LspaceStr4(b), eq);
    if (ch_N(a)<=5 && ch_N(b)<=5) vrfyeq_(algo::LspaceStr5(a) == algo::LspaceStr5(b), eq);
    if (ch_N(a)<=6 && ch_N(b)<=6) vrfyeq_(algo::LspaceStr6(a) == algo::LspaceStr6(b), eq);
    if (ch_N(a)<=8 && ch_N(b)<=8) vrfyeq_(algo::LspaceStr8(a) == algo::LspaceStr8(b), eq);
    if (ch_N(a)<=8 && ch_N(b)<=8) vrfyeq_(algo::LspaceStr8(a) == algo::LspaceStr8(b), eq);
    if (ch_N(a)<=9 && ch_N(b)<=9) vrfyeq_(algo::LspaceStr9(a) == algo::LspaceStr9(b), eq);

    if (ch_N(a)<=1 && ch_N(b)<=1) vrfyeq_(algo::RnullStr1(a) == algo::RnullStr1(b), eq);
    if (ch_N(a)<=2 && ch_N(b)<=2) vrfyeq_(algo::RnullStr2(a) == algo::RnullStr2(b), eq);
    if (ch_N(a)<=3 && ch_N(b)<=3) vrfyeq_(algo::RnullStr3(a) == algo::RnullStr3(b), eq);
    if (ch_N(a)<=4 && ch_N(b)<=4) vrfyeq_(algo::RnullStr4(a) == algo::RnullStr4(b), eq);
    if (ch_N(a)<=5 && ch_N(b)<=5) vrfyeq_(algo::RnullStr5(a) == algo::RnullStr5(b), eq);
    if (ch_N(a)<=6 && ch_N(b)<=6) vrfyeq_(algo::RnullStr6(a) == algo::RnullStr6(b), eq);
    if (ch_N(a)<=7 && ch_N(b)<=7) vrfyeq_(algo::RnullStr7(a) == algo::RnullStr7(b), eq);
    if (ch_N(a)<=8 && ch_N(b)<=8) vrfyeq_(algo::RnullStr8(a) == algo::RnullStr8(b), eq);
    if (ch_N(a)<=9 && ch_N(b)<=9) vrfyeq_(algo::RnullStr9(a) == algo::RnullStr9(b), eq);

    if (ch_N(a)<=100 && ch_N(b)<=100) vrfyeq_(algo::Smallstr100(a) == algo::Smallstr100(b), eq);
}

// -----------------------------------------------------------------------------

static bool strptr_Lt2(algo::strptr a, algo::strptr b) {
    int n = i32_Min(a.n_elems,b.n_elems);
    for (int i=0; i<n; i++) {
        if (a[i] != b[i]) return a[i]<b[i];
    }
    return a.n_elems < b.n_elems;
}

// -----------------------------------------------------------------------------

static tempstr RandomString(int n) {
    tempstr ret;
    frep_(i,n) {
        ch_Alloc(ret)=32+i32_WeakRandom(64);
    }
    return ret;
}

// -----------------------------------------------------------------------------

static void TestLt(algo::strptr a, algo::strptr b) {
    bool lt = strptr_Lt(a,b);
    bool lt2=strptr_Lt2(a,b);
    vrfy_(lt==lt2);
    vrfy_(lt != strptr_Lt(b,a));
    vrfy_(lt != strptr_Lt2(b,a));
}

// -----------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_SmallstrEq() {
    // test strptr_Lt against a straightforward implementation
    for (int i=0; i<10000; i++) {
        tempstr a(RandomString(64));
        tempstr b(RandomString(64));
        TestLt(a,b);
        vrfy_(!strptr_Lt(a,a));
        TestLt("a","aa");
        TestLt("a","b");
        TestLt("","a");
    }

    // test equality of some sample strings
    TestStringEq("a","A",false);
    TestStringEq("a","a",true);
    TestStringEq("aa","aa",true);
    TestStringEq("aa","ab",false);
    TestStringEq("aaa","aaa",true);
    TestStringEq("aaa","aab",false);
    TestStringEq("123412341234","123412341234",true);
    TestStringEq("123412341234","123412341233",false);
    TestStringEq("","123412341233",false);
    TestStringEq("","",true);

    // test performance of strptr_Eq
    {
        cstring a;
        frep_(i,128) {
            ch_Alloc(a)='a'+i32_WeakRandom(32);
        }
        char cc=a.ch_elems[0];
        cstring b(a);
        int x=0;
        DO_PERF_TEST("algo::strptr eq 128-byte equal strings",{
                x += strptr_Eq(a,b);
                a.ch_elems[0]=cc;
            });
        prlog(x);
        x=0;
        DO_PERF_TEST("strncmp 128-byte equal strings ",{
                x += strncmp(a.ch_elems,b.ch_elems,i32_Min(a.ch_n,b.ch_n))==0;
                a.ch_elems[0]=cc;
            });
        prlog(x);
    }

    // looks like RnullStr8 equality is about 2.5x faster than strptr equality
    algo::RnullStr8 a("3333300");
    algo::RnullStr8 b("3333333");;
    int i=0;
    DO_PERF_TEST("RnullStr8 eq",frep_(h,1000){i+= RnullStr8_Eq(a,b); a.ch[6]++;});
    prlog(i);
    i=0;
    DO_PERF_TEST("strptr eq",frep_(h,1000){i+= algo::strptr_Eq(ch_Getary(a),ch_Getary(b)); a.ch[6]++;});
    prlog(i);
    i=0;
    DO_PERF_TEST("strcmp",frep_(h,1000){i+= strcmp((char*)a.ch,(char*)b.ch)==0; a.ch[6]++;});
    prlog(i);
}

void atf_unit::unittest_algo_lib_ReverseBits() {
    vrfyeq_(algo::u8_ReverseBits(0x00),0x00);
    vrfyeq_(algo::u8_ReverseBits(0x01),0x80);
    vrfyeq_(algo::u8_ReverseBits(0x02),0x40);
    vrfyeq_(algo::u8_ReverseBits(0x04),0x20);
    vrfyeq_(algo::u8_ReverseBits(0x08),0x10);
    vrfyeq_(algo::u8_ReverseBits(0x10),0x08);
    vrfyeq_(algo::u8_ReverseBits(0x20),0x04);
    vrfyeq_(algo::u8_ReverseBits(0x40),0x02);
    vrfyeq_(algo::u8_ReverseBits(0x80),0x01);
    vrfyeq_(algo::u8_ReverseBits(0xff),0xff);
    vrfyeq_(algo::u8_ReverseBits(0xfe),0x7f);
    vrfyeq_(algo::u8_ReverseBits(0xfd),0xbf);
    vrfyeq_(algo::u8_ReverseBits(0xfb),0xdf);
    vrfyeq_(algo::u8_ReverseBits(0xf7),0xef);
    vrfyeq_(algo::u8_ReverseBits(0xef),0xf7);
    vrfyeq_(algo::u8_ReverseBits(0xdf),0xfb);
    vrfyeq_(algo::u8_ReverseBits(0xbf),0xfd);
    vrfyeq_(algo::u8_ReverseBits(0x7f),0xfe);
    vrfyeq_(algo::u8_ReverseBits(0x0f),0xf0);
    vrfyeq_(algo::u8_ReverseBits(0xf0),0x0f);
    vrfyeq_(algo::u8_ReverseBits(0x33),0xCC);
    vrfyeq_(algo::u8_ReverseBits(0xCC),0x33);
    vrfyeq_(algo::u8_ReverseBits(0x55),0xaa);
    vrfyeq_(algo::u8_ReverseBits(0xaa),0x55);
}

static void Zigzag32(i32 value, strptr hex) {
    {
        algo::ByteAry buf;
        algo::EncodeVLCLEI32Z(buf,value);
        tempstr out = tempstr() << ary_Getary(buf);
        vrfyeq_(out,hex);
    }
    {
        i32 decoded(0);
        algo::ByteAry buf;
        ByteAry_ReadStrptrMaybe(buf,hex);
        algo::memptr ptr = ary_Getary(buf);
        algo::DecodeVLCLEI32Z(ptr,decoded);
        vrfy_(!elems_N(ptr));
        vrfyeq_(decoded,value);
    }
}

static void Zigzag64(i64 value, strptr hex) {
    {
        algo::ByteAry buf;
        algo::EncodeVLCLEI64Z(buf,value);
        tempstr out = tempstr() << ary_Getary(buf);
        vrfyeq_(out,hex);
    }
    {
        i64 decoded(0);
        algo::ByteAry buf;
        ByteAry_ReadStrptrMaybe(buf,hex);
        algo::memptr ptr = ary_Getary(buf);
        algo::DecodeVLCLEI64Z(ptr,decoded);
        vrfy_(!elems_N(ptr));
        vrfyeq_(decoded,value);
    }
}

void atf_unit::unittest_algo_lib_Zigzag() {
    Zigzag32(0,"00");
    Zigzag32(-1,"01");
    Zigzag32(1,"02");
    Zigzag32(-2,"03");
    Zigzag32(2, "04");
    Zigzag32(1000,"d0 0f");
    Zigzag32(-1000,"cf 0f");
    Zigzag32(0x7fffffff,"fe ff ff ff 0f");
    Zigzag32(-0x80000000,"ff ff ff ff 0f");

    Zigzag64(0,"00");
    Zigzag64(-1,"01");
    Zigzag64(1,"02");
    Zigzag64(-2,"03");
    Zigzag64(2, "04");
    Zigzag64(1000,"d0 0f");
    Zigzag64(-1000,"cf 0f");
    //Zigzag64(0x7fffffffffffffff,"fe ff ff ff ff ff ff ff ff 01");
    Zigzag64(-0x8000000000000000,"ff ff ff ff ff ff ff ff ff 01");
}
// --------------------------------------------------------------------------------

// Test that a file opened in append mode always writes at the end
// regardless of file position
void atf_unit::unittest_algo_lib_FileAppend() {
    algo::strptr fname = "temp/appendfile";
    DeleteFile(fname);
    algo::Fildes fildes = algo::OpenFile(fname, algo_FileFlags_append);
    WriteFile(fildes, (u8*)"blah", 4);
    SeekFile(fildes, 2);
    WriteFile(fildes, (u8*)"bleh", 4);
    vrfy(FileToString(fname)=="blahbleh", "append mode doesn't work");
}

// --------------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_Url() {
    vrfyeq_(algo::UrlDecode("///012abcdefACDEF%21%22%234%%%1z+",false),"///012abcdefACDEF!\"#4%%%1z+");
    algo::Tuple tuple;
    ParseUrl(tuple,"/api/v1/stream?beg=100&end=inf&par%201=Hello%20World!&flag&par2=John%26Co.");
    vrfyeq_(tempstr()<<tuple,"/api/v1/stream  beg:100  end:inf  \"par 1\":\"Hello World!\"  flag:\"\"  par2:John&Co.");
}

// --------------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_Blkpool() {
    atf_unit::FMsg& msg = atf_unit::msg_AllocExtra(NULL, 123);
    atf_unit::msg_Delete(msg);
}

// --------------------------------------------------------------------------------

#ifndef WIN32
// Read FD to EOF and return its contents.
static algo::tempstr ReadAllFd(algo::Fildes fd) {
    algo::tempstr out;
    char buf[256];
    ssize_t nread;
    while ((nread = read(fd.value, buf, sizeof(buf))) > 0) {
        out << algo::strptr(buf, (int)nread);
    }
    return out;
}

// Write DATA into the child's stdin pipe and close it so the child sees EOF.
static void PumpStdin(command::bash_proc &p, algo::strptr data) {
    algo::WriteFile(p.to_stdin, (u8*)data.elems, data.n_elems);
    algo_lib::Close(p.to_stdin);
}
#endif

// Spawn an arbitrary external process (here `cat` and `false`) directly through
// algo_lib::FProc -- no amc-generated _proc type. Build argv into proc.args,
// set redirects, and drive the lifecycle with the generic ProcStart/Wait/Exec.
// This is the path one would use to spawn `ssh` or any other external command.
#ifndef WIN32
// Spawn a long-lived proc holding a stdout-pipe read end (with the given cloexec
// setting), then spawn a second child that lists its own open fds, and report
// whether the held pipe leaked into that second child.  Matches the pipe's
// unique "pipe:[inode]" name rather than the fd number, so the lister's own dir
// fd can't cause a false positive.
static bool PipeFdLeaksQ(bool cloexec) {
    algo_lib::FProc longproc;
    ary_Alloc(longproc.args) = "sleep";
    ary_Alloc(longproc.args) = "30";
    longproc.fstdout = "|";
    longproc.cloexec = cloexec;
    algo_lib::ProcStart(longproc);
    tempstr linkpath;
    linkpath << "/proc/self/fd/" << longproc.from_stdout.value;
    char target[64];
    ssize_t n = readlink(Zeroterm(linkpath), target, sizeof(target)-1);
    tempstr pipename;
    if (n > 0) {
        pipename = algo::strptr(target, (int)n);
    }
    algo_lib::FProc lsproc;
    ary_Alloc(lsproc.args) = "sh";
    ary_Alloc(lsproc.args) = "-c";
    ary_Alloc(lsproc.args) = "ls -l /proc/self/fd";
    lsproc.fstdout = "|";
    algo_lib::ProcStart(lsproc);
    algo::tempstr listing = ReadAllFd(lsproc.from_stdout);
    algo_lib::ProcWait(lsproc);
    algo_lib::ProcKill(longproc);
    return ch_N(pipename) > 0 && algo::FindStr(listing, pipename) != -1;
}
#endif

void atf_unit::unittest_algo_lib_FProc() {
#ifndef WIN32
    algo::strptr data = "the quick brown fox\n";

    // round-trip data through `cat` with stdin + stdout pipes
    {
        algo_lib::FProc proc;
        ary_Alloc(proc.args) = "cat";
        proc.fstdin  = "|";
        proc.fstdout = "|";
        algo_lib::ProcStart(proc);
        algo::WriteFile(proc.to_stdin, (u8*)data.elems, data.n_elems);
        algo_lib::Close(proc.to_stdin); // EOF -> cat exits
        algo::tempstr out;
        char buf[256];
        ssize_t nread;
        while ((nread = read(proc.from_stdout.value, buf, sizeof(buf))) > 0) {
            out << algo::strptr(buf, (int)nread);
        }
        algo_lib::ProcWait(proc);
        vrfyeq_(out, data);
        TESTCMP(algo_lib::ProcExitCode(proc), 0);
    }

    // exit code is collected from a non-zero exit
    {
        algo_lib::FProc proc;
        ary_Alloc(proc.args) = "false";
        TESTCMP(algo_lib::ProcExec(proc) != 0, true);
        TESTCMP(algo_lib::ProcExitCode(proc), 1);
    }

    // args[0] stays the command name (not the PATH-resolved pathname): the child
    // sees its own name, so error messages read "bash:" not "/usr/bin/bash:".
    {
        algo_lib::FProc proc;
        ary_Alloc(proc.args) = "bash";
        ary_Alloc(proc.args) = "-c";
        ary_Alloc(proc.args) = "echo $0";
        proc.fstdout = "|";
        algo_lib::ProcStart(proc);
        algo::tempstr out;
        char buf[256];
        ssize_t nread;
        while ((nread = read(proc.from_stdout.value, buf, sizeof(buf))) > 0) {
            out << algo::strptr(buf, (int)nread);
        }
        algo_lib::ProcWait(proc);
        vrfyeq_(algo::Trimmed(out), "bash");
    }

    // cloexec (default true) keeps the parent's pipe fd out of later-spawned
    // children; cloexec=false lets it leak in.
    TESTCMP(PipeFdLeaksQ(true), false);
    TESTCMP(PipeFdLeaksQ(false), true);
#endif
}

// Pump a bit of data through cat/tee and verify it round-trips through every
// amc-generated _proc pipe redirect: stdin, stdout, stderr, merged stdout+stderr
// (fstdout="|" + fstderr=">&1"), and all three at once.
// Source of truth: amc::tfunc_Exec_Start in cpp/amc/exec.cpp.
void atf_unit::unittest_algo_lib_ExecPipe() {
#ifndef WIN32
    algo::strptr data = "the quick brown fox\n";

    // 1. stdin pipe: feed via to_stdin; child writes stdout to a file we read back
    {
        algo::strptr fname = "temp/algo_lib_execpipe.out";
        DeleteFile(fname);
        command::bash_proc p;
        p.cmd.c   = "cat";
        p.fstdin  = "|";
        p.fstdout = tempstr() << ">" << fname;
        bash_Start(p);
        PumpStdin(p, data);
        bash_Wait(p);
        TESTCMP(p.status, 0);
        vrfyeq_(algo::FileToString(fname), data);
        DeleteFile(fname);
    }

    // 2. stdout pipe: child cats stdin to stdout; parent reads from_stdout
    {
        command::bash_proc p;
        p.cmd.c   = "cat";
        p.fstdin  = "|";
        p.fstdout = "|";
        bash_Start(p);
        PumpStdin(p, data);
        vrfyeq_(ReadAllFd(p.from_stdout), data);
        bash_Wait(p);
        TESTCMP(p.status, 0);
    }

    // 3. stderr pipe: child cats stdin to stderr; parent reads from_stderr
    {
        command::bash_proc p;
        p.cmd.c   = "cat 1>&2";
        p.fstdin  = "|";
        p.fstderr = "|";
        bash_Start(p);
        PumpStdin(p, data);
        vrfyeq_(ReadAllFd(p.from_stderr), data);
        bash_Wait(p);
        TESTCMP(p.status, 0);
    }

    // 4. stdout+stderr merged: tee writes data to stdout and to /dev/stderr, and
    //    fstderr=">&1" folds stderr into the stdout pipe, so it arrives twice
    {
        command::bash_proc p;
        p.cmd.c   = "tee /dev/stderr";
        p.fstdin  = "|";
        p.fstdout = "|";
        p.fstderr = ">&1";
        bash_Start(p);
        PumpStdin(p, data);
        vrfyeq_(ReadAllFd(p.from_stdout), tempstr() << data << data);
        bash_Wait(p);
        TESTCMP(p.status, 0);
    }

    // 5. all three pipes: tee splits stdin to the stdout pipe and (via
    //    /dev/stderr) the separate stderr pipe
    {
        command::bash_proc p;
        p.cmd.c   = "tee /dev/stderr";
        p.fstdin  = "|";
        p.fstdout = "|";
        p.fstderr = "|";
        bash_Start(p);
        PumpStdin(p, data);
        algo::tempstr out = ReadAllFd(p.from_stdout);
        algo::tempstr err = ReadAllFd(p.from_stderr);
        vrfyeq_(out, data);
        vrfyeq_(err, data);
        bash_Wait(p);
        TESTCMP(p.status, 0);
    }
#endif
}

// -----------------------------------------------------------------------------

// The calibration this process runs on came from the kernel: where the
// kernel exports a TSC rate, algo_lib::_db.hz is that exact figure, and
// RequireKernelCpuHz admits the process.  Exact equality is the point of
// the test -- a rate derived any other way (timed against a wall clock, or
// read off a P-state file) lands near the kernel's figure rather than on
// it, so an approximate comparison would pass for the very values the
// kernel export exists to exclude.
//
// A host with no export has nothing to compare against, so the check is
// announced as skipped rather than passing silently.  That host is exactly
// where an x2 process refuses to start unless the rate is stated, and where
// the file source /etc/x2.d/tscfreq_khz is exercised -- installing that file
// needs root, so it is covered on such a host rather than here.  The second
// half of the test covers the variable on either kind of host:
// where the export exists it must be ignored, and where it does not it must be
// installed exactly as written.  The rate used is 2.5GHz, a figure inside the
// range ApplyCpuHz admits and unequal to any host's real rate, so a variable
// that was silently ignored cannot pass for one that was honored.  The
// process runs on the restored calibration afterwards, so the original is put
// back before the test returns.
void atf_unit::unittest_algo_lib_RequireKernelCpuHz() {
    i32 freq_khz = 0;
    tempstr value(algo::FileToString("/sys/devices/system/cpu/cpu0/tsc_freq_khz", algo::FileFlags()));
    algo::StringIter iter(value);
    (void)TryParseI32(iter, freq_khz);
    bool exported = freq_khz > 0;
    double orig_hz = algo_lib::_db.hz;
    prlog("atf_unit.kernel_cpu_hz"
          <<Keyval("exported",exported)
          <<Keyval("kernel_hz",u64(double(freq_khz)*1e3))
          <<Keyval("db_hz",u64(algo_lib::_db.hz)));
    if (exported) {
        algo_lib::RequireKernelCpuHz("/nonexistent/tscfreq_khz", "ATF_UNIT_TSCFREQKHZ");
        vrfy_(algo_lib::_db.hz == double(freq_khz)*1e3);
    }
    setenv("ATF_UNIT_TSCFREQKHZ","2500000",1);
    algo_lib::RequireKernelCpuHz("/nonexistent/tscfreq_khz", "ATF_UNIT_TSCFREQKHZ");
    if (exported) {
        vrfy_(algo_lib::_db.hz == double(freq_khz)*1e3);
    } else {
        vrfy_(algo_lib::_db.hz == 2500000.0*1e3);
    }
    unsetenv("ATF_UNIT_TSCFREQKHZ");
    algo_lib::ApplyCpuHz(orig_hz);
}

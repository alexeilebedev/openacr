// (C) 2018-2019 NYSE | Intercontinental Exchange
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
// Source: cpp/atf/unit/string.cpp
//
// Created By: alexei.lebedev
// Recent Changes: alexei.lebedev
//

#include "include/atf_unit.h"


// -----------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_SubstringIndex() {
    vrfy_(algo::SubstringIndex("",'.',3) == "");
    vrfy_(algo::SubstringIndex("a.",'.',1) == "a");
    vrfy_(algo::SubstringIndex("a.b",'.',1) == "a");
    vrfy_(algo::SubstringIndex("a.b.c",'.',-1) == "c");
    vrfy_(algo::SubstringIndex("a.b.c",'.',-2) == "b.c");
    vrfy_(algo::SubstringIndex("a.b.c",'.',-3) == "a.b.c");
    vrfy_(algo::SubstringIndex("a.b.c",'.',1) == "a");
    vrfy_(algo::SubstringIndex("a.b.c",'.',2) == "a.b");
    vrfy_(algo::SubstringIndex("a.b.c",'.',3) == "a.b.c");
}

// -----------------------------------------------------------------------------

static void CheckAligned(const char *in_a, const char *in_b, int result) {
    cstring a;
    cstring b;
    a=in_a;
    b=in_b;
    vrfy_(AlignedEqual(a,b)==result);
}

void atf_unit::unittest_algo_lib_Aligned() {
    CheckAligned("abcd", "abcd", 1);
    CheckAligned("abcd", "abce", 0);
    CheckAligned("0123456789ABCDEFkyky", "0123456789ABCDEFkyky", 1);
    CheckAligned("0123456789ABCDEFkyky", "0123456789ABCDEFkyk", 0);
    CheckAligned("0123456789ABCDEFkyky", "0123456789ABCDEFkyyk", 0);
    CheckAligned("", "", 1);
}

// -----------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_CString() {
    {
        cstring a("test");
        vrfy_(ch_N(a) == 4);

        const char *t = "test";
        vrfy_(strptr(a) == t);
        vrfy_(!(strptr(a)!=strptr(t)));

        strptr b("t");
        vrfy_(b<strptr(a));
        vrfy_(!(strptr(a)==strptr(b)));

        cstring c("u");
        vrfy_(!(strptr(a)==strptr(c)));

        a = "T";
        a<<'c';
        a<<'t';
        a<<'i';
        vrfy_(a == "Tcti");
    }
    // Test printing i64
    {
        cstring s;
        s<<i64(2345234523452345234LL);
        vrfy_(s == "2345234523452345234");
    }
    {
        cstring a("abcdefghijklmnopqrstuvwxyz");
        vrfy_(a == a);
        vrfy_(a == "abcdefghijklmnopqrstuvwxyz");
        vrfy_("a" < a);
    }
    // Test printing ints and float
    {
        cstring b;
        b="";
        int i = 14;
        b<<i;
        vrfy_(b=="14");

        float f=1.3f;
        ch_RemoveAll(b);
        b<<f;
        vrfy_(b=="1.3");
    }

    {
        cstring b("bbbbb");
        tempstr x;
        x<<b;
        cstring a(x);
        vrfy_(a=="bbbbb");
        vrfy_(x=="")// stolen!
            }

    {
        strptr x("xxx");
        cstring X(x);
        X<<"yyy";
        vrfy_(cstring(x)=="xxx");
        vrfy_(X=="xxxyyy");
    }
    // Check temporary strings.
    {
        tempstr a;
        a<<strptr("bbbb");
        tempstr b;
        b<<((char*)"xxxxx");
    }
    {
        cstring test("abcdefgh");
        cstring test2;
        test2 = ch_FirstN(test,5);
        vrfy_(test2=="abcde");
        test=test2;
        test2 = ch_LastN(test,2);
        vrfy_(test2=="de");
    }
    {
        cstring test("abcdefgh");
        cstring test2;
        test2 = ch_FirstN(test,5);
        vrfy_(test2=="abcde");
    }
    {
        cstring result("test TEST TeSt");
        Replace(result, "test", "check", false, true);
        vrfy_(result==strptr("check CHECK ChEck"));
    }

    {
        cstring S("This is a string");
        S.ch_n = 4;
        vrfy_(S=="This");
    }


    {// Check trimming.
        vrfy_(algo::TrimmedRight("  test test ") == "  test test");
        vrfy_(algo::TrimmedLeft ("  test test ") == "test test ");
        vrfy_(algo::Trimmed     ("  test test ") == "test test");
        vrfy_(algo::LimitLengthEllipsis("123456789",9) == "123456789");
        vrfy_(algo::LimitLengthEllipsis("123456789",8) == "12345...");
        vrfy_(algo::LimitLengthEllipsis("123456789",1) == ".");
        vrfy_(algo::LimitLengthEllipsis("123456789",4) == "1...");
    }

    {
        cstring s;
        ch_Addary(s, strptr((char*)"abc",3));
        vrfy_(s=="abc");
    }
}

// --------------------------------------------------------------------------------

static tempstr Reversed(strptr s) {
    tempstr ret;
    ch_Reserve(ret,s.n_elems);
    for (int i=0; i<s.n_elems; i++) {
        ret.ch_elems[s.n_elems-1-i]=s.elems[i];
    }
    ret.ch_n = s.n_elems;
    return ret;
}

// --------------------------------------------------------------------------------

static void CheckFindFirst(strptr str, char c, i32_Range expect) {
    int offset = expect.beg==expect.end ? -1 : expect.beg;

    vrfy_(algo::ch_FindFirst(str,c) == expect);
    vrfy_(algo::ch_FindLast(Reversed(str),c) == i32_Range(str.n_elems-expect.end,str.n_elems-expect.beg));
    vrfy_(algo::FindStr(str,tempstr()<<c) == offset);
    vrfy_(algo::FindFrom(str,tempstr()<<c,0) == offset);
    vrfy_(algo::FindFrom(str,c,0) == offset);
    vrfy_(algo::FindChar(str,c) == offset);
    if (expect.beg == str.n_elems -1 && expect.end == str.n_elems) {
        vrfy_(algo::EndsWithQ(str,tempstr()<<c));
    }
    if (expect.beg == 0 && expect.end == 1) {
        vrfy_(algo::StartsWithQ(str,tempstr()<<c));
    }
    {
        StringIter iter(str);
        vrfy_(GetTokenStrptr(iter,tempstr()<<c)==FirstN(str,expect.beg));
    }
}

// --------------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_StringFind() {
    CheckFindFirst("a",'a', i32_Range(0,1));
    CheckFindFirst("a",'b', i32_Range(1,1));
    CheckFindFirst("",'b', i32_Range(0,0));
    CheckFindFirst("aa",'a', i32_Range(0,1));
    CheckFindFirst("aaa",'a', i32_Range(0,1));
    CheckFindFirst("bbba",'a', i32_Range(3,4));
}

// -----------------------------------------------------------------------------

static tempstr ToLower(strptr s) {
    tempstr ret(s);
    MakeLower(ret);
    return ret;
}

static tempstr ToUpper(strptr s) {
    tempstr ret(s);
    MakeUpper(ret);
    return ret;
}

void atf_unit::unittest_algo_lib_StringCase() {
    vrfyeq_(ToLower("A"),"a");
    vrfyeq_(ToLower("a"),"a");
    vrfyeq_(ToLower("AAA"),"aaa");

    vrfyeq_(ToUpper("A"),"A");
    vrfyeq_(ToUpper("a"),"A");
    vrfyeq_(ToUpper("aaa"),"AAA");
}

// --------------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_Tabulate() {
    strptr text = ""
        "abcd\ta\tee\t\n"
        "aa\tbbbbbb\tasdfasdfasdfa\n";
    tempstr result(Tabulated(text, "\t", "lrc", 3));
    vrfyeq_(result,""
            "abcd        a        ee\n"
            "aa     bbbbbb   asdfasdfasdfa\n");
    prlog(result);
}

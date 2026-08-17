// Copyright (C) 2023-2024,2026 AlgoRND
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
// Source: cpp/atf_amc/numstr.cpp
//

#include "include/atf_amc.h"

void atf_amc::amctest_Numstr() {
    // '0' pad
    {
        algo::LnumStr6_U32 x;
        ch_SetnumMaybe(x,0);
        vrfy_(0 == ch_GetnumDflt(x,1));
        vrfy_(strptr((char*)x.ch,6) == "000000");
        ch_SetnumMaybe(x,1);
        vrfy_(1 == ch_GetnumDflt(x,0));
        vrfy_(strptr((char*)x.ch,6) == "000001");
        ch_SetnumMaybe(x,12);
        vrfy_(12 == ch_GetnumDflt(x,0));
        vrfy_(strptr((char*)x.ch,6) == "000012");
        ch_SetnumMaybe(x,123);
        vrfy_(123 == ch_GetnumDflt(x,0));
        vrfy_(strptr((char*)x.ch,6) == "000123");
        ch_SetnumMaybe(x,1234);
        vrfy_(1234 == ch_GetnumDflt(x,0));
        vrfy_(strptr((char*)x.ch,6) == "001234");
        ch_SetnumMaybe(x,12345);
        vrfy_(12345 == ch_GetnumDflt(x,0));
        vrfy_(strptr((char*)x.ch,6) == "012345");
        ch_SetnumMaybe(x,123456);
        vrfy_(123456 == ch_GetnumDflt(x,0));
        vrfy_(strptr((char*)x.ch,6) == "123456");
        vrfy_(!ch_SetnumMaybe(x,1234567));
        // test empty string
        ch_SetStrptr(x,"");
        vrfy_(0 == ch_GetnumDflt(x,1));
    }

    // space pad
    {
        algo::LspaceStr6_U32 x;
        ch_SetnumMaybe(x,0);
        vrfy_(0 == ch_GetnumDflt(x,1));
        vrfy_(strptr((char*)x.ch,6) == "     0");
        ch_SetnumMaybe(x,1);
        vrfy_(1 == ch_GetnumDflt(x,0));
        vrfy_(strptr((char*)x.ch,6) == "     1");

        ch_SetnumMaybe(x,12);
        vrfy_(12 == ch_GetnumDflt(x,0));
        vrfy_(strptr((char*)x.ch,6) == "    12");

        ch_SetnumMaybe(x,123);
        vrfy_(123 == ch_GetnumDflt(x,0));
        vrfy_(strptr((char*)x.ch,6) == "   123");

        ch_SetnumMaybe(x,1234);
        vrfy_(1234 == ch_GetnumDflt(x,0));
        vrfy_(strptr((char*)x.ch,6) == "  1234");

        ch_SetnumMaybe(x,12345);
        vrfy_(12345 == ch_GetnumDflt(x,0));
        vrfy_(strptr((char*)x.ch,6) == " 12345");

        ch_SetnumMaybe(x,123456);
        vrfy_(123456 == ch_GetnumDflt(x,0));
        vrfy_(strptr((char*)x.ch,6) == "123456");

        bool ok = !ch_SetnumMaybe(x,1234567);
        vrfy_(ok);

        // test empty string
        ch_SetStrptr(x,"");
        vrfy_(0 == ch_GetnumDflt(x,1));
    }

    // null pad on the right
    {
        atf_amc::RnullStr6_U32 x;

        ch_SetnumMaybe(x,0);
        vrfy_(0 == ch_GetnumDflt(x,1));
        vrfy_(strptr((char*)x.ch,6) == strptr("0\000\000\000\000\000",6));

        ch_SetnumMaybe(x,1);
        vrfy_(1 == ch_GetnumDflt(x,0));
        vrfy_(strptr((char*)x.ch,6) == strptr("1\000\000\000\000\000",6));

        ch_SetnumMaybe(x,12);
        vrfy_(12 == ch_GetnumDflt(x,0));
        vrfy_(strptr((char*)x.ch,6) == strptr("12\000\000\000\000",6));

        ch_SetnumMaybe(x,123);
        vrfy_(123 == ch_GetnumDflt(x,0));
        vrfy_(strptr((char*)x.ch,6) == strptr("123\000\000\000",6));

        ch_SetnumMaybe(x,1234);
        vrfy_(1234 == ch_GetnumDflt(x,0));
        vrfy_(strptr((char*)x.ch,6) == strptr("1234\000\000",6));

        ch_SetnumMaybe(x,12345);
        vrfy_(12345 == ch_GetnumDflt(x,0));
        vrfy_(strptr((char*)x.ch,6) == strptr("12345\000",6));

        ch_SetnumMaybe(x,123456);
        vrfy_(123456 == ch_GetnumDflt(x,0));
        vrfy_(strptr((char*)x.ch,6) == strptr("123456",6));

        bool ok = !ch_SetnumMaybe(x,1234567);
        vrfy_(ok);

        // test empty string
        ch_SetStrptr(x,"");
        vrfy_(0 == ch_GetnumDflt(x,1));
    }

    // rpascal string
    {
        atf_amc::RpasU32Str6 x;

        ch_SetnumMaybe(x,0);
        vrfy_(0 == ch_GetnumDflt(x,1));

        ch_SetnumMaybe(x,1);
        vrfy_(1 == ch_GetnumDflt(x,0));

        ch_SetnumMaybe(x,12);
        vrfy_(12 == ch_GetnumDflt(x,0));

        ch_SetnumMaybe(x,123);
        vrfy_(123 == ch_GetnumDflt(x,0));

        ch_SetnumMaybe(x,1234);
        vrfy_(1234 == ch_GetnumDflt(x,0));

        ch_SetnumMaybe(x,12345);
        vrfy_(12345 == ch_GetnumDflt(x,0));

        ch_SetnumMaybe(x,123456);
        vrfy_(123456 == ch_GetnumDflt(x,0));

        bool ok = !ch_SetnumMaybe(x,1234567);
        vrfy_(ok);

        // test empty string
        ch_SetStrptr(x,"");
        vrfy_(0 ==ch_GetnumDflt(x,1));
    }

}

// --------------------------------------------------------------------------------

static void NumstrStep(i64 set, i64 get, bool success) {
    u64 pad1=0xeeeeeeeeeeeeeeeeULL;
    algo::LnumStr4_U32 str;
    u64 pad2=0xaaaaaaaaaaaaaaaaULL;
    bool ok=ch_SetnumMaybe(str,set);
    vrfyeq_(ok,success);
    vrfyeq_(pad1,u64(0xeeeeeeeeeeeeeeeeULL));
    vrfyeq_(pad2,u64(0xaaaaaaaaaaaaaaaaULL));
    i64 val=ch_GetnumDflt(str,get+1);
    vrfyeq_(val,get);
}

void atf_amc::amctest_NumstrCorruption() {
    NumstrStep(1,1,true);
    NumstrStep(10,10,true);
    NumstrStep(100,100,true);
    NumstrStep(1000,1000,true);
    NumstrStep(9999,9999,true);
    NumstrStep(0,0,true);
    NumstrStep(10000,0,false);
    NumstrStep(-1,0,false);
    NumstrStep(100000,0,false);
}

// --------------------------------------------------------------------------------

// Signed numstr with min_len>1: digits are zero-padded to min_len,
// then '-' is prepended; canonical strings roundtrip exactly
void atf_amc::amctest_NumstrSignedMinLen() {
    atf_amc::LspaceStr7_I32 x;

    ch_SetnumMaybe(x,-5);
    vrfy_(strptr((char*)x.ch,7) == "   -005");
    vrfy_(-5 == ch_GetnumDflt(x,0));

    ch_SetnumMaybe(x,-12);
    vrfy_(strptr((char*)x.ch,7) == "   -012");
    vrfy_(-12 == ch_GetnumDflt(x,0));

    ch_SetnumMaybe(x,-123);
    vrfy_(strptr((char*)x.ch,7) == "   -123");
    vrfy_(-123 == ch_GetnumDflt(x,0));

    ch_SetnumMaybe(x,-123456);
    vrfy_(strptr((char*)x.ch,7) == "-123456");
    vrfy_(-123456 == ch_GetnumDflt(x,0));

    // 7 digits + sign do not fit in 7 chars
    vrfy_(!ch_SetnumMaybe(x,-1234567));

    ch_SetnumMaybe(x,5);
    vrfy_(strptr((char*)x.ch,7) == "    005");
    vrfy_(5 == ch_GetnumDflt(x,0));

    ch_SetnumMaybe(x,0);
    vrfy_(strptr((char*)x.ch,7) == "    000");
    vrfy_(0 == ch_GetnumDflt(x,1));

    // canonical external string parses back
    ch_SetStrptr(x,"-005");
    vrfy_(-5 == ch_GetnumDflt(x,0));
}

// --------------------------------------------------------------------------------

// Letter digits stop at the base's last valid digit: base 16 accepts a-f,
// base 36 accepts a-z; the next letter ('g', '{', and the uppercase twins)
// is invalid and GetnumDflt returns the default
void atf_amc::amctest_NumstrLetterDigit() {
    algo::LnumStr8_U32_Base16 x;
    ch_SetStrptr(x,"f");
    vrfy_(15 == ch_GetnumDflt(x,42));
    ch_SetStrptr(x,"F");
    vrfy_(15 == ch_GetnumDflt(x,42));
    ch_SetStrptr(x,"g");
    vrfy_(42 == ch_GetnumDflt(x,42));
    ch_SetStrptr(x,"G");
    vrfy_(42 == ch_GetnumDflt(x,42));

    algo::LnumStr5_U32_Base36 y;
    ch_SetStrptr(y,"z");
    vrfy_(35 == ch_GetnumDflt(y,42));
    ch_SetStrptr(y,"Z");
    vrfy_(35 == ch_GetnumDflt(y,42));
    ch_SetStrptr(y,"{");
    vrfy_(42 == ch_GetnumDflt(y,42));
    ch_SetStrptr(y,"[");
    vrfy_(42 == ch_GetnumDflt(y,42));
}

// --------------------------------------------------------------------------------

// A numstr longer than 64 bits' worth of digits detects u64 overflow in the
// digit loop: values above 2^64-1 are rejected, the boundary parses exactly
void atf_amc::amctest_NumstrOverflowU64() {
    algo::LnumStr20_U64 x;
    ch_SetStrptr(x,"18446744073709551615");
    vrfy_(0xffffffffffffffffULL == ch_GetnumDflt(x,42));
    ch_SetStrptr(x,"18446744073709551616");
    vrfy_(42 == ch_GetnumDflt(x,42));
    // wraps mod 2^64 to a value bigger than the pre-wrap partial sum,
    // so a compare-after-multiply check does not see it
    ch_SetStrptr(x,"21000000000000000000");
    vrfy_(42 == ch_GetnumDflt(x,42));

    algo::LnumStr22_U64 y;
    ch_SetStrptr(y,"9999999999999999999999");
    vrfy_(42 == ch_GetnumDflt(y,42));
    ch_SetStrptr(y,"18446744073709551615");
    vrfy_(0xffffffffffffffffULL == ch_GetnumDflt(y,42));
}

// --------------------------------------------------------------------------------

// A signed numstr range-checks the parsed magnitude even when the string
// holds more digits than 64 bits: values beyond the numtype range are
// rejected instead of wrapping through the i64 cast, and the negative
// bound is one larger than the positive one
void atf_amc::amctest_NumstrOverflowSigned() {
    algo::LspaceStr20_I64 x;
    ch_SetStrptr(x,"9223372036854775807");
    vrfy_(0x7fffffffffffffffLL == ch_GetnumDflt(x,42));
    ch_SetStrptr(x,"-9223372036854775808");
    vrfy_(-0x7fffffffffffffffLL - 1 == ch_GetnumDflt(x,42));
    // one past i64max: fits u64, wraps negative through the cast
    ch_SetStrptr(x,"9223372036854775808");
    vrfy_(42 == ch_GetnumDflt(x,42));
    ch_SetStrptr(x,"-9223372036854775809");
    vrfy_(42 == ch_GetnumDflt(x,42));
    ch_SetStrptr(x,"18000000000000000000");
    vrfy_(42 == ch_GetnumDflt(x,42));
    ch_SetStrptr(x,"-9999999999999999999");
    vrfy_(42 == ch_GetnumDflt(x,42));

    // signed range check below 64 bits of digit capacity keeps working
    algo::LspaceStr7_I32_Base36 y;
    ch_SetStrptr(y,"-Z");
    vrfy_(-35 == ch_GetnumDflt(y,42));
    ch_SetStrptr(y,"-ZZZZZZ");
    vrfy_(42 == ch_GetnumDflt(y,42));
}

// --------------------------------------------------------------------------------

// Geti64 fails when the stored value does not fit in i64: a u64 numstr
// holding a value above i64max would otherwise wrap through the plain
// cast and return a silently wrong negative number with out_ok true
void atf_amc::amctest_NumstrGeti64Range() {
    algo::LnumStr20_U64 x;
    bool ok = true;
    ch_SetStrptr(x,"9223372036854775807"); // i64max: the largest value Geti64 can return
    vrfy_(0x7fffffffffffffffLL == ch_Geti64(x,ok));
    vrfy_(ok);
    ch_SetStrptr(x,"9223372036854775808"); // one past i64max
    ok = true;
    ch_Geti64(x,ok);
    vrfy_(!ok);
    ch_SetStrptr(x,"18446744073709551615"); // u64 max: the plain cast yields -1
    ok = true;
    ch_Geti64(x,ok);
    vrfy_(!ok);

    // an i64 numtype spans exactly the values Geti64 returns: no range check
    algo::LspaceStr20_I64 y;
    ch_SetStrptr(y,"-9223372036854775808");
    ok = false;
    vrfy_(-0x7fffffffffffffffLL - 1 == ch_Geti64(y,ok));
    vrfy_(ok);
}

// --------------------------------------------------------------------------------

// SetnumMaybe returns false for a value outside the numtype range and
// leaves the string unchanged, for every numtype width and sign.  The
// string-length gate cannot stand in for the range gate: the base-10
// fast path formats through a u32-parameter FmtBuf, whose mod-2^32
// digit string can be short enough to pass it
void atf_amc::amctest_NumstrSetnumRange() {
    algo::LnumStr7_U32 x;
    ch_SetnumMaybe(x,123);
    vrfy_(!ch_SetnumMaybe(x,4294967301LL)); // 2^32+5
    vrfy_(123 == ch_GetnumDflt(x,0));
    vrfy_(!ch_SetnumMaybe(x,-1));
    vrfy_(123 == ch_GetnumDflt(x,0));

    atf_amc::LspaceStr7_I32 y;
    ch_SetnumMaybe(y,-45);
    vrfy_(!ch_SetnumMaybe(y,4294967301LL));
    vrfy_(!ch_SetnumMaybe(y,-4294967301LL));
    vrfy_(!ch_SetnumMaybe(y,2147483648LL));  // i32max+1
    vrfy_(!ch_SetnumMaybe(y,-2147483649LL)); // i32min-1
    vrfy_(-45 == ch_GetnumDflt(y,0));

    algo::LspaceStr3_I16 z;
    ch_SetnumMaybe(z,-99);
    vrfy_(!ch_SetnumMaybe(z,4294967301LL));
    vrfy_(-99 == ch_GetnumDflt(z,0));

    // negative is out of range for a u64 numtype, even when its 20 digits fit
    algo::LnumStr20_U64 w;
    ch_SetnumMaybe(w,7);
    vrfy_(!ch_SetnumMaybe(w,-1));
    vrfy_(7 == ch_GetnumDflt(w,0));
}

// --------------------------------------------------------------------------------

// SetnumMaybe negates a negative value in u64 space: i64min, whose
// magnitude has no i64 representation, formats and round-trips exactly
void atf_amc::amctest_NumstrSetnumI64Min() {
    algo::LspaceStr20_I64 x;
    vrfy_(ch_SetnumMaybe(x,-0x7fffffffffffffffLL - 1));
    vrfy_(strptr((char*)x.ch,20) == "-9223372036854775808");
    vrfy_(-0x7fffffffffffffffLL - 1 == ch_GetnumDflt(x,42));
}

// --------------------------------------------------------------------------------

// min_len padding writes the base's zero digit -- ' ' in base 95, NUL in
// base 256; the character '0' is a nonzero digit in those bases (16 and 48)
// and padding with it would change the stored value
void atf_amc::amctest_NumstrPadHighBase() {
    atf_amc::LspaceStr5_U32_Base95 x;
    ch_SetnumMaybe(x,5);
    vrfy_(5 == ch_GetnumDflt(x,42));
    ch_SetnumMaybe(x,0);
    vrfy_(0 == ch_GetnumDflt(x,42));
    ch_SetnumMaybe(x,9024); // 94*95+94: two max digits, one pad digit
    vrfy_(9024 == ch_GetnumDflt(x,42));

    atf_amc::LnullStr4_U32_Base256 y;
    ch_SetnumMaybe(y,7);
    vrfy_(7 == ch_GetnumDflt(y,42));
    ch_SetnumMaybe(y,0x0102); // two nonzero digit bytes, one pad digit
    vrfy_(0x0102 == ch_GetnumDflt(y,42));
}

// --------------------------------------------------------------------------------

// base-256 digits are unsigned bytes -- a byte >= 0x80 is a large digit,
// not a negative one; base 95 accepts exactly the printable range ' '..'~'
// and rejects every other character
void atf_amc::amctest_NumstrDigitHighBase() {
    atf_amc::LnullStr4_U32_Base256 x;
    ch_SetnumMaybe(x,200);
    vrfy_(200 == ch_GetnumDflt(x,42));
    ch_SetnumMaybe(x,0x81828384LL); // every digit byte has the high bit set
    vrfy_(0x81828384 == ch_GetnumDflt(x,42));

    atf_amc::LspaceStr5_U32_Base95 y;
    ch_SetStrptr(y,"~"); // '~' is the largest base-95 digit
    vrfy_(94 == ch_GetnumDflt(y,42));
    ch_SetStrptr(y,"\x7f"); // DEL: one past '~'
    vrfy_(42 == ch_GetnumDflt(y,42));
    ch_SetStrptr(y,"\x01"); // control char: wraps past 94 in u8 space
    vrfy_(42 == ch_GetnumDflt(y,42));
}

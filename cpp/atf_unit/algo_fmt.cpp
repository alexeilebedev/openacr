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
// Target: atf_unit (exe) -- Unit tests (see unittest table)
// Exceptions: yes
// Source: cpp/atf_unit/algo_fmt.cpp
//

#include "include/atf_unit.h"

void atf_unit::unittest_algo_lib_FmtBufDec() {
    char buf[128];
    vrfyeq_(strptr(buf,algo::u64_FmtBufDec(1         , 8, (u8*)buf)), "0.00000001");
    vrfyeq_(strptr(buf,algo::u64_FmtBufDec(10        , 8, (u8*)buf)), "0.0000001");
    vrfyeq_(strptr(buf,algo::u64_FmtBufDec(100       , 8, (u8*)buf)), "0.000001");
    vrfyeq_(strptr(buf,algo::u64_FmtBufDec(1000      , 8, (u8*)buf)), "0.00001");
    vrfyeq_(strptr(buf,algo::u64_FmtBufDec(10000     , 8, (u8*)buf)), "0.0001");
    vrfyeq_(strptr(buf,algo::u64_FmtBufDec(100000    , 8, (u8*)buf)), "0.001");
    vrfyeq_(strptr(buf,algo::u64_FmtBufDec(1000000   , 8, (u8*)buf)), "0.01");
    vrfyeq_(strptr(buf,algo::u64_FmtBufDec(10000000  , 8, (u8*)buf)), "0.1");
    vrfyeq_(strptr(buf,algo::u64_FmtBufDec(100000000 , 8, (u8*)buf)), "1");

    vrfyeq_(strptr(buf,algo::u64_FmtBufDec(1         , 3, (u8*)buf, false)), "0.001");
    vrfyeq_(strptr(buf,algo::u64_FmtBufDec(10        , 3, (u8*)buf, false)), "0.01");
    vrfyeq_(strptr(buf,algo::u64_FmtBufDec(100       , 3, (u8*)buf, false)), "0.1");
    vrfyeq_(strptr(buf,algo::u64_FmtBufDec(1000      , 3, (u8*)buf, false)), "1");
    vrfyeq_(strptr(buf,algo::u64_FmtBufDec(10000     , 3, (u8*)buf, false)), "10");
    vrfyeq_(strptr(buf,algo::u64_FmtBufDec(100000    , 3, (u8*)buf, false)), "100");

    vrfyeq_(strptr(buf,algo::u64_FmtBufDec(1         , 3, (u8*)buf, true)), "0.001");
    vrfyeq_(strptr(buf,algo::u64_FmtBufDec(10        , 3, (u8*)buf, true)), "0.010");
    vrfyeq_(strptr(buf,algo::u64_FmtBufDec(100       , 3, (u8*)buf, true)), "0.100");
    vrfyeq_(strptr(buf,algo::u64_FmtBufDec(1000      , 3, (u8*)buf, true)), "1.000");
    vrfyeq_(strptr(buf,algo::u64_FmtBufDec(10000     , 3, (u8*)buf, true)), "10.000");
    vrfyeq_(strptr(buf,algo::u64_FmtBufDec(100000    , 3, (u8*)buf, true)), "100.000");

    vrfyeq_(strptr(buf,algo::u64_FmtBufDec(0         , 2, (u8*)buf, false)), "0");
    vrfyeq_(strptr(buf,algo::u64_FmtBufDec(0         , 2, (u8*)buf, true) ), "0.00");
    vrfyeq_(strptr(buf,algo::u64_FmtBufDec(123       , 1, (u8*)buf, false)), "12.3");
    vrfyeq_(strptr(buf,algo::u64_FmtBufDec(123       , 2, (u8*)buf, false)), "1.23");
    vrfyeq_(strptr(buf,algo::u64_FmtBufDec(123       , 2, (u8*)buf, true) ), "1.23");
    vrfyeq_(strptr(buf,algo::u64_FmtBufDec(1230      , 3, (u8*)buf, false)), "1.23");
    vrfyeq_(strptr(buf,algo::u64_FmtBufDec(1230      , 3, (u8*)buf, true) ), "1.230");



    vrfyeq_(strptr(buf,algo::u64_FmtBufDec(1434520000      , 6, (u8*)buf, true) ), "1434.520000");

    vrfyeq_(strptr(buf,algo::u64_FmtBufDec(0, 0, (u8*)buf)), "0");
    vrfyeq_(strptr(buf,algo::u64_FmtBufDec(1234567812345678ULL, 0, (u8*)buf)), "1234567812345678");
}

// -----------------------------------------------------------------------------

static tempstr ToCamel(strptr s) {
    tempstr ret;
    strptr_PrintCamel(s,ret);
    return ret;
}

static tempstr ToLowerUnder(strptr s) {
    tempstr ret;
    strptr_PrintLowerUnder(s,ret);
    return ret;
}

void atf_unit::unittest_algo_lib_CaseConversion() {
    vrfyeq_(ToCamel("abcd"), "Abcd");
    vrfyeq_(ToCamel("abcd_ef"), "AbcdEf");
    vrfyeq_(ToCamel("a__b"), "A_B");
    vrfyeq_(ToLowerUnder("abcd testString"), "abcd test_string");
    vrfyeq_(ToLowerUnder("Abcd"), "abcd");
    vrfyeq_(ToLowerUnder("Abcd"), "abcd");
    vrfyeq_(ToLowerUnder("BBOPrice"), "bboprice");
    vrfyeq_(ToLowerUnder("SymbolID"), "symbol_id");
}

void atf_unit::unittest_algo_FileFlags() {
    vrfy_((tempstr() << algo::FileFlags(algo_FileFlags_read | algo_FileFlags_write)) == "read,write");
    vrfy_((tempstr() << algo::FileFlags(algo_FileFlags_write | algo_FileFlags_read)) == "read,write");
    vrfy_((tempstr() << algo::FileFlags()) == "");
}

static tempstr ToBase64(strptr s) {
    tempstr ret;
    strptr_PrintBase64(s,ret);
    return ret;
}

static void TestBase64(strptr decoded, strptr encoded) {
    vrfyeq_(ToBase64(decoded),encoded);
}

// Base64 written plainly from the definition, to check algo's encoder against:
// a fast encoder is not self-evidently a correct one.
static tempstr RefBase64(strptr s) {
    static const char *alpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    tempstr ret;
    for (i64 i = 0; i < ch_N(s); i += 3) {
        i64 nhave = i64_Min(i64(3), ch_N(s) - i);
        u32 quantum = 0;
        for (i64 j = 0; j < 3; j++) {
            quantum <<= 8;
            if (j < nhave) {
                quantum |= u8(s[i+j]);
            }
        }
        ret << alpha[(quantum>>18) & 0x3f];
        ret << alpha[(quantum>>12) & 0x3f];
        ret << (nhave >= 2 ? alpha[(quantum>>6) & 0x3f] : '=');
        ret << (nhave >= 3 ? alpha[quantum & 0x3f] : '=');
    }
    return ret;
}

void atf_unit::unittest_algo_Base64() {
    TestBase64("","");
    TestBase64("a"     ,"YQ==");
    TestBase64("ab"    ,"YWI=");
    TestBase64("abc"   ,"YWJj");
    TestBase64("abcd"  ,"YWJjZA==");
    TestBase64("abcde" ,"YWJjZGU=");
    TestBase64("abcdef","YWJjZGVm");
    // check whole alphabet
    TestBase64(strptr(
                      "\x00\x10\x83\x10\x51\x87\x20\x92\x8b\x30\xd3\x8f\x41\x14\x93\x51"
                      "\x55\x97\x61\x96\x9b\x71\xd7\x9f\x82\x18\xa3\x92\x59\xa7\xa2\x9a"
                      "\xab\xb2\xdb\xaf\xc3\x1c\xb3\xd3\x5d\xb7\xe3\x9e\xbb\xf3\xdf\xbf"
                      ,16*3),
               "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/");
    // Every length from empty to a few dozen quanta, over bytes walking the
    // whole 0..255 range.  The cases above pin the alphabet and padding.
    tempstr buf;
    for (i64 n = 0; n < 256; n++) {
        vrfyeq_(ToBase64(buf), RefBase64(buf));
        buf << char(u8((n*37 + 11) & 0xff));
    }
}


// Decode ENCODED and check it yields DECODED.
static void TestReadBase64(strptr encoded, strptr decoded) {
    tempstr got;
    vrfy_(algo::strptr_ReadBase64(encoded,got));
    vrfyeq_(got,decoded);
}

void atf_unit::unittest_algo_ReadBase64() {
    TestReadBase64(""        ,"");
    TestReadBase64("YQ=="    ,"a");
    TestReadBase64("YWI="    ,"ab");
    TestReadBase64("YWJj"    ,"abc");
    TestReadBase64("YWJjZA==","abcd");
    TestReadBase64("YWJjZGU=","abcde");
    TestReadBase64("YWJjZGVm","abcdef");
    // Padding is skipped rather than counted -- the group boundary already
    // says how many bits of the last quantum are real -- so an unpadded
    // string decodes to the same bytes.
    TestReadBase64("YQ" ,"a");
    TestReadBase64("YWI","ab");
    // Whatever the encoder writes reads back, over every length from empty to
    // a few dozen quanta and bytes walking the whole 0..255 range.
    tempstr buf;
    for (i64 n = 0; n < 256; n++) {
        TestReadBase64(ToBase64(buf),buf);
        buf << char(u8((n*37 + 11) & 0xff));
    }
    // A character outside the alphabet is refused, and the characters around
    // it still decode.
    tempstr bad;
    vrfy_(!algo::strptr_ReadBase64("YQ*==",bad));
    vrfyeq_(bad,"a");
    // The bytes are appended, so a caller can decode into a string it is
    // already building.
    tempstr acc("x");
    vrfy_(algo::strptr_ReadBase64("YQ==",acc));
    vrfyeq_(acc,"xa");
}

void atf_unit::unittest_algo_lib_PrintUuid() {
    {
        algo::Uuid dflt;
        cstring dflt_str;
        algo::Uuid_Print(dflt,dflt_str);
        vrfyeq_(dflt_str,"00000000-0000-0000-0000-000000000000");
    }
    {
        algo::Uuid some;
        const u8 bytes[] = {0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10};
        memcpy(some.value_elems,bytes,sizeof bytes);
        cstring some_str;
        algo::Uuid_Print(some,some_str);
        vrfyeq_(some_str,"01234567-89ab-cdef-fedc-ba9876543210");
    }
}


void atf_unit::unittest_algo_lib_ReadUuid() {
    algo::Uuid uuid;
    vrfy_(algo::Uuid_ReadStrptrMaybe(uuid,"01234567-89ab-cdef-fedc-ba9876543210"));
    vrfyeq_(uuid.value_elems[0],0x01);
    vrfyeq_(uuid.value_elems[1],0x23);
    vrfyeq_(uuid.value_elems[2],0x45);
    vrfyeq_(uuid.value_elems[3],0x67);
    vrfyeq_(uuid.value_elems[4],0x89);
    vrfyeq_(uuid.value_elems[5],0xab);
    vrfyeq_(uuid.value_elems[6],0xcd);
    vrfyeq_(uuid.value_elems[7],0xef);
    vrfyeq_(uuid.value_elems[8],0xfe);
    vrfyeq_(uuid.value_elems[9],0xdc);
    vrfyeq_(uuid.value_elems[10],0xba);
    vrfyeq_(uuid.value_elems[11],0x98);
    vrfyeq_(uuid.value_elems[12],0x76);
    vrfyeq_(uuid.value_elems[13],0x54);
    vrfyeq_(uuid.value_elems[14],0x32);
    vrfyeq_(uuid.value_elems[15],0x10);
    vrfy_(!algo::Uuid_ReadStrptrMaybe(uuid,""));
    vrfy_(!algo::Uuid_ReadStrptrMaybe(uuid,"01234567-89ab-cdef-fedc-ba987654321001"));
    vrfy_(!algo::Uuid_ReadStrptrMaybe(uuid,"01234567-89ab-cdef-fedc-ba98765432101"));
    vrfy_(!algo::Uuid_ReadStrptrMaybe(uuid,"0123456-89ab-cdef-fedc-ba98765432101"));
    vrfy_(!algo::Uuid_ReadStrptrMaybe(uuid,"012345671-89ab-cdef-fedc-ba987654321"));
    vrfy_(!algo::Uuid_ReadStrptrMaybe(uuid,"ZDSK"));
}

// -----------------------------------------------------------------------------

// strptr formatters in fmt.cpp: SQL/XML/CPP/DOT/URI/TeX quoting, padding,
// thousands separators, case copy, trailing-zero trim.
void atf_unit::unittest_algo_StrptrPrintFmt() {
    {
        cstring out;
        algo::strptr_PrintSql("O'Brien", out);
        vrfyeq_(out, "'O''Brien'");
    }
    {
        cstring out;
        algo::strptr_PrintSql("say \"hi\"", out, '"');
        vrfyeq_(out, "\"say \"\"hi\"\"\"");
    }
    {
        cstring out;
        algo::strptr_PrintSql("a\nb", out);
        vrfyeq_(out, "'a\\nb'");
    }
    {
        cstring out;
        algo::strptr_PrintXml("a<b>&\"'", out, false);
        vrfyeq_(out, "a&lt;b&gt;&amp;&quot;&apos;");
    }
    {
        cstring out;
        algo::strptr_PrintXml("ab", out, true);
        vrfyeq_(out, "\"ab\"");
    }
    {
        cstring out;
        algo::strptr_PrintCpp("hi", out);
        vrfyeq_(out, "\"hi\"");
    }
    {
        cstring out;
        algo::strptr_PrintDot("a<b>:c", out);
        vrfyeq_(out, "\"a_b__c\"");
    }
    {
        cstring out;
        algo::strptr_PrintUri("a b/c", out, true);
        vrfyeq_(out, "a+b%2Fc");
    }
    {
        cstring out;
        algo::strptr_PrintTexString("a_b#c", out);
        vrfyeq_(out, "a\\_b\\#c");
    }
    {
        cstring out;
        algo::strptr_PrintTexIdent("a1.b", out);
        vrfyeq_(out, "aOneb");
    }
    {
        cstring out;
        algo::strptr_PrintRegxSql("abc", out);
        vrfyeq_(out, "abc");
    }
    {
        cstring out;
        algo::strptr_PrintPadLeft("ab", out, 5);
        vrfyeq_(out, "   ab");
    }
    {
        cstring out;
        algo::strptr_PrintPadLeft("ab", out, 5, '0');
        vrfyeq_(out, "000ab");
    }
    {
        cstring out;
        algo::strptr_PrintPadRight("ab", out, 5);
        vrfyeq_(out, "ab   ");
    }
    {
        cstring out;
        algo::strptr_PrintWithCommas("1234567", out);
        vrfyeq_(out, "1,234,567");
    }
    {
        cstring out;
        algo::strptr_PrintWithCommas("-1234.5", out);
        vrfyeq_(out, "-1,234.5");
    }
    {
        cstring out;
        algo::strptr_PrintWithCommas("+980061", out);
        vrfyeq_(out, "+980,061");
    }
    {
        cstring out;
        algo::strptr_PrintCopyCase("ABC", out, "xyz");
        vrfyeq_(out, "XYZ");
    }
    {
        cstring out;
        algo::strptr_PrintAligned("ab", out, 5, algo_TextJust_j_right, '.');
        vrfyeq_(out, "...ab");
    }
    {
        tempstr t("1.2300");
        strptr s = t;
        algo::strptr_TrimZerosRight(s);
        vrfyeq_(s, "1.23");
    }
}

// -----------------------------------------------------------------------------

// Number/quantity formatters in fmt.cpp: hex, base32, 128-bit, pointer,
// percent, comma grouping, ranges, and the scaled Hz/nsec helpers driven
// through the merged unit tables.
void atf_unit::unittest_algo_NumPrintFmt() {
    {
        cstring out;
        algo::u64_PrintHex(255, out, 2, false);
        vrfyeq_(out, "ff");
    }
    {
        cstring out;
        algo::u64_PrintHex(0, out, 4, false);
        vrfyeq_(out, "0000");
    }
    {
        cstring out;
        algo::u64_PrintHex(0xABCD, out, 2, false, true);
        vrfyeq_(out, "ABCD");
    }
    {
        cstring out;
        algo::u64_PrintHex(255, out, 2);
        vrfyeq_(out, "0xff");
    }
    {
        cstring out;
        algo::u64_PrintBase32(0, out);
        vrfyeq_(out, "0");
    }
    {
        cstring out;
        algo::u64_PrintBase32(31, out);
        vrfyeq_(out, "V");
    }
    {
        cstring out;
        algo::u64_PrintBase32(32, out);
        vrfyeq_(out, "10");
    }
    {
        cstring out;
        algo::u128_PrintHex(u128(0xABCDEF), out, false);
        vrfyeq_(out, "ABCDEF");
    }
    {
        cstring out;
        u128_Print(u128(1234567890), out);
        vrfyeq_(out, "1234567890");
    }
    {
        cstring out;
        algo::Ptr_Print((void*)0x1234, out);
        vrfyeq_(out, "0x00001234");
    }
    {
        cstring out;
        algo::double_PrintPercent(0.125, out, 1);
        vrfyeq_(out, "12.5");
    }
    {
        vrfyeq_(algo::double_PrintWithCommas(1234567.0, 0), "1,234,567");
    }
    {
        algo::i32_Range r;
        r.beg = 3;
        r.end = 7;
        cstring out;
        algo::i32_Range_Print(r, out);
        vrfyeq_(out, "3 7");
    }
    {
        vrfyeq_(algo::HzToStr(1500000, true), "1500000");
        vrfyeq_(algo::NsecToStr(250, true), "250");
    }
    {
        // scaled (non-numeric) path: print picks the largest enabled unit
        vrfyeq_(algo::HzToStr(1500000000), "1.5GHz");
        vrfyeq_(algo::HzToStr(500), "0.5KHz");      // sub-KHz still prints as KHz
        vrfyeq_(algo::NsecToStr(1500), "1.5us");
        vrfyeq_(algo::SizeToStr(1500), "1.50K");
        vrfyeq_(algo::SizeToStr(500), "500");       // bare integer below 1000
    }
}

// -----------------------------------------------------------------------------

// Numeric parsers in fmt.cpp: packed-digit fast parse, octal, counts, and
// the suffix parsers exercising the merged unit tables (longest-suffix and
// case-alias matching).
void atf_unit::unittest_algo_ParseNumFmt() {
    {
        u32 ok = 0;
        vrfyeq_(algo::aParseNum4("1234", ok), u32(1234));
        vrfy_(ok);
    }
    {
        u32 ok = 0;
        vrfyeq_(algo::aParseNum8("12345678", ok), u32(12345678));
        vrfy_(ok);
    }
    {
        u32 ok = 0;
        vrfyeq_(algo::aParseNum16("1234567890123456", ok), u64(1234567890123456ULL));
        vrfy_(ok);
    }
    {
        u32 ok = 0;
        vrfyeq_(algo::aParseNum16("1234", ok), u64(1234));
        vrfy_(ok);
    }
    {
        u32 ok = 1;
        algo::aParseNum4("12x4", ok);
        vrfy_(!ok);
    }
    {
        u8 res = 0;
        u32 packed = u32('7') | (u32('7') << 8) | (u32('7') << 16);
        int advance = algo::ParseOct3(packed, 3, res);
        vrfyeq_(advance, 3);
        vrfyeq_(int(res), 0777 & 0xff);
    }
    {
        vrfyeq_(algo::ParseCount("123"), i64(123));
        vrfyeq_(algo::ParseCount("inf"), i64(1) << 62);
        vrfyeq_(algo::ParseCount("", 9), i64(9));
        vrfyeq_(algo::ParseCount("xyz", 5), i64(5));
    }
    {
        vrfyeq_(algo::ParseHz("100"), u64(100));
        vrfyeq_(algo::ParseHz("", 7), u64(7));
        vrfyeq_(algo::ParseHz("1kHz"), u64(1000));
        vrfyeq_(algo::ParseHz("2GHz"), u64(2000000000ULL));
        vrfyeq_(algo::ParseNsec("250"), u64(250));
        vrfyeq_(algo::ParseNsec("5ms"), u64(5000000));
        vrfyeq_(algo::ParseSize("2K"), u64(2000));
        vrfyeq_(algo::ParseSize("1m"), u64(1000000));      // lowercase alias
        vrfyeq_(algo::ParseSize("inf"), u64(1) << 62);
    }
}

// -----------------------------------------------------------------------------

// Print/Read round-trips for the time and diff types in fmt.cpp.  Compare
// parsed values (not text) so the check is independent of the exact format.
void atf_unit::unittest_algo_TimeRoundtrip() {
    algo::SetTz("GMT");
    {
        algo::UnixTime t1(1000000000), t2;
        cstring s;
        algo::UnixTime_Print(t1, s);
        vrfy_(algo::UnixTime_ReadStrptrMaybe(t2, s));
        vrfyeq_(t2.value, t1.value);
    }
    {
        algo::WTime w1, w2;
        vrfy_(algo::WTime_ReadStrptrMaybe(w1, "2001/09/09 01:46:40"));
        cstring s;
        algo::WTime_Print(w1, s);
        vrfy_(algo::WTime_ReadStrptrMaybe(w2, s));
        vrfyeq_(w2.value, w1.value);
    }
    {
        algo::UnDiff d1, d2;
        vrfy_(algo::UnDiff_ReadStrptrMaybe(d1, "01:02:03"));
        cstring s;
        algo::UnDiff_Print(d1, s);
        vrfy_(algo::UnDiff_ReadStrptrMaybe(d2, s));
        vrfyeq_(d2.value, d1.value);
    }
    {
        algo::WDiff w1, w2;
        vrfy_(algo::WDiff_ReadStrptrMaybe(w1, "01:02:03"));
        cstring s;
        algo::WDiff_Print(w1, s);
        vrfy_(algo::WDiff_ReadStrptrMaybe(w2, s));
        vrfyeq_(w2.value, w1.value);
    }
    {
        // UnixDiff has no reader; print a constructed second count directly
        algo::UnixDiff d;
        d.value = 3723;
        cstring s;
        algo::UnixDiff_Print(d, s);
        vrfyeq_(s, "01:02:03");
    }
}

// -----------------------------------------------------------------------------

// Print/Read round-trips for URL and Ipmask in fmt.cpp.
void atf_unit::unittest_algo_UrlIpmaskFmt() {
    {
        algo::URL u1, u2;
        vrfy_(algo::URL_ReadStrptrMaybe(u1, "http://www.example.com:8080/dir/file"));
        cstring s;
        algo::URL_Print(u1, s);
        vrfy_(algo::URL_ReadStrptrMaybe(u2, s));
        vrfyeq_(u2.server, u1.server);
        vrfyeq_(u2.dir, u1.dir);
        vrfyeq_(u2.port, u1.port);
    }
    {
        algo::Ipmask m1, m2;
        vrfy_(algo::Ipmask_ReadStrptrMaybe(m1, "192.168.0.0/16"));
        cstring s;
        algo::Ipmask_Print(m1, s);
        vrfy_(algo::Ipmask_ReadStrptrMaybe(m2, s));
        vrfyeq_(m2.ip_host, m1.ip_host);
        vrfyeq_(m2.mask, m1.mask);
    }
}

// An integer range list round-trips through its text form; a span is inclusive
// at both ends where the struct behind it is half-open; and the order the list
// states is the order it keeps.
void atf_unit::unittest_algo_lib_RangeAryList() {
    {
        // a span names both its ends; the range behind it is half-open
        algo::I32RangeAry ary;
        vrfy_(algo::I32RangeAry_ReadStrptrMaybe(ary, "4-7"));
        vrfyeq_(ary_N(ary), 1);
        vrfyeq_(ary_qFind(ary,0).beg, 4);
        vrfyeq_(ary_qFind(ary,0).end, 8);
        vrfyeq_(algo::range_N(ary_qFind(ary,0)), 4);
    }
    {
        // one value is one range, and prints back as the value, not as 7-7
        algo::I32RangeAry ary;
        vrfy_(algo::I32RangeAry_ReadStrptrMaybe(ary, "7"));
        vrfyeq_(ary_qFind(ary,0).beg, 7);
        vrfyeq_(ary_qFind(ary,0).end, 8);
        cstring out;
        algo::I32RangeAry_Print(ary, out);
        vrfyeq_(out, "7");
    }
    {
        // the list is a sequence: nothing sorts it, nothing merges it, so a
        // preference order survives the round trip exactly as written
        algo::I32RangeAry ary;
        vrfy_(algo::I32RangeAry_ReadStrptrMaybe(ary, "2,0,4-7"));
        vrfyeq_(ary_N(ary), 3);
        cstring out;
        algo::I32RangeAry_Print(ary, out);
        vrfyeq_(out, "2,0,4-7");
    }
    {
        // adjacent values stay separate items rather than collapsing to a span
        algo::I32RangeAry ary;
        vrfy_(algo::I32RangeAry_ReadStrptrMaybe(ary, "3,4"));
        vrfyeq_(ary_N(ary), 2);
        cstring out;
        algo::I32RangeAry_Print(ary, out);
        vrfyeq_(out, "3,4");
    }
    {
        // an empty string is an empty list and not an error: it is what a
        // field defaulting to "" holds, and it means no opinion
        algo::I32RangeAry ary;
        vrfy_(algo::I32RangeAry_ReadStrptrMaybe(ary, ""));
        vrfyeq_(ary_N(ary), 0);
        cstring out;
        algo::I32RangeAry_Print(ary, out);
        vrfyeq_(out, "");
    }
    {
        // a trailing comma names nothing further and is accepted
        algo::I32RangeAry ary;
        vrfy_(algo::I32RangeAry_ReadStrptrMaybe(ary, "0,2,"));
        vrfyeq_(ary_N(ary), 2);
    }
    {
        // an item is a number by the same parser every numeric field in the
        // tree uses, so it admits what that parser admits: a hex prefix, and
        // leading whitespace.  A stricter number here would be the
        // inconsistency, since -nmsg and every other numeric option take both
        algo::I32RangeAry ary;
        vrfy_(algo::I32RangeAry_ReadStrptrMaybe(ary, "0x10"));
        vrfyeq_(ary_qFind(ary,0).beg, 16);
        vrfy_(algo::I32RangeAry_ReadStrptrMaybe(ary, " 1"));
        vrfyeq_(ary_qFind(ary,0).beg, 1);
    }
    {
        // What the notation itself forbids, as opposed to what the number
        // parser tolerates inside one item: a negative value (the separator
        // would be ambiguous), a span running backwards, an item naming
        // nothing, a second separator, and a dangling one.  Every rejection
        // leaves the list empty rather than half filled, and each is preceded
        // by an accepted control so a parser that accepted nothing at all
        // could not pass this.
        algo::strptr bad[] = {"-1", "7-4", "0,,2", "x", "0-", "-", "1-2-3"};
        for (int i = 0; i < int(sizeof(bad)/sizeof(bad[0])); i++) {
            algo::I32RangeAry ary;
            vrfy_(algo::I32RangeAry_ReadStrptrMaybe(ary, "9"));
            algo_lib::ResetErrtext();
            bool ok = algo::I32RangeAry_ReadStrptrMaybe(ary, bad[i]);
            vrfy(!ok, tempstr()<<"accepted a malformed range list"<<Keyval("value",bad[i]));
            vrfyeq_(ary_N(ary), 0);
        }
        algo_lib::ResetErrtext();
    }
}

// Split CMDLINE and join the words with '|', so a test can state the whole
// expected argv as one string.
static tempstr ArgvJoin(strptr cmdline) {
    algo::StringAry words;
    algo::CmdlineToArgv(cmdline, words);
    tempstr ret;
    algo::ListSep ls("|");
    ind_beg(algo::StringAry_ary_curs, word, words) {
        ret << ls << word;
    }ind_end;
    return ret;
}

// The argv split reads a token by scanning to the next break character, and
// the ssim break set is "[]{}()\t \r\n:".  A brace therefore ends a token
// without being read, and a scan that only ever ends tokens never gets past
// one: a bare "{" spun forever, taking x2cli with it, until each such
// character became a word of its own.
void atf_unit::unittest_algo_CmdlineToArgv() {
    vrfyeq_(ArgvJoin(""), "");
    vrfyeq_(ArgvJoin("ls /"), "ls|/");
    vrfyeq_(ArgvJoin("user -update -passhash:abc"), "user|-update|-passhash:abc");
    vrfyeq_(ArgvJoin("a \"b c\""), "a|b c");
    // every break character stands alone as a word, wherever it falls
    algo::strptr brk[] = {"{", "}", "[", "]", "(", ")"};
    for (int i = 0; i < int(sizeof(brk)/sizeof(brk[0])); i++) {
        vrfyeq_(ArgvJoin(brk[i]), brk[i]);
    }
    vrfyeq_(ArgvJoin("ls /a{b"), "ls|/a|{|b");
    vrfyeq_(ArgvJoin("ls {}"), "ls|{|}");
    // quoted, the same character is data and not a word boundary
    vrfyeq_(ArgvJoin("ls \"{\""), "ls|{");
}

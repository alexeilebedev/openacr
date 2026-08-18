// Copyright (C) 2023-2024,2026 AlgoRND
// Copyright (C) 2020-2023 Astra
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
// Target: atf_unit (exe) -- Unit tests (see unittest table)
// Exceptions: yes
// Source: cpp/atf_unit/parsenum.cpp
//

#include "include/atf_unit.h"

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
template <u32 N, typename argtype, typename rettype> static void TestStrNumConv(rettype (*func) (argtype arg, u32 &ok),rettype (*afunc)(argtype arg, u32 len, u32 &ok),rettype (*sfunc)(strptr  arg, u32 &ok)) {
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
        vrfyeq_(ok!=0, true);
    } else {
        aParseNum8(s, ok);
        vrfyeq_(ok!=0, false);
    }
    if (ch_N(s) <= 4) {
        vrfyeq_(aParseNum4(s, ok), n);
        vrfyeq_(ok!=0, true);
    } else {
        aParseNum4(s, ok);
        vrfyeq_(ok!=0, false);
    }
    if (ch_N(s) <= 16) {
        vrfyeq_(aParseNum16(s, ok), n);
        vrfyeq_(ok!=0, true);
    } else {
        aParseNum16(s, ok);
        vrfyeq_(ok!=0, false);
    }
    {
        char c = char(algo::i32_WeakRandom(256));
        if (c>='0' && c<='9') {
            c += 10;
        }
        tempstr t;
        t=s;
        ch_qFind(t, algo::i32_WeakRandom(ch_N(s))) = c;
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

// -----------------------------------------------------------------------------

// One helper for every <T>_ReadStrptrMaybe boundary case: PARSEFN parses STR;
// expect return flag OK and, on success, value RESULT. The message carries the
// input string, so a failing case in a long assertion block is identifiable
// from the report alone. RESULT keeps the call site's literal type and is
// converted to the parsed type for the comparison.
template <typename T, typename V> static void TestParse(bool (*parsefn)(T&, algo::strptr), strptr str, bool ok, V result) {
    T val;
    bool ret = (*parsefn)(val,str);
    vrfy(ret==ok && (!ok || val==T(result)), tempstr()<<"TestParse str:"<<str<<" ret:"<<ret<<" val:"<<val);
}

static void TestTryParseI32(strptr str, bool ok, i32 result) {
    algo::StringIter iter(str);
    i32 val = 0;
    bool ret = algo::TryParseI32(iter, val);
    vrfy(ret==ok && (!ok || val==result)
         , tempstr()<<"TestTryParseI32 str:"<<str<<" ret:"<<ret<<" val:"<<val);
}

static void TestTryParseI64(strptr str, bool ok, i64 result) {
    algo::StringIter iter(str);
    i64 val = 0;
    bool ret = algo::TryParseI64(iter, val);
    vrfy(ret==ok && (!ok || val==result)
         , tempstr()<<"TestTryParseI64 str:"<<str<<" ret:"<<ret<<" val:"<<val);
}

// -----------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_ParseNumber() {
    TestStrNumConv<8,u64,u32>(algo::ParseNum8,0,0);
    TestStrNumConv<4,u32,u32>(algo::ParseNum4,0,0);
    TestStrNumConv<8,u64,u32>(0,algo::ParseNum8,0);
    TestStrNumConv<4,u32,u32>(0,algo::ParseNum4,0);
    TestStrNumConv<8,u64,u32>(0,0,algo::aParseNum8);
    TestStrNumConv<4,u32,u32>(0,0,algo::aParseNum4);
    TestStrNumConv<16,int,u64>(0,0,algo::aParseNum16); // 2nd arg is fake

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
        TestNumber(algo::i32_WeakRandom(INT_MAX) & ((1<<algo::i32_WeakRandom(32))-1));
    }

    {
        u32 ok=1;
        aParseNum8(strptr("ParseNumber_"), ok);
        vrfyeq_(ok, false);
    }

    {
        u32 ok=1;
        aParseNum16(strptr("ParseNumber_"), ok);
        vrfyeq_(ok, false);
    }

    {
        u32 ok=1;
        algo::ParseNum8(0, 0, ok);
        vrfyeq_(ok, false);
    }

    {
        u32 ok=1;
        algo::ParseNum4(0, 0, ok);
        vrfyeq_(ok, false);
    }

    {
        TestParse(i64_ReadStrptrMaybe, "123456789123456789", true, 123456789123456789LL);
        TestParse(i64_ReadStrptrMaybe, "123456789123456789123456776545", true, 0x7fffffffffffffff);
        TestParse(i64_ReadStrptrMaybe, "  0234 ", true, 234);
        TestParse(i64_ReadStrptrMaybe, "  234", true, 234);
        TestParse(i64_ReadStrptrMaybe, "\r\n\t -12222", true, -12222);
        TestParse(i64_ReadStrptrMaybe, "-3333333333333333333", true, -3333333333333333333LL);
        TestParse(i64_ReadStrptrMaybe, "-9223372036854775808", true, LLONG_MIN);
        TestParse(i64_ReadStrptrMaybe, "-99999999999999999999", true, LLONG_MIN);
        TestParse(i64_ReadStrptrMaybe, "9223372036854775807", true, LLONG_MAX);
        // one past either bound clips instead of wrapping through u64
        TestParse(i64_ReadStrptrMaybe, "9223372036854775808", true, LLONG_MAX);
        TestParse(i64_ReadStrptrMaybe, "-9223372036854775809", true, LLONG_MIN);
        TestParse(i64_ReadStrptrMaybe, "2147483647", true, 2147483647);
        TestParse(i64_ReadStrptrMaybe, "2147483648", true, 2147483648);
        TestParse(i64_ReadStrptrMaybe, "2147483649", true, 2147483649);
        TestParse(i64_ReadStrptrMaybe, "3147483648", true, 3147483648);
        TestParse(i64_ReadStrptrMaybe, "4147483649", true, 4147483649);
        TestParse(i64_ReadStrptrMaybe, "12147483649", true, 12147483649);
        TestParse(i64_ReadStrptrMaybe, "0x10", true, 16);
        TestParse(i64_ReadStrptrMaybe, "0x10000", true, 65536);
        TestParse(i64_ReadStrptrMaybe, "0x", false, 0);
        TestParse(i64_ReadStrptrMaybe, "xxx", false, 0);
        TestParse(i64_ReadStrptrMaybe, "0", true, 0);
        TestParse(i64_ReadStrptrMaybe, "1", true, 1);
        TestParse(i64_ReadStrptrMaybe, "-1", true, -1);
        TestParse(i64_ReadStrptrMaybe, "123x", true, 123);
        for (i64 i=0; i<LLONG_MAX-10000; i += LLONG_MAX/10000) {
            TestParse(i64_ReadStrptrMaybe, tempstr()<<i, true, i);
        }
    }

    {
        TestParse(u64_ReadStrptrMaybe, "123456789123456789", true, 123456789123456789LL);
        TestParse(u64_ReadStrptrMaybe, "123456789123456789123456776545", true, 0xffffffffffffffff);
        // clipping at the exact boundary: 2^64-1 parses, anything above clips to it
        TestParse(u64_ReadStrptrMaybe, "18446744073709551615", true, 0xffffffffffffffff);
        TestParse(u64_ReadStrptrMaybe, "18446744073709551616", true, 0xffffffffffffffff);
        TestParse(u64_ReadStrptrMaybe, "18446744073709551619", true, 0xffffffffffffffff);
        TestParse(u64_ReadStrptrMaybe, "18446744073709999999", true, 0xffffffffffffffff);
        TestParse(u64_ReadStrptrMaybe, "0x123456789123456789123456776545", true, 0x6789123456776545);
        TestParse(u64_ReadStrptrMaybe, "  0234 ", true, 234);
        TestParse(u64_ReadStrptrMaybe, "  234", true, 234);
        TestParse(u64_ReadStrptrMaybe, "\r\n\t -12222", true, 0);
        TestParse(u64_ReadStrptrMaybe, "-3333333333333333333", true, 0);
        TestParse(u64_ReadStrptrMaybe, "2147483647", true, 2147483647);
        TestParse(u64_ReadStrptrMaybe, "2147483648", true, 2147483648);
        TestParse(u64_ReadStrptrMaybe, "2147483649", true, 2147483649);
        TestParse(u64_ReadStrptrMaybe, "3147483648", true, 3147483648);
        TestParse(u64_ReadStrptrMaybe, "4147483649", true, 4147483649);
        TestParse(u64_ReadStrptrMaybe, "12147483649", true, 12147483649);
        TestParse(u64_ReadStrptrMaybe, "0x10", true, 16);
        TestParse(u64_ReadStrptrMaybe, "0x10000", true, 65536);
        TestParse(u64_ReadStrptrMaybe, "0x", false, 0);
        TestParse(u64_ReadStrptrMaybe, "xxx", false, 0);
        TestParse(u64_ReadStrptrMaybe, "0", true, 0);
        TestParse(u64_ReadStrptrMaybe, "1", true, 1);
        TestParse(u64_ReadStrptrMaybe, "-1", true, 0);
        TestParse(u64_ReadStrptrMaybe, "123x", true, 123);
        // contract: empty parses as 0, whitespace-only fails, and parsing
        // stops at the first non-digit -- trailing text is silently ignored
        TestParse(u64_ReadStrptrMaybe, "", true, 0);
        TestParse(u64_ReadStrptrMaybe, "   ", false, 0);
        TestParse(u64_ReadStrptrMaybe, "12abc", true, 12);
        TestParse(i64_ReadStrptrMaybe, "", true, 0);
        TestParse(i64_ReadStrptrMaybe, " \t", false, 0);
        TestParse(i64_ReadStrptrMaybe, "12abc", true, 12);
    }

    {
        TestParse(i32_ReadStrptrMaybe, "  0234 ", true, 234);
        TestParse(i32_ReadStrptrMaybe, "  234", true, 234);
        TestParse(i32_ReadStrptrMaybe, "\r\n\t -12222", true, -12222);
        TestParse(i32_ReadStrptrMaybe, "3333333333333333333", true, INT_MAX);
        TestParse(i32_ReadStrptrMaybe, "-3333333333333333333", true, INT_MIN);
        TestParse(i32_ReadStrptrMaybe, "-2147483648", true, INT_MIN);
        TestParse(i32_ReadStrptrMaybe, "2147483647", true, INT_MAX);
        TestParse(i32_ReadStrptrMaybe, "2147483648", true, INT_MAX);
        TestParse(i32_ReadStrptrMaybe, "4147483649", true, INT_MAX);
        TestParse(i32_ReadStrptrMaybe, "12147483649", true, INT_MAX);
        TestParse(i32_ReadStrptrMaybe, "0x10", true, 16);
        TestParse(i32_ReadStrptrMaybe, "0x10000", true, 65536);
        TestParse(i32_ReadStrptrMaybe, "0x", false, 0);
        TestParse(i32_ReadStrptrMaybe, "xxx", false, 0);
        TestParse(i32_ReadStrptrMaybe, "0", true, 0);
        TestParse(i32_ReadStrptrMaybe, "1", true, 1);
        TestParse(i32_ReadStrptrMaybe, "-1", true, -1);
        TestParse(i32_ReadStrptrMaybe, "123x", true, 123);
    }
    {
        TestParse(u32_ReadStrptrMaybe, "  0234 ", true, 234);
        TestParse(u32_ReadStrptrMaybe, "  234", true, 234);
        TestParse(u32_ReadStrptrMaybe, "\r\n\t -12222", true, 0);
        TestParse(u32_ReadStrptrMaybe, "3333333333333333333", true, UINT_MAX);
        TestParse(u32_ReadStrptrMaybe, "-3333333333333333333", true, 0);
        TestParse(u32_ReadStrptrMaybe, "2147483647", true, 2147483647);
        TestParse(u32_ReadStrptrMaybe, "2147483648", true, 2147483648);
        TestParse(u32_ReadStrptrMaybe, "4147483649", true, 4147483649);
        TestParse(u32_ReadStrptrMaybe, "12147483649", true, UINT_MAX);
        TestParse(u32_ReadStrptrMaybe, "1214748364900000", true, UINT_MAX);
        TestParse(u32_ReadStrptrMaybe, "0x10", true, 16);
        TestParse(u32_ReadStrptrMaybe, "0x10000", true, 65536);
        TestParse(u32_ReadStrptrMaybe, "0x", false, 0);
        TestParse(u32_ReadStrptrMaybe, "xxx", false, 0);
        TestParse(u32_ReadStrptrMaybe, "0", true, 0);
        TestParse(u32_ReadStrptrMaybe, "1", true, 1);
        TestParse(u32_ReadStrptrMaybe, "-1", true, 0);
        TestParse(u32_ReadStrptrMaybe, "123x", true, 123);
    }

    {
        TestParse(u128_ReadStrptrMaybe, "123456789123456789", true, 123456789123456789LL);
        TestParse(u128_ReadStrptrMaybe, "123456789123456789123456776545", true, u128(1234567891234)*100000000000000000+u128(56789123456776545));
        TestParse(u128_ReadStrptrMaybe, "123456789123456789123456776545123456789123456789123456776545", true
                  , u128(0xffffffffffffffff)<<64|u128(0xffffffffffffffff));
        // clipping at the exact boundary: 2^128-1 parses, one more clips to it
        TestParse(u128_ReadStrptrMaybe, "340282366920938463463374607431768211455", true
                  , u128(0xffffffffffffffff)<<64|u128(0xffffffffffffffff));
        TestParse(u128_ReadStrptrMaybe, "340282366920938463463374607431768211456", true
                  , u128(0xffffffffffffffff)<<64|u128(0xffffffffffffffff));
        TestParse(u128_ReadStrptrMaybe, "  0234 ", true, 234);
        TestParse(u128_ReadStrptrMaybe, "  234", true, 234);
        TestParse(u128_ReadStrptrMaybe, "\r\n\t -12222", true, 0);
        TestParse(u128_ReadStrptrMaybe, "\r\n\t 12222", true, 12222);
        TestParse(u128_ReadStrptrMaybe, "-3333333333333333333", true, 0);
        TestParse(u128_ReadStrptrMaybe, "2147483647", true, 2147483647);
        TestParse(u128_ReadStrptrMaybe, "2147483648", true, 2147483648);
        TestParse(u128_ReadStrptrMaybe, "2147483649", true, 2147483649);
        TestParse(u128_ReadStrptrMaybe, "3147483648", true, 3147483648);
        TestParse(u128_ReadStrptrMaybe, "4147483649", true, 4147483649);
        TestParse(u128_ReadStrptrMaybe, "12147483649", true, 12147483649);
        TestParse(u128_ReadStrptrMaybe, "0x10", true, 16);
        TestParse(u128_ReadStrptrMaybe, "0x10000", true, 65536);
        TestParse(u128_ReadStrptrMaybe, "0x", false, 0);
        TestParse(u128_ReadStrptrMaybe, "xxx", false, 0);
        TestParse(u128_ReadStrptrMaybe, "0", true, 0);
        TestParse(u128_ReadStrptrMaybe, "1", true, 1);
        TestParse(u128_ReadStrptrMaybe, "-1", true, 0);
        TestParse(u128_ReadStrptrMaybe, "123x", true, 123);
    }

    // The digit window that bounds overflow detection counts significant
    // digits only: leading zeros carry no magnitude and are skipped before
    // the window starts, so a zero-padded value keeps all its digits, and
    // a sign does not shrink the window. Digits past the window imply the
    // value exceeds the type's range, so the standard clip applies.
    {
        TestParse(u8_ReadStrptrMaybe, "00042", true, 42);
        TestParse(u8_ReadStrptrMaybe, "0", true, 0);
        TestParse(u8_ReadStrptrMaybe, "00", true, 0);
        TestParse(u8_ReadStrptrMaybe, "0000000000000000000000255", true, 255);
        TestParse(u8_ReadStrptrMaybe, "000999", true, 255);
        TestParse(i8_ReadStrptrMaybe, "-0", true, 0);
        TestParse(i8_ReadStrptrMaybe, "-00042", true, -42);
        TestParse(i8_ReadStrptrMaybe, "-000128", true, -128);
        TestParse(i8_ReadStrptrMaybe, "-1000", true, -128);
        TestParse(i8_ReadStrptrMaybe, "+1000", true, 127);
        TestParse(u32_ReadStrptrMaybe, "0000000000005", true, 5);
        TestParse(u32_ReadStrptrMaybe, "000000000000000000000000000000", true, 0);
        TestParse(u32_ReadStrptrMaybe, "04294967295", true, UINT_MAX);
        TestParse(u32_ReadStrptrMaybe, "0x00000010", true, 16);
        TestParse(u64_ReadStrptrMaybe, "018446744073709551615", true, 0xffffffffffffffff);
        TestParse(u64_ReadStrptrMaybe, "00000000000000000000000000018446744073709551615", true, 0xffffffffffffffff);
        TestParse(u64_ReadStrptrMaybe, "018446744073709551616", true, 0xffffffffffffffff);
        TestParse(i64_ReadStrptrMaybe, "-09223372036854775808", true, LLONG_MIN);
        TestParse(i64_ReadStrptrMaybe, "000000000000009223372036854775807", true, LLONG_MAX);
        TestParse(u128_ReadStrptrMaybe, "0000000000000000000000000000000000000000340282366920938463463374607431768211455", true
                  , u128(0xffffffffffffffff)<<64|u128(0xffffffffffffffff));
    }

    // Direct coverage of algo::TryParseI32 / algo::TryParseI64 — separate
    // codepath from the *_ReadStrptrMaybe functions the TestParse blocks above
    // exercise. Pins the INT_MIN / INT64_MIN boundary at exact value, both
    // clamp arms, hex form, and parse-failure semantics.
    {
        // Decimal boundaries.
        TestTryParseI64("0", true, 0);
        TestTryParseI64("1", true, 1);
        TestTryParseI64("-1", true, -1);
        TestTryParseI64("9223372036854775807", true, LLONG_MAX);
        TestTryParseI64("-9223372036854775808", true, LLONG_MIN);
        // (v > max) clamp arm — magnitude fits in u64.
        TestTryParseI64("-9223372036854775809", true, LLONG_MIN);
        // overflow flag arm — magnitude does NOT fit in u64.
        TestTryParseI64("-1234567890123456789012345", true, LLONG_MIN);
        TestTryParseI64("1234567890123456789012345", true, LLONG_MAX);
        // Hex boundary.
        TestTryParseI64("-0x8000000000000000", true, LLONG_MIN);
        TestTryParseI64("0x7fffffffffffffff", true, LLONG_MAX);
        // Parse-failure cases — result must be left unchanged (helper inits to 0).
        TestTryParseI64("", false, 0);
        TestTryParseI64("x", false, 0);
    }

    {
        // Decimal boundaries.
        TestTryParseI32("0", true, 0);
        TestTryParseI32("1", true, 1);
        TestTryParseI32("-1", true, -1);
        TestTryParseI32("2147483647", true, INT_MAX);
        TestTryParseI32("-2147483648", true, INT_MIN);
        TestTryParseI32("-2147483649", true, INT_MIN);
        // overflow flag arm.
        TestTryParseI32("-12345678901234567890", true, INT_MIN);
        TestTryParseI32("12345678901234567890", true, INT_MAX);
        // Hex boundary.
        TestTryParseI32("-0x80000000", true, INT_MIN);
        TestTryParseI32("0x7fffffff", true, INT_MAX);
        // Parse-failure cases.
        TestTryParseI32("", false, 0);
        TestTryParseI32("x", false, 0);
    }

    {
        // Transitive coverage for the algo::ParseI32 / algo::ParseI64
        // default-arg wrappers — they forward to TryParseI32/I64 and so
        // inherit the boundary. Bind to a local first: vrfyeq_ may evaluate
        // its first arg twice on failure and ParseI* mutates the iterator.
        algo::StringIter it_min64("-9223372036854775808");
        i64 r64 = algo::ParseI64(it_min64, 0);
        // i64 is typedef'd to signed long, LLONG_MIN is long long — without
        // the cast, vrfyeq_'s operator<< overload resolution is ambiguous.
        vrfyeq_(r64, i64(LLONG_MIN));
        algo::StringIter it_min32("-2147483648");
        i32 r32 = algo::ParseI32(it_min32, 0);
        vrfyeq_(r32, i32(INT_MIN));
    }
}

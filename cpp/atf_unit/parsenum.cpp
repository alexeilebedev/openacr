// Copyright (C) 2023-2024 AlgoRND
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

static void TestParseI32(strptr str, bool ok, i32 result) {
    i32 val;
    bool ret = i32_ReadStrptrMaybe(val,str);
    vrfy(ret==ok && (!ok || val==result), tempstr()<<"TestParseI32 "<<ret<<" "<<val);
}

static void TestParseU32(strptr str, bool ok, u32 result) {
    u32 val;
    bool ret = u32_ReadStrptrMaybe(val,str);
    vrfy(ret==ok && (!ok || val==result), tempstr()<<"TestParseU32 "<<ret<<" "<<val);
}

static void TestParseI64(strptr str, bool ok, i64 result) {
    if (str == "123456789123456789") {
        prlog("sfasdfasd");
    }
    i64 val;
    bool ret = i64_ReadStrptrMaybe(val,str);
    vrfy(ret==ok && (!ok || val==result), tempstr()<<"TestParseI64 "<<ret<<" "<<val);
}

static void TestParseU64(strptr str, bool ok, u64 result) {
    u64 val;
    bool ret = u64_ReadStrptrMaybe(val,str);
    vrfy(ret==ok && (!ok || val==result), tempstr()<<"TestParseU64 "<<ret<<" "<<val);
}

static void TestParseU128(strptr str, bool ok, u128 result) {
    u128 val;
    bool ret = u128_ReadStrptrMaybe(val,str);
    vrfy(ret==ok && (!ok || val==result), tempstr()<<"TestParseU128 "<<ret<<" "<<val);
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
        TestParseI64("123456789123456789", true, 123456789123456789LL);
        TestParseI64("123456789123456789123456776545", true, 0x7fffffffffffffff);
        TestParseI64("  0234 ", true, 234);
        TestParseI64("  234", true, 234);
        TestParseI64("\r\n\t -12222", true, -12222);
        TestParseI64("-3333333333333333333", true, -3333333333333333333LL);
        TestParseI64("2147483647", true, 2147483647);
        TestParseI64("2147483648", true, 2147483648);
        TestParseI64("2147483649", true, 2147483649);
        TestParseI64("3147483648", true, 3147483648);
        TestParseI64("4147483649", true, 4147483649);
        TestParseI64("12147483649", true, 12147483649);
        TestParseI64("0x10", true, 16);
        TestParseI64("0x10000", true, 65536);
        TestParseI64("0x", false, 0);
        TestParseI64("xxx", false, 0);
        TestParseI64("0", true, 0);
        TestParseI64("1", true, 1);
        TestParseI64("-1", true, -1);
        TestParseI64("123x", true, 123);
        for (i64 i=0; i<LLONG_MAX-10000; i += LLONG_MAX/10000) {
            TestParseI64(tempstr()<<i, true, i);
        }
    }

    {
        TestParseU64("123456789123456789", true, 123456789123456789LL);
        TestParseU64("123456789123456789123456776545", true, 0xffffffffffffffff);
        TestParseU64("0x123456789123456789123456776545", true, 0x6789123456776545);
        TestParseU64("  0234 ", true, 234);
        TestParseU64("  234", true, 234);
        TestParseU64("\r\n\t -12222", true, 0);
        TestParseU64("-3333333333333333333", true, 0);
        TestParseU64("2147483647", true, 2147483647);
        TestParseU64("2147483648", true, 2147483648);
        TestParseU64("2147483649", true, 2147483649);
        TestParseU64("3147483648", true, 3147483648);
        TestParseU64("4147483649", true, 4147483649);
        TestParseU64("12147483649", true, 12147483649);
        TestParseU64("0x10", true, 16);
        TestParseU64("0x10000", true, 65536);
        TestParseU64("0x", false, 0);
        TestParseU64("xxx", false, 0);
        TestParseU64("0", true, 0);
        TestParseU64("1", true, 1);
        TestParseU64("-1", true, 0);
        TestParseU64("123x", true, 123);
    }

    {
        TestParseI32("  0234 ", true, 234);
        TestParseI32("  234", true, 234);
        TestParseI32("\r\n\t -12222", true, -12222);
        TestParseI32("3333333333333333333", true, INT_MAX);
        TestParseI32("-3333333333333333333", true, -INT_MAX);
        TestParseI32("2147483647", true, INT_MAX);
        TestParseI32("2147483648", true, INT_MAX);
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
    }
    {
        TestParseU32("  0234 ", true, 234);
        TestParseU32("  234", true, 234);
        TestParseU32("\r\n\t -12222", true, 0);
        TestParseU32("3333333333333333333", true, UINT_MAX);
        TestParseU32("-3333333333333333333", true, 0);
        TestParseU32("2147483647", true, 2147483647);
        TestParseU32("2147483648", true, 2147483648);
        TestParseU32("4147483649", true, 4147483649);
        TestParseU32("12147483649", true, UINT_MAX);
        TestParseU32("1214748364900000", true, UINT_MAX);
        TestParseU32("0x10", true, 16);
        TestParseU32("0x10000", true, 65536);
        TestParseU32("0x", false, 0);
        TestParseU32("xxx", false, 0);
        TestParseU32("0", true, 0);
        TestParseU32("1", true, 1);
        TestParseU32("-1", true, 0);
        TestParseU32("123x", true, 123);
    }

    {
        TestParseU128("123456789123456789", true, 123456789123456789LL);
        TestParseU128("123456789123456789123456776545", true, u128(1234567891234)*100000000000000000+u128(56789123456776545));
        TestParseU128("123456789123456789123456776545123456789123456789123456776545", true
                      , u128(0xffffffffffffffff)<<64|u128(0xffffffffffffffff));
        TestParseU128("  0234 ", true, 234);
        TestParseU128("  234", true, 234);
        TestParseU128("\r\n\t -12222", true, 0);
        TestParseU128("\r\n\t 12222", true, 12222);
        TestParseU128("-3333333333333333333", true, 0);
        TestParseU128("2147483647", true, 2147483647);
        TestParseU128("2147483648", true, 2147483648);
        TestParseU128("2147483649", true, 2147483649);
        TestParseU128("3147483648", true, 3147483648);
        TestParseU128("4147483649", true, 4147483649);
        TestParseU128("12147483649", true, 12147483649);
        TestParseU128("0x10", true, 16);
        TestParseU128("0x10000", true, 65536);
        TestParseU128("0x", false, 0);
        TestParseU128("xxx", false, 0);
        TestParseU128("0", true, 0);
        TestParseU128("1", true, 1);
        TestParseU128("-1", true, 0);
        TestParseU128("123x", true, 123);
    }
}

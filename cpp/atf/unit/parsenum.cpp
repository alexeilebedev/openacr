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
// Target: atf_unit (exe) -- Algo Test Framework: unit tests
// Exceptions: yes
// Source: cpp/atf/unit/parsenum.cpp
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

void atf_unit::unittest_algo_lib_ParseNumber() {
    TestStrNumConv<8,u64,u32>(ParseNum8,0,0);
    TestStrNumConv<4,u32,u32>(ParseNum4,0,0);
    TestStrNumConv<8,u64,u32>(0,ParseNum8,0);
    TestStrNumConv<4,u32,u32>(0,ParseNum4,0);
    TestStrNumConv<8,u64,u32>(0,0,aParseNum8);
    TestStrNumConv<4,u32,u32>(0,0,aParseNum4);
    TestStrNumConv<16,int,u64>(0,0,aParseNum16); // 2nd arg is fake

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
        ParseNum8(0, 0, ok);
        vrfyeq_(ok, false);
    }

    {
        u32 ok=1;
        ParseNum4(0, 0, ok);
        vrfyeq_(ok, false);
    }

}

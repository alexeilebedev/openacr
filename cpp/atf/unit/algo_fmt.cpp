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
// Source: cpp/atf/unit/algo_fmt.cpp
//
// Created By: alexei.lebedev
// Recent Changes: alexei.lebedev
//

#include "include/atf_unit.h"

void atf_unit::unittest_algo_lib_FmtBufDec() {
    char buf[128];
    vrfyeq_(strptr(buf,u64_FmtBufDec(1         , 8, (u8*)buf)), "0.00000001");
    vrfyeq_(strptr(buf,u64_FmtBufDec(10        , 8, (u8*)buf)), "0.0000001");
    vrfyeq_(strptr(buf,u64_FmtBufDec(100       , 8, (u8*)buf)), "0.000001");
    vrfyeq_(strptr(buf,u64_FmtBufDec(1000      , 8, (u8*)buf)), "0.00001");
    vrfyeq_(strptr(buf,u64_FmtBufDec(10000     , 8, (u8*)buf)), "0.0001");
    vrfyeq_(strptr(buf,u64_FmtBufDec(100000    , 8, (u8*)buf)), "0.001");
    vrfyeq_(strptr(buf,u64_FmtBufDec(1000000   , 8, (u8*)buf)), "0.01");
    vrfyeq_(strptr(buf,u64_FmtBufDec(10000000  , 8, (u8*)buf)), "0.1");
    vrfyeq_(strptr(buf,u64_FmtBufDec(100000000 , 8, (u8*)buf)), "1");

    vrfyeq_(strptr(buf,u64_FmtBufDec(1         , 3, (u8*)buf, false)), "0.001");
    vrfyeq_(strptr(buf,u64_FmtBufDec(10        , 3, (u8*)buf, false)), "0.01");
    vrfyeq_(strptr(buf,u64_FmtBufDec(100       , 3, (u8*)buf, false)), "0.1");
    vrfyeq_(strptr(buf,u64_FmtBufDec(1000      , 3, (u8*)buf, false)), "1");
    vrfyeq_(strptr(buf,u64_FmtBufDec(10000     , 3, (u8*)buf, false)), "10");
    vrfyeq_(strptr(buf,u64_FmtBufDec(100000    , 3, (u8*)buf, false)), "100");

    vrfyeq_(strptr(buf,u64_FmtBufDec(1         , 3, (u8*)buf, true)), "0.001");
    vrfyeq_(strptr(buf,u64_FmtBufDec(10        , 3, (u8*)buf, true)), "0.010");
    vrfyeq_(strptr(buf,u64_FmtBufDec(100       , 3, (u8*)buf, true)), "0.100");
    vrfyeq_(strptr(buf,u64_FmtBufDec(1000      , 3, (u8*)buf, true)), "1.000");
    vrfyeq_(strptr(buf,u64_FmtBufDec(10000     , 3, (u8*)buf, true)), "10.000");
    vrfyeq_(strptr(buf,u64_FmtBufDec(100000    , 3, (u8*)buf, true)), "100.000");

    vrfyeq_(strptr(buf,u64_FmtBufDec(0         , 2, (u8*)buf, false)), "0");
    vrfyeq_(strptr(buf,u64_FmtBufDec(0         , 2, (u8*)buf, true) ), "0.00");
    vrfyeq_(strptr(buf,u64_FmtBufDec(123       , 1, (u8*)buf, false)), "12.3");
    vrfyeq_(strptr(buf,u64_FmtBufDec(123       , 2, (u8*)buf, false)), "1.23");
    vrfyeq_(strptr(buf,u64_FmtBufDec(123       , 2, (u8*)buf, true) ), "1.23");
    vrfyeq_(strptr(buf,u64_FmtBufDec(1230      , 3, (u8*)buf, false)), "1.23");
    vrfyeq_(strptr(buf,u64_FmtBufDec(1230      , 3, (u8*)buf, true) ), "1.230");



    vrfyeq_(strptr(buf,u64_FmtBufDec(1434520000      , 6, (u8*)buf, true) ), "1434.520000");

    vrfyeq_(strptr(buf,u64_FmtBufDec(0, 0, (u8*)buf)), "0");
    vrfyeq_(strptr(buf,u64_FmtBufDec(1234567812345678ULL, 0, (u8*)buf)), "1234567812345678");
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

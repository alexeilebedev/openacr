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
// Target: atf_amc (exe) -- Unit tests for amc (see amctest table)
// Exceptions: NO
// Source: cpp/atf/amc/bitfld.cpp
//
// Created By: alexei.lebedev
// Recent Changes: alexei.lebedev
//

#include "include/atf_amc.h"

void atf_amc::amctest_TestBitfld() {
    atf_amc::BitfldU16 f;
    vrfyeq_(bits0_4_Get(f),0);
    vrfyeq_(bits8_12_Get(f),0);

    bits0_4_Set(f, 3);
    vrfyeq_(bits0_4_Get(f),3);
    vrfyeq_(bits8_12_Get(f),0);

    bits8_12_Set(f, 5);
    vrfyeq_(bits0_4_Get(f),3);
    vrfyeq_(bits8_12_Get(f),5);

    bits0_4_Set(f, 0);
    vrfyeq_(bits0_4_Get(f),0);
    vrfyeq_(bits8_12_Get(f),5);
}

void atf_amc::amctest_TestBitfld2() {
    atf_amc::BitfldU128 f;
    vrfyeq_(bits1_65_Get(f),u64(0));
    vrfyeq_(bits65_128_Get(f),u64(0));

    bits1_65_Set(f, u64(0x0123456789abcdef));
    vrfyeq_(bits1_65_Get(f),u64(0x0123456789abcdef));
    vrfyeq_(bits65_128_Get(f),u64(0));

    bits65_128_Set(f, u64(0x7edcba9876543210));// use 63 bits
    vrfyeq_(bits1_65_Get(f),u64(0x0123456789abcdef));
    vrfyeq_(bits65_128_Get(f),u64(0x7edcba9876543210));

    bits1_65_Set(f, u64(0));
    vrfyeq_(bits1_65_Get(f),u64(0));
    vrfyeq_(bits65_128_Get(f),u64(0x7edcba9876543210));
}

// -----------------------------------------------------------------------------

// Big-endian bitfield test.
// Set bits 0..4
// Set bits 8..12
// Set bits 0..4 again
// At each step, check that total field has the expected value
void atf_amc::amctest_BitfldNet() {
    atf_amc::NetBitfld1 f;
    vrfyeq_(bits0_4_Get(f),0);
    vrfyeq_(bits8_12_Get(f),0);

    bits0_4_Set(f, 3);
    vrfyeq_(bits0_4_Get(f),3);
    vrfyeq_(bits8_12_Get(f),0);

    bits8_12_Set(f, 5);
    vrfyeq_(bits0_4_Get(f),3);
    vrfyeq_(bits8_12_Get(f),5);

    bits0_4_Set(f, 0);
    vrfyeq_(bits0_4_Get(f),0);
    vrfyeq_(bits8_12_Get(f),5);
}

// -----------------------------------------------------------------------------

void atf_amc::amctest_BitfldTuple() {
    atf_amc::BitfldType1 var1;
    bit1_Set(var1,1);
    bits5_Set(var1,17);
    cstring str;
    str << var1;
    vrfyeq_(str, "atf_amc.BitfldType1  bit1:1  bits5:17");

    atf_amc::BitfldType1 var2;
    BitfldType1_ReadStrptrMaybe(var2,str);
    vrfyeq_(var1.value,var2.value);
}

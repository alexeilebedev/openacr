// Copyright (C) 2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
//
// License: Apache
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Contacting ICE: <https://www.theice.com/contact>
// Target: atf_amc (exe) -- Unit tests for amc (see amctest table)
// Exceptions: yes
// Source: cpp/atf_amc/bitfld.cpp
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

// -----------------------------------------------------------------------------

// Fconst on a bitfld field: the field has no direct member, so the numeric
// fallback of ReadStrptrMaybe stores through the generated Set
void atf_amc::amctest_BitfldFconst() {
    atf_amc::BitfldType1 bitfld_type1;
    // symbolic read via the fconst table
    vrfy_(atf_amc::bits5_ReadStrptrMaybe(bitfld_type1, "high"));
    vrfyeq_(bits5_Get(bitfld_type1), 1000);
    // numeric fallback
    vrfy_(atf_amc::bits5_ReadStrptrMaybe(bitfld_type1, "9"));
    vrfyeq_(bits5_Get(bitfld_type1), 9);
    // unknown symbol fails and leaves the field unchanged
    vrfy_(!atf_amc::bits5_ReadStrptrMaybe(bitfld_type1, "zzz"));
    vrfyeq_(bits5_Get(bitfld_type1), 9);
}

// -----------------------------------------------------------------------------

void atf_amc::amctest_BitfldBitset() {
    {
        atf_amc::BitfldType2 var1;
        bit0_Set(var1,true);
        bit1_Set(var1,true);
        var1.freebool = true;
        cstring str;
        str << var1;
        vrfyeq_(str, "bit0,bit1,freebool");

        atf_amc::BitfldType2 var2;
        vrfy_(BitfldType2_ReadStrptrMaybe(var2,str));
        vrfyeq_(var1.value,var2.value);
    }
    {
        atf_amc::BitfldType2 var1;
        bit0_Set(var1,true);
        cstring str;
        str << var1;
        vrfyeq_(str, "bit0");

        atf_amc::BitfldType2 var2;
        vrfy_(BitfldType2_ReadStrptrMaybe(var2,str));
        vrfyeq_(var1.value,var2.value);
    }
    {
        atf_amc::BitfldType2 var1;
        bit1_Set(var1,true);
        cstring str;
        str << var1;
        vrfyeq_(str, "bit1");

        atf_amc::BitfldType2 var2;
        vrfy_(BitfldType2_ReadStrptrMaybe(var2,str));
        vrfyeq_(var1.value,var2.value);
    }
    {
        atf_amc::BitfldType2 var1;
        var1.freebool = true;;
        cstring str;
        str << var1;
        vrfyeq_(str, "freebool");

        atf_amc::BitfldType2 var2;
        vrfy_(BitfldType2_ReadStrptrMaybe(var2,str));
        vrfyeq_(var1.value,var2.value);
    }
    {
        atf_amc::BitfldType2 var;
        vrfy_(BitfldType2_ReadStrptrMaybe(var,",, ,  "));
        vrfy_(!bit0_Get(var));
        vrfy_(!bit1_Get(var));
    }
    {
        atf_amc::BitfldType2 var;
        vrfy_(BitfldType2_ReadStrptrMaybe(var,",,, freebool , bit1 , , bit0 "));
        vrfy_(bit0_Get(var));
        vrfy_(bit1_Get(var));
        vrfy_(var.freebool);
    }
    {
        atf_amc::BitfldType2 var;
        vrfy_(!BitfldType2_ReadStrptrMaybe(var,"blah"));
        vrfy_(FindStr(algo_lib::DetachBadTags(),"blah")!=-1);
    }
    {
        atf_amc::BitfldType2 var;
        vrfy_(!BitfldType2_ReadStrptrMaybe(var,"bit00"));
        vrfy_(FindStr(algo_lib::DetachBadTags(),"bit00")!=-1);
    }
    {
        atf_amc::BitfldType2 var;
        vrfy_(!BitfldType2_ReadStrptrMaybe(var,"bbit0"));
        vrfy_(FindStr(algo_lib::DetachBadTags(),"bbit0")!=-1);
    }
    {
        atf_amc::BitfldType2 var;
        vrfy_(!BitfldType2_ReadStrptrMaybe(var,"bit0bit1freebool"));
        vrfy_(FindStr(algo_lib::DetachBadTags(),"bit0bit1freebool")!=-1);
    }
}

// --------------------------------------------------------------------------------

// Bitfld on a global (FDb) ctype: the default value is applied at init,
// and Get/Set take no parent argument.
void atf_amc::amctest_BitfldGlobal() {
    vrfyeq_(atf_amc::flaglo_Get(), u32(3));
    vrfyeq_(atf_amc::_db.flagbits, u32(3));
    atf_amc::flaglo_Set(9);
    vrfyeq_(atf_amc::_db.flagbits, u32(9));
    atf_amc::flaglo_Set(3);
}

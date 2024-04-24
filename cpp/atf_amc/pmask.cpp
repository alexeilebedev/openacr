// Copyright (C) 2023-2024 AlgoRND
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
// Exceptions: NO
// Source: cpp/atf_amc/pmask.cpp
//

#include "include/atf_amc.h"

void atf_amc::amctest_TestPmask1() {
    atf_amc::PmaskU32 pmask_u32;
    vrfyeq_(value_PresentQ(pmask_u32), false);
}

void atf_amc::amctest_TestPmask2() {
    atf_amc::PmaskU32 pmask_u32;
    value_Set(pmask_u32, 33);
    vrfyeq_(value_PresentQ(pmask_u32), true);
    vrfyeq_(pmask_u32.value, (u32)33);
}

void atf_amc::amctest_TestPmask3() {
    atf_amc::PmaskU32 pmask_u32;
    PmaskU32_ReadStrptrMaybe(pmask_u32, "atf_amc.PmaskU32  unrelated_key:unrelated_value");
    vrfyeq_(value_PresentQ(pmask_u32), false);
}

void atf_amc::amctest_TestPmask4() {
    atf_amc::PmaskU32 pmask_u32;
    PmaskU32_ReadStrptrMaybe(pmask_u32, "atf_amc.PmaskU32  value:33");
    vrfyeq_(value_PresentQ(pmask_u32), true);
    value_Set(pmask_u32, (u32)33);
}

void atf_amc::amctest_TestPmask5() {
    atf_amc::PmaskU32 pmask_u32;
    vrfyeq_(pmask_u32.pmask, (u32)0);
    value_Set(pmask_u32, 1);
    vrfyeq_(pmask_u32.pmask, (u32)0x1);
    value2_Set(pmask_u32, 2);
    vrfyeq_(pmask_u32.pmask, (u32)0x3);
    value3_Set(pmask_u32, 3);
    vrfyeq_(pmask_u32.pmask, (u32)0x7);
    value4_Set(pmask_u32, 4);
    vrfyeq_(pmask_u32.pmask, (u32)0xf);
    value5_Set(pmask_u32, 5);
    vrfyeq_(pmask_u32.pmask, (u32)0x1f);
}

void atf_amc::amctest_TestPmask6() {
    atf_amc::PmaskU32 pmask_u32;
    PmaskU32_ReadStrptrMaybe(pmask_u32, "atf_amc.PmaskU32  pmask:11");
    vrfyeq_(pmask_u32.pmask, (u32)0);
}

void atf_amc::amctest_TestPmask7() {
    atf_amc::PmaskU128 pmask_u128;
    value63_Set(pmask_u128, 63);
    value64_Set(pmask_u128, 64);
    value71_Set(pmask_u128, 71);
    vrfyeq_(value55_PresentQ(pmask_u128), false);
    vrfyeq_(value63_PresentQ(pmask_u128), true);
    vrfyeq_(value64_PresentQ(pmask_u128), true);
    vrfyeq_(value71_PresentQ(pmask_u128), true);
    vrfyeq_(pmask_u128.value71, (u32)71);
    vrfyeq_(pmask_u128.value63, (u32)63);
    vrfyeq_(pmask_u128.value64, (u32)64);
}

void atf_amc::amctest_TestPmask8() {
    atf_amc::PmaskU555 pmask_u555;
    value_Set(pmask_u555, 1);
    vrfyeq_(pmask_qGetBit(pmask_u555, 0), 1);
}

void atf_amc::amctest_PmaskMultiple() {
    atf_amc::PmaskMultiple p;
    // check that value1 is in none of the pmasks
    vrfyeq_(value1_PresentQ(p),false);
    vrfyeq_(value1_AssignedQ(p),false);
    vrfyeq_(value1_NullableQ(p),false);

    vrfyeq_(value3_Present_GetBit(p),2);
    vrfyeq_(value3_Assigned_GetBit(p),2);
    vrfyeq_(value3_Nullable_GetBit(p),1);

    // set values
    value1_Set(p,1);
    value2_Set(p,2);

    // check that value1 is in all pmasks
    vrfyeq_(value1_PresentQ(p),true);
    vrfyeq_(value1_AssignedQ(p),true);
    vrfyeq_(value1_NullableQ(p),true);

    // pmask3 controls printing.
    prlog(p);
    vrfyeq_(tempstr()<<p, "atf_amc.PmaskMultiple  value1:1  value2:2  value4:0  value6:0  value7:0");
}

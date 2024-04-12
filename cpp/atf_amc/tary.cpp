// Copyright (C) 2023-2024 AlgoRND
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
// Target: atf_amc (exe) -- Unit tests for amc (see amctest table)
// Exceptions: NO
// Source: cpp/atf_amc/tary.cpp
//

#include "include/atf_amc.h"


void atf_amc::amctest_TaryInit() {
    atf_amc::TaryU32 tary_u32;
    frep_(i,100) {
        u32 &elem = tary_u32_Alloc(tary_u32);
        vrfyeq_(elem, u32(44));
    }
}

void atf_amc::amctest_TaryInit2() {
    atf_amc::TaryU32 tary_u32;
    frep_(i,100) {
        algo::aryptr<u32> elems = tary_u32_AllocN(tary_u32, i);
        frep_(j,elems_N(elems)) {
            vrfyeq_(elems[j], u32(44));
        }
    }
}

void atf_amc::amctest_TaryInit3() {
    atf_amc::TaryU8 tary_u8;
    frep_(i,100) {
        u8 &elem = ary_Alloc(tary_u8);
        vrfyeq_(elem, u8(44));
    }
}

void atf_amc::amctest_TaryInit4() {
    atf_amc::TaryU8 tary_u8;
    frep_(i,100) {
        algo::aryptr<u8> elems = ary_AllocN(tary_u8, i);
        frep_(j,elems_N(elems)) {
            vrfyeq_(elems[j], u8(44));
        }
    }
}

void atf_amc::amctest_TaryReserve() {
    atf_amc::TaryU8 tary_u8;
    vrfy_(ary_N(tary_u8) + 1 > ary_Max(tary_u8));
    ary_Reserve(tary_u8,1);
    vrfy_(ary_N(tary_u8) + 1 <= ary_Max(tary_u8));
    vrfy_(ary_N(tary_u8) + 10 > ary_Max(tary_u8));
    ary_Reserve(tary_u8,10);
    vrfy_(ary_N(tary_u8) + 10 <= ary_Max(tary_u8));
    vrfy_(ary_N(tary_u8) + 100 > ary_Max(tary_u8));
    ary_Reserve(tary_u8,100);
    vrfy_(ary_N(tary_u8) + 100 <= ary_Max(tary_u8));
    vrfy_(ary_N(tary_u8) + 1000 > ary_Max(tary_u8));
}

// Copyright (C) 2023 AlgoRND
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
// Target: atf_amc (exe) -- Unit tests for amc (see amctest table)
// Exceptions: NO
// Source: cpp/atf/amc/bitset.cpp
//

#include "include/atf_amc.h"

void atf_amc::amctest_BitsetTary64() {
    algo_lib::Bitset B;
    ary_ExpandBits(B, 100);
    vrfy_(ary_N(B) == 2);
    ary_qSetBit(B, 0);
    ary_qSetBit(B, 4);
    ary_qSetBit(B, 63);
    vrfy_(ary_Sum1s(B) == 3);
    vrfy_( ary_qGetBit(B, 0));
    vrfy_( ary_qGetBit(B, 4));
    vrfy_( ary_qGetBit(B, 63));
    vrfy_(!ary_qGetBit(B, 64));
    vrfy_(!ary_qGetBit(B, 1));

    ary_qClearBit(B, 0);
    vrfy_(ary_Sum1s(B) == 2);
    vrfy_(!ary_qGetBit(B, 0));

    ary_ClearBitsAll(B);
    vrfy_(ary_Sum1s(B)==0);

    ary_AllocBit(B, 1000);
    ary_qSetBit(B, 1000);
    vrfy_(ary_qGetBit(B, 1000));
}

void atf_amc::amctest_BitsetInlary16() {
    Bitset frame;
    vrfyeq_(fld1_Sum1s(frame), u64(0));
    vrfyeq_(fld1_BitsEmptyQ(frame), true);
    Fill(fld1_Getary(frame), (short unsigned int)-1);
    vrfyeq_(fld1_Sum1s(frame), u64(64));
    vrfyeq_(fld1_BitsEmptyQ(frame), false);

    fld1_qClearBit(frame, 4);
    vrfyeq_(fld1_qGetBit(frame, 4), false);
    vrfyeq_(fld1_Sum1s(frame), u64(63));
    vrfyeq_(fld1_BitsEmptyQ(frame), false);

    fld1_ClearBit(frame, 64); // out of range
    vrfyeq_(fld1_Sum1s(frame), u64(63));
    vrfyeq_(fld1_BitsEmptyQ(frame), false);

    fld1_ClearBit(frame, 63);
    vrfyeq_(fld1_Sup(frame),63);

    fld1_qClearBit(frame, 1);

    vrfy_( fld1_qGetBit(frame, 0));
    vrfy_(!fld1_qGetBit(frame, 1));
    vrfy_( fld1_qGetBit(frame, 2));
    vrfy_( fld1_qGetBit(frame, 3));
    vrfy_(!fld1_qGetBit(frame, 4));
    vrfy_(!fld1_qGetBit(frame, 63));
    vrfyeq_(fld1_Sum1s(frame), u64(61));
    vrfyeq_(fld1_BitsEmptyQ(frame), false);
    fld1_ClearBitsAll(frame);
    vrfyeq_(fld1_Sum1s(frame), u64(0));
    vrfyeq_(fld1_BitsEmptyQ(frame), true);
}

void atf_amc::amctest_BitsetVal8() {
    Bitset frame;
    vrfyeq_(frame.fld8, u8(0));

    // set all 8 bits one by one
    for(int i=0; i<8; i++) {
        fld8_qSetBit(frame, i);
        vrfyeq_(frame.fld8, (i==7 ? u8(-1) : (u8(1)<<(i+1))-1));
        vrfyeq_(fld8_Sum1s(frame), u8(i+1));
        vrfyeq_(fld8_Sup(frame), i+1);
    }
    // clear all 8 bits one by one
    for (int i=7; i>=0; i--) {
        fld8_qClearBit(frame, i);
        vrfyeq_(frame.fld8, (u8(1)<<i)-1);
        vrfyeq_(fld8_Sum1s(frame), u8(i));
        vrfyeq_(fld8_Sup(frame), i);
    }
}

void atf_amc::amctest_BitsetVal64() {
    Bitset frame;
    vrfyeq_(frame.fld64, u64(0));

    // set all 64 bits one by one
    for(int i=0; i<64; i++) {
        fld64_qSetBit(frame, i);
        vrfyeq_(frame.fld64, (i==63 ? u64(-1) : (u64(1)<<(i+1))-1));
        vrfyeq_(fld64_Sum1s(frame), u64(i+1));
        vrfyeq_(fld64_Sup(frame), i+1);
        vrfyeq_(fld64_BitsEmptyQ(frame), false);
    }
    // clear all 64 bits one by one
    for (int i=63; i>=0; i--) {
        fld64_qClearBit(frame, i);
        vrfyeq_(frame.fld64, (u64(1)<<i)-1);
        vrfyeq_(fld64_Sum1s(frame), u64(i));
        vrfyeq_(fld64_Sup(frame), i);
        vrfyeq_(fld64_BitsEmptyQ(frame), i==0);
    }
}

void atf_amc::amctest_BitsetVal128() {
    Bitset frame;
    vrfyeq_(frame.fld128, u128(0));

    // set all 128 bits one by one
    for(int i=0; i<128; i++) {
        fld128_qSetBit(frame, i);
        vrfyeq_(frame.fld128, (u64(i)==127 ? u128(-1) : (u128(1)<<(i+1))-u128(1)));
        vrfyeq_(fld128_Sum1s(frame), u128(i+1));
    }
    // clear all 128 bits one by one
    for (int i=127; i>=0; i--) {
        fld128_qClearBit(frame, i);
        vrfyeq_(frame.fld128, (u128(1)<<i)-1);
        vrfyeq_(fld128_Sum1s(frame), u128(i));
    }
}

#define TEST_BITCURS(CURSTYPE,PARENT,EXPECTED)  \
    do {                                        \
        cstring bits;                           \
        algo::ListSep ls(",");                  \
        ind_beg(CURSTYPE,i,PARENT) {            \
            bits << ls << i;                    \
        }ind_end;                               \
        vrfyeq_(bits,strptr(EXPECTED))          \
            } while (0)

void atf_amc::amctest_BitsetBitcurs() {
    {
        algo_lib::Bitset B;
        TEST_BITCURS(algo_lib::Bitset_ary_bitcurs,B,"");
        ary_AllocBit(B, 1000);
        TEST_BITCURS(algo_lib::Bitset_ary_bitcurs,B,"");
        ary_qSetBit(B, 0);
        ary_qSetBit(B, 127);
        ary_qSetBit(B, 128);
        ary_qSetBit(B, 150);
        ary_qSetBit(B, 1000);
        TEST_BITCURS(algo_lib::Bitset_ary_bitcurs,B,"0,127,128,150,1000");
    }

    {
        Bitset B;
        TEST_BITCURS(Bitset_fld1_bitcurs,B,"");
        fld1_qSetBit(B, 0);
        fld1_qSetBit(B, 3);
        fld1_qSetBit(B, 15);
        fld1_qSetBit(B, 16);
        fld1_qSetBit(B, 43);
        fld1_qSetBit(B, 63);
        TEST_BITCURS(Bitset_fld1_bitcurs,B,"0,3,15,16,43,63");
    }

    {
        Bitset B;
        TEST_BITCURS(Bitset_fld8_bitcurs,B,"");
        fld8_qSetBit(B, 0);
        fld8_qSetBit(B, 2);
        fld8_qSetBit(B, 5);
        fld8_qSetBit(B, 7);
        TEST_BITCURS(Bitset_fld8_bitcurs,B,"0,2,5,7");
    }

    {
        Bitset B;
        TEST_BITCURS(Bitset_fld64_bitcurs,B,"");
        fld64_qSetBit(B, 0);
        fld64_qSetBit(B, 3);
        fld64_qSetBit(B, 15);
        fld64_qSetBit(B, 16);
        fld64_qSetBit(B, 43);
        fld64_qSetBit(B, 63);
        TEST_BITCURS(Bitset_fld64_bitcurs,B,"0,3,15,16,43,63");
    }
}

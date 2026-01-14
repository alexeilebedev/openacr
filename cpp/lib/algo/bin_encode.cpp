// Copyright (C) 2024-2026 AlgoRND
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
// Target: algo_lib (lib) -- Support library for all executables
// Exceptions: NO
// Source: cpp/lib/algo/bin_encode.cpp
//

#include "include/algo.h"

////////////////////////////////////////////////////////////////////////////////
//                           FIXED-LENGTH INTEGERS                            //
////////////////////////////////////////////////////////////////////////////////

void algo::EncodeBoolean(algo::ByteAry &buf, bool value) {
    ary_Alloc(buf) = value;
}

void algo::EncodeU8(algo::ByteAry &buf, u8 value) {
    ary_Alloc(buf) = value;
}

void algo::EncodeI8(algo::ByteAry &buf, i8 value) {
    ary_Alloc(buf) = value;
}

void algo::EncodeBEU16(algo::ByteAry &buf, u16 value) {
    *(u16*)ary_AllocN(buf,sizeof value).elems = htobe16(value);
}

void algo::EncodeBEI16(algo::ByteAry &buf, i16 value) {
    EncodeBEU16(buf,value);
}

void algo::EncodeBEU32(algo::ByteAry &buf, u32 value) {
    *(u32*)ary_AllocN(buf,sizeof value).elems = htobe32(value);
}

void algo::EncodeBEI32(algo::ByteAry &buf, i32 value) {
    EncodeBEU32(buf,value);
}

void algo::EncodeBEU64(algo::ByteAry &buf, u64 value) {
    *(u64*)ary_AllocN(buf,sizeof value).elems = htobe64(value);
}

void algo::EncodeBEI64(algo::ByteAry &buf, i64 value) {
    EncodeBEU64(buf,value);
}

////////////////////////////////////////////////////////////////////////////////
//                                    FLOAT                                   //
////////////////////////////////////////////////////////////////////////////////

void algo::EncodeBEF64(algo::ByteAry &buf, double value) {
#ifdef __STDC_IEC_559__
    char *ptr = (char*)&value;
    EncodeBEU64(buf,*(u64*)ptr);
#else
#error "Unconformant float representation format"
#endif
}

////////////////////////////////////////////////////////////////////////////////
//                           VARIABLE-LENGTH INTEGERS                         //
////////////////////////////////////////////////////////////////////////////////

// variable-length, continuation bit, u32
void algo::EncodeVLCLEU32(algo::ByteAry &buf, u32 value) {
    algo::EncodeVLCLEU64(buf,value);
}

// variable-length, continuation bit, i32, zigzag
void algo::EncodeVLCLEI32Z(algo::ByteAry &buf, i32 value) {
    EncodeVLCLEI64Z(buf,value);
}

// variable-length, continuation bit, u64
void algo::EncodeVLCLEU64(algo::ByteAry &buf, u64 value) {
    if (value) {
        int n_bits = algo::u64_BitScanReverse(value) + 1;
        int n_bytes = (n_bits + 6) / 7;
        algo::memptr bytes = ary_AllocN(buf,n_bytes);
        frep_(i,n_bytes) {
            bytes[i] = (value & 0x7f) | 0x80;
            value = value >> 7;
        }
        bytes[n_bytes-1] &= 0x7f;
    } else {
        ary_AllocN(buf,1).elems[0] = 0;
    }
}

// variable-length, continuation bit, i64, zigzag
void algo::EncodeVLCLEI64Z(algo::ByteAry &buf, i64 value) {
    u64 tmp = (value << 1) ^ (value >> 63);
    EncodeVLCLEU64(buf,tmp);
}

////////////////////////////////////////////////////////////////////////////////
//                                SPECIFIC TYPES                              //
////////////////////////////////////////////////////////////////////////////////

// UUID - raw bytes
void algo::EncodeUuid(algo::ByteAry &buf, algo::Uuid &value) {
    algo::memptr tmp((u8*)&value,sizeof value);
    ary_Addary(buf,tmp);
}

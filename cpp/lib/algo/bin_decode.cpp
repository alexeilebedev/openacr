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
// Source: cpp/lib/algo/bin_decode.cpp
//

#include "include/algo.h"

////////////////////////////////////////////////////////////////////////////////
//                           FIXED-LENGTH INTEGERS                            //
////////////////////////////////////////////////////////////////////////////////

// single byte
bool algo::DecodeU8(algo::memptr &buf, u8 &result) {
    result = 0;
    bool ok = elems_N(buf) >= 1;
    if (ok) {
        result = buf.elems[0];
        buf = RestFrom(buf,1);
    }
    return ok;
}

//------------------------------------------------------------------------------

// single byte, signed
bool algo::DecodeI8(algo::memptr &buf, i8 &result) {
    u8 tmp(0);
    bool ok = algo::DecodeU8(buf,tmp);
    result = tmp;
    return ok;
}

//------------------------------------------------------------------------------

// single byte, boolean
bool algo::DecodeBoolean(algo::memptr &buf, bool &result) {
    u8 tmp(0);
    bool ok = algo::DecodeU8(buf,tmp);
    result = tmp;
    return ok;
}

//------------------------------------------------------------------------------

// two-byte, big-endian
bool algo::DecodeBEU16(algo::memptr &buf, u16 &result) {
    constexpr int len = sizeof result;
    result = 0;
    bool ok = buf.n_elems >= len;
    if (ok) {
        result = be16toh(*(u16*)buf.elems);
        buf = RestFrom(buf,len);
    }
    return ok;
}

//------------------------------------------------------------------------------

// two-byte, big endian, signed
bool algo::DecodeBEI16(algo::memptr &buf, i16 &result) {
    u16 tmp;
    bool ok = algo::DecodeBEU16(buf,tmp);
    result = tmp;
    return ok;
}

//------------------------------------------------------------------------------

// four-byte, big-endian
bool algo::DecodeBEU32(algo::memptr &buf, u32 &result) {
    constexpr int len = sizeof result;
    result = 0;
    bool ok = buf.n_elems >= len;
    if (ok) {
        result = be32toh(*(u32*)buf.elems);
        buf = RestFrom(buf,len);
    }
    return ok;
}

//------------------------------------------------------------------------------

// four-byte, big-endian signed
bool algo::DecodeBEI32(algo::memptr &buf, i32 &result) {
    u32 tmp;
    bool ok = algo::DecodeBEU32(buf,tmp);
    result = tmp;
    return ok;
}

//------------------------------------------------------------------------------

// eight-byte, big-endian
bool algo::DecodeBEU64(algo::memptr &buf, u64 &result) {
    constexpr int len = sizeof result;
    result = 0;
    bool ok = buf.n_elems >= len;
    if (ok) {
        result = be64toh(*(u64*)buf.elems);
        buf = RestFrom(buf,len);
    }
    return ok;
}

//------------------------------------------------------------------------------

// eight-byte, big-endian signed
bool algo::DecodeBEI64(algo::memptr &buf, i64 &result) {
    u64 tmp;
    bool ok = algo::DecodeBEU64(buf,tmp);
    result = tmp;
    return ok;
}

// four-byte, little-endian
bool algo::DecodeLEU32(algo::memptr &buf, u32 &result) {
    constexpr int len = sizeof result;
    result = 0;
    bool ok = buf.n_elems >= len;
    if (ok) {
        result = le32toh(*(u32*)buf.elems);
        buf = RestFrom(buf,len);
    }
    return ok;
}

//------------------------------------------------------------------------------

// four-byte, little-endian signed
bool algo::DecodeLEI32(algo::memptr &buf, i32 &result) {
    u32 tmp;
    bool ok = algo::DecodeLEU32(buf,tmp);
    result = tmp;
    return ok;
}

//------------------------------------------------------------------------------

// eight-byte, little-endian
bool algo::DecodeLEU64(algo::memptr &buf, u64 &result) {
    constexpr int len = sizeof result;
    result = 0;
    bool ok = buf.n_elems >= len;
    if (ok) {
        result = le64toh(*(u64*)buf.elems);
        buf = RestFrom(buf,len);
    }
    return ok;
}

//------------------------------------------------------------------------------

// eight-byte, little-endian signed
bool algo::DecodeLEI64(algo::memptr &buf, i64 &result) {
    u64 tmp;
    bool ok = algo::DecodeLEU64(buf,tmp);
    result = tmp;
    return ok;
}

////////////////////////////////////////////////////////////////////////////////
//                                    FLOAT                                   //
////////////////////////////////////////////////////////////////////////////////

bool algo::DecodeBEF64(algo::memptr &buf, double &result) {
#if defined(__STDC_IEC_559__) || defined(__APPLE__)
    u64 tmp;
    bool ok = algo::DecodeBEU64(buf,tmp);
    char *ptr = (char*)&tmp;
    result = *(double*)ptr;
    return ok;
#else
#error "Unconformant float representation format"
#endif
}

bool algo::DecodeLEF32(algo::memptr &buf, float &result) {
#if defined(__STDC_IEC_559__) || defined(__APPLE__)
    u32 tmp;
    bool ok = algo::DecodeLEU32(buf,tmp);
    memcpy(&result,&tmp,sizeof result);
    return ok;
#else
#error "Unconformant float representation format"
#endif
}

bool algo::DecodeLEF64(algo::memptr &buf, double &result) {
#if defined(__STDC_IEC_559__) || defined(__APPLE__)
    u64 tmp;
    bool ok = algo::DecodeLEU64(buf,tmp);
    memcpy(&result,&tmp,sizeof result);
    return ok;
#else
#error "Unconformant float representation format"
#endif
}

////////////////////////////////////////////////////////////////////////////////
//                           VARIABLE-LENGTH INTEGERS                         //
////////////////////////////////////////////////////////////////////////////////

// continuation bit (bit 7 of each byte), little-endian, u32.
// A group is taken only while the shift stays inside the accumulator's width and
// the group keeps every one of its bits under that shift, so a varint naming a
// value above 0xffffffff, a varint longer than five bytes, and a varint the
// buffer never terminates all leave BUF and RESULT alone.
// Pinned by atf_unit algo_lib.DecodeVLCLERange.
bool algo::DecodeVLCLEU32(algo::memptr &buf, u32 &result) {
    result = 0;
    u32 value(0);
    bool overflow(false);
    int pos(0);
    bool stop(false);
    int shift = 0;
    for (; !stop && !overflow && shift<32 && pos<elems_N(buf); ++pos) {
        u8 c = buf[pos];
        stop = ~c & 0x80;
        u32 bytes = c & 0x7f;
        value |= bytes << shift;
        overflow = bytes != (bytes << shift) >> shift;
        shift += 7;
    }
    bool ret = stop && !overflow;
    if (ret) {
        result = value;
        buf = RestFrom(buf,pos);
    }
    return ret;
}

//------------------------------------------------------------------------------

// continuation bit (bit 7 of each byte), little-endian, u32, signed zigzag
// sign - bit 0 (lsb)
// unsigned mantissa - bits 1..N (msb)
bool algo::DecodeVLCLEI32Z(algo::memptr &buf, i32 &result) {
    result = 0;
    u32 tmp;
    bool ok = DecodeVLCLEU32(buf,tmp);
    if (ok) {
        result = (tmp >> 1) ^ (-(tmp & 1));
    }
    return ok;
}

//------------------------------------------------------------------------------

// continuation bit (bit 7 of each byte), little-endian, u64.
// A group is taken only while the shift stays inside the accumulator's width and
// the group keeps every one of its bits under that shift, so a varint naming a
// value above 0xffffffffffffffff, a varint longer than ten bytes, and a varint
// the buffer never terminates all leave BUF and RESULT alone.  Bounding the
// shift is also what keeps the group's placement defined: an eleventh group
// would be shifted by 70, and a shift at or past the width has no result.
// Pinned by atf_unit algo_lib.DecodeVLCLERange.
bool algo::DecodeVLCLEU64(algo::memptr &buf, u64 &result) {
    result = 0;
    u64 value(0);
    bool overflow(false);
    int pos(0);
    bool stop(false);
    int shift = 0;
    for (; !stop && !overflow && shift<64 && pos<elems_N(buf); ++pos) {
        u8 c = buf[pos];
        stop = ~c & 0x80;
        u64 bytes = c & 0x7f;
        value |= bytes << shift;
        overflow = bytes != (bytes << shift) >> shift;
        shift += 7;
    }
    bool ret = stop && !overflow;
    if (ret) {
        result = value;
        buf = RestFrom(buf,pos);
    }
    return ret;
}

//------------------------------------------------------------------------------

// continuation - bit 7 (msb) of each byte, little-endian, signed zigzag:
// sign - bit 0 (lsb)
// unsigned mantissa - bits 1..N (msb)
bool algo::DecodeVLCLEI64Z(algo::memptr &buf, i64 &result) {
    result = 0;
    u64 tmp;
    bool ok = DecodeVLCLEU64(buf,tmp);
    if (ok) {
        result = (tmp >> 1) ^ (-(tmp & 1));
    }
    return ok;
}

////////////////////////////////////////////////////////////////////////////////
//                                 RAW BYTES                                  //
////////////////////////////////////////////////////////////////////////////////

// N bytes, raw.  N comes from the wire, so it is accepted only in
// [0,bytes remaining in BUF]; anything else leaves BUF and RESULT alone.
// The count is signed 64-bit because every caller's length type -- u16, u32,
// i32, i64 -- converts into it without changing value, so a length no buffer
// can satisfy cannot wrap into the accepted range.
// Pinned by atf_unit algo_lib.DecodeNRange.
bool algo::DecodeNBytes(algo::memptr &buf, i64 n, algo::memptr &result) {
    result = algo::memptr();
    bool ok = n >= 0 && elems_N(buf) >= n;
    if (ok) {
        result = FirstN(buf,n);
        buf = RestFrom(buf,n);
    }
    return ok;
}

////////////////////////////////////////////////////////////////////////////////
//                                 STRINGS                                    //
////////////////////////////////////////////////////////////////////////////////

// N bytes as chars, accepting the same range of N as DecodeNBytes.
bool algo::DecodeNChars(algo::memptr &buf, i64 n, strptr &result) {
    result = strptr();
    bool ok = n >= 0 && elems_N(buf) >= n;
    if (ok) {
        result = ToStrPtr(FirstN(buf,n));
        buf = RestFrom(buf,n);
    }
    return ok;
}

////////////////////////////////////////////////////////////////////////////////

// zero-terminated string
bool algo::DecodeZeroterm(algo::strptr &buf, strptr &result) {
    result = strptr();
    int end = FindChar(buf,'\0');
    bool ok = end>=0;
    if (ok) {
        result = FirstN(buf,end);
        buf = RestFrom(buf,end+1);
    }
    return ok;
}

// zero-terminated string
bool algo::DecodeZeroterm(algo::memptr &buf, strptr &result) {
    strptr tmp = memptr_ToStrptr(buf);
    bool ok = DecodeZeroterm(tmp,result);
    buf = strptr_ToMemptr(tmp);
    return ok;
}

////////////////////////////////////////////////////////////////////////////////
//                                SPECIFIC TYPES                              //
////////////////////////////////////////////////////////////////////////////////

// UUID - raw bytes
bool algo::DecodeUuid(algo::memptr &buf, algo::Uuid &result) {
    constexpr int len = sizeof result;
    algo::memptr tmp;
    bool ok = DecodeNBytes(buf, len, tmp);
    if (ok) {
        memcpy(&result, tmp.elems, len);
    }
    return ok;
}

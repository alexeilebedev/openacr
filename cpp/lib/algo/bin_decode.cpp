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

////////////////////////////////////////////////////////////////////////////////
//                                    FLOAT                                   //
////////////////////////////////////////////////////////////////////////////////

bool algo::DecodeBEF64(algo::memptr &buf, double &result) {
#ifdef __STDC_IEC_559__
    u64 tmp;
    bool ok = algo::DecodeBEU64(buf,tmp);
    char *ptr = (char*)&tmp;
    result = *(double*)ptr;
    return ok;
#else
#error "Unconformant float representation format"
#endif
}

////////////////////////////////////////////////////////////////////////////////
//                           VARIABLE-LENGTH INTEGERS                         //
////////////////////////////////////////////////////////////////////////////////

// continuation bit (bit 7 of each byte), little-endian, u32
bool algo::DecodeVLCLEU32(algo::memptr &buf, u32 &result) {
    result = 0;
    u32 value(0);
    u32 overflow(0);
    int pos(0);
    bool stop(false);
    int shift = 0;
    for (; !stop && !overflow && pos<elems_N(buf); ++pos) {
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

// continuation bit (bit 7 of each byte), little-endian, u64
bool algo::DecodeVLCLEU64(algo::memptr &buf, u64 &result) {
    result = 0;
    u64 value(0);
    u64 overflow(0);
    int pos(0);
    bool stop(false);
    int shift = 0;
    for (; !stop && !overflow && pos<elems_N(buf); ++pos) {
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

// N bytes, raw
bool algo::DecodeNBytes(algo::memptr &buf, int n, algo::memptr &result) {
    result = algo::memptr();
    bool ok = elems_N(buf) >= n;
    if (ok) {
        result = FirstN(buf,n);
        buf = RestFrom(buf,n);
    }
    return ok;
}

////////////////////////////////////////////////////////////////////////////////
//                                 STRINGS                                    //
////////////////////////////////////////////////////////////////////////////////

// N bytes as chars
bool algo::DecodeNChars(algo::memptr &buf, int n, strptr &result) {
    result = strptr();
    bool ok = elems_N(buf) >= n;
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

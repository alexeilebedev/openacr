// Copyright (C) 2020-2021 Astra
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
// Target: algo_lib (lib) -- Support library for all executables
// Exceptions: NO
// Header: include/u128.h
//

#pragma once

#ifdef WIN32
#pragma warning (disable:4324) // structure was padded due to __declspec(align())
__declspec(align(16))
#endif
struct U128 {
    u64 hi,lo;
U128() : hi(0), lo(0) {
}
U128(const U128 &rhs) :hi(rhs.hi), lo(rhs.lo) {
}
U128(u64 rhs) : hi(0),lo(rhs) {
}
U128(u32 rhs) : hi(0),lo(rhs) {
}
U128(i64 rhs) : hi(-(rhs<0)),lo(rhs) {
}
U128(i32 rhs) : hi(-(rhs<0)),lo(rhs) {
}
    U128 &operator =(const U128 &rhs) { lo=rhs.lo; hi=rhs.hi; return *this; }
        U128 &operator =(u64 rhs)         { return *this = U128(rhs); }
            U128 &operator =(u32 rhs)         { return *this = U128(rhs); }
                U128 &operator =(i64 rhs)         { return *this = U128(rhs); }
                    U128 &operator =(i32 rhs)         { return *this = U128(rhs); }

                        bool operator <(const U128 &rhs) const { return hi<rhs.hi || (hi==rhs.hi && lo<rhs.lo); }
    bool operator >(const U128 &rhs) const { return rhs < *this; }

    bool operator ==(const U128 &rhs) const { return hi==rhs.hi && lo==rhs.lo; }
    bool operator ==(u64 rhs) const { return operator ==(U128(rhs)); }
    bool operator ==(u32 rhs) const { return operator ==(U128(rhs)); }
    bool operator ==(int rhs) const { return operator ==(U128(rhs)); }
    // causes operator confusion -- remove
    friend bool operator ==(u64 lhs, const U128 &rhs) { return rhs == lhs; }
    friend bool operator ==(u32 lhs, const U128 &rhs) { return rhs == lhs; }

    bool operator !=(const U128 &rhs) const { return !(*this == rhs); }
    bool operator !=(u64 rhs) const { return !(*this == rhs); }
    bool operator !=(u32 rhs) const { return !(*this == rhs); }
    bool operator !=(int rhs) const { return !(*this == rhs); }
    friend bool operator !=(u64 lhs, const U128 &rhs) { return rhs != lhs; }
    friend bool operator !=(u32 lhs, const U128 &rhs) { return rhs != lhs; }

    U128 operator >>(u64 count) const;
    U128 operator >>(u32 count) const { return operator >>(u64(count)); }
    U128 operator >>(i32 count) const { return operator >>(u64(count)); }
    U128 operator <<(u64 count) const;
    U128 operator <<(u32 count) const { return operator <<(u64(count)); }
    U128 operator <<(i32 count) const { return operator <<(u64(count)); }

    U128 &operator >>=(u32 count) { *this = *this>>count; return *this; }
    U128 &operator <<=(u32 count) { *this = *this<<count; return *this; }

    U128 operator ~() const {
        U128 ret;
        ret.lo = ~lo;
        ret.hi = ~hi;
        return ret;
    }
    U128 operator &(const U128 &rhs) const;
    U128 operator &(u64 val) const { return operator &(U128(val)); }
    U128 operator &(i64 val) const { return operator &(U128(val)); }
    U128 operator &(u32 val) const { return operator &(U128(val)); }
    U128 operator &(i32 val) const { return operator &(U128(val)); }

    U128 operator |(const U128 &rhs) const;
    U128 operator |(i64 val) const { return operator |(U128(val)); }
    U128 operator |(u64 val) const { return operator |(U128(val)); }
    U128 operator |(u32 val) const { return operator |(U128(val)); }
    U128 operator |(i32 val) const { return operator |(U128(val)); }
    U128 &operator &=(const U128 &rhs) {
        lo &= rhs.lo;
        hi &= rhs.hi;
        return *this;
    }
    U128 &operator |=(const U128 &rhs) {
        lo |= rhs.lo;
        hi |= rhs.hi;
        return *this;
    }
    U128 operator -(const U128 &val) const;
    U128 operator -(u64 val) const { return operator -(U128(val)); }
    U128 operator -(u32 val) const { return operator -(U128(val)); }
    U128 operator -(i32 val) const { return operator -(U128(val)); }
    U128 operator +(const U128 &val) const;
    U128 operator +(u64 val) const { return operator +(U128(val)); }
    U128 operator +(u32 val) const { return operator +(U128(val)); }
    U128 operator +(i32 val) const { return operator +(U128(val)); }
    U128 operator *(const U128 &rhs) const;
    U128 operator *(u32 val) const { return operator *(U128(val)); }// delegate
    U128 operator *(u64 val) const { return operator *(U128(val)); }
    U128 operator /(const U128 &val) const;
    U128 operator /(u32 val) const { return operator /(U128(val)); }// delegate
    U128 operator /(u64 val) const { return operator /(U128(val)); }
    U128 operator %(const U128 &val) const;
    u32 operator %(u32 val) const { return operator %(U128(val)); }// delegate
    u64 operator %(u64 val) const { return operator %(U128(val)); }
    // unary negation
    U128 operator -() const { return ~*this + U128(u32(1)); }
    // extract / clip -- explicit
    operator u64() const {return lo;}
    operator u32() const {return lo;}
    operator i32() const {return lo;}
    // convert to bool -- no loss
    operator bool() const {return (hi!=0 || lo!=0);}
};

struct I128 {
    u64 hi,lo;
    // no initialization
};

#ifdef WIN32
typedef U128 u128;
typedef I128 i128;
#endif

// Copyright (C) 2020-2021 Astra
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

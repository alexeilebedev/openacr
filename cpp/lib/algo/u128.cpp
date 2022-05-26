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
// Target: algo_lib (lib) -- Support library for all executables
// Exceptions: NO
// Source: cpp/lib/algo/u128.cpp
//

#include "include/algo.h"

U128 U128::operator >>(u64 count) const {
    U128 ret(*this);
    // clip shift amount
    count = count > 128 ? 128 : count;
    while (count>0) {
        u32 shift1 = count > 63 ? 63 : count;
        ret.lo = (ret.lo >> shift1) | (ret.hi << (64-shift1));
        ret.hi = ret.hi >> shift1;
        count -= shift1;
    }
    return ret;
}

U128 U128::operator <<(u64 count) const {
    U128 ret(*this);
    count = count > 128 ? 128 : count;
    while (count>0) {
        u32 shift1 = count > 63 ? 63 : count;
        ret.hi = (ret.hi << shift1) | (ret.lo >> (64-shift1));
        ret.lo = ret.lo << shift1;
        count -= shift1;
    }
    return ret;
}

U128 U128::operator &(const U128 &rhs) const {
    U128 ret;
    ret.lo = lo & rhs.lo;
    ret.hi = hi & rhs.hi;
    return ret;
}

U128 U128::operator |(const U128 &rhs) const {
    U128 ret;
    ret.lo = lo | rhs.lo;
    ret.hi = hi | rhs.hi;
    return ret;
}

U128 U128::operator -(const U128 &val) const {
    U128 ret;
    ret.lo = lo - val.lo;
    ret.hi = hi - val.hi - (val.lo > lo);
    return ret;
}

U128 U128::operator +(const U128 &val) const {
    U128 ret;
    ret.lo = lo + val.lo;
    ret.hi = hi + val.hi + (ret.lo < lo);
    return ret;
}

static void Divmod(const U128 &start, const U128 &val, U128 &quot, U128 &rem) {
    rem=start;
    quot=U128(0);
    if (val) {
        // convert multiplication into bitwise additions
        for (i32 i=127; i >= 0 && bool(rem); i--) {
            // if the result of this multiplication fits the value,
            // then the bit is present in the remainder
            // but if the multiplication u128, then not
            U128 test = val<<u32(i);
            if (!(test > rem) && ((test >> u32(i)) == val)) {
                rem = rem - test;
                quot = quot + (U128(1)<<u32(i));
            }
        }
    }
}

U128 U128::operator %(const U128 &val) const {
    U128 quot,rem;
    Divmod(*this,val,quot,rem);
    return rem;
}

U128 U128::operator /(const U128 &val) const {
    U128 quot,rem;
    Divmod(*this,val,quot,rem);
    return quot;
}

U128 U128::operator *(const U128 &rhs) const {
    U128 ret(0);
    // convert multiplication into bitwise additions
    for (u32 i=0; i < 128; i++) {
        if (bool((rhs >> i) & U128(1))) {
            ret = ret + (*this << i);
        }
    }
    return ret;
}

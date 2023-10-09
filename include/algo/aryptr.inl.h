// Copyright (C) 2017-2019 NYSE | Intercontinental Exchange
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
// Contacting ICE: <https://www.theice.com/contact>
// Target: algo_lib (lib) -- Support library for all executables
// Exceptions: NO
// Header: include/algo/aryptr.inl.h
//
// -----------------------------------------------------------------------------

#pragma once

template<class T> inline int algo::elems_N(const aryptr<T> &ary) {
    return ary.n_elems;
}

template<class T> inline int algo::ch_N(const aryptr<T> &ary) {
    return ary.n_elems;
}

template<class T> inline void algo::Fill(const aryptr<T> &lhs, const T &t) {
    frep_(i,ch_N(lhs)) {
        lhs[i]=t;
    }
}

template<class T> inline algo::aryptr<T>::aryptr() {
    elems = 0;
    n_elems = 0;
}

template<class T> inline T &algo::aryptr<T>::operator [](u32 idx) const {
    return elems[idx];
}

template<class T, class U> inline int algo::Find(const algo::aryptr<T> &lhs, const U&t) {
    frep_ (i, lhs.n_elems) {
        if (lhs.elems[i]==t) {
            return i;
        }
    }
    return -1;
}

template<class T> inline algo::aryptr<T> algo::FirstN(const algo::aryptr<T> &lhs, u32 n) {
    u32 lim = lhs.n_elems;
    if (n > lim) {
        n = lim;
    }
    return algo::aryptr<T>(lhs.elems, n);
}

template<class T> inline algo::aryptr<T> algo::LastN(const algo::aryptr<T> &lhs, u32 n) {
    u32 lim = lhs.n_elems;
    if (n > lim) {
        n = lim;
    }
    return algo::aryptr<T>(lhs.elems + lim - n, n);
}

template<class T> inline algo::aryptr<T> algo::RestFrom(const algo::aryptr<T> &lhs, u32 n) {
    u32 lim = lhs.n_elems;
    if (n > lim) {
        n = lim;
    }
    return algo::aryptr<T>(lhs.elems + n, lim - n);
}

template<class T> inline algo::aryptr<T> algo::qGetRegion(const algo::aryptr<T> &lhs, u32 lo, u32 n) {
    return algo::aryptr<T>(lhs.elems + lo, n);
}

template<class T> inline algo::aryptr<T> algo::GetRegion(const algo::aryptr<T> &lhs, u32 lo, u32 n) {
    u32 max = lhs.n_elems;
    lo = u32_Min(lo, max);
    n  = u32_Min(n,  max - lo);
    return algo::aryptr<T>(lhs.elems + lo, n);
}

template<class T> inline algo::aryptr<u8> algo::BytesOf(const T &t) {
    return algo::aryptr<u8>((u8*)&t,sizeof(t));
}

template<class T> inline algo::aryptr<T>::aryptr(const T *e, i32 in_n) : elems(const_cast<T*>(e)) , n_elems(in_n) {
}

template<class T> inline algo::aryptr<T>::aryptr(const char *e) : elems(const_cast<T*>(e)) , n_elems(algo::ImpliedLength((T*)NULL,e)) {
}

template<class T> inline T &algo::qLast(const algo::aryptr<T> &ary) {
    return ary.elems[ary.n_elems-1];
}

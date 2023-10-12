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
// Source: cpp/lib/algo/base64.cpp -- base64 encoding
//

#include "include/algo.h"

namespace {

    // convert value to letter from base64 alphabet
    inline char ToAlpha64(unsigned v) {
        v &= 0x3f;
        return v < 26
            ? v + 'A'
            : v < 26+26
            ? v-26+'a'
            : v < 26+26+10
            ? v-26-26+'0'
            : v == 26+26+10
            ? '+'
            : '/';
    }

} // anon namespace

// print base64-encoded string
void algo::strptr_PrintBase64(strptr str, cstring &out) {
    ch_Reserve(out,(ch_N(str)+2)/3*4);
    char *ip = str.elems;
    char *op = out.ch_elems + out.ch_n;
    unsigned len(str.n_elems);
    while (len) {
        unsigned quantum(0);
        unsigned quantum_len(0);
        for (int shift=16; shift>=0 && len; shift -= 8, --len, ++quantum_len) {
            quantum |= (ip++[0] & 0xff) << shift;
        }
        op++[0] = ToAlpha64(quantum >> 18);
        op++[0] = ToAlpha64(quantum >> 12);
        op++[0] = quantum_len >=2 ? ToAlpha64(quantum >> 6) : '=';
        op++[0] = quantum_len >=3 ? ToAlpha64(quantum)      : '=';
    }
    out.ch_n = op - out.ch_elems;
}

// Copyright (C) 2023-2026 AlgoRND
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

// The base64 alphabet, indexed by the 6-bit group it stands for.
static const char alpha64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// print base64-encoded string
void algo::strptr_PrintBase64(strptr str, cstring &out) {
    ch_Reserve(out,(ch_N(str)+2)/3*4);
    const u8 *ip = (const u8*)str.elems;
    char *op = out.ch_elems + out.ch_n;
    i64 nwhole = ch_N(str) / 3;
    for (i64 i = 0; i < nwhole; i++) {
        u32 quantum = (u32(ip[0])<<16) | (u32(ip[1])<<8) | u32(ip[2]);
        ip += 3;
        op[0] = alpha64[(quantum>>18) & 0x3f];
        op[1] = alpha64[(quantum>>12) & 0x3f];
        op[2] = alpha64[(quantum>>6) & 0x3f];
        op[3] = alpha64[quantum & 0x3f];
        op += 4;
    }
    i64 ntail = ch_N(str) - nwhole*3;
    if (ntail > 0) {
        u32 quantum = u32(ip[0])<<16;
        if (ntail == 2) {
            quantum |= u32(ip[1])<<8;
        }
        op[0] = alpha64[(quantum>>18) & 0x3f];
        op[1] = alpha64[(quantum>>12) & 0x3f];
        op[2] = ntail == 2 ? alpha64[(quantum>>6) & 0x3f] : '=';
        op[3] = '=';
        op += 4;
    }
    out.ch_n = op - out.ch_elems;
}

// Append the bytes STR encodes to OUT.  False when STR holds a character
// outside the alphabet, which is skipped like the padding is, so OUT still
// carries what the rest of the string decoded to.
bool algo::strptr_ReadBase64(strptr str, cstring &out) {
    bool ret = true;
    u32 quantum = 0;
    int nbit = 0;
    ch_Reserve(out, ch_N(str)/4*3 + 3);
    for (i64 i = 0; i < ch_N(str); i++) {
        char c = str[i];
        if (c != '=') {
            const char *pos = c == 0 ? NULL : strchr(alpha64, c);
            if (!pos) {
                ret = false;
            } else {
                quantum = (quantum<<6) | u32(pos - alpha64);
                nbit += 6;
                if (nbit >= 8) {
                    nbit -= 8;
                    out << char((quantum >> nbit) & 0xff);
                }
            }
        }
    }
    return ret;
}

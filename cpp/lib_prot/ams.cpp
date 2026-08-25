// Copyright (C) 2026 AlgoRND
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
// Target: lib_prot (lib) -- Library covering all protocols
// Exceptions: NO
// Source: cpp/lib_prot/ams.cpp
//

#include "include/algo.h"
#include "include/gen/ams_gen.h"
#include "include/gen/ws_gen.h"
#include "include/gen/ws_gen.inl.h"

u32 ws::payload_N(const ws::Frame& parent) {
    return payload_len_Get(parent);
}

u32 ws::payload_N(const ws::FrameMasked& parent) {
    return payload_len_Get(parent);
}

// I64Price8: fixed-point price with 8 decimal places
void ams::I64Price8_Print(ams::I64Price8 row, algo::cstring &str) {
    i64 whole = row.value / 100000000LL;
    i64 frac  = labs(row.value % 100000000LL);
    str << whole;
    if (frac) {
        str << ".";
        // print up to 8 digits, trimming trailing zeros
        tempstr fracstr;
        i64_PrintPadLeft(frac, fracstr, 8);
        int end = ch_N(fracstr);
        while (end > 0 && ch_qFind(fracstr, end-1) == '0') {
            --end;
        }
        str << ch_FirstN(fracstr, end);
    }
}

bool ams::I64Price8_ReadStrptrMaybe(ams::I64Price8 &row, algo::strptr str) {
    double dval = 0;
    bool ok = double_ReadStrptrMaybe(dval, str);
    if (ok) {
        row.value = i64(dval * 100000000.0 + (dval >= 0 ? 0.5 : -0.5));
    }
    return ok;
}

// SampMengSymbol extern functions are not needed (Raw printfmt generates them)

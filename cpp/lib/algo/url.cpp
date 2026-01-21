// Copyright (C) 2025-2026 AlgoRND
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
// Source: cpp/lib/algo/url.cpp
//

#include "include/algo.h"

// perform standard url percent decoding (for raw url, path),
// and optionally translate '+' to space (for application/x-www-form-urlencoded)
tempstr algo::UrlDecode(strptr in, bool plus) {
    tempstr out;
    ch_Reserve(out,ch_N(in));
    u8 hi,lo;
    frep_(i,ch_N(in)) {
        if (in[i]=='%' && i+2<ch_N(in) && ParseHex1(in[i+1],hi) && ParseHex1(in[i+2],lo)) {
            out << char((hi<<4)|lo);
            i+=2;
        } else if (plus && in[i]=='+') {
            out << ' ';
        } else {
            out << in[i];
        }
    }
    return out;
}

static strptr ParseSep(strptr &buf, char sep) {
    int ind = FindChar(buf,sep);
    ind = ind >=0 ? ind : ch_N(buf);
    strptr ret = ch_FirstN(buf,ind);
    buf = ch_RestFrom(buf,ind+1);
    return ret;
}

// Parses HTTP URL
// Place path part as value of first attr,
// other attrs are key-value pairs in order as the appear
void algo::ParseUrl(algo::Tuple &out, strptr in) {
    attrs_RemoveAll(out);
    strptr path = UrlDecode(ParseSep(in,'?'),false);
    if (ch_N(path)) {
        attrs_Alloc(out).value = path;
        while (ch_N(in)) {
            strptr arg = ParseSep(in,'&');
            algo::Attr &attr = attrs_Alloc(out);
            attr.name = UrlDecode(ParseSep(arg,'='),true);
            attr.value = UrlDecode(arg,true);
        }
    }
}

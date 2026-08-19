// Copyright (C) 2023-2024,2026 AlgoRND
// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
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
// Target: atf_amc (exe) -- Unit tests for amc (see amctest table)
// Exceptions: yes
// Source: cpp/atf_amc/inlary.cpp
//

#include "include/atf_amc.h"

void atf_amc::amctest_Inlary_ReadPrint() {
    atf_amc::InlaryPrint p;
    p.fixary_elems[0]=3;
    p.fixary_elems[1]=4;

    inlary_Alloc(p)=5;
    inlary_Alloc(p)=6;
    inlary_Alloc(p)=7;

    tempstr out;
    atf_amc::InlaryPrint_Print(p,out);
    prlog(out);
    vrfy_(out == "atf_amc.InlaryPrint  fixary:\"3 4 0\"  inlary:\"5 6 7\"");

    // read it back -- must match
    atf_amc::InlaryPrint q;
    atf_amc::InlaryPrint_ReadStrptrMaybe(q,out);
    tempstr out2;
    atf_amc::InlaryPrint_Print(q,out2);
    prlog(out2);
    vrfy_(out == out2);

    // memcmp will not succeed because unallocated portions
    // of inlary are uninitialized
}

// --------------------------------------------------------------------------------

// Assert that COPY carries the three elements 5,6,7 of the copied source
static void CheckInlaryCopy(atf_amc::InlaryPrint &copy) {
    vrfy_(inlary_N(copy) == 3);
    vrfy_(inlary_qFind(copy,0) == 5);
    vrfy_(inlary_qFind(copy,1) == 6);
    vrfy_(inlary_qFind(copy,2) == 7);
}

// Copy of a variable inlary preserves the element count:
// copy constructor and operator= both route through inlary_Setary
void atf_amc::amctest_InlaryCopyCount() {
    atf_amc::InlaryPrint p;
    inlary_Alloc(p)=5;
    inlary_Alloc(p)=6;
    inlary_Alloc(p)=7;

    atf_amc::InlaryPrint q(p);
    CheckInlaryCopy(q);

    atf_amc::InlaryPrint r;
    r = p;
    CheckInlaryCopy(r);
}

// --------------------------------------------------------------------------------

// A variable inlary with min:2 preallocates two elements at Init.
// Reading a separated string replaces the contents, never drops the count
// below min, and value-initializes the slots the input does not cover,
// so the result is a function of the input alone
void atf_amc::amctest_InlaryMinRead() {
    atf_amc::InlaryMin m;
    vrfy_(ary_N(m) == 2);
    // more tokens than min: count matches the input
    vrfy_(ary_ReadStrptrMaybe(m, "5 6 7"));
    vrfy_(ary_N(m) == 3);
    vrfy_(ary_qFind(m,0) == 5);
    vrfy_(ary_qFind(m,1) == 6);
    vrfy_(ary_qFind(m,2) == 7);
    // fewer tokens than min: the count stays at min, and the slot the input
    // does not cover is value-initialized, not left over from the prior read
    vrfy_(ary_ReadStrptrMaybe(m, "9"));
    vrfy_(ary_N(m) == 2);
    vrfy_(ary_qFind(m,0) == 9);
    vrfy_(ary_qFind(m,1) == 0);
    // read-print round trip: the printed record carries no element fabricated
    // from the record's prior contents
    tempstr out;
    ary_Print(m, out);
    vrfy_(out == "9 0");
    // Setary keeps the same floor and value-initializes the slot the
    // source does not cover, instead of retaining the prior element
    vrfy_(ary_ReadStrptrMaybe(m, "5 6 7"));
    u32 one[1] = {4};
    ary_Setary(m, algo::aryptr<u32>(one,1));
    vrfy_(ary_N(m) == 2);
    vrfy_(ary_qFind(m,0) == 4);
    vrfy_(ary_qFind(m,1) == 0);
}

// --------------------------------------------------------------------------------

// ReadStrptrMaybe into a variable-length char inlary on a global ctype:
// the string is copied and the length updated on _db
void atf_amc::amctest_InlaryCharReadGlobal() {
    vrfy_(atf_amc::charbuf_ReadStrptrMaybe("hello"));
    vrfy_(atf_amc::charbuf_Getary() == "hello");
    // overlong input is silently truncated to charbuf_max
    vrfy_(atf_amc::charbuf_ReadStrptrMaybe("0123456789abcdef"));
    vrfy_(atf_amc::charbuf_Getary() == "0123456789");
    vrfy_(atf_amc::charbuf_ReadStrptrMaybe(""));
    vrfy_(atf_amc::charbuf_Getary() == "");
}

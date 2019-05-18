// (C) 2018-2019 NYSE | Intercontinental Exchange
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
// Contacting ICE: <https://www.theice.com/contact>
//
// Target: atf_amc (exe) -- Unit tests for amc (see amctest table)
// Exceptions: NO
// Source: cpp/atf/amc/inlary.cpp
//
// Created By: alexei.lebedev
// Recent Changes: alexei.lebedev
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

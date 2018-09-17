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
// Target: atf_amc (exe)
// Exceptions: NO
// Source: cpp/atf/amc/readstr.cpp
//
// Created By: alexei.lebedev
// Recent Changes: alexei.lebedev
//

#include "include/atf_amc.h"

// -----------------------------------------------------------------------------

void atf_amc::amctest_ReadTuple1() {
    tempstr out;
    atf_amc::Ctype1Attr out_ctype;
    out_ctype.attr1 = 33;
    Ctype1Attr_Print(out_ctype, out);
    vrfyeq_(out, "33");

    // Check that it can be read back
    atf_amc::Ctype1Attr in_ctype;
    Ctype1Attr_ReadStrptrMaybe(in_ctype, out);
    vrfyeq_(in_ctype.attr1, out_ctype.attr1);
}

// -----------------------------------------------------------------------------

void atf_amc::amctest_ReadTuple2() {
    // Note the silent switch from single-value to tuple format.
    tempstr out;
    atf_amc::Ctype2Attr out_ctype;
    out_ctype.attr1 = 33;
    out_ctype.attr2 = 44;
    Ctype2Attr_Print(out_ctype, out);
    vrfyeq_(out, "atf_amc.Ctype2Attr  attr1:33  attr2:44");
    // Check that ReadStrptr works
    atf_amc::Ctype2Attr in_ctype;
    Ctype2Attr_ReadStrptrMaybe(in_ctype, out);
    vrfyeq_(in_ctype, out_ctype);
    // Check that ReadTuple works the same
    algo::Tuple tuple;
    Refurbish(in_ctype);
    Tuple_ReadStrptr(tuple, out, true /*attrs only*/);
    Ctype2Attr_ReadTupleMaybe(in_ctype, tuple);
    vrfyeq_(in_ctype, out_ctype);
}

// -----------------------------------------------------------------------------

void atf_amc::amctest_ReadTuple3() {
    int i=0;
    ind_beg(algo::Attr_curs,curs,"abcd e:f h:g #ddddd") {
        vrfyeq_(!(i==0) || (curs.name == "" && curs.value == "abcd"), true);
        vrfyeq_(!(i==1) || (curs.name == "e" && curs.value == "f"), true);
        vrfyeq_(!(i==2) || (curs.name == "h" && curs.value == "g"), true);
        vrfyeq_(!(i==3) || (false), true);// will exit by then
        i++;
    }ind_end;
}

// -----------------------------------------------------------------------------

void atf_amc::amctest_ReadTuple4() {
    ind_beg(algo::Attr_curs,curs,"") {
        (void)curs;
        vrfyeq_(false,true);//shouldn't be here
    }ind_end;
}

// -----------------------------------------------------------------------------

void atf_amc::amctest_ReadTuple5() {
    int i=0;
    ind_beg(algo::Attr_curs,curs,"'a':'b'") {
        vrfyeq_(!(i==0) || (curs.name == "a" && curs.value == "b"), true);
        i++;
    }ind_end;
}

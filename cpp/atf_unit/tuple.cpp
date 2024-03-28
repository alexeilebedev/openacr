// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
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
// Target: atf_unit (exe) -- Unit tests (see unittest table)
// Exceptions: yes
// Source: cpp/atf_unit/tuple.cpp
//

#include "include/atf_unit.h"

// -----------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_Tuple1() {
    cstring temp;
    temp << MaybeSpace;
    PrintAttr(temp, "abcd", "ef");
    vrfy_(temp == "abcd:ef");

    ch_RemoveAll(temp);
    temp << MaybeSpace; PrintAttr(temp, "", "ef");
    vrfy_(temp == "ef");
    ch_RemoveAll(temp);
    temp << MaybeSpace; PrintAttr(temp, "abcd", "ef gf");
    vrfy_(temp == "abcd:\"ef gf\"");
    ch_RemoveAll(temp);
    temp << MaybeSpace; PrintAttr(temp, "abcd", "\"ef");
    vrfy_(temp == "abcd:'\"ef'");
}

// -----------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_Tuple2() {
    Tuple cmdline;

    tempstr out;
    Tuple_ReadStrptr(cmdline, "b:c  a  b", false);
    ch_RemoveAll(out);
    out<< cmdline;
    vrfy_(out == "b:c  a  b");

    vrfy(Tuple_ReadStrptrMaybe(cmdline, "c  a  b:d"), algo_lib::_db.errtext);
    ch_RemoveAll(out);
    out<< cmdline;
    vrfy_(out == "c  a  b:d");
}

// -----------------------------------------------------------------------------

static void ScanAttrs() {
    strptr text="key1:abcde  key2:\"asdfasdfasdf asdfasdfasdf\"  key3:\"\"";
    ind_beg(algo::Attr_curs,attr,text) {
        if (attr.name=="key1") {
            vrfyeq_(attr.value, "abcde");
        } else if (attr.name == "key2") {
            vrfyeq_(attr.value, "asdfasdfasdf asdfasdfasdf");
        } else if (attr.name == "key3") {
            vrfyeq_(attr.value, "");
        }
    }ind_end;
}

// --------------------------------------------------------------------------------

// Check Attr_curs
void atf_unit::unittest_algo_lib_Tuple() {
    ScanAttrs();

    // Check TupleSubst
    algo_lib::Replscope R;
    Set(R,"$S","abc");
    Set(R,"$T","A");
    vrfyeq_(algo_lib::Tuple_Subst(R,"a:b  $S:c  e:\"fff$Tzzz\""), "a:b  abc:c  e:fffAzzz");
}

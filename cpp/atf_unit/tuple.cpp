// Copyright (C) 2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
//
// License: Apache
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
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

// --------------------------------------------------------------------------------

// An unterminated quote fails the parse regardless of which side of a colon
// it falls on: bare-token, name, and value positions all reject the line,
// and a properly closed quote in any position stays accepted.
void atf_unit::unittest_algo_lib_TupleBadQuote() {
    Tuple tuple;
    vrfy_(!Tuple_ReadStrptrMaybe(tuple, "gendb.dispsig  dispsig:b.Y  signature:\"unterminated"));
    vrfy_(!Tuple_ReadStrptrMaybe(tuple, "\"unterminated  a:b"));
    vrfy_(!Tuple_ReadStrptrMaybe(tuple, "head  \"unterminated:value"));
    vrfy_(Tuple_ReadStrptrMaybe(tuple, "head  \"a b\":c"));
    vrfy_(Tuple_ReadStrptrMaybe(tuple, "head  a:\"b c\""));
}

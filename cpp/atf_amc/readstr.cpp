// Copyright (C) 2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2021 Astra
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
// Target: atf_amc (exe) -- Unit tests for amc (see amctest table)
// Exceptions: yes
// Source: cpp/atf_amc/readstr.cpp
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
    vrfy_(Ctype2Attr_ReadStrptrMaybe(in_ctype, out));
    vrfyeq_(in_ctype, out_ctype);
    // Check that ReadTuple works the same
    algo::Tuple tuple;
    algo::Refurbish(in_ctype);
    Tuple_ReadStrptr(tuple, out, false);
    vrfy_(Ctype2Attr_ReadTupleMaybe(in_ctype, tuple));
    vrfyeq_(in_ctype, out_ctype);
}

// -----------------------------------------------------------------------------

// An attribute naming no field is extra information: the read succeeds and the
// fields the tuple does name land.  With algo_lib::_db.strict_attr set the
// attribute is an error naming itself, and acr.rowid is never one.  A positional
// past the last anonymous field is refused under either setting.
void atf_amc::amctest_ReadTupleUnknownAttr() {
    atf_amc::Ctype2AttrAnon anon;
    vrfy_(Ctype2AttrAnon_ReadStrptrMaybe(anon, "atf_amc.Ctype2AttrAnon  1  2"));
    vrfy_(!Ctype2AttrAnon_ReadStrptrMaybe(anon, "atf_amc.Ctype2AttrAnon  1  2  3"));
    atf_amc::Ctype2Attr in_ctype;
    vrfy_(Ctype2Attr_ReadStrptrMaybe(in_ctype, "atf_amc.Ctype2Attr  attr1:33  zzz:1  attr2:44"));
    vrfyeq_(in_ctype.attr1, 33u);
    vrfyeq_(in_ctype.attr2, 44u);
    algo_lib::_db.strict_attr = true;
    bool ok = Ctype2Attr_ReadStrptrMaybe(in_ctype, "atf_amc.Ctype2Attr  attr1:1  zzz:1");
    tempstr err = algo_lib::DetachBadTags();
    bool ok_rowid = Ctype2Attr_ReadStrptrMaybe(in_ctype, "atf_amc.Ctype2Attr  attr1:2  acr.rowid:7");
    algo_lib::_db.strict_attr = false;
    vrfy_(!ok);
    vrfy_(algo::FindStr(err, "unrecognized attr") != -1);
    vrfy_(algo::FindStr(err, "attr:zzz") != -1);
    vrfy_(ok_rowid);
    vrfyeq_(in_ctype.attr1, 2u);
}

// -----------------------------------------------------------------------------

void atf_amc::amctest_ReadTuple2a() {
    tempstr out;
    atf_amc::Ctype2AttrAnon out_ctype;
    out_ctype.attr1 = 55;
    out_ctype.attr2 = 66;
    Ctype2AttrAnon_Print(out_ctype, out);
    vrfyeq_(out, "atf_amc.Ctype2AttrAnon  attr1:55  attr2:66");
    // Check that ReadStrptr works with named attrs
    {
        atf_amc::Ctype2AttrAnon in_ctype;
        Ctype2AttrAnon_ReadStrptrMaybe(in_ctype, out);
        vrfyeq_(in_ctype.attr1,55);
        vrfyeq_(in_ctype.attr2,66);
    }
    // Check that ReadStrptr works with anon attrs
    {
        atf_amc::Ctype2AttrAnon in_ctype;
        Ctype2AttrAnon_ReadStrptrMaybe(in_ctype, "atf_amc.Ctype2AttrAnon 77 88");
        vrfyeq_(in_ctype.attr1,77);
        vrfyeq_(in_ctype.attr2,88);
    }
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

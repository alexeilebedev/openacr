// Copyright (C) 2023-2024 AlgoRND
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
// Exceptions: NO
// Source: cpp/atf_amc/dispatch.cpp
//

#include "include/atf_amc.h"

// -----------------------------------------------------------------------------

void atf_amc::amctest_DispRead1() {
    // nothing interesting
}

// -----------------------------------------------------------------------------

void atf_amc::amctest_DispRead2() {
    strptr msg = "atf_amc.VarlenK  i.0:0  i.1:1  i.2:2";
    algo::ByteAry byteary;
    ary_Reserve(byteary, 1024);
    atf_amc::Msgs_ReadStrptrMaybe(msg, byteary);// must return switch, or else type information is lost
    atf_amc::VarlenK *varlenk = (atf_amc::VarlenK*)byteary.ary_elems;
    vrfyeq_(varlenk && (i32)varlenk->length == ary_N(byteary), true);
    if (i_N(*varlenk) == 3) {
        vrfyeq_((i32)i_Getary(*varlenk)[0], 0);
        vrfyeq_((i32)i_Getary(*varlenk)[1], 1);
        vrfyeq_((i32)i_Getary(*varlenk)[2], 2);
    }
    // print the message back!
    tempstr out;
    VarlenK_Print(*varlenk, out);
    vrfyeq_(out, "atf_amc.VarlenK  i.0:0  i.1:1  i.2:2");
}

// -----------------------------------------------------------------------------

void atf_amc::amctest_DispRead3() {
}

// -----------------------------------------------------------------------------

void atf_amc::amctest_DispRead4() {
    strptr msg = "atf_amc.Seqmsg  payload:'atf_amc.Text  text:\"blahblah\"'";
    algo::ByteAry byteary;
    ary_Reserve(byteary, 1024);
    atf_amc::MsgHeaderMsgs_ReadStrptrMaybe(msg, byteary);
    atf_amc::Seqmsg *seqmsg = (atf_amc::Seqmsg*)byteary.ary_elems;
    vrfyeq_(seqmsg->type, atf_amc_MsgType_atf_amc_Seqmsg);

    atf_amc::MsgHeader *payload = payload_Get(*seqmsg);
    vrfyeq_(payload!=NULL, true);
    if (payload) {
        atf_amc::Text *text = atf_amc::Text_Castdown(*payload);
        vrfyeq_(text!=NULL, true);
        if (text) {
            vrfyeq_(text_Getary(*text), "blahblah");
        }
    }
}

// -----------------------------------------------------------------------------

// Check that dispatch read supports both lowercase and uppercase versions
void atf_amc::amctest_DispReadSsimfile() {
    algo::ByteAry byteary;
    // Detect lowercase version
    {
        ary_RemoveAll(byteary);
        atf_amc::SsimfilesCase result = atf_amc::Ssimfiles_ReadStrptr("dmmeta.ctype ctype:a.b",byteary);
        dmmeta::Ctype &ctype=(dmmeta::Ctype&)byteary.ary_elems[0];
        vrfy_(result== atf_amc_SsimfilesCase_dmmeta_Ctype);
        vrfy_(ctype.ctype == "a.b");
    }
    // Uppercase version
    {
        ary_RemoveAll(byteary);
        atf_amc::SsimfilesCase result = atf_amc::Ssimfiles_ReadStrptr("dmmeta.Ctype ctype:c.d",byteary);
        dmmeta::Ctype &ctype=(dmmeta::Ctype&)byteary.ary_elems[0];
        vrfy_(result== atf_amc_SsimfilesCase_dmmeta_Ctype);
        vrfy_(ctype.ctype == "c.d");
    }
    // unknown type
    {
        ary_RemoveAll(byteary);
        atf_amc::SsimfilesCase result = atf_amc::Ssimfiles_ReadStrptr("something else",byteary);
        vrfy_(result.value == 0);
    }
}

// -----------------------------------------------------------------------------

void atf_amc::amctest_TestDispFilter() {
    atf_amc::DispFilter filter;
    Tuple tuple;
    (void)Tuple_ReadStrptrMaybe(tuple, "atf_amc.DispFilter  strval:abcd  start_intval:44 end_intval:100");
    DispFilter_ReadTupleMaybe(filter, tuple);
    atf_amc::DispType1 type1;
    type1.strval = "abcd";
    vrfyeq_(atf_amc::DispFilter_MatchDispType1(filter, type1), true);
    atf_amc::DispType2 type2;
    type2.intval = 44;
    vrfyeq_(atf_amc::DispFilter_MatchDispType2(filter, type2), true);
    type2.intval = 100;
    vrfyeq_(atf_amc::DispFilter_MatchDispType2(filter, type2), true);
    type2.intval = 43;
    vrfyeq_(atf_amc::DispFilter_MatchDispType2(filter, type2), false);
    type2.intval = 101;
    vrfyeq_(atf_amc::DispFilter_MatchDispType2(filter, type2), false);
    atf_amc::DispType3 type3;
    type3.intval = 55;
    type3.strval="abcd";
    vrfyeq_(atf_amc::DispFilter_MatchDispType3(filter, type3), true);
}

// -----------------------------------------------------------------------------

void atf_amc::amctest_TestDispFilter2() {
    atf_amc::DispFilter filter;
    Tuple tuple;
    vrfy(Tuple_ReadStrptrMaybe(tuple, "atf_amc.DispFilter  strval_regx:a%d"), algo_lib::_db.errtext);
    DispFilter_ReadTupleMaybe(filter, tuple);
    atf_amc::DispType1 type1;
    type1.strval = "abcd";
    vrfyeq_(atf_amc::DispFilter_MatchDispType1(filter, type1), true);
    type1.strval = "acbe";
    vrfyeq_(atf_amc::DispFilter_MatchDispType1(filter, type1), false);
    type1.strval = "adfffffffffffffffffd";
    vrfyeq_(atf_amc::DispFilter_MatchDispType1(filter, type1), true);
}

// -----------------------------------------------------------------------------

void atf_amc::amctest_TestDispFilter3() {
    atf_amc::DispFilter filter;
    Tuple tuple;
    vrfy(Tuple_ReadStrptrMaybe(tuple, "atf_amc.DispFilter  intval:44"), algo_lib::_db.errtext);
    DispFilter_ReadTupleMaybe(filter, tuple);
    atf_amc::DispType2 type2;
    type2.intval = 44;
    vrfyeq_(atf_amc::DispFilter_MatchDispType2(filter, type2), true);
    type2.intval = 45;
    vrfyeq_(atf_amc::DispFilter_MatchDispType2(filter, type2), false);
}

// -----------------------------------------------------------------------------

void atf_amc::amctest_TestDispFilter4() {
    atf_amc::DispFilter filter;
    Tuple tuple;
    (void)Tuple_ReadStrptrMaybe(tuple, "atf_amc.DispFilter"
                                "  start_dateval:'2015-01-01T01:02:03.456789012'"
                                "  end_dateval:'2015-02-03T04:05:06.789012345'");
    DispFilter_ReadTupleMaybe(filter, tuple);

    atf_amc::DispType1 type1;
    vrfy(UnTime_ReadStrptrMaybe(type1.dateval, "2014-01-01T00:00:00"), algo_lib::_db.errtext);
    vrfyeq_(atf_amc::DispFilter_MatchDispType1(filter, type1), false);// too early
    vrfy(UnTime_ReadStrptrMaybe(type1.dateval, "2015-01-01T01:02:03.456789011"), algo_lib::_db.errtext);
    vrfyeq_(atf_amc::DispFilter_MatchDispType1(filter, type1), false);// one nanosecond too early
    vrfy(UnTime_ReadStrptrMaybe(type1.dateval, "2015-02-03T04:05:06.789012346"), algo_lib::_db.errtext);
    vrfyeq_(atf_amc::DispFilter_MatchDispType1(filter, type1), false);// one nanosecond too late
    vrfy(UnTime_ReadStrptrMaybe(type1.dateval, "2015-01-01T01:02:03.456789012"), algo_lib::_db.errtext);
    vrfyeq_(atf_amc::DispFilter_MatchDispType1(filter, type1), true);// earliest valid time
    vrfy(UnTime_ReadStrptrMaybe(type1.dateval, "2015-02-03T04:05:06.789012345"), algo_lib::_db.errtext);
    vrfyeq_(atf_amc::DispFilter_MatchDispType1(filter, type1), true);// latest valid time
    vrfy(UnTime_ReadStrptrMaybe(type1.dateval, "2015-01-10T00:00:00"), algo_lib::_db.errtext);
    vrfyeq_(atf_amc::DispFilter_MatchDispType1(filter, type1), true);// some time in the middle
}

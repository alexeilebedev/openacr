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
// Source: cpp/atf/amc/numstr.cpp
//
// Created By: alexei.lebedev
// Recent Changes: alexei.lebedev
//

#include "include/atf_amc.h"

void atf_amc::amctest_Numstr() {
    // '0' pad
    {
        algo::LnumStr6_U32 x;
        ch_SetnumMaybe(x,0);
        vrfy_(0 == ch_GetnumDflt(x,1));
        vrfy_(strptr((char*)x.ch,6) == "000000");
        ch_SetnumMaybe(x,1);
        vrfy_(1 == ch_GetnumDflt(x,0));
        vrfy_(strptr((char*)x.ch,6) == "000001");
        ch_SetnumMaybe(x,12);
        vrfy_(12 == ch_GetnumDflt(x,0));
        vrfy_(strptr((char*)x.ch,6) == "000012");
        ch_SetnumMaybe(x,123);
        vrfy_(123 == ch_GetnumDflt(x,0));
        vrfy_(strptr((char*)x.ch,6) == "000123");
        ch_SetnumMaybe(x,1234);
        vrfy_(1234 == ch_GetnumDflt(x,0));
        vrfy_(strptr((char*)x.ch,6) == "001234");
        ch_SetnumMaybe(x,12345);
        vrfy_(12345 == ch_GetnumDflt(x,0));
        vrfy_(strptr((char*)x.ch,6) == "012345");
        ch_SetnumMaybe(x,123456);
        vrfy_(123456 == ch_GetnumDflt(x,0));
        vrfy_(strptr((char*)x.ch,6) == "123456");
        vrfy_(!ch_SetnumMaybe(x,1234567));
        // test empty string
        ch_SetStrptr(x,"");
        vrfy_(0 == ch_GetnumDflt(x,1));
    }

    // space pad
    {
        algo::LspaceStr6_U32 x;
        ch_SetnumMaybe(x,0);
        vrfy_(0 == ch_GetnumDflt(x,1));
        vrfy_(strptr((char*)x.ch,6) == "     0");
        ch_SetnumMaybe(x,1);
        vrfy_(1 == ch_GetnumDflt(x,0));
        vrfy_(strptr((char*)x.ch,6) == "     1");

        ch_SetnumMaybe(x,12);
        vrfy_(12 == ch_GetnumDflt(x,0));
        vrfy_(strptr((char*)x.ch,6) == "    12");

        ch_SetnumMaybe(x,123);
        vrfy_(123 == ch_GetnumDflt(x,0));
        vrfy_(strptr((char*)x.ch,6) == "   123");

        ch_SetnumMaybe(x,1234);
        vrfy_(1234 == ch_GetnumDflt(x,0));
        vrfy_(strptr((char*)x.ch,6) == "  1234");

        ch_SetnumMaybe(x,12345);
        vrfy_(12345 == ch_GetnumDflt(x,0));
        vrfy_(strptr((char*)x.ch,6) == " 12345");

        ch_SetnumMaybe(x,123456);
        vrfy_(123456 == ch_GetnumDflt(x,0));
        vrfy_(strptr((char*)x.ch,6) == "123456");

        bool ok = !ch_SetnumMaybe(x,1234567);
        vrfy_(ok);

        // test empty string
        ch_SetStrptr(x,"");
        vrfy_(0 == ch_GetnumDflt(x,1));
    }

    // null pad on the right
    {
        atf_amc::RnullStr6_U32 x;

        ch_SetnumMaybe(x,0);
        vrfy_(0 == ch_GetnumDflt(x,1));
        vrfy_(strptr((char*)x.ch,6) == strptr("0\000\000\000\000\000",6));

        ch_SetnumMaybe(x,1);
        vrfy_(1 == ch_GetnumDflt(x,0));
        vrfy_(strptr((char*)x.ch,6) == strptr("1\000\000\000\000\000",6));

        ch_SetnumMaybe(x,12);
        vrfy_(12 == ch_GetnumDflt(x,0));
        vrfy_(strptr((char*)x.ch,6) == strptr("12\000\000\000\000",6));

        ch_SetnumMaybe(x,123);
        vrfy_(123 == ch_GetnumDflt(x,0));
        vrfy_(strptr((char*)x.ch,6) == strptr("123\000\000\000",6));

        ch_SetnumMaybe(x,1234);
        vrfy_(1234 == ch_GetnumDflt(x,0));
        vrfy_(strptr((char*)x.ch,6) == strptr("1234\000\000",6));

        ch_SetnumMaybe(x,12345);
        vrfy_(12345 == ch_GetnumDflt(x,0));
        vrfy_(strptr((char*)x.ch,6) == strptr("12345\000",6));

        ch_SetnumMaybe(x,123456);
        vrfy_(123456 == ch_GetnumDflt(x,0));
        vrfy_(strptr((char*)x.ch,6) == strptr("123456",6));

        bool ok = !ch_SetnumMaybe(x,1234567);
        vrfy_(ok);

        // test empty string
        ch_SetStrptr(x,"");
        vrfy_(0 == ch_GetnumDflt(x,1));
    }

    // rpascal string
    {
        atf_amc::RpasU32Str6 x;

        ch_SetnumMaybe(x,0);
        vrfy_(0 == ch_GetnumDflt(x,1));

        ch_SetnumMaybe(x,1);
        vrfy_(1 == ch_GetnumDflt(x,0));

        ch_SetnumMaybe(x,12);
        vrfy_(12 == ch_GetnumDflt(x,0));

        ch_SetnumMaybe(x,123);
        vrfy_(123 == ch_GetnumDflt(x,0));

        ch_SetnumMaybe(x,1234);
        vrfy_(1234 == ch_GetnumDflt(x,0));

        ch_SetnumMaybe(x,12345);
        vrfy_(12345 == ch_GetnumDflt(x,0));

        ch_SetnumMaybe(x,123456);
        vrfy_(123456 == ch_GetnumDflt(x,0));

        bool ok = !ch_SetnumMaybe(x,1234567);
        vrfy_(ok);

        // test empty string
        ch_SetStrptr(x,"");
        vrfy_(0 ==ch_GetnumDflt(x,1));
    }

}

// --------------------------------------------------------------------------------

static void NumstrStep(i64 set, i64 get, bool success) {
    u64 pad1=0xeeeeeeeeeeeeeeeeULL;
    algo::LnumStr4_U32 str;
    u64 pad2=0xaaaaaaaaaaaaaaaaULL;
    bool ok=ch_SetnumMaybe(str,set);
    vrfyeq_(ok,success);
    vrfyeq_(pad1,u64(0xeeeeeeeeeeeeeeeeULL));
    vrfyeq_(pad2,u64(0xaaaaaaaaaaaaaaaaULL));
    i64 val=ch_GetnumDflt(str,get+1);
    vrfyeq_(val,get);
}

void atf_amc::amctest_NumstrCorruption() {
    NumstrStep(1,1,true);
    NumstrStep(10,10,true);
    NumstrStep(100,100,true);
    NumstrStep(1000,1000,true);
    NumstrStep(9999,9999,true);
    NumstrStep(0,0,true);
    NumstrStep(10000,0,false);
    NumstrStep(-1,0,false);
    NumstrStep(100000,0,false);
}

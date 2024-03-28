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
// Target: atf_amc (exe) -- Unit tests for amc (see amctest table)
// Exceptions: NO
// Source: cpp/atf_amc/cmp.cpp
//

#include "include/atf_amc.h"

static void CheckCmp(strptr str1, strptr str2, int vs, int novs) {
    {
        atf_amc::SortedStr a,b;
        a.vs=str1;
        b.vs=str2;
        vrfy_(atf_amc::SortedStr_Cmp(a,b)==vs);
    }
    {
        atf_amc::SortedStr a,b;
        a.novs=str1;
        b.novs=str2;
        vrfy_(atf_amc::SortedStr_Cmp(a,b)==novs);
    }
}

void atf_amc::amctest_Fcmp() {
    CheckCmp("a","b",-1,-1);// less, under all circumstances
    CheckCmp("9","10",-1,1);
    CheckCmp("a01","a02",-1,-1);
    CheckCmp("b","a",1,1);
    CheckCmp("a","a",0,0);
    CheckCmp("33","33",0,0);
    CheckCmp("/","0",-1,-1);// ascii 47 sorts before ascii 48 in all cases
    CheckCmp(":","9",1,1);// ascii 58 sorts after ascii 57 in all cases
}

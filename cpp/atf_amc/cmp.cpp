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

// --------------------------------------------------------------------------------

// Compare STR1 and STR2 through the fcmp functions of two Smallstr fields:
// VsortStr9.ch (versionsort, case-insensitive) must return VS,
// NovsStr9.ch (plain, case-sensitive) must return NOVS
static void CheckSmallstrCmp(strptr str1, strptr str2, int vs, int novs) {
    {
        atf_amc::VsortStr9 a,b;
        a = str1;
        b = str2;
        vrfy_(atf_amc::ch_Cmp(a,b)==vs);
        vrfy_(atf_amc::ch_Lt(a,b) == (vs<0));
    }
    {
        atf_amc::NovsStr9 a,b;
        a = str1;
        b = str2;
        vrfy_(atf_amc::ch_Cmp(a,b)==novs);
        vrfy_(atf_amc::ch_Lt(a,b) == (novs<0));
    }
}

// fcmp on a Smallstr field: the field itself is the char array, so the
// comparison reads it through the field's Getary/Nextchar
void atf_amc::amctest_SmallstrFcmp() {
    CheckSmallstrCmp("a","b",-1,-1);
    CheckSmallstrCmp("9","10",-1,1);// versionsort compares embedded integers
    CheckSmallstrCmp("a01","a02",-1,-1);
    CheckSmallstrCmp("b","a",1,1);
    CheckSmallstrCmp("a","a",0,0);
    CheckSmallstrCmp("33","33",0,0);
    CheckSmallstrCmp("A","a",0,-1);// case-insensitive equal; case-sensitive 'A' < 'a'
}

// --------------------------------------------------------------------------------

// Compare STR1 and STR2 through the fcmp of RspaceStr9.ch (rightpad, pad ' ',
// versionsort): expect CMP, and Lt consistent with it
static void CheckRspaceCmp(strptr str1, strptr str2, int cmp) {
    atf_amc::RspaceStr9 a,b;
    a = str1;
    b = str2;
    vrfy_(atf_amc::ch_Cmp(a,b)==cmp);
    vrfy_(atf_amc::ch_Lt(a,b) == (cmp<0));
}

// fcmp on a padded Smallstr field: the comparison reads the value through
// Getary, which excludes the pad bytes, so neither the length tiebreak nor
// the versionsort digit walk sees the padding
void atf_amc::amctest_SmallstrFcmpPad() {
    CheckRspaceCmp("ab ","ab",0);// a trailing pad char in the input is not part of the value
    CheckRspaceCmp("9","10",-1);// versionsort: pad bytes do not extend the embedded integer
    CheckRspaceCmp("a2","a10",-1);
    CheckRspaceCmp("a","ab",-1);// prefix sorts first
    CheckRspaceCmp("b","a",1);
    CheckRspaceCmp("abcdefghi","abcdefghi",0);// full-width value: no pad at all
}

// --------------------------------------------------------------------------------

// Lt for a ctype whose single field is an Upptr: compares the pointer values
void atf_amc::amctest_UpptrLtSingleField() {
    atf_amc::FTypeA &typea1 = atf_amc::typea_Alloc();
    atf_amc::FTypeA &typea2 = atf_amc::typea_Alloc();
    atf_amc::UpptrLt a;
    atf_amc::UpptrLt b;
    a.p_typea = &typea1;
    b.p_typea = &typea2;
    // compare addresses as integers: relational compare of pointers into
    // two distinct pool rows is unspecified, and the generated Lt itself
    // compares (u64)(void*) values
    vrfy_(atf_amc::UpptrLt_Lt(a,b) == ((u64)(void*)&typea1 < (u64)(void*)&typea2));
    vrfy_(atf_amc::UpptrLt_Lt(b,a) == ((u64)(void*)&typea2 < (u64)(void*)&typea1));
    vrfy_(!atf_amc::UpptrLt_Lt(a,a));
    atf_amc::typea_RemoveLast();
    atf_amc::typea_RemoveLast();
}

// --------------------------------------------------------------------------------

// Field-level Lt for an fcmp field whose arg type defines Cmp but no Lt
// (ccmp order:N): the comparison goes through Cmp
void atf_amc::amctest_ErrcodeLtField() {
    atf_amc::ErrcodeFldLt a;
    atf_amc::ErrcodeFldLt b;
    a.err.value = 1;
    b.err.value = 2;
    vrfy_(atf_amc::err_Lt(a,b));
    vrfy_(!atf_amc::err_Lt(b,a));
    vrfy_(!atf_amc::err_Lt(a,a));
    vrfy_(atf_amc::err_Cmp(a,b) < 0);
    vrfy_(atf_amc::err_Cmp(b,a) > 0);
    vrfy_(atf_amc::err_Cmp(a,a) == 0);
}

// --------------------------------------------------------------------------------

// Lt for an ordered ctype whose single field's type defines Cmp but no Lt
// (ccmp order:N): the comparison goes through Cmp
void atf_amc::amctest_ErrcodeLtSingleField() {
    atf_amc::ErrcodeLt a;
    atf_amc::ErrcodeLt b;
    a.err.value = 1;
    b.err.value = 2;
    vrfy_(atf_amc::ErrcodeLt_Lt(a,b));
    vrfy_(!atf_amc::ErrcodeLt_Lt(b,a));
    vrfy_(!atf_amc::ErrcodeLt_Lt(a,a));
}

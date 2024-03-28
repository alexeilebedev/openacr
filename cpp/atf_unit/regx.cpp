// Copyright (C) 2020-2021 Astra
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
// Target: atf_unit (exe) -- Unit tests (see unittest table)
// Exceptions: yes
// Source: cpp/atf_unit/regx.cpp
//

#include "include/atf_unit.h"

//*****------------------------------------------------ Oct 8 1999 ------*****

static bool RegxMatch(strptr expr, strptr str) {
    algo_lib::Regx regx;
    Regx_ReadDflt(regx,expr);
    return Regx_Match(regx,str);
}
static bool RegxMatchShell(strptr expr, strptr str) {
    algo_lib::Regx regx;
    Regx_ReadShell(regx,expr,true);
    return Regx_Match(regx,str);
}
static bool RegxMatchSql(strptr expr, strptr str) {
    algo_lib::Regx regx;
    Regx_ReadSql(regx,expr,true);
    return Regx_Match(regx,str);
}
static bool RegxAcrLiteralQ(strptr expr) {
    algo_lib::Regx regx;
    Regx_ReadAcr(regx,expr,true);
    return regx.literal;
}
static bool RegxSqlLiteralQ(strptr expr) {
    algo_lib::Regx regx;
    Regx_ReadSql(regx,expr,true);
    return regx.literal;
}

// --------------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_Regx() {
    vrfyeq_(RegxMatch("", ""), true);// empty regx matches empty string
    vrfyeq_(RegxMatch("", "x"), false);// empty regx matches empty string only
    vrfyeq_(RegxMatch(".*", "abcd")                     , true);
    vrfyeq_(RegxMatch("a.*", "abcd")                     , true);
    vrfyeq_(RegxMatch("ab.*", "abcd")                     , true);
    vrfyeq_(RegxMatch("abc.*", "abcd")                     , true);
    vrfyeq_(RegxMatch("abcd.*", "abcd")                     , true);
    vrfyeq_(RegxMatch("abcd.*", "abcde")                     , true);
    vrfyeq_(RegxMatch(".+", "abcd")                     , true);
    vrfyeq_(RegxMatch(".+", ""    )                     , false);
    vrfyeq_(RegxMatch("ab+c", "abc")                    , true);
    vrfyeq_(RegxMatch("ab+c", "abbbbbbbbbbbbbbbbbbbbc") , true);
    vrfyeq_(RegxMatch("abcd", "abcd")                   , true);
    vrfyeq_(RegxMatch("a(b|c)d", "acd")                 , true);
    vrfyeq_(RegxMatch("a(b|c)d", "abd")                 , true);
    vrfyeq_(RegxMatch("abcd", "abc")                    , false);
    vrfyeq_(RegxMatch(".*", "")                 , true);
    vrfyeq_(RegxMatch(".*", ".*")                 , true);
    vrfyeq_(RegxMatch(".*", "\x80\xff")                 , true);
    vrfyeq_(RegxMatch("\x82\x83", "\x82\x83")           , true);
    vrfyeq_(RegxMatch("\x82", "\x81")                   , false);
    vrfyeq_(RegxMatch("[\x81-\x85]+", "\x82\x83")       , true);
    vrfyeq_(RegxMatch("[\x81-\x85]", "\x80")            , false);

    vrfyeq_(RegxMatch("(", ""), true);// bad regx -- but must match
    vrfyeq_(RegxMatch("(((", ""), true);// bad regx -- but must match
    vrfyeq_(RegxMatch(")", ")"), true);// bad regx -- but must match
    vrfyeq_(RegxMatch("[", "["), true);// bad regx -- but must match
    vrfyeq_(RegxMatch("]", "]"), true);// bad regx -- but must match
    vrfyeq_(RegxMatch("]](", "]]"), true);// bad regx -- but must match
    vrfyeq_(RegxMatch("[a-b", "[a-b"), true);// bad regx -- but must match
    vrfyeq_(RegxMatch("", ""), true);// bad regx -- but must match

    vrfyeq_(RegxMatchShell("*makefile*", "temp_makefile"), true);
    vrfyeq_(RegxMatchShell("*makefile*", "makefile"), true);
    vrfyeq_(RegxMatchShell("*make.ile", "makefile"), false);// should not match -- . is not special
    vrfyeq_(RegxMatchShell("%", "makefile"), false);// should not match -- . is not special

    vrfyeq_(RegxMatchSql("", ""), true);// empty regx matches empty string
    vrfyeq_(RegxMatchSql("", "x"), false);// empty regx matches empty string only
    vrfyeq_(RegxMatchSql("\\_", "a"), false);// escaped underscore -> real char
    vrfyeq_(RegxMatchSql("%", ""), true);
    vrfyeq_(RegxMatchSql("%", "a"), true);
    vrfyeq_(RegxMatchSql("%%%%%%", "a"), true);
    vrfyeq_(RegxMatchSql("asdf%f", "asdfasdfasdf"), true);// wildcard
    vrfyeq_(RegxMatchSql("asdf\\%f", "asdfasdfasdf"), false);// escaped wildcard
    vrfyeq_(RegxMatchSql("asdf\\%f", "asdf%f"), true);// should be ok
    vrfyeq_(RegxMatchSql("(a|b)", "a"), true);// should be ok
    vrfyeq_(RegxMatchSql("(a|b)", "b"), true);// should be ok
    vrfyeq_(RegxMatchSql("(a|b)", "c"), false);// should be ok
    vrfyeq_(RegxMatchSql("(a|b)(d|e)", "ae"), true);// should be ok
    vrfyeq_(RegxMatchSql("(a|b)(d|e)", "bd"), true);// should be ok
    vrfyeq_(RegxMatchSql("(a|b)(d|e)", "bd"), true);// should be ok

    vrfy_(RegxSqlLiteralQ(""));
    vrfy_(RegxSqlLiteralQ("2"));
    vrfy_(RegxSqlLiteralQ("."));
    vrfy_(RegxSqlLiteralQ("*"));
    vrfy_(RegxSqlLiteralQ("+"));
    vrfy_(RegxSqlLiteralQ("?"));
    vrfy_(RegxSqlLiteralQ("$"));
    vrfy_(RegxSqlLiteralQ("^"));
    vrfy_(!RegxSqlLiteralQ("_"));
    vrfy_(!RegxSqlLiteralQ("|"));
    vrfy_(!RegxSqlLiteralQ("%"));
    vrfy_(!RegxSqlLiteralQ("("));
    vrfy_(!RegxSqlLiteralQ(")"));
    vrfy_(!RegxSqlLiteralQ("["));
    vrfy_(!RegxSqlLiteralQ("]"));

    vrfy_(RegxAcrLiteralQ(""));
    vrfy_(RegxAcrLiteralQ("2"));
    vrfy_(RegxAcrLiteralQ("."));
    vrfy_(RegxAcrLiteralQ("_"));
    vrfy_(RegxAcrLiteralQ("*"));
    vrfy_(RegxAcrLiteralQ("+"));
    vrfy_(RegxAcrLiteralQ("?"));
    vrfy_(RegxAcrLiteralQ("$"));
    vrfy_(RegxAcrLiteralQ("^"));
    vrfy_(!RegxAcrLiteralQ("|"));
    vrfy_(!RegxAcrLiteralQ("%"));
    vrfy_(!RegxAcrLiteralQ("("));
    vrfy_(!RegxAcrLiteralQ(")"));
    vrfy_(!RegxAcrLiteralQ("["));
    vrfy_(!RegxAcrLiteralQ("]"));
}

// -----------------------------------------------------------------------------

static void ShortCircuitMatch(strptr regx_str, strptr str, int njunk, strptr junkstr, bool expect, int maxcycles) {
    algo_lib::Regx regx;
    Regx_ReadDflt(regx,regx_str);
    // a megabyte of junk
    cstring test(str);
    for (int i=0; i<njunk; i++) {
        test << junkstr;
    }
    bool good =false;
    for (int iter=0; iter<100; iter++) {
        u64 start=algo::get_cycles();
        bool result=Regx_Match(regx,test);
        u64 end=algo::get_cycles();
        TESTCMP(result,expect);
        if (start + maxcycles > end) {
            good=true;
            break;
        }
    }
    TESTCMP(good,true);
}

// --------------------------------------------------------------------------------

// Test that matching a huge string with a regex that
// ends in .* is fast.
void atf_unit::unittest_algo_lib_RegxShortCircuit() {
    ShortCircuitMatch("abcd.*", "abcde", 1000000, "x", true, 10000);
    ShortCircuitMatch("abcd.*.*", "abcdef", 1000000, "x", true, 10000);
    ShortCircuitMatch(".*", "abcde", 1000000, "x", true, 10000);
    ShortCircuitMatch("", "abcde", 1000000, "x", false, 10000);// must quickly NOT match this
}

// --------------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_RegxReadTwice() {
    // Read a regx twice. It should continue to work
    algo_lib::Regx regx;
    Regx_ReadSql(regx, "a", true);
    vrfyeq_(Regx_Match(regx, "a"), true);
    vrfyeq_(Regx_Match(regx, "b"), false);

    Regx_ReadSql(regx, "b", true);
    vrfyeq_(Regx_Match(regx, "a"), false);
    vrfyeq_(Regx_Match(regx, "b"), true);
}

// --------------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_RegxReadTwice2() {
    // Read a regx twice. It should continue to work
    algo_lib::Regx regx;
    Regx_ReadSql(regx, "%", false);
    vrfyeq_(Regx_Match(regx, "aasdfasdf"), true);
    vrfyeq_(Regx_Match(regx, "asdfasdf"), true);

    Regx_ReadSql(regx, "b", true);
    vrfyeq_(Regx_Match(regx, "a"), false);
    vrfyeq_(Regx_Match(regx, "b"), true);
}

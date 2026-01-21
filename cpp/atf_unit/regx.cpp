// Copyright (C) 2023-2024,2026 AlgoRND
// Copyright (C) 2020-2021 Astra
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

static bool RegxMatch(strptr expr, strptr str, algo_lib::RegxStyle style = algo_lib_RegxStyle_default) {
    algo_lib::Regx regx;
    Regx_ReadStyle(regx,expr,style,true);
    return Regx_Match(regx,str);
}
static bool RegxLiteralQ(strptr expr, algo_lib::RegxStyle style) {
    algo_lib::Regx regx;
    Regx_ReadStyle(regx,expr,style,true);
    return literal_Get(regx.flags);
}
static algo::i32_Range RegxFind(strptr expr, algo::strptr str, int start = 0, algo_lib::RegxStyle style = algo_lib_RegxStyle_default) {
    algo_lib::Regx regx;
    Regx_ReadStyle(regx,expr,style,false);
    return Regx_Find(regx,str,start);
}
static bool ValidRegxQ(strptr expr) {
    algo_lib::Regx regx;
    Regx_ReadDflt(regx,expr);
    return valid_Get(regx.flags);
}

// --------------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_Regx() {
    // Check empty (just-initialized) regex
    {
        algo_lib::Regx regx;
        vrfyeq_(Regx_Match(regx,""),true);
        vrfyeq_(Regx_Match(regx,"a"),false);
    }

    // Macros
    vrfyeq_(RegxMatch("\\w+","abcde_12345"),true);
    vrfyeq_(RegxMatch("\\w+","a-b"),false);
    vrfyeq_(RegxMatch("\\d+","0123456789"),true);
    vrfyeq_(RegxMatch("\\s*a","a"),true);
    vrfyeq_(RegxMatch("\\s+a","a"),false);
    vrfyeq_(RegxMatch("\\s+a","   a"),true);

    vrfyeq_(RegxMatch("", ""), true);
    vrfyeq_(RegxMatch("", "x"), false);
    vrfyeq_(RegxMatch("x", "x"), true);
    vrfyeq_(RegxMatch("x", "xy"), false);

    vrfyeq_(RegxMatch(".*", ""), true);
    vrfyeq_(RegxMatch(".*", "abcd"), true);
    vrfyeq_(RegxMatch("a.*", "abcd"), true);
    vrfyeq_(RegxMatch("ab.*", "abcd"), true);
    vrfyeq_(RegxMatch("abc.*", "abcd"), true);
    vrfyeq_(RegxMatch("abcd.*", "abcd"), true);
    vrfyeq_(RegxMatch("abcd.*", "abcde"), true);
    vrfyeq_(RegxMatch(".*", ".*"), true);
    vrfyeq_(RegxMatch(".*", "\x80\xff"), true);

    vrfyeq_(RegxMatch(".+", "abcd"), true);
    vrfyeq_(RegxMatch(".+", ""    ), false);
    vrfyeq_(RegxMatch("ab+c", "abc"), true);
    vrfyeq_(RegxMatch("ab+c", "abbbbbbbbbbbbbbbbbbbbc"), true);

    vrfyeq_(RegxMatch("a|b", "a"), true);
    vrfyeq_(RegxMatch("a|b", "b"), true);

    vrfyeq_(RegxMatch("a|b", "c"), false);
    vrfyeq_(RegxMatch("(a|b)", "b"), true);
    vrfyeq_(RegxMatch("(a|b)", "a"), true);

    vrfyeq_(RegxMatch("[a]", "a"), true);
    vrfyeq_(RegxMatch("[a]", "b"), false);
    vrfyeq_(RegxMatch("c[a]b", "cab"), true);
    vrfyeq_(RegxMatch("c[a-z]b", "ctb"), true);
    vrfyeq_(RegxMatch("c[a-z]b", "cAb"), false);
    vrfyeq_(RegxMatch("[^a]", "a"), false);
    vrfyeq_(RegxMatch("[^a]", "b"), true);

    vrfyeq_(RegxMatch("a(b|c)d", "acd"), true);
    vrfyeq_(RegxMatch("a(b|c)d", "abd"), true);
    vrfyeq_(RegxMatch("abcd", "abc"), false);
    vrfyeq_(RegxMatch("\x82\x83", "\x82\x83"), true);
    vrfyeq_(RegxMatch("\x82", "\x81"), false);
    vrfyeq_(RegxMatch("[\x81-\x85]+", "\x82\x83"), true);
    vrfyeq_(RegxMatch("[\x81-\x85]", "\x80"), false);

    vrfyeq_(RegxMatch("()", ""), true);
    vrfyeq_(RegxMatch(".*(.*).*", "abcde"), true);

    vrfyeq_(RegxMatch("^aaa", "aaa"), true);
    vrfyeq_(RegxMatch("^aaa", "bbb"), false);

    vrfyeq_(RegxMatch(".*(^a|b)", "a"), true);
    vrfyeq_(RegxMatch(".*(^a|b)", "b"), true);
    vrfyeq_(RegxMatch(".*(^a|b)", "c"), false);
    vrfyeq_(RegxMatch(".*(^a|b)", ""), false);
    vrfyeq_(RegxMatch(".*(^a|b)", "DDDb"), true);
    vrfyeq_(RegxMatch(".*(^a|b)", "DDDa"), false);

    // test trailing anchor
    vrfyeq_(RegxMatch(".*(a$|b).*", "bbba"), true);
    vrfyeq_(RegxMatch(".*(a$|b).*", "cccbfff"), true);
    vrfyeq_(RegxMatch(".*(a$|b).*", ""), false);
    vrfyeq_(RegxMatch(".*(a$|b).*", "a"), true);
    vrfyeq_(RegxMatch(".*(a$|b).*", "ac"), false);
    vrfyeq_(RegxMatch(".*(a$|b).*", "ab"), true);
    vrfyeq_(RegxMatch("xxxb$", "xxxb"), true);
    vrfyeq_(!RegxMatch("xxxb$", "xxxbc"), true);
    vrfyeq_(RegxMatch("abra.*$", "abra"), true);
    vrfyeq_(RegxMatch("abra.*$", "abraddd"), true);
    vrfyeq_(RegxMatch("abra.*$", "abcaddd"), false);

    vrfyeq_(ValidRegxQ("("), false); // bad regx

    vrfyeq_(RegxMatch("(", ""), false); // bad regx
    vrfyeq_(RegxMatch("(((", ""), false);// bad regx
    vrfyeq_(RegxMatch(")", ")"), true);
    vrfyeq_(RegxMatch("[", "["), true);// bad regx -- but must match
    vrfyeq_(RegxMatch("]", "]"), true);// bad regx -- but must match
    vrfyeq_(RegxMatch("]](", "]]"), false);// bad regx
    vrfyeq_(RegxMatch("[a-b", "[a-b"), true);// bad regx -- but must match
    vrfyeq_(RegxMatch("", ""), true);// bad regx -- but must match

    vrfyeq_(RegxMatch("*makefile*", "temp_makefile", algo_lib_RegxStyle_shell), true);
    vrfyeq_(RegxMatch("*makefile*", "makefile", algo_lib_RegxStyle_shell), true);
    vrfyeq_(RegxMatch("*make.ile", "makefile", algo_lib_RegxStyle_shell), false);// should not match -- . is not special
    vrfyeq_(RegxMatch("%", "makefile", algo_lib_RegxStyle_shell), false);// should not match -- . is not special

    vrfyeq_(RegxMatch("", "", algo_lib_RegxStyle_sql), true);
    vrfyeq_(RegxMatch("", "x", algo_lib_RegxStyle_sql), false);
    vrfyeq_(RegxMatch("\\_", "a", algo_lib_RegxStyle_sql), false);// escaped underscore -> real char
    vrfyeq_(RegxMatch("%", "", algo_lib_RegxStyle_sql), true);
    vrfyeq_(RegxMatch("%", "a", algo_lib_RegxStyle_sql), true);
    vrfyeq_(RegxMatch("%%%%%%", "a", algo_lib_RegxStyle_sql), true);
    vrfyeq_(RegxMatch("asdf%f", "asdfasdfasdf", algo_lib_RegxStyle_sql), true);// wildcard
    vrfyeq_(RegxMatch("asdf\\%f", "asdfasdfasdf", algo_lib_RegxStyle_sql), false);// escaped wildcard
    vrfyeq_(RegxMatch("asdf\\%f", "asdf%f", algo_lib_RegxStyle_sql), true);// should be ok
    vrfyeq_(RegxMatch("(a|b)", "a", algo_lib_RegxStyle_sql), true);// should be ok
    vrfyeq_(RegxMatch("(a|b)", "b", algo_lib_RegxStyle_sql), true);// should be ok
    vrfyeq_(RegxMatch("(a|b)", "c", algo_lib_RegxStyle_sql), false);// should be ok
    vrfyeq_(RegxMatch("(a|b)(d|e)", "ae", algo_lib_RegxStyle_sql), true);
    vrfyeq_(RegxMatch("(a|b)(d|e)", "bd", algo_lib_RegxStyle_sql), true);
    vrfyeq_(RegxMatch("(a|b)(d|e)", "bd", algo_lib_RegxStyle_sql), true);

    vrfyeq_(RegxMatch("", "", algo_lib_RegxStyle_literal), true);
    vrfyeq_(RegxMatch("", "x", algo_lib_RegxStyle_literal), false);
    vrfyeq_(RegxMatch("x", "x", algo_lib_RegxStyle_literal), true);
    vrfyeq_(RegxMatch("x", "", algo_lib_RegxStyle_literal), false);
    vrfyeq_(RegxMatch(".*", "xyz", algo_lib_RegxStyle_literal), false);
    vrfyeq_(RegxMatch(".*", ".*", algo_lib_RegxStyle_literal), true);

    vrfy_(RegxLiteralQ("", algo_lib_RegxStyle_default));
    vrfy_(RegxLiteralQ("abc", algo_lib_RegxStyle_default));
    vrfy_(!RegxLiteralQ(".*", algo_lib_RegxStyle_default));
    vrfy_(!RegxLiteralQ(".*().*", algo_lib_RegxStyle_default));
    vrfy_(!RegxLiteralQ(".*(.*).*", algo_lib_RegxStyle_default));

    vrfy_(RegxLiteralQ("2", algo_lib_RegxStyle_sql));
    vrfy_(RegxLiteralQ("abcxyz$^", algo_lib_RegxStyle_sql));
    vrfy_(!RegxLiteralQ("a|b", algo_lib_RegxStyle_sql));
    vrfy_(!RegxLiteralQ("(d)", algo_lib_RegxStyle_sql));
    vrfy_(!RegxLiteralQ("%", algo_lib_RegxStyle_sql));

    vrfy_(RegxLiteralQ("", algo_lib_RegxStyle_sql));
    vrfy_(RegxLiteralQ("2abcxyz$^", algo_lib_RegxStyle_sql));
    vrfy_(!RegxLiteralQ("a|b", algo_lib_RegxStyle_sql));
    vrfy_(!RegxLiteralQ("(d)", algo_lib_RegxStyle_sql));
    vrfy_(!RegxLiteralQ("%", algo_lib_RegxStyle_sql));

    vrfy_(RegxLiteralQ("2abcxyz$^", algo_lib_RegxStyle_acr));
    vrfy_(!RegxLiteralQ("a|b", algo_lib_RegxStyle_acr));
    vrfy_(!RegxLiteralQ("(d)", algo_lib_RegxStyle_acr));
    vrfy_(!RegxLiteralQ("%", algo_lib_RegxStyle_acr));

    vrfyeq_(RegxFind("ca","abracadabra"),algo::i32_Range(4,6));
    vrfyeq_(RegxFind("c.d.b","abracadabra"),algo::i32_Range(4,9));

    // this one doesn't work because regx figures out right away that abracadabra matches .*
    // and returns 0,0 0 -- todo figure out what to do with that
    //vrfyeq_(RegxFind(".*","abracadabra"),algo::i32_Range(0,11));
}

// -----------------------------------------------------------------------------

static void ShortCircuitMatch(strptr regx_str, strptr str, int njunk, strptr junkstr, bool expect, int maxcycles) {
    prlog("test short circuit"
          <<Keyval("regx_str",regx_str)
          <<Keyval("str",tempstr()<<str<<junkstr<<" x "<<njunk)
          <<Keyval("maxycles",maxcycles)
          <<Keyval("expect",expect));
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
    ShortCircuitMatch("abcd.*"  , "abcde" , 1000000, "x", true , 100000);
    ShortCircuitMatch("abcd.*.*", "abcdef", 1000000, "x", true , 100000);
    ShortCircuitMatch(".*"      , "abcde" , 1000000, "x", true , 100000);
    ShortCircuitMatch(""        , "abcde" , 1000000, "x", false, 100000);// must quickly NOT match this
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

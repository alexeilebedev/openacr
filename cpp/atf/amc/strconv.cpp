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
// Source: cpp/atf/amc/strconv.cpp
//
// Created By: alexei.lebedev
// Recent Changes: alexei.lebedev
//

#include "include/atf_amc.h"
#include "include/algo/pcbuf.h"

// -----------------------------------------------------------------------------

static void TestOneNumber(StringDesc &desc, i64 num) {
    // ignore negative numbers with unsigned string.
    if (num < 0 && !desc.issigned) {
        return;
    }
    // if the number doesn't fit the target numeric type, no sense trying
    if (desc.issigned
        ? (i64(num) < i64(desc.numtype_min) || i64(num) > i64(desc.numtype_max))
        : (u64(num) < u64(desc.numtype_min) || u64(num) > u64(desc.numtype_max))) {
        return;
    }

    u8 stringbuf[1024];//
    desc.Init(stringbuf);

    // test Setnum
    bool setnum_ok = desc.SetnumMaybe(stringbuf, num);

    static char base36[37] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    u64 val = Abs(num);
    char tempbuf[64];
    u32 offset = sizeof(tempbuf);
    int min_digits = desc.min_length;
    do {
        vrfy_(offset>0);
        tempbuf[--offset] = base36[val % desc.base];
        --min_digits;
        val /= desc.base;
    } while (val != 0 || min_digits > 0);
    if (desc.issigned && num < 0) {
        vrfy_(offset>0);
        tempbuf[--offset] = '-';
    }
    u32 len = sizeof(tempbuf) - offset;
    tempstr expect_str;
    if (desc.strtype == dmmeta_Strtype_strtype_leftpad) {
        char_PrintNTimes(desc.pad, expect_str, desc.max_length - len);
    }
    expect_str << strptr(tempbuf+offset, len);
    if (desc.strtype == dmmeta_Strtype_strtype_rightpad) {
        char_PrintNTimes(desc.pad, expect_str, desc.max_length - len);
    }

    bool expect_ok = len <= desc.max_length;
    strptr stored_str = desc.Getary(stringbuf);
    if (desc.strtype == dmmeta_Strtype_strtype_rightpad || desc.strtype == dmmeta_Strtype_strtype_leftpad) {
        // grab entire string, including padding
        stored_str = strptr((char*)stringbuf, desc.max_length);
    }

    // string does not ho
    bool str_match = stored_str == expect_str;
    bool string_ok = false;
    i64 stored_num = desc.Geti64(stringbuf,string_ok);
    bool num_match = stored_num == num;
    strptr error;
    if (expect_ok != setnum_ok) {
        error = "Exception thrown mismatch";
    }
    if (expect_ok && elems_N(error) == 0 && !str_match) {
        error = "Stored / expected string mismatch";
    }
    if (expect_ok && elems_N(error) == 0 && stored_num != num) {
        error= "Geti64 value mismatch.";
    }
    if (!string_ok) {
        error= "Geti64 returns error.";
    }

    if (algo_lib::_db.cmdline.verbose > 1 || elems_N(error) > 0) {
        prerr("");
        prerr("NUMSTR DEFINITION");
        prerr("smallstr           " << desc.smallstr);
        prerr("string type        " << desc.strtype);
        prerr("max length         " << desc.max_length);
        prerr("base               " << desc.base);
        prerr("pad                " << int(desc.pad));
        prerr("signed             " << desc.issigned);
        prerr("num min            " << desc.numtype_min<<" (min. number for numeric type)");
        prerr("num max            " << desc.numtype_max<<" (max. number for numeric type)");

        prerr("");
        prerr("TEST DEFINITION");
        prerr("number tested     " << num);
        prerr("expect string      ["<<expect_str<<"], length "<<ch_N(expect_str));
        prerr("expect ok         " << expect_ok);

        prerr("");
        prerr("TEST RESULT");
        prerr("actual string      ["<<stored_str<<"], length "<<elems_N(stored_str));
        prerr("actual string ok   " << string_ok);
        prerr("actual exception   " << !setnum_ok);
        prerr("actual str match   " << str_match);
        prerr("actual num match   " << num_match);
        prerr("actual number read " << stored_num);
    }
    if (elems_N(error) > 0) {
        vrfy(0,error);
    }
}

static void StringConversionTest(StringDesc& desc) {
    int iter_n = 10000;
    u64 strtype_max = pow(desc.base, desc.max_length);
    u64 strtype_min = -pow(desc.base, desc.max_length-1);

    // first N integers
    for (int i = 0; i < iter_n; ++i) {
        TestOneNumber(desc, i);
    }

    // numbers around max
    for (int i = 0; i < iter_n; ++i) {
        TestOneNumber(desc, strtype_max - iter_n/2 + i);
    }

    // first N negative integers
    for (int i = 0; i < iter_n; ++i) {
        TestOneNumber(desc, -i);
    }

    // numbers around string min.
    for (int i = 0; i < iter_n; ++i) {
        TestOneNumber(desc, strtype_min - iter_n/2 + i);
    }

    // numbers around num. max
    for (int i = 0; i < iter_n; ++i) {
        TestOneNumber(desc, desc.numtype_max - iter_n/2 + i);
    }

    // numbers around num. min
    for (int i = 0; i < iter_n; ++i) {
        TestOneNumber(desc, desc.numtype_min - iter_n/2 + i);
    }
}

// -----------------------------------------------------------------------------

void atf_amc::amctest_TestString() {
    atf_amc::ForAllStrings(&StringConversionTest);
    algo::ForAllStrings(&StringConversionTest);

    algo_assert(sizeof(algo::RspaceStr9) == 9);
    algo_assert(sizeof(algo::RspaceStr10) == 10);
    algo_assert(sizeof(algo::RspaceStr11) == 11);

    algo_assert(sizeof(byte)== 1);
    algo_assert(sizeof(i8)  == 1);
    algo_assert(sizeof(i16) == 2);
    algo_assert(sizeof(i32) == 4);
    algo_assert(sizeof(i64) == 8);
    algo_assert(sizeof(u8)  == 1);
    algo_assert(sizeof(u16) == 2);
    algo_assert(sizeof(u32) == 4);
    algo_assert(sizeof(u64) == 8);
    algo_assert(sizeof(f32) == 4);
    algo_assert(sizeof(f64) == 8);

#ifdef UNAME_LINUX
    algo_assert(sizeof(off64_t) == sizeof(u64));
#endif
    algo_assert(sizeof(algo::PCPLine)==CACHE_LINE);
}

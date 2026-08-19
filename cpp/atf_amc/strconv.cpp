// Copyright (C) 2023-2024,2026 AlgoRND
// Copyright (C) 2020-2021 Astra
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
// Source: cpp/atf_amc/strconv.cpp
//

#include "include/atf_amc.h"

// -----------------------------------------------------------------------------

// The character encoding digit DIGIT in numstr's base-BASE alphabet:
// '0'..'9' then 'A'..'Z' through base 36, ' '..'~' in base 95, and the
// raw byte in base 256.
static char DigitChar(u32 base, u32 digit) {
    char retval;
    if (base == 256) {
        retval = char(digit);
    } else if (base == 95) {
        retval = char(' ' + digit);
    } else if (digit < 10) {
        retval = char('0' + digit);
    } else {
        retval = char('A' + digit - 10);
    }
    return retval;
}

// -----------------------------------------------------------------------------

// STR as a double-quoted C++ literal with nonprintable bytes escaped:
// base-256 digit strings carry NUL and control bytes that must not reach
// the log raw.
static tempstr Quoted(strptr str) {
    tempstr retval;
    algo::strptr_PrintCpp(str, retval);
    return retval;
}

// -----------------------------------------------------------------------------

// Stored bytes of the string field in STRINGBUF: the whole fixed-width
// region for a padded strtype, otherwise the logical string.
static strptr StoredStr(StringDesc &desc, u8 *stringbuf) {
    strptr retval = desc.Getary(stringbuf);
    if (desc.strtype == dmmeta_Strtype_strtype_rightpad || desc.strtype == dmmeta_Strtype_strtype_leftpad) {
        retval = strptr((char*)stringbuf, desc.max_length);
    }
    return retval;
}

// -----------------------------------------------------------------------------

// A value within the numtype range: when its digits (with min_length
// padding and any sign) fit the string, SetnumMaybe must store exactly the
// expected digit string and Geti64 must read the value back; when they do
// not fit, SetnumMaybe must return false and leave the stored bytes as
// they were.
static void TestInrangeNumber(StringDesc &desc, u8 *stringbuf, i64 num) {
    tempstr before_str(StoredStr(desc, stringbuf));
    bool setnum_ok = desc.SetnumMaybe(stringbuf, num);

    // negate after widening to u64: -num in i64 is undefined for i64min,
    // whose magnitude has no i64 representation
    u64 val = num < 0 ? 0 - u64(num) : u64(num);
    char tempbuf[64];
    u32 offset = sizeof(tempbuf);
    int min_digits = desc.min_length;
    do {
        vrfy_(offset>0);
        tempbuf[--offset] = DigitChar(desc.base, u32(val % desc.base));
        --min_digits;
        val /= desc.base;
    } while (val != 0 || min_digits > 0);
    if (desc.issigned && num < 0) {
        vrfy_(offset>0);
        tempbuf[--offset] = '-';
    }
    u32 len = sizeof(tempbuf) - offset;
    bool expect_ok = len <= desc.max_length;
    // the expected stored bytes: the digit string, padded per strtype, when
    // it fits; the prior bytes -- a refused value stores nothing -- when not
    tempstr expect_str;
    if (expect_ok) {
        if (desc.strtype == dmmeta_Strtype_strtype_leftpad) {
            char_PrintNTimes(desc.pad, expect_str, desc.max_length - len);
        }
        expect_str << strptr(tempbuf+offset, len);
        if (desc.strtype == dmmeta_Strtype_strtype_rightpad) {
            char_PrintNTimes(desc.pad, expect_str, desc.max_length - len);
        }
    } else {
        expect_str << before_str;
    }

    strptr stored_str = StoredStr(desc, stringbuf);
    bool str_match = stored_str == expect_str;
    bool string_ok = false;
    i64 stored_num = desc.Geti64(stringbuf,string_ok);
    bool num_match = stored_num == num;
    strptr error;
    if (expect_ok != setnum_ok) {
        error = "SetnumMaybe success mismatch";
    }
    if (elems_N(error) == 0 && !str_match) {
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
        prerr("expect string      "<<Quoted(expect_str)<<", length "<<ch_N(expect_str));
        prerr("expect ok         " << expect_ok);

        prerr("");
        prerr("TEST RESULT");
        prerr("actual string      "<<Quoted(stored_str)<<", length "<<elems_N(stored_str));
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

// -----------------------------------------------------------------------------

// A value outside the numtype range: SetnumMaybe must return false and
// leave the stored bytes exactly as they were.
static void TestOutofrangeNumber(StringDesc &desc, u8 *stringbuf, i64 num) {
    vrfy_(desc.SetnumMaybe(stringbuf, 0));
    tempstr seed_str(StoredStr(desc, stringbuf));
    bool setnum_ok = desc.SetnumMaybe(stringbuf, num);
    tempstr stored_str(StoredStr(desc, stringbuf));
    strptr error;
    if (setnum_ok) {
        error = "SetnumMaybe accepted an out-of-range value";
    } else if (stored_str != seed_str) {
        error = "refused SetnumMaybe modified the stored string";
    }
    if (algo_lib::_db.cmdline.verbose > 1 || elems_N(error) > 0) {
        prerr("");
        prerr("NUMSTR DEFINITION");
        prerr("smallstr           " << desc.smallstr);
        prerr("number tested      " << num);
        prerr("accepted           " << setnum_ok);
        prerr("stored before      "<<Quoted(seed_str)<<", length "<<ch_N(seed_str));
        prerr("stored after       "<<Quoted(stored_str)<<", length "<<ch_N(stored_str));
    }
    if (elems_N(error) > 0) {
        vrfy(0,error);
    }
}

// -----------------------------------------------------------------------------

// One value against one numstr shape: a value inside the numtype range
// must store and read back exactly; a value outside it must be refused
// with the stored bytes left unchanged.
static void TestOneNumber(StringDesc &desc, i64 num) {
    bool inrange = desc.issigned
        ? (num >= desc.numtype_min && num <= i64(desc.numtype_max))
        : (num >= 0 && u64(num) <= desc.numtype_max);
    u8 stringbuf[1024];//
    desc.Init(stringbuf);
    if (inrange) {
        TestInrangeNumber(desc, stringbuf, num);
    } else {
        TestOutofrangeNumber(desc, stringbuf, num);
    }
}

// BASE raised to EXP, saturated at u64max. A boundary beyond u64 (a
// 20-digit base-10 string's capacity is 1e20) still saturates to a value
// past every reachable probe, so the sweep lands on the reachable maximum
static u64 PowSat(u32 base, u32 exp) {
    u64 retval = 1;
    for (u32 i = 0; i < exp; i++) {
        if (retval > 0xffffffffffffffffULL / base) {
            retval = 0xffffffffffffffffULL;
        } else {
            retval *= base;
        }
    }
    return retval;
}

// -----------------------------------------------------------------------------

// V clamped into the i64 range: SetnumMaybe and Geti64 exchange values as
// i64, so a boundary beyond i64max is probed at i64max, the largest value
// the interface can express
static i64 ClampI64(u64 v) {
    return v > 0x7fffffffffffffffULL ? i64(0x7fffffffffffffffULL) : i64(v);
}

// -----------------------------------------------------------------------------

// Test ITER_N consecutive values centered on CENTER, shifting the window
// inward where it would cross the i64 range: every probed value must
// itself be a representable i64, or the loop arithmetic overflows
static void SweepAround(StringDesc &desc, i64 center, int iter_n) {
    i64 i64min = i64(0x8000000000000000ULL);
    i64 i64max = i64(0x7fffffffffffffffULL);
    i64 start = center < i64min + iter_n/2 ? i64min : center - iter_n/2;
    if (start > i64max - (iter_n - 1)) {
        start = i64max - (iter_n - 1);
    }
    for (int i = 0; i < iter_n; i++) {
        TestOneNumber(desc, start + i);
    }
}

// -----------------------------------------------------------------------------

static void StringConversionTest(StringDesc& desc) {
    int iter_n = 10000;
    // the generator-reported range must be exact two's complement: the
    // signed minimum is -(max+1) and the unsigned minimum is 0.
    // TestOneNumber classifies each value against the reported bounds, so
    // a mis-reported minimum would silently shift the in-range split
    if (desc.issigned) {
        vrfy_(desc.numtype_min == -i64(desc.numtype_max) - 1);
    } else {
        vrfy_(desc.numtype_min == 0);
    }
    // one past the largest value the string can hold, and one past the most
    // negative one: integer arithmetic throughout -- pow in double converts
    // back to u64 as undefined behavior once base^length exceeds u64, and a
    // sweep computed from the converted garbage probes around 0 instead of
    // the capacity boundary
    i64 strtype_max = ClampI64(PowSat(desc.base, desc.max_length));
    i64 strtype_min = 0 - ClampI64(PowSat(desc.base, desc.max_length-1));

    // first N integers
    for (int i = 0; i < iter_n; ++i) {
        TestOneNumber(desc, i);
    }

    // numbers around the string capacity boundary
    SweepAround(desc, strtype_max, iter_n);

    // first N negative integers
    for (int i = 0; i < iter_n; ++i) {
        TestOneNumber(desc, -i);
    }

    // numbers around the negative string capacity boundary
    SweepAround(desc, strtype_min, iter_n);

    // numbers around the numtype maximum
    SweepAround(desc, ClampI64(desc.numtype_max), iter_n);

    // numbers around the numtype minimum
    SweepAround(desc, desc.numtype_min, iter_n);
}

// -----------------------------------------------------------------------------

void atf_amc::amctest_TestString() {
    atf_amc::ForAllStrings(&StringConversionTest);
    algo::ForAllStrings(&StringConversionTest);
}

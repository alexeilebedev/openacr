// (C) AlgoEngineering LLC 2008-2012
// (C) 2013-2019 NYSE | Intercontinental Exchange
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
// Target: atf_unit (exe) -- Unit tests (see unittest table)
// Exceptions: yes
// Header: include/atf_unit.h
//
// Created By: alexei.lebedev
// Authors: alexei.lebedev
// Recent Changes: alexei.lebedev
//

#pragma once

#include "include/algo.h"
#include "include/gen/atf_unit_gen.h"
#include "include/gen/atf_unit_gen.inl.h"

// -----------------------------------------------------------------------------

#define DO_PERF_TEST(name,action) {                             \
        u64 start =get_cycles();                                \
        u64 limit = start + atf_unit::_db.perf_cycle_budget;    \
        u64 end = 0;                                            \
        u64    nloops        = 0;                               \
        do {                                                    \
            frep_(loop_iter,1000) {                             \
                action;                                         \
            }                                                   \
            nloops += 1000;                                     \
            end = get_cycles();                                 \
        } while (end<limit);                                    \
        atf_unit::PrintPerfSample(name,nloops,(end-start));     \
    }

// -----------------------------------------------------------------------------

// Evaluate two expressions (possibly with side effects)
// Trigger error if the expressions are not equal
#define TESTCMP(a,b) atf_unit::Testcmp(__FILE__,__LINE__,#a,#b,(a)==(b))

// -----------------------------------------------------------------------------

namespace atf_unit { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/atf/unit/acr.cpp
    //

    // Check selecting a single tuple from file.
    //     (user-implemented function, prototype is in amc-generated header)
    // void unittest_acr_Read1();

    // Check that selecting all tuples from a mixed file
    // reorders them in topological order by ctype (i.e. ctype, then field)
    // void unittest_acr_Read2();

    // Check that selection on non-pkey attribute works
    // void unittest_acr_Read3();

    // Check that -rowid numbers records, but not for relations which are fully sorted
    // void unittest_acr_Rowid1();

    // Check that with -print:N, nothing is shown
    // void unittest_acr_Read5();

    // Test re-writing a single file specified with -in
    // void unittest_acr_Write1();

    // Test re-writing a single file back in tree mode
    // void unittest_acr_Write2();

    // Test that -insert -trunc removes all existing records of a given type
    // upon first insertion
    // void unittest_acr_Insert1();

    // Insert a single record into file, no truncation
    // void unittest_acr_Insert2();

    // Check that acr detects bad references.
    // void unittest_acr_Check1();

    // Check that -unused deselects records that are referred to
    // void unittest_acr_Unused1();

    // Check that -fldfunc expansion operates
    // void unittest_acr_Fldfunc1();

    // Check that selection + -nup selects an appropriate record
    // and not another available record
    // void unittest_acr_Xref1();
    // void unittest_acr_Xref2();
    // void unittest_acr_Field1();

    // Construct regx of matching records
    // Input order is preserved, and dots are escaped (to allow interoperability with perl)
    // void unittest_acr_Regx1();

    // Delete a record from a file
    // void unittest_acr_Del1();

    // Update a non-primary attribute
    // void unittest_acr_Merge1();

    // Replace a record. Unspecified attributes revert to defaults.
    // void unittest_acr_Replace1();

    // Test -meta query
    // -meta loads meta-data from the same source as data, so we can't easily use it on a file,
    // so for this query we use the default data set instead of good old temp/x.
    // void unittest_acr_Meta1();

    // -------------------------------------------------------------------
    // cpp/atf/unit/algo_fmt.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void unittest_algo_lib_FmtBufDec();
    // void unittest_algo_lib_CaseConversion();

    // -------------------------------------------------------------------
    // cpp/atf/unit/algo_lib.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void unittest_algo_lib_PopCnt1();
    // void unittest_algo_lib_PopCnt2();
    // void unittest_algo_lib_TestBitSet();
    // void unittest_algo_lib_TestFbitset();
    // void unittest_algo_lib_TestFbitset2();
    // void unittest_algo_lib_TestFbitset3();
    // void unittest_algo_lib_TestFbitset4();
    // void unittest_algo_lib_DoTestRounding();

    // CPU should only use the lower 5 bits for shifting
    // void unittest_algo_lib_CheckShiftMask();
    // void unittest_algo_lib_Interlocked();
    // void unittest_algo_lib_ReadLine();
    // void unittest_algo_lib_Ceiling();

    //
    // GetCSVToken, GetCSVTokens
    //
    //
    //
    //
    // void unittest_algo_lib_CSVTokens();
    // void unittest_algo_lib_Strfind();
    // void unittest_algo_lib_PrintSsim();

    //
    // Min, Max, Floor, Round, Ceiling, UpdateMin, UpdateMax
    //
    //
    //
    //
    // void unittest_algo_lib_MinMax();
    // void unittest_algo_lib_NToh();
    // void unittest_algo_lib_ParseNumber_Overflow1();
    // void unittest_algo_lib_ParseNumber_Overflow2();
    // void unittest_algo_lib_ParseNumber_Overflow3();
    // void unittest_algo_lib_PrintDoubleWithCommas();
    // void unittest_algo_lib_PrintCppQuoted();
    // void unittest_algo_lib_PrintPad();
    // void unittest_algo_lib_PrintHex();
    // void unittest_algo_lib_TestString();
    // void unittest_algo_lib_TestStringFmt();
    // void unittest_algo_lib_TestStringFmt2();
    // void unittest_algo_lib_TestStringFmt3();
    // void unittest_algo_lib_Smallstr();
    // void unittest_algo_lib_StringIter();
    // void unittest_algo_lib_test_strptr();

    // The reasons why not to use library calls to setup fixtures -
    // 1) Test condition should be "clean" as much as possible and
    // 2) library under test may interfere test results
    // void unittest_algo_lib_ParseOct1();
    // void unittest_algo_lib_ParseHex1();
    // void unittest_algo_lib_ParseOct3();
    // void unittest_algo_lib_ParseHex2();
    // void unittest_algo_lib_UnescapeC();
    // void unittest_algo_lib_ParseURL1();
    // void unittest_algo_lib_PerfParseNum();
    // void unittest_algo_lib_DirBeg();
    // void unittest_algo_lib_RemDirRecurse();
    // void unittest_algo_lib_RemDirRecurse1();
    // void unittest_algo_lib_RemDirRecurse2();
    // void unittest_algo_lib_RemDirRecurse3();
    // void unittest_algo_lib_RemDirRecurse4();
    // void unittest_algo_lib_ReadModuleId();
    // void unittest_algo_lib_Tempfile();
    // void unittest_algo_lib_NextSep();
    // void unittest_algo_lib_I32Dec3Fmt();
    // void unittest_algo_lib_OrderID();
    // void unittest_algo_lib_IntPrice();
    // void unittest_algo_lib_Keyval();
    // void unittest_algo_lib_StringToFile();
    // void unittest_algo_lib_U128PrintHex();
    // void unittest_algo_lib_FileToString();
    // void unittest_algo_lib_CheckIpmask();
    // void unittest_algo_lib_TimeConstants();
    // void unittest_algo_lib_Datecache();
    // void unittest_algo_lib_Cmp();
    // void unittest_algo_lib_SchedTime();
    // void unittest_algo_lib_StringSubrange();
    // void unittest_algo_lib_Clipped();
    // void unittest_algo_lib_Abs();
    // void unittest_algo_lib_PerfMinMaxAvg();
    // void unittest_algo_lib_PerfIntrinsics();
    // void unittest_algo_lib_PerfTruncVsFtol();
    // void unittest_algo_lib_PerfParseDouble();
    // void unittest_algo_lib_PerfSort();
    // void unittest_algo_lib_Replscope();
    // void unittest_algo_lib_AvlvsMap();
    // void unittest_algo_lib_Sleep();
    // void unittest_algo_lib_strptr_Eq();
    // void unittest_algo_lib_SysEval();
    // void unittest_algo_lib_TrimZerosRight();
    // void unittest_algo_lib_PrintWithCommas();

    // -------------------------------------------------------------------
    // cpp/atf/unit/bash.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void unittest_algo_lib_PrintBash();

    // -------------------------------------------------------------------
    // cpp/atf/unit/charset.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void unittest_algo_lib_Charset();

    // -------------------------------------------------------------------
    // cpp/atf/unit/lib_exec.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void unittest_lib_exec_Parallel1();
    // void unittest_lib_exec_TooManyFds();
    // void unittest_lib_exec_Timeout();
    // void unittest_lib_exec_Dependency();

    // -------------------------------------------------------------------
    // cpp/atf/unit/lib_json.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void unittest_lib_json_Empty();
    // void unittest_lib_json_TokenNull();
    // void unittest_lib_json_TokenFalse();
    // void unittest_lib_json_TokenTrue();
    // void unittest_lib_json_SimpleNumber();
    // void unittest_lib_json_SimpleString();
    // void unittest_lib_json_EmptyArray();
    // void unittest_lib_json_Emptyobject();
    // void unittest_lib_json_SecString();
    // void unittest_lib_json_NumberZero();
    // void unittest_lib_json_NumberManyDigits();
    // void unittest_lib_json_NumberDecimal();
    // void unittest_lib_json_NumberExponent1();
    // void unittest_lib_json_NumberExponent2();
    // void unittest_lib_json_NumberCombined1();
    // void unittest_lib_json_NumberCombined2();
    // void unittest_lib_json_StringEmpty();
    // void unittest_lib_json_StringWithEscapes();
    // void unittest_lib_json_ObjFieldTokenNull();
    // void unittest_lib_json_ObjFieldTokenFalse();
    // void unittest_lib_json_ObjFieldTokenTrue();
    // void unittest_lib_json_ObjFieldSimpleNumber();
    // void unittest_lib_json_ObjFieldSimpleString();
    // void unittest_lib_json_ObjFieldEmptyArray();
    // void unittest_lib_json_ObjFieldEmptyObject();
    // void unittest_lib_json_ObjFieldAll();
    // void unittest_lib_json_ArrayTokenNull();
    // void unittest_lib_json_ArrayTokenFalse();
    // void unittest_lib_json_ArrayTokenTrue();
    // void unittest_lib_json_ArraySimpleNumber();
    // void unittest_lib_json_ArraySimpleString();
    // void unittest_lib_json_ArrayEmptyArray();
    // void unittest_lib_json_ArrayEmptyObject();
    // void unittest_lib_json_ArrayAll();
    // void unittest_lib_json_Typical();
    // void unittest_lib_json_CtrlCharEscape();
    // void unittest_lib_json_ErrorBadToken1();
    // void unittest_lib_json_ErrorBadToken2();
    // void unittest_lib_json_ErrorBadNumber();
    // void unittest_lib_json_ErrorBadString1();
    // void unittest_lib_json_ErrorBadString2();
    // void unittest_lib_json_ErrorBadString3();
    // void unittest_lib_json_ErrorBadUString1();
    // void unittest_lib_json_ErrorBadUString2();
    // void unittest_lib_json_ErrorBadUString3();
    // void unittest_lib_json_ErrorBadUString4();
    // void unittest_lib_json_ErrorBadUString5();
    // void unittest_lib_json_ErrorBadUString6();
    // void unittest_lib_json_ErrorBadUString7();
    // void unittest_lib_json_ErrorBrMismatch1();
    // void unittest_lib_json_ErrorBrMismatch2();
    // void unittest_lib_json_ErrorBrMismatch3();
    // void unittest_lib_json_ErrorBrMismatch4();
    // void unittest_lib_json_ErrorBrMismatch5();
    // void unittest_lib_json_ErrorBrMismatch6();
    // void unittest_lib_json_ErrorBrMismatch7();
    // void unittest_lib_json_ErrorBrMismatch8();
    // void unittest_lib_json_ErrorBrMismatch9();
    // void unittest_lib_json_ErrorBrMismatch10();
    // void unittest_lib_json_ErrorBrMismatch11();
    // void unittest_lib_json_ErrorBrMismatch12();
    // void unittest_lib_json_ErrorBrMismatch13();
    // void unittest_lib_json_ErrorBrMismatch14();
    // void unittest_lib_json_ErrorArrayComma1();
    // void unittest_lib_json_ErrorArrayComma2();
    // void unittest_lib_json_ErrorArrayComma3();
    // void unittest_lib_json_ErrorArrayComma4();
    // void unittest_lib_json_ErrorObjectComma1();
    // void unittest_lib_json_ErrorObjectComma2();
    // void unittest_lib_json_ErrorObjectComma3();
    // void unittest_lib_json_ErrorObjectComma4();
    // void unittest_lib_json_ErrorBareComma();
    // void unittest_lib_json_ErrorBareValuesWithComma();
    // void unittest_lib_json_ErrorObjectNoValue();
    // void unittest_lib_json_ErrorObjectColon1();
    // void unittest_lib_json_ErrorObjectColon2();
    // void unittest_lib_json_ErrorObjectColon3();
    // void unittest_lib_json_ErrorObjectColon4();
    // void unittest_lib_json_ErrorObjectColon5();
    // void unittest_lib_json_ErrorBareColon();
    // void unittest_lib_json_ErrorArrayColon();
    // void unittest_lib_json_ErrorBareValuesWithColon();
    // void unittest_lib_json_ErrorObjectDupField();
    // void unittest_lib_json_FmtJson_u64_0();
    // void unittest_lib_json_FmtJson_u64_max();
    // void unittest_lib_json_FmtJson_u32_0();
    // void unittest_lib_json_FmtJson_u32_max();
    // void unittest_lib_json_FmtJson_u16_0();
    // void unittest_lib_json_FmtJson_u16_max();
    // void unittest_lib_json_FmtJson_u8_0();
    // void unittest_lib_json_FmtJson_u8_max();
    // void unittest_lib_json_FmtJson_i64_min();
    // void unittest_lib_json_FmtJson_i64_max();
    // void unittest_lib_json_FmtJson_i32_min();
    // void unittest_lib_json_FmtJson_i32_max();
    // void unittest_lib_json_FmtJson_i16_min();
    // void unittest_lib_json_FmtJson_i16_max();
    // void unittest_lib_json_FmtJson_i8_min();
    // void unittest_lib_json_FmtJson_i8_max();
    // void unittest_lib_json_FmtJson_double_prec();
    // void unittest_lib_json_FmtJson_float_prec();
    // void unittest_lib_json_FmtJson_bool_true();
    // void unittest_lib_json_FmtJson_bool_false();
    // void unittest_lib_json_FmtJson_char();
    // void unittest_lib_json_FmtJson_TypeA();
    // void unittest_lib_json_FmtJson_Object();

    // -------------------------------------------------------------------
    // cpp/atf/unit/lib_sql.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void unittest_lib_sql_Main();

    // -------------------------------------------------------------------
    // cpp/atf/unit/line.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void unittest_algo_lib_FileLine_curs();

    // -------------------------------------------------------------------
    // cpp/atf/unit/lockfile.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void unittest_algo_lib_Lockfile();

    // -------------------------------------------------------------------
    // cpp/atf/unit/main.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void unittest_amc_Unit();
    void Main();

    // usage:
    // DO_PERF_TEST("Testing XYZ",xyz());
    // The expression will be evaluated for 2 seconds, after which average speed will be printed.
    void PrintPerfSample(const strptr& action, u64 nloops, u64 clocks);
    void Testcmp(const char *file, int line, strptr value, strptr expect, bool eq);
    void Testcmp(const char *file, int line, const char *value, const char *expect, bool eq);

    // -------------------------------------------------------------------
    // cpp/atf/unit/parsenum.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void unittest_algo_lib_ParseNumber();

    // -------------------------------------------------------------------
    // cpp/atf/unit/regx.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void unittest_algo_lib_Regx();

    // Test that matching a huge string with a regex that
    // ends in .* is fast.
    // void unittest_algo_lib_RegxShortCircuit();
    // void unittest_algo_lib_RegxReadTwice();
    // void unittest_algo_lib_RegxReadTwice2();

    // -------------------------------------------------------------------
    // cpp/atf/unit/string.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void unittest_algo_lib_SubstringIndex();
    // void unittest_algo_lib_Aligned();
    // void unittest_algo_lib_CString();
    // void unittest_algo_lib_StringFind();
    // void unittest_algo_lib_StringCase();
    // void unittest_algo_lib_Tabulate();

    // -------------------------------------------------------------------
    // cpp/atf/unit/time.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void unittest_algo_lib_TimeConversion();
    // void unittest_algo_lib_PrintTime();
    // void unittest_algo_lib_ParseUnTime();
    // void unittest_algo_lib_DayName();

    // -------------------------------------------------------------------
    // cpp/atf/unit/tuple.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void unittest_algo_lib_Argtuple_ReadArgv();
    // void unittest_algo_lib_Argtuple_ReadStrptr();
    // void unittest_algo_lib_Argtuple2();
    // void unittest_algo_lib_Tuple1();
    // void unittest_algo_lib_Tuple2();

    // Check Attr_curs
    // void unittest_algo_lib_Tuple();
}

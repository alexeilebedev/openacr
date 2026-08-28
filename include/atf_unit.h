// Copyright (C) 2023-2024,2026 AlgoRND
// Copyright (C) 2020-2023 Astra
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2008-2012 AlgoEngineering LLC
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
// Target: atf_unit (exe) -- Unit tests (see unittest table)
// Exceptions: yes
// Header: include/atf_unit.h
//

#pragma once

#include "include/algo.h"
#include "include/gen/atf_unit_gen.h"
#include "include/gen/atf_unit_gen.inl.h"

// -----------------------------------------------------------------------------

#define DO_PERF_TEST(name,action) {                             \
        u64 start =algo::get_cycles();                          \
        u64 limit = start + atf_unit::_db.perf_cycle_budget;    \
        u64 end = 0;                                            \
        u64    nloops        = 0;                               \
        do {                                                    \
            frep_(loop_iter,1000) {                             \
                action;                                         \
            }                                                   \
            nloops += 1000;                                     \
            end = algo::get_cycles();                           \
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
    // cpp/atf_unit/acr.cpp
    //

    // Check selecting a single tuple from file.
    //     (user-implemented function, prototype is in amc-generated header)
    // void unittest_acr_Read1(); // gstatic/atfdb.unittest:acr.Read1

    // Check that selecting all tuples from a mixed file
    // reorders them in topological order by ctype (i.e. ctype, then field)
    // void unittest_acr_Read2(); // gstatic/atfdb.unittest:acr.Read2

    // Check that selection on non-pkey attribute works
    // void unittest_acr_Read3(); // gstatic/atfdb.unittest:acr.Read3

    // Check that -rowid numbers records, but not for relations which are fully sorted
    // void unittest_acr_Rowid1(); // gstatic/atfdb.unittest:acr.Rowid1

    // Check that with -print:N, nothing is shown
    // void unittest_acr_Read5(); // gstatic/atfdb.unittest:acr.Read5

    // Test re-writing a single file specified with -in
    // void unittest_acr_Write1(); // gstatic/atfdb.unittest:acr.Write1

    // Test re-writing a single file back in tree mode
    // void unittest_acr_Write2(); // gstatic/atfdb.unittest:acr.Write2

    // Test that -insert -trunc removes all existing records of a given type
    // upon first insertion
    // void unittest_acr_Insert1(); // gstatic/atfdb.unittest:acr.Insert1

    // Insert a single record into file, no truncation
    // void unittest_acr_Insert2(); // gstatic/atfdb.unittest:acr.Insert2

    // Check that acr detects bad references.
    // void unittest_acr_Check1(); // gstatic/atfdb.unittest:acr.Check1

    // Check that -unused deselects records that are referred to
    // void unittest_acr_Unused1(); // gstatic/atfdb.unittest:acr.Unused1

    // Check that -fldfunc expansion operates
    // void unittest_acr_Fldfunc1(); // gstatic/atfdb.unittest:acr.Fldfunc1

    // Check that selection + -nup selects an appropriate record
    // and not another available record
    // void unittest_acr_Xref1(); // gstatic/atfdb.unittest:acr.Xref1
    // void unittest_acr_Xref2(); // gstatic/atfdb.unittest:acr.Xref2
    // void unittest_acr_Field1(); // gstatic/atfdb.unittest:acr.Field1

    // Construct regx of matching records
    // Input order is preserved, and dots are escaped (to allow interoperability with perl)
    // void unittest_acr_Regx1(); // gstatic/atfdb.unittest:acr.Regx1

    // Delete a record from a file
    // void unittest_acr_Del1(); // gstatic/atfdb.unittest:acr.Del1

    // Update a non-primary attribute
    // void unittest_acr_Merge1(); // gstatic/atfdb.unittest:acr.Merge1

    // Replace a record. Unspecified attributes revert to defaults.
    // void unittest_acr_Replace1(); // gstatic/atfdb.unittest:acr.Replace1

    // -------------------------------------------------------------------
    // cpp/atf_unit/algo_fmt.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void unittest_algo_lib_FmtBufDec(); // gstatic/atfdb.unittest:algo_lib.FmtBufDec
    // void unittest_algo_lib_CaseConversion(); // gstatic/atfdb.unittest:algo_lib.CaseConversion
    // void unittest_algo_FileFlags(); // gstatic/atfdb.unittest:algo.FileFlags
    // void unittest_algo_Base64(); // gstatic/atfdb.unittest:algo.Base64
    // void unittest_algo_ReadBase64(); // gstatic/atfdb.unittest:algo.ReadBase64
    // void unittest_algo_lib_PrintUuid(); // gstatic/atfdb.unittest:algo_lib.PrintUuid
    // void unittest_algo_lib_ReadUuid(); // gstatic/atfdb.unittest:algo_lib.ReadUuid

    // strptr formatters in fmt.cpp: SQL/XML/CPP/DOT/URI/TeX quoting, padding,
    // thousands separators, case copy, trailing-zero trim.
    // void unittest_algo_StrptrPrintFmt(); // gstatic/atfdb.unittest:algo.StrptrPrintFmt

    // Number/quantity formatters in fmt.cpp: hex, base32, 128-bit, pointer,
    // percent, comma grouping, ranges, and the scaled Hz/nsec helpers driven
    // through the merged unit tables.
    // void unittest_algo_NumPrintFmt(); // gstatic/atfdb.unittest:algo.NumPrintFmt

    // Numeric parsers in fmt.cpp: packed-digit fast parse, octal, counts, and
    // the suffix parsers exercising the merged unit tables (longest-suffix and
    // case-alias matching).
    // void unittest_algo_ParseNumFmt(); // gstatic/atfdb.unittest:algo.ParseNumFmt

    // Print/Read round-trips for the time and diff types in fmt.cpp.  Compare
    // parsed values (not text) so the check is independent of the exact format.
    // void unittest_algo_TimeRoundtrip(); // gstatic/atfdb.unittest:algo.TimeRoundtrip

    // Print/Read round-trips for URL and Ipmask in fmt.cpp.
    // void unittest_algo_UrlIpmaskFmt(); // gstatic/atfdb.unittest:algo.UrlIpmaskFmt

    // An integer range list round-trips through its text form; a span is inclusive
    // at both ends where the struct behind it is half-open; and the order the list
    // states is the order it keeps.
    // void unittest_algo_lib_RangeAryList(); // gstatic/atfdb.unittest:algo_lib.RangeAryList

    // The argv split reads a token by scanning to the next break character, and
    // the ssim break set is "[]{}()\t \r\n:".  A brace therefore ends a token
    // without being read, and a scan that only ever ends tokens never gets past
    // one: a bare "{" spun forever, taking x2cli with it, until each such
    // character became a word of its own.
    // void unittest_algo_CmdlineToArgv(); // gstatic/atfdb.unittest:algo.CmdlineToArgv

    // -------------------------------------------------------------------
    // cpp/atf_unit/algo_lib.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void unittest_algo_lib_PopCnt1(); // gstatic/atfdb.unittest:algo_lib.PopCnt1
    // void unittest_algo_lib_PopCnt2(); // gstatic/atfdb.unittest:algo_lib.PopCnt2
    // void unittest_algo_lib_DoTestRounding(); // gstatic/atfdb.unittest:algo_lib.DoTestRounding

    // CPU should only use the lower 5 bits for shifting
    // void unittest_algo_lib_CheckShiftMask(); // gstatic/atfdb.unittest:algo_lib.CheckShiftMask
    // void unittest_algo_lib_ReadLine(); // gstatic/atfdb.unittest:algo_lib.ReadLine
    // void unittest_algo_lib_Ceiling(); // gstatic/atfdb.unittest:algo_lib.Ceiling

    //
    // GetCSVToken, GetCSVTokens
    //
    //
    //
    //
    // void unittest_algo_lib_CSVTokens(); // gstatic/atfdb.unittest:algo_lib.CSVTokens
    // void unittest_algo_lib_Strfind(); // gstatic/atfdb.unittest:algo_lib.Strfind
    // void unittest_algo_lib_PrintSsim(); // gstatic/atfdb.unittest:algo_lib.PrintSsim

    //
    // Min, Max, Floor, Round, Ceiling, UpdateMin, UpdateMax
    //
    //
    //
    //
    // void unittest_algo_lib_MinMax(); // gstatic/atfdb.unittest:algo_lib.MinMax
    // void unittest_algo_lib_NToh(); // gstatic/atfdb.unittest:algo_lib.NToh
    // void unittest_algo_lib_ParseNumber_Overflow1(); // gstatic/atfdb.unittest:algo_lib.ParseNumber_Overflow1
    // void unittest_algo_lib_ParseNumber_Overflow2(); // gstatic/atfdb.unittest:algo_lib.ParseNumber_Overflow2
    // void unittest_algo_lib_ParseNumber_Overflow3(); // gstatic/atfdb.unittest:algo_lib.ParseNumber_Overflow3
    // void unittest_algo_lib_PrintDoubleWithCommas(); // gstatic/atfdb.unittest:algo_lib.PrintDoubleWithCommas
    // void unittest_algo_lib_PrintCppQuoted(); // gstatic/atfdb.unittest:algo_lib.PrintCppQuoted
    // void unittest_algo_lib_PrintPad(); // gstatic/atfdb.unittest:algo_lib.PrintPad
    // void unittest_algo_lib_PrintHex(); // gstatic/atfdb.unittest:algo_lib.PrintHex
    // void unittest_algo_lib_TestString(); // gstatic/atfdb.unittest:algo_lib.TestString
    // void unittest_algo_lib_TestStringFmt(); // gstatic/atfdb.unittest:algo_lib.TestStringFmt
    // void unittest_algo_lib_TestStringFmt2(); // gstatic/atfdb.unittest:algo_lib.TestStringFmt2
    // void unittest_algo_lib_TestStringFmt3(); // gstatic/atfdb.unittest:algo_lib.TestStringFmt3
    // void unittest_algo_lib_Smallstr(); // gstatic/atfdb.unittest:algo_lib.Smallstr
    // void unittest_algo_lib_StringIter(); // gstatic/atfdb.unittest:algo_lib.StringIter
    // void unittest_algo_lib_test_strptr(); // gstatic/atfdb.unittest:algo_lib.test_strptr

    // The reasons why not to use library calls to setup fixtures -
    // 1) Test condition should be "clean" as much as possible and
    // 2) library under test may interfere test results
    // void unittest_algo_lib_ParseOct1(); // gstatic/atfdb.unittest:algo_lib.ParseOct1
    // void unittest_algo_lib_ParseHex1(); // gstatic/atfdb.unittest:algo_lib.ParseHex1
    // void unittest_algo_lib_ParseOct3(); // gstatic/atfdb.unittest:algo_lib.ParseOct3
    // void unittest_algo_lib_ParseHex2(); // gstatic/atfdb.unittest:algo_lib.ParseHex2
    // void unittest_algo_lib_UnescapeC(); // gstatic/atfdb.unittest:algo_lib.UnescapeC
    // void unittest_algo_lib_ParseURL1(); // gstatic/atfdb.unittest:algo_lib.ParseURL1
    // void unittest_algo_lib_PerfParseNum(); // gstatic/atfdb.unittest:algo_lib.PerfParseNum
    // void unittest_algo_lib_DirBeg(); // gstatic/atfdb.unittest:algo_lib.DirBeg
    // void unittest_algo_lib_RemDirRecurse(); // gstatic/atfdb.unittest:algo_lib.RemDirRecurse
    // void unittest_algo_lib_ReadModuleId(); // gstatic/atfdb.unittest:algo_lib.ReadModuleId
    // void unittest_algo_lib_Tempfile(); // gstatic/atfdb.unittest:algo_lib.Tempfile
    // void unittest_algo_lib_NextSep(); // gstatic/atfdb.unittest:algo_lib.NextSep
    // void unittest_algo_lib_I32Dec3Fmt(); // gstatic/atfdb.unittest:algo_lib.I32Dec3Fmt
    // void unittest_algo_lib_OrderID(); // gstatic/atfdb.unittest:algo_lib.OrderID

    // DecodeNBytes and DecodeNChars take a length that always comes from the wire:
    // a protobuf field header carrying the varint 0xFFFFFFFF, a kafka record whose
    // signed varint length decodes to -1. Such a length must be refused, and the
    // refusal has to hold for every integer type a caller can hand over -- the
    // callers spell the length u16, u32, i32 and i64, and each of those spans a
    // different set of values that no buffer can satisfy.
    //
    // The length is therefore compared as a signed 64-bit count, which every
    // caller's type converts into without changing value, and the accepted range is
    // stated on both ends: at least zero and at most the number of bytes left in
    // the buffer. A negative length has no reading as a byte count, and a length
    // above the remaining size cannot be served. A rejected call leaves the buffer
    // where it was and the result empty, so a caller that ignores the return value
    // still sees no fabricated payload.
    // void unittest_algo_lib_DecodeNRange(); // gstatic/atfdb.unittest:algo_lib.DecodeNRange

    // A varint arrives as a sequence of seven-bit groups and is accumulated into a
    // fixed-width integer, so the decoder has to answer what happens when the
    // encoding names a value the accumulator cannot hold. A protobuf length field
    // carrying the five bytes 83 80 80 80 10 names 4294967299 bytes of payload;
    // keeping only the low 32 bits of that leaves 3, and a decode reported as
    // successful then hands the caller the three-byte payload of a message that
    // declared four gigabytes.
    //
    // The general condition is that a group's bits have to survive the shift that
    // places them. A group shifted by the accumulator's width or more contributes
    // nothing, and a group whose top bits fall off the end contributes only part of
    // itself; in both cases the encoding names a value the accumulator does not
    // hold, and the decode of the whole varint is refused.
    //
    // Each decoder therefore walks groups only while the shift stays inside its own
    // width -- five bytes for u32, ten for u64 -- and only while the group under
    // that shift keeps all its bits. A varint that runs past the width, and one the
    // buffer never terminates, both leave the buffer where it was and the result
    // zero. The zigzag decoders read through the same two accumulators and answer
    // the same table.
    // void unittest_algo_lib_DecodeVLCLERange(); // gstatic/atfdb.unittest:algo_lib.DecodeVLCLERange

    // The Dec reader accumulates the digits into a u64 and then stores the
    // result in the field's own type. A string whose scaled value exceeds that
    // type must be refused: 700.00 in a u16-backed field with two implied
    // places scales to 70000, and storing it would wrap to 4464 -- a read that
    // returns true after silently changing the value. A field as wide as the
    // accumulator is exposed to the same failure one level up: the scaled value
    // of 495765611597143987 with two implied places is 49576561159714398700,
    // which does not fit a u64, and the top of the range must still read. The
    // signed field is bounded by the same magnitude, so its own maximum reads
    // and one unit past it is refused.
    //
    // A signed field's negative range reaches one unit further than its positive
    // range, and the reader admits that unit: value_Print of an i32-backed field
    // with one implied place at the type minimum emits -214748364.8, and reading
    // that string back has to return the same value, or a record that printed
    // cannot be loaded. Digits past the field's scale are dropped rather than
    // rounded, so -214748364.85 reads as the minimum too, while a magnitude one
    // unit further down is refused. An unsigned field has no negative range at
    // all and refuses any leading minus.
    // void unittest_algo_lib_DecReadRange(); // gstatic/atfdb.unittest:algo_lib.DecReadRange

    // SetDoubleMaybe stores the value rounded to the nearest scaled integer, so
    // the bound it checks is the range the rounded value must land in, half a
    // unit wider than the field's own range at each end: 214748364.7 scales to
    // exactly 2147483647, the top of an i32-backed field with one implied place,
    // and must store, while a value half a unit further up rounds past the top
    // and is refused. The bottom of the range grants the same allowance, so a
    // value that rounds up to the field's minimum stores and one that rounds
    // below it is refused. A field whose type is as wide as the double's own
    // integer reach is refused at the point where the scaled value reaches the
    // type's magnitude, so no out-of-range double is ever stored.
    // void unittest_algo_lib_DecSetDoubleRange(); // gstatic/atfdb.unittest:algo_lib.DecSetDoubleRange
    // void unittest_algo_lib_IntPrice(); // gstatic/atfdb.unittest:algo_lib.IntPrice
    // void unittest_algo_lib_Keyval(); // gstatic/atfdb.unittest:algo_lib.Keyval
    // void unittest_algo_lib_StringToFile(); // gstatic/atfdb.unittest:algo_lib.StringToFile

    // SafeStringToFile stages the new contents in a mkstemp tempfile beside the
    // target, then renames it over the target. A failed rename -- forced here by
    // making the target an existing directory -- must not leave the tempfile
    // behind: each failed save would otherwise leak one <target>-XXXXXX file
    // beside the target. Verify the call reports failure and removes the tempfile.
    // void unittest_algo_lib_SsfRenameFail(); // gstatic/atfdb.unittest:algo_lib.SsfRenameFail
    // void unittest_algo_lib_U128PrintHex(); // gstatic/atfdb.unittest:algo_lib.U128PrintHex
    // void unittest_algo_lib_FileToString(); // gstatic/atfdb.unittest:algo_lib.FileToString
    // void unittest_algo_lib_CheckIpmask(); // gstatic/atfdb.unittest:algo_lib.CheckIpmask
    // void unittest_algo_lib_TimeConstants(); // gstatic/atfdb.unittest:algo_lib.TimeConstants
    // void unittest_algo_lib_Datecache(); // gstatic/atfdb.unittest:algo_lib.Datecache
    // void unittest_algo_lib_Cmp(); // gstatic/atfdb.unittest:algo_lib.Cmp
    // void unittest_algo_lib_SchedTime(); // gstatic/atfdb.unittest:algo_lib.SchedTime
    // void unittest_algo_lib_StringSubrange(); // gstatic/atfdb.unittest:algo_lib.StringSubrange
    // void unittest_algo_lib_Clipped(); // gstatic/atfdb.unittest:algo_lib.Clipped
    // void unittest_algo_lib_Abs(); // gstatic/atfdb.unittest:algo_lib.Abs
    // void unittest_algo_lib_PerfMinMaxAvg(); // gstatic/atfdb.unittest:algo_lib.PerfMinMaxAvg
    // void unittest_algo_lib_PerfIntrinsics(); // gstatic/atfdb.unittest:algo_lib.PerfIntrinsics
    // void unittest_algo_lib_PerfTruncVsFtol(); // gstatic/atfdb.unittest:algo_lib.PerfTruncVsFtol
    // void unittest_algo_lib_PerfParseDouble(); // gstatic/atfdb.unittest:algo_lib.PerfParseDouble
    // void unittest_algo_lib_PerfSort(); // gstatic/atfdb.unittest:algo_lib.PerfSort
    // void unittest_algo_lib_Replscope(); // gstatic/atfdb.unittest:algo_lib.Replscope
    // void unittest_algo_lib_ReplscopeSharedPrefix(); // gstatic/atfdb.unittest:algo_lib.ReplscopeSharedPrefix
    // void unittest_algo_lib_AvlvsMap(); // gstatic/atfdb.unittest:algo_lib.AvlvsMap
    // void unittest_algo_lib_Sleep(); // gstatic/atfdb.unittest:algo_lib.Sleep
    // void unittest_algo_lib_strptr_Eq(); // gstatic/atfdb.unittest:algo_lib.strptr_Eq
    // void unittest_algo_lib_SysEval(); // gstatic/atfdb.unittest:algo_lib.SysEval
    // void unittest_algo_lib_TrimZerosRight(); // gstatic/atfdb.unittest:algo_lib.TrimZerosRight
    // void unittest_algo_lib_PrintWithCommas(); // gstatic/atfdb.unittest:algo_lib.PrintWithCommas
    // void unittest_algo_lib_FTruncate(); // gstatic/atfdb.unittest:algo_lib.FTruncate
    // void unittest_algo_lib_GetCpuHz(); // gstatic/atfdb.unittest:algo_lib.GetCpuHz
    // void unittest_algo_lib_flock(); // gstatic/atfdb.unittest:algo_lib.flock
    // void unittest_algo_lib_u128(); // gstatic/atfdb.unittest:algo_lib.u128
    // void unittest_algo_lib_Mmap(); // gstatic/atfdb.unittest:algo_lib.Mmap
    // void unittest_algo_lib_FileQ(); // gstatic/atfdb.unittest:algo_lib.FileQ

    // _Exec returns the raw wait status; ExportWaitStatus decomposes it into the
    // process's exit facts: exit_code carries the shell-convention code (exit N
    // -> N, death by signal -> 128+signal), exit_signal carries the terminating
    // signal alone, 0 when the child exited.
    // void unittest_algo_lib_ExitCode(); // gstatic/atfdb.unittest:algo_lib.ExitCode
    // void unittest_algo_lib_KillRecurse(); // gstatic/atfdb.unittest:algo_lib.KillRecurse

    // check that all characters print from memptr, and get parsed
    // back as a string
    // void unittest_algo_lib_PrintMemptr(); // gstatic/atfdb.unittest:algo_lib.PrintMemptr
    bool Smallstr150_Eq(const algo::Smallstr150 & lhs,const algo::Smallstr150 & rhs);
    //     (user-implemented function, prototype is in amc-generated header)
    // void unittest_algo_lib_SmallstrEq(); // gstatic/atfdb.unittest:algo_lib.SmallstrEq
    // void unittest_algo_lib_ReverseBits(); // gstatic/atfdb.unittest:algo_lib.ReverseBits
    // void unittest_algo_lib_Zigzag(); // gstatic/atfdb.unittest:algo_lib.Zigzag

    // Test that a file opened in append mode always writes at the end
    // regardless of file position
    // void unittest_algo_lib_FileAppend(); // gstatic/atfdb.unittest:algo_lib.FileAppend
    // void unittest_algo_lib_Url(); // gstatic/atfdb.unittest:algo_lib.Url
    // void unittest_algo_lib_Blkpool(); // gstatic/atfdb.unittest:algo_lib.Blkpool
    // void unittest_algo_lib_FProc(); // gstatic/atfdb.unittest:algo_lib.FProc

    // Pump a bit of data through cat/tee and verify it round-trips through every
    // amc-generated _proc pipe redirect: stdin, stdout, stderr, merged stdout+stderr
    // (fstdout="|" + fstderr=">&1"), and all three at once.
    // Source of truth: amc::tfunc_Exec_Start in cpp/amc/exec.cpp.
    // void unittest_algo_lib_ExecPipe(); // gstatic/atfdb.unittest:algo_lib.ExecPipe

    // The calibration this process runs on came from the kernel: where the
    // kernel exports a TSC rate, algo_lib::_db.hz is that exact figure, and
    // RequireKernelCpuHz admits the process.  Exact equality is the point of
    // the test -- a rate derived any other way (timed against a wall clock, or
    // read off a P-state file) lands near the kernel's figure rather than on
    // it, so an approximate comparison would pass for the very values the
    // kernel export exists to exclude.
    //
    // A host with no export has nothing to compare against, so the check is
    // announced as skipped rather than passing silently.  That host is exactly
    // where a process that needs the rate refuses to start unless it is stated, and
    // where the file source under /etc is exercised -- installing that file
    // needs root, so it is covered on such a host rather than here.  The second
    // half of the test covers the variable on either kind of host:
    // where the export exists it must be ignored, and where it does not it must be
    // installed exactly as written.  The rate used is 2.5GHz, a figure inside the
    // range ApplyCpuHz admits and unequal to any host's real rate, so a variable
    // that was silently ignored cannot pass for one that was honored.  The
    // process runs on the restored calibration afterwards, so the original is put
    // back before the test returns.
    // void unittest_algo_lib_RequireKernelCpuHz(); // gstatic/atfdb.unittest:algo_lib.RequireKernelCpuHz

    // -------------------------------------------------------------------
    // cpp/atf_unit/algo_txttbl.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void unittest_algo_lib_Txttbl(); // gstatic/atfdb.unittest:algo_lib.Txttbl

    // -------------------------------------------------------------------
    // cpp/atf_unit/bash.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void unittest_algo_lib_PrintBash(); // gstatic/atfdb.unittest:algo_lib.PrintBash

    // -------------------------------------------------------------------
    // cpp/atf_unit/charset.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void unittest_algo_lib_Charset(); // gstatic/atfdb.unittest:algo_lib.Charset

    // -------------------------------------------------------------------
    // cpp/atf_unit/decimal.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void unittest_algo_lib_Decimal(); // gstatic/atfdb.unittest:algo_lib.Decimal

    // -------------------------------------------------------------------
    // cpp/atf_unit/lib_exec.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void unittest_lib_exec_Parallel1(); // gstatic/atfdb.unittest:lib_exec.Parallel1
    // void unittest_lib_exec_TooManyFds(); // gstatic/atfdb.unittest:lib_exec.TooManyFds
    // void unittest_lib_exec_Timeout(); // gstatic/atfdb.unittest:lib_exec.Timeout
    // void unittest_lib_exec_Dependency(); // gstatic/atfdb.unittest:lib_exec.Dependency

    // Verify amc-generated _ToArgv forwards -debug to subprocess argv with one fewer
    // level, matching how -verbose is already forwarded. Source of truth: the loop
    // emitted by amc::tfunc_Exec_ToArgv in cpp/amc/exec.cpp.
    // void unittest_lib_exec_DebugForward(); // gstatic/atfdb.unittest:lib_exec.DebugForward

    // Verify amc-generated _ToArgv picks -name:value vs -name value based on whether
    // the wrapped command has a ccmdline. acr_ed has one (amc-built; its ReadArgv
    // parses colon syntax); bash does not (external tool; needs two-token form).
    // Source of truth: the branch on cmdtype.c_ccmdline in amc::tfunc_Exec_ToArgv
    // in cpp/amc/exec.cpp.
    // void unittest_lib_exec_ExecToArgvSyntax(); // gstatic/atfdb.unittest:lib_exec.ExecToArgvSyntax
    // void unittest_lib_exec_PtyIn(); // gstatic/atfdb.unittest:lib_exec.PtyIn

    // -------------------------------------------------------------------
    // cpp/atf_unit/lib_json.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void unittest_lib_json_Empty(); // gstatic/atfdb.unittest:lib_json.Empty
    // void unittest_lib_json_TokenNull(); // gstatic/atfdb.unittest:lib_json.TokenNull
    // void unittest_lib_json_TokenFalse(); // gstatic/atfdb.unittest:lib_json.TokenFalse
    // void unittest_lib_json_TokenTrue(); // gstatic/atfdb.unittest:lib_json.TokenTrue
    // void unittest_lib_json_SimpleNumber(); // gstatic/atfdb.unittest:lib_json.SimpleNumber
    // void unittest_lib_json_SimpleString(); // gstatic/atfdb.unittest:lib_json.SimpleString
    // void unittest_lib_json_EmptyArray(); // gstatic/atfdb.unittest:lib_json.EmptyArray
    // void unittest_lib_json_Emptyobject(); // gstatic/atfdb.unittest:lib_json.Emptyobject
    // void unittest_lib_json_SecString(); // gstatic/atfdb.unittest:lib_json.SecString
    // void unittest_lib_json_NumberZero(); // gstatic/atfdb.unittest:lib_json.NumberZero
    // void unittest_lib_json_NumberManyDigits(); // gstatic/atfdb.unittest:lib_json.NumberManyDigits
    // void unittest_lib_json_NumberDecimal(); // gstatic/atfdb.unittest:lib_json.NumberDecimal
    // void unittest_lib_json_NumberExponent1(); // gstatic/atfdb.unittest:lib_json.NumberExponent1
    // void unittest_lib_json_NumberExponent2(); // gstatic/atfdb.unittest:lib_json.NumberExponent2
    // void unittest_lib_json_NumberCombined1(); // gstatic/atfdb.unittest:lib_json.NumberCombined1
    // void unittest_lib_json_NumberCombined2(); // gstatic/atfdb.unittest:lib_json.NumberCombined2
    // void unittest_lib_json_StringEmpty(); // gstatic/atfdb.unittest:lib_json.StringEmpty
    // void unittest_lib_json_StringWithEscapes(); // gstatic/atfdb.unittest:lib_json.StringWithEscapes
    // void unittest_lib_json_ObjFieldTokenNull(); // gstatic/atfdb.unittest:lib_json.ObjFieldTokenNull
    // void unittest_lib_json_ObjFieldTokenFalse(); // gstatic/atfdb.unittest:lib_json.ObjFieldTokenFalse
    // void unittest_lib_json_ObjFieldTokenTrue(); // gstatic/atfdb.unittest:lib_json.ObjFieldTokenTrue
    // void unittest_lib_json_ObjFieldSimpleNumber(); // gstatic/atfdb.unittest:lib_json.ObjFieldSimpleNumber
    // void unittest_lib_json_ObjFieldSimpleString(); // gstatic/atfdb.unittest:lib_json.ObjFieldSimpleString
    // void unittest_lib_json_ObjFieldEmptyArray(); // gstatic/atfdb.unittest:lib_json.ObjFieldEmptyArray
    // void unittest_lib_json_ObjFieldEmptyObject(); // gstatic/atfdb.unittest:lib_json.ObjFieldEmptyObject
    // void unittest_lib_json_ObjFieldAll(); // gstatic/atfdb.unittest:lib_json.ObjFieldAll
    // void unittest_lib_json_ArrayTokenNull(); // gstatic/atfdb.unittest:lib_json.ArrayTokenNull
    // void unittest_lib_json_ArrayTokenFalse(); // gstatic/atfdb.unittest:lib_json.ArrayTokenFalse
    // void unittest_lib_json_ArrayTokenTrue(); // gstatic/atfdb.unittest:lib_json.ArrayTokenTrue
    // void unittest_lib_json_ArraySimpleNumber(); // gstatic/atfdb.unittest:lib_json.ArraySimpleNumber
    // void unittest_lib_json_ArraySimpleString(); // gstatic/atfdb.unittest:lib_json.ArraySimpleString
    // void unittest_lib_json_ArrayEmptyArray(); // gstatic/atfdb.unittest:lib_json.ArrayEmptyArray
    // void unittest_lib_json_ArrayEmptyObject(); // gstatic/atfdb.unittest:lib_json.ArrayEmptyObject
    // void unittest_lib_json_ArrayAll(); // gstatic/atfdb.unittest:lib_json.ArrayAll
    // void unittest_lib_json_Typical(); // gstatic/atfdb.unittest:lib_json.Typical
    // void unittest_lib_json_CtrlCharEscape(); // gstatic/atfdb.unittest:lib_json.CtrlCharEscape
    // void unittest_lib_json_ErrorBadToken1(); // gstatic/atfdb.unittest:lib_json.ErrorBadToken1
    // void unittest_lib_json_ErrorBadToken2(); // gstatic/atfdb.unittest:lib_json.ErrorBadToken2
    // void unittest_lib_json_ErrorBadNumber(); // gstatic/atfdb.unittest:lib_json.ErrorBadNumber
    // void unittest_lib_json_ErrorBadString1(); // gstatic/atfdb.unittest:lib_json.ErrorBadString1
    // void unittest_lib_json_ErrorBadString2(); // gstatic/atfdb.unittest:lib_json.ErrorBadString2
    // void unittest_lib_json_ErrorBadString3(); // gstatic/atfdb.unittest:lib_json.ErrorBadString3
    // void unittest_lib_json_ErrorBadUString1(); // gstatic/atfdb.unittest:lib_json.ErrorBadUString1
    // void unittest_lib_json_ErrorBadUString2(); // gstatic/atfdb.unittest:lib_json.ErrorBadUString2
    // void unittest_lib_json_ErrorBadUString3(); // gstatic/atfdb.unittest:lib_json.ErrorBadUString3
    // void unittest_lib_json_ErrorBadUString4(); // gstatic/atfdb.unittest:lib_json.ErrorBadUString4
    // void unittest_lib_json_ErrorBadUString5(); // gstatic/atfdb.unittest:lib_json.ErrorBadUString5
    // void unittest_lib_json_ErrorBadUString6(); // gstatic/atfdb.unittest:lib_json.ErrorBadUString6
    // void unittest_lib_json_ErrorBadUString7(); // gstatic/atfdb.unittest:lib_json.ErrorBadUString7
    // void unittest_lib_json_ErrorBrMismatch1(); // gstatic/atfdb.unittest:lib_json.ErrorBrMismatch1
    // void unittest_lib_json_ErrorBrMismatch2(); // gstatic/atfdb.unittest:lib_json.ErrorBrMismatch2
    // void unittest_lib_json_ErrorBrMismatch3(); // gstatic/atfdb.unittest:lib_json.ErrorBrMismatch3
    // void unittest_lib_json_ErrorBrMismatch4(); // gstatic/atfdb.unittest:lib_json.ErrorBrMismatch4
    // void unittest_lib_json_ErrorBrMismatch5(); // gstatic/atfdb.unittest:lib_json.ErrorBrMismatch5
    // void unittest_lib_json_ErrorBrMismatch6(); // gstatic/atfdb.unittest:lib_json.ErrorBrMismatch6
    // void unittest_lib_json_ErrorBrMismatch7(); // gstatic/atfdb.unittest:lib_json.ErrorBrMismatch7
    // void unittest_lib_json_ErrorBrMismatch8(); // gstatic/atfdb.unittest:lib_json.ErrorBrMismatch8
    // void unittest_lib_json_ErrorBrMismatch9(); // gstatic/atfdb.unittest:lib_json.ErrorBrMismatch9
    // void unittest_lib_json_ErrorBrMismatch10(); // gstatic/atfdb.unittest:lib_json.ErrorBrMismatch10
    // void unittest_lib_json_ErrorBrMismatch11(); // gstatic/atfdb.unittest:lib_json.ErrorBrMismatch11
    // void unittest_lib_json_ErrorBrMismatch12(); // gstatic/atfdb.unittest:lib_json.ErrorBrMismatch12
    // void unittest_lib_json_ErrorBrMismatch13(); // gstatic/atfdb.unittest:lib_json.ErrorBrMismatch13
    // void unittest_lib_json_ErrorBrMismatch14(); // gstatic/atfdb.unittest:lib_json.ErrorBrMismatch14
    // void unittest_lib_json_ErrorArrayComma1(); // gstatic/atfdb.unittest:lib_json.ErrorArrayComma1
    // void unittest_lib_json_ErrorArrayComma2(); // gstatic/atfdb.unittest:lib_json.ErrorArrayComma2
    // void unittest_lib_json_ErrorArrayComma3(); // gstatic/atfdb.unittest:lib_json.ErrorArrayComma3
    // void unittest_lib_json_ErrorArrayComma4(); // gstatic/atfdb.unittest:lib_json.ErrorArrayComma4
    // void unittest_lib_json_ErrorObjectComma1(); // gstatic/atfdb.unittest:lib_json.ErrorObjectComma1
    // void unittest_lib_json_ErrorObjectComma2(); // gstatic/atfdb.unittest:lib_json.ErrorObjectComma2
    // void unittest_lib_json_ErrorObjectComma3(); // gstatic/atfdb.unittest:lib_json.ErrorObjectComma3
    // void unittest_lib_json_ErrorObjectComma4(); // gstatic/atfdb.unittest:lib_json.ErrorObjectComma4
    // void unittest_lib_json_ErrorBareComma(); // gstatic/atfdb.unittest:lib_json.ErrorBareComma
    // void unittest_lib_json_ErrorBareValuesWithComma(); // gstatic/atfdb.unittest:lib_json.ErrorBareValuesWithComma
    // void unittest_lib_json_ErrorObjectNoValue(); // gstatic/atfdb.unittest:lib_json.ErrorObjectNoValue
    // void unittest_lib_json_ErrorObjectColon1(); // gstatic/atfdb.unittest:lib_json.ErrorObjectColon1
    // void unittest_lib_json_ErrorObjectColon2(); // gstatic/atfdb.unittest:lib_json.ErrorObjectColon2
    // void unittest_lib_json_ErrorObjectColon3(); // gstatic/atfdb.unittest:lib_json.ErrorObjectColon3
    // void unittest_lib_json_ErrorObjectColon4(); // gstatic/atfdb.unittest:lib_json.ErrorObjectColon4
    // void unittest_lib_json_ErrorObjectColon5(); // gstatic/atfdb.unittest:lib_json.ErrorObjectColon5
    // void unittest_lib_json_ErrorBareColon(); // gstatic/atfdb.unittest:lib_json.ErrorBareColon
    // void unittest_lib_json_ErrorArrayColon(); // gstatic/atfdb.unittest:lib_json.ErrorArrayColon
    // void unittest_lib_json_ErrorBareValuesWithColon(); // gstatic/atfdb.unittest:lib_json.ErrorBareValuesWithColon
    // void unittest_lib_json_ErrorObjectDupField(); // gstatic/atfdb.unittest:lib_json.ErrorObjectDupField
    // void unittest_lib_json_FmtJson_u64_0(); // gstatic/atfdb.unittest:lib_json.FmtJson_u64_0
    // void unittest_lib_json_FmtJson_u64_max(); // gstatic/atfdb.unittest:lib_json.FmtJson_u64_max
    // void unittest_lib_json_FmtJson_u32_0(); // gstatic/atfdb.unittest:lib_json.FmtJson_u32_0
    // void unittest_lib_json_FmtJson_u32_max(); // gstatic/atfdb.unittest:lib_json.FmtJson_u32_max
    // void unittest_lib_json_FmtJson_u16_0(); // gstatic/atfdb.unittest:lib_json.FmtJson_u16_0
    // void unittest_lib_json_FmtJson_u16_max(); // gstatic/atfdb.unittest:lib_json.FmtJson_u16_max
    // void unittest_lib_json_FmtJson_u8_0(); // gstatic/atfdb.unittest:lib_json.FmtJson_u8_0
    // void unittest_lib_json_FmtJson_u8_max(); // gstatic/atfdb.unittest:lib_json.FmtJson_u8_max
    // void unittest_lib_json_FmtJson_i64_min(); // gstatic/atfdb.unittest:lib_json.FmtJson_i64_min
    // void unittest_lib_json_FmtJson_i64_max(); // gstatic/atfdb.unittest:lib_json.FmtJson_i64_max
    // void unittest_lib_json_FmtJson_i32_min(); // gstatic/atfdb.unittest:lib_json.FmtJson_i32_min
    // void unittest_lib_json_FmtJson_i32_max(); // gstatic/atfdb.unittest:lib_json.FmtJson_i32_max
    // void unittest_lib_json_FmtJson_i16_min(); // gstatic/atfdb.unittest:lib_json.FmtJson_i16_min
    // void unittest_lib_json_FmtJson_i16_max(); // gstatic/atfdb.unittest:lib_json.FmtJson_i16_max
    // void unittest_lib_json_FmtJson_i8_min(); // gstatic/atfdb.unittest:lib_json.FmtJson_i8_min
    // void unittest_lib_json_FmtJson_i8_max(); // gstatic/atfdb.unittest:lib_json.FmtJson_i8_max
    // void unittest_lib_json_FmtJson_double_prec(); // gstatic/atfdb.unittest:lib_json.FmtJson_double_prec
    // void unittest_lib_json_FmtJson_float_prec(); // gstatic/atfdb.unittest:lib_json.FmtJson_float_prec
    // void unittest_lib_json_FmtJson_bool_true(); // gstatic/atfdb.unittest:lib_json.FmtJson_bool_true
    // void unittest_lib_json_FmtJson_bool_false(); // gstatic/atfdb.unittest:lib_json.FmtJson_bool_false
    // void unittest_lib_json_FmtJson_char(); // gstatic/atfdb.unittest:lib_json.FmtJson_char
    // void unittest_lib_json_FmtJson_TypeA(); // gstatic/atfdb.unittest:lib_json.FmtJson_TypeA

    // A record standing on a single array field prints as that array, not as an
    // object wrapping it: the returned node is the array itself, and each element
    // is a string, since Smallstr20 has no Json cfmt and prints through Print.
    // void unittest_lib_json_FmtJson_Ary(); // gstatic/atfdb.unittest:lib_json.FmtJson_Ary
    // void unittest_lib_json_FmtJson_Object(); // gstatic/atfdb.unittest:lib_json.FmtJson_Object

    // -------------------------------------------------------------------
    // cpp/atf_unit/lib_sql.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void unittest_lib_sql_Main(); // gstatic/atfdb.unittest:lib_sql.Main

    // -------------------------------------------------------------------
    // cpp/atf_unit/lib_ws.cpp
    //

    // Roundtrip small frame (payload <= 125): byte1's low 7 bits hold the length
    // directly. Covers both unmasked (byte1 < 0x80) and masked (byte1 >= 0x80).
    //     (user-implemented function, prototype is in amc-generated header)
    // void unittest_lib_ws_SmallFrame(); // gstatic/atfdb.unittest:lib_ws.SmallFrame

    // Roundtrip Frame16 / FrameMasked16 (payload in [126..65535]):
    // byte1 == 126 (unmasked) or 254 (masked), followed by u16 big-endian length.
    // void unittest_lib_ws_Frame16(); // gstatic/atfdb.unittest:lib_ws.Frame16

    // Roundtrip Frame64 / FrameMasked64 (payload >= 65536):
    // byte1 == 127 (unmasked) or 255 (masked), followed by u64 big-endian length.
    // void unittest_lib_ws_Frame64(); // gstatic/atfdb.unittest:lib_ws.Frame64

    // Confirm rsv1/rsv2/rsv3 setters land in byte0 at the documented bit positions
    // (RFC 6455 §5.2: RSV1=bit6, RSV2=bit5, RSV3=bit4). The x2gw parser uses these
    // getters to reject extensions we don't support; this keeps the encoding stable.
    // void unittest_lib_ws_RsvBits(); // gstatic/atfdb.unittest:lib_ws.RsvBits

    // -------------------------------------------------------------------
    // cpp/atf_unit/line.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void unittest_algo_lib_FileLine_curs(); // gstatic/atfdb.unittest:algo_lib.FileLine_curs

    // -------------------------------------------------------------------
    // cpp/atf_unit/lockfile.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void unittest_algo_lib_Lockfile(); // gstatic/atfdb.unittest:algo_lib.Lockfile

    // -------------------------------------------------------------------
    // cpp/atf_unit/main.cpp
    //
    void AdjustDebugPath(algo::cstring &path);

    // Compare contents of file `outfname` with the reference file.
    // Any difference = error
    void CompareOutput(strptr outfname);

    // Run specified test (called both with -nofork and without)
    void Main_StartTest(atf_unit::FUnittest &test, lib_exec::FSyscmd *start, lib_exec::FSyscmd *end);
    //     (user-implemented function, prototype is in amc-generated header)
    // void unittest_amc_Unit(); // gstatic/atfdb.unittest:amc.Unit

    // usage:
    // DO_PERF_TEST("Testing XYZ",xyz());
    // The expression will be evaluated for 2 seconds, after which average speed will be printed.
    void PrintPerfSample(const strptr& action, u64 nloops, u64 clocks);
    void Testcmp(const char *file, int line, strptr value, strptr expect, bool eq);
    void Testcmp(const char *file, int line, const char *value, const char *expect, bool eq);
    //     (user-implemented function, prototype is in amc-generated header)
    // void unittest_atf_unit_Outfile(); // gstatic/atfdb.unittest:atf_unit.Outfile
    // void Main(); // main:atf_unit

    // -------------------------------------------------------------------
    // cpp/atf_unit/parsenum.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void unittest_algo_lib_ParseNumber(); // gstatic/atfdb.unittest:algo_lib.ParseNumber

    // -------------------------------------------------------------------
    // cpp/atf_unit/regx.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void unittest_algo_lib_Regx(); // gstatic/atfdb.unittest:algo_lib.Regx

    // Test that matching a huge string with a regex that
    // ends in .* is fast.
    // void unittest_algo_lib_RegxShortCircuit(); // gstatic/atfdb.unittest:algo_lib.RegxShortCircuit
    // void unittest_algo_lib_RegxReadTwice(); // gstatic/atfdb.unittest:algo_lib.RegxReadTwice
    // void unittest_algo_lib_RegxReadTwice2(); // gstatic/atfdb.unittest:algo_lib.RegxReadTwice2

    // -------------------------------------------------------------------
    // cpp/atf_unit/string.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void unittest_algo_lib_SubstringIndex(); // gstatic/atfdb.unittest:algo_lib.SubstringIndex
    // void unittest_algo_lib_Aligned(); // gstatic/atfdb.unittest:algo_lib.Aligned
    // void unittest_algo_lib_CString(); // gstatic/atfdb.unittest:algo_lib.CString
    // void unittest_algo_lib_StringFind(); // gstatic/atfdb.unittest:algo_lib.StringFind
    // void unittest_algo_lib_ReplaceIdent(); // gstatic/atfdb.unittest:algo_lib.ReplaceIdent
    // void unittest_algo_lib_StringCase(); // gstatic/atfdb.unittest:algo_lib.StringCase
    // void unittest_algo_lib_Tabulate(); // gstatic/atfdb.unittest:algo_lib.Tabulate
    // void unittest_algo_lib_StringSepCurs(); // gstatic/atfdb.unittest:algo_lib.StringSepCurs
    // void unittest_algo_lib_LongStr(); // gstatic/atfdb.unittest:algo_lib.LongStr

    // -------------------------------------------------------------------
    // cpp/atf_unit/time.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void unittest_algo_lib_TimeConversion(); // gstatic/atfdb.unittest:algo_lib.TimeConversion
    // void unittest_algo_lib_TstampCache(); // gstatic/atfdb.unittest:algo_lib.TstampCache
    // void unittest_algo_lib_PrintUnTime(); // gstatic/atfdb.unittest:algo_lib.PrintUnTime
    // void unittest_algo_lib_ParseUnTime(); // gstatic/atfdb.unittest:algo_lib.ParseUnTime
    // void unittest_algo_lib_DayName(); // gstatic/atfdb.unittest:algo_lib.DayName
    // void unittest_algo_lib_CurrentTime(); // gstatic/atfdb.unittest:algo_lib.CurrentTime
    // void unittest_algo_lib_TimeConvert(); // gstatic/atfdb.unittest:algo_lib.TimeConvert

    // -------------------------------------------------------------------
    // cpp/atf_unit/tuple.cpp
    //
    //     (user-implemented function, prototype is in amc-generated header)
    // void unittest_algo_lib_Tuple1(); // gstatic/atfdb.unittest:algo_lib.Tuple1
    // void unittest_algo_lib_Tuple2(); // gstatic/atfdb.unittest:algo_lib.Tuple2

    // Check Attr_curs
    // void unittest_algo_lib_Tuple(); // gstatic/atfdb.unittest:algo_lib.Tuple

    // An unterminated quote fails the parse regardless of which side of a colon
    // it falls on: bare-token, name, and value positions all reject the line,
    // and a properly closed quote in any position stays accepted.
    // void unittest_algo_lib_TupleBadQuote(); // gstatic/atfdb.unittest:algo_lib.TupleBadQuote
}

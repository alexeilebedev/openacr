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
// Target: algo_lib (lib) -- Support library for all executables
// Exceptions: NO
// Header: include/algo/time.h -- Time functions
//
// Created By: alexei.lebedev
// Recent Changes: alexei.lebedev
//

#pragma once

namespace algo {
    static const i64 MSECS_PER_DAY     = 60*60*24*1000;
    static const i64 SECS_PER_DAY      = 60*60*24;
    static const i64 SECS_PER_HOUR     = 60*60;
    static const i64 SECS_PER_MIN      = 60;
    static const i64 UNTIME_PER_SEC    = 1*1000*1000*1000;
    static const i64 UNTIME_PER_MSEC   = 1*1000*1000;
    static const i64 UNTIME_PER_USEC   = 1*1000;
    static const i64 WTIME_OFFSET   = 116444736000000000LL;
    static const i64 WTIME_PER_SEC  = 10000000;
    static const i64 WTIME_PER_MSEC = 10000;
    static const i64 WTIME_PER_USEC = 10;

    UnixDiff UnixDiffHMS(i64 h, int m=0, int s = 0);
    UnDiff UnDiffHMS(int h, int m=0, int s=0);

    struct TimeStruct : tm {
        int  tm_nsec;
        bool tm_neg;
        inline TimeStruct();
        inline TimeStruct(const struct tm &t);
    };
}

namespace algo { // update-hdr srcfile:"%/algo/time.%"
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/lib/algo/time.cpp -- UnTime / UnDiff functions
    //
    bool TimeStruct_Read(TimeStruct &out, algo::StringIter &iter, const strptr& spec);
    TimeStruct ToTimeStruct(UnDiff   U);
    TimeStruct ToTimeStruct(UnixDiff U);
    algo::UnixDiff ToUnixDiff(const TimeStruct &S);
    algo::UnDiff ToUnDiff(const TimeStruct &S);
    const algo::UnixTime LocalDate(UnixTime in);
    TimeStruct GetLocalTimeStruct(UnixTime U);
    TimeStruct GetLocalTimeStruct(UnTime U);
    const algo::UnTime LocalDate(UnTime in);
    algo::TimeStruct GetGMTimeStruct(algo::UnTime U);
    algo::UnixTime ToUnixTime(const TimeStruct &S);
    algo::UnTime ToUnTime(const TimeStruct &S);

    // empty string -> 0
    // invalid weekday -> -1
    // All other weeks days map to 0..6, numbers compatible with struct tm's tm_wday field
    int GetWeekday(strptr wday);
    const strptr GetWeekdayName(int index);
    int GetMonthZeroBased(strptr month);
    int GetWholeMonthZeroBased(strptr s);

    // Inverse mapping to abbreviated month names (Jan, Feb, etc.)
    // If short_name==false, returns full name (January, February, etc.)
    const strptr GetMonthNameZeroBased(int index);
    const strptr GetMonthNameZeroBasedShort(int index);

    // DateCache -- Roughly 200x faster LocalDate
    const algo::UnTime DateCache_LocalDate(algo::DateCache &dc, UnTime in);

    // Todo: test on windows
    algo::UnTime CurrUnTime();
    algo::UnTime ToUnTime(WTime s);

    // Change TZ environment variable to specified value
    // and notify C runtime lib of the change
    void SetTz(strptr zone);

    // -------------------------------------------------------------------
    // include/algo/time.inl.h
    //

    // get CPU HZ value as u64
    inline u64 get_cpu_hz_int();

    // get CPU HZ value as double
    inline double get_cpu_hz();

    // cpu_hz (untyped SchedTime)
    // use this for timestamps.
    // these calls may be pipelined and reordered, so measuring instruction
    // latency with these is not possible. for that, use rdtscp
    inline u64 get_cycles();

    // Convert scheduler time units to seconds.
    inline double ToSecs(SchedTime ticks);
    inline algo::UnDiff ToUnDiff(SchedTime ticks);
    inline algo::SchedTime ToSchedTime(double secs);
    inline algo::UnTime ToUnTime(UnixTime t);
    inline algo::UnixTime ToUnixTime(UnTime t);
    inline double ToSecs(UnDiff t);
    inline double ToSecs(UnTime t);
    inline algo::UnixTime CurrUnixTime();

    // use this for performance measurements.
    // according to Intel software manual, lfence followed by rdtsc
    // is the beez knees.
    inline u64 rdtscp();
    inline algo::UnixDiff UnixDiffHMS(i64 h, int m, int s);
    inline algo::UnDiff UnDiffSecs(double d);
    inline algo::UnTime UnTimeSecs(double d);
    inline algo::UnDiff UnDiffSecs(i64 i);
    inline algo::UnDiff UnDiffSecs(i32 i);
    inline algo::UnDiff UnDiffHMS(int h, int m, int s);

    // Current value of get_cycles();
    inline algo::SchedTime CurrSchedTime();

    // Elapsed time in seconds between two SchedTimes.
    inline double ElapsedSecs(algo::SchedTime start, algo::SchedTime end);
    inline algo::WDiff ToWDiff(algo::UnixDiff d);
    inline algo::WDiff ToWDiff(algo::UnDiff d);
    inline algo::UnixDiff ToUnixDiff(algo::WDiff d);
    inline algo::WTime ToWTime(algo::UnTime s);
    inline algo::UnDiff ToUnDiff(algo::WDiff d);
    inline algo::UnixTime ToUnixTime(algo::WTime nt);
    inline algo::WTime ToWTime(algo::UnixTime t);
    inline double ToSecs(algo::WDiff t);
}

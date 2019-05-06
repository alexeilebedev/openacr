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
// Target: algo_lib (lib) -- Support library for all executables
// Exceptions: NO
// Header: include/algo/time.inl.h
//
// Created By: alexei.lebedev
// Recent Changes: alexei.lebedev
//

#pragma once

// cpu_hz (untyped SchedTime)
// use this for timestamps.
// these calls may be pipelined and reordered, so measuring instruction
// latency with these is not possible. for that, use rdtscp
inline u64 algo::get_cycles() {
    unsigned low, high;
    asm volatile (
                  "rdtsc"
                  : "=a" (low), "=d" (high)
                  :
                  :
                  );
    return u64(high)<<32 | low;
}

// Convert scheduler time units to seconds.
inline double algo::ToSecs(SchedTime ticks) {
    return ticks.value * (1.0/algo_lib::_db.hz);
}

inline algo::UnDiff algo::ToUnDiff(SchedTime ticks) {
    return UnDiff(i64(ticks.value * (1e9/algo_lib::_db.hz)));
}

inline algo::SchedTime algo::ToSchedTime(double secs) {
    return SchedTime(u64(secs * algo_lib::_db.hz));
}

inline algo::UnTime algo::ToUnTime(UnixTime t) {
    return UnTime(t.value * UNTIME_PER_SEC);
}

inline algo::UnixTime algo::ToUnixTime(UnTime t) {
    const i64 quotient = t.value/UNTIME_PER_SEC;
    return UnixTime((t.value % UNTIME_PER_SEC < 0) ? quotient-1 : quotient);
}

inline double algo::ToSecs(UnDiff t) {
    return t.value / double(UNTIME_PER_SEC);
}

inline algo::UnixTime algo::CurrUnixTime(){
    time_t secs;
    time(&secs);
    return UnixTime(secs);
}
//
// use this for performance measurements.
// according to Intel software manual, lfence followed by rdtsc
// is the beez knees.
//
inline u64 algo::rdtscp() {
    unsigned low, high;
    asm volatile (
                  "lfence\n"
                  "rdtsc"
                  : "=a" (low), "=d" (high) // output
                  :                         // input
                  : "memory"                // clobbered
                  );
    return u64(high)<<32 | low;
}

inline UnixDiff algo::UnixDiffHMS(i64 h, int m, int s) {
    return UnixDiff(i64(h)*SECS_PER_HOUR + i64(m)*SECS_PER_MIN + s);
}

inline UnDiff algo::UnDiffSecs(double d) {
    return UnDiff(i64(d  * UNTIME_PER_SEC));
}

inline UnDiff algo::UnDiffSecs(i64 i) {
    return UnDiff(    i  * UNTIME_PER_SEC);
}

inline UnDiff algo::UnDiffSecs(i32 i) {
    return UnDiff(i64(i) * UNTIME_PER_SEC);
}

inline UnDiff algo::UnDiffHMS(int h, int m, int s) {
    return UnDiffSecs(i64(h) * SECS_PER_HOUR + i64(m)*SECS_PER_MIN + s);
}

// Current value of get_cycles();
inline algo::SchedTime algo::CurrSchedTime() {
    return algo::SchedTime(get_cycles());
}

// Elapsed time in seconds between two SchedTimes.
inline double algo::ElapsedSecs(algo::SchedTime start, algo::SchedTime end) {
    return (end-start)/get_cpu_hz();
}

inline algo::TimeStruct::TimeStruct() {
    ZeroBytes(static_cast<tm&>(*this));
    tm_nsec = 0;
    tm_neg = false;
    tm_isdst = -1;
}

inline algo::TimeStruct::TimeStruct(const struct tm &t) : tm(t) {
    tm_nsec = 0;
    tm_neg = false;
}

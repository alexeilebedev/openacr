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

// -----------------------------------------------------------------------------

// get CPU HZ value as u64
inline u64 algo::get_cpu_hz_int() {
    return algo_lib::_db.cpu_hz;
}

// get CPU HZ value as double
inline double algo::get_cpu_hz() {
    return algo_lib::_db.hz;
}

// cpu_hz (untyped SchedTime)
// use this for timestamps.
// these calls may be pipelined and reordered, so measuring instruction
// latency with these is not possible. for that, use rdtscp
inline u64 algo::get_cycles() {
#ifdef WIN32
    return __rdtsc();
#else
    unsigned low, high;
    asm volatile (
                  "rdtsc"
                  : "=a" (low), "=d" (high)
                  :
                  :
                  );
    return u64(high)<<32 | low;
#endif
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

inline double algo::ToSecs(UnTime t) {
    return t.value / double(UNTIME_PER_SEC);
}

inline algo::UnixTime algo::CurrUnixTime(){
    time_t secs;
    time(&secs);
    return UnixTime(secs);
}

// use this for performance measurements.
// according to Intel software manual, lfence followed by rdtsc
// is the beez knees.
inline u64 algo::rdtscp() {
#ifdef WIN32
    _ReadBarrier();
    return get_cycles();
#else
    unsigned low, high;
    asm volatile (
                  "lfence\n"
                  "rdtsc"
                  : "=a" (low), "=d" (high) // output
                  :                         // input
                  : "memory"                // clobbered
                  );
    return u64(high)<<32 | low;
#endif
}

inline algo::UnixDiff algo::UnixDiffHMS(i64 h, int m, int s) {
    return algo::UnixDiff(i64(h)*SECS_PER_HOUR + i64(m)*SECS_PER_MIN + s);
}

inline algo::UnDiff algo::UnDiffSecs(double d) {
    return algo::UnDiff(i64(d  * UNTIME_PER_SEC));
}

inline algo::UnTime algo::UnTimeSecs(double d) {
    return algo::UnTime(i64(d  * UNTIME_PER_SEC));
}

inline algo::UnDiff algo::UnDiffSecs(i64 i) {
    return algo::UnDiff(    i  * UNTIME_PER_SEC);
}

inline algo::UnDiff algo::UnDiffSecs(i32 i) {
    return algo::UnDiff(i64(i) * UNTIME_PER_SEC);
}

inline algo::UnDiff algo::UnDiffHMS(int h, int m, int s) {
    return algo::UnDiffSecs(i64(h) * SECS_PER_HOUR + i64(m)*SECS_PER_MIN + s);
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

inline algo::WDiff algo::ToWDiff(algo::UnixDiff d) {
    return algo::WDiff(d.value * WTIME_PER_SEC);
}

inline algo::WDiff algo::ToWDiff(algo::UnDiff d) {
    return algo::WDiff(d.value / 100);
}

inline algo::UnixDiff algo::ToUnixDiff(algo::WDiff d) {
    return algo::UnixDiff(d.value / WTIME_PER_SEC);
}

inline algo::WTime algo::ToWTime(algo::UnTime s) {
    return WTime(i64(s.value / (algo::UNTIME_PER_SEC / WTIME_PER_SEC) + WTIME_OFFSET));
}

inline algo::UnDiff algo::ToUnDiff(algo::WDiff d) {
    return algo::UnDiff(d.value * 100);
}

inline algo::UnixTime algo::ToUnixTime(algo::WTime nt) {
    i64 tmp(nt.value - WTIME_OFFSET);
    // clip to the earliest possible UnixTime
    return algo::UnixTime(tmp >= 0 ? int(u64(tmp) / WTIME_PER_SEC) : 0);
}

inline algo::WTime algo::ToWTime(algo::UnixTime t) {
    return algo::WTime(i64(t.value) * WTIME_PER_SEC + WTIME_OFFSET);
}

inline double algo::ToSecs(algo::WDiff t) {
    return t.value / double(algo::WTIME_PER_SEC);
}

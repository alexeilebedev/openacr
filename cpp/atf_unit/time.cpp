// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2020-2023 Astra
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
// Contacting ICE: <https://www.theice.com/contact>
// Target: atf_unit (exe) -- Unit tests (see unittest table)
// Exceptions: yes
// Source: cpp/atf_unit/time.cpp
//

#include "include/atf_unit.h"
static int counter=0;
struct Counter {
    Counter() {counter++;}
    ~Counter() {counter--;}
    Counter(const Counter &) {counter++;}
};

// 11 is minimal coprime integer to:
// 1Ex     - decimal second fraction per second
// 60      - seconds per minute, minutes per hour
// 24      - hours per day
// 7       - days per week
// 28-31   - days per month
// 12      - months per year
// 365,366 - days per year
// 100     - years per century
// 4,400   - leap year cycles
// 146097  - days (20871 weeks, 400 years) of full circle of Gregorian calendar
// so the combinatins will have wider spread than for binary run

enum {
    GEN_MULTIPLY = 11
    ,GEN_ADD      = 1
};

// ----------------------------------------------------------------------------

// Linear congruential generator

struct GenNum {
    const i64 a;
    const i64 c;
    const i64 m;
    i64 x;

    GenNum(i64 in_m) : a(GEN_MULTIPLY), c(GEN_ADD), m(in_m), x(0) { }
    void Next() { if (!m) return; x = (a * x + c) % m; }
    i64  Get()  { return x; }
};

// ----------------------------------------------------------------------------

struct TimeTestIter {
    enum : i64 {
        MIN_UTIME = -2208988800 // 1900-01-01 00:00:00 - min value to test
        ,MAX_UTIME = 2147483647  // 2038-01-19 03:14:07 - max value to test
        ,SPREAD    = MAX_UTIME - MIN_UTIME
        ,LIMIT     = 1000        // iteration limit
    };

    GenNum  gen_ut;              // unix time generator
    GenNum  gen_frac;            // fraction generator
    i64     ups;                 // units per sec
    i64     off;                 // offset

    i64     ut;                  // unix time
    i64     frac;                // fraction
    i64     ft;                  // full time
    i64     dft;                 // diff time (always positive)

    i64     iter;                // iteration number

    TimeTestIter(i64 in_ups, i64 in_off);
    void Sync(bool no_gen=false);  // synchronize data (no gen - do not ask generators)
    void Next();                   // advance
    bool EndQ();                   // is finished
};

TimeTestIter::TimeTestIter(i64 in_ups, i64 in_off)
    :gen_ut(SPREAD)
    ,gen_frac(in_ups)
    ,ups(in_ups)
    ,off(in_off)
    ,iter(LIMIT) {
    Sync();
}

void TimeTestIter::Sync(bool no_gen) {
    if (!no_gen) {
        ut   = gen_ut.Get() + MIN_UTIME ;
        frac = gen_frac.Get();
    }
    ft   = (ut + off) * ups + frac;
    dft  = (ut - MIN_UTIME) * ups + frac;
}

void TimeTestIter::Next() {
    if (iter > 0) {
        --iter;
        gen_ut.Next();
        gen_frac.Next();
        Sync();
    } else if (iter == 0) {
        --iter;
        ut   = MAX_UTIME;
        frac = ups - 1 ;
        Sync(true);
    } else if (iter == -1) {
        --iter;
    }
}

bool   TimeTestIter::EndQ() {
    return iter < -1 ;
}


// helper macros to iterate time

#define time_test_beg(in_ut,in_frac,in_ft,in_ups,in_off) {  \
    TimeTestIter v##Iter(in_ups,in_off);                    \
    for (;!v##Iter.EndQ();v##Iter.Next()) {                 \
    const time_t &in_ut   = v##Iter.ut;    (void)in_ut;     \
    const i64    &in_frac = v##Iter.frac;  (void)in_frac;   \
    const i64    &in_ft   = v##Iter.ft;    (void)in_ft;

#define time_test_end }}

// helper macros to iterate time diff

#define timediff_test_beg(in_dft,in_ups,in_off) {           \
    TimeTestIter v##Iter(in_ups,in_off);                    \
    for (;!v##Iter.EndQ();v##Iter.Next()) {                 \
    int v##_posneg = 0;                                     \
    for (;v##_posneg < 2;++v##_posneg) {                    \
    i64 in_dft = v##_posneg ? v##Iter.dft : -v##Iter.dft;

#define timediff_test_end }}}

// ----------------------------------------------------------------------------

// Template function to facilitate tests for conversion to TimeStruct

template <typename T> void TestTimeToTimeStruct(i64 units_per_sec, i64 offset_sec) {
    time_test_beg(ut,frac,test_time,units_per_sec,offset_sec) {
        T t(test_time);
        struct tm stm;
        vrfy_(localtime_r(&ut,&stm));
        TimeStruct S = GetLocalTimeStruct(t);
        vrfyeq_(S.tm_sec   ,stm.tm_sec);
        vrfyeq_(S.tm_min   ,stm.tm_min);
        vrfyeq_(S.tm_hour  ,stm.tm_hour);
        vrfyeq_(S.tm_mday  ,stm.tm_mday);
        vrfyeq_(S.tm_mon   ,stm.tm_mon);
        vrfyeq_(S.tm_year  ,stm.tm_year);
        vrfyeq_(S.tm_wday  ,stm.tm_wday);
        vrfyeq_(S.tm_yday  ,stm.tm_yday);
        vrfyeq_(S.tm_isdst ,stm.tm_isdst);
        vrfyeq_(S.tm_nsec  ,frac*(1000000000/units_per_sec));
    } time_test_end;
}

// The same for diff types

template <typename T> void TestDiffToTimeStruct(i64 units_per_sec, i64 offset_sec) {
    timediff_test_beg(td,units_per_sec,offset_sec) {
        T t(td);
        TimeStruct S = ToTimeStruct(t);
        i64 test_diff = td;
        vrfyeq_(S.tm_nsec, algo::Abs((test_diff % units_per_sec) * (1000000000/units_per_sec)));
        test_diff /= units_per_sec;
        vrfyeq_(S.tm_sec ,algo::Abs(test_diff % 60));
        test_diff /= 60;
        vrfyeq_(S.tm_min ,algo::Abs(test_diff % 60));
        test_diff /= 60;
        vrfyeq_(S.tm_hour,algo::Abs(test_diff));
        vrfyeq_(S.tm_neg,td < 0);
    } timediff_test_end;
}

// ----------------------------------------------------------------------------

// Template function to facilitate tests for conversion from TimeStruct

template <typename T> void TestTimeStructToTime(i64 units_per_sec, i64 offset_sec, T conv(const TimeStruct&)) {
    time_test_beg(ut,frac,test_time,units_per_sec,offset_sec) {
        TimeStruct S;
        vrfy_(localtime_r(&ut,static_cast<struct tm*>(&S)));
        S.tm_nsec = frac*(1000000000/units_per_sec);
        T t = conv(S);
        vrfyeq_(t.value,test_time);
    } time_test_end;
}

// The same for diff types

template <typename T> void TestTimeStructToDiff(i64 units_per_sec, i64 offset_sec, T conv(const TimeStruct&)) {
    timediff_test_beg(td,units_per_sec,offset_sec) {
        i64 x = td;
        TimeStruct S;
        S.tm_neg = x < 0;
        S.tm_nsec = algo::Abs((x % units_per_sec) * (1000000000/units_per_sec));
        x /= units_per_sec;
        S.tm_sec = algo::Abs(x % 60);
        x /= 60;
        S.tm_min = algo::Abs(x % 60);
        x /= 60;
        S.tm_hour = algo::Abs(x);
        T t = conv(S);
        vrfyeq_(t.value,td);
    } timediff_test_end;
}

// ----------------------------------------------------------------------------

// Template function to facilitate tests for conversion from one type to another T1==>T2

template <typename T1, typename T2> void TestTimeConv(i64 units_per_sec, i64 offset_sec, i64 div, i64 mul, i64 shift, T2 conv(const T1)) {
    time_test_beg(ut,frac,test_time,units_per_sec,offset_sec) {
        i64 res_test_time = test_time;
        if (div) {
            res_test_time = (res_test_time % div < 0) ? res_test_time / div - 1 : res_test_time / div ;
        }
        res_test_time *= mul;
        res_test_time += shift;
        T1 t1(test_time);
        T2 t2 = conv(t1);
        vrfyeq_(t2.value,res_test_time);
    } time_test_end;
}


// -----------------------------------------------------------------------------

static void TstampCache_CheckTimeRange(algo::TstampCache& cache, strptr start_date, strptr end_date, i64 abs_nano_step){
    algo::UnTime beg, end;
    vrfy_(UnTime_ReadStrptrMaybe(beg, start_date));
    vrfy_(UnTime_ReadStrptrMaybe(end, end_date));
    i64 nano_step = abs_nano_step *( end.value > beg.value ? 1 : -1);
    cstring out1, out2;
    int progress = 1;
    int n = (end.value - beg.value ) / nano_step;
    int report_progress = n / 2;
    for(; beg.value < end.value; beg.value+=nano_step){
        UnTime_PrintSpec(beg, out1, cache.spec);
        algo::tstamp_cache_Print(cache, beg, out2);
        vrfy(out1 == out2, out1<<" must be equal to " << out2);
        if(progress++ % report_progress == 0){
            prlog(Keyval("test", "PrintTime")<<
                  Keyval("spec", cache.spec)<<
                  Keyval("progress", (progress-1) * 1.0/ n )<<
                  Keyval("UnTime_PrintSpec", out1) <<
                  Keyval("TstampCache", out2));
        }
        ch_RemoveAll(out1);
        ch_RemoveAll(out2);
    }
}

static void TstampCache_CheckTimeSpec(strptr spec){
    cstring out1;
    cstring out2;
    algo::UnTime t;
    algo::TstampCache cache;
    algo::tstamp_cache_Init(cache, spec, false);
    //pass forward to leap year.
    TstampCache_CheckTimeRange(cache, "1995/12/30 00:00:00", "1996/01/03 00:00:00", algo::UNTIME_PER_SEC / 2 -1);
    //pass backward from leap year
    TstampCache_CheckTimeRange(cache, "1996/01/03 00:00:00", "1995/12/30 00:00:00", algo::UNTIME_PER_SEC / 2 -1);
    TstampCache_CheckTimeRange(cache, "1996/12/30 00:00:00", "1997/01/02 00:00:00", algo::UNTIME_PER_SEC / 2 -1);
    TstampCache_CheckTimeRange(cache, "1997/01/03 00:00:00", "1996/12/30 00:00:00", algo::UNTIME_PER_SEC / 2 -1);
    TstampCache_CheckTimeRange(cache, "1999/12/30 00:00:00", "2000/01/03 00:00:00", algo::UNTIME_PER_SEC / 2 -1);
    TstampCache_CheckTimeRange(cache, "1980/12/30 00:00:00", "2040/01/03 00:00:00", algo::SECS_PER_DAY * (algo::UNTIME_PER_SEC / 2 -1 ));
}

// ----------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_TimeConversion() {
    // conversion to time struct
    algo::SetTz("GMT");
    TestTimeToTimeStruct<algo::UnixTime>(         1, 0);
    TestTimeToTimeStruct<algo::UnTime>  (1000000000, 0);
    TestDiffToTimeStruct<algo::UnDiff>  (1000000000, 0);
    // conversion from time struct
    TestTimeStructToTime<algo::UnixTime>(         1, 0,algo::ToUnixTime);
    TestTimeStructToTime<algo::UnTime>  (1000000000, 0,algo::ToUnTime  );
    TestTimeStructToDiff<algo::UnDiff>  (1000000000, 0,algo::ToUnDiff  );
    // conversion between types
    TestTimeConv<algo::UnixTime,algo::UnTime>(1         , 0, 1         , 1000000000, 0, algo::ToUnTime);
    TestTimeConv<algo::UnTime,algo::UnixTime>(1000000000, 0, 1000000000, 1         , 0, algo::ToUnixTime);

    // print/parse roundtrip
    time_test_beg(ut,frac,test_time,1, 0) {
        algo::UnixTime t1(test_time);
        tempstr str;
        str << t1;
        algo::UnixTime t2;
        vrfy(UnixTime_ReadStrptrMaybe(t2, str), algo_lib::_db.errtext);
        vrfyeq_(t2.value,test_time);
    } time_test_end;

    time_test_beg(ut,frac,test_time,1000000000, 0) {
        algo::UnTime t1(test_time);
        tempstr str;
        str << t1;
        algo::UnTime t2;
        vrfy(UnTime_ReadStrptrMaybe(t2, str), algo_lib::_db.errtext);
        vrfyeq_(t2.value,test_time);
    } time_test_end;

    timediff_test_beg(test_diff, 1000000000, 0) {
        algo::UnDiff t1(test_diff);
        tempstr str;
        str << t1;
        algo::UnDiff t2;
        vrfy(UnDiff_ReadStrptrMaybe(t2, str), algo_lib::_db.errtext);
        vrfyeq_(t2.value,test_diff);
    } timediff_test_end;
}

// -----------------------------------------------------------------------------

// Check that STR can be parsed as UnTime according to FORMAT,
// and that when it's printed back using same FORMAT, it results in STR.
// if PREC=0, then same roundtrip is checked for UnixTime format
static void TestParseTime(const strptr& str, const char *format, int prec=0) {
    TimeStruct time_struct;
    algo::StringIter iter(str);
    vrfy(TimeStruct_Read(time_struct, iter, format), "bad format");

    if (prec <= 9) {
        tempstr buf;
        algo::UnTime t = ToUnTime(time_struct);
        UnTime_PrintSpec(t, buf, format);
        vrfy_(buf == str);
    }
    if (prec == 0) {
        tempstr buf;
        algo::UnixTime t = ToUnixTime(time_struct);
        UnixTime_PrintSpec(t, buf, format);
        vrfy_(buf == str);
    }
}

// -----------------------------------------------------------------------------

// Check that IN, when parsed as UnTime, and printed back out,
// results in RESULT
static void ParseUnTimeStr(const strptr& in, strptr result) {
    if (!elems_N(result)) {
        result = in;
    }
    algo::UnTime test;
    vrfy(UnTime_ReadStrptrMaybe(test, in), algo_lib::_db.errtext);
    tempstr out;
    UnTime_Print(test, out);
    vrfyeq_(out,result);
}

// -----------------------------------------------------------------------------

// Check that reading IN as UnDiff and printing it back results in RESULT
static void TestParseUnDiff(const strptr& in, strptr result) {
    if (!elems_N(result)) {
        result = in;
    }
    algo::StringIter iter(in);
    TimeStruct time_struct;
    vrfy(TimeStruct_Read(time_struct, iter, "%-%T") ,tempstr()<<"can't parse "<<in);
    algo::UnDiff test = ToUnDiff(time_struct);
    tempstr    out;
    UnDiff_Print(test, out);
    vrfyeq_(out,result);
}

// -----------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_TstampCache(){
    TstampCache_CheckTimeSpec("%Y/%m/%d %T");
    TstampCache_CheckTimeSpec("%Y%m%d-%H-%M:%S.%X");
    TstampCache_CheckTimeSpec("%Y%m%d-%H-%M:%S.%.9X");
    TstampCache_CheckTimeSpec("%Y%m%d:%H:%M:%S.%X");
    TstampCache_CheckTimeSpec("%Y%m%d-%H:%M:%S.%.3x");
    TstampCache_CheckTimeSpec("%.3x%Y%m%d-%H:%M:%S.");
    TstampCache_CheckTimeSpec("%M:%S.%X");
}

// -----------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_PrintUnTime(){
    algo::SetTz("GMT");
    vrfyeq_((tempstr() << algo::UnTime(4)),"1970-01-01T00:00:00.000000004");
}

// -----------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_ParseUnTime() {
    ParseUnTimeStr("1970-02-01T12:13:14.567890123","1970-02-01T12:13:14.567890123");
    ParseUnTimeStr("1984-12-31T12:13:14.567","1984-12-31T12:13:14.567");
    ParseUnTimeStr("2013-11-21T12:13:14.000123000crap","2013-11-21T12:13:14.000123");
    ParseUnTimeStr("2013-11-21" ,"2013-11-21T00:00:00");
    ParseUnTimeStr("2013/11/21" ,"2013-11-21T00:00:00");
    TestParseUnDiff("12:13:14.567890123","12:13:14.567890123");
    TestParseUnDiff("13:14.567890123", "00:13:14.567890123");
    TestParseUnDiff("14.567890123",  "00:00:14.567890123");
    TestParseUnDiff("00.567890123", "00:00:00.567890123");
    TestParseUnDiff("00:00:00", "00:00:00");
    TestParseTime("May 11 1986 01:20:30"          , "%b %d %Y %H:%M:%S");
    TestParseTime("Jan 11 1986 01:20:30"          , "%b %d %Y %H:%M:%S");
    TestParseTime("Jan 11 1986 01:20:30.000000001", "%b %d %Y %H:%M:%S.%X", 9);
    TestParseTime("Jan 11 1986 01:20:30.999999999", "%b %d %Y %H:%M:%S.%X", 9);
    TestParseTime("Jan 11 1986 01:20:30.0000001"  , "%b %d %Y %H:%M:%S.%X", 7);
    TestParseTime("Jan 11 1986 01:20:30.9999999"  , "%b %d %Y %H:%M:%S.%X", 7);
    TestParseTime("Jan 11 1986 01:20:30.00001"    , "%b %d %Y %H:%M:%S.%X", 5);
    TestParseTime("Jan 11 1986 01:20:30.99999"    , "%b %d %Y %H:%M:%S.%X", 5);
    TestParseTime("Jan 11 1986 01:20:30.001"      , "%b %d %Y %H:%M:%S.%X", 3);
    TestParseTime("Jan 11 1986 01:20:30.999"      , "%b %d %Y %H:%M:%S.%X", 3);
    TestParseTime("%Feb 28 99"                    , "%%%b %d %y");
    TestParseTime("2004/03/10 09:30:00"           , "%Y/%m/%d %H:%M:%S");
    TestParseTime("2013/02/05 10:10:13"           , "%Y/%m/%d %T");
    TestParseTime("Jun 10, 2004:  09:30"          , "%b %d, %Y:  %H:%M");
    TestParseTime("FFF"                           , "FFF");
    TestParseTime("May 11 1986 01:20:30"          , "%B %d %Y %H:%M:%S");
    TestParseTime("January 11 1986 01:20:30"          , "%B %d %Y %H:%M:%S");
    TestParseTime("January 11 1986 01:20:30.000000001", "%B %d %Y %H:%M:%S.%X", 9);
    TestParseTime("January 11 1986 01:20:30.999999999", "%B %d %Y %H:%M:%S.%X", 9);
    TestParseTime("January 11 1986 01:20:30.0000001"  , "%B %d %Y %H:%M:%S.%X", 7);
    TestParseTime("January 11 1986 01:20:30.9999999"  , "%B %d %Y %H:%M:%S.%X", 7);
    TestParseTime("January 11 1986 01:20:30.00001"    , "%B %d %Y %H:%M:%S.%X", 5);
    TestParseTime("January 11 1986 01:20:30.99999"    , "%B %d %Y %H:%M:%S.%X", 5);
    TestParseTime("January 11 1986 01:20:30.001"      , "%B %d %Y %H:%M:%S.%X", 3);
    TestParseTime("January 11 1986 01:20:30.999"      , "%B %d %Y %H:%M:%S.%X", 3);
    TestParseTime("%February 28 99"                    , "%%%B %d %y");
    TestParseTime("June 10, 2004:  09:30"          , "%B %d, %Y:  %H:%M");
}

// -----------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_DayName() {
    vrfy_(algo::GetMonthNameZeroBasedShort(0)=="Jan");
    vrfy_(algo::GetMonthNameZeroBased(0)=="January");
    vrfy_(algo::GetMonthNameZeroBasedShort(11)=="Dec");
    vrfy_(algo::GetMonthNameZeroBased(11)=="December");
    vrfy_(algo::GetWeekday("Sun")==0);
    vrfy_(algo::GetWeekday("Sat")==6);
    vrfy_(algo::GetWeekdayName(0)=="Sun");
    vrfy_(algo::GetWeekdayName(6)=="Sat");
    for (int i=0; i<7; i++) {
        vrfy_(algo::GetWeekday(algo::GetWeekdayName(i))==i);
    }
}

// -----------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_CurrentTime() {
    double totaldiff = 0;
    int niter=100;
    for (int i=0; i<niter; i++) {
        algo::UnTime t = algo::CurrUnTime();
        algo::UnixTime t2 = algo::CurrUnixTime();
        double diff = abs(t.value/1e9 - t2.value);
        totaldiff += diff;
        verblog(Keyval("untime",t)
                <<Keyval("unixtime",t2)
                <<Keyval("diff",diff)
                <<Keyval("totaldiff",totaldiff));
        // check that individual sample difference between time values
        // is not crazy
        vrfy_(diff < 10.0);
        algo::SleepMsec(5);
    }
    // max difference should be no more than half a second on average
    // add some leeway in case the process gets interrupted
    prlog("avg diff between UnTime and UnixTime: "<<totaldiff/niter);
    vrfy_(totaldiff/niter < 0.9);
}

// -----------------------------------------------------------------------------

void atf_unit::unittest_algo_lib_TimeConvert() {
    {
        algo::UnixDiff d(1);
        vrfy_(ToWDiff(d).value == algo::WTIME_PER_SEC);
    }
    {
        algo::UnDiff d(algo::UNTIME_PER_SEC);
        vrfy_(ToWDiff(d).value == algo::WTIME_PER_SEC);
        vrfy_(ToSecs(d) == 1.0);
    }
    {
        algo::WDiff d(algo::WTIME_PER_SEC);
        vrfy_(ToUnixDiff(d).value == 1);
        vrfy_(ToUnDiff(d).value == algo::UNTIME_PER_SEC);
        vrfy_(ToWDiff(ToUnDiff(d)).value == d.value);
        vrfy_(ToWDiff(ToUnixDiff(d)).value == d.value);
        vrfy_(ToSecs(d) == 1.0);
    }
    {
        algo::UnTime t(0);
        vrfy_(ToWTime(t).value == algo::WTIME_OFFSET);
        vrfy_(ToUnTime(ToWTime(t)).value == t.value);
    }
    {
        algo::UnTime t(0);
        vrfy_(ToWTime(t).value == algo::WTIME_OFFSET);
    }
    {
        algo::UnixTime t(0);
        vrfy_(ToWTime(t).value == algo::WTIME_OFFSET);
        vrfy_(ToUnixTime(ToWTime(t)).value == t.value);
    }
}

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
// Source: cpp/lib/algo/time.cpp -- UnTime / UnDiff functions
//
// Created By: alexei.lebedev hayk.mkrtchyan
// Authors: alexei.lebedev
// Recent Changes: alexei.lebedev hayk.mkrtchyan
//

static int ReadInt(StringIter &parser, int width) {
    if (width<=0) {
        width=0x7fffffff;
    }
    int  number_so_far = 0;
    int  chars_read    = 0;
    bool isneg         = SkipChar(parser, '-');
    (void)SkipChar(parser, '+');
    do {
        char c        = parser.Peek();
        if (!algo_lib::DigitCharQ(c)) break;
        int  prev     = number_so_far;
        number_so_far = number_so_far*10+(int(c)-'0');
        parser.GetChar();
        chars_read++;
        if (prev>number_so_far) {
            return 0;
        }
    } while (chars_read < width);
    return i32_NegateIf(number_so_far,isneg);
}

static int ReadNS(StringIter &parser, int width) {
    int start = parser.index;
    int ns = ReadInt(parser,width);
    int chars_read = parser.index - start;
    u32 mul = 1;
    switch(9 - chars_read) {
    case 0: mul  = 1; break;
    case 1: mul  = 10; break;
    case 2: mul  = 100; break;
    case 3: mul  = 1000; break;
    case 4: mul  = 10000; break;
    case 5: mul  = 100000; break;
    case 6: mul  = 1000000; break;
    case 7: mul  = 10000000; break;
    case 8: mul  = 100000000; break;
    case 9: mul  = 1000000000; break;
    default: mul = INT_MAX; break;
    }
    return ns * mul;
}

// -----------------------------------------------------------------------------

bool algo::TimeStruct_Read(TimeStruct &out, StringIter &iter, const strptr& spec) {
    int    field_width  = 10;
    bool   percent      = false;
    for (int i=0; i<elems_N(spec); i++) {
        if (percent) {
            switch (spec[i]) {
            case '%':
                if (!SkipChar(iter, '%')) {
                    return 0;
                }
                break;
            case 'b':
                {
                    iter.Ws();
                    if (iter.index+3 > elems_N(iter.expr)) {
                        return 0;// out-of-bounds
                    }
                    strptr ident=qGetRegion(iter.expr, iter.index,3);
                    iter.index+=elems_N(ident);
                    out.tm_mon = GetMonthZeroBased(ident);
                    if (out.tm_mon==-1) {
                        return 0;
                    }
                }
                break;
            case 'a':
                {
                    iter.Ws();
                    strptr ident=qGetRegion(iter.expr, iter.index,3);
                    if (iter.index+3 > elems_N(iter.expr)) {
                        return 0;// out-of-bounds
                    }
                    iter.index+=elems_N(ident);
                    out.tm_wday = GetWeekday(ident);
                    if (out.tm_wday==-1) {
                        return 0;
                    }
                }
                break;
            case 'm':
                field_width = i32_Min(field_width,2);
                out.tm_mon = i32_Max(ReadInt(iter,field_width)-1, 0);
                break;
            case 'T':
                {
                    out.tm_sec = ReadInt(iter,10);
                    if (SkipChar(iter, ':')) {
                        out.tm_min = out.tm_sec;
                        out.tm_sec = ReadInt(iter,10);
                        if (SkipChar(iter, ':')) {
                            out.tm_hour = out.tm_min;
                            out.tm_min = out.tm_sec;
                            out.tm_sec=ReadInt(iter,10);
                        }
                    }
                    if (SkipChar(iter, '.')) {
                        out.tm_nsec = ReadNS(iter, field_width);
                    }
                }
                break;
            case 'd':
                field_width = i32_Min(field_width,2);
                out.tm_mday = ReadInt(iter,field_width);
                if (out.tm_mday == -1) {
                    return 0;
                }
                break;
            case 'y':
                out.tm_year = ReadInt(iter,field_width);
                if (out.tm_year == -1) {
                    return 0;
                }
                if (out.tm_year>=0 && out.tm_year<=69) {
                    out.tm_year += 100;
                } else if (out.tm_year>=70 && out.tm_year<=99) {
                } else if (out.tm_year>=1970 && out.tm_year<2038) {
                    out.tm_year -= 1900;
                } else {
                    return 0;
                }
                break;
            case 'Y':
                field_width = i32_Min(field_width,4);
                out.tm_year = ReadInt(iter,field_width);
                out.tm_year -= 1900;
                break;
            case 'H':
                field_width = i32_Min(field_width,2);
                out.tm_hour = ReadInt(iter,field_width);
                break;
            case 'M':
                field_width = i32_Min(field_width,2);
                out.tm_min = ReadInt(iter,field_width);
                break;
            case 'S':
                field_width = i32_Min(field_width,2);
                out.tm_sec = ReadInt(iter,field_width);
                break;
            case 'X':
                out.tm_nsec = ReadNS(iter, field_width);
                break;
            case '-':
                out.tm_neg = SkipChar(iter, '-');
                if (!out.tm_neg) {
                    (void)SkipChar(iter, '+');
                }
                break;
            default:
                return 0;
                break;
            }
            percent=false;
        } else if (spec[i] == '%') {
            percent=true;
            if (i<elems_N(spec)-2 && spec[i+1]=='.' && algo_lib::DigitCharQ(spec[i+2])) {
                field_width = spec[i+2] - '0';
                i+=2;
            } else {
                field_width = 10;
            }
        } else if (spec[i] == '/') {
            // / separator matches both / and -
            bool ok = SkipChar(iter, '/')
                || SkipChar(iter, '-');
            if (!ok) {
                return false;
            }
        } else if (spec[i] == ' ') {
            int index=iter.index;
            iter.Ws();
            if (i>0 && spec[i-1] == ' ') {// multiple spaces in the template
                continue;
            } else {
                if (iter.index-index<1) {
                    return 0;   // no ws found
                }
            }
        } else {
            if (!SkipChar(iter, spec[i])) {
                return 0;
            }
        }
    }
    return true;
}

// -----------------------------------------------------------------------------

static TimeStruct SecsToTs(i64 secs, i64 nsecs) {
    TimeStruct ret;
    ret.tm_neg = secs < 0;
    secs = Abs(secs);
    ret.tm_nsec  = int(Abs(nsecs));
    ret.tm_sec   = int(secs % 60);
    secs        /= 60;
    ret.tm_min   = int(secs % 60);
    secs        /= 60;
    ret.tm_hour  = int(secs);
    return ret;
}

TimeStruct algo::ToTimeStruct(UnDiff   U) {
    return SecsToTs(U.value / UNTIME_PER_SEC  , U.value % UNTIME_PER_SEC  );
}
TimeStruct algo::ToTimeStruct(UnixDiff U) {
    return SecsToTs(U.value, 0);
}

UnixDiff algo::ToUnixDiff(const TimeStruct &S) {
    return UnixDiff(i64_NegateIf(i64(S.tm_hour) * SECS_PER_HOUR  + i64(S.tm_min) * SECS_PER_MIN + i64(S.tm_sec),S.tm_neg));
}

UnDiff algo::ToUnDiff(const TimeStruct &S) {
    return UnDiff(i64_NegateIf((i64(S.tm_hour) * SECS_PER_HOUR   + i64(S.tm_min) * SECS_PER_MIN + i64(S.tm_sec)) * UNTIME_PER_SEC   + i64(S.tm_nsec),S.tm_neg));
}

// uses gettimeofday, returns UTC seconds + fractions
// UTC time for midnight; Local timezone is used in computation
void algo::SetTz(strptr tz) {
    tempstr tzspec = tempstr() << ":" << tz;
    setenv("TZ", Zeroterm(tzspec), true);
    tzset();
}

// -----------------------------------------------------------------------------

const UnixTime algo::LocalDate(UnixTime in) {
    struct tm t = GetLocalTimeStruct(in);
    t.tm_hour   = 0;
    t.tm_min    = 0;
    t.tm_sec    = 0;
    t.tm_isdst  = -1;
    return UnixTime(mktime(&t));
}

// -----------------------------------------------------------------------------

TimeStruct algo::GetLocalTimeStruct(UnixTime U) {
    const time_t secs = U.value;
    TimeStruct ret;
    localtime_r(&secs, &ret);
    return ret;
}

// -----------------------------------------------------------------------------

TimeStruct algo::GetLocalTimeStruct(UnTime U) {
    time_t secs  = time_t(U.value / UNTIME_PER_SEC);
    int    nsecs = int(U.value % UNTIME_PER_SEC);
    // correct negative value
    if (nsecs<0) {
        --secs;
        nsecs += UNTIME_PER_SEC;
    }
    TimeStruct ret;
    ret.tm_nsec = nsecs;
    localtime_r(&secs, &ret);
    return ret;
}

// -----------------------------------------------------------------------------

const UnTime algo::LocalDate(UnTime in) {
    TimeStruct t = GetLocalTimeStruct(in);
    t.tm_hour   = 0;
    t.tm_min    = 0;
    t.tm_sec    = 0;
    t.tm_nsec   = 0;
    t.tm_isdst  = -1;
    return ToUnTime(t);
}

// -----------------------------------------------------------------------------

TimeStruct algo::GetGMTimeStruct(UnTime U) {
    time_t secs  = time_t(U.value / UNTIME_PER_SEC);
    int    nsecs = int(U.value % UNTIME_PER_SEC);
    // correct negative value
    if (nsecs<0) {
        --secs;
        nsecs += UNTIME_PER_SEC;
    }
    TimeStruct ret;
    ret.tm_nsec = nsecs;
    gmtime_r(&secs, &ret);
    return ret;
}

UnixTime algo::ToUnixTime(const TimeStruct &S) {
    UnixTime ret;
    i64 secs = mktime((tm*)&S);
    if (secs != -1) {
        ret.value = secs;
    }
    return ret;
}

UnTime algo::ToUnTime(const TimeStruct &S) {
    UnTime ret;
    i64 secs = mktime((tm*)&S);
    if (secs != -1) {
        ret.value = secs * UNTIME_PER_SEC + S.tm_nsec;
    }
    return ret;
}

// -----------------------------------------------------------------------------

static const char *weekdays_names[] = {
    "Sun","Mon","Tue","Wed","Thu","Fri","Sat"
};

// empty string -> 0
// invalid weekday -> -1
// All other weeks days map to 0..6, numbers compatible with struct tm's tm_wday field
int algo::GetWeekday(strptr wday){
    rep_(i,_array_count(weekdays_names)) {
        if (StrEqual(wday, weekdays_names[i],false)) return i;
    }
    return -1;
}

const strptr algo::GetWeekdayName(int index){
    return LIKELY(u32(index) < u32(_array_count(weekdays_names))) ? weekdays_names[index] : strptr();
}
static const char *month_names[] = {
    "January","February","March","April","May","June","July","August","September","October","November","December",
};
static const int month_days[] = {
    31,29,31,30,31,30,31,31,30,31,30,31
};
int algo::GetMonthZeroBased(strptr month) {
    rep_(i,_array_count(month_names)) {
        strptr m = month_names[i];
        if (elems_N(month) <= elems_N(m) && StartsWithQ(m, month,false)) {
            return i;
        }
    }
    return -1;
}

// Inverse mapping to abbreviated month names (Jan, Feb, etc.)
// If short_name==false, returns full name (January, February, etc.)
const strptr algo::GetMonthNameZeroBased(int index) {
    return LIKELY(u32(index) < u32(_array_count(month_names))) ? strptr(month_names[index]) : strptr();
}

const strptr algo::GetMonthNameZeroBasedShort(int index) {
    return u32(index) < u32(_array_count(month_names)) ? strptr((char*)month_names[index],3) : strptr();
}

// -----------------------------------------------------------------------------

// DateCache -- Roughly 200x faster LocalDate
const UnTime algo::DateCache_LocalDate(algo::DateCache &dc, UnTime in) {
    if (!(in.value >= dc.min.value && in.value < dc.max.value)) {
        UnTime date = LocalDate(in);
        dc.min = date;
        // Because some days have 23 hours, let the cache be slow
        // in the last hour of the short day (which is a Saturday anyway)
        dc.max.value = date.value + UNTIME_PER_SEC * SECS_PER_HOUR * 23;
    }
    return dc.min;
}

// -----------------------------------------------------------------------------

UnTime algo::CurrUnTime() {
    struct timeval tv;
    ZeroBytes(tv);
    gettimeofday(&tv,NULL);
    UnTime ret;
    ret.value = (i64(tv.tv_sec) * 1000000 + tv.tv_usec) * 1000;
    return ret;
}

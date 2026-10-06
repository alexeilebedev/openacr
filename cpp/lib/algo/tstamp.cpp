// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2017-2019 NYSE | Intercontinental Exchange
//
// License: Apache
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Contacting ICE: <https://www.theice.com/contact>
// Target: algo_lib (lib) -- Support library for all executables
// Exceptions: NO
// Source: cpp/lib/algo/tstamp.cpp -- timestamp cache
//

#include "include/algo.h"


//Allowing only for fixed length formats.
void algo::tstamp_cache_Init(TstampCache& cache, strptr spec, bool gmt){
    cache.spec = spec;
    cache.gmtQ = gmt;
    cstring shortest;
    cstring longest;
    TimeStruct ts;
    ts.tm_year = 117;
    ts.tm_mon = 11;
    ts.tm_mday  = 8;
    ts.tm_hour = 0;
    ts.tm_min = 0;
    ts.tm_sec = 0;
    ts.tm_nsec = 0;
    TimeStruct_Print(ts, shortest, cache.spec);
    ts.tm_hour = 22;
    ts.tm_min = 44;
    ts.tm_sec = 55;
    ts.tm_nsec = 999999999;
    TimeStruct_Print(ts, longest, cache.spec);
    cache.cacheable = ch_N(shortest) == ch_N(longest);
    if(cache.cacheable){
        cache.min_location = FindStr(longest, "44");
        cache.sec_location = FindStr(longest, "55");
        i32_Range nano_range1 = ch_FindFirst(longest, '9');
        i32_Range nano_range2 = ch_FindLast(longest,  '9');
        cache.nano_location = nano_range1.beg;
        cache.nano_size = nano_range2.end - nano_range1.beg;
    }
}

static algo::TimeStruct GetTimeStruct(algo::TstampCache& cache, algo::UnTime t){
    return cache.gmtQ ? algo::GetGMTimeStruct(t) : algo::GetLocalTimeStruct(t);
}


static void tstamp_cache_PlaceMS(algo::TstampCache& cache, u32 seconds, strptr out){
    if(cache.min_location != -1){
        u32 min = seconds / 60;
        out[cache.min_location] = min / 10 + '0';
        out[cache.min_location+1] = min % 10 + '0';
    }
    if(cache.sec_location != -1){
        u32 sec = seconds % 60;
        out[cache.sec_location] = sec / 10 + '0';
        out[cache.sec_location+1] = sec % 10 + '0';
    }
}

inline static void tstamp_cache_PlaceNanos(algo::TstampCache& cache, u32 nanos, strptr out){
    if(cache.nano_location != -1){
        u64 usec_buf64;
        int nano    = nanos / 10;
        int quot    = nanos % 10;
        usec_buf64    = (algo::u32_CvtLestr8Small(nano) + 0x3030303030303030); //convert microsecs to string
        u8  *usec_buf = (u8*)&usec_buf64;
        int max = i32_Min(cache.nano_size, 8);
        memcpy(out.elems + cache.nano_location ,usec_buf, max);
        if(cache.nano_size > 8){
            out[cache.nano_location + 8] = quot + '0';
        }
    }
}

inline static void tsamp_cache_Update(algo::TstampCache& cache, algo::UnTime t){
    TimeStruct ts = GetTimeStruct(cache, t);
    u64 delta = (ts.tm_min * 60 + ts.tm_sec ) * algo::UNTIME_PER_SEC + ts.tm_nsec;
    ts.tm_min = 0;
    ts.tm_sec = 0;
    ts.tm_nsec = 0;
    ch_RemoveAll(cache.cached);
    TimeStruct_Print(ts, cache.cached, cache.spec);
    cache.valid_beg.value = t.value - delta;
    cache.valid_end.value = cache.valid_beg.value + u64(algo::UNTIME_PER_SEC) * algo::SECS_PER_HOUR;
}


void algo::tstamp_cache_Print(algo::TstampCache& cache, UnTime t, cstring& out){
    if(cache.cacheable){
        if (t.value < cache.valid_beg.value || t.value >= cache.valid_end.value){
            tsamp_cache_Update(cache, t);
        }
        int start = out.ch_n;
        out << cache.cached;
        strptr str = ch_GetRegion(out, start, ch_N(cache.cached));
        u64 offset_nsec = t.value - cache.valid_beg.value;
        u64 nano = offset_nsec % 1000000000;
        u64 sec = offset_nsec / 1000000000;
        tstamp_cache_PlaceNanos(cache, nano, str);
        tstamp_cache_PlaceMS(cache, sec , str);
    }else{
        TimeStruct ts = GetTimeStruct(cache, t);
        TimeStruct_Print(ts, out, cache.spec);
    }
}

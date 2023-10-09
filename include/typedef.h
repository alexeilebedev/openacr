// Copyright (C) 2017-2019 NYSE | Intercontinental Exchange
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
// Target: algo_lib (lib) -- Support library for all executables
// Exceptions: NO
// Header: include/typedef.h -- Typedefs
//

#pragma once

#ifdef WIN32
typedef struct DIR DIR;// TODO:IMPLEMENT ME
typedef void*              pthread_t;// TODO:IMPLEMENT ME
typedef int              pid_t;// TODO:IMPLEMENT ME
#pragma warning(disable:4244) // conversion, possible loss of data
#pragma warning(disable:4800) // forcing value to bool
#else
typedef __uint128_t        u128;
#endif

typedef unsigned char      byte;
typedef   signed char      i8;
typedef unsigned char      u8;
typedef   signed short     i16;
typedef unsigned short     u16;
typedef   signed int       i32;
typedef unsigned int       u32;
typedef          float     f32;
typedef          double    f64;
typedef     long double    f80;
#ifdef WIN32
typedef __int64            i64;
typedef i64                ssize_t;
typedef unsigned __int64   u64;
#else
typedef unsigned long      u64;
typedef   signed long      i64;
#endif
typedef i64                int_ptr;
typedef u64                uint_ptr;
typedef void               *thread_ret_t;

#if defined(__MACH__)
typedef u64 off64_t;
#endif

namespace algo_lib {
    struct FLogcat;
}

namespace algo {
    struct cstring;
    struct tempstr;
    template<class T> struct aryptr;
    typedef aryptr<u8>    memptr;
    typedef aryptr<char>  strptr;
    struct StringDesc;
    struct ImrowPtr;
    struct Tuple;
    struct SchedTime;
    typedef void (*TuplecmdFcn)(Tuple&);
    typedef void(*InitFcn)(void* str);
    typedef bool(*SetnumFcn)(void* str, i64 num);
    typedef i64(*Geti64Fcn)(void* str, bool &out_ok);
    typedef algo::aryptr<char>(*GetaryFcn)(void* str);
    typedef bool (*ImdbInsertStrptrMaybeFcn)(strptr str);
    typedef void (*PrlogFcn)(algo_lib::FLogcat *logcat, algo::SchedTime tstamp, strptr str);
    typedef void (*ImdbStepFcn)();
    typedef void (*ImdbMainLoopFcn)();
    typedef void (*ImdbGetTraceFcn)(cstring &str);
    typedef void (*ImrowXrefXFcn)(algo::ImrowPtr);
    typedef int (*ImrowNItemsFcn)();
    typedef void (*ImrowPrintFcn)(algo::ImrowPtr data, algo::cstring &lhs);
    typedef algo::ImrowPtr (*ImrowGetElemFcn)(algo::memptr pkey);
    typedef algo::ImrowPtr (*ImrowRowidFindFcn)(int i);

    template<class T> struct  aryptr {
        typedef          T               ValueType;
        T    *elems;
        i32 n_elems;

        aryptr(const T *e, i32 in_n);
        aryptr(const char *e);
        aryptr();
        T              &operator [](u32 idx) const;
    };
}
using algo::strptr;
using algo::tempstr;

// On Windows we must use stat64 to get 64-bit file sizes,
// so on other platforms we're forced to use this type as well.
// Due to use of macros, `struct stat` cannot be used on Windows.
#ifdef WIN32
typedef struct _stat64 StatStruct;
#else
typedef struct stat StatStruct;
#endif

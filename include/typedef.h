// Copyright (C) 2025-2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2023 Astra
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
// Supplied by cpp/lib/algo/macos.cpp, which is the whole of the macOS
// adaptation layer.  Darwin's libc stops at pipe, so a caller asking for
// O_CLOEXEC on both ends needs this declaration to reach that definition.
int pipe2(int fd[2], int flags);
// Darwin has no posix_fallocate; the layer answers with F_PREALLOCATE.
int posix_fallocate(int fd, off_t offset, off_t len);
// Darwin has no advice to populate page tables; madvise refuses the value with
// EINVAL, and the caller counts the refusal.
#define MADV_POPULATE_WRITE 23
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
    struct Alloc;
    struct SchedTime;
    typedef void(*InitFcn)(void* str);
    typedef bool(*SetnumFcn)(void* str, i64 num);
    typedef i64(*Geti64Fcn)(void* str, bool &out_ok);
    typedef algo::aryptr<char>(*GetaryFcn)(void* str);
    typedef bool (*ImdbInsertStrptrMaybeFcn)(strptr str);
    typedef bool (*ImdbRemoveStrptrMaybeFcn)(strptr str);
    typedef void (*PrlogFcn)(algo_lib::FLogcat *logcat, algo::SchedTime tstamp, strptr str);
    typedef void (*ImdbStepFcn)();
    typedef void (*ImdbMainLoopFcn)();
    typedef void (*ImdbGetTraceFcn)(cstring &str);
    typedef void (*ImrowXrefXFcn)(algo::ImrowPtr);
    typedef int (*ImrowNItemsFcn)();
    typedef void *(*BeginAllocFcn)(void *ctx, i32 len);
    typedef void (*EndAllocFcn)(void *ctx, void *ptr, i32 len);
    typedef void (*ImrowPrintFcn)(algo::ImrowPtr data, algo::cstring &lhs);
    typedef algo::ImrowPtr (*ImrowRowidFindFcn)(int i);

    template<class T> struct  aryptr {
        typedef T ValueType;
        T    *elems;
        i64 n_elems;

        aryptr(const T *e, i64 in_n);
        aryptr();
        T &operator [](u64 idx) const;
    };
    // specialization for char
    template<> struct aryptr<char> {
        typedef char ValueType;
        char *elems;
        i32 n_elems;

        aryptr() : elems(NULL), n_elems(0) {}
        aryptr(const char *e) { elems=(char*)e; n_elems=e ? strlen(e) : 0; }
        aryptr(const char *e, i64 n) : elems((char*)e), n_elems(n) {}
        char &operator [](u64 idx) const { return elems[idx]; }
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

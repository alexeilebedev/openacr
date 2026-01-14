// Copyright (C) 2023-2026 AlgoRND
// Copyright (C) 2020-2023 Astra
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2008-2013 AlgoEngineering LLC
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
// Header: include/define.h -- Macros
//

#pragma once

// Usage:
// In C++ files (where default arguments are not allowed)
//    void F(int blah DFLTVAL(0))
// In headers, this is translated to
//    void F(int blah = 0)
#define DFLTVAL(X)

// Usage:
// In C++ files (where function attrs are not allowed):
//    void F() FUNCATTR(__attribute__((nonnull)))
// In headers:
//    void F() __attribute__((nonnull))
#define FUNCATTR(X)

#define CACHE_LINE                  64

#define SQL_QUOTE                    "\""

#define PREPROC_TO_STRING2(x)        #x
#define PREPROC_TO_STRING(x)         PREPROC_TO_STRING2(x)
#define PREPROC_LOCATION()           __FILE__ ":" PREPROC_TO_STRING(__LINE__)

#define ANONYMOUS_JOIN2(a,b)         a##b
#define ANONYMOUS_JOIN1(a,b)         ANONYMOUS_JOIN2(a,b)
#define ANONYMOUS                    ANONYMOUS_JOIN1(anon,__LINE__)
#define ANONYMOUS2                   ANONYMOUS_JOIN1(anon_,__LINE__)

#ifdef WIN32

#define V_UNUSED
#define UNLIKELY(x)                  (x)
#define LIKELY(x)                    (x)

#define F_NONNULL

#define T_MAY_ALIAS
// todo: clean all of these up...
#pragma warning(disable:4481) // non-standard extension used: override specified
#pragma warning(disable:4146) // unary minus applied to unsigned type, result still unsigned
#pragma warning(disable:4127) // conditional expression is constant (while (true))
#pragma warning(disable:4512) // assignment operator could not be generated
#pragma warning(disable:4065) // switch sattement contains no case labels (gen code)
#pragma warning(disable:4267) // conversion from larger to smaller integer type, possible loss of data
#pragma warning(disable:4480) // non-standard extension used: override specified
#define __attribute__(x)

// MSC doesn't have __func__, instead it has __FUNCTION__
// msdn.microsoft.com/en-us/library/b0084kay(v=vs.80).aspx
#define __func__                     __FUNCTION__

#else  // GCC

#define GCC_VERSION                 (__GNUC__ * 10 + __GNUC_MINOR__)

#if GCC_VERSION < 40
#error only gcc
#endif

#define T_MAY_ALIAS                 __attribute__((__may_alias__))
#define V_UNUSED                    __attribute__((unused))
#define F_NONNULL                   __attribute__((nonnull))

#define UNLIKELY(x)                  __builtin_expect(!!(x), 0)
#define LIKELY(x)                    __builtin_expect(!!(x), 1)

#endif

#define algo_assert(TTT)          switch(0) {case 0:; case(TTT):;}


#define _array_count(array)         int(sizeof(array)/sizeof(array[0]))

#define ssizeof(X)                  ptrdiff_t(sizeof(X))

// # of u128s it takes to represent this many elements of type X
#define sizeu128(X,N)               ((sizeof(X)*N+15)/16)

// gcc will complain about improper use of offsetof macro
// for non-POD types. using 128 instead of 0 gets around the error.
// MSC, on the other hand, needs a zero, otherwise it doesn't consider the
// expression a compile-time constant.
#if defined(WIN32)
#define _offset_of(T,FLD)           (size_t)((char*)&((T*)NULL)->FLD      )
#else
#define _offset_of(T,FLD)           (size_t)offsetof(T,FLD)
#endif

#define   rep_(i, max)   for (i32 i = 0              ;i < (max)   ; i++)  // rep from 0 to lim without caching
#define  frep_(i, max)   for (i32 i = 0, i##lim=(max); i < i##lim ; i++)  // rep from 0 to lim with caching
#define frep1_(i, max)   for (i32 i = 1, i##lim=(max); i < i##lim ; i++)  // rep from 1 to lim with caching
#define  rrep_(i, max)   for (i32 i = (max)          ; i-->0      ;)      // reverse rep
#define rrep1_(i, max)   for (i32 i = (max)          ; --i>0      ;)      // reverse rep down to 1

// Walk over array by reference
#define ind_beg_aryptr(type,e,ary)                      \
    {                                                   \
    algo::aryptr<type> e##temp(ary);                    \
    for (i32 e##i=0; e##i < e##temp.n_elems; e##i++) {  \
    type &e = e##temp.elems[e##i];

#define ind_end_aryptr } }

// -----------------------------------------------------------------------------

#define ind_beg(CURS,VAR,...) { CURS VAR##i; CURS##_Reset(VAR##i,__VA_ARGS__); for(; CURS##_ValidQ(VAR##i); CURS##_Next(VAR##i)) { \
    CURS::ChildType &VAR=CURS##_Access(VAR##i);

#define ind_end }}

// access cursor state
#define ind_curs(x)  (x##i)

// *vrfy*(x,...) x must be true or an exception of type ErrorX is thrown.
// There is no exception hierarchy. ErrorX holds a string value representing error text.
#define vrfy_(a)           { if (UNLIKELY(!(a))) algo::Throw(#a          , algo::Errcode()); }
#define vrfy(a,b)          { if (UNLIKELY(!(a))) algo::Throw( b          , algo::Errcode()); }
#define errno_vrfy_(a)     { if (UNLIKELY(!(a))) algo::Throw(#a          , algo::FromErrno(errno)); }
#define errno_vrfy(a,b)    { if (UNLIKELY(!(a))) algo::Throw( b          , algo::FromErrno(errno)); }
#define vrfyeq(a,b,c)      vrfy((a)==(b), tempstr()<< c << ": " #a " should be "<<(b)<<", not "<<(a))
#define vrfyeq_(a,b)       vrfy((a)==(b), tempstr()<<      ": " #a " should be "<<(b)<<", not "<<(a))

// Create u32 corresponding to a 4-, 3-, 2-character string
// Example: LE_STR4("ABCD")
#define LE_STR2(C0,C1)              u32(                          (C1) << 8 | (C0))
#define LE_STR3(C0,C1,C2)           u32(             (C2) << 16 | (C1) << 8 | (C0))
#define LE_STR4(C0,C1,C2,C3)        u32((C3) << 24 | (C2) << 16 | (C1) << 8 | (C0))

//
// Create u64 corresponding to a 5-, 6-, 7-, 8-character string
//
//
#define LE_STR5(C0,C1,C2,C3,C4)            (LE_STR4(C0,C1,C2,C3) | u64(C4)                   << 32)
#define LE_STR6(C0,C1,C2,C3,C4,C5)         (LE_STR4(C0,C1,C2,C3) | u64(LE_STR2(C4,C5))       << 32)
#define LE_STR7(C0,C1,C2,C3,C4,C5,C6)      (LE_STR4(C0,C1,C2,C3) | u64(LE_STR3(C4,C5,C6))    << 32)
#define LE_STR8(C0,C1,C2,C3,C4,C5,C6,C7)   (LE_STR4(C0,C1,C2,C3) | u64(LE_STR4(C4,C5,C6,C7)) << 32)

// -----------------------------------------------------------------------------

#define MULTICHAR_CONST2(c,d)     (                    (c)<<8 | (d))
#define MULTICHAR_CONST3(b,c,d)   (          (b)<<16 | (c)<<8 | (d))
#define MULTICHAR_CONST4(a,b,c,d) ((a)<<24 | (b)<<16 | (c)<<8 | (d))

#define log_msg_(cat,x,eol) {                                           \
        algo::SchedTime _tstamp = algo::CurrSchedTime();                \
        algo::cstring &_outstr = algo_lib::_db.log_str;                 \
        int _saved = ch_N(_outstr);                                     \
        _outstr << x;                                                   \
        if (eol) {                                                      \
            _outstr << '\n';                                            \
        }                                                               \
        algo_lib::_db.Prlog(&cat, _tstamp, algo::ch_RestFrom(_outstr,_saved)); \
        _outstr.ch_n = _saved;                                          \
    }

#define prlog_(x)             log_msg_(algo_lib_logcat_stdout,x,false)
#define prerr_(x)             log_msg_(algo_lib_logcat_stderr,x,false)

#define prlog(x)              log_msg_(algo_lib_logcat_stdout,x,true)
#define prerr(x)              log_msg_(algo_lib_logcat_stderr,x,true)
#define prcat(cat,x)          { if(UNLIKELY(algo_lib_logcat_##cat.enabled)) log_msg_(algo_lib_logcat_##cat,x,true); }

// these 4 are deprecated
#define prlog_cat(cat,x)      { if(UNLIKELY(cat.enabled)) log_msg_(cat,x,true); }
#define verblog(x)            prcat(verbose,x)
#define verblog2(x)           prcat(verbose2,x)
#define dbglog(x)             prcat(debug,x)

#ifdef constant
#undef constant
#endif

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

// compiler instructions for reordering
// not processor instructions!!!
#if  WIN32
#define sfence() __faststorefence()
#define lfence()
#define mfence() sfence()
#else
#define sfence() asm volatile("sfence" ::: "memory")
#define lfence() asm volatile("lfence" ::: "memory")
#define mfence() asm volatile("mfence" ::: "memory")
#endif

// tag to help update-hdr to insert __attribute__((nothrow))
// in header.
// this tag must be applied to the function *implementation*
// gcc only allows attributes on function declarations, and src_func
// populates headers based on global declarations it sees in source files
#define NOTHROW
// tag to help update-hdr to insert __attribute__((noreturn))
#define NORETURN

// work-arounds for apple's missing byteswap functions
#ifdef __MACH__
#define htobe16(x) OSSwapHostToBigInt16(x)
#define htole16(x) OSSwapHostToLittleInt16(x)
#define be16toh(x) OSSwapBigToHostInt16(x)
#define le16toh(x) OSSwapLittleToHostInt16(x)

#define htobe32(x) OSSwapHostToBigInt32(x)
#define htole32(x) OSSwapHostToLittleInt32(x)
#define be32toh(x) OSSwapBigToHostInt32(x)
#define le32toh(x) OSSwapLittleToHostInt32(x)

#define htobe64(x) OSSwapHostToBigInt64(x)
#define htole64(x) OSSwapHostToLittleInt64(x)
#define be64toh(x) OSSwapBigToHostInt64(x)
#define le64toh(x) OSSwapLittleToHostInt64(x)
#endif

#ifdef WIN32
#pragma intrinsic(__rdtsc)
#pragma intrinsic(_ReadBarrier)
#pragma intrinsic(_BitScanForward  )
#pragma intrinsic(_BitScanForward64)
#pragma intrinsic(_BitScanReverse  )
#pragma intrinsic(_BitScanReverse64)
#endif

#ifdef __linux__
#define __cdecl
#endif

// (C) AlgoEngineering LLC 2008-2013
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
// Header: include/define.h -- Macros
//
// Created By: alexei.lebedev ara.aslyan
// Authors: alexei.lebedev
// Recent Changes: alexei.lebedev ara.aslyan
//

#pragma once

#ifdef UNAME_DARWIN
#define strndupa(c,n) ({ char *_temp=(char*)alloca(n+1); strncpy(_temp,c,n); _temp[n]=0; _temp; })
#endif

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

#define ifgcc(x)
#define ifmsc(x)                     x

#define V_UNUSED
#define UNLIKELY(x)                  (x)
#define LIKELY(x)                    (x)

#define CACHE_ALIGN                  __declspec(align(CACHE_LINE))

#define F_NONNULL
#define F_DEPRECATED                 __declspec(deprecated)
#define V_ALIGN(N)

#define T_MAY_ALIAS
#pragma warning (disable : 4481)     // non-standard extension used: override specified

// todo: eliminate. this is confising.
#define __attribute__(x)

// MSC doesn't have __func__, instead it has __FUNCTION__
// msdn.microsoft.com/en-us/library/b0084kay(v=vs.80).aspx
#define __func__                     __FUNCTION__

#ifndef __INTEL_COMPILER
#pragma inline_depth(50)             // removes the arbitrary limitation of 8 levels
#endif

#define _64BIT 1

#else  // GCC

#define GCC_VERSION                 (__GNUC__ * 10 + __GNUC_MINOR__)

#if defined(__LP64__) || defined(_LP64)
#define _64BIT 1
#endif

#if GCC_VERSION < 40
#error only gcc
#endif

#define ifgcc(x)                    x
#define ifmsc(x)

#define T_MAY_ALIAS                 __attribute__((__may_alias__))
#define CACHE_ALIGN                 __attribute__((aligned (CACHE_LINE)))
#define V_UNUSED                    __attribute__((unused))
#define F_NONNULL                   __attribute__((nonnull))
#define V_ALIGN(N)                  __attribute__((aligned(N)))

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
//
#define ary_beg(type,e,ary)                             \
    {                                                   \
    algo::aryptr<type> e##temp(ary);                    \
    for (i32 e##i=0; e##i < e##temp.n_elems; e##i++) {  \
    type &e = e##temp.elems[e##i];

#define ary_end } }

#define ary_idx(e) e##i

// -----------------------------------------------------------------------------

#define ind_beg(CURS,VAR,PARENT) { CURS VAR##i; CURS##_Reset(VAR##i,PARENT); for(; CURS##_ValidQ(VAR##i); CURS##_Next(VAR##i)) { \
    CURS::ChildType &VAR=CURS##_Access(VAR##i);

#define ind_end }}

// access cursor state
#define ind_curs(x)  (x##i)

// *vrfy*(x,...) x must be true or an exception of type ErrorX is thrown.
// There is no exception hierarchy. ErrorX holds a string value representing error text.
//
// EXPRESSION     A              B
// vrfy (a,b)     bool expr      expression evaluating to string
// pvrfy(a,b)     bool expr      c++ expression that can be formatted to cstring
//
// -----------------------------------------------------------------------------
//
// errno_vrfy_    - interpret error code in errno and include in text
// errno_vrfy
// errno_pvrfy
//
// win_vrfy_      - interpret GetLastError() and include in text
// win_vrfy
// win_pvrfy
//
// On Windows, GetLastError() and errno are different things.
// Make sure to use the right vrfy_ flavor when checking return codes.
// WSAGetLastError() is the same as GetLastError() so
// doesn't need to be considered separately.
//
//

#define vrfy_(a)           { if (UNLIKELY(!(a))) algo::Throw(#a          , algo::Errcode()); }
#define vrfy(a,b)          { if (UNLIKELY(!(a))) algo::Throw( b          , algo::Errcode()); }

#define errno_vrfy_(a)     { if (UNLIKELY(!(a))) algo::Throw(#a          , algo::FromErrno(errno)); }
#define errno_vrfy(a,b)    { if (UNLIKELY(!(a))) algo::Throw( b          , algo::FromErrno(errno)); }

// will break compilation on non-windows
#define win_vrfy_(a)       { if (UNLIKELY(!(a))) algo::Throw(#a          , algo::FromWinErr(GetLastError())); }
#define win_vrfy(a,b)      { if (UNLIKELY(!(a))) algo::Throw( b          , algo::FromWinErr(GetLastError())); }

// These are used in startup tests mostly.
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

#define log_msg_(fd,x,eol)            {                 \
        algo::cstring &_outstr = algo_lib::_db.log_str; \
        int _saved = ch_N(_outstr);                     \
        _outstr << x;                                   \
        algo::Prlog(fd, _outstr, _saved, eol);          \
    }

#define prlog_(x)             log_msg_(1,x,false)
#define prerr_(x)             log_msg_(2,x,false)

#define prlog(x)              log_msg_(1,x,true)
#define prerr(x)              log_msg_(2,x,true)

#define verblog(x)            { if(UNLIKELY(algo_lib::_db.cmdline.verbose)) log_msg_(2,x,true); }
#define verblog2(x)           { if(UNLIKELY(algo_lib::_db.cmdline.verbose>1)) log_msg_(2,x,true); }
#define verblog3(x)           { if(UNLIKELY(algo_lib::_db.cmdline.verbose>2)) log_msg_(2,x,true); }
#define dbglog(x)             { if(UNLIKELY(algo_lib::_db.cmdline.debug)) log_msg_(2,x,true); }

#define bitset_beg(INDTYPE,IND,SET) {                                   \
    algo::aryptr<u64> IND##_temp(ary_Getary(SET));                      \
    for (i32 IND##_i##i=0; IND##_i##i < IND##_temp.n_elems; IND##_i##i++) { \
    u64 IND##_i = IND##_temp.elems[IND##_i##i];                         \
    while (IND##_i) {                                                   \
    u64 IND##_j = algo::u64_BitScanForward(IND##_i);                    \
    IND##_i &= ~(u64(1)<<IND##_j);                                      \
    INDTYPE IND = INDTYPE(ary_idx(IND##_i) * 64 + IND##_j);

#define bitset_end }}}

#ifdef WIN32
//
// thanks Microsoft, but we won't switch to your proprietary versions
// of standard C functions.
//
#define _CRT_SECURE_NO_DEPRECATE 1
#define _CRT_NONSTDC_NO_DEPRECATE 1
#define _CRT_SECURE_NO_WARNINGS 1

// Settings these macros indicates that we are aware that our code will only execute
// on Windows 5.0 (2000) or later.

#if _MSC_VER >= 1400
#else
#define WINVER 0x0500
#endif

#undef _WIN32_WINNT
#define _WIN32_WINNT 0x0500

#undef _WIN32_WINDOWS
#define _WIN32_WINDOWS 0x0500

#ifndef NOMINMAX
#define NOMINMAX
#endif

#define VC_EXTRALEAN        // Exclude rarely-used stuff from Windows headers

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN
#endif

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifndef STRICT
#define STRICT
#endif

#endif

#ifdef constant
#undef constant
#endif

#ifdef grp1
#undef grp1
#endif

#ifdef grp2
#undef grp2
#endif

#ifdef grp3
#undef grp3
#endif

#ifdef grp4
#undef grp4
#endif

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

// compiler instructions for reordering
// not processor instructions!!!
#define sfence() asm volatile("sfence" ::: "memory")
#define lfence() asm volatile("lfence" ::: "memory")
#define mfence() asm volatile("mfence" ::: "memory")

// tag to help update-hdr to insert __attribute__((nothrow))
// in header.
// this tag must be applied to the function *implementation*
// gcc only allows attributes on function declarations, and src_func
// populates headers based on global declarations it sees in source files
#define NOTHROW
// tag to help update-hdr to insert __attribute__((noreturn))
#define NORETURN

// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2023 Astra
// Copyright (C) 2017-2019 NYSE | Intercontinental Exchange
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
// Header: include/algo.inl.h -- Inline functions
//

#pragma once

// cl requires 
namespace algo { 
    struct tempstr; 
    template<class T> inline tempstr& operator<<(const tempstr &lhs, const T &rhs) {
        (cstring&)lhs << rhs;
        return (tempstr&)lhs;
    }
}

// Refurbish: destroy and re-create object in place.
// This function is exception-safe: if ~T throws an exception, T::T still gets
// called.
// Implementation note: explicit calls to destructor are checked for NULL pointer
// by GCC, so the nonnull attribute is required.
template<class T> inline void algo::Refurbish(T &t) FUNCATTR(F_NONNULL) {
    algo::Renew<T> r(t);
    t.~T();
}


template<class T> inline int algo::elems_N(const aryptr<T> &ary) {
    return ary.n_elems;
}

template<class T> inline int algo::ch_N(const aryptr<T> &ary) {
    return ary.n_elems;
}

template<class T> inline void algo::Fill(const aryptr<T> &lhs, const T &t) {
    frep_(i,ch_N(lhs)) {
        lhs[i]=t;
    }
}

template<class T> inline algo::aryptr<T>::aryptr() {
    elems = 0;
    n_elems = 0;
}

template<class T> inline T &algo::aryptr<T>::operator [](u32 idx) const {
    return elems[idx];
}

template<class T, class U> inline int algo::Find(const algo::aryptr<T> &lhs, const U&t) {
    frep_ (i, lhs.n_elems) {
        if (lhs.elems[i]==t) {
            return i;
        }
    }
    return -1;
}

template<class T> inline algo::aryptr<T> algo::FirstN(const algo::aryptr<T> &lhs, u32 n) {
    u32 lim = lhs.n_elems;
    if (n > lim) {
        n = lim;
    }
    return algo::aryptr<T>(lhs.elems, n);
}

template<class T> inline algo::aryptr<T> algo::LastN(const algo::aryptr<T> &lhs, u32 n) {
    u32 lim = lhs.n_elems;
    if (n > lim) {
        n = lim;
    }
    return algo::aryptr<T>(lhs.elems + lim - n, n);
}

template<class T> inline algo::aryptr<T> algo::RestFrom(const algo::aryptr<T> &lhs, u32 n) {
    u32 lim = lhs.n_elems;
    if (n > lim) {
        n = lim;
    }
    return algo::aryptr<T>(lhs.elems + n, lim - n);
}

template<class T> inline algo::aryptr<T> algo::qGetRegion(const algo::aryptr<T> &lhs, u32 lo, u32 n) {
    return algo::aryptr<T>(lhs.elems + lo, n);
}

template<class T> inline algo::aryptr<T> algo::GetRegion(const algo::aryptr<T> &lhs, u32 lo, u32 n) {
    u32 max = lhs.n_elems;
    lo = u32_Min(lo, max);
    n  = u32_Min(n,  max - lo);
    return algo::aryptr<T>(lhs.elems + lo, n);
}

template<class T> inline algo::aryptr<u8> algo::BytesOf(const T &t) {
    return algo::aryptr<u8>((u8*)&t,sizeof(t));
}

template<class T> inline algo::aryptr<T>::aryptr(const T *e, i32 in_n) : elems(const_cast<T*>(e)) , n_elems(in_n) {
}

template<class T> inline algo::aryptr<T>::aryptr(const char *e) : elems(const_cast<T*>(e)) , n_elems(algo::ImpliedLength((T*)NULL,e)) {
}

template<class T> inline T &algo::qLast(const algo::aryptr<T> &ary) {
    return ary.elems[ary.n_elems-1];
}


#ifdef WIN32
inline u16 htobe16(u16 val) { return _byteswap_ushort(val); }
inline u32 htobe32(u32 val) { return _byteswap_ulong(val); }
inline u64 htobe64(u64 val) { return _byteswap_uint64(val); }
inline u16 be16toh(u16 val) { return _byteswap_ushort(val); }
inline u32 be32toh(u32 val) { return _byteswap_ulong(val); }
inline u64 be64toh(u64 val) { return _byteswap_uint64(val); }
inline u16 htole16(u16 val) { return val; }
inline u32 htole32(u32 val) { return val; }
inline u64 htole64(u64 val) { return val; }
#endif

// Taylor series expansion. For x around 0.01, this produces 2 digits per
// iteration, so with N=4 will be reasonably fast.
inline double algo::ExpTaylor(double x, int n) {
    double result = 1;
    double f = x;
    for (int i=2; i<=n; i++) {
        result += f;
        f *= x/i;
    }
    return result;
}

// -----------------------------------------------------------------------------

inline u32 algo::ParseNum8(u64 x, u32 &ok) {
    u64 y = x - 0x3030303030303030ULL;
    ok    = ((y | (0x0909090909090909 - y)) & 0x8080808080808080ULL) == 0;
    y     = (((y*266  ) >>8 )) & 0X00FF00FF00FF00FFULL;
    y     = (((y*65636) >>16)) & 0x0000FFFF0000FFFFULL;
    return u32((y & 0xffffffff) + (y>>32)*10000);
}

inline u32 algo::ParseNum4(u32 x, u32 &ok) {
    u32 y = x - 0x30303030UL;
    ok    = ((y | (0x09090909 - y)) & 0x80808080ULL) == 0;
    y     = (((y*266 ) >>8 )) & 0X00FF00FFULL;
    return (y & 0xffff) + (y>>16)*100;
}

inline u32 algo::aParseNum8(strptr s, u32 &ok) {
    return ParseNum8(ReadBE64(s.elems),s.n_elems,ok);
}

inline u32 algo::aParseNum4(strptr s, u32 &ok) {
    return ParseNum4(ReadBE32(s.elems),s.n_elems,ok);

}

inline int algo::ParseOct1(u32 c, u8 &result) {
    result = u8(c-'0');
    return result < 8;
}

// C           single hex character, upper or lowercase
// RETURN      value of hex character
// RESULT      value of hex digit
// RETURN      success code
inline int algo::ParseHex1(u32 c, u8 &result) {
    u32 shift1 = c >= 'a' ? 'A' - 'a' : 0;
    c          = c + shift1;
    u32 shift2 = c >= 'A' ? 10 + '0' - 'A' : 0;
    c          = c + shift2 - '0';
    result     = u8(c);
    return c < 10 || (shift2 && c < 16);
}

// -----------------------------------------------------------------------------

inline void algo::PageBufInit(PageBuf &F, u64 n, u64 align) {
#ifdef WIN32
    F.elems = (u8*)_aligned_malloc(n, align);
    vrfy(F.elems!=NULL, "aligned_malloc");
#else
    errno_vrfy(posix_memalign((void**)&F.elems, align, n)==0, "posix_memalign");
#endif
    F.n_elems  = (int)n;
}

inline u32 u8_Hash (u32 prev, u8  val) {
    return _mm_crc32_u8 (prev,val);
}

inline u32 u16_Hash(u32 prev, u16 val) {
    return _mm_crc32_u16(prev,val);
}

inline u32 u32_Hash(u32 prev, u32 val) {
    return _mm_crc32_u32(prev,val);
}

inline u32 u64_Hash(u32 prev, u64 val) {
    return _mm_crc32_u64(prev, val);
}

inline u32 i8_Hash (u32 prev, i8  val) {
    return _mm_crc32_u8 (prev,val);
}

inline u32 i16_Hash(u32 prev, i16 val) {
    return _mm_crc32_u16(prev,val);
}

inline u32 i32_Hash(u32 prev, i32 val) {
    return _mm_crc32_u32(prev,val);
}

inline u32 i64_Hash(u32 prev, i64 val) {
    return _mm_crc32_u64(prev, val);
}

inline u32 bool_Hash(u32 prev, bool  val) {
    return _mm_crc32_u8 (prev,val);
}

inline u32 char_Hash(u32 prev, char  val) {
    return _mm_crc32_u8 (prev,val);
}

inline u32 float_Hash(u32 prev, float val) {
    return algo::CRC32Step(prev, (u8*)&val, sizeof(val));
}

inline u32 double_Hash(u32 prev, double  val) {
    return algo::CRC32Step(prev, (u8*)&val, sizeof(val));
}

inline u32 u128_Hash(u32 prev, u128 t) {
    return algo::CRC32Step(prev, (u8*)&t, sizeof(t));
}

inline u32 algo::strptr_Hash(u32 prev, algo::strptr val) {
    return algo::CRC32Step(prev, (u8*)val.elems, val.n_elems);
}

inline u32 algo::cstring_Hash(u32 prev, const algo::strptr &val) {
    return algo::CRC32Step(prev, (u8*)val.elems, val.n_elems);
}

inline u32 algo::cstring_Hash(u32 prev, algo::cstring &val) {
    return algo::CRC32Step(prev, (u8*)val.ch_elems, val.ch_n);
}

inline u32 algo::cstring_Hash(u32 prev, const algo::cstring &val) {
    return algo::CRC32Step(prev, (u8*)val.ch_elems, val.ch_n);
}

// -----------------------------------------------------------------------------

inline i32 u8_Cmp (u8 a, u8 b) {
    return a<b ? -1 : a>b;
}

inline i32 u16_Cmp(u16 a, u16 b) {
    return a<b ? -1 : a>b;
}

inline i32 u32_Cmp(u32 a, u32 b) {
    return a<b ? -1 : a>b;
}

inline i32 u64_Cmp(u64 a, u64 b) {
    return a<b ? -1 : a>b;
}

inline i32 i8_Cmp (i8 a, i8 b) {
    return a<b ? -1 : a>b;
}

inline i32 i16_Cmp(i16 a, i16 b) {
    return a<b ? -1 : a>b;
}

inline i32 i32_Cmp(i32 a, i32 b) {
    return a<b ? -1 : a>b;
}

inline i32 i64_Cmp(i64 a, i64 b) {
    return a<b ? -1 : a>b;
}

inline i32 bool_Cmp(bool a, bool b) {
    return a<b ? -1 : a>b;
}

inline i32 char_Cmp(char a, char b) {
    return a<b ? -1 : a>b;
}

inline i32 float_Cmp(float a, float b) {
    return a<b ? -1 : a>b;
}

inline i32 double_Cmp(double a, double b) {
    return a<b ? -1 : a>b;
}

inline i32 u128_Cmp(u128 a, u128 b) {
    return a<b ? -1 : a>b;
}

// -----------------------------------------------------------------------------

inline bool u8_Lt (u8 a, u8 b) {
    return a < b;
}

inline bool u16_Lt(u16 a, u16 b) {
    return a < b;
}

inline bool u32_Lt(u32 a, u32 b) {
    return a < b;
}

inline bool u64_Lt(u64 a, u64 b) {
    return a < b;
}

inline bool i8_Lt (i8 a, i8 b) {
    return a < b;
}

inline bool i16_Lt(i16 a, i16 b) {
    return a < b;
}

inline bool i32_Lt(i32 a, i32 b) {
    return a < b;
}

inline bool i64_Lt(i64 a, i64 b) {
    return a < b;
}

inline bool bool_Lt(bool a, bool b) {
    return a < b;
}

inline bool char_Lt(char a, char b) {
    return a < b;
}

inline bool float_Lt(float a, float b) {
    return a < b;
}

inline bool double_Lt(double a, double b) {
    return a < b;
}

inline bool u128_Lt(u128 a, u128 b) {
    return a < b;
}

// -----------------------------------------------------------------------------

inline bool u8_Eq (u8 a, u8 b) {
    return a == b;
}

inline bool u16_Eq(u16 a, u16 b) {
    return a == b;
}

inline bool u32_Eq(u32 a, u32 b) {
    return a == b;
}

inline bool u64_Eq(u64 a, u64 b) {
    return a == b;
}

inline bool i8_Eq (i8 a, i8 b) {
    return a == b;
}

inline bool i16_Eq(i16 a, i16 b) {
    return a == b;
}

inline bool i32_Eq(i32 a, i32 b) {
    return a == b;
}

inline bool i64_Eq(i64 a, i64 b) {
    return a == b;
}

inline bool bool_Eq(bool a, bool b) {
    return a == b;
}

inline bool char_Eq(char a, char b) {
    return a == b;
}

inline bool float_Eq(float a, float b) {
    return a == b;
}

inline bool double_Eq(double a, double b) {
    return a == b;
}

inline bool u128_Eq(u128 a, u128 b) {
    return a == b;
}

#ifdef AOS_SSE42

// this is inefficient because it would be nice to be able to overrun strings.
// however, we don't check byte alignment at the beginning,
// so a 2-byte string that lies 4 bytes before the end of a page could fault.
inline u32 algo::CRC32Step(u32 old, const u8 *x, size_t len) {
    u64 h = old;
    while (len>=8) { h = _mm_crc32_u64(h,*(u64 T_MAY_ALIAS*)x); x = (u8*)x + 8; len -= 8; }
    if    (len>=4) { h = _mm_crc32_u32(h,*(u32 T_MAY_ALIAS*)x); x = (u8*)x + 4; len -= 4; }
    if    (len>=2) { h = _mm_crc32_u16(h,*(u16 T_MAY_ALIAS*)x); x = (u8*)x + 2; len -= 2; }
    if    (len>=1) { h = _mm_crc32_u8 (h,*(u8  *)x); x = (u8*)x + 1; len -= 1; }
    return (u32)h;
}

#else

inline u32 _mm_crc32_u64(u32 prev, u64 val) {
    return CRC32Step(prev, &val, sizeof(val));
}

inline u32 _mm_crc32_u32(u32 prev, u32 val) {
    return CRC32Step(prev, &val, sizeof(val));
}

inline u32 _mm_crc32_u16(u32 prev, u16 val) {
    return CRC32Step(prev, &val, sizeof(val));
}

inline u32 _mm_crc32_u8 (u32 prev, u8  val) {
    return CRC32Step(prev, &val, sizeof(val));
}

#endif

inline int algo::P1Mod(int a, int b)  {
    a++;
    if (a>=b) a=0;
    return a;
}

inline bool operator ==(const algo::strptr &a, const algo::strptr &b) {
    return strptr_Eq(a,b);
}
inline bool operator !=(const algo::strptr &a, const algo::strptr &b) {
    return !strptr_Eq(a,b);
}
inline bool operator <(const algo::strptr &a, const algo::strptr &b) {
    return strptr_Lt(a,b);
}

// -----------------------------------------------------------------------------

inline algo::UnixTime operator +  (const algo::UnixTime &a, algo::UnixDiff d) {
    return algo::UnixTime(a.value + d.value);
}

inline algo::UnixTime operator -  (const algo::UnixTime &a, algo::UnixDiff d) {
    return algo::UnixTime(a.value - d.value);
}

inline algo::UnixDiff operator - (const algo::UnixTime &a, algo::UnixTime b) {
    return algo::UnixDiff(a.value - b.value);
}

inline algo::UnTime operator + (algo::UnTime a, algo::UnDiff d) {
    return algo::UnTime(a.value + d.value);
}

inline algo::UnTime operator += (algo::UnTime &a, const algo::UnDiff d) {
    a.value += d.value;
    return a;
}

inline algo::UnDiff operator += (algo::UnDiff &a, const algo::UnDiff d) {
    a.value += d.value;
    return a;
}

inline algo::UnTime operator -    (algo::UnTime a, algo::UnDiff d) {
    return algo::UnTime(a.value - d.value);
}

inline algo::UnDiff operator -    (algo::UnTime a, algo::UnTime b) {
    return algo::UnDiff(a.value - b.value);
}

inline algo::UnDiff operator -    (const algo::UnDiff a) {
    return algo::UnDiff{-a.value};
}

// -----------------------------------------------------------------------------

inline algo::UnixDiff operator *  (const algo::UnixDiff &a, i64 d) {
    return algo::UnixDiff(a.value *d);
}

inline algo::UnixDiff operator /  (const algo::UnixDiff &a, i64 d) {
    return algo::UnixDiff(a.value /d);
}

inline algo::UnixDiff operator +  (const algo::UnixDiff &a, algo::UnixDiff d) {
    return algo::UnixDiff(a.value + d.value);
}

inline algo::UnixDiff operator -  (const algo::UnixDiff &a, algo::UnixDiff d) {
    return algo::UnixDiff(a.value - d.value);
}


inline algo::UnDiff operator +    (const algo::UnDiff &a, algo::UnDiff d) {
    return algo::UnDiff(a.value + d.value);
}

inline algo::UnDiff operator -    (const algo::UnDiff &a, algo::UnDiff d) {
    return algo::UnDiff(a.value - d.value);
}

inline algo::UnDiff operator *  (const algo::UnDiff &a, i64 d) {
    return algo::UnDiff(a.value *d);
}

inline algo::UnDiff operator /  (const algo::UnDiff &a, i64 d) {
    return algo::UnDiff(a.value /d);
}

inline algo::WTime operator+(const algo::WTime &t, const algo::WDiff &t2) {
    return algo::WTime(t.value + t2.value);
}

inline algo::WDiff operator+(const algo::WDiff &t, const algo::WDiff &t2) {
    return algo::WDiff(t.value + t2.value);
}

inline algo::WTime operator-(const algo::WTime &t, const algo::WDiff &t2) {
    return algo::WTime(t.value - t2.value);
}

inline algo::WDiff operator-(const algo::WTime &t, const algo::WTime &t2) {
    return algo::WDiff(t.value - t2.value);
}

inline algo::WDiff operator-(const algo::WDiff &t, const algo::WDiff &t2) {
    return algo::WDiff(t.value - t2.value);
}

inline algo::SchedTime operator +  (const algo::SchedTime &a, algo::SchedTime d) {
    return algo::SchedTime(a.value + d.value);
}

inline algo::SchedTime operator -  (const algo::SchedTime &a, algo::SchedTime d) {
    return algo::SchedTime(a.value - d.value);
}


// move ctor
inline algo::tempstr::tempstr(const tempstr &rhs) : algo::cstring(rhs) {
    tempstr &r  = (tempstr&) rhs;
    r.ch_elems     = 0;
    r.ch_n   = 0;
    r.ch_max = 0;
}

inline u32 algo::u32_Count1s(u32 x) {
#ifdef AOS_SSE42
    return _mm_popcnt_u32(x);
#else
    // http://en.wikipedia.org/wiki/Hamming_weight
    u32 m1 = 0x55555555; // 0101010101010101 - 1-bit blocks
    u32 m2 = 0x33333333; // 0011001100110011 - 2-bit blocks
    u32 m4 = 0x0f0f0f0f; // 1111000011110000 - 4-bit blocks
    x -= (x >> 1) & m1;
    x = (x & m2) + ((x >> 2) & m2);
    x = (x + (x >> 4)) & m4;
    x += x >>  8;
    return (x + (x >> 16)) & 0x3f;
#endif
}

inline u32 algo::u8_Count1s(u8 x) {
    return u32_Count1s(x);
}

inline u32 algo::u16_Count1s(u16 x) {
    return u32_Count1s(x);
}

inline u32 algo::u64_Count1s(u64 x) {
#ifdef AOS_SSE42
    return _mm_popcnt_u64(x);
#else
    return u32_Count1s(u32(x)) + u32_Count1s(u32(x >> 32));
#endif
}

inline u32 algo::u128_Count1s(u128 x) {
    return u64_Count1s(u64(x>>u32(64))) + u64_Count1s(u64(x));
}

template<class T> inline void algo::ZeroBytes(T &t) {
    memset(&t,0,sizeof(t));
}

template<class T, class U> inline T algo::PtrAdd(U *ptr, int_ptr offset) {
    return (T)((u8*)ptr + offset);
}

template<class T> inline void algo::TSwap(T &a, T &b)  {
    char t[sizeof(T)];
    memcpy(&t, &a, sizeof(T));
    memcpy(&a, &b, sizeof(T));
    memcpy(&b, &t, sizeof(T));
}

// Default, and invalid, value for Fildes is -1. 0 is a valid value (stdin)!
inline bool algo::ValidQ(Fildes fd) {
    return fd.value != -1;// -1 = invalid value
}

inline int algo::i32_NegateIf(int x, bool i) {
    return i ? -x : x;
}

inline i64 algo::i64_NegateIf(i64 x, bool i) {
    return i ? -x : x;
}

inline float algo::float_NegateIf(float x, bool i) {
    return i ? -x : x;
}

inline double algo::double_NegateIf(double x, bool i) {
    return i ? -x : x;
}

inline double algo::DRound(double a) {
    if (a>0) {
        return floor(a+0.5);
    }
    return -floor(-a+0.5);
}

inline double algo::DCeiling(double a) {
    return ceil(a);
}

inline double algo::DFloor(double a) {
    return floor(a);
}

inline double algo::DTrunc(double a) {
    if (a>0) return floor(a);
    return -floor(-a);
}

inline algo_FileFlagsEnum operator |(algo_FileFlagsEnum lhs, algo_FileFlagsEnum rhs) {
    return algo_FileFlagsEnum((i32)lhs | (i32)rhs);
}

// Clipping to an open interval (exclisive upper limit) [0..hi-1] or [lo..hi-1]
inline int algo::Clipped(int a, int hi) {
    return a >= hi ? hi-1 : a<0  ? 0  : a;
}

inline int algo::Clipped(int a, int lo, int hi) {
    return a >= hi ? hi-1 : a<lo ? lo : a;
}

// Clipping to a closed interval lo..hi
inline double algo::Clippedf(double a, double lo, double hi) {
    return a>hi  ? hi   : a<lo ? lo : a;
}

//unsigned subtract with clipping
inline u64 algo::u64_SubClip( u64 a, u64 b) {
    return u64_Max(a,b)-b;
}

inline u32 algo::u32_SubClip( u32 a, u32 b) {
    return u32_Max(a,b)-b;
}

inline u16 algo::u16_SubClip( u16 a, u16 b) {
    return u16_Max(a,b)-b;
}

inline int algo::Abs(i8  a) {
    return   abs(int(a));
}

inline int algo::Abs(i16 a) {
    return   abs(int(a));
}

inline i32 algo::Abs(i32 a) {
    return   abs(    a );
}

inline i64 algo::Abs(i64 a) {
    return a>0 ? a:-a;
}

inline f32 algo::Abs(f32 a) {
    return fabsf(    a );
}

inline f64 algo::Abs(f64 a) {
    return  fabs(    a );
}

inline f80 algo::Abs(f80 a) {
    return fabsl(    a );
}

// Read big-endian word
inline u16 algo::ReadBE16(const void *val) {
    return htobe16(*(const u16*)val);
}

inline u32 algo::ReadBE32(const void *val) {
    return htobe32(*(const u32*)val);
}

inline u64 algo::ReadBE64(const void *val) {
    return htobe64(*(const u64*)val);
}

// Read little-endian word
inline u16 algo::ReadLE16(const void *val) {
    return htole16(*(const u16*)val);
}

inline u32 algo::ReadLE32(const void *val) {
    return htole32(*(const u32*)val);
}

inline u64 algo::ReadLE64(const void *val) {
    return htole64(*(const u64*)val);
}

// -----------------------------------------------------------------------------

// Forward:  returns 0-based index of least significant bit that is set
// Reverse:  returns 0-based index of most  significant bit that is set.
// input argument must not be zero.
// input result in 0 is undefined (see Intel manual)
// http://www.intel.com/content/dam/doc/manual/64-ia-32-architectures-software-developer-vol-2a-2b-instruction-set-a-z-manual.pdf

inline u32 algo::u32_BitScanForward(u32 v) {
#ifdef WIN32
    unsigned long r;
    _BitScanForward(&r,v);
#else
    u32 r;
    asm ("bsfl %1, %0" : "=r"(r) : "rm"(v) );
#endif
    return r;
}

inline u64 algo::u64_BitScanForward(u64 v) {
#ifdef WIN32
    unsigned long r;
    _BitScanForward64(&r,v);
#else
    u64 r;
    asm ("bsfq %1, %0" : "=r"(r) : "rm"(v) );
#endif
    return r;
}

inline u32 algo::u32_BitScanReverse(u32 v) {
#ifdef WIN32
    unsigned long r;
    _BitScanReverse(&r,v);
#else
    u32 r;
    asm ("bsrl %1, %0" : "=r"(r) : "rm"(v) );
#endif
    return r;
}

inline u64 algo::u64_BitScanReverse(u64 v) {
#ifdef WIN32
    unsigned long r;
    _BitScanReverse64(&r,v);
#else
    u64 r;
    asm ("bsrq %1, %0" : "=r"(r) : "rm"(v) );
#endif
    return r;
}

inline u32 algo::u16_BitScanForward(u16 v) {
    return u32_BitScanForward(v);
}

inline u32 algo::u16_BitScanReverse(u16 v) {
    return u32_BitScanReverse(v);
}

inline u32 algo::u8_BitScanForward(u8 v) {
    return u32_BitScanForward(v);
}

inline u32 algo::u8_BitScanReverse(u8 v) {
    return u32_BitScanReverse(v);
}

inline u32  algo::CeilingLog2(u32 orig) {
    return orig>1 ? u32_BitScanReverse(orig-1)+1 : 0;
}

inline u64  algo::CeilingLog2(u64 orig) {
    return orig>1 ? u64_BitScanReverse(orig-1)+1 : 0;
}


inline u32  algo::FloorLog2(u32 i) {
    u32 log = 0; while (i>>=1) log++; return log;
}

inline u64  algo::FloorLog2(u64 i) {
    u32 log = 0; while (i>>=1) log++; return log;
}


inline u32 algo::BumpToPow2(u32 i) {
    return u32(1) << CeilingLog2(i);
}

inline u64 algo::BumpToPow2(u64 i) {
    return u64(1) << CeilingLog2(i);
}


inline u64   algo::u64_CeilPow2(u64 a, u64 b) {
    return (a + (b-1)) & ~(b-1);
}

inline u32   algo::u32_CeilPow2(u32 a, u32 b) {
    return (a + (b-1)) & ~(b-1);
}


inline u64    algo::u64_ModPow2(u64 a, u64 b) {
    return a & (b-1);
}

inline u32    algo::u32_ModPow2(u32 a, u32 b) {
    return a & (b-1);
}


inline bool     algo::u64_Pow2Q(u64 i) {
    return (i>0) && !(i & (i-1));
}

inline bool     algo::u32_Pow2Q(u32 i) {
    return (i>0) && !(i & (i-1));
}

inline algo::PageBuf::~PageBuf() {
    free(elems);// posix_memalign_free?
}


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

inline algo::UnDiff algo::UnDiffMsecs(const i64 i) {
    return algo::UnDiff(i * UNTIME_PER_MSEC);
}

inline algo::UnDiff algo::UnDiffUsecs(const double d) {
    return algo::UnDiff(static_cast<i64>(d * UNTIME_PER_USEC));
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

// Append and end-of-line sequence to string S.

inline void algo::eol(cstring &s) {
    ch_Alloc(s) = '\n';
}

inline algo::strptr algo::ToStrPtr(memptr buf)  {
    return strptr((char*)buf.elems,elems_N(buf));
}

// Note: cstring << algo::ToLower(int) will output an *integer*, not a character
inline int algo::ToLower(int i)  {
    return i>='A' && i<='Z' ? i-'A'+'a' : i;
}

// Note: cstring << algo::ToUpper(int) will output an *integer*, not a character
inline int algo::ToUpper(int i)  {
    return i>='a' && i<='z' ? i-'a'+'A' : i;
}

inline char algo::ToLower(char i)  {
    return i>='A' && i<='Z' ? char(i-'A'+'a') : i;
}

inline char algo::ToUpper(char i)  {
    return i>='a' && i<='z' ? char(i-'a'+'A') : i;
}

inline bool algo::StringIter::EofQ() const {
    return index==elems_N(expr);
}

inline char algo::StringIter::Peek() {
    if (index==elems_N(expr)) return 0;
    return (unsigned char)expr[index];
}

inline char algo::StringIter::GetChar() {
    if (index==elems_N(expr)) return 0;
    return (unsigned char)expr[index++];
}

inline algo::StringIter &algo::StringIter::Ws() {
    for (; index < elems_N(expr); index++) {
        if (!algo_lib::WhiteCharQ(expr[index])) {
            break;
        }
    }
    return *this;
}

inline algo::i32_Range algo::TFind(const strptr &s, char match) {
    return ch_FindFirst(s,match);
}

inline algo::i32_Range algo::TRevFind(const strptr &s, char match) {
    return ch_FindLast(s,match);
}

inline algo::aryptr<char> algo::ch_FirstN(const strptr &lhs, u32 n) {
    u32 lim = lhs.n_elems;
    if (n > lim) {
        n = lim;
    }
    return algo::aryptr<char>(lhs.elems, n);
}

inline algo::aryptr<char> algo::ch_LastN(const strptr &lhs, u32 n) {
    u32 lim = lhs.n_elems;
    if (n > lim) {
        n = lim;
    }
    return algo::aryptr<char>(lhs.elems + lim - n, n);
}

inline algo::aryptr<char> algo::ch_RestFrom(const strptr &lhs, u32 n) {
    u32 lim = lhs.n_elems;
    if (n > lim) {
        n = lim;
    }
    return algo::aryptr<char>(lhs.elems + n, lim - n);
}

inline algo::aryptr<char> algo::ch_GetRegion(const strptr &lhs, u32 lo, u32 n) {
    u32 max = lhs.n_elems;
    lo = u32_Min(lo, max);
    n  = u32_Min(n,  max - lo);
    return algo::aryptr<char>(lhs.elems + lo, n);
}

inline int algo::ImpliedLength(char *, const char *c) {
    return int(c ? strlen(c) : 0);
}

inline int algo::ImpliedLength(const char *, const char *c) {
    return int(c ? strlen(c) : 0);
}

inline int algo::ch_N(const strptr &s) {
    return s.n_elems;
}

inline int algo::ch_First(const strptr &s, int dflt DFLTVAL(0)) {
    return s.n_elems>0 ? s.elems[0] : dflt;
}

inline int algo::ch_Last(const strptr &s, int dflt DFLTVAL(0)) {
    return s.n_elems>0 ? s.elems[s.n_elems-1] : dflt;
}

inline int algo::ch_N(const tempstr &str) {
    return str.ch_n;
}

inline int algo::range_N(const i32_Range &rhs) {
    return rhs.end - rhs.beg;
}

inline algo::aryptr<u8> algo::strptr_ToMemptr(algo::aryptr<char> rhs) {
    return algo::aryptr<u8>((u8*)rhs.elems,rhs.n_elems);
}

inline algo::aryptr<char> algo::memptr_ToStrptr(algo::aryptr<u8> rhs) {
    return algo::aryptr<char>((char*)rhs.elems,rhs.n_elems);
}

// if next character matches WHAT, skip and return true
// otherwise return false
inline bool algo::SkipChar(StringIter &iter, char what) {
    char cur = iter.Peek();
    bool ret = (cur==what);
    iter.index += ret;
    return ret;
}

// Convert string to numeric type.
// If there is an error, or string is empty, return DFLT.
// If there is junk at the end of the string, silently ignore it.
// Use StringIter version to check for junk, or use -X version to throw exception.
inline int algo::ParseI32(strptr str, int dflt) {
    StringIter iter(str); return ParseI32(iter, dflt);
}

inline u32 algo::ParseU32(strptr str, u32 dflt) {
    StringIter iter(str); return ParseU32(iter, dflt);
}

inline i64 algo::ParseI64(strptr str, i64 dflt) {
    StringIter iter(str); return ParseI64(iter, dflt);
}

inline u64 algo::ParseU64(strptr str, u64 dflt) {
    StringIter iter(str); return ParseU64(iter, dflt);
}

inline strptr algo::StringIter::Rest() {
    return RestFrom(expr, index);
}

inline algo::Attr_curs &algo::Attr_curs_Access(Attr_curs &curs) {
    return curs;
}

inline bool algo::Attr_curs_ValidQ(Attr_curs &curs) {
    return curs.valid;
}

inline bool algo::Line_curs_ValidQ(Line_curs &curs) {
    return !curs.eof;
}

inline strptr &algo::Line_curs_Access(Line_curs &curs) {
    return curs.line;
}

inline bool algo::Word_curs_ValidQ(Word_curs &curs) {
    return curs.token.elems < curs.text.elems + curs.text.n_elems;
}

inline strptr &algo::Word_curs_Access(Word_curs &curs) {
    return curs.token;
}

template<class T> inline void algo::Init_Set(StringDesc &desc, void(*fcn)(T&)) {
    desc.Init=InitFcn(fcn);
}

template<class T> inline void algo::SetnumMaybe_Set(StringDesc &desc, bool(*fcn)(T&, i64 num)) {
    desc.SetnumMaybe=SetnumFcn(fcn);
}

template<class T> inline void algo::Geti64_Set(StringDesc &desc, i64(*fcn)(T&, bool &)) {
    desc.Geti64=Geti64Fcn(fcn);
}

template<class T> inline void algo::Getary_Set(StringDesc &desc, algo::aryptr<char>(*fcn)(T&)) {
    desc.Getary=GetaryFcn(fcn);
}

inline strptr &algo::FileLine_curs_Access(algo::FileLine_curs &curs) {
    return curs.line;
}

inline bool algo::FileLine_curs_ValidQ(algo::FileLine_curs &curs) {
    return !curs.eof;
}

inline algo::ListSep::ListSep(strptr sep_) : sep(sep_), iter(0) {
}

inline algo::Line_curs::Line_curs() : eof(true),i(-1){
}

inline algo::Word_curs::Word_curs() : index(0){
}

inline bool algo::Sep_curs_ValidQ(algo::Sep_curs &curs) {
    return curs.state != Sep_curs::State::invalid;
}

inline strptr &algo::Sep_curs_Access(algo::Sep_curs &curs) {
    return curs.token;
}

inline algo::Sep_curs::Sep_curs() : sep('\0'), index(-1), state(algo::Sep_curs::State::invalid){
}


inline u32 algo::u16_FmtBuf(u32 value, u8 *buf) {
    return u32_FmtBufSmall(value,buf);
}

inline algo::cstring &algo::operator<<(algo::cstring &out, const char* t) {
    strptr_Print (strptr(t),out);
    return out;
}

inline algo::cstring &algo::operator<<(algo::cstring &out, const ListSep &t) {
    ListSep_Print(t,out);
    return out;
}

inline algo::cstring &algo::operator<<(algo::cstring &out, const strptr &t) {
    strptr_Print (t,out);
    return out;
}

inline algo::cstring &algo::operator<<(algo::cstring &out, void (*t)(algo::cstring &)) {
    t(out);
    return out;
}

// SameSign returns false if one of the arguments is zero.
template<typename T> inline bool algo::SameSignQ(T a, T b) {
    return (a>0 && b>0) || (a<0 && b<0);
}

// Compare two strings for equality, case-sensitively
inline bool algo::strptr_Eq(algo::strptr a, algo::strptr b) {
    // strncmp is heavily optimized using vector instructions
    // and is the fastest way to do this
    return a.n_elems==b.n_elems && strncmp(a.elems,b.elems,a.n_elems)==0;
}

// Test if string A is lexicographically less than string B.
inline bool algo::strptr_Lt(algo::strptr a, algo::strptr b) {
    int n = i32_Min(a.n_elems,b.n_elems);
    int cmp=strncmp(a.elems,b.elems,n);
    return cmp==0 ? a.n_elems<b.n_elems : cmp<0;
}

// reverse bit order in a byte
inline u8 algo::u8_ReverseBits(u8 b) {
    b=(b&0x0f)<<4 | (b&0xf0)>>4;
    b=(b&0x33)<<2 | (b&0xcc)>>2;
    b=(b&0x55)<<1 | (b&0xaa)>>1;
    return b;
}

// Return unix-epoch time with nanosecond resolution
// On Linux, this function calls clock_gettime() which takes about
// 30ns and uses rdtsc() to increase underlying clock resolution
inline algo::UnTime algo::CurrUnTime() {
    algo::UnTime ret;
    #if defined(WIN32)
        static LARGE_INTEGER freq;
        static BOOL qpc = QueryPerformanceCounter(&freq);
        if (qpc) {
            LARGE_INTEGER t_now;
            QueryPerformanceCounter(&t_now);
            ret.value = static_cast<i64>(t_now.QuadPart * 1000000000 / freq.QuadPart);
        } else {
            FILETIME ft;
            GetSystemTimeAsFileTime(&ft);
            ULARGE_INTEGER x;
            x.LowPart = ft.dwLowDateTime;
            x.HighPart = ft.dwHighDateTime;
            x.QuadPart -= 116444736000000000ULL;
            ret.value = x.QuadPart * 100;
        }
    #else
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME,&ts);
        ret.value = i64(ts.tv_sec) * 1000000000 + ts.tv_nsec;
    #endif
    return ret;
}




/*

#include <cstdint>

#ifdef _WIN32
#include <Windows.h>
#else
#include <time.h>
#endif

namespace algo {

struct UnTime {
    int64_t value;
};

inline UnTime CurrUnTime() {
    UnTime ret;

#ifdef _WIN32
    // Windows-specific implementation
    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    ULARGE_INTEGER li;
    li.LowPart = ft.dwLowDateTime;
    li.HighPart = ft.dwHighDateTime;
    // Windows file time starts from 1601-01-01
    // Need to convert it to Unix epoch (1970-01-01)
    ret.value = (li.QuadPart - 116444736000000000LL) * 100; // in nanoseconds
#else
    // POSIX implementation
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ret.value = static_cast<int64_t>(ts.tv_sec) * 1000000000 + ts.tv_nsec;
#endif

    return ret;
}

} // namespace algo
 */

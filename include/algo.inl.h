// (C) 2017-2019 NYSE | Intercontinental Exchange
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
// Header: include/algo.inl.h -- Inline functions
//
// Created By: alexei.lebedev hayk.mkrtchyan
// Authors: alexei.lebedev
// Recent Changes: alexei.lebedev hayk.mkrtchyan
//

#pragma once

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
    errno_vrfy(posix_memalign((void**)&F.elems, align, n)==0, "posix_memalign");
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

inline algo::UnixDiff operator -  (const algo::UnixTime &a, algo::UnixTime b) {
    return algo::UnixDiff(a.value - b.value);
}

inline algo::UnTime operator +    (algo::UnTime a, algo::UnDiff d) {
    return algo::UnTime(a.value + d.value);
}

inline algo::UnTime operator -    (algo::UnTime a, algo::UnDiff d) {
    return algo::UnTime(a.value - d.value);
}

inline algo::UnDiff operator -    (algo::UnTime a, algo::UnTime b) {
    return algo::UnDiff(a.value - b.value);
}

// -----------------------------------------------------------------------------

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


inline bool operator > (const algo::UnixDiff &lhs, const algo::UnixDiff &rhs) {
    return lhs.value >  rhs.value;
}

inline bool operator >=(const algo::UnixDiff &lhs, const algo::UnixDiff &rhs) {
    return lhs.value >= rhs.value;
}

inline bool operator <=(const algo::UnixDiff &lhs, const algo::UnixDiff &rhs) {
    return lhs.value <= rhs.value;
}

inline bool operator !=(const algo::UnixDiff &lhs, const algo::UnixDiff &rhs) {
    return lhs.value != rhs.value;
}


inline bool operator > (const algo::UnDiff &lhs, const algo::UnDiff &rhs) {
    return lhs.value >  rhs.value;
}

inline bool operator >=(const algo::UnDiff &lhs, const algo::UnDiff &rhs) {
    return lhs.value >= rhs.value;
}

inline bool operator <=(const algo::UnDiff &lhs, const algo::UnDiff &rhs) {
    return lhs.value <= rhs.value;
}

inline bool operator !=(const algo::UnDiff &lhs, const algo::UnDiff &rhs) {
    return lhs.value != rhs.value;
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
    return u64_Count1s(u64(x>>64)) + u64_Count1s(u64(x));
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

static inline bool algo::CASU64(u64 volatile& what, u64& test_and_prev, u64 new_val) {
    u8 ret;
    __asm__ __volatile__("lock cmpxchgq %3,%0\n\t"
                         "sete %1\n\t"
                         :"+m"(what),
                          "=r"(ret),
                          "+a"(test_and_prev)
                         :"r"(new_val)
                         :"cc");
    return ret;
}

static inline void algo::SwapMTU64(u64 volatile &mem, u64& x) {
    asm volatile (
                  "lock xchgq %1, %0\n" // add x to mem
                  : "+m"(mem),
                    "+r"(x)
                  :
                  : "memory");
}

template<class T> inline T algo::IncrMT(T *i, T by) {
    return __sync_fetch_and_add(i, by)+by;
}

template<class T> inline T algo::DecrMT(T *i, T by) {
    return __sync_fetch_and_sub(i, by)-by;
}

// ignore:ptr_byref
template<class T> inline void algo::SwapMT(T* volatile &mem, T *&x) {
    SwapMTU64((u64 volatile&)mem, (u64&)x);
}

// ignore:ptr_byref
template<class T> inline bool algo::CAS(T *volatile & what, T*& test_and_prev, T* new_val) {
    return CASU64((u64 volatile&)what, (u64&)test_and_prev, (u64)new_val);
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
    u32 r;
    asm ("bsfl %1, %0" : "=r"(r) : "rm"(v) );
    return r;
}

inline u64 algo::u64_BitScanForward(u64 v) {
    u64 r;
    asm ("bsfq %1, %0" : "=r"(r) : "rm"(v) );
    return r;
}

inline u32 algo::u32_BitScanReverse(u32 v) {
    u32 r;
    asm ("bsrl %1, %0" : "=r"(r) : "rm"(v) );
    return r;
}

inline u64 algo::u64_BitScanReverse(u64 v) {
    u64 r;
    asm ("bsrq %1, %0" : "=r"(r) : "rm"(v) );
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

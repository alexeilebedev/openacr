// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2020-2021 Astra
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
// Header: include/algo/string.inl.h
//
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

inline int algo::ch_First(const strptr &s, int dflt) {
    return s.n_elems>0 ? s.elems[0] : dflt;
}

inline int algo::ch_Last(const strptr &s, int dflt) {
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

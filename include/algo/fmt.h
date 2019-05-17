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
// Header: include/algo/fmt.h -- Print/Read functions
//
// Created By: alexei.lebedev shreejith.lokegowda
// Authors: alexei.lebedev
// Recent Changes: alexei.lebedev hayk.mkrtchyan shreejith.lokegowda
//

#pragma once

namespace algo { // update-hdr srcfile:%/algo/fmt.%
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/lib/algo/fmt.cpp -- Print to string / Read from string
    //

    // Read time from STR to ROW
    // Return success code.
    // If funciton does not succeed, ROW is not modified
    // Several formats are supported:
    // %Y-%m-%dT%T
    // %Y-%m-%d %T
    // %Y/%m/%d %T
    // Where %T is %H:%M:%S.%X
    // And %X is the nanosecond portion
    bool UnTime_ReadStrptrMaybe(algo::UnTime &row, algo::strptr str);
    bool UnDiff_ReadStrptrMaybe(UnDiff &row, algo::strptr str);
    bool UnixTime_ReadStrptrMaybe(algo::UnixTime &row, algo::strptr str);
    bool cstring_ReadStrptrMaybe(algo::cstring &row, algo::strptr str);

    // Parse a URL from string STR to OUT.
    // The format of a URL is
    // protocol://user:password@somehost:port<-source_interface/dir1/dir2
    // URL fields are
    // protocol, username, password, server, port, host, source_addr_host, dir
    //
    //
    // Windows pathnames are supported, e.g.
    // when parsing
    // file://c:/dir/dir2
    // c: will not be parsed as a username, but as part of the pathname.
    //
    bool URL_ReadStrptrMaybe(URL &out, algo::strptr str);
    bool Ipmask_ReadStrptrMaybe(Ipmask &row, algo::strptr str);
    void Ptr_Print(void *ptr, cstring &out);

    // pads with zeros on the left so that at least 'atleast' characters are output.
    void i64_PrintPadLeft(i64 num, cstring &out, int atleast);

    // Assuming SRC is a number, Transfer SRC to OUT, inserting
    // commas between groups of 3 numbers.
    // 0 -> 0
    // 11 -> 11
    // 222 -> 222
    // 3333 -> 3,333
    // 4567.3 -> 4,567.3
    // 1.11111 -> 1.11111
    void strptr_PrintWithCommas(strptr src, cstring &out);

    // Assuming STR is a number, remove any unnecessary characters from the right of it.
    // Unnecessary characters are trailing zeros after a dot.
    // If the string contains no dot, nothing is done.
    // If a trailing '.' or a single '-' remains, it is removed as well.
    // If the resulting string is empty, a single zero is returned.
    //
    // 0         -> 0
    // 0.1       -> 0.1
    // 0.0       -> 0
    // 12345.000 -> 12345
    // -0        -> -0
    // .0        -> 0
    // -.0       -> 0
    // -0.0      -> -0
    // -10.0     -> -10
    // ->            empty string is not touched
    // Since the string may be edited, you can't pass compile-time constants
    // to this function. In fact the only reason this function is not private to algo_lib
    // is because of unit testing.
    // If the initial string is empty, nothing is done.
    // BEWARE: this function will happily convert 1e+60 to 1e+6
    //
    void strptr_TrimZerosRight(strptr &str);

    // Print double D into string OUT with PRECISION digits
    // after the decimal point.
    // If OMIT_ZEROS is specified, trailing zeros that are safe to omit are omitted.
    // If COMMAS is specified, the large numbers are printed in groups of 3 digits
    // with commas between them.
    void double_PrintPrec(double d, cstring &out, int precision, bool omit_zeros, bool commas);

    // Print string, replacing '_x' with 'X', and capitalizing first word.
    void strptr_PrintCamel(algo::strptr str, cstring& out);

    // Convert CamelCase to lower_under.
    // A consecutive run of caps gets a single leading underscore.
    // Leading cap doesn't get an underscore
    // Example: "abcd testString"  -> "abcd test_string"
    // Example: "Abcd" -> "abcd"
    // Example: "BBOPrice" -> "bboprice"
    // Example: "SymbolID" -> "symbol_id"
    void strptr_PrintLowerUnder(algo::strptr str, cstring &out);

    // Output string STR to OUT changing all characters to uppercase.
    void strptr_ToUpper(algo::strptr str, cstring& out);
    void u128_PrintHex(u128 value, cstring &out, bool prefix);
    void u64_PrintHex(u64 value, cstring &out, int atleast, bool prefix,  bool caps);

    // Print value VALUE to string OUT,
    // producing least ATLEAST hex characters.
    // Include prefix 0x.
    void u64_PrintHex(u64 value, cstring &out, int atleast);

    // Print value VALUE to string OUT,
    // producing least ATLEAST hex characters.
    // if PREFIX is specified, include prefix 0x.
    void u64_PrintHex(u64 value, cstring &out, int atleast, bool prefix);

    // print character as c++ character, surrounded by single quotes
    // and properly escaped.
    void char_PrintCppSingleQuote(int c, cstring &out);
    void memptr_Print(memptr ary, algo::cstring &out);

    // print 64 bytes per line
    void memptr_PrintHex(memptr bytes, cstring &str);

    // prints a character suitable for appearance inside an XML string.
    // character is either printed as-is, or is escaped so that its meaning does not change.
    void char_PrintXml(char c, cstring &out, bool quoted);

    // Print string NAME as a TeX identifier to OUT, converting digits to letters
    // (0 -> zero, etc) and squashing other characters not belonging to TeX
    // identifier character class
    void strptr_PrintTexIdent(strptr name, cstring &out);

    // print STR to OUT, escaping TeX characters that switch modes
    // (_, #, &, $)
    void strptr_PrintTexString(strptr str, cstring &out);

    // Print string using XML rules.
    void strptr_PrintXml(algo::strptr s, cstring &out, bool quoted);

    // Append N instances of character C to string OUT.
    void char_PrintNTimes(char c, cstring &out, int n);
    void strptr_PrintAligned(algo::strptr str, cstring &out, int nplaces, algo::TextJust align, char c);
    void cstring_Print(algo::cstring &row, algo::cstring &str);
    void strptr_Print(const algo::strptr &row, algo::cstring &str);

    // Append a space unless the string already ends with a space
    void  MaybeSpace(cstring &str);

    // print value*100
    // Example:
    // double_PrintPercent(0.334, str, 1) -> "33.4%"
    void double_PrintPercent(double value, cstring &str, int prec);
    void i32_Range_Print(algo::i32_Range &r, cstring &o);
    void double_PrintWithCommas(double value, cstring &str, int prec);

    // ignore:bigret
    algo::cstring double_PrintWithCommas(double value, int prec);

    // Print scaled decimal.
    // VALUE           integer value
    // SCALE           log10 to get the real value: real value = value / pow(10,scale)
    // BUF             target buffer. buffer must be large enough.
    // FIXED           if true, make sure there are exactly SCALE characters after decimal point (and always emit decimal point)
    // RETURN          number of characters printed.
    // Example:
    // u64_FmtBufDec(0    , 2, .. false ) -> "0"
    // u64_FmtBufDec(0    , 2, .. true  ) -> "0.00"
    // u64_FmtBufDec(123  , 1, .. false ) -> "12.3"
    // u64_FmtBufDec(123  , 2, .. false ) -> "1.23"
    // u64_FmtBufDec(123  , 2, .. true  ) -> "1.23"
    // u64_FmtBufDec(1230 , 3, .. false ) -> "1.23"
    // u64_FmtBufDec(1230 , 3, .. true  ) -> "1.230"
    u32 u64_FmtBufDec(u64 num, int scale, u8 *buf, bool fixed);
    u32 u64_FmtBufDec(u64 num, int scale, u8 *buf);

    // print TIME to STR, according to spec SPEC
    // todo: document format chars...
    void TimeStruct_Print(const TimeStruct &time, cstring &str, const algo::strptr &spec);
    void Tuple_Print(algo::Tuple &row_, cstring &str);
    void Bool_Print(algo::Bool row, algo::cstring &str);
    void UnDiff_Print(UnDiff row, algo::cstring &str);
    void UnixDiff_Print(UnixDiff row, algo::cstring &str);
    void UnTime_Print(algo::UnTime row, algo::cstring &str);
    void UnixTime_Print(algo::UnixTime row, algo::cstring &str);
    void UnTime_PrintSpec(UnTime t, algo::cstring &out, const algo::strptr &spec);
    void UnixTime_PrintSpec(UnixTime t, algo::cstring &out, const algo::strptr &spec);
    void UnDiff_PrintSpec(UnDiff   t, algo::cstring &out, const algo::strptr &spec);
    void UnixDiff_PrintSpec(UnixDiff t, algo::cstring &out, const algo::strptr &spec);
    void Ipmask_Print(algo::Ipmask &row, algo::cstring &str);
    void Errcode_Print(algo::Errcode &row, algo::cstring &str);

    // Append STR to OUT, using comma-separated-values encoding
    void strptr_PrintCsv(algo::strptr str, cstring &out);
    void URL_Print(algo::URL &url, cstring &str);

    // Append STR to OUT, and pad remainder with character FILL
    void strptr_PrintPadRight(algo::strptr str, cstring &out, int nplaces, char fill);

    // Append STR to OUT, and pad remainder with space characters
    void strptr_PrintPadRight(algo::strptr str, cstring &out, int nplaces);

    // Append STR to OUT, padding start with character FILL
    void strptr_PrintPadLeft(algo::strptr str, cstring &out, int nplaces, char fill);

    // Append STR to OUT, padding start with spaces.
    void strptr_PrintPadLeft(algo::strptr str, cstring &out, int nplaces);

    // Append STR to OUT as an SQL string using quote character Q.
    // Escaped characters:
    // '\' (backslash)
    // ' quoted with '' if q is ' and not quoted otherwise. (Compatible with MariaDB & T-SQL.)
    // " quoted with "" if q is " and not quoted otherwise. (Compatible with MariaDB & T-SQL.)
    // NUL (ascii zero)
    // \n
    // \r
    // \t
    void strptr_PrintSql(algo::strptr str, cstring &out, char q);
    void strptr_PrintSql(algo::strptr str, cstring &out);
    void Attr_Print(algo::Attr &attr, cstring &str);

    // Print a string suitable for parsing with Tuple
    // If the string doesn't need quotes, it is printed as-is.
    // If quotes are needed, first determine whether to use single or double quotes to minimize
    // escaping. Then print the string using char_PrintCppEsc for quoting.
    // Empty string is printed as ""
    void PrintAttrSpace(cstring &str, algo::strptr name, const strptr &value);
    void ListSep_Print(const algo::ListSep &t, algo::cstring &str);

    // copy string B to TO, using ORIG as a case template
    // I.e. PrintCopyCase("AbcD", to, "somestring") -> "SomEstring"
    void strptr_PrintCopyCase(const algo::strptr &orig, cstring &to, const algo::strptr &b);
    bool Tuple_ReadStrptrMaybe(Tuple &row, algo::strptr s);

    // T             target tuple. the tuple is not emptied before parsing.
    // STR           source string
    // ATTRONLY        if set, all loaded attrs are appended to the ATTRS
    // array. otherwise, the first attr becomes HEAD.
    // CMT_CHAR      character at which to stop parsing.
    // Parse sequence of attrs (name-value pairs) into tuple T.
    // Roughly:
    // ATTR       -> VALUE | VALUE ':' VALUE
    // VALUE      -> IDENTIFIER | C++-STRING
    // Return value: FALSE if unterminated string or other badness found in data.
    bool Tuple_ReadStrptr(algo::Tuple &tuple, strptr str, bool attronly);

    // Implementations of built-in read functions for AMC.
    // basis function for reading tuples.
    // scan ITER for identifier, or quoted string.
    // return FALSE if attribute is malformed (i.e. unterminated string)
    bool cstring_ReadCmdarg(cstring &out, StringIter &S, bool is_value);

    // Read Charset from list of chars.
    // Every character in RHS is simply added to the bitset
    void Charset_ReadStrptrPlain(algo::Charset &lhs, strptr desc);
    bool Charset_ReadStrptrMaybe(algo::Charset &lhs, strptr rhs);

    // Print STR to OUT in a way that's acceptable as input for bash.
    void strptr_PrintBash(strptr str, cstring &out);

    // encode uri component.
    // before ? use plus=false (no + substitution by space)
    void strptr_PrintUri(strptr str, cstring &out, bool plus);

    // Print string, escaping characters in a way suitable for graphviz dot format
    void strptr_PrintDot(strptr s, cstring &out);

    // print binary octet string as hex
    void Sha1sig_Print(algo::Sha1sig &sha1sig, cstring &out);
    bool Sha1sig_ReadStrptrMaybe(algo::Sha1sig &sha1sig, algo::strptr str);

    // Write character C into string OUT, using C++ character escapement rules
    // QUOTE_CHAR also gets escaped.
    void char_PrintCppEsc(char c, cstring &out, char quote_char);

    // Print STR, surrounded by quotes as C++ string
    // surrounded by QUOTE_CHAR quotes, to buffer OUT.
    // All string characters are escaped using char_PrintCppEsc.
    void strptr_PrintCppQuoted(algo::strptr str, cstring &out, char quote_char);

    // Print STR as a C++ string to OUT.
    void strptr_PrintCpp(algo::strptr str, cstring &out);

    // Print STR as a valid SSIM key/value to OUT.
    // SSIM tokens use quotes only when the printed value contains
    // characters outside of the 'safe' set a-zA-Z0-9_;&*^%$@.!:,+/-
    void strptr_PrintSsim(algo::strptr str, cstring &out);

    // Begin append key/value to string LHS
    int BeginKeyval(cstring &lhs, const strptr &name);

    // End append key/value to string LHS
    cstring &EndKeyval(cstring &lhs, int start);

    // Append key-value pair NAME:VALUE to string STR
    void PrintAttr(cstring &str, const algo::strptr &name, const strptr &value);

    // Append key-value pair NAME:VALUE to string STR, and remove all
    // characters from VALUE (used by amc)
    void PrintAttrSpaceReset(cstring &str, const algo::strptr &name, cstring &value);

    // Append key-value pair NAME:VALUE to string STR,
    // and remove all characters from VALUE (used by amc)
    void PrintAttrSpaceReset(cstring &str, const char *name, cstring &value);

    // Convert VALUE to a string, encoded as a u64.
    // Value must be >=0 and <100000000.
    // Resulting u64 is little-endian -- MSB contains first byte
    // of the resulting string. To be used as an ascii string, value
    // 0x3030303030303030 (8 ascii '0's) must be added to the result.
    // Don't use this function. Use u32_FmtBufSmall, and u64_FmtBuf.
    u64 u32_CvtLestr8Small(u32 value);

    // Format VALUE, which should be >=0 and <100000000,
    // as a string and write it to buffer BUF. Return length of resulting string.
    // Buffer must be big enough.
    u32 u32_FmtBufSmall(u32 value, u8 *buf);

    // Format VALUE, which is an arbitrary u64,
    // as a string and write it to buffer BUF.
    // Return length of resulting string.
    // Buffer must be big enough.
    u32 u64_FmtBuf(u64 value, u8 *buf);

    // Format VALUE, which is an arbitrary u32,
    // as a string and write it to buffer BUF.
    // Return length of resulting string.
    // Buffer must be big enough.
    u32 u32_FmtBuf(u32 value, u8 *buf);
    u64 aParseNum16(strptr s, u32 &ok);

    // STR         little-endian string up to 2 chars in length
    // LEN         length of STR
    // RESULT      value of hex number encoded in STR
    // RETURN      number of characters to skip (0 - unsuccessful)
    int ParseHex2(u32 str, int len, u8 &result);

    // STR        little-endian string up to 4 chars
    // LEN        length of STR
    // RESULT     Octal value encoded in str
    // RETURN     number of characters to skip (0 - unsuccessful)
    int ParseOct3(u32 str, int len, u8 &result);
    u32 ParseNum4(u32 x, u32 len, u32 &ok);
    u32 ParseNum8(u64 x, u32 len, u32 &ok);

    // Print a string as a classic regex, escaping all special
    // characters. This regex will only match the specified string.
    void strptr_PrintRegxSql(algo::strptr value, cstring &str);

    // -------------------------------------------------------------------
    // include/algo/fmt.inl.h
    //
    inline u32 u16_FmtBuf(u32 value, u8 *buf);
    inline algo::cstring &operator<<(algo::cstring &out, const char* t);
    inline algo::cstring &operator<<(algo::cstring &out, const ListSep &t);
    inline algo::cstring &operator<<(algo::cstring &out, const memptr &t);
    inline algo::cstring &operator<<(algo::cstring &out, const strptr &t);
    inline algo::cstring &operator<<(algo::cstring &out, void (*t)(algo::cstring &));
}

// -----------------------------------------------------------------------------

namespace algo {
    template<class T> inline tempstr &operator <<(const tempstr &lhs, const T &rhs) {
        (cstring&)lhs << rhs;
        return (tempstr&)lhs;
    }
}

// Keyval printing support. Usage:
// tempstr() << Keyval("a", something printable).
// - insert double space if necessary
// - if key is empty, only value is printed (i.e. anomymous arg)
// - both key and value are ssim-quoted (i.e. c++ quoting rules but outer quotes can be single or double)
// - reasonably fast.
template<class Val> struct Keyval_ {
    const strptr &key;
    Val &val;
    Keyval_(const strptr &a, const Val &b) :key(a),val((Val&)b) {
    }
};
template<class Val> inline cstring &operator <<(cstring &lhs, const Keyval_<Val> &kv) {
    int start=algo::BeginKeyval(lhs,kv.key);
    lhs<<kv.val;
    return algo::EndKeyval(lhs,start);
}
template<class Val> Keyval_<Val> inline Keyval(const strptr &key, const Val &val) {
    return Keyval_<Val>(key,val);
}

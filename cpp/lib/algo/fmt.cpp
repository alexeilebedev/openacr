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
// Source: cpp/lib/algo/fmt.cpp -- Print to string / Read from string
//
// Created By: alexei.lebedev vladimir.parizhsky
// Authors: alexei.lebedev
// Recent Changes: alexei.lebedev hayk.mkrtchyan edward.danileyko
//

#include "include/algo.h"
#ifndef WIN32
#include <arpa/inet.h>
#endif
#ifdef WIN32
#include <stdio.h>
#endif

// -----------------------------------------------------------------------------

// Examine string VALUE
// Determine if it needs quoting as an ssim key/value,
// and return appropriate quote character. If no quote character is needed,
// return NUL.
char algo::PickSsimQuoteChar(strptr value) {
    int singleq = 0;
    int needquotes = elems_N(value) == 0;
    frep_(i,elems_N(value)) {
        char c = value[i];
        if (c=='\'') {
            singleq++;
            needquotes++;
        } else if (c=='"') {
            singleq--;
            needquotes++;
        } else if (unsigned(c) >= 127 || !algo_lib::SsimQuotesafeQ(unsigned(c))) {
            needquotes++;
        }
    }
    return needquotes ? (singleq>=0 ? '"' : '\'') : 0;
}

// -----------------------------------------------------------------------------

bool char_ReadStrptrMaybe(char &row, algo::strptr str) {
    bool retval = true;
    if (LIKELY(elems_N(str)==1)) {
        row = str[0];
    } else {
        retval = false;
        algo_lib::SaveBadTag("comment", "bad char");
        algo_lib::SaveBadTag("value",str);
    }
    return retval;
}

// -----------------------------------------------------------------------------

bool float_ReadStrptrMaybe(float &row, algo::strptr str) {
    bool retval = true;
    double val;
    if (LIKELY(double_ReadStrptrMaybe(val, str))) {
        row = val;
    } else {
        retval = false;
    }
    return retval;
}

// -----------------------------------------------------------------------------

bool double_ReadStrptrMaybe(double &row, algo::strptr str) {
    bool retval = true;
    algo::StringIter iter(str);
    double val=0;
    if (LIKELY(elems_N(str)==0 || (TryParseDouble(iter, val) && iter.index>0))) {
        row = val;
    } else {
        retval = false;
        algo_lib::SaveBadTag("comment", "bad number");
        algo_lib::SaveBadTag("value",str);
    }
    return retval;
}

// -----------------------------------------------------------------------------

bool bool_ReadStrptrMaybe(bool &row, algo::strptr str) {
    bool retval = true;
    Bool ret;
    if (LIKELY(algo::value_SetStrptrMaybe(ret, str))) {
        row = ret.value;
    } else {
        retval = false;
        algo_lib::SaveBadTag("comment", "invalid bool");
        algo_lib::SaveBadTag("value",str);
    }
    return retval;
}

// -----------------------------------------------------------------------------

// Read time from STR to ROW
// Return success code.
// If funciton does not succeed, ROW is not modified
// Several formats are supported:
// %Y-%m-%dT%T
// %Y-%m-%d %T
// %Y/%m/%d %T
// %Y-%m-%d
// %Y/%m/%d
// Where %T is %H:%M:%S.%X
// And %X is the nanosecond portion
bool algo::UnTime_ReadStrptrMaybe(algo::UnTime &row, algo::strptr str) {
    bool retval = true;
    algo::StringIter iter(str);
    TimeStruct time_struct;
    retval = TimeStruct_Read(time_struct, iter, "%Y-%m-%dT%T");//ISO 8601
    if (!retval) {
        iter.index = 0;
        Refurbish(time_struct);
        retval = TimeStruct_Read(time_struct, iter, "%Y-%m-%d %T");
    }
    if (!retval) {
        iter.index = 0;
        Refurbish(time_struct);
        retval = TimeStruct_Read(time_struct, iter, "%Y/%m/%d %T");
    }
    if (!retval) {
        iter.index = 0;
        Refurbish(time_struct);
        retval = TimeStruct_Read(time_struct, iter, "%Y-%m-%d");
    }
    if (!retval) {
        iter.index = 0;
        Refurbish(time_struct);
        retval = TimeStruct_Read(time_struct, iter, "%Y/%m/%d");
    }
    if (LIKELY(retval)) {
        row = ToUnTime(time_struct);
    } else {
        retval = false;
        algo_lib::SaveBadTag("comment", "bad time");
        algo_lib::SaveBadTag("value",str);
    }
    return retval;
}

// -----------------------------------------------------------------------------

bool algo::UnDiff_ReadStrptrMaybe(UnDiff &row, algo::strptr str) {
    bool retval = true;
    algo::StringIter iter(str);
    TimeStruct time_struct;
    strptr format = "%-%T";
    retval = TimeStruct_Read(time_struct, iter, format);
    if (LIKELY(retval)) {
        row = ToUnDiff(time_struct);
    } else {
        retval = false;
        algo_lib::SaveBadTag("comment", "bad time");
        algo_lib::SaveBadTag("value",str);
        algo_lib::SaveBadTag("format",format);
    }
    return retval;
}

// -----------------------------------------------------------------------------

bool algo::UnixTime_ReadStrptrMaybe(algo::UnixTime &row, algo::strptr str) {
    bool retval = true;
    UnTime tmp;
    if (LIKELY(UnTime_ReadStrptrMaybe(tmp, str))) {
        row = ToUnixTime(tmp);
    } else {
        retval = false;
    }
    return retval;
}

bool algo::cstring_ReadStrptrMaybe(algo::cstring &row, algo::strptr str) {
    bool retval = true;
    row = str;
    return retval;
}

// -----------------------------------------------------------------------------

// Parse a URL from string STR to OUT.
// The format of a URL is
// protocol://user:password@somehost:port<-source_interface/dir1/dir2
// URL fields are
// protocol, username, password, server, port, host, source_addr_host, dir
//
//
// Windows pathnames are supported, e.g.
// when parsing
//    file://c:/dir/dir2
// c: will not be parsed as a username, but as part of the pathname.
//
bool algo::URL_ReadStrptrMaybe(URL &out, algo::strptr str) {
    bool retval = true;
    Refurbish(out);
    out.port = -1;
    frep_(i,elems_N(str)-2) {
        if (str[i]==':' && str[i+1]=='/' && str[i+2]=='/') {    // find ://
            out.protocol = FirstN(str,i);
            str = RestFrom(str,i+3);
        }
        if (!algo_lib::IdentCharQ(str[i])) {
            break;
        }
    }
    if (out.port < 0) {
        if (StrEqual(out.protocol,"ftp", false)){
            out.port = 21;
        } else if (StrEqual(out.protocol,"http", false)){
            out.port = 80;
        }
    }
    if (StartsWithQ(out.protocol,"file") && elems_N(str)>1 && str[1] == ':') {  // file://c:/dir/dir2    -> recognizes c:
        out.dir = str;
    } else {
        i32_Range R = TFind(str,'/');
        if(StrEqual(out.protocol,"scp",false)){
            R = TFind(str,':');
        }
        out.dir = R.end > R.beg ? RestFrom(str,R.end - (R.beg>0)) : strptr(); // proto:///dir/foo  -- dir = dir/foo
        str     = FirstN(str,R.beg); // www.example.com:80
        R            = TRevFind(str, '@');
        strptr login = FirstN(str, R.beg);
        str          = RestFrom(str, R.end);
        R            = TFind(login,':');
        out.username = FirstN(login, R.beg);
        out.password = RestFrom(login, R.end);
        R          = substr_FindFirst(str, strptr("<-"));
        if (R.beg == R.end) R = substr_FindFirst(str, strptr("~-"));
        out.host   = RestFrom(str,R.end);
        str        = FirstN(str,R.beg);
        if (elems_N(str)>2 && str[0]=='(' && str[elems_N(str)-1]==')') {
            str=GetRegion(str,1,elems_N(str)-2);
            Ipmask mask;
            i32_Range comma = substr_FindLast(str, strptr(","));
            retval = retval && Ipmask_ReadStrptrMaybe(mask,FirstN(str, comma.beg));
            if (mask.ip_host != 0) {
                out.source_addr_host = mask.ip_host;
            }
            str = RestFrom(str, comma.end);
        }
        R      = TFind(str, ':');
        out.server = FirstN(str,R.beg);
        algo::StringIter iter(str,R.end);
        int ret = TryParseI32(iter, out.port);
        (void)ret; // coverity
    }
    return retval;
}

bool ietf::Ipv4_ReadStrptrMaybe(ietf::Ipv4 &ip, algo::strptr str) {
    bool retval = true;
    ip.ipv4 = 0;
    algo::StringIter iter(str);
    while (!iter.EofQ()) {
        int piece;
        if (TryParseI32(iter, piece)) {
            ip.ipv4 = ip.ipv4 << 8 | piece;
            (void)SkipChar(iter, '.');
        } else {
            iter.index = iter.expr.n_elems;// go to the end
        }
    }
    return retval;
}

// -----------------------------------------------------------------------------

// Parse an IpV4 address from STR to IP
// Return success value. If not successful, output value is not modified.
bool ietf::Ipv4Addr_ReadStrptrMaybe(ietf::Ipv4Addr &ip, algo::strptr str) {
    bool retval = true;
    u32 addr = 0;
    algo::StringIter iter(str);
    while (!iter.EofQ()) {
        int octet;
        if (TryParseI32(iter, octet)) {
            addr = addr << 8 | octet;
            if (!SkipChar(iter, '.')) {
                iter.index = iter.expr.n_elems;// go to the end
            }
        } else {
            iter.index = iter.expr.n_elems;// go to the end
        }
    }
    addr_Set(ip,addr);
    return retval;
}

// -----------------------------------------------------------------------------

bool algo::Ipmask_ReadStrptrMaybe(Ipmask &row, algo::strptr str) {
    bool retval = true;
    algo::StringIter iter(str);
    int ip = 0;
    int mask = 0;
    int asterisks = 0;
    // this code parses expressions like 192.168.*.*
    // and 2130706433 (which is 127.0.0.1)
    // but it can't parse a combination of numbers that don't fit one byte and asterisks, such as
    // 1234.*.1234
    rep_(i,4) {
        if (SkipChar(iter, '*')) {
            asterisks++;
            ip=ip<<8;
            mask=mask<<8;
        } else {
            int piece;
            if (TryParseI32(iter, piece)) {
                ip=ip<<8|piece;
                mask=mask<<8|0xff;
            } else {
                return true;// do nothing
            }
        }
        if (i!=3 && !SkipChar(iter, '.')) {
            break;
        }
    }
    if (asterisks == 0) {
        mask = 0xffffffff;
    }
    if (SkipChar(iter, '/')) {
        u32 bits=0;
        int i=0;
        for(; i<4; i++) {
            int piece;
            if (TryParseI32(iter, piece)) {
                bits=bits<<8|piece;
            } else {
                return true;// do nothing
            }
            if (i!=3 && !SkipChar(iter, '.')) {
                break;
            }
        }
        if (i==4) {
        } else if (i==0) {
            if (bits!=0) {// special case: A.B.C.D/0 -> bits 0
                bits = u32(-1) << (32-bits);
            }
        }
        mask &= bits;
    }
    row.ip_host=ip;
    row.mask = mask;
    return retval;
}

// -----------------------------------------------------------------------------

void algo::Ptr_Print(void *ptr, algo::cstring &out) {
    u64_PrintHex(u64(ptr), out, sizeof(u64), true ,false);
}

// pads with zeros on the left so that at least 'atleast' characters are output.
void algo::i64_PrintPadLeft(i64 num, algo::cstring &out, int atleast) {
    i64 number=num;
    char c[32];
    int ndig = 0;
    int neg=number<0;
    number=Abs(number);
    if (!number) {
        c[ndig++]='0';
    } else {
        while (number!=0) {
            c[ndig++]=char('0' + number%10);
            number/=10;
        }
    }
    strptr result(ch_AllocN(out, i32_Max(ndig,atleast)+neg));
    int i = 0;
    if (neg) {  // minus sign
        result[i++] = '-';
        atleast--;
    }
    for (int k=ndig; k<atleast; k++) {  // pad with zeros from left
        result[i++]='0';
    }
    while (--ndig >= 0) {   // print the rest
        result[i++]=c[ndig];
    }
}

// -----------------------------------------------------------------------------

// Assuming SRC is a number, Transfer SRC to OUT, inserting
// commas between groups of 3 numbers.
// 0 -> 0
// 11 -> 11
// 222 -> 222
// 3333 -> 3,333
// 4567.3 -> 4,567.3
// 1.11111 -> 1.11111
void algo::strptr_PrintWithCommas(strptr src, algo::cstring &out) {
    int dec = Find(src, '.');
    if (dec==-1) {
        dec=src.n_elems;
    }
    ch_Reserve(out, src.n_elems * 2);
    int i=0;
    if (src.elems[0]=='-') {
        out.ch_elems[out.ch_n++] = src.elems[i++];
    }
    int remaining=(dec-i)%3;
    if (remaining==0) {
        remaining += 3;
    }
    for (; i<dec; i++) {
        if (remaining==0) {
            out.ch_elems[out.ch_n++] = ',';
            remaining += 3;
        }
        remaining--;
        out.ch_elems[out.ch_n++] = src.elems[i];
    }
    // print remainder of the string as-is
    out << RestFrom(src,dec);
}

// -----------------------------------------------------------------------------

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
//           ->            empty string is not touched
// Since the string may be edited, you can't pass compile-time constants
// to this function. In fact the only reason this function is not private to algo_lib
// is because of unit testing.
// If the initial string is empty, nothing is done.
// BEWARE: this function will happily convert 1e+60 to 1e+6
//
void algo::strptr_TrimZerosRight(strptr &str) {
    int n=str.n_elems;
    int dotpos=0;
    for (; dotpos < str.n_elems; dotpos++) {
        if (str.elems[dotpos] == '.') {
            break;
        }
    }
    if (n > 0) {
        // kill trailing zeros, but don't leave an empty string
        while (n > dotpos && str.elems[n-1] == '0') {
            n--;
        }
        // kill trailing .
        if (n > 0 && str.elems[n-1] == '.') {
            n--;
        }
        // kill trailing -
        // (this effectively kills -. as well)
        if (n > 0 && str.elems[n-1] == '-') {
            n--;
        }
        // fix empty string
        if (n == 0) {
            str.elems[n++] = '0';
        }
        str.n_elems=n;
    }
}

// -----------------------------------------------------------------------------

// Return true if a number appears to be
// formatted using scientific notation
static inline bool ScientificQ(strptr str) {
    bool ret=false;
    for (int i=0; i<str.n_elems; i++) {
        if (str.elems[i] == 'e' || str.elems[i] == 'E') {
            ret=true;
            break;
        }
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Print double D into string OUT with PRECISION digits
// after the decimal point.
// If OMIT_ZEROS is specified, trailing zeros that are safe to omit are omitted.
// If COMMAS is specified, the large numbers are printed in groups of 3 digits
// with commas between them.
void algo::double_PrintPrec(double d, algo::cstring &out, int precision, bool omit_zeros, bool commas) {
    char buf[512];
    precision = Clipped(precision,20);
    // create a format string for the specified precision
    // i.e. %.7lf
    char fmt[8];
    int i=0;
    fmt[i++] = '%';
    fmt[i++] = '.';
    if (precision < 10) {
        fmt[i++] = '0'+precision;
    } else {
        fmt[i++] = '1';
        fmt[i++] = '0'+precision-10;
    }
    fmt[i++] = 'l';
    fmt[i++] = 'f';
    fmt[i++] = 0;
    int n=snprintf(buf,sizeof(buf),fmt,d);
    strptr result(buf,n);
    if (ScientificQ(result)) {
        omit_zeros=false;
        commas=false;
    }
    if (omit_zeros) {
        strptr_TrimZerosRight(result);
    }
    if (commas) {
        strptr_PrintWithCommas(result,out);
    } else {
        out << result;
    }
}

// -----------------------------------------------------------------------------

// Print string, replacing '_x' with 'X', and capitalizing first word.
void algo::strptr_PrintCamel(algo::strptr str, algo::cstring& out) {
    int needuc=true;
    for (int i=0; i<str.n_elems; i++) {
        if (str.elems[i] == '_' && !needuc) {
            needuc=true;
        } else {
            int ch=str.elems[i];
            if (needuc && ch != '_') {
                ch=algo::ToUpper(ch);
                needuc=false;
            }
            out << char(ch);
        }
    }
}

// -----------------------------------------------------------------------------

// Convert CamelCase to lower_under.
// A consecutive run of caps gets a single leading underscore.
// Leading cap doesn't get an underscore
// Example: "abcd testString"  -> "abcd test_string"
// Example: "Abcd" -> "abcd"
// Example: "BBOPrice" -> "bboprice"
// Example: "SymbolID" -> "symbol_id"
void algo::strptr_PrintLowerUnder(algo::strptr str, algo::cstring &out) {
    int prevlc = false;
    frep_(i,str.n_elems) {
        char c= str.elems[i];
        if (algo_lib::UpperCharQ(c)) {
            if (prevlc) {
                out << '_';
            }
        }
        int lower = ToLower(c);
        prevlc = c==lower;
        out << char(lower);
    }
}

// -----------------------------------------------------------------------------

// Output string STR to OUT changing all characters to uppercase.
void algo::strptr_ToUpper(algo::strptr str, algo::cstring& out) {
    u32 n = elems_N(str);
    u32 i =0;
    while (i < n) {
        out << ToUpper(str[i++]);
    }
}

void algo::u128_PrintHex(u128 value, algo::cstring &out, bool prefix) {
    if (prefix) out<< "0X";
    char c[32+1];
    c[32]=0;
    int i=32;
    do {
        i--;
        c[i]= "0123456789ABCDEF"[u32(value)&15];
        value = value>>u32(4);
    } while (value);
    out<<c+i;
}

void algo::u64_PrintHex(u64 value, algo::cstring &out, int atleast, bool prefix,  bool caps) {
    if (prefix) out<<(caps ? "0X" : "0x");
    char c[16+1];
    c[16]=0;
    int i=16;
    int lim = i32_Max(0,16-atleast);
    do {
        i--;
        c[i]=(caps ? "0123456789ABCDEF" : "0123456789abcdef")[value&15];
        value>>=4;
    } while (value||i>lim);
    out<<c+i;
}

// -----------------------------------------------------------------------------

// Print value VALUE to string OUT,
// producing least ATLEAST hex characters.
// Include prefix 0x.
void algo::u64_PrintHex(u64 value, algo::cstring &out, int atleast) {
    u64_PrintHex(value,out,atleast,true,false);
}

// -----------------------------------------------------------------------------

// Print value VALUE to string OUT,
// producing least ATLEAST hex characters.
// if PREFIX is specified, include prefix 0x.
void algo::u64_PrintHex(u64 value, algo::cstring &out, int atleast, bool prefix) {
    u64_PrintHex(value,out,atleast,prefix,false);
}

// -----------------------------------------------------------------------------

// print character as c++ character, surrounded by single quotes
// and properly escaped.
void algo::char_PrintCppSingleQuote(int c, algo::cstring &out) {
    out<<'\'';
    if (c=='\'') {
        out<<"\\'";
    } else {
        char_PrintCppEsc(char(c), out, '\'');
    }
    out<<'\'';
}

void algo::memptr_Print(memptr ary, algo::cstring &out) {
    out << '"';
    ind_beg_aryptr(u8,c,ary) {
        if (c == '\\' || c == '"') {
            out << "\\" << c;
        } else if (c >= 20 && c < 128) {
            out << char(c);
        } else {
            out << "\\x";
            u64_PrintHex(c,out,2,false,true);
        }
    }ind_end_aryptr;
    out << '"';
}

// -----------------------------------------------------------------------------

// print 64 bytes per line
void algo::memptr_PrintHex(memptr bytes, algo::cstring &str) {
    bool non_printable=false;
    frep_(i,elems_N(bytes)) {
        if (!isprint(bytes[i])){
            non_printable=true;
            break;
        }
    }
    if (non_printable) {
        frep_(i,elems_N(bytes)) {
            u64_PrintHex(u32(bytes[i]),str, 2,false,false);
            str<<(((i % 15) == 14 ) ? "\n" : " ");
        }
    } else {
        str<<ToStrPtr(bytes);
    }
}

// -----------------------------------------------------------------------------

// prints a character suitable for appearance inside an XML string.
// character is either printed as-is, or is escaped so that its meaning does not change.
void algo::char_PrintXml(char c, algo::cstring &out, bool quoted) {
    switch (c) {
    case '&':
        out<<"&amp;";
        break;
    case '<':
        out<<"&lt;";
        break;
    case '>':
        out<<"&gt;";
        break;
    case '"':
        out<<"&quot;";
        break;
    case '\'':
        out<<"&apos;";
        break;
    default:
        // #AP# XML does not need escaping of other chars: non-printable, etc.
        // with one exception, some xml parser implementations treat newline
        // inside attribute value as whitespace
        if (quoted && iscntrl(c)) {
            out<<"&#x";
            u64_PrintHex(c&0xff,out, 2,false,true);
            out<<";";
        } else {
            out<<c;
        }
        break;
    }
}

// -----------------------------------------------------------------------------

// Print string NAME as a TeX identifier to OUT, converting digits to letters
// (0 -> zero, etc) and squashing other characters not belonging to TeX
// identifier character class
void algo::strptr_PrintTexIdent(strptr name, algo::cstring &out) {
    for (int i=0; i<name.n_elems; i++) {
        char ch=name.elems[i];
        switch(ch) {
        case '0': out << "Zero"; break;
        case '1': out << "One"; break;
        case '2': out << "Two"; break;
        case '3': out << "Three"; break;
        case '4': out << "Four"; break;
        case '5': out << "Five"; break;
        case '6': out << "Six"; break;
        case '7': out << "Seven"; break;
        case '8': out << "Eight"; break;
        case '9': out << "Nine"; break;
        case '.': break;
        case ',': break;
        case ':': break;
        case '<': break;
        case '>': break;
        case '*': out << "Star"; break;
        case '_': break;
        default: out << ch;
        }
    }
}

// -----------------------------------------------------------------------------

// print STR to OUT, escaping TeX characters that switch modes
// (_, #, &, $)
void algo::strptr_PrintTexString(strptr str, algo::cstring &out) {
    ch_Reserve(out,str.n_elems*4);
    int j=ch_N(out);
    for (int i=0; i<str.n_elems; i++) {
        switch(str[i]) {
        case '_':
        case '#':
        case '&':
            // replacing ':' with '\:' causes errors in table of contents
            // how is this not a bug in TeX???
        case '$':
            out.ch_elems[j++] = '\\';
            out.ch_elems[j++] = str[i];
            break;
        default:
            out.ch_elems[j++] = str[i];
            break;
        }
    }
    out.ch_n=j;
}

// -----------------------------------------------------------------------------

// Print string using XML rules.
void algo::strptr_PrintXml(algo::strptr s, algo::cstring &out, bool quoted) {
    strptr q;
    if (quoted) {
        q="\"";
    }
    out << q;
    frep_(i,elems_N(s)) {
        char_PrintXml(s[i], out, quoted);
    }
    out << q;
}

// -----------------------------------------------------------------------------

// Append N instances of character C to string OUT.

void algo::char_PrintNTimes(char c, algo::cstring &out, int n) {
    if (n>0) {
        Fill(ch_AllocN(out, n),c);
    }
}

void algo::strptr_PrintAligned(algo::strptr str, algo::cstring &out, int nplaces, algo::TextJust align, char c) {
    int extra  = i32_Max(nplaces - elems_N(str), 0);
    int before = 0;
    if (align == algo_TextJust_j_center) {
        before = extra/2;
    }
    if (align == algo_TextJust_j_right) {
        before = extra;
    }
    char_PrintNTimes(c, out, before);
    out << str;
    char_PrintNTimes(c, out, extra-before);
}

void i32_Print(i32 i, algo::cstring &str) {
    ch_Reserve(str, 32);
    u32 u = i;
    int n = str.ch_n;
    if (i<0) {
        str.ch_elems[n] = '-';
        n++;
        u = -i;
    }
    n += algo::u32_FmtBuf(u,(u8*)(str.ch_elems+n));
    str.ch_n=n;
}

void i64_Print(i64 i, algo::cstring &str) {
    ch_Reserve(str, 32);
    u64 u = i;
    int n = str.ch_n;
    if (i<0) {
        str.ch_elems[n] = '-';
        n++;
        u = -i;
    }
    n += algo::u64_FmtBuf(u,(u8*)(str.ch_elems+n));
    str.ch_n=n;
}

void i16_Print(i16 i, algo::cstring &str) {
    i32_Print(i, str);
}

void u16_Print(u16 i, algo::cstring &str) {
    u32_Print(i, str);
}

void u8_Print(u8 i, algo::cstring &str) {
    u32_Print(i, str);
}

void i8_Print(i8 i, algo::cstring &str) {
    i32_Print(i, str);
}

void u32_Print(u32 i, algo::cstring &str) {
    ch_Reserve(str, 32);
    int n = str.ch_n;
    n += algo::u32_FmtBuf(i,(u8*)(str.ch_elems+n));
    str.ch_n=n;
}

void u64_Print(u64 i, algo::cstring &str) {
    ch_Reserve(str, 32);
    int n = str.ch_n;
    n += algo::u64_FmtBuf(i,(u8*)(str.ch_elems+n));
    str.ch_n=n;
}

void u128_Print(u128 num, algo::cstring &out) {
    char buf[64];
    int i=64;
    do {
        i--;
        buf[i] = '0' + num % u32(10);
        num=num / u32(10);
    } while (num != 0 && i > 0);
    out << strptr(buf+i, sizeof(buf)-i);
}

void algo::cstring_Print(algo::cstring &row, algo::cstring &str) {
    ch_Addary(str, row);
}

void algo::strptr_Print(const algo::strptr &row, algo::cstring &str) {
    ch_Addary(str, row);
}

// Append a space unless the string already ends with a space
void  algo::MaybeSpace(cstring &str) {
    if (ch_N(str) && !algo_lib::WhiteCharQ(ch_qLast(str))) {
        str << ' ';
    }
}

// print value*100
// Example:
// double_PrintPercent(0.334, str, 1) -> "33.4%"
void algo::double_PrintPercent(double value, algo::cstring &str, int prec) {
    double_PrintPrec(value*100.0, str, prec, true, false);
}

void algo::i32_Range_Print(algo::i32_Range &r, algo::cstring &o) {
    o<<r.beg<<' '<<r.end;
}

void double_Print(double d, algo::cstring &str) {
    char c[128];
    ch_Addary(str, strptr(c,sprintf(c,"%g",d)));
}

void float_Print(float d, algo::cstring &str) {
    double_Print(d, str);
}

void algo::double_PrintWithCommas(double value, algo::cstring &str, int prec) {
    double_PrintPrec(value, str, prec, true /*omit zeros*/, true /*commas*/);
}

// ignore:bigret
algo::cstring algo::double_PrintWithCommas(double value, int prec) {
    tempstr str;
    double_PrintWithCommas(value, str, prec);
    return str;
}

// -----------------------------------------------------------------------------

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
u32 algo::u64_FmtBufDec(u64 num, int scale, u8 *buf, bool fixed) {
    char temp_elems[32];// output in reverse order
    int temp_n        = 0;
    int skip_zeros    = !fixed;//
    int first_nonzero = 0;
    do {
        int k = int(num%10);//the digit
        num     = num/10;
        if (k==0) {
            first_nonzero += skip_zeros;// index of character where
        }
        skip_zeros &= k==0; // count # of leading zeros
        temp_elems[temp_n++]=char('0'+k);
        if (temp_n==scale) {
            temp_elems[temp_n++] = '.';
            skip_zeros=false;
        }
    } while (num > 0);
    if (temp_n < scale) {
        while (temp_n<scale) {
            temp_elems[temp_n++]='0';
            first_nonzero += skip_zeros;
        }
        temp_elems[temp_n++] = '.';
        temp_elems[temp_n++] = '0';
    } else if (temp_n==scale+1) {
        temp_elems[temp_n++] = '0';
    }
    int num_chars = temp_n - first_nonzero;
    u8 *start = buf;
    while (temp_n > first_nonzero) {// transfer over & skip trailing zeros
        *start++ = temp_elems[--temp_n];
    }
    num_chars -= buf[num_chars-1]=='.';// remove trailing .
    return num_chars;
}

// -----------------------------------------------------------------------------

u32 algo::u64_FmtBufDec(u64 num, int scale, u8 *buf) {
    return algo::u64_FmtBufDec(num, scale, buf, false);
}

// -----------------------------------------------------------------------------

static void i32_PrintNanosec(int val, algo::cstring &str, int wid, int dflt) {
    i64_PrintPadLeft(val, str, wid ? wid : dflt);
    // trim trailing 0s if width was default
    if (wid==0) {
        while (ch_N(str) && ch_qLast(str)=='0') {
            str.ch_n--;
        }
    }
}

// -----------------------------------------------------------------------------

// print TIME to STR, according to spec SPEC
// The following control characters are supported in SPEC.
// %Y     Year printed as 4 digits, e.g. 2002
// %y     Last 2 digits of year printed as 2 digits
// %B     Long month name (February)
// %b     Short month name (Feb)
// %a     Week day name (Monday)
// %m     Month printed as 2 digits, with January being 01
// %d     Day of month printed as at least 2 digits (or WIDTH)
// %.X    where X is a number -- set width
// %I     Hour using 12-hour format
// %H     Hour using 24-hour format, printed as 2 digits
// %M     Minute, printed as 2 digits
// %S     Second, printed as 2 digits
// %T     Shortcut for %H:%M:%S.%.9X
// %X     Print fractions of a second according to width
// %x     Print milliseconds with 3 decimal places
// %-     Print - sign if timespec is negative
// %%     Print % sign
void algo::TimeStruct_Print(const TimeStruct &time, algo::cstring &str, const algo::strptr &spec) {
    for(int i =0; i<elems_N(spec);) {
        int selector = spec[i++];
        if (selector != '%') {
            str<<char(selector);
            continue;
        }
        int width = 0;
        if (i+1 < elems_N(spec)-1 && spec[i]=='.' && algo_lib::DigitCharQ(spec[i+1])) {
            width = spec[i+1] - '0';
            i+=2;
        }
        if (i == elems_N(spec)) break;
        selector = spec[i++];
        switch(selector) {
        case 'Y':
            i64_PrintPadLeft(time.tm_year+1900, str, 4);
            break;
        case 'y':
            i64_PrintPadLeft(time.tm_year+1900>=2000 ? time.tm_year-100 : time.tm_year, str, 2);
            break;
        case 'B':
            str<<GetMonthNameZeroBased(Clipped(time.tm_mon,12));
            break;
        case 'b':
            str<<GetRegion(GetMonthNameZeroBasedShort(Clipped(time.tm_mon,12)), 0,3);
            break;
        case 'a':
            str<<GetWeekdayName(Clipped(time.tm_wday,7));
            break;
        case 'm':
            i64_PrintPadLeft(int(time.tm_mon)+1, str, i32_Max(width,2));
            break;
        case 'd':
            i64_PrintPadLeft(int(time.tm_mday), str, i32_Max(width,2));
            break;
        case 'I':
            i64_PrintPadLeft(time.tm_hour > 12 ? time.tm_hour - 12 : time.tm_hour, str, 2);
            break;
        case 'H':
            i64_PrintPadLeft(time.tm_hour, str, 2);
            break;
        case 'M':
            i64_PrintPadLeft(time.tm_min, str, 2);
            break;
        case 'S':
            i64_PrintPadLeft(time.tm_sec, str, 2);
            break;
        case 'T':
            i64_PrintPadLeft(time.tm_hour, str, 2);
            str<<":";
            i64_PrintPadLeft(time.tm_min, str, 2);
            str<<":";
            i64_PrintPadLeft(time.tm_sec, str, 2);
            if (time.tm_nsec) {
                str << '.';
                i32_PrintNanosec(time.tm_nsec, str, width, 9);
            }
            break;
        case 'X':
            {
                int div = 1;
                switch(9-width) {
                case 0: div= 1; break;
                case 1: div= 10; break;
                case 2: div= 100; break;
                case 3: div= 1000; break;
                case 4: div= 10000; break;
                case 5: div= 100000; break;
                case 6: div= 1000000; break;
                case 7: div= 10000000; break;
                case 8: div= 100000000; break;
                case 9: div= 1; break;// default?????
                default: div= 1; break;
                }
                i32_PrintNanosec(time.tm_nsec / div, str, width, 9);
            }
            break;
        case 'x':
            i32_PrintNanosec(time.tm_nsec / 1000000, str, width, 3);
            break;
        case '-':
            if (time.tm_neg) str << '-';
            break;
        case '%':
            str<<'%';
            break;
        default:
            str<<char(selector);
            break;
        }
    }
}


void algo::Tuple_Print(algo::Tuple &row_, algo::cstring &str) {
    int start = -1;
    int spaces = 2;
    Tuple &row = (Tuple&)row_;
    int nsp=0;
    if (start==-1 && (ch_N(row.head.name) || ch_N(row.head.value))) {
        PrintAttr(str, row.head.name, row.head.value);
        nsp=spaces;
    }
    for (int i = i32_Max(start,0); i < attrs_N(row); i++) {
        Attr &attr = attrs_qFind(row, i);
        char_PrintNTimes(' ', str, nsp);
        nsp=spaces;
        PrintAttr(str, attr.name, attr.value);
    }
}

void char_Print(char c, algo::cstring &str) {
    ch_Alloc(str) = c;
}

void bool_Print(bool row, algo::cstring &str) {
    ch_Alloc(str) = row ? 'Y' : 'N';
}

void algo::Bool_Print(algo::Bool row, algo::cstring &str) {
    bool_Print(row, str);
}

void algo::UnDiff_Print(UnDiff row, algo::cstring &str) {
    algo::UnDiff_PrintSpec(row, str, "%-%T"); // optional minus sign, then timestamp with nanoseconds
}

void algo::UnixDiff_Print(UnixDiff row, algo::cstring &str) {
    algo::UnixDiff_PrintSpec(row, str, "%-%H:%M:%S");
}

void algo::UnTime_Print(algo::UnTime row, algo::cstring &str) {
    algo::UnTime_PrintSpec(row, str, "%Y-%m-%dT%T"); // ISO 8601
}

void algo::UnixTime_Print(algo::UnixTime row, algo::cstring &str) {
    algo::UnixTime_PrintSpec(row, str, "%Y/%m/%d %T");
}

void algo::UnTime_PrintSpec(UnTime t, algo::cstring &out, const algo::strptr &spec) {
    TimeStruct time_struct = GetLocalTimeStruct(t);
    TimeStruct_Print(time_struct, out, spec);
}


void algo::UnixTime_PrintSpec(UnixTime t, algo::cstring &out, const algo::strptr &spec) {
    TimeStruct time_struct = GetLocalTimeStruct(t);
    TimeStruct_Print(time_struct, out, spec);
}

void algo::UnDiff_PrintSpec(UnDiff   t, algo::cstring &out, const algo::strptr &spec) {
    TimeStruct time_struct = ToTimeStruct(t);
    TimeStruct_Print(time_struct, out, spec);
}

void algo::UnixDiff_PrintSpec(UnixDiff t, algo::cstring &out, const algo::strptr &spec) {
    TimeStruct time_struct = ToTimeStruct(t);
    TimeStruct_Print(time_struct, out, spec);
}

void ietf::Ipv4_Print(ietf::Ipv4 row, algo::cstring &str) {
    str<< ((row.ipv4 >> 24) & 0xff);
    str<< ".";
    str<< ((row.ipv4 >> 16) & 0xff);
    str<< ".";
    str<< ((row.ipv4 >> 8)  & 0xff);
    str<< ".";
    str<< (row.ipv4         & 0xff);
}

void ietf::Ipv4Addr_Print(ietf::Ipv4Addr row, algo::cstring &str) {
    ietf::Ipv4_Print(ietf::Ipv4(addr_Get(row)), str);
}

void algo::Ipmask_Print(algo::Ipmask &row, algo::cstring &str) {
    int bit = row.mask ? u32_BitScanForward(u32(row.mask)) : 32;
    Ipv4_Print(ietf::Ipv4(row.ip_host), str);
    str << "/";
    str << 32-bit;
}

void algo::Errcode_Print(algo::Errcode &row, algo::cstring &str) {
    int code = code_Get(row);
    int type = type_Get(row);
    if (code != 0) {
        switch(type) {
        case algo_Errns_unix: {
            char buf[512];
            str << strerror_r(code,buf,sizeof(buf));
            break;
        }
        case algo_Errns_win: {
            str<<"Windows Error "<<code;
            break;
        }
        default:
            break;
        }
    }
}

void algo_lib::ErrorX_Print(algo_lib::ErrorX &row, algo::cstring &str) {
    try {
        algo::ListSep sep(": ");
        if (ch_N(row.str)) {
            str << sep << row.str;
        }
    } catch (algo_lib::ErrorX &) {
        // appeasing coverity
    }
}

void ietf::Ipport_Print(ietf::Ipport row, algo::cstring &str) {
    Ipv4_Print(row.ip, str);
    str<<":"<<row.port;
}

bool ietf::Ipport_ReadStrptrMaybe(ietf::Ipport &row, strptr in) {
    bool retval = true;
    strptr left=Pathcomp(in,":LL");
    strptr right=Pathcomp(in,":LR");
    retval = retval && Ipv4_ReadStrptrMaybe(row.ip, left);
    retval = retval && u16_ReadStrptrMaybe(row.port, right);
    return retval;
}

// -----------------------------------------------------------------------------

// Append STR to OUT, using comma-separated-values encoding
// If QUOTE is 0, the need for quotes and the type of quote is determined automatically.
// If it is any other value, then that type of quote is used
// According to the CSV standard, the quote itself is the escape character.
void algo::strptr_PrintCsv(algo::strptr str, algo::cstring &out, char quote) {
    if (quote == 0) {
        bool has_comma=false;
        bool has_quote=false;
        ind_beg_aryptr(char,c,str) {
            if (c == ',') {
                has_comma=true;
            } else if (c == '\'' || c == '"' || algo_lib::WhiteCharQ(c)) {
                has_quote=true;
            }
        }ind_end_aryptr;
        if (has_comma || has_quote) {
            quote = '"';
        }
    }
    if (quote) {
        out<<quote;
    }
    ind_beg_aryptr(char,c,str) {
        if (c==quote) {
            out << c;// repeat it
        }
        out << c;
    }ind_end_aryptr;
    if (quote) {
        out<<quote;
    }
}

// -----------------------------------------------------------------------------

// Print CSV field, auto-determine quotes
void algo::strptr_PrintCsv(algo::strptr str, algo::cstring &out) {
    strptr_PrintCsv(str,out,0);
}

// -----------------------------------------------------------------------------

static void PrintRow(cstring &str, algo_lib::FTxtrow &row, bool use_style) {
    ind_beg(algo_lib::txtrow_c_txtcell_curs, txtcell, row) {
        int extra = txtcell.width - ch_N(txtcell.rsep) - ch_N(txtcell.text);
        int l = txtcell.justify>0 ? extra
            : txtcell.justify<0 ? 0
            : extra/2;
        char_PrintNTimes(' ', str,  l);
        algo::TermStyle style = use_style ? txtcell.style : algo::TermStyle(algo_TermStyle_default);
        if (style != algo_TermStyle_default) {
            if (style & algo_TermStyle_bold) {
                // do it
            }
            if (style & algo_TermStyle_red) {
                str << "\033[91m";
            }
            if (style & algo_TermStyle_green) {
                str << "\033[92m";
            }
            if (style & algo_TermStyle_blue) {
                str << "\033[94m";
            }
        }
        str << txtcell.text;
        if (style) {
            strptr_Print("\033[0m", str);
        }
        char_PrintNTimes(' ', str,  extra-l);
        str << txtcell.rsep;
    }ind_end;
    // trim whitespace on the right
    while (ch_N(str) && ch_qLast(str) == ' ') {
        str.ch_n--;
    }
    str << eol;
}

static void Normalize(algo::NormTxttbl &norm, algo_lib::FTxttbl &txttbl) {
    start_Alloc(norm) = 0;
    int maxspan = 0;
    ind_beg(algo_lib::txttbl_c_txtrow_curs,txtrow,txttbl) if (txtrow.select) {
        ind_beg(algo_lib::txtrow_c_txtcell_curs, txtcell, txtrow) {
            maxspan = i32_Max(maxspan, txtcell.span);
        }ind_end;
    }ind_end;
    frep_(span,maxspan+1) {
        ind_beg(algo_lib::txttbl_c_txtrow_curs,txtrow,txttbl) if (txtrow.select) {
            int i = 0;
            ind_beg(algo_lib::txtrow_c_txtcell_curs, txtcell, txtrow) {
                if (txtcell.span == span) {
                    int w = i32_Max(ch_N(txtcell.text) + ch_N(txtcell.rsep), txtcell.width);
                    // apply width
                    int r = i + txtcell.span;
                    while (r >= start_N(norm)) {
                        int last = start_qLast(norm);
                        start_Alloc(norm) = last;
                    }
                    int extra = w - (start_qFind(norm, r) - start_qFind(norm, i));
                    if (extra > 0) {
                        for (int j = r; j < start_N(norm); j++) {
                            start_qFind(norm, j) += extra;
                        }
                    }
                }
                i += txtcell.span;
            }ind_end;
        }ind_end;
    }
    ind_beg(algo_lib::txttbl_c_txtrow_curs,txtrow,txttbl) if (txtrow.select) {
        int i = 0;
        ind_beg(algo_lib::txtrow_c_txtcell_curs, txtcell, txtrow) {
            txtcell.width = start_qFind(norm, i+txtcell.span) - start_qFind(norm, i);
            i += txtcell.span;
        }ind_end;;
    }ind_end;;
}

void algo_lib::FTxttbl_Print(algo_lib::FTxttbl &T_, algo::cstring &str) {
    algo_lib::FTxttbl            &txttbl         = const_cast<algo_lib::FTxttbl&>(T_);
    bool              use_style = algo::SaneTerminalQ();
    if (!txttbl.normalized) {
        algo::NormTxttbl norm;
        txttbl.normalized = true;
        Normalize(norm, txttbl);
    }
    ind_beg(algo_lib::txttbl_c_txtrow_curs,txtrow,txttbl) if (txtrow.select) {
        PrintRow(str,txtrow,use_style);
    }ind_end;
}

void algo::URL_Print(algo::URL &url, algo::cstring &str) {
    if (ch_N(url.protocol)) {
        str<<url.protocol<<"://";
    }
    if (ch_N(url.username)) {
        str<<url.username;
    }
    if (ch_N(url.password)) {
        str<<":"<<url.password;
    }
    if (ch_N(url.username) || ch_N(url.password)) {
        str<<"@";
    }
    if (url.source_addr_host!=0) {
        str<<"(";
        str << ietf::Ipv4(url.source_addr_host);
        str<<","<<url.server;
        if (url.port!=-1) {
            str<<":"<<url.port;
        }
        str<<")";
    } else {
        str<<url.server;
        if (url.port!=-1) {
            str<<":"<<url.port;
        }
    }
    if (ch_N(url.host)) {
        str<<"<-"<<url.host;
    }
    str<<url.dir;
}

// Append STR to OUT, and pad remainder with character FILL
void algo::strptr_PrintPadRight(algo::strptr str, algo::cstring &out, int nplaces, char fill) {
    int pos  = ch_N(out);
    ch_Addary(out, str);
    int npad = nplaces-(ch_N(out)-pos);
    char_PrintNTimes(fill, out,  npad);
}

// -----------------------------------------------------------------------------

// Append STR to OUT, and pad remainder with space characters
void algo::strptr_PrintPadRight(algo::strptr str, algo::cstring &out, int nplaces) {
    strptr_PrintPadRight(str,out,nplaces,' ');
}

// -----------------------------------------------------------------------------

// Append STR to OUT, padding start with character FILL
void algo::strptr_PrintPadLeft(algo::strptr str, algo::cstring &out, int nplaces, char fill) {
    int npad = nplaces-ch_N(str);
    char_PrintNTimes(fill, out,  npad);
    ch_Addary(out, str);
}

// -----------------------------------------------------------------------------

// Append STR to OUT, padding start with spaces.
void algo::strptr_PrintPadLeft(algo::strptr str, algo::cstring &out, int nplaces) {
    strptr_PrintPadLeft(str,out,nplaces,' ');
}

// -----------------------------------------------------------------------------

// Append STR to OUT as an SQL string using quote character Q.
// Escaped characters:
// '\' (backslash)
// ' quoted with '' if q is ' and not quoted otherwise. (Compatible with MariaDB & T-SQL.)
// " quoted with "" if q is " and not quoted otherwise. (Compatible with MariaDB & T-SQL.)
// NUL (ascii zero)
// \n
// \r
// \t
void algo::strptr_PrintSql(algo::strptr str, algo::cstring &out, char q) {
    ch_Reserve(out, elems_N(str) * 2 + 2);
    char *elems = out.ch_elems;
    int j = out.ch_n;
    elems[j] = q;
    j += 1;
    frep_(i,elems_N(str)) {
        char c = str[i];
        switch(c) {
        case 0:
            elems[j++] = '\\';
            elems[j++] = '0';
            break;
        case '\\':
            elems[j++] = '\\';
            elems[j++] = c;
            break;
        case '\n':
            elems[j++] = '\\';
            elems[j++] = 'n';
            break;
        case '\r':
            elems[j++] = '\\';
            elems[j++] = 'r';
            break;
        case '\t':
            elems[j++] = '\\';
            elems[j++] = 't';
            break;
        case '\'':
            if ( q == c  )  {
                elems[j++] = c;
                elems[j++] = c;
            } else {
                elems[j++] = c;
            }
            break;
        case '\"':
            if ( q == c  )  {
                elems[j++] = c;
                elems[j++] = c;
            } else {
                elems[j++] = c;
            }
            break;
        default:
            elems[j++] = c;
        }
    }
    elems[j++] = q;
    out.ch_n = j;
}

// -----------------------------------------------------------------------------

void algo::strptr_PrintSql(algo::strptr str, algo::cstring &out) {
    strptr_PrintSql(str,out,'\'');
}

// -----------------------------------------------------------------------------

void algo::Attr_Print(algo::Attr &attr, algo::cstring &str) {
    PrintAttr(str, attr.name, attr.value);
}

// Print a string suitable for parsing with Tuple
// If the string doesn't need quotes, it is printed as-is.
// If quotes are needed, first determine whether to use single or double quotes to minimize
// escaping. Then print the string using char_PrintCppEsc for quoting.
// Empty string is printed as ""
void algo::PrintAttrSpace(cstring &str, algo::strptr name, const strptr &value) {
    str << "  ";
    PrintAttr(str,name,value);
}

void algo::ListSep_Print(const algo::ListSep &t, algo::cstring &str) {
    if (t.iter) {
        str << t.sep;
    }
    t.iter++;
}

// -----------------------------------------------------------------------------

// copy string B to TO, using ORIG as a case template
// I.e. PrintCopyCase("AbcD", to, "somestring") -> "SomEstring"
void algo::strptr_PrintCopyCase(const algo::strptr &orig, algo::cstring &to, const algo::strptr &b) {
    if (elems_N(b)) {
        int nupper=0, nlower=0, nletters=0;
        int lim = i32_Min(elems_N(b),elems_N(orig));
        ch_Reserve(to, elems_N(b));
        frep_(i,lim) {
            if (algo_lib::UpperCharQ(orig[i])) {
                nletters++;
                nupper++;
            } else if (algo_lib::LowerCharQ(orig[i])) {
                nletters++;
                nlower++;
            }
        }
        if (nupper==nletters) {
            frep_(i,elems_N(b)) {
                ch_Alloc(to) = ToUpper(b[i]);
            }
        } else if (nlower==nletters) {
            frep_(i,elems_N(b)) {
                ch_Alloc(to) = ToLower(b[i]);
            }
        } else {
            int l = i32_Min(elems_N(orig),elems_N(b));
            frep_(i,l) {
                ch_Alloc(to) = algo_lib::UpperCharQ(orig[i]) ? ToUpper(b[i]) : ToLower(b[i]);
            }
            to << RestFrom(b,l);
        }
    }
}

bool algo::Tuple_ReadStrptrMaybe(Tuple &row, algo::strptr s) {
    bool retval = algo::Tuple_ReadStrptr(row, s, false);
    return retval;
}

// T             target tuple. the tuple is not emptied before parsing.
// STR           source string
// ATTRONLY        if set, all loaded attrs are appended to the ATTRS
//                 array. otherwise, the first attr becomes HEAD.
// CMT_CHAR      character at which to stop parsing.
// Parse sequence of attrs (name-value pairs) into tuple T.
// Roughly:
// ATTR       -> VALUE | VALUE ':' VALUE
// VALUE      -> IDENTIFIER | C++-STRING
// Return value: FALSE if unterminated string or other badness found in data.
bool algo::Tuple_ReadStrptr(algo::Tuple &tuple, strptr str, bool attronly) {
    bool ret = true;
    ch_RemoveAll(tuple.head.name);
    ch_RemoveAll(tuple.head.value);
    attrs_RemoveAll(tuple);
    algo::StringIter iter(str);
    bool need_head = !attronly;
    while (!iter.Ws().EofQ()) {
        Attr arg;
        int start = iter.index;

        bool ok = false;
        if (iter.Ws().Peek()!='#') {
            cstring_ReadCmdarg(arg.value, iter, false);
            if (SkipChar(iter, ':')) {
                arg.name=arg.value;
                ret &= cstring_ReadCmdarg(arg.value, iter, true);
            }
            ok = iter.index > start;
        }
        if (!ok) {
            break;
        }
        Attr &tgt = need_head ? tuple.head : attrs_Alloc(tuple);
        TSwap(arg,tgt);
        need_head = false;
    }
    return ret;
}

// Implementations of built-in read functions for AMC.
// basis function for reading tuples.
// scan ITER for identifier, or quoted string.
// return FALSE if attribute is malformed (i.e. unterminated string)
bool algo::cstring_ReadCmdarg(cstring &out, algo::StringIter &S, bool is_value) {
    bool ret = false;
    char quote = S.Peek();
    if (quote == '"' || quote == '\'') {
        ch_RemoveAll(out);
        S.index++;
        while (!S.EofQ() && !ret) {
            char ch = S.GetChar();
            if (ch == quote) {
                ret = true;
            } else if (algo_lib::NewLineCharQ(ch)) {
                break;
            } else if (ch == '\\') {
                int len = elems_N(S.expr)-S.index;
                // note: may read past the end of the string
                u32 c = ReadLE32(S.expr.elems+S.index);
                u8 result;
                u32 advance = UnescapeC(c, len, result);
                S.index += advance;
                if (!advance) {
                    break;
                }
                ch_Alloc(out) = result;
            } else {
                ch_Alloc(out) = ch;
            }
        }
        if (!ret) {
            S.index=elems_N(S.expr);// go to end-of-file
        }
    } else {
        Charset &breaks=is_value ? algo_lib::_db.SsimBreakValue : algo_lib::_db.SsimBreakName;
        int from=S.index;
        while (S.index<elems_N(S.expr) && !ch_qGetBit(breaks, (unsigned char)S.expr[S.index])){
            S.index++;
        }
        strptr result = qGetRegion(S.expr,from,S.index-from);
        out = result;
        ret = true;
    }
    return ret;
}

// Read Charset from list of chars.
// Every character in RHS is simply added to the bitset
void algo::Charset_ReadStrptrPlain(algo::Charset &lhs, strptr desc) {
    rep_(i,elems_N(desc)) {
        ch_qSetBit(lhs, desc[i]);
    }
}

bool algo::Charset_ReadStrptrMaybe(algo::Charset &lhs, strptr rhs) {
    bool retval = true;
    bool escape = false;
    for (int i=0; i<elems_N(rhs); i++) {
        if (escape) {
            escape = false;
            ch_qSetBit(lhs, rhs[i]);
        } else if (rhs[i] == '\\') {
            escape=true;
        } else if (i<elems_N(rhs)-2 && rhs[i+1]=='-') {
            int from = rhs[i];
            int to = rhs[i+2];
            if (to>=from) {
                for (int k=from; k<=to; k++) {
                    ch_qSetBit(lhs, k);
                }
            } else {
                retval = false;
                algo_lib::SaveBadTag("comment","Invalid char range");
            }
            i+=2;
        } else {
            ch_qSetBit(lhs, rhs[i]);
        }
    }
    return retval;
}

// -----------------------------------------------------------------------------

// Print STR to OUT in a way that's acceptable as input for bash.
void algo::strptr_PrintBash(strptr str, algo::cstring &out) {
    int needquotes = elems_N(str) == 0; // need quotes if string is empty
    bool needdollar = false;
    frep_(i,elems_N(str)) {
        char c = str[i];
        bool quotesafe = algo_lib::BashQuotesafeQ(unsigned(c));
        needquotes += !quotesafe;
        needdollar |= !quotesafe && c != ' ';
    }
    if (needquotes) {
        if (needdollar) {
            ch_Reserve(out, 1);
            out.ch_elems[out.ch_n++] = '$';
        }
        algo::strptr_PrintCppQuoted(str, out, '\'');
    } else {
        out<<str;
    }
}

// -----------------------------------------------------------------------------

// Escape S according to bash rules and return result
tempstr algo::strptr_ToBash(strptr str) {
    tempstr out;
    strptr_PrintBash(str,out);
    return out;
}

// -----------------------------------------------------------------------------

// encode uri component.
// before ? use plus=false (no + substitution by space)
void algo::strptr_PrintUri(strptr str, algo::cstring &out, bool plus) {
    int i=0;
    for(;i<elems_N(str);++i) {
        if (algo_lib::UrlsafeQ(str[i])) {
            out<<str[i];
        } else if ((plus && str[i]==' ')|| (!plus && str[i]=='+')) {
            out<<'+';
        } else {
            out << "%";
            u64_PrintHex(str[i]&0xff,out, 2,false,true);
        }
    }
}

// -----------------------------------------------------------------------------

// Print string, escaping characters in a way suitable for graphviz dot format
void algo::strptr_PrintDot(strptr s, algo::cstring &out) {
    tempstr t(s);
    Translate(t,"<>:\"'","_____");
    out <<"\"";
    out << t;
    out <<"\"";
}

// -----------------------------------------------------------------------------

// print binary octet string as hex
void algo::Sha1sig_Print(algo::Sha1sig &sha1sig, algo::cstring &out) {
    ind_beg(algo::Sha1sig_sha1sig_curs,_byte,const_cast<algo::Sha1sig&>(sha1sig)) {
        u64_PrintHex(_byte,out,2,false/*prefix*/,false/*caps*/);
    }ind_end;
}

// -----------------------------------------------------------------------------

bool algo::Sha1sig_ReadStrptrMaybe(algo::Sha1sig &sha1sig, algo::strptr str) {
    int len = i32_Min(elems_N(str)/2, 20);
    bool retval = len == 20;
    algo::Sha1sig temp;
    frep_(i,len) {
        retval = retval && ParseHex2(str[i*2+1] << 8 | str[i*2], 2, sha1sig_qFind(temp, i));
    }
    if (retval) {    // if signature cannot be read correctly, zero it out.
        sha1sig=temp;
    } else {
        retval = false;
        algo_lib::SaveBadTag("comment", "invalid sha1sig format");
    }
    return retval;
}

// -----------------------------------------------------------------------------

// Write character C into buffer BUF, using C++ character escapement rules
// BUF must have room for at least 4 extra characters
// Return number of characters written
// QUOTE_CHAR also gets escaped.
static inline int _PrintQuotedChar(char c, char *buf, char quote_char) {
    int n=0;
    if (UNLIKELY((c==quote_char) | (c=='\\'))) {
        buf[0]='\\';
        buf[1]=c;
        n=2;
    } else if (UNLIKELY((unsigned(c) < 32) | (unsigned(c) >= 127))) {
        if (c=='\n') {// 10
            buf[0]='\\';
            buf[1]='n';
            n=2;
        } else if (c=='\t') {// 9
            buf[0]='\\';
            buf[1]='t';
            n=2;
        } else if (c=='\r') {// 13
            buf[0]='\\';
            buf[1]='r';
            n=2;
        } else {
            unsigned x = c&0xff;
            buf[0]='\\';
            buf[1]='0' + ((x>>6) & 7);
            buf[2]='0' + ((x>>3) & 7);
            buf[3]='0' + ((x>>0) & 7);
            n=4;
        }
    } else {
        buf[0]=c;
        n=1;
    }
    return n;
}

// -----------------------------------------------------------------------------

// Write character C into string OUT, using C++ character escapement rules
// QUOTE_CHAR also gets escaped.
void algo::char_PrintCppEsc(char c, algo::cstring &out, char quote_char) {
    ch_Reserve(out, 4);
    out.ch_n += _PrintQuotedChar(c, out.ch_elems + out.ch_n, quote_char);
}

// -----------------------------------------------------------------------------

// Print STR, surrounded by quotes as C++ string
// surrounded by QUOTE_CHAR quotes, to buffer OUT.
// All string characters are escaped using char_PrintCppEsc.
void algo::strptr_PrintCppQuoted(algo::strptr str, algo::cstring &out, char quote_char) {
    ch_Reserve(out, 2+str.n_elems*4);// max possible
    char *srcbuf=str.elems;// source string
    char *buf = out.ch_elems;// destination buffer
    int src_n=str.n_elems;// source count of chars
    int n=out.ch_n;// destination count of chars
    buf[n++] = quote_char;// opening quote
    for (int i=0; i<src_n; i++) {
        n += _PrintQuotedChar(srcbuf[i],buf+n,quote_char);
    }
    buf[n++] = quote_char;// closing quote
    out.ch_n = n;
}

// -----------------------------------------------------------------------------

// Print STR as a C++ string to OUT.
void algo::strptr_PrintCpp(algo::strptr str, algo::cstring &out) {
    algo::strptr_PrintCppQuoted(str,out,'"');
}

// -----------------------------------------------------------------------------

// Print STR as a valid SSIM key/value to OUT.
// SSIM tokens use quotes only when the printed value contains
// characters outside of the 'safe' set a-zA-Z0-9_;&*^%$@.!:,+/-
void algo::strptr_PrintSsim(algo::strptr str, algo::cstring &out) {
    char quote_char = algo::PickSsimQuoteChar(str);
    if (quote_char) {
        strptr_PrintCppQuoted(str, out, quote_char);
    } else {
        out<<str;
    }
}

// -----------------------------------------------------------------------------

// Begin append key/value to string LHS
int algo::BeginKeyval(cstring &lhs, const strptr &name) {
    // insert double space if necessary
    if (ch_N(lhs) && ch_qFind(lhs,ch_N(lhs)-1) != ' ') {
        ch_Addary(lhs, strptr("  ",2));
    }
    // print name unless it's empty.
    if (elems_N(name) > 0) {
        algo::strptr_PrintSsim(name,lhs);
        ch_Alloc(lhs)=':';
    }
    return ch_N(lhs); // return index for start of value;
}

// -----------------------------------------------------------------------------

// End append key/value to string LHS
cstring &algo::EndKeyval(cstring &lhs, int start) {
    strptr val(lhs.ch_elems + start, lhs.ch_n - start);
    // rewrite tail of the string if it needs quoting.
    // empty strings need quoting
    if (char quote_char = algo::PickSsimQuoteChar(val)) {
        tempstr temp(val);
        lhs.ch_n = start;// trim at the start
        strptr_PrintCppQuoted(temp, lhs, quote_char);
    }
    return lhs;
}

// -----------------------------------------------------------------------------

// Append key-value pair NAME:VALUE to string STR
void algo::PrintAttr(cstring &str, const algo::strptr &name, const strptr &value) {
    if (elems_N(name)) {
        algo::strptr_PrintSsim(name,str);
        ch_Alloc(str)=':';
    }
    algo::strptr_PrintSsim(value,str);
}

// Append key-value pair NAME:VALUE to string STR, and remove all
// characters from VALUE (used by amc)
void algo::PrintAttrSpaceReset(cstring &str, const algo::strptr &name, algo::cstring &value) {
    ch_Addary(str, strptr("  ",2));
    PrintAttr(str,name,value);
    ch_RemoveAll(value);
}

// Append key-value pair NAME:VALUE to string STR,
// and remove all characters from VALUE (used by amc)
void algo::PrintAttrSpaceReset(cstring &str, const char *name, algo::cstring &value) {
    PrintAttrSpaceReset(str, strptr(name), value);
}

// -----------------------------------------------------------------------------

// Convert VALUE to a string, encoded as a u64.
// Value must be >=0 and <100000000.
// Resulting u64 is little-endian -- MSB contains first byte
// of the resulting string. To be used as an ascii string, value
// 0x3030303030303030 (8 ascii '0's) must be added to the result.
// Don't use this function. Use u32_FmtBufSmall, and u64_FmtBuf.
u64 algo::u32_CvtLestr8Small(u32 value) {
    u32 l = value % 10000;
    u32 h = value / 10000;

    u32 ll = l % 100;
    u32 lh = l / 100;
    u32 hl = h % 100;
    u32 hh = h / 100;

    u32 lll = ll % 10;
    u32 llh = ll / 10;
    u32 lhl = lh % 10;
    u32 lhh = lh / 10;
    u32 hll = hl % 10;
    u32 hlh = hl / 10;
    u32 hhl = hh % 10;
    u32 hhh = hh / 10;

    return
        ( (u64)lll << 56
          | (u64)llh << 48
          | (u64)lhl << 40
          | (u64)lhh << 32
          | (u64)hll << 24
          | (u64)hlh << 16
          | (u64)hhl << 8
          | (u64)hhh << 0);
}

// -----------------------------------------------------------------------------

// Format VALUE, which should be >=0 and <100000000,
// as a string and write it to buffer BUF. Return length of resulting string.
// Buffer must be big enough.
u32 algo::u32_FmtBufSmall(u32 value, u8 *buf) {
    u64 result = u32_CvtLestr8Small(value);
    u64 zeros  = u64_BitScanForward(result);
    zeros      = value == 0 ? 56 : zeros;
    u64 len    = 8 - zeros / 8;
    *(u64*)buf = (result + 0x3030303030303030) >> zeros/8*8;
    return u32(len);
}

// -----------------------------------------------------------------------------

// Format VALUE, which is an arbitrary u64,
// as a string and write it to buffer BUF.
// Return length of resulting string.
// Buffer must be big enough.
u32 algo::u64_FmtBuf(u64 value, u8 *buf) {
    static const u32 thresh = 100000000;
    u32 len=0;
    if (value < thresh) {
        return u32_FmtBufSmall((u32)value, buf);
    }
    u64 r1 = value % thresh;
    value  = value / thresh;
    if (value < thresh) {
        len              = u32_FmtBufSmall((u32)value,buf);
        *(u64*)(buf+len) = u32_CvtLestr8Small((u32)r1) + 0x3030303030303030;
        return len+8;
    }
    u64 r2             = value % thresh;
    u64 r3             = value / thresh;
    len                = u32_FmtBufSmall((u32)r3,buf);
    *(u64*)(buf+len)   = u32_CvtLestr8Small((u32)r2) + 0x3030303030303030;
    *(u64*)(buf+len+8) = u32_CvtLestr8Small((u32)r1) + 0x3030303030303030;
    return len+8+8;
}

// -----------------------------------------------------------------------------

// Format VALUE, which is an arbitrary u32,
// as a string and write it to buffer BUF.
// Return length of resulting string.
// Buffer must be big enough.
u32 algo::u32_FmtBuf(u32 value, u8 *buf) {
    static const u32 thresh = 100000000;
    u32 len;
    if (value < thresh) {
        return u32_FmtBufSmall(value, buf);
    }
    u32 r1           = value % thresh;
    value            = value / thresh;
    len              = u32_FmtBufSmall(value,buf);
    *(u64*)(buf+len) = u32_CvtLestr8Small(r1) + 0x3030303030303030;
    return len+8;
}

// -----------------------------------------------------------------------------

u64 algo::aParseNum16(strptr s, u32 &ok) {
    if (s.n_elems <= 8) {
        return ParseNum8(ReadBE64(s.elems), s.n_elems, ok);
    }
    u32 ok1, ok2;
    u32 len2 = s.n_elems - 8;
    u64 a    = ParseNum8(ReadBE64(s.elems      ), len2, ok1);
    u64 b    = ParseNum8(ReadBE64(s.elems + len2),      ok2);
    ok       = ok1 & ok2 & (s.n_elems <= 16);
    return a * 100000000 + b;
}

// -----------------------------------------------------------------------------

// STR         little-endian string up to 2 chars in length
// LEN         length of STR
// RESULT      value of hex number encoded in STR
// RETURN      number of characters to skip (0 - unsuccessful)
int algo::ParseHex2(u32 str, int len, u8 &result) {
    u8 res = 0;
    u8 c;
    int advance = 0;
    while(advance < 2 && len && ParseHex1(str&0xff,c)) {
        res = (res<<4) | c;
        str = str>>8;
        ++advance;
        --len;
    }
    result = res;
    return advance;
}

// -----------------------------------------------------------------------------

// STR        little-endian string up to 4 chars
// LEN        length of STR
// RESULT     Octal value encoded in str
// RETURN     number of characters to skip (0 - unsuccessful)
int algo::ParseOct3(u32 str, int len, u8 &result) {
    u8 res = 0;
    u8 c;
    int advance = 0;
    while(advance < 3 && len && ParseOct1(str&0xff,c)) {
        res = (res<<3) | c;
        str = str>>8;
        ++advance;
        --len;
    }
    result = res;
    return advance;
}

// -----------------------------------------------------------------------------

u32 algo::ParseNum4(u32 x, u32 len, u32 &ok) {
    u32 shift  = 32-len*8;
    u32 mask   = 0xFFFFFFFF;
    mask       = mask >> shift;
    mask       = ~mask & 0x30303030UL;
    x          = x>>shift | mask;
    u32 ok1;
    u32 result = ParseNum4(x, ok1);
    ok         = ok1 & (len-1 < 4);
    return result;
}

// -----------------------------------------------------------------------------

u32 algo::ParseNum8(u64 x, u32 len, u32 &ok) {
    u32 shift  = 64-len*8;
    u64 mask   = 0XFFFFFFFFFFFFFFFF;
    mask       = mask >> shift;
    mask       = ~mask & 0x3030303030303030ULL;
    x          = x>>shift | mask;
    u32 ok1;
    u32 result = ParseNum8(x,ok1);
    ok         = ok1 & (shift<64);
    return result;
}

// -----------------------------------------------------------------------------

// Print a string as a classic regex, escaping all special
// characters. This regex will only match the specified string.
void algo::strptr_PrintRegxSql(algo::strptr value, algo::cstring &str) {
    frep_(i, elems_N(value)) {
        if (algo_lib::RegxSqlSpecialQ(value.elems[i])) {
            str << '\\';
        }
        str << value.elems[i];
    }
}

void algo::WDiff_Print(algo::WDiff row, algo::cstring &str) {
    str << algo::ToUnDiff(row);
}

void algo::WTime_Print(algo::WTime row, algo::cstring &str) {
    str << algo::ToUnTime(row);
}

bool algo::WDiff_ReadStrptrMaybe(algo::WDiff &parent, algo::strptr in_str) {
    UnDiff d;
    bool ret = UnDiff_ReadStrptrMaybe(d,in_str);
    if (ret) {
        parent = algo::ToWDiff(d);
    }
    return ret;
}

bool algo::WTime_ReadStrptrMaybe(algo::WTime &parent, algo::strptr in_str) {
    UnTime d;
    bool ret = UnTime_ReadStrptrMaybe(d,in_str);
    if (ret) {
        parent = algo::ToWTime(d);
    }
    return ret;
}

void algo::u64_PrintBase32(u64 k, algo::cstring &str) {
    enum { MAXCHARS = 20 };
    char buf[MAXCHARS];
    int start = MAXCHARS;
    do {
        u32 idx = u32(k&31);
        buf[--start] = (idx < 10 ? char('0' + idx) : char('A' + idx-10));
        k>>=5;
    } while (k);
    ch_Addary(str, algo::strptr(buf+start, MAXCHARS-start));
}

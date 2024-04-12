// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2008-2012 AlgoEngineering LLC
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
// Header: include/algo.h -- Main header
//

#pragma once

#include "include/sysincl.h"
#include "include/typedef.h"
#include "include/u128.h"
#include "include/define.h"
#include "include/gen/_gen.h"
#include "include/gen/algo_gen.h"
#include "include/gen/algo_lib_gen.h"

// STRUCTS
namespace algo {
    struct PageBuf : memptr {
        PageBuf(){}
        ~PageBuf();
    private:
        PageBuf(const PageBuf&);
        void operator =(const PageBuf&){}
    };

    struct RegxMacro {
        char c;
        const char *crange;
    };

    // -----------------------------------------------------------------------------

    template<class T>
    struct Renew {
        T &t;
        Renew(T &in_t) : t(in_t){
        }
        ~Renew() {
            new (&t)T;
        }
    };

    // -----------------------------------------------------------------------------
    // Cursor support for directory scanning:
    // ind_beg(Dir_curs,entry,"*.gcov") {
    // }ind_end;
    //
    // ind_beg(Dir_curs,entry,".atf_ci/covbuild/*.xml.errors") {
    // }ind_end;
    struct Dir_curs {
        typedef DirEntry ChildType;
        DirEntry E;
    };

    // -----------------------------------------------------------------------------
    // Cursor for reading a text file line by line
    // ind_beg(FileLine_curs,line,filename") {
    // }ind_end;
    // Access current line number with ind_curs(line).i+1
    struct FileLine_curs {
        typedef strptr ChildType;
        algo_lib::InTextFile file;// current file
        strptr line;// current line
        int i;// current line number - 1 (for compatibility with array iteration)
        Bool eof;
    };

    struct StringIter {
        strptr expr;
        int    index;

        explicit StringIter(strptr s=strptr(), int in_index=0) : expr(s), index(in_index) {
        }
        char        Peek();
        char        GetChar();
        strptr      Rest();
        StringIter  &Ws();
        bool        EofQ() const;
        int         GetDigit(int dflt=-1);
    };

    // -----------------------------------------------------------------------------
    // List formatting helper.
    // When printed for the first time, prints nothing
    // When printed subsequently, prints specified string.
    // Usage:
    // ListSep ls(",");
    // for (i=0; i<3; i++) {
    //     str << ls << i;
    // }
    // Result: 1,0,2

    struct ListSep {
        strptr sep;
        mutable int iter;
        inline ListSep(strptr sep_=", ");
    };

    // -----------------------------------------------------------------------------
    // Line cursor (works with ind_beg/ind_end)
    // Usage:
    // ind_beg(Line_curs,line,file_contents) {
    //     prlog(line);
    // }ind_end;
    struct Line_curs {
        typedef strptr ChildType;
        StringIter contents;
        bool eof;
        int i;
        strptr line;
        inline Line_curs();
    };

    // Word cursor (works with ind_beg/ind_end)
    // Usage:
    // ind_beg(Word_curs,word,line) {
    //     prlog(word);
    // }ind_end;
    // Whitespace separator characters before and in between words are discarded.
    // So, " aaa     b" is two tokens, "aaa" and "b";
    //
    struct Word_curs {
        typedef strptr ChildType;
        strptr text;
        strptr token;// current token
        int index;// current index (may be past token.end)
        inline Word_curs();
    };

    // Token cursor (works with ind_beg/ind_end)
    // Tokens are separated by sep char.
    // Usage:
    // ind_beg(Sep_curs,token,line,sep) {
    //     prlog("Token number "<<ind_curs(token).index<<" is "<<token);
    // }ind_end;
    // Consecutive separators are not collapsed i.e. empty token is issued between them.
    // If no separator found, the token is entire string even it is an empty string.
    struct Sep_curs {
        typedef strptr ChildType;
        strptr rest;
        char   sep;
        strptr token; // current token
        int    index; // token index starts from 0, not needed for cursor, just for convenience
        enum State {  // state machine:
            normal,   // valid token before separator
            last,     // valid token before eol
            invalid   // invalid token
        } state;
        inline Sep_curs();
    };

    // -----------------------------------------------------------------------------
    // Unsigned integer ranges cursor
    // Usage:
    // ind_beg(u64_Ranges_curs,i,"1,5,8-10,14-16") {
    //     // u64 &i iterates over 1, 5, 8, 9, 10, 14, 15, 16
    // }ind_end;
    // Note:
    // - iteration is done exactly as it is written - no sorting or de-duplication happens;
    // - values or ranges are comma-separated;
    // - ranges are inclusive, syntax: begin value, minus sign, end value;
    // - range where end value is less than begin one is ignored (no swap);
    // - except whitespace, extra characters are not allowed (double comma is error);
    // - iteration stops on first syntax error.
    struct u64_Ranges_curs {
        typedef u64 ChildType;
        algo::StringIter iter;
        u64              cur;
        u64              end;
        bool             valid;
        u64_Ranges_curs();
    };

    // -----------------------------------------------------------------------------
    // Day range cursor from start to end
    struct DayRange_curs {
        typedef algo::UnTime ChildType;
        algo::UnTime         cur;
        algo::UnTime         end;
        DayRange_curs() : cur(), end() {};
    };

    // -----------------------------------------------------------------------------
    // tempstr:
    // This type is used when returning a string from a function
    // It behaves exactly like cstring with the following exceptions:
    // Whenever tempstr A is assigned to tempstr or cstring B, the contents of A and B is swapped
    // This MOVE CONSTRUCTOR has the effect of saving an extra copy/dealloc when returning string
    // from function.
    // For this reason, never use tempstr other than for this intended purpose,
    // as its contents can suddently be lost if it ever appears on the right hand side of an assignment
    struct tempstr : cstring {
        tempstr();
        explicit tempstr(const cstring &rhs) : cstring(rhs){}
        explicit tempstr(const strptr& rhs) : cstring(rhs){}
        tempstr(const tempstr &rhs);// move ctor.
        void operator=(strptr s) { cstring::operator =(s); }
        void operator=(const tempstr &s) { operator=(strptr(s)); }
        ~tempstr();
        operator strptr() const { return strptr((char*)ch_elems,ch_n); }
    };

    // -----------------------------------------------------------------------------
    // string descriptor, used for testing of amc-generated strings

    struct StringDesc {
        InitFcn         Init;
        SetnumFcn       SetnumMaybe;
        Geti64Fcn       Geti64;
        GetaryFcn       Getary;
        algo::Smallstr100   smallstr;
        algo::Smallstr50    strtype;
        char            pad;
        u32         base;
        u32         max_length;
        u32         min_length;
        u64         numtype_max;
        i64         numtype_min;
        bool            issigned;
    };

    // -----------------------------------------------------------------------------
    // Ssim tuple attribute cursor
    // Usage:
    // ind_beg(Attr_curs,attr,"a:b c:d") {
    //     attr.name == "a";
    //     attr.value == "b";
    // }ind_end;
    struct Attr_curs { // algo.Attr_curs
        typedef Attr_curs ChildType;
        algo::StringIter iter;
        tempstr          name;
        tempstr          value;
        bool             valid;
        Attr_curs();
    };

    struct TimeStruct : tm {
        int  tm_nsec;
        bool tm_neg;
        inline TimeStruct();
        inline TimeStruct(const struct tm &t);
    };

    static const i64 MSECS_PER_DAY     = 60*60*24*1000;
    static const i64 SECS_PER_DAY      = 60*60*24;
    static const i64 SECS_PER_HOUR     = 60*60;
    static const i64 SECS_PER_MIN      = 60;
    static const i64 UNTIME_PER_SEC    = 1*1000*1000*1000;
    static const i64 UNTIME_PER_MSEC   = 1*1000*1000;
    static const i64 UNTIME_PER_USEC   = 1*1000;
    static const i64 WTIME_OFFSET   = 116444736000000000LL;
    static const i64 WTIME_PER_SEC  = 10000000;
    static const i64 WTIME_PER_MSEC = 10000;
    static const i64 WTIME_PER_USEC = 10;
}

// -----------------------------------------------------------------------------

// functions in algo_lib from algo namespace
namespace algo { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/lib/algo/arg.cpp -- Parse command-line
    //
    strptr gitinfo_Get();

    // -------------------------------------------------------------------
    // cpp/lib/algo/backtrace.cpp -- Print stack trace in case of crash
    //
    void ShowStackTrace(uintptr_t start_ip, cstring &out);
    void FatalErrorExit(const char *a) __attribute__((noreturn));

    // catch fatal signals and show backtrace
    void SetupFatalSignals();

    // -------------------------------------------------------------------
    // cpp/lib/algo/base64.cpp -- base64 encoding
    //

    // print base64-encoded string
    void strptr_PrintBase64(strptr str, cstring &out);

    // -------------------------------------------------------------------
    // cpp/lib/algo/crc32.cpp -- Software-based CRC32
    //
    u32 CRC32Step(u32 old, const u8 *data, size_t len);

    // -------------------------------------------------------------------
    // cpp/lib/algo/decimal.cpp
    //

    // Normalize decimal, so mantissa does not contain tail 0
    void Decimal_Normalize(algo::Decimal &parent);

    // Try parse Decimal [+-][0-9]*[.][0-9]*
    // Stop after first non-conforming char, error if no digit has been met.
    bool TryParseDecimal(algo::StringIter &iter, algo::Decimal &result);

    // Print Decimal
    void Decimal_Print(algo::Decimal parent, algo::cstring &str);

    // Read Decimal from string
    bool Decimal_ReadStrptrMaybe(algo::Decimal &parent, algo::strptr in_str);

    // Convert Decimal to double
    double Decimal_GetDouble(algo::Decimal parent);

    // Convert double to Decimal
    void Decimal_SetDouble(algo::Decimal &parent, double value);

    // -------------------------------------------------------------------
    // cpp/lib/algo/file.cpp -- File functions
    //

    // Copy file FROM to file TO, creating the file if necessary
    // with mode MODE.
    // Throw exception on failure.
    void CopyFileX(strptr from, strptr to, int mode);

    // Read all bytes from IN_FD and copy them to OUT_FD, using read/write.
    // Return success status.
    bool CopyFd(Fildes in_fd, Fildes out_fd) __attribute__((nothrow));

    // Test whether FNAME refers to a valid filesystem entity (file, directory, or special file)
    // If FNAME is a soft link, then TRUE is returned even if the link points to
    // a non-existent location.
    bool FileObjectExistsQ(strptr fname) __attribute__((nothrow));

    // Test whether PATH is an existing directory
    bool DirectoryQ(strptr path) __attribute__((nothrow));

    // Test if F refers to an existing regular file (i.e. not a special file or directory)
    bool FileQ(strptr fname) __attribute__((nothrow));

    // Wrapper for c library realpath function.
    // On Windows: read path and expand all soft links along the way; Also eat ..'s.
    // The following table shows successive values of LEFT and RIGHT as the loop executes
    // ExpandLinks("bin/amc")
    // LEFT                                   RIGHT
    // bin                                    amc
    // bin/amc
    // link=../build/release/amc
    // bin                                    ../build/release/amc
    // build/release/amc
    // build                                  release/amc
    // build/release                          amc
    // link=CYGWIN_NT-cl.release-x86_64
    // build                                  CYGWIN_NT-cl.release-x86_64/amc
    // build/CYGWIN_NT-cl.release-x86_64      amc
    // build/CYGWIN_NT-cl.release-x86_64/amc
    tempstr GetFullPath(strptr path) __attribute__((nothrow));

    // Delete file F.
    // Return success code.
    bool DeleteFile(strptr f) __attribute__((nothrow));

    // Remove everything up to and including the last slash in A,
    // and return that. This is equivalent to Pathcomp(A,"/RR");
    strptr StripDirName(strptr a) __attribute__((nothrow));

    // Strip last slash and everything after it in A,
    // and return that. This is equivalent to Pathcomp(A,"/RL");
    strptr GetDirName(strptr a) __attribute__((nothrow));

    // Recursively create directory S and all of its components,
    // using mode MODE for newly created directories.
    // if DO_THROW is specified, throw exceptions on failure.
    // If DO_THROW is false, return success value.
    // TODO: test on windows
    bool CreateDirRecurse(strptr s, bool do_throw = true, u32 mode = 0755);

    // Strip last component of a path.
    // If the path ends in a slash, the slash is stripped as well.
    // So, StripDirComponent("abcde/d/") -> abcde/
    // So, StripDirComponent("abcde/d") -> abcde/
    strptr StripDirComponent(strptr a) __attribute__((nothrow));

    // Combine paths A and B, placing a slash in the middle if necessary.
    // If B is already an absolute pathname, just return that.
    tempstr DirFileJoin(strptr a, strptr b) __attribute__((nothrow));

    // Remove pathname A with any extension stripped.
    // StripExt("abcd/e.txt") -> "abcd/e"
    // StripExt("abcd/e.txt/f") -> "abcd/e/f"
    // (The last slash shields the extension, so this function has no corresponding
    // Pathcomp expression)
    strptr StripExt(strptr a) __attribute__((nothrow));

    // Return extension of pathname A, including the dot.
    // GetFileExt("abcd/e.txt") -> ".txt"
    // GetFileExt("abcd/e.txt/f") -> ""
    // This function is equivalent to Pathcomp(A,"/RR.RR")
    strptr GetFileExt(strptr a) __attribute__((nothrow));

    // Check if path PATH is an absolute pathname,
    // meaning that it starts with / or ~
    // On windows, the same test is in force, but in addition any pathname
    // where the second character is a : (e.g. c:\blah) is recognized as an absolute
    // path name
    bool AbsolutePathQ(strptr path) __attribute__((nothrow));

    // Replace extension in pathname A with string B.
    tempstr ReplaceExt(strptr a, strptr b) __attribute__((nothrow));

    // Return current directory name.
    tempstr GetCurDir() __attribute__((nothrow));

    // Change current directory to DIR and return success status
    // errno is set as witih chdir() call
    bool SetCurDir(strptr in_dir) __attribute__((nothrow));

    // Calculate size of file referred to by FILENAME.
    // If file is not found or an error occurs, 0 is returned.
    i64 GetFileSize(strptr filename) __attribute__((nothrow));

    // Return size of file referred to by FD.
    // On error, return zero.
    i64 GetFileSize(Fildes fd) __attribute__((nothrow));

    // Strip directory name in PATH. and return the rest
    // This is equivalent to Pathcomp(path,"/RR");
    // c:\dir\file.txt -> file
    strptr GetFileName(const strptr& path) __attribute__((nothrow));

    // F  ("a.txt","b") -> "b.txt";
    // F  ("a.txt","/b.jpg") -> "b.jpg.txt"
    tempstr ReplaceFileName(const strptr& a, const strptr& b);
    algo::UnTime ModTime(strptr filename);
    bool RemDir(strptr name);

    // Destroy directory NAME, and any subdirectories.
    // If REMOVE_TOPMOST is specified, delete the directory as well,
    // (otherwise, only the contents are deleted).
    // If filesystem entry exists and is NOT a directory, return true.
    bool RemDirRecurse(strptr name, bool remove_topmost);
    void dir_handle_Cleanup(algo::DirEntry &dir_entry);

    // Open file FILENAME with flags FLAGS, return resulting file descriptor
    // Possible flags:
    // write   -> open file for writing, create file if missing
    // if write is specified, and append is not, then file is truncated upon opening
    // append  -> open file in append mode (automatically sets 'write' flag)
    // in append mode, seek to end of file after opening
    // read    -> open flag in read-only mode
    // _throw  -> throw exception if an error occurs
    // NOTE: this function doesn't set O_APPEND flag, because it doesn't work'
    // on NFS mounted filesystems.
    // Without O_APPEND, two processes cannot reliably append to the same file.
    algo::Fildes OpenFile(const strptr& filename, algo::FileFlags flags);

    // Write BYTES to file FD;
    // If FD is non-blocking, the function may fail.
    // Throw exception on error. Upon completion, the function has succeeded
    void WriteFileX(Fildes fd, memptr bytes);

    // Set file position of FD to OFF
    // Return success status
    bool SeekFile(Fildes fd, i64 off);

    // Return current file position on FD
    // There is no provision to return an error code; only the offset is returned
    // (zero on failure?)
    i64 GetPos(Fildes fd);

    // Truncate file indicated by FD to size SIZE.
    // Return success status
    // TODO: Test on windows
    bool TruncateFile(Fildes fd, i64 size);

    // Return contents of file as string.
    // File is trimmed at first NULL character
    // Implementation note:
    // linux files in /proc directory lie about their size
    // and are sometimes zero-terminated.
    // File is read using a "safe" method of succesively calling read.
    // relying on reported file size or using mmap does not work in all cases
    // Todo: test on windows
    const tempstr FileToString(const strptr& fname, algo::FileFlags flags = algo_FileFlags__throw);

    // Read all bytes from IN_FD as a string.
    // NUL characters in file will appear in the string --
    // TODO: add option to sanitize string
    // If FD is non-blocking, reading will terminate at first EAGAIN.
    // If an error occurs, all data prior to occurrence of the error is returned.
    const tempstr FdToString(Fildes in_fd) __attribute__((nothrow));
    algo::Fildes CreateReplacementFile(cstring &oldfname, cstring &newfname, int dfltmode);

    // Compare STR to contents of FILENAME.
    // If they differ, use a tempfile construction to atomically UPDATE file
    // with contents of STR.
    // NOTE: This will result in a new inode for file!
    // No exceptions are thrown. If the function fails, check errno.
    // Default mode for new file is provided by MODE.
    // If the file is being replaced, MODE is ignored and copied from the old file.
    bool SafeStringToFile(const strptr& str, const strptr& filename, int dfltmode);
    bool SafeStringToFile(const strptr& str, const strptr& filename);

    // Replace contents of file FILENAME with string STR.
    // If CHECK_SAME is specified, first compare contents and do not perform a write
    // if the contents are the same.
    // FLAGS may specify algo_FileFlags__throw, in which case an exception is thrown on error
    bool StringToFile(const strptr& str, const strptr& filename, algo::FileFlags flags = algo_FileFlags__throw, bool check_same = true);
    algo::Fildes OpenWrite(const strptr& filename, algo::FileFlags flags = algo::FileFlags());
    algo::Fildes OpenRead(const strptr& filename, algo::FileFlags flags = algo::FileFlags());

    // Write NWRITE bytes starting at START to file descriptor FILDES.
    // Go until all bytes are written on an error occurs.
    // If FILDES is non-blocking, spin indefinitely until bytes do get through.
    // At the end, return success status (TRUE if all bytes written)
    bool WriteFile(algo::Fildes fildes, u8 *start, int nwrite);
    void Dir_curs_Next(Dir_curs &curs) __attribute__((nothrow));

    // Begin scanning files matching shell pattern PATTERN.
    void Dir_curs_Reset(Dir_curs &curs, strptr pattern) __attribute__((nothrow));
    bool Dir_curs_ValidQ(Dir_curs &curs) __attribute__((nothrow));
    algo::DirEntry &Dir_curs_Access(Dir_curs &curs) __attribute__((nothrow));

    // Set blocking mode on file descriptor FD to BLOCKING
    // Return result of FCNTL (0==success)
    int SetBlockingMode(Fildes fildes, bool blocking) __attribute__((nothrow));

    // Read soft link and return resulting path.
    // If PATH is not a soft link, return empty string
    // This is not the function to resolve symlinks (if link temp/x points to y, then
    // this function will return string "y", not "temp/y"), use GetFullPath for
    // full service
    tempstr ReadLink(strptr path);

    // -------------------------------------------------------------------
    // cpp/lib/algo/fmt.cpp -- Print to string / Read from string
    //

    // Examine string VALUE
    // Determine if it needs quoting as an ssim key/value,
    // and return appropriate quote character. If no quote character is needed,
    // return NUL.
    char PickSsimQuoteChar(strptr value);

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
    void Ptr_Print(void *ptr, algo::cstring &out);

    // pads with zeros on the left so that at least 'atleast' characters are output.
    void i64_PrintPadLeft(i64 num, algo::cstring &out, int atleast);

    // Assuming SRC is a number, Transfer SRC to OUT, inserting
    // commas between groups of 3 numbers.
    // 0 -> 0
    // 11 -> 11
    // 222 -> 222
    // 3333 -> 3,333
    // 4567.3 -> 4,567.3
    // 1.11111 -> 1.11111
    void strptr_PrintWithCommas(strptr src, algo::cstring &out);

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
    void double_PrintPrec(double d, algo::cstring &out, int precision, bool omit_zeros, bool commas);

    // Print string, replacing '_x' with 'X', and capitalizing first word.
    void strptr_PrintCamel(algo::strptr str, algo::cstring& out);

    // Convert CamelCase to lower_under.
    // A consecutive run of caps gets a single leading underscore.
    // Leading cap doesn't get an underscore
    // Example: "abcd testString"  -> "abcd test_string"
    // Example: "Abcd" -> "abcd"
    // Example: "BBOPrice" -> "bboprice"
    // Example: "SymbolID" -> "symbol_id"
    // Example: "A_B" -> "a_b"
    void strptr_PrintLowerUnder(algo::strptr str, algo::cstring &out);

    // Output string STR to OUT changing all characters to uppercase.
    void strptr_ToUpper(algo::strptr str, algo::cstring& out);
    void u128_PrintHex(u128 value, algo::cstring &out, bool prefix);
    void u64_PrintHex(u64 value, algo::cstring &out, int atleast, bool prefix,  bool caps);

    // Print value VALUE to string OUT,
    // producing least ATLEAST hex characters.
    // Include prefix 0x.
    void u64_PrintHex(u64 value, algo::cstring &out, int atleast);

    // Print value VALUE to string OUT,
    // producing least ATLEAST hex characters.
    // if PREFIX is specified, include prefix 0x.
    void u64_PrintHex(u64 value, algo::cstring &out, int atleast, bool prefix);

    // print character as c++ character, surrounded by single quotes
    // and properly escaped according to c++ rules
    void char_PrintCppSingleQuote(int c, algo::cstring &out);

    // prints a character suitable for appearance inside an XML string.
    // character is either printed as-is, or is escaped so that its meaning does not change.
    void char_PrintXml(char c, algo::cstring &out, bool quoted);

    // Print string NAME as a TeX identifier to OUT, converting digits to letters
    // (0 -> zero, etc) and squashing other characters not belonging to TeX
    // identifier character class
    void strptr_PrintTexIdent(strptr name, algo::cstring &out);

    // print STR to OUT, escaping TeX characters that switch modes
    // (_, #, &, $)
    void strptr_PrintTexString(strptr str, algo::cstring &out);

    // Print string using XML rules.
    void strptr_PrintXml(algo::strptr s, algo::cstring &out, bool quoted);

    // Append N instances of character C to string OUT.
    void char_PrintNTimes(char c, algo::cstring &out, int n);
    void strptr_PrintAligned(algo::strptr str, algo::cstring &out, int nplaces, algo::TextJust align, char c);
    void cstring_Print(algo::cstring &row, algo::cstring &str);
    void strptr_Print(const algo::strptr &row, algo::cstring &str);

    // Append a space unless the string already ends with a space
    void  MaybeSpace(cstring &str);

    // print value*100
    // Example:
    // double_PrintPercent(0.334, str, 1) -> "33.4%"
    void double_PrintPercent(double value, algo::cstring &str, int prec);
    void i32_Range_Print(algo::i32_Range &r, algo::cstring &o);
    void double_PrintWithCommas(double value, algo::cstring &str, int prec);

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
    void TimeStruct_Print(const TimeStruct &time, algo::cstring &str, const algo::strptr &spec);
    void Tuple_Print(algo::Tuple &row_, algo::cstring &str);
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
    // If QUOTE is 0, the need for quotes and the type of quote is determined automatically.
    // If it is any other value, then that type of quote is used
    // According to the CSV standard, the quote itself is the escape character.
    void strptr_PrintCsv(algo::strptr str, algo::cstring &out, char quote);

    // Print CSV field, auto-determine quotes
    void strptr_PrintCsv(algo::strptr str, algo::cstring &out);
    void URL_Print(algo::URL &url, algo::cstring &str);

    // Append STR to OUT, and pad remainder with character FILL
    void strptr_PrintPadRight(algo::strptr str, algo::cstring &out, int nplaces, char fill);

    // Append STR to OUT, and pad remainder with space characters
    void strptr_PrintPadRight(algo::strptr str, algo::cstring &out, int nplaces);

    // Append STR to OUT, padding start with character FILL
    void strptr_PrintPadLeft(algo::strptr str, algo::cstring &out, int nplaces, char fill);

    // Append STR to OUT, padding start with spaces.
    void strptr_PrintPadLeft(algo::strptr str, algo::cstring &out, int nplaces);

    // Append STR to OUT as an SQL string using quote character Q.
    // Escaped characters:
    // '\' (backslash)
    // ' quoted with '' if q is ' and not quoted otherwise. (Compatible with MariaDB & T-SQL.)
    // " quoted with "" if q is " and not quoted otherwise. (Compatible with MariaDB & T-SQL.)
    // NUL (ascii zero)
    // \n
    // \r
    // \t
    void strptr_PrintSql(algo::strptr str, algo::cstring &out, char q);
    void strptr_PrintSql(algo::strptr str, algo::cstring &out);
    void Attr_Print(algo::Attr &attr, algo::cstring &str);

    // Print a string suitable for parsing with Tuple
    // If the string doesn't need quotes, it is printed as-is.
    // If quotes are needed, first determine whether to use single or double quotes to minimize
    // escaping. Then print the string using char_PrintCppEsc for quoting.
    // Empty string is printed as ""
    void PrintAttrSpace(cstring &str, algo::strptr name, const strptr &value);
    void ListSep_Print(const algo::ListSep &t, algo::cstring &str);

    // copy string B to TO, using ORIG as a case template
    // I.e. PrintCopyCase("AbcD", to, "somestring") -> "SomEstring"
    void strptr_PrintCopyCase(const algo::strptr &orig, algo::cstring &to, const algo::strptr &b);
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
    bool cstring_ReadCmdarg(cstring &out, algo::StringIter &S, bool is_value);

    // Read Charset from list of chars.
    // Every character in RHS is simply added to the bitset
    void Charset_ReadStrptrPlain(algo::Charset &lhs, strptr desc);
    bool Charset_ReadStrptrMaybe(algo::Charset &lhs, strptr rhs);

    // Print STR to OUT in a way that's acceptable as input for bash.
    void strptr_PrintBash(strptr str, algo::cstring &out);

    // Escape S according to bash rules and return result
    tempstr strptr_ToBash(strptr str);

    // Escape S according to ssim rules and return result
    tempstr strptr_ToSsim(strptr str);

    // encode uri component.
    // before ? use plus=false (no + substitution by space)
    void strptr_PrintUri(strptr str, algo::cstring &out, bool plus);

    // Print string, escaping characters in a way suitable for graphviz dot format
    void strptr_PrintDot(strptr s, algo::cstring &out);

    // print binary octet string as hex
    void Sha1sig_Print(algo::Sha1sig &sha1sig, algo::cstring &out);
    bool Sha1sig_ReadStrptrMaybe(algo::Sha1sig &sha1sig, algo::strptr str);

    // Write character C into string OUT, using C++ character escapement rules
    // QUOTE_CHAR also gets escaped.
    void char_PrintCppEsc(char c, algo::cstring &out, char quote_char);

    // Print STR, surrounded by quotes as C++ string
    // surrounded by QUOTE_CHAR quotes, to buffer OUT.
    // All string characters are escaped using char_PrintCppEsc.
    void strptr_PrintCppQuoted(algo::strptr str, algo::cstring &out, char quote_char);

    // Print STR as a C++ string to OUT.
    void strptr_PrintCpp(algo::strptr str, algo::cstring &out);

    // Print STR as a valid SSIM key/value to OUT.
    // SSIM tokens use quotes only when the printed value contains
    // characters outside of the 'safe' set a-zA-Z0-9_;&*^%$@.!:,+/-
    void strptr_PrintSsim(algo::strptr str, algo::cstring &out);

    // Begin append key/value to string LHS
    int BeginKeyval(cstring &lhs, const strptr &name);

    // End append key/value to string LHS
    cstring &EndKeyval(cstring &lhs, int start);

    // Append key-value pair NAME:VALUE to string STR
    void PrintAttr(cstring &str, const algo::strptr &name, const strptr &value);

    // Append key-value pair NAME:VALUE to string STR, and remove all
    // characters from VALUE (used by amc)
    void PrintAttrSpaceReset(cstring &str, const algo::strptr &name, algo::cstring &value);

    // Append key-value pair NAME:VALUE to string STR,
    // and remove all characters from VALUE (used by amc)
    void PrintAttrSpaceReset(cstring &str, const char *name, algo::cstring &value);

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
    void strptr_PrintRegxSql(algo::strptr value, algo::cstring &str);
    void WDiff_Print(algo::WDiff row, algo::cstring &str);
    void WTime_Print(algo::WTime row, algo::cstring &str);
    bool WDiff_ReadStrptrMaybe(algo::WDiff &parent, algo::strptr in_str);
    bool WTime_ReadStrptrMaybe(algo::WTime &parent, algo::strptr in_str);
    void u64_PrintBase32(u64 k, algo::cstring &str);

    // -------------------------------------------------------------------
    // cpp/lib/algo/lib.cpp -- Main file
    //

    // Execute unix command and return output.
    tempstr SysEval(strptr cmd, FailokQ fail_ok, int max_output, bool echo = false);

    // Execute unix command and return status code.
    // Execute command using system().
    // fail_ok: if true, command must succeed or throw exception
    // dry_run: if true, do not run command, just print
    // echo: print command. command alwyas echoed if verbose mode is on
    int SysCmd(strptr cmd, FailokQ fail_ok = FailokQ(true), DryrunQ dry_run = DryrunQ(false), EchoQ echo = EchoQ(false));

    // Return a human-readable description of STATUS as returned by wait() / waitpid()
    tempstr DescribeWaitStatus(int status);

    // The several variants of Throw are all the same -- the goal is to get as many
    // setup instructions out of the execution path as possible, to avoid polluting instruction
    // cache.
    void Throw(strptr text, Errcode err) __attribute__((noreturn));
    void Throw() __attribute__((noreturn));
    bool Tuple_EqualQ(Tuple &t1, Tuple &t2);
    Attr* attr_Find(Tuple &tuple, strptr name, int occurence = 0);
    strptr attr_GetString(Tuple &T, strptr name, strptr dflt = strptr());

    // Append new attribute with specified name and value.
    void attr_Add(Tuple &T, strptr name, strptr value);

    // Call sleep/usleep, giving up MS milliseconds
    void SleepMsec(int ms);
    Errcode FromErrno(i64 val);
    Errcode FromWinErr(i64 val);
    i64 I64Power10(u32 power);

    // Copy bytes from one location to another in blocks of 16 bytes.
    void StreamFromToB16(void *from, void *to, u32 n);
    void CopyFromToB16(void *from, void *to, u32 n);

    // SSE implementation of fast exponentiation idea from
    // http//nic.linotune.com/pubs/Schraudolph99.pdf
    // ExpFast(x) goes bad at x < -709 (exp doesn't)
    // Where it works, ExpFast makes a steady relative error of about 2%.
    double ExpFast(double y);

    // lock all presently, and future allocated bytes in physical memory.
    // Return success value. Errno provides more info.
    bool LockAllMemory();
    void SetupExitSignals(bool sigint = true);
    const tempstr GetHostname();
    const tempstr GetDomainname();
    double double_WeakRandom(double scale);
    i32 i32_WeakRandom(i32 modulo);

    // -------------------------------------------------------------------
    // cpp/lib/algo/line.cpp -- Line processing
    //

    // Accept data block DATA as input for the line buffer
    // LinebufNext will attempt to return pointers to a subregion of DATA until
    // it returns false, so this block must be valid until then.
    // If IMPLIED_EOF is set, then this is the last call to LinebufBegin.
    // LinebufNext will know to return a partial line (with no \n) and set the
    // EOF flag
    void LinebufBegin(LineBuf &L, memptr data, bool implied_eof);

    // Scan memory provided by LinebufBegin for line separator (hard-coded to be '\n')
    // If the function returns FALSE, and there is a partial line remaining,
    // it is saved to an internal buffer in LineBuf (called BUF).
    // It will be subsequently recombined with incoming data to form a contiguous line,
    // but copying memory is avoided whenever possible.
    // Usage:
    // LinebufBegin(linebuf,data,true);
    // for (strptr line; LinebufNext(linebuf,line); ) {
    // <process line>
    // }
    bool LinebufNext(LineBuf &L, strptr &result);

    // Read characters up to newline or end of file.  Return
    // resulting substring, skipping the newline
    // (which is either \n or \r\n).
    strptr GetLine(StringIter &S);

    // Line_curs uses strptr to hold on to the string being scanned
    // (to avoid copying a potentially huge string)
    // The use of "strptr&" prevents passing a temporary.
    // Line_curs handling of newlines is identical to that of FileLine_curs
    void Line_curs_Reset(Line_curs &curs, algo::strptr &text);
    void Line_curs_Reset(Line_curs &curs, algo::cstring &text);

    // Scan for next line.
    void Line_curs_Next(Line_curs &curs);
    void FileLine_curs_Reset(algo::FileLine_curs &curs, strptr filename);
    void FileLine_curs_Reset(algo::FileLine_curs &curs, algo::Fildes fd);
    void FileLine_curs_Next(algo::FileLine_curs &curs);

    // -------------------------------------------------------------------
    // cpp/lib/algo/prlog.cpp -- prlog macro
    //

    // Default implementation of prlog handler
    //
    // Notes on WriteFile use:
    // some tools set fd 1 to nonblocking mode, which causes EAGAIN during fast writes,
    // so we must use WriteFile (which contains a loop) to write all the bytes out.
    void Prlog(algo_lib::FLogcat *logcat, algo::SchedTime tstamp, strptr str);

    // -------------------------------------------------------------------
    // cpp/lib/algo/string.cpp -- cstring functions
    //

    // S         source string
    // EXPR      string in the form (XYZ)*
    // - X is the character to search
    // - Y is L to search from left, R to search from right
    // - Z is which site to pick: L=left, R=right
    // If the character to search is not found, it assumed to be found
    // at the far end of the string with respect to search direction.
    // Function processes characters from expr in groups of 3.
    // Example:
    // s = "a.b.c.d"
    // expr = ".LR.LL"
    // first, search for . from left; pick right hand side (b.c.d)
    // then, search for . from left. pick left hand side (b)
    // result: b
    // More examples:
    // s = "abcd"; expr = ".LL"; result = "abcd"
    // s = "abcd"; expr = ".LR"; result = ""
    strptr Pathcomp(strptr s, strptr expr);

    // Check if the Pathcomp expression PATHCOMP
    // refers to the leftmost part of the key
    // (The third character of each Pathcomp step must be 'L')
    bool LeftPathcompQ(algo::strptr pathcomp);

    // Append NUL character to the end of the string and return a
    // pointer to the C string.
    // There is no guarantee that there are no NUL characters in the middle
    // of STR, that is up to the user.
    // cstring is normally NOT null terminated (it is length-based),
    // so this function is necessary when passing strings to C library functions
    // or the OS
    char *Zeroterm(cstring &rhs);

    // Same thing but with tempstr.
    // Typical usage is to take a strptr
    // expression and pass it to some unix call
    // some_unix_call(Zeroterm(tempstr(some_strptr)))
    //
    char *Zeroterm(const tempstr &rhs);

    // Construct ssim filename from key
    // SsimFname("data", "abcd") -> data/abcd.ssim
    // SsimFname("data/", "abcd.ef") -> data/abcd/ef.ssim
    tempstr SsimFname(strptr root, strptr rel);

    // Extract Namespace name from "ns.name" format.
    // This is equivalent to Pathcomp(s,".LL")
    strptr GetNs(strptr s);

    // if IDENT starts with NS_NAME., return ident without prefix
    // otherwise return ident
    // if NS_NAME is empty, removes any dotted prefix from IDENT.
    // NS_NAME      name of 'current' namespace (optional)
    // IDENT        identifier in question
    //
    // StripNs("abc", "abc.def") -> "def"
    // StripNs(""   , "abc.def") -> "def"
    // StripNs("abc", "def.ghi") -> "def.ghi"
    strptr StripNs(strptr ns_name, strptr ident);

    // This function echoes the corresponding MySQL function.
    // Treat STR as an array of tokens separated by C.
    // Locate and return tokens 1..IDX. Negative numbers cause scanning from the right.
    // Indexes are 1-based
    // Example:
    // SubstringIndex("a.b.c", '.',  1) -> "a"
    // SubstringIndex("a.b.c", '.',  2) -> "a.b"
    // SubstringIndex("a.b.c", '.',  3) -> "a.b.c"
    // SubstringIndex("a.b.c", '.',  4) -> "a.b.c"
    // SubstringIndex("a.b.c", '.', -1) -> "c"
    // SubstringIndex("a.b.c", '.', -2) -> "b.c"
    // SubstringIndex("a.b.c", '.', -3) -> "a.b.c"
    strptr SubstringIndex(strptr str, char c, int idx);
    int FindFrom(strptr s, strptr t, int from, bool case_sensitive);
    int FindFrom(strptr s, strptr t, int from);
    int FindFrom(strptr s, char c, int from);

    // Search for character/string from left to right
    // If found, return index where match occurs.
    // If not found, return -1
    int FindChar(strptr lhs, char c);
    int FindStr(strptr lhs, strptr t, bool case_sensitive);
    int FindStr(strptr lhs, strptr t);
    int CompareNoCase(strptr lhs, strptr s);

    // Check strings for equality, optionally in a case-insensitive way
    bool StrEqual(strptr a, strptr b, bool case_sens);
    bool StartsWithQ(strptr s, strptr sstr, bool case_sensitive = true);
    bool EndsWithQ(strptr s, strptr sstr);
    void MakeLower(strptr s);
    void MakeUpper(strptr s);

    // Locate all occurences of SEP in string IN, scanning line-by-line.
    // Compute column widths; Scan string IN again.
    // Expand each column to the required width, leaving COLSPACE characters between
    // columns.
    // String FMT specifies, for each column, whether to align its contents left,center,or right
    // ('l','c','r'). Default is 'l'.
    // Example text
    // aa\tcc\tbb
    // asdfasdf\t\tyy
    // Tabulated(text, "\t", "rl", 2) ->
    // aa  cc  bb
    // asdfasdf      yy
    tempstr Tabulated(strptr in, strptr sep, strptr fmt, int colspace);
    tempstr Tabulated(strptr in, strptr sep);

    // Given a string of up to 4 characters encoded in u32 STR
    // (LSB = first char, etc.)
    // Decode character into RESULT, returning number of characters read.
    // The character is C++-escaped: \a, \b, \f, \n, \r, \t, \v, \', \", \\, \? are supported
    // as well as octal (\OOO) and hex (\xHH) sequences.
    int UnescapeC(u32 str, int len, u8 &result);
    int Replace(cstring &str, const strptr& from, const strptr& to, bool case_sensitive = true, bool preserve_case = true);

    // Scan S. For any character in S that occurs in string FROM, replace with corresponding
    // character from string TO.
    // Example:
    // Translate("aabcd", "bd", "xy") -> "aaxcy"
    void Translate(strptr s, strptr from, strptr to);

    // Return a run of characters up to next occurence of SEP (or to end of string)
    // Skip any characters equal to SEP that follow the run
    // sep="|", string = "a||b|"; return value -> "a", rest -> "b|";
    // sep="|", string = "a";     return value -> "a", rest -> "";
    // sep="|", string = "|x";    return value -> "" , rest -> "x";
    strptr GetTokenChar(algo::StringIter &S, char sep);

    // Skip leading characters matching SEP
    // Return run of characters up to next matching SEP, or EOF.
    // Do not skip trailing separators.
    strptr GetWordCharf(algo::StringIter &iter, bool (*sep)(u32) = algo_lib::WhiteCharQ);

    // Skip any leading whitespace in STR.
    // Read and return next word.
    strptr GetTypeTag(strptr str);
    bool TryParseI32(algo::StringIter &iter, i32 &result);
    bool TryParseI64(algo::StringIter &iter, i64 &result);
    bool TryParseU32(algo::StringIter &iter, u32 &result);
    bool TryParseU64(algo::StringIter &iter, u64 &result);
    bool TryParseU128(algo::StringIter &iter, u128 &result);

    // Read a series of digits and return resulting number.
    // Return success code
    bool TryParseDigits(algo::StringIter &S, double &result);

    // TODO: document these more carefully
    // Read a series of digits N, returning N / pow(10, length(N))
    // If successful, advance index. Otherwise, leave index where it was.
    bool TryParseFraction(algo::StringIter &S, double &result);
    bool TryParseDouble(algo::StringIter &iter, double &result);

    // Search for the next occurence of SEP. If not found, assume it occurs at EOF
    // Return any characters between current position and the occurence of SEP
    // Skip SEP, if needed.
    strptr GetTokenStrptr(StringIter &iter, const strptr& separator);

    // if the next characters match STR, skip and return true
    // optionally do case sensitive comparison
    bool SkipStrptr(StringIter &iter, strptr str, bool case_sens);

    // Case-sensitive version
    bool SkipStrptr(StringIter &iter, strptr str);
    i64 ParseI64(StringIter &iter, i64 dflt);

    // Convert string to numeric type.
    // If there is an error, or string is empty, return DFLT.
    // If there is junk at the end of the string, silently ignore it.
    // Use StringIter version to check for junk, or use -X version to throw exception.
    u64 ParseU64(StringIter &iter, u64 dflt);
    int ParseI32(StringIter &iter, int dflt);
    u32 ParseU32(StringIter &iter, u32 dflt);

    // Find first occurence of MATCH in S
    // If found, return range corresponding to the match.
    // If not found, return range (S.n_elems,S.n_elems) -- an empty range positioned at
    // the end of S
    algo::i32_Range substr_FindFirst(const aryptr<char> &s, const aryptr<char> &match);

    // Same as above but search right-to-left.
    // In case of failure, return range (0,0) -- empty range positioned at start of S.
    algo::i32_Range substr_FindLast(const aryptr<char> &s, const aryptr<char> &match);

    // Strip leading whitespace, return new strptr.
    strptr TrimmedLeft(strptr s);

    // Strip trailing whitespace, return new strptr.
    strptr TrimmedRight(strptr s);

    // TrimmedLeft + TrimmedRight
    strptr Trimmed(strptr s);

    // scan STR until character SEP is found.
    // if SEP is not found, it is assumed to be beyond the right side of the string
    // set LEFT to point to the characters to the left of found location.
    // set STR to point to the characters to the right of found location.
    // Example:
    // str = "abc.def", sep='.'; After NextSep, str="def", left="abc"
    // str = "abc-def", sep='.'; After NextSep, str="", left="abc-def"
    void NextSep(algo::strptr &str, char sep, algo::strptr &left);
    void Word_curs_Reset(Word_curs &curs, strptr text);
    void Word_curs_Next(Word_curs &curs);

    // Skip any leading whitespace.
    // Read next word (GetWordCharf) and compare to typetag.
    // Skip any subsequent whitespace as well.
    // Return true if type typetag matches
    // if typetag doesn't match, input string remains unchanged
    // " a  b " -> typetag is "a", rest is "b "
    // "a    b" -> typetag is "a", rest is "b"
    bool StripTypeTag(strptr &in_str, strptr typetag);

    // Limit length of string S ot at most LEN characters
    // If S is trimmed, append "..." to the end
    tempstr LimitLengthEllipsis(strptr s, int len);
    algo::i32_Range ch_FindFirst(const algo::strptr &s, char match);
    algo::i32_Range ch_FindLast(const algo::strptr &s, char match);
    bool strptr_ReadStrptrMaybe(strptr , strptr );

    // Append / to string STR unless STR already ends in one.
    // str << dirname << MaybeDirSep << filename.
    // The separator is always /. To support windows-specific pathnames,
    // use ToWindows path where appropriate.
    void MaybeDirSep(cstring &str);
    i32 strptr_Cmp(algo::strptr a, algo::strptr b);
    void Attr_curs_Reset(Attr_curs &curs, strptr line);
    void Attr_curs_Next(Attr_curs &curs);

    // Compare two strings whose start address is 16-byte aligned
    // Do not use this function --
    bool AlignedEqual(strptr a, strptr b);

    // insert TEXT into OUT, indenting as necessary;
    // Initial indentation is INDENT, it's adjusted as necessary as { and } are found
    // in the TEXT.
    // Each indent is 4 spaces.
    // Trailing //-sytle comments are stripped
    // /* */-style comments are not supported
    // Lines beginning with # (#ifdef, etc) are printed at column zero.
    void InsertIndent(algo::cstring &out, strptr text, int indent);

    // Convert unix path to windows path
    // This replaces slashes with backslashes
    tempstr ToWindowsPath(strptr path);

    // compatibility
    void reset(algo::cstring &lhs);
    void DayRange_curs_Reset(algo::DayRange_curs &curs, const algo::DayRange& range);
    void DayRange_curs_Next(algo::DayRange_curs &curs);
    bool DayRange_curs_ValidQ(algo::DayRange_curs &curs);
    algo::UnTime& DayRange_curs_Access(algo::DayRange_curs &curs);
    void u64_Ranges_curs_Reset(algo::u64_Ranges_curs &curs, strptr str);
    void u64_Ranges_curs_Next(algo::u64_Ranges_curs &curs);
    bool u64_Ranges_curs_ValidQ(algo::u64_Ranges_curs &curs);
    u64 &u64_Ranges_curs_Access(algo::u64_Ranges_curs &curs);
    void Sep_curs_Reset(algo::Sep_curs &curs, strptr line, char sep);
    void Sep_curs_Next(algo::Sep_curs &curs);

    // -------------------------------------------------------------------
    // cpp/lib/algo/time.cpp -- UnTime / UnDiff functions
    //
    bool TimeStruct_Read(TimeStruct &out, algo::StringIter &iter, const strptr& spec);
    TimeStruct ToTimeStruct(UnDiff   U);
    TimeStruct ToTimeStruct(UnixDiff U);
    algo::UnixDiff ToUnixDiff(const TimeStruct &S);
    algo::UnDiff ToUnDiff(const TimeStruct &S);
    const algo::UnixTime LocalDate(UnixTime in);
    TimeStruct GetLocalTimeStruct(UnixTime U);
    TimeStruct GetLocalTimeStruct(UnTime U);
    const algo::UnTime LocalDate(UnTime in);
    algo::UnDiff StripDate(UnTime t);
    algo::TimeStruct GetGMTimeStruct(algo::UnTime U);
    algo::UnixTime ToUnixTime(const TimeStruct &S);
    algo::UnTime ToUnTime(const TimeStruct &S);

    // empty string -> 0
    // invalid weekday -> -1
    // All other weeks days map to 0..6, numbers compatible with struct tm's tm_wday field
    int GetWeekday(strptr wday);
    const strptr GetWeekdayName(int index);
    int GetMonthZeroBased(strptr month);
    int GetWholeMonthZeroBased(strptr s);

    // Inverse mapping to abbreviated month names (Jan, Feb, etc.)
    // If short_name==false, returns full name (January, February, etc.)
    const strptr GetMonthNameZeroBased(int index);
    const strptr GetMonthNameZeroBasedShort(int index);

    // DateCache -- Roughly 200x faster LocalDate
    const algo::UnTime DateCache_LocalDate(algo::DateCache &dc, UnTime in);
    algo::UnTime ToUnTime(WTime s);
    algo::UnTime ParseUnTime(StringIter &s, const algo::strptr spec);
    algo::UnTime ParseUnTime(const algo::strptr& s, const algo::strptr spec);

    // Change TZ environment variable to specified value
    // and notify C runtime lib of the change
    void SetTz(strptr zone);

    // -------------------------------------------------------------------
    // cpp/lib/algo/tstamp.cpp -- timestamp cache
    //

    // Allowing only for fixed length formats.
    void tstamp_cache_Init(TstampCache& cache, strptr spec, bool gmt);
    void tstamp_cache_Print(algo::TstampCache& cache, UnTime t, cstring& out);

    // -------------------------------------------------------------------
    // cpp/lib/algo/txttbl.cpp -- Ascii table
    //
    bool SaneTerminalQ();

    // -------------------------------------------------------------------
    // include/algo.inl.h -- Inline functions
    //

    // Refurbish: destroy and re-create object in place.
    // This function is exception-safe: if ~T throws an exception, T::T still gets
    // called.
    // Implementation note: explicit calls to destructor are checked for NULL pointer
    // by GCC, so the nonnull attribute is required.
    template<class T> inline void Refurbish(T &t) F_NONNULL;
    template<class T> inline int elems_N(const aryptr<T> &ary);
    template<class T> inline int ch_N(const aryptr<T> &ary);
    template<class T> inline void Fill(const aryptr<T> &lhs, const T &t);
    template<class T, class U> inline int Find(const algo::aryptr<T> &lhs, const U&t);
    template<class T> inline algo::aryptr<T> FirstN(const algo::aryptr<T> &lhs, u32 n);
    template<class T> inline algo::aryptr<T> LastN(const algo::aryptr<T> &lhs, u32 n);
    template<class T> inline algo::aryptr<T> RestFrom(const algo::aryptr<T> &lhs, u32 n);
    template<class T> inline algo::aryptr<T> qGetRegion(const algo::aryptr<T> &lhs, u32 lo, u32 n);
    template<class T> inline algo::aryptr<T> GetRegion(const algo::aryptr<T> &lhs, u32 lo, u32 n);
    template<class T> inline algo::aryptr<u8> BytesOf(const T &t);
    template<class T> inline T &qLast(const algo::aryptr<T> &ary);

    // Taylor series expansion. For x around 0.01, this produces 2 digits per
    // iteration, so with N=4 will be reasonably fast.
    inline double ExpTaylor(double x, int n);
    inline u32 ParseNum8(u64 x, u32 &ok);
    inline u32 ParseNum4(u32 x, u32 &ok);
    inline u32 aParseNum8(strptr s, u32 &ok);
    inline u32 aParseNum4(strptr s, u32 &ok);
    inline int ParseOct1(u32 c, u8 &result);

    // C           single hex character, upper or lowercase
    // RETURN      value of hex character
    // RESULT      value of hex digit
    // RETURN      success code
    inline int ParseHex1(u32 c, u8 &result);
    inline void PageBufInit(PageBuf &F, u64 n, u64 align);
    inline u32 strptr_Hash(u32 prev, algo::strptr val);
    inline u32 cstring_Hash(u32 prev, const algo::strptr &val);
    inline u32 cstring_Hash(u32 prev, algo::cstring &val);
    inline u32 cstring_Hash(u32 prev, const algo::cstring &val);

    // this is inefficient because it would be nice to be able to overrun strings.
    // however, we don't check byte alignment at the beginning,
    // so a 2-byte string that lies 4 bytes before the end of a page could fault.
    inline u32 CRC32Step(u32 old, const u8 *x, size_t len);
    inline int P1Mod(int a, int b);
    inline u32 u32_Count1s(u32 x);
    inline u32 u8_Count1s(u8 x);
    inline u32 u16_Count1s(u16 x);
    inline u32 u64_Count1s(u64 x);
    inline u32 u128_Count1s(u128 x);
    template<class T> inline void ZeroBytes(T &t);
    template<class T, class U> inline T PtrAdd(U *ptr, int_ptr offset);
    template<class T> inline void TSwap(T &a, T &b);

    // Default, and invalid, value for Fildes is -1. 0 is a valid value (stdin)!
    inline bool ValidQ(Fildes fd);
    inline int i32_NegateIf(int x, bool i);
    inline i64 i64_NegateIf(i64 x, bool i);
    inline float float_NegateIf(float x, bool i);
    inline double double_NegateIf(double x, bool i);
    inline double DRound(double a);
    inline double DCeiling(double a);
    inline double DFloor(double a);
    inline double DTrunc(double a);

    // Clipping to an open interval (exclisive upper limit) [0..hi-1] or [lo..hi-1]
    inline int Clipped(int a, int hi);
    inline int Clipped(int a, int lo, int hi);

    // Clipping to a closed interval lo..hi
    inline double Clippedf(double a, double lo, double hi);

    // unsigned subtract with clipping
    inline u64 u64_SubClip( u64 a, u64 b);
    inline u32 u32_SubClip( u32 a, u32 b);
    inline u16 u16_SubClip( u16 a, u16 b);
    inline int Abs(i8  a);
    inline int Abs(i16 a);
    inline i32 Abs(i32 a);
    inline i64 Abs(i64 a);
    inline f32 Abs(f32 a);
    inline f64 Abs(f64 a);
    inline f80 Abs(f80 a);

    // Read big-endian word
    inline u16 ReadBE16(const void *val);
    inline u32 ReadBE32(const void *val);
    inline u64 ReadBE64(const void *val);

    // Read little-endian word
    inline u16 ReadLE16(const void *val);
    inline u32 ReadLE32(const void *val);
    inline u64 ReadLE64(const void *val);

    // Forward:  returns 0-based index of least significant bit that is set
    // Reverse:  returns 0-based index of most  significant bit that is set.
    // input argument must not be zero.
    // input result in 0 is undefined (see Intel manual)
    // http://www.intel.com/content/dam/doc/manual/64-ia-32-architectures-software-developer-vol-2a-2b-instruction-set-a-z-manual.pdf
    inline u32 u32_BitScanForward(u32 v);
    inline u64 u64_BitScanForward(u64 v);
    inline u32 u32_BitScanReverse(u32 v);
    inline u64 u64_BitScanReverse(u64 v);
    inline u32 u16_BitScanForward(u16 v);
    inline u32 u16_BitScanReverse(u16 v);
    inline u32 u8_BitScanForward(u8 v);
    inline u32 u8_BitScanReverse(u8 v);
    inline u32  CeilingLog2(u32 orig);
    inline u64  CeilingLog2(u64 orig);
    inline u32  FloorLog2(u32 i);
    inline u64  FloorLog2(u64 i);
    inline u32 BumpToPow2(u32 i);
    inline u64 BumpToPow2(u64 i);
    inline u64   u64_CeilPow2(u64 a, u64 b);
    inline u32   u32_CeilPow2(u32 a, u32 b);
    inline u64    u64_ModPow2(u64 a, u64 b);
    inline u32    u32_ModPow2(u32 a, u32 b);
    inline bool     u64_Pow2Q(u64 i);
    inline bool     u32_Pow2Q(u32 i);

    // get CPU HZ value as u64
    inline u64 get_cpu_hz_int();

    // get CPU HZ value as double
    inline double get_cpu_hz();

    // cpu_hz (untyped SchedTime)
    // use this for timestamps.
    // these calls may be pipelined and reordered, so measuring instruction
    // latency with these is not possible. for that, use rdtscp
    inline u64 get_cycles();

    // Convert scheduler time units to seconds.
    inline double ToSecs(SchedTime ticks);
    inline algo::UnDiff ToUnDiff(SchedTime ticks);
    inline algo::SchedTime ToSchedTime(double secs);
    inline algo::UnTime ToUnTime(UnixTime t);
    inline algo::UnixTime ToUnixTime(UnTime t);
    inline double ToSecs(UnDiff t);
    inline double ToSecs(UnTime t);
    inline algo::UnixTime CurrUnixTime();

    // use this for performance measurements.
    // according to Intel software manual, lfence followed by rdtsc
    // is the beez knees.
    inline u64 rdtscp();
    inline algo::UnixDiff UnixDiffHMS(i64 h, int m, int s);
    inline algo::UnDiff UnDiffSecs(double d);
    inline algo::UnDiff UnDiffMsecs(const i64 i);
    inline algo::UnDiff UnDiffUsecs(const double d);
    inline algo::UnTime UnTimeSecs(double d);
    inline algo::UnDiff UnDiffSecs(i64 i);
    inline algo::UnDiff UnDiffSecs(i32 i);
    inline algo::UnDiff UnDiffHMS(int h, int m, int s);

    // Current value of get_cycles();
    inline algo::SchedTime CurrSchedTime();

    // Elapsed time in seconds between two SchedTimes.
    inline double ElapsedSecs(algo::SchedTime start, algo::SchedTime end);
    inline algo::WDiff ToWDiff(algo::UnixDiff d);
    inline algo::WDiff ToWDiff(algo::UnDiff d);
    inline algo::UnixDiff ToUnixDiff(algo::WDiff d);
    inline algo::WTime ToWTime(algo::UnTime s);
    inline algo::UnDiff ToUnDiff(algo::WDiff d);
    inline algo::UnixTime ToUnixTime(algo::WTime nt);
    inline algo::WTime ToWTime(algo::UnixTime t);
    inline double ToSecs(algo::WDiff t);

    // Append and end-of-line sequence to string S.
    inline void eol(cstring &s);
    inline algo::strptr ToStrPtr(memptr buf);

    // Note: cstring << algo::ToLower(int) will output an *integer*, not a character
    inline int ToLower(int i);

    // Note: cstring << algo::ToUpper(int) will output an *integer*, not a character
    inline int ToUpper(int i);
    inline char ToLower(char i);
    inline char ToUpper(char i);
    inline algo::i32_Range TFind(const strptr &s, char match);
    inline algo::i32_Range TRevFind(const strptr &s, char match);
    inline algo::aryptr<char> ch_FirstN(const strptr &lhs, u32 n);
    inline algo::aryptr<char> ch_LastN(const strptr &lhs, u32 n);
    inline algo::aryptr<char> ch_RestFrom(const strptr &lhs, u32 n);
    inline algo::aryptr<char> ch_GetRegion(const strptr &lhs, u32 lo, u32 n);
    inline int ImpliedLength(char *, const char *c);
    inline int ImpliedLength(const char *, const char *c);
    inline int ch_N(const strptr &s);
    inline int ch_First(const strptr &s, int dflt = 0);
    inline int ch_Last(const strptr &s, int dflt = 0);
    inline int ch_N(const tempstr &str);
    inline int range_N(const i32_Range &rhs);
    inline algo::aryptr<u8> strptr_ToMemptr(algo::aryptr<char> rhs);
    inline algo::aryptr<char> memptr_ToStrptr(algo::aryptr<u8> rhs);

    // if next character matches WHAT, skip and return true
    // otherwise return false
    inline bool SkipChar(StringIter &iter, char what);

    // Convert string to numeric type.
    // If there is an error, or string is empty, return DFLT.
    // If there is junk at the end of the string, silently ignore it.
    // Use StringIter version to check for junk, or use -X version to throw exception.
    inline int ParseI32(strptr str, int dflt);
    inline u32 ParseU32(strptr str, u32 dflt);
    inline i64 ParseI64(strptr str, i64 dflt);
    inline u64 ParseU64(strptr str, u64 dflt);
    inline algo::Attr_curs &Attr_curs_Access(Attr_curs &curs);
    inline bool Attr_curs_ValidQ(Attr_curs &curs);
    inline bool Line_curs_ValidQ(Line_curs &curs);
    inline strptr &Line_curs_Access(Line_curs &curs);
    inline bool Word_curs_ValidQ(Word_curs &curs);
    inline strptr &Word_curs_Access(Word_curs &curs);
    template<class T> inline void Init_Set(StringDesc &desc, void(*fcn)(T&));
    template<class T> inline void SetnumMaybe_Set(StringDesc &desc, bool(*fcn)(T&, i64 num));
    template<class T> inline void Geti64_Set(StringDesc &desc, i64(*fcn)(T&, bool &));
    template<class T> inline void Getary_Set(StringDesc &desc, algo::aryptr<char>(*fcn)(T&));
    inline strptr &FileLine_curs_Access(algo::FileLine_curs &curs);
    inline bool FileLine_curs_ValidQ(algo::FileLine_curs &curs);
    inline bool Sep_curs_ValidQ(algo::Sep_curs &curs);
    inline strptr &Sep_curs_Access(algo::Sep_curs &curs);
    inline u32 u16_FmtBuf(u32 value, u8 *buf);
    inline algo::cstring &operator<<(algo::cstring &out, const char* t);
    inline algo::cstring &operator<<(algo::cstring &out, const ListSep &t);
    inline algo::cstring &operator<<(algo::cstring &out, const strptr &t);
    inline algo::cstring &operator<<(algo::cstring &out, void (*t)(algo::cstring &));

    // SameSign returns false if one of the arguments is zero.
    template<typename T> inline bool SameSignQ(T a, T b);

    // Compare two strings for equality, case-sensitively
    inline bool strptr_Eq(algo::strptr a, algo::strptr b);

    // Test if string A is lexicographically less than string B.
    inline bool strptr_Lt(algo::strptr a, algo::strptr b);

    // reverse bit order in a byte
    inline u8 u8_ReverseBits(u8 b);

    // Return unix-epoch time with nanosecond resolution
    // On Linux, this function calls clock_gettime() which takes about
    // 30ns and uses rdtsc() to increase underlying clock resolution
    inline algo::UnTime CurrUnTime();
}

// -----------------------------------------------------------------------------

// Keyval printing support. Usage:
// tempstr() << Keyval("a", something printable).
// - insert double space if necessary
// - if key is empty, only value is printed (i.e. anomymous arg)
// - both key and value are ssim-quoted (i.e. c++ quoting rules but outer quotes can be single or double)
// - reasonably fast.
template<class Val> struct Keyval_ {
    const algo::strptr &key;
    Val &val;
    Keyval_(const algo::strptr &a, const Val &b) :key(a),val((Val&)b) {
    }
    Keyval_(const Keyval_ &rhs) : key(rhs.key), val((Val&)rhs.val) {
    }
private:
    void operator =(const Keyval_ &) {
    }
};

template<class Val> inline algo::cstring &operator <<(algo::cstring &lhs, const Keyval_<Val> &kv) {
    int start=algo::BeginKeyval(lhs,kv.key);
    lhs<<kv.val;
    return algo::EndKeyval(lhs,start);
}

template<class Val> Keyval_<Val> inline Keyval(const algo::strptr &key, const Val &val) {
    return Keyval_<Val>(key,val);
}

// -----------------------------------------------------------------------------

namespace algo_lib { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/lib/algo/cpu_hz.cpp -- Grab cpu_hz from OS
    //
    void InitCpuHz();

    // -------------------------------------------------------------------
    // cpp/lib/algo/crc32.cpp -- Software-based CRC32
    //

    // This is a low-quality random number generator suitable for simple tasks...
    // Set seed for srng state
    void srng_SetSeed(algo_lib::Srng &srng, int z, int w);

    // retrieve random u32
    u32 srng_GetU32(algo_lib::Srng &srng);

    // retrieve random double in 0..1 range
    double srng_GetDouble(algo_lib::Srng &srng);

    // -------------------------------------------------------------------
    // cpp/lib/algo/errtext.cpp
    //
    void SaveBadField(strptr name, strptr value);

    // Reset value of algo_lib::_db.errtext and return it for further editing
    // Usage:
    // algo_lib::ResetBadTags() << ...errors...
    algo::cstring &ResetErrtext();

    // Add key-value pair to algo_lib::_db.errtext
    // Error text beyond a reasonable limit is discarded -- keep errors short!
    void SaveBadTag(const strptr &name, const strptr &value);

    // Better name than SaveBadTAg
    void AppendErrtext(const strptr &name, const strptr &value);

    // Retrieve whatever bad tags were saved with SaveBadTag,
    // and clear the state.
    // SaveBadTag is typically called by string read functions that encounter
    // something unreadable. This is the only way to retrieve that
    // additional information
    tempstr DetachBadTags();

    // Increment algo_lib::_db.trace.tot_insert_err
    // And print accumulated 'bad tags' using prerr.
    // if SetShowInsertErrLim was previously called.
    // Note: printing is disabled by default, and must be explicitly enabled.
    void NoteInsertErr(strptr tuple);

    // Specify how many times NoteInsertErr can use prerr
    // to output.
    void SetShowInsertErrLim(int n);

    // -------------------------------------------------------------------
    // cpp/lib/algo/file.cpp -- File functions
    //
    void fildes_Cleanup(algo_lib::FLockfile &lockfile);

    // If PATH is an existing path, leave it unchanged
    // On Windows, If PATH.EXE is an existing path, return that
    // Return true if file exists
    bool TryExeSuffix(algo::cstring &path);

    // Update FNAME to be a filename that can be passed to Unix exec call.
    // If FNAME is an absolute path, don't perform a search
    // If FNAME is a relative path, perform a search using the PATH environment
    // the first executable file that's found is the result.
    void ResolveExecFname(algo::cstring &fname);

    // -------------------------------------------------------------------
    // cpp/lib/algo/fmt.cpp -- Print to string / Read from string
    //
    void ErrorX_Print(algo_lib::ErrorX &row, algo::cstring &str);
    void FTxttbl_Print(algo_lib::FTxttbl &T_, algo::cstring &str);

    // -------------------------------------------------------------------
    // cpp/lib/algo/iohook.cpp
    //
    void IohookInit();

    // Register IOHOOK to be called whenever an IO operation is possible.
    // OK to add an fd twice with different flags. Subsequent calls override previous ones.
    // Add iohook to epoll in read, write or read/write mode
    // Optionally, add as edge triggered
    void IohookAdd(algo_lib::FIohook& iohook, algo::IOEvtFlags inflags) __attribute__((nothrow));

    // De-register interest in iohook
    void IohookRemove(algo_lib::FIohook& iohook) __attribute__((nothrow));

    // give up unused time to the OS.
    // Compute number of clocks to sleep before next scheduling cycle
    // If there was no sleep on the previous cycle, the sleep is zero.
    // This last bit is important because it prevents deadlocks
    // when one step implicitly creates work for another step
    // that occurs before it in the main loop.
    // Sleep will not extend beyond algo_lib::_db.limit
    //     (user-implemented function, prototype is in amc-generated header)
    // void giveup_time_Step();

    // -------------------------------------------------------------------
    // cpp/lib/algo/lib.cpp -- Main file
    //

    // Set exit time of main loop to current time.
    void ReqExitMainLoop();
    void fd_Cleanup(algo_lib::FFildes &fildes);
    void fildes_Cleanup(algo_lib::FIohook &iohook);
    //     (user-implemented function, prototype is in amc-generated header)
    // void bh_timehook_Step();

    // Check signature on incoming data
    bool dispsigcheck_InputMaybe(dmmeta::Dispsigcheck &dispsigcheck);

    // Die when parent process dies
    void DieWithParent();

    // Create temporary file
    // tempfile.fildes points to the new temp file after this
    // temp file is created under temp/
    // prefix is a namespace-unique name, such as "amc.xyz"
    // Actual file that's created becomes "temp/amc.xyz.XXXXXX"
    // Computed filename is saved to tempfile.filename
    void TempfileInitX(algo_lib::FTempfile &tempfile, strptr prefix);
    void fildes_Cleanup(algo_lib::FTempfile &tempfile);

    // Interpret redirect string, return resulting fd
    // If no redirect applies, return -1
    // If a valid fd is returned, it is unique and may be closd with close()
    // Supported redirects:
    // >filename  -- on exit, dst_fd is writing to a file
    // <filename  -- on exit, dst_fd is reading from a file
    // >>filename -- on exit, dst_fd is appending to a file
    // >&fd       -- on exit, dst_fd is pointing to fd
    // <&fd       -- on exit, dst_fd is pointing to fd
    // This function could be called openex
    int CreateRedirect(strptr redirect);

    // Interpret redirect string and make DST_FD consistent with
    // the intended state. Return 0 on success, -1 on failure
    // This function is usually called in the child process right after fork
    // See CreateRedirect for interpretation of redirect string
    int ApplyRedirect(strptr redirect, int dst_fd);
    bool IpmaskValidQ(const strptr ipmask);

    // Return TRUE if current user is root.
    // #AL# is this more correct than checking that effective uid is 0?
    bool RootQ();

    // Return name of user executing this command.
    // If the command is being executed under sudo, return name
    // of original user.
    tempstr EffectiveUser();

    // if OWN_FD is cleared, clean up file descriptor before it is closed
    void file_Cleanup(algo_lib::InTextFile &file);

    // Walk child process tree for parent process pid, in post-order traversal way,
    // and send signal sig to each process. Kill_topmost is an option whether
    // to send signal to parent process itself. Return value - number of processes
    // to whose the signal has been actually sent.
    // Does not throw exceptions, just prints error message if kill() fails.
    // Linux only.
    int KillRecurse(int pid, int sig, bool kill_topmost);

    // Return computed name for sandbox SANDBOX
    tempstr SandboxDir(algo::strptr sandbox);

    // Enter sandbox directory remember previous directory
    void PushDir(algo::strptr dir);

    // Change to the directory that was current before sandbox mode
    // Must be balanced with PushDir
    void PopDir();

    // -------------------------------------------------------------------
    // cpp/lib/algo/line.cpp -- Line processing
    //

    // Read next line from FILE into RESULT
    // When end of file is reached, return FALSE.
    // If error occurs during reading, return false.
    // Typical usage:
    // InTextFile file;
    // file.file = OpenRead(filename);
    // for (strptr line; ReadLine(file,line); ) {
    // <do something with LINE>
    // }
    // A partial line at the end of the file is returned.
    bool ReadLine(algo_lib::InTextFile &file, algo::strptr &result);

    // -------------------------------------------------------------------
    // cpp/lib/algo/lockfile.cpp -- Lock file
    //

    // Attempt to lock the file named NAME, updating state of LOCKFILE.
    // Return TRUE if lock is successfully applied.
    // If FAIL_OK is false, an error causes an exception (function always returns true)
    // If an exception is thrown, the text contains error description.
    // Otherwise, algo_lib::_db.errtext (DetachBadTags()) contains human-readable description of error.
    // If WAIT_TIMEOUT is non-zero, block up to WAIT_TIMEOUT seconds before failing
    // Write pid to file specified in NAME, and lock file using flock().
    bool LockFileInit(algo_lib::FLockfile &lockfile, strptr name, algo::FailokQ fail_ok, algo::UnDiff wait_timeout);

    // Write pid to lockfile, separate function to update pid after fork().
    // Sets error text in case of error, and return false.
    bool WritePid(algo_lib::FLockfile &lockfile);

    // Non-blocking attempt to lock LOCKFILE
    // Return success status
    // If FAIL_OK is FALSE, throw exception on error (must succeed)
    bool LockFileInit(algo_lib::FLockfile &lockfile, strptr name, algo::FailokQ fail_ok);

    // Non-blocking attempt to lock LOCKFILE.
    // Throw exception on error.
    // If the function returns, it has succeeded.
    void LockFileInit(algo_lib::FLockfile &lockfile, strptr name);

    // Read contents of lock file FNAME, extracting the pid that's stored in there.
    // On failure, return 0
    pid_t LockFileRead(strptr fname);

    // -------------------------------------------------------------------
    // cpp/lib/algo/mmap.cpp -- Mmap wrapper
    //

    // User-defined cleanup function for MMAP.MEM
    void mem_Cleanup(algo_lib::Mmap &mmap);

    // Attach mmapfile MMAPFILE to FD.
    // Return success code.
    bool MmapFile_LoadFd(MmapFile &mmapfile, algo::Fildes fd);

    // Attach mmapfile MMAPFILE to FNAME
    // Return success code.
    bool MmapFile_Load(MmapFile &mmapfile, strptr fname);

    // -------------------------------------------------------------------
    // cpp/lib/algo/regx.cpp -- Sql Regx implementation
    //

    // there is not enough information in a regx expression to fully specify it.
    // sql vs shell vs classic regx, vs acr, partial vs full.
    // we print back the original expression that was read in, but the information
    // about what function read it is lost.
    void Regx_Print(algo_lib::Regx &regx, algo::cstring &lhs);

    // Check if REGX matches S, return result
    bool Regx_Match(algo_lib::Regx &regx, algo::strptr text);

    // Parse string INPUT as regex and store in REGX.
    // Supported features:
    // \n,\r,\t,\a,\e,\v,\f, [..], $, ^,
    // ., *, +, (..), {..}, \d,\D,\w,\W,\s,\S
    void Regx_ReadDflt(algo_lib::Regx &regx, algo::strptr input);

    // Parse bash-style regx:
    // * -> .*
    // ? -> .?
    // All other regx chars are escaped away
    // if FULL is set to false, input is treated as ".*input.*"
    void Regx_ReadShell(algo_lib::Regx &regx, algo::strptr input, bool full);

    // Parse SQL-style regx:
    // % is rewritten as .*
    // _ is rewritten as .
    // (, ), [, ] are passed through
    // ., *, ?, + are escaped
    // if FULL is set to false, input is treated as ".*input.*"
    void Regx_ReadSql(algo_lib::Regx &regx, algo::strptr input, bool full);

    // Parse ACR-style regx:
    // % is rewritten as .*
    // (, ), [, ], _ are passed through
    // ., *, ?, + are escaped
    // if FULL is set to false, input is treated as ".*input.*"
    // If the input expression can be matched as a string, set REGX.LITERAL to true
    void Regx_ReadAcr(algo_lib::Regx &regx, algo::strptr input, bool full);

    // Set REGX to match string INPUT literally
    void Regx_ReadLiteral(algo_lib::Regx &regx, algo::strptr input);
    bool Regx_ReadStrptrMaybe(algo_lib::Regx &regx, algo::strptr input);

    // -------------------------------------------------------------------
    // cpp/lib/algo/string.cpp -- cstring functions
    //
    void ind_replvar_Cleanup(algo_lib::Replscope &replscope);

    // Set value of key KEY value VALUE
    // KEY        string to replace
    // VALUE      value to replace it with
    // SUBST      If set, $-expand the VALUE parameter
    void Set(algo_lib::Replscope &scope, strptr from, strptr to, bool subst = true);

    // Append TEXT to OUT, performing $-substitution using variables from SCOPE (must be non-NULL)
    // if EOL is set, then new line is appended at the end.
    void Ins(algo_lib::Replscope *scope, algo::cstring &out, strptr text, bool eol = true);

    // Enable comma-eating (default true)
    void eatcomma_Set(algo_lib::Replscope &scope, bool enable);

    // Enable strict mode (default true -- any failed substitution kills process)
    // If strict mode is off, failed substitution acts as if there was no substitution
    void fatal_Set(algo_lib::Replscope &scope, bool enable);

    // Perform $-substitutions in TEXT and return new value.
    tempstr Subst(algo_lib::Replscope &scope, strptr text);

    // read TEXT into a tuple.
    // perform variable substitution on key-value pairs.
    // write tuple back.
    // return resulting string
    tempstr Tuple_Subst(algo_lib::Replscope &R, strptr text);
    void RunCsvParse(algo_lib::CsvParse &csvparse);

    // -------------------------------------------------------------------
    // cpp/lib/algo/timehook.cpp
    //

    // Initialize time hook TH as non-recurrent, with delay DELAY.
    // Usage:
    // ThInit(th, SchedTime());     // schedule at current time
    // hook_Set0(th, myfunction);   // set callback
    // bh_timehook_Reheap(th);      // insert into timehook heap
    // ... eventually algo_lib::Step() will call the hook
    void ThInit(algo_lib::FTimehook& th, algo::SchedTime delay) __attribute__((nothrow));

    // Similar to the above, but recurrent.
    void ThInitRecur(algo_lib::FTimehook& th, algo::SchedTime delay) __attribute__((nothrow));

    // -------------------------------------------------------------------
    // cpp/lib/algo/txttbl.cpp -- Ascii table
    //

    // Add new cell to the given row.
    algo_lib::FTxtcell &AddCell(algo_lib::FTxtrow &txtrow, algo::strptr title, algo_TextJustEnum justify, int span);
    algo_lib::FTxtcell &AddCell(algo_lib::FTxtrow &txtrow, algo::strptr title, algo_TextJustEnum justify);

    // Add new cell to the last row of of table and return references to its text
    algo_lib::FTxtrow &AddRow(algo_lib::FTxttbl &txttbl);

    // Add column to the last row of table and return reference to it
    algo_lib::FTxtcell &AddCellEx(algo_lib::FTxttbl &txttbl);

    // Text table.
    // Text table is a table with rows; Each row has an array of cells.
    // Each call has text, col span, text justification, and optional style (color).
    // Add column to the last row of table.
    void AddCol(algo_lib::FTxttbl &txttbl, algo::strptr col, algo_TextJustEnum justify);
    void AddCol(algo_lib::FTxttbl &txttbl, algo::strptr col);
    algo::cstring &AddCell(algo_lib::FTxttbl &txttbl);

    // Use prlog(txttbl) to print.
    void TxttblCsv(algo_lib::FTxttbl &tbl);
}


using algo::Attr;
using algo::Bool;
using algo::BumpToPow2;
using algo::Charset;
using algo::DCeiling;
using algo::DryrunQ;
using algo::EchoQ;
using algo::Errcode;
using algo::FailokQ;
using algo::FatalErrorExit;
using algo::Fildes;
using algo::Fill;
using algo::FindStr;
using algo::GetWeekdayName;
using algo::GetaryFcn;
using algo::Geti64Fcn;
using algo::IOEvtFlags;
using algo::InitFcn;
using algo::LineBuf;
using algo::Line_curs;
using algo::LspaceStr4;
using algo::MSECS_PER_DAY;
using algo::MaybeDirSep;
using algo::MaybeSpace;
using algo::Pathcomp;
using algo::SECS_PER_DAY;
using algo::SECS_PER_HOUR;
using algo::SECS_PER_MIN;
using algo::SetnumFcn;
using algo::SsimFname;
using algo::StringDesc;
using algo::SysCmd;
using algo::TimeStruct;
using algo::Tuple;
using algo::URL;
using algo::Word_curs;
using algo::aParseNum16;
using algo::aParseNum4;
using algo::aParseNum8;
using algo::cstring;
using algo::double_WeakRandom;
using algo::eol;
using algo::i32_WeakRandom;
using algo::strptr_Hash;
using algo::tempstr;

#include "include/gen/report_gen.h"
#include "include/gen/ietf_gen.h"
#include "include/win32.h" // windows compatibility api

// INLINE FILE SECTION

#include "include/gen/_gen.inl.h"
#include "include/algo.inl.h"
#include "include/gen/report_gen.inl.h"
#include "include/gen/ietf_gen.inl.h"
#include "include/gen/algo_gen.inl.h"
#include "include/gen/algo_lib_gen.inl.h"

// presently cannot be avoided
#include "include/lib_json.h"

// Gcache pragma to precompile this header
void __gcache_pragma_pch_preprocess();

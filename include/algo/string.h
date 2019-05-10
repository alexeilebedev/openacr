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
// Header: include/algo/string.h -- String functions
//
// Created By: alexei.lebedev hayk.mkrtchyan
// Recent Changes: alexei.lebedev hayk.mkrtchyan
//

#pragma once

namespace algo {
    int ch_First(const strptr &s, int dflt=0);    // Get first character
    int ch_Last(const strptr &s, int dflt=0);    // Get last character

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
        ListSep(strptr sep_ = ", ") : sep(sep_), iter(0) {}
    };

    int CompareNoCase(strptr lhs, strptr rhs)               __attribute__((nothrow));
    bool StartsWithQ(strptr lhs, strptr sstr, bool case_sens=true)  __attribute__((nothrow));

    // -----------------------------------------------------------------------------

    int            FindStr(strptr lhs, strptr t, bool case_sens = true)     __attribute__((nothrow));
    int           FindFrom(strptr lhs, strptr t, int from, bool case_sens=true) __attribute__((nothrow));

    // Replace all occurences of FROM with TO in STR.
    // PRESERVE_CASE
    // CASE_SENSITIVE
    int Replace(cstring &str, const strptr& from, const strptr& to
                , bool case_sensitive = true
                , bool preserve_case = true);

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
        Line_curs(): eof(true),i(-1){}// mostly for coverity
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
        Word_curs() : index(0){}
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
}


namespace algo { // update-hdr srcfile:'(%/algo/string.%|%/algo/line.%)'
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

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

    // Append NUL character to the end of the string and return a
    // pointer to the C string.
    // There is no guarantee that there are no NUL characters in the middle
    // of STR, that is up to the user.
    // cstring is normally NOT null terminated (it is length-based),
    // so this function is necessary when passing strings to C library functions
    // or the OS
    const char *Zeroterm(cstring &rhs);

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
    int FindFrom(strptr s, char c, int from);

    // Search for character/string from left to right
    // If found, return index where match occurs.
    // If not found, return -1
    int FindChar(strptr lhs, char c);
    int FindStr(strptr lhs, strptr t, bool case_sensitive);
    int CompareNoCase(strptr lhs, strptr s);

    // Check strings for equality, optionally in a case-insensitive way
    bool StrEqual(strptr a, strptr b, bool case_sens);
    bool StartsWithQ(strptr s, strptr sstr, bool case_sensitive);
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
    int UnescapeC(u32 str, int len, u8 &result);
    int Replace(cstring &str, const strptr& from, const strptr& to, bool case_sensitive, bool preserve_case);

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
    strptr GetTokenChar(StringIter &S, char sep);

    // Skip leading characters matching SEP
    // Return run of characters up to next matching SEP, or EOF.
    // Do not skip trailing separators.
    strptr GetWordCharf(StringIter &iter, bool (*sep)(u32));

    // Skip leading whitespace characters
    // Return run of characters up to next whitespace, or EOF.
    // Do not skip trailing whitespace
    strptr GetWordCharf(StringIter &iter);

    // Skip any leading whitespace in STR.
    // Read and return next word.
    strptr GetTypeTag(strptr str);
    bool TryParseI32(algo::StringIter &iter, i32 &result);
    bool TryParseI64(algo::StringIter &iter, i64 &result);
    bool TryParseU32(algo::StringIter &iter, u32 &result);
    bool TryParseU64(algo::StringIter &iter, u64 &result);

    // Read a series of digits and return resulting number.
    // Return success code
    bool TryParseDigits(StringIter &S, double &result);

    // TODO: document these more carefully
    // Read a series of digits N, returning N / pow(10, length(N))
    // If successful, advance index. Otherwise, leave index where it was.
    bool TryParseFraction(StringIter &S, double &result);
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
    i32_Range substr_FindFirst(const aryptr<char> &s, const aryptr<char> &match);

    // Same as above but search right-to-left.
    // In case of failure, return range (0,0) -- empty range positioned at start of S.
    i32_Range substr_FindLast(const aryptr<char> &s, const aryptr<char> &match);

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

    // Append a directory separator to string STR unless
    // STR already ends in one.
    // Example:
    // str << dirname << MaybeDirSep << filename.
    void MaybeDirSep(cstring &str);
    i32 strptr_Cmp(algo::strptr a, algo::strptr b);
    bool strptr_Lt(algo::strptr a, algo::strptr b);

    // Compare two strings for equality, case-sensitively
    bool strptr_Eq(algo::strptr a, algo::strptr b);
    void Attr_curs_Reset(Attr_curs &curs, strptr line);
    void Attr_curs_Next(Attr_curs &curs);

    // Compare two strings whose start address is 16-byte aligned
    // Do not use this function --
    bool AlignedEqual(strptr a, strptr b);

    // insert TEXT into OUT, indenting as necessary;
    // Initial indentation is INDENT, it's adjusted as necessary as { and } are found
    // in the TEXT.
    // Each indent is 4 spaces.
    void InsertIndent(algo::cstring &out, strptr text, int indent);

    // -------------------------------------------------------------------
    // include/algo/string.inl.h
    //
    inline void eol(cstring &s);
    inline algo::strptr ToStrPtr(memptr buf);
    inline char ToLower(char i);
    inline char ToUpper(char i);
    inline i32_Range TFind(const strptr s, char match);
    inline i32_Range TRevFind(const strptr s, char match);
    inline aryptr<char> ch_FirstN(const cstring &lhs, u32 n);
    inline aryptr<char> ch_LastN(const cstring &lhs, u32 n);
    inline aryptr<char> ch_RestFrom(const cstring &lhs, u32 n);
    inline aryptr<char> ch_GetRegion(const cstring &lhs, u32 lo, u32 n);
    inline int ImpliedLength(char *, const char *c);
    inline int ImpliedLength(const char *, const char *c);
    inline int ch_N(const strptr &s);
    inline int ch_First(const strptr &s, int dflt);
    inline int ch_Last(const strptr &s, int dflt);
    inline int ch_N(const tempstr &str);
    inline int range_N(const i32_Range &rhs);
    inline aryptr<u8> strptr_ToMemptr(aryptr<char> rhs);
    inline aryptr<char> memptr_ToStrptr(aryptr<u8> rhs);

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
}

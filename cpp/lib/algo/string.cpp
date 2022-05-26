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
// Source: cpp/lib/algo/string.cpp -- cstring functions
//
// Created By: alexei.lebedev jeffrey.wang
// Authors: alexei.lebedev
// Recent Changes: alexei.lebedev hayk.mkrtchyan jeffrey.wang
//

#include "include/algo.h"

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
strptr algo::Pathcomp(strptr s, strptr expr) {
    int start  = 0;
    int end    = s.n_elems;
    for (int i=0; i <= elems_N(expr)-3; i+=3) {
        char c     = expr[i];
        int idx;
        bool match = false;
        switch ((expr[i+1]=='R')*2 + (expr[i+2]=='R')) {
        case 0: // LL
            for (idx=start; idx<end && !match; idx++) { match=s[idx]==c; }
            end = idx - match;
            break;
        case 1: // LR
            for (idx=start; idx<end && !match; idx++) { match=s[idx]==c; }
            start = idx;
            break;
        case 2: // RL
            for (idx=end; idx>start && !match; idx--) { match=s[idx-1]==c; }
            end = idx;
            break;
        case 3: // RR
            for (idx=end; idx>start && !match; idx--) { match=s[idx-1]==c; }
            start = idx + match;
            break;
        }
    }
    return strptr(s.elems + start, end-start);
}

// -----------------------------------------------------------------------------

// Append NUL character to the end of the string and return a
// pointer to the C string.
// There is no guarantee that there are no NUL characters in the middle
// of STR, that is up to the user.
// cstring is normally NOT null terminated (it is length-based),
// so this function is necessary when passing strings to C library functions
// or the OS
char *algo::Zeroterm(cstring &rhs) {
    ch_Reserve(rhs, 1);
    rhs.ch_elems[rhs.ch_n]=0;
    return rhs.ch_elems;
}

// Same thing but with tempstr.
// Typical usage is to take a strptr
// expression and pass it to some unix call
// some_unix_call(Zeroterm(tempstr(some_strptr)))
//
char *algo::Zeroterm(const tempstr &rhs) {
    return algo::Zeroterm(static_cast<cstring&>(const_cast<tempstr&>(rhs)));
}

// -----------------------------------------------------------------------------

// Construct ssim filename from key
// SsimFname("data", "abcd") -> data/abcd.ssim
// SsimFname("data/", "abcd.ef") -> data/abcd/ef.ssim
tempstr algo::SsimFname(strptr root, strptr rel) {
    tempstr s;
    s << rel;
    Replace(s, ".", "/");

    tempstr ret;
    ret << root;
    ret << MaybeDirSep;
    ret << s;
    ret << ".ssim";
    return ret;
}

// -----------------------------------------------------------------------------

// Extract Namespace name from "ns.name" format.
// This is equivalent to Pathcomp(s,".LL")
strptr algo::GetNs(strptr s) {
    i32_Range R = TFind(s, '.');
    strptr ret = FirstN(s, R.beg);
    if (R.end == R.beg) ret = strptr();
    return ret;
}

// -----------------------------------------------------------------------------

// if IDENT starts with NS_NAME., return ident without prefix
//  otherwise return ident
// if NS_NAME is empty, removes any dotted prefix from IDENT.
// NS_NAME      name of 'current' namespace (optional)
// IDENT        identifier in question
//
// StripNs("abc", "abc.def") -> "def"
// StripNs(""   , "abc.def") -> "def"
// StripNs("abc", "def.ghi") -> "def.ghi"
strptr algo::StripNs(strptr ns_name, strptr ident) {
    i32_Range R = TRevFind(ident, '.');
    if (!elems_N(ns_name) || FirstN(ident, R.beg) == ns_name) {
        ident = RestFrom(ident, R.end);
    }
    return ident;
}

// -----------------------------------------------------------------------------

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
strptr algo::SubstringIndex(strptr str, char c, int idx) {
    int start = 0;
    int end = elems_N(str);
    if (idx < 0) {
        int j = end;
        while (j > 0) {
            idx += str[j-1] == c;
            if (idx==0) break;
            j--;
        }
        start = j;
    } else {
        int j = 0;
        while (j < end) {
            idx -= str[j] == c;
            if (idx==0) break;
            j++;
        }
        end = j;
    }
    return strptr(str.elems + start, end - start);
}

// -----------------------------------------------------------------------------

static int NoCaseStrCmp(const char *A ,const char *B, size_t count) {
    int f,l;
    if ( count ){
        do {
            f = *A++;
            if (f >= 'A' && f <= 'Z') {
                f -= 'A' - 'a';
            }
            l = *B++;
            if (l >= 'A' && l <= 'Z') {
                l -= 'A' - 'a';
            }
        } while (--count && f && (f == l));
        return( f - l );
    }
    return( 0 );
}

int algo::FindFrom(strptr s, strptr t, int from, bool case_sensitive) {
    u32 n = elems_N(t);
    if (n > (u32)s.n_elems || !n) {
        return -1;
    }
    if (case_sensitive) {
        for (int i=from, lim=s.n_elems-n+1; i<lim; i++) {
            if (strncmp(s.elems+i, t.elems, n)==0) {
                return i;
            }
        }
    } else {
        for (int i=from, lim=s.n_elems-n+1; i<lim; i++) {
            if (CompareNoCase(strptr(s.elems+i,n), strptr(t.elems, n))==0) {
                return i;
            }
        }
    }
    return -1;
}

int algo::FindFrom(strptr s, char c, int from) {
    for (int i=from; i < (int)s.n_elems; i++) if (s.elems[i]==c) return i;
    return -1;
}

// Search for character/string from left to right
// If found, return index where match occurs.
// If not found, return -1
int algo::FindChar(strptr lhs, char c) {
    return FindFrom(lhs, c,    0);
}

int algo::FindStr(strptr lhs, strptr t, bool case_sensitive) {
    return FindFrom(lhs, t,    0, case_sensitive);
}


static inline int GetCharNum(strptr s, int &i) {
    int c = s[i];
    if (c >= 'A' && c<= 'Z') {
        i++;
        return c + int('a') - 'A';
    }
    if (unsigned(c-'0')<10) {
        c=c-'0';
        i++;
        while (i<elems_N(s) && unsigned(s[i]-'0')<10) {
            c=c*10+s[i]-'0';
            i++;
        }
    }
    i++;
    return c;
}

int algo::CompareNoCase(strptr lhs, strptr s) {
    if (int result=NoCaseStrCmp(lhs.elems,s.elems,i32_Min(lhs.n_elems,s.n_elems))) {
        return result;
    }
    return lhs.n_elems-s.n_elems;
}

// Check strings for equality, optionally in a case-insensitive way
bool algo::StrEqual(strptr a, strptr b, bool case_sens) {
    if (case_sens) {
        return strptr_Eq(a,b);
    } else {
        if (a.n_elems!=b.n_elems) {
            return false;
        }
        return NoCaseStrCmp(a.elems,b.elems,a.n_elems)==0;
    }
}

bool algo::StartsWithQ(strptr s, strptr sstr, bool case_sensitive) {
    return StrEqual(FirstN(s,elems_N(sstr)), sstr, case_sensitive);
}

bool algo::EndsWithQ(strptr s, strptr sstr) {
    return LastN(s,elems_N(sstr)) == sstr;
}

void algo::MakeLower(strptr s) {
    frep_(i,elems_N(s)) {
        s[i]=ToLower(s[i]);
    }
}

void algo::MakeUpper(strptr s) {
    frep_(i,elems_N(s)) {
        s[i]=ToUpper(s[i]);
    }
}

// -----------------------------------------------------------------------------
// two-pass: compute column widths on the first pass, print on the second
// if no separator found on a line, print line as-is.

static void TabulateLine(algo_lib::Tabulate &frame,strptr line, strptr sep, strptr fmt, int colspace, int pass) {
    int start = 0;
    int pos   = 0;
    int col   = 0;
    do {
        pos = FindFrom(line,sep,pos,true);
        if (start == 0 && pos == -1) {
            break;// no separators found.
        }
        // this is a hint that Find should return elems_N(line)
        // and not -1.
        if (pos==-1) {
            pos=elems_N(line);
        }
        int width = pos - start;
        if (pass == 0) {
            if (col+1 > width_N(frame)) {
                Fill(width_AllocN(frame, col+1-width_N(frame)), 0);
            }
            width_qFind(frame, col) = i32_Max(width_qFind(frame, col), width);
        } else {
            char s    = fmt[i32_Min(col, elems_N(fmt)-1)];
            algo_TextJustEnum just(s=='r' ? algo_TextJust_j_right
                                   : s=='c' ? algo_TextJust_j_center : algo_TextJust_j_left);
            algo::strptr_PrintAligned(qGetRegion(line, start, pos-start), frame.temp, width_qFind(frame, col), just, ' ');
            char_PrintNTimes(' ', frame.temp,  colspace);
        }
        col++;
        pos   += elems_N(sep);
        start  = pos;
    } while (pos < elems_N(line));
    if (pass == 1) {
        frame.temp << RestFrom(line, start);
        // trim whitespace at the end of last column
        // (but don't merge two lines, so test is for ' ' not WhiteCharQ)
        while (ch_N(frame.temp) && ch_qLast(frame.temp) == ' ') {
            frame.temp.ch_n--;
        }
        frame.temp << eol;
    }
}

// -----------------------------------------------------------------------------

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
//       aa  cc  bb
// asdfasdf      yy
tempstr algo::Tabulated(strptr in, strptr sep, strptr fmt, int colspace) {
    algo_lib::Tabulate frame;
    rep_(pass,2) {
        algo::StringIter iter(in);
        while (!iter.EofQ()) {
            strptr line  = GetLine(iter);
            TabulateLine(frame,line,sep,fmt,colspace,pass);
        }
    }
    tempstr ret;
    ret << frame.temp;
    return ret;
}

// -----------------------------------------------------------------------------

tempstr algo::Tabulated(strptr in, strptr sep) {
    return algo::Tabulated(in,sep,"l",2);
}

// -----------------------------------------------------------------------------

int algo::UnescapeC(u32 str, int len, u8 &result) {
    u32 lo = (str & 0xff);
    u32 i;
    switch(lo) {
    case 'a'  : result = '\a'; break;
    case 'b'  : result = '\b'; break;
    case 'f'  : result = '\f'; break;
    case 'n'  : result = '\n'; break;
    case 'r'  : result = '\r'; break;
    case 't'  : result = '\t'; break;
    case 'v'  : result = '\v'; break;
    case '\'' : result = '\''; break;
    case '"'  : result = '"';  break;
    case '\\' : result = '\\'; break;
    case '?'  : result = '?';  break;
    case 'X'  :
    case 'x'  : i = ParseHex2(str>>8,len-1,result); return i ? i+1 : 0;
    case '0'  :
    case '1'  :
    case '2'  :
    case '3'  :
    case '4'  :
    case '5'  :
    case '6'  :
    case '7'  : return ParseOct3(str,len,result);
    default   : result = (u8)lo;
    }
    return 1;
}

// -----------------------------------------------------------------------------

int algo::Replace(cstring &str, const strptr& from, const strptr& to, bool case_sensitive, bool preserve_case) {
    if (!elems_N(from)) {
        return 0;
    }
    cstring out;
    int base=0;
    int nrep=0;
    int idx;
    if (case_sensitive || !elems_N(to)) {
        preserve_case=false;
    }
    while ((idx = FindFrom(str,from,base,case_sensitive))!=-1) {
        nrep++;
        out << ch_GetRegion(str,base,idx-base);
        if (preserve_case) {
            strptr_PrintCopyCase(ch_GetRegion(str,idx,elems_N(from)), out, to);
        } else {
            out << to;
        }
        base=idx + elems_N(from);
    }
    out << ch_RestFrom(str,base);
    TSwap(str,out);
    return nrep;
}

// -----------------------------------------------------------------------------

// Scan S. For any character in S that occurs in string FROM, replace with corresponding
// character from string TO.
// Example:
// Translate("aabcd", "bd", "xy") -> "aaxcy"
void algo::Translate(strptr s, strptr from, strptr to) {
    from = FirstN(from,u32_Min(from.n_elems,to.n_elems));// avoid out of bounds access on 'to'
    frep_(i,elems_N(s)) {
        int idx = Find(from,s[i]);
        if (idx !=-1) s[i] = to[idx];
    }
}

// -----------------------------------------------------------------------------

void algo_lib::ind_replvar_Cleanup(algo_lib::Replscope &replscope) {
    (void)replscope;
}

// -----------------------------------------------------------------------------

static void SetVar(algo_lib::Replscope &scope, strptr from, strptr to) {
    // make sure the new variable is not a suffix of any existin variable
    algo_lib::FReplvar *replvar = algo_lib::ind_replvar_Find(scope, from);
    if (!replvar) {
        replvar              = &algo_lib::replvar_Alloc();
        replvar->p_replscope = &scope;
        replvar->key         = from;
        (void)replvar_XrefMaybe(*replvar);
    }
    replvar->value = to;
}

// -----------------------------------------------------------------------------

// Set value of key KEY value VALUE
// KEY        string to replace
// VALUE      value to replace it with
// SUBST      If set, $-expand the VALUE parameter
void algo_lib::Set(algo_lib::Replscope &scope, strptr from, strptr to, bool subst) {
    if (subst) {
        tempstr temp;
        Ins(&scope, temp, to, false);
        SetVar(scope,from,temp);
    } else {
        SetVar(scope,from,to);
    }
}

// -----------------------------------------------------------------------------

void algo_lib::Set(algo_lib::Replscope &scope, strptr from, strptr to) {
    Set(scope,from,to,true);
}

// -----------------------------------------------------------------------------

//
// An empty substitution followed by
static int EatComma(strptr text, int j) {
    if (j < text.n_elems && (text.elems[j] == ',' || text.elems[j] == '.')) {
        j++;
    }
    if (j < text.n_elems && algo_lib::WhiteCharQ(text.elems[j])) {
        j++;
    }
    return j;
}

// Scan TEXT starting at position I for a variable that is defined in R
// If the variable is found, I is set to the character position right after
// the variable, and the variable value is returned.
// If the variable is not found because another $-expression or end-of-string is found,
// the program is killed (if R.fatal is set)
// or the original text of the string is returned without substitution.
static strptr ScanVar(algo_lib::Replscope &R, strptr text, int &i) {
    strptr ret;
    int j=i+1;
    bool ok=false;
    for (; j<=text.n_elems; j++) {
        strptr key(text.elems+i, j-i);
        if (algo_lib::FReplvar *replvar = algo_lib::ind_replvar_Find(R,key)) {
            ok=true;
            ret=replvar->value;
            if (ret.n_elems==0 && R.eatcomma) {
                j = EatComma(text,j);
            }
            break;
        }
    }
    if (LIKELY(ok)) {
        i=j;
    } else {
        if (R.fatal) {
            tempstr msg;
            msg<<"algo_lib.badreplace"
               <<Keyval("text",LimitLengthEllipsis(text,80))
               <<Keyval("expr",LimitLengthEllipsis(strptr(text.elems+i, j-i),20))
               <<Keyval("comment","substitution pattern not found");
            FatalErrorExit(Zeroterm(msg));
        } else {
            ret=strptr(text.elems+i, 1);
            i++; // just skip $
        }
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Append TEXT to OUT, performing $-substitution
// There are no separators between $-parameters and rest
// of the text. The earliest possible match is replaced.
//
// EOL      append end-of-line (default)
// SCOPE    if not NULL, replace any $-string in TEXT with corresponding value.
//            it is an error if any $-string does not expand.
void algo_lib::Ins(algo_lib::Replscope *scope, cstring &out, strptr text, bool eol) {
    int i=0;
    int lim = text.n_elems;
    int start=0;
    strptr var;
    while (i < lim) {
        int ch = text.elems[i];
        if (ch=='$') {
            out << qGetRegion(text,start,i-start);// previous stuff
            out << ScanVar(*scope,text,i);// updates i
            start=i;
        } else {
            i++;
        }
    }
    if (i > start) {
        out << qGetRegion(text,start,i-start);
    }
    if (eol) {
        out << '\n';
    }
}

void algo_lib::Ins(algo_lib::Replscope *scope, cstring &out, strptr text) {
    Ins(scope,out,text,true);
}

// -----------------------------------------------------------------------------

// Enable comma-eating (default true)
void algo_lib::SetEatComma(algo_lib::Replscope &scope, bool enable) {
    scope.eatcomma=enable;
}

// Enable strict mode (default true -- any failed substitution kills process)
// If strict mode is off, failed substitution acts as if there was no substitution
void algo_lib::SetStrictMode(algo_lib::Replscope &scope, bool enable) {
    scope.fatal=enable;
}

// -----------------------------------------------------------------------------

// Perform $-substitutions in TEXT and return new value.
tempstr algo_lib::Subst(algo_lib::Replscope &scope, strptr text) {
    tempstr retval;
    Ins(&scope, retval, text, false);
    return retval;
}

// read TEXT into a tuple.
// perform variable substitution on key-value pairs.
// write tuple back.
// return resulting string
tempstr algo_lib::Tuple_Subst(algo_lib::Replscope &R, strptr text) {
    Tuple tuple;
    (void)Tuple_ReadStrptrMaybe(tuple,text);
    tempstr str;
    ind_beg(algo::Tuple_attrs_curs, attr, tuple) {
        str        = Subst(R, attr.name);
        attr.name  = str;
        str        = Subst(R, attr.value);
        attr.value = str;
    }ind_end;
    tempstr ret;
    Tuple_Print(tuple, ret);
    return ret;
}

algo::tempstr::tempstr() {
#ifdef WIN32
    // since algo_lib is single-threaded,
    // tempstr() cache cannot be used on Windows
#else
    int n = algo_lib::_db.n_temp;
    if (n > 0) {
        n--;
        cstring &str   = algo_lib::temp_strings_qFind(n);
        algo_lib::_db.n_temp = n;
        ch_elems          = str.ch_elems;
        ch_n        = str.ch_n;
        ch_max      = str.ch_max;
        str.ch_elems      = NULL;
        str.ch_n    = 0;
        str.ch_max  = 0;
    }
#endif
}

algo::tempstr::~tempstr() {
#ifdef WIN32
    // since algo_lib is single-threaded,
    // tempstr() cache cannot be used on Windows
#else
    i32        n   = algo_lib::_db.n_temp;
    if (n < algo_lib::temp_strings_N()) {
        cstring   &str = algo_lib::temp_strings_qFind(n);
        // temp strings sitting in the free pool are empty.
        n++;
        algo_lib::_db.n_temp = n;
        str.ch_elems = ch_elems;
        str.ch_n     = 0;
        str.ch_max   = ch_max;
        ch_elems     = NULL;
        ch_n         = 0;
        ch_max       = 0;
    }
#endif
}

// Return a run of characters up to next occurence of SEP (or to end of string)
// Skip any characters equal to SEP that follow the run
// sep="|", string = "a||b|"; return value -> "a", rest -> "b|";
// sep="|", string = "a";     return value -> "a", rest -> "";
// sep="|", string = "|x";    return value -> "" , rest -> "x";
strptr algo::GetTokenChar(algo::StringIter &S, char sep) {
    int from = S.index;
    int to   = from;
    int lim  = elems_N(S.expr);
    while (to < lim && !(S.expr[to] == sep)) to++;
    int end  = to;
    lim      = i32_Min(lim, to + 1);
    while (end < lim && (S.expr[end] == sep)) end++;
    S.index  = end;
    return qGetRegion(S.expr, from, to-from);
}

static inline algo::strptr _GetWordCharf(algo::StringIter &iter, bool (*sep)(u32)) {
    int from = iter.index;
    int lim  = elems_N(iter.expr);
    while (from < lim && sep(iter.expr[from])) from++;
    int to   = from;
    while (to < lim && !sep(iter.expr[to])) to++;
    iter.index  = to;
    return qGetRegion(iter.expr, from, to-from);
}

// Skip leading characters matching SEP
// Return run of characters up to next matching SEP, or EOF.
// Do not skip trailing separators.
strptr algo::GetWordCharf(algo::StringIter &iter, bool (*sep)(u32)) {
    return _GetWordCharf(iter,sep);
}

// Skip leading whitespace characters
// Return run of characters up to next whitespace, or EOF.
// Do not skip trailing whitespace
strptr algo::GetWordCharf(algo::StringIter &iter) {
    // optimized using inline
    return _GetWordCharf(iter,algo_lib::WhiteCharQ);
}

// Skip any leading whitespace in STR.
// Read and return next word.
strptr algo::GetTypeTag(strptr str) {
    algo::StringIter iter(str);
    iter.Ws();
    return _GetWordCharf(iter,algo_lib::WhiteCharQ);
}

// -----------------------------------------------------------------------------

static algo::NumParseFlags ParseNumber(algo::StringIter &S, u64 &result) {
    strptr expr = S.expr;
    int index = S.index;
    algo::NumParseFlags flags = algo_NumParseFlags_ok;
    u64 num=0;
    char c;
    do {
        if (index == elems_N(expr)) {
            return algo_NumParseFlags_err;
        }
        c = expr[index];
        index++;
        if (algo_lib::WhiteCharQ(c) || c=='+') {
        } else if (c=='-') {
            flags = algo_NumParseFlagsEnum(flags | algo_NumParseFlags_neg);
        } else if (algo_lib::DigitCharQ(c)) {
            break;
        } else {
            return algo_NumParseFlags_err;
        }
    } while (true);
    num = c-'0';
    if (index==elems_N(expr)) {
        result=num;
        S.index=index;
        return flags;
    }
    c = expr[index];
    u64 prev = num;
    u32 overflow = 0;
    if (c == 'x' || c == 'X') {
        index++;
        if (index == elems_N(expr)) {
            return algo_NumParseFlags_err;
        }
        for (; index < elems_N(expr); index++) {
            c = expr[index];
            u8 val;
            if (!algo::ParseHex1(c, val)) {
                break;
            }
            prev = num;
            num = num*16 + val;
            overflow |= num<prev;
        }
    } else {
        for (; index < elems_N(expr); index++) {
            c = expr[index];
            if (!algo_lib::DigitCharQ(c)) {
                break;
            }
            prev = num;
            num = num*10 + (c-'0');
            overflow |= num<prev;
        }
    }
    if (overflow) {
        flags = algo_NumParseFlagsEnum(flags | algo_NumParseFlags_overflow);
    }
    result = num;
    S.index = index;
    return flags;
}

bool algo::TryParseI32(algo::StringIter &iter, i32 &result) {
    u64 v;
    u64 max = u32(-1) >> 1;
    algo::NumParseFlags flags = ParseNumber(iter, v);
    if (flags & algo_NumParseFlags_err) {
        return false;
    }
    if ((v > max) | (flags & algo_NumParseFlags_overflow)) {
        v = max;
    }
    i32 out = (i32)v;
    if (flags & algo_NumParseFlags_neg) {
        out = -out;
    }
    result = out;
    return true;
}

bool algo::TryParseI64(algo::StringIter &iter, i64 &result) {
    u64 v;
    u64 max = u64(-1) >> 1;
    algo::NumParseFlags flags = ParseNumber(iter, v);
    if (flags & algo_NumParseFlags_err) {
        return false;
    }
    if ((v > max) | (flags & algo_NumParseFlags_overflow)) {
        v = max;
    }
    i64 out = v;
    if (flags & algo_NumParseFlags_neg) {
        out = -out;
    }
    result = out;
    return true;
}

bool algo::TryParseU32(algo::StringIter &iter, u32 &result) {
    u64 v;
    u64 max = u32(-1);
    algo::NumParseFlags flags = ParseNumber(iter, v);
    if (flags & algo_NumParseFlags_err) {
        return false;
    }
    if ((v > max) | (flags & algo_NumParseFlags_overflow)) {
        v = max;
    }
    if (flags & algo_NumParseFlags_neg) {
        v = 0;
    }
    result = (u32)v;
    return true;
}

bool algo::TryParseU64(algo::StringIter &iter, u64 &result) {
    u64 v;
    algo::NumParseFlags flags = ParseNumber(iter, v);
    if (flags & algo_NumParseFlags_err) {
        return false;
    }
    if (flags & algo_NumParseFlags_overflow) {
        v = u64(-1);
    }
    if (flags & algo_NumParseFlags_neg) {
        v = 0;
    }
    result = v;
    return true;
}

// -----------------------------------------------------------------------------

// Read a series of digits and return resulting number.
// Return success code
bool algo::TryParseDigits(algo::StringIter &S, double &result) {
    int inum;
    int limit = elems_N(S.expr);
    int c;
    if (S.index==limit) return false;
    c = S.expr[S.index];
    if (c>='0' && c<='9') {
        inum = c-'0';
        S.index++;
    } else {
        return false;
    }
    while (S.index<limit) {
        c = S.expr[S.index];
        if (c>='0' && c<='9') {
            if (inum >= ((INT_MAX-9)/10)) {
                goto long_case;
            }
            inum=inum*10+c-'0';
            S.index++;
        } else {
            break;
        }
    }
    result = inum;
    return true;
 long_case:
    double num = inum;
    do{
        num=num*10+c-'0';
        S.index++;
        if (S.index==limit) {
            break;
        }
        c=S.expr[S.index];
    } while (c>='0' && c<='9');
    result = num;
    return true;
}

// TODO: document these more carefully
// Read a series of digits N, returning N / pow(10, length(N))
// If successful, advance index. Otherwise, leave index where it was.
bool algo::TryParseFraction(algo::StringIter &S, double &result) {
    int ifrac,idenom;
    strptr expr = S.expr;
    int limit = elems_N(expr);
    int c;
    if (S.index==limit) return false;
    c = expr[S.index];
    if (c>='0' && c<='9') {
        ifrac = c-'0';
        idenom = 10;
        S.index++;
    } else {
        return false;
    }
    while (S.index<limit) {
        c = expr[S.index];
        if (c>='0' && c<='9') {
            if (idenom >= (INT_MAX-9)/10) {
                goto long_case;
            }
            ifrac = ifrac*10+c-'0';
            idenom *= 10;
            S.index++;
        } else {
            break;
        }
    }
    result = double(ifrac)/idenom;
    return true;
 long_case:
    double frac=ifrac,denom=idenom;
    do{
        frac = frac*10+c-'0';
        denom *= 10;
        S.index++;
        if (S.index==limit) {
            break;
        }
        c=expr[S.index];
    } while (c>='0' && c<='9');
    result = frac/denom;
    return true;
}

bool algo::TryParseDouble(algo::StringIter &iter, double &result) {
    int limit = elems_N(iter.expr);
    // skip whitespace
    while(iter.index < limit) {
        int c = iter.expr[iter.index];
        if (c==' '||c=='\n'||c=='\r'||c=='\t') {
            iter.index++;
            continue;
        } else {
            break;
        }
    }
    char c;
    int old_index = iter.index;
    if (iter.index == limit) {
        return false;
    }
    // skip prefix - or +
    c = iter.expr[iter.index];
    bool isneg = false;
    if (c == '-') {
        isneg=true;
        iter.index++;
        if (iter.index == limit) {
            iter.index = old_index;
            return false;
        }
        c = iter.expr[iter.index];
    } else if (c == '+') {
        iter.index++;
        if (iter.index == limit) {
            iter.index = old_index;
            return false;
        }
        c = iter.expr[iter.index];
    }
    double num;
    if (TryParseDigits(iter, num)) { // 0123.456
        if (SkipChar(iter, '.')) {
            double frac;
            if (TryParseFraction(iter,frac)) {
                num+=frac;
            }
        }
    } else if (c=='.') {    // .456
        iter.index++;
        if (!TryParseFraction(iter,num)) {
            iter.index = old_index;
            return false;
        }
    } else {
        iter.index=old_index;    // error
        return false;
    }
    if (iter.index < limit) {    // e+123 or e-123
        c = iter.expr[iter.index];
        if (c == 'e' || c == 'E') {
            iter.index++;
            bool neg=false;
            if (iter.index < limit) {
                c = iter.expr[iter.index];
                if (c == '+') {
                    iter.index++;
                } else if (c == '-') {
                    iter.index++;
                    neg=true;
                }
            }
            if (!algo_lib::DigitCharQ(iter.Peek())) {
                iter.index = old_index;
                return false;
            }
            double exp;
            if (TryParseDigits(iter, exp)) {
                num *= pow(10.,double_NegateIf(exp,neg));
            } else {
                iter.index = old_index;
                return false;
            }
        }
    }
    switch (iter.Peek()) {
    case 'B':
        num *= 1e9;
        goto found_scaling_factor;
    case 'M':
        num *= 1e6;
    found_scaling_factor:
        iter.GetChar();
    }
    result = isneg ? -num : num;    // return result (maybe negate)
    return true;
}

// Search for the next occurence of SEP. If not found, assume it occurs at EOF
// Return any characters between current position and the occurence of SEP
// Skip SEP, if needed.
strptr algo::GetTokenStrptr(StringIter &iter, const strptr& separator) {
    int i = FindFrom(iter.expr, separator, iter.index);
    strptr result;
    if (i==-1) {
        result=iter.Rest();
        iter.index=elems_N(iter.expr);
    } else {
        result=qGetRegion(iter.expr, iter.index,i-iter.index);
        iter.index=i+elems_N(separator);
    }
    return result;
}

// -----------------------------------------------------------------------------

// if the next characters match STR, skip and return true
// optionally do case sensitive comparison
bool algo::SkipStrptr(StringIter &iter, strptr str, bool case_sens) {
    bool match = StrEqual(GetRegion(iter.expr, iter.index, elems_N(str)), str, case_sens);
    iter.index += match ? elems_N(str) : 0;
    return match;
}

// -----------------------------------------------------------------------------

// Case-sensitive version
bool algo::SkipStrptr(StringIter &iter, strptr str) {
    return SkipStrptr(iter,str,true);
}

// -----------------------------------------------------------------------------

i64 algo::ParseI64(StringIter &iter, i64 dflt) {
    i64 result;
    return TryParseI64(iter, result) ? result : dflt;
}

// Convert string to numeric type.
// If there is an error, or string is empty, return DFLT.
// If there is junk at the end of the string, silently ignore it.
// Use StringIter version to check for junk, or use -X version to throw exception.
u64 algo::ParseU64(StringIter &iter, u64 dflt) {
    u64 result;
    return TryParseU64(iter, result) ? result : dflt;
}

int algo::ParseI32(StringIter &iter, int dflt) {
    int result = dflt;
    if (TryParseI32(iter, result)) {
        return result;
    }
    return dflt;
}

u32 algo::ParseU32(StringIter &iter, u32 dflt) {
    u32 result = dflt;
    if (TryParseU32(iter, result)) {
        return result;
    }
    return dflt;
}

int algo::StringIter::GetDigit(int dflt) {
    if (index < elems_N(expr)) {
        int c = expr[index];
        index++;
        if (c>='0' && c<='9') {
            return c-'0';
        }
    }
    return dflt;
}

// -----------------------------------------------------------------------------

void algo_lib::RunCsvParse(algo_lib::CsvParse &parsecsv) {
    ary_tok_RemoveAll(parsecsv);
    if (elems_N(parsecsv.input)) {
        algo::StringIter iter(parsecsv.input);
        bool trailing_comma=false;
        do {
            iter.Ws();
            int ch = iter.Peek();
            strptr tok;
            if (ch=='\'' || ch=='"') {
                iter.index++;
                tok = GetTokenChar(iter, u8(ch));
                iter.Ws();
            } else {
                int from=iter.index;
                while (iter.index < elems_N(iter.expr) && iter.expr[iter.index] != parsecsv.sep) {
                    iter.index++;
                }
                tok = qGetRegion(iter.expr,from,iter.index-from);
                tok = TrimmedRight(tok);
            }
            ary_tok_Alloc(parsecsv) = tok;
            trailing_comma = SkipChar(iter, parsecsv.sep);
            if (!(trailing_comma || iter.EofQ())) {
                break; // invalid token -- break early
            }
        } while (!iter.EofQ());
        if (trailing_comma) {
            algo::strptr &ignore = ary_tok_Alloc(parsecsv);// extra element at the end
            (void)ignore;
        }
    }
}

// Find first occurence of MATCH in S
// If found, return range corresponding to the match.
// If not found, return range (S.n_elems,S.n_elems) -- an empty range positioned at
// the end of S
algo::i32_Range algo::substr_FindFirst(const aryptr<char> &s, const aryptr<char> &match) {
    int len = elems_N(match);
    int lim = elems_N(s) - (len-1);
    for (int i=0; i< lim; i++) {
        if (qGetRegion(s,i,len) == match) {
            return i32_Range(i,i+len);
        }
    }
    return i32_Range(elems_N(s),elems_N(s));
}

// Same as above but search right-to-left.
// In case of failure, return range (0,0) -- empty range positioned at start of S.
algo::i32_Range algo::substr_FindLast(const aryptr<char> &s, const aryptr<char> &match) {
    int len = elems_N(match);
    rrep_(i,elems_N(s) - (len-1)) {
        if (qGetRegion(s,i,len) == match) {
            return i32_Range(i,i+len);
        }
    }
    return i32_Range(0,0);
}

// Strip leading whitespace, return new strptr.
strptr algo::TrimmedLeft(strptr s) {
    int i=0;
    for (; i<elems_N(s) && algo_lib::WhiteCharQ(s[i]); i++) {
    }
    return RestFrom(s,i);
}

// Strip trailing whitespace, return new strptr.
strptr algo::TrimmedRight(strptr s) {
    int i=elems_N(s);
    for (; i>0 && algo_lib::WhiteCharQ(s[i-1]); i--) {
    }
    return FirstN(s,i);
}

// TrimmedLeft + TrimmedRight
strptr algo::Trimmed(strptr s) {
    return TrimmedLeft(TrimmedRight(s));
}

// scan STR until character SEP is found.
// if SEP is not found, it is assumed to be beyond the right side of the string
// set LEFT to point to the characters to the left of found location.
// set STR to point to the characters to the right of found location.
// Example:
// str = "abc.def", sep='.'; After NextSep, str="def", left="abc"
// str = "abc-def", sep='.'; After NextSep, str="", left="abc-def"
void algo::NextSep(algo::strptr &str, char sep, algo::strptr &left) {
    int i=0;
    int j=str.n_elems;// start of str's next value
    left.elems=str.elems;
    for (; i<str.n_elems; i++) {
        if (str.elems[i] == sep) {
            j=i+1;
            break;
        }
    }
    left.n_elems=i;
    str.elems += j;
    str.n_elems -= j;
}

// -----------------------------------------------------------------------------

void algo::Word_curs_Reset(Word_curs &curs, strptr text) {
    curs.text  = text;
    curs.index = 0;
    Word_curs_Next(curs);
}

void algo::Word_curs_Next(Word_curs &curs) {
    char *elems = curs.text.elems;
    int index = curs.index;
    while (index < curs.text.n_elems && algo_lib::WhiteCharQ(elems[index])) {// skip white chars before word
        index++;
    }
    curs.token.elems = elems + index;
    while (index<curs.text.n_elems && !algo_lib::WhiteCharQ(elems[index])) {// skip to first white char
        index++;
    }
    curs.token.n_elems = elems + index - curs.token.elems;
    curs.index=index;
}

// -----------------------------------------------------------------------------

// Skip any leading whitespace.
// Read next word (GetWordCharf) and compare to typetag.
// Skip any subsequent whitespace as well.
// Return true if type typetag matches
// if typetag doesn't match, input string remains unchanged
// " a  b " -> typetag is "a", rest is "b "
// "a    b" -> typetag is "a", rest is "b"
bool algo::StripTypeTag(strptr &in_str, strptr typetag) {
    algo::StringIter iter(in_str);
    iter.Ws();             // skip leading whitespace
    strptr w = GetWordCharf(iter); // read next word
    bool retval = w == typetag;    // compare
    if (retval) {
        iter.Ws();      // skip subsequent whitespace
        in_str = iter.Rest();   // save rest of string
    }
    return retval;
}

// -----------------------------------------------------------------------------

// Limit length of string S ot at most LEN characters
// If S is trimmed, append "..." to the end
tempstr algo::LimitLengthEllipsis(strptr s, int len) {
    tempstr ret;
    ret << FirstN(s,len);
    if (elems_N(s) > len) {
        // Replace last few characters with '.'
        for (u32 i=0; i<3; i++) {
            if (ret.ch_n > i) {
                ret.ch_elems[ret.ch_n-1-i]='.';
            }
        }
    }
    return ret;
}

// -----------------------------------------------------------------------------

algo::i32_Range algo::ch_FindFirst(const algo::strptr &s, char match) {
    int lim = ch_N(s);
    for (int i=0; i < lim; i++) {
        if (s.elems[i] == match) {
            return i32_Range(i,i+1);
        }
    }
    return i32_Range(lim,lim);
}

// -----------------------------------------------------------------------------

algo::i32_Range algo::ch_FindLast(const algo::strptr &s, char match) {
    rrep_(i,ch_N(s)){
        if (s.elems[i] == match) {
            return i32_Range(i,i+1);
        }
    }
    return i32_Range(0,0);
}


bool algo::strptr_ReadStrptrMaybe(strptr , strptr ){
    return false;
}

// -----------------------------------------------------------------------------

// Append / to string STR unless STR already ends in one.
// str << dirname << MaybeDirSep << filename.
// The separator is always /. To support windows-specific pathnames,
// use ToWindows path where appropriate.
void algo::MaybeDirSep(cstring &str) {
    if (ch_N(str)) {
        char last=ch_qLast(str);
        if (last!='\\' && last!='/') {
            str<<"/";
        }
    }
}

// -----------------------------------------------------------------------------

i32 algo::strptr_Cmp(algo::strptr a, algo::strptr b) {
    int n = i32_Min(a.n_elems,b.n_elems);
    for (int i=0; i<n; i++) {
        if (a[i] != b[i]) return a[i]-b[i];
    }
    return i32_Cmp(a.n_elems, b.n_elems);
}

// -----------------------------------------------------------------------------

bool algo::strptr_Lt(algo::strptr a, algo::strptr b) {
    int n = i32_Min(a.n_elems,b.n_elems);
    for (int i=0; i<n; i++) {
        if (a[i] != b[i]) return a[i]<b[i];
    }
    return a.n_elems < b.n_elems;
}

// -----------------------------------------------------------------------------

// Compare two strings for equality, case-sensitively
bool algo::strptr_Eq(algo::strptr a, algo::strptr b) {
    int lim = a.n_elems;
    if (lim != b.n_elems) {
        return false;
    }
    int i=0;
    // 8 bytes at a time
    while (i+ssizeof(u64) <= lim) {
        if (*(u64*)(a.elems+i) != *(u64*)(b.elems+i)) {
            return false;
        }
        i += ssizeof(u64);
    }
    // another 4
    if (i+ssizeof(u32) <= lim) {
        if (*(u32*)(a.elems+i) != *(u32*)(b.elems+i)) {
            return false;
        }
        i += ssizeof(u32);
    }
    // the rest
    for (; i<lim; i++) {
        if (a[i] != b[i]) {
            return false;
        }
    }
    return true;
}

// -----------------------------------------------------------------------------

algo::Attr_curs::Attr_curs() {
    valid=false;
}

// -----------------------------------------------------------------------------

void algo::Attr_curs_Reset(Attr_curs &curs, strptr line) {
    curs.iter=algo::StringIter(line);
    curs.valid=true;
    Attr_curs_Next(curs);
}

// -----------------------------------------------------------------------------

void algo::Attr_curs_Next(Attr_curs &curs) {
    ch_RemoveAll(curs.name);
    ch_RemoveAll(curs.value);
    // skip whitespace
    int start = curs.iter.index;
    curs.iter.Ws();
    curs.valid = !curs.iter.EofQ() && curs.iter.Peek() != '#';
    // scan name/value
    if (curs.valid) {
        cstring_ReadCmdarg(curs.value, curs.iter, false);
        if (SkipChar(curs.iter, ':')) {
            TSwap(curs.name,curs.value);
            bool ok = cstring_ReadCmdarg(curs.value, curs.iter, true);
            curs.valid &= ok;
        }
        curs.valid &= curs.iter.index > start;
    }
}

// -----------------------------------------------------------------------------

// Compare two strings whose start address is 16-byte aligned
// Do not use this function --
bool algo::AlignedEqual(strptr a, strptr b) {
#ifdef AOS_SSE42
    i32 remains=a.n_elems;
    if (remains != (i32)b.n_elems) {
        return false;
    }
    __m128i A;
    __m128i B;
    while (remains>0) {
        A=*(__m128i*)(a.elems);
        B=*(__m128i*)(b.elems);
        int c = _mm_cmpestri(B, remains
                             , A, remains
                             , _SIDD_CMP_EQUAL_ORDERED);
        if (c!=0) {
            return 0;
        }
        a.elems += 16;
        b.elems += 16;
        remains -= 16;
    }
    return 1;
#else
    return StrEqual(a, b, true);
#endif
}

// -----------------------------------------------------------------------------

// insert TEXT into OUT, indenting as necessary;
// Initial indentation is INDENT, it's adjusted as necessary as { and } are found
// in the TEXT.
// Each indent is 4 spaces.
// Trailing //-sytle comments are stripped
// /* */-style comments are not supported
// Lines beginning with # (#ifdef, etc) are printed at column zero.
void algo::InsertIndent(algo::cstring &out, strptr text, int indent) {
    ind_beg(Line_curs,line,text) {
        line = TrimmedRight(line);
        int next_indent = indent;
        int lcurly=-1;// offset
        int rcurly=-1;// offset
        int lstart=-1;
        for (int i=0; i<line.n_elems; i++) {
            char c = line.elems[i];
            if (algo_lib::WhiteCharQ(c)) {
                // whitespace
            } else {
                if (lstart == -1) {// save start of line
                    lstart=i;
                }
                if (c == '{') {// opening curly in last position
                    lcurly = i;
                } else if (c== '}') {
                    rcurly = lstart == i ? i:-1;// closing curly in first position
                } else if (c=='/' && i<line.n_elems-1 && line[i+1]=='/') {
                    break;
                } else {
                    lcurly=-1;// reset lcurly on any non-ws char
                }
            }
        }
        if (rcurly != -1) {
            indent--;
            next_indent--;
        }
        if (lcurly != -1) {
            next_indent++;
        }
        if (lstart != -1) {
            // line is un-indented if it starts with #
            if (!(line.n_elems>0 && line.elems[0]=='#')) {
                char_PrintNTimes(' ', out, indent*4);
            }
            out << RestFrom(line,lstart);
        }
        out << eol;
        indent = i32_Max(next_indent,0);
    }ind_end;
}

// -----------------------------------------------------------------------------

// Convert unix path to windows path
// This replaces slashes with backslashes
tempstr algo::ToWindowsPath(strptr path) {
    tempstr ret;
    for (int i=0; i<path.n_elems; i++) {
        if (path[i] == ':') {
            // not an allowed character
        } else if (path[i] == '/') {
            if (ret.ch_n && ret.ch_elems[ret.ch_n-1]=='\\') {
                // remove double slashes
            } else {
                ch_Alloc(ret) = '\\';
            }
        } else {
            ch_Alloc(ret) = path[i];
        }
    }
    return ret;
}

// -----------------------------------------------------------------------------

// compatibility
void algo::reset(algo::cstring &lhs) {
    ch_RemoveAll(lhs);
}

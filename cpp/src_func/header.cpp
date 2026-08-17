// Copyright (C) 2023-2024,2026 AlgoRND
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
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
// Target: src_func (exe) -- Access / edit functions
// Exceptions: yes
// Source: cpp/src_func/header.cpp -- Generate prototypes in header
//

#include "include/src_func.h"

// -----------------------------------------------------------------------------

// Number of bytes of TEXT the tail of a string or character literal occupies,
// QUOTE being the byte that closes it.
// QUOTE is cleared once that byte is found, and it is what tells a line that
// begins inside a literal from one that does not.
// A quoted literal is carried to the next line by a backslash standing at the end
// of this one, which escapes the newline: `"a\` opens a literal whose text goes on
// below, so the braces on the next line up to the closing quote are literal text
// and nest nothing. That is the one form a quoted literal spans lines in, so QUOTE
// is left set for exactly it. A tail that reaches the end of the line any other
// way -- a header cut mid-string -- closes the literal there, since carrying it on
// would read the rest of the file as string text.
static int LiteralTailN(strptr text, cstring &quote) {
    char close=strptr(quote)[0];
    int i=0;
    while (i<elems_N(text) && text.elems[i]!=close) {
        i += text.elems[i]=='\\' ? 2 : 1;
    }
    if (i<=elems_N(text)) {
        quote = "";
    }
    return i<elems_N(text) ? i+1 : elems_N(text);
}

// -----------------------------------------------------------------------------

// Number of bytes the string or character literal starting at the first byte
// of TEXT occupies, both quotes counted.
// The count exists so a brace scan can step over a literal without reading
// what stands inside it. QUOTE carries the state of a literal continued onto the
// next line, which LiteralTailN describes.
// The literal here is the quoted kind, whose closing byte is another quote and
// whose backslash escapes one. A raw string closes on `)delim"` instead and
// escapes nothing, so RawstrN measures that form.
static int LiteralN(strptr text, cstring &quote) {
    quote = ch_FirstN(text,1);
    return 1 + LiteralTailN(RestFrom(text,1), quote);
}

// -----------------------------------------------------------------------------

// The run of identifier bytes standing immediately before offset I of LINE.
// What a quote byte means depends on the word in front of it, and both tests
// below read that word: L, u, U and u8 make a quote an encoded literal, those
// same prefixes followed by R -- and a bare R -- make it a raw string, and any
// other word makes it the digit separator of a number.
static strptr PrefixWord(strptr line, int i) {
    int beg = i;
    while (beg > 0 && algo_lib::IdentCharQ(line.elems[beg-1])) {
        beg--;
    }
    return ch_FirstN(RestFrom(line,beg), i-beg);
}

// -----------------------------------------------------------------------------

// The run of identifier bytes starting at offset I of LINE.
// A scan that reads a line word by word steps over one such run per move, and the
// word it reads is that run. PrefixWord reads the run standing before an offset,
// which is the same question asked backwards.
static strptr IdentWord(strptr line, int i) {
    int end = i;
    while (end < elems_N(line) && algo_lib::IdentCharQ(line.elems[end])) {
        end++;
    }
    return ch_FirstN(RestFrom(line,i), end-i);
}

// -----------------------------------------------------------------------------

// Test whether the quote byte at offset I of LINE opens a character literal
// rather than standing between the digits of a number.
// C++ spells the two with the same byte. In 1'000'000 the quote separates two
// digits and encloses nothing; in L'}' it opens a literal whose one character is
// a brace. A brace scan that reads the second as a separator counts that brace
// and puts the line one level away from where the compiler puts it, which moves
// where the section's closing brace appears to be; reading the first as a literal
// opening steps over the rest of the number and loses whatever braces stand
// behind it.
// A separator stands between two identifier bytes, which is what a digit and a
// hex digit both are. So does the quote of an encoded literal, whose encoding
// prefix is an identifier byte too -- and one of them, u8, even ends in a digit.
// The prefix is therefore read as the whole word standing before the quote: L, u,
// U and u8 open a literal whatever follows them, and every other word before a
// quote that has an identifier byte on both sides is a number.
// test/src_func/sectionclose.hdr carries the encoded literals and the digit
// separator, and src_func.SectionClose is the golden.
static bool CharLitOpenQ(strptr line, int i) {
    strptr word = PrefixWord(line,i);
    char next = i+1 < elems_N(line) ? line.elems[i+1] : '\0';
    bool prefix = word=="L" || word=="u" || word=="U" || word=="u8";
    bool sep = ch_N(word) > 0 && !prefix && algo_lib::IdentCharQ(next);
    return !sep;
}

// -----------------------------------------------------------------------------

// Test whether C may stand in the delimiter of a raw string.
// C++ admits any byte there but whitespace, a paren, a backslash and the quote.
static bool RawdelimCharQ(char c) {
    return !algo_lib::WhiteCharQ(c) && c!='(' && c!=')' && c!='\\' && c!='"';
}

// -----------------------------------------------------------------------------

// Test whether the quote byte at offset I of LINE opens a raw string.
// A raw string is spelled R"delim(text)delim", with the encoding prefixes a
// character literal takes standing in front of the R. The bytes between the quote
// and the paren are the delimiter, and C++ bounds them at sixteen and admits no
// whitespace, paren, backslash or quote among them. A quote whose paren stands
// past such a byte, or past sixteen of them, opens no raw string even with a word
// ending in R in front of it, and reading it as one would step over the rest of
// the line.
static bool RawstrOpenQ(strptr line, int i) {
    strptr word = PrefixWord(line,i);
    bool prefix = word=="R" || word=="LR" || word=="uR" || word=="UR" || word=="u8R";
    int lim = i+17 < elems_N(line) ? i+17 : elems_N(line);
    int j = i+1;
    while (j<lim && RawdelimCharQ(line.elems[j])) {
        j++;
    }
    return prefix && j<elems_N(line) && line.elems[j]=='(';
}

// -----------------------------------------------------------------------------

// Number of bytes of TEXT the tail of a raw string occupies, RAWTERM being the
// `)delim"` that closes it.
// RAWTERM is cleared once the terminator is found. A raw string may span lines,
// so a tail that does not close on TEXT runs to its end and leaves RAWTERM set
// for the next line to go on with.
static int RawstrTailN(strptr text, cstring &rawterm) {
    int end = FindStr(text,rawterm);
    int ret = end<0 ? elems_N(text) : end+ch_N(rawterm);
    if (end>=0) {
        rawterm = "";
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Number of bytes of TEXT the raw string opening at its first byte occupies.
// TEXT begins at the quote of `R"delim(`, which RawstrOpenQ has accepted, so the
// paren stands on this line and every byte between it and the quote belongs to
// the delimiter.
// RAWTERM is set to the terminator the delimiter spells, and it holds the whole
// `)delim"` rather than the delimiter alone because a delimiter is allowed to be
// empty and a terminator never is: RAWTERM is therefore what tells a line that
// begins inside a raw string from one that does not.
static int RawstrN(strptr text, cstring &rawterm) {
    int open = FindChar(text,'(');
    rawterm = ")";
    rawterm << ch_FirstN(RestFrom(text,1), open-1);
    rawterm << "\"";
    return open+1 + RawstrTailN(RestFrom(text,open+1), rawterm);
}

// -----------------------------------------------------------------------------

// Number of braces LINE opens minus the number it closes, counting only the
// braces that nest C++ code.
// A brace byte inside a comment or inside a literal nests nothing. A header
// whose update-hdr section holds `char eol = '}'`, or a `/* } */` remark, would
// otherwise be read as one brace deeper or shallower than the compiler reads
// it, and the section's close would be located on the wrong line -- either a
// line above the real close, whose acceptance drops the lines between, or none
// at all. Stepping over the quoted and commented spans leaves exactly the
// braces the compiler sees.
// INCOMMENT carries the state of a `/* */` comment in and out, because a block
// comment spans lines and the previous line decides how this one reads. RAWTERM
// carries the state of a raw string the same way and for the same reason: a raw
// string spans lines too, and while one is open every byte of every line belongs
// to it until its terminator. CONTLIT is the third such span, and the one that is
// easy to miss: an ordinary quoted literal spans lines when a backslash at the end
// of one escapes the newline, so `const char *s = "a\` puts the braces of the next
// line, up to the closing quote, inside a string. Reading that next line as code
// counts those braces and takes the first quote it meets -- the literal's own
// closing quote -- for an opening one, which puts every brace on the line on the
// wrong side of the literal.
// CONTCOMMENT is the fourth, and it is a whole line rather than part of one: C++
// splices lines before it recognizes comments, so a `//` comment whose line ends in
// a backslash swallows the line below it entire, and the one below that when it ends
// in a backslash too. A header holding `// see below \` above a namespace line has
// no such namespace at all, and reading that line as code counts a brace the
// compiler never sees.
// NCODE is set to the number of leading bytes of LINE that stand outside the
// comment LINE ends in: the offset of a trailing `//`, the offset of a `/*` the
// line does not close, and the length of the line when it ends in code. A
// caller that has to tell a live trailing comment from text quoted inside a
// comment or a literal reads it; the brace count itself does not.
// NMAX is set to the deepest the line goes, counted from where it began. It is
// what tells a line that opened no brace at all from one that opened a brace and
// closed it again, which the net count reports alike: both end level with where
// they started. A caller locating the brace a section opens reads it, since a
// section whose braces are all closed on one line is not a section whose opening
// is still ahead.
static int BraceDelta(strptr line, bool &incomment, cstring &rawterm, cstring &contlit, bool &contcomment, int &ncode, int &nmax) {
    int ret=0;
    int i=0;
    nmax=0;
    ncode = incomment || contcomment ? 0 : elems_N(line);
    while (i<elems_N(line)) {
        char c=line.elems[i];
        char next = i+1<elems_N(line) ? line.elems[i+1] : '\0';
        int step=1;
        if (contcomment) {
            step=elems_N(line)-i;// the whole line belongs to the comment above it
            contcomment=EndsWithQ(line,"\\");
        } else if (contlit != "") {
            step=LiteralTailN(RestFrom(line,i),contlit);
        } else if (rawterm != "") {
            step=RawstrTailN(RestFrom(line,i),rawterm);
        } else if (incomment) {
            incomment = !(c=='*' && next=='/');
            step = incomment ? 1 : 2;
            if (!incomment) {
                ncode = elems_N(line);
            }
        } else if (c=='/' && next=='*') {
            incomment=true;
            ncode=i;
            step=2;
        } else if (c=='/' && next=='/') {
            ncode=i;
            step=elems_N(line)-i;// rest of the line is a comment
            contcomment=EndsWithQ(line,"\\");
        } else if (c=='"' && RawstrOpenQ(line,i)) {
            step=RawstrN(RestFrom(line,i),rawterm);
        } else if (c=='"' || (c=='\'' && CharLitOpenQ(line,i))) {
            step=LiteralN(RestFrom(line,i),contlit);
        } else if (c=='{') {
            ret++;
            nmax = ret>nmax ? ret : nmax;
        } else if (c=='}') {
            ret--;
        }
        i+=step;
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Extract the namespace of the update-hdr section a line like
// 'namespace xyz { // update-hdr' opens.
// A marker line may open more than one namespace -- `namespace outer { namespace
// inner {` -- and the section's body then stands inside the innermost of them,
// which is where the prototypes are written. The functions written there are the
// ones whose definition names that namespace, and a definition inside
// outer::inner names inner, so the innermost name is the one the insertion point
// and the function selection agree on. Reading the outermost name instead
// declares outer's functions inside inner, and leaves inner's own functions with
// no declaration at all.
// C++17 spells the same nesting as one namespace line, `namespace outer::inner {`,
// and the innermost name is again the one to read: the section body stands inside
// outer::inner either way, and a definition there names inner. So the name is read
// as the last identifier of the definition's head -- the tokens from the keyword to
// the brace that opens the body -- and every identifier the head carries after the
// keyword replaces the one before it. That is what carries the attribute C++ admits
// there: the words of `[[deprecated]]` are read and then replaced by the name that
// follows them. A brace is what ends the head, since a marker line may carry code
// of its own, as `namespace foo { void A(); }` does, whose identifiers name no
// namespace.
// Which bytes of the line are read is the second half of the rule, and the half a
// scan over the raw line gets wrong. `namespace foo { // see namespace bar for the
// rest // update-hdr` is an ordinary marker line carrying a remark, and a scan that
// reads every identifier byte finds the keyword inside that remark, starts a second
// head there, and comes back with `rest`. The section then receives the functions of
// a namespace the header never mentions while foo's own stay undeclared, at exit 0.
// An attribute carrying an argument is the same mistake read the other way round:
// the parens of `namespace [[deprecated("use x")]] bar {` are not code standing
// between the keyword and the name, and a scan that ends the head on a paren never
// reaches `bar`.
// The insight is that a namespace head is spelled in code, and a comment or a
// literal on the line is text the compiler does not read as code. So the walk steps
// over each span that is not code -- a block comment, a quoted or a raw string
// literal, and the trailing line comment -- and reads identifiers and braces from
// what is left. The marker tag needs no test of its own for the same reason: the tag
// stands in a comment, and the walk stops where that comment begins. A marker line
// begins outside every such span, which is one of the tests that made it a marker,
// so the walk starts in code and needs no state carried in.
// An empty answer means the line names no namespace at all, which the caller
// refuses rather than fills; a section filled under an empty name receives the
// functions defined at global scope.
// NHEAD is set to the number of namespaces the line names, counting the keywords
// the walk reads as code. It is how deep the section's own braces are meant to
// reach, which a caller comparing it against the depth the opening line ends at
// uses to tell a brace the section owns from one it does not.
// Pinned by src_func.SectionNs and src_func.SectionUnnamed.
strptr src_func::Nsline_GetNamespace(strptr line, int &nhead) {
    strptr ret;
    bool inhead = false;
    nhead = 0;
    bool incomment = false;
    cstring rawterm;
    cstring contlit;
    int i = 0;
    while (i < elems_N(line)) {
        char c = line.elems[i];
        char next = i+1 < elems_N(line) ? line.elems[i+1] : '\0';
        int step = 1;
        if (incomment) {
            incomment = !(c=='*' && next=='/');
            step = incomment ? 1 : 2;
        } else if (c=='/' && next=='*') {
            incomment = true;
            step = 2;
        } else if (c=='/' && next=='/') {
            step = elems_N(line)-i;// rest of the line is a comment
        } else if (c=='"' && RawstrOpenQ(line,i)) {
            step = RawstrN(RestFrom(line,i),rawterm);
        } else if (c=='"' || (c=='\'' && CharLitOpenQ(line,i))) {
            step = LiteralN(RestFrom(line,i),contlit);
        } else if (algo_lib::IdentCharQ(c)) {
            strptr word = IdentWord(line,i);
            step = ch_N(word);
            if (word == "namespace") {
                inhead = true;
                nhead++;
                ret = strptr();
            } else if (inhead) {
                ret = word;
            }
        } else if (c=='{' || c=='}') {
            inhead = false;
        }
        i += step;
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Report the update-hdr section of namespace NS in FNAME that never reaches its
// closing brace, and fail the run.
// The rewrite drops each section's old body as it copies the file, trusting the
// section's closing brace to mark where the fresh prototypes go. A section that
// never closes has no such point: everything from its marker line to the next
// closing brace -- or to EOF -- would be dropped and the truncation written
// back, so the file is left untouched instead.
// Every open section in the file is reported, and each counts as its own error:
// one edit-and-rerun cycle then clears all of them, and the run's error count
// is the number of sections to fix.
static void ReportUnterminatedSection(strptr fname, strptr ns) {
    prerr("src_func.unterminated_section"
          <<Keyval("file",fname)
          <<Keyval("ns",ns)
          <<Keyval("comment","update-hdr section is missing its closing brace; file not rewritten"));
    algo_lib::_db.exit_code++;
}

// -----------------------------------------------------------------------------

// Report the update-hdr section in FNAME whose marker line names no namespace, and
// fail the run.
// The prototypes a section receives are the functions whose definition names the
// section's own namespace, so a section whose namespace could not be read from its
// marker line has no such set. What it gets instead is the functions defined at
// global scope, since those are the ones whose namespace is the empty name, and the
// header comes back declaring them inside whatever the marker opened -- an
// anonymous namespace, or a spelling of a namespace line the reader does not
// recognize.
// The insight is that the name is what joins the two halves of the rewrite: it
// decides which functions are selected, and the section it writes them into is the
// place their definitions are visible from. An unread name breaks the join while
// leaving both halves willing to proceed.
// So a marker line with no name opens no section, and the file is left as it
// stands, as it is for a section that never closes.
// Pinned by src_func.SectionUnnamed.
static void ReportUnnamedSection(strptr fname) {
    prerr("src_func.unnamed_section"
          <<Keyval("file",fname)
          <<Keyval("comment","update-hdr marker line names no namespace; file not rewritten"));
    algo_lib::_db.exit_code++;
}

// -----------------------------------------------------------------------------

// Report the update-hdr section of namespace NS in FNAME whose braces are all
// closed on the line that opened them, and fail the run.
// The prototypes a section receives go above the line that closes it, so a
// section closed on its own opening line has nowhere to hold them: `namespace foo
// { void A(); } // update-hdr` asks for a section and leaves no room for one, and
// a marker line ending one level below where it began -- one that closes the
// namespace enclosing it -- is the same shape written the other way.
// Reporting the line is also what keeps the run from reading some later brace as
// this section's opening. The depth a section's opening reaches is what separates
// section body from the lines below the section, so a section still open with no
// opening located takes the next brace anywhere down the file for its own, drops
// the lines nesting under it as body, and writes the prototypes at its close.
// The file is left as it stands, as it is for a section that never closes, so one
// edit-and-rerun clears the diagnostic.
// Pinned by src_func.SectionBodyless.
static void ReportBodylessSection(strptr fname, strptr ns) {
    prerr("src_func.bodyless_section"
          <<Keyval("file",fname)
          <<Keyval("ns",ns)
          <<Keyval("comment","update-hdr section opens and closes on one line; file not rewritten"));
    algo_lib::_db.exit_code++;
}

// -----------------------------------------------------------------------------

// Report the update-hdr section of namespace NS in FNAME whose opening leaves more
// braces open than its marker line names namespaces, and fail the run.
// `namespace foo { struct S { // update-hdr` is the shape: the line opens the
// namespace and a struct, so the section's body stands at the struct's depth and the
// first line that comes back below it is the struct's own `};` -- a closing brace,
// which passes every test a section close has to pass. The prototypes are written
// inside the struct, the struct's members are dropped as section body, and the
// header is saved at exit 0. A marker that opens two namespaces and closes the
// inner one again is the same disagreement with the depth left one level too low.
// The general shape is that the depth a section opens at is taken for the depth of
// the namespace whose functions the section receives. A brace that opens anything
// else -- a struct, an enum, a function body -- raises that depth without moving the
// namespace, and the close is then located on the inner block's brace. The number of
// namespaces the marker line names says how deep the section's own braces reach, so
// an opening line ending at any other depth has opened a brace the section does not
// own.
// The file is left as it stands, as it is for every other disagreement about where a
// section begins or ends, so one edit-and-rerun clears the diagnostic.
// Pinned by src_func.SectionBodyless.
static void ReportForeignBrace(strptr fname, strptr ns) {
    prerr("src_func.foreign_brace"
          <<Keyval("file",fname)
          <<Keyval("ns",ns)
          <<Keyval("comment","update-hdr section opens a brace that none of its namespaces opened; file not rewritten"));
    algo_lib::_db.exit_code++;
}

// -----------------------------------------------------------------------------

// Report the line that takes NS's update-hdr section in FNAME below the depth the
// section's marker opened while carrying something other than a closing brace, and
// fail the run.
// `namespace foo { // update-hdr` followed by `void B(); }` is the shape: the
// declaration and the namespace's brace share one line, so the depth returns below
// the marker's on a line whose text is a hand-written declaration. Accepting it
// writes the fresh prototypes above that line and leaves the declaration standing
// inside the region whose banner says its text was produced by scanning sources,
// which every later run reproduces because the line is still there.
// The general shape is a disagreement between the two things that describe a
// section's end. The depth says the section ended here; the line's own text says
// it did not, because a section is closed by the namespace's brace and a closing
// brace is the first thing on that line -- which holds for every close the fixture
// carries, `}`, `};`, `}}`, a brace behind a tab, and a brace followed by a
// comment of either kind. Where the two disagree the depth is the one that cannot
// be trusted, because a brace the scan counted may be one the compiler never
// reads as nesting: a macro that spells a block opener, a preprocessor conditional
// whose two arms do not balance.
// So the disagreement refuses the file rather than resolving it, and the section's
// prototypes are not written. The file is left as it stands, as it is for a section
// that never closes and for one that closes on its own marker line, so one
// edit-and-rerun clears the diagnostic.
// Pinned by src_func.SectionUneven.
static void ReportUnbracedClose(strptr fname, strptr ns) {
    prerr("src_func.unbraced_close"
          <<Keyval("file",fname)
          <<Keyval("ns",ns)
          <<Keyval("comment","update-hdr section ends on a line that does not begin with a closing brace; file not rewritten"));
    algo_lib::_db.exit_code++;
}

// -----------------------------------------------------------------------------

// Report FNAME, whose braces do not balance, as a file whose update-hdr sections
// cannot be located, and fail the run. DEPTH is what the count reaches at end of
// file.
// A header holding `#define OPENBLOCK {` inside an update-hdr section is the
// shape. The section's own closing brace only takes the count back to the depth
// the marker opened, so the brace is read as section body and dropped, and the
// copy goes on dropping lines until some construct further down takes the count
// lower -- an enclosing namespace's brace, which is a closing brace and passes
// every test a close has to pass. The hand-written lines between the two are gone
// and the header is saved at exit 0.
// The insight is that the rewrite is decided from one model of the file, its brace
// nesting, and a file whose braces do not balance is a file that model does not
// describe: some brace the scan counted is not nesting, or one the compiler reads
// is not counted. Every line the rewrite drops was chosen by that model, so an
// untrustworthy model is what silently truncates a header.
// So a file is rewritten only when its braces balance. This refuses a header whose
// unbalanced brace stands nowhere near a section as well, which is the price of a
// test that reads the whole file rather than guessing which braces matter.
// Pinned by src_func.SectionUneven.
static void ReportUnbalancedFile(strptr fname, int depth) {
    prerr("src_func.unbalanced_file"
          <<Keyval("file",fname)
          <<Keyval("depth",depth)
          <<Keyval("comment","brace count does not return to zero at end of file; file not rewritten"));
    algo_lib::_db.exit_code++;
}

// -----------------------------------------------------------------------------

// Load body of file
// Scan lines in file
// If found begin marker (namespace XYZ .. // update-hdr)
// Collect includes for namespace
// Find end marker
// Replace middle with prototypes for namespace XYZ
// When finished, save file
// A file that cannot be read fails the run: treating it as empty would
// exit 0 with its update-hdr block silently left stale.
// A file whose last update-hdr section never reaches its closing brace is
// reported and left unwritten instead of being truncated at the marker.
// A section that ends on a line other than a closing brace, and a file whose
// braces do not balance, are reported and left unwritten for the same reason: the
// lines a rewrite drops are chosen by the brace count, and both shapes say the
// count is not describing the file.
// A section whose opening leaves open a brace no namespace of its marker line
// opened is reported as well, since the depth that brace raises is where the
// section's close would be located.
// A marker line naming no namespace is reported and left unwritten as well, since
// the name is what selects the functions the section receives.
static void UpdateHeader(src_func::FTarget &target, strptr fname) {
    algo_lib::MmapFile file;
    if (!MmapFile_Load(file,fname)) {
        algo::PrerrFileFail("src_func.file_read",fname,"source file could not be read");
        algo_lib::_db.exit_code++;
    } else {
        int nerr=algo_lib::_db.exit_code;
        cstring newfile;
        cstring ns;
        cstring srcfilter;
        bool inside=false;
        bool incomment=false;
        bool contcomment=false;
        cstring rawterm;
        cstring contlit;
        int depth=0;
        int nopen=0;
        int nhead=0;
        int nupdate=0;
        int filedepth=0;
        ind_beg(Line_curs,line,file.text) {
            strptr trimmed=Trimmed(line);
            bool opencomment=incomment || contcomment;
            bool openraw=rawterm != "";
            bool openlit=contlit != "";
            int ncode=0;
            int nmax=0;
            int delta=BraceDelta(line,incomment,rawterm,contlit,contcomment,ncode,nmax);
            filedepth += delta;
            // A line opens a section only when the compiler reads the namespace
            // and the marker tag as code. A header carrying a commented-out
            // section, or prose quoting the marker, holds the marker text
            // without meaning it: a hand edit that wraps a section in `/* */`
            // leaves the marker line word for word inside the comment. Taking
            // it for a marker opens a section that was never meant to exist,
            // and the rewrite then replaces everything from there to the next
            // lone brace -- the rest of the comment and the hand-written code
            // below it -- and exits 0. The namespace is code when the line
            // begins outside a comment, which OPENCOMMENT records before the line
            // is scanned: a block comment the line above left open, and a `//`
            // comment the line above continued with a backslash, both put the
            // whole namespace line inside a comment. The tag is code when it stands in the trailing
            // `//` comment the line ends in, and that takes both of the remaining
            // tests: NCODE is where that comment begins, so the tag is looked for
            // from NCODE onward, and the comment is a `//` one only when the line
            // does not end inside a `/* */` one -- which is what INCOMMENT holds
            // after the scan. A line such as `namespace zz { /* off: // update-hdr`
            // carries the tag inside a block comment it never closes, and it opened
            // a section whose rewrite swallowed the rest of the comment and the
            // code below it. A tag inside a `/* */` remark or inside a string
            // literal is text by the same two tests.
            // Where the tag is looked for from matters as much as what bounds it. A
            // line may carry the text twice -- `namespace zz { /* was: //
            // update-hdr */ // update-hdr` quotes the marker in a remark and then
            // writes the real one -- and a search from the line's start finds the
            // quoted occurrence, which stands below NCODE and so reads as text.
            // The line is then no marker at all, its section is never opened, and
            // the run exits 0 with the header left as it was: the silent staleness
            // the read and write failures are reported to prevent. Searching from
            // NCODE reads the line as the compiler does, whichever of the two
            // occurrences comes first. The tag carries the section's attributes as
            // well, and they are read from the same occurrence for the same reason,
            // which is why NSCOMMENT is what both questions are asked of.
            // A raw string is the second span a line can begin inside, because it
            // spans lines as a block comment does: `const char *s = R"(` opens
            // one, and every line until the `)"` that closes it is text the
            // compiler never reads as code. A header that carries a code sample
            // or a table of tuples that way holds whole namespace lines inside
            // the literal, and a `namespace zz { // update-hdr` among them is one
            // of them. OPENRAW records the open raw string the way OPENCOMMENT
            // records the open block comment. A quoted literal carried over by a
            // backslash at the end of a line is the third such span, and OPENLIT
            // records it for the same reason: the namespace of a line whose first
            // bytes are string text is string text too.
            // Pinned by src_func.SectionOpen.
            strptr nscomment = RestFrom(line,ncode);
            bool marker = !opencomment && !openraw && !openlit && !incomment && StartsWithQ(trimmed,"namespace") && FindStr(nscomment,"// update-hdr")>=0;
            // Sections do not nest: a marker line reached while a section is
            // still open means that section never closed. Its own marker is
            // then being dropped as section body, so without this test the
            // rewrite attributes the next closing brace to the open section
            // and swallows everything in between. The marker still opens its
            // own section, so a file that leaves several of them open reports
            // every one.
            if (marker && inside) {
                ReportUnterminatedSection(fname,ns);
            }
            // A marker line whose namespace cannot be read opens no section: the
            // name is what selects the functions to declare, and an empty name
            // selects the ones defined at global scope.
            if (marker) {
                ns = src_func::Nsline_GetNamespace(trimmed,nhead);
                srcfilter = src_func::Nsline_GetSrcfile(nscomment);
                inside = ch_N(ns) > 0;
                depth=0;
                nopen=0;
            }
            if (marker && !inside) {
                ReportUnnamedSection(fname);
            }
            int newdepth = inside ? depth+delta : 0;
            // NOPEN is the number of braces the section's opening carries, read
            // off the first line of the section that opens a brace. The opening
            // need not stand on the marker line -- a header may write
            // `namespace foo // update-hdr` and put the `{` on the line below,
            // which is one section written over two lines -- so a line reached
            // before that brace is not section body and is copied, which is what
            // keeps the brace in the file.
            // The line that opens a brace is not always the line that leaves the
            // depth raised. `namespace foo { void A(); } // update-hdr` opens one
            // and closes it again, and a marker line that closes the namespace
            // enclosing it ends a level below where it began; reading NOPEN off
            // the depth at a line's end leaves it unset for both, and the first
            // brace anywhere further down the file is then latched as this
            // section's opening. NMAX is what says a brace was opened at all, so
            // the opening is located on the line whose deepest point stands past
            // where the line began, whatever the line's end leaves behind.
            // Where the opening line ends is the second question, and it is the
            // one that says whether the section has a body. A line ending at or
            // below the depth the section started at has closed every brace it
            // opened and leaves nowhere to write the prototypes. A line ending
            // above it is still open whatever it opened and closed along the way,
            // so `namespace foo { struct S {}; // update-hdr` is a section one
            // level deep whose body stands below it, and reading the line's
            // deepest point as its end would refuse it as a section with no body.
            // NOPEN is therefore the depth the opening line ends at, and only a
            // line that ends back at or below the section's own start is a
            // section with no body, reported rather than filled.
            // The depth an opening line ends at also has to be a depth the
            // section's own namespaces account for. `namespace foo { struct S {
            // // update-hdr` opens one namespace and one struct, so the depth is
            // the struct's, the close is located on the struct's `};`, and the
            // prototypes are written among its members while they are dropped as
            // section body. NHEAD is how many namespaces the marker line names,
            // which is how deep the section's own braces reach, and an opening
            // line ending anywhere else is refused rather than filled.
            // NOPEN is also what tells the two meanings of a zero depth apart.
            // Before the opening brace the depth is zero because nothing has been
            // opened, and once the marker's own braces are all closed it is zero
            // again because everything has been. Reading the depth alone confuses
            // the two, and a shape test on the line -- "nothing here but a
            // brace" -- is the wrong way to separate them: `};` closes a
            // namespace and is legal C++, and a line the shape test refuses still
            // takes the depth to zero, after which the copy resumes and the next
            // lone brace anywhere below is taken as the close. The header is then
            // written back at exit 0 with lines deleted and the fresh prototypes
            // standing in another namespace.
            // With NOPEN known, the line's text is not read at all. A line is
            // section body while the depth stays at or above NOPEN, because
            // whatever the body opens nests deeper than the marker's own braces:
            // a struct's `};`, a template's function body, a namespace the body
            // itself opens. The first line that brings the depth back below NOPEN
            // is the section's close: the prototypes go above it, the line itself
            // is copied, and the section ends there.
            // A marker opening two namespaces is closed by two separate `}`
            // lines, and only the first of them is the section's close. The second
            // is copied for the reason every line below a section is -- it is
            // outside the section -- which is what keeps the header compiling, and
            // the section has to end at the first close rather than at the depth
            // reaching zero. Between the two closes the depth stands one level
            // above zero and one level below NOPEN, so a section still open there
            // takes a line that opens a brace for body and drops it, and takes
            // that line's own close for a second section close and writes the
            // prototypes a second time. A header holding `struct Extra { int a;
            // };` between the two closes comes back with the struct deleted, a
            // stray `};`, and two prototype blocks, at exit 0. Ending the section
            // at its first close leaves no such window: for a marker opening one
            // namespace the two rules agree, and for a marker opening several the
            // lines below the first close are read as what they are.
            // The depth alone says where a section ends; the line's own text says
            // whether that is a place a section can end. A section is closed by the
            // brace of the namespace its marker opened, and a closing brace is the
            // first thing on the line that carries it -- every close the fixture
            // holds is written that way. A line that takes the depth below NOPEN
            // while its first byte is something else therefore ends a section the
            // depth located wrongly, and the two are refused rather than reconciled,
            // because the depth is the side that a macro spelling a block opener or
            // an unbalanced preprocessor arm makes wrong.
            // Pinned by src_func.SectionClose, which carries a section per shape,
            // and by src_func.SectionUneven, which carries the disagreements.
            bool opening = inside && nopen==0 && nmax>0;
            bool bodyless = opening && newdepth<=0;
            bool foreignbrace = opening && !bodyless && newdepth!=nhead;
            bool body = inside && nopen>0 && newdepth>=nopen;
            bool closedepth = inside && nopen>0 && depth>=nopen && newdepth<nopen;
            bool sectionclose = closedepth && StartsWithQ(trimmed,"}");
            bool unbracedclose = closedepth && !sectionclose;
            if (sectionclose) {
                PrintGlobalProtos(target,ns,srcfilter,newfile);
            }
            if (bodyless) {
                ReportBodylessSection(fname,ns);
            }
            if (foreignbrace) {
                ReportForeignBrace(fname,ns);
            }
            if (unbracedclose) {
                ReportUnbracedClose(fname,ns);
            }
            if (!body) {
                newfile << line << eol;
            }
            if (inside) {
                depth = newdepth;
                if (opening && !bodyless && !foreignbrace) {
                    nopen = newdepth;
                }
                if (sectionclose) {
                    nupdate++;
                }
                if (sectionclose || bodyless || foreignbrace || unbracedclose) {
                    inside=false;
                }
            }
        }ind_end;
        if (inside) {
            ReportUnterminatedSection(fname,ns);
        }
        // A file any of the refusals above reported is left as it stands, whichever
        // refusal it was and however many of them there were. A section closed off by
        // the marker line of the next one is the case that rule exists for: the run
        // ends with INSIDE clear and a section closed, so a write gated on those two
        // alone would save a copy that lost every line from the open marker to the
        // next, while the diagnostic said the file was not rewritten. Pinned by
        // src_func.SectionOpenTwice.
        // Which refusals happened is read off the run's error count rather than
        // counted a second time here: every reporter raises that count, NERR is where
        // it stood as this file was reached, and a count of its own would have to be
        // raised again beside each reporting site -- where a site that forgets prints
        // its diagnostic and saves the truncated header anyway.
        // The brace count reaching zero at end of file is the last thing a rewrite
        // waits on, and it is a test on the file rather than on any one section: a
        // count that does not balance is a count the file's braces do not explain,
        // and every line the rewrite drops was chosen by it. A file already refused
        // for a section of its own is not tested, because the diagnostic it has is
        // the one to act on.
        if (algo_lib::_db.exit_code==nerr && nupdate>0 && filedepth!=0) {
            ReportUnbalancedFile(fname,filedepth);
        }
        if (algo_lib::_db.exit_code==nerr && nupdate>0) {// no section found -> no update
            algo::Refurbish(file);
            prcat(verbose2,"src_func.update"
                  <<Keyval("file",fname));
            int nbefore = algo_lib::_db.stringtofile_nwrite;
            // a write that fails (missing directory, permission) fails the run:
            // exiting 0 with the header silently unwritten would leave the
            // update-hdr block stale
            if (!algo::SaveFile(newfile,fname,"src_func.file_write","header file could not be written")) {
                algo_lib::_db.exit_code++;
            }
            src_func::_db.report.n_filemod += algo_lib::_db.stringtofile_nwrite - nbefore;
        }
    }
}

// -----------------------------------------------------------------------------

static bool UpdateFileQ(src_func::FTargsrc &targsrc) {
    strptr src=src_Get(targsrc);
    return FindStr(src,"/gen/")==-1;
}

// -----------------------------------------------------------------------------

// Go over all sources (includes & sources!)
// Look for 'update-hdr' sections and insert function prototypes there.
void src_func::Main_UpdateHeader() {
    ind_beg(src_func::_db_targsrc_curs,targsrc,_db) if (targsrc.select) {
        if (UpdateFileQ(targsrc)) {
            UpdateHeader(*targsrc.p_target,src_Get(targsrc));
        }
    }ind_end;
}

// Copyright (C) 2026 AlgoRND
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
// Target: doc (exe) -- Render a markdown document to the terminal
// Exceptions: yes
// Source: cpp/doc/hilite.cpp
//
// Colour the contents of a fenced code block.
// Two languages are worth a lexer here, and the corpus says which: of the
// fenced blocks under txt/, two thousand are c++ and three hundred and fifty
// are bash, with a handful of everything else.  A block in any other language
// prints uncoloured, which is the honest result -- a wrong colour reads as a
// claim about the code, and a shell line mistaken for c++ makes that claim on
// every line.
// The lexers are deliberately shallow.  They find comments, strings, numbers,
// keywords and command flags, and they do not attempt to parse anything.  A
// documentation listing is read, not compiled, and those five categories are
// what a reader's eye separates on.

#include "include/algo.h"
#include "include/doc.h"

// Return true when WORD appears in the comma-separated list CSV.
static bool KeywordQ(algo::strptr word, algo::strptr csv) {
    bool ret = false;
    ind_beg(algo::Sep_curs, token, csv, ',') {
        ret = ret || token == word;
    }ind_end;
    return ret;
}

// Return true when CH can appear inside an identifier.
static bool WordCharQ(char ch) {
    return algo_lib::AlphaCharQ(ch) || algo_lib::DigitCharQ(ch) || ch == '_';
}

// Append the run of TEXT from BEGIN to END to RUN as cells carrying ATTR and
// COLOR.  Every coloured run in this file goes through here.
static void Run(algo::strptr text, int begin, int end, doc::Attr attr, doc::Color color, doc::FRun &run) {
    doc::PutRunText(run, algo::strptr(text.elems + begin, i32_Max(0, end - begin)), attr, color, 0);
}

// Append the span of TEXT from BEGIN to END to RUN as a flag, leading to the heading that
// documents it when the page carries one.
//
// A tool's README opens with the usage its own command line prints, and every option in
// that block is written about further down under a heading of its own -- so the flag a
// reader is looking at and the paragraph explaining it are on one page, a screen apart,
// with nothing joining them.  The heading's anchor is the flag, which is what lets the
// block link to it without either half knowing about the other.
static void Flagrun(algo::strptr text, int begin, int end, doc::Color color, doc::FRun &run) {
    algo::strptr flag(text.elems + begin, i32_Max(0, end - begin));
    doc::PutRunText(run, flag, doc::Attr(doc_Attr_plain), color, doc::NewLink(doc::Headanchor(flag)));
}

// Return the offset just past the string literal that opens at TEXT[POS] with
// the quote character QUOTE.  A backslash escapes the character after it, so a
// quote inside a literal does not end it.
//
// A quote that is not closed before the line ends opens nothing, and POS comes
// back to say so.  Most of what these lexers see is prose rather than code -- a
// bare fence reaches the shell lexer, and those are mostly help output -- and
// prose is full of apostrophes: "don't", "the tool's own README", "filename, -
// for stdin".  Scanning on past the line ending painted every remaining line of
// the block as a string; stopping at the line ending and colouring nothing when
// nothing closed leaves prose looking like prose.
static int StringEnd(algo::strptr text, int pos, char quote) {
    int i = pos + 1;
    bool closed = false;
    bool done = false;
    while (i < ch_N(text) && !done) {
        if (text.elems[i] == '\n') {
            done = true;
        } else if (text.elems[i] == '\\') {
            i += 2;
        } else if (text.elems[i] == quote) {
            closed = true;
            done = true;
            i++;
        } else {
            i++;
        }
    }
    return closed ? i32_Min(i, ch_N(text)) : pos;
}

// Return the offset just past the identifier that starts at TEXT[POS].
static int WordEnd(algo::strptr text, int pos) {
    int i = pos;
    while (i < ch_N(text) && WordCharQ(text.elems[i])) {
        i++;
    }
    return i;
}

// Colour TEXT as c++ into RUN.
static void HighlightCpp(algo::strptr text, doc::FRun &run) {
    doc::Attr none(doc_Attr_plain);
    doc::Attr dim(doc_Attr_dim);
    doc::Color plain(doc_Color_default);
    doc::Color green(doc_Color_green);
    doc::Color amber(doc_Color_amber);
    doc::Color blue(doc_Color_blue);
    doc::Color cyan(doc_Color_cyan);
    doc::Color yellow(doc_Color_yellow);
    doc::Color magenta(doc_Color_magenta);
    algo::strptr keyword = "alignas,auto,bool,break,case,catch,char,class,const,constexpr,continue"
        ",default,delete,do,double,else,enum,explicit,extern,false,float,for,friend,goto,if"
        ",inline,int,long,namespace,new,nullptr,operator,private,protected,public,return"
        ",short,signed,sizeof,static,struct,switch,template,this,throw,true,try,typedef"
        ",typename,union,unsigned,using,virtual,void,volatile,while,i8,i16,i32,i64,u8,u16"
        ",u32,u64,strptr,tempstr,cstring,NULL";
    int i = 0;
    while (i < ch_N(text)) {
        char ch = text.elems[i];
        char next = i + 1 < ch_N(text) ? text.elems[i + 1] : '\0';
        int bol = i == 0 || text.elems[i - 1] == '\n' ? i : -1;
        int strend = ch == '"' || ch == '\'' ? StringEnd(text, i, ch) : i;
        if (ch == '/' && next == '/') {
            int end = doc::FindClose(text, i, "\n");
            Run(text, i, end, none, green, run);
            i = end;
        } else if (ch == '/' && next == '*') {
            int end = i32_Min(ch_N(text), doc::FindClose(text, i + 2, "*/") + 2);
            Run(text, i, end, none, green, run);
            i = end;
        } else if (strend > i) {
            Run(text, i, strend, none, amber, run);
            i = strend;
        } else if (bol >= 0 && ch == '#') {
            int end = doc::FindClose(text, i, "\n");
            Run(text, i, end, none, magenta, run);
            i = end;
        } else if (algo_lib::DigitCharQ(ch)) {
            int end = WordEnd(text, i);
            Run(text, i, end, none, cyan, run);
            i = end;
        } else if (WordCharQ(ch)) {
            int end = WordEnd(text, i);
            algo::strptr word(text.elems + i, end - i);
            Run(text, i, end, none, KeywordQ(word, keyword) ? blue : plain, run);
            i = end;
        } else {
            Run(text, i, i + 1, none, plain, run);
            i++;
        }
    }
}

// Return true when a command flag starts at TEXT[POS]: one or two dashes at a
// word boundary, with a word character after them.
//
// The last condition is what keeps the dash in "filename, - for stdin" plain.
// A lone dash in a sentence is punctuation, and colouring it as an option says
// the sentence contains one.
static bool FlagQ(algo::strptr text, int pos) {
    char prev = pos > 0 ? text.elems[pos - 1] : ' ';
    bool edge = prev == ' ' || prev == '\n' || prev == '\t';
    int dash = pos + 1 < ch_N(text) && text.elems[pos + 1] == '-' ? pos + 2 : pos + 1;
    return text.elems[pos] == '-' && edge && dash < ch_N(text) && WordCharQ(text.elems[dash]);
}

// Colour TEXT as bash into RUN.
//
// There is no keyword list here, and that is deliberate.  Shell keywords are
// ordinary English words -- for, in, done, exit, echo, read, set -- and most
// fenced blocks in this corpus are not shell at all: they are help output,
// tuples and captured runs, written with a bare fence and so reaching this
// lexer by default.  Colouring "for" inside the sentence "Input directory or
// filename, - for stdin" states that the word is a keyword, which is false, and
// it is false on far more lines than a keyword list is ever right on.
//
// A comment is green and a string amber, which is the pairing every editor a reader of this
// tree has used draws them in.  Two of one colour would say the two are one thing.
//
// What is left cannot be mistaken for prose: a comment, a quoted string, a
// variable, and a flag.  The flag is the one that earns its place in a
// repository whose documentation is mostly command lines -- a reader scanning
// for how a tool is invoked is looking for its options, and giving those their
// own colour turns a wall of shell into something with landmarks.
static void HighlightBash(algo::strptr text, doc::FRun &run) {
    doc::Attr none(doc_Attr_plain);
    doc::Attr dim(doc_Attr_dim);
    doc::Color plain(doc_Color_default);
    doc::Color green(doc_Color_green);
    doc::Color amber(doc_Color_amber);
    doc::Color cyan(doc_Color_cyan);
    doc::Color yellow(doc_Color_yellow);
    doc::Color magenta(doc_Color_magenta);
    int i = 0;
    while (i < ch_N(text)) {
        char ch = text.elems[i];
        char prev = i > 0 ? text.elems[i - 1] : ' ';
        int strend = ch == '"' || ch == '\'' ? StringEnd(text, i, ch) : i;
        if (ch == '#' && (i == 0 || prev == ' ' || prev == '\n' || prev == '\t')) {
            int end = doc::FindClose(text, i, "\n");
            Run(text, i, end, none, green, run);
            i = end;
        } else if (strend > i) {
            Run(text, i, strend, none, amber, run);
            i = strend;
        } else if (ch == '$') {
            int end = i + 1 < ch_N(text) && text.elems[i + 1] == '{' ? doc::FindClose(text, i, "}") + 1 : WordEnd(text, i + 1);
            Run(text, i, i32_Min(end, ch_N(text)), none, cyan, run);
            i = i32_Min(end, ch_N(text));
        } else if (FlagQ(text, i)) {
            int dash = i + 1 < ch_N(text) && text.elems[i + 1] == '-' ? i + 2 : i + 1;
            int end = WordEnd(text, dash);
            Flagrun(text, i, end, yellow, run);
            i = end;
        } else {
            int end = WordCharQ(ch) ? WordEnd(text, i) : i + 1;
            Run(text, i, end, none, plain, run);
            i = end;
        }
    }
}

// Colour the fenced block TEXT, whose fence named the language LANG, into RUN.
// A language with no lexer here prints uncoloured, so nothing is claimed about
// code the tool cannot read.
//
// `usage` is a program's own help, and it reaches the shell lexer because of the four
// things that lexer marks -- a comment, a string, a variable and a flag -- a usage block
// holds only flags, and the flag is the whole of what a reader is looking at.
void doc::Highlight(algo::strptr lang, algo::strptr text, doc::FRun &run) {
    bool cpp = lang == "c++" || lang == "cpp" || lang == "c";
    bool bash = lang == "bash" || lang == "sh" || lang == "shell" || lang == "usage" || lang == "";
    doc::Attr plain(doc_Attr_plain);
    if (cpp) {
        HighlightCpp(text, run);
    } else if (bash) {
        HighlightBash(text, run);
    } else {
        doc::PutRunText(run, text, plain, doc::Color(doc_Color_default), 0);
    }
}

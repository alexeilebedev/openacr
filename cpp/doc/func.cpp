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
// Source: cpp/doc/func.cpp -- The functions and the generated code of a selection
//
// Reading the code that a page of records is about, and the access paths through it.
// A record says what a thing was declared to be, and the C++ that acts on it is
// somewhere else.  Finding that code by hand means knowing which namespace to
// look in and what the type ended up being called, which is two facts the record
// already carries.  So a query page has two further views of the same selection:
// f lists the functions and c shows what amc generated, and both are reached
// with one keystroke from the records.
// The function view is src_func's own answer, one row per prototype, and the file
// and line that open each row are a link into the source at the line the function
// was declared on.  Reading a record and reading its code is then one walk, with
// the same Backspace out of it.
// What both views need first is the subject of the page -- the things its records
// are about -- and that is read off each record's primary key.  A ctype row names
// a ctype, a row keyed by a field names the ctype the field belongs to, and a
// namespace row names a whole namespace.  Ctypes win over namespaces: the
// cross-reference closure of a ctype carries its namespace's record too, and a
// page asking about the dozen functions of one ctype must not be answered with
// the two thousand of its library.
// The closure is therefore what decides how wide the question is, and t is what a
// reader turns it with: the bare selection asks about one ctype, and the closure
// around it asks about every ctype it reaches.

#include "include/algo.h"
#include "include/doc.h"

// Return the offset of the first byte of LINE at or after POS that is not blank.
int doc::SkipSpace(algo::strptr line, int pos) {
    int i = pos;
    while (i < ch_N(line) && line.elems[i] == ' ') {
        i++;
    }
    return i;
}

// Record NAME as one of the things the page is about, unless it is there already.
static void SubjectAdd(algo::strptr name) {
    if (ch_N(name) > 0) {
        (void)doc::ind_subject_GetOrCreate(name);
    }
}

// Make the ctype NAME the one thing the page is about.
//
// A page about one ctype is about that ctype and nothing else.  Reading the subjects off
// the records instead would widen the question, because the closure of a ctype row
// reaches the FDb field that holds a pool of it -- and answering "what acts on this
// type" with the two thousand functions of its module is not an answer.
void doc::SubjectOne(algo::strptr name) {
    doc::subject_RemoveAll();
    doc::_db.subjectns = false;
    SubjectAdd(name);
}

// Return what the record whose primary key is ATTR is about -- a namespace when
// NS is true, a ctype when it is false -- or the empty string when the key names
// neither.
//
// What a primary key identifies is read off the tables this tool already loads.
// The value may be a ctype outright, as it is on a row of dmmeta.ctype.  Failing
// that, the part of it before its last dot may be one, which is what a row keyed
// by a field carries: a field row, a cascdel row, an xref row and a llist row all
// name lib_x2.FStream.c_appender, and all four therefore describe lib_x2.FStream.
// A key with no dot in it may be a namespace.
//
// Only the primary key is read, and that is what holds the page's subject to what
// its records are rather than to everything they mention.  A field row names its
// arg as well, so a subject that grew to cover every attribute would answer a
// question about one ctype with the functions of the twenty types its fields
// happen to be.
static tempstr SubjectOf(algo::strptr attr, bool ns) {
    int colon = algo::FindChar(attr, ':');
    algo::strptr value = colon >= 0 ? doc::RestFrom(attr, colon + 1) : algo::strptr();
    algo::strptr ctype = Pathcomp(value, ".RL");
    bool own = doc::ind_ctype_Find(value) != NULL;
    bool prefix = !own && ctype != value && doc::ind_ctype_Find(ctype) != NULL;
    bool nsown = doc::ind_ns_Find(value) != NULL;
    tempstr ret;
    if (ns && nsown) {
        ret << value;
    } else if (!ns && own) {
        ret << value;
    } else if (!ns && prefix) {
        ret << ctype;
    }
    return ret;
}

// Add whatever the acr answer OUT says its records are about, taking namespaces
// when NS is true and ctypes when it is false.
//
// A record occupies one line, its table first and its primary key second, and a
// line of a cross-reference tree is indented by however deep the record sits.  A
// line acr wrote for some other reason -- its closing report, a complaint about
// the query -- has a second word that names no ctype and no namespace, so it
// contributes nothing.
static void SubjectScan(algo::strptr out, bool ns) {
    ind_beg(algo::Line_curs, line, out) {
        int tagend = doc::TokenEnd(line, doc::SkipSpace(line, 0));
        int attrbeg = doc::SkipSpace(line, tagend);
        int attrend = doc::TokenEnd(line, attrbeg);
        algo::strptr attr(line.elems + attrbeg, attrend - attrbeg);
        SubjectAdd(SubjectOf(attr, ns));
    }ind_end;
}

// Read off the records of the acr answer OUT the things the page is about.
//
// A page names either ctypes or namespaces and never a mixture, because the
// namespaces are collected only when no ctype was found.  A record page opened on
// a ctype carries the ctype's own namespace row in its closure, so a page whose
// subject is one ctype would otherwise also be a page whose subject is the whole
// library that declares it.  What is left for the namespace pass is the page that
// names no ctype at all, which is what a page of dmmeta.ns records is.
void doc::SubjectRead(algo::strptr out) {
    doc::subject_RemoveAll();
    SubjectScan(out, false);
    bool ctype = doc::subject_N() > 0;
    doc::_db.subjectns = !ctype;
    if (!ctype) {
        SubjectScan(out, true);
    }
}

// Return the sql regx that selects the functions of the page's subjects: every
// function of any subject namespace, or every prototype naming any subject ctype.
//
// A prototype in this tree is printed with its types fully qualified, so matching
// lib_x2::FStream against the prototype finds the functions of every namespace
// that takes the type and not only those of the one that declares it.  The name of
// the function is part of its prototype too, which is what also brings in the
// functions named after the type without taking one.
//
// The regx takes alternation, so one pattern covers a page naming several
// subjects and one src_func run answers the whole page however many there are.
static tempstr SubjectRegx() {
    tempstr ret;
    int i = 0;
    ret << (doc::_db.subjectns ? "(" : "%(");
    ind_beg(doc::_db_subject_curs, subject, doc::_db) {
        tempstr name;
        if (doc::_db.subjectns) {
            name << subject.subject;
        } else {
            name << Pathcomp(subject.subject, ".LL") << "::" << Pathcomp(subject.subject, ".LR");
        }
        ret << (i > 0 ? "|" : "") << name;
        i++;
    }ind_end;
    ret << (doc::_db.subjectns ? ").%" : ")%");
    return ret;
}

// Return the selection that confines a function scan to the page's subjects, as one regx.
//
// src_func keys a function by its namespace and its name, so naming the namespace selects
// every function written in it.  A subject that is a namespace is its own namespace, and a
// subject that is a ctype carries it before the dot, so one expression covers both; two
// ctypes of one namespace name it twice, which a regx does not mind.
//
// A constructor is keyed differently and has to be asked for by name.  src_func reads the
// key off the C++ name, and `doc::FAnchor::FAnchor` gives up `FAnchor.FAnchor` with the
// namespace gone -- so a ctype's own leaf name joins the alternation beside its namespace,
// or the page about a type would list every accessor and not the function that builds one.
static tempstr SubjectSel() {
    tempstr ret;
    int i = 0;
    ret << "(";
    ind_beg(doc::_db_subject_curs, subject, doc::_db) {
        ret << (i > 0 ? "|" : "") << Pathcomp(subject.subject, ".LL");
        if (!doc::_db.subjectns) {
            ret << "|" << Pathcomp(subject.subject, ".LR");
        }
        i++;
    }ind_end;
    ret << ").%";
    return ret;
}

// Return the src_func command line that lists the functions of the page's subjects,
// spelled so that a reader can paste it into a shell.
//
// A ctype's functions are the ones whose prototype names it, which is what `-matchproto`
// asks.  Nothing narrows by a function's own name: an accessor amc wrote for a field is
// named after the field, and such a filter cut the list to a fraction of itself.
//
// Nothing narrows by target either.  Doing so assumed a namespace's sources sit in a
// target of the same name, and that is false wherever the code lives elsewhere -- `algo`
// is written in `cpp/lib/algo` under another target, so `-targsrc:algo/%` matched nothing
// and both the namespace page and every algo ctype page reported no functions at all.
// src_func answers off an index either way, and the scan of the tree costs a tenth of a
// second where the scan of one target cost a hundredth.
//
// The comment above each function comes back as well, since that is where this repo says
// what calling it does.
//
// A ctype's generated functions are what a reader of the code section came for -- the
// accessors amc wrote are the interface of that type -- so the scan includes them.  A
// namespace's functions are the other question, and there the generated code is fifteen
// thousand functions nobody asked to read, so it stays out.
//
// A regex needs none of that.  The reader wrote what to match, so it is passed as it
// stands, and the generated sources are scanned along with the rest: the accessors amc
// wrote for a type are as much its functions as the hand-written ones, and a regex such as
// `algo.Smallstr100%` matches nothing outside them at all.
//
// Either way the scan is confined to the subject's own namespace.  A prototype match alone
// answers with every function anywhere that takes the type as a parameter, and for a
// message header that is six hundred functions across a dozen namespaces where the reader
// asked about one -- twenty-five hundred lines of code section under a page about a
// twelve-byte struct.  The box carries no selector to widen it back, and a section a
// reader cannot narrow had better not be the widest reading of the question.
tempstr doc::FuncCmdline(algo::strptr query) {
    bool regx = doc::FuncregxQ(tempstr() << "func:" << query);
    tempstr ret;
    ret << "bin/src_func";
    ret << " " << strptr_ToBash(regx ? query : SubjectSel());
    if (regx) {
        ret << " -gen";
    } else if (!doc::_db.subjectns) {
        ret << " -gen -matchproto:" << strptr_ToBash(SubjectRegx());
    }
    ret << " -in:" << strptr_ToBash(doc::_db.cmdline.in);
    ret << " -showcomment -showloc";
    return ret;
}

// Return the byte offset of the colon that closes the file position src_func's
// answer LINE opens with -- the second colon of `path:line: prototype` -- or -1
// when the line carries no position at all.
//
// A line src_func wrote for some other reason, a complaint or a report, carries
// none.  Both emitters cut the line at the same place, so a browser hangs its
// anchor on exactly the text a terminal underlines.
//
// A path of this tree carries no space, and that is the whole of what keeps a comment
// out of the answer: `// in algo_lib::_db.errtext` has two colons and is a sentence, and
// without the test it was read as the file `// in algo_lib` and offered as a link.
int doc::LocEnd(algo::strptr line) {
    int colon = algo::FindChar(line, ':');
    algo::strptr path = colon >= 0 ? algo::strptr(line.elems, colon) : algo::strptr();
    algo::strptr rest = colon >= 0 ? doc::RestFrom(line, colon + 1) : algo::strptr();
    int colon2 = algo::FindChar(rest, ':');
    bool spaced = algo::FindChar(path, ' ') >= 0;
    return colon >= 0 && colon2 >= 0 && !spaced ? colon + 1 + colon2 : -1;
}

// Return the number of functions the src_func answer ANSWER lists.  A line carrying a
// file position is one function; anything else src_func said is not.
int doc::NFunc(algo::strptr answer) {
    int ret = 0;
    ind_beg(algo::Line_curs, line, answer) {
        ret += doc::LocEnd(line) >= 0 ? 1 : 0;
    }ind_end;
    return ret;
}

// Lay one line of src_func's answer onto the page: the key amc generated the function
// from, then where it was declared and its prototype coloured as C++.  KEYED carries
// whether the line just laid down was the key, and is what this reads and sets.
//
// The link is on the key when there is one, and on the position when there is not.  A
// reader looking for a generated function is looking for `doc.FDb.anchor.Alloc`, which is
// what they have in hand -- the file and the line are where amc happened to put it this
// time, and a link on them says nothing about what is being opened.  Both lead to the same
// page, so a hand-written function, which carries no key, keeps its position as the link.
//
// A line carrying neither -- a complaint, a report -- is put down dim and whole, so that
// what src_func said reaches the page even when it said no.
static void PutFuncline(algo::strptr line, bool &keyed) {
    int end = doc::LocEnd(line);
    int colon = algo::FindChar(line, ':');
    algo::strptr path = end >= 0 ? algo::strptr(line.elems, colon) : algo::strptr();
    algo::strptr num = end >= 0 ? algo::strptr(line.elems + colon + 1, end - colon - 1) : algo::strptr();
    algo::strptr at = end >= 0 ? algo::strptr(line.elems, end) : algo::strptr();
    algo::strptr proto = end >= 0 ? doc::RestFrom(line, doc::SkipSpace(line, end + 1)) : algo::strptr();
    tempstr key(end < 0 ? doc::Funckey(line) : tempstr());
    tempstr keyloc(ch_N(key) > 0 ? doc::Funckeyloc(key) : tempstr());
    if (end >= 0) {
        tempstr target;
        target << "/" << path << ":" << num;
        int link = keyed ? 0 : doc::NewLink(target);
        doc::Attr attr(keyed ? doc_Attr_dim : doc_Attr_underline);
        doc::PutText(at, attr, doc::Color(doc_Color_default), link);
        doc::PutText("  ", doc::Attr(doc_Attr_plain), doc::Color(doc_Color_default));
        doc::FRun &run = doc::run_Alloc();
        doc::Highlight("c++", proto, run);
        doc::PutRun(run, 0, i32(ary_N(run)));
        doc::run_RemoveAll();
        doc::EndRow();
        keyed = false;
    } else if (ch_N(keyloc) > 0) {
        doc::PutText("// --- ", doc::Attr(doc_Attr_dim), doc::Color(doc_Color_default));
        doc::PutText(key, doc::Attr(doc_Attr_underline), doc::Color(doc_Color_default), doc::NewLink(keyloc));
        doc::EndRow();
        keyed = true;
    } else {
        doc::PutNote(line);
    }
}

// Run acr for QUERY, read off what its records are about, and lay src_func's
// answer for those subjects on the page.
//
// The page opens with the src_func line that produced it, and that line names
// every subject in its regx -- so what the page is about is on the page, and the
// reader can paste the line into a shell to ask src_func something else about the
// same set.
//
// The selection is asked plainly, with no closure around it.  A reader who names one
// ctype is asking about that ctype's functions, and the closure of a ctype row reaches
// its namespace, its table and the arg of every field -- so the closure would answer
// with the functions of twenty types the reader did not name.
void doc::RenderFunc(algo::strptr query) {
    doc::SubjectRead(doc::AcrFlat(query));
    doc::RenderFuncset(query);
}

// Lay src_func's answer about the subjects already recorded onto the page.  QUERY names
// them, and is on the page only so that a page about nothing can say what it asked.
void doc::RenderFuncset(algo::strptr query) {
    bool have = doc::FuncregxQ(tempstr() << "func:" << query) || doc::subject_N() > 0;
    tempstr cmd(have ? doc::FuncCmdline(query) : tempstr());
    tempstr head;
    if (have) {
        head << cmd;
    } else {
        head << "acr " << query << "   selects no ctype and no namespace";
    }
    tempstr out(have ? doc::ToolOut(cmd) : tempstr());
    if (have) {
        doc::PutNote(tempstr() << doc::Count(doc::NFunc(out), "function") << "   " << doc::Querytext(head));
    } else {
        doc::PutNote(doc::Querytext(head));
    }
    bool keyed = false;
    ind_beg(algo::Line_curs, line, out) {
        PutFuncline(line, keyed);
    }ind_end;
}

// Return the chunk of a generated header that declares CTYPE, with PATH set to
// the header it came from and AT to the line it begins at.  The chunk is empty
// when the header declares no such ctype, and PATH is empty as well when CTYPE is
// a builtin type, which belongs to no namespace and so to no generated header.
//
// amc divides a generated header into sections and opens each one with a comment naming
// what the section is for, so the span from `// --- lib_x2.FStream` onward is that ctype's
// generated code: where the records live, the struct itself, and then the accessor of every
// field of it.
//
// The chunk ends where the struct does.  What follows it is functions, and functions belong
// in the list src_func builds, where each is a line a reader can follow to its source --
// rather than a wall of text nothing in which can be clicked.
tempstr doc::CodeChunk(algo::strptr ctype, algo::cstring &path, int &at) {
    algo::strptr ns = Pathcomp(ctype, ".LL");
    ch_RemoveAll(path);
    if (ns != ctype) {
        path << "include/gen/" << ns << "_gen.h";
    }
    tempstr open;
    open << "// --- " << ctype;
    tempstr ret;
    int row = 0;
    bool done = false;
    at = -1;
    if (ch_N(path) > 0 && algo::FileQ(path)) {
        ind_beg(algo::FileLine_curs, line, path) {
            row++;
            bool opens = at < 0 && line == algo::strptr(open);
            bool shuts = at >= 0 && !opens && (StartsWithQ(line, "// --- ") || line == "};");
            at = opens ? row : at;
            if (at >= 0 && !done) {
                ret << line << eol;
            }
            done = done || shuts;
        }ind_end;
    }
    return ret;
}

// Lay the generated declaration of CTYPE onto the page, or one row saying that
// its namespace's generated header carries none.
//
// amc divides a generated header into sections and opens each one with a comment
// naming what the section is for, so the span from `// --- lib_x2.FStream` to the
// next such comment is exactly that ctype's generated code: where the records
// live, the struct itself, and the accessor of every field of it.  Reading amc's
// own boundary is what keeps the extraction one rule, where matching the struct
// and guessing at where its accessors stop would be a list of the shapes amc
// emits and would fall behind the next shape it learns.
void doc::RenderCodeOne(algo::strptr ctype) {
    tempstr path;
    int at = 0;
    tempstr chunk(doc::CodeChunk(ctype, path, at));
    tempstr note;
    if (ch_N(chunk) > 0) {
        tempstr target;
        target << "/" << path << "#L" << at;
        doc::PutText(tempstr() << path << ":" << at, doc::Attr(doc_Attr_underline), doc::Color(doc_Color_default), doc::NewLink(target));
        doc::EndRow();
        doc::FRun &run = doc::run_Alloc();
        doc::Highlight("c++", chunk, run);
        doc::PourRun(run, 0);
        doc::run_RemoveAll();
    } else if (ch_N(path) > 0) {
        note << path << "   declares no " << ctype;
    } else {
        note << ctype << "   is a builtin type, and amc generates nothing for one";
    }
    if (ch_N(note) > 0) {
        doc::PutNote(note);
    }
}

// Lay one row naming the generated header of namespace NS, as a link to the whole
// of it.  A namespace has no one declaration -- its header is thousands of them --
// so the page offers the file rather than extracting anything from it.
static void PutCodefile(algo::strptr ns) {
    tempstr path;
    path << "include/gen/" << ns << "_gen.h";
    doc::PutNote(tempstr() << "the generated header of " << ns << ", whole");
    doc::PutText(path, doc::Attr(doc_Attr_underline), doc::Color(doc_Color_default), doc::NewLink(tempstr() << "/" << path));
    doc::EndRow();
}

// Run acr for QUERY, read off which ctypes its records are about, and lay what amc
// generated for each of them onto the page.
//
// A record page says what a field was declared as; what a reader wants next is
// the accessor amc made of it, and that is a chunk of one generated header rather
// than anything acr can answer.  The chunks follow one another with a blank row
// between, and each opens with the file and line it was taken from, as a link --
// so a reader who wants the surroundings can have the whole header instead.
void doc::RenderCode(algo::strptr query) {
    doc::SubjectRead(doc::AcrFlat(query));
    if (doc::subject_N() == 0) {
        doc::PutNote(tempstr() << "acr " << query << "   selects no ctype and no namespace");
    }
    ind_beg(doc::_db_subject_curs, subject, doc::_db) {
        if (doc::_db.subjectns) {
            PutCodefile(subject.subject);
        } else {
            doc::RenderCodeOne(subject.subject);
        }
        doc::EndRow();
    }ind_end;
}

// Return the src_func command line that answers about the function NAME, spelled so
// that a reader can paste it into a shell.
//
// It asks for tuples rather than for a listing, because what the page needs is where
// the function begins and ends and those are attributes of a record.  Reading them off
// a line of prose meant for a person is the shape that breaks when the prose changes.
static tempstr FuncsrcCmdline(algo::strptr name, algo::strptr src, int at) {
    command::src_func src_func;
    tempstr func;
    src_func.in = doc::_db.cmdline.in;
    if (at > 0) {
        func << "%";
    } else if (doc::AmckeyQ(name)) {
        func << "%";
        (void)command::matchcomment_ReadStrptrMaybe(src_func, tempstr() << "%--- " << name << "%");
    } else if (ch_N(src) > 0) {
        func << "%." << name;
    } else {
        func << name;
    }
    if (ch_N(src) > 0) {
        (void)command::targsrc_ReadStrptrMaybe(src_func, tempstr() << "%/" << src);
    }
    (void)command::func_ReadStrptrMaybe(src_func, func);
    src_func.gen = true;
    src_func.printssim = true;
    src_func.showcomment = true;
    return command::src_func_ToCmdline(src_func);
}

// Return true when the function beginning at line AT of the file at PATH is the one amc
// generated from KEY.
//
// amc opens the comment above each thing it generates with `// --- <key>`, so the nearest
// such marker above a function is the key that function came from, and reading it is how a
// key is turned back into a function.  It cannot be done by the regx that fetched the
// candidates: `doc.FDb.anchor.Alloc` is a prefix of `doc.FDb.anchor.AllocMaybe`, so a
// substring match over the comments answers with both.
static bool MarkedQ(algo::strptr path, int at, algo::strptr key) {
    tempstr want;
    want << "// --- " << key;
    int row = 0;
    bool seen = false;
    bool ret = false;
    if (algo::FileQ(path)) {
        ind_beg(algo::FileLine_curs, line, path) {
            row++;
            if (StartsWithQ(algo::Trimmed(line), "// --- ")) {
                seen = algo::Trimmed(line) == algo::strptr(want);
            }
            if (row == at) {
                ret = seen;
            }
        }ind_end;
    }
    return ret;
}

// Return the line NAME spells when it is a number, and zero when it is not.
//
// A function is named either by what it is called or by the line it starts on, and the
// second spelling is the one a link can carry without parsing a prototype for a name.
static int LineOf(algo::strptr name) {
    bool digits = ch_N(name) > 0;
    frep_(i, ch_N(name)) {
        digits = digits && name.elems[i] >= '0' && name.elems[i] <= '9';
    }
    return digits ? algo::ParseI32(name, 0) : 0;
}

// Return true when LINE is a comment and nothing else.
static bool CommentlineQ(algo::strptr line) {
    int at = doc::SkipSpace(line, 0);
    return at + 1 < ch_N(line) && line.elems[at] == '/' && line.elems[at + 1] == '/';
}

// Return the line of TEXT the comment block ending just above line AT begins at, or AT
// itself when the line above it is not a comment.
//
// Every function in this tree is introduced by a comment saying what calling it does
// and what it achieves, so that block is half of what a reader came for and src_func
// reports the line below it.  The run ends at the first line that is not a comment,
// which keeps the function above from being dragged in with it.
static int CommentTop(algo::strptr text, int at) {
    int n = 0;
    int top = 0;
    int ret = at;
    ind_beg(algo::Line_curs, line, text) {
        n++;
        top = CommentlineQ(line) ? (top > 0 ? top : n) : 0;
        if (n == at - 1) {
            ret = top > 0 ? top : at;
        }
    }ind_end;
    return ret;
}

// Return the lines FIRST through LAST of TEXT, both included.
static tempstr Lines(algo::strptr text, int first, int last) {
    int n = 0;
    tempstr ret;
    ind_beg(algo::Line_curs, line, text) {
        n++;
        if (n >= first && n <= last) {
            ret << line << eol;
        }
    }ind_end;
    return ret;
}

// Ask src_func where the function NAME lives and read its source into _db.funcsrc.
//
// NAME is either what the function is called or the line it starts on, and a link built
// from a listing carries the second, since a line is a fact src_func already reported
// while a name would have to be parsed back out of a prototype.  SRC narrows the
// question to one file and is empty when the location named the function alone: a key
// is a target and a name with a dot between them and is unique in the tree, while a
// bare name is not, so the spelling carrying a file is what says which of two static
// helpers of one name is meant.
//
// The record ends up empty when no function answers, and `src` is what says so.  What
// it holds otherwise is the comment that introduces the function, its prototype and its
// body, and none of the rest of the file: a prototype says what to call, a body says
// what it does, and the file around them is thousands of lines nobody asked for.
void doc::ReadFuncsrc(algo::strptr name, algo::strptr src) {
    int want = LineOf(name);
    bool bykey = doc::AmckeyQ(name);
    doc::Funcsrc &funcsrc = doc::_db.funcsrc;
    funcsrc = doc::Funcsrc();
    funcsrc.cmd << FuncsrcCmdline(name, src, want);
    tempstr out(doc::ToolOut(funcsrc.cmd));
    ind_beg(algo::Line_curs, answer, out) {
        algo::Tuple tuple;
        int rowline = 0;
        int rowend = 0;
        tempstr rowsrc;
        bool row = ch_N(funcsrc.src) == 0 && algo::Tuple_ReadStrptrMaybe(tuple, answer)
            && tuple.head.value == "src_func.func";
        if (row) {
            ind_beg(algo::Tuple_attrs_curs, attr, tuple) {
                if (attr.name == "src") {
                    rowsrc << attr.value;
                } else if (attr.name == "line") {
                    (void)i32_ReadStrptrMaybe(rowline, attr.value);
                } else if (attr.name == "endline") {
                    (void)i32_ReadStrptrMaybe(rowend, attr.value);
                }
            }ind_end;
        }
        bool take = row && rowline > 0 && rowend >= rowline && (want == 0 || rowline == want);
        if (take && bykey) {
            take = MarkedQ(rowsrc, rowline, name);
        }
        if (take) {
            funcsrc.src << rowsrc;
            funcsrc.line = u32(rowline);
            funcsrc.endline = u32(rowend);
        }
    }ind_end;
    bool found = ch_N(funcsrc.src) > 0 && algo::FileQ(funcsrc.src);
    tempstr text(found ? algo::FileToString(funcsrc.src, algo::FileFlags()) : tempstr());
    if (found) {
        funcsrc.first = u32(CommentTop(text, i32(funcsrc.line)));
        funcsrc.text << Lines(text, i32(funcsrc.first), i32(funcsrc.endline));
    } else {
        ch_RemoveAll(funcsrc.src);
    }
}

// Lay the source of the function NAME onto the page, under a link to the file at the
// line the function starts on.
//
// The listing carries the file's own line numbers, so a link naming L and a number
// lands on the line it names here exactly as it does on the page showing the whole
// file.  The head is a link, so a reader who wants the surroundings after all is one
// Enter from them.
void doc::RenderFuncsrc(algo::strptr name, algo::strptr src) {
    doc::ReadFuncsrc(name, src);
    doc::Funcsrc &funcsrc = doc::_db.funcsrc;
    if (ch_N(funcsrc.src) > 0) {
        tempstr target;
        target << "/" << funcsrc.src << "#L" << funcsrc.line;
        tempstr at;
        at << funcsrc.src << ":" << funcsrc.line;
        doc::PutText(at, doc::Attr(doc_Attr_underline), doc::Color(doc_Color_default), doc::NewLink(target));
        doc::PutText("   ", doc::Attr(doc_Attr_plain), doc::Color(doc_Color_default));
        doc::PutText(doc::Querytext(funcsrc.cmd), doc::Attr(doc_Attr_dim), doc::Color(doc_Color_default));
        doc::EndRow();
        doc::ParseListingText(funcsrc.text, doc::FileLang(funcsrc.src));
        doc::RenderDoc();
        doc::_db.listbase = funcsrc.first;
    } else {
        doc::PutNote(tempstr() << "no function of that name   " << doc::Querytext(funcsrc.cmd));
    }
}

// Return the amc_vis command line that draws the access paths of the page's subjects.
//
// A drawing of one ctype shows what points at it and what it points at, so a page about
// several of them wants one drawing holding all of them: a regx naming every subject is
// one run rather than one per subject, and the edges between two subjects are then drawn
// once instead of twice.
static tempstr VisCmdline() {
    command::amc_vis amc_vis;
    tempstr regx;
    int i = 0;
    regx << "(";
    ind_beg(doc::_db_subject_curs, subject, doc::_db) {
        regx << (i > 0 ? "|" : "") << subject.subject;
        i++;
    }ind_end;
    regx << (doc::_db.subjectns ? ").%" : ")");
    amc_vis.in = doc::_db.cmdline.in;
    (void)command::ctype_ReadStrptrMaybe(amc_vis, regx);
    return command::amc_vis_ToCmdline(amc_vis);
}

// Read what the page at LOC is about and return amc_vis's drawing of the access paths of
// those ctypes, with CMD set to the command line that produced it.
//
// A word of the drawing that names a ctype is drawn as a link to that ctype's page: what
// amc_vis draws is a graph of ctypes, so nearly every word in it is one a reader wants to
// open, and a drawing nobody can follow is a picture of the thing rather than a way in.
//
// The drawing is empty when the records name no ctype and no namespace, and that is
// what says the page has no access paths to show rather than none being drawn.
tempstr doc::VisOut(algo::strptr loc, algo::cstring &cmd) {
    if (doc::CtypeQ(loc)) {
        doc::SubjectOne(Pathcomp(loc, ":LR"));
    } else {
        doc::SubjectRead(doc::AcrFlat(doc::SectionQuery(loc)));
    }
    bool have = doc::subject_N() > 0;
    tempstr ret;
    if (have) {
        cmd << VisCmdline();
        ret << doc::ToolOut(cmd);
    }
    return ret;
}

// Lay the access paths of the ctypes the page at LOC is about onto the page.
//
// What amc_vis draws is preformatted: every column of it is placed, so it is poured
// line for line the way a listing is and nothing wraps it.  A page that is about no
// ctype says so instead, since an empty drawing and a drawing nobody asked for look
// the same on the glass.
void doc::RenderVis(algo::strptr loc) {
    tempstr cmd;
    tempstr out(doc::VisOut(loc, cmd));
    doc::EndRow();
    if (ch_N(out) > 0) {
        doc::PutNote(doc::Querytext(cmd));
        ind_beg(algo::Line_curs, line, out) {
            int i = 0;
            while (i < ch_N(line)) {
                int end = doc::WordEnd(line, i);
                algo::strptr word(line.elems + i, i32_Max(0, end - i));
                doc::FCtype *ctype = end > i ? doc::ind_ctype_Find(word) : NULL;
                algo::strptr text(end > i ? word : algo::strptr(line.elems + i, 1));
                int link = ctype ? doc::NewLink(tempstr() << "ctype:" << ctype->ctype) : 0;
                doc::Attr attr(link > 0 ? doc_Attr_underline : doc_Attr_plain);
                doc::PutText(text, attr, doc::Color(doc_Color_default), link);
                i += i32_Max(1, end - i);
            }
            doc::EndRow();
        }ind_end;
    } else {
        doc::PutNote("the records on this page name no ctype and no namespace");
    }
}

// Lay the two sections every page of records carries onto the page: the access paths of
// the ctypes it is about, and the code amc generated for them.
//
// Both are about whatever the page selected rather than about one kind of page, so they
// are laid out here, once, after every body that selects records.  A heading is drawn
// whether or not the section is open, because a reader who cannot see that the code is
// there has no reason to open it, and the heading is the link that opens it.
//
// A comptest's row brings its transcript, which is the file that test is checked against.
// Three things a reader of a comptest wants and each lives somewhere else: the row says
// when it runs and what it may cost, the transcript is what it asserted the last time
// somebody captured it, and the function is what it does.
//
// A page showing a whole table ends with two more: the table itself, and then the ctype it
// is an instance of.  A reader who typed a table's name asked what is in it, so the records
// come first and the schema is what they reach for after -- where the table sits is one
// line and the schema is the shape of every line above it.
//
// The prose somebody wrote about the table opens the first of the two.  Most of what there
// is to say about a table is computed, but not all of it, and what a person wrote is the
// half a reader cannot get anywhere else.
void doc::RenderSection(algo::strptr loc) {
    tempstr query(doc::SectionQuery(loc));
    tempstr table(doc::Datassimfile(loc));
    if (doc::SectionQ(loc)) {
        doc::EndRow();
        doc::PutFoldhead("Access Paths", "access-paths", "-vis", doc::_db.vis);
        if (doc::_db.vis) {
            doc::RenderVis(loc);
        }
        doc::EndRow();
        doc::PutFoldhead(ch_N(doc::Sectionns(loc)) > 0 ? "Functions" : "Code", "code", "-code", doc::_db.code);
        if (doc::_db.code) {
            doc::SubjectRead(doc::AcrFlat(query));
            ind_beg(doc::_db_subject_curs, subject, doc::_db) {
                if (doc::_db.subjectns) {
                    PutCodefile(subject.subject);
                } else {
                    doc::RenderCodeOne(subject.subject);
                }
                doc::EndRow();
            }ind_end;
            doc::RenderFuncset(query);
        }
    }
    if (ch_N(table) > 0) {
        doc::EndRow();
        doc::PutFoldhead("Data", "data", "-data", doc::_db.data);
        if (doc::_db.data) {
            doc::RenderQuery(tempstr() << table << ":%");
        }
    }
    tempstr transcript(doc::Rectranscript(loc));
    if (ch_N(transcript) > 0) {
        doc::EndRow();
        doc::PutFoldhead("Transcript", "transcript", "", true);
        if (algo::FileQ(transcript)) {
            doc::ParseListingText(algo::FileToString(transcript, algo::FileFlags()), "text");
            doc::RenderDoc();
        } else {
            doc::PutNote(tempstr() << transcript << " has not been captured");
        }
    }
    tempstr func(doc::Recfunc(loc));
    if (ch_N(func) > 0) {
        doc::EndRow();
        doc::PutFoldhead("As Function", "as-function", "", true);
        doc::RenderFuncsrc(func, "");
    }
    tempstr whole(doc::Wholetable(doc::LocQuery(loc)));
    if (ch_N(whole) > 0) {
        tempstr prose(doc::Ssimdoc(whole));
        doc::EndRow();
        doc::PutFoldhead("Ssimfile", "ssimfile", "", true);
        if (ch_N(prose) > 0) {
            doc::ParseText(prose);
            doc::RenderDoc();
            doc::EndRow();
        }
        doc::RenderFlat(tempstr() << "dmmeta.ssimfile:" << whole);
    }
}

// Lay the access paths of the ctypes REGX names onto the page.
//
// A page of its own, rather than the section a query page grows under the vis flag,
// because a reader sometimes wants the drawing and nothing else -- a whole namespace's
// paths at once, say, which no single record page is about.
void doc::RenderAccesspath(algo::strptr regx) {
    command::amc_vis amc_vis;
    amc_vis.in = doc::_db.cmdline.in;
    (void)command::ctype_ReadStrptrMaybe(amc_vis, regx);
    tempstr cmd(command::amc_vis_ToCmdline(amc_vis));
    tempstr out(doc::ToolOut(cmd));
    doc::PutNote(doc::Querytext(cmd));
    ind_beg(algo::Line_curs, line, out) {
        doc::PutText(line, doc::Attr(doc_Attr_plain), doc::Color(doc_Color_default));
        doc::EndRow();
    }ind_end;
}

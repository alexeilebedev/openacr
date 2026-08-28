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
// Source: cpp/doc/acr.cpp -- Rendering an acr query as a page
//
// Reading the ssim database the way the documents are read.
// A record is a page of its own here: `doc dmmeta.ctype:dmmeta.Ctype` runs acr,
// puts the answer on the page, and turns every attribute that names another
// record into a link.  Following one runs acr again on what it named, so a
// reader walks the database the same way they walk a citation, with the same
// keys and the same Backspace.
// Nothing here reformats acr's answer.  The lines are copied to the page as acr
// wrote them, which keeps its column alignment and a tree's indentation intact;
// what the walk adds is the boundary of each token, so a link can be hung on an
// attribute without the text being rebuilt.  What an attribute points at comes
// from the schema instead -- dmmeta.ssimfile and dmmeta.field are loaded as
// inputs of this tool, so the field an attribute names and the table its arg
// lives in are read from typed records rather than guessed at from the text.
// The colours are the ones the tree's emacs mode gives an ssimfile, because the
// same records are read in both places and a reader should not have to learn a
// second scheme: the type tag in pale blue, the name of an attribute in sky
// blue, its value in amber, and a quoted value in pale green.  Those are four
// indices of the 256-colour palette chosen for their nearness to the four hex
// colours conf/emacs.el names, which the eight-colour palette cannot reach.

#include "include/algo.h"
#include "include/doc.h"

// Return true when LOC is answered from the ssim database rather than from a
// document of the tree.
//
// A location the reader can be taken to is one of two things, and a colon is
// what tells them apart.  A query is written ssimfile:key -- the shape
// dmmeta.ctype:dmmeta.Ctype has -- while no document in this tree carries a
// colon in its path.  A web address carries one too, and NewLink refuses those
// before they can become a location at all.
bool doc::QueryQ(algo::strptr loc) {
    return algo::FindChar(loc, ':') >= 0;
}

// Return the word LOC opens with when doc synthesizes a page for that word, and the
// empty string when it does not.
//
// A location carrying a colon is answered from the ssim database, and the word before
// that colon says which answer.  A table's full name is a namespace and a name with a
// dot between them, so a word with no dot in it is not a table, and that is the whole
// of the discrimination: func:dmmeta.ctype:lib_x2.FStream is the page doc builds about
// those records, while dmmeta.func:x is the row acr answers with.
//
// The vocabulary is closed and every word in it has a page.  It is dev.docpage that says
// which words those are, so a page is a row and a function and nothing else -- a list
// repeated here would be a second place to edit, and the two would part company on the
// day somebody added a page and read only one of them.  A bare word doc does not know is
// a table's short name instead, which ExpandQuery turns into the full one, so nothing a
// reader can type is left without an answer.
tempstr doc::ViewWord(algo::strptr loc) {
    algo::strptr word = Pathcomp(loc, ":LL");
    bool bare = ch_N(word) < ch_N(loc) && algo::FindChar(word, '.') < 0;
    doc::FDocpage *docpage = bare ? doc::ind_docpage_Find(word) : NULL;
    tempstr ret;
    if (docpage && docpage->word) {
        ret << word;
    }
    return ret;
}

// Return the query the location LOC means when its word was handed a pattern, and the
// empty string when it was not.
//
// A word of the vocabulary answers about one thing: `ns:acr` is one namespace and the page
// doc writes about it, gathering that namespace's ctypes and sources and tests.  A pattern
// names a set instead, and there is no such page for a set -- what a reader who typed
// `ns:%` wants is every namespace, which is the rows of `dmmeta.ns` and an ordinary page of
// records.  Without this a pattern is looked up as though it were a name, and `ns:%`
// answers that the tree holds no namespace called `%`.
//
// Which words those are is `bytable` on their `dev.docpage` rows, so the vocabulary states
// its own rule.  A word's name is its table's short name where it has one, which is what
// `ExpandQuery` already knows, so the row says only whether that expansion is what a
// pattern means -- and it is not always: `func` expands to `dmmeta.func`, a table that has
// nothing to do with the functions `func:` is about.  A word with the mark off keeps its
// page whatever it is handed, which is what leaves `accesspath:acr.%` taking the regx it
// always took.
//
// A word that is a location on its own already answers about all of them, and `msg` is
// one: the bare word is every message header in the tree.  So `msg:%` is that word, and
// the summary has one name rather than two.
tempstr doc::Wordpattern(algo::strptr loc) {
    tempstr word(doc::ViewWord(loc));
    algo::strptr rest = Pathcomp(loc, ":LR");
    doc::FDocpage *docpage = ch_N(word) > 0 ? doc::ind_docpage_Find(word) : NULL;
    bool pattern = docpage && algo::FindChar(rest, '%') >= 0;
    tempstr ret;
    if (pattern && docpage->bytable) {
        ret << doc::ExpandQuery(loc);
    } else if (pattern && docpage->bare && rest == "%") {
        ret << word;
    }
    return ret;
}

// Return QUERY with a table's short name expanded to the full one, or QUERY as it
// stands when no single table answers to that name.
//
// acr takes a table's short name as well as its full one, so `acr ssimfile:dmmeta.ctype`
// answers.  A page has one canonical text, though, and two spellings of one location
// would give a reader two histories of the same walk -- so the short name is expanded
// here, and every location that reaches a link, a history or a URL carries the full one.
//
// Ten short names in the tree name two tables each, gitfile and target and user among
// them, and one of those is left exactly as the reader wrote it.  Choosing between two
// tables is acr's business rather than this tool's.
tempstr doc::ExpandQuery(algo::strptr query) {
    algo::strptr word = Pathcomp(query, ":LL");
    bool bare = ch_N(word) < ch_N(query) && algo::FindChar(word, '.') < 0;
    int nmatch = 0;
    tempstr full;
    if (bare) {
        ind_beg(doc::_db_ssimfile_curs, ssimfile, doc::_db) {
            if (doc::name_Get(ssimfile) == word) {
                nmatch++;
                ch_RemoveAll(full);
                full << ssimfile.ssimfile;
            }
        }ind_end;
    }
    tempstr ret;
    if (nmatch == 1) {
        ret << full << ":" << Pathcomp(query, ":LR");
    } else {
        ret << query;
    }
    return ret;
}


// Return the function that carries the record the location LOC selects, and the empty
// string when no function does.
//
// A table a dmmeta.gstatic row names is compiled into the program that reads it, and amc
// binds each of its rows to a function of that program.  The gstatic names the field those
// rows land in -- `atf_comp.FDb.comptest` -- and that field says both which program the
// function belongs to and what the functions of the table are called: the name of the
// field, an underscore, and the row's own key with its dots replaced.  So the comptest
// `doc.Fold` is `atf_comp.comptest_doc_Fold` and the x2test `s3queuebound` is
// `atf_x2.x2test_s3queuebound`.
//
// A dmmeta.dispatch_msg record is the other case, and its function is named after both
// halves of the key rather than read off a gstatic -- the record says which dispatch calls
// which message, and amc names the handler after the two.
//
// A location selecting a pattern rather than one key answers with the empty string.  A
// page of forty tests is about forty functions, and a section showing one of them would be
// picking a row out of the page for no reason the reader gave.
// Return the handler the dmmeta.dispatch_msg record KEY names, and the empty string when
// KEY is not one of those records.
//
// A dispatch_msg is a dispatch and a message with a slash between them, and amc names the
// function that dispatch calls after both: `x2txn.Shm/x2.MemberHbMsg` is handled by
// `x2txn::Shm_MemberHbMsg`.  The message's own namespace does not enter the name, since
// the dispatch is what says which program the handler belongs to.
static tempstr Dispatchfunc(algo::strptr key) {
    algo::strptr dispatch = Pathcomp(key, "/LL");
    algo::strptr msg = Pathcomp(key, "/LR");
    tempstr ret;
    if (msg != key && ch_N(dispatch) > 0 && ch_N(msg) > 0) {
        ret << Pathcomp(dispatch, ".LL") << "." << Pathcomp(dispatch, ".RR")
            << "_" << Pathcomp(msg, ".RR");
    }
    return ret;
}

tempstr doc::Recfunc(algo::strptr loc) {
    tempstr query(doc::LocQuery(loc));
    algo::strptr table = Pathcomp(query, ":LL");
    algo::strptr key = Pathcomp(query, ":LR");
    bool one = ch_N(key) > 0 && algo::FindChar(key, '%') < 0;
    doc::FSsimfile *ssimfile = one ? doc::ind_ssimfile_Find(table) : NULL;
    tempstr ret(one && table == "dmmeta.dispatch_msg" ? Dispatchfunc(key) : tempstr());
    ind_beg(doc::_db_gstatic_curs, gstatic, doc::_db) {
        doc::FField *field = ssimfile ? doc::ind_field_Find(gstatic.field) : NULL;
        doc::FField *base = field ? doc::ind_field_Find(tempstr() << field->arg << ".base") : NULL;
        if (base && base->arg == ssimfile->ctype) {
            ch_RemoveAll(ret);
            ret << Pathcomp(gstatic.field, ".LL") << "." << Pathcomp(gstatic.field, ".RR") << "_";
            frep_(i, ch_N(key)) {
                ret << (key.elems[i] == '.' ? '_' : key.elems[i]);
            }
        }
    }ind_end;
    return ret;
}

// Return the transcript the page at LOC is about, and the empty string when the page is
// about no single comptest.
//
// A comptest is a row of atfdb.comptest and its transcript is the file it is checked
// against, so a page selecting that one row is a page a reader wants the transcript on --
// whichever spelling they reached it by.  Reading it off the records rather than off the
// word in the location is what makes `comptest:doc.Fold` and `atfdb.comptest:doc.Fold` one
// page, and it is how the function section beside it already works.
//
// A selection matching a pattern answers with the empty string.  A page of forty tests is
// about forty transcripts, and showing one of them would be picking a row out of the page
// for no reason the reader gave.
tempstr doc::Rectranscript(algo::strptr loc) {
    tempstr query(doc::LocQuery(loc));
    algo::strptr key = Pathcomp(query, ":LR");
    bool one = Pathcomp(query, ":LL") == "atfdb.comptest" && ch_N(key) > 0
        && algo::FindChar(key, '%') < 0;
    tempstr ret;
    if (one) {
        ret << doc::ComptestFile(key);
    }
    return ret;
}

// Return the path of the transcript the comptest COMPTEST is checked against.
tempstr doc::ComptestFile(algo::strptr comptest) {
    tempstr ret;
    ret << "test/atf_comp/" << comptest;
    return ret;
}

// Return how many lines TEXT holds.
int doc::NLine(algo::strptr text) {
    int ret = 0;
    ind_beg(algo::Line_curs, line, text) {
        (void)line;
        ret++;
    }ind_end;
    return ret;
}

// Return the byte offset just past the word at POS in LINE, where a word is what a name in
// this tree is made of: letters, digits, underscores and the dot between a namespace and a
// type.
int doc::WordEnd(algo::strptr line, int pos) {
    int i = pos;
    while (i < ch_N(line) && (algo_lib::AlphaCharQ(line.elems[i]) || algo_lib::DigitCharQ(line.elems[i])
                              || line.elems[i] == '_' || line.elems[i] == '.')) {
        i++;
    }
    return i;
}

// Return the location the text of a code span names, or the empty string when it names
// nothing this tool can open.
//
// A word in backticks is the name of something more often than not, and in this tree those
// names are recognizable: `dmmeta.thash` is a table, `lib_x2.FStream` is a ctype, `amc` is a
// namespace, and `dmmeta.ctype:doc.FSpan` is a selection of records.  A table leads to its
// rows rather than to the page about it: a reader who wrote a table's name in a sentence is
// pointing at what is in it.  Each becomes a link
// wherever it is written, so a document carries them without an author spelling out a target
// -- which is most of what makes a reference page worth reading through this tool rather
// than reading as a file.
//
// A directory of the documentation is the fifth: `txt/rule` is where the rule files are,
// and a sentence that names it leads there.  It is tried last, so a name that is also a
// table or a namespace stays what it was.
//
// Everything else in backticks names nothing here and stays plain text: a command line, a
// C++ expression, an option, a file, a field of a struct.  Three tests are the whole of
// what keeps that true.  A span carrying a space is prose or a command line rather than a
// name.  A span with no colon has to be a table, a ctype or a namespace outright, and a
// ctype carries its namespace -- which is what keeps `u32`, a ctype and a word in half the
// sentences of this tree, from linking to the page that says it is builtin.  A span with a colon has to
// name a table or one of doc's own words before it, which is what leaves `arg:u32` and
// `-color:N` alone.
//
// Where a word of the vocabulary and a table answer to the same name, the word wins, as it
// does in a location a reader types.  `msg` is both -- doc's page about the protocols and
// the short name of `dmmeta.msg` -- and a span expanded to the table led to the row rather
// than to the page the same text opens at the command line.
tempstr doc::Codeloc(algo::strptr text) {
    bool spaced = algo::FindChar(text, ' ') >= 0 || ch_N(text) == 0;
    algo::strptr word = Pathcomp(text, ":LL");
    bool colon = !spaced && ch_N(word) < ch_N(text);
    bool own = colon && ch_N(doc::ViewWord(text)) > 0;
    tempstr full(colon && !own ? doc::ExpandQuery(text) : tempstr());
    algo::strptr name = colon ? Pathcomp(full, ":LL") : text;
    bool dotted = algo::FindChar(text, '.') >= 0;
    doc::FSsimfile *ssimfile = spaced ? NULL : doc::ind_ssimfile_Find(name);
    doc::FCtype *ctype = spaced || colon || !dotted ? NULL : doc::ind_ctype_Find(text);
    doc::FNs *ns = spaced || colon || dotted ? NULL : doc::ind_ns_Find(text);
    doc::FReadmedir *dir = spaced || colon ? NULL : doc::ind_readmedir_Find(text);
    tempstr ret;
    if (own) {
        ret << text;
    } else if (colon && ssimfile) {
        ret << full;
    } else if (ssimfile) {
        ret << ssimfile->ssimfile << ":%";
    } else if (ctype) {
        ret << "ctype:" << ctype->ctype;
    } else if (ns) {
        ret << "ns:" << ns->ns;
    } else if (dir) {
        ret << "/" << dir->readmedir << "/";
    }
    return ret;
}

// Record every directory the documentation has a document in, so that a code span naming
// one can be recognized as naming it.
//
// The list is the directories of dev.readmefile with the file taken off, each kept once,
// and it is built after the inputs are read because that is when the rows are all there.
// Reading it off that table rather than off the tree is the same rule the listings follow:
// what the reader is offered is what git tracks.
void doc::ReadmedirRead() {
    ind_beg(doc::_db_readmefile_curs, readmefile, doc::_db) {
        (void)doc::ind_readmedir_GetOrCreate(Pathcomp(readmefile.gitfile, "/RL"));
    }ind_end;
}

// Return true when LOC names the function view of a query: the C++ functions the
// query's records are about, rather than the records themselves.
//
// Everything that carries a location therefore carries a view for free: the history
// pushes and pops one, so Backspace leaves the function view, and -links reaches its
// links without a terminal.
bool doc::FuncQ(algo::strptr loc) {
    return doc::ViewWord(loc) == "func";
}

// Return the file LOC names when LOC names a function inside a particular file, and
// the empty string otherwise.
//
// A source file and a function name with a colon between them is how a reader asks for
// one function of a file they are already reading, and it is the spelling a link out of
// that file's own listing carries.  The test is that the text before the colon is a
// file of the tree, which no query and no other location can be.
tempstr doc::FuncsrcSrc(algo::strptr loc) {
    algo::strptr head = Pathcomp(loc, ":LL");
    tempstr ret;
    if (ch_N(head) < ch_N(loc) && algo::FileQ(head)) {
        ret << head;
    }
    return ret;
}

// Return the function LOC names, and the empty string when LOC names none.
//
// `func:` addresses functions, and what follows it says which.  A selection of records
// carries a colon; a ctype's name is a ctype this tool has a record of; and what is
// neither is a function's own key.  So `func:dmmeta.ctype:dmmeta.Ctype` and
// `func:dmmeta.Ctype` are both every function about that ctype, while
// `func:doc.HtmlNav` is that one function's source.  One word serves all three because
// all three are functions, and a file and a name is a second spelling of the last.
//
// Two keys name a set however singular they look, and both have to be refused here.  A
// key carrying `%` is a pattern, and `%` means the same thing in this tool that it means
// everywhere else in the tree.  A key that is a namespace outright names that
// namespace's functions.  Without those two tests `algo_lib.%` is read as one function's
// key, and since a key is looked up by running src_func and taking the first row it
// answered, the page shows one arbitrary function out of the 2,331 that matched.
tempstr doc::FuncsrcName(algo::strptr loc) {
    algo::strptr rest = Pathcomp(loc, ":LR");
    bool set = algo::FindChar(rest, '%') >= 0 || doc::ind_ns_Find(rest) != NULL;
    bool bykey = doc::FuncQ(loc) && algo::FindChar(rest, ':') < 0 && !set
        && !doc::ind_ctype_Find(rest);
    tempstr ret;
    if (bykey || ch_N(doc::FuncsrcSrc(loc)) > 0) {
        ret << rest;
    }
    return ret;
}

// Return true when KEY is an amc key naming one function.
//
// amc keys a ctype by a namespace and a name, a field by a ctype and a name, and a
// function by a field and a verb -- so the count of dots is what says which of the three
// a key is, and three of them is a function.  `doc.FDb.anchor.Alloc` is one, and so is
// `doc.FAnchor..Init`, whose empty middle is the ctype's own constructor.
bool doc::AmckeyQ(algo::strptr key) {
    return StartsWithQ(doc::Funckeyloc(key), "func:");
}

// Return the amc key the marker line LINE names, and the empty string when LINE is not a
// marker.
//
// amc opens the comment above each thing it generates with `// --- <key>`, so the marker
// is what says which key a chunk of generated code came from -- and a page showing that
// code has the key on it already, needing only to be made a link.
tempstr doc::Funckey(algo::strptr line) {
    algo::strptr text = algo::Trimmed(line);
    tempstr ret;
    if (StartsWithQ(text, "// --- ")) {
        ret << doc::RestFrom(text, 7);
    }
    return ret;
}

// Return the location the amc key KEY leads to, and the empty string when it leads
// nowhere.
//
// A key of three dots is a function and a key of one is a ctype; amc writes markers for
// neither a field nor an enum name, so those two are the whole of it.
tempstr doc::Funckeyloc(algo::strptr key) {
    int ndot = 0;
    frep_(i, ch_N(key)) {
        ndot += key.elems[i] == '.' ? 1 : 0;
    }
    tempstr ret;
    if (ndot == 3) {
        ret << "func:" << key;
    } else if (ndot == 1) {
        ret << "ctype:" << key;
    }
    return ret;
}

// Return true when LOC names the source of one function.
bool doc::FuncsrcQ(algo::strptr loc) {
    return ch_N(doc::FuncsrcName(loc)) > 0;
}

// Return true when LOC is a `func:` whose argument is a regex over function names.
//
// A function's name is arbitrary.  It may belong to a ctype, and then amc keys it by that
// ctype and a verb; it may be global, and then the namespace is all it shares with its
// neighbours.  So neither a ctype's name nor a namespace's constrains what a reader can
// ask for, and an argument carrying `%` is handed to src_func as it stands rather than
// resolved to a record first.  `algo.Smallstr100%` is the case that says so: it is a
// ctype's name and a prefix of thirty-one function keys, and resolving it either way --
// to the ctype, or to the namespace its first component names -- answers a question the
// reader did not ask.
//
// A colon means the argument is a selection of records instead, and those name their own
// subjects.
bool doc::FuncregxQ(algo::strptr loc) {
    algo::strptr rest = Pathcomp(loc, ":LR");
    return doc::FuncQ(loc) && algo::FindChar(rest, ':') < 0
        && algo::FindChar(rest, '%') >= 0;
}

// Return true when LOC names the generated-code view of a query: what amc emitted
// for the ctypes the query's records are about.
bool doc::CodeQ(algo::strptr loc) {
    return doc::ViewWord(loc) == "code";
}

// Return true when LOC names one ctype's own page.
bool doc::CtypeQ(algo::strptr loc) {
    return doc::ViewWord(loc) == "ctype";
}

// Return the acr query that selects the ctype NAME.
tempstr doc::CtypeQuery(algo::strptr name) {
    tempstr ret;
    ret << "dmmeta.ctype:" << name;
    return ret;
}

// Return the ctype page QUERY has one of, and the empty string when it selects
// anything but a single ctype.
//
// The entry exists on a records page so that a reader looking at one ctype's row can
// reach the page about that ctype, and it is absent from a page selecting many because
// there is no one ctype for it to lead to.
tempstr doc::CtypeLoc(algo::strptr query) {
    algo::strptr table = Pathcomp(query, ":LL");
    algo::strptr key = Pathcomp(query, ":LR");
    bool one = table == "dmmeta.ctype" && ch_N(key) > 0 && algo::FindChar(key, '%') < 0;
    tempstr ret;
    if (one) {
        ret << "ctype:" << key;
    }
    return ret;
}

// Return true when the page at LOC selects records from the ssim database.
//
// A document resolves to itself through LocQuery the way a query does, so the query
// alone does not say which of the two a location is; both tests together do.  Whatever
// selects records can be asked for its schema and drawn as access paths, and whatever
// does not must not carry those flags in its URL.
bool doc::RecordsQ(algo::strptr loc) {
    return doc::QueryQ(loc) && ch_N(doc::LocQuery(loc)) > 0;
}

// Return the namespace whose ctypes the sections of the page at LOC are about, and the
// empty string when the page is about no whole namespace.
//
// `ns:<name>` is the one page that is.  What makes the code section of such a page the
// namespace's own functions rather than one type's accessors is that the subject read off
// it is a namespace.
//
// A namespace's README is not one of them.  It is the writing about the namespace, and
// what a reader opens it for is that writing; the machinery is what the internals link at
// its foot leads to, and putting the same two sections under the prose would answer a
// question nobody asked on the page they asked it on.
tempstr doc::Sectionns(algo::strptr loc) {
    tempstr ret;
    if (doc::NsQ(loc)) {
        ret << Pathcomp(loc, ":LR");
    }
    return ret;
}

// Return true when the page at LOC carries the two sections that open under a triangle:
// the access paths of what it is about, and the code.
//
// A ctype's page and a namespace's page carry them, and nothing else does.  Both are about
// ctypes, which is what amc_vis draws and what amc generated code for; a page of arbitrary
// records is about whatever was typed, and asking those two questions of it answers with a
// complaint as often as with anything.
//
// A view of a query -- the functions, the generated code, one function's source -- is
// already one of those sections, on a page of its own so that a link can name it, and
// offering the same section under a triangle beneath it would be the same thing twice.
bool doc::SectionQ(algo::strptr loc) {
    return ch_N(doc::Sectionns(loc)) > 0 || doc::CtypeQ(loc);
}


// Return true when the page at LOC is a page of records: acr answered it, so the pane
// maps its rows and whoever laid the records out is who lays the sections under them.
//
// A namespace's README carries the same sections and is not one of these: it is a
// document, its pane is the documents of its directory, and its sections go under the
// text somebody wrote rather than under a table acr produced.
bool doc::RecpageQ(algo::strptr loc) {
    return doc::QueryQ(loc) && doc::FoldQ(loc);
}

// Return true when the page at LOC carries any section that opens under a heading of its
// own: the access paths and the code, the rows of a table, the function behind a record.
bool doc::FoldQ(algo::strptr loc) {
    return doc::SectionQ(loc) || ch_N(doc::Datassimfile(loc)) > 0 || ch_N(doc::Recfunc(loc)) > 0
        || ch_N(doc::Rectranscript(loc)) > 0
        || ch_N(doc::Wholetable(doc::LocQuery(loc))) > 0;
}

// Return the table whose rows the data section of the page at LOC shows, and the empty
// string when the page is about no single table.
//
// A page selecting one row of dmmeta.ssimfile is a page about a table, and what a reader
// looking at it wants next is what is in that table -- so the rows are a section of that
// page rather than another location to walk to.  The section is closed until asked for,
// since a table of ten thousand rows is not what somebody reading the declaration asked
// for.
tempstr doc::Datassimfile(algo::strptr loc) {
    bool one = Pathcomp(loc, ":LL") == "dmmeta.ssimfile";
    doc::FSsimfile *ssimfile = one ? doc::ind_ssimfile_Find(Pathcomp(loc, ":LR")) : NULL;
    tempstr ret;
    if (ssimfile) {
        ret << ssimfile->ssimfile;
    }
    return ret;
}

// Return the selection the sections of the page at LOC are about.
//
// A page of records is about what it selected.  A namespace's page is about the namespace,
// which is the one record of dmmeta.ns naming it -- and that is what makes the code section
// of such a page the namespace's own functions rather than one type's accessors, since the
// subject read off that record is a namespace.
tempstr doc::SectionQuery(algo::strptr loc) {
    tempstr ns(doc::Sectionns(loc));
    tempstr ret;
    if (ch_N(ns) > 0) {
        ret << "dmmeta.ns:" << ns;
    } else {
        ret << doc::LocQuery(loc);
    }
    return ret;
}

// Return true when LOC names an access path drawing.
bool doc::AccesspathQ(algo::strptr loc) {
    return doc::ViewWord(loc) == "accesspath";
}

// Return true when LOC names one namespace's own page.
bool doc::NsQ(algo::strptr loc) {
    return doc::ViewWord(loc) == "ns";
}

// Return true when LOC names one comptest's own page.
bool doc::ComptestQ(algo::strptr loc) {
    return doc::ViewWord(loc) == "comptest";
}

// Return true when LOC is the page about one message.
bool doc::MsgQ(algo::strptr loc) {
    return doc::ViewWord(loc) == "msg";
}

// Return true when LOC names the page of keys rather than anything to read.
//
// It is a location like any other, so the help arrives by the same Enter that a
// citation does and leaves by the same Backspace, and `doc help` writes the table
// out at a pipe.  The word carries no colon, so nothing takes it for a query, and
// no document of the tree is named help.
bool doc::HelpQ(algo::strptr loc) {
    return loc == "help";
}

// Return true when the bare key LOC is the name of a page doc writes.
//
// Most pages are addressed by a word and a colon, because they are about something and
// the argument says what.  A page that is about the whole tree has nothing to put after
// the colon: the keys the reader has, and the roots of every protocol.  So the word
// alone is the location, and dev.docpage is what says which words those are.
bool doc::BarepageQ(algo::strptr loc) {
    doc::FDocpage *docpage = algo::FindChar(loc, ':') < 0 ? doc::ind_docpage_Find(loc) : NULL;
    return docpage && docpage->bare;
}

// Return the acr query LOC selects its records with, and the empty string when LOC
// selects no records at all.
//
// A view of a query carries that query after its word.  A ctype page carries a ctype
// name instead, and the records it is about are that ctype's row and everything hanging
// off it, so the word expands to the table it is the short name of.  A page showing one
// function and a page showing a drawing select nothing, and the empty string is what
// says so -- their locations carry a colon, so nothing else would.
tempstr doc::LocQuery(algo::strptr loc) {
    bool view = doc::FuncQ(loc) || doc::CodeQ(loc);
    tempstr ret;
    if (doc::FuncsrcQ(loc) || doc::AccesspathQ(loc) || doc::NsQ(loc)) {
        ret << "";
    } else if (doc::CtypeQ(loc) || doc::MsgQ(loc)) {
        ret << doc::CtypeQuery(Pathcomp(loc, ":LR"));
    } else if (doc::ComptestQ(loc)) {
        ret << "atfdb.comptest:" << Pathcomp(loc, ":LR");
    } else if (view) {
        ret << Pathcomp(loc, ":LR");
    } else {
        ret << loc;
    }
    return ret;
}

// Return the table QUERY selects the whole of, and the empty string when it selects
// anything less.
//
// `dmmeta.ns:%` is the whole of that table and `dmmeta.ns:dmmeta` is one row of it, which
// is a difference the closure makes enormous: the closure of a record is its neighbours,
// and the closure of every record is the database.  Asking acr for `dmmeta.ns:%` with
// -xref answers with 53,071 records, where the table has 199.
tempstr doc::Wholetable(algo::strptr query) {
    algo::strptr table = Pathcomp(query, ":LL");
    bool whole = Pathcomp(query, ":LR") == "%";
    doc::FSsimfile *ssimfile = whole ? doc::ind_ssimfile_Find(table) : NULL;
    tempstr ret;
    if (ssimfile) {
        ret << ssimfile->ssimfile;
    }
    return ret;
}

// Return true when the query QUERY is answered with its records and nothing around them.
//
// How much of a record to fetch is not a decision the reader makes, and this is the whole
// of the rule that settles it.  A selection naming one record is fetched as its
// cross-reference closure, descended through pkeys and drawn as a tree: a record read
// alone says little, since the row for one field names its ctype and its arg, and the
// closure is what puts those on the page beside it.  The descent is held to pkeys because
// the full closure of a common ctype reaches every row that mentions it and runs to
// several hundred, and the tree is what says which row hangs off which.
//
// A selection naming a set is fetched flat, and `%` is what says it names one.  The
// closure of one record is its neighbours and the closure of a set is the database:
// `acr dmmeta.ns:% -xref` answers with 53,071 records where the table holds 199, and even
// the five rows of `dmmeta.ns:abt%` reach 629 that way.  A set of records read together is
// read for what they have in common, which is the columns, and a closure is what buries
// those.
bool doc::FlatQ(algo::strptr query) {
    return algo::FindChar(Pathcomp(query, ":LR"), '%') >= 0;
}

// Return the flags acr answers the query QUERY under, spelled the way acr takes them.
//
// They follow from the selection rather than from anything a reader set, so this is the
// line the page ran and the line a reader can paste into a shell.
tempstr doc::AcrFlag(algo::strptr query) {
    tempstr ret;
    if (!doc::FlatQ(query)) {
        ret << " -xref -l -tree";
    }
    if (doc::_db.meta) {
        ret << " -meta";
    }
    return ret;
}

// Return the options the page at LOC carries: the sections it has open, and the tab.
//
// doc takes every one of these under the name it is printed by, and that is the whole
// reason the flags are spelled acr's way here rather than shortened: the text of the box
// is a command line, and a reader who copies it out of the box has to be able to run it.
// What acr was asked is not among them -- the closure follows from the selection, so it
// is a fact about the query rather than a decision the reader made.
//
// A location carries only the flags it honours, and each of the three tests here is the
// one the emitter itself branches on: the tabs belong to a page acr answered, the two
// folds to a ctype's page and a namespace's page, and the data fold to a page about one
// table.  Testing them any other way is how `ns:abt` came to draw an Access Paths fold
// whose link carried no `-vis`, so following it redrew the page with the fold still shut.
tempstr doc::PageFlag(algo::strptr loc) {
    bool tab = doc::RecordsQ(loc);
    bool section = doc::SectionQ(loc);
    bool data = ch_N(doc::Datassimfile(loc)) > 0;
    tempstr ret;
    if (tab && doc::_db.meta) {
        ret << " -meta";
    }
    if (tab && doc::_db.showcmd) {
        ret << " -showcmd";
    }
    if (section && doc::_db.vis) {
        ret << " -vis";
    }
    if (section && doc::_db.code) {
        ret << " -code";
    }
    if (data && doc::_db.data) {
        ret << " -data";
    }
    return ret;
}

// Return the canonical text of the page showing LOC: the flags in force, then the
// location.
//
// Every page has one text that says what it is, and that text says the whole of it --
// which is what lets a reader see the state rather than infer it, and lets them type
// it back and arrive at the same page.  The flags come first because what follows
// them is the thing being asked about, and they are spelled the way acr takes them,
// since that is the spelling a reader of this tree already knows.
//
// It is the same text on both devices: the reader's status line and the browser's
// search box show it, so a page has one name and not one per screen it appears on.
tempstr doc::PageText(algo::strptr loc) {
    tempstr flag(doc::PageFlag(loc));
    tempstr ret;
    if (ch_N(flag) > 0) {
        ret << algo::Trimmed(flag) << " ";
    }
    ret << loc;
    return ret;
}

// Take the flags the canonical text TEXT spells and return the location that follows
// them.
//
// The text lists exactly what is in force, so a flag absent from it is off.  acr's own
// `-meta:N` spelling is accepted as well, because a reader who knows acr will reach for
// it, and it says the same thing as leaving the flag out.  A leading word this tool does
// not take is stepped over rather than read as part of the location, so a line carrying an
// option doc has no use for still arrives at the page it names.
tempstr doc::ReadText(algo::strptr text) {
    int i = doc::SkipSpace(text, 0);
    bool flag = true;
    doc::_db.meta = false;
    doc::_db.showcmd = false;
    doc::_db.vis = false;
    doc::_db.code = false;
    doc::_db.data = false;
    while (flag && i < ch_N(text)) {
        int end = i;
        while (end < ch_N(text) && text.elems[end] != ' ') {
            end++;
        }
        algo::strptr word(text.elems + i, end - i);
        algo::strptr name = Pathcomp(word, ":LL");
        bool on = algo::FindChar(word, ':') < 0 || Pathcomp(word, ":LR") != "N";
        flag = ch_N(word) > 0 && word.elems[0] == '-';
        if (flag) {
            doc::_db.meta = name == "-meta" ? on : doc::_db.meta;
            doc::_db.showcmd = name == "-showcmd" ? on : doc::_db.showcmd;
            doc::_db.vis = name == "-vis" ? on : doc::_db.vis;
            doc::_db.code = name == "-code" ? on : doc::_db.code;
            doc::_db.data = name == "-data" ? on : doc::_db.data;
            i = doc::SkipSpace(text, end);
        }
    }
    tempstr ret;
    ret << doc::RestFrom(text, i);
    return ret;
}

// Return the acr command line that answers QUERY in the shape its selection asks for,
// which is what `FlatQ` decides.  With EDIT the line carries -e, which is what asks acr to
// put the records it selected into an editor and write back whatever comes out.
tempstr doc::AcrCmdline(algo::strptr query, bool edit DFLTVAL(false)) {
    bool flat = doc::FlatQ(query);
    command::acr acr;
    acr.query = query;
    acr.in = doc::_db.cmdline.in;
    acr.xref = !flat;
    acr.l = !flat;
    acr.tree = !flat;
    acr.loose = !flat;
    acr.meta = doc::_db.meta;
    acr.report = false;
    acr.e = edit;
    return command::acr_ToCmdline(acr);
}

// Return the byte offset just past the token of LINE that begins at POS.
//
// A token runs to the next space, except that acr quotes a value carrying
// spaces: comment:"File with ssim tuples" is one attribute and not three.  So a
// quote suspends the rule until its partner arrives, and both quote characters
// occur -- a comment comes back in double quotes, a c++ default such as
// dflt:'"data"' in single ones.
int doc::TokenEnd(algo::strptr line, int pos) {
    int i = pos;
    char quote = 0;
    while (i < ch_N(line) && (quote != 0 || line.elems[i] != ' ')) {
        bool open = quote == 0 && (line.elems[i] == '"' || line.elems[i] == '\'');
        bool close = quote != 0 && line.elems[i] == quote;
        quote = open ? line.elems[i] : quote;
        quote = close ? 0 : quote;
        i++;
    }
    return i;
}

// Return TEXT without the pair of quotes acr wrapped it in, or TEXT itself when
// it carries none.  What a query needs is the value, and the quotes belong to
// the printed form rather than to it.
algo::strptr doc::Unquote(algo::strptr text) {
    int n = ch_N(text);
    char first = n > 0 ? text.elems[0] : 0;
    bool quoted = n >= 2 && (first == '"' || first == '\'') && text.elems[n - 1] == first;
    return quoted ? algo::strptr(text.elems + 1, n - 2) : text;
}

// Return the query that retrieves what the attribute NAME:VALUE of a record of
// SSIMFILE points at, or the empty string when the attribute points at nothing.
//
// An attribute names a field of the record's ctype, and that field's arg names
// a second ctype.  When the second ctype has a table of its own, the value is a
// key into it, so the table and the value together are the query that retrieves
// what the attribute refers to.  An attribute whose arg is a plain scalar -- a
// comment, a boolean, a length -- refers to no record, and gets no query.
// Return the location that shows the record VALUE of the table SSIMFILE.
//
// A ctype has a page of its own -- its declaration, the C++ that acts on it, and its
// access paths -- so an attribute naming one leads there rather than to the bare row.
// The two are the same selection: `ctype` is the short name of `dmmeta.ctype`, and the
// page is what doc makes of it.
tempstr doc::TableLoc(algo::strptr ssimfile, algo::strptr value) {
    tempstr ret;
    ret << (ssimfile == "dmmeta.ctype" ? algo::strptr("ctype") : ssimfile) << ":" << value;
    return ret;
}

static tempstr AttrQuery(algo::strptr ssimfile, algo::strptr name, algo::strptr value) {
    doc::FSsimfile *file = doc::ind_ssimfile_Find(ssimfile);
    doc::FField *field = file ? doc::ind_field_Find(dmmeta::Field_Concat_ctype_name(file->ctype, name)) : NULL;
    doc::FSsimfile *argfile = field ? doc::ind_ssimfile_ctype_Find(field->arg) : NULL;
    tempstr ret;
    if (argfile && ch_N(value) > 0) {
        ret << doc::TableLoc(argfile->ssimfile, value);
    }
    return ret;
}

// Return the query the attribute TEXT of a record of SSIMFILE leads to, or the
// empty string when it leads nowhere.  PKEY says TEXT is the record's own primary
// key, which is the first attribute of its line.
//
// Two attributes can be followed and they lead to different places.  One refers to
// another record, and AttrQuery says which; standing on `ns:dmmeta` of a ctype
// leads to the namespace record.  The other is the record's own key, whose arg is a
// plain string with no table behind it -- and following that narrows the page to
// the one record, which is what a reader wants when they are looking at a listing
// of two hundred.  The first sense wins where a key is also a reference, since a
// page already showing the record has nothing to narrow to.
//
// This is the rule rather than the drawing of it, which is why it is here and not
// inside the painter: a browser hangs an anchor on the same attribute a terminal
// underlines, and the two must agree about where it goes.
tempstr doc::AttrTarget(algo::strptr ssimfile, algo::strptr text, bool pkey) {
    int colon = algo::FindChar(text, ':');
    algo::strptr name = colon >= 0 ? algo::strptr(text.elems, colon) : text;
    algo::strptr shown = colon >= 0 ? doc::RestFrom(text, colon + 1) : algo::strptr();
    algo::strptr value = doc::Unquote(shown);
    tempstr ret(AttrQuery(ssimfile, name, value));
    bool own = ch_N(ret) == 0 && pkey && ch_N(ssimfile) > 0 && ch_N(value) > 0;
    if (own) {
        ret << doc::TableLoc(ssimfile, value);
    }
    return ret;
}

// Lay the type tag that opens a line of acr's answer onto the page, and return
// the table it names.
//
// The tag is coloured and is not a link.  What a link on it could offer is the
// whole of the table, and the attribute directly to its right already leads
// where a reader on this line wants to go -- so a link here would sit in front
// of that one, on every row of the page, and have to be stepped past to reach
// it.
//
// A line acr wrote for some other reason -- its closing report, a complaint
// about the query -- opens with a word naming no table.  That word is drawn dim
// and uncoloured, so what carries colour down the left edge of the page is the
// records.
static tempstr PutHead(algo::strptr text) {
    doc::FSsimfile *file = doc::ind_ssimfile_Find(text);
    doc::Attr attr(file ? doc_Attr_plain : doc_Attr_dim);
    doc::PutText(text, attr, doc::Color(file ? doc_Color_paleblue : doc_Color_default));
    tempstr ret;
    if (file) {
        ret << text;
    }
    return ret;
}

// Lay one attribute of a record onto the page, as its name, a colon and its
// value, underlined as a whole when it names a record the reader can be taken
// to.  PKEY says this is the first attribute of the line, which is the record's
// own primary key.
//
// Two attributes can be followed and they are followed to different places.  One
// refers to another record, and AttrQuery says which; standing on `ns:dmmeta` of
// a ctype and pressing Enter shows the namespace record.  The other is the
// record's own key, whose arg is a plain string with no table behind it -- and
// following that narrows the page to the one record, which is what a reader
// wants when they are looking at a listing of two hundred.  The first sense wins
// where a key is also a reference, since a page already showing the record has
// nothing to narrow to.
//
// The colours are the ones the tree's own emacs mode gives an ssimfile, so a
// record reads the same in both places: the name of the attribute in sky blue,
// its value in amber, and a quoted value in pale green.  Whether the value was
// quoted is what tells a string from a bare token, and it is known here because
// the value is carried twice -- as printed, quotes and all, and unquoted for the
// query.
static void PutAttr(algo::strptr ssimfile, algo::strptr text, bool pkey) {
    int colon = algo::FindChar(text, ':');
    algo::strptr name = colon >= 0 ? algo::strptr(text.elems, colon) : text;
    algo::strptr shown = colon >= 0 ? doc::RestFrom(text, colon + 1) : algo::strptr();
    algo::strptr value = doc::Unquote(shown);
    int link = doc::NewLink(doc::AttrTarget(ssimfile, text, pkey));
    doc::Attr attr(doc_Attr_plain);
    if (link > 0) {
        attr.value |= doc_Attr_underline;
    }
    doc::Color keycolor(colon >= 0 ? doc_Color_skyblue : doc_Color_default);
    doc::Color valcolor(ch_N(shown) > ch_N(value) ? doc_Color_palegreen : doc_Color_amber);
    doc::PutText(name, attr, keycolor, link);
    doc::PutText(colon >= 0 ? ":" : "", attr, doc::Color(doc_Color_default), link);
    doc::PutText(shown, attr, valcolor, link);
}

// Lay one line of acr's answer onto the page.
//
// The line is copied over as acr wrote it, spaces and all, so the columns acr
// aligned stay aligned and a tree keeps its indentation.  The walk exists to
// find where each token starts and stops: the first is the name of the table,
// every one after it is an attribute, and each becomes a link to what it names.
static void PutLine(algo::strptr line) {
    tempstr ssimfile;
    int i = 0;
    int itok = 0;
    while (i < ch_N(line)) {
        bool space = line.elems[i] == ' ';
        int end = space ? i + 1 : doc::TokenEnd(line, i);
        algo::strptr text(line.elems + i, end - i);
        if (space) {
            doc::PutText(text, doc::Attr(doc_Attr_plain), doc::Color(doc_Color_default));
        } else if (itok == 0) {
            ssimfile = PutHead(text);
        } else {
            PutAttr(ssimfile, text, itok == 1);
        }
        itok += space ? 0 : 1;
        i = end;
    }
    doc::EndRow();
}

// Return the noun NOUN with the count N after it in parentheses.
//
// The noun stays singular, which is the rule the whole tree names things by: a word names
// one element of a set and the same word names the set, so there is no plural to form.
// Putting the count second follows from that -- what a heading is about is the noun, and
// how many there happen to be is a fact about this page rather than part of the name.
tempstr doc::Count(int n, algo::strptr noun) {
    tempstr ret;
    ret << noun << " (" << n << ")";
    return ret;
}

// Return the number of records the acr answer ANSWER carries.
//
// A record is a line opening with the name of a table, which is also what makes the
// count right under a closure: every record the closure reached is a line of its own.
// acr's own report says how many it selected, but it says so at the foot -- and the
// count belongs at the head, where it tells a reader that an empty page is an empty
// table rather than a link that went nowhere.
int doc::NRecord(algo::strptr answer) {
    int ret = 0;
    ind_beg(algo::Line_curs, line, answer) {
        int beg = doc::SkipSpace(line, 0);
        algo::strptr tag(line.elems + beg, doc::TokenEnd(line, beg) - beg);
        ret += doc::ind_ssimfile_Find(tag) ? 1 : 0;
    }ind_end;
    return ret;
}

// Return the acr command line that answers with the declaration of the ctype NAME.
//
// The flags are the reader's, as they are on any page answered by acr.  A ctype page
// asks a narrower question than a raw query page -- it selects one row rather than
// whatever was typed -- but what a reader wants to see of the answer is the same
// decision either way, and a page that ignored the boxes in the pane would be a page
// whose canonical text did not describe it.
tempstr doc::CtypeCmdline(algo::strptr name) {
    return doc::AcrCmdline(doc::CtypeQuery(name));
}

// Lay the ctype NAME onto the page: what it was declared to be, what amc made of it,
// what acts on it, and where it sits.
//
// A ctype's declaration is its own row together with the field rows beneath it, which
// is exactly what the pkey closure of that one row selects -- so the page opens with
// the fields, their types, their defaults and what each is for.  The generated code
// and the functions follow when their flags are set, in that order: what amc made of
// the declaration is nearer to it than what hand-written code does with the result.
// Then where the ctype sits: which tables build their key out of it, which merely
// mention it, which command lines take it as an option, and what named values its
// fields carry.  A declaration says what a record holds, and none of that is visible in
// the rows of the ctype itself.
void doc::RenderCtype(algo::strptr name) {
    tempstr cmd(doc::CtypeCmdline(name));
    tempstr out(doc::ToolOut(cmd));
    doc::PutAnchor("records");
    doc::PutNote(tempstr() << doc::Count(doc::NRecord(out), "record") << "   " << doc::Querytext(cmd));
    ind_beg(algo::Line_curs, line, out) {
        PutLine(line);
    }ind_end;
    doc::EndRow();
    doc::SubjectOne(name);
    tempstr prose(doc::Ssimdoc(doc::CtypeSsimfile(name)));
    if (ch_N(prose) > 0) {
        doc::ParseText(prose);
        doc::RenderDoc();
        doc::EndRow();
    }
    tempstr text(doc::CtypeText(name));
    if (ch_N(text) > 0) {
        doc::EndRow();
        doc::ParseText(text);
        doc::RenderDoc();
    }
}

// Run acr for QUERY and lay its answer on the page.
//
// The page opens with how many records it holds and the command line that produced
// them.  The count is what keeps an empty table from reading as a broken link, and the
// command line is there because a key does not say which flags were in force when it
// was answered -- and because the line can then be pasted into a shell.  What follows is acr's own output, one page row
// per line of it.  acr's diagnostics come back on the same stream, so a query
// naming a table that does not exist shows the complaint where the records would
// have been, rather than an empty page.
void doc::RenderQuery(algo::strptr query) {
    tempstr out(doc::AcrOut(query));
    tempstr cmd;
    cmd << "acr " << query << doc::AcrFlag(query);
    doc::PutAnchor("records");
    doc::PutNote(tempstr() << doc::Count(doc::NRecord(out), "record") << "   " << doc::Querytext(cmd));
    ind_beg(algo::Line_curs, line, out) {
        PutLine(line);
    }ind_end;
}

// Lay the records QUERY selects onto the page, with no closure around them.
//
// The note above them is the plain command line, so what the box says it ran is what it
// ran.  A reader looking at a table's rows reaches the ssimfile record this way, and one
// record is the whole of what they came for.
void doc::RenderFlat(algo::strptr query) {
    tempstr out(doc::AcrFlat(query));
    tempstr cmd;
    cmd << "acr " << query;
    doc::PutNote(tempstr() << doc::Count(doc::NRecord(out), "record") << "   " << doc::Querytext(cmd));
    ind_beg(algo::Line_curs, line, out) {
        PutLine(line);
    }ind_end;
}

// Return everything the command line CMD wrote, its diagnostics with it.
//
// Every answer this tool shows comes from running another one -- acr for records, src_func
// for functions, amc_vis for access paths -- and each of them writes its complaints to
// the error stream.  A page that shows the complaint where the records would have been is
// better than a page that shows nothing, so the two streams are joined and a non-zero
// exit is not an error here.
//
// The cap is one size for every caller.  The largest answer any page asks for is a
// namespace's generated code, and a caller-by-caller guess at a smaller one buys nothing
// but a page silently cut off.
tempstr doc::ToolOut(algo::strptr cmd) {
    return algo::SysEval(tempstr() << cmd << " 2>&1", algo::FailokQ(true), 64 * 1024 * 1024);
}

// Return the command line that selects QUERY and nothing around it.
//
// The flags in force say how much of the closure the page's own records show, and they are
// a statement about the query the reader asked.  A box holding some other query answers a
// different question, so it is asked plainly: what a reader wants of the ssimfile record
// under a table's rows is that record, not the schema its closure would drag in.
//
// The same plain answer is what a section beside the records is computed from.  A record's
// closure runs to the schema around it -- a field's ctype, its reftype, the namespace, the
// license -- and asking src_func or amc_vis about all of that is a scan nobody asked for
// and a page nobody can read.
tempstr doc::AcrFlatCmdline(algo::strptr query) {
    command::acr acr;
    acr.query = query;
    acr.in = doc::_db.cmdline.in;
    acr.report = false;
    return command::acr_ToCmdline(acr);
}

// Run acr for QUERY with no flags at all and return everything it wrote.
tempstr doc::AcrFlat(algo::strptr query) {
    return doc::ToolOut(doc::AcrFlatCmdline(query));
}

// Run acr for QUERY under the view now in force and return everything it wrote.
//
// The diagnostics come back on the same stream as the records, so a query naming
// a table that does not exist answers with the complaint rather than with
// nothing, and the page shows it where the records would have been.
tempstr doc::AcrOut(algo::strptr query) {
    tempstr cmd(doc::AcrCmdline(query));
    return doc::ToolOut(cmd);
}

// Lay one dim row of plain text onto the page: the command line the rows below it
// came from, or a note saying why there are none.
//
// A page opens with the command that produced it because a key does not say which
// flags were in force when it was answered, and because the line can then be
// pasted into a shell.
// Return what to show in place of the command line CMD: the line itself, or the word
// `query` when it is too long to read.
//
// A command line naming twenty subjects runs to several hundred characters, and at the
// top of every page that is five rows of the screen spent on something a reader wants
// once in twenty pages.  Eighty characters is the measure of a sentence, so a line past
// it is put away behind the word for what it is.  A browser opens the word on a click;
// the terminal has no click and shows the word alone, where a reader who wants to run
// the line again has `e` for it.
tempstr doc::Querytext(algo::strptr cmd) {
    tempstr ret;
    ret << (ch_N(cmd) > 80 ? algo::strptr("query") : cmd);
    return ret;
}

// Lay one dim row of plain text onto the page: the command line the rows below it came
// from, or a note saying why there are none.
void doc::PutNote(algo::strptr text) {
    doc::PutText(text, doc::Attr(doc_Attr_dim), doc::Color(doc_Color_default));
    doc::EndRow();
}

// Lay the heading of a section that opens and closes onto the page: NAME, anchored at SLUG,
// drawn open when ON, and turning the option OPT the other way when it is followed.
//
// The heading is drawn whether or not the section is open, because a reader who cannot see
// that the code is there has no reason to open it.  What opens it is the heading itself:
// it is a link like every other thing on the page, and following it flips OPT, which is
// the option spelled as doc's own command line spells it.  The link carries the section's
// anchor as well, so the reader who asked for the code is left standing on it rather than
// wherever the row they were on has moved to.
//
// A section that cannot close names no option.  It is then a heading with no triangle and
// no link, since a triangle on something that never turns says nothing.
void doc::PutFoldhead(algo::strptr name, algo::strptr slug, algo::strptr opt, bool on) {
    tempstr target;
    if (ch_N(opt) > 0) {
        target << opt << "#" << slug;
    }
    int link = doc::NewLink(target);
    doc::PutAnchor(slug);
    if (ch_N(opt) > 0) {
        doc::PutText(on ? "\u25be " : "\u25b8 ", doc::Attr(doc_Attr_bold), doc::Color(doc_Color_default), link);
    }
    doc::PutText(name, doc::Attr(doc_Attr_bold), doc::Color(doc_Color_amber), link);
    doc::EndRow();
}

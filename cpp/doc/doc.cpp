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
// Source: cpp/doc/doc.cpp
//
// Find the document a reader asked for, render it, and put it on the screen.
// The argument is a key, not a filename.  Every markdown file in the tree is a
// row of dev.readmefile, and the path conventions abt_md already enforces say
// what each row documents: txt/exe/<ns>/README.md is a tool, txt/rule/<ns>.md
// is a namespace's rules, txt/ssimdb/<ns>/<name>.md is one ssimfile.  So `doc
// acr` can mean the acr README without anyone having to type where it lives,
// and a key matching more than one document lists them rather than guessing.
// A key carrying a colon is not a document key at all but a query over the ssim
// database, which acr answers and acr.cpp lays out.  The two languages coexist
// in one argument because no document in this tree has a colon in its path.
// This file also owns the decisions the renderer depends on but cannot make for
// itself: how wide the measure is, whether the output may carry colour, and
// which of acr's flags a query starts under.  The first two come from the
// terminal when there is one, and both collapse to plain text when the output is
// a pipe, so `doc acr | grep` behaves.

#include "include/algo.h"
#include "include/doc.h"

#include <sys/ioctl.h>
#include <unistd.h>

// Record the document at GITFILE as a match, if the tree holds such a file.
static void MatchFile(algo::strptr gitfile) {
    doc::FReadmefile *readmefile = doc::ind_readmefile_Find(gitfile);
    if (readmefile) {
        doc::c_match_InsertMaybe(*readmefile);
    }
}

// Fill _db.c_match with the documents KEY names, and return true when KEY
// matched one of the path conventions rather than being searched for.
//
// The conventions are probed in order, best first, and the order they are
// probed in is the order the matches come out in.  That ranking is what makes
// the common case land: `acr` names two documents in the tree -- the tool's README and
// the rule file beside it -- and a reader who types the tool's name wants its README,
// every time.  The tiers below the
// first are still reachable, because they follow the winner in the list and
// the renderer names them at the end of what it shows.
//
// A key that matches no convention becomes a substring search over every
// document path.  The two cases are told apart by the return value, and the
// caller uses it to decide between showing a document and listing candidates:
// a name resolves, a guess offers.
bool doc::ResolveKey(algo::strptr key) {
    algo::strptr ns = Pathcomp(key, ".LL");
    algo::strptr name = Pathcomp(key, ".LR");
    bool dotted = ch_N(name) > 0 && name != key;
    MatchFile(algo::tempstr() << "txt/exe/" << key << "/README.md");
    MatchFile(algo::tempstr() << "txt/lib/" << key << "/README.md");
    MatchFile(algo::tempstr() << "txt/ssimdb/" << key << "/README.md");
    MatchFile(algo::tempstr() << "txt/protocol/" << key << "/README.md");
    if (dotted) {
        MatchFile(algo::tempstr() << "txt/protocol/" << ns << "/" << name << ".md");
    }
    MatchFile(algo::tempstr() << "txt/rule/" << key << ".md");
    {
        MatchFile(algo::tempstr() << "txt/openacr/" << key << ".md");
        MatchFile(algo::tempstr() << "txt/x2/" << key << ".md");
        MatchFile(algo::tempstr() << "txt/script/" << key << ".md");
        MatchFile(algo::tempstr() << "txt/" << key << ".md");
        MatchFile(algo::tempstr() << "txt/" << key << "/README.md");
    }
    bool exact = doc::c_match_N() > 0;
    if (!exact) {
        algo_lib::Regx regx;
        algo_lib::Regx_ReadSql(regx, algo::tempstr() << "%" << key << "%", true);
        ind_beg(doc::_db_readmefile_curs, readmefile, doc::_db) {
            if (algo_lib::Regx_Match(regx, readmefile.gitfile)) {
                doc::c_match_InsertMaybe(readmefile);
            }
        }ind_end;
    }
    return exact;
}

// Return the query locations a key that named no document could have meant, one per
// line, and set NFOUND to how many there were even when NMAX cuts the list.
//
// A document is half of what this tool reads and the database is the other half, so
// a key that matched no document has only been half looked for.  A table whose name
// carries the key leads to the whole of that table, and a ctype whose name carries it
// leads to its record -- which is what a reader typing `FStream` is after, since no
// document is named that and eight ctypes are.
//
// A dotted key whose namespace exists offers that namespace's ctypes as well.  The
// key that prompts this is the near miss: `lib_x2.FCtype` names nothing at all, and
// the useful answer is not silence but the list its author was picking from.
tempstr doc::GuessLoc(algo::strptr key, int nmax, int &nfound) {
    algo::strptr ns = Pathcomp(key, ".LL");
    bool dotted = ns != key && ch_N(ns) > 0;
    algo_lib::Regx regx;
    algo_lib::Regx_ReadSql(regx, algo::tempstr() << "%" << key << "%", true);
    tempstr ret;
    nfound = 0;
    if (ch_N(key) > 0) {
        ind_beg(doc::_db_ssimfile_curs, ssimfile, doc::_db) {
            if (algo_lib::Regx_Match(regx, ssimfile.ssimfile)) {
                nfound++;
                if (nfound <= nmax) {
                    ret << ssimfile.ssimfile << ":%" << eol;
                }
            }
        }ind_end;
        ind_beg(doc::_db_ctype_curs, ctype, doc::_db) {
            if (algo_lib::Regx_Match(regx, ctype.ctype)) {
                nfound++;
                if (nfound <= nmax) {
                    ret << "dmmeta.ctype:" << ctype.ctype << eol;
                }
            }
        }ind_end;
    }
    if (dotted && doc::ind_ns_Find(ns)) {
        nfound++;
        ret << "dmmeta.ctype:" << ns << ".%" << eol;
    }
    return ret;
}

// Return the acr query the key KEY selects records with, or nothing when it selects
// none.
//
// A key that is already a query is one.  Otherwise the key may name a ctype, whose
// record it is; a namespace, whose record it is; or a table, in which case the whole
// of that table is the selection.
//
// A pattern handed to `code:` selects ctypes, since that view is about ctypes and nothing
// else.  `func:` is not resolved here at all when its argument is a regex -- a function's
// name is arbitrary, so the regex goes to src_func as it stands.
//
// This is what a view of a query resolves its key with, and it prefers records where
// resolving a key on its own prefers documents.  `func:lib_x2` asks about the
// functions of that library, where a bare `lib_x2` opens the library's README --
// which is right both times, because a view is a view of records and there is nothing
// else for it to be a view of.
tempstr doc::KeyQuery(algo::strptr key) {
    tempstr ret;
    if (doc::QueryQ(key)) {
        ret << key;
    } else if (doc::ind_ctype_Find(key)) {
        ret << "dmmeta.ctype:" << key;
    } else if (doc::ind_ns_Find(key)) {
        ret << "dmmeta.ns:" << key;
    } else if (doc::ind_ssimfile_Find(key)) {
        ret << key << ":%";
    } else if (algo::FindChar(key, '%') >= 0) {
        ret << "dmmeta.ctype:" << key;
    }
    return ret;
}

// Return true when KEY names a directory git tracks a file under.
//
// A location says a directory with a trailing slash, and a reader typing one leaves it off
// as often as not.  Nothing in a key's own text says which of the two it is, so the question
// is put to dev.gitfile: a directory of this tree is the prefix of a path git tracks, and no
// file is.
//
// A bare word is tried as a directory last of all.  `apm` is a directory of this tree and
// also a tool, and a reader typing it wants the tool -- so the path conventions, the tables
// and the ctypes are asked first, and the directory answers only for a word that named none
// of them.  Spelled with its slash it is a directory outright, which is how to ask for the
// one a document shadows.
static bool TreedirQ(algo::strptr key) {
    tempstr want;
    want << key << "/";
    bool ret = false;
    ind_beg(doc::_db_gitfile_curs, gitfile, doc::_db) {
        ret = ret || StartsWithQ(gitfile.gitfile, want);
    }ind_end;
    return ret;
}

// Return the location the key KEY names, or the empty string when it names no one
// thing.  The documents it could have meant are left in c_match either way.
//
// This is the whole of what a key means, and it is one function because it is asked
// twice: once by the command line, and once by the search box on the page, whose
// visitor typed a key into it and expects the same answer.  A key that is already a
// path, a query or the page of keys is that location outright; a key matching a path
// convention is the document it names; a key matching no document may still name a
// ctype, and then it is that ctype's record.  Naming nothing at all is the
// documentation index, which is the page to start from.
//
// A leading slash comes off first.  Everywhere else in this tool a target beginning
// with one is a path from the top of the repository -- that is how the documents
// under txt/ cite each other -- so a key reads the same way, and `/cpp/doc/html.cpp`
// means what `cpp/doc/html.cpp` means.  One consequence is worth naming: an absolute
// path of the filesystem is then not a key at all, so nothing reaches a file test
// with a leading slash still on it.
//
// A fragment comes off with the leading slash and goes back onto whatever resolved, so
// `txt/rule/doc.md#invariants` names that heading of that document.  It is the one part
// of a page's state the server never learns on its own, because a browser keeps a
// fragment to itself -- so the box carries it back here when a reader submits.
//
// A directory resolves to its README, with the slash or without it, because that is the
// page somebody opening a directory wants: a README says what the directory is about and
// ends with the listing of what it holds, so it answers both questions where the bare
// listing answers one.  A directory holding no README resolves to itself, its page is the
// listing, and the location it resolves to carries the slash -- which is the whole of
// what tells a directory from a file everywhere below here.
//
// A table's name is that table's rows.  `doc dmmeta.ns` is a question about what is in
// that table, and it is answered with the records; the schema and the table's own row
// follow them as sections of the same page, which is the order the reader asked in.  The
// file under txt/ssimdb that holds the prose is still a document and still opens when it
// is named by its path.
//
// A word doc does not synthesize a page for is a table's short name, and it is expanded
// to the full one here.  So `ssimfile:dmmeta.ctype` and `dmmeta.ssimfile:dmmeta.ctype`
// are one page rather than two, and the location a link or a history carries is the
// full name whichever of them the reader typed.
//
// A key of three dots is an amc key naming one function, and it opens that function's
// source.  amc keys a ctype by a namespace and a name and a field by a ctype and a name,
// so the count of dots is what tells the three apart, and `x2ui.FCtype.c_cstr.Remove`
// needs no word in front of it to say which it is.
//
// A word whose argument is a pattern is the query over that word's own table.  `ns:acr`
// is one namespace and the page doc writes about it; `ns:%` is every namespace, which is
// no such page and is the rows of `dmmeta.ns`.  Which table a word is about is the
// `ssimfile` of its `dev.docpage` row, so a word with no table keeps its own page
// whatever it is handed.
//
// A key naming a view of a query carries a key of its own, and that inner key is
// resolved to records rather than to a document.  So `func:lib_x2.FStream` is the
// functions of that ctype and `code:lib_x2` the generated code of that namespace,
// while what comes back is spelled out in full -- `func:dmmeta.ctype:lib_x2.FStream`
// -- because one page has one canonical text and the short form is a way of typing
// it rather than a second name for it.
tempstr doc::ResolveLoc(algo::strptr want) {
    algo::strptr bare = StartsWithQ(want, "/") ? doc::RestFrom(want, 1) : want;
    algo::strptr typed = Pathcomp(bare, "#LL");
    algo::strptr frag = Pathcomp(bare, "#LR");
    bool anchored = frag != bare && ch_N(frag) > 0;
    tempstr pattern(doc::Wordpattern(typed));
    tempstr key;
    if (ch_N(pattern) > 0) {
        key << pattern;
    } else if (ch_N(doc::ViewWord(typed)) > 0) {
        key << typed;
    } else {
        key << doc::ExpandQuery(typed);
    }
    bool funcsrc = doc::FuncsrcQ(key);
    bool view = !funcsrc && !doc::FuncregxQ(key) && (doc::FuncQ(key) || doc::CodeQ(key));
    tempstr inner(view ? doc::KeyQuery(doc::LocQuery(key)) : tempstr());
    tempstr readme;
    readme << key << (doc::DirQ(key) ? "" : "/") << "README.md";
    bool literal = !view && ch_N(key) > 0
        && (algo::FileQ(key) || doc::QueryQ(key) || doc::BarepageQ(key));
    bool index = !view && !literal && algo::FileQ(readme);
    bool listing = !view && !literal && !index && doc::DirQ(key);
    bool open = !view && !literal && !index && !listing;
    doc::FSsimfile *table = open ? doc::ind_ssimfile_Find(key) : NULL;
    doc::c_match_RemoveAll();
    bool named = open && !table && doc::ResolveKey(key);
    doc::FCtype *ctype = open && !table && !named ? doc::ind_ctype_Find(key) : NULL;
    bool amckey = open && !table && !named && !ctype && doc::AmckeyQ(key);
    bool tree = open && !table && !named && !ctype && !amckey && ch_N(key) > 0 && TreedirQ(key);
    tempstr ret;
    if (view && ch_N(inner) > 0) {
        ret << (doc::FuncQ(key) ? "func:" : "code:") << inner;
    } else if (view) {
        ret << "";
    } else if (literal) {
        ret << key;
    } else if (index) {
        ret << readme;
    } else if (listing) {
        ret << key;
    } else if (table) {
        ret << table->ssimfile << ":%";
    } else if (ch_N(key) == 0) {
        ret << "txt/README.md";
    } else if (named) {
        ret << doc::c_match_qFind(0).gitfile;
    } else if (ctype) {
        ret << "dmmeta.ctype:" << key;
    } else if (amckey) {
        ret << "func:" << key;
    } else if (tree) {
        ret << key << "/";
    }
    if (anchored && ch_N(ret) > 0) {
        ret << "#" << frag;
    }
    return ret;
}

// Print what the key could have meant, one location per line, so a reader can pick
// one: the documents whose path carries it, then the records.
static void ListMatch(algo::strptr key) {
    int nguess = 0;
    tempstr guess(doc::GuessLoc(key, 20, nguess));
    ind_beg(doc::_db_c_match_curs, readmefile, doc::_db) {
        prlog(readmefile.gitfile);
    }ind_end;
    ind_beg(algo::Line_curs, loc, guess) {
        prlog(loc);
    }ind_end;
    prlog("report.doc  n_match:" << doc::c_match_N() << Keyval("n_guess", nguess));
}

// Return the number of columns the terminal offers, or zero when stdout is
// not a terminal.
static int TerminalWidth() {
    int ret = 0;
    if (isatty(STDOUT_FILENO)) {
        struct winsize winsize;
        winsize.ws_col = 0;
        ioctl(STDOUT_FILENO, TIOCGWINSZ, &winsize);
        ret = winsize.ws_col;
    }
    return ret;
}

// Take the reader's starting view of a query from the command line.
//
// The flags say which sections are laid out beside acr's answer, and which of the three
// tabs the page opens on, rather than how any of it is drawn; the reader toggles each of
// them with a keystroke once the page is up.  What the command line supplies is therefore
// where the reader starts, which is also the whole of the story for a run that never
// opens the reader at all.
static void ResolveView() {
    doc::_db.meta = doc::_db.cmdline.meta;
    doc::_db.showcmd = doc::_db.cmdline.showcmd;
    doc::_db.vis = doc::_db.cmdline.vis;
    doc::_db.code = doc::_db.cmdline.code;
    doc::_db.data = doc::_db.cmdline.data;
}

// Settle the measure and the colour decision, both of which the renderer reads
// off _db and cannot work out for itself.
//
// The measure is the window, capped at a hundred columns.  The cap is what
// makes a wide terminal readable: prose set to two hundred columns forces the
// eye to travel back across the whole screen to find the next line, which is
// why a book is narrow and why man pages have a measure at all.  A table is
// laid out inside the same measure, so a wide window buys the table more room
// only up to that point.
//
// Colour follows the terminal too.  Output that is not a terminal is somebody
// else's input -- a pipe into grep, a file being diffed -- and escapes in it
// are corruption, so -force_color is the only way to put them there.
static void ResolveOutput() {
    int width = TerminalWidth();
    bool html = doc::_db.cmdline.html || ch_N(doc::_db.cmdline.bind) > 0;
    doc::_db.width = doc::_db.cmdline.width > 0 ? doc::_db.cmdline.width : i32_Min(100, width > 0 ? width : 80);
    doc::_db.color = !html && (doc::_db.cmdline.force_color || (doc::_db.cmdline.color && width > 0));
}

// Write TEXT to stdout as it stands.
static void EmitOutput(algo::strptr text) {
    prlog_(text);
}

// Return the location a link TARGET written on the page at BASE leads to, or
// the empty string when it leads nowhere this tool can open.
//
// Five forms occur.  A link into a namespace's ssimdb documentation, anchored at a
// table's own name, is that table's page: a file in the tree can spell a synthesized
// page no other way, since a markdown link may name a path and a fragment and nothing
// else, and the generated field tables carry thousands of them.  A target carrying a
// colon is a query over the ssim database, and it is its own location -- an attribute of
// a record names the record it refers to, not a path, so nothing is resolved against
// anything.  A target beginning with a slash is a path from the top of the repository,
// which is how the documents under txt/ cite each other.  A target beginning with a hash
// is a heading inside the document being read, and names no file at all.  Anything else
// is a path relative to the directory the citing document lives in.
tempstr doc::ResolveTarget(algo::strptr base, algo::strptr target) {
    algo::strptr path = Pathcomp(target, "#LL");
    algo::strptr frag = Pathcomp(target, "#LR");
    bool anchored = frag != target && ch_N(frag) > 0;
    tempstr table(anchored && StartsWithQ(path, "/txt/ssimdb/") && EndsWithQ(path, "/README.md")
                  ? doc::Ssimanchor(frag) : tempstr());
    tempstr ret;
    if (ch_N(table) > 0) {
        ret << "ctype:" << table;
    } else if (doc::QueryQ(target)) {
        ret << target;
    } else if (StartsWithQ(path, "/")) {
        ret << doc::RestFrom(path, 1);
    } else if (ch_N(path) > 0) {
        ret << DirFileJoin(Pathcomp(base, "/RL"), path);
    }
    return ret;
}

// Flip the display option the link target TARGET names, and return true when it names
// none of them.  A fold heading is the link that carries one.
//
// Following such a link is not a navigation.  The location does not change, the option
// rides along every link on the page after it, and nothing goes onto the history -- a
// reader who opened the code and then followed an attribute expects Backspace to bring
// back the page with the code still open, not to close it.
//
// The target is the option as doc's own command line spells it, which is what lets the
// heading say what it does: the page a reader gets by following `-code` is the page
// `doc -code` would have printed.
bool doc::Foldflip(algo::strptr target) {
    bool ret = true;
    if (target == "-vis") {
        doc::_db.vis = !doc::_db.vis;
    } else if (target == "-code") {
        doc::_db.code = !doc::_db.code;
    } else if (target == "-data") {
        doc::_db.data = !doc::_db.data;
    } else {
        ret = false;
    }
    return ret;
}

// Print the page's links, one per line, numbered so -follow can name one.
// Print the page's links, one per line, with where each one sits and where
// following it would land.
//
// `anchor` is the row a target naming a heading of this page resolves to, and
// -1 when the target names none -- either because it points at another document
// or because the page carries no such anchor.  It is the only view of the
// anchor table from outside the reader, which cannot be driven without a
// terminal.
static void ListLink() {
    int i = 0;
    ind_beg(doc::_db_link_curs, link, doc::_db) {
        i++;
        algo::strptr name = Pathcomp(link.target, "#LR");
        bool named = name != algo::strptr(link.target) && ch_N(name) > 0;
        prlog("doc.link"
              << Keyval("n", i)
              << Keyval("row", link.row)
              << Keyval("anchor", named ? doc::AnchorRow(name) : -1)
              << Keyval("target", link.target));
    }ind_end;
    prlog("report.doc  n_link:" << doc::link_N());
}

// Show one document, answer one query, or say which documents the key could have
// meant.
//
// A key carrying a colon is a query over the ssim database rather than the name
// of a document, so `doc dmmeta.ctype:dmmeta.Ctype` puts that record on the page
// and every attribute of it becomes a link.  No document in the tree has a colon
// in its path, which is what lets the one argument mean both things.
//
// A terminal gets the reader, which owns the screen so that links can be
// selected and followed.  Anything else -- a pipe, a file, a capture -- gets the
// page written out once as plain text, which is what makes `doc acr | grep`
// and a comptest see the same thing.
//
// -html asks for the same location as an HTML page instead, and then the cell grid
// is never built: a browser wants the structure a grid has already thrown away, so
// the HTML comes from the blocks.  The grid is still built when -links or -follow
// is asked for, because those report what the grid decided.
//
// -bind turns the whole thing around.  Instead of one location shown once, the
// process answers a location per request until it is killed, and the key on the
// command line is not used at all -- every request brings its own.
//
// Naming nothing opens the documentation index, which is a page of links into
// every part of the tree and so the one page a reader who does not yet know
// what to ask for can start from.  It is txt/README.md rather than the copy at
// the top of the repository, because that copy is generated from this one and
// opens with two lines telling the reader not to edit it.  `-list` with no key
// still lists every document.
//
// A key that names no document at all may still name a ctype, and `doc
// lib_x2.FStream` then opens that ctype's record.  Most of the tree's types have
// no document of their own, and the record is what there is to read about one:
// its fields, and from there the functions that take it.
//
// The one key that names neither a document nor a record is `help`, which is the
// page of keys the reader's ? opens.  It is a location like the others, so it can
// be written out at a pipe as well as read on the screen.
void doc::Main() {
    ResolveOutput();
    ResolveView();
    doc::ReadmedirRead();
    tempstr key;
    if (doc::_db.cmdline.q) {
        key << doc::ExpandQuery(doc::_db.cmdline.key);
    } else {
        key << doc::_db.cmdline.key;
    }
    tempstr path(doc::ResolveLoc(key));
    bool exact = ch_N(path) > 0 && !(doc::_db.cmdline.list && doc::c_match_N() > 0);
    bool html = doc::_db.cmdline.html;
    bool serving = ch_N(doc::_db.cmdline.bind) > 0;
    bool grid = !html && !serving;
    bool listing = doc::_db.cmdline.list;
    if (exact && (grid || doc::_db.cmdline.links || doc::_db.cmdline.follow > 0)) {
        doc::RenderPage(path);
    } else if (!exact && !listing && !html) {
        doc::RenderMatch(key);
    }
    doc::FLink *follow = exact && doc::_db.cmdline.follow > 0 ? doc::link_Find(doc::_db.cmdline.follow - 1) : NULL;
    tempstr target(follow ? algo::strptr(follow->target) : algo::strptr());
    bool fold = doc::Foldflip(Pathcomp(target, "#LL"));
    tempstr here(doc::_db.page);
    tempstr next = fold ? tempstr() : doc::ResolveTarget(here, target);
    if (fold) {
        doc::RenderPage(here);
    } else if (ch_N(next) > 0) {
        doc::RenderPage(next);
    }
    bool interactive = doc::_db.cmdline.pager && !html && isatty(STDOUT_FILENO) && isatty(STDIN_FILENO);
    if (ch_N(doc::_db.cmdline.bind) > 0) {
        doc::Serve();
    } else if (listing) {
        ListMatch(key);
    } else if (doc::_db.cmdline.links) {
        ListLink();
    } else if (html && exact) {
        EmitOutput(doc::HtmlPage(path));
    } else if (html) {
        EmitOutput(doc::HtmlFind(key));
    } else if (interactive) {
        doc::Browse();
    } else {
        algo::tempstr out;
        doc::PaintPage(out);
        EmitOutput(out);
    }
}

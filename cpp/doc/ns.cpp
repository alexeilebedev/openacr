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
// Source: cpp/doc/ns.cpp
//
// A namespace is the unit this tree is organized by, and what there is to say about one
// is scattered over half a dozen tables: the types it declares, the files it is built
// from, what it is built on and what is built on it, the tables it reads, the tests that
// exercise it.  Each of those is a query anyone could run, and running six of them by
// hand and reading the answers side by side is the work this page does instead.
// The page is written as markdown and handed to the same parser a document goes
// through.  That is what makes it one page rather than two: the terminal renderer and
// the html emitter both start from blocks, so a table written here is a table on the
// glass and a <table> in a browser, and a link written here is followed by the same
// Enter that follows a link in a README.  Nothing about this page knows which device is
// asking.

#include "include/algo.h"
#include "include/doc.h"

// Append a heading reading TEXT, anchored at SLUG, to OUT.
//
// A heading is the name of a section and nothing else, because it is what the pane lists
// and what a link to the section carries.  How many rows the section holds goes into the
// table under it, in the corner where a table's first column is named -- the same place a
// box of records says how many records it holds.
//
// The anchor is what a browser scrolls to and what the pane hangs its entries on, and it
// is written here for the same reason abt_md writes one under every heading it
// generates: a heading with no anchor is a place in the page that nothing can name.  The
// slug is fixed rather than made from the text, so a link to a section survives the
// count in that section's heading changing.
void doc::PutSecthead(algo::cstring &out, algo::strptr slug, algo::strptr text) {
    out << "### " << text << eol;
    out << "<a href=\"#" << slug << "\"></a>" << eol << eol;
}

// Append TEXT to OUT as the contents of one cell of a markdown table.
//
// A pipe inside a cell would end it, and a comment is free text that may hold one, so
// the pipes are escaped here rather than hoped about.  An empty cell is written as a
// space, since a table row of nothing but separators reads as a rule.
void doc::PutTablecell(algo::cstring &out, algo::strptr text) {
    out << " ";
    ind_beg(algo::Line_curs, line, text) {
        frep_(i, ch_N(line)) {
            out << (line.elems[i] == '|' ? "\\" : "") << line.elems[i];
        }
    }ind_end;
    out << " |";
}

// Append the ctypes of the namespace NS to OUT, each leading to its own page.
static void PutNsctype(algo::cstring &out, algo::strptr ns) {
    tempstr body;
    int n = 0;
    ind_beg(doc::_db_ctype_curs, ctype, doc::_db) {
        if (doc::ns_Get(ctype) == ns) {
            n++;
            body << "|";
            doc::PutTablecell(body, tempstr() << "[" << ctype.ctype << "](ctype:" << ctype.ctype << ")");
            doc::PutTablecell(body, ctype.comment);
            body << eol;
        }
    }ind_end;
    if (n > 0) {
        doc::PutSecthead(out, "ctypes", "Ctypes");
        out << "| " << doc::Count(n, "ctype") << " | what it is |" << eol
            << "|---|---|" << eol << body << eol;
    }
}

// Append the source files of the target NS to OUT, each leading to its listing.
static void PutNssrc(algo::cstring &out, algo::strptr ns) {
    tempstr body;
    int n = 0;
    ind_beg(doc::_db_targsrc_curs, targsrc, doc::_db) {
        if (doc::target_Get(targsrc) == ns) {
            n++;
            body << "|";
            doc::PutTablecell(body, tempstr() << "[" << doc::src_Get(targsrc) << "](/" << doc::src_Get(targsrc) << ")");
            doc::PutTablecell(body, targsrc.comment);
            body << eol;
        }
    }ind_end;
    if (n > 0) {
        doc::PutSecthead(out, "sources", "Sources");
        out << "| " << doc::Count(n, "source") << " | what it holds |" << eol
            << "|---|---|" << eol << body << eol;
    }
}

// Append the targets NS is built on, and those built on NS, to OUT.
//
// Both are one table read two ways, so they are written as one paragraph of links each
// rather than as a table: a dependency has nothing to say beyond its name, and a row
// per name would be six lines of separators around six words.
static void PutNsdep(algo::cstring &out, algo::strptr ns) {
    tempstr on;
    tempstr by;
    ind_beg(doc::_db_targdep_curs, targdep, doc::_db) {
        bool ours = doc::target_Get(targdep) == ns;
        bool theirs = doc::parent_Get(targdep) == ns;
        if (ours) {
            on << "[" << doc::parent_Get(targdep) << "](ns:" << doc::parent_Get(targdep) << ") ";
        }
        if (theirs) {
            by << "[" << doc::target_Get(targdep) << "](ns:" << doc::target_Get(targdep) << ") ";
        }
    }ind_end;
    if (ch_N(on) > 0) {
        doc::PutSecthead(out, "built-on", "Built on");
        out << on << eol << eol;
    }
    if (ch_N(by) > 0) {
        doc::PutSecthead(out, "built-into", "Built into");
        out << by << eol << eol;
    }
}

// Append the ssim tables the target NS reads to OUT, each leading to its records.
//
// A finput names the field it fills, and the table behind that field is what a reader
// wants: the field is where the rows land and the table is what they are.  Two steps
// reach it, because a target does not hold ssim rows as they are written -- it holds a
// projection of them, a ctype of its own carrying the ssim ctype as its base.  So the
// finput's field names the projection, the projection's base names the ssim ctype, and
// the ssimfile of that ctype is the table.
static void PutNsinput(algo::cstring &out, algo::strptr ns) {
    tempstr body;
    int n = 0;
    ind_beg(doc::_db_finput_curs, finput, doc::_db) {
        doc::FField *field = doc::ns_Get(finput) == ns ? doc::ind_field_Find(finput.field) : NULL;
        doc::FField *base = field ? doc::ind_field_Find(tempstr() << field->arg << ".base") : NULL;
        doc::FSsimfile *ssimfile = base ? doc::ind_ssimfile_ctype_Find(base->arg) : NULL;
        if (ssimfile) {
            n++;
            body << "|";
            doc::PutTablecell(body, tempstr() << "[" << ssimfile->ssimfile << "](" << ssimfile->ssimfile << ":%)");
            doc::PutTablecell(body, finput.field);
            body << eol;
        }
    }ind_end;
    if (n > 0) {
        doc::PutSecthead(out, "inputs", "Inputs");
        out << "| " << doc::Count(n, "input") << " | read into |" << eol
            << "|---|---|" << eol << body << eol;
    }
}

// Append the messages the namespace NS consumes to OUT, one table per dispatch.
//
// A dispatch is a set of message types and the handler each one is delivered to, so what a
// reader of a module wants to know is which messages reach it and by what route -- and a
// module with three dispatches consumes three sets rather than one, since a message on the
// shared-memory dispatch and a message on the network dispatch arrive by different means.
// So the tables are one per dispatch and the dispatch names each of them.
//
// Every message leads to its ctype's page and every dispatch to its own rows, which is
// where the handler of each message is.
static void PutNsdispatch(algo::cstring &out, algo::strptr ns) {
    bool head = false;
    ind_beg(doc::_db_dispatch_curs, dispatch, doc::_db) {
        if (Pathcomp(dispatch.dispatch, ".LL") == ns) {
            tempstr body;
            int n = 0;
            ind_beg(doc::_db_dispatch_msg_curs, dispatch_msg, doc::_db) {
                doc::FCtype *ctype = Pathcomp(dispatch_msg.dispatch_msg, "/LL") == algo::strptr(dispatch.dispatch)
                    ? doc::ind_ctype_Find(Pathcomp(dispatch_msg.dispatch_msg, "/RR")) : NULL;
                if (ctype) {
                    n++;
                    body << "|";
                    doc::PutTablecell(body, tempstr() << "[" << ctype->ctype << "](ctype:" << ctype->ctype << ")");
                    doc::PutTablecell(body, ch_N(dispatch_msg.comment) > 0 ? algo::strptr(dispatch_msg.comment) : algo::strptr(ctype->comment));
                    body << eol;
                }
            }ind_end;
            if (n > 0 && !head) {
                doc::PutSecthead(out, "input-messages", "Input Messages");
                head = true;
            }
            if (n > 0) {
                out << "| " << doc::Count(n, "message") << " via [" << dispatch.dispatch
                    << "](dispatch:" << dispatch.dispatch << ") | what it is |" << eol
                    << "|---|---|" << eol << body << eol;
            }
        }
    }ind_end;
}

// Append the comptests of the namespace NS to OUT, each leading to its own page.// Append the comptests of the namespace NS to OUT, each leading to its own page.
static void PutNstest(algo::cstring &out, algo::strptr ns) {
    tempstr body;
    int n = 0;
    ind_beg(doc::_db_comptest_curs, comptest, doc::_db) {
        if (doc::target_Get(comptest) == ns) {
            n++;
            body << "|";
            doc::PutTablecell(body, tempstr() << "[" << comptest.comptest << "](comptest:" << comptest.comptest << ")");
            doc::PutTablecell(body, comptest.comment);
            body << eol;
        }
    }ind_end;
    if (n > 0) {
        doc::PutSecthead(out, "comptests", "Comptests");
        out << "| " << doc::Count(n, "comptest") << " | what it asserts |" << eol
            << "|---|---|" << eol << body << eol;
    }
}

// Return the page about the namespace NS, as markdown.
//
// It opens with what the namespace is for, in its own words from dmmeta.ns, and with
// the three answers that are pages of their own rather than sections here -- its
// records, its functions, its access paths.  Then the sections, each one a table, and
// each absent when the namespace has none of that kind: a library has no comptests, a
// ssimdb namespace has no sources, and a heading over an empty table says only that the
// generator did not check.
tempstr doc::NsText(algo::strptr ns) {
    doc::FNs *nsrec = doc::ind_ns_Find(ns);
    tempstr ret;
    if (nsrec) {
        ret << "## " << ns;
        if (ch_N(nsrec->comment) > 0) {
            ret << ": " << nsrec->comment;
        }
        ret << eol << "<a href=\"#" << ns << "\"></a>" << eol << eol;
        PutNsctype(ret, ns);
        PutNssrc(ret, ns);
        PutNsdep(ret, ns);
        PutNsinput(ret, ns);
        PutNsdispatch(ret, ns);
        PutNstest(ret, ns);
    }
    return ret;
}

// Lay the page about the namespace NS onto the page.
void doc::RenderNs(algo::strptr ns) {
    tempstr text(doc::NsText(ns));
    if (ch_N(text) > 0) {
        doc::ParseText(text);
        doc::RenderDoc();
    } else {
        doc::PutNote(tempstr() << "the tree holds no namespace " << ns);
    }
}

// Append one row of a directory listing to OUT: the child at CHILD, shown as NAME and
// named by its title.  DIRQ says the child is a directory, which is drawn with the
// separator it is reached by and is named by the README it holds.
//
// A directory has no title of its own, and the one document in it that is about the
// directory rather than about something in it is its README -- so that document's title
// is what says what the directory is.  A directory holding no README is named by its own
// name and nothing else.
static void PutDirrow(algo::cstring &out, algo::strptr name, algo::strptr child, bool dirq) {
    tempstr at;
    at << child;
    if (dirq) {
        at << "README.md";
    }
    out << "|";
    doc::PutTablecell(out, tempstr() << "[" << name << (dirq ? "/" : "") << "](/" << child << ")");
    doc::PutTablecell(out, doc::Doctitle(at));
    out << eol;
}

// Return the namespace whose tables the directory DIR holds the writing about, and the
// empty string when DIR is not one of those directories.
//
// txt/ssimdb/<ns>/ is where the prose about the tables of one namespace is kept, and the
// directory's own name is the whole of what says which namespace that is.  Reading it
// off the path rather than out of a file is what lets a namespace nobody has written a
// word about have a page all the same.
tempstr doc::Ssimdbns(algo::strptr dir) {
    algo::strptr ns = Pathcomp(dir, "/RL/RR");
    algo::strptr up = Pathcomp(dir, "/RL/RL");
    doc::FNs *nsrec = up == "txt/ssimdb" ? doc::ind_ns_Find(ns) : NULL;
    tempstr ret;
    if (nsrec && nsrec->nstype == "ssimdb") {
        ret << nsrec->ns;
    }
    return ret;
}

// Return how many tables the namespace NS declares.
static int NSsimfile(algo::strptr ns) {
    int ret = 0;
    ind_beg(doc::_db_ssimfile_curs, ssimfile, doc::_db) {
        ret += Pathcomp(ssimfile.ssimfile, ".LL") == ns ? 1 : 0;
    }ind_end;
    return ret;
}

// Append the ssim databases to OUT, each leading to the tables it holds.
//
// A database is a namespace whose records are tables, which dmmeta.ns says and no
// directory does: kafkadb has rows in the database and no directory under txt/ssimdb,
// and a list read off the tree would leave it out while a list read off the database
// carries it.
static void PutSsimdb(algo::cstring &out) {
    tempstr body;
    int n = 0;
    ind_beg(doc::_db_ns_curs, ns, doc::_db) {
        if (ns.nstype == "ssimdb") {
            n++;
            body << "|";
            doc::PutTablecell(body, tempstr() << "[" << ns.ns << "](/txt/ssimdb/" << ns.ns << "/)");
            doc::PutTablecell(body, ns.comment);
            doc::PutTablecell(body, tempstr() << NSsimfile(ns.ns));
            body << eol;
        }
    }ind_end;
    if (n > 0) {
        doc::PutSecthead(out, "databases", "Databases");
        out << "| " << doc::Count(n, "database") << " | what it holds | tables |" << eol
            << "|---|---|---|" << eol << body << eol;
    }
}

// Append the tables of the namespace NS to OUT, each leading to its own page.
//
// A table is named the way a reader types it -- the ssimfile's name, lower case, which
// is what a query carries -- while the page it leads to is the page about the ctype
// those records are, since the declaration and the prose are what somebody opening a
// table wants first.
static void PutSsimdbtable(algo::cstring &out, algo::strptr ns) {
    tempstr body;
    int n = 0;
    ind_beg(doc::_db_ssimfile_curs, ssimfile, doc::_db) {
        doc::FCtype *ctype = Pathcomp(ssimfile.ssimfile, ".LL") == ns ? doc::ind_ctype_Find(ssimfile.ctype) : NULL;
        if (ctype) {
            n++;
            body << "|";
            doc::PutTablecell(body, tempstr() << "[" << ssimfile.ssimfile << "](ctype:" << ctype->ctype << ")");
            doc::PutTablecell(body, ctype->comment);
            body << eol;
        }
    }ind_end;
    if (n > 0) {
        doc::PutSecthead(out, "tables", "Tables");
        out << "| " << doc::Count(n, "table") << " | what it holds |" << eol
            << "|---|---|" << eol << body << eol;
    }
}

// Append the children of the directory DIR to OUT, leaving out the child SKIP.
//
// The walk goes over dev.gitfile, so what it shows is what git tracks and nothing else --
// the same rule the server answers by, arrived at from the same table rather than from a
// second one that could disagree with it.
//
// The children come out in path order, and each is named by its own title.
static void PutDirchild(algo::cstring &out, algo::strptr dir, algo::strptr skip) {
    tempstr last;
    tempstr body;
    int n = 0;
    ind_beg(doc::_db_gitfile_curs, gitfile, doc::_db) {
        if (StartsWithQ(gitfile.gitfile, dir)) {
            algo::strptr rest = doc::RestFrom(gitfile.gitfile, ch_N(dir));
            int slash = algo::FindChar(rest, '/');
            algo::strptr name = slash >= 0 ? algo::strptr(rest.elems, slash) : rest;
            tempstr child;
            child << dir << name << (slash >= 0 ? "/" : "");
            if (child != last && child != skip) {
                n++;
                PutDirrow(body, name, child, slash >= 0);
                ch_RemoveAll(last);
                last << child;
            }
        }
    }ind_end;
    if (n > 0 && ch_N(skip) > 0) {
        doc::PutSecthead(out, "in-this-directory", tempstr() << "In " << dir);
    }
    if (n > 0) {
        out << "| name | what it is |" << eol << "|---|---|" << eol << body << eol;
    }
}

// Return the page of what the key KEY could have meant, as markdown.
//
// A key that resolves to nothing has still been half looked for.  The documents whose path
// carries it are one answer and the tables and ctypes whose name carries it are another,
// and a key is often genuinely both -- `x2test` names two tables of this tree, and choosing
// between them is the reader's business rather than this tool's.
//
// The second list is headed Disambiguation, since that is the question the page is asking.
// Every entry is a link, so a reader picks one where they would otherwise type the name a
// second time.
tempstr doc::MatchText(algo::strptr key) {
    int nguess = 0;
    tempstr guess(doc::GuessLoc(key, 50, nguess));
    int nshown = i32_Min(nguess, 50);
    tempstr ret;
    ret << "## " << key << eol << eol;
    if (doc::c_match_N() == 0 && nguess == 0) {
        ret << "no document, table or ctype of the tree is named that" << eol << eol;
    }
    if (doc::c_match_N() > 0) {
        doc::PutSecthead(ret, "documents", "Documents");
        ind_beg(doc::_db_c_match_curs, readmefile, doc::_db) {
            ret << "* [" << readmefile.gitfile << "](/" << readmefile.gitfile << ")" << eol;
        }ind_end;
        ret << eol;
    }
    if (nguess > 0) {
        doc::PutSecthead(ret, "disambiguation", "Disambiguation");
        ind_beg(algo::Line_curs, loc, guess) {
            ret << "* [" << loc << "](" << loc << ")" << eol;
        }ind_end;
        ret << eol;
    }
    if (nguess > nshown) {
        ret << nshown << " of " << nguess << " shown" << eol << eol;
    }
    return ret;
}

// Return the page listing what the directory DIR holds, as markdown.
//
// What a directory holds is usually files, and a document among them is named by its
// title rather than by its path, since the path is what the author had to call the file
// and the title is what the file is about.  A subdirectory has no title and is named by
// itself.
//
// Under txt/ssimdb it is tables instead.  That tree is the writing about the ssim
// database, one directory per namespace, and the tables of a namespace are what a reader
// arriving there is looking for -- so the rows come from the database, which knows every
// table whether or not anybody wrote a line about it.
//
// SKIP is the one child to leave out, and it says which of two pages this is.  Empty,
// the listing is the directory's own page and opens with the directory's name.  Named,
// the listing is being appended to a document of that directory -- a README, whose
// subject is what the directory is about where the listing is what it holds -- and the
// document it is appended to is the one child there is no point offering.
tempstr doc::DirText(algo::strptr dir, algo::strptr skip) {
    tempstr ns(doc::Ssimdbns(dir));
    doc::FNs *nsrec = ch_N(ns) > 0 ? doc::ind_ns_Find(ns) : NULL;
    bool database = dir == "txt/ssimdb/";
    tempstr title;
    tempstr body;
    if (database) {
        title << "Ssim databases";
        PutSsimdb(body);
    } else if (nsrec) {
        title << nsrec->ns << " - " << nsrec->comment;
        PutSsimdbtable(body, nsrec->ns);
    } else {
        title << dir;
        PutDirchild(body, dir, skip);
    }
    tempstr ret;
    if (ch_N(skip) == 0) {
        ret << "## " << title << eol << eol;
    }
    ret << body;
    return ret;
}

// Lay the listing of the directory DIR onto the page, leaving out the child SKIP.
void doc::RenderDir(algo::strptr dir, algo::strptr skip) {
    doc::ParseText(doc::DirText(dir, skip));
    doc::RenderDoc();
}

// Return TEXT with INTRO inserted after its title.
//
// A document opens by saying what it is, and what doc generates about the document --
// the tables of an ssimdb namespace, the link to the internals of the namespace it is
// about -- belongs under that rather than above it, since a reader who has not yet been
// told what they are looking at has nothing to hang either on.  So the insertion goes
// after the first heading, and after the anchor line under it when there is one, since an
// anchor belongs to the heading above it and would otherwise be read as belonging to what
// follows.
tempstr doc::Withintro(algo::strptr text, algo::strptr intro) {
    int nline = 0;
    int at = -1;
    tempstr ret;
    ind_beg(algo::Line_curs, line, text) {
        bool head = at < 0 && ch_N(line) > 0 && line.elems[0] == '#';
        bool mark = at == nline - 1 && StartsWithQ(line, "<a href=");
        at = head || mark ? nline : at;
        nline++;
    }ind_end;
    nline = 0;
    ind_beg(algo::Line_curs, line, text) {
        ret << line << eol;
        if (nline == at) {
            ret << eol << intro;
        }
        nline++;
    }ind_end;
    if (at < 0) {
        ch_RemoveAll(ret);
        ret << intro << eol << text;
    }
    return ret;
}


// Return true when the document at PATH is where a namespace's tables are written about.
static bool SsimdbreadmeQ(algo::strptr path) {
    tempstr dir;
    dir << Pathcomp(path, "/RL") << "/";
    return EndsWithQ(path, "/README.md") && ch_N(doc::Ssimdbns(dir)) > 0;
}

// Return true when the page for the document at PATH ends with the listing of its own
// directory.
//
// A README does, since what its directory holds is the other half of what a reader
// arriving there is asking about.  A README under txt/ssimdb does not: what that
// directory holds is the namespace's tables, and they have already gone under the title.
bool doc::FootdirQ(algo::strptr path) {
    return EndsWithQ(path, "/README.md") && !SsimdbreadmeQ(path);
}

// Return what doc generates about the document at PATH to go under its title, as
// markdown.
//
// A document under txt/ssimdb is about a namespace's tables, and the tables go here, under
// the title: they are what a reader arriving there is looking for, and the page has
// nothing else to say first.  A document under txt/protocol is about a namespace's
// messages, and its ctypes go here for the same reason -- one convention, two kinds of
// namespace, and in both the list is computed from the database rather than pasted into
// the file by a generator.  Every other document has nothing above its own first
// paragraph.
//
// The headings of the document are not here.  They are the pane, which is where a map
// belongs on both devices -- a list of them in the body scrolls away with the text and
// says exactly what the pane says.
tempstr doc::IntroText(algo::strptr path) {
    tempstr dir;
    dir << Pathcomp(path, "/RL") << "/";
    tempstr ns(doc::Docns(path));
    doc::FNs *nsrec = ch_N(ns) > 0 ? doc::ind_ns_Find(ns) : NULL;
    tempstr ret(SsimdbreadmeQ(path) ? doc::DirText(dir, path) : tempstr());
    if (nsrec && nsrec->nstype == dmmeta_Nstype_nstype_protocol) {
        PutNsctype(ret, ns);
    }
    return ret;
}

// Return the link to the internals of the namespace the document at PATH is about, as
// markdown, and the empty string when the document is about no namespace.
//
// A README under txt/exe, txt/lib, txt/protocol or txt/ssimdb sits in a directory named
// after a namespace, and that is the whole of what says which document is about which
// namespace.  What the link opens is the namespace page doc computes, so it says what
// the tree holds now rather than what it held when a generator last ran.
//
// It goes at the foot of the page, under the listing of the directory and above the see
// also.  What a reader arrives at a README for is the writing in it, and the machinery
// behind the namespace is where they go once they have read it.
tempstr doc::InternalsText(algo::strptr path) {
    tempstr ns(doc::Docns(path));
    tempstr ret;
    if (ch_N(ns) > 0) {
        doc::PutSecthead(ret, "internals", "Internals");
        ret << "[what " << ns << " is made of](ns:" << ns << ")" << eol << eol;
    }
    return ret;
}

// Return the namespace the document at PATH is about, and the empty string when it is
// about none.
//
// A README under txt/exe, txt/lib, txt/protocol or txt/ssimdb sits in a directory named
// after a namespace, and that path is the whole of what says which document is about
// which one.  Reading it off the path rather than out of the file is what lets a
// namespace nobody has written a word about carry the same page as one they have.
tempstr doc::Docns(algo::strptr path) {
    algo::strptr dir = Pathcomp(path, "/RL");
    algo::strptr kind = Pathcomp(Pathcomp(dir, "/RL"), "/RR");
    algo::strptr name = Pathcomp(dir, "/RR");
    bool under = StartsWithQ(path, "txt/") && EndsWithQ(path, "/README.md");
    bool named = kind == "exe" || kind == "lib" || kind == "protocol" || kind == "ssimdb";
    doc::FNs *nsrec = under && named ? doc::ind_ns_Find(name) : NULL;
    tempstr ret;
    if (nsrec) {
        ret << nsrec->ns;
    }
    return ret;
}

// Return the ssimfile whose prose the heading anchored at FRAG opens, or the empty
// string when FRAG anchors no such heading.
//
// A table's prose is anchored at the table's own name, which is the one spelling of a
// synthesized page a file in the tree can carry: a markdown link may name a path and a
// fragment and nothing else, so `txt/ssimdb/dmmeta/README.md#dmmeta-bitfld` is how a
// generated field table points at a table.  The anchor is matched by building it from
// each ssimfile rather than by taking it apart, since the transformation that made it
// throws characters away.
tempstr doc::Ssimanchor(algo::strptr frag) {
    tempstr ret;
    ind_beg(doc::_db_ssimfile_curs, ssimfile, doc::_db) {
        tempstr want;
        algo::strptr name = ssimfile.ssimfile;
        frep_(i, ch_N(name)) {
            want << (algo_lib::IdentCharQ(name.elems[i]) ? name.elems[i] : '-');
        }
        if (want == frag) {
            ret << ssimfile.ctype;
        }
    }ind_end;
    return ret;
}

// Return the prose somebody wrote about the ssimfile SSIMFILE, as markdown, and the
// empty string when nobody wrote any.
//
// Most of what there is to say about a table is in the database and is computed, but not
// all of it: what a bitfield is for, and the example that makes it clear, is writing.  A
// namespace's tables are written about in one file, txt/ssimdb/<ns>/README.md, and each
// table's prose sits under a heading naming that table -- so the heading is the whole of
// what says which prose belongs to which table, and one file holds a namespace's
// documentation instead of one file per table holding a title and little else.
//
// The heading itself is left behind, since the page it is being put on already says
// which table this is, and so is the anchor line under it.  What is taken is everything
// down to the next heading of the same level or shallower.
tempstr doc::Ssimdoc(algo::strptr ssimfile) {
    tempstr path;
    path << "txt/ssimdb/" << Pathcomp(ssimfile, ".LL") << "/README.md";
    tempstr want;
    want << "### " << ssimfile;
    tempstr ret;
    bool in = false;
    if (algo::FileQ(path)) {
        ind_beg(algo::FileLine_curs, line, path) {
            bool head = ch_N(line) > 0 && line.elems[0] == '#';
            bool open = head && algo::Trimmed(line) == algo::strptr(want);
            bool mark = in && ch_N(ret) == 0 && StartsWithQ(line, "<a href=");
            in = open || (in && !head);
            if (in && !open && !mark) {
                ret << line << eol;
            }
        }ind_end;
    }
    return algo::Trimmed(ret) == "" ? tempstr() : ret;
}

// Return the ssimfile that holds the records of the ctype NAME, or the empty string
// when the ctype is not a table.
tempstr doc::CtypeSsimfile(algo::strptr name) {
    doc::FSsimfile *ssimfile = doc::ind_ssimfile_ctype_Find(name);
    tempstr ret;
    if (ssimfile) {
        ret << ssimfile->ssimfile;
    }
    return ret;
}

// Append to OUT the tables whose key is built out of the ctype NAME, and the tables
// that name it some other way.
//
// A field whose arg is this ctype is a reference to it, and the reftype says which kind.
// A Pkey makes the referring table's own key contain this one, which is what a subset
// is.  A Base is a program holding a projection of these rows in memory, which is what
// says who reads the table.  Anything else is a table that merely mentions it, and a
// field in the `command` namespace is a command line that takes it as an option.
//
// All four are read off the field table walked once, since the question is about every
// field in the tree rather than about the fields of this ctype.
static void PutCtyperef(algo::cstring &out, algo::strptr name) {
    tempstr subset;
    tempstr related;
    tempstr command;
    tempstr used;
    ind_beg(doc::_db_field_curs, field, doc::_db) {
        algo::strptr owner = Pathcomp(field.field, ".RL");
        algo::strptr ns = Pathcomp(owner, ".LL");
        doc::FNs *nsrec = field.arg == name ? doc::ind_ns_Find(ns) : NULL;
        bool built = nsrec && (nsrec->nstype == "exe" || nsrec->nstype == "lib");
        tempstr table(field.arg == name ? doc::CtypeSsimfile(owner) : tempstr());
        bool cmd = field.arg == name && ns == "command";
        if (built && field.reftype == "Base") {
            used << "- [" << ns << "](ns:" << ns << ") as [" << owner << "](ctype:" << owner
                 << ")" << eol;
        }
        if (ch_N(table) > 0 && field.reftype == "Pkey") {
            subset << "- [" << table << " via " << Pathcomp(field.field, ".RR") << "](ctype:"
                   << owner << ")" << eol;
        } else if (ch_N(table) > 0) {
            related << "- [" << table << " via " << Pathcomp(field.field, ".RR") << "](ctype:"
                    << owner << ")" << eol;
        }
        if (cmd) {
            command << "- [" << Pathcomp(owner, ".RR") << " -" << Pathcomp(field.field, ".RR")
                    << "](/txt/exe/" << Pathcomp(owner, ".RR") << "/README.md) &mdash; "
                    << algo::Trimmed(field.comment) << eol;
        }
    }ind_end;
    if (ch_N(subset) > 0) {
        doc::PutSecthead(out, "subsets", "Subsets");
        out << subset << eol;
    }
    if (ch_N(related) > 0) {
        doc::PutSecthead(out, "related", "Related");
        out << related << eol;
    }
    if (ch_N(command) > 0) {
        doc::PutSecthead(out, "used-in-commands", "Used in commands");
        out << command << eol;
    }
    if (ch_N(used) > 0) {
        doc::PutSecthead(out, "read-by", "Read by");
        out << used << eol;
    }
}

// Append to OUT the named constants declared for the fields of the ctype NAME.
static void PutCtypeconst(algo::cstring &out, algo::strptr name) {
    tempstr body;
    ind_beg(doc::_db_fconst_curs, fconst, doc::_db) {
        algo::strptr field = Pathcomp(fconst.fconst, ".RL");
        if (Pathcomp(field, ".RL") == name) {
            body << "|";
            doc::PutTablecell(body, Pathcomp(field, ".RR"));
            doc::PutTablecell(body, Pathcomp(fconst.fconst, ".RR"));
            doc::PutTablecell(body, fconst.value.value);
            doc::PutTablecell(body, fconst.comment);
            body << eol;
        }
    }ind_end;
    if (ch_N(body) > 0) {
        doc::PutSecthead(out, "constants", "Constants");
        out << "| field | fconst | value | what it means |" << eol
            << "|---|---|---|---|" << eol << body << eol;
    }
}

// Return what the ssim database says about the ctype NAME beyond its own declaration,
// as markdown.
//
// A declaration says what a record holds; these sections say where it sits.  Which
// tables build their key out of this one, which merely mention it, which command lines
// take it as an option, and what named values its fields carry -- each of those is a
// walk of a table doc already holds, and none of them is visible in the rows of the
// ctype itself.
tempstr doc::CtypeText(algo::strptr name) {
    tempstr ret;
    PutCtyperef(ret, name);
    PutCtypeconst(ret, name);
    return ret;
}

// Return the name of the function the prototype PROTO declares, or nothing when PROTO
// declares none.
//
// The name is the identifier just before the argument list, and the namespace in front
// of it comes off -- a pane full of `lib_x2::` says nothing, since every entry on a
// file's page shares it.
tempstr doc::Funcname(algo::strptr proto) {
    int open = algo::FindChar(proto, '(');
    int end = open;
    while (end > 0 && (proto.elems[end - 1] == ' ' || proto.elems[end - 1] == '\t')) {
        end--;
    }
    int beg = end;
    while (beg > 0 && (algo_lib::AlphaCharQ(proto.elems[beg - 1]) || algo_lib::DigitCharQ(proto.elems[beg - 1])
                       || proto.elems[beg - 1] == '_' || proto.elems[beg - 1] == '~')) {
        beg--;
    }
    tempstr ret;
    if (open > 0 && end > beg) {
        ret << algo::strptr(proto.elems + beg, end - beg);
    }
    return ret;
}

// Append one entry of the pane to OUT: TEXT, leading to TARGET, indented DEPTH steps
// under the entry above it.
static void PutNaventry(algo::cstring &out, algo::strptr target, algo::strptr text, int depth) {
    frep_(i, depth) {
        out << "  ";
    }
    out << "- [" << text << "](" << target << ")" << eol;
}

// Append the headings of the blocks now parsed to OUT, each an entry leading to itself,
// indented DEPTH steps under whatever it is being listed below.  TITLE says the first
// heading is already an entry of its own and is left out here.
//
// The blocks rather than the file, because a page doc generated for itself has headings
// too and is not a file anybody can re-read.
//
// Every heading is indented by how far below the shallowest of them it sits, which is what
// makes the depth of a section visible in the pane instead of being written into the page
// as a list nobody can collapse.  A fourth level is left out: it is a paragraph with a name
// rather than a place in the document, and a pane listing every one of them is longer than
// the page it maps.
//
// A heading carrying no anchor is left out as well, since an entry leading nowhere is worse
// than no entry.  An entry names the fragment alone, so following one is a move inside the
// page rather than a fresh request for it: a browser scrolls, and the reader at the terminal
// lands on the row the anchor was laid out on.
static void PutNavsect(algo::cstring &out, int depth, bool title) {
    int top = 9;
    int nhead = 0;
    ind_beg(doc::_db_block_curs, block, doc::_db) {
        bool listed = block.blocktype == doc_Blocktype_heading && ch_N(block.anchor) > 0
            && block.level <= 3;
        top = listed ? i32_Min(top, i32(block.level)) : top;
    }ind_end;
    ind_beg(doc::_db_block_curs, block, doc::_db) {
        bool head = block.blocktype == doc_Blocktype_heading;
        bool listed = head && ch_N(block.anchor) > 0 && block.level <= 3
            && !(title && nhead == 0);
        nhead += head ? 1 : 0;
        if (listed) {
            PutNaventry(out, tempstr() << "#" << block.anchor, block.text,
                        depth + i32(block.level) - top);
        }
    }ind_end;
}

// Append the entries for the sections of the page at LOC to OUT, and for the functions
// the code section named when it is open.
//
// The sections are entries whether or not they are open, because the pane is what says the
// page has a code section at all.  The functions under it are the ones the body just
// listed, recorded as it went -- asking src_func again to build the pane would double the
// slowest question on the page.
//
// Every page that carries the sections has these entries, and the two kinds of page that
// carry them are a page of records and the README of a namespace, which is why this is
// here rather than in either of the two panes.
//
// A section that opens and closes gets the same link in the pane that its heading carries:
// the option that draws it, and then the anchor.  A reader who picks Access Paths out of
// the pane asked to see the access paths, so the entry has to flip the option the way the
// heading does -- an entry naming the anchor alone lands them on a closed heading, with
// the triangle still pointing the other way and nothing underneath it.  The sections that
// never close name no option, and their entries stay plain anchors.
static void PutNavfold(algo::cstring &out, algo::strptr loc) {
    if (doc::SectionQ(loc)) {
        PutNaventry(out, "-vis#access-paths", "Access Paths", 0);
        PutNaventry(out, "-code#code", ch_N(doc::Sectionns(loc)) > 0 ? "Functions" : "Code", 0);
        ind_beg(algo::Line_curs, name, doc::_db.navfunc) {
            PutNaventry(out, tempstr() << "#fn-" << name, name, 1);
        }ind_end;
    }
    if (ch_N(doc::Datassimfile(loc)) > 0) {
        PutNaventry(out, "-data#data", "Data", 0);
    }
    if (ch_N(doc::Rectranscript(loc)) > 0) {
        PutNaventry(out, "#transcript", "Transcript", 0);
    }
    if (ch_N(doc::Recfunc(loc)) > 0) {
        PutNaventry(out, "#as-function", "As Function", 0);
    }
    if (ch_N(doc::Wholetable(doc::LocQuery(loc))) > 0) {
        PutNaventry(out, "#ssimfile", "Ssimfile", 0);
    }
}

// Append the documents of the directory holding PATH to OUT, with the headings of PATH
// nested under its own entry.
//
// A document is one of several in its directory, and the others are where a reader goes
// next: the tutorials beside a tutorial, the chapters beside an overview.  So the pane is
// the directory and the page together -- the documents in the order the directory holds
// them, and under the one being read, its own sections.
//
// The page keeps its place in that list rather than being lifted to the top, because a
// reader who walks from one document to the next finds the pane the same shape every time,
// with a different entry opened.
//
// Each document is named by its title rather than by its path.  A subdirectory is left out:
// its own README is what a reader wants of it, and that is one step further than a pane
// should reach.
//
// The documents are the rows of dev.readmefile, which is the tree's own list of them.  A
// markdown file with no such row -- a fixture under test/, a file added since the last
// abt_md run -- is not in that list, so its page carries its own headings alone rather than
// nothing at all.
//
// The document is parsed here rather than read out of whatever the blocks hold.  A browser
// builds the pane after the body, and the last thing a README's body parses is the listing
// of its own directory -- so a pane reading those blocks would map the listing and call it
// the document.
//
// It is parsed with what doc adds to it, which is the page the reader is looking at: the
// tables of an ssimdb namespace and the ctypes of a protocol one are headings of that page,
// and a pane drawn from the file alone would leave them out.
//
// The entries come out in the order the page does, which is why the internals are the last
// of them rather than a heading of the document: on the page they sit under the sections,
// and a map that listed them above would send a reader to the wrong end of it.
static void PutNavdoc(algo::cstring &out, algo::strptr path) {
    tempstr intro(doc::IntroText(path));
    tempstr foot(doc::InternalsText(path));
    doc::ParseFile(path);
    if (ch_N(intro) > 0) {
        doc::ParseText(doc::Withintro(algo::FileToString(path, algo::FileFlags()), intro));
    }
    tempstr dir;
    dir << Pathcomp(path, "/RL") << "/";
    bool listed = false;
    ind_beg(doc::_db_readmefile_curs, readmefile, doc::_db) {
        algo::strptr rest = doc::RestFrom(readmefile.gitfile, ch_N(dir));
        bool under = StartsWithQ(readmefile.gitfile, dir) && algo::FindChar(rest, '/') < 0;
        bool here = under && algo::strptr(readmefile.gitfile) == path;
        tempstr title(under ? doc::Doctitle(readmefile.gitfile) : tempstr());
        if (under) {
            PutNaventry(out, tempstr() << "/" << readmefile.gitfile,
                        ch_N(title) > 0 ? algo::strptr(title) : rest, 0);
        }
        if (here) {
            listed = true;
            PutNavsect(out, 1, true);
        }
    }ind_end;
    if (!listed) {
        PutNavsect(out, 0, false);
    }
    PutNavfold(out, path);
    if (ch_N(foot) > 0) {
        doc::ParseText(foot);
        PutNavsect(out, 0, false);
    }
}

// Append the functions of the file at PATH to OUT as entries, at most NMAX of them, each
// leading to the line the function starts on.
//
// A source file runs to thousands of lines, and what a reader wants of it first is its
// shape: the list of what it declares.  src_func is what knows that shape, and asking it
// costs the page what a query page already costs.
static void PutNavfunc(algo::cstring &out, algo::strptr path, int nmax) {
    command::src_func src_func;
    src_func.in = doc::_db.cmdline.in;
    src_func.gen = true;
    src_func.showloc = true;
    (void)targsrc_ReadStrptrMaybe(src_func, tempstr() << "%" << path);
    tempstr answer(doc::ToolOut(command::src_func_ToCmdline(src_func)));
    int nfunc = 0;
    ind_beg(algo::Line_curs, line, answer) {
        int end = doc::LocEnd(line);
        int colon = algo::FindChar(line, ':');
        tempstr name(end >= 0 ? doc::Funcname(doc::RestFrom(line, doc::SkipSpace(line, end + 1))) : tempstr());
        if (ch_N(name) > 0) {
            nfunc++;
            if (nfunc <= nmax) {
                tempstr at;
                at << "#L" << algo::strptr(line.elems + colon + 1, end - colon - 1);
                PutNaventry(out, at, name, 0);
            }
        }
    }ind_end;
}


// Append the entries of the page of records at LOC to OUT.
//
// A namespace's page opens with headings of its own -- its ctypes, its sources, its tests
// -- so those are its entries.  Every other page of records has one section of records
// instead, which is the page itself.
//
// The namespace's text is parsed here to be read for its headings, which costs the blocks
// now in the table.  That is safe on both devices and for opposite reasons: at the terminal
// the pane is drawn before the body and the body parses again, and in a browser the pane is
// built after the body is already written out.
static void PutNavrec(algo::cstring &out, algo::strptr loc) {
    if (doc::NsQ(loc)) {
        doc::ParseText(doc::NsText(Pathcomp(loc, ":LR")));
        PutNavsect(out, 0, false);
    } else {
        PutNaventry(out, "#records", "Records", 0);
    }
    PutNavfold(out, loc);
}

// Return the pane of the page at LOC, as markdown, and the empty string when the page has
// no map worth drawing.
//
// The pane is where a reader sees the shape of what they are reading: the headings of a
// document, the sections of a page of records, the functions of a listing.  It is markdown
// like every other page this tool generates, which is what lets one text be drawn above
// the page at the terminal and beside it in a browser -- two placements of one map rather
// than two maps.
//
// The headings are not written into the page as a list of chapters.  A list in the body
// scrolls away with the text and cannot be collapsed, and it says exactly what the pane
// says, so the pane is where it belongs on both devices.
//
// Every page's pane is its own anchors, whatever the page turned out to be, so nothing here
// asks what kind of page it is except to know what to add: the sections of a page of
// records, the functions of a listing, the other documents beside a document.  A page whose
// body holds no anchor at all -- the listing of a directory, one function's source -- has
// nothing to map and carries no pane.
//
// A fourth-level heading keeps its anchor in the file, since a document elsewhere may link
// to it; what it does not get is an entry here, where it would bury the sections it sits
// under.
tempstr doc::NavText(algo::strptr loc) {
    bool leaf = !doc::QueryQ(loc) && !doc::DirQ(loc) && algo::FileQ(loc);
    bool listing = leaf && !EndsWithQ(loc, ".md");
    tempstr ret;
    if (doc::ComptestQ(loc)) {
        PutNaventry(ret, "#records", "Records", 0);
        PutNaventry(ret, "#transcript", "Transcript", 0);
        PutNavfold(ret, loc);
    } else if (doc::RecpageQ(loc)) {
        PutNavrec(ret, loc);
    } else if (leaf && !listing) {
        PutNavdoc(ret, loc);
    } else {
        PutNavsect(ret, 0, false);
    }
    if (listing) {
        PutNavfunc(ret, loc, 200);
    }
    return ret;
}

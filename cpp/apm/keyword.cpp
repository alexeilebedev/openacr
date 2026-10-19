// Copyright (C) 2026 AlgoX2 Corp
//
// License: Apache
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Target: apm (exe) -- Algo Package Manager
// Exceptions: yes
// Source: cpp/apm/keyword.cpp
//
// The words a package must not carry.  A package published downstream is read
// by people who have never seen the tree it was published from, so a name that
// belongs to that tree -- a namespace, a tool, a host -- is at best noise and at
// worst a reference to something they cannot look up.  This file checks every
// record, file name and line a package publishes against the words its
// relations forbid it, and every link of a published document against the files
// that go out with it.

#include "include/algo.h"
#include "include/apm.h"


// TRUE when LINE carries a word REGX matches.
//
// The test is per word rather than per line because the words that must not
// appear are prefixes -- a namespace, a tool -- and a substring search on a
// prefix answers yes to any word that merely contains it: a forbidden prefix of
// two characters is held by any identifier and by half the hex constants in the
// tree without being that prefix.  A word here is what a name is made of,
// letters, digits and underscore, and every other character ends one.
static bool MentionQ(algo_lib::Regx &regx, strptr line) {
    bool ret = false;
    int beg = 0;
    for (int i = 0; i <= line.n_elems; i++) {
        char c = i < line.n_elems ? line[i] : ' ';
        bool wordchar = (c>='a'&&c<='z') || (c>='A'&&c<='Z') || (c>='0'&&c<='9') || c=='_';
        if (!wordchar) {
            if (i > beg && Regx_Match(regx, strptr(line.elems + beg, i - beg))) {
                ret = true;
            }
            beg = i + 1;
        }
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Add to c_keyword the words PACKAGE must not contain, walking the packages that
// depend on it.  Two relations supply the words.  A package's identifying words
// (forbid:N) are kept out of every package it builds on, so a package that
// extends openacr keeps its own names out of openacr; a package that PACKAGE
// contains ships inside it, so its words are PACKAGE's own and stay allowed, and
// FORBID false skips contain edges.  A package's forbidden words (forbid:Y) are
// kept out of the package and of everything it carries, so FORBID true follows
// every edge.  The visited flag stops the walk at a package it has seen, so the
// depth is that of the package graph, a handful.
static void CollectKeyword(apm::FPackage &package, bool forbid) {
    ind_beg(apm::package_c_pkgdep_parent_curs, pkgdep, package) {
        apm::FPackage &dependent = *pkgdep.p_package;
        bool contain = pkgdep.pkgdeptype == apm::dev_pkgdeptype_contain;
        if ((forbid || !contain) && !dependent.visited) {
            dependent.visited = true;
            ind_beg(apm::package_c_pkgkeyword_curs, pkgkeyword, dependent) if (pkgkeyword.forbid == forbid) {
                apm::c_keyword_Insert(pkgkeyword);
            }ind_end;
            CollectKeyword(dependent, forbid);
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

// Add to c_keyword the words PACKAGE must not contain, and the words of every
// package it ships inside.  apm is contained in openacr and goes out with it, so
// the words a package extending openacr keeps out of openacr are kept out of apm
// as well.  FORBID chooses the kind of word, as for CollectKeyword.
static void CollectShippedKeyword(apm::FPackage &package, bool forbid) {
    CollectKeyword(package, forbid);
    ind_beg(apm::package_c_pkgdep_curs, pkgdep, package) if (pkgdep.pkgdeptype == apm::dev_pkgdeptype_contain) {
        CollectShippedKeyword(*pkgdep.p_parent, forbid);
    }ind_end;
}

// -----------------------------------------------------------------------------

// TRUE when the published file FNAME is read for words.  Generated code is
// regenerated downstream from that tree's own schema, data/ is records, which
// are checked as records, extern/ is third-party code, a symlink names a built
// binary, and a file holding a NUL byte in its first 8K is binary.
static bool ScanFileQ(strptr fname) {
    bool ret = false;
    StatStruct st;
    bool plain = lstat(Zeroterm(tempstr() << fname), &st) == 0 && S_ISREG(st.st_mode);
    if (plain && FindStr(fname, "/gen/") == -1 && !StartsWithQ(fname, "data/") && !StartsWithQ(fname, "extern/")) {
        algo_lib::MmapFile file;
        MmapFile_Load(file, fname);
        ret = FindChar(FirstN(file.text, 8192), '\0') == -1;
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Print the first ten of a package's findings, or every one under -v: PACKAGE
// names it, WHERE says which record or which file and line, and TEXT is what
// carries the word.  NBAD counts every finding and NSHOW the ones printed.
static void ReportKeyword(apm::FPackage &package, strptr where, strptr text, int &nbad, int &nshow) {
    nbad++;
    if (nshow < 10 || algo_lib::_db.cmdline.verbose > 0) {
        prerr("apm.keyword"
              <<Keyval("package",package.package)
              <<Keyval("where",where)
              <<Keyval("text",text));
        nshow++;
    }
}

// -----------------------------------------------------------------------------

// Report each line of the published file FNAME on which REGX matches a word,
// counting into NBAD and NSHOW for PACKAGE.  Five kinds of line are skipped,
// because the destination regenerates them or they are owed: a copyright
// notice, which names the owner of the code in every tree that carries it; a
// line inside a header's update-hdr block, a README's usage block, or the
// fenced output of a document's inline command, all rebuilt from that tree's
// own sources and data; and a line that carries ignore:pkgkeyword.  With STUB
// the package carries the file as a stub, and the stub's text is what is
// scanned.
static void ScanFile(apm::FPackage &package, algo_lib::Regx &regx, strptr fname, bool stub, int &nbad, int &nshow) {
    tempstr text(apm::GetPublishText(fname, stub));
    // only a header has an update-hdr block; a document that names the marker
    // is scanned to its last line
    bool header = EndsWithQ(fname, ".h");
    bool hdr = false;
    bool fence = false;
    bool generated = false;
    ind_beg(algo::Line_curs, line, text) {
        // a fence whose first line is inline-command: holds that command's output
        bool fencehead = fence && !generated && StartsWithQ(line, "inline-command:");
        if (fencehead) {
            generated = true;
        }
        bool skip = hdr || generated
            || FindStr(line, "Copyright") != -1
            || FindStr(line, "ignore:pkgkeyword") != -1;
        if (header && FindStr(line, "// update-hdr") != -1) {
            hdr = true;
        } else if (hdr && StartsWithQ(line, "}")) {
            hdr = false;
        }
        if (fence && StartsWithQ(line, "```")) {
            fence = false;
            generated = false;
        } else if (StartsWithQ(line, "```")) {
            fence = true;
            generated = StartsWithQ(line, "```usage");
        }
        if (!skip && MentionQ(regx, line)) {
            ReportKeyword(package, tempstr() << fname << ":" << ind_curs(line).i+1, line, nbad, nshow);
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

// Report every selected package that carries a word it must not contain, in a
// record, a file name or a line of a published file, and return how many
// findings there are.
//
// Each package names the words that identify it, in dev.pkgkeyword, and the
// rows belong to that package.  So openacr carries none of the words of a
// package that extends it, and carries no list of them either: the list ships
// only with the extender.  A package names the words it must never carry as
// forbid:Y rows, and those hold for what it carries too.
//
// What is checked is the package's evaluation, records and files alike, since
// that is exactly what a push carries, so a downstream table named in a
// published document is reported in the commit that writes it.  A file the
// package maps to another path is checked under that path's name too.
//
// Only the first ten findings of a package are printed, because a package that
// has gone wrong tends to go wrong in bulk, and -v prints them all.  The summary reports how many there
// are alongside how many it showed: a fix written from the printout alone would
// address ten of them and leave the rest to surface next run.
int apm::CheckKeyword() {
    int ret = 0;
    ind_beg(_db_zd_sel_package_curs,package,_db) {
        c_keyword_RemoveAll();
        for (int forbid = 0; forbid <= 1; forbid++) {
            ind_beg(_db_package_curs,other,_db) {
                other.visited = false;
            }ind_end;
            if (forbid) {
                package.visited = true;
                ind_beg(package_c_pkgkeyword_curs, pkgkeyword, package) if (pkgkeyword.forbid) {
                    c_keyword_Insert(pkgkeyword);
                }ind_end;
            }
            CollectShippedKeyword(package, forbid);
        }
        ind_beg(_db_package_curs,other,_db) {
            other.visited = false;
        }ind_end;
        if (c_keyword_N() > 0) {
            algo::ListSep ls("|");
            tempstr expr;
            ind_beg(_db_c_keyword_curs,pkgkeyword,_db) {
                expr << ls << keyword_Get(pkgkeyword);
            }ind_end;
            algo_lib::Regx regx;
            Regx_ReadSql(regx, tempstr() << "(" << expr << ")", true);
            int nbad = 0;
            int nshow = 0;
            zd_selrec_RemoveAll();
            SelectPkgRecs(package);
            ind_beg(_db_zd_selrec_curs, rec, _db) {
                tempstr text;
                text << rec.tuple;
                // a copyright row names an owner, and the notice must name it
                // in every package that carries the owner's code
                bool notice = rec.p_ssimfile->ssimfile == "dev.copyright";
                // an exclusion has to name what it keeps out of the package
                bool exclusion = rec.p_ssimfile->ssimfile == "dev.pkgkey" && attr_GetString(rec.tuple, "exclude") == "Y";
                if (!notice && !exclusion && MentionQ(regx, text)) {
                    ReportKeyword(package, rec.rec, text, nbad, nshow);
                }
                bool file = rec.p_ssimfile->ssimfile == "dev.gitfile";
                if (file && ScanFileQ(Pathcomp(rec.rec, ":LR"))) {
                    ScanFile(package, regx, Pathcomp(rec.rec, ":LR"), apm::StubQ(rec, package), nbad, nshow);
                }
            }ind_end;
            // a mapped file goes out under the mapped name, and a source the
            // package does not carry goes out only there
            ind_beg(package_c_pkgfile_curs, pkgfile, package) {
                apm::FRec *src = ind_rec_Find(tempstr() << "dev.gitfile:" << pkgfile.gitfile);
                if (MentionQ(regx, dest_Get(pkgfile))) {
                    ReportKeyword(package, pkgfile.pkgfile, dest_Get(pkgfile), nbad, nshow);
                }
                if (src && !zd_selrec_InLlistQ(*src) && ScanFileQ(pkgfile.gitfile)) {
                    ScanFile(package, regx, pkgfile.gitfile, false, nbad, nshow);
                }
            }ind_end;
            zd_selrec_RemoveAll();
            if (nbad > 0) {
                prerr("apm.keyword"
                      <<Keyval("package",package.package)
                      <<Keyval("words",expr)
                      <<Keyval("n_bad",nbad)
                      <<Keyval("n_shown",nshow)
                      <<Keyval("comment","the package carries words it must not contain; exclude the keys, or rewrite the lines"));
            }
            ret += nbad;
        }
    }ind_end;
    c_keyword_RemoveAll();
    return ret;
}

// -----------------------------------------------------------------------------

// True when a package of the carrier set maps a file of this tree to PATH of its
// projection, so the projection holds a file at PATH whether or not the carrier
// set carries this tree's own file there.
static bool MappedQ(algo::strptr path) {
    bool ret = false;
    ind_beg(apm::_db_package_curs, package, apm::_db) if (package.visited) {
        ind_beg(apm::package_c_pkgfile_curs, pkgfile, package) {
            ret = ret || apm::dest_Get(pkgfile) == path;
        }ind_end;
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

// Report the reference to record KEY at FNAME:LINENO when the record exists
// here and PACKAGE does not carry it, and return 1 for a report.  A key that
// names no record is abt_md's to report, and it is skipped here.  A link to a
// path that a mapping fills resolves downstream, so it is not reported.
static int ReportLink(apm::FPackage &package, algo::strptr fname, int lineno, algo::strptr key) {
    int ret = 0;
    apm::FRec *rec = apm::ind_rec_Find(key);
    bool mapped = StartsWithQ(key, "dev.gitfile:") && MappedQ(Pathcomp(key, ":LR"));
    if (rec && !apm::CarriedQ(*rec) && !mapped) {
        prerr("apm.deadlink" << Keyval("package", package.package)
              << Keyval("file", tempstr() << fname << ":" << lineno)
              << Keyval("target", key)
              << Keyval("comment", "the document names a record the package does not carry"));
        ret = 1;
    }
    return ret;
}

// -----------------------------------------------------------------------------

// True when the carrier set carries REC, and carries it only as a stub.
static bool CarriedStubQ(apm::FRec &rec) {
    bool carried = false;
    bool whole = false;
    ind_beg(apm::rec_zd_rec_pkgrec_curs, pkgrec, rec) if (pkgrec.p_package->visited) {
        carried = true;
        whole = whole || !pkgrec.stub;
    }ind_end;
    return carried && !whole;
}

// -----------------------------------------------------------------------------

// Report the link at FNAME:LINENO to section ANCHOR of file PATH when PACKAGE's
// carrier set carries PATH only as a stub and the stub has no such anchor, and
// return 1 for a report.  A stub keeps the anchor of its title alone, so a link
// to any other section of the page dangles downstream.  An empty ANCHOR links
// the page, which the stub keeps.
static int ReportStubAnchor(apm::FPackage &package, algo::strptr fname, int lineno, algo::strptr path, algo::strptr anchor) {
    int ret = 0;
    apm::FRec *rec = ch_N(anchor) > 0 ? apm::ind_rec_Find(tempstr() << "dev.gitfile:" << path) : NULL;
    if (rec && CarriedStubQ(*rec) && FindStr(apm::GetStubText(path), tempstr() << "<a href=\"#" << anchor << "\">") == -1) {
        prerr("apm.deadlink" << Keyval("package", package.package)
              << Keyval("file", tempstr() << fname << ":" << lineno)
              << Keyval("target", tempstr() << path << "#" << anchor)
              << Keyval("comment", "the link names a section of a page the package carries as a stub"));
        ret = 1;
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Report each reference of the published markdown file FNAME to a record that
// PACKAGE does not carry, and return how many there are.  A document refers to
// a record in two ways.  A link `](/<path>)` names the file <path>, up to an
// anchor.  An inline span `<ns>.<table>:<key>` names a row, and abt_md resolves
// it against the tree it runs in; `ssimfile:<ns>.<table>` is the short form for
// a row of dmmeta.ssimfile.  A span carrying `%` or `<` is a query or a
// template, and spans inside a fence are skipped, the same as abt_md does.  With
// STUB the package carries the file as a stub, and only the stub's text is
// checked.
static int CheckLinkFile(apm::FPackage &package, algo::strptr fname, bool stub) {
    int ret = 0;
    bool fence = false;
    tempstr text(apm::GetPublishText(fname, stub));
    ind_beg(algo::Line_curs, line, text) {
        int lineno = ind_curs(line).i + 1;
        fence = fence != StartsWithQ(algo::Trimmed(line), "```");
        int i = FindStr(line, "](/");
        while (i != -1) {
            algo::strptr rest = ch_RestFrom(line, i + 3);
            int end = 0;
            while (end < rest.n_elems && rest[end] != ')' && rest[end] != '#' && rest[end] != ' ') {
                end++;
            }
            algo::strptr path = algo::ch_FirstN(rest, end);
            algo::strptr anchor;
            if (end < rest.n_elems && rest[end] == '#') {
                algo::strptr after = algo::ch_RestFrom(rest, end + 1);
                int len = 0;
                while (len < after.n_elems && after[len] != ')' && after[len] != ' ') {
                    len++;
                }
                anchor = algo::ch_FirstN(after, len);
            }
            ret += ReportLink(package, fname, lineno, tempstr() << "dev.gitfile:" << path);
            ret += ReportStubAnchor(package, fname, lineno, path, anchor);
            int next = FindStr(rest, "](/");
            i = next == -1 ? -1 : i + 3 + next;
        }
        algo::StringIter iter(line);
        while (!fence && !iter.EofQ()) {
            (void)algo::GetTokenChar(iter, '`');
            algo::strptr span = algo::GetTokenChar(iter, '`');
            algo::strptr table = algo::Pathcomp(span, ":LL");
            algo::strptr value = algo::Pathcomp(span, ":LR");
            bool keyshape = ch_N(table) > 0 && ch_N(value) > 0
                && algo::FindChar(span, ' ') == -1
                && algo::FindChar(value, '%') == -1
                && algo::FindChar(value, '<') == -1;
            if (keyshape && algo::FindChar(table, '.') != -1) {
                ret += ReportLink(package, fname, lineno, span);
            } else if (keyshape && table == "ssimfile") {
                ret += ReportLink(package, fname, lineno, tempstr() << "dmmeta." << span);
            }
        }
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

// Report every link of a selected package's published markdown that names a
// tracked file the package does not carry, and return how many there are.  A
// markdown file the package maps to another path is published too.  The
// carrier set is the package and every package it carries, the files a push
// sends together, so a link from a contained package into its parent's docs
// resolves.  Every package is checked, whether or not it has a destination yet,
// so a link that would dangle downstream fails the commit that writes it.
int apm::CheckLink() {
    int ret = 0;
    ind_beg(_db_zd_sel_package_curs, package, _db) {
        SetCarrier(&package);
        zd_selrec_RemoveAll();
        SelectPkgRecs(package);
        ind_beg(_db_zd_selrec_curs, rec, _db) if (rec.p_ssimfile->ssimfile == "dev.gitfile") {
            algo::strptr fname = Pathcomp(rec.rec, ":LR");
            if (EndsWithQ(fname, ".md") && ScanFileQ(fname)) {
                ret += CheckLinkFile(package, fname, apm::StubQ(rec, package));
            }
        }ind_end;
        // a source the package maps and does not carry goes out at the mapped path
        ind_beg(package_c_pkgfile_curs, pkgfile, package) {
            apm::FRec *src = ind_rec_Find(tempstr() << "dev.gitfile:" << pkgfile.gitfile);
            if (src && !zd_selrec_InLlistQ(*src) && EndsWithQ(pkgfile.gitfile, ".md") && ScanFileQ(pkgfile.gitfile)) {
                ret += CheckLinkFile(package, pkgfile.gitfile, false);
            }
        }ind_end;
        zd_selrec_RemoveAll();
    }ind_end;
    SetCarrier(NULL);
    return ret;
}

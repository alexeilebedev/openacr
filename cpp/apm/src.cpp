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
// Source: cpp/apm/src.cpp
//

#include "include/algo.h"
#include "include/apm.h"


// -----------------------------------------------------------------------------

// Return true if PATH is a hand-written C++ source or header: a .cpp or .h
// file outside the generated trees.
static bool HandsrcQ(algo::strptr path) {
    bool gen = algo::StartsWithQ(path, "cpp/gen/") || algo::StartsWithQ(path, "include/gen/");
    bool ext = algo::EndsWithQ(path, ".cpp") || algo::EndsWithQ(path, ".h");
    return !gen && ext;
}

// -----------------------------------------------------------------------------

// Return true if CPPNAME, spelled ns.name or name alone, is a symbol of
// generated code, and add to c_srcref each record it comes from that the
// database holds.  The overloads of one user function share a name and come
// from several records.  With DEFINE, the name stands where a definition does,
// and a record whose user function this is goes to c_srcreq too.
static bool NoteCppnameQ(algo::strptr cppname, bool define) {
    apm::FCppname *group = apm::ind_cppname_Find(cppname);
    if (group) {
        ind_beg(apm::cppname_c_cppsym_curs, cppsym, *group) {
            apm::FRec *rec = apm::ind_rec_Find(acrkey_Get(cppsym));
            if (rec) {
                apm::c_srcref_Insert(*rec);
            }
            if (rec && cppsym.extrn && define) {
                apm::c_srcreq_Insert(*rec);
            }
        }ind_end;
    }
    return group != NULL;
}

// Look up identifier IDENT, which follows PREFIX:: in the source, or stands
// alone when PREFIX is empty.  DEFINE says IDENT stands at file scope, where a
// function is defined; inside braces a name is used.  A bare identifier names an enum constant or a
// static reference as it is.  A qualified one names a symbol in namespace
// PREFIX: a type, ns::Name, a string symbol, a user function, or one of the
// functions generated for a type, ns::Name_Func, so a miss drops trailing
// _segments until a type answers or none is left.
static void NoteIdent(algo::strptr prefix, algo::strptr ident, bool define) {
    (void)NoteCppnameQ(ident, define);
    int end = elems_N(prefix) > 0 ? elems_N(ident) : 0;
    bool found = false;
    while (!found && end > 0) {
        found = NoteCppnameQ(tempstr() << prefix << "." << algo::FirstN(ident, end), define);
        end--;
        while (end > 0 && ident[end] != '_') {
            end--;
        }
    }
}

// -----------------------------------------------------------------------------

// Return the index in TEXT just past the token that starts at index I: a
// comment, a string or character literal, a number, an identifier, or one
// other character.  Comments and literals are skipped whole, since a symbol
// mentioned in prose or in a message is not a use of it.
static int SkipToken(algo::strptr text, int i) {
    int n = elems_N(text);
    char c = text[i];
    char next = i + 1 < n ? text[i + 1] : 0;
    int j = i + 1;
    if (c == '/' && next == '/') {
        while (j < n && text[j] != '\n') {
            j++;
        }
    } else if (c == '/' && next == '*') {
        j = i + 2;
        while (j < n && !(text[j - 1] == '*' && text[j] == '/' && j > i + 2)) {
            j++;
        }
        j = j < n ? j + 1 : n;
    } else if (c == '"' || c == '\'') {
        while (j < n && text[j] != c && text[j] != '\n') {
            j += text[j] == '\\' ? 2 : 1;
        }
        j = j < n ? j + 1 : n;
    } else if (algo_lib::IdentCharQ(c)) {
        while (j < n && (algo_lib::IdentCharQ(text[j]) || (algo_lib::DigitCharQ(c) && (text[j] == '\'' || text[j] == '.')))) {
            j++;
        }
    }
    return j;
}

// Return TEXT, a C++ source, without its update-hdr sections.  Such a section
// opens on a line that ends in `// update-hdr` and closes on a `}` in the first
// column, and update-hdr fills it with the prototypes of whatever sources the
// tree compiles.  A tree that receives the file rewrites the section from its
// own sources, so the names in it say nothing about what the file needs.
static tempstr StripUpdateHdr(algo::strptr text) {
    tempstr ret;
    bool section = false;
    ind_beg(algo::Line_curs, line, text) {
        bool open = !section && algo::FindStr(line, "// update-hdr") != -1;
        bool close = section && algo::StartsWithQ(line, "}");
        if (!section && !open) {
            ret << line << "\n";
        }
        section = (section || open) && !close;
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

// Add to c_srcref every record whose generated symbol TEXT, the body of a C++
// source, names.  A `::` between two identifiers makes the second one
// qualified by the first.  Hand-written sources have no namespace blocks, so a
// name outside every brace is where a function is defined.
static void ScanText(algo::strptr text) {
    int n = elems_N(text);
    int i = 0;
    algo::strptr prev;
    bool qual = false;
    int depth = 0;
    while (i < n) {
        char c = text[i];
        char next = i + 1 < n ? text[i + 1] : 0;
        int j = c == ':' && next == ':' ? i + 2 : SkipToken(text, i);
        algo::strptr token = algo::qGetRegion(text, i, j - i);
        bool ident = algo_lib::AlphaCharQ(c) || c == '_';
        if (ident) {
            NoteIdent(qual ? prev : algo::strptr(), token, depth == 0);
        }
        depth += c == '{' ? 1 : 0;
        depth -= c == '}' && depth > 0 ? 1 : 0;
        if (c == ':' && next == ':') {
            qual = elems_N(prev) > 0;
        } else if (ident) {
            prev = token;
            qual = false;
        } else if (!algo_lib::WhiteCharQ(c)) {
            prev = algo::strptr();
            qual = false;
        }
        i = j;
    }
}

// -----------------------------------------------------------------------------

// Group the rows of gendb.cppsym by C++ name, the key a source lookup has.
static void IndexCppname() {
    ind_beg(apm::_db_cppsym_curs, cppsym, apm::_db) {
        apm::FCppname &group = apm::ind_cppname_GetOrCreate(cppname_Get(cppsym));
        apm::c_cppsym_Insert(group, cppsym);
    }ind_end;
}

// Give every hand-written C++ file in the database a reference to each record
// whose generated symbol its code names or whose user function it defines.
// Take cpp/amc/ctype.cpp, which compares a field's reftype against
// dmmeta_Reftype_reftype_Val.  The constant exists because dmmeta.reftype has a
// row for it, so a package that carries the file and not the row ships code that does
// not compile -- and no record says so, because the file's dev.gitfile row
// references nothing.  amc knows which record each symbol comes from and writes
// that to gendb.cppsym, so reading the file is enough to draw the edge.  A user
// function is the same edge in the other direction: a file defining
// atf_comp::comptest_apm_SrcScan needs atfdb.comptest:apm.SrcScan, or amc
// generates no prototype for it and the definition fails to compile.  So the
// file requires that record, and a package that lacks the record cannot carry
// the file.  The record does not require the file in turn, since a package may
// carry a row whose file the origin keeps.  The
// edge is an ordinary reference: the ref closure brings the row with the file,
// and apm -check reports it as dangling for a package that leaves it behind.
void apm::ScanSources() {
    apm::FSsimfile *gitfile = apm::ind_ssimfile_Find("dev.gitfile");
    IndexCppname();
    if (gitfile && apm::cppsym_N() > 0) {
        ind_beg(apm::ssimfile_zd_ssimfile_rec_curs, rec, *gitfile) {
            algo::strptr path = attrs_Find(rec.tuple, 0)->value;
            if (HandsrcQ(path)) {
                ScanText(StripUpdateHdr(algo::FileToString(path)));
                ind_beg(apm::_db_c_srcref_curs, target, apm::_db) {
                    if (&target != &rec) {
                        c_ref_Insert(rec, target);
                        c_parent_Insert(rec, target);
                    }
                }ind_end;
                ind_beg(apm::_db_c_srcreq_curs, target, apm::_db) {
                    if (&target != &rec) {
                        c_req_Insert(rec, target);
                        c_extrn_Insert(rec, target);
                    }
                }ind_end;
                apm::c_srcref_RemoveAll();
                apm::c_srcreq_RemoveAll();
            }
        }ind_end;
    }
}

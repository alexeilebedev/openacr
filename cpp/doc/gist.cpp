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
// Target: doc (exe) -- Render a markdown document to the terminal
// Exceptions: yes
// Source: cpp/doc/gist.cpp
//
// A table that a dmmeta.gstatic names is compiled into a program, and when the program's
// ctype carries a hook, amc binds each of its rows to a function of that program: the row
// `gen:prep_ctype` of amcdb.gen is the function amc.gen_prep_ctype, and the row is how the
// program finds it.  Somebody who
// wrote the row described it in a line, and somebody who wrote the function described it
// at length in the comment above it.  So the two together are the documentation of the
// thing the row names, and the whole table is a document with a section per row.
// The gist page is that document.  It is written as markdown and handed to the parser
// every document goes through, as the namespace page is, so both devices draw it from
// the same text.

#include "include/algo.h"
#include "include/doc.h"

// Return true when LOC names the gist of a selection.
bool doc::GistQ(algo::strptr loc) {
    return doc::ViewWord(loc) == "gist";
}

// Point every ctype at its field of reftype Base, the one naming the ctype it projects.
//
// A program does not hold ssim rows as they are written; it holds a ctype of its own that
// carries the table's ctype as its Base field.  The field's name is whatever its author
// chose, so the reftype is what says which field it is.
void doc::BaseRead() {
    ind_beg(doc::_db_field_curs, field, doc::_db) {
        doc::FCtype *ctype = field.reftype == "Base" ? doc::ind_ctype_Find(doc::ctype_Get(field)) : NULL;
        if (ctype) {
            ctype->c_base = &field;
        }
    }ind_end;
}

// Return the table whose rows the program ctype CTYPE holds a projection of, and NULL when
// CTYPE projects no table.
doc::FSsimfile *doc::Basessimfile(algo::strptr ctype) {
    doc::FCtype *proj = doc::ind_ctype_Find(ctype);
    doc::FField *base = proj ? proj->c_base : NULL;
    return base ? doc::ind_ssimfile_ctype_Find(base->arg) : NULL;
}

// Record, for every table whose rows a program binds to functions, the gstatic that binds
// them, and on each gstatic the hook fields of its ctype.
//
// amc gives each row of a gstatic one function per hook field of the program's ctype, so a
// table compiled in with no hook is a list of constants and stands for no function.  A
// gstatic names a field, and that field's ctype is the program's projection of the table.  A
// table that two programs compile in is bound by the one carrying hooks.
void doc::GstaticRead() {
    ind_beg(doc::_db_field_curs, field, doc::_db) {
        bool hook = field.reftype == "Hook";
        ind_beg(doc::_db_gstatic_curs, gstatic, doc::_db) {
            doc::FField *proj = hook ? doc::ind_field_Find(gstatic.field) : NULL;
            if (proj && proj->arg == doc::ctype_Get(field)) {
                doc::c_hook_Insert(gstatic, field);
            }
        }ind_end;
    }ind_end;
    ind_beg(doc::_db_gstatic_curs, gstatic, doc::_db) {
        doc::FField *proj = doc::ind_field_Find(gstatic.field);
        doc::FSsimfile *ssimfile = proj ? doc::Basessimfile(proj->arg) : NULL;
        if (ssimfile && doc::c_hook_N(gstatic) > 0) {
            ssimfile->p_gstatic = &gstatic;
        }
    }ind_end;
}

// Return the gstatic binding the rows of the table TABLE to functions, and NULL when no
// program does.
doc::FGstatic *doc::Tablegstatic(algo::strptr table) {
    doc::FSsimfile *ssimfile = doc::ind_ssimfile_Find(table);
    return ssimfile ? ssimfile->p_gstatic : NULL;
}

// Return the gist location of the selection QUERY, and the empty string when QUERY selects
// rows of no table that a program binds to functions.
tempstr doc::Gistloc(algo::strptr query) {
    tempstr ret;
    if (doc::Tablegstatic(Pathcomp(query, ":LL"))) {
        ret << "gist:" << query;
    }
    return ret;
}

// Ask src_func for the functions the records QUERY selects stand for, and hold each in
// doc.FDb.gist under the record it stands for, in place of whatever an earlier selection
// left there, with the command line that asked in doc.FDb.gistcmd.
//
// src_func reports with every function the acr key that caused it -- `amcdb.gen:prep_ctype`
// for amc.gen_prep_ctype -- so one question about the whole selection answers for every
// row of it, and the selection is itself a pattern over those keys.  A row whose ctype has
// several hooks has several functions, and each reports the same key.
void doc::GistRead(algo::strptr query) {
    command::src_func src_func;
    src_func.in = doc::_db.cmdline.in;
    (void)command::acrkey_ReadStrptrMaybe(src_func, query);
    src_func.printssim = true;
    doc::gist_RemoveAll();
    doc::gistrec_RemoveAll();
    doc::_db.gistcmd = command::src_func_ToCmdline(src_func);
    tempstr out(doc::ToolOut(doc::_db.gistcmd));
    ind_beg(algo::Line_curs, line, out) {
        algo::Tuple tuple;
        bool row = algo::Tuple_ReadStrptrMaybe(tuple, line) && tuple.head.value == "src_func.func";
        tempstr key(row ? algo::attr_GetString(tuple, "acrkey") : algo::strptr());
        if (ch_N(key) > 0) {
            doc::FGist &gist = doc::gist_Alloc();
            gist.gist = algo::attr_GetString(tuple, "func");
            gist.src = algo::attr_GetString(tuple, "src");
            (void)u32_ReadStrptrMaybe(gist.line, algo::attr_GetString(tuple, "line"));
            (void)u32_ReadStrptrMaybe(gist.endline, algo::attr_GetString(tuple, "endline"));
            gist.p_gistrec = &doc::ind_gistrec_GetOrCreate(key);
            (void)doc::gist_XrefMaybe(gist);
        }
    }ind_end;
}

// Return the comment above the function GIST as markdown that reads as the source does,
// followed by an empty line.  An empty string means the function carries no comment.
//
// The comment markers come off, and what is left is plain text: it is escaped the way a
// row's comment is, so a `<ns>` in it stays a name and a line opening with `#` stays a
// line.  A line indented past the marker stays indented, which is what keeps an example
// in the comment a listing.
static tempstr GistComment(doc::FGist &gist) {
    tempstr text(algo::FileQ(gist.src) ? algo::FileToString(gist.src, algo::FileFlags()) : tempstr());
    int top = doc::CommentTop(text, i32(gist.line));
    tempstr block(doc::Lines(text, top, i32(gist.line) - 1));
    tempstr plain;
    ind_beg(algo::Line_curs, line, block) {
        algo::strptr rest = doc::RestFrom(line, doc::SkipSpace(line, 0) + 2);
        plain << (StartsWithQ(rest, " ") ? doc::RestFrom(rest, 1) : rest) << eol;
    }ind_end;
    tempstr ret;
    doc::PutPlain(ret, plain);
    if (ch_N(ret) > 0) {
        ret << eol;
    }
    return ret;
}

// Append the section of one record to OUT.  LINE is the record as acr printed it, and
// TABLE is the table it was selected from.
//
// The heading is the record's key, since the key is what the pane lists and what a link
// to the section carries.  The comment of the row comes next as the lead, then the comment
// of each function as the body, each followed by the function's name leading to its
// source.
static void PutGistrec(algo::cstring &out, algo::strptr table, algo::strptr line) {
    algo::Tuple tuple;
    bool row = algo::Tuple_ReadStrptrMaybe(tuple, line) && tuple.head.value == table
        && attrs_N(tuple) > 0;
    tempstr key(row ? algo::strptr(attrs_qFind(tuple, 0).value) : algo::strptr());
    doc::FGistrec *gistrec = row ? doc::ind_gistrec_Find(tempstr() << table << ":" << key) : NULL;
    algo::strptr comment = row ? algo::attr_GetString(tuple, "comment") : algo::strptr();
    if (row) {
        doc::PutSecthead(out, key, key);
    }
    if (ch_N(comment) > 0) {
        doc::PutPlain(out, comment);
        out << eol;
    }
    if (gistrec) {
        ind_beg(doc::gistrec_zd_gist_curs, gist, *gistrec) {
            out << GistComment(gist);
            out << "[" << gist.gist << "](func:" << gist.gist << ")" << eol << eol;
        }ind_end;
    }
}

// Return the gist of the selection QUERY as markdown, and the empty string when QUERY
// selects rows of no table that a program binds to functions.
//
// The title is the table and what its ctype says it is.  The sections follow in the order
// the table holds its rows, which is the order somebody chose when they wrote them.
tempstr doc::GistText(algo::strptr query) {
    algo::strptr table = Pathcomp(query, ":LL");
    doc::FSsimfile *ssimfile = doc::Tablegstatic(table) ? doc::ind_ssimfile_Find(table) : NULL;
    doc::FCtype *ctype = ssimfile ? doc::ind_ctype_Find(ssimfile->ctype) : NULL;
    tempstr ret;
    if (ssimfile) {
        GistRead(query);
        tempstr out(doc::AcrFlat(query));
        ret << "## " << table;
        if (ctype && ch_N(ctype->comment) > 0) {
            ret << ": " << ctype->comment;
        }
        ret << eol << "<a href=\"#" << table << "\"></a>" << eol << eol;
        ind_beg(algo::Line_curs, line, out) {
            PutGistrec(ret, table, line);
        }ind_end;
    }
    return ret;
}

// Lay the gist at the location LOC onto the page.
void doc::RenderGist(algo::strptr loc) {
    tempstr text(doc::GistText(doc::LocQuery(loc)));
    if (ch_N(text) > 0) {
        doc::ParseText(text);
        doc::RenderDoc();
    } else {
        doc::PutNote(tempstr() << doc::LocQuery(loc) << " selects rows of no table a program binds to functions");
    }
}

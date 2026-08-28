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
// Source: cpp/doc/docpage.cpp
//
// The pages this tool writes rather than reads, one function each.
// dev.docpage is the list of them: a row per page, its name the word a location
// addresses it by, and its comment what the page shows.  The rows are compiled in,
// each bound to the function below of the same name, so the vocabulary of this tool
// is a table and adding a page is a row plus a function rather than a case in two
// dispatch chains.
// Each function draws the page for the location on `_db.page`, onto whichever device
// asked: `_db.p_out` is where a browser wants the html, and it is NULL when the page
// is being laid onto the screen. That is the whole of what a page generator knows
// about the difference, which is why the two emitters of a page can be read side by
// side.

#include "include/algo.h"
#include "include/doc.h"

// Lay the access paths of the ctypes the location names onto the page.
void doc::docpage_accesspath() {
    algo::strptr regx = Pathcomp(doc::_db.page, ":LR");
    if (doc::_db.p_out) {
        doc::PutAccesspath(*doc::_db.p_out, regx);
    } else {
        doc::RenderAccesspath(regx);
    }
}

// Lay what amc generated for the ctypes of a selection onto the page.
void doc::docpage_code() {
    tempstr query(doc::LocQuery(doc::_db.page));
    if (doc::_db.p_out) {
        doc::PutCodeHtml(*doc::_db.p_out, query);
    } else {
        doc::RenderCode(query);
    }
}

// Lay one ctype onto the page: its declaration, the prose about its table, where it sits.
void doc::docpage_ctype() {
    algo::strptr name = Pathcomp(doc::_db.page, ":LR");
    if (doc::_db.p_out) {
        doc::PutCtype(*doc::_db.p_out, name);
    } else {
        doc::RenderCtype(name);
    }
}

// Lay the functions a selection is about, or one function's source, onto the page.
void doc::docpage_func() {
    bool one = doc::FuncsrcQ(doc::_db.page);
    tempstr query(doc::LocQuery(doc::_db.page));
    if (doc::_db.p_out && one) {
        doc::PutFuncsrc(*doc::_db.p_out, doc::_db.page);
    } else if (doc::_db.p_out) {
        doc::PutFunc(*doc::_db.p_out, query);
    } else if (one) {
        doc::RenderFuncsrc(doc::FuncsrcName(doc::_db.page), doc::FuncsrcSrc(doc::_db.page));
    } else {
        doc::RenderFunc(query);
    }
}

// Lay the page of keys onto the page.
void doc::docpage_help() {
    if (doc::_db.p_out) {
        doc::PutHelp(*doc::_db.p_out);
    } else {
        doc::RenderHelp();
    }
}

// Lay one namespace onto the page: its ctypes, sources, dependencies, inputs, tests.
void doc::docpage_ns() {
    algo::strptr ns = Pathcomp(doc::_db.page, ":LR");
    if (doc::_db.p_out) {
        doc::PutNs(*doc::_db.p_out, ns);
    } else {
        doc::RenderNs(ns);
    }
}

// Lay the message roots, or one message, onto the page.
void doc::docpage_msg() {
    if (doc::_db.p_out) {
        doc::PutMsg(*doc::_db.p_out, doc::_db.page);
    } else {
        doc::RenderMsg(doc::_db.page);
    }
}

// Lay the records a query selects onto the page.
void doc::docpage_query() {
    if (doc::_db.p_out) {
        doc::PutQuery(*doc::_db.p_out, doc::_db.page);
    } else {
        doc::RenderQuery(doc::_db.page);
    }
}

// Return the page the location LOC asks for, or NULL when no generated page answers for it.
//
// The word before the first colon names the page, and a location carrying no such word is
// the records of a query -- which is the one page addressed by what it selects rather than
// by a name.
//
// A bare key is the other case that carries no word: a page about the whole tree has
// nothing to put after a colon, so its own name is the location.
//
// A function's source is the case that carries no word at all: `cpp/doc/ns.cpp:NavText` is
// a file and a name, so the word before its colon is a path.  It answers to the same row as
// `func:`, because both are questions about functions and the argument's own shape is what
// says which of the two was asked.
doc::FDocpage *doc::Docpage(algo::strptr loc) {
    tempstr word(doc::ViewWord(loc));
    doc::FDocpage *ret = NULL;
    if (doc::BarepageQ(loc)) {
        ret = doc::ind_docpage_Find(loc);
    } else if (doc::FuncsrcQ(loc)) {
        ret = doc::ind_docpage_Find("func");
    } else if (ch_N(word) > 0) {
        ret = doc::ind_docpage_Find(word);
    } else if (doc::RecordsQ(loc)) {
        ret = doc::ind_docpage_Find("query");
    }
    return ret;
}

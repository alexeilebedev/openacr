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
// Target: amc (exe) -- Algo Model Compiler: generate code under include/gen and cpp/gen
// Exceptions: yes
// Source: cpp/amc/cppsym.cpp
//
// The C++ symbols of generated code, each with the ssim record it comes from.
// Hand-written code names generated symbols -- a type, an enum constant, a
// string symbol -- and nothing in the schema says which code names which
// record.  amc is the one place that knows both the record and the name it
// emits for it, so every generator that emits a symbol a record implies
// registers the pair here, and gendb.cppsym carries the table out to tools
// that read source, such as apm.

#include "include/algo.h"
#include "include/amc.h"


// Return the key of the record TUPLE holds, <ssimfile>:<first attribute>,
// the spelling every record key takes.
tempstr amc::GetTupleRec(algo::Tuple &tuple) {
    algo::Attr *attr = attrs_Find(tuple, 0);
    return tempstr() << tuple.head.value << ":" << (attr ? strptr(attr->value) : strptr());
}

// -----------------------------------------------------------------------------

// Register CPPNAME, a symbol spelled ns.name (or name alone at global scope),
// as coming from the record whose key is ACRKEY.  With EXTRN, hand-written
// code defines the symbol and generated code calls it; otherwise generated code
// defines it.  One name can come from several records, as the overloads of one
// user function do, so the row's key is the pair.
// An empty CPPNAME, which the schema's empty ctype row yields, names nothing and
// registers nothing.
void amc::InsCppsym(algo::strptr cppname, algo::strptr acrkey, bool extrn DFLTVAL(false)) {
    if (ch_N(cppname) > 0) {
        gendb::Cppsym row;
        row.cppsym = tempstr() << cppname << "/" << acrkey;
        row.extrn = extrn;
        (void)amc::cppsym_InsertMaybe(row);
    }
}

// -----------------------------------------------------------------------------

// Return the C++ name of FUNC as ns.name, or name alone at global scope, the
// spelling gendb.cppsym uses.
tempstr amc::GetCppname(amc::FFunc &func) {
    tempstr ret;
    if (ch_N(ns_Get(func)) > 0) {
        ret << ns_Get(func) << ".";
    }
    ret << algo::Pathcomp(func.proto, "(LL");
    return ret;
}

// -----------------------------------------------------------------------------

// Return the key of the record that makes amc call user function FUNC, as
// <ssimfile>:<pkey>, or empty when no record does.  amc spells the binding
// gstatic/<ssimfile>:<pkey> for a gstatic table, and <table>:<pkey> for a
// dmmeta table such as fstep or dispatch_msg.
tempstr amc::GetFuncAcrkey(amc::FFunc &func) {
    algo::strptr acrkey = algo::StartsWithQ(func.acrkey, "gstatic/") ? algo::Pathcomp(func.acrkey, "/LR") : algo::strptr(func.acrkey);
    algo::strptr table = algo::Pathcomp(acrkey, ":LL");
    bool short_table = ch_N(acrkey) > 0 && algo::FindChar(table, '.') == -1;
    return tempstr() << (short_table ? "dmmeta." : "") << acrkey;
}

// -----------------------------------------------------------------------------

// Mark what the run loaded from disk, before any generator adds ctypes or
// constants of its own.  A loaded ctype is original, and in a namespace with
// C++ output its struct ns::Name comes from dmmeta.ctype.  A loaded
// constant comes from its own dmmeta.fconst row; a constant amc makes up later
// sets its own record or none.
void amc::gen_origin() {
    ind_beg(amc::_db_ctype_curs, ctype, amc::_db) {
        ctype.original = true;
        if (ctype.p_ns->c_nscpp) {
            InsCppsym(ctype.ctype, tempstr() << "dmmeta.ctype:" << ctype.ctype);
        }
    }ind_end;
    ind_beg(amc::_db_fconst_curs, fconst, amc::_db) {
        fconst.rec = tempstr() << "dmmeta.fconst:" << fconst.fconst;
    }ind_end;
}

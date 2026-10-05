// Copyright (C) 2025-2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
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
// Contacting ICE: <https://www.theice.com/contact>
// Target: amc (exe) -- Algo Model Compiler: generate code under include/gen and cpp/gen
// Exceptions: yes
// Source: cpp/amc/query.cpp -- Query mode
//

#include "include/amc.h"

// -----------------------------------------------------------------------------

// Report a query match in a namespace that emits no C++.
// A query names records, not namespaces, so ctype:amcdb.Tclass matches a ctype
// whose namespace is an ssimdb projected to TypeScript alone. There is no C++
// rendering of that record for the query to print. Whether a namespace emits
// C++ at all is stated by its dmmeta.nscpp record, and the per-namespace code
// buffers are allocated from that record, so the record is what a match is
// tested against. Such a match is an error and counts toward the run's exit
// code, which keeps a query that printed nothing from reporting success.
// The report goes to stderr because query mode's stdout carries the generated
// C++ and is consumed programmatically: a caller that wants code alone writes
// 2>/dev/null, and one that folds the two streams together captures the report
// beside the code. KEY names the kind of match, ctype or func; VALUE is the
// matched name.
static void ReportQueryNocpp(amc::FNs &ns, strptr key, strptr value) {
    prerr("amc.query_nocpp"
          <<Keyval("ns",ns.ns)
          <<Keyval(key,value)
          <<Keyval("comment","namespace has no dmmeta.nscpp record and emits no C++"));
    algo_lib::_db.exit_code++;
}

// -----------------------------------------------------------------------------

static void Query_Ctype(algo_lib::Regx &regx_value, cstring &out) {
    ind_beg(amc::_db_ctype_curs, ctype, amc::_db) {
        amc::FNs& ns = *ctype.p_ns;
        if (Regx_Match(regx_value, ctype.ctype)) {
            if (!ns.c_nscpp) {
                ReportQueryNocpp(ns, "ctype", ctype.ctype);
            } else {
                amc::_db.report.n_ctype++;
                ch_RemoveAll(*ns.cpp);
                ch_RemoveAll(*ns.hdr);
                ch_RemoveAll(*ns.inl);
                Main_GenEnum(ns, ctype);
                if (!ctype.c_cextern) {
                    GenStruct(ns, ctype);
                }
                if (ch_N(*ns.hdr)) {
                    out << *ns.hdr;
                }
                if (ch_N(*ns.inl)) {
                    out << *ns.inl;
                }
                if (ch_N(*ns.cpp)) {
                    out << *ns.cpp;
                }
            }
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

static void Query_Func(algo_lib::Regx &regx, cstring &out) {
    ind_beg(amc::_db_func_curs, func, amc::_db) {
        if (ch_N(func.proto) > 0 && !func.disable) {
            amc::FNs& ns = *func.p_ns;
            if (Regx_Match(regx, func.func) || (func.extrn && Regx_Match(regx, amc::GetCppname(func)))) {
                if (!ns.c_nscpp) {
                    ReportQueryNocpp(ns, "func", func.func);
                } else {
                    ch_RemoveAll(*ns.cpp);
                    ch_RemoveAll(*ns.hdr);
                    ch_RemoveAll(*ns.inl);
                    if (amc::_db.cmdline.proto || func.extrn || func.deleted) {
                        amc::_db.report.n_func++;
                        tempstr proto;
                        PrintFuncProto(func, NULL, proto,true);
                        algo::InsertIndent(*ns.hdr, proto, 0);
                    } else {
                        amc::_db.report.n_func++;
                        PrintFuncBody(ns, func);
                    }
                    if (ch_N(*ns.hdr)) {
                        out << *ns.hdr;
                    }
                    if (ch_N(*ns.inl)) {
                        out << *ns.inl;
                    }
                    if (ch_N(*ns.cpp)) {
                        out << *ns.cpp;
                    }
                }
            }
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

void amc::Main_Querymode() {
    strptr key = Pathcomp(amc::_db.cmdline.query, ":RL");
    tempstr value(algo::Pathcomp(amc::_db.cmdline.query, ":RR"));
    if (key == "") {
        key = "%";
    }
    if (value == "") {
        value = "%";
    } else if (FindStr(value, ".")==-1) { // <ns> -> <ns>.%
        value << ".%";
    }
    verblog("amc.query"
            <<Keyval("key",key)
            <<Keyval("value",value));
    algo_lib::Regx regx_key;
    Regx_ReadSql(regx_key, key, true);
    algo_lib::Regx regx_value;
    Regx_ReadSql(regx_value, value, true);
    algo_lib::Replscope R;
    tempstr out;

    if (Regx_Match(regx_key, "ctype")) {
        Query_Ctype(regx_value,out);
    }
    if (Regx_Match(regx_key, "func")) {
        Query_Func(regx_value,out);
    }
    amc::_db.report.n_cppline += amc::CountLines(out);
    prlog(out);
}

// -----------------------------------------------------------------------------

// True if amc was invoked in query-only mode
bool amc::QueryModeQ() {
    return  ch_N(amc::_db.cmdline.query) > 0;
}

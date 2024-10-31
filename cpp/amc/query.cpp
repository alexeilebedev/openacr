// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
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
// Contacting ICE: <https://www.theice.com/contact>
// Target: amc (exe) -- Algo Model Compiler: generate code under include/gen and cpp/gen
// Exceptions: NO
// Source: cpp/amc/query.cpp -- Query mode
//

#include "include/amc.h"

// -----------------------------------------------------------------------------

static void Query_Ctype(algo_lib::Regx &regx_value, cstring &out) {
    ind_beg(amc::_db_ctype_curs, ctype, amc::_db) {
        amc::FNs& ns = *ctype.p_ns;
        if (Regx_Match(regx_value, ctype.ctype)) {
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
    }ind_end;
}

// -----------------------------------------------------------------------------

static void Query_Func(algo_lib::Regx &regx, cstring &out) {
    ind_beg(amc::_db_func_curs, func, amc::_db) {
        if (ch_N(func.proto) > 0 && !func.disable) {
            amc::FNs& ns = *func.p_ns;
            if (Regx_Match(regx, func.func)) {
                ch_RemoveAll(*ns.cpp);
                ch_RemoveAll(*ns.hdr);
                ch_RemoveAll(*ns.inl);
                if (amc::_db.cmdline.proto || func.extrn || func.deleted) {
                    amc::_db.report.n_func++;
                    tempstr proto;
                    PrintFuncProto(func, NULL, proto);
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
    }ind_end;
}

// -----------------------------------------------------------------------------

void amc::Main_Querymode() {
    tempstr value(algo::Pathcomp(amc::_db.cmdline.query, ":RR"));
    if (value == "") {
        value = "%";
    } else if (FindStr(value, ".")==-1) {
        value << ".%";
    }
    strptr key = Pathcomp(amc::_db.cmdline.query, ":RL");
    if (key == "") {
        key = "%";
    }
    algo_lib::Regx regx_key;
    Regx_ReadSql(regx_key, key, true);
    algo_lib::Regx regx_value;
    Regx_ReadSql(regx_value, value, true);
    algo_lib::Replscope R;
    tempstr out;

    if (Regx_Match(regx_key, "ctype")) {
        verblog("query ctype "<<regx_value.expr);
        Query_Ctype(regx_value,out);
    }
    if (Regx_Match(regx_key, "func")) {
        verblog("query func "<<regx_value.expr);
        Query_Func(regx_value,out);
    }
    frep_(i,ch_N(out)) {
        amc::_db.report.n_cppline += ch_qFind(out, i) == '\n';
    }
    prlog(out);
}

// -----------------------------------------------------------------------------

// True if amc was invoked in query-only mode
bool amc::QueryModeQ() {
    return  ch_N(amc::_db.cmdline.query) > 0;
}

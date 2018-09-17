// (C) 2018-2019 NYSE | Intercontinental Exchange
//
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
//
// Target: amc (exe) -- Algo Model Compiler: generate code under include/gen and cpp/gen
// Exceptions: NO
// Source: cpp/amc/query.cpp
//
// Created By: alexei.lebedev
// Recent Changes: alexei.lebedev
//

#include "include/amc.h"

// -----------------------------------------------------------------------------

void amc::Main_Querymode() {
    algo_lib::Replscope R;
    tempstr out;

    ind_beg(amc::_db_ctype_curs, ctype, amc::_db) {
        amc::FNs& ns = *ctype.p_ns;
        if (Regx_Match(amc::_db.cmdline.query, ctype.ctype)) {
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

    // query
    ind_beg(amc::_db_func_curs, func, amc::_db) if (ch_N(func.proto) > 0 && !func.disable) {
        amc::FNs& ns = *func.p_ns;
        if (Regx_Match(amc::_db.cmdline.query, func.func)) {
            ch_RemoveAll(*ns.cpp);
            ch_RemoveAll(*ns.hdr);
            ch_RemoveAll(*ns.inl);
            if (amc::_db.cmdline.proto) {
                if (!func.priv && !func.globns && !func.oper) {
                    amc::_db.report.n_func++;
                    GenFuncProto(ns, func, false, false);
                }
            } else {
                if (!func.extrn && !func.globns) {
                    amc::_db.report.n_func++;
                    GenFuncBody(ns, func);
                }
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

    frep_(i,ch_N(out)) {
        amc::_db.report.n_cppline += ch_qFind(out, i) == '\n';
    }
    prlog(out);
}

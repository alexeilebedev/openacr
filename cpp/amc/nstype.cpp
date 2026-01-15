// Copyright (C) 2023-2026 AlgoRND
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
// Source: cpp/amc/nstype.cpp -- Namespace types
//

#include "include/amc.h"
#include "include/gen/dmmeta_gen.h"
#include "include/gen/dmmeta_gen.inl.h"

// -----------------------------------------------------------------------------

void amc::gen_ssimdb() {
    ind_beg(amc::_db_ctype_curs,ctype,amc::_db) {
        if (ctype.p_ns->nstype == dmmeta_Nstype_nstype_ssimdb && ctype.original) {
            if (!ctype.c_ssimfile && !ctype.c_nossimfile) {
                prlog("amc.need_ssimfile"
                      <<Keyval("ctype",ctype.ctype)
                      <<Keyval("comment","ctype belonging to a 'nsdb' namespace must have a corresponding ssimfile"));
                algo_lib::_db.exit_code++;
            }
        }
    }ind_end;
}

// -----------------------------------------------------------------------------

void amc::gen_ns_check_nstype() {
    amc::FNs &ns=*amc::_db.c_ns;
    if ((ns.nstype==dmmeta_Nstype_nstype_exe || ns.nstype==dmmeta_Nstype_nstype_lib) && !ns.c_nsx) {
        amccheck(0,"amc.nsx_missing"
                 <<Keyval("ns",ns.ns)
                 <<Keyval("comment","nstype requires nsx record"));
    }
    if (ns.nstype==dmmeta_Nstype_nstype_protocol && !ns.c_nsproto) {
        amccheck(0,"amc.nsproto_missing"
                 <<Keyval("ns",ns.ns)
                 <<Keyval("comment","nstype:protocol requires nsproto record"));
    }
    if (ns.nstype==dmmeta_Nstype_nstype_ssimdb && !ns.c_nsdb) {
        amccheck(0,"amc.nsdb_missing"
                 <<Keyval("ns",ns.ns)
                 <<Keyval("comment","nstype:ssimdb requires nsdb record"));
    }
}

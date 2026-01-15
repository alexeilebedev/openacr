// Copyright (C) 2023-2026 AlgoRND
// Copyright (C) 2017-2019 NYSE | Intercontinental Exchange
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
// Source: cpp/amc/disp/main.cpp -- Dispatch main
//

#include "include/amc.h"

// -----------------------------------------------------------------------------

void amc::gen_ns_dispatch() {
    amc::FNs &ns =*amc::_db.c_ns;
    ind_beg(amc::ns_c_dispatch_curs, disp,ns) {
        if (disp.call) {
            Disp_Call(disp);
        }
    }ind_end;
    ind_beg(amc::ns_c_dispatch_curs, disp,ns) {
        if (disp.p_ctype_hdr && disp.print) {
            Disp_Print(disp);
        }
    }ind_end;
    ind_beg(amc::ns_c_dispatch_curs, disp,ns) {
        if (disp.read) {
            Disp_Read(disp);
        }
    }ind_end;
    ind_beg(amc::ns_c_dispatch_curs, disp,ns) {
        if (disp.dyn) {
            Disp_Delete(disp);
        }
    }ind_end;
    ind_beg(amc::ns_c_dispatch_curs, disp,ns) {
        if (disp.kafka) {
            Disp_KafkaEncode(disp);
        }
    }ind_end;
    ind_beg(amc::ns_c_dispatch_curs, disp,ns) {
        if (disp.kafka) {
            Disp_KafkaDecode(disp);
        }
    }ind_end;
    Filter_Gen(ns);
}

// -----------------------------------------------------------------------------

// Create new fields for dispatch filters.
void amc::Disp_NewField() {
    Filter_NewField();
}

// -----------------------------------------------------------------------------

void amc::gen_dispenum() {
    Disp_CreateFromMsg();
    // If there is no common header, create a case type that enumerates
    // members of the dispatch
    ind_beg(amc::_db_dispatch_curs, dispatch, amc::_db) {
        Disp_CreateCasetype(dispatch);
    }ind_end;
}

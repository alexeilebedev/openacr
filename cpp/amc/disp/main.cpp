// Copyright (C) 2025-2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2017-2019 NYSE | Intercontinental Exchange
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
        if (disp.p_ctype_hdr && disp.print && disp.msgs && !disp.dyn && disp.p_ctype_hdr->c_lenfld) {
            Disp_PrintFmt(disp);
            Disp_HeartbeatQ(disp);
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

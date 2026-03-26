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
// Target: acr_nav (exe) -- TUI schema explorer for browsing ctypes, fields, and cross-references
// Exceptions: yes
// Source: cpp/acr_nav/acr_nav.cpp
//

#include "include/algo.h"
#include "include/acr_nav.h"

void acr_nav::Main() {
    // Phase 1: populate selection list -- match namespace against cmdline regex
    ind_beg(acr_nav::_db_ctype_curs, ctype, acr_nav::_db) {
        bool match = Regx_Match(acr_nav::_db.cmdline.ns, ns_Get(ctype));
        if (match) {
            zd_sel_ctype_Insert(ctype);
        }
    } ind_end;

    // Phase 2: print each selected ctype with its fields
    // Upptr dereferences (p_arg, p_reftype) are safe -- field_XrefMaybe rejects
    // any field where the lookup fails, so all surviving FField records have
    // non-NULL Upptr pointers
    ind_beg(acr_nav::_db_zd_sel_ctype_curs, ctype, acr_nav::_db) {
        prlog(ctype.ctype << "  (" << c_field_N(ctype) << " fields)");
        ind_beg(acr_nav::ctype_c_field_curs, field, ctype) {
            tempstr out;
            out << "  " << name_Get(field);
            char_PrintNTimes(' ', out, i32_Max(1, 24 - ch_N(out)));
            out << field.p_arg->ctype;
            char_PrintNTimes(' ', out, i32_Max(1, 52 - ch_N(out)));
            out << field.p_reftype->reftype;
            prlog(out);
        } ind_end;
    } ind_end;

    // Phase 3: summary proving all finput chains loaded
    prlog("acr_nav.report"
          << Keyval("n_ctype", ctype_N())
          << Keyval("n_field", field_N())
          << Keyval("n_sel_ctype", zd_sel_ctype_N())
          << Keyval("n_keybind", keybind_N())
          << Keyval("n_panel", panel_N())
          << Keyval("n_navaction", navaction_N())
          << Keyval("n_navmode", navmode_N()));
}

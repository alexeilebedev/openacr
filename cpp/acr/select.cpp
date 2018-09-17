// (C) AlgoEngineering LLC 2008-2013
// (C) 2017-2019 NYSE | Intercontinental Exchange
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
// Target: acr (exe) -- Algo Cross-Reference - ssimfile database & update tool
// Exceptions: NO
// Source: cpp/acr/select.cpp -- Selection
//
// Created By: alexei.lebedev
// Recent Changes: alexei.lebedev
//

#include "include/acr.h"

// -----------------------------------------------------------------------------

void acr::Rec_Deselect(acr::FRec& rec) {
    acr::zd_selrec_Remove(*rec.p_ctype, rec);
    acr::zd_all_selrec_Remove(rec);
}

// -----------------------------------------------------------------------------

// De-select all records
void acr::Rec_DeselectAll() {
    acr::zd_all_selrec_RemoveAll();
    while(acr::FCtype *ctype=acr::zd_sel_ctype_First()) {
        acr::zd_selrec_RemoveAll(*ctype);
        acr::zd_sel_ctype_RemoveFirst();
    }
}

// -----------------------------------------------------------------------------

// Select all records from all files
void acr::Rec_SelectAll() {
    Rec_DeselectAll();
    ind_beg(acr::_db_file_curs, file, acr::_db) {
        ind_beg(acr::file_zd_frec_curs, rec, file) {
            Rec_Select(rec);
        }ind_end;
    }ind_end;
}

// -----------------------------------------------------------------------------

// Conditionally insert record into selection set
// - Record is added to zd_selrec list for is ctype
// - Record is added to zd_all_selrec (global list)
// - Selected ctype is added to zd_sel_ctype list
bool acr::Rec_Select(acr::FRec& rec) {
    bool add = !acr::zd_all_selrec_InLlistQ(rec);
    if (add) {
        rec.seldist = 0;
        acr::zd_all_selrec_Insert(rec);
        acr::zd_selrec_Insert(*rec.p_ctype,rec);
        acr::zd_sel_ctype_Insert(*rec.p_ctype);
    }
    return add;
}

// -----------------------------------------------------------------------------

// Deselect all records.
// Select only records in the error set
void acr::SelectErrRecs() {
    acr::Rec_DeselectAll();
    ind_beg(acr::_db_zd_all_err_curs, err,acr::_db) {
        if (err.rec) {
            Rec_Select(*err.rec);
        }
    }ind_end;
}

// Copyright (C) 2008-2013 AlgoEngineering LLC
// Copyright (C) 2017-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2023-2024 AlgoRND
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
// Target: acr (exe) -- Algo Cross-Reference - ssimfile database & update tool
// Exceptions: NO
// Source: cpp/acr/err.cpp -- Show errors / suggestions
//

#include "include/acr.h"

// -----------------------------------------------------------------------------

void acr::NoteErr(acr::FCtype* ctype, acr::FRec* rec, acr::FField *fld, strptr text) {
    acr::FErr& err  = acr::err_Alloc();
    err.id    = acr::_db.err_seq++;
    err.ctype = ctype;
    err.rec   = rec;
    err.fld   = fld;
    err.text  = text;
    (void)acr::err_XrefMaybe(err);
}

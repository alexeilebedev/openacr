// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2017-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2008-2013 AlgoEngineering LLC
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

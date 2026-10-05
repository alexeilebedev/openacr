// Copyright (C) 2026 AlgoX2 Corp
// Copyright (C) 2023 Astra
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
// Target: lib_sqlite (lib) -- SQLite binding: connections and a virtual table over ssimfiles
// Exceptions: yes
// Header: include/lib_sqlite.h
//

#pragma once
#include "include/gen/lib_sqlite_gen.h"
#include "include/gen/lib_sqlite_gen.inl.h"

namespace lib_sqlite {
    extern sqlite3_module SsimModule;
}

namespace lib_sqlite { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/lib_sqlite/lib_sqlite.cpp
    //
    int Open(lib_sqlite::FConn& conn);
    //     (user-implemented function, prototype is in amc-generated header)
    // void db_Cleanup(lib_sqlite::FConn &parent); // dmmeta.ffunc:lib_sqlite.FConn.db.Cleanup
    void Init();

    // -------------------------------------------------------------------
    // cpp/lib_sqlite/vtab.cpp
    //

    // Scalar function to initialize virtual tables with given data path
    void VtabInitFunc(sqlite3_context *context, int argc, sqlite3_value **argv);

    // Entrypoint for vtab extension
    int VtabInitExt(sqlite3 *db, char **pzErrMsg, const sqlite3_api_routines *pApi);
}

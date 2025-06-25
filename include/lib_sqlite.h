// Copyright (C) 2023 Astra
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
// Target: lib_sqlite (lib)
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
    void db_Cleanup(lib_sqlite::FConn &parent);
    void Init();

    // -------------------------------------------------------------------
    // cpp/lib_sqlite/vtab.cpp
    //

    // Scalar function to initialize virtual tables with given data path
    void VtabInitFunc(sqlite3_context *context, int argc, sqlite3_value **argv);

    // Entrypoint for vtab extension
    int VtabInitExt(sqlite3 *db, char **pzErrMsg, const sqlite3_api_routines *pApi);
}

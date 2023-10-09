// Copyright (C) 2008-2013 AlgoEngineering LLC
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2023 Astra
// Copyright (C) 2023 AlgoRND
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
// Target: lib_mysql (lib) -- Mysql adaptor
// Exceptions: NO
// Header: include/lib/lib_mysql.h
//

#pragma once
#include "include/algo.h"
#include "include/gen/lib_mysql_gen.h"
#include "include/gen/lib_mysql_gen.inl.h"
#include <mariadb/mysql.h>
//typedef struct st_mysql MYSQL;
//typedef struct st_mysql_res MYSQL_RES;

namespace lib_mysql { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/lib/lib_mysql.cpp
    //

    // LHS      output buffer
    // CONN     mysql context (includes collation info)
    // RHS      string to print
    // QUOTES   quotes to use (' or "); if set to zero, quotes are omitted.
    void MPrintQuoted(cstring &lhs, MYSQL *conn, strptr rhs, char quotes);

    // replace first occurence of ? in LHS with Y
    // QUOTES   quotes to use (' or "); if set to zero, quotes are omitted.
    void MBind(MYSQL *conn, cstring &lhs, strptr y, char quotes);

    // Execute query and access result
    // Execute query and access result
    // Previous result, if present, is discarded.
    // If this is not done, you get
    // "Commands out of sync; you can't run this command now"
    // error
    void MQuery(MYSQL *conn, strptr query, lib_mysql::Res &res);
    void mysql_Cleanup();
    void res_Cleanup(lib_mysql::Res &res);
}

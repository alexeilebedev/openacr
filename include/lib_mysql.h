// Copyright (C) 2026 AlgoX2 Corp
// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2023 Astra
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
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
// Target: lib_mysql (lib) -- Mysql adaptor
// Exceptions: NO
// Header: include/lib_mysql.h
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

    // Execute query and access result.
    // Previous result, if present, is discarded.
    // If this is not done, you get
    // "Commands out of sync; you can't run this command now"
    // error
    void MQuery(MYSQL *conn, strptr query, lib_mysql::Res &res);
    //     (user-implemented function, prototype is in amc-generated header)
    // void mysql_Cleanup(); // dmmeta.ffunc:lib_mysql.FDb.mysql.Cleanup
    // void res_Cleanup(lib_mysql::Res &res); // dmmeta.ffunc:lib_mysql.Res.res.Cleanup
}

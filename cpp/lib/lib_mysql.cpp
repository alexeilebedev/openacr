// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2021 Astra
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
// Source: cpp/lib/lib_mysql.cpp
//
// Helper functions for mysql

#include "include/lib_mysql.h"
#include "include/lib_iconv.h"

// LHS      output buffer
// CONN     mysql context (includes collation info)
// RHS      string to print
// QUOTES   quotes to use (' or "); if set to zero, quotes are omitted.
void lib_mysql::MPrintQuoted(cstring &lhs, MYSQL *conn, strptr rhs, char quotes) {
    strptr q;
    if (quotes) {
        q = strptr(&quotes,1);
    }
    lhs << q;
    ch_Reserve(lhs, elems_N(rhs)*2+1);
    u32 ret = mysql_real_escape_string(conn, lhs.ch_elems + lhs.ch_n, rhs.elems, elems_N(rhs));
    vrfy(ret < lhs.ch_max-ch_N(lhs), "buffer overflow");
    lhs.ch_n = ch_N(lhs) + ret;
    lhs << q;
}

// replace first occurence of ? in LHS with Y
// QUOTES   quotes to use (' or "); if set to zero, quotes are omitted.
void lib_mysql::MBind(MYSQL *conn, cstring &lhs, strptr y, char quotes) {
    algo::i32_Range R = ch_FindFirst(lhs,'?');
    vrfy(R.end > R.beg,tempstr()<< "No unbound arguments left in ["<<lhs<<"]");
    tempstr tmp;
    tmp << ch_FirstN(lhs,R.beg);
    MPrintQuoted(tmp, conn, y, quotes);
    tmp << ch_RestFrom(lhs,R.end);
    lhs = tmp;
}

// Execute query and access result.
// Previous result, if present, is discarded.
// If this is not done, you get
//   "Commands out of sync; you can't run this command now"
// error
void lib_mysql::MQuery(MYSQL *conn, strptr query, lib_mysql::Res &res) {
    verblog(query);
    res_Cleanup(res);
    vrfy(mysql_query(conn, Zeroterm(tempstr() << query))==0, mysql_error(conn));
    res.res = mysql_use_result(conn);
}

void lib_mysql::mysql_Cleanup() {
    if (lib_mysql::_db.mysql) {
        mysql_close(lib_mysql::_db.mysql);
        lib_mysql::_db.mysql = NULL;
    }
}

void lib_mysql::res_Cleanup(lib_mysql::Res &res) {
    if (res.res) {
        mysql_free_result(res.res);
        res.res = NULL;
    }
}

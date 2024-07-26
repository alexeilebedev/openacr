// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2020-2021 Astra
// Copyright (C) 2013-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2008-2013 AlgoEngineering LLC
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
// Source: cpp/lib/lib_mysql.cpp
//
// Helper functions for mysql

#include "include/lib_mysql.h"

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

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
// Target: aqlite (exe) -- Runs sqlite queries against ssim files
// Exceptions: yes
// Source: cpp/aqlite/aqlite.cpp
//

#include "include/algo.h"
#include "include/lib_ctype.h"
#include "include/lib_sqlite.h"
#include "include/aqlite.h"

int aqlite::exec_cb(void*, int na, char** av, char** cols) {
    verblog(__PRETTY_FUNCTION__);
    algo::Tuple row;
    for(auto i=0; i<na; i++) {
        auto& attr = attrs_Alloc(row);
        attr.name = cols[i];
        attr.value = av[i];
    }
    auto change = attrs_N(row) != attrs_N(_db.last_cols);
    if (!change) {
        ind_beg(algo::Tuple_attrs_curs, attr, row) {
            auto& last = attrs_qFind(_db.last_cols, ind_curs(attr).index);
            if (attr.name != last.name) {
                change = true;
                break;
            }
        }ind_end;
    }
    if (change) {
        _db.stmt++;
    }
    row.head.value << "stmt" << _db.stmt;
    prlog(row);
    if (change) {
        algo::TSwap(row, _db.last_cols);
    }
    return SQLITE_OK;
}

void aqlite::Main() {
    auto rc = sqlite3_auto_extension((void (*)())lib_sqlite::VtabInitExt);
    auto& conn = lib_sqlite::ind_conn_GetOrCreate(":memory:");
    if (rc == SQLITE_OK) {
        rc = Open(conn);
    }
    if (rc == SQLITE_OK) {
        auto init_cmd = tempstr() << "SELECT init_ssim('" << _db.cmdline.in << "')";
        rc = sqlite3_exec(conn.db, algo::Zeroterm(init_cmd), nullptr, nullptr, nullptr);
    }
    if (rc == SQLITE_OK) {
        verblog(_db.cmdline.cmd);
        rc = sqlite3_exec(conn.db, algo::Zeroterm(_db.cmdline.cmd), exec_cb, &conn, nullptr);
    }
    if (rc != SQLITE_OK) {
        prerr(sqlite3_errmsg(conn.db));
    }
    algo_lib::_db.exit_code = rc;
}

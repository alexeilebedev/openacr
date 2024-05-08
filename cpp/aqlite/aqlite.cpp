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
    cstring out;
    for(auto i=0; i<na; i++) {
        out << Keyval(cols[i], av[i] ? av[i] : "NULL");
    }
    prlog(out);
    return SQLITE_OK;
}

void aqlite::Main() {
    lib_sqlite::Init();
    auto& conn = lib_sqlite::ind_conn_GetOrCreate(":memory:");
    auto rc = sqlite3_open(algo::Zeroterm(conn.name), &conn.db);
    if (rc == SQLITE_OK) {
        rc = sqlite3_create_module(conn.db, "ssimdb", &lib_sqlite::SsimModule, 0);
    }
    if (rc == SQLITE_OK) {
        auto create = cstring();
        algo_lib::Replscope R;
        ind_beg(_db_ns_curs,ns,_db) if (ns.nstype == dmmeta_Nstype_nstype_ssimdb) {
            Set(R,"$ns",ns.ns);
            Ins(&R,create, "attach ':memory:' as $ns;");
        }ind_end;
        ind_beg(lib_ctype::_db_ssimfile_curs, ssimfile, lib_ctype::_db) {
            Set(R,"$data",_db.cmdline.data);
            Set(R,"$ssimfile",ssimfile.ssimfile);
            Ins(&R,create, "create virtual table $ssimfile using ssimdb( $data , $ssimfile );");
        }ind_end;
        verblog(create);
        rc = sqlite3_exec(conn.db, algo::Zeroterm(create), nullptr, nullptr, nullptr);
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

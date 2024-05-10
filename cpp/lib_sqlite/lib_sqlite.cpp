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
// Source: cpp/lib_sqlite/lib_sqlite.cpp
//

#include "include/algo.h"
#include "include/lib_ctype.h"
#include "include/lib_sqlite.h"

void lib_sqlite::Open(lib_sqlite::FConn& conn) {
    vrfyeq(sqlite3_open(algo::Zeroterm(conn.name), &conn.db), 0, sqlite3_errmsg(conn.db));
}

void lib_sqlite::db_Cleanup(lib_sqlite::FConn &parent) {
    sqlite3_close(parent.db);
}

void lib_sqlite::Init() {
    lib_ctype::Init();
    auto rc = sqlite3_config(SQLITE_CONFIG_SINGLETHREAD);
    vrfyeq_(rc, SQLITE_OK);
}

// Copyright (C) 2026 AlgoRND
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
#include "include/lib_sqlite.h"

int lib_sqlite::Open(lib_sqlite::FConn& conn) {
    return sqlite3_open(algo::Zeroterm(conn.name), &conn.db);
}

void lib_sqlite::db_Cleanup(lib_sqlite::FConn &parent) {
    sqlite3_close(parent.db);
}

// Rewrite field with reftype Pkey by replacing Pkey with Val,
// and arg,p_arg with new type
// This happens recursively as long if the target field is also a Pkey
// The default for the resulting field is then taken to be the default of the target type.
static void RewritePkey() {
    ind_beg(lib_sqlite::_db_field_curs,field,lib_sqlite::_db) {
        if (field.reftype == dmmeta_Reftype_reftype_Pkey) {
            lib_sqlite::FField *parentfield = &field;
            lib_sqlite::FCtype *arg = parentfield->p_arg;
            while (parentfield->reftype == dmmeta_Reftype_reftype_Pkey && c_field_N(*arg) > 0) {
                parentfield = c_field_Find(*arg, 0);
                arg = parentfield->p_arg;
            }
            field.p_arg   = arg;
            field.arg     = arg->ctype;
            field.reftype = dmmeta_Reftype_reftype_Val;
        }
    }ind_end;
}

void lib_sqlite::Init() {
    RewritePkey();
    // create an empty idx of idxNum=0
    auto& idx = bestidx_Alloc();
    (void)idx;
    ind_beg(_db_ctype_curs,ctype,_db) {
        ind_beg(ctype_c_field_curs,field,ctype){
            field.id = ind_curs(field).index;
        }ind_end;
    }ind_end;
}

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

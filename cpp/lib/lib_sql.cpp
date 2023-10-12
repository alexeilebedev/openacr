// Copyright (C) 2017-2019 NYSE | Intercontinental Exchange
// Copyright (C) 2020-2021 Astra
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
// Target: lib_sql (lib) -- SQL formatting functions
// Exceptions: yes
// Source: cpp/lib/lib_sql.cpp
//

#include "include/algo.h"
#include "include/lib_sql.h"

// -----------------------------------------------------------------------------

// SQL-quote string S
// and return result
const tempstr lib_sql::SqlQuoted(strptr s) {
    tempstr ret;
    if (s.n_elems == 0) {
        ret << "''";
    } else {
        strptr_PrintSql(s,ret);
    }
    return ret;
}

// -----------------------------------------------------------------------------

// Return contents of ATTR as a SQL expression in the context of tuple TUPLE.
// The string is properly quoted (i.e. SqlQuoted is not required)
// Translation is controlled with DeclareBool etc.
const tempstr lib_sql::SqlQuotedValue(Tuple &tuple, Attr &attr) {
    tempstr ret;
    if (lib_sql::ind_attr_N()) {
        tempstr key(tempstr() << tuple.head.value << "." << attr.name);
        if (lib_sql::FAttr *fattr = lib_sql::ind_attr_Find(key)) {
            if (fattr->isbool) {
                bool val=false;
                (void)bool_ReadStrptrMaybe(val,attr.value);
                ret = val ? "1" : "0";
            } else {
                // other types here
            }
        }
    }
    // empty result = treat as string
    return ch_N(ret) ? ret : SqlQuoted(attr.value);
}

// -----------------------------------------------------------------------------

// Return a comma-separate SQL name-list
const tempstr lib_sql::SqlNames(Tuple &tuple) {
    tempstr ret;
    algo::ListSep ls(",");
    ind_beg(Tuple_attrs_curs,attr,tuple) {
        ret << ls << attr.name;
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

// Return a comma-separate SQL name=value list
const tempstr lib_sql::SqlNameValues(Tuple &tuple) {
    tempstr ret;
    algo::ListSep ls(",");
    ind_beg(Tuple_attrs_curs,attr,tuple) {
        ret << ls << attr.name << "=" << SqlQuotedValue(tuple,attr);
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

// Return a comma-separated SQL value list
tempstr lib_sql::SqlValues(Tuple &tuple) {
    tempstr ret;
    algo::ListSep ls(",");
    ind_beg(Tuple_attrs_curs,attr,tuple) {
        ret << ls << SqlQuotedValue(tuple,attr);
    }ind_end;
    return ret;
}

// -----------------------------------------------------------------------------

// Generate SQL code to insert, delete, or update record STRTUPLE into table TABLENAME,
// Using TSQL 'upsert' trick.
void lib_sql::UpsertOrDelete(strptr strtuple, strptr tablename, cstring &query, bool del) {
    Tuple tuple;
    (void)Tuple_ReadStrptrMaybe(tuple,strtuple);
    Attr *pkey = attrs_Find(tuple,0);
    if (pkey) {
        if (del) {
            query << "DELETE FROM "<<tablename
                  <<" WHERE "<<pkey->name<<"="<<SqlQuotedValue(tuple,*pkey)
                  <<";\n";
        } else {
            query << "UPDATE " << tablename
                  << " SET " << lib_sql::SqlNameValues(tuple)
                  << " WHERE "<<pkey->name<<"="<<SqlQuotedValue(tuple,*pkey)
                  << ";\n"
                  << "IF @@rowcount = 0\n"
                  << "BEGIN\n"
                  << "  INSERT INTO " << tablename << "(" << lib_sql::SqlNames(tuple)<< ") "
                  << "  VALUES (" << lib_sql::SqlValues(tuple) << ")\n"
                  << "END\n\n";
        }
    }
}

// -----------------------------------------------------------------------------

// Declare attribute NAME as boolean so it can be translated to SQL Server format
void lib_sql::DeclareBool(strptr name) {
    ind_attr_GetOrCreate(name).isbool = true;
}

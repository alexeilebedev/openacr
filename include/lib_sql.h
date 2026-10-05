// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2017-2019 NYSE | Intercontinental Exchange
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
// Target: lib_sql (lib) -- SQL formatting functions
// Exceptions: yes
// Header: include/lib_sql.h
//

#include "include/algo.h"
#include "include/gen/lib_sql_gen.h"
#include "include/gen/lib_sql_gen.inl.h"

namespace lib_sql { // update-hdr
    // Dear human:
    //     Text from here to the closing curly brace was produced by scanning
    //     source files. Editing this text is futile.
    //     To refresh the contents of this section, run 'update-hdr'.
    //     To convert this section to a hand-written section, remove the word 'update-hdr' from namespace line.

    // -------------------------------------------------------------------
    // cpp/lib/lib_sql.cpp
    //

    // SQL-quote string S
    // and return result
    const tempstr SqlQuoted(strptr s);

    // Return contents of ATTR as a SQL expression in the context of tuple TUPLE.
    // The string is properly quoted (i.e. SqlQuoted is not required)
    // Translation is controlled with DeclareBool etc.
    const tempstr SqlQuotedValue(Tuple &tuple, Attr &attr);

    // Return a comma-separate SQL name-list
    const tempstr SqlNames(Tuple &tuple);

    // Return a comma-separate SQL name=value list
    const tempstr SqlNameValues(Tuple &tuple);

    // Return a comma-separated SQL value list
    tempstr SqlValues(Tuple &tuple);

    // Generate SQL code to insert, delete, or update record STRTUPLE into table TABLENAME,
    // Using TSQL 'upsert' trick.
    void UpsertOrDelete(strptr strtuple, strptr tablename, cstring &query, bool del);

    // Declare attribute NAME as boolean so it can be translated to SQL Server format
    void DeclareBool(strptr name);
}

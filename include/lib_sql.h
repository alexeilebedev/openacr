// (C) 2017-2019 NYSE | Intercontinental Exchange
//
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
//
// Target: lib_sql (lib) -- SQL formatting functions
// Exceptions: yes
// Header: include/lib_sql.h
//

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

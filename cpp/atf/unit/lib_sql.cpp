// (C) 2018-2019 NYSE | Intercontinental Exchange
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
// Target: atf_unit (exe) -- Algo Test Framework: unit tests
// Exceptions: yes
// Source: cpp/atf/unit/lib_sql.cpp
//
// Created By: alexei.lebedev ara.aslyan
// Recent Changes: alexei.lebedev ara.aslyan
//

#include "include/atf_unit.h"
#include "include/lib_sql.h"

// --------------------------------------------------------------------------------

void atf_unit::unittest_lib_sql_Main() {
    TESTCMP(lib_sql::SqlQuoted(""), "''");
    TESTCMP(lib_sql::SqlQuoted("abcd"), "'abcd'");
    TESTCMP(lib_sql::SqlQuoted("abcd'"), "'abcd'''");
    TESTCMP(lib_sql::SqlQuoted("abcd\n"), "'abcd\\n'");
    TESTCMP(lib_sql::SqlQuoted("abcd\r"), "'abcd\\r'");
    TESTCMP(lib_sql::SqlQuoted("abcd\t"), "'abcd\\t'");
    TESTCMP(lib_sql::SqlQuoted("abcd\""), "'abcd\"'");

    algo::cstring out;
    algo::strptr_PrintSql("",out,'"');
    TESTCMP(out, "\"\"");
    out = "";
    algo::strptr_PrintSql("abcd",out,'"');
    TESTCMP(out, "\"abcd\"");
    out = "";
    algo::strptr_PrintSql("abcd'",out,'"');
    TESTCMP(out, "\"abcd'\"");
    out = "";
    algo::strptr_PrintSql("abcd\n",out,'"');
    TESTCMP(out, "\"abcd\\n\"");
    out = "";
    algo::strptr_PrintSql("abcd\r",out,'"');
    TESTCMP(out, "\"abcd\\r\"");
    out = "";
    algo::strptr_PrintSql("abcd\t",out,'"');
    TESTCMP(out, "\"abcd\\t\"");
    out = "";
    algo::strptr_PrintSql("abcd\"",out,'"');
    TESTCMP(out, "\"abcd\"\"\"");

    algo::cstring tstr("head key:value key2:value2  key3:''  key4:Y  key5:N");
    Tuple tuple;
    (void)Tuple_ReadStrptrMaybe(tuple, tstr);

    TESTCMP(lib_sql::SqlNames(tuple), "key,key2,key3,key4,key5");
    TESTCMP(lib_sql::SqlValues(tuple), "'value','value2','','Y','N'");
    TESTCMP(lib_sql::SqlNameValues(tuple), "key='value',key2='value2',key3='',key4='Y',key5='N'");

    lib_sql::DeclareBool("head.key4");
    lib_sql::DeclareBool("head.key5");
    TESTCMP(lib_sql::SqlNameValues(tuple), "key='value',key2='value2',key3='',key4=1,key5=0");
    TESTCMP(lib_sql::SqlNameValues(tuple), "key='value',key2='value2',key3='',key4=1,key5=0");

    //avoiding coverity check
    Attr* key4 = attr_Find(tuple,"key4");
    Attr* key5 = attr_Find(tuple,"key5");

    if ( key4 && key5 ) {
        TESTCMP(lib_sql::SqlQuotedValue(tuple,*key4),"1");// no quotes!
        TESTCMP(lib_sql::SqlQuotedValue(tuple,*key5),"0");// no quotes!
    }

    cstring query;
    lib_sql::UpsertOrDelete(tstr, "blahblah", query, false);
    TESTCMP(query, "UPDATE blahblah SET key='value',key2='value2',key3='',key4=1,key5=0 WHERE key='value';\n"
            "IF @@rowcount = 0\nBEGIN\n  INSERT INTO blahblah(key,key2,key3,key4,key5)   VALUES ('value','value2','',1,0)\nEND\n\n");
    query="";
    lib_sql::UpsertOrDelete(tstr, "blahblah", query, true);
    TESTCMP(query, "DELETE FROM blahblah WHERE key='value';\n");
}

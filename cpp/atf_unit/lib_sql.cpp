// Copyright (C) 2023-2024 AlgoRND
// Copyright (C) 2018-2019 NYSE | Intercontinental Exchange
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
// Target: atf_unit (exe) -- Unit tests (see unittest table)
// Exceptions: yes
// Source: cpp/atf_unit/lib_sql.cpp
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

## lib_sql - SQL formatting functions


### Table Of Contents
<a href="#table-of-contents"></a>
* [Ctypes](#ctypes)
* [Functions](#functions)
* [Sources](#sources)
* [Inputs](#inputs)

### Ctypes
<a href="#ctypes"></a>
Other ctypes in this namespace which don't have own readme files

#### lib_sql.FAttr - 
<a href="#lib_sql-fattr"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|attr|algo.cstring|Val|
|isbool|bool|Val|

#### lib_sql.FDb - In-memory database for lib_sql
<a href="#lib_sql-fdb"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|_db|lib_sql.FDb|Global|
|attr|lib_sql.FAttr|Lary|
|ind_attr|lib_sql.FAttr|Thash|

### Functions
<a href="#functions"></a>
Functions exported from this namespace:

```c++
// SQL-quote string S
// and return result
const tempstr lib_sql::SqlQuoted(strptr s) 
```

```c++
// Return contents of ATTR as a SQL expression in the context of tuple TUPLE.
// The string is properly quoted (i.e. SqlQuoted is not required)
// Translation is controlled with DeclareBool etc.
const tempstr lib_sql::SqlQuotedValue(Tuple &tuple, Attr &attr) 
```

```c++
// Return a comma-separate SQL name-list
const tempstr lib_sql::SqlNames(Tuple &tuple) 
```

```c++
// Return a comma-separate SQL name=value list
const tempstr lib_sql::SqlNameValues(Tuple &tuple) 
```

```c++
// Return a comma-separated SQL value list
tempstr lib_sql::SqlValues(Tuple &tuple) 
```

```c++
// Generate SQL code to insert, delete, or update record STRTUPLE into table TABLENAME,
// Using TSQL 'upsert' trick.
void lib_sql::UpsertOrDelete(strptr strtuple, strptr tablename, cstring &query, bool del) 
```

```c++
// Declare attribute NAME as boolean so it can be translated to SQL Server format
void lib_sql::DeclareBool(strptr name) 
```

### Sources
<a href="#sources"></a>
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/gen/lib_sql_gen.cpp](/cpp/gen/lib_sql_gen.cpp)||
|[cpp/lib/lib_sql.cpp](/cpp/lib/lib_sql.cpp)||
|[include/gen/lib_sql_gen.h](/include/gen/lib_sql_gen.h)||
|[include/gen/lib_sql_gen.inl.h](/include/gen/lib_sql_gen.inl.h)||
|[include/lib_sql.h](/include/lib_sql.h)||

### Inputs
<a href="#inputs"></a>
`lib_sql` takes the following tables on input:
|ssimfile|comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|


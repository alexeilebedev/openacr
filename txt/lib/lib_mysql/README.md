## lib_mysql - Mysql adaptor


### Table Of Contents
<a href="#table-of-contents"></a>
* [Ctypes](#ctypes)
* [Functions](#functions)
* [Sources](#sources)

### Ctypes
<a href="#ctypes"></a>
Other ctypes in this namespace which don't have own readme files

#### lib_mysql.FDb - In-memory database for lib_mysql
<a href="#lib_mysql-fdb"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|res|lib_mysql.Res|Cppstack||Provides default name for variables of this type|
|_db|lib_mysql.FDb|Global|
|mysql|MYSQL|Ptr|

#### lib_mysql.Res - 
<a href="#lib_mysql-res"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|res|MYSQL_RES|Ptr|

### Functions
<a href="#functions"></a>
Functions exported from this namespace:

```c++
// LHS      output buffer
// CONN     mysql context (includes collation info)
// RHS      string to print
// QUOTES   quotes to use (' or "); if set to zero, quotes are omitted.
void lib_mysql::MPrintQuoted(cstring &lhs, MYSQL *conn, strptr rhs, char quotes) 
```

```c++
// replace first occurence of ? in LHS with Y
// QUOTES   quotes to use (' or "); if set to zero, quotes are omitted.
void lib_mysql::MBind(MYSQL *conn, cstring &lhs, strptr y, char quotes) 
```

```c++
// Execute query and access result.
// Previous result, if present, is discarded.
// If this is not done, you get
// "Commands out of sync; you can't run this command now"
// error
void lib_mysql::MQuery(MYSQL *conn, strptr query, lib_mysql::Res &res) 
```

```c++
void lib_mysql::mysql_Cleanup() 
```

```c++
void lib_mysql::res_Cleanup(lib_mysql::Res &res) 
```

### Sources
<a href="#sources"></a>
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/gen/lib_mysql_gen.cpp](/cpp/gen/lib_mysql_gen.cpp)||
|[cpp/lib/lib_mysql.cpp](/cpp/lib/lib_mysql.cpp)||
|[include/gen/lib_mysql_gen.h](/include/gen/lib_mysql_gen.h)||
|[include/gen/lib_mysql_gen.inl.h](/include/gen/lib_mysql_gen.inl.h)||
|[include/lib_mysql.h](/include/lib_mysql.h)||


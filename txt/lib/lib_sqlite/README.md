## lib_sqlite -


### Table Of Contents
<a href="#table-of-contents"></a>
* [Ctypes](#ctypes)
* [Functions](#functions)
* [Sources](#sources)
* [Inputs](#inputs)

### Ctypes
<a href="#ctypes"></a>
Other ctypes in this namespace which don't have own readme files

#### lib_sqlite.FConn - 
<a href="#lib_sqlite-fconn"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|name|algo.cstring|Val|
|db|sqlite3|Ptr|

#### lib_sqlite.FDb - 
<a href="#lib_sqlite-fdb"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|_db|lib_sqlite.FDb|Global|
|conn|lib_sqlite.FConn|Tpool|
|ind_conn|lib_sqlite.FConn|Thash|
|substr|lib_sqlite.FSubstr|Lary|

#### lib_sqlite.FSubstr - 
<a href="#lib_sqlite-fsubstr"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[dmmeta.Substr](/txt/ssimdb/dmmeta/substr.md)|Base|

#### lib_sqlite.Vtab - An instance of the virtual table
<a href="#lib_sqlite-vtab"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|sqlite3_vtab|Val|
|filename|algo.cstring|Val|
|c_ssimfile|lib_ctype.FSsimfile|Ptr|
|p_ctype|lib_ctype.FCtype|Ptr|

#### lib_sqlite.VtabCurs - A cursor for the virtual table
<a href="#lib_sqlite-vtabcurs"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|sqlite3_vtab_cursor|Val||Base class. Must be first|
|file|algo_lib.InTextFile|Val||current file|
|line|algo.strptr|Val||current line|
|i|i32|Val||current line number|
|eof|bool|Val|
|row|algo.Tuple|Val||Parsed row|

### Functions
<a href="#functions"></a>
Functions exported from this namespace:

```c++
void lib_sqlite::Open(lib_sqlite::FConn& conn) 
```

```c++
void lib_sqlite::db_Cleanup(lib_sqlite::FConn &parent) 
```

```c++
void lib_sqlite::Init() 
```

```c++
sqlite3_module lib_sqlite::SsimModule = 
```

### Sources
<a href="#sources"></a>
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/gen/lib_sqlite_gen.cpp](/cpp/gen/lib_sqlite_gen.cpp)||
|[cpp/lib_sqlite/lib_sqlite.cpp](/cpp/lib_sqlite/lib_sqlite.cpp)||
|[cpp/lib_sqlite/vtab.cpp](/cpp/lib_sqlite/vtab.cpp)||
|[include/gen/lib_sqlite_gen.h](/include/gen/lib_sqlite_gen.h)||
|[include/gen/lib_sqlite_gen.inl.h](/include/gen/lib_sqlite_gen.inl.h)||
|[include/lib_sqlite.h](/include/lib_sqlite.h)||

### Inputs
<a href="#inputs"></a>
`lib_sqlite` takes the following tables on input:
|ssimfile|comment|
|---|---|
|[dmmeta.ctype](/txt/ssimdb/dmmeta/ctype.md)|Struct|
|[dmmeta.field](/txt/ssimdb/dmmeta/field.md)|Specify field of a struct|
|[dev.unstablefld](/txt/ssimdb/dev/unstablefld.md)|Fields that should be stripped from component test output because they contain timestamps etc.|
|[dmmeta.substr](/txt/ssimdb/dmmeta/substr.md)|Specify that the field value is computed from a substring of another field|
|[dmmeta.ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|File with ssim tuples|
|[dmmeta.sqltype](/txt/ssimdb/dmmeta/sqltype.md)|Mapping of ctype -> SQL expression|
|[dmmeta.ftuple](/txt/ssimdb/dmmeta/ftuple.md)||
|[dmmeta.fconst](/txt/ssimdb/dmmeta/fconst.md)|Specify enum value (integer + string constant) for a field|
|[dmmeta.cppfunc](/txt/ssimdb/dmmeta/cppfunc.md)|Value of field provided by this expression|
|[dmmeta.cfmt](/txt/ssimdb/dmmeta/cfmt.md)|Specify options for printing/reading ctypes into multiple formats|
|[dmmeta.cdflt](/txt/ssimdb/dmmeta/cdflt.md)|Specify default value for single-value types that lack fields|
|[amcdb.bltin](/txt/ssimdb/amcdb/bltin.md)|Specify properties of a C built-in type|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|


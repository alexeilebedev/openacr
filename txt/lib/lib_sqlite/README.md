## lib_sqlite -


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Toc -->
&nbsp;&nbsp;&bull;&nbsp;  [Functions](#functions)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Sources](#sources)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Dependencies](#dependencies)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [In Memory DB](#in-memory-db)<br/>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Toc -->

### Functions
<a href="#functions"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Functions -->
Functions exported from this namespace:

```c++
void lib_sqlite::Init() 
```

```c++
int lib_sqlite::Open(lib_sqlite::FConn& conn) 
```

```c++
// Entrypoint for vtab extension
int lib_sqlite::VtabInitExt(sqlite3 *db, char **pzErrMsg, const sqlite3_api_routines *pApi) 
```

```c++
// Scalar function to initialize virtual tables with given data path
void lib_sqlite::VtabInitFunc(sqlite3_context *context, int argc, sqlite3_value **argv) 
```

```c++
void lib_sqlite::db_Cleanup(lib_sqlite::FConn &parent) 
```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Functions -->

### Inputs
<a href="#inputs"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Inputs -->
`lib_sqlite` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|
|[dmmeta.ctype](/txt/ssimdb/dmmeta/ctype.md)|Struct|
|[dmmeta.field](/txt/ssimdb/dmmeta/field.md)|Specify field of a struct|
|[dmmeta.ns](/txt/ssimdb/dmmeta/ns.md)|Namespace (for in-memory database, protocol, etc)|
|[dmmeta.sqltype](/txt/ssimdb/dmmeta/sqltype.md)|Mapping of ctype -> SQL expression|
|[dmmeta.ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|File with ssim tuples|
|[dmmeta.substr](/txt/ssimdb/dmmeta/substr.md)|Specify that the field value is computed from a substring of another field|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Inputs -->

### Sources
<a href="#sources"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Sources -->
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/gen/lib_sqlite_gen.cpp](/cpp/gen/lib_sqlite_gen.cpp)||
|[cpp/lib_sqlite/lib_sqlite.cpp](/cpp/lib_sqlite/lib_sqlite.cpp)||
|[cpp/lib_sqlite/vtab.cpp](/cpp/lib_sqlite/vtab.cpp)||
|[extern/sqlite-3500400/sqlite3.c](/extern/sqlite-3500400/sqlite3.c)||
|[extern/sqlite-3500400/sqlite3.h](/extern/sqlite-3500400/sqlite3.h)||
|[include/gen/lib_sqlite_gen.h](/include/gen/lib_sqlite_gen.h)||
|[include/gen/lib_sqlite_gen.inl.h](/include/gen/lib_sqlite_gen.inl.h)||
|[include/lib_sqlite.h](/include/lib_sqlite.h)||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Sources -->

### Dependencies
<a href="#dependencies"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Dependencies -->
The build target depends on the following libraries
|Target|Comment|
|---|---|
|[algo_lib](/txt/lib/algo_lib/README.md)|Support library for all executables|
|[lib_prot](/txt/lib/lib_prot/README.md)|Library covering all protocols|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Dependencies -->

### In Memory DB
<a href="#in-memory-db"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Imdb -->
`lib_sqlite` generated code creates the tables below.
All allocations are done through global `lib_sqlite::_db` [lib_sqlite.FDb](#lib_sqlite-fdb) structure
|Ctype|Ssimfile|Create|Access|
|---|---|---|---|
|[lib_sqlite.Cons](#lib_sqlite-cons)||FIdx.cons (Tary)|
||||VtabCurs.c_pkey (Ptr)|
|[lib_sqlite.FConn](#lib_sqlite-fconn)||FDb.conn (Tpool)|ind_conn (Thash, hash field name)|
|[lib_sqlite.FCtype](#lib_sqlite-fctype)|[dmmeta.ctype](/txt/ssimdb/dmmeta/ctype.md)|FDb.ctype (Lary)|ctype (Lary, by rowid)|ind_ctype (Thash, hash field ctype)|
||||FField.p_arg (Upptr)|
||||FField.p_ctype (Upptr)|
||||FRow.p_ctype (Upptr)|
||||FSsimfile.p_ctype (Upptr)|
||||Vtab.p_ctype (Ptr)|
||||VtabCurs.p_ctype (Upptr)|
|[lib_sqlite.FDb](#lib_sqlite-fdb)||FDb._db (Global)|
|[lib_sqlite.FField](#lib_sqlite-ffield)|[dmmeta.field](/txt/ssimdb/dmmeta/field.md)|FDb.field (Lary)|field (Lary, by rowid)|ind_field (Thash, hash field field)|
||||Cons.p_field (Upptr)|
||||FCtype.c_field (Ptrary)|
||||FCtype.ind_field_name (Thash)|
||||FSubstr.p_srcfield (Upptr)|
|[lib_sqlite.FIdx](#lib_sqlite-fidx)||FDb.bestidx (Tary)|
||||VtabCurs.c_idx (Ptr)|
|[lib_sqlite.FNs](#lib_sqlite-fns)|[dmmeta.ns](/txt/ssimdb/dmmeta/ns.md)|FDb.ns (Lary)|ns (Lary, by rowid)|ind_ns (Thash, hash field ns)|
||||FSsimfile.p_ns (Upptr)|
|[lib_sqlite.FRow](#lib_sqlite-frow)||FDb.trow (Lary)|trow (Lary, by rowid)|
||||FCtype.zd_row (Llist)|
||||FCtype.ind_pkey (Thash)|
||||FCtype.c_row (Ptrary)|
||||VtabCurs.c_row (Ptr)|
|[lib_sqlite.FSqltype](#lib_sqlite-fsqltype)|[dmmeta.sqltype](/txt/ssimdb/dmmeta/sqltype.md)|FDb.sqltype (Lary)|sqltype (Lary, by rowid)|
||||FCtype.c_sqltype (Ptr)|
|[lib_sqlite.FSsimfile](#lib_sqlite-fssimfile)|[dmmeta.ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|FDb.ssimfile (Lary)|ssimfile (Lary, by rowid)|ind_ssimfile (Thash, hash field ssimfile)|
||||FCtype.c_ssimfile (Ptr)|
||||FNs.c_ssimfile (Ptrary)|
|[lib_sqlite.FSubstr](#lib_sqlite-fsubstr)|[dmmeta.substr](/txt/ssimdb/dmmeta/substr.md)|FDb.substr (Lary)|substr (Lary, by rowid)|
||||FField.c_substr (Ptr)|
|[lib_sqlite.Vtab](#lib_sqlite-vtab)||
||||VtabCurs.p_vtab (Upptr)|
|[lib_sqlite.VtabCurs](#lib_sqlite-vtabcurs)||
||||Vtab.c_curs (Ptrary)|

#### lib_sqlite.Cons - 
<a href="#lib_sqlite-cons"></a>

#### lib_sqlite.Cons Fields
<a href="#lib_sqlite-cons-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_sqlite.Cons.p_field|[lib_sqlite.FField](/txt/lib/lib_sqlite/README.md#lib_sqlite-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|lib_sqlite.Cons.value|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_sqlite.Cons.icol|i32|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_sqlite.Cons.op|i32|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct Cons
<a href="#struct-cons"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_sqlite_gen.h](/include/gen/lib_sqlite_gen.h)
```
struct Cons { // lib_sqlite.Cons
    lib_sqlite::FField*   p_field;   // reference to parent row
    algo::cstring         value;     //
    i32                   icol;      //   0
    i32                   op;        //   0
    // func:lib_sqlite.Cons..Ctor
    inline               Cons() __attribute__((nothrow));
};
```

#### lib_sqlite.FConn - 
<a href="#lib_sqlite-fconn"></a>

#### lib_sqlite.FConn Fields
<a href="#lib_sqlite-fconn-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_sqlite.FConn.name|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_sqlite.FConn.db|sqlite3|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||

#### Struct FConn
<a href="#struct-fconn"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_sqlite_gen.h](/include/gen/lib_sqlite_gen.h)
```
struct FConn { // lib_sqlite.FConn
    algo::cstring        name;               //
    sqlite3*             db;                 // optional pointer
    lib_sqlite::FConn*   conn_next;          // Pointer to next free element int tpool
    lib_sqlite::FConn*   ind_conn_next;      // hash next
    u32                  ind_conn_hashval;   // hash value
    // user-defined fcleanup on lib_sqlite.FConn.db prevents copy
    // func:lib_sqlite.FConn..AssignOp
    inline lib_sqlite::FConn& operator =(const lib_sqlite::FConn &rhs) = delete;
    // user-defined fcleanup on lib_sqlite.FConn.db prevents copy
    // func:lib_sqlite.FConn..CopyCtor
    inline               FConn(const lib_sqlite::FConn &rhs) = delete;
private:
    // func:lib_sqlite.FConn..Ctor
    inline               FConn() __attribute__((nothrow));
    // func:lib_sqlite.FConn..Dtor
    inline               ~FConn() __attribute__((nothrow));
    friend lib_sqlite::FConn&   conn_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend lib_sqlite::FConn*   conn_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 conn_Delete(lib_sqlite::FConn &row) __attribute__((nothrow));
};
```

#### lib_sqlite.FCtype - Struct
<a href="#lib_sqlite-fctype"></a>

#### lib_sqlite.FCtype Fields
<a href="#lib_sqlite-fctype-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_sqlite.FCtype.base|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Base](/txt/ssimdb/dmmeta/ctype.md)|||
|lib_sqlite.FCtype.c_field|[lib_sqlite.FField](/txt/lib/lib_sqlite/README.md#lib_sqlite-ffield)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|lib_sqlite.FCtype.c_sqltype|[lib_sqlite.FSqltype](/txt/lib/lib_sqlite/README.md#lib_sqlite-fsqltype)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|lib_sqlite.FCtype.ind_field_name|[lib_sqlite.FField](/txt/lib/lib_sqlite/README.md#lib_sqlite-ffield)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|lib_sqlite.FCtype.c_ssimfile|[lib_sqlite.FSsimfile](/txt/lib/lib_sqlite/README.md#lib_sqlite-fssimfile)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|lib_sqlite.FCtype.zd_row|[lib_sqlite.FRow](/txt/lib/lib_sqlite/README.md#lib_sqlite-frow)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|lib_sqlite.FCtype.ind_pkey|[lib_sqlite.FRow](/txt/lib/lib_sqlite/README.md#lib_sqlite-frow)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|lib_sqlite.FCtype.c_row|[lib_sqlite.FRow](/txt/lib/lib_sqlite/README.md#lib_sqlite-frow)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||

#### Struct FCtype
<a href="#struct-fctype"></a>
*Note:* field ``lib_sqlite.FCtype.base`` has reftype ``base`` so the fields of [dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_sqlite_gen.h](/include/gen/lib_sqlite_gen.h)
```
struct FCtype { // lib_sqlite.FCtype
    algo::Smallstr100        ctype;                          // Identifier. must be ns.typename
    algo::Comment            comment;                        //
    lib_sqlite::FField**     c_field_elems;                  // array of pointers
    u32                      c_field_n;                      // array of pointers
    u32                      c_field_max;                    // capacity of allocated array
    lib_sqlite::FSqltype*    c_sqltype;                      // optional pointer
    lib_sqlite::FField**     ind_field_name_buckets_elems;   // pointer to bucket array
    i32                      ind_field_name_buckets_n;       // number of elements in bucket array
    i32                      ind_field_name_n;               // number of elements in the hash table
    lib_sqlite::FSsimfile*   c_ssimfile;                     // optional pointer
    lib_sqlite::FRow*        zd_row_head;                    // zero-terminated doubly linked list
    i32                      zd_row_n;                       // zero-terminated doubly linked list
    lib_sqlite::FRow*        zd_row_tail;                    // pointer to last element
    lib_sqlite::FRow**       ind_pkey_buckets_elems;         // pointer to bucket array
    i32                      ind_pkey_buckets_n;             // number of elements in bucket array
    i32                      ind_pkey_n;                     // number of elements in the hash table
    lib_sqlite::FRow**       c_row_elems;                    // array of pointers
    u32                      c_row_n;                        // array of pointers
    u32                      c_row_max;                      // capacity of allocated array
    lib_sqlite::FCtype*      ind_ctype_next;                 // hash next
    u32                      ind_ctype_hashval;              // hash value
    // reftype Ptrary of lib_sqlite.FCtype.c_field prohibits copy
    // x-reference on lib_sqlite.FCtype.c_sqltype prevents copy
    // reftype Thash of lib_sqlite.FCtype.ind_field_name prohibits copy
    // x-reference on lib_sqlite.FCtype.c_ssimfile prevents copy
    // reftype Llist of lib_sqlite.FCtype.zd_row prohibits copy
    // reftype Thash of lib_sqlite.FCtype.ind_pkey prohibits copy
    // reftype Ptrary of lib_sqlite.FCtype.c_row prohibits copy
    // func:lib_sqlite.FCtype..AssignOp
    lib_sqlite::FCtype&  operator =(const lib_sqlite::FCtype &rhs) = delete;
    // reftype Ptrary of lib_sqlite.FCtype.c_field prohibits copy
    // x-reference on lib_sqlite.FCtype.c_sqltype prevents copy
    // reftype Thash of lib_sqlite.FCtype.ind_field_name prohibits copy
    // x-reference on lib_sqlite.FCtype.c_ssimfile prevents copy
    // reftype Llist of lib_sqlite.FCtype.zd_row prohibits copy
    // reftype Thash of lib_sqlite.FCtype.ind_pkey prohibits copy
    // reftype Ptrary of lib_sqlite.FCtype.c_row prohibits copy
    // func:lib_sqlite.FCtype..CopyCtor
    FCtype(const lib_sqlite::FCtype &rhs) = delete;
private:
    // func:lib_sqlite.FCtype..Ctor
    inline               FCtype() __attribute__((nothrow));
    // func:lib_sqlite.FCtype..Dtor
    inline               ~FCtype() __attribute__((nothrow));
    friend lib_sqlite::FCtype&  ctype_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend lib_sqlite::FCtype*  ctype_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 ctype_RemoveAll() __attribute__((nothrow));
    friend void                 ctype_RemoveLast() __attribute__((nothrow));
};
```

#### lib_sqlite.FDb - 
<a href="#lib_sqlite-fdb"></a>

#### lib_sqlite.FDb Fields
<a href="#lib_sqlite-fdb-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_sqlite.FDb._db|[lib_sqlite.FDb](/txt/lib/lib_sqlite/README.md#lib_sqlite-fdb)|[Global](/txt/exe/amc/reftypes.md#global)|||
|lib_sqlite.FDb.conn|[lib_sqlite.FConn](/txt/lib/lib_sqlite/README.md#lib_sqlite-fconn)|[Tpool](/txt/exe/amc/reftypes.md#tpool)|||
|lib_sqlite.FDb.ind_conn|[lib_sqlite.FConn](/txt/lib/lib_sqlite/README.md#lib_sqlite-fconn)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|lib_sqlite.FDb.substr|[lib_sqlite.FSubstr](/txt/lib/lib_sqlite/README.md#lib_sqlite-fsubstr)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|lib_sqlite.FDb.field|[lib_sqlite.FField](/txt/lib/lib_sqlite/README.md#lib_sqlite-ffield)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|lib_sqlite.FDb.ind_field|[lib_sqlite.FField](/txt/lib/lib_sqlite/README.md#lib_sqlite-ffield)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|lib_sqlite.FDb.ctype|[lib_sqlite.FCtype](/txt/lib/lib_sqlite/README.md#lib_sqlite-fctype)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|lib_sqlite.FDb.ind_ctype|[lib_sqlite.FCtype](/txt/lib/lib_sqlite/README.md#lib_sqlite-fctype)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|lib_sqlite.FDb.ssimfile|[lib_sqlite.FSsimfile](/txt/lib/lib_sqlite/README.md#lib_sqlite-fssimfile)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|lib_sqlite.FDb.ind_ssimfile|[lib_sqlite.FSsimfile](/txt/lib/lib_sqlite/README.md#lib_sqlite-fssimfile)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|lib_sqlite.FDb.sqltype|[lib_sqlite.FSqltype](/txt/lib/lib_sqlite/README.md#lib_sqlite-fsqltype)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|lib_sqlite.FDb.bestidx|[lib_sqlite.FIdx](/txt/lib/lib_sqlite/README.md#lib_sqlite-fidx)|[Tary](/txt/exe/amc/reftypes.md#tary)|||
|lib_sqlite.FDb.trow|[lib_sqlite.FRow](/txt/lib/lib_sqlite/README.md#lib_sqlite-frow)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|lib_sqlite.FDb.ns|[lib_sqlite.FNs](/txt/lib/lib_sqlite/README.md#lib_sqlite-fns)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|lib_sqlite.FDb.ind_ns|[lib_sqlite.FNs](/txt/lib/lib_sqlite/README.md#lib_sqlite-fns)|[Thash](/txt/exe/amc/reftypes.md#thash)|||

#### Struct FDb
<a href="#struct-fdb"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_sqlite_gen.h](/include/gen/lib_sqlite_gen.h)
```
struct FDb { // lib_sqlite.FDb
    u64                       conn_blocksize;               // # bytes per block
    lib_sqlite::FConn*        conn_free;                    //
    lib_sqlite::FConn**       ind_conn_buckets_elems;       // pointer to bucket array
    i32                       ind_conn_buckets_n;           // number of elements in bucket array
    i32                       ind_conn_n;                   // number of elements in the hash table
    lib_sqlite::FSubstr*      substr_lary[32];              // level array
    i32                       substr_n;                     // number of elements in array
    lib_sqlite::FField*       field_lary[32];               // level array
    i32                       field_n;                      // number of elements in array
    lib_sqlite::FField**      ind_field_buckets_elems;      // pointer to bucket array
    i32                       ind_field_buckets_n;          // number of elements in bucket array
    i32                       ind_field_n;                  // number of elements in the hash table
    lib_sqlite::FCtype*       ctype_lary[32];               // level array
    i32                       ctype_n;                      // number of elements in array
    lib_sqlite::FCtype**      ind_ctype_buckets_elems;      // pointer to bucket array
    i32                       ind_ctype_buckets_n;          // number of elements in bucket array
    i32                       ind_ctype_n;                  // number of elements in the hash table
    lib_sqlite::FSsimfile*    ssimfile_lary[32];            // level array
    i32                       ssimfile_n;                   // number of elements in array
    lib_sqlite::FSsimfile**   ind_ssimfile_buckets_elems;   // pointer to bucket array
    i32                       ind_ssimfile_buckets_n;       // number of elements in bucket array
    i32                       ind_ssimfile_n;               // number of elements in the hash table
    lib_sqlite::FSqltype*     sqltype_lary[32];             // level array
    i32                       sqltype_n;                    // number of elements in array
    lib_sqlite::FIdx*         bestidx_elems;                // pointer to elements
    u32                       bestidx_n;                    // number of elements in array
    u32                       bestidx_max;                  // max. capacity of array before realloc
    lib_sqlite::FRow*         trow_lary[32];                // level array
    i32                       trow_n;                       // number of elements in array
    lib_sqlite::FNs*          ns_lary[32];                  // level array
    i32                       ns_n;                         // number of elements in array
    lib_sqlite::FNs**         ind_ns_buckets_elems;         // pointer to bucket array
    i32                       ind_ns_buckets_n;             // number of elements in bucket array
    i32                       ind_ns_n;                     // number of elements in the hash table
    lib_sqlite::trace         trace;                        //
};
```

#### lib_sqlite.FField - Specify field of a struct
<a href="#lib_sqlite-ffield"></a>

#### lib_sqlite.FField Fields
<a href="#lib_sqlite-ffield-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_sqlite.FField.base|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Base](/txt/ssimdb/dmmeta/field.md)|||
|lib_sqlite.FField.c_substr|[lib_sqlite.FSubstr](/txt/lib/lib_sqlite/README.md#lib_sqlite-fsubstr)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|lib_sqlite.FField.p_arg|[lib_sqlite.FCtype](/txt/lib/lib_sqlite/README.md#lib_sqlite-fctype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|lib_sqlite.FField.p_ctype|[lib_sqlite.FCtype](/txt/lib/lib_sqlite/README.md#lib_sqlite-fctype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|lib_sqlite.FField.id|u32|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FField
<a href="#struct-ffield"></a>
*Note:* field ``lib_sqlite.FField.base`` has reftype ``base`` so the fields of [dmmeta.Field](/txt/ssimdb/dmmeta/field.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_sqlite_gen.h](/include/gen/lib_sqlite_gen.h)
```
struct FField { // lib_sqlite.FField
    lib_sqlite::FField*    ctype_ind_field_name_next;      // hash next
    u32                    ctype_ind_field_name_hashval;   // hash value
    lib_sqlite::FField*    ind_field_next;                 // hash next
    u32                    ind_field_hashval;              // hash value
    algo::Smallstr100      field;                          // Primary key, as ctype.name
    algo::Smallstr100      arg;                            // Type of field
    algo::Smallstr50       reftype;                        //   "Val"  Type constructor
    algo::CppExpr          dflt;                           // Default value (c++ expression)
    algo::Comment          comment;                        //
    lib_sqlite::FSubstr*   c_substr;                       // optional pointer
    lib_sqlite::FCtype*    p_arg;                          // reference to parent row
    lib_sqlite::FCtype*    p_ctype;                        // reference to parent row
    u32                    id;                             //   0
    bool                   ctype_c_field_in_ary;           //   false  membership flag
    // x-reference on lib_sqlite.FField.c_substr prevents copy
    // x-reference on lib_sqlite.FField.p_arg prevents copy
    // x-reference on lib_sqlite.FField.p_ctype prevents copy
    // func:lib_sqlite.FField..AssignOp
    lib_sqlite::FField&  operator =(const lib_sqlite::FField &rhs) = delete;
    // x-reference on lib_sqlite.FField.c_substr prevents copy
    // x-reference on lib_sqlite.FField.p_arg prevents copy
    // x-reference on lib_sqlite.FField.p_ctype prevents copy
    // func:lib_sqlite.FField..CopyCtor
    FField(const lib_sqlite::FField &rhs) = delete;
private:
    // func:lib_sqlite.FField..Ctor
    inline               FField() __attribute__((nothrow));
    // func:lib_sqlite.FField..Dtor
    inline               ~FField() __attribute__((nothrow));
    friend lib_sqlite::FField&  field_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend lib_sqlite::FField*  field_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 field_RemoveAll() __attribute__((nothrow));
    friend void                 field_RemoveLast() __attribute__((nothrow));
};
```

#### lib_sqlite.FIdx - 
<a href="#lib_sqlite-fidx"></a>

#### lib_sqlite.FIdx Fields
<a href="#lib_sqlite-fidx-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_sqlite.FIdx.cons|[lib_sqlite.Cons](/txt/lib/lib_sqlite/README.md#lib_sqlite-cons)|[Tary](/txt/exe/amc/reftypes.md#tary)|||
|lib_sqlite.FIdx.rowid|i32|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FIdx
<a href="#struct-fidx"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_sqlite_gen.h](/include/gen/lib_sqlite_gen.h)
```
struct FIdx { // lib_sqlite.FIdx
    lib_sqlite::Cons*   cons_elems;   // pointer to elements
    u32                 cons_n;       // number of elements in array
    u32                 cons_max;     // max. capacity of array before realloc
    i32                 rowid;        //   0
    // func:lib_sqlite.FIdx..AssignOp
    lib_sqlite::FIdx&    operator =(const lib_sqlite::FIdx &rhs) __attribute__((nothrow));
    // func:lib_sqlite.FIdx..Ctor
    inline               FIdx() __attribute__((nothrow));
    // func:lib_sqlite.FIdx..Dtor
    inline               ~FIdx() __attribute__((nothrow));
    // func:lib_sqlite.FIdx..CopyCtor
    FIdx(const lib_sqlite::FIdx &rhs) __attribute__((nothrow));
};
```

#### lib_sqlite.FNs - Namespace (for in-memory database, protocol, etc)
<a href="#lib_sqlite-fns"></a>

#### lib_sqlite.FNs Fields
<a href="#lib_sqlite-fns-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_sqlite.FNs.base|[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)|[Base](/txt/ssimdb/dmmeta/ns.md)|||
|lib_sqlite.FNs.c_ssimfile|[lib_sqlite.FSsimfile](/txt/lib/lib_sqlite/README.md#lib_sqlite-fssimfile)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||

#### Struct FNs
<a href="#struct-fns"></a>
*Note:* field ``lib_sqlite.FNs.base`` has reftype ``base`` so the fields of [dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_sqlite_gen.h](/include/gen/lib_sqlite_gen.h)
```
struct FNs { // lib_sqlite.FNs
    lib_sqlite::FNs*          ind_ns_next;        // hash next
    u32                       ind_ns_hashval;     // hash value
    algo::Smallstr16          ns;                 // Namespace name (primary key)
    algo::Smallstr50          nstype;             // Namespace type
    algo::Smallstr50          license;            // Associated license
    algo::Comment             comment;            //
    lib_sqlite::FSsimfile**   c_ssimfile_elems;   // array of pointers
    u32                       c_ssimfile_n;       // array of pointers
    u32                       c_ssimfile_max;     // capacity of allocated array
    // reftype Ptrary of lib_sqlite.FNs.c_ssimfile prohibits copy
    // func:lib_sqlite.FNs..AssignOp
    inline lib_sqlite::FNs& operator =(const lib_sqlite::FNs &rhs) = delete;
    // reftype Ptrary of lib_sqlite.FNs.c_ssimfile prohibits copy
    // func:lib_sqlite.FNs..CopyCtor
    inline               FNs(const lib_sqlite::FNs &rhs) = delete;
private:
    // func:lib_sqlite.FNs..Ctor
    inline               FNs() __attribute__((nothrow));
    // func:lib_sqlite.FNs..Dtor
    inline               ~FNs() __attribute__((nothrow));
    friend lib_sqlite::FNs&     ns_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend lib_sqlite::FNs*     ns_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 ns_RemoveAll() __attribute__((nothrow));
    friend void                 ns_RemoveLast() __attribute__((nothrow));
};
```

#### lib_sqlite.FRow - 
<a href="#lib_sqlite-frow"></a>

#### lib_sqlite.FRow Fields
<a href="#lib_sqlite-frow-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_sqlite.FRow.pkey|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_sqlite.FRow.tuple|[algo.Tuple](/txt/protocol/algo/Tuple.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_sqlite.FRow.p_ctype|[lib_sqlite.FCtype](/txt/lib/lib_sqlite/README.md#lib_sqlite-fctype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|lib_sqlite.FRow.rowid|i64|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FRow
<a href="#struct-frow"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_sqlite_gen.h](/include/gen/lib_sqlite_gen.h)
```
struct FRow { // lib_sqlite.FRow
    lib_sqlite::FRow*     ctype_zd_row_next;        // zslist link; -1 means not-in-list
    lib_sqlite::FRow*     ctype_zd_row_prev;        // previous element
    lib_sqlite::FRow*     ctype_ind_pkey_next;      // hash next
    u32                   ctype_ind_pkey_hashval;   // hash value
    algo::cstring         pkey;                     //
    algo::Tuple           tuple;                    //
    lib_sqlite::FCtype*   p_ctype;                  // reference to parent row
    i64                   rowid;                    //   0
    bool                  ctype_c_row_in_ary;       //   false  membership flag
    // func:lib_sqlite.FRow..AssignOp
    inline lib_sqlite::FRow& operator =(const lib_sqlite::FRow &rhs) = delete;
    // func:lib_sqlite.FRow..CopyCtor
    inline               FRow(const lib_sqlite::FRow &rhs) = delete;
private:
    // func:lib_sqlite.FRow..Ctor
    inline               FRow() __attribute__((nothrow));
    // func:lib_sqlite.FRow..Dtor
    inline               ~FRow() __attribute__((nothrow));
    friend lib_sqlite::FRow&    trow_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend lib_sqlite::FRow*    trow_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 trow_RemoveAll() __attribute__((nothrow));
    friend void                 trow_RemoveLast() __attribute__((nothrow));
};
```

#### lib_sqlite.FSqltype - Mapping of ctype -> SQL expression
<a href="#lib_sqlite-fsqltype"></a>

#### lib_sqlite.FSqltype Fields
<a href="#lib_sqlite-fsqltype-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_sqlite.FSqltype.base|[dmmeta.Sqltype](/txt/ssimdb/dmmeta/sqltype.md)|[Base](/txt/ssimdb/dmmeta/sqltype.md)|||

#### Struct FSqltype
<a href="#struct-fsqltype"></a>
*Note:* field ``lib_sqlite.FSqltype.base`` has reftype ``base`` so the fields of [dmmeta.Sqltype](/txt/ssimdb/dmmeta/sqltype.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_sqlite_gen.h](/include/gen/lib_sqlite_gen.h)
```
struct FSqltype { // lib_sqlite.FSqltype
    algo::Smallstr100   ctype;     //
    algo::Smallstr100   expr;      //
    algo::Comment       comment;   //
    // func:lib_sqlite.FSqltype..AssignOp
    inline lib_sqlite::FSqltype& operator =(const lib_sqlite::FSqltype &rhs) = delete;
    // func:lib_sqlite.FSqltype..CopyCtor
    inline               FSqltype(const lib_sqlite::FSqltype &rhs) = delete;
private:
    // func:lib_sqlite.FSqltype..Ctor
    inline               FSqltype() __attribute__((nothrow));
    // func:lib_sqlite.FSqltype..Dtor
    inline               ~FSqltype() __attribute__((nothrow));
    friend lib_sqlite::FSqltype& sqltype_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend lib_sqlite::FSqltype* sqltype_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 sqltype_RemoveAll() __attribute__((nothrow));
    friend void                 sqltype_RemoveLast() __attribute__((nothrow));
};
```

#### lib_sqlite.FSsimfile - File with ssim tuples
<a href="#lib_sqlite-fssimfile"></a>

#### lib_sqlite.FSsimfile Fields
<a href="#lib_sqlite-fssimfile-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_sqlite.FSsimfile.base|[dmmeta.Ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|[Base](/txt/ssimdb/dmmeta/ssimfile.md)|||
|lib_sqlite.FSsimfile.p_ctype|[lib_sqlite.FCtype](/txt/lib/lib_sqlite/README.md#lib_sqlite-fctype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|lib_sqlite.FSsimfile.p_ns|[lib_sqlite.FNs](/txt/lib/lib_sqlite/README.md#lib_sqlite-fns)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FSsimfile
<a href="#struct-fssimfile"></a>
*Note:* field ``lib_sqlite.FSsimfile.base`` has reftype ``base`` so the fields of [dmmeta.Ssimfile](/txt/ssimdb/dmmeta/ssimfile.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_sqlite_gen.h](/include/gen/lib_sqlite_gen.h)
```
struct FSsimfile { // lib_sqlite.FSsimfile
    lib_sqlite::FSsimfile*   ind_ssimfile_next;      // hash next
    u32                      ind_ssimfile_hashval;   // hash value
    algo::Smallstr50         ssimfile;               //
    algo::Smallstr100        ctype;                  //
    lib_sqlite::FCtype*      p_ctype;                // reference to parent row
    lib_sqlite::FNs*         p_ns;                   // reference to parent row
    bool                     ns_c_ssimfile_in_ary;   //   false  membership flag
    // x-reference on lib_sqlite.FSsimfile.p_ctype prevents copy
    // x-reference on lib_sqlite.FSsimfile.p_ns prevents copy
    // func:lib_sqlite.FSsimfile..AssignOp
    inline lib_sqlite::FSsimfile& operator =(const lib_sqlite::FSsimfile &rhs) = delete;
    // x-reference on lib_sqlite.FSsimfile.p_ctype prevents copy
    // x-reference on lib_sqlite.FSsimfile.p_ns prevents copy
    // func:lib_sqlite.FSsimfile..CopyCtor
    inline               FSsimfile(const lib_sqlite::FSsimfile &rhs) = delete;
private:
    // func:lib_sqlite.FSsimfile..Ctor
    inline               FSsimfile() __attribute__((nothrow));
    // func:lib_sqlite.FSsimfile..Dtor
    inline               ~FSsimfile() __attribute__((nothrow));
    friend lib_sqlite::FSsimfile& ssimfile_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend lib_sqlite::FSsimfile* ssimfile_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 ssimfile_RemoveAll() __attribute__((nothrow));
    friend void                 ssimfile_RemoveLast() __attribute__((nothrow));
};
```

#### lib_sqlite.FSubstr - Specify that the field value is computed from a substring of another field
<a href="#lib_sqlite-fsubstr"></a>

#### lib_sqlite.FSubstr Fields
<a href="#lib_sqlite-fsubstr-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_sqlite.FSubstr.base|[dmmeta.Substr](/txt/ssimdb/dmmeta/substr.md)|[Base](/txt/ssimdb/dmmeta/substr.md)|||
|lib_sqlite.FSubstr.p_srcfield|[lib_sqlite.FField](/txt/lib/lib_sqlite/README.md#lib_sqlite-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FSubstr
<a href="#struct-fsubstr"></a>
*Note:* field ``lib_sqlite.FSubstr.base`` has reftype ``base`` so the fields of [dmmeta.Substr](/txt/ssimdb/dmmeta/substr.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_sqlite_gen.h](/include/gen/lib_sqlite_gen.h)
```
struct FSubstr { // lib_sqlite.FSubstr
    algo::Smallstr100     field;        //
    algo::CppExpr         expr;         //
    algo::Smallstr100     srcfield;     //
    lib_sqlite::FField*   p_srcfield;   // reference to parent row
    // x-reference on lib_sqlite.FSubstr.p_srcfield prevents copy
    // func:lib_sqlite.FSubstr..AssignOp
    inline lib_sqlite::FSubstr& operator =(const lib_sqlite::FSubstr &rhs) = delete;
    // x-reference on lib_sqlite.FSubstr.p_srcfield prevents copy
    // func:lib_sqlite.FSubstr..CopyCtor
    inline               FSubstr(const lib_sqlite::FSubstr &rhs) = delete;
private:
    // func:lib_sqlite.FSubstr..Ctor
    inline               FSubstr() __attribute__((nothrow));
    // func:lib_sqlite.FSubstr..Dtor
    inline               ~FSubstr() __attribute__((nothrow));
    friend lib_sqlite::FSubstr& substr_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend lib_sqlite::FSubstr* substr_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 substr_RemoveAll() __attribute__((nothrow));
    friend void                 substr_RemoveLast() __attribute__((nothrow));
};
```

#### lib_sqlite.Vtab - An instance of the virtual table
<a href="#lib_sqlite-vtab"></a>

#### lib_sqlite.Vtab Fields
<a href="#lib_sqlite-vtab-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_sqlite.Vtab.base|sqlite3_vtab|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_sqlite.Vtab.filename|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_sqlite.Vtab.p_ctype|[lib_sqlite.FCtype](/txt/lib/lib_sqlite/README.md#lib_sqlite-fctype)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|lib_sqlite.Vtab.c_curs|[lib_sqlite.VtabCurs](/txt/lib/lib_sqlite/README.md#lib_sqlite-vtabcurs)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|lib_sqlite.Vtab.loaded|bool|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct Vtab
<a href="#struct-vtab"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_sqlite_gen.h](/include/gen/lib_sqlite_gen.h)
```
struct Vtab { // lib_sqlite.Vtab: An instance of the virtual table
    sqlite3_vtab             base;           //
    algo::cstring            filename;       //
    lib_sqlite::FCtype*      p_ctype;        // optional pointer
    lib_sqlite::VtabCurs**   c_curs_elems;   // array of pointers
    u32                      c_curs_n;       // array of pointers
    u32                      c_curs_max;     // capacity of allocated array
    bool                     loaded;         //   false
    // func:lib_sqlite.Vtab..Ctor
    inline               Vtab() __attribute__((nothrow));
    // func:lib_sqlite.Vtab..Dtor
    inline               ~Vtab() __attribute__((nothrow));
};
```

#### lib_sqlite.VtabCurs - A cursor for the virtual table
<a href="#lib_sqlite-vtabcurs"></a>

#### lib_sqlite.VtabCurs Fields
<a href="#lib_sqlite-vtabcurs-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_sqlite.VtabCurs.base|sqlite3_vtab_cursor|[Val](/txt/exe/amc/reftypes.md#val)||Base class. Must be first|
|lib_sqlite.VtabCurs.eof|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_sqlite.VtabCurs.p_vtab|[lib_sqlite.Vtab](/txt/lib/lib_sqlite/README.md#lib_sqlite-vtab)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|lib_sqlite.VtabCurs.p_ctype|[lib_sqlite.FCtype](/txt/lib/lib_sqlite/README.md#lib_sqlite-fctype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|lib_sqlite.VtabCurs.c_row|[lib_sqlite.FRow](/txt/lib/lib_sqlite/README.md#lib_sqlite-frow)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|lib_sqlite.VtabCurs.c_pkey|[lib_sqlite.Cons](/txt/lib/lib_sqlite/README.md#lib_sqlite-cons)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|lib_sqlite.VtabCurs.c_idx|[lib_sqlite.FIdx](/txt/lib/lib_sqlite/README.md#lib_sqlite-fidx)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|lib_sqlite.VtabCurs.rowid|i32|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_sqlite.VtabCurs.attrs|[algo.cstring](/txt/protocol/algo/cstring.md)|[Tary](/txt/exe/amc/reftypes.md#tary)|||

#### Struct VtabCurs
<a href="#struct-vtabcurs"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_sqlite_gen.h](/include/gen/lib_sqlite_gen.h)
```
struct VtabCurs { // lib_sqlite.VtabCurs: A cursor for the virtual table
    sqlite3_vtab_cursor   base;                   // Base class. Must be first
    bool                  eof;                    //   false
    lib_sqlite::Vtab*     p_vtab;                 // reference to parent row
    lib_sqlite::FCtype*   p_ctype;                // reference to parent row
    lib_sqlite::FRow*     c_row;                  // optional pointer
    lib_sqlite::Cons*     c_pkey;                 // optional pointer
    lib_sqlite::FIdx*     c_idx;                  // optional pointer
    i32                   rowid;                  //   0
    algo::cstring*        attrs_elems;            // pointer to elements
    u32                   attrs_n;                // number of elements in array
    u32                   attrs_max;              // max. capacity of array before realloc
    bool                  parent_c_curs_in_ary;   //   false  membership flag
    // func:lib_sqlite.VtabCurs..AssignOp
    lib_sqlite::VtabCurs& operator =(const lib_sqlite::VtabCurs &rhs) __attribute__((nothrow));
    // func:lib_sqlite.VtabCurs..Ctor
    inline               VtabCurs() __attribute__((nothrow));
    // func:lib_sqlite.VtabCurs..Dtor
    inline               ~VtabCurs() __attribute__((nothrow));
    // func:lib_sqlite.VtabCurs..CopyCtor
    VtabCurs(const lib_sqlite::VtabCurs &rhs) __attribute__((nothrow));
};
```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Imdb -->


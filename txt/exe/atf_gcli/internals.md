## atf_gcli - Internals


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Toc -->
* [Description](#description)
* [Sources](#sources)
* [Dependencies](#dependencies)
* [In Memory DB](#in-memory-db)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Toc -->

### Description
<a href="#description"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Description -->
for usage, see [atf_gcli - test harness for gcli](/txt/exe/atf_gcli/README.md)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Description -->

### Sources
<a href="#sources"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Sources -->
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/atf_gcli/atf_gcli.cpp](/cpp/atf_gcli/atf_gcli.cpp)||
|[cpp/gen/atf_gcli_gen.cpp](/cpp/gen/atf_gcli_gen.cpp)||
|[include/atf_gcli.h](/include/atf_gcli.h)||
|[include/gen/atf_gcli_gen.h](/include/gen/atf_gcli_gen.h)||
|[include/gen/atf_gcli_gen.inl.h](/include/gen/atf_gcli_gen.inl.h)||

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
`atf_gcli` generated code creates the tables below.
All allocations are done through global `atf_gcli::_db` [atf_gcli.FDb](#atf_gcli-fdb) structure
|Ctype|Ssimfile|Create|Access|
|---|---|---|---|
|[atf_gcli.FDb](#atf_gcli-fdb)||FDb._db (Global)|
|[atf_gcli.FGclienv](#atf_gcli-fgclienv)|[gclidb.gclienv](/txt/ssimdb/gclidb/gclienv.md)|FDb.gclienv (Lary)|gclienv (Lary, by rowid)|ind_gclienv (Thash, hash field gclienv)|
||||FGtblacttst.p_gclienv (Upptr)|
|[atf_gcli.FGclienvsub](#atf_gcli-fgclienvsub)|[gclidb.gclienvsub](/txt/ssimdb/gclidb/gclienvsub.md)|FDb.gclienvsub (Lary)|gclienvsub (Lary, by rowid)|ind_gclienvsub (Thash, hash field gclienvsub)|
||||FGclienv.c_gclienvsub (Ptrary)|
|[atf_gcli.FGtblact](#atf_gcli-fgtblact)|[gclidb.gtblact](/txt/ssimdb/gclidb/gtblact.md)|FDb.gtblact (Lary)|gtblact (Lary, by rowid)|ind_gtblact (Thash, hash field gtblact)|
||||FGtblacttst.p_gtblact (Upptr)|
|[atf_gcli.FGtblacttst](#atf_gcli-fgtblacttst)|[gclidb.gtblacttst](/txt/ssimdb/gclidb/gtblacttst.md)|FDb.gtblacttst (Lary)|gtblacttst (Lary, by rowid)|ind_gtblacttst (Thash, hash field gtblacttst)|c_gtblacttst (Ptrary)|
|[atf_gcli.FGtblacttstout](#atf_gcli-fgtblacttstout)|[gclidb.gtblacttstout](/txt/ssimdb/gclidb/gtblacttstout.md)|FDb.gtblacttstout (Lary)|gtblacttstout (Lary, by rowid)|ind_gtblacttstout (Thash, hash field gtblacttstout)|
||||FGtblacttst.c_gtblacttstout (Ptrary)|

#### atf_gcli.FDb - In-memory database for atf_gcli
<a href="#atf_gcli-fdb"></a>

#### atf_gcli.FDb Fields
<a href="#atf_gcli-fdb-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_gcli.FDb._db|[atf_gcli.FDb](/txt/exe/atf_gcli/internals.md#atf_gcli-fdb)|[Global](/txt/exe/amc/reftypes.md#global)|||
|atf_gcli.FDb.cmdline|[command.atf_gcli](/txt/protocol/command/README.md#command-atf_gcli)|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_gcli.FDb.gtblacttst|[atf_gcli.FGtblacttst](/txt/exe/atf_gcli/internals.md#atf_gcli-fgtblacttst)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|atf_gcli.FDb.gtblacttstout|[atf_gcli.FGtblacttstout](/txt/exe/atf_gcli/internals.md#atf_gcli-fgtblacttstout)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|atf_gcli.FDb.ind_gtblacttst|[atf_gcli.FGtblacttst](/txt/exe/atf_gcli/internals.md#atf_gcli-fgtblacttst)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|atf_gcli.FDb.tempfile|[algo_lib.FTempfile](/txt/lib/algo_lib/README.md#algo_lib-ftempfile)|[Val](/txt/exe/amc/reftypes.md#val)||Temp file to download command output|
|atf_gcli.FDb.ind_gtblacttstout|[atf_gcli.FGtblacttstout](/txt/exe/atf_gcli/internals.md#atf_gcli-fgtblacttstout)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|atf_gcli.FDb.c_gtblacttst|[atf_gcli.FGtblacttst](/txt/exe/atf_gcli/internals.md#atf_gcli-fgtblacttst)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|atf_gcli.FDb.gclienvsub|[atf_gcli.FGclienvsub](/txt/exe/atf_gcli/internals.md#atf_gcli-fgclienvsub)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|atf_gcli.FDb.gclienv|[atf_gcli.FGclienv](/txt/exe/atf_gcli/internals.md#atf_gcli-fgclienv)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|atf_gcli.FDb.ind_gclienv|[atf_gcli.FGclienv](/txt/exe/atf_gcli/internals.md#atf_gcli-fgclienv)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|atf_gcli.FDb.ind_gclienvsub|[atf_gcli.FGclienvsub](/txt/exe/atf_gcli/internals.md#atf_gcli-fgclienvsub)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|atf_gcli.FDb.gtblact|[atf_gcli.FGtblact](/txt/exe/atf_gcli/internals.md#atf_gcli-fgtblact)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|atf_gcli.FDb.ind_gtblact|[atf_gcli.FGtblact](/txt/exe/atf_gcli/internals.md#atf_gcli-fgtblact)|[Thash](/txt/exe/amc/reftypes.md#thash)|||

#### Struct FDb
<a href="#struct-fdb"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_gcli_gen.h](/include/gen/atf_gcli_gen.h)
```
struct FDb { // atf_gcli.FDb: In-memory database for atf_gcli
    command::atf_gcli            cmdline;                           //
    atf_gcli::FGtblacttst*       gtblacttst_lary[32];               // level array
    i32                          gtblacttst_n;                      // number of elements in array
    atf_gcli::FGtblacttstout*    gtblacttstout_lary[32];            // level array
    i32                          gtblacttstout_n;                   // number of elements in array
    atf_gcli::FGtblacttst**      ind_gtblacttst_buckets_elems;      // pointer to bucket array
    i32                          ind_gtblacttst_buckets_n;          // number of elements in bucket array
    i32                          ind_gtblacttst_n;                  // number of elements in the hash table
    algo_lib::FTempfile          tempfile;                          // Temp file to download command output
    atf_gcli::FGtblacttstout**   ind_gtblacttstout_buckets_elems;   // pointer to bucket array
    i32                          ind_gtblacttstout_buckets_n;       // number of elements in bucket array
    i32                          ind_gtblacttstout_n;               // number of elements in the hash table
    atf_gcli::FGtblacttst**      c_gtblacttst_elems;                // array of pointers
    u32                          c_gtblacttst_n;                    // array of pointers
    u32                          c_gtblacttst_max;                  // capacity of allocated array
    atf_gcli::FGclienvsub*       gclienvsub_lary[32];               // level array
    i32                          gclienvsub_n;                      // number of elements in array
    atf_gcli::FGclienv*          gclienv_lary[32];                  // level array
    i32                          gclienv_n;                         // number of elements in array
    atf_gcli::FGclienv**         ind_gclienv_buckets_elems;         // pointer to bucket array
    i32                          ind_gclienv_buckets_n;             // number of elements in bucket array
    i32                          ind_gclienv_n;                     // number of elements in the hash table
    atf_gcli::FGclienvsub**      ind_gclienvsub_buckets_elems;      // pointer to bucket array
    i32                          ind_gclienvsub_buckets_n;          // number of elements in bucket array
    i32                          ind_gclienvsub_n;                  // number of elements in the hash table
    atf_gcli::FGtblact*          gtblact_lary[32];                  // level array
    i32                          gtblact_n;                         // number of elements in array
    atf_gcli::FGtblact**         ind_gtblact_buckets_elems;         // pointer to bucket array
    i32                          ind_gtblact_buckets_n;             // number of elements in bucket array
    i32                          ind_gtblact_n;                     // number of elements in the hash table
    atf_gcli::trace              trace;                             //
};
```

#### atf_gcli.FGclienv - 
<a href="#atf_gcli-fgclienv"></a>

#### atf_gcli.FGclienv Fields
<a href="#atf_gcli-fgclienv-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_gcli.FGclienv.base|[gclidb.Gclienv](/txt/ssimdb/gclidb/gclienv.md)|[Base](/txt/ssimdb/gclidb/gclienv.md)|||
|atf_gcli.FGclienv.c_gclienvsub|[atf_gcli.FGclienvsub](/txt/exe/atf_gcli/internals.md#atf_gcli-fgclienvsub)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||

#### Struct FGclienv
<a href="#struct-fgclienv"></a>
*Note:* field ``atf_gcli.FGclienv.base`` has reftype ``base`` so the fields of [gclidb.Gclienv](/txt/ssimdb/gclidb/gclienv.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_gcli_gen.h](/include/gen/atf_gcli_gen.h)
```
struct FGclienv { // atf_gcli.FGclienv
    atf_gcli::FGclienv*       ind_gclienv_next;      // hash next
    u32                       ind_gclienv_hashval;   // hash value
    algo::Smallstr50          gclienv;               //
    algo::cstring             addon;                 //
    algo::Comment             comment;               //
    atf_gcli::FGclienvsub**   c_gclienvsub_elems;    // array of pointers
    u32                       c_gclienvsub_n;        // array of pointers
    u32                       c_gclienvsub_max;      // capacity of allocated array
    // reftype Ptrary of atf_gcli.FGclienv.c_gclienvsub prohibits copy
    // func:atf_gcli.FGclienv..AssignOp
    inline atf_gcli::FGclienv& operator =(const atf_gcli::FGclienv &rhs) = delete;
    // reftype Ptrary of atf_gcli.FGclienv.c_gclienvsub prohibits copy
    // func:atf_gcli.FGclienv..CopyCtor
    inline               FGclienv(const atf_gcli::FGclienv &rhs) = delete;
private:
    // func:atf_gcli.FGclienv..Ctor
    inline               FGclienv() __attribute__((nothrow));
    // func:atf_gcli.FGclienv..Dtor
    inline               ~FGclienv() __attribute__((nothrow));
    friend atf_gcli::FGclienv&  gclienv_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend atf_gcli::FGclienv*  gclienv_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 gclienv_RemoveAll() __attribute__((nothrow));
    friend void                 gclienv_RemoveLast() __attribute__((nothrow));
};
```

#### atf_gcli.FGclienvsub - 
<a href="#atf_gcli-fgclienvsub"></a>

#### atf_gcli.FGclienvsub Fields
<a href="#atf_gcli-fgclienvsub-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_gcli.FGclienvsub.base|[gclidb.Gclienvsub](/txt/ssimdb/gclidb/gclienvsub.md)|[Base](/txt/ssimdb/gclidb/gclienvsub.md)|||

#### Struct FGclienvsub
<a href="#struct-fgclienvsub"></a>
*Note:* field ``atf_gcli.FGclienvsub.base`` has reftype ``base`` so the fields of [gclidb.Gclienvsub](/txt/ssimdb/gclidb/gclienvsub.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_gcli_gen.h](/include/gen/atf_gcli_gen.h)
```
struct FGclienvsub { // atf_gcli.FGclienvsub
    atf_gcli::FGclienvsub*   ind_gclienvsub_next;           // hash next
    u32                      ind_gclienvsub_hashval;        // hash value
    algo::Smallstr50         gclienvsub;                    //
    bool                     fwd;                           //   false
    bool                     rev;                           //   false
    algo::cstring            uval;                          //
    bool                     gclienv_c_gclienvsub_in_ary;   //   false  membership flag
    // func:atf_gcli.FGclienvsub..AssignOp
    inline atf_gcli::FGclienvsub& operator =(const atf_gcli::FGclienvsub &rhs) = delete;
    // func:atf_gcli.FGclienvsub..CopyCtor
    inline               FGclienvsub(const atf_gcli::FGclienvsub &rhs) = delete;
private:
    // func:atf_gcli.FGclienvsub..Ctor
    inline               FGclienvsub() __attribute__((nothrow));
    // func:atf_gcli.FGclienvsub..Dtor
    inline               ~FGclienvsub() __attribute__((nothrow));
    friend atf_gcli::FGclienvsub& gclienvsub_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend atf_gcli::FGclienvsub* gclienvsub_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 gclienvsub_RemoveAll() __attribute__((nothrow));
    friend void                 gclienvsub_RemoveLast() __attribute__((nothrow));
};
```

#### atf_gcli.FGtblact - static combination of gcli tables and actions
<a href="#atf_gcli-fgtblact"></a>

#### atf_gcli.FGtblact Fields
<a href="#atf_gcli-fgtblact-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_gcli.FGtblact.base|[gclidb.Gtblact](/txt/ssimdb/gclidb/gtblact.md)|[Base](/txt/ssimdb/gclidb/gtblact.md)|||
|atf_gcli.FGtblact.glab_fail|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_gcli.FGtblact.glab_run|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_gcli.FGtblact.ghub_run|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_gcli.FGtblact.ghub_fail|u32|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FGtblact
<a href="#struct-fgtblact"></a>
*Note:* field ``atf_gcli.FGtblact.base`` has reftype ``base`` so the fields of [gclidb.Gtblact](/txt/ssimdb/gclidb/gtblact.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_gcli_gen.h](/include/gen/atf_gcli_gen.h)
```
struct FGtblact { // atf_gcli.FGtblact
    atf_gcli::FGtblact*   ind_gtblact_next;      // hash next
    u32                   ind_gtblact_hashval;   // hash value
    algo::Smallstr50      gtblact;               //
    bool                  t;                     //   false
    bool                  e;                     //   false
    bool                  auth;                  //   false
    u32                   glab_fail;             //   0
    u32                   glab_run;              //   0
    u32                   ghub_run;              //   0
    u32                   ghub_fail;             //   0
    // func:atf_gcli.FGtblact..AssignOp
    atf_gcli::FGtblact&  operator =(const atf_gcli::FGtblact &rhs) = delete;
    // func:atf_gcli.FGtblact..CopyCtor
    FGtblact(const atf_gcli::FGtblact &rhs) = delete;
private:
    // func:atf_gcli.FGtblact..Ctor
    inline               FGtblact() __attribute__((nothrow));
    // func:atf_gcli.FGtblact..Dtor
    inline               ~FGtblact() __attribute__((nothrow));
    friend atf_gcli::FGtblact&  gtblact_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend atf_gcli::FGtblact*  gtblact_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 gtblact_RemoveAll() __attribute__((nothrow));
    friend void                 gtblact_RemoveLast() __attribute__((nothrow));
};
```

#### atf_gcli.FGtblacttst - 
<a href="#atf_gcli-fgtblacttst"></a>

#### atf_gcli.FGtblacttst Fields
<a href="#atf_gcli-fgtblacttst-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_gcli.FGtblacttst.base|[gclidb.Gtblacttst](/txt/ssimdb/gclidb/gtblacttst.md)|[Base](/txt/ssimdb/gclidb/gtblacttst.md)|||
|atf_gcli.FGtblacttst.fail|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_gcli.FGtblacttst.c_gtblacttstout|[atf_gcli.FGtblacttstout](/txt/exe/atf_gcli/internals.md#atf_gcli-fgtblacttstout)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|atf_gcli.FGtblacttst.select|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_gcli.FGtblacttst.p_gclienv|[atf_gcli.FGclienv](/txt/exe/atf_gcli/internals.md#atf_gcli-fgclienv)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|atf_gcli.FGtblacttst.p_gtblact|[atf_gcli.FGtblact](/txt/exe/atf_gcli/internals.md#atf_gcli-fgtblact)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FGtblacttst
<a href="#struct-fgtblacttst"></a>
*Note:* field ``atf_gcli.FGtblacttst.base`` has reftype ``base`` so the fields of [gclidb.Gtblacttst](/txt/ssimdb/gclidb/gtblacttst.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_gcli_gen.h](/include/gen/atf_gcli_gen.h)
```
struct FGtblacttst { // atf_gcli.FGtblacttst
    atf_gcli::FGtblacttst*       ind_gtblacttst_next;      // hash next
    u32                          ind_gtblacttst_hashval;   // hash value
    algo::Smallstr250            gtblacttst;               //
    bool                         working;                  //   true
    algo::cstring                cmd;                      //
    algo::Comment                comment;                  //
    bool                         fail;                     //   false
    atf_gcli::FGtblacttstout**   c_gtblacttstout_elems;    // array of pointers
    u32                          c_gtblacttstout_n;        // array of pointers
    u32                          c_gtblacttstout_max;      // capacity of allocated array
    bool                         select;                   //   false
    atf_gcli::FGclienv*          p_gclienv;                // reference to parent row
    atf_gcli::FGtblact*          p_gtblact;                // reference to parent row
    bool                         c_gtblacttst_in_ary;      //   false  membership flag
    // reftype Ptrary of atf_gcli.FGtblacttst.c_gtblacttstout prohibits copy
    // x-reference on atf_gcli.FGtblacttst.p_gclienv prevents copy
    // x-reference on atf_gcli.FGtblacttst.p_gtblact prevents copy
    // func:atf_gcli.FGtblacttst..AssignOp
    atf_gcli::FGtblacttst& operator =(const atf_gcli::FGtblacttst &rhs) = delete;
    // reftype Ptrary of atf_gcli.FGtblacttst.c_gtblacttstout prohibits copy
    // x-reference on atf_gcli.FGtblacttst.p_gclienv prevents copy
    // x-reference on atf_gcli.FGtblacttst.p_gtblact prevents copy
    // func:atf_gcli.FGtblacttst..CopyCtor
    FGtblacttst(const atf_gcli::FGtblacttst &rhs) = delete;
private:
    // func:atf_gcli.FGtblacttst..Ctor
    inline               FGtblacttst() __attribute__((nothrow));
    // func:atf_gcli.FGtblacttst..Dtor
    inline               ~FGtblacttst() __attribute__((nothrow));
    friend atf_gcli::FGtblacttst& gtblacttst_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend atf_gcli::FGtblacttst* gtblacttst_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 gtblacttst_RemoveAll() __attribute__((nothrow));
    friend void                 gtblacttst_RemoveLast() __attribute__((nothrow));
};
```

#### atf_gcli.FGtblacttstout - 
<a href="#atf_gcli-fgtblacttstout"></a>

#### atf_gcli.FGtblacttstout Fields
<a href="#atf_gcli-fgtblacttstout-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_gcli.FGtblacttstout.base|[gclidb.Gtblacttstout](/txt/ssimdb/gclidb/gtblacttstout.md)|[Base](/txt/ssimdb/gclidb/gtblacttstout.md)|||
|atf_gcli.FGtblacttstout.tout|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FGtblacttstout
<a href="#struct-fgtblacttstout"></a>
*Note:* field ``atf_gcli.FGtblacttstout.base`` has reftype ``base`` so the fields of [gclidb.Gtblacttstout](/txt/ssimdb/gclidb/gtblacttstout.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_gcli_gen.h](/include/gen/atf_gcli_gen.h)
```
struct FGtblacttstout { // atf_gcli.FGtblacttstout
    atf_gcli::FGtblacttstout*   ind_gtblacttstout_next;              // hash next
    u32                         ind_gtblacttstout_hashval;           // hash value
    algo::Smallstr250           gtblacttstout;                       //
    algo::cstring               text;                                //
    algo::cstring               tout;                                //
    bool                        gtblacttst_c_gtblacttstout_in_ary;   //   false  membership flag
    // func:atf_gcli.FGtblacttstout..AssignOp
    inline atf_gcli::FGtblacttstout& operator =(const atf_gcli::FGtblacttstout &rhs) = delete;
    // func:atf_gcli.FGtblacttstout..CopyCtor
    inline               FGtblacttstout(const atf_gcli::FGtblacttstout &rhs) = delete;
private:
    // func:atf_gcli.FGtblacttstout..Ctor
    inline               FGtblacttstout() __attribute__((nothrow));
    // func:atf_gcli.FGtblacttstout..Dtor
    inline               ~FGtblacttstout() __attribute__((nothrow));
    friend atf_gcli::FGtblacttstout& gtblacttstout_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend atf_gcli::FGtblacttstout* gtblacttstout_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 gtblacttstout_RemoveAll() __attribute__((nothrow));
    friend void                 gtblacttstout_RemoveLast() __attribute__((nothrow));
};
```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Imdb -->


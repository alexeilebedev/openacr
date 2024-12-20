## aqlite - Internals


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Toc -->
* [Description](#description)
* [Sources](#sources)
* [Dependencies](#dependencies)
* [In Memory DB](#in-memory-db)
* [Tests](#tests)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Toc -->

### Description
<a href="#description"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Description -->
for usage, see [aqlite - Runs sqlite queries against ssim files](/txt/exe/aqlite/README.md)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Description -->

### Sources
<a href="#sources"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Sources -->
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/aqlite/aqlite.cpp](/cpp/aqlite/aqlite.cpp)||
|[cpp/gen/aqlite_gen.cpp](/cpp/gen/aqlite_gen.cpp)||
|[include/aqlite.h](/include/aqlite.h)||
|[include/gen/aqlite_gen.h](/include/gen/aqlite_gen.h)||
|[include/gen/aqlite_gen.inl.h](/include/gen/aqlite_gen.inl.h)||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Sources -->

### Dependencies
<a href="#dependencies"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Dependencies -->
The build target depends on the following libraries
|Target|Comment|
|---|---|
|[algo_lib](/txt/lib/algo_lib/README.md)|Support library for all executables|
|[lib_prot](/txt/lib/lib_prot/README.md)|Library covering all protocols|
|[lib_sqlite](/txt/lib/lib_sqlite/README.md)||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Dependencies -->

### In Memory DB
<a href="#in-memory-db"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Imdb -->
`aqlite` generated code creates the tables below.
All allocations are done through global `aqlite::_db` [aqlite.FDb](#aqlite-fdb) structure
|Ctype|Ssimfile|Create|Access|
|---|---|---|---|
|[aqlite.FDb](#aqlite-fdb)||FDb._db (Global)|
|[aqlite.FNs](#aqlite-fns)|[dmmeta.ns](/txt/ssimdb/dmmeta/ns.md)|FDb.ns (Lary)|ns (Lary, by rowid)|ind_ns (Thash, hash field ns)|

#### aqlite.FDb - 
<a href="#aqlite-fdb"></a>

#### aqlite.FDb Fields
<a href="#aqlite-fdb-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|aqlite.FDb._db|[aqlite.FDb](/txt/exe/aqlite/internals.md#aqlite-fdb)|[Global](/txt/exe/amc/reftypes.md#global)|||
|aqlite.FDb.cmdline|[command.aqlite](/txt/protocol/command/README.md#command-aqlite)|[Val](/txt/exe/amc/reftypes.md#val)|||
|aqlite.FDb.ns|[aqlite.FNs](/txt/exe/aqlite/internals.md#aqlite-fns)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|aqlite.FDb.ind_ns|[aqlite.FNs](/txt/exe/aqlite/internals.md#aqlite-fns)|[Thash](/txt/exe/amc/reftypes.md#thash)|||

#### Struct FDb
<a href="#struct-fdb"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/aqlite_gen.h](/include/gen/aqlite_gen.h)
```
struct FDb { // aqlite.FDb
    command::aqlite   cmdline;                //
    aqlite::FNs*      ns_lary[32];            // level array
    i32               ns_n;                   // number of elements in array
    aqlite::FNs**     ind_ns_buckets_elems;   // pointer to bucket array
    i32               ind_ns_buckets_n;       // number of elements in bucket array
    i32               ind_ns_n;               // number of elements in the hash table
    aqlite::trace     trace;                  //
};
```

#### aqlite.FNs - Namespace (for in-memory database, protocol, etc)
<a href="#aqlite-fns"></a>

#### aqlite.FNs Fields
<a href="#aqlite-fns-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|aqlite.FNs.base|[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)|[Base](/txt/ssimdb/dmmeta/ns.md)|||
|aqlite.FNs.select|bool|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FNs
<a href="#struct-fns"></a>
*Note:* field ``aqlite.FNs.base`` has reftype ``base`` so the fields of [dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/aqlite_gen.h](/include/gen/aqlite_gen.h)
```
struct FNs { // aqlite.FNs
    aqlite::FNs*       ind_ns_next;   // hash next
    algo::Smallstr16   ns;            // Namespace name (primary key)
    algo::Smallstr50   nstype;        // Namespace type
    algo::Smallstr50   license;       // Associated license
    algo::Comment      comment;       //
    bool               select;        //   false
    // func:aqlite.FNs..AssignOp
    inline aqlite::FNs&  operator =(const aqlite::FNs &rhs) = delete;
    // func:aqlite.FNs..CopyCtor
    inline               FNs(const aqlite::FNs &rhs) = delete;
private:
    // func:aqlite.FNs..Ctor
    inline               FNs() __attribute__((nothrow));
    // func:aqlite.FNs..Dtor
    inline               ~FNs() __attribute__((nothrow));
    friend aqlite::FNs&         ns_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend aqlite::FNs*         ns_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 ns_RemoveAll() __attribute__((nothrow));
    friend void                 ns_RemoveLast() __attribute__((nothrow));
};
```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Imdb -->

### Tests
<a href="#tests"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Tests -->
The following component tests are defined for `aqlite`.
These can be executed with `atf_comp <comptest> -v`
|Comptest|Comment|
|---|---|
|[aqlite.Number](/test/atf_comp/aqlite.Number)||
|[aqlite.Smoke](/test/atf_comp/aqlite.Smoke)||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Tests -->


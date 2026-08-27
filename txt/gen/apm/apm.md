## apm - Internals


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- abt_md.toc_beg -->
&nbsp;&nbsp;&bull;&nbsp;  [Description](#description)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Sources](#sources)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Dependencies](#dependencies)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [In Memory DB](#in-memory-db)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Tests](#tests)<br/>
<!-- abt_md.toc_end -->

### Description
<a href="#description"></a>
for usage, see [apm - Algo Package Manager](/txt/exe/apm/README.md)

### Sources
<a href="#sources"></a>
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/apm/annotate.cpp](/cpp/apm/annotate.cpp)||
|[cpp/apm/check.cpp](/cpp/apm/check.cpp)||
|[cpp/apm/diff.cpp](/cpp/apm/diff.cpp)||
|[cpp/apm/install.cpp](/cpp/apm/install.cpp)||
|[cpp/apm/main.cpp](/cpp/apm/main.cpp)||
|[cpp/apm/push.cpp](/cpp/apm/push.cpp)||
|[cpp/apm/rec.cpp](/cpp/apm/rec.cpp)||
|[cpp/apm/remove.cpp](/cpp/apm/remove.cpp)||
|[cpp/apm/reset.cpp](/cpp/apm/reset.cpp)||
|[cpp/apm/show.cpp](/cpp/apm/show.cpp)||
|[cpp/apm/update.cpp](/cpp/apm/update.cpp)||
|[cpp/gen/apm_gen.cpp](/cpp/gen/apm_gen.cpp)||
|[include/apm.h](/include/apm.h)||
|[include/gen/apm_gen.h](/include/gen/apm_gen.h)||
|[include/gen/apm_gen.inl.h](/include/gen/apm_gen.inl.h)||

### Dependencies
<a href="#dependencies"></a>
The build target depends on the following libraries
|Target|Comment|
|---|---|
|[algo_lib](/txt/lib/algo_lib/README.md)|Support library for all executables|
|[lib_ctype](/txt/lib/lib_ctype/README.md)|Helpful library for reading/writing ctypes as text tuples|
|[lib_git](/txt/lib/lib_git/README.md)|Helpful git wrappers|
|[lib_prot](/txt/lib/lib_prot/README.md)|Library covering all protocols|

### In Memory DB
<a href="#in-memory-db"></a>
`apm` generated code creates the tables below.
All allocations are done through global `apm::_db` [apm.FDb](#apm-fdb) structure
|Ctype|Ssimfile|Create|Access|
|---|---|---|---|
|[apm.FBltin](#apm-fbltin)|[amcdb.bltin](/txt/ssimdb/amcdb/bltin.md)|FDb.bltin (Lary)|bltin (Lary, by rowid)|
||||FCtype.c_bltin (Ptr)|
|[apm.FCtype](#apm-fctype)|[dmmeta.ctype](/txt/ssimdb/dmmeta/ctype.md)|FDb.ctype (Lary)|ctype (Lary, by rowid)|ind_ctype (Thash, hash field ctype)|
||||FField.p_ctype (Upptr)|
||||FField.p_arg (Upptr)|
||||FSsimfile.p_ctype (Upptr)|
||||FSsimreq.p_ctype (Upptr)|
|[apm.FDb](#apm-fdb)||FDb._db (Global)|
|[apm.FField](#apm-ffield)|[dmmeta.field](/txt/ssimdb/dmmeta/field.md)|FDb.field (Lary)|field (Lary, by rowid)|ind_field (Thash, hash field field)|
||||FCtype.c_field (Ptrary)|
||||FSsimreq.p_field (Upptr)|
||||FSubstr.p_srcfield (Upptr)|
|[apm.FMergefile](#apm-fmergefile)||FDb.mergefile (Lary)|mergefile (Lary, by rowid)|ind_mergefile (Thash, hash field mergefile)|
|[apm.FMkdir](#apm-fmkdir)||FDb.mkdir (Lary)|mkdir (Lary, by rowid)|ind_mkdir (Thash, hash field mkdir)|
|[apm.FNs](#apm-fns)|[dmmeta.ns](/txt/ssimdb/dmmeta/ns.md)|FDb.ns (Lary)|ns (Lary, by rowid)|
|[apm.FPackage](#apm-fpackage)|[dev.package](/txt/ssimdb/dev/package.md)|FDb.package (Lary)|package (Lary, by rowid)|zd_sel_package (Llist)|zd_temp_package (Llist)|ind_package (Thash, hash field package)|zd_topo_package (Llist)|
||||FPkgdep.p_parent (Upptr)|
||||FPkgdep.p_package (Upptr)|
||||FPkgrec.p_package (Upptr)|
|[apm.FPkgdep](#apm-fpkgdep)|[dev.pkgdep](/txt/ssimdb/dev/pkgdep.md)|FDb.pkgdep (Lary)|pkgdep (Lary, by rowid)|
||||FPackage.c_pkgdep (Ptrary)|
||||FPackage.c_pkgdep_parent (Ptrary)|
|[apm.FPkgkey](#apm-fpkgkey)|[dev.pkgkey](/txt/ssimdb/dev/pkgkey.md)|FDb.pkgkey (Lary)|pkgkey (Lary, by rowid)|ind_pkgkey (Thash, hash field pkgkey)|
||||FPackage.zd_pkgkey (Llist)|
||||FPkgrec.p_pkgkey (Upptr)|
|[apm.FPkgrec](#apm-fpkgrec)||FDb.pkgrec (Tpool)|
||||FPackage.zd_pkgrec (Llist)|
||||FPkgkey.c_pkgrec (Ptrary)|
||||FRec.zd_rec_pkgrec (Llist)|
|[apm.FRec](#apm-frec)||FDb.rec (Tpool)|ind_rec (Thash, hash field rec)|c_rec (Ptrary)|zd_rec (Llist)|zd_selrec (Llist)|zd_chooserec (Llist)|
||||FPkgrec.p_rec (Upptr)|
||||FRec.c_parent (Ptrary)|
||||FRec.c_child (Ptrary)|
||||FSsimfile.zd_ssimfile_rec (Llist)|
|[apm.FSsimfile](#apm-fssimfile)|[dmmeta.ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|FDb.ssimfile (Lary)|ssimfile (Lary, by rowid)|ind_ssimfile (Thash, hash field ssimfile)|
||||FCtype.c_ssimfile (Ptr)|
||||FRec.p_ssimfile (Upptr)|
||||FSsimreq.p_child_ssimfile (Upptr)|
|[apm.FSsimreq](#apm-fssimreq)|[dmmeta.ssimreq](/txt/ssimdb/dmmeta/ssimreq.md)|FDb.ssimreq (Lary)|ssimreq (Lary, by rowid)|
||||FCtype.c_ssimreq (Ptrary)|
|[apm.FSsimsort](#apm-fssimsort)|[dmmeta.ssimsort](/txt/ssimdb/dmmeta/ssimsort.md)|FDb.ssimsort (Lary)|ssimsort (Lary, by rowid)|ind_ssimsort (Thash, hash field ssimfile)|
||||FSsimfile.c_ssimsort (Ptr)|
|[apm.FSubstr](#apm-fsubstr)|[dmmeta.substr](/txt/ssimdb/dmmeta/substr.md)|FDb.substr (Lary)|substr (Lary, by rowid)|
||||FField.c_substr (Ptr)|

#### apm.FBltin - Specify properties of a C built-in type
<a href="#apm-fbltin"></a>

#### apm.FBltin Fields
<a href="#apm-fbltin-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|apm.FBltin.base|[amcdb.Bltin](/txt/ssimdb/amcdb/bltin.md)|[Base](/txt/ssimdb/amcdb/bltin.md)|||

#### Struct FBltin
<a href="#struct-fbltin"></a>
*Note:* field ``apm.FBltin.base`` has reftype ``base`` so the fields of [amcdb.Bltin](/txt/ssimdb/amcdb/bltin.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/apm_gen.h](/include/gen/apm_gen.h)
```
struct FBltin { // apm.FBltin
    algo::Smallstr100   ctype;      //
    bool                likeu64;    //   false
    bool                bigendok;   //   false
    bool                issigned;   //   false
    algo::cstring       comment;    //
    // func:apm.FBltin..AssignOp
    inline apm::FBltin&  operator =(const apm::FBltin &rhs) = delete;
    // func:apm.FBltin..CopyCtor
    inline               FBltin(const apm::FBltin &rhs) = delete;
private:
    // func:apm.FBltin..Ctor
    inline               FBltin() __attribute__((nothrow));
    // func:apm.FBltin..Dtor
    inline               ~FBltin() __attribute__((nothrow));
    friend apm::FBltin&         bltin_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend apm::FBltin*         bltin_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 bltin_RemoveAll() __attribute__((nothrow));
    friend void                 bltin_RemoveLast() __attribute__((nothrow));
};
```

#### apm.FCtype - Struct
<a href="#apm-fctype"></a>

#### apm.FCtype Fields
<a href="#apm-fctype-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|apm.FCtype.base|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Base](/txt/ssimdb/dmmeta/ctype.md)|||
|apm.FCtype.c_ssimfile|[apm.FSsimfile](/txt/gen/apm/apm.md#apm-fssimfile)|[Ptr](/txt/exe/amc/reftype/Ptr.md)|||
|apm.FCtype.c_field|[apm.FField](/txt/gen/apm/apm.md#apm-ffield)|[Ptrary](/txt/exe/amc/reftype/Ptrary.md)|||
|apm.FCtype.c_ssimreq|[apm.FSsimreq](/txt/gen/apm/apm.md#apm-fssimreq)|[Ptrary](/txt/exe/amc/reftype/Ptrary.md)|||
|apm.FCtype.c_bltin|[apm.FBltin](/txt/gen/apm/apm.md#apm-fbltin)|[Ptr](/txt/exe/amc/reftype/Ptr.md)|||

#### Struct FCtype
<a href="#struct-fctype"></a>
*Note:* field ``apm.FCtype.base`` has reftype ``base`` so the fields of [dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/apm_gen.h](/include/gen/apm_gen.h)
```
struct FCtype { // apm.FCtype
    algo::Smallstr100   ctype;               // Identifier. must be ns.typename
    algo::cstring       comment;             //
    apm::FSsimfile*     c_ssimfile;          // optional pointer
    apm::FField**       c_field_elems;       // array of pointers
    u64                 c_field_n;           // current size
    u64                 c_field_max;         // capacity of allocated array
    apm::FSsimreq**     c_ssimreq_elems;     // array of pointers
    u64                 c_ssimreq_n;         // current size
    u64                 c_ssimreq_max;       // capacity of allocated array
    apm::FBltin*        c_bltin;             // optional pointer
    apm::FCtype*        ind_ctype_next;      // hash next
    u32                 ind_ctype_hashval;   // hash value
    // x-reference on apm.FCtype.c_ssimfile prevents copy
    // reftype Ptrary of apm.FCtype.c_field prohibits copy
    // reftype Ptrary of apm.FCtype.c_ssimreq prohibits copy
    // x-reference on apm.FCtype.c_bltin prevents copy
    // func:apm.FCtype..AssignOp
    inline apm::FCtype&  operator =(const apm::FCtype &rhs) = delete;
    // x-reference on apm.FCtype.c_ssimfile prevents copy
    // reftype Ptrary of apm.FCtype.c_field prohibits copy
    // reftype Ptrary of apm.FCtype.c_ssimreq prohibits copy
    // x-reference on apm.FCtype.c_bltin prevents copy
    // func:apm.FCtype..CopyCtor
    inline               FCtype(const apm::FCtype &rhs) = delete;
private:
    // func:apm.FCtype..Ctor
    inline               FCtype() __attribute__((nothrow));
    // func:apm.FCtype..Dtor
    inline               ~FCtype() __attribute__((nothrow));
    friend apm::FCtype&         ctype_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend apm::FCtype*         ctype_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 ctype_RemoveAll() __attribute__((nothrow));
    friend void                 ctype_RemoveLast() __attribute__((nothrow));
};
```

#### apm.FDb - In-memory database for apm
<a href="#apm-fdb"></a>

#### apm.FDb Fields
<a href="#apm-fdb-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|apm.FDb._db|[apm.FDb](/txt/gen/apm/apm.md#apm-fdb)|[Global](/txt/exe/amc/reftype/Global.md)|||
|apm.FDb.cmdline|[command.apm](/txt/protocol/command/README.md#command-apm)|[Val](/txt/exe/amc/reftype/Val.md)|||
|apm.FDb.package|[apm.FPackage](/txt/gen/apm/apm.md#apm-fpackage)|[Lary](/txt/exe/amc/reftype/Lary.md)|||
|apm.FDb.zd_sel_package|[apm.FPackage](/txt/gen/apm/apm.md#apm-fpackage)|[Llist](/txt/exe/amc/reftype/Llist.md)|||
|apm.FDb.zd_temp_package|[apm.FPackage](/txt/gen/apm/apm.md#apm-fpackage)|[Llist](/txt/exe/amc/reftype/Llist.md)|||
|apm.FDb.script|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)|||
|apm.FDb.scriptfile|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)|||
|apm.FDb.ind_package|[apm.FPackage](/txt/gen/apm/apm.md#apm-fpackage)|[Thash](/txt/exe/amc/reftype/Thash.md)|||
|apm.FDb.pkgkey|[apm.FPkgkey](/txt/gen/apm/apm.md#apm-fpkgkey)|[Lary](/txt/exe/amc/reftype/Lary.md)|||
|apm.FDb.pkgdep|[apm.FPkgdep](/txt/gen/apm/apm.md#apm-fpkgdep)|[Lary](/txt/exe/amc/reftype/Lary.md)|||
|apm.FDb.mergefile|[apm.FMergefile](/txt/gen/apm/apm.md#apm-fmergefile)|[Lary](/txt/exe/amc/reftype/Lary.md)|||
|apm.FDb.ind_mergefile|[apm.FMergefile](/txt/gen/apm/apm.md#apm-fmergefile)|[Thash](/txt/exe/amc/reftype/Thash.md)|||
|apm.FDb.ind_pkgkey|[apm.FPkgkey](/txt/gen/apm/apm.md#apm-fpkgkey)|[Thash](/txt/exe/amc/reftype/Thash.md)|||
|apm.FDb.base_sandbox|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)||Sandbox name for 'base' files|
|apm.FDb.theirs_sandbox|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)||Sandbox name for 'theirs' files|
|apm.FDb.ours_recfile|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)|||
|apm.FDb.theirs_recfile|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)|||
|apm.FDb.base_recfile|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)|||
|apm.FDb.ctype|[apm.FCtype](/txt/gen/apm/apm.md#apm-fctype)|[Lary](/txt/exe/amc/reftype/Lary.md)|||
|apm.FDb.ind_ctype|[apm.FCtype](/txt/gen/apm/apm.md#apm-fctype)|[Thash](/txt/exe/amc/reftype/Thash.md)|||
|apm.FDb.field|[apm.FField](/txt/gen/apm/apm.md#apm-ffield)|[Lary](/txt/exe/amc/reftype/Lary.md)|||
|apm.FDb.ind_field|[apm.FField](/txt/gen/apm/apm.md#apm-ffield)|[Thash](/txt/exe/amc/reftype/Thash.md)|||
|apm.FDb.ssimfile|[apm.FSsimfile](/txt/gen/apm/apm.md#apm-fssimfile)|[Lary](/txt/exe/amc/reftype/Lary.md)|||
|apm.FDb.ind_ssimfile|[apm.FSsimfile](/txt/gen/apm/apm.md#apm-fssimfile)|[Thash](/txt/exe/amc/reftype/Thash.md)|||
|apm.FDb.rec|[apm.FRec](/txt/gen/apm/apm.md#apm-frec)|[Tpool](/txt/exe/amc/reftype/Tpool.md)|||
|apm.FDb.ind_rec|[apm.FRec](/txt/gen/apm/apm.md#apm-frec)|[Thash](/txt/exe/amc/reftype/Thash.md)|||
|apm.FDb.c_rec|[apm.FRec](/txt/gen/apm/apm.md#apm-frec)|[Ptrary](/txt/exe/amc/reftype/Ptrary.md)||Temporary array for sorting|
|apm.FDb.zd_rec|[apm.FRec](/txt/gen/apm/apm.md#apm-frec)|[Llist](/txt/exe/amc/reftype/Llist.md)|||
|apm.FDb.zd_selrec|[apm.FRec](/txt/gen/apm/apm.md#apm-frec)|[Llist](/txt/exe/amc/reftype/Llist.md)|||
|apm.FDb.zd_chooserec|[apm.FRec](/txt/gen/apm/apm.md#apm-frec)|[Llist](/txt/exe/amc/reftype/Llist.md)|||
|apm.FDb.substr|[apm.FSubstr](/txt/gen/apm/apm.md#apm-fsubstr)|[Lary](/txt/exe/amc/reftype/Lary.md)|||
|apm.FDb.pkgrec|[apm.FPkgrec](/txt/gen/apm/apm.md#apm-fpkgrec)|[Tpool](/txt/exe/amc/reftype/Tpool.md)|||
|apm.FDb.zd_topo_package|[apm.FPackage](/txt/gen/apm/apm.md#apm-fpackage)|[Llist](/txt/exe/amc/reftype/Llist.md)|||
|apm.FDb.ssimreq|[apm.FSsimreq](/txt/gen/apm/apm.md#apm-fssimreq)|[Lary](/txt/exe/amc/reftype/Lary.md)|||
|apm.FDb.merged_recfile|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)|||
|apm.FDb.mkdir|[apm.FMkdir](/txt/gen/apm/apm.md#apm-fmkdir)|[Lary](/txt/exe/amc/reftype/Lary.md)|||
|apm.FDb.ind_mkdir|[apm.FMkdir](/txt/gen/apm/apm.md#apm-fmkdir)|[Thash](/txt/exe/amc/reftype/Thash.md)|||
|apm.FDb.ns|[apm.FNs](/txt/gen/apm/apm.md#apm-fns)|[Lary](/txt/exe/amc/reftype/Lary.md)|||
|apm.FDb.pkgdata_recfile|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)||File containing new package definitions|
|apm.FDb.ssimsort|[apm.FSsimsort](/txt/gen/apm/apm.md#apm-fssimsort)|[Lary](/txt/exe/amc/reftype/Lary.md)|||
|apm.FDb.ind_ssimsort|[apm.FSsimsort](/txt/gen/apm/apm.md#apm-fssimsort)|[Thash](/txt/exe/amc/reftype/Thash.md)|||
|apm.FDb.bltin|[apm.FBltin](/txt/gen/apm/apm.md#apm-fbltin)|[Lary](/txt/exe/amc/reftype/Lary.md)|||

#### Struct FDb
<a href="#struct-fdb"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/apm_gen.h](/include/gen/apm_gen.h)
```
struct FDb { // apm.FDb: In-memory database for apm
    command::apm        cmdline;                       //
    apm::FPackage*      package_lary[36];              // level array
    i64                 package_n;                     // number of elements in array
    apm::FPackage*      zd_sel_package_head;           // zero-terminated doubly linked list
    i32                 zd_sel_package_n;              // zero-terminated doubly linked list
    apm::FPackage*      zd_sel_package_tail;           // pointer to last element
    apm::FPackage*      zd_temp_package_head;          // zero-terminated doubly linked list
    i32                 zd_temp_package_n;             // zero-terminated doubly linked list
    apm::FPackage*      zd_temp_package_tail;          // pointer to last element
    algo::cstring       script;                        //
    algo::cstring       scriptfile;                    //
    apm::FPackage**     ind_package_buckets_elems;     // pointer to bucket array
    i32                 ind_package_buckets_n;         // number of elements in bucket array
    i32                 ind_package_n;                 // number of elements in the hash table
    apm::FPkgkey*       pkgkey_lary[36];               // level array
    i64                 pkgkey_n;                      // number of elements in array
    apm::FPkgdep*       pkgdep_lary[36];               // level array
    i64                 pkgdep_n;                      // number of elements in array
    apm::FMergefile*    mergefile_lary[36];            // level array
    i64                 mergefile_n;                   // number of elements in array
    apm::FMergefile**   ind_mergefile_buckets_elems;   // pointer to bucket array
    i32                 ind_mergefile_buckets_n;       // number of elements in bucket array
    i32                 ind_mergefile_n;               // number of elements in the hash table
    apm::FPkgkey**      ind_pkgkey_buckets_elems;      // pointer to bucket array
    i32                 ind_pkgkey_buckets_n;          // number of elements in bucket array
    i32                 ind_pkgkey_n;                  // number of elements in the hash table
    algo::cstring       base_sandbox;                  // Sandbox name for 'base' files
    algo::cstring       theirs_sandbox;                // Sandbox name for 'theirs' files
    algo::cstring       ours_recfile;                  //
    algo::cstring       theirs_recfile;                //
    algo::cstring       base_recfile;                  //
    apm::FCtype*        ctype_lary[36];                // level array
    i64                 ctype_n;                       // number of elements in array
    apm::FCtype**       ind_ctype_buckets_elems;       // pointer to bucket array
    i32                 ind_ctype_buckets_n;           // number of elements in bucket array
    i32                 ind_ctype_n;                   // number of elements in the hash table
    apm::FField*        field_lary[36];                // level array
    i64                 field_n;                       // number of elements in array
    apm::FField**       ind_field_buckets_elems;       // pointer to bucket array
    i32                 ind_field_buckets_n;           // number of elements in bucket array
    i32                 ind_field_n;                   // number of elements in the hash table
    apm::FSsimfile*     ssimfile_lary[36];             // level array
    i64                 ssimfile_n;                    // number of elements in array
    apm::FSsimfile**    ind_ssimfile_buckets_elems;    // pointer to bucket array
    i32                 ind_ssimfile_buckets_n;        // number of elements in bucket array
    i32                 ind_ssimfile_n;                // number of elements in the hash table
    u64                 rec_blocksize;                 // # bytes per block
    apm::FRec*          rec_free;                      //
    apm::FRec**         ind_rec_buckets_elems;         // pointer to bucket array
    i32                 ind_rec_buckets_n;             // number of elements in bucket array
    i32                 ind_rec_n;                     // number of elements in the hash table
    apm::FRec**         c_rec_elems;                   // array of pointers
    u64                 c_rec_n;                       // current size
    u64                 c_rec_max;                     // capacity of allocated array
    apm::FRec*          zd_rec_head;                   // zero-terminated doubly linked list
    i32                 zd_rec_n;                      // zero-terminated doubly linked list
    apm::FRec*          zd_rec_tail;                   // pointer to last element
    apm::FRec*          zd_selrec_head;                // zero-terminated doubly linked list
    i32                 zd_selrec_n;                   // zero-terminated doubly linked list
    apm::FRec*          zd_selrec_tail;                // pointer to last element
    apm::FRec*          zd_chooserec_head;             // zero-terminated doubly linked list
    i32                 zd_chooserec_n;                // zero-terminated doubly linked list
    apm::FRec*          zd_chooserec_tail;             // pointer to last element
    apm::FSubstr*       substr_lary[36];               // level array
    i64                 substr_n;                      // number of elements in array
    u64                 pkgrec_blocksize;              // # bytes per block
    apm::FPkgrec*       pkgrec_free;                   //
    apm::FPackage*      zd_topo_package_head;          // zero-terminated doubly linked list
    i32                 zd_topo_package_n;             // zero-terminated doubly linked list
    apm::FPackage*      zd_topo_package_tail;          // pointer to last element
    apm::FSsimreq*      ssimreq_lary[36];              // level array
    i64                 ssimreq_n;                     // number of elements in array
    algo::cstring       merged_recfile;                //
    apm::FMkdir*        mkdir_lary[36];                // level array
    i64                 mkdir_n;                       // number of elements in array
    apm::FMkdir**       ind_mkdir_buckets_elems;       // pointer to bucket array
    i32                 ind_mkdir_buckets_n;           // number of elements in bucket array
    i32                 ind_mkdir_n;                   // number of elements in the hash table
    apm::FNs*           ns_lary[36];                   // level array
    i64                 ns_n;                          // number of elements in array
    algo::cstring       pkgdata_recfile;               // File containing new package definitions
    apm::FSsimsort*     ssimsort_lary[36];             // level array
    i64                 ssimsort_n;                    // number of elements in array
    apm::FSsimsort**    ind_ssimsort_buckets_elems;    // pointer to bucket array
    i32                 ind_ssimsort_buckets_n;        // number of elements in bucket array
    i32                 ind_ssimsort_n;                // number of elements in the hash table
    apm::FBltin*        bltin_lary[36];                // level array
    i64                 bltin_n;                       // number of elements in array
    apm::trace          trace;                         //
};
```

#### apm.FField - Specify field of a struct
<a href="#apm-ffield"></a>

#### apm.FField Fields
<a href="#apm-ffield-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|apm.FField.base|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Base](/txt/ssimdb/dmmeta/field.md)|||
|apm.FField.p_ctype|[apm.FCtype](/txt/gen/apm/apm.md#apm-fctype)|[Upptr](/txt/exe/amc/reftype/Upptr.md)|||
|apm.FField.c_substr|[apm.FSubstr](/txt/gen/apm/apm.md#apm-fsubstr)|[Ptr](/txt/exe/amc/reftype/Ptr.md)|||
|apm.FField.p_arg|[apm.FCtype](/txt/gen/apm/apm.md#apm-fctype)|[Upptr](/txt/exe/amc/reftype/Upptr.md)|||

#### Struct FField
<a href="#struct-ffield"></a>
*Note:* field ``apm.FField.base`` has reftype ``base`` so the fields of [dmmeta.Field](/txt/ssimdb/dmmeta/field.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/apm_gen.h](/include/gen/apm_gen.h)
```
struct FField { // apm.FField
    apm::FField*        ind_field_next;         // hash next
    u32                 ind_field_hashval;      // hash value
    algo::Smallstr150   field;                  // Primary key, as ctype.name
    algo::Smallstr100   arg;                    // Type of field
    algo::Smallstr50    reftype;                //   "Val"  Type constructor
    algo::CppExpr       dflt;                   // Default value (c++ expression)
    algo::cstring       comment;                //
    apm::FCtype*        p_ctype;                // reference to parent row
    apm::FSubstr*       c_substr;               // optional pointer
    apm::FCtype*        p_arg;                  // reference to parent row
    bool                ctype_c_field_in_ary;   //   false  membership flag
    // x-reference on apm.FField.p_ctype prevents copy
    // x-reference on apm.FField.c_substr prevents copy
    // x-reference on apm.FField.p_arg prevents copy
    // func:apm.FField..AssignOp
    apm::FField&         operator =(const apm::FField &rhs) = delete;
    // x-reference on apm.FField.p_ctype prevents copy
    // x-reference on apm.FField.c_substr prevents copy
    // x-reference on apm.FField.p_arg prevents copy
    // func:apm.FField..CopyCtor
    FField(const apm::FField &rhs) = delete;
private:
    // func:apm.FField..Ctor
    inline               FField() __attribute__((nothrow));
    // func:apm.FField..Dtor
    inline               ~FField() __attribute__((nothrow));
    friend apm::FField&         field_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend apm::FField*         field_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 field_RemoveAll() __attribute__((nothrow));
    friend void                 field_RemoveLast() __attribute__((nothrow));
};
```

#### apm.FMergefile
<a href="#apm-fmergefile"></a>

#### apm.FMergefile Fields
<a href="#apm-fmergefile-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|apm.FMergefile.mergefile|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)|||
|apm.FMergefile.base_file|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)|||
|apm.FMergefile.base_mode|i32|[Val](/txt/exe/amc/reftype/Val.md)|||
|apm.FMergefile.ours_file|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)|||
|apm.FMergefile.ours_mode|i32|[Val](/txt/exe/amc/reftype/Val.md)|||
|apm.FMergefile.theirs_file|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)|||
|apm.FMergefile.theirs_mode|i32|[Val](/txt/exe/amc/reftype/Val.md)|||

#### Struct FMergefile
<a href="#struct-fmergefile"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/apm_gen.h](/include/gen/apm_gen.h)
```
struct FMergefile { // apm.FMergefile
    apm::FMergefile*   ind_mergefile_next;      // hash next
    u32                ind_mergefile_hashval;   // hash value
    algo::cstring      mergefile;               //
    algo::cstring      base_file;               //
    i32                base_mode;               //   0
    algo::cstring      ours_file;               //
    i32                ours_mode;               //   0
    algo::cstring      theirs_file;             //
    i32                theirs_mode;             //   0
    // func:apm.FMergefile..AssignOp
    inline apm::FMergefile& operator =(const apm::FMergefile &rhs) = delete;
    // func:apm.FMergefile..CopyCtor
    inline               FMergefile(const apm::FMergefile &rhs) = delete;
private:
    // func:apm.FMergefile..Ctor
    inline               FMergefile() __attribute__((nothrow));
    // func:apm.FMergefile..Dtor
    inline               ~FMergefile() __attribute__((nothrow));
    friend apm::FMergefile&     mergefile_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend apm::FMergefile*     mergefile_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 mergefile_RemoveAll() __attribute__((nothrow));
    friend void                 mergefile_RemoveLast() __attribute__((nothrow));
};
```

#### apm.FMkdir
<a href="#apm-fmkdir"></a>

#### apm.FMkdir Fields
<a href="#apm-fmkdir-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|apm.FMkdir.mkdir|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

#### Struct FMkdir
<a href="#struct-fmkdir"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/apm_gen.h](/include/gen/apm_gen.h)
```
struct FMkdir { // apm.FMkdir
    apm::FMkdir*    ind_mkdir_next;      // hash next
    u32             ind_mkdir_hashval;   // hash value
    algo::cstring   mkdir;               //
    // func:apm.FMkdir..AssignOp
    inline apm::FMkdir&  operator =(const apm::FMkdir &rhs) = delete;
    // func:apm.FMkdir..CopyCtor
    inline               FMkdir(const apm::FMkdir &rhs) = delete;
private:
    // func:apm.FMkdir..Ctor
    inline               FMkdir() __attribute__((nothrow));
    // func:apm.FMkdir..Dtor
    inline               ~FMkdir() __attribute__((nothrow));
    friend apm::FMkdir&         mkdir_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend apm::FMkdir*         mkdir_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 mkdir_RemoveAll() __attribute__((nothrow));
    friend void                 mkdir_RemoveLast() __attribute__((nothrow));
};
```

#### apm.FNs - Namespace (for in-memory database, protocol, etc)
<a href="#apm-fns"></a>

#### apm.FNs Fields
<a href="#apm-fns-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|apm.FNs.base|[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)|[Base](/txt/ssimdb/dmmeta/ns.md)|||

#### Struct FNs
<a href="#struct-fns"></a>
*Note:* field ``apm.FNs.base`` has reftype ``base`` so the fields of [dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/apm_gen.h](/include/gen/apm_gen.h)
```
struct FNs { // apm.FNs
    algo::Smallstr16   ns;        // Namespace name (primary key)
    algo::Smallstr50   nstype;    // Namespace type
    algo::Smallstr50   license;   // Associated license
    algo::cstring      comment;   //
private:
    // func:apm.FNs..Ctor
    inline               FNs() __attribute__((nothrow));
    friend apm::FNs&            ns_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend apm::FNs*            ns_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 ns_RemoveAll() __attribute__((nothrow));
    friend void                 ns_RemoveLast() __attribute__((nothrow));
};
```

#### apm.FPackage - OpenACR package
<a href="#apm-fpackage"></a>

#### apm.FPackage Fields
<a href="#apm-fpackage-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|apm.FPackage.base|[dev.Package](/txt/ssimdb/dev/package.md)|[Base](/txt/ssimdb/dev/package.md)|||
|apm.FPackage.zd_pkgkey|[apm.FPkgkey](/txt/gen/apm/apm.md#apm-fpkgkey)|[Llist](/txt/exe/amc/reftype/Llist.md)|||
|apm.FPackage.c_pkgdep|[apm.FPkgdep](/txt/gen/apm/apm.md#apm-fpkgdep)|[Ptrary](/txt/exe/amc/reftype/Ptrary.md)|||
|apm.FPackage.c_pkgdep_parent|[apm.FPkgdep](/txt/gen/apm/apm.md#apm-fpkgdep)|[Ptrary](/txt/exe/amc/reftype/Ptrary.md)|||
|apm.FPackage.zd_pkgrec|[apm.FPkgrec](/txt/gen/apm/apm.md#apm-fpkgrec)|[Llist](/txt/exe/amc/reftype/Llist.md)|||
|apm.FPackage.visited|bool|[Val](/txt/exe/amc/reftype/Val.md)|||

#### Struct FPackage
<a href="#struct-fpackage"></a>
*Note:* field ``apm.FPackage.base`` has reftype ``base`` so the fields of [dev.Package](/txt/ssimdb/dev/package.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/apm_gen.h](/include/gen/apm_gen.h)
```
struct FPackage { // apm.FPackage
    apm::FPackage*      zd_sel_package_next;     // zslist link; -1 means not-in-list
    apm::FPackage*      zd_sel_package_prev;     // previous element
    apm::FPackage*      zd_temp_package_next;    // zslist link; -1 means not-in-list
    apm::FPackage*      zd_temp_package_prev;    // previous element
    apm::FPackage*      ind_package_next;        // hash next
    u32                 ind_package_hashval;     // hash value
    apm::FPackage*      zd_topo_package_next;    // zslist link; -1 means not-in-list
    apm::FPackage*      zd_topo_package_prev;    // previous element
    algo::Smallstr50    package;                 // Package name
    algo::Smallstr50    baseref;                 // Base gitref of installed package
    algo::Smallstr200   origin;                  // Source URL for published package
    algo::Smallstr50    nomention;               // Regx of words the package contents must not mention; empty disables the check
    algo::cstring       comment;                 //
    apm::FPkgkey*       zd_pkgkey_head;          // zero-terminated doubly linked list
    i32                 zd_pkgkey_n;             // zero-terminated doubly linked list
    apm::FPkgkey*       zd_pkgkey_tail;          // pointer to last element
    apm::FPkgdep**      c_pkgdep_elems;          // array of pointers
    u64                 c_pkgdep_n;              // current size
    u64                 c_pkgdep_max;            // capacity of allocated array
    apm::FPkgdep**      c_pkgdep_parent_elems;   // array of pointers
    u64                 c_pkgdep_parent_n;       // current size
    u64                 c_pkgdep_parent_max;     // capacity of allocated array
    apm::FPkgrec*       zd_pkgrec_head;          // zero-terminated doubly linked list
    i32                 zd_pkgrec_n;             // zero-terminated doubly linked list
    apm::FPkgrec*       zd_pkgrec_tail;          // pointer to last element
    bool                visited;                 //   false
    // reftype Llist of apm.FPackage.zd_pkgkey prohibits copy
    // reftype Ptrary of apm.FPackage.c_pkgdep prohibits copy
    // reftype Ptrary of apm.FPackage.c_pkgdep_parent prohibits copy
    // reftype Llist of apm.FPackage.zd_pkgrec prohibits copy
    // func:apm.FPackage..AssignOp
    apm::FPackage&       operator =(const apm::FPackage &rhs) = delete;
    // reftype Llist of apm.FPackage.zd_pkgkey prohibits copy
    // reftype Ptrary of apm.FPackage.c_pkgdep prohibits copy
    // reftype Ptrary of apm.FPackage.c_pkgdep_parent prohibits copy
    // reftype Llist of apm.FPackage.zd_pkgrec prohibits copy
    // func:apm.FPackage..CopyCtor
    FPackage(const apm::FPackage &rhs) = delete;
private:
    // func:apm.FPackage..Ctor
    inline               FPackage() __attribute__((nothrow));
    // func:apm.FPackage..Dtor
    inline               ~FPackage() __attribute__((nothrow));
    friend apm::FPackage&       package_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend apm::FPackage*       package_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 package_RemoveAll() __attribute__((nothrow));
    friend void                 package_RemoveLast() __attribute__((nothrow));
};
```

#### apm.FPkgdep - OpenACR Package dependency
<a href="#apm-fpkgdep"></a>

#### apm.FPkgdep Fields
<a href="#apm-fpkgdep-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|apm.FPkgdep.base|[dev.Pkgdep](/txt/ssimdb/dev/pkgdep.md)|[Base](/txt/ssimdb/dev/pkgdep.md)|||
|apm.FPkgdep.p_parent|[apm.FPackage](/txt/gen/apm/apm.md#apm-fpackage)|[Upptr](/txt/exe/amc/reftype/Upptr.md)|||
|apm.FPkgdep.p_package|[apm.FPackage](/txt/gen/apm/apm.md#apm-fpackage)|[Upptr](/txt/exe/amc/reftype/Upptr.md)|||

#### Struct FPkgdep
<a href="#struct-fpkgdep"></a>
*Note:* field ``apm.FPkgdep.base`` has reftype ``base`` so the fields of [dev.Pkgdep](/txt/ssimdb/dev/pkgdep.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/apm_gen.h](/include/gen/apm_gen.h)
```
struct FPkgdep { // apm.FPkgdep
    algo::Smallstr100   pkgdep;                           //
    bool                soft;                             //   false  Soft dependency: affects package ordering but not forced update
    algo::Smallstr50    pkgdeptype;                       //   "require"  How this package stands to its parent
    algo::cstring       comment;                          //
    apm::FPackage*      p_parent;                         // reference to parent row
    apm::FPackage*      p_package;                        // reference to parent row
    bool                package_c_pkgdep_in_ary;          //   false  membership flag
    bool                package_c_pkgdep_parent_in_ary;   //   false  membership flag
    // x-reference on apm.FPkgdep.p_parent prevents copy
    // x-reference on apm.FPkgdep.p_package prevents copy
    // func:apm.FPkgdep..AssignOp
    apm::FPkgdep&        operator =(const apm::FPkgdep &rhs) = delete;
    // x-reference on apm.FPkgdep.p_parent prevents copy
    // x-reference on apm.FPkgdep.p_package prevents copy
    // func:apm.FPkgdep..CopyCtor
    FPkgdep(const apm::FPkgdep &rhs) = delete;
private:
    // func:apm.FPkgdep..Ctor
    inline               FPkgdep() __attribute__((nothrow));
    // func:apm.FPkgdep..Dtor
    inline               ~FPkgdep() __attribute__((nothrow));
    friend apm::FPkgdep&        pkgdep_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend apm::FPkgdep*        pkgdep_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 pkgdep_RemoveAll() __attribute__((nothrow));
    friend void                 pkgdep_RemoveLast() __attribute__((nothrow));
};
```

#### apm.FPkgkey - Keys belonging to the OpenACR package
<a href="#apm-fpkgkey"></a>

#### apm.FPkgkey Fields
<a href="#apm-fpkgkey-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|apm.FPkgkey.base|[dev.Pkgkey](/txt/ssimdb/dev/pkgkey.md)|[Base](/txt/ssimdb/dev/pkgkey.md)|||
|apm.FPkgkey.n_explicit|u32|[Val](/txt/exe/amc/reftype/Val.md)||# of explicitly matched records|
|apm.FPkgkey.n_up|u32|[Val](/txt/exe/amc/reftype/Val.md)||# of records connected upwards|
|apm.FPkgkey.n_down|u32|[Val](/txt/exe/amc/reftype/Val.md)||# of records connected downwards|
|apm.FPkgkey.c_pkgrec|[apm.FPkgrec](/txt/gen/apm/apm.md#apm-fpkgrec)|[Ptrary](/txt/exe/amc/reftype/Ptrary.md)|||

#### Struct FPkgkey
<a href="#struct-fpkgkey"></a>
*Note:* field ``apm.FPkgkey.base`` has reftype ``base`` so the fields of [dev.Pkgkey](/txt/ssimdb/dev/pkgkey.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/apm_gen.h](/include/gen/apm_gen.h)
```
struct FPkgkey { // apm.FPkgkey
    apm::FPkgkey*       ind_pkgkey_next;          // hash next
    u32                 ind_pkgkey_hashval;       // hash value
    apm::FPkgkey*       package_zd_pkgkey_next;   // zslist link; -1 means not-in-list
    apm::FPkgkey*       package_zd_pkgkey_prev;   // previous element
    algo::Smallstr150   pkgkey;                   //
    bool                up;                       //   false  perform transitive closure upwards
    bool                down;                     //   false  perform transitive closure downwards (via the leftmost parent link)
    bool                exclude;                  //   false  exclude selected records from the package
    algo::cstring       comment;                  //
    u32                 n_explicit;               //   0  # of explicitly matched records
    u32                 n_up;                     //   0  # of records connected upwards
    u32                 n_down;                   //   0  # of records connected downwards
    apm::FPkgrec**      c_pkgrec_elems;           // array of pointers
    u64                 c_pkgrec_n;               // current size
    u64                 c_pkgrec_max;             // capacity of allocated array
    // reftype Ptrary of apm.FPkgkey.c_pkgrec prohibits copy
    // func:apm.FPkgkey..AssignOp
    apm::FPkgkey&        operator =(const apm::FPkgkey &rhs) = delete;
    // reftype Ptrary of apm.FPkgkey.c_pkgrec prohibits copy
    // func:apm.FPkgkey..CopyCtor
    FPkgkey(const apm::FPkgkey &rhs) = delete;
private:
    // func:apm.FPkgkey..Ctor
    inline               FPkgkey() __attribute__((nothrow));
    // func:apm.FPkgkey..Dtor
    inline               ~FPkgkey() __attribute__((nothrow));
    friend apm::FPkgkey&        pkgkey_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend apm::FPkgkey*        pkgkey_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 pkgkey_RemoveAll() __attribute__((nothrow));
    friend void                 pkgkey_RemoveLast() __attribute__((nothrow));
};
```

#### apm.FPkgrec
<a href="#apm-fpkgrec"></a>

#### apm.FPkgrec Fields
<a href="#apm-fpkgrec-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|apm.FPkgrec.p_package|[apm.FPackage](/txt/gen/apm/apm.md#apm-fpackage)|[Upptr](/txt/exe/amc/reftype/Upptr.md)|||
|apm.FPkgrec.p_rec|[apm.FRec](/txt/gen/apm/apm.md#apm-frec)|[Upptr](/txt/exe/amc/reftype/Upptr.md)|||
|apm.FPkgrec.p_pkgkey|[apm.FPkgkey](/txt/gen/apm/apm.md#apm-fpkgkey)|[Upptr](/txt/exe/amc/reftype/Upptr.md)|||

#### Struct FPkgrec
<a href="#struct-fpkgrec"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/apm_gen.h](/include/gen/apm_gen.h)
```
struct FPkgrec { // apm.FPkgrec
    apm::FPkgrec*    pkgrec_next;              // Pointer to next free element int tpool
    apm::FPkgrec*    package_zd_pkgrec_next;   // zslist link; -1 means not-in-list
    apm::FPkgrec*    package_zd_pkgrec_prev;   // previous element
    apm::FPackage*   p_package;                // reference to parent row
    apm::FRec*       p_rec;                    // reference to parent row
    apm::FPkgkey*    p_pkgkey;                 // reference to parent row
    bool             pkgkey_c_pkgrec_in_ary;   //   false  membership flag
    apm::FPkgrec*    rec_zd_rec_pkgrec_next;   // zslist link; -1 means not-in-list
    apm::FPkgrec*    rec_zd_rec_pkgrec_prev;   // previous element
    // func:apm.FPkgrec..AssignOp
    inline apm::FPkgrec& operator =(const apm::FPkgrec &rhs) = delete;
    // func:apm.FPkgrec..CopyCtor
    inline               FPkgrec(const apm::FPkgrec &rhs) = delete;
private:
    // func:apm.FPkgrec..Ctor
    inline               FPkgrec() __attribute__((nothrow));
    // func:apm.FPkgrec..Dtor
    inline               ~FPkgrec() __attribute__((nothrow));
    friend apm::FPkgrec&        pkgrec_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend apm::FPkgrec*        pkgrec_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 pkgrec_Delete(apm::FPkgrec &row) __attribute__((nothrow));
};
```

#### apm.FRec
<a href="#apm-frec"></a>

#### apm.FRec Fields
<a href="#apm-frec-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|apm.FRec.rec|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)||Primary key: ssimfile:pkey|
|apm.FRec.p_ssimfile|[apm.FSsimfile](/txt/gen/apm/apm.md#apm-fssimfile)|[Upptr](/txt/exe/amc/reftype/Upptr.md)||Pointer to ssimfile|
|apm.FRec.tuple|[algo.Tuple](/txt/protocol/algo/Tuple.md)|[Val](/txt/exe/amc/reftype/Val.md)||Original record from file|
|apm.FRec.sortkey|[algo_lib.RecSortkey](/txt/gen/algo_lib/algo_lib.md#algo_lib-recsortkey)|[Val](/txt/exe/amc/reftype/Val.md)||Output sort key|
|apm.FRec.c_parent|[apm.FRec](/txt/gen/apm/apm.md#apm-frec)|[Ptrary](/txt/exe/amc/reftype/Ptrary.md)|||
|apm.FRec.c_child|[apm.FRec](/txt/gen/apm/apm.md#apm-frec)|[Ptrary](/txt/exe/amc/reftype/Ptrary.md)|||
|apm.FRec.level|i32|[Val](/txt/exe/amc/reftype/Val.md)|||
|apm.FRec.zd_rec_pkgrec|[apm.FPkgrec](/txt/gen/apm/apm.md#apm-fpkgrec)|[Llist](/txt/exe/amc/reftype/Llist.md)|||

#### Struct FRec
<a href="#struct-frec"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/apm_gen.h](/include/gen/apm_gen.h)
```
struct FRec { // apm.FRec
    apm::FRec*             rec_next;                        // Pointer to next free element int tpool
    apm::FRec*             ind_rec_next;                    // hash next
    u32                    ind_rec_hashval;                 // hash value
    apm::FRec*             zd_rec_next;                     // zslist link; -1 means not-in-list
    apm::FRec*             zd_rec_prev;                     // previous element
    apm::FRec*             zd_selrec_next;                  // zslist link; -1 means not-in-list
    apm::FRec*             zd_selrec_prev;                  // previous element
    apm::FRec*             zd_chooserec_next;               // zslist link; -1 means not-in-list
    apm::FRec*             zd_chooserec_prev;               // previous element
    algo::cstring          rec;                             // Primary key: ssimfile:pkey
    apm::FSsimfile*        p_ssimfile;                      // reference to parent row
    algo::Tuple            tuple;                           // Original record from file
    algo_lib::RecSortkey   sortkey;                         // Output sort key
    apm::FRec**            c_parent_elems;                  // array of pointers
    u64                    c_parent_n;                      // current size
    u64                    c_parent_max;                    // capacity of allocated array
    apm::FRec**            c_child_elems;                   // array of pointers
    u64                    c_child_n;                       // current size
    u64                    c_child_max;                     // capacity of allocated array
    i32                    level;                           //   0
    apm::FPkgrec*          zd_rec_pkgrec_head;              // zero-terminated doubly linked list
    i32                    zd_rec_pkgrec_n;                 // zero-terminated doubly linked list
    apm::FPkgrec*          zd_rec_pkgrec_tail;              // pointer to last element
    apm::FRec*             ssimfile_zd_ssimfile_rec_next;   // zslist link; -1 means not-in-list
    apm::FRec*             ssimfile_zd_ssimfile_rec_prev;   // previous element
    // reftype Ptrary of apm.FRec.c_parent prohibits copy
    // reftype Ptrary of apm.FRec.c_child prohibits copy
    // reftype Llist of apm.FRec.zd_rec_pkgrec prohibits copy
    // func:apm.FRec..AssignOp
    inline apm::FRec&    operator =(const apm::FRec &rhs) = delete;
    // reftype Ptrary of apm.FRec.c_parent prohibits copy
    // reftype Ptrary of apm.FRec.c_child prohibits copy
    // reftype Llist of apm.FRec.zd_rec_pkgrec prohibits copy
    // func:apm.FRec..CopyCtor
    inline               FRec(const apm::FRec &rhs) = delete;
private:
    // func:apm.FRec..Ctor
    inline               FRec() __attribute__((nothrow));
    // func:apm.FRec..Dtor
    inline               ~FRec() __attribute__((nothrow));
    friend apm::FRec&           rec_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend apm::FRec*           rec_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 rec_Delete(apm::FRec &row) __attribute__((nothrow));
};
```

#### apm.FSsimfile - File with ssim tuples
<a href="#apm-fssimfile"></a>

#### apm.FSsimfile Fields
<a href="#apm-fssimfile-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|apm.FSsimfile.base|[dmmeta.Ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|[Base](/txt/ssimdb/dmmeta/ssimfile.md)|||
|apm.FSsimfile.p_ctype|[apm.FCtype](/txt/gen/apm/apm.md#apm-fctype)|[Upptr](/txt/exe/amc/reftype/Upptr.md)|||
|apm.FSsimfile.c_ssimsort|[apm.FSsimsort](/txt/gen/apm/apm.md#apm-fssimsort)|[Ptr](/txt/exe/amc/reftype/Ptr.md)|||
|apm.FSsimfile.zd_ssimfile_rec|[apm.FRec](/txt/gen/apm/apm.md#apm-frec)|[Llist](/txt/exe/amc/reftype/Llist.md)|||

#### Struct FSsimfile
<a href="#struct-fssimfile"></a>
*Note:* field ``apm.FSsimfile.base`` has reftype ``base`` so the fields of [dmmeta.Ssimfile](/txt/ssimdb/dmmeta/ssimfile.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/apm_gen.h](/include/gen/apm_gen.h)
```
struct FSsimfile { // apm.FSsimfile
    apm::FSsimfile*     ind_ssimfile_next;      // hash next
    u32                 ind_ssimfile_hashval;   // hash value
    algo::Smallstr50    ssimfile;               //
    algo::Smallstr100   ctype;                  //
    apm::FCtype*        p_ctype;                // reference to parent row
    apm::FSsimsort*     c_ssimsort;             // optional pointer
    apm::FRec*          zd_ssimfile_rec_head;   // zero-terminated doubly linked list
    i32                 zd_ssimfile_rec_n;      // zero-terminated doubly linked list
    apm::FRec*          zd_ssimfile_rec_tail;   // pointer to last element
    // x-reference on apm.FSsimfile.p_ctype prevents copy
    // x-reference on apm.FSsimfile.c_ssimsort prevents copy
    // reftype Llist of apm.FSsimfile.zd_ssimfile_rec prohibits copy
    // func:apm.FSsimfile..AssignOp
    inline apm::FSsimfile& operator =(const apm::FSsimfile &rhs) = delete;
    // x-reference on apm.FSsimfile.p_ctype prevents copy
    // x-reference on apm.FSsimfile.c_ssimsort prevents copy
    // reftype Llist of apm.FSsimfile.zd_ssimfile_rec prohibits copy
    // func:apm.FSsimfile..CopyCtor
    inline               FSsimfile(const apm::FSsimfile &rhs) = delete;
private:
    // func:apm.FSsimfile..Ctor
    inline               FSsimfile() __attribute__((nothrow));
    // func:apm.FSsimfile..Dtor
    inline               ~FSsimfile() __attribute__((nothrow));
    friend apm::FSsimfile&      ssimfile_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend apm::FSsimfile*      ssimfile_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 ssimfile_RemoveAll() __attribute__((nothrow));
    friend void                 ssimfile_RemoveLast() __attribute__((nothrow));
};
```

#### apm.FSsimreq - Extended constraints for ssim records
<a href="#apm-fssimreq"></a>

#### apm.FSsimreq Fields
<a href="#apm-fssimreq-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|apm.FSsimreq.base|[dmmeta.Ssimreq](/txt/ssimdb/dmmeta/ssimreq.md)|[Base](/txt/ssimdb/dmmeta/ssimreq.md)|||
|apm.FSsimreq.p_child_ssimfile|[apm.FSsimfile](/txt/gen/apm/apm.md#apm-fssimfile)|[Upptr](/txt/exe/amc/reftype/Upptr.md)|||
|apm.FSsimreq.p_ctype|[apm.FCtype](/txt/gen/apm/apm.md#apm-fctype)|[Upptr](/txt/exe/amc/reftype/Upptr.md)|||
|apm.FSsimreq.p_field|[apm.FField](/txt/gen/apm/apm.md#apm-ffield)|[Upptr](/txt/exe/amc/reftype/Upptr.md)|||
|apm.FSsimreq.regx_value|[algo_lib.Regx](/txt/gen/algo_lib/algo_lib.md#algo_lib-regx)|[Val](/txt/exe/amc/reftype/Val.md)|||
|apm.FSsimreq.exclude|bool|[Val](/txt/exe/amc/reftype/Val.md)|||

#### Struct FSsimreq
<a href="#struct-fssimreq"></a>
*Note:* field ``apm.FSsimreq.base`` has reftype ``base`` so the fields of [dmmeta.Ssimreq](/txt/ssimdb/dmmeta/ssimreq.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/apm_gen.h](/include/gen/apm_gen.h)
```
struct FSsimreq { // apm.FSsimreq
    algo::Smallstr100   ssimreq;                  //
    algo::Smallstr100   parent;                   //
    bool                reqchild;                 //   false
    bool                bidir;                    //   false
    algo::cstring       comment;                  //
    apm::FSsimfile*     p_child_ssimfile;         // reference to parent row
    apm::FCtype*        p_ctype;                  // reference to parent row
    apm::FField*        p_field;                  // reference to parent row
    algo_lib::Regx      regx_value;               //
    bool                exclude;                  //   false
    bool                ctype_c_ssimreq_in_ary;   //   false  membership flag
    // x-reference on apm.FSsimreq.p_child_ssimfile prevents copy
    // x-reference on apm.FSsimreq.p_ctype prevents copy
    // x-reference on apm.FSsimreq.p_field prevents copy
    // func:apm.FSsimreq..AssignOp
    apm::FSsimreq&       operator =(const apm::FSsimreq &rhs) = delete;
    // x-reference on apm.FSsimreq.p_child_ssimfile prevents copy
    // x-reference on apm.FSsimreq.p_ctype prevents copy
    // x-reference on apm.FSsimreq.p_field prevents copy
    // func:apm.FSsimreq..CopyCtor
    FSsimreq(const apm::FSsimreq &rhs) = delete;
private:
    // func:apm.FSsimreq..Ctor
    inline               FSsimreq() __attribute__((nothrow));
    // func:apm.FSsimreq..Dtor
    inline               ~FSsimreq() __attribute__((nothrow));
    friend apm::FSsimreq&       ssimreq_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend apm::FSsimreq*       ssimreq_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 ssimreq_RemoveAll() __attribute__((nothrow));
    friend void                 ssimreq_RemoveLast() __attribute__((nothrow));
};
```

#### apm.FSsimsort - Define sort order for ssimfile
<a href="#apm-fssimsort"></a>

#### apm.FSsimsort Fields
<a href="#apm-fssimsort-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|apm.FSsimsort.base|[dmmeta.Ssimsort](/txt/ssimdb/dmmeta/ssimsort.md)|[Base](/txt/ssimdb/dmmeta/ssimsort.md)|||

#### Struct FSsimsort
<a href="#struct-fssimsort"></a>
*Note:* field ``apm.FSsimsort.base`` has reftype ``base`` so the fields of [dmmeta.Ssimsort](/txt/ssimdb/dmmeta/ssimsort.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/apm_gen.h](/include/gen/apm_gen.h)
```
struct FSsimsort { // apm.FSsimsort
    apm::FSsimsort*     ind_ssimsort_next;      // hash next
    u32                 ind_ssimsort_hashval;   // hash value
    algo::Smallstr50    ssimfile;               //
    algo::Smallstr150   sortfld;                //
    algo::cstring       comment;                //
    // func:apm.FSsimsort..AssignOp
    inline apm::FSsimsort& operator =(const apm::FSsimsort &rhs) = delete;
    // func:apm.FSsimsort..CopyCtor
    inline               FSsimsort(const apm::FSsimsort &rhs) = delete;
private:
    // func:apm.FSsimsort..Ctor
    inline               FSsimsort() __attribute__((nothrow));
    // func:apm.FSsimsort..Dtor
    inline               ~FSsimsort() __attribute__((nothrow));
    friend apm::FSsimsort&      ssimsort_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend apm::FSsimsort*      ssimsort_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 ssimsort_RemoveAll() __attribute__((nothrow));
    friend void                 ssimsort_RemoveLast() __attribute__((nothrow));
};
```

#### apm.FSubstr - Specify that the field value is computed from a substring of another field
<a href="#apm-fsubstr"></a>

#### apm.FSubstr Fields
<a href="#apm-fsubstr-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|apm.FSubstr.base|[dmmeta.Substr](/txt/ssimdb/dmmeta/substr.md)|[Base](/txt/ssimdb/dmmeta/substr.md)|||
|apm.FSubstr.p_srcfield|[apm.FField](/txt/gen/apm/apm.md#apm-ffield)|[Upptr](/txt/exe/amc/reftype/Upptr.md)|||

#### Struct FSubstr
<a href="#struct-fsubstr"></a>
*Note:* field ``apm.FSubstr.base`` has reftype ``base`` so the fields of [dmmeta.Substr](/txt/ssimdb/dmmeta/substr.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/apm_gen.h](/include/gen/apm_gen.h)
```
struct FSubstr { // apm.FSubstr
    algo::Smallstr150   field;        //
    algo::CppExpr       expr;         //
    algo::Smallstr150   srcfield;     //
    apm::FField*        p_srcfield;   // reference to parent row
    // x-reference on apm.FSubstr.p_srcfield prevents copy
    // func:apm.FSubstr..AssignOp
    inline apm::FSubstr& operator =(const apm::FSubstr &rhs) = delete;
    // x-reference on apm.FSubstr.p_srcfield prevents copy
    // func:apm.FSubstr..CopyCtor
    inline               FSubstr(const apm::FSubstr &rhs) = delete;
private:
    // func:apm.FSubstr..Ctor
    inline               FSubstr() __attribute__((nothrow));
    // func:apm.FSubstr..Dtor
    inline               ~FSubstr() __attribute__((nothrow));
    friend apm::FSubstr&        substr_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend apm::FSubstr*        substr_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 substr_RemoveAll() __attribute__((nothrow));
    friend void                 substr_RemoveLast() __attribute__((nothrow));
};
```

### Tests
<a href="#tests"></a>
The following component tests are defined for `apm`.
These can be executed with `atf_comp <comptest> -v`
|Comptest|Comment|
|---|---|
|[apm.DryRun](/test/atf_comp/apm.DryRun)|a dry run prints the plan and leaves no transaction script, even where temp/ does not exist|
|[apm.RecfileWriteFail](/test/atf_comp/apm.RecfileWriteFail)|a failed records-file write stops the removal instead of emitting a script that deletes nothing|
|[apm.TransactionExit](/test/atf_comp/apm.TransactionExit)|a failed transaction exits with the script exit code, not the raw wait status|
|[apm.UpdateFate](/test/atf_comp/apm.UpdateFate)|a step of an update that fails stops the update instead of a plan built from the empty file the step left|

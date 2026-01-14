## samp_make - Internals


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Toc -->
* [Description](#description)
* [Sources](#sources)
* [In Memory DB](#in-memory-db)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Toc -->

### Description
<a href="#description"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Description -->
for operations description see [samp_make - sample program for Makefile management](/txt/exe/samp_make/README.md)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Description -->

### Sources
<a href="#sources"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Sources -->
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/gen/samp_make_gen.cpp](/cpp/gen/samp_make_gen.cpp)||
|[cpp/samp_make/samp_make.cpp](/cpp/samp_make/samp_make.cpp)||
|[include/gen/samp_make_gen.h](/include/gen/samp_make_gen.h)||
|[include/gen/samp_make_gen.inl.h](/include/gen/samp_make_gen.inl.h)||
|[include/samp_make.h](/include/samp_make.h)||

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
`samp_make` generated code creates the tables below.
All allocations are done through global `samp_make::_db` [samp_make.FDb](#samp_make-fdb) structure
|Ctype|Ssimfile|Create|Access|
|---|---|---|---|
|[samp_make.FDb](#samp_make-fdb)||FDb._db (Global)|
|[samp_make.FGitfile](#samp_make-fgitfile)|[sampdb.gitfile](/txt/ssimdb/sampdb/gitfile.md)|FDb.gitfile (Lary)|gitfile (Lary, by rowid)|ind_gitfile (Thash, hash field gitfile)|
|[samp_make.FTargdep](#samp_make-ftargdep)|[sampdb.targdep](/txt/ssimdb/sampdb/targdep.md)|FDb.targdep (Lary)|targdep (Lary, by rowid)|ind_targdep (Thash, hash field targdep)|
||||FTarget.c_targdep (Ptrary)|
|[samp_make.FTarget](#samp_make-ftarget)|[sampdb.target](/txt/ssimdb/sampdb/target.md)|FDb.target (Lary)|target (Lary, by rowid)|ind_target (Thash, hash field target)|c_target (Ptrary)|
||||FTargdep.p_parent (Upptr)|
||||FTargdep.p_target (Upptr)|
|[samp_make.FTargrec](#samp_make-ftargrec)|[sampdb.targrec](/txt/ssimdb/sampdb/targrec.md)|FDb.targrec (Lary)|targrec (Lary, by rowid)|ind_targrec (Thash, hash field target)|
||||FTarget.c_targrec (Ptr)|
|[samp_make.FTargsrc](#samp_make-ftargsrc)|[sampdb.targsrc](/txt/ssimdb/sampdb/targsrc.md)|FDb.targsrc (Lary)|targsrc (Lary, by rowid)|ind_targsrc (Thash, hash field targsrc)|
||||FTarget.c_targsrc (Ptrary)|

#### samp_make.FDb - 
<a href="#samp_make-fdb"></a>

#### samp_make.FDb Fields
<a href="#samp_make-fdb-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|samp_make.FDb._db|[samp_make.FDb](/txt/exe/samp_make/internals.md#samp_make-fdb)|[Global](/txt/exe/amc/reftypes.md#global)|||
|samp_make.FDb.cmdline|[command.samp_make](/txt/protocol/command/README.md#command-samp_make)|[Val](/txt/exe/amc/reftypes.md#val)|||
|samp_make.FDb.gitfile|[samp_make.FGitfile](/txt/exe/samp_make/internals.md#samp_make-fgitfile)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|samp_make.FDb.ind_gitfile|[samp_make.FGitfile](/txt/exe/samp_make/internals.md#samp_make-fgitfile)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|samp_make.FDb.target|[samp_make.FTarget](/txt/exe/samp_make/internals.md#samp_make-ftarget)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|samp_make.FDb.ind_target|[samp_make.FTarget](/txt/exe/samp_make/internals.md#samp_make-ftarget)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|samp_make.FDb.targdep|[samp_make.FTargdep](/txt/exe/samp_make/internals.md#samp_make-ftargdep)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|samp_make.FDb.ind_targdep|[samp_make.FTargdep](/txt/exe/samp_make/internals.md#samp_make-ftargdep)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|samp_make.FDb.targsrc|[samp_make.FTargsrc](/txt/exe/samp_make/internals.md#samp_make-ftargsrc)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|samp_make.FDb.ind_targsrc|[samp_make.FTargsrc](/txt/exe/samp_make/internals.md#samp_make-ftargsrc)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|samp_make.FDb.targrec|[samp_make.FTargrec](/txt/exe/samp_make/internals.md#samp_make-ftargrec)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|samp_make.FDb.ind_targrec|[samp_make.FTargrec](/txt/exe/samp_make/internals.md#samp_make-ftargrec)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|samp_make.FDb.c_target|[samp_make.FTarget](/txt/exe/samp_make/internals.md#samp_make-ftarget)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||

#### Struct FDb
<a href="#struct-fdb"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/samp_make_gen.h](/include/gen/samp_make_gen.h)
```
struct FDb { // samp_make.FDb
    command::samp_make      cmdline;                     //
    samp_make::FGitfile*    gitfile_lary[32];            // level array
    i32                     gitfile_n;                   // number of elements in array
    samp_make::FGitfile**   ind_gitfile_buckets_elems;   // pointer to bucket array
    i32                     ind_gitfile_buckets_n;       // number of elements in bucket array
    i32                     ind_gitfile_n;               // number of elements in the hash table
    samp_make::FTarget*     target_lary[32];             // level array
    i32                     target_n;                    // number of elements in array
    samp_make::FTarget**    ind_target_buckets_elems;    // pointer to bucket array
    i32                     ind_target_buckets_n;        // number of elements in bucket array
    i32                     ind_target_n;                // number of elements in the hash table
    samp_make::FTargdep*    targdep_lary[32];            // level array
    i32                     targdep_n;                   // number of elements in array
    samp_make::FTargdep**   ind_targdep_buckets_elems;   // pointer to bucket array
    i32                     ind_targdep_buckets_n;       // number of elements in bucket array
    i32                     ind_targdep_n;               // number of elements in the hash table
    samp_make::FTargsrc*    targsrc_lary[32];            // level array
    i32                     targsrc_n;                   // number of elements in array
    samp_make::FTargsrc**   ind_targsrc_buckets_elems;   // pointer to bucket array
    i32                     ind_targsrc_buckets_n;       // number of elements in bucket array
    i32                     ind_targsrc_n;               // number of elements in the hash table
    samp_make::FTargrec*    targrec_lary[32];            // level array
    i32                     targrec_n;                   // number of elements in array
    samp_make::FTargrec**   ind_targrec_buckets_elems;   // pointer to bucket array
    i32                     ind_targrec_buckets_n;       // number of elements in bucket array
    i32                     ind_targrec_n;               // number of elements in the hash table
    samp_make::FTarget**    c_target_elems;              // array of pointers
    u32                     c_target_n;                  // array of pointers
    u32                     c_target_max;                // capacity of allocated array
    samp_make::trace        trace;                       //
};
```

#### samp_make.FGitfile - samp_make list of gitfile sources
<a href="#samp_make-fgitfile"></a>

#### samp_make.FGitfile Fields
<a href="#samp_make-fgitfile-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|samp_make.FGitfile.base|[sampdb.Gitfile](/txt/ssimdb/sampdb/gitfile.md)|[Base](/txt/ssimdb/sampdb/gitfile.md)|||

#### Struct FGitfile
<a href="#struct-fgitfile"></a>
*Note:* field ``samp_make.FGitfile.base`` has reftype ``base`` so the fields of [sampdb.Gitfile](/txt/ssimdb/sampdb/gitfile.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/samp_make_gen.h](/include/gen/samp_make_gen.h)
```
struct FGitfile { // samp_make.FGitfile
    samp_make::FGitfile*   ind_gitfile_next;      // hash next
    u32                    ind_gitfile_hashval;   // hash value
    algo::Smallstr50       gitfile;               //
    algo::Comment          comment;               //
    // func:samp_make.FGitfile..AssignOp
    inline samp_make::FGitfile& operator =(const samp_make::FGitfile &rhs) = delete;
    // func:samp_make.FGitfile..CopyCtor
    inline               FGitfile(const samp_make::FGitfile &rhs) = delete;
private:
    // func:samp_make.FGitfile..Ctor
    inline               FGitfile() __attribute__((nothrow));
    // func:samp_make.FGitfile..Dtor
    inline               ~FGitfile() __attribute__((nothrow));
    friend samp_make::FGitfile& gitfile_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend samp_make::FGitfile* gitfile_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 gitfile_RemoveAll() __attribute__((nothrow));
    friend void                 gitfile_RemoveLast() __attribute__((nothrow));
};
```

#### samp_make.FTargdep - samp_make targets dependencies
<a href="#samp_make-ftargdep"></a>

#### samp_make.FTargdep Fields
<a href="#samp_make-ftargdep-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|samp_make.FTargdep.base|[sampdb.Targdep](/txt/ssimdb/sampdb/targdep.md)|[Base](/txt/ssimdb/sampdb/targdep.md)|||
|samp_make.FTargdep.p_parent|[samp_make.FTarget](/txt/exe/samp_make/internals.md#samp_make-ftarget)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|samp_make.FTargdep.p_target|[samp_make.FTarget](/txt/exe/samp_make/internals.md#samp_make-ftarget)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FTargdep
<a href="#struct-ftargdep"></a>
*Note:* field ``samp_make.FTargdep.base`` has reftype ``base`` so the fields of [sampdb.Targdep](/txt/ssimdb/sampdb/targdep.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/samp_make_gen.h](/include/gen/samp_make_gen.h)
```
struct FTargdep { // samp_make.FTargdep
    samp_make::FTargdep*   ind_targdep_next;          // hash next
    u32                    ind_targdep_hashval;       // hash value
    algo::Smallstr50       targdep;                   //
    bool                   rec;                       //   false  use in recipe
    bool                   pre;                       //   false  use in prerequisite
    algo::Comment          comment;                   //
    samp_make::FTarget*    p_parent;                  // reference to parent row
    samp_make::FTarget*    p_target;                  // reference to parent row
    bool                   target_c_targdep_in_ary;   //   false  membership flag
    // x-reference on samp_make.FTargdep.p_parent prevents copy
    // x-reference on samp_make.FTargdep.p_target prevents copy
    // func:samp_make.FTargdep..AssignOp
    samp_make::FTargdep& operator =(const samp_make::FTargdep &rhs) = delete;
    // x-reference on samp_make.FTargdep.p_parent prevents copy
    // x-reference on samp_make.FTargdep.p_target prevents copy
    // func:samp_make.FTargdep..CopyCtor
    FTargdep(const samp_make::FTargdep &rhs) = delete;
private:
    // func:samp_make.FTargdep..Ctor
    inline               FTargdep() __attribute__((nothrow));
    // func:samp_make.FTargdep..Dtor
    inline               ~FTargdep() __attribute__((nothrow));
    friend samp_make::FTargdep& targdep_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend samp_make::FTargdep* targdep_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 targdep_RemoveAll() __attribute__((nothrow));
    friend void                 targdep_RemoveLast() __attribute__((nothrow));
};
```

#### samp_make.FTarget - samp_make targets list
<a href="#samp_make-ftarget"></a>

#### samp_make.FTarget Fields
<a href="#samp_make-ftarget-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|samp_make.FTarget.base|[sampdb.Target](/txt/ssimdb/sampdb/target.md)|[Base](/txt/ssimdb/sampdb/target.md)|||
|samp_make.FTarget.prereq|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||collection of make prereq lines|
|samp_make.FTarget.select|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|samp_make.FTarget.recipe|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||collection of make recipe lines|
|samp_make.FTarget.c_targsrc|[samp_make.FTargsrc](/txt/exe/samp_make/internals.md#samp_make-ftargsrc)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|samp_make.FTarget.c_targdep|[samp_make.FTargdep](/txt/exe/samp_make/internals.md#samp_make-ftargdep)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|samp_make.FTarget.c_targrec|[samp_make.FTargrec](/txt/exe/samp_make/internals.md#samp_make-ftargrec)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||

#### Struct FTarget
<a href="#struct-ftarget"></a>
*Note:* field ``samp_make.FTarget.base`` has reftype ``base`` so the fields of [sampdb.Target](/txt/ssimdb/sampdb/target.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/samp_make_gen.h](/include/gen/samp_make_gen.h)
```
struct FTarget { // samp_make.FTarget
    samp_make::FTarget*     ind_target_next;      // hash next
    u32                     ind_target_hashval;   // hash value
    algo::Smallstr50        target;               //
    bool                    dflt;                 //   false  make first target - default target
    algo::Comment           comment;              //
    algo::cstring           prereq;               // collection of make prereq lines
    bool                    select;               //   false
    algo::cstring           recipe;               // collection of make recipe lines
    samp_make::FTargsrc**   c_targsrc_elems;      // array of pointers
    u32                     c_targsrc_n;          // array of pointers
    u32                     c_targsrc_max;        // capacity of allocated array
    samp_make::FTargdep**   c_targdep_elems;      // array of pointers
    u32                     c_targdep_n;          // array of pointers
    u32                     c_targdep_max;        // capacity of allocated array
    samp_make::FTargrec*    c_targrec;            // optional pointer
    bool                    c_target_in_ary;      //   false  membership flag
    // reftype Ptrary of samp_make.FTarget.c_targsrc prohibits copy
    // reftype Ptrary of samp_make.FTarget.c_targdep prohibits copy
    // x-reference on samp_make.FTarget.c_targrec prevents copy
    // func:samp_make.FTarget..AssignOp
    samp_make::FTarget&  operator =(const samp_make::FTarget &rhs) = delete;
    // reftype Ptrary of samp_make.FTarget.c_targsrc prohibits copy
    // reftype Ptrary of samp_make.FTarget.c_targdep prohibits copy
    // x-reference on samp_make.FTarget.c_targrec prevents copy
    // func:samp_make.FTarget..CopyCtor
    FTarget(const samp_make::FTarget &rhs) = delete;
private:
    // func:samp_make.FTarget..Ctor
    inline               FTarget() __attribute__((nothrow));
    // func:samp_make.FTarget..Dtor
    inline               ~FTarget() __attribute__((nothrow));
    friend samp_make::FTarget&  target_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend samp_make::FTarget*  target_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 target_RemoveAll() __attribute__((nothrow));
    friend void                 target_RemoveLast() __attribute__((nothrow));
};
```

#### samp_make.FTargrec - samp_make recipe for a target
<a href="#samp_make-ftargrec"></a>

#### samp_make.FTargrec Fields
<a href="#samp_make-ftargrec-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|samp_make.FTargrec.base|[sampdb.Targrec](/txt/ssimdb/sampdb/targrec.md)|[Base](/txt/ssimdb/sampdb/targrec.md)|||

#### Struct FTargrec
<a href="#struct-ftargrec"></a>
*Note:* field ``samp_make.FTargrec.base`` has reftype ``base`` so the fields of [sampdb.Targrec](/txt/ssimdb/sampdb/targrec.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/samp_make_gen.h](/include/gen/samp_make_gen.h)
```
struct FTargrec { // samp_make.FTargrec
    samp_make::FTargrec*   ind_targrec_next;      // hash next
    u32                    ind_targrec_hashval;   // hash value
    algo::Smallstr50       target;                //
    algo::cstring          recipe;                //
    algo::Comment          comment;               //
    // func:samp_make.FTargrec..AssignOp
    inline samp_make::FTargrec& operator =(const samp_make::FTargrec &rhs) = delete;
    // func:samp_make.FTargrec..CopyCtor
    inline               FTargrec(const samp_make::FTargrec &rhs) = delete;
private:
    // func:samp_make.FTargrec..Ctor
    inline               FTargrec() __attribute__((nothrow));
    // func:samp_make.FTargrec..Dtor
    inline               ~FTargrec() __attribute__((nothrow));
    friend samp_make::FTargrec& targrec_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend samp_make::FTargrec* targrec_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 targrec_RemoveAll() __attribute__((nothrow));
    friend void                 targrec_RemoveLast() __attribute__((nothrow));
};
```

#### samp_make.FTargsrc - samp_make targets source dependencies
<a href="#samp_make-ftargsrc"></a>

#### samp_make.FTargsrc Fields
<a href="#samp_make-ftargsrc-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|samp_make.FTargsrc.base|[sampdb.Targsrc](/txt/ssimdb/sampdb/targsrc.md)|[Base](/txt/ssimdb/sampdb/targsrc.md)|||

#### Struct FTargsrc
<a href="#struct-ftargsrc"></a>
*Note:* field ``samp_make.FTargsrc.base`` has reftype ``base`` so the fields of [sampdb.Targsrc](/txt/ssimdb/sampdb/targsrc.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/samp_make_gen.h](/include/gen/samp_make_gen.h)
```
struct FTargsrc { // samp_make.FTargsrc
    samp_make::FTargsrc*   ind_targsrc_next;          // hash next
    u32                    ind_targsrc_hashval;       // hash value
    algo::Smallstr50       targsrc;                   //
    bool                   pre;                       //   false  use in prerequisite
    bool                   rec;                       //   false  use in recipe
    algo::Comment          comment;                   //
    bool                   target_c_targsrc_in_ary;   //   false  membership flag
    // func:samp_make.FTargsrc..AssignOp
    inline samp_make::FTargsrc& operator =(const samp_make::FTargsrc &rhs) = delete;
    // func:samp_make.FTargsrc..CopyCtor
    inline               FTargsrc(const samp_make::FTargsrc &rhs) = delete;
private:
    // func:samp_make.FTargsrc..Ctor
    inline               FTargsrc() __attribute__((nothrow));
    // func:samp_make.FTargsrc..Dtor
    inline               ~FTargsrc() __attribute__((nothrow));
    friend samp_make::FTargsrc& targsrc_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend samp_make::FTargsrc* targsrc_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 targsrc_RemoveAll() __attribute__((nothrow));
    friend void                 targsrc_RemoveLast() __attribute__((nothrow));
};
```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Imdb -->


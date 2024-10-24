## atf_ci - Internals


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
for usage, see [atf_ci - Normalization tests (see citest table)](/txt/exe/atf_ci/README.md)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Description -->

### Sources
<a href="#sources"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Sources -->
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/atf_ci/acr_ed.cpp](/cpp/atf_ci/acr_ed.cpp)||
|[cpp/atf_ci/apm.cpp](/cpp/atf_ci/apm.cpp)||
|[cpp/atf_ci/bootstrap.cpp](/cpp/atf_ci/bootstrap.cpp)||
|[cpp/atf_ci/cppcheck.cpp](/cpp/atf_ci/cppcheck.cpp)||
|[cpp/atf_ci/main.cpp](/cpp/atf_ci/main.cpp)||
|[cpp/atf_ci/src.cpp](/cpp/atf_ci/src.cpp)|source file (cpp/, include/) normalizations including amc|
|[cpp/atf_ci/ssim.cpp](/cpp/atf_ci/ssim.cpp)|ssim database normalizations|
|[cpp/gen/atf_ci_gen.cpp](/cpp/gen/atf_ci_gen.cpp)||
|[include/atf_ci.h](/include/atf_ci.h)||
|[include/gen/atf_ci_gen.h](/include/gen/atf_ci_gen.h)||
|[include/gen/atf_ci_gen.inl.h](/include/gen/atf_ci_gen.inl.h)||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Sources -->

### Dependencies
<a href="#dependencies"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Dependencies -->
The build target depends on the following libraries
|Target|Comment|
|---|---|
|[algo_lib](/txt/lib/algo_lib/README.md)|Support library for all executables|
|[lib_ctype](/txt/lib/lib_ctype/README.md)|Helpful library for reading/writing ctypes as text tuples|
|[lib_git](/txt/lib/lib_git/README.md)|Helpful git wrappers|
|[lib_prot](/txt/lib/lib_prot/README.md)|Library covering all protocols|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Dependencies -->

### In Memory DB
<a href="#in-memory-db"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Imdb -->
`atf_ci` generated code creates the tables below.
All allocations are done through global `atf_ci::_db` [atf_ci.FDb](#atf_ci-fdb) structure
|Ctype|Ssimfile|Create|Access|
|---|---|---|---|
|[atf_ci.FBuilddir](#atf_ci-fbuilddir)|[dev.builddir](/txt/ssimdb/dev/builddir.md)|FDb.builddir (Lary)|builddir (Lary, by rowid)|ind_builddir (Thash, hash field builddir)|
|[atf_ci.FCfg](#atf_ci-fcfg)|[dev.cfg](/txt/ssimdb/dev/cfg.md)|FDb.cfg (Lary)|cfg (Lary, by rowid)|
|[atf_ci.FCipackage](#atf_ci-fcipackage)|[atfdb.cipackage](/txt/ssimdb/atfdb/cipackage.md)|FDb.cipackage (Lary)|cipackage (Lary, by rowid)|
|[atf_ci.FCitest](#atf_ci-fcitest)|[atfdb.citest](/txt/ssimdb/atfdb/citest.md)|FDb.citest (Lary)|**static**|citest (Lary, by rowid)|c_citest (Ptr)|ind_citest (Thash, hash field citest)|
|[atf_ci.FDb](#atf_ci-fdb)||FDb._db (Global)|
|[atf_ci.FExecLimit](#atf_ci-fexeclimit)||
|[atf_ci.FGitfile](#atf_ci-fgitfile)|[dev.gitfile](/txt/ssimdb/dev/gitfile.md)|FDb.gitfile (Lary)|gitfile (Lary, by rowid)|ind_gitfile (Thash, hash field gitfile)|
|[atf_ci.FMsgfile](#atf_ci-fmsgfile)|[dev.msgfile](/txt/ssimdb/dev/msgfile.md)|FDb.msgfile (Lary)|msgfile (Lary, by rowid)|
|[atf_ci.FNoindent](#atf_ci-fnoindent)|[dev.noindent](/txt/ssimdb/dev/noindent.md)|FDb.noindent (Lary)|noindent (Lary, by rowid)|
||||FGitfile.c_noindent (Ptr)|
|[atf_ci.FNs](#atf_ci-fns)|[dmmeta.ns](/txt/ssimdb/dmmeta/ns.md)|FDb.ns (Lary)|ns (Lary, by rowid)|ind_ns (Thash, hash field ns)|
|[atf_ci.FReadme](#atf_ci-freadme)|[dev.readme](/txt/ssimdb/dev/readme.md)|FDb.readme (Lary)|readme (Lary, by rowid)|
|[atf_ci.FScriptfile](#atf_ci-fscriptfile)|[dev.scriptfile](/txt/ssimdb/dev/scriptfile.md)|FDb.scriptfile (Lary)|scriptfile (Lary, by rowid)|ind_scriptfile (Thash, hash field gitfile)|
||||FGitfile.c_scriptfile (Ptr)|
|[atf_ci.FSsimfile](#atf_ci-fssimfile)|[dmmeta.ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|FDb.ssimfile (Lary)|ssimfile (Lary, by rowid)|ind_ssimfile (Thash, hash field ssimfile)|
|[atf_ci.FTargsrc](#atf_ci-ftargsrc)|[dev.targsrc](/txt/ssimdb/dev/targsrc.md)|FDb.targsrc (Lary)|targsrc (Lary, by rowid)|
||||FGitfile.c_targsrc (Ptr)|
|[atf_ci.File](#atf_ci-file)||FDb.file (Lary)|file (Lary, by rowid)|ind_file (Thash, hash field file)|

#### atf_ci.FBuilddir - Directory where object files/executables go. Determines compile/link options
<a href="#atf_ci-fbuilddir"></a>

#### atf_ci.FBuilddir Fields
<a href="#atf_ci-fbuilddir-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_ci.FBuilddir.base|[dev.Builddir](/txt/ssimdb/dev/builddir.md)|[Base](/txt/ssimdb/dev/builddir.md)|||

#### Struct FBuilddir
<a href="#struct-fbuilddir"></a>
*Note:* field ``atf_ci.FBuilddir.base`` has reftype ``base`` so the fields of [dev.Builddir](/txt/ssimdb/dev/builddir.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_ci_gen.h](/include/gen/atf_ci_gen.h)
```
struct FBuilddir { // atf_ci.FBuilddir
    algo::Smallstr50     builddir;            // Primary key - uname.compiler.cfg-arch
    algo::Comment        comment;             //
    atf_ci::FBuilddir*   ind_builddir_next;   // hash next
    // func:atf_ci.FBuilddir..AssignOp
    inline atf_ci::FBuilddir& operator =(const atf_ci::FBuilddir &rhs) = delete;
    // func:atf_ci.FBuilddir..CopyCtor
    inline               FBuilddir(const atf_ci::FBuilddir &rhs) = delete;
private:
    // func:atf_ci.FBuilddir..Ctor
    inline               FBuilddir() __attribute__((nothrow));
    // func:atf_ci.FBuilddir..Dtor
    inline               ~FBuilddir() __attribute__((nothrow));
    friend atf_ci::FBuilddir&   builddir_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend atf_ci::FBuilddir*   builddir_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 builddir_RemoveAll() __attribute__((nothrow));
    friend void                 builddir_RemoveLast() __attribute__((nothrow));
};
```

#### atf_ci.FCfg - Compiler configuration
<a href="#atf_ci-fcfg"></a>

#### atf_ci.FCfg Fields
<a href="#atf_ci-fcfg-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_ci.FCfg.base|[dev.Cfg](/txt/ssimdb/dev/cfg.md)|[Base](/txt/ssimdb/dev/cfg.md)|||

#### Struct FCfg
<a href="#struct-fcfg"></a>
*Note:* field ``atf_ci.FCfg.base`` has reftype ``base`` so the fields of [dev.Cfg](/txt/ssimdb/dev/cfg.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_ci_gen.h](/include/gen/atf_ci_gen.h)
```
struct FCfg { // atf_ci.FCfg
    algo::Smallstr50   cfg;       //
    algo::Smallstr5    suffix;    //
    algo::Comment      comment;   //
private:
    // func:atf_ci.FCfg..Ctor
    inline               FCfg() __attribute__((nothrow));
    friend atf_ci::FCfg&        cfg_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend atf_ci::FCfg*        cfg_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 cfg_RemoveAll() __attribute__((nothrow));
    friend void                 cfg_RemoveLast() __attribute__((nothrow));
};
```

#### atf_ci.FCipackage - 
<a href="#atf_ci-fcipackage"></a>

#### atf_ci.FCipackage Fields
<a href="#atf_ci-fcipackage-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_ci.FCipackage.base|[atfdb.Cipackage](/txt/ssimdb/atfdb/cipackage.md)|[Base](/txt/ssimdb/atfdb/cipackage.md)|||

#### Struct FCipackage
<a href="#struct-fcipackage"></a>
*Note:* field ``atf_ci.FCipackage.base`` has reftype ``base`` so the fields of [atfdb.Cipackage](/txt/ssimdb/atfdb/cipackage.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_ci_gen.h](/include/gen/atf_ci_gen.h)
```
struct FCipackage { // atf_ci.FCipackage
    algo::Smallstr50    package;     //
    bool                remove;      //   false
    bool                build;       //   false
    algo::Smallstr100   reinstall;   //
    algo::Comment       comment;     //
private:
    // func:atf_ci.FCipackage..Ctor
    inline               FCipackage() __attribute__((nothrow));
    friend atf_ci::FCipackage&  cipackage_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend atf_ci::FCipackage*  cipackage_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 cipackage_RemoveAll() __attribute__((nothrow));
    friend void                 cipackage_RemoveLast() __attribute__((nothrow));
};
```

#### atf_ci.FCitest - 
<a href="#atf_ci-fcitest"></a>

#### atf_ci.FCitest Fields
<a href="#atf_ci-fcitest-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_ci.FCitest.base|[atfdb.Citest](/txt/ssimdb/atfdb/citest.md)|[Base](/txt/ssimdb/atfdb/citest.md)|||
|atf_ci.FCitest.nerr|i32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_ci.FCitest.step||[Hook](/txt/exe/amc/reftypes.md#hook)|||

#### Struct FCitest
<a href="#struct-fcitest"></a>
*Note:* field ``atf_ci.FCitest.base`` has reftype ``base`` so the fields of [atfdb.Citest](/txt/ssimdb/atfdb/citest.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_ci_gen.h](/include/gen/atf_ci_gen.h)
```
struct FCitest { // atf_ci.FCitest
    algo::Smallstr50           citest;            // Primary key
    algo::Smallstr50           cijob;             //   "test"  CI job in which this test runs
    bool                       sandbox;           //   false  Run test in sandbox
    algo::Comment              comment;           //
    i32                        nerr;              //   0
    atf_ci::citest_step_hook   step;              //   NULL  Pointer to a function
    atf_ci::FCitest*           ind_citest_next;   // hash next
    // reftype Hook of atf_ci.FCitest.step prohibits copy
    // func:atf_ci.FCitest..AssignOp
    inline atf_ci::FCitest& operator =(const atf_ci::FCitest &rhs) = delete;
    // reftype Hook of atf_ci.FCitest.step prohibits copy
    // func:atf_ci.FCitest..CopyCtor
    inline               FCitest(const atf_ci::FCitest &rhs) = delete;
private:
    // func:atf_ci.FCitest..Ctor
    inline               FCitest() __attribute__((nothrow));
    // func:atf_ci.FCitest..Dtor
    inline               ~FCitest() __attribute__((nothrow));
    friend atf_ci::FCitest&     citest_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend atf_ci::FCitest*     citest_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 citest_RemoveAll() __attribute__((nothrow));
    friend void                 citest_RemoveLast() __attribute__((nothrow));
};
```

#### atf_ci.FDb - In-memory database for atf_ci
<a href="#atf_ci-fdb"></a>

#### atf_ci.FDb Fields
<a href="#atf_ci-fdb-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_ci.FDb._db|[atf_ci.FDb](/txt/exe/atf_ci/internals.md#atf_ci-fdb)|[Global](/txt/exe/amc/reftypes.md#global)|||
|atf_ci.FDb.cmdline|[command.atf_ci](/txt/protocol/command/README.md#command-atf_ci)|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_ci.FDb.citest|[atf_ci.FCitest](/txt/exe/atf_ci/internals.md#atf_ci-fcitest)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|atf_ci.FDb.ssimfile|[atf_ci.FSsimfile](/txt/exe/atf_ci/internals.md#atf_ci-fssimfile)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|atf_ci.FDb.ind_ssimfile|[atf_ci.FSsimfile](/txt/exe/atf_ci/internals.md#atf_ci-fssimfile)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|atf_ci.FDb.c_citest|[atf_ci.FCitest](/txt/exe/atf_ci/internals.md#atf_ci-fcitest)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|atf_ci.FDb.scriptfile|[atf_ci.FScriptfile](/txt/exe/atf_ci/internals.md#atf_ci-fscriptfile)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|atf_ci.FDb.ind_scriptfile|[atf_ci.FScriptfile](/txt/exe/atf_ci/internals.md#atf_ci-fscriptfile)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|atf_ci.FDb.ns|[atf_ci.FNs](/txt/exe/atf_ci/internals.md#atf_ci-fns)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|atf_ci.FDb.ind_ns|[atf_ci.FNs](/txt/exe/atf_ci/internals.md#atf_ci-fns)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|atf_ci.FDb.readme|[atf_ci.FReadme](/txt/exe/atf_ci/internals.md#atf_ci-freadme)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|atf_ci.FDb.builddir|[atf_ci.FBuilddir](/txt/exe/atf_ci/internals.md#atf_ci-fbuilddir)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|atf_ci.FDb.cfg|[atf_ci.FCfg](/txt/exe/atf_ci/internals.md#atf_ci-fcfg)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|atf_ci.FDb.ind_builddir|[atf_ci.FBuilddir](/txt/exe/atf_ci/internals.md#atf_ci-fbuilddir)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|atf_ci.FDb.gitfile|[atf_ci.FGitfile](/txt/exe/atf_ci/internals.md#atf_ci-fgitfile)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|atf_ci.FDb.ind_gitfile|[atf_ci.FGitfile](/txt/exe/atf_ci/internals.md#atf_ci-fgitfile)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|atf_ci.FDb.noindent|[atf_ci.FNoindent](/txt/exe/atf_ci/internals.md#atf_ci-fnoindent)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|atf_ci.FDb.targsrc|[atf_ci.FTargsrc](/txt/exe/atf_ci/internals.md#atf_ci-ftargsrc)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|atf_ci.FDb.msgfile|[atf_ci.FMsgfile](/txt/exe/atf_ci/internals.md#atf_ci-fmsgfile)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|atf_ci.FDb.ind_citest|[atf_ci.FCitest](/txt/exe/atf_ci/internals.md#atf_ci-fcitest)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|atf_ci.FDb.file|[atf_ci.File](/txt/exe/atf_ci/internals.md#atf_ci-file)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|atf_ci.FDb.ind_file|[atf_ci.File](/txt/exe/atf_ci/internals.md#atf_ci-file)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|atf_ci.FDb.sandbox_need_init|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_ci.FDb.cipackage|[atf_ci.FCipackage](/txt/exe/atf_ci/internals.md#atf_ci-fcipackage)|[Lary](/txt/exe/amc/reftypes.md#lary)|||

#### Struct FDb
<a href="#struct-fdb"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_ci_gen.h](/include/gen/atf_ci_gen.h)
```
struct FDb { // atf_ci.FDb: In-memory database for atf_ci
    command::atf_ci         cmdline;                        //
    atf_ci::FCitest*        citest_lary[32];                // level array
    i32                     citest_n;                       // number of elements in array
    atf_ci::FSsimfile*      ssimfile_lary[32];              // level array
    i32                     ssimfile_n;                     // number of elements in array
    atf_ci::FSsimfile**     ind_ssimfile_buckets_elems;     // pointer to bucket array
    i32                     ind_ssimfile_buckets_n;         // number of elements in bucket array
    i32                     ind_ssimfile_n;                 // number of elements in the hash table
    atf_ci::FCitest*        c_citest;                       // optional pointer
    atf_ci::FScriptfile*    scriptfile_lary[32];            // level array
    i32                     scriptfile_n;                   // number of elements in array
    atf_ci::FScriptfile**   ind_scriptfile_buckets_elems;   // pointer to bucket array
    i32                     ind_scriptfile_buckets_n;       // number of elements in bucket array
    i32                     ind_scriptfile_n;               // number of elements in the hash table
    atf_ci::FNs*            ns_lary[32];                    // level array
    i32                     ns_n;                           // number of elements in array
    atf_ci::FNs**           ind_ns_buckets_elems;           // pointer to bucket array
    i32                     ind_ns_buckets_n;               // number of elements in bucket array
    i32                     ind_ns_n;                       // number of elements in the hash table
    atf_ci::FReadme*        readme_lary[32];                // level array
    i32                     readme_n;                       // number of elements in array
    atf_ci::FBuilddir*      builddir_lary[32];              // level array
    i32                     builddir_n;                     // number of elements in array
    atf_ci::FCfg*           cfg_lary[32];                   // level array
    i32                     cfg_n;                          // number of elements in array
    atf_ci::FBuilddir**     ind_builddir_buckets_elems;     // pointer to bucket array
    i32                     ind_builddir_buckets_n;         // number of elements in bucket array
    i32                     ind_builddir_n;                 // number of elements in the hash table
    atf_ci::FGitfile*       gitfile_lary[32];               // level array
    i32                     gitfile_n;                      // number of elements in array
    atf_ci::FGitfile**      ind_gitfile_buckets_elems;      // pointer to bucket array
    i32                     ind_gitfile_buckets_n;          // number of elements in bucket array
    i32                     ind_gitfile_n;                  // number of elements in the hash table
    atf_ci::FNoindent*      noindent_lary[32];              // level array
    i32                     noindent_n;                     // number of elements in array
    atf_ci::FTargsrc*       targsrc_lary[32];               // level array
    i32                     targsrc_n;                      // number of elements in array
    atf_ci::FMsgfile*       msgfile_lary[32];               // level array
    i32                     msgfile_n;                      // number of elements in array
    atf_ci::FCitest**       ind_citest_buckets_elems;       // pointer to bucket array
    i32                     ind_citest_buckets_n;           // number of elements in bucket array
    i32                     ind_citest_n;                   // number of elements in the hash table
    atf_ci::File*           file_lary[32];                  // level array
    i32                     file_n;                         // number of elements in array
    atf_ci::File**          ind_file_buckets_elems;         // pointer to bucket array
    i32                     ind_file_buckets_n;             // number of elements in bucket array
    i32                     ind_file_n;                     // number of elements in the hash table
    bool                    sandbox_need_init;              //   false
    atf_ci::FCipackage*     cipackage_lary[32];             // level array
    i32                     cipackage_n;                    // number of elements in array
    atf_ci::trace           trace;                          //
};
```

#### atf_ci.FExecLimit - 
<a href="#atf_ci-fexeclimit"></a>

#### atf_ci.FExecLimit Fields
<a href="#atf_ci-fexeclimit-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|

#### Struct FExecLimit
<a href="#struct-fexeclimit"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_ci_gen.h](/include/gen/atf_ci_gen.h)
```
struct FExecLimit { // atf_ci.FExecLimit
    // func:atf_ci.FExecLimit..Ctor
    inline               FExecLimit() __attribute__((nothrow));
};
```

#### atf_ci.FGitfile - File managed by git
<a href="#atf_ci-fgitfile"></a>

#### atf_ci.FGitfile Fields
<a href="#atf_ci-fgitfile-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_ci.FGitfile.base|[dev.Gitfile](/txt/ssimdb/dev/gitfile.md)|[Base](/txt/ssimdb/dev/gitfile.md)|||
|atf_ci.FGitfile.c_noindent|[atf_ci.FNoindent](/txt/exe/atf_ci/internals.md#atf_ci-fnoindent)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|atf_ci.FGitfile.c_scriptfile|[atf_ci.FScriptfile](/txt/exe/atf_ci/internals.md#atf_ci-fscriptfile)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|atf_ci.FGitfile.c_targsrc|[atf_ci.FTargsrc](/txt/exe/atf_ci/internals.md#atf_ci-ftargsrc)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||

#### Struct FGitfile
<a href="#struct-fgitfile"></a>
*Note:* field ``atf_ci.FGitfile.base`` has reftype ``base`` so the fields of [dev.Gitfile](/txt/ssimdb/dev/gitfile.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_ci_gen.h](/include/gen/atf_ci_gen.h)
```
struct FGitfile { // atf_ci.FGitfile
    atf_ci::FGitfile*      ind_gitfile_next;   // hash next
    algo::Smallstr200      gitfile;            //
    atf_ci::FNoindent*     c_noindent;         // optional pointer
    atf_ci::FScriptfile*   c_scriptfile;       // optional pointer
    atf_ci::FTargsrc*      c_targsrc;          // optional pointer
    // x-reference on atf_ci.FGitfile.c_noindent prevents copy
    // x-reference on atf_ci.FGitfile.c_scriptfile prevents copy
    // x-reference on atf_ci.FGitfile.c_targsrc prevents copy
    // func:atf_ci.FGitfile..AssignOp
    inline atf_ci::FGitfile& operator =(const atf_ci::FGitfile &rhs) = delete;
    // x-reference on atf_ci.FGitfile.c_noindent prevents copy
    // x-reference on atf_ci.FGitfile.c_scriptfile prevents copy
    // x-reference on atf_ci.FGitfile.c_targsrc prevents copy
    // func:atf_ci.FGitfile..CopyCtor
    inline               FGitfile(const atf_ci::FGitfile &rhs) = delete;
private:
    // func:atf_ci.FGitfile..Ctor
    inline               FGitfile() __attribute__((nothrow));
    // func:atf_ci.FGitfile..Dtor
    inline               ~FGitfile() __attribute__((nothrow));
    friend atf_ci::FGitfile&    gitfile_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend atf_ci::FGitfile*    gitfile_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 gitfile_RemoveAll() __attribute__((nothrow));
    friend void                 gitfile_RemoveLast() __attribute__((nothrow));
};
```

#### atf_ci.FMsgfile - File in repo containing messages that should be normalized during CI
<a href="#atf_ci-fmsgfile"></a>

#### atf_ci.FMsgfile Fields
<a href="#atf_ci-fmsgfile-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_ci.FMsgfile.base|[dev.Msgfile](/txt/ssimdb/dev/msgfile.md)|[Base](/txt/ssimdb/dev/msgfile.md)|||

#### Struct FMsgfile
<a href="#struct-fmsgfile"></a>
*Note:* field ``atf_ci.FMsgfile.base`` has reftype ``base`` so the fields of [dev.Msgfile](/txt/ssimdb/dev/msgfile.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_ci_gen.h](/include/gen/atf_ci_gen.h)
```
struct FMsgfile { // atf_ci.FMsgfile
    algo::Smallstr200   gitfile;   // Name of file containing messages
    bool                strict;    //   false  Delete unrecognizable lines (except comments)
    algo::Comment       comment;   //
private:
    // func:atf_ci.FMsgfile..Ctor
    inline               FMsgfile() __attribute__((nothrow));
    friend atf_ci::FMsgfile&    msgfile_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend atf_ci::FMsgfile*    msgfile_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 msgfile_RemoveAll() __attribute__((nothrow));
    friend void                 msgfile_RemoveLast() __attribute__((nothrow));
};
```

#### atf_ci.FNoindent - Indicates that a file should not be automatically indented
<a href="#atf_ci-fnoindent"></a>

#### atf_ci.FNoindent Fields
<a href="#atf_ci-fnoindent-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_ci.FNoindent.base|[dev.Noindent](/txt/ssimdb/dev/noindent.md)|[Base](/txt/ssimdb/dev/noindent.md)|||

#### Struct FNoindent
<a href="#struct-fnoindent"></a>
*Note:* field ``atf_ci.FNoindent.base`` has reftype ``base`` so the fields of [dev.Noindent](/txt/ssimdb/dev/noindent.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_ci_gen.h](/include/gen/atf_ci_gen.h)
```
struct FNoindent { // atf_ci.FNoindent
    algo::Smallstr200   gitfile;   //
    algo::Comment       comment;   //
    // func:atf_ci.FNoindent..AssignOp
    inline atf_ci::FNoindent& operator =(const atf_ci::FNoindent &rhs) = delete;
    // func:atf_ci.FNoindent..CopyCtor
    inline               FNoindent(const atf_ci::FNoindent &rhs) = delete;
private:
    // func:atf_ci.FNoindent..Ctor
    inline               FNoindent() __attribute__((nothrow));
    // func:atf_ci.FNoindent..Dtor
    inline               ~FNoindent() __attribute__((nothrow));
    friend atf_ci::FNoindent&   noindent_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend atf_ci::FNoindent*   noindent_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 noindent_RemoveAll() __attribute__((nothrow));
    friend void                 noindent_RemoveLast() __attribute__((nothrow));
};
```

#### atf_ci.FNs - Namespace (for in-memory database, protocol, etc)
<a href="#atf_ci-fns"></a>

#### atf_ci.FNs Fields
<a href="#atf_ci-fns-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_ci.FNs.base|[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)|[Base](/txt/ssimdb/dmmeta/ns.md)|||

#### Struct FNs
<a href="#struct-fns"></a>
*Note:* field ``atf_ci.FNs.base`` has reftype ``base`` so the fields of [dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_ci_gen.h](/include/gen/atf_ci_gen.h)
```
struct FNs { // atf_ci.FNs
    atf_ci::FNs*       ind_ns_next;   // hash next
    algo::Smallstr16   ns;            // Namespace name (primary key)
    algo::Smallstr50   nstype;        // Namespace type
    algo::Smallstr50   license;       // Associated license
    algo::Comment      comment;       //
    // func:atf_ci.FNs..AssignOp
    inline atf_ci::FNs&  operator =(const atf_ci::FNs &rhs) = delete;
    // func:atf_ci.FNs..CopyCtor
    inline               FNs(const atf_ci::FNs &rhs) = delete;
private:
    // func:atf_ci.FNs..Ctor
    inline               FNs() __attribute__((nothrow));
    // func:atf_ci.FNs..Dtor
    inline               ~FNs() __attribute__((nothrow));
    friend atf_ci::FNs&         ns_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend atf_ci::FNs*         ns_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 ns_RemoveAll() __attribute__((nothrow));
    friend void                 ns_RemoveLast() __attribute__((nothrow));
};
```

#### atf_ci.FReadme - File containing documentation
<a href="#atf_ci-freadme"></a>

#### atf_ci.FReadme Fields
<a href="#atf_ci-freadme-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_ci.FReadme.base|[dev.Readme](/txt/ssimdb/dev/readme.md)|[Base](/txt/ssimdb/dev/readme.md)|||

#### Struct FReadme
<a href="#struct-freadme"></a>
*Note:* field ``atf_ci.FReadme.base`` has reftype ``base`` so the fields of [dev.Readme](/txt/ssimdb/dev/readme.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_ci_gen.h](/include/gen/atf_ci_gen.h)
```
struct FReadme { // atf_ci.FReadme
    algo::Smallstr200   gitfile;   //
    bool                inl;       //   false
    bool                sandbox;   //   false
    algo::Smallstr100   filter;    //
    algo::Comment       comment;   //
private:
    // func:atf_ci.FReadme..Ctor
    inline               FReadme() __attribute__((nothrow));
    friend atf_ci::FReadme&     readme_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend atf_ci::FReadme*     readme_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 readme_RemoveAll() __attribute__((nothrow));
    friend void                 readme_RemoveLast() __attribute__((nothrow));
};
```

#### atf_ci.FScriptfile - Known script file
<a href="#atf_ci-fscriptfile"></a>

#### atf_ci.FScriptfile Fields
<a href="#atf_ci-fscriptfile-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_ci.FScriptfile.base|[dev.Scriptfile](/txt/ssimdb/dev/scriptfile.md)|[Base](/txt/ssimdb/dev/scriptfile.md)|||

#### Struct FScriptfile
<a href="#struct-fscriptfile"></a>
*Note:* field ``atf_ci.FScriptfile.base`` has reftype ``base`` so the fields of [dev.Scriptfile](/txt/ssimdb/dev/scriptfile.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_ci_gen.h](/include/gen/atf_ci_gen.h)
```
struct FScriptfile { // atf_ci.FScriptfile
    atf_ci::FScriptfile*   ind_scriptfile_next;   // hash next
    algo::Smallstr200      gitfile;               //
    algo::Smallstr50       license;               //
    algo::Comment          comment;               //
    // func:atf_ci.FScriptfile..AssignOp
    inline atf_ci::FScriptfile& operator =(const atf_ci::FScriptfile &rhs) = delete;
    // func:atf_ci.FScriptfile..CopyCtor
    inline               FScriptfile(const atf_ci::FScriptfile &rhs) = delete;
private:
    // func:atf_ci.FScriptfile..Ctor
    inline               FScriptfile() __attribute__((nothrow));
    // func:atf_ci.FScriptfile..Dtor
    inline               ~FScriptfile() __attribute__((nothrow));
    friend atf_ci::FScriptfile& scriptfile_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend atf_ci::FScriptfile* scriptfile_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 scriptfile_RemoveAll() __attribute__((nothrow));
    friend void                 scriptfile_RemoveLast() __attribute__((nothrow));
};
```

#### atf_ci.FSsimfile - File with ssim tuples
<a href="#atf_ci-fssimfile"></a>

#### atf_ci.FSsimfile Fields
<a href="#atf_ci-fssimfile-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_ci.FSsimfile.base|[dmmeta.Ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|[Base](/txt/ssimdb/dmmeta/ssimfile.md)|||

#### Struct FSsimfile
<a href="#struct-fssimfile"></a>
*Note:* field ``atf_ci.FSsimfile.base`` has reftype ``base`` so the fields of [dmmeta.Ssimfile](/txt/ssimdb/dmmeta/ssimfile.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_ci_gen.h](/include/gen/atf_ci_gen.h)
```
struct FSsimfile { // atf_ci.FSsimfile
    atf_ci::FSsimfile*   ind_ssimfile_next;   // hash next
    algo::Smallstr50     ssimfile;            //
    algo::Smallstr100    ctype;               //
    // func:atf_ci.FSsimfile..AssignOp
    inline atf_ci::FSsimfile& operator =(const atf_ci::FSsimfile &rhs) = delete;
    // func:atf_ci.FSsimfile..CopyCtor
    inline               FSsimfile(const atf_ci::FSsimfile &rhs) = delete;
private:
    // func:atf_ci.FSsimfile..Ctor
    inline               FSsimfile() __attribute__((nothrow));
    // func:atf_ci.FSsimfile..Dtor
    inline               ~FSsimfile() __attribute__((nothrow));
    friend atf_ci::FSsimfile&   ssimfile_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend atf_ci::FSsimfile*   ssimfile_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 ssimfile_RemoveLast() __attribute__((nothrow));
};
```

#### atf_ci.FTargsrc - List of sources for target
<a href="#atf_ci-ftargsrc"></a>

#### atf_ci.FTargsrc Fields
<a href="#atf_ci-ftargsrc-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_ci.FTargsrc.base|[dev.Targsrc](/txt/ssimdb/dev/targsrc.md)|[Base](/txt/ssimdb/dev/targsrc.md)|||

#### Struct FTargsrc
<a href="#struct-ftargsrc"></a>
*Note:* field ``atf_ci.FTargsrc.base`` has reftype ``base`` so the fields of [dev.Targsrc](/txt/ssimdb/dev/targsrc.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_ci_gen.h](/include/gen/atf_ci_gen.h)
```
struct FTargsrc { // atf_ci.FTargsrc
    algo::Smallstr100   targsrc;   //
    algo::Comment       comment;   //
    // func:atf_ci.FTargsrc..AssignOp
    inline atf_ci::FTargsrc& operator =(const atf_ci::FTargsrc &rhs) = delete;
    // func:atf_ci.FTargsrc..CopyCtor
    inline               FTargsrc(const atf_ci::FTargsrc &rhs) = delete;
private:
    // func:atf_ci.FTargsrc..Ctor
    inline               FTargsrc() __attribute__((nothrow));
    // func:atf_ci.FTargsrc..Dtor
    inline               ~FTargsrc() __attribute__((nothrow));
    friend atf_ci::FTargsrc&    targsrc_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend atf_ci::FTargsrc*    targsrc_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 targsrc_RemoveAll() __attribute__((nothrow));
    friend void                 targsrc_RemoveLast() __attribute__((nothrow));
};
```

#### atf_ci.File - 
<a href="#atf_ci-file"></a>

#### atf_ci.File Fields
<a href="#atf_ci-file-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_ci.File.file|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Filename|
|atf_ci.File.file_exists|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_ci.File.record_exists|bool|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct File
<a href="#struct-file"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_ci_gen.h](/include/gen/atf_ci_gen.h)
```
struct File { // atf_ci.File
    atf_ci::File*   ind_file_next;   // hash next
    algo::cstring   file;            // Filename
    bool            file_exists;     //   false
    bool            record_exists;   //   false
    // func:atf_ci.File..AssignOp
    inline atf_ci::File& operator =(const atf_ci::File &rhs) = delete;
    // func:atf_ci.File..CopyCtor
    inline               File(const atf_ci::File &rhs) = delete;
private:
    // func:atf_ci.File..Ctor
    inline               File() __attribute__((nothrow));
    // func:atf_ci.File..Dtor
    inline               ~File() __attribute__((nothrow));
    friend atf_ci::File&        file_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend atf_ci::File*        file_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 file_RemoveAll() __attribute__((nothrow));
    friend void                 file_RemoveLast() __attribute__((nothrow));
};
```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Imdb -->


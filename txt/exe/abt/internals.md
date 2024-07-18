## abt - Internals


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
for usage, see [abt - Algo Build Tool - build & link C++ targets](/txt/exe/abt/README.md)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Description -->

### Sources
<a href="#sources"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Sources -->
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/abt/build.cpp](/cpp/abt/build.cpp)|Build dag execution|
|[cpp/abt/disas.cpp](/cpp/abt/disas.cpp)|Disassemble|
|[cpp/abt/main.cpp](/cpp/abt/main.cpp)|Algo Build Tool - Main file|
|[cpp/abt/ood.cpp](/cpp/abt/ood.cpp)||
|[cpp/abt/opt.cpp](/cpp/abt/opt.cpp)|Calculate compiler options|
|[cpp/abt/scan.cpp](/cpp/abt/scan.cpp)||
|[cpp/gen/abt_gen.cpp](/cpp/gen/abt_gen.cpp)||
|[include/abt.h](/include/abt.h)|Main header|
|[include/gen/abt_gen.h](/include/gen/abt_gen.h)||
|[include/gen/abt_gen.inl.h](/include/gen/abt_gen.inl.h)||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Sources -->

### Dependencies
<a href="#dependencies"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Dependencies -->
The build target depends on the following libraries
|Target|Comment|
|---|---|
|[algo_lib](/txt/lib/algo_lib/README.md)|Support library for all executables|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Dependencies -->

### In Memory DB
<a href="#in-memory-db"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Imdb -->
`abt` generated code creates the tables below.
All allocations are done through global `abt::_db` [abt.FDb](#abt-fdb) structure
|Ctype|Ssimfile|Create|Access|
|---|---|---|---|
|[abt.FArch](#abt-farch)|[dev.arch](/txt/ssimdb/dev/arch.md)|FDb.arch (Lary)|arch (Lary, by rowid)|ind_arch (Thash, hash field arch)|
|[abt.FBuilddir](#abt-fbuilddir)|[dev.builddir](/txt/ssimdb/dev/builddir.md)|FDb.builddir (Lary)|builddir (Lary, by rowid)|ind_builddir (Thash, hash field builddir)|c_builddir (Ptr)|
|[abt.FCfg](#abt-fcfg)|[dev.cfg](/txt/ssimdb/dev/cfg.md)|FDb.cfg (Lary)|cfg (Lary, by rowid)|ind_cfg (Thash, hash field cfg)|
|[abt.FCompiler](#abt-fcompiler)|[dev.compiler](/txt/ssimdb/dev/compiler.md)|FDb.compiler (Lary)|compiler (Lary, by rowid)|ind_compiler (Thash, hash field compiler)|
||||FBuilddir.p_compiler (Upptr)|
|[abt.FDb](#abt-fdb)||FDb._db (Global)|
|[abt.FFilestat](#abt-ffilestat)||FDb.filestat (Lary)|filestat (Lary, by rowid)|ind_filestat (Thash, hash field filename)|
|[abt.FInclude](#abt-finclude)|[dev.include](/txt/ssimdb/dev/include.md)|FDb.include (Lary)|include (Lary, by rowid)|ind_include (Thash, hash field include)|
||||FSrcfile.zd_include (Llist)|
|[abt.FNs](#abt-fns)|[dmmeta.ns](/txt/ssimdb/dmmeta/ns.md)|FDb.ns (Lary)|ns (Lary, by rowid)|ind_ns (Thash, hash field ns)|
||||FTarget.p_ns (Upptr)|
|[abt.FSrcfile](#abt-fsrcfile)|[dev.srcfile](/txt/ssimdb/dev/srcfile.md)|FDb.srcfile (Lary)|srcfile (Lary, by rowid)|ind_srcfile (Thash, hash field srcfile)|zs_srcfile_read (Llist)|zd_inclstack (Llist)|
||||FInclude.p_header (Upptr)|
||||FTarget.c_srcfile (Ptrary)|
|[abt.FSyscmd](#abt-fsyscmd)|[dev.syscmd](/txt/ssimdb/dev/syscmd.md)|FDb.syscmd (Lary)|syscmd (Lary, by rowid)|ind_syscmd (Thash, hash field syscmd)|ind_running (Thash, hash field pid)|bh_syscmd (Bheap, sort field execkey)|
||||FSyscmddep.p_child (Upptr)|
||||FSyscmddep.p_parent (Upptr)|
||||FTarget.syscmd_start (Ptr)|
||||FTarget.syscmd_compile (Ptr)|
||||FTarget.syscmd_link (Ptr)|
||||FTarget.syscmd_end (Ptr)|
|[abt.FSyscmddep](#abt-fsyscmddep)|[dev.syscmddep](/txt/ssimdb/dev/syscmddep.md)|FDb.syscmddep (Lary)|syscmddep (Lary, by rowid)|
||||FSyscmd.c_prior (Ptrary)|
||||FSyscmd.c_next (Ptrary)|
|[abt.FSyslib](#abt-fsyslib)|[dev.syslib](/txt/ssimdb/dev/syslib.md)|FDb.syslib (Lary)|syslib (Lary, by rowid)|ind_syslib (Thash, hash field syslib)|
||||FTargsyslib.p_syslib (Upptr)|
|[abt.FTargdep](#abt-ftargdep)|[dev.targdep](/txt/ssimdb/dev/targdep.md)|FDb.targdep (Lary)|targdep (Lary, by rowid)|
||||FTarget.c_targdep (Ptrary)|
|[abt.FTarget](#abt-ftarget)|[dev.target](/txt/ssimdb/dev/target.md)|FDb.target (Lary)|ind_target (Thash, hash field target)|target (Lary, by rowid)|zs_sel_target (Llist)|zs_origsel_target (Llist)|
||||FSrcfile.p_target (Upptr)|
||||FTargdep.p_parent (Upptr)|
||||FTarget.c_alldep (Ptrary)|
||||FTargsrc.p_target (Upptr)|
|[abt.FTargsrc](#abt-ftargsrc)|[dev.targsrc](/txt/ssimdb/dev/targsrc.md)|FDb.targsrc (Lary)|ind_targsrc (Thash, hash field targsrc)|targsrc (Lary, by rowid)|
||||FTarget.c_targsrc (Ptrary)|
|[abt.FTargsyslib](#abt-ftargsyslib)|[dev.targsyslib](/txt/ssimdb/dev/targsyslib.md)|FDb.targsyslib (Lary)|targsyslib (Lary, by rowid)|
||||FTarget.c_targsyslib (Ptrary)|
|[abt.FToolOpt](#abt-ftoolopt)|[dev.tool_opt](/txt/ssimdb/dev/tool_opt.md)|FDb.tool_opt (Lary)|tool_opt (Lary, by rowid)|
|[abt.FUname](#abt-funame)|[dev.uname](/txt/ssimdb/dev/uname.md)|FDb.uname (Lary)|uname (Lary, by rowid)|ind_uname (Thash, hash field uname)|
|[abt.config](#abt-config)||

#### abt.FArch - Machine architecture
<a href="#abt-farch"></a>

#### abt.FArch Fields
<a href="#abt-farch-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|abt.FArch.msghdr|[dev.Arch](/txt/ssimdb/dev/arch.md)|[Base](/txt/ssimdb/dev/arch.md)|||

#### Struct FArch
<a href="#struct-farch"></a>
*Note:* field ``abt.FArch.msghdr`` has reftype ``base`` so the fields of [dev.Arch](/txt/ssimdb/dev/arch.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/abt_gen.h](/include/gen/abt_gen.h)
```
struct FArch { // abt.FArch: Machine architecture
    algo::Smallstr50   arch;            //
    algo::Comment      comment;         //
    abt::FArch*        ind_arch_next;   // hash next
    // func:abt.FArch..AssignOp
    inline abt::FArch&   operator =(const abt::FArch &rhs) = delete;
    // func:abt.FArch..CopyCtor
    inline               FArch(const abt::FArch &rhs) = delete;
private:
    // func:abt.FArch..Ctor
    inline               FArch() __attribute__((nothrow));
    // func:abt.FArch..Dtor
    inline               ~FArch() __attribute__((nothrow));
    friend abt::FArch&          arch_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend abt::FArch*          arch_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 arch_RemoveLast() __attribute__((nothrow));
};
```

#### abt.FBuilddir - Directory where object files/executables go. Determines compile/link options
<a href="#abt-fbuilddir"></a>

#### abt.FBuilddir Fields
<a href="#abt-fbuilddir-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|abt.FBuilddir.base|[dev.Builddir](/txt/ssimdb/dev/builddir.md)|[Base](/txt/ssimdb/dev/builddir.md)|||
|abt.FBuilddir.select|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|abt.FBuilddir.path|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Path for this builddir|
|abt.FBuilddir.lockfile|[algo_lib.FLockfile](/txt/lib/algo_lib/README.md#algo_lib-flockfile)|[Val](/txt/exe/amc/reftypes.md#val)|||
|abt.FBuilddir.R|[algo_lib.Replscope](/txt/lib/algo_lib/README.md#algo_lib-replscope)|[Val](/txt/exe/amc/reftypes.md#val)|||
|abt.FBuilddir.p_compiler|[abt.FCompiler](/txt/exe/abt/internals.md#abt-fcompiler)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FBuilddir
<a href="#struct-fbuilddir"></a>
*Note:* field ``abt.FBuilddir.base`` has reftype ``base`` so the fields of [dev.Builddir](/txt/ssimdb/dev/builddir.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/abt_gen.h](/include/gen/abt_gen.h)
```
struct FBuilddir { // abt.FBuilddir
    algo::Smallstr50      builddir;            // Primary key - uname.compiler.cfg-arch
    algo::Comment         comment;             //
    bool                  select;              //   false
    algo::cstring         path;                // Path for this builddir
    algo_lib::FLockfile   lockfile;            //
    algo_lib::Replscope   R;                   //
    abt::FCompiler*       p_compiler;          // reference to parent row
    abt::FBuilddir*       ind_builddir_next;   // hash next
    // value field abt.FBuilddir.R is not copiable
    // x-reference on abt.FBuilddir.p_compiler prevents copy
    // func:abt.FBuilddir..AssignOp
    abt::FBuilddir&      operator =(const abt::FBuilddir &rhs) = delete;
    // value field abt.FBuilddir.R is not copiable
    // x-reference on abt.FBuilddir.p_compiler prevents copy
    // func:abt.FBuilddir..CopyCtor
    FBuilddir(const abt::FBuilddir &rhs) = delete;
private:
    // func:abt.FBuilddir..Ctor
    inline               FBuilddir() __attribute__((nothrow));
    // func:abt.FBuilddir..Dtor
    inline               ~FBuilddir() __attribute__((nothrow));
    friend abt::FBuilddir&      builddir_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend abt::FBuilddir*      builddir_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 builddir_RemoveAll() __attribute__((nothrow));
    friend void                 builddir_RemoveLast() __attribute__((nothrow));
};
```

#### abt.FCfg - Build configuration
<a href="#abt-fcfg"></a>

#### abt.FCfg Fields
<a href="#abt-fcfg-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|abt.FCfg.msghdr|[dev.Cfg](/txt/ssimdb/dev/cfg.md)|[Base](/txt/ssimdb/dev/cfg.md)|||

#### Struct FCfg
<a href="#struct-fcfg"></a>
*Note:* field ``abt.FCfg.msghdr`` has reftype ``base`` so the fields of [dev.Cfg](/txt/ssimdb/dev/cfg.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/abt_gen.h](/include/gen/abt_gen.h)
```
struct FCfg { // abt.FCfg: Build configuration
    algo::Smallstr50   cfg;            //
    algo::Smallstr5    suffix;         //
    algo::Comment      comment;        //
    abt::FCfg*         ind_cfg_next;   // hash next
    // func:abt.FCfg..AssignOp
    inline abt::FCfg&    operator =(const abt::FCfg &rhs) = delete;
    // func:abt.FCfg..CopyCtor
    inline               FCfg(const abt::FCfg &rhs) = delete;
private:
    // func:abt.FCfg..Ctor
    inline               FCfg() __attribute__((nothrow));
    // func:abt.FCfg..Dtor
    inline               ~FCfg() __attribute__((nothrow));
    friend abt::FCfg&           cfg_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend abt::FCfg*           cfg_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 cfg_RemoveLast() __attribute__((nothrow));
};
```

#### abt.FCompiler - Compiler
<a href="#abt-fcompiler"></a>

#### abt.FCompiler Fields
<a href="#abt-fcompiler-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|abt.FCompiler.msghdr|[dev.Compiler](/txt/ssimdb/dev/compiler.md)|[Base](/txt/ssimdb/dev/compiler.md)|||

#### Struct FCompiler
<a href="#struct-fcompiler"></a>
*Note:* field ``abt.FCompiler.msghdr`` has reftype ``base`` so the fields of [dev.Compiler](/txt/ssimdb/dev/compiler.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/abt_gen.h](/include/gen/abt_gen.h)
```
struct FCompiler { // abt.FCompiler: Compiler
    algo::Smallstr50   compiler;            //
    algo::Smallstr50   ranlib;              //
    algo::Smallstr50   ar;                  //
    algo::Smallstr50   link;                //
    algo::Smallstr50   libext;              //
    algo::Smallstr20   exeext;              //
    algo::Smallstr20   pchext;              //
    algo::Smallstr20   objext;              //
    algo::Smallstr50   rc;                  //
    algo::Comment      comment;             //
    abt::FCompiler*    ind_compiler_next;   // hash next
    // func:abt.FCompiler..AssignOp
    abt::FCompiler&      operator =(const abt::FCompiler &rhs) = delete;
    // func:abt.FCompiler..CopyCtor
    FCompiler(const abt::FCompiler &rhs) = delete;
private:
    // func:abt.FCompiler..Ctor
    inline               FCompiler() __attribute__((nothrow));
    // func:abt.FCompiler..Dtor
    inline               ~FCompiler() __attribute__((nothrow));
    friend abt::FCompiler&      compiler_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend abt::FCompiler*      compiler_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 compiler_RemoveLast() __attribute__((nothrow));
};
```

#### abt.FDb - In-memory database for abt
<a href="#abt-fdb"></a>

#### abt.FDb Fields
<a href="#abt-fdb-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|abt.FDb.lpool|u8|[Lpool](/txt/exe/amc/reftypes.md#lpool)||private memory pool|
|abt.FDb.cmdline|[command.abt](/txt/protocol/command/README.md#command-abt)|[Val](/txt/exe/amc/reftypes.md#val)|||
|abt.FDb.srcfile|[abt.FSrcfile](/txt/exe/abt/internals.md#abt-fsrcfile)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|abt.FDb.targdep|[abt.FTargdep](/txt/exe/abt/internals.md#abt-ftargdep)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|abt.FDb.tool_opt|[abt.FToolOpt](/txt/exe/abt/internals.md#abt-ftoolopt)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|abt.FDb.ind_target|[abt.FTarget](/txt/exe/abt/internals.md#abt-ftarget)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|abt.FDb.target|[abt.FTarget](/txt/exe/abt/internals.md#abt-ftarget)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|abt.FDb.ind_targsrc|[abt.FTargsrc](/txt/exe/abt/internals.md#abt-ftargsrc)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|abt.FDb.targsrc|[abt.FTargsrc](/txt/exe/abt/internals.md#abt-ftargsrc)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|abt.FDb.syscmddep|[abt.FSyscmddep](/txt/exe/abt/internals.md#abt-fsyscmddep)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|abt.FDb.syscmd|[abt.FSyscmd](/txt/exe/abt/internals.md#abt-fsyscmd)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|abt.FDb._db|[abt.FDb](/txt/exe/abt/internals.md#abt-fdb)|[Global](/txt/exe/amc/reftypes.md#global)|||
|abt.FDb.ind_syscmd|[abt.FSyscmd](/txt/exe/abt/internals.md#abt-fsyscmd)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|abt.FDb.ind_running|[abt.FSyscmd](/txt/exe/abt/internals.md#abt-fsyscmd)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|abt.FDb.ind_srcfile|[abt.FSrcfile](/txt/exe/abt/internals.md#abt-fsrcfile)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|abt.FDb.cfg|[abt.FCfg](/txt/exe/abt/internals.md#abt-fcfg)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|abt.FDb.ind_cfg|[abt.FCfg](/txt/exe/abt/internals.md#abt-fcfg)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|abt.FDb.uname|[abt.FUname](/txt/exe/abt/internals.md#abt-funame)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|abt.FDb.ind_uname|[abt.FUname](/txt/exe/abt/internals.md#abt-funame)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|abt.FDb.compiler|[abt.FCompiler](/txt/exe/abt/internals.md#abt-fcompiler)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|abt.FDb.ind_compiler|[abt.FCompiler](/txt/exe/abt/internals.md#abt-fcompiler)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|abt.FDb.arch|[abt.FArch](/txt/exe/abt/internals.md#abt-farch)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|abt.FDb.ind_arch|[abt.FArch](/txt/exe/abt/internals.md#abt-farch)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|abt.FDb.bh_syscmd|[abt.FSyscmd](/txt/exe/abt/internals.md#abt-fsyscmd)|[Bheap](/txt/exe/amc/reftypes.md#bheap)|||
|abt.FDb.zs_srcfile_read|[abt.FSrcfile](/txt/exe/abt/internals.md#abt-fsrcfile)|[Llist](/txt/exe/amc/reftypes.md#llist)||List of source files remaining to be read.|
|abt.FDb.zs_sel_target|[abt.FTarget](/txt/exe/abt/internals.md#abt-ftarget)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|abt.FDb.report|[report.abt](/txt/protocol/report/README.md#report-abt)|[Val](/txt/exe/amc/reftypes.md#val)||Final report|
|abt.FDb.targsyslib|[abt.FTargsyslib](/txt/exe/abt/internals.md#abt-ftargsyslib)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|abt.FDb.syslib|[abt.FSyslib](/txt/exe/abt/internals.md#abt-fsyslib)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|abt.FDb.ind_syslib|[abt.FSyslib](/txt/exe/abt/internals.md#abt-fsyslib)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|abt.FDb.include|[abt.FInclude](/txt/exe/abt/internals.md#abt-finclude)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|abt.FDb.ind_include|[abt.FInclude](/txt/exe/abt/internals.md#abt-finclude)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|abt.FDb.sysincl|[algo.cstring](/txt/protocol/algo/cstring.md)|[Tary](/txt/exe/amc/reftypes.md#tary)||Array of system include dirs to try|
|abt.FDb.zs_origsel_target|[abt.FTarget](/txt/exe/abt/internals.md#abt-ftarget)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|abt.FDb.ns|[abt.FNs](/txt/exe/abt/internals.md#abt-fns)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|abt.FDb.ind_ns|[abt.FNs](/txt/exe/abt/internals.md#abt-fns)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|abt.FDb.ccache|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|abt.FDb.gcache|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|abt.FDb.filestat|[abt.FFilestat](/txt/exe/abt/internals.md#abt-ffilestat)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|abt.FDb.ind_filestat|[abt.FFilestat](/txt/exe/abt/internals.md#abt-ffilestat)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|abt.FDb.builddir|[abt.FBuilddir](/txt/exe/abt/internals.md#abt-fbuilddir)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|abt.FDb.ind_builddir|[abt.FBuilddir](/txt/exe/abt/internals.md#abt-fbuilddir)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|abt.FDb.c_builddir|[abt.FBuilddir](/txt/exe/abt/internals.md#abt-fbuilddir)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|abt.FDb.zd_inclstack|[abt.FSrcfile](/txt/exe/abt/internals.md#abt-fsrcfile)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|abt.FDb.tty|bool|[Val](/txt/exe/amc/reftypes.md#val)||Output|

#### Struct FDb
<a href="#struct-fdb"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/abt_gen.h](/include/gen/abt_gen.h)
```
struct FDb { // abt.FDb: In-memory database for abt
    lpool_Lpblock*      lpool_free[36];               // Lpool levels
    command::abt        cmdline;                      //
    abt::FSrcfile*      srcfile_lary[32];             // level array
    i32                 srcfile_n;                    // number of elements in array
    abt::FTargdep*      targdep_lary[32];             // level array
    i32                 targdep_n;                    // number of elements in array
    abt::FToolOpt*      tool_opt_lary[32];            // level array
    i32                 tool_opt_n;                   // number of elements in array
    abt::FTarget**      ind_target_buckets_elems;     // pointer to bucket array
    i32                 ind_target_buckets_n;         // number of elements in bucket array
    i32                 ind_target_n;                 // number of elements in the hash table
    abt::FTarget*       target_lary[32];              // level array
    i32                 target_n;                     // number of elements in array
    abt::FTargsrc**     ind_targsrc_buckets_elems;    // pointer to bucket array
    i32                 ind_targsrc_buckets_n;        // number of elements in bucket array
    i32                 ind_targsrc_n;                // number of elements in the hash table
    abt::FTargsrc*      targsrc_lary[32];             // level array
    i32                 targsrc_n;                    // number of elements in array
    abt::FSyscmddep*    syscmddep_lary[32];           // level array
    i32                 syscmddep_n;                  // number of elements in array
    abt::FSyscmd*       syscmd_lary[32];              // level array
    i32                 syscmd_n;                     // number of elements in array
    abt::FSyscmd**      ind_syscmd_buckets_elems;     // pointer to bucket array
    i32                 ind_syscmd_buckets_n;         // number of elements in bucket array
    i32                 ind_syscmd_n;                 // number of elements in the hash table
    abt::FSyscmd**      ind_running_buckets_elems;    // pointer to bucket array
    i32                 ind_running_buckets_n;        // number of elements in bucket array
    i32                 ind_running_n;                // number of elements in the hash table
    abt::FSrcfile**     ind_srcfile_buckets_elems;    // pointer to bucket array
    i32                 ind_srcfile_buckets_n;        // number of elements in bucket array
    i32                 ind_srcfile_n;                // number of elements in the hash table
    abt::FCfg*          cfg_lary[32];                 // level array
    i32                 cfg_n;                        // number of elements in array
    abt::FCfg**         ind_cfg_buckets_elems;        // pointer to bucket array
    i32                 ind_cfg_buckets_n;            // number of elements in bucket array
    i32                 ind_cfg_n;                    // number of elements in the hash table
    abt::FUname*        uname_lary[32];               // level array
    i32                 uname_n;                      // number of elements in array
    abt::FUname**       ind_uname_buckets_elems;      // pointer to bucket array
    i32                 ind_uname_buckets_n;          // number of elements in bucket array
    i32                 ind_uname_n;                  // number of elements in the hash table
    abt::FCompiler*     compiler_lary[32];            // level array
    i32                 compiler_n;                   // number of elements in array
    abt::FCompiler**    ind_compiler_buckets_elems;   // pointer to bucket array
    i32                 ind_compiler_buckets_n;       // number of elements in bucket array
    i32                 ind_compiler_n;               // number of elements in the hash table
    abt::FArch*         arch_lary[32];                // level array
    i32                 arch_n;                       // number of elements in array
    abt::FArch**        ind_arch_buckets_elems;       // pointer to bucket array
    i32                 ind_arch_buckets_n;           // number of elements in bucket array
    i32                 ind_arch_n;                   // number of elements in the hash table
    abt::FSyscmd**      bh_syscmd_elems;              // binary heap by execkey
    i32                 bh_syscmd_n;                  // number of elements in the heap
    i32                 bh_syscmd_max;                // max elements in bh_syscmd_elems
    abt::FSrcfile*      zs_srcfile_read_head;         // zero-terminated singly linked list
    abt::FSrcfile*      zs_srcfile_read_tail;         // pointer to last element
    abt::FTarget*       zs_sel_target_head;           // zero-terminated singly linked list
    i32                 zs_sel_target_n;              // zero-terminated singly linked list
    abt::FTarget*       zs_sel_target_tail;           // pointer to last element
    report::abt         report;                       // Final report
    abt::FTargsyslib*   targsyslib_lary[32];          // level array
    i32                 targsyslib_n;                 // number of elements in array
    abt::FSyslib*       syslib_lary[32];              // level array
    i32                 syslib_n;                     // number of elements in array
    abt::FSyslib**      ind_syslib_buckets_elems;     // pointer to bucket array
    i32                 ind_syslib_buckets_n;         // number of elements in bucket array
    i32                 ind_syslib_n;                 // number of elements in the hash table
    abt::FInclude*      include_lary[32];             // level array
    i32                 include_n;                    // number of elements in array
    abt::FInclude**     ind_include_buckets_elems;    // pointer to bucket array
    i32                 ind_include_buckets_n;        // number of elements in bucket array
    i32                 ind_include_n;                // number of elements in the hash table
    algo::cstring*      sysincl_elems;                // pointer to elements
    u32                 sysincl_n;                    // number of elements in array
    u32                 sysincl_max;                  // max. capacity of array before realloc
    abt::FTarget*       zs_origsel_target_head;       // zero-terminated singly linked list
    abt::FTarget*       zs_origsel_target_tail;       // pointer to last element
    abt::FNs*           ns_lary[32];                  // level array
    i32                 ns_n;                         // number of elements in array
    abt::FNs**          ind_ns_buckets_elems;         // pointer to bucket array
    i32                 ind_ns_buckets_n;             // number of elements in bucket array
    i32                 ind_ns_n;                     // number of elements in the hash table
    bool                ccache;                       //   false
    bool                gcache;                       //   false
    abt::FFilestat*     filestat_lary[32];            // level array
    i32                 filestat_n;                   // number of elements in array
    abt::FFilestat**    ind_filestat_buckets_elems;   // pointer to bucket array
    i32                 ind_filestat_buckets_n;       // number of elements in bucket array
    i32                 ind_filestat_n;               // number of elements in the hash table
    abt::FBuilddir*     builddir_lary[32];            // level array
    i32                 builddir_n;                   // number of elements in array
    abt::FBuilddir**    ind_builddir_buckets_elems;   // pointer to bucket array
    i32                 ind_builddir_buckets_n;       // number of elements in bucket array
    i32                 ind_builddir_n;               // number of elements in the hash table
    abt::FBuilddir*     c_builddir;                   // optional pointer
    abt::FSrcfile*      zd_inclstack_head;            // zero-terminated doubly linked list
    i32                 zd_inclstack_n;               // zero-terminated doubly linked list
    abt::FSrcfile*      zd_inclstack_tail;            // pointer to last element
    bool                tty;                          //   false  Output
    abt::trace          trace;                        //
};
```

#### abt.FFilestat - Stat cache
<a href="#abt-ffilestat"></a>

#### abt.FFilestat Fields
<a href="#abt-ffilestat-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|abt.FFilestat.filename|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|abt.FFilestat.modtime|[algo.UnTime](/txt/protocol/algo/UnTime.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|abt.FFilestat.size|u64|[Val](/txt/exe/amc/reftypes.md#val)|||
|abt.FFilestat.isdir|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|abt.FFilestat.exists|bool|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FFilestat
<a href="#struct-ffilestat"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/abt_gen.h](/include/gen/abt_gen.h)
```
struct FFilestat { // abt.FFilestat: Stat cache
    abt::FFilestat*   ind_filestat_next;   // hash next
    algo::cstring     filename;            //
    algo::UnTime      modtime;             //
    u64               size;                //   0
    bool              isdir;               //   false
    bool              exists;              //   false
    // func:abt.FFilestat..AssignOp
    inline abt::FFilestat& operator =(const abt::FFilestat &rhs) = delete;
    // func:abt.FFilestat..CopyCtor
    inline               FFilestat(const abt::FFilestat &rhs) = delete;
private:
    // func:abt.FFilestat..Ctor
    inline               FFilestat() __attribute__((nothrow));
    // func:abt.FFilestat..Dtor
    inline               ~FFilestat() __attribute__((nothrow));
    friend abt::FFilestat&      filestat_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend abt::FFilestat*      filestat_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 filestat_RemoveAll() __attribute__((nothrow));
    friend void                 filestat_RemoveLast() __attribute__((nothrow));
};
```

#### abt.FInclude - A site where one file includes another
<a href="#abt-finclude"></a>

#### abt.FInclude Fields
<a href="#abt-finclude-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|abt.FInclude.msghdr|[dev.Include](/txt/ssimdb/dev/include.md)|[Base](/txt/ssimdb/dev/include.md)|||
|abt.FInclude.p_header|[abt.FSrcfile](/txt/exe/abt/internals.md#abt-fsrcfile)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|abt.FInclude.wantprint|bool|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FInclude
<a href="#struct-finclude"></a>
*Note:* field ``abt.FInclude.msghdr`` has reftype ``base`` so the fields of [dev.Include](/txt/ssimdb/dev/include.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/abt_gen.h](/include/gen/abt_gen.h)
```
struct FInclude { // abt.FInclude
    abt::FInclude*      ind_include_next;   // hash next
    algo::Smallstr200   include;            //
    bool                sys;                //   false
    algo::Comment       comment;            //
    abt::FSrcfile*      p_header;           // reference to parent row
    bool                wantprint;          //   false
    abt::FInclude*      zd_include_next;    // zslist link; -1 means not-in-list
    abt::FInclude*      zd_include_prev;    // previous element
    // x-reference on abt.FInclude.p_header prevents copy
    // func:abt.FInclude..AssignOp
    inline abt::FInclude& operator =(const abt::FInclude &rhs) = delete;
    // x-reference on abt.FInclude.p_header prevents copy
    // func:abt.FInclude..CopyCtor
    inline               FInclude(const abt::FInclude &rhs) = delete;
private:
    // func:abt.FInclude..Ctor
    inline               FInclude() __attribute__((nothrow));
    // func:abt.FInclude..Dtor
    inline               ~FInclude() __attribute__((nothrow));
    friend abt::FInclude&       include_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend abt::FInclude*       include_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 include_RemoveLast() __attribute__((nothrow));
};
```

#### abt.FNs - Namespace (for in-memory database, protocol, etc)
<a href="#abt-fns"></a>

#### abt.FNs Fields
<a href="#abt-fns-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|abt.FNs.base|[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)|[Base](/txt/ssimdb/dmmeta/ns.md)|||

#### Struct FNs
<a href="#struct-fns"></a>
*Note:* field ``abt.FNs.base`` has reftype ``base`` so the fields of [dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/abt_gen.h](/include/gen/abt_gen.h)
```
struct FNs { // abt.FNs
    abt::FNs*          ind_ns_next;   // hash next
    algo::Smallstr16   ns;            // Namespace name (primary key)
    algo::Smallstr50   nstype;        // Namespace type
    algo::Smallstr50   license;       // Associated license
    algo::Comment      comment;       //
    // func:abt.FNs..AssignOp
    inline abt::FNs&     operator =(const abt::FNs &rhs) = delete;
    // func:abt.FNs..CopyCtor
    inline               FNs(const abt::FNs &rhs) = delete;
private:
    // func:abt.FNs..Ctor
    inline               FNs() __attribute__((nothrow));
    // func:abt.FNs..Dtor
    inline               ~FNs() __attribute__((nothrow));
    friend abt::FNs&            ns_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend abt::FNs*            ns_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 ns_RemoveLast() __attribute__((nothrow));
};
```

#### abt.FSrcfile - Source file or header (key is pathname)
<a href="#abt-fsrcfile"></a>

#### abt.FSrcfile Fields
<a href="#abt-fsrcfile-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|abt.FSrcfile.msghdr|[dev.Srcfile](/txt/ssimdb/dev/srcfile.md)|[Base](/txt/ssimdb/dev/srcfile.md)|||
|abt.FSrcfile.mod_time|[algo.UnTime](/txt/protocol/algo/UnTime.md)|[Val](/txt/exe/amc/reftypes.md#val)||File's modification time|
|abt.FSrcfile.p_target|[abt.FTarget](/txt/exe/abt/internals.md#abt-ftarget)|[Upptr](/txt/exe/amc/reftypes.md#upptr)||Optional parent target (NULL for common headers)|
|abt.FSrcfile.objkey|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Path to file, / replaced with .|
|abt.FSrcfile.cum_modtime|[algo.UnTime](/txt/protocol/algo/UnTime.md)|[Val](/txt/exe/amc/reftypes.md#val)||Cumulative modtime of src and all headers|
|abt.FSrcfile.ood|bool|[Val](/txt/exe/amc/reftypes.md#val)||Out-of-date flag|
|abt.FSrcfile.cum_mod_visited|bool|[Val](/txt/exe/amc/reftypes.md#val)||Lockout for topo sort|
|abt.FSrcfile.line_n|u64|[Val](/txt/exe/amc/reftypes.md#val)||Number of lines attributed to this file|
|abt.FSrcfile.srcfile_visited|bool|[Val](/txt/exe/amc/reftypes.md#val)||Lockout for srcfiles collection|
|abt.FSrcfile.zd_include|[abt.FInclude](/txt/exe/abt/internals.md#abt-finclude)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|abt.FSrcfile.printed|bool|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FSrcfile
<a href="#struct-fsrcfile"></a>
*Note:* field ``abt.FSrcfile.msghdr`` has reftype ``base`` so the fields of [dev.Srcfile](/txt/ssimdb/dev/srcfile.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/abt_gen.h](/include/gen/abt_gen.h)
```
struct FSrcfile { // abt.FSrcfile: Source file or header (key is pathname)
    abt::FSrcfile*      ind_srcfile_next;       // hash next
    abt::FSrcfile*      zs_srcfile_read_next;   // zslist link; -1 means not-in-list
    abt::FSrcfile*      zd_inclstack_next;      // zslist link; -1 means not-in-list
    abt::FSrcfile*      zd_inclstack_prev;      // previous element
    algo::Smallstr200   srcfile;                //
    algo::UnTime        mod_time;               // File's modification time
    abt::FTarget*       p_target;               // reference to parent row
    algo::cstring       objkey;                 // Path to file, / replaced with .
    algo::UnTime        cum_modtime;            // Cumulative modtime of src and all headers
    bool                ood;                    //   false  Out-of-date flag
    bool                cum_mod_visited;        //   false  Lockout for topo sort
    u64                 line_n;                 //   0  Number of lines attributed to this file
    bool                srcfile_visited;        //   false  Lockout for srcfiles collection
    abt::FInclude*      zd_include_head;        // zero-terminated doubly linked list
    i32                 zd_include_n;           // zero-terminated doubly linked list
    abt::FInclude*      zd_include_tail;        // pointer to last element
    bool                printed;                //   false
    // reftype Llist of abt.FSrcfile.zd_include prohibits copy
    // func:abt.FSrcfile..AssignOp
    abt::FSrcfile&       operator =(const abt::FSrcfile &rhs) = delete;
    // reftype Llist of abt.FSrcfile.zd_include prohibits copy
    // func:abt.FSrcfile..CopyCtor
    FSrcfile(const abt::FSrcfile &rhs) = delete;
private:
    // func:abt.FSrcfile..Ctor
    inline               FSrcfile() __attribute__((nothrow));
    // func:abt.FSrcfile..Dtor
    inline               ~FSrcfile() __attribute__((nothrow));
    friend abt::FSrcfile&       srcfile_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend abt::FSrcfile*       srcfile_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 srcfile_RemoveAll() __attribute__((nothrow));
    friend void                 srcfile_RemoveLast() __attribute__((nothrow));
};
```

#### abt.FSyscmd - A build command
<a href="#abt-fsyscmd"></a>

#### abt.FSyscmd Fields
<a href="#abt-fsyscmd-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|abt.FSyscmd.msghdr|[dev.Syscmd](/txt/ssimdb/dev/syscmd.md)|[Base](/txt/ssimdb/dev/syscmd.md)|||
|abt.FSyscmd.fstdout|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||filename for stdout output|
|abt.FSyscmd.fstderr|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||filename for stderr output|
|abt.FSyscmd.c_prior|[abt.FSyscmddep](/txt/exe/abt/internals.md#abt-fsyscmddep)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|abt.FSyscmd.c_next|[abt.FSyscmddep](/txt/exe/abt/internals.md#abt-fsyscmddep)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|abt.FSyscmd.rowid|i32|[Val](/txt/exe/amc/reftypes.md#val)|||
|abt.FSyscmd.fd_stdout|[algo_lib.FFildes](/txt/lib/algo_lib/README.md#algo_lib-ffildes)|[Val](/txt/exe/amc/reftypes.md#val)||fd for stdout|
|abt.FSyscmd.fd_stderr|[algo_lib.FFildes](/txt/lib/algo_lib/README.md#algo_lib-ffildes)|[Val](/txt/exe/amc/reftypes.md#val)||fd for stderr|
|abt.FSyscmd.line_n|u64|[Val](/txt/exe/amc/reftypes.md#val)||Number of lines attributed to this command|
|abt.FSyscmd.outfile|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|abt.FSyscmd.redirect|bool|[Val](/txt/exe/amc/reftypes.md#val)|true||

#### Struct FSyscmd
<a href="#struct-fsyscmd"></a>
*Note:* field ``abt.FSyscmd.msghdr`` has reftype ``base`` so the fields of [dev.Syscmd](/txt/ssimdb/dev/syscmd.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/abt_gen.h](/include/gen/abt_gen.h)
```
struct FSyscmd { // abt.FSyscmd: A build command
    abt::FSyscmd*       ind_syscmd_next;    // hash next
    abt::FSyscmd*       ind_running_next;   // hash next
    i32                 bh_syscmd_idx;      // index in heap; -1 means not-in-heap
    i64                 syscmd;             //   0  Step number
    algo::cstring       command;            // Command to execute
    i32                 pid;                //   0  PID, if running
    i32                 status;             //   0  Exit status (if command has completed)
    i32                 nprereq;            //   0  Number of live pre-requisites
    bool                fail_prereq;        //   false  Set if one of pre-requisites fails
    bool                completed;          //   false  Completed?
    i32                 maxtime;            //   0  Optional max running time (used to use SIGALRM)
    algo::cstring       fstdout;            // filename for stdout output
    algo::cstring       fstderr;            // filename for stderr output
    abt::FSyscmddep**   c_prior_elems;      // array of pointers
    u32                 c_prior_n;          // array of pointers
    u32                 c_prior_max;        // capacity of allocated array
    abt::FSyscmddep**   c_next_elems;       // array of pointers
    u32                 c_next_n;           // array of pointers
    u32                 c_next_max;         // capacity of allocated array
    i32                 rowid;              //   0
    algo_lib::FFildes   fd_stdout;          // fd for stdout
    algo_lib::FFildes   fd_stderr;          // fd for stderr
    u64                 line_n;             //   0  Number of lines attributed to this command
    algo::cstring       outfile;            //
    bool                redirect;           //   true
    // reftype Ptrary of abt.FSyscmd.c_prior prohibits copy
    // reftype Ptrary of abt.FSyscmd.c_next prohibits copy
    // func:abt.FSyscmd..AssignOp
    abt::FSyscmd&        operator =(const abt::FSyscmd &rhs) = delete;
    // reftype Ptrary of abt.FSyscmd.c_prior prohibits copy
    // reftype Ptrary of abt.FSyscmd.c_next prohibits copy
    // func:abt.FSyscmd..CopyCtor
    FSyscmd(const abt::FSyscmd &rhs) = delete;
private:
    // func:abt.FSyscmd..Ctor
    inline               FSyscmd() __attribute__((nothrow));
    // func:abt.FSyscmd..Dtor
    inline               ~FSyscmd() __attribute__((nothrow));
    friend abt::FSyscmd&        syscmd_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend abt::FSyscmd*        syscmd_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 syscmd_RemoveLast() __attribute__((nothrow));
};
```

#### abt.FSyscmddep - Dependency between commands
<a href="#abt-fsyscmddep"></a>

#### abt.FSyscmddep Fields
<a href="#abt-fsyscmddep-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|abt.FSyscmddep.msghdr|[dev.Syscmddep](/txt/ssimdb/dev/syscmddep.md)|[Base](/txt/ssimdb/dev/syscmddep.md)|||
|abt.FSyscmddep.p_child|[abt.FSyscmd](/txt/exe/abt/internals.md#abt-fsyscmd)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|abt.FSyscmddep.p_parent|[abt.FSyscmd](/txt/exe/abt/internals.md#abt-fsyscmd)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FSyscmddep
<a href="#struct-fsyscmddep"></a>
*Note:* field ``abt.FSyscmddep.msghdr`` has reftype ``base`` so the fields of [dev.Syscmddep](/txt/ssimdb/dev/syscmddep.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/abt_gen.h](/include/gen/abt_gen.h)
```
struct FSyscmddep { // abt.FSyscmddep: Dependency between commands
    i64             child;                   //   0
    i64             parent;                  //   0
    abt::FSyscmd*   p_child;                 // reference to parent row
    abt::FSyscmd*   p_parent;                // reference to parent row
    bool            syscmd_c_next_in_ary;    //   false  membership flag
    bool            syscmd_c_prior_in_ary;   //   false  membership flag
    // x-reference on abt.FSyscmddep.p_child prevents copy
    // x-reference on abt.FSyscmddep.p_parent prevents copy
    // func:abt.FSyscmddep..AssignOp
    inline abt::FSyscmddep& operator =(const abt::FSyscmddep &rhs) = delete;
    // x-reference on abt.FSyscmddep.p_child prevents copy
    // x-reference on abt.FSyscmddep.p_parent prevents copy
    // func:abt.FSyscmddep..CopyCtor
    inline               FSyscmddep(const abt::FSyscmddep &rhs) = delete;
private:
    // func:abt.FSyscmddep..Ctor
    inline               FSyscmddep() __attribute__((nothrow));
    // func:abt.FSyscmddep..Dtor
    inline               ~FSyscmddep() __attribute__((nothrow));
    friend abt::FSyscmddep&     syscmddep_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend abt::FSyscmddep*     syscmddep_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 syscmddep_RemoveLast() __attribute__((nothrow));
};
```

#### abt.FSyslib - System library
<a href="#abt-fsyslib"></a>

#### abt.FSyslib Fields
<a href="#abt-fsyslib-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|abt.FSyslib.msghdr|[dev.Syslib](/txt/ssimdb/dev/syslib.md)|[Base](/txt/ssimdb/dev/syslib.md)|||

#### Struct FSyslib
<a href="#struct-fsyslib"></a>
*Note:* field ``abt.FSyslib.msghdr`` has reftype ``base`` so the fields of [dev.Syslib](/txt/ssimdb/dev/syslib.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/abt_gen.h](/include/gen/abt_gen.h)
```
struct FSyslib { // abt.FSyslib: System library
    abt::FSyslib*      ind_syslib_next;   // hash next
    algo::Smallstr50   syslib;            //
    algo::Comment      comment;           //
    // func:abt.FSyslib..AssignOp
    inline abt::FSyslib& operator =(const abt::FSyslib &rhs) = delete;
    // func:abt.FSyslib..CopyCtor
    inline               FSyslib(const abt::FSyslib &rhs) = delete;
private:
    // func:abt.FSyslib..Ctor
    inline               FSyslib() __attribute__((nothrow));
    // func:abt.FSyslib..Dtor
    inline               ~FSyslib() __attribute__((nothrow));
    friend abt::FSyslib&        syslib_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend abt::FSyslib*        syslib_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 syslib_RemoveLast() __attribute__((nothrow));
};
```

#### abt.FTargdep - Dependency between targets
<a href="#abt-ftargdep"></a>

#### abt.FTargdep Fields
<a href="#abt-ftargdep-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|abt.FTargdep.msghdr|[dev.Targdep](/txt/ssimdb/dev/targdep.md)|[Base](/txt/ssimdb/dev/targdep.md)|||
|abt.FTargdep.p_parent|[abt.FTarget](/txt/exe/abt/internals.md#abt-ftarget)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FTargdep
<a href="#struct-ftargdep"></a>
*Note:* field ``abt.FTargdep.msghdr`` has reftype ``base`` so the fields of [dev.Targdep](/txt/ssimdb/dev/targdep.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/abt_gen.h](/include/gen/abt_gen.h)
```
struct FTargdep { // abt.FTargdep: Dependency between targets
    algo::Smallstr50   targdep;                   // Primary key - target.parent
    algo::Comment      comment;                   //
    abt::FTarget*      p_parent;                  // reference to parent row
    bool               target_c_targdep_in_ary;   //   false  membership flag
    // x-reference on abt.FTargdep.p_parent prevents copy
    // func:abt.FTargdep..AssignOp
    inline abt::FTargdep& operator =(const abt::FTargdep &rhs) = delete;
    // x-reference on abt.FTargdep.p_parent prevents copy
    // func:abt.FTargdep..CopyCtor
    inline               FTargdep(const abt::FTargdep &rhs) = delete;
private:
    // func:abt.FTargdep..Ctor
    inline               FTargdep() __attribute__((nothrow));
    // func:abt.FTargdep..Dtor
    inline               ~FTargdep() __attribute__((nothrow));
    friend abt::FTargdep&       targdep_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend abt::FTargdep*       targdep_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 targdep_RemoveLast() __attribute__((nothrow));
};
```

#### abt.FTarget - Build target
<a href="#abt-ftarget"></a>

#### abt.FTarget Fields
<a href="#abt-ftarget-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|abt.FTarget.msghdr|[dev.Target](/txt/ssimdb/dev/target.md)|[Base](/txt/ssimdb/dev/target.md)|||
|abt.FTarget.cum_modtime|[algo.UnTime](/txt/protocol/algo/UnTime.md)|[Val](/txt/exe/amc/reftypes.md#val)||Cumulative modtime of all source files in this target|
|abt.FTarget.ood|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|File is out of date|
|abt.FTarget.syscmd_start|[abt.FSyscmd](/txt/exe/abt/internals.md#abt-fsyscmd)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|abt.FTarget.syscmd_compile|[abt.FSyscmd](/txt/exe/abt/internals.md#abt-fsyscmd)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|abt.FTarget.syscmd_link|[abt.FSyscmd](/txt/exe/abt/internals.md#abt-fsyscmd)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|abt.FTarget.syscmd_end|[abt.FSyscmd](/txt/exe/abt/internals.md#abt-fsyscmd)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|abt.FTarget.c_targsrc|[abt.FTargsrc](/txt/exe/abt/internals.md#abt-ftargsrc)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|abt.FTarget.c_srcfile|[abt.FSrcfile](/txt/exe/abt/internals.md#abt-fsrcfile)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|abt.FTarget.c_targdep|[abt.FTargdep](/txt/exe/abt/internals.md#abt-ftargdep)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|abt.FTarget.ood_visited|bool|[Val](/txt/exe/amc/reftypes.md#val)||Lockout for topo sort|
|abt.FTarget.c_targsyslib|[abt.FTargsyslib](/txt/exe/abt/internals.md#abt-ftargsyslib)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|abt.FTarget.c_alldep|[abt.FTarget](/txt/exe/abt/internals.md#abt-ftarget)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)||Transitive closure of all dependencies for this target|
|abt.FTarget.p_ns|[abt.FNs](/txt/exe/abt/internals.md#abt-fns)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|abt.FTarget.libdep_visited|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|abt.FTarget.out_modtime|[algo.UnTime](/txt/protocol/algo/UnTime.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|abt.FTarget.origsel|bool|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FTarget
<a href="#struct-ftarget"></a>
*Note:* field ``abt.FTarget.msghdr`` has reftype ``base`` so the fields of [dev.Target](/txt/ssimdb/dev/target.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/abt_gen.h](/include/gen/abt_gen.h)
```
struct FTarget { // abt.FTarget: Build target
    abt::FTarget*        ind_target_next;          // hash next
    abt::FTarget*        zs_sel_target_next;       // zslist link; -1 means not-in-list
    abt::FTarget*        zs_origsel_target_next;   // zslist link; -1 means not-in-list
    algo::Smallstr16     target;                   // Primary key - name of target
    algo::UnTime         cum_modtime;              // Cumulative modtime of all source files in this target
    bool                 ood;                      //   false  File is out of date
    abt::FSyscmd*        syscmd_start;             // optional pointer
    abt::FSyscmd*        syscmd_compile;           // optional pointer
    abt::FSyscmd*        syscmd_link;              // optional pointer
    abt::FSyscmd*        syscmd_end;               // optional pointer
    abt::FTargsrc**      c_targsrc_elems;          // array of pointers
    u32                  c_targsrc_n;              // array of pointers
    u32                  c_targsrc_max;            // capacity of allocated array
    abt::FSrcfile**      c_srcfile_elems;          // array of pointers
    u32                  c_srcfile_n;              // array of pointers
    u32                  c_srcfile_max;            // capacity of allocated array
    abt::FTargdep**      c_targdep_elems;          // array of pointers
    u32                  c_targdep_n;              // array of pointers
    u32                  c_targdep_max;            // capacity of allocated array
    bool                 ood_visited;              //   false  Lockout for topo sort
    abt::FTargsyslib**   c_targsyslib_elems;       // array of pointers
    u32                  c_targsyslib_n;           // array of pointers
    u32                  c_targsyslib_max;         // capacity of allocated array
    abt::FTarget**       c_alldep_elems;           // array of pointers
    u32                  c_alldep_n;               // array of pointers
    u32                  c_alldep_max;             // capacity of allocated array
    abt::FNs*            p_ns;                     // reference to parent row
    bool                 libdep_visited;           //   false
    algo::UnTime         out_modtime;              //
    bool                 origsel;                  //   false
    // reftype Ptrary of abt.FTarget.c_targsrc prohibits copy
    // reftype Ptrary of abt.FTarget.c_srcfile prohibits copy
    // reftype Ptrary of abt.FTarget.c_targdep prohibits copy
    // reftype Ptrary of abt.FTarget.c_targsyslib prohibits copy
    // reftype Ptrary of abt.FTarget.c_alldep prohibits copy
    // x-reference on abt.FTarget.p_ns prevents copy
    // func:abt.FTarget..AssignOp
    abt::FTarget&        operator =(const abt::FTarget &rhs) = delete;
    // reftype Ptrary of abt.FTarget.c_targsrc prohibits copy
    // reftype Ptrary of abt.FTarget.c_srcfile prohibits copy
    // reftype Ptrary of abt.FTarget.c_targdep prohibits copy
    // reftype Ptrary of abt.FTarget.c_targsyslib prohibits copy
    // reftype Ptrary of abt.FTarget.c_alldep prohibits copy
    // x-reference on abt.FTarget.p_ns prevents copy
    // func:abt.FTarget..CopyCtor
    FTarget(const abt::FTarget &rhs) = delete;
private:
    // func:abt.FTarget..Ctor
    inline               FTarget() __attribute__((nothrow));
    // func:abt.FTarget..Dtor
    inline               ~FTarget() __attribute__((nothrow));
    friend abt::FTarget&        target_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend abt::FTarget*        target_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 target_RemoveLast() __attribute__((nothrow));
};
```

#### abt.FTargsrc - Source file for specific target
<a href="#abt-ftargsrc"></a>

#### abt.FTargsrc Fields
<a href="#abt-ftargsrc-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|abt.FTargsrc.msghdr|[dev.Targsrc](/txt/ssimdb/dev/targsrc.md)|[Base](/txt/ssimdb/dev/targsrc.md)|||
|abt.FTargsrc.p_target|[abt.FTarget](/txt/exe/abt/internals.md#abt-ftarget)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FTargsrc
<a href="#struct-ftargsrc"></a>
*Note:* field ``abt.FTargsrc.msghdr`` has reftype ``base`` so the fields of [dev.Targsrc](/txt/ssimdb/dev/targsrc.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/abt_gen.h](/include/gen/abt_gen.h)
```
struct FTargsrc { // abt.FTargsrc: Source file for specific target
    abt::FTargsrc*      ind_targsrc_next;          // hash next
    algo::Smallstr100   targsrc;                   //
    algo::Comment       comment;                   //
    abt::FTarget*       p_target;                  // reference to parent row
    bool                target_c_targsrc_in_ary;   //   false  membership flag
    // x-reference on abt.FTargsrc.p_target prevents copy
    // func:abt.FTargsrc..AssignOp
    inline abt::FTargsrc& operator =(const abt::FTargsrc &rhs) = delete;
    // x-reference on abt.FTargsrc.p_target prevents copy
    // func:abt.FTargsrc..CopyCtor
    inline               FTargsrc(const abt::FTargsrc &rhs) = delete;
private:
    // func:abt.FTargsrc..Ctor
    inline               FTargsrc() __attribute__((nothrow));
    // func:abt.FTargsrc..Dtor
    inline               ~FTargsrc() __attribute__((nothrow));
    friend abt::FTargsrc&       targsrc_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend abt::FTargsrc*       targsrc_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 targsrc_RemoveLast() __attribute__((nothrow));
};
```

#### abt.FTargsyslib - Target -> system library dependency
<a href="#abt-ftargsyslib"></a>

#### abt.FTargsyslib Fields
<a href="#abt-ftargsyslib-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|abt.FTargsyslib.msghdr|[dev.Targsyslib](/txt/ssimdb/dev/targsyslib.md)|[Base](/txt/ssimdb/dev/targsyslib.md)|||
|abt.FTargsyslib.p_syslib|[abt.FSyslib](/txt/exe/abt/internals.md#abt-fsyslib)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FTargsyslib
<a href="#struct-ftargsyslib"></a>
*Note:* field ``abt.FTargsyslib.msghdr`` has reftype ``base`` so the fields of [dev.Targsyslib](/txt/ssimdb/dev/targsyslib.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/abt_gen.h](/include/gen/abt_gen.h)
```
struct FTargsyslib { // abt.FTargsyslib: Target -> system library dependency
    algo::Smallstr50   targsyslib;                   //
    algo::Comment      comment;                      //
    abt::FSyslib*      p_syslib;                     // reference to parent row
    bool               target_c_targsyslib_in_ary;   //   false  membership flag
    // x-reference on abt.FTargsyslib.p_syslib prevents copy
    // func:abt.FTargsyslib..AssignOp
    inline abt::FTargsyslib& operator =(const abt::FTargsyslib &rhs) = delete;
    // x-reference on abt.FTargsyslib.p_syslib prevents copy
    // func:abt.FTargsyslib..CopyCtor
    inline               FTargsyslib(const abt::FTargsyslib &rhs) = delete;
private:
    // func:abt.FTargsyslib..Ctor
    inline               FTargsyslib() __attribute__((nothrow));
    // func:abt.FTargsyslib..Dtor
    inline               ~FTargsyslib() __attribute__((nothrow));
    friend abt::FTargsyslib&    targsyslib_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend abt::FTargsyslib*    targsyslib_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 targsyslib_RemoveLast() __attribute__((nothrow));
};
```

#### abt.FToolOpt - Single tool option
<a href="#abt-ftoolopt"></a>

#### abt.FToolOpt Fields
<a href="#abt-ftoolopt-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|abt.FToolOpt.msghdr|[dev.ToolOpt](/txt/ssimdb/dev/tool_opt.md)|[Base](/txt/ssimdb/dev/tool_opt.md)|||
|abt.FToolOpt.regx_opt|[algo.cstring](/txt/protocol/algo/cstring.md)|[RegxSql](/txt/exe/amc/reftypes.md#regxsql)|||
|abt.FToolOpt.regx_target|[algo.cstring](/txt/protocol/algo/cstring.md)|[RegxSql](/txt/exe/amc/reftypes.md#regxsql)|||

#### Struct FToolOpt
<a href="#struct-ftoolopt"></a>
*Note:* field ``abt.FToolOpt.msghdr`` has reftype ``base`` so the fields of [dev.ToolOpt](/txt/ssimdb/dev/tool_opt.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/abt_gen.h](/include/gen/abt_gen.h)
```
struct FToolOpt { // abt.FToolOpt: Single tool option
    algo::Smallstr100   tool_opt;      // Primary key - uname.compiler-cfg.arch/target-opt_type:opt
    algo::Comment       comment;       //
    algo_lib::Regx      regx_opt;      // Sql Regx
    algo_lib::Regx      regx_target;   // Sql Regx
private:
    // func:abt.FToolOpt..Ctor
    inline               FToolOpt() __attribute__((nothrow));
    friend abt::FToolOpt&       tool_opt_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend abt::FToolOpt*       tool_opt_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 tool_opt_RemoveLast() __attribute__((nothrow));
};
```

#### abt.FUname - Unix name
<a href="#abt-funame"></a>

#### abt.FUname Fields
<a href="#abt-funame-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|abt.FUname.msghdr|[dev.Uname](/txt/ssimdb/dev/uname.md)|[Base](/txt/ssimdb/dev/uname.md)|||

#### Struct FUname
<a href="#struct-funame"></a>
*Note:* field ``abt.FUname.msghdr`` has reftype ``base`` so the fields of [dev.Uname](/txt/ssimdb/dev/uname.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/abt_gen.h](/include/gen/abt_gen.h)
```
struct FUname { // abt.FUname: Unix name
    abt::FUname*       ind_uname_next;   // hash next
    algo::Smallstr50   uname;            //
    algo::Comment      comment;          //
    // func:abt.FUname..AssignOp
    inline abt::FUname&  operator =(const abt::FUname &rhs) = delete;
    // func:abt.FUname..CopyCtor
    inline               FUname(const abt::FUname &rhs) = delete;
private:
    // func:abt.FUname..Ctor
    inline               FUname() __attribute__((nothrow));
    // func:abt.FUname..Dtor
    inline               ~FUname() __attribute__((nothrow));
    friend abt::FUname&         uname_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend abt::FUname*         uname_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 uname_RemoveLast() __attribute__((nothrow));
};
```

#### abt.config - Breadcrumbs printed by abt when executing
<a href="#abt-config"></a>

#### abt.config Fields
<a href="#abt-config-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|abt.config.builddir|[dev.Builddir](/txt/ssimdb/dev/builddir.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|||
|abt.config.ood_src|i32|[Val](/txt/exe/amc/reftypes.md#val)|||
|abt.config.ood_target|i32|[Val](/txt/exe/amc/reftypes.md#val)|||
|abt.config.cache|[algo.Smallstr20](/txt/protocol/algo/README.md#algo-smallstr20)|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct config
<a href="#struct-config"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/abt_gen.h](/include/gen/abt_gen.h)
```
struct config { // abt.config: Breadcrumbs printed by abt when executing
    algo::Smallstr50   builddir;     //
    i32                ood_src;      //   0
    i32                ood_target;   //   0
    algo::Smallstr20   cache;        //
    // func:abt.config..Ctor
    inline               config() __attribute__((nothrow));
    // func:abt.config..FieldwiseCtor
    explicit inline               config(const algo::strptr& in_builddir, i32 in_ood_src, i32 in_ood_target, const algo::strptr& in_cache) __attribute__((nothrow));
};
```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Imdb -->


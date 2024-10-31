## abt_md - Internals


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
for usage, see [abt_md - Tool to generate markdown documentation](/txt/exe/abt_md/README.md)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Description -->

### Sources
<a href="#sources"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Sources -->
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/abt_md/file_section.cpp](/cpp/abt_md/file_section.cpp)||
|[cpp/abt_md/main.cpp](/cpp/abt_md/main.cpp)||
|[cpp/abt_md/mdsection.cpp](/cpp/abt_md/mdsection.cpp)||
|[cpp/gen/abt_md_gen.cpp](/cpp/gen/abt_md_gen.cpp)||
|[include/abt_md.h](/include/abt_md.h)||
|[include/gen/abt_md_gen.h](/include/gen/abt_md_gen.h)||
|[include/gen/abt_md_gen.inl.h](/include/gen/abt_md_gen.inl.h)||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Sources -->

### Dependencies
<a href="#dependencies"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Dependencies -->
The build target depends on the following libraries
|Target|Comment|
|---|---|
|[algo_lib](/txt/lib/algo_lib/README.md)|Support library for all executables|
|[lib_amcdb](/txt/lib/lib_amcdb/README.md)|Library used by amc|
|[lib_prot](/txt/lib/lib_prot/README.md)|Library covering all protocols|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Dependencies -->

### In Memory DB
<a href="#in-memory-db"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Imdb -->
`abt_md` generated code creates the tables below.
All allocations are done through global `abt_md::_db` [abt_md.FDb](#abt_md-fdb) structure
|Ctype|Ssimfile|Create|Access|
|---|---|---|---|
|[abt_md.FAnchor](#abt_md-fanchor)||FDb.anchor (Lary)|anchor (Lary, by rowid)|ind_anchor (Thash, hash field anchor)|
|[abt_md.FComptest](#abt_md-fcomptest)|[atfdb.comptest](/txt/ssimdb/atfdb/comptest.md)|FDb.comptest (Lary)|comptest (Lary, by rowid)|
||||FNs.c_comptest (Ptrary)|
|[abt_md.FCtype](#abt_md-fctype)|[dmmeta.ctype](/txt/ssimdb/dmmeta/ctype.md)|FDb.ctype (Lary)|ctype (Lary, by rowid)|ind_ctype (Thash, hash field ctype)|
||||FField.p_arg (Upptr)|
||||FField.p_ctype (Upptr)|
||||FNs.c_ctype (Ptrary)|
||||FReadme.p_ctype (Upptr)|
||||FSsimfile.p_ctype (Upptr)|
|[abt_md.FMdsection](#abt_md-fmdsection)|[dev.mdsection](/txt/ssimdb/dev/mdsection.md)|FDb.mdsection (Inlary)|**static**|
||||FFileSection.p_mdsection (Upptr)|
|[abt_md.FDb](#abt_md-fdb)||FDb._db (Global)|
|[abt_md.FDirent](#abt_md-fdirent)||FDirscan.dirent (Lary)|
||||FDirscan.bh_dirent (Bheap)|
|[abt_md.FDirscan](#abt_md-fdirscan)||
||||FDirent.p_dirscan (Upptr)|
|[abt_md.FFconst](#abt_md-ffconst)|[dmmeta.fconst](/txt/ssimdb/dmmeta/fconst.md)|FDb.fconst (Lary)|fconst (Lary, by rowid)|
||||FField.c_fconst (Ptrary)|
|[abt_md.FField](#abt_md-ffield)|[dmmeta.field](/txt/ssimdb/dmmeta/field.md)|FDb.field (Lary)|field (Lary, by rowid)|ind_field (Thash, hash field field)|
||||FCtype.c_field (Ptrary)|
||||FCtype.c_field_arg (Ptrary)|
||||FSubstr.p_srcfield (Upptr)|
|[abt_md.FFileSection](#abt_md-ffilesection)||FDb.file_section (Lary)|file_section (Lary, by rowid)|bh_file_section (Bheap, sort field sortkey)|
||||FMdsection.step (Hook)|
||||FMdsection.zd_file_section (Llist)|
|[abt_md.FGconst](#abt_md-fgconst)|[dmmeta.gconst](/txt/ssimdb/dmmeta/gconst.md)|FDb.gconst (Lary)|gconst (Lary, by rowid)|
||||FField.c_gconst (Ptr)|
|[abt_md.FGstatic](#abt_md-fgstatic)|[dmmeta.gstatic](/txt/ssimdb/dmmeta/gstatic.md)|FDb.gstatic (Lary)|gstatic (Lary, by rowid)|ind_gstatic (Thash, hash field field)|
|[abt_md.FHumanText](#abt_md-fhumantext)||FDb.human_text (Tpool)|ind_human_text (Thash, hash field key)|
|[abt_md.FLink](#abt_md-flink)||FDb.link (Lary)|link (Lary, by rowid)|
|[abt_md.FNs](#abt_md-fns)|[dmmeta.ns](/txt/ssimdb/dmmeta/ns.md)|FDb.ns (Lary)|ns (Lary, by rowid)|ind_ns (Thash, hash field ns)|
||||FCtype.p_ns (Upptr)|
||||FReadme.p_ns (Upptr)|
||||FTarget.p_ns (Upptr)|
|[abt_md.FNstype](#abt_md-fnstype)|[dmmeta.nstype](/txt/ssimdb/dmmeta/nstype.md)|FDb.nstype (Lary)|nstype (Lary, by rowid)|ind_nstype (Thash, hash field nstype)|
|[abt_md.FReadme](#abt_md-freadme)|[dev.readme](/txt/ssimdb/dev/readme.md)|FDb.readme (Lary)|readme (Lary, by rowid)|ind_readme (Thash, hash field gitfile)|c_readme (Ptr)|
||||FNs.c_readme (Ptr)|
|[abt_md.FReadmesort](#abt_md-freadmesort)|[dev.readmesort](/txt/ssimdb/dev/readmesort.md)|FDb.readmesort (Lary)|readmesort (Lary, by rowid)|ind_readmesort (Thash, hash field readmesort)|
|[abt_md.FReftype](#abt_md-freftype)|[dmmeta.reftype](/txt/ssimdb/dmmeta/reftype.md)|FDb.reftype (Lary)|reftype (Lary, by rowid)|
|[abt_md.FScriptfile](#abt_md-fscriptfile)|[dev.scriptfile](/txt/ssimdb/dev/scriptfile.md)|FDb.scriptfile (Lary)|scriptfile (Lary, by rowid)|ind_scriptfile (Thash, hash field gitfile)|
||||FReadme.p_scriptfile (Upptr)|
|[abt_md.FSsimfile](#abt_md-fssimfile)|[dmmeta.ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|FDb.ssimfile (Lary)|ssimfile (Lary, by rowid)|ind_ssimfile (Thash, hash field ssimfile)|
||||FCtype.c_ssimfile (Ptr)|
||||FReadme.p_ssimfile (Upptr)|
|[abt_md.FSubstr](#abt_md-fsubstr)|[dmmeta.substr](/txt/ssimdb/dmmeta/substr.md)|FDb.substr (Lary)|substr (Lary, by rowid)|
||||FField.c_substr (Ptr)|
|[abt_md.FTargdep](#abt_md-ftargdep)|[dev.targdep](/txt/ssimdb/dev/targdep.md)|FDb.targdep (Lary)|targdep (Lary, by rowid)|
||||FTarget.c_targdep (Ptrary)|
|[abt_md.FTarget](#abt_md-ftarget)|[dev.target](/txt/ssimdb/dev/target.md)|FDb.target (Lary)|target (Lary, by rowid)|ind_target (Thash, hash field target)|
||||FNs.c_target (Ptr)|
||||FTargdep.p_parent (Upptr)|
|[abt_md.FTargsrc](#abt_md-ftargsrc)|[dev.targsrc](/txt/ssimdb/dev/targsrc.md)|FDb.targsrc (Lary)|targsrc (Lary, by rowid)|
||||FNs.c_targsrc (Ptrary)|
|[abt_md.FTclass](#abt_md-ftclass)|[amcdb.tclass](/txt/ssimdb/amcdb/tclass.md)|FDb.tclass (Lary)|tclass (Lary, by rowid)|ind_tclass (Thash, hash field tclass)|
||||FReftype.p_tclass (Upptr)|

#### abt_md.FAnchor - 
<a href="#abt_md-fanchor"></a>

#### abt_md.FAnchor Fields
<a href="#abt_md-fanchor-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|abt_md.FAnchor.anchor|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FAnchor
<a href="#struct-fanchor"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/abt_md_gen.h](/include/gen/abt_md_gen.h)
```
struct FAnchor { // abt_md.FAnchor
    algo::cstring      anchor;            //
    abt_md::FAnchor*   ind_anchor_next;   // hash next
    // func:abt_md.FAnchor..AssignOp
    inline abt_md::FAnchor& operator =(const abt_md::FAnchor &rhs) = delete;
    // func:abt_md.FAnchor..CopyCtor
    inline               FAnchor(const abt_md::FAnchor &rhs) = delete;
private:
    // func:abt_md.FAnchor..Ctor
    inline               FAnchor() __attribute__((nothrow));
    // func:abt_md.FAnchor..Dtor
    inline               ~FAnchor() __attribute__((nothrow));
    friend abt_md::FAnchor&     anchor_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend abt_md::FAnchor*     anchor_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 anchor_RemoveAll() __attribute__((nothrow));
    friend void                 anchor_RemoveLast() __attribute__((nothrow));
};
```

#### abt_md.FComptest - 
<a href="#abt_md-fcomptest"></a>

#### abt_md.FComptest Fields
<a href="#abt_md-fcomptest-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|abt_md.FComptest.base|[atfdb.Comptest](/txt/ssimdb/atfdb/comptest.md)|[Base](/txt/ssimdb/atfdb/comptest.md)|||

#### Struct FComptest
<a href="#struct-fcomptest"></a>
*Note:* field ``abt_md.FComptest.base`` has reftype ``base`` so the fields of [atfdb.Comptest](/txt/ssimdb/atfdb/comptest.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/abt_md_gen.h](/include/gen/abt_md_gen.h)
```
struct FComptest { // abt_md.FComptest
    algo::Smallstr50   comptest;               //
    i32                timeout;                //   10
    bool               memcheck;               //   true
    u8                 exit_code;              //   0  Exit code to check
    algo::Comment      comment;                //
    bool               ns_c_comptest_in_ary;   //   false  membership flag
    // func:abt_md.FComptest..AssignOp
    inline abt_md::FComptest& operator =(const abt_md::FComptest &rhs) = delete;
    // func:abt_md.FComptest..CopyCtor
    inline               FComptest(const abt_md::FComptest &rhs) = delete;
private:
    // func:abt_md.FComptest..Ctor
    inline               FComptest() __attribute__((nothrow));
    // func:abt_md.FComptest..Dtor
    inline               ~FComptest() __attribute__((nothrow));
    friend abt_md::FComptest&   comptest_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend abt_md::FComptest*   comptest_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 comptest_RemoveAll() __attribute__((nothrow));
    friend void                 comptest_RemoveLast() __attribute__((nothrow));
};
```

#### abt_md.FCtype - Struct
<a href="#abt_md-fctype"></a>

#### abt_md.FCtype Fields
<a href="#abt_md-fctype-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|abt_md.FCtype.base|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Base](/txt/ssimdb/dmmeta/ctype.md)|||
|abt_md.FCtype.c_ssimfile|[abt_md.FSsimfile](/txt/exe/abt_md/internals.md#abt_md-fssimfile)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|abt_md.FCtype.c_field|[abt_md.FField](/txt/exe/abt_md/internals.md#abt_md-ffield)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|abt_md.FCtype.p_ns|[abt_md.FNs](/txt/exe/abt_md/internals.md#abt_md-fns)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|abt_md.FCtype.c_field_arg|[abt_md.FField](/txt/exe/abt_md/internals.md#abt_md-ffield)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||

#### Struct FCtype
<a href="#struct-fctype"></a>
*Note:* field ``abt_md.FCtype.base`` has reftype ``base`` so the fields of [dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/abt_md_gen.h](/include/gen/abt_md_gen.h)
```
struct FCtype { // abt_md.FCtype
    algo::Smallstr100    ctype;               // Identifier. must be ns.typename
    algo::Comment        comment;             //
    abt_md::FSsimfile*   c_ssimfile;          // optional pointer
    abt_md::FField**     c_field_elems;       // array of pointers
    u32                  c_field_n;           // array of pointers
    u32                  c_field_max;         // capacity of allocated array
    abt_md::FNs*         p_ns;                // reference to parent row
    abt_md::FField**     c_field_arg_elems;   // array of pointers
    u32                  c_field_arg_n;       // array of pointers
    u32                  c_field_arg_max;     // capacity of allocated array
    bool                 ns_c_ctype_in_ary;   //   false  membership flag
    abt_md::FCtype*      ind_ctype_next;      // hash next
    // x-reference on abt_md.FCtype.c_ssimfile prevents copy
    // reftype Ptrary of abt_md.FCtype.c_field prohibits copy
    // x-reference on abt_md.FCtype.p_ns prevents copy
    // reftype Ptrary of abt_md.FCtype.c_field_arg prohibits copy
    // func:abt_md.FCtype..AssignOp
    abt_md::FCtype&      operator =(const abt_md::FCtype &rhs) = delete;
    // x-reference on abt_md.FCtype.c_ssimfile prevents copy
    // reftype Ptrary of abt_md.FCtype.c_field prohibits copy
    // x-reference on abt_md.FCtype.p_ns prevents copy
    // reftype Ptrary of abt_md.FCtype.c_field_arg prohibits copy
    // func:abt_md.FCtype..CopyCtor
    FCtype(const abt_md::FCtype &rhs) = delete;
private:
    // func:abt_md.FCtype..Ctor
    inline               FCtype() __attribute__((nothrow));
    // func:abt_md.FCtype..Dtor
    inline               ~FCtype() __attribute__((nothrow));
    friend abt_md::FCtype&      ctype_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend abt_md::FCtype*      ctype_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 ctype_RemoveAll() __attribute__((nothrow));
    friend void                 ctype_RemoveLast() __attribute__((nothrow));
};
```

#### abt_md.FMdsection - Template for updating a markdown file
<a href="#abt_md-fmdsection"></a>

#### abt_md.FMdsection Fields
<a href="#abt_md-fmdsection-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|abt_md.FMdsection.base|[dev.Mdsection](/txt/ssimdb/dev/mdsection.md)|[Base](/txt/ssimdb/dev/mdsection.md)|||
|abt_md.FMdsection.step|[abt_md.FFileSection](/txt/exe/abt_md/internals.md#abt_md-ffilesection)|[Hook](/txt/exe/amc/reftypes.md#hook)|||
|abt_md.FMdsection.regx_match|[algo.cstring](/txt/protocol/algo/cstring.md)|[RegxSql](/txt/exe/amc/reftypes.md#regxsql)|||
|abt_md.FMdsection.regx_path|[algo.cstring](/txt/protocol/algo/cstring.md)|[RegxSql](/txt/exe/amc/reftypes.md#regxsql)|||
|abt_md.FMdsection.rowid|i32|[Val](/txt/exe/amc/reftypes.md#val)|||
|abt_md.FMdsection.zd_file_section|[abt_md.FFileSection](/txt/exe/abt_md/internals.md#abt_md-ffilesection)|[Llist](/txt/exe/amc/reftypes.md#llist)|||

#### Struct FMdsection
<a href="#struct-fmdsection"></a>
*Note:* field ``abt_md.FMdsection.base`` has reftype ``base`` so the fields of [dev.Mdsection](/txt/ssimdb/dev/mdsection.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/abt_md_gen.h](/include/gen/abt_md_gen.h)
```
struct FMdsection { // abt_md.FMdsection
    algo::Smallstr50              mdsection;              //
    algo::Smallstr200             match;                  //
    algo::Smallstr100             path;                   //
    algo::Smallstr50              genlist;                //
    algo::Comment                 comment;                //
    abt_md::mdsection_step_hook   step;                   //   NULL  Pointer to a function
    algo_lib::Regx                regx_match;             // Sql Regx
    algo_lib::Regx                regx_path;              // Sql Regx
    i32                           rowid;                  //   0
    abt_md::FFileSection*         zd_file_section_head;   // zero-terminated doubly linked list
    i32                           zd_file_section_n;      // zero-terminated doubly linked list
    abt_md::FFileSection*         zd_file_section_tail;   // pointer to last element
    // func:abt_md.FMdsection..Ctor
    inline               FMdsection() __attribute__((nothrow));
};
```

#### abt_md.FDb - In-memory database for abt_md
<a href="#abt_md-fdb"></a>

#### abt_md.FDb Fields
<a href="#abt_md-fdb-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|abt_md.FDb._db|[abt_md.FDb](/txt/exe/abt_md/internals.md#abt_md-fdb)|[Global](/txt/exe/amc/reftypes.md#global)|||
|abt_md.FDb.cmdline|[command.abt_md](/txt/protocol/command/README.md#command-abt_md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|abt_md.FDb.readme|[abt_md.FReadme](/txt/exe/abt_md/internals.md#abt_md-freadme)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|abt_md.FDb.ind_readme|[abt_md.FReadme](/txt/exe/abt_md/internals.md#abt_md-freadme)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|abt_md.FDb.mdsection|[abt_md.FMdsection](/txt/exe/abt_md/internals.md#abt_md-fmdsection)|[Inlary](/txt/exe/amc/reftypes.md#inlary)|||
|abt_md.FDb.file_section|[abt_md.FFileSection](/txt/exe/abt_md/internals.md#abt_md-ffilesection)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|abt_md.FDb.bh_file_section|[abt_md.FFileSection](/txt/exe/abt_md/internals.md#abt_md-ffilesection)|[Bheap](/txt/exe/amc/reftypes.md#bheap)|||
|abt_md.FDb.c_readme|[abt_md.FReadme](/txt/exe/abt_md/internals.md#abt_md-freadme)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|abt_md.FDb.ns|[abt_md.FNs](/txt/exe/abt_md/internals.md#abt_md-fns)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|abt_md.FDb.ind_ns|[abt_md.FNs](/txt/exe/abt_md/internals.md#abt_md-fns)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|abt_md.FDb.R|[algo_lib.Replscope](/txt/lib/algo_lib/README.md#algo_lib-replscope)|[Val](/txt/exe/amc/reftypes.md#val)|||
|abt_md.FDb.ssimfile|[abt_md.FSsimfile](/txt/exe/abt_md/internals.md#abt_md-fssimfile)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|abt_md.FDb.ind_ssimfile|[abt_md.FSsimfile](/txt/exe/abt_md/internals.md#abt_md-fssimfile)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|abt_md.FDb.ctype|[abt_md.FCtype](/txt/exe/abt_md/internals.md#abt_md-fctype)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|abt_md.FDb.ind_ctype|[abt_md.FCtype](/txt/exe/abt_md/internals.md#abt_md-fctype)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|abt_md.FDb.field|[abt_md.FField](/txt/exe/abt_md/internals.md#abt_md-ffield)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|abt_md.FDb.ind_field|[abt_md.FField](/txt/exe/abt_md/internals.md#abt_md-ffield)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|abt_md.FDb.targsrc|[abt_md.FTargsrc](/txt/exe/abt_md/internals.md#abt_md-ftargsrc)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|abt_md.FDb.substr|[abt_md.FSubstr](/txt/exe/abt_md/internals.md#abt_md-fsubstr)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|abt_md.FDb.human_text|[abt_md.FHumanText](/txt/exe/abt_md/internals.md#abt_md-fhumantext)|[Tpool](/txt/exe/amc/reftypes.md#tpool)|||
|abt_md.FDb.ind_human_text|[abt_md.FHumanText](/txt/exe/abt_md/internals.md#abt_md-fhumantext)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|abt_md.FDb.scriptfile|[abt_md.FScriptfile](/txt/exe/abt_md/internals.md#abt_md-fscriptfile)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|abt_md.FDb.ind_scriptfile|[abt_md.FScriptfile](/txt/exe/abt_md/internals.md#abt_md-fscriptfile)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|abt_md.FDb.nstype|[abt_md.FNstype](/txt/exe/abt_md/internals.md#abt_md-fnstype)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|abt_md.FDb.ind_nstype|[abt_md.FNstype](/txt/exe/abt_md/internals.md#abt_md-fnstype)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|abt_md.FDb.comptest|[abt_md.FComptest](/txt/exe/abt_md/internals.md#abt_md-fcomptest)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|abt_md.FDb.anchor|[abt_md.FAnchor](/txt/exe/abt_md/internals.md#abt_md-fanchor)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|abt_md.FDb.ind_anchor|[abt_md.FAnchor](/txt/exe/abt_md/internals.md#abt_md-fanchor)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|abt_md.FDb.link|[abt_md.FLink](/txt/exe/abt_md/internals.md#abt_md-flink)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|abt_md.FDb.reftype|[abt_md.FReftype](/txt/exe/abt_md/internals.md#abt_md-freftype)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|abt_md.FDb.tclass|[abt_md.FTclass](/txt/exe/abt_md/internals.md#abt_md-ftclass)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|abt_md.FDb.ind_tclass|[abt_md.FTclass](/txt/exe/abt_md/internals.md#abt_md-ftclass)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|abt_md.FDb.fconst|[abt_md.FFconst](/txt/exe/abt_md/internals.md#abt_md-ffconst)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|abt_md.FDb.gconst|[abt_md.FGconst](/txt/exe/abt_md/internals.md#abt_md-fgconst)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|abt_md.FDb.readmesort|[abt_md.FReadmesort](/txt/exe/abt_md/internals.md#abt_md-freadmesort)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|abt_md.FDb.ind_readmesort|[abt_md.FReadmesort](/txt/exe/abt_md/internals.md#abt_md-freadmesort)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|abt_md.FDb.gstatic|[abt_md.FGstatic](/txt/exe/abt_md/internals.md#abt_md-fgstatic)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|abt_md.FDb.ind_gstatic|[abt_md.FGstatic](/txt/exe/abt_md/internals.md#abt_md-fgstatic)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|abt_md.FDb.target|[abt_md.FTarget](/txt/exe/abt_md/internals.md#abt_md-ftarget)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|abt_md.FDb.targdep|[abt_md.FTargdep](/txt/exe/abt_md/internals.md#abt_md-ftargdep)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|abt_md.FDb.ind_target|[abt_md.FTarget](/txt/exe/abt_md/internals.md#abt_md-ftarget)|[Thash](/txt/exe/amc/reftypes.md#thash)|||

#### Struct FDb
<a href="#struct-fdb"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/abt_md_gen.h](/include/gen/abt_md_gen.h)
```
struct FDb { // abt_md.FDb: In-memory database for abt_md
    command::abt_md          cmdline;                                           //
    abt_md::FReadme*         readme_lary[32];                                   // level array
    i32                      readme_n;                                          // number of elements in array
    abt_md::FReadme**        ind_readme_buckets_elems;                          // pointer to bucket array
    i32                      ind_readme_buckets_n;                              // number of elements in bucket array
    i32                      ind_readme_n;                                      // number of elements in the hash table
    u128                     mdsection_data[sizeu128(abt_md::FMdsection,25)];   // place for data
    i32                      mdsection_n;                                       // number of elems current in existence
    enum { mdsection_max = 25 };
    abt_md::FFileSection*    file_section_lary[32];                             // level array
    i32                      file_section_n;                                    // number of elements in array
    abt_md::FFileSection**   bh_file_section_elems;                             // binary heap by sortkey
    i32                      bh_file_section_n;                                 // number of elements in the heap
    i32                      bh_file_section_max;                               // max elements in bh_file_section_elems
    abt_md::FReadme*         c_readme;                                          // optional pointer
    abt_md::FNs*             ns_lary[32];                                       // level array
    i32                      ns_n;                                              // number of elements in array
    abt_md::FNs**            ind_ns_buckets_elems;                              // pointer to bucket array
    i32                      ind_ns_buckets_n;                                  // number of elements in bucket array
    i32                      ind_ns_n;                                          // number of elements in the hash table
    algo_lib::Replscope      R;                                                 //
    abt_md::FSsimfile*       ssimfile_lary[32];                                 // level array
    i32                      ssimfile_n;                                        // number of elements in array
    abt_md::FSsimfile**      ind_ssimfile_buckets_elems;                        // pointer to bucket array
    i32                      ind_ssimfile_buckets_n;                            // number of elements in bucket array
    i32                      ind_ssimfile_n;                                    // number of elements in the hash table
    abt_md::FCtype*          ctype_lary[32];                                    // level array
    i32                      ctype_n;                                           // number of elements in array
    abt_md::FCtype**         ind_ctype_buckets_elems;                           // pointer to bucket array
    i32                      ind_ctype_buckets_n;                               // number of elements in bucket array
    i32                      ind_ctype_n;                                       // number of elements in the hash table
    abt_md::FField*          field_lary[32];                                    // level array
    i32                      field_n;                                           // number of elements in array
    abt_md::FField**         ind_field_buckets_elems;                           // pointer to bucket array
    i32                      ind_field_buckets_n;                               // number of elements in bucket array
    i32                      ind_field_n;                                       // number of elements in the hash table
    abt_md::FTargsrc*        targsrc_lary[32];                                  // level array
    i32                      targsrc_n;                                         // number of elements in array
    abt_md::FSubstr*         substr_lary[32];                                   // level array
    i32                      substr_n;                                          // number of elements in array
    u64                      human_text_blocksize;                              // # bytes per block
    abt_md::FHumanText*      human_text_free;                                   //
    abt_md::FHumanText**     ind_human_text_buckets_elems;                      // pointer to bucket array
    i32                      ind_human_text_buckets_n;                          // number of elements in bucket array
    i32                      ind_human_text_n;                                  // number of elements in the hash table
    abt_md::FScriptfile*     scriptfile_lary[32];                               // level array
    i32                      scriptfile_n;                                      // number of elements in array
    abt_md::FScriptfile**    ind_scriptfile_buckets_elems;                      // pointer to bucket array
    i32                      ind_scriptfile_buckets_n;                          // number of elements in bucket array
    i32                      ind_scriptfile_n;                                  // number of elements in the hash table
    abt_md::FNstype*         nstype_lary[32];                                   // level array
    i32                      nstype_n;                                          // number of elements in array
    abt_md::FNstype**        ind_nstype_buckets_elems;                          // pointer to bucket array
    i32                      ind_nstype_buckets_n;                              // number of elements in bucket array
    i32                      ind_nstype_n;                                      // number of elements in the hash table
    abt_md::FComptest*       comptest_lary[32];                                 // level array
    i32                      comptest_n;                                        // number of elements in array
    abt_md::FAnchor*         anchor_lary[32];                                   // level array
    i32                      anchor_n;                                          // number of elements in array
    abt_md::FAnchor**        ind_anchor_buckets_elems;                          // pointer to bucket array
    i32                      ind_anchor_buckets_n;                              // number of elements in bucket array
    i32                      ind_anchor_n;                                      // number of elements in the hash table
    abt_md::FLink*           link_lary[32];                                     // level array
    i32                      link_n;                                            // number of elements in array
    abt_md::FReftype*        reftype_lary[32];                                  // level array
    i32                      reftype_n;                                         // number of elements in array
    abt_md::FTclass*         tclass_lary[32];                                   // level array
    i32                      tclass_n;                                          // number of elements in array
    abt_md::FTclass**        ind_tclass_buckets_elems;                          // pointer to bucket array
    i32                      ind_tclass_buckets_n;                              // number of elements in bucket array
    i32                      ind_tclass_n;                                      // number of elements in the hash table
    abt_md::FFconst*         fconst_lary[32];                                   // level array
    i32                      fconst_n;                                          // number of elements in array
    abt_md::FGconst*         gconst_lary[32];                                   // level array
    i32                      gconst_n;                                          // number of elements in array
    abt_md::FReadmesort*     readmesort_lary[32];                               // level array
    i32                      readmesort_n;                                      // number of elements in array
    abt_md::FReadmesort**    ind_readmesort_buckets_elems;                      // pointer to bucket array
    i32                      ind_readmesort_buckets_n;                          // number of elements in bucket array
    i32                      ind_readmesort_n;                                  // number of elements in the hash table
    abt_md::FGstatic*        gstatic_lary[32];                                  // level array
    i32                      gstatic_n;                                         // number of elements in array
    abt_md::FGstatic**       ind_gstatic_buckets_elems;                         // pointer to bucket array
    i32                      ind_gstatic_buckets_n;                             // number of elements in bucket array
    i32                      ind_gstatic_n;                                     // number of elements in the hash table
    abt_md::FTarget*         target_lary[32];                                   // level array
    i32                      target_n;                                          // number of elements in array
    abt_md::FTargdep*        targdep_lary[32];                                  // level array
    i32                      targdep_n;                                         // number of elements in array
    abt_md::FTarget**        ind_target_buckets_elems;                          // pointer to bucket array
    i32                      ind_target_buckets_n;                              // number of elements in bucket array
    i32                      ind_target_n;                                      // number of elements in the hash table
    abt_md::trace            trace;                                             //
};
```

#### abt_md.FDirent - 
<a href="#abt_md-fdirent"></a>

#### abt_md.FDirent Fields
<a href="#abt_md-fdirent-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|abt_md.FDirent.is_dir|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|abt_md.FDirent.p_dirscan|[abt_md.FDirscan](/txt/exe/abt_md/internals.md#abt_md-fdirscan)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|abt_md.FDirent.filename|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|abt_md.FDirent.sortfld|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|abt_md.FDirent.pathname|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FDirent
<a href="#struct-fdirent"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/abt_md_gen.h](/include/gen/abt_md_gen.h)
```
struct FDirent { // abt_md.FDirent
    bool                is_dir;          //   false
    abt_md::FDirscan*   p_dirscan;       // reference to parent row
    algo::cstring       filename;        //
    algo::cstring       sortfld;         //
    algo::cstring       pathname;        //
    i32                 bh_dirent_idx;   // index in heap; -1 means not-in-heap
    // func:abt_md.FDirent..AssignOp
    inline abt_md::FDirent& operator =(const abt_md::FDirent &rhs) = delete;
    // func:abt_md.FDirent..CopyCtor
    inline               FDirent(const abt_md::FDirent &rhs) = delete;
private:
    // func:abt_md.FDirent..Ctor
    inline               FDirent() __attribute__((nothrow));
    // func:abt_md.FDirent..Dtor
    inline               ~FDirent() __attribute__((nothrow));
    friend abt_md::FDirent&     dirent_Alloc(abt_md::FDirscan& parent) __attribute__((__warn_unused_result__, nothrow));
    friend abt_md::FDirent*     dirent_AllocMaybe(abt_md::FDirscan& parent) __attribute__((__warn_unused_result__, nothrow));
    friend void                 dirent_RemoveAll(abt_md::FDirscan& parent) __attribute__((nothrow));
    friend void                 dirent_RemoveLast(abt_md::FDirscan& parent) __attribute__((nothrow));
};
```

#### abt_md.FDirscan - 
<a href="#abt_md-fdirscan"></a>

#### abt_md.FDirscan Fields
<a href="#abt_md-fdirscan-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|abt_md.FDirscan.bh_dirent|[abt_md.FDirent](/txt/exe/abt_md/internals.md#abt_md-fdirent)|[Bheap](/txt/exe/amc/reftypes.md#bheap)|||
|abt_md.FDirscan.dirent|[abt_md.FDirent](/txt/exe/abt_md/internals.md#abt_md-fdirent)|[Lary](/txt/exe/amc/reftypes.md#lary)|||

#### Struct FDirscan
<a href="#struct-fdirscan"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/abt_md_gen.h](/include/gen/abt_md_gen.h)
```
struct FDirscan { // abt_md.FDirscan
    abt_md::FDirent**   bh_dirent_elems;   // binary heap by sortfld
    i32                 bh_dirent_n;       // number of elements in the heap
    i32                 bh_dirent_max;     // max elements in bh_dirent_elems
    abt_md::FDirent*    dirent_lary[32];   // level array
    i32                 dirent_n;          // number of elements in array
    // func:abt_md.FDirscan..Ctor
    inline               FDirscan() __attribute__((nothrow));
    // func:abt_md.FDirscan..Dtor
    inline               ~FDirscan() __attribute__((nothrow));
};
```

#### abt_md.FFconst - Specify enum value (integer + string constant) for a field
<a href="#abt_md-ffconst"></a>

#### abt_md.FFconst Fields
<a href="#abt_md-ffconst-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|abt_md.FFconst.base|[dmmeta.Fconst](/txt/ssimdb/dmmeta/fconst.md)|[Base](/txt/ssimdb/dmmeta/fconst.md)|||

#### Struct FFconst
<a href="#struct-ffconst"></a>
*Note:* field ``abt_md.FFconst.base`` has reftype ``base`` so the fields of [dmmeta.Fconst](/txt/ssimdb/dmmeta/fconst.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/abt_md_gen.h](/include/gen/abt_md_gen.h)
```
struct FFconst { // abt_md.FFconst
    algo::Smallstr100   fconst;                  //
    algo::CppExpr       value;                   //
    algo::Comment       comment;                 //
    bool                field_c_fconst_in_ary;   //   false  membership flag
    // func:abt_md.FFconst..AssignOp
    inline abt_md::FFconst& operator =(const abt_md::FFconst &rhs) = delete;
    // func:abt_md.FFconst..CopyCtor
    inline               FFconst(const abt_md::FFconst &rhs) = delete;
private:
    // func:abt_md.FFconst..Ctor
    inline               FFconst() __attribute__((nothrow));
    // func:abt_md.FFconst..Dtor
    inline               ~FFconst() __attribute__((nothrow));
    friend abt_md::FFconst&     fconst_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend abt_md::FFconst*     fconst_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 fconst_RemoveAll() __attribute__((nothrow));
    friend void                 fconst_RemoveLast() __attribute__((nothrow));
};
```

#### abt_md.FField - Specify field of a struct
<a href="#abt_md-ffield"></a>

#### abt_md.FField Fields
<a href="#abt_md-ffield-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|abt_md.FField.base|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Base](/txt/ssimdb/dmmeta/field.md)|||
|abt_md.FField.text|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|abt_md.FField.c_substr|[abt_md.FSubstr](/txt/exe/abt_md/internals.md#abt_md-fsubstr)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|abt_md.FField.p_arg|[abt_md.FCtype](/txt/exe/abt_md/internals.md#abt_md-fctype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|abt_md.FField.p_ctype|[abt_md.FCtype](/txt/exe/abt_md/internals.md#abt_md-fctype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|abt_md.FField.ispkey|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|abt_md.FField.c_fconst|[abt_md.FFconst](/txt/exe/abt_md/internals.md#abt_md-ffconst)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|abt_md.FField.c_gconst|[abt_md.FGconst](/txt/exe/abt_md/internals.md#abt_md-fgconst)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||

#### Struct FField
<a href="#struct-ffield"></a>
*Note:* field ``abt_md.FField.base`` has reftype ``base`` so the fields of [dmmeta.Field](/txt/ssimdb/dmmeta/field.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/abt_md_gen.h](/include/gen/abt_md_gen.h)
```
struct FField { // abt_md.FField
    abt_md::FField*     ind_field_next;             // hash next
    algo::Smallstr100   field;                      // Primary key, as ctype.name
    algo::Smallstr100   arg;                        // Type of field
    algo::Smallstr50    reftype;                    //   "Val"  Type constructor
    algo::CppExpr       dflt;                       // Default value (c++ expression)
    algo::Comment       comment;                    //
    algo::cstring       text;                       //
    abt_md::FSubstr*    c_substr;                   // optional pointer
    abt_md::FCtype*     p_arg;                      // reference to parent row
    abt_md::FCtype*     p_ctype;                    // reference to parent row
    bool                ispkey;                     //   false
    abt_md::FFconst**   c_fconst_elems;             // array of pointers
    u32                 c_fconst_n;                 // array of pointers
    u32                 c_fconst_max;               // capacity of allocated array
    abt_md::FGconst*    c_gconst;                   // optional pointer
    bool                ctype_c_field_in_ary;       //   false  membership flag
    bool                ctype_c_field_arg_in_ary;   //   false  membership flag
    // x-reference on abt_md.FField.c_substr prevents copy
    // x-reference on abt_md.FField.p_arg prevents copy
    // x-reference on abt_md.FField.p_ctype prevents copy
    // reftype Ptrary of abt_md.FField.c_fconst prohibits copy
    // x-reference on abt_md.FField.c_gconst prevents copy
    // func:abt_md.FField..AssignOp
    abt_md::FField&      operator =(const abt_md::FField &rhs) = delete;
    // x-reference on abt_md.FField.c_substr prevents copy
    // x-reference on abt_md.FField.p_arg prevents copy
    // x-reference on abt_md.FField.p_ctype prevents copy
    // reftype Ptrary of abt_md.FField.c_fconst prohibits copy
    // x-reference on abt_md.FField.c_gconst prevents copy
    // func:abt_md.FField..CopyCtor
    FField(const abt_md::FField &rhs) = delete;
private:
    // func:abt_md.FField..Ctor
    inline               FField() __attribute__((nothrow));
    // func:abt_md.FField..Dtor
    inline               ~FField() __attribute__((nothrow));
    friend abt_md::FField&      field_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend abt_md::FField*      field_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 field_RemoveAll() __attribute__((nothrow));
    friend void                 field_RemoveLast() __attribute__((nothrow));
};
```

#### abt_md.FFileSection - 
<a href="#abt_md-ffilesection"></a>

#### abt_md.FFileSection Fields
<a href="#abt_md-ffilesection-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|abt_md.FFileSection.title|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||First line|
|abt_md.FFileSection.text|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Remaining lines|
|abt_md.FFileSection.sortkey|i32|[Val](/txt/exe/amc/reftypes.md#val)||Sort key|
|abt_md.FFileSection.select|bool|[Val](/txt/exe/amc/reftypes.md#val)|true|Selected for processing|
|abt_md.FFileSection.firstline|i32|[Val](/txt/exe/amc/reftypes.md#val)||Line in file where section starts|
|abt_md.FFileSection.p_mdsection|[abt_md.FMdsection](/txt/exe/abt_md/internals.md#abt_md-fmdsection)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FFileSection
<a href="#struct-ffilesection"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/abt_md_gen.h](/include/gen/abt_md_gen.h)
```
struct FFileSection { // abt_md.FFileSection
    abt_md::FFileSection*   zd_file_section_next;   // zslist link; -1 means not-in-list
    abt_md::FFileSection*   zd_file_section_prev;   // previous element
    i32                     bh_file_section_idx;    // index in heap; -1 means not-in-heap
    algo::cstring           title;                  // First line
    algo::cstring           text;                   // Remaining lines
    i32                     sortkey;                //   0  Sort key
    bool                    select;                 //   true  Selected for processing
    i32                     firstline;              //   0  Line in file where section starts
    abt_md::FMdsection*     p_mdsection;            // reference to parent row
    // func:abt_md.FFileSection..AssignOp
    inline abt_md::FFileSection& operator =(const abt_md::FFileSection &rhs) = delete;
    // func:abt_md.FFileSection..CopyCtor
    inline               FFileSection(const abt_md::FFileSection &rhs) = delete;
private:
    // func:abt_md.FFileSection..Ctor
    inline               FFileSection() __attribute__((nothrow));
    // func:abt_md.FFileSection..Dtor
    inline               ~FFileSection() __attribute__((nothrow));
    friend abt_md::FFileSection& file_section_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend abt_md::FFileSection* file_section_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 file_section_RemoveAll() __attribute__((nothrow));
    friend void                 file_section_RemoveLast() __attribute__((nothrow));
};
```

#### abt_md.FGconst - Import ssim table columns as fconst for a field
<a href="#abt_md-fgconst"></a>

#### abt_md.FGconst Fields
<a href="#abt_md-fgconst-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|abt_md.FGconst.base|[dmmeta.Gconst](/txt/ssimdb/dmmeta/gconst.md)|[Base](/txt/ssimdb/dmmeta/gconst.md)|||

#### Struct FGconst
<a href="#struct-fgconst"></a>
*Note:* field ``abt_md.FGconst.base`` has reftype ``base`` so the fields of [dmmeta.Gconst](/txt/ssimdb/dmmeta/gconst.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/abt_md_gen.h](/include/gen/abt_md_gen.h)
```
struct FGconst { // abt_md.FGconst
    algo::Smallstr100   field;      // Field with which to associate constants
    algo::Smallstr100   namefld;    // Field which gives the name of the constant
    algo::Smallstr100   idfld;      // Optional attribute of namefld that gives numeric ID for gconst
    bool                wantenum;   //   true
    // func:abt_md.FGconst..AssignOp
    inline abt_md::FGconst& operator =(const abt_md::FGconst &rhs) = delete;
    // func:abt_md.FGconst..CopyCtor
    inline               FGconst(const abt_md::FGconst &rhs) = delete;
private:
    // func:abt_md.FGconst..Ctor
    inline               FGconst() __attribute__((nothrow));
    // func:abt_md.FGconst..Dtor
    inline               ~FGconst() __attribute__((nothrow));
    friend abt_md::FGconst&     gconst_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend abt_md::FGconst*     gconst_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 gconst_RemoveAll() __attribute__((nothrow));
    friend void                 gconst_RemoveLast() __attribute__((nothrow));
};
```

#### abt_md.FGstatic - Load entries for this table at startup time
<a href="#abt_md-fgstatic"></a>

#### abt_md.FGstatic Fields
<a href="#abt_md-fgstatic-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|abt_md.FGstatic.base|[dmmeta.Gstatic](/txt/ssimdb/dmmeta/gstatic.md)|[Base](/txt/ssimdb/dmmeta/gstatic.md)|||

#### Struct FGstatic
<a href="#struct-fgstatic"></a>
*Note:* field ``abt_md.FGstatic.base`` has reftype ``base`` so the fields of [dmmeta.Gstatic](/txt/ssimdb/dmmeta/gstatic.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/abt_md_gen.h](/include/gen/abt_md_gen.h)
```
struct FGstatic { // abt_md.FGstatic
    abt_md::FGstatic*   ind_gstatic_next;   // hash next
    algo::Smallstr100   field;              //
    algo::Comment       comment;            //
    // func:abt_md.FGstatic..AssignOp
    inline abt_md::FGstatic& operator =(const abt_md::FGstatic &rhs) = delete;
    // func:abt_md.FGstatic..CopyCtor
    inline               FGstatic(const abt_md::FGstatic &rhs) = delete;
private:
    // func:abt_md.FGstatic..Ctor
    inline               FGstatic() __attribute__((nothrow));
    // func:abt_md.FGstatic..Dtor
    inline               ~FGstatic() __attribute__((nothrow));
    friend abt_md::FGstatic&    gstatic_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend abt_md::FGstatic*    gstatic_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 gstatic_RemoveAll() __attribute__((nothrow));
    friend void                 gstatic_RemoveLast() __attribute__((nothrow));
};
```

#### abt_md.FHumanText - 
<a href="#abt_md-fhumantext"></a>

#### abt_md.FHumanText Fields
<a href="#abt_md-fhumantext-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|abt_md.FHumanText.key|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|abt_md.FHumanText.text|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|abt_md.FHumanText.used|bool|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FHumanText
<a href="#struct-fhumantext"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/abt_md_gen.h](/include/gen/abt_md_gen.h)
```
struct FHumanText { // abt_md.FHumanText
    abt_md::FHumanText*   human_text_next;       // Pointer to next free element int tpool
    abt_md::FHumanText*   ind_human_text_next;   // hash next
    algo::cstring         key;                   //
    algo::cstring         text;                  //
    bool                  used;                  //   false
    // func:abt_md.FHumanText..AssignOp
    inline abt_md::FHumanText& operator =(const abt_md::FHumanText &rhs) = delete;
    // func:abt_md.FHumanText..CopyCtor
    inline               FHumanText(const abt_md::FHumanText &rhs) = delete;
private:
    // func:abt_md.FHumanText..Ctor
    inline               FHumanText() __attribute__((nothrow));
    // func:abt_md.FHumanText..Dtor
    inline               ~FHumanText() __attribute__((nothrow));
    friend abt_md::FHumanText&  human_text_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend abt_md::FHumanText*  human_text_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 human_text_Delete(abt_md::FHumanText &row) __attribute__((nothrow));
};
```

#### abt_md.FLink - 
<a href="#abt_md-flink"></a>

#### abt_md.FLink Fields
<a href="#abt_md-flink-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|abt_md.FLink.location|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||txt/exe/acr.md:31|
|abt_md.FLink.text|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||txt/exe/acr.md:31|
|abt_md.FLink.target|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||URL|

#### Struct FLink
<a href="#struct-flink"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/abt_md_gen.h](/include/gen/abt_md_gen.h)
```
struct FLink { // abt_md.FLink
    algo::cstring   location;   // txt/exe/acr.md:31
    algo::cstring   text;       // txt/exe/acr.md:31
    algo::cstring   target;     // URL
private:
    // func:abt_md.FLink..Ctor
    inline               FLink() __attribute__((nothrow));
    friend abt_md::FLink&       link_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend abt_md::FLink*       link_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 link_RemoveAll() __attribute__((nothrow));
    friend void                 link_RemoveLast() __attribute__((nothrow));
};
```

#### abt_md.FNs - Namespace (for in-memory database, protocol, etc)
<a href="#abt_md-fns"></a>

#### abt_md.FNs Fields
<a href="#abt_md-fns-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|abt_md.FNs.base|[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)|[Base](/txt/ssimdb/dmmeta/ns.md)|||
|abt_md.FNs.c_readme|[abt_md.FReadme](/txt/exe/abt_md/internals.md#abt_md-freadme)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|abt_md.FNs.c_targsrc|[abt_md.FTargsrc](/txt/exe/abt_md/internals.md#abt_md-ftargsrc)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|abt_md.FNs.c_ctype|[abt_md.FCtype](/txt/exe/abt_md/internals.md#abt_md-fctype)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|abt_md.FNs.c_comptest|[abt_md.FComptest](/txt/exe/abt_md/internals.md#abt_md-fcomptest)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|abt_md.FNs.c_target|[abt_md.FTarget](/txt/exe/abt_md/internals.md#abt_md-ftarget)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||

#### Struct FNs
<a href="#struct-fns"></a>
*Note:* field ``abt_md.FNs.base`` has reftype ``base`` so the fields of [dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/abt_md_gen.h](/include/gen/abt_md_gen.h)
```
struct FNs { // abt_md.FNs
    abt_md::FNs*          ind_ns_next;        // hash next
    algo::Smallstr16      ns;                 // Namespace name (primary key)
    algo::Smallstr50      nstype;             // Namespace type
    algo::Smallstr50      license;            // Associated license
    algo::Comment         comment;            //
    abt_md::FReadme*      c_readme;           // optional pointer
    abt_md::FTargsrc**    c_targsrc_elems;    // array of pointers
    u32                   c_targsrc_n;        // array of pointers
    u32                   c_targsrc_max;      // capacity of allocated array
    abt_md::FCtype**      c_ctype_elems;      // array of pointers
    u32                   c_ctype_n;          // array of pointers
    u32                   c_ctype_max;        // capacity of allocated array
    abt_md::FComptest**   c_comptest_elems;   // array of pointers
    u32                   c_comptest_n;       // array of pointers
    u32                   c_comptest_max;     // capacity of allocated array
    abt_md::FTarget*      c_target;           // optional pointer
    // reftype Ptrary of abt_md.FNs.c_targsrc prohibits copy
    // reftype Ptrary of abt_md.FNs.c_ctype prohibits copy
    // reftype Ptrary of abt_md.FNs.c_comptest prohibits copy
    // x-reference on abt_md.FNs.c_target prevents copy
    // func:abt_md.FNs..AssignOp
    abt_md::FNs&         operator =(const abt_md::FNs &rhs) = delete;
    // reftype Ptrary of abt_md.FNs.c_targsrc prohibits copy
    // reftype Ptrary of abt_md.FNs.c_ctype prohibits copy
    // reftype Ptrary of abt_md.FNs.c_comptest prohibits copy
    // x-reference on abt_md.FNs.c_target prevents copy
    // func:abt_md.FNs..CopyCtor
    FNs(const abt_md::FNs &rhs) = delete;
private:
    // func:abt_md.FNs..Ctor
    inline               FNs() __attribute__((nothrow));
    // func:abt_md.FNs..Dtor
    inline               ~FNs() __attribute__((nothrow));
    friend abt_md::FNs&         ns_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend abt_md::FNs*         ns_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 ns_RemoveAll() __attribute__((nothrow));
    friend void                 ns_RemoveLast() __attribute__((nothrow));
};
```

#### abt_md.FNstype - Namespace type
<a href="#abt_md-fnstype"></a>

#### abt_md.FNstype Fields
<a href="#abt_md-fnstype-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|abt_md.FNstype.base|[dmmeta.Nstype](/txt/ssimdb/dmmeta/nstype.md)|[Base](/txt/ssimdb/dmmeta/nstype.md)|||

#### Struct FNstype
<a href="#struct-fnstype"></a>
*Note:* field ``abt_md.FNstype.base`` has reftype ``base`` so the fields of [dmmeta.Nstype](/txt/ssimdb/dmmeta/nstype.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/abt_md_gen.h](/include/gen/abt_md_gen.h)
```
struct FNstype { // abt_md.FNstype
    abt_md::FNstype*   ind_nstype_next;   // hash next
    algo::Smallstr50   nstype;            //
    algo::Comment      comment;           //
    // func:abt_md.FNstype..AssignOp
    inline abt_md::FNstype& operator =(const abt_md::FNstype &rhs) = delete;
    // func:abt_md.FNstype..CopyCtor
    inline               FNstype(const abt_md::FNstype &rhs) = delete;
private:
    // func:abt_md.FNstype..Ctor
    inline               FNstype() __attribute__((nothrow));
    // func:abt_md.FNstype..Dtor
    inline               ~FNstype() __attribute__((nothrow));
    friend abt_md::FNstype&     nstype_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend abt_md::FNstype*     nstype_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 nstype_RemoveAll() __attribute__((nothrow));
    friend void                 nstype_RemoveLast() __attribute__((nothrow));
};
```

#### abt_md.FReadme - File containing documentation
<a href="#abt_md-freadme"></a>

#### abt_md.FReadme Fields
<a href="#abt_md-freadme-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|abt_md.FReadme.base|[dev.Readme](/txt/ssimdb/dev/readme.md)|[Base](/txt/ssimdb/dev/readme.md)|||
|abt_md.FReadme.p_ns|[abt_md.FNs](/txt/exe/abt_md/internals.md#abt_md-fns)|[Upptr](/txt/exe/amc/reftypes.md#upptr)||Optional Associated namespace|
|abt_md.FReadme.p_ssimfile|[abt_md.FSsimfile](/txt/exe/abt_md/internals.md#abt_md-fssimfile)|[Upptr](/txt/exe/amc/reftypes.md#upptr)||Optional Associated ssimfile|
|abt_md.FReadme.p_ctype|[abt_md.FCtype](/txt/exe/abt_md/internals.md#abt_md-fctype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)||Optional Associated ctype|
|abt_md.FReadme.p_scriptfile|[abt_md.FScriptfile](/txt/exe/abt_md/internals.md#abt_md-fscriptfile)|[Upptr](/txt/exe/amc/reftypes.md#upptr)||Optional Associated scriptfile|
|abt_md.FReadme.select|bool|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FReadme
<a href="#struct-freadme"></a>
*Note:* field ``abt_md.FReadme.base`` has reftype ``base`` so the fields of [dev.Readme](/txt/ssimdb/dev/readme.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/abt_md_gen.h](/include/gen/abt_md_gen.h)
```
struct FReadme { // abt_md.FReadme
    abt_md::FReadme*       ind_readme_next;   // hash next
    algo::Smallstr200      gitfile;           //
    bool                   inl;               //   false
    bool                   sandbox;           //   false
    algo::Smallstr100      filter;            //
    algo::Comment          comment;           //
    abt_md::FNs*           p_ns;              // reference to parent row
    abt_md::FSsimfile*     p_ssimfile;        // reference to parent row
    abt_md::FCtype*        p_ctype;           // reference to parent row
    abt_md::FScriptfile*   p_scriptfile;      // reference to parent row
    bool                   select;            //   false
    // func:abt_md.FReadme..AssignOp
    abt_md::FReadme&     operator =(const abt_md::FReadme &rhs) = delete;
    // func:abt_md.FReadme..CopyCtor
    FReadme(const abt_md::FReadme &rhs) = delete;
private:
    // func:abt_md.FReadme..Ctor
    inline               FReadme() __attribute__((nothrow));
    // func:abt_md.FReadme..Dtor
    inline               ~FReadme() __attribute__((nothrow));
    friend abt_md::FReadme&     readme_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend abt_md::FReadme*     readme_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 readme_RemoveAll() __attribute__((nothrow));
    friend void                 readme_RemoveLast() __attribute__((nothrow));
};
```

#### abt_md.FReadmesort - sorted categories of txt/ *.md files presented in dev.readme
<a href="#abt_md-freadmesort"></a>

#### abt_md.FReadmesort Fields
<a href="#abt_md-freadmesort-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|abt_md.FReadmesort.base|[dev.Readmesort](/txt/ssimdb/dev/readmesort.md)|[Base](/txt/ssimdb/dev/readmesort.md)|||
|abt_md.FReadmesort.sortfld|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FReadmesort
<a href="#struct-freadmesort"></a>
*Note:* field ``abt_md.FReadmesort.base`` has reftype ``base`` so the fields of [dev.Readmesort](/txt/ssimdb/dev/readmesort.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/abt_md_gen.h](/include/gen/abt_md_gen.h)
```
struct FReadmesort { // abt_md.FReadmesort
    abt_md::FReadmesort*   ind_readmesort_next;   // hash next
    algo::Smallstr250      readmesort;            //
    algo::Comment          comment;               //
    algo::cstring          sortfld;               //
    // func:abt_md.FReadmesort..AssignOp
    inline abt_md::FReadmesort& operator =(const abt_md::FReadmesort &rhs) = delete;
    // func:abt_md.FReadmesort..CopyCtor
    inline               FReadmesort(const abt_md::FReadmesort &rhs) = delete;
private:
    // func:abt_md.FReadmesort..Ctor
    inline               FReadmesort() __attribute__((nothrow));
    // func:abt_md.FReadmesort..Dtor
    inline               ~FReadmesort() __attribute__((nothrow));
    friend abt_md::FReadmesort& readmesort_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend abt_md::FReadmesort* readmesort_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 readmesort_RemoveAll() __attribute__((nothrow));
    friend void                 readmesort_RemoveLast() __attribute__((nothrow));
};
```

#### abt_md.FReftype - Field type constructor (e.g. reference type)
<a href="#abt_md-freftype"></a>

#### abt_md.FReftype Fields
<a href="#abt_md-freftype-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|abt_md.FReftype.base|[dmmeta.Reftype](/txt/ssimdb/dmmeta/reftype.md)|[Base](/txt/ssimdb/dmmeta/reftype.md)|||
|abt_md.FReftype.p_tclass|[abt_md.FTclass](/txt/exe/abt_md/internals.md#abt_md-ftclass)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FReftype
<a href="#struct-freftype"></a>
*Note:* field ``abt_md.FReftype.base`` has reftype ``base`` so the fields of [dmmeta.Reftype](/txt/ssimdb/dmmeta/reftype.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/abt_md_gen.h](/include/gen/abt_md_gen.h)
```
struct FReftype { // abt_md.FReftype
    algo::Smallstr50   reftype;       //   "Val"
    bool               isval;         //   false  True if field makes values of target type
    bool               cascins;       //   false  Field is cascade-insert
    bool               usebasepool;   //   false  Fields with this type make use of dmmeta.basepool
    bool               cancopy;       //   false  This type of field can be copied
    bool               isxref;        //   false  This type of field is an x-ref
    bool               del;           //   false  Supports random deletion?
    bool               up;            //   false  This type of field is a reference
    bool               isnew;         //   false  If set, skip this relation in amc_vis
    bool               hasalloc;      //   false  Generte Alloc/Delete functions for arg type
    bool               inst;          //   false  Field creates an instance of arg type (directly or indirectly)
    bool               varlen;        //   false  This pool supports varlen allocations
    abt_md::FTclass*   p_tclass;      // reference to parent row
private:
    // func:abt_md.FReftype..Ctor
    inline               FReftype() __attribute__((nothrow));
    friend abt_md::FReftype&    reftype_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend abt_md::FReftype*    reftype_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 reftype_RemoveAll() __attribute__((nothrow));
    friend void                 reftype_RemoveLast() __attribute__((nothrow));
};
```

#### abt_md.FScriptfile - Known script file
<a href="#abt_md-fscriptfile"></a>

#### abt_md.FScriptfile Fields
<a href="#abt_md-fscriptfile-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|abt_md.FScriptfile.base|[dev.Scriptfile](/txt/ssimdb/dev/scriptfile.md)|[Base](/txt/ssimdb/dev/scriptfile.md)|||

#### Struct FScriptfile
<a href="#struct-fscriptfile"></a>
*Note:* field ``abt_md.FScriptfile.base`` has reftype ``base`` so the fields of [dev.Scriptfile](/txt/ssimdb/dev/scriptfile.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/abt_md_gen.h](/include/gen/abt_md_gen.h)
```
struct FScriptfile { // abt_md.FScriptfile
    abt_md::FScriptfile*   ind_scriptfile_next;   // hash next
    algo::Smallstr200      gitfile;               //
    algo::Smallstr50       license;               //
    algo::Comment          comment;               //
    // func:abt_md.FScriptfile..AssignOp
    inline abt_md::FScriptfile& operator =(const abt_md::FScriptfile &rhs) = delete;
    // func:abt_md.FScriptfile..CopyCtor
    inline               FScriptfile(const abt_md::FScriptfile &rhs) = delete;
private:
    // func:abt_md.FScriptfile..Ctor
    inline               FScriptfile() __attribute__((nothrow));
    // func:abt_md.FScriptfile..Dtor
    inline               ~FScriptfile() __attribute__((nothrow));
    friend abt_md::FScriptfile& scriptfile_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend abt_md::FScriptfile* scriptfile_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 scriptfile_RemoveAll() __attribute__((nothrow));
    friend void                 scriptfile_RemoveLast() __attribute__((nothrow));
};
```

#### abt_md.FSsimfile - File with ssim tuples
<a href="#abt_md-fssimfile"></a>

#### abt_md.FSsimfile Fields
<a href="#abt_md-fssimfile-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|abt_md.FSsimfile.base|[dmmeta.Ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|[Base](/txt/ssimdb/dmmeta/ssimfile.md)|||
|abt_md.FSsimfile.p_ctype|[abt_md.FCtype](/txt/exe/abt_md/internals.md#abt_md-fctype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FSsimfile
<a href="#struct-fssimfile"></a>
*Note:* field ``abt_md.FSsimfile.base`` has reftype ``base`` so the fields of [dmmeta.Ssimfile](/txt/ssimdb/dmmeta/ssimfile.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/abt_md_gen.h](/include/gen/abt_md_gen.h)
```
struct FSsimfile { // abt_md.FSsimfile
    abt_md::FSsimfile*   ind_ssimfile_next;   // hash next
    algo::Smallstr50     ssimfile;            //
    algo::Smallstr100    ctype;               //
    abt_md::FCtype*      p_ctype;             // reference to parent row
    // x-reference on abt_md.FSsimfile.p_ctype prevents copy
    // func:abt_md.FSsimfile..AssignOp
    inline abt_md::FSsimfile& operator =(const abt_md::FSsimfile &rhs) = delete;
    // x-reference on abt_md.FSsimfile.p_ctype prevents copy
    // func:abt_md.FSsimfile..CopyCtor
    inline               FSsimfile(const abt_md::FSsimfile &rhs) = delete;
private:
    // func:abt_md.FSsimfile..Ctor
    inline               FSsimfile() __attribute__((nothrow));
    // func:abt_md.FSsimfile..Dtor
    inline               ~FSsimfile() __attribute__((nothrow));
    friend abt_md::FSsimfile&   ssimfile_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend abt_md::FSsimfile*   ssimfile_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 ssimfile_RemoveAll() __attribute__((nothrow));
    friend void                 ssimfile_RemoveLast() __attribute__((nothrow));
};
```

#### abt_md.FSubstr - Specify that the field value is computed from a substring of another field
<a href="#abt_md-fsubstr"></a>

#### abt_md.FSubstr Fields
<a href="#abt_md-fsubstr-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|abt_md.FSubstr.base|[dmmeta.Substr](/txt/ssimdb/dmmeta/substr.md)|[Base](/txt/ssimdb/dmmeta/substr.md)|||
|abt_md.FSubstr.p_srcfield|[abt_md.FField](/txt/exe/abt_md/internals.md#abt_md-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FSubstr
<a href="#struct-fsubstr"></a>
*Note:* field ``abt_md.FSubstr.base`` has reftype ``base`` so the fields of [dmmeta.Substr](/txt/ssimdb/dmmeta/substr.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/abt_md_gen.h](/include/gen/abt_md_gen.h)
```
struct FSubstr { // abt_md.FSubstr
    algo::Smallstr100   field;        //
    algo::CppExpr       expr;         //
    algo::Smallstr100   srcfield;     //
    abt_md::FField*     p_srcfield;   // reference to parent row
    // x-reference on abt_md.FSubstr.p_srcfield prevents copy
    // func:abt_md.FSubstr..AssignOp
    inline abt_md::FSubstr& operator =(const abt_md::FSubstr &rhs) = delete;
    // x-reference on abt_md.FSubstr.p_srcfield prevents copy
    // func:abt_md.FSubstr..CopyCtor
    inline               FSubstr(const abt_md::FSubstr &rhs) = delete;
private:
    // func:abt_md.FSubstr..Ctor
    inline               FSubstr() __attribute__((nothrow));
    // func:abt_md.FSubstr..Dtor
    inline               ~FSubstr() __attribute__((nothrow));
    friend abt_md::FSubstr&     substr_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend abt_md::FSubstr*     substr_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 substr_RemoveAll() __attribute__((nothrow));
    friend void                 substr_RemoveLast() __attribute__((nothrow));
};
```

#### abt_md.FTargdep - Dependency between targets
<a href="#abt_md-ftargdep"></a>

#### abt_md.FTargdep Fields
<a href="#abt_md-ftargdep-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|abt_md.FTargdep.base|[dev.Targdep](/txt/ssimdb/dev/targdep.md)|[Base](/txt/ssimdb/dev/targdep.md)|||
|abt_md.FTargdep.p_parent|[abt_md.FTarget](/txt/exe/abt_md/internals.md#abt_md-ftarget)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FTargdep
<a href="#struct-ftargdep"></a>
*Note:* field ``abt_md.FTargdep.base`` has reftype ``base`` so the fields of [dev.Targdep](/txt/ssimdb/dev/targdep.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/abt_md_gen.h](/include/gen/abt_md_gen.h)
```
struct FTargdep { // abt_md.FTargdep
    algo::Smallstr50   targdep;                   // Primary key - target.parent
    algo::Comment      comment;                   //
    abt_md::FTarget*   p_parent;                  // reference to parent row
    bool               target_c_targdep_in_ary;   //   false  membership flag
    // x-reference on abt_md.FTargdep.p_parent prevents copy
    // func:abt_md.FTargdep..AssignOp
    inline abt_md::FTargdep& operator =(const abt_md::FTargdep &rhs) = delete;
    // x-reference on abt_md.FTargdep.p_parent prevents copy
    // func:abt_md.FTargdep..CopyCtor
    inline               FTargdep(const abt_md::FTargdep &rhs) = delete;
private:
    // func:abt_md.FTargdep..Ctor
    inline               FTargdep() __attribute__((nothrow));
    // func:abt_md.FTargdep..Dtor
    inline               ~FTargdep() __attribute__((nothrow));
    friend abt_md::FTargdep&    targdep_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend abt_md::FTargdep*    targdep_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 targdep_RemoveAll() __attribute__((nothrow));
    friend void                 targdep_RemoveLast() __attribute__((nothrow));
};
```

#### abt_md.FTarget - Build target
<a href="#abt_md-ftarget"></a>

#### abt_md.FTarget Fields
<a href="#abt_md-ftarget-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|abt_md.FTarget.base|[dev.Target](/txt/ssimdb/dev/target.md)|[Base](/txt/ssimdb/dev/target.md)|||
|abt_md.FTarget.c_targdep|[abt_md.FTargdep](/txt/exe/abt_md/internals.md#abt_md-ftargdep)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|abt_md.FTarget.p_ns|[abt_md.FNs](/txt/exe/abt_md/internals.md#abt_md-fns)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FTarget
<a href="#struct-ftarget"></a>
*Note:* field ``abt_md.FTarget.base`` has reftype ``base`` so the fields of [dev.Target](/txt/ssimdb/dev/target.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/abt_md_gen.h](/include/gen/abt_md_gen.h)
```
struct FTarget { // abt_md.FTarget
    abt_md::FTarget*     ind_target_next;   // hash next
    algo::Smallstr16     target;            // Primary key - name of target
    abt_md::FTargdep**   c_targdep_elems;   // array of pointers
    u32                  c_targdep_n;       // array of pointers
    u32                  c_targdep_max;     // capacity of allocated array
    abt_md::FNs*         p_ns;              // reference to parent row
    // reftype Ptrary of abt_md.FTarget.c_targdep prohibits copy
    // x-reference on abt_md.FTarget.p_ns prevents copy
    // func:abt_md.FTarget..AssignOp
    inline abt_md::FTarget& operator =(const abt_md::FTarget &rhs) = delete;
    // reftype Ptrary of abt_md.FTarget.c_targdep prohibits copy
    // x-reference on abt_md.FTarget.p_ns prevents copy
    // func:abt_md.FTarget..CopyCtor
    inline               FTarget(const abt_md::FTarget &rhs) = delete;
private:
    // func:abt_md.FTarget..Ctor
    inline               FTarget() __attribute__((nothrow));
    // func:abt_md.FTarget..Dtor
    inline               ~FTarget() __attribute__((nothrow));
    friend abt_md::FTarget&     target_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend abt_md::FTarget*     target_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 target_RemoveAll() __attribute__((nothrow));
    friend void                 target_RemoveLast() __attribute__((nothrow));
};
```

#### abt_md.FTargsrc - List of sources for target
<a href="#abt_md-ftargsrc"></a>

#### abt_md.FTargsrc Fields
<a href="#abt_md-ftargsrc-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|abt_md.FTargsrc.base|[dev.Targsrc](/txt/ssimdb/dev/targsrc.md)|[Base](/txt/ssimdb/dev/targsrc.md)|||

#### Struct FTargsrc
<a href="#struct-ftargsrc"></a>
*Note:* field ``abt_md.FTargsrc.base`` has reftype ``base`` so the fields of [dev.Targsrc](/txt/ssimdb/dev/targsrc.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/abt_md_gen.h](/include/gen/abt_md_gen.h)
```
struct FTargsrc { // abt_md.FTargsrc
    algo::Smallstr100   targsrc;               //
    algo::Comment       comment;               //
    bool                ns_c_targsrc_in_ary;   //   false  membership flag
    // func:abt_md.FTargsrc..AssignOp
    inline abt_md::FTargsrc& operator =(const abt_md::FTargsrc &rhs) = delete;
    // func:abt_md.FTargsrc..CopyCtor
    inline               FTargsrc(const abt_md::FTargsrc &rhs) = delete;
private:
    // func:abt_md.FTargsrc..Ctor
    inline               FTargsrc() __attribute__((nothrow));
    // func:abt_md.FTargsrc..Dtor
    inline               ~FTargsrc() __attribute__((nothrow));
    friend abt_md::FTargsrc&    targsrc_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend abt_md::FTargsrc*    targsrc_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 targsrc_RemoveAll() __attribute__((nothrow));
    friend void                 targsrc_RemoveLast() __attribute__((nothrow));
};
```

#### abt_md.FTclass - AMC template class
<a href="#abt_md-ftclass"></a>

#### abt_md.FTclass Fields
<a href="#abt_md-ftclass-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|abt_md.FTclass.base|[amcdb.Tclass](/txt/ssimdb/amcdb/tclass.md)|[Base](/txt/ssimdb/amcdb/tclass.md)|||

#### Struct FTclass
<a href="#struct-ftclass"></a>
*Note:* field ``abt_md.FTclass.base`` has reftype ``base`` so the fields of [amcdb.Tclass](/txt/ssimdb/amcdb/tclass.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/abt_md_gen.h](/include/gen/abt_md_gen.h)
```
struct FTclass { // abt_md.FTclass
    abt_md::FTclass*   ind_tclass_next;   // hash next
    algo::Smallstr50   tclass;            //
    algo::Comment      comment;           //
    // func:abt_md.FTclass..AssignOp
    inline abt_md::FTclass& operator =(const abt_md::FTclass &rhs) = delete;
    // func:abt_md.FTclass..CopyCtor
    inline               FTclass(const abt_md::FTclass &rhs) = delete;
private:
    // func:abt_md.FTclass..Ctor
    inline               FTclass() __attribute__((nothrow));
    // func:abt_md.FTclass..Dtor
    inline               ~FTclass() __attribute__((nothrow));
    friend abt_md::FTclass&     tclass_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend abt_md::FTclass*     tclass_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 tclass_RemoveAll() __attribute__((nothrow));
    friend void                 tclass_RemoveLast() __attribute__((nothrow));
};
```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Imdb -->


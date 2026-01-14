## gcli - Internals


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
for usage, see [gcli - Gcli - gitlab/github command-line client](/txt/exe/gcli/README.md)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Description -->

### Sources
<a href="#sources"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Sources -->
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/gcli/git.cpp](/cpp/gcli/git.cpp)||
|[cpp/gcli/gtblact.cpp](/cpp/gcli/gtblact.cpp)||
|[cpp/gcli/help.cpp](/cpp/gcli/help.cpp)||
|[cpp/gcli/issue.cpp](/cpp/gcli/issue.cpp)||
|[cpp/gcli/main.cpp](/cpp/gcli/main.cpp)||
|[cpp/gcli/milestone.cpp](/cpp/gcli/milestone.cpp)||
|[cpp/gcli/mr.cpp](/cpp/gcli/mr.cpp)||
|[cpp/gcli/note.cpp](/cpp/gcli/note.cpp)||
|[cpp/gcli/repo.cpp](/cpp/gcli/repo.cpp)||
|[cpp/gcli/rest.cpp](/cpp/gcli/rest.cpp)||
|[cpp/gcli/search.cpp](/cpp/gcli/search.cpp)||
|[cpp/gcli/user.cpp](/cpp/gcli/user.cpp)||
|[cpp/gen/gcli_gen.cpp](/cpp/gen/gcli_gen.cpp)||
|[include/gcli.h](/include/gcli.h)||
|[include/gen/gcli_gen.h](/include/gen/gcli_gen.h)||
|[include/gen/gcli_gen.inl.h](/include/gen/gcli_gen.inl.h)||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Sources -->

### Dependencies
<a href="#dependencies"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Dependencies -->
The build target depends on the following libraries
|Target|Comment|
|---|---|
|[algo_lib](/txt/lib/algo_lib/README.md)|Support library for all executables|
|[lib_json](/txt/lib/lib_json/README.md)|Full json support library|
|[lib_prot](/txt/lib/lib_prot/README.md)|Library covering all protocols|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Dependencies -->

### In Memory DB
<a href="#in-memory-db"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Imdb -->
`gcli` generated code creates the tables below.
All allocations are done through global `gcli::_db` [gcli.FDb](#gcli-fdb) structure
|Ctype|Ssimfile|Create|Access|
|---|---|---|---|
|[gcli.FDb](#gcli-fdb)||FDb._db (Global)|
|[gcli.FGact](#gcli-fgact)|[gclidb.gact](/txt/ssimdb/gclidb/gact.md)|FDb.gact (Lary)|gact (Lary, by rowid)|ind_gact (Thash, hash field gact)|
|[gcli.FGclicmd](#gcli-fgclicmd)|[gclidb.gclicmd](/txt/ssimdb/gclidb/gclicmd.md)|FDb.gclicmd (Lary)|**static**|gclicmd (Lary, by rowid)|ind_gclicmd (Thash, hash field gclicmd)|c_gclicmd (Ptrary)|
||||FGclicmd.step (Hook)|
||||FGclicmd.p_gclicmd (Upptr)|
||||FGclicmdf2j.p_gclicmd (Upptr)|
||||FGclicmdt.p_gclicmd (Upptr)|
|[gcli.FGclicmdarg](#gcli-fgclicmdarg)||FDb.gclicmdarg (Lary)|gclicmdarg (Lary, by rowid)|ind_gclicmdarg (Thash, hash field gclicmdarg)|
||||FGclicmd.c_gclicmdarg (Ptrary)|
|[gcli.FGclicmdc](#gcli-fgclicmdc)||FDb.gclicmdc (Lary)|gclicmdc (Lary, by rowid)|ind_gclicmdc (Thash, hash field gclicmdc)|
||||FGclicmd.c_gclicmdc (Ptrary)|
|[gcli.FGclicmdf](#gcli-fgclicmdf)||FDb.gclicmdf (Lary)|gclicmdf (Lary, by rowid)|ind_gclicmdf (Thash, hash field gclicmdf)|
||||FGclicmdc.c_gclicmdf (Ptrary)|
||||FGclicmdj2f.c_gclicmdf (Ptrary)|
|[gcli.FGclicmdf2j](#gcli-fgclicmdf2j)|[gclidb.gclicmdf2j](/txt/ssimdb/gclidb/gclicmdf2j.md)|FDb.gclicmdf2j (Lary)|gclicmdf2j (Lary, by rowid)|
||||FGclicmd.c_gclicmdf2j (Ptrary)|
|[gcli.FGclicmdj2f](#gcli-fgclicmdj2f)||FDb.gclicmdj2f (Lary)|gclicmdj2f (Lary, by rowid)|ind_gclicmdj2f (Thash, hash field gclicmdj2f)|
|[gcli.FGclicmdt](#gcli-fgclicmdt)|[gclidb.gclicmdt](/txt/ssimdb/gclidb/gclicmdt.md)|FDb.gclicmdt (Lary)|gclicmdt (Lary, by rowid)|ind_gclicmdt (Thash, hash field gclicmdt)|
|[gcli.FGfld](#gcli-fgfld)|[gclidb.gfld](/txt/ssimdb/gclidb/gfld.md)|FDb.gfld (Lary)|gfld (Lary, by rowid)|ind_gfld (Thash, hash field gfld)|c_gfld (Ptrary)|
||||FGtblactfld.p_gfld (Upptr)|
|[gcli.FGithost](#gcli-fgithost)||FDb.githost (Lary)|githost (Lary, by rowid)|ind_githost (Thash, hash field githost)|
|[gcli.FGmethod](#gcli-fgmethod)|[gclidb.gmethod](/txt/ssimdb/gclidb/gmethod.md)|FDb.gmethod (Lary)|gmethod (Lary, by rowid)|ind_gmethod (Thash, hash field gmethod)|
|[gcli.FGrepo](#gcli-fgrepo)|[gclidb.grepo](/txt/ssimdb/gclidb/grepo.md)|FDb.grepo (Lary)|ind_grepo (Thash, hash field grepo)|grepo (Lary, by rowid)|
|[gcli.FGstatet](#gcli-fgstatet)|[gclidb.gstatet](/txt/ssimdb/gclidb/gstatet.md)|FDb.gstatet (Lary)|gstatet (Lary, by rowid)|ind_gstatet (Thash, hash field gstatet)|
|[gcli.FGtbl](#gcli-fgtbl)|[gclidb.gtbl](/txt/ssimdb/gclidb/gtbl.md)|FDb.gtbl (Lary)|p_gtbl (Ptr)|gtbl (Lary, by rowid)|ind_gtbl (Thash, hash field gtbl)|
|[gcli.FGtblact](#gcli-fgtblact)|[gclidb.gtblact](/txt/ssimdb/gclidb/gtblact.md)|FDb.gtblact (Lary)|**static**|gtblact (Lary, by rowid)|ind_gtblact (Thash, hash field gtblact)|zd_gtblact (Llist)|
||||FGtbl.c_gtblact (Ptrary)|
||||FGtblact.step (Hook)|
|[gcli.FGtblactfld](#gcli-fgtblactfld)|[gclidb.gtblactfld](/txt/ssimdb/gclidb/gtblactfld.md)|FDb.gtblactfld (Lary)|gtblactfld (Lary, by rowid)|ind_gtblactfld (Thash, hash field gtblactfld)|
||||FGfld.c_gtblactfld (Ptrary)|
||||FGtblact.c_gtblactfld (Ptrary)|
|[gcli.FGtype](#gcli-fgtype)|[gclidb.gtype](/txt/ssimdb/gclidb/gtype.md)|FDb.gtype (Lary)|gtype (Lary, by rowid)|p_gtype (Ptr)|ind_gtype (Thash, hash field gtype)|
||||FGclicmdt.p_gtype (Upptr)|
||||FGtypeprefix.p_gtype (Upptr)|
|[gcli.FGtypeh](#gcli-fgtypeh)|[gclidb.gtypeh](/txt/ssimdb/gclidb/gtypeh.md)|FDb.gtypeh (Lary)|gtypeh (Lary, by rowid)|
||||FGtype.c_gtypeh (Ptrary)|
|[gcli.FGtypeprefix](#gcli-fgtypeprefix)|[gclidb.gtypeprefix](/txt/ssimdb/gclidb/gtypeprefix.md)|FDb.gtypeprefix (Lary)|gtypeprefix (Lary, by rowid)|
||||FGtype.c_gtypeprefix (Ptrary)|
|[gcli.FHttp](#gcli-fhttp)||
|[gcli.FIssue](#gcli-fissue)|[gclidb.issue](/txt/ssimdb/gclidb/issue.md)|FDb.issue (Lary)|ind_issue (Thash, hash field issue)|issue (Lary, by rowid)|
||||FGclicmd.p_issue (Ptr)|
|[gcli.FIssuenote](#gcli-fissuenote)|[gclidb.issuenote](/txt/ssimdb/gclidb/issuenote.md)|FDb.issuenote (Lary)|issuenote (Lary, by rowid)|ind_issuenote (Thash, hash field issuenote)|
||||FIssue.c_issuenote (Ptrary)|
|[gcli.FMilestone](#gcli-fmilestone)|[gclidb.milestone](/txt/ssimdb/gclidb/milestone.md)|FDb.milestone (Lary)|ind_milestone (Thash, hash field milestone)|milestone (Lary, by rowid)|
|[gcli.FMr](#gcli-fmr)|[gclidb.mr](/txt/ssimdb/gclidb/mr.md)|FDb.mr (Lary)|ind_mr (Thash, hash field mr)|mr (Lary, by rowid)|
||||FGclicmd.p_mr (Ptr)|
||||FIssue.p_mr_open (Ptr)|
|[gcli.FMrjob](#gcli-fmrjob)|[gclidb.mrjob](/txt/ssimdb/gclidb/mrjob.md)|FDb.mrjob (Lary)|mrjob (Lary, by rowid)|ind_mrjob (Thash, hash field mrjob)|
||||FIssue.c_mrjob (Ptrary)|
||||FMr.c_mrjob (Ptrary)|
|[gcli.FMrnote](#gcli-fmrnote)|[gclidb.mrnote](/txt/ssimdb/gclidb/mrnote.md)|FDb.mrnote (Lary)|mrnote (Lary, by rowid)|ind_mrnote (Thash, hash field mrnote)|
||||FMr.c_mrnote (Ptrary)|
|[gcli.FTuples](#gcli-ftuples)||FDb.tuples (Lary)|tuples (Lary, by rowid)|
||||FGclicmd.c_tuples (Ptrary)|
|[gcli.FUser](#gcli-fuser)|[gclidb.user](/txt/ssimdb/gclidb/user.md)|FDb.user (Lary)|user (Lary, by rowid)|ind_user (Thash, hash field user)|

#### gcli.FDb - In-memory database for gcli
<a href="#gcli-fdb"></a>

#### gcli.FDb Fields
<a href="#gcli-fdb-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gcli.FDb._db|[gcli.FDb](/txt/exe/gcli/internals.md#gcli-fdb)|[Global](/txt/exe/amc/reftypes.md#global)|||
|gcli.FDb.cmdline|[command.gcli](/txt/protocol/command/README.md#command-gcli)|[Val](/txt/exe/amc/reftypes.md#val)|||
|gcli.FDb.gtype|[gcli.FGtype](/txt/exe/gcli/internals.md#gcli-fgtype)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|gcli.FDb.githost|[gcli.FGithost](/txt/exe/gcli/internals.md#gcli-fgithost)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|gcli.FDb.need_auth|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|gcli.FDb.gstatet|[gcli.FGstatet](/txt/exe/gcli/internals.md#gcli-fgstatet)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|gcli.FDb.gmethod|[gcli.FGmethod](/txt/exe/gcli/internals.md#gcli-fgmethod)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|gcli.FDb.gclicmdt|[gcli.FGclicmdt](/txt/exe/gcli/internals.md#gcli-fgclicmdt)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|gcli.FDb.gclicmdf|[gcli.FGclicmdf](/txt/exe/gcli/internals.md#gcli-fgclicmdf)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|gcli.FDb.gclicmdf2j|[gcli.FGclicmdf2j](/txt/exe/gcli/internals.md#gcli-fgclicmdf2j)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|gcli.FDb.gclicmd|[gcli.FGclicmd](/txt/exe/gcli/internals.md#gcli-fgclicmd)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|gcli.FDb.gtypeh|[gcli.FGtypeh](/txt/exe/gcli/internals.md#gcli-fgtypeh)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|gcli.FDb.p_gtype|[gcli.FGtype](/txt/exe/gcli/internals.md#gcli-fgtype)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|gcli.FDb.p_gtbl|[gcli.FGtbl](/txt/exe/gcli/internals.md#gcli-fgtbl)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|gcli.FDb.grepo_sel|[gclidb.Grepo](/txt/ssimdb/gclidb/grepo.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|gcli.FDb.ind_gclicmd|[gcli.FGclicmd](/txt/exe/gcli/internals.md#gcli-fgclicmd)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|gcli.FDb.ind_gclicmdf|[gcli.FGclicmdf](/txt/exe/gcli/internals.md#gcli-fgclicmdf)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|gcli.FDb.ind_gclicmdt|[gcli.FGclicmdt](/txt/exe/gcli/internals.md#gcli-fgclicmdt)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|gcli.FDb.ind_gmethod|[gcli.FGmethod](/txt/exe/gcli/internals.md#gcli-fgmethod)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|gcli.FDb.ind_grepo|[gcli.FGrepo](/txt/exe/gcli/internals.md#gcli-fgrepo)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|gcli.FDb.ind_issue|[gcli.FIssue](/txt/exe/gcli/internals.md#gcli-fissue)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|gcli.FDb.regx_repo|[algo.cstring](/txt/protocol/algo/cstring.md)|[RegxSql](/txt/exe/amc/reftypes.md#regxsql)|"%"||
|gcli.FDb.ind_gstatet|[gcli.FGstatet](/txt/exe/gcli/internals.md#gcli-fgstatet)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|gcli.FDb.ind_githost|[gcli.FGithost](/txt/exe/gcli/internals.md#gcli-fgithost)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|gcli.FDb.ind_gtype|[gcli.FGtype](/txt/exe/gcli/internals.md#gcli-fgtype)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|gcli.FDb.home|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||User's HOME directory|
|gcli.FDb.auth_file|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|gcli.FDb.edit_file|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|gcli.FDb.c_gclicmd|[gcli.FGclicmd](/txt/exe/gcli/internals.md#gcli-fgclicmd)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|gcli.FDb.unix_user|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||UNIX user login name|
|gcli.FDb.editor|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Command line to invoke editor|
|gcli.FDb.gclicmdj2f|[gcli.FGclicmdj2f](/txt/exe/gcli/internals.md#gcli-fgclicmdj2f)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|gcli.FDb.gclicmdc|[gcli.FGclicmdc](/txt/exe/gcli/internals.md#gcli-fgclicmdc)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|gcli.FDb.gclicmdarg|[gcli.FGclicmdarg](/txt/exe/gcli/internals.md#gcli-fgclicmdarg)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|gcli.FDb.ind_gclicmdj2f|[gcli.FGclicmdj2f](/txt/exe/gcli/internals.md#gcli-fgclicmdj2f)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|gcli.FDb.ind_gclicmdarg|[gcli.FGclicmdarg](/txt/exe/gcli/internals.md#gcli-fgclicmdarg)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|gcli.FDb.ind_gclicmdc|[gcli.FGclicmdc](/txt/exe/gcli/internals.md#gcli-fgclicmdc)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|gcli.FDb.issue|[gcli.FIssue](/txt/exe/gcli/internals.md#gcli-fissue)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|gcli.FDb.grepo|[gcli.FGrepo](/txt/exe/gcli/internals.md#gcli-fgrepo)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|gcli.FDb.tuples|[gcli.FTuples](/txt/exe/gcli/internals.md#gcli-ftuples)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|gcli.FDb.issuenote|[gcli.FIssuenote](/txt/exe/gcli/internals.md#gcli-fissuenote)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|gcli.FDb.mrjob|[gcli.FMrjob](/txt/exe/gcli/internals.md#gcli-fmrjob)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|gcli.FDb.mrnote|[gcli.FMrnote](/txt/exe/gcli/internals.md#gcli-fmrnote)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|gcli.FDb.ind_mrnote|[gcli.FMrnote](/txt/exe/gcli/internals.md#gcli-fmrnote)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|gcli.FDb.ind_issuenote|[gcli.FIssuenote](/txt/exe/gcli/internals.md#gcli-fissuenote)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|gcli.FDb.ind_mrjob|[gcli.FMrjob](/txt/exe/gcli/internals.md#gcli-fmrjob)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|gcli.FDb.user|[gcli.FUser](/txt/exe/gcli/internals.md#gcli-fuser)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|gcli.FDb.ind_user|[gcli.FUser](/txt/exe/gcli/internals.md#gcli-fuser)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|gcli.FDb.ind_mr|[gcli.FMr](/txt/exe/gcli/internals.md#gcli-fmr)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|gcli.FDb.ind_milestone|[gcli.FMilestone](/txt/exe/gcli/internals.md#gcli-fmilestone)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|gcli.FDb.milestone|[gcli.FMilestone](/txt/exe/gcli/internals.md#gcli-fmilestone)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|gcli.FDb.mr|[gcli.FMr](/txt/exe/gcli/internals.md#gcli-fmr)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|gcli.FDb.gtypeprefix|[gcli.FGtypeprefix](/txt/exe/gcli/internals.md#gcli-fgtypeprefix)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|gcli.FDb.gtblact|[gcli.FGtblact](/txt/exe/gcli/internals.md#gcli-fgtblact)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|gcli.FDb.gtblactfld|[gcli.FGtblactfld](/txt/exe/gcli/internals.md#gcli-fgtblactfld)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|gcli.FDb.ind_gtblact|[gcli.FGtblact](/txt/exe/gcli/internals.md#gcli-fgtblact)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|gcli.FDb.zd_gtblact|[gcli.FGtblact](/txt/exe/gcli/internals.md#gcli-fgtblact)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|gcli.FDb.ind_gtblactfld|[gcli.FGtblactfld](/txt/exe/gcli/internals.md#gcli-fgtblactfld)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|gcli.FDb.gfld|[gcli.FGfld](/txt/exe/gcli/internals.md#gcli-fgfld)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|gcli.FDb.ind_gfld|[gcli.FGfld](/txt/exe/gcli/internals.md#gcli-fgfld)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|gcli.FDb.c_gfld|[gcli.FGfld](/txt/exe/gcli/internals.md#gcli-fgfld)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|gcli.FDb.gtbl|[gcli.FGtbl](/txt/exe/gcli/internals.md#gcli-fgtbl)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|gcli.FDb.ind_gtbl|[gcli.FGtbl](/txt/exe/gcli/internals.md#gcli-fgtbl)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|gcli.FDb.gact|[gcli.FGact](/txt/exe/gcli/internals.md#gcli-fgact)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|gcli.FDb.ind_gact|[gcli.FGact](/txt/exe/gcli/internals.md#gcli-fgact)|[Thash](/txt/exe/amc/reftypes.md#thash)|||

#### Struct FDb
<a href="#struct-fdb"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/gcli_gen.h](/include/gen/gcli_gen.h)
```
struct FDb { // gcli.FDb: In-memory database for gcli
    command::gcli         cmdline;                        //
    gcli::FGtype*         gtype_lary[32];                 // level array
    i32                   gtype_n;                        // number of elements in array
    gcli::FGithost*       githost_lary[32];               // level array
    i32                   githost_n;                      // number of elements in array
    bool                  need_auth;                      //   false
    gcli::FGstatet*       gstatet_lary[32];               // level array
    i32                   gstatet_n;                      // number of elements in array
    gcli::FGmethod*       gmethod_lary[32];               // level array
    i32                   gmethod_n;                      // number of elements in array
    gcli::FGclicmdt*      gclicmdt_lary[32];              // level array
    i32                   gclicmdt_n;                     // number of elements in array
    gcli::FGclicmdf*      gclicmdf_lary[32];              // level array
    i32                   gclicmdf_n;                     // number of elements in array
    gcli::FGclicmdf2j*    gclicmdf2j_lary[32];            // level array
    i32                   gclicmdf2j_n;                   // number of elements in array
    gcli::FGclicmd*       gclicmd_lary[32];               // level array
    i32                   gclicmd_n;                      // number of elements in array
    gcli::FGtypeh*        gtypeh_lary[32];                // level array
    i32                   gtypeh_n;                       // number of elements in array
    gcli::FGtype*         p_gtype;                        // optional pointer
    gcli::FGtbl*          p_gtbl;                         // optional pointer
    gclidb::Grepo         grepo_sel;                      //
    gcli::FGclicmd**      ind_gclicmd_buckets_elems;      // pointer to bucket array
    i32                   ind_gclicmd_buckets_n;          // number of elements in bucket array
    i32                   ind_gclicmd_n;                  // number of elements in the hash table
    gcli::FGclicmdf**     ind_gclicmdf_buckets_elems;     // pointer to bucket array
    i32                   ind_gclicmdf_buckets_n;         // number of elements in bucket array
    i32                   ind_gclicmdf_n;                 // number of elements in the hash table
    gcli::FGclicmdt**     ind_gclicmdt_buckets_elems;     // pointer to bucket array
    i32                   ind_gclicmdt_buckets_n;         // number of elements in bucket array
    i32                   ind_gclicmdt_n;                 // number of elements in the hash table
    gcli::FGmethod**      ind_gmethod_buckets_elems;      // pointer to bucket array
    i32                   ind_gmethod_buckets_n;          // number of elements in bucket array
    i32                   ind_gmethod_n;                  // number of elements in the hash table
    gcli::FGrepo**        ind_grepo_buckets_elems;        // pointer to bucket array
    i32                   ind_grepo_buckets_n;            // number of elements in bucket array
    i32                   ind_grepo_n;                    // number of elements in the hash table
    gcli::FIssue**        ind_issue_buckets_elems;        // pointer to bucket array
    i32                   ind_issue_buckets_n;            // number of elements in bucket array
    i32                   ind_issue_n;                    // number of elements in the hash table
    algo_lib::Regx        regx_repo;                      //   "%"  Sql Regx
    gcli::FGstatet**      ind_gstatet_buckets_elems;      // pointer to bucket array
    i32                   ind_gstatet_buckets_n;          // number of elements in bucket array
    i32                   ind_gstatet_n;                  // number of elements in the hash table
    gcli::FGithost**      ind_githost_buckets_elems;      // pointer to bucket array
    i32                   ind_githost_buckets_n;          // number of elements in bucket array
    i32                   ind_githost_n;                  // number of elements in the hash table
    gcli::FGtype**        ind_gtype_buckets_elems;        // pointer to bucket array
    i32                   ind_gtype_buckets_n;            // number of elements in bucket array
    i32                   ind_gtype_n;                    // number of elements in the hash table
    algo::cstring         home;                           // User's HOME directory
    algo::cstring         auth_file;                      //
    algo::cstring         edit_file;                      //
    gcli::FGclicmd**      c_gclicmd_elems;                // array of pointers
    u32                   c_gclicmd_n;                    // array of pointers
    u32                   c_gclicmd_max;                  // capacity of allocated array
    algo::cstring         unix_user;                      // UNIX user login name
    algo::cstring         editor;                         // Command line to invoke editor
    gcli::FGclicmdj2f*    gclicmdj2f_lary[32];            // level array
    i32                   gclicmdj2f_n;                   // number of elements in array
    gcli::FGclicmdc*      gclicmdc_lary[32];              // level array
    i32                   gclicmdc_n;                     // number of elements in array
    gcli::FGclicmdarg*    gclicmdarg_lary[32];            // level array
    i32                   gclicmdarg_n;                   // number of elements in array
    gcli::FGclicmdj2f**   ind_gclicmdj2f_buckets_elems;   // pointer to bucket array
    i32                   ind_gclicmdj2f_buckets_n;       // number of elements in bucket array
    i32                   ind_gclicmdj2f_n;               // number of elements in the hash table
    gcli::FGclicmdarg**   ind_gclicmdarg_buckets_elems;   // pointer to bucket array
    i32                   ind_gclicmdarg_buckets_n;       // number of elements in bucket array
    i32                   ind_gclicmdarg_n;               // number of elements in the hash table
    gcli::FGclicmdc**     ind_gclicmdc_buckets_elems;     // pointer to bucket array
    i32                   ind_gclicmdc_buckets_n;         // number of elements in bucket array
    i32                   ind_gclicmdc_n;                 // number of elements in the hash table
    gcli::FIssue*         issue_lary[32];                 // level array
    i32                   issue_n;                        // number of elements in array
    gcli::FGrepo*         grepo_lary[32];                 // level array
    i32                   grepo_n;                        // number of elements in array
    gcli::FTuples*        tuples_lary[32];                // level array
    i32                   tuples_n;                       // number of elements in array
    gcli::FIssuenote*     issuenote_lary[32];             // level array
    i32                   issuenote_n;                    // number of elements in array
    gcli::FMrjob*         mrjob_lary[32];                 // level array
    i32                   mrjob_n;                        // number of elements in array
    gcli::FMrnote*        mrnote_lary[32];                // level array
    i32                   mrnote_n;                       // number of elements in array
    gcli::FMrnote**       ind_mrnote_buckets_elems;       // pointer to bucket array
    i32                   ind_mrnote_buckets_n;           // number of elements in bucket array
    i32                   ind_mrnote_n;                   // number of elements in the hash table
    gcli::FIssuenote**    ind_issuenote_buckets_elems;    // pointer to bucket array
    i32                   ind_issuenote_buckets_n;        // number of elements in bucket array
    i32                   ind_issuenote_n;                // number of elements in the hash table
    gcli::FMrjob**        ind_mrjob_buckets_elems;        // pointer to bucket array
    i32                   ind_mrjob_buckets_n;            // number of elements in bucket array
    i32                   ind_mrjob_n;                    // number of elements in the hash table
    gcli::FUser*          user_lary[32];                  // level array
    i32                   user_n;                         // number of elements in array
    gcli::FUser**         ind_user_buckets_elems;         // pointer to bucket array
    i32                   ind_user_buckets_n;             // number of elements in bucket array
    i32                   ind_user_n;                     // number of elements in the hash table
    gcli::FMr**           ind_mr_buckets_elems;           // pointer to bucket array
    i32                   ind_mr_buckets_n;               // number of elements in bucket array
    i32                   ind_mr_n;                       // number of elements in the hash table
    gcli::FMilestone**    ind_milestone_buckets_elems;    // pointer to bucket array
    i32                   ind_milestone_buckets_n;        // number of elements in bucket array
    i32                   ind_milestone_n;                // number of elements in the hash table
    gcli::FMilestone*     milestone_lary[32];             // level array
    i32                   milestone_n;                    // number of elements in array
    gcli::FMr*            mr_lary[32];                    // level array
    i32                   mr_n;                           // number of elements in array
    gcli::FGtypeprefix*   gtypeprefix_lary[32];           // level array
    i32                   gtypeprefix_n;                  // number of elements in array
    gcli::FGtblact*       gtblact_lary[32];               // level array
    i32                   gtblact_n;                      // number of elements in array
    gcli::FGtblactfld*    gtblactfld_lary[32];            // level array
    i32                   gtblactfld_n;                   // number of elements in array
    gcli::FGtblact**      ind_gtblact_buckets_elems;      // pointer to bucket array
    i32                   ind_gtblact_buckets_n;          // number of elements in bucket array
    i32                   ind_gtblact_n;                  // number of elements in the hash table
    gcli::FGtblact*       zd_gtblact_head;                // zero-terminated doubly linked list
    i32                   zd_gtblact_n;                   // zero-terminated doubly linked list
    gcli::FGtblact*       zd_gtblact_tail;                // pointer to last element
    gcli::FGtblactfld**   ind_gtblactfld_buckets_elems;   // pointer to bucket array
    i32                   ind_gtblactfld_buckets_n;       // number of elements in bucket array
    i32                   ind_gtblactfld_n;               // number of elements in the hash table
    gcli::FGfld*          gfld_lary[32];                  // level array
    i32                   gfld_n;                         // number of elements in array
    gcli::FGfld**         ind_gfld_buckets_elems;         // pointer to bucket array
    i32                   ind_gfld_buckets_n;             // number of elements in bucket array
    i32                   ind_gfld_n;                     // number of elements in the hash table
    gcli::FGfld**         c_gfld_elems;                   // array of pointers
    u32                   c_gfld_n;                       // array of pointers
    u32                   c_gfld_max;                     // capacity of allocated array
    gcli::FGtbl*          gtbl_lary[32];                  // level array
    i32                   gtbl_n;                         // number of elements in array
    gcli::FGtbl**         ind_gtbl_buckets_elems;         // pointer to bucket array
    i32                   ind_gtbl_buckets_n;             // number of elements in bucket array
    i32                   ind_gtbl_n;                     // number of elements in the hash table
    gcli::FGact*          gact_lary[32];                  // level array
    i32                   gact_n;                         // number of elements in array
    gcli::FGact**         ind_gact_buckets_elems;         // pointer to bucket array
    i32                   ind_gact_buckets_n;             // number of elements in bucket array
    i32                   ind_gact_n;                     // number of elements in the hash table
    gcli::trace           trace;                          //
};
```

#### gcli.FGact - Action that may be performed on a table
<a href="#gcli-fgact"></a>

#### gcli.FGact Fields
<a href="#gcli-fgact-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gcli.FGact.base|[gclidb.Gact](/txt/ssimdb/gclidb/gact.md)|[Base](/txt/ssimdb/gclidb/gact.md)|||

#### Struct FGact
<a href="#struct-fgact"></a>
*Note:* field ``gcli.FGact.base`` has reftype ``base`` so the fields of [gclidb.Gact](/txt/ssimdb/gclidb/gact.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/gcli_gen.h](/include/gen/gcli_gen.h)
```
struct FGact { // gcli.FGact
    gcli::FGact*       ind_gact_next;      // hash next
    u32                ind_gact_hashval;   // hash value
    algo::Smallstr50   gact;               //
    algo::Comment      comment;            //
    // func:gcli.FGact..AssignOp
    inline gcli::FGact&  operator =(const gcli::FGact &rhs) = delete;
    // func:gcli.FGact..CopyCtor
    inline               FGact(const gcli::FGact &rhs) = delete;
private:
    // func:gcli.FGact..Ctor
    inline               FGact() __attribute__((nothrow));
    // func:gcli.FGact..Dtor
    inline               ~FGact() __attribute__((nothrow));
    friend gcli::FGact&         gact_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend gcli::FGact*         gact_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 gact_RemoveAll() __attribute__((nothrow));
    friend void                 gact_RemoveLast() __attribute__((nothrow));
};
```

#### gcli.FGclicmd - table of static gcli commands
<a href="#gcli-fgclicmd"></a>

#### gcli.FGclicmd Fields
<a href="#gcli-fgclicmd-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gcli.FGclicmd.base|[gclidb.Gclicmd](/txt/ssimdb/gclidb/gclicmd.md)|[Base](/txt/ssimdb/gclidb/gclicmd.md)|||
|gcli.FGclicmd.url|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|gcli.FGclicmd.sep|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|gcli.FGclicmd.host|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|gcli.FGclicmd.proj|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|gcli.FGclicmd.token|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|gcli.FGclicmd.body|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|gcli.FGclicmd.note_id|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|gcli.FGclicmd.issue_id|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|gcli.FGclicmd.cond|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|gcli.FGclicmd.select|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|gcli.FGclicmd.write_auth|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|gcli.FGclicmd.finish|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|gcli.FGclicmd.gclicmdt_key|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|gcli.FGclicmd.method|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|gcli.FGclicmd.p_issue|[gcli.FIssue](/txt/exe/gcli/internals.md#gcli-fissue)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|gcli.FGclicmd.p_mr|[gcli.FMr](/txt/exe/gcli/internals.md#gcli-fmr)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|gcli.FGclicmd.c_tuples|[gcli.FTuples](/txt/exe/gcli/internals.md#gcli-ftuples)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|gcli.FGclicmd.step|[gcli.FGclicmd](/txt/exe/gcli/internals.md#gcli-fgclicmd)|[Hook](/txt/exe/amc/reftypes.md#hook)|||
|gcli.FGclicmd.c_gclicmdarg|[gcli.FGclicmdarg](/txt/exe/gcli/internals.md#gcli-fgclicmdarg)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|gcli.FGclicmd.c_gclicmdc|[gcli.FGclicmdc](/txt/exe/gcli/internals.md#gcli-fgclicmdc)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|gcli.FGclicmd.c_gclicmdf2j|[gcli.FGclicmdf2j](/txt/exe/gcli/internals.md#gcli-fgclicmdf2j)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|gcli.FGclicmd.p_gclicmd|[gcli.FGclicmd](/txt/exe/gcli/internals.md#gcli-fgclicmd)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FGclicmd
<a href="#struct-fgclicmd"></a>
*Note:* field ``gcli.FGclicmd.base`` has reftype ``base`` so the fields of [gclidb.Gclicmd](/txt/ssimdb/gclidb/gclicmd.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/gcli_gen.h](/include/gen/gcli_gen.h)
```
struct FGclicmd { // gcli.FGclicmd
    gcli::FGclicmd*           ind_gclicmd_next;      // hash next
    u32                       ind_gclicmd_hashval;   // hash value
    algo::Smallstr250         gclicmd;               // table of static commands
    algo::Smallstr250         gclicmdf2j;            // Base for gclicmdf2j
    algo::Comment             comment;               //
    algo::cstring             url;                   //
    algo::cstring             sep;                   //
    algo::cstring             host;                  //
    algo::cstring             proj;                  //
    algo::cstring             token;                 //
    algo::cstring             body;                  //
    algo::cstring             note_id;               //
    algo::cstring             issue_id;              //
    algo::cstring             cond;                  //
    bool                      select;                //   false
    bool                      write_auth;            //   false
    bool                      finish;                //   false
    algo::cstring             gclicmdt_key;          //
    u32                       method;                //   0
    gcli::FIssue*             p_issue;               // optional pointer
    gcli::FMr*                p_mr;                  // optional pointer
    gcli::FTuples**           c_tuples_elems;        // array of pointers
    u32                       c_tuples_n;            // array of pointers
    u32                       c_tuples_max;          // capacity of allocated array
    gcli::gclicmd_step_hook   step;                  //   NULL  Pointer to a function
    gcli::FGclicmdarg**       c_gclicmdarg_elems;    // array of pointers
    u32                       c_gclicmdarg_n;        // array of pointers
    u32                       c_gclicmdarg_max;      // capacity of allocated array
    gcli::FGclicmdc**         c_gclicmdc_elems;      // array of pointers
    u32                       c_gclicmdc_n;          // array of pointers
    u32                       c_gclicmdc_max;        // capacity of allocated array
    gcli::FGclicmdf2j**       c_gclicmdf2j_elems;    // array of pointers
    u32                       c_gclicmdf2j_n;        // array of pointers
    u32                       c_gclicmdf2j_max;      // capacity of allocated array
    gcli::FGclicmd*           p_gclicmd;             // reference to parent row
    bool                      c_gclicmd_in_ary;      //   false  membership flag
    // reftype Ptrary of gcli.FGclicmd.c_tuples prohibits copy
    // reftype Hook of gcli.FGclicmd.step prohibits copy
    // reftype Ptrary of gcli.FGclicmd.c_gclicmdarg prohibits copy
    // reftype Ptrary of gcli.FGclicmd.c_gclicmdc prohibits copy
    // reftype Ptrary of gcli.FGclicmd.c_gclicmdf2j prohibits copy
    // func:gcli.FGclicmd..AssignOp
    gcli::FGclicmd&      operator =(const gcli::FGclicmd &rhs) = delete;
    // reftype Ptrary of gcli.FGclicmd.c_tuples prohibits copy
    // reftype Hook of gcli.FGclicmd.step prohibits copy
    // reftype Ptrary of gcli.FGclicmd.c_gclicmdarg prohibits copy
    // reftype Ptrary of gcli.FGclicmd.c_gclicmdc prohibits copy
    // reftype Ptrary of gcli.FGclicmd.c_gclicmdf2j prohibits copy
    // func:gcli.FGclicmd..CopyCtor
    FGclicmd(const gcli::FGclicmd &rhs) = delete;
private:
    // func:gcli.FGclicmd..Ctor
    inline               FGclicmd() __attribute__((nothrow));
    // func:gcli.FGclicmd..Dtor
    inline               ~FGclicmd() __attribute__((nothrow));
    friend gcli::FGclicmd&      gclicmd_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend gcli::FGclicmd*      gclicmd_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 gclicmd_RemoveAll() __attribute__((nothrow));
    friend void                 gclicmd_RemoveLast() __attribute__((nothrow));
};
```

#### gcli.FGclicmdarg - 
<a href="#gcli-fgclicmdarg"></a>

#### gcli.FGclicmdarg Fields
<a href="#gcli-fgclicmdarg-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gcli.FGclicmdarg.gclicmdarg|[algo.Smallstr250](/txt/protocol/algo/README.md#algo-smallstr250)|[Val](/txt/exe/amc/reftypes.md#val)|||
|*gcli.FGclicmdarg.gclicmd (@LL)*|[gclidb.Gclicmd](/txt/ssimdb/gclidb/gclicmd.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|||
|*gcli.FGclicmdarg.arg (@LR)*|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FGclicmdarg
<a href="#struct-fgclicmdarg"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/gcli_gen.h](/include/gen/gcli_gen.h)
```
struct FGclicmdarg { // gcli.FGclicmdarg
    gcli::FGclicmdarg*   ind_gclicmdarg_next;           // hash next
    u32                  ind_gclicmdarg_hashval;        // hash value
    algo::Smallstr250    gclicmdarg;                    //
    bool                 gclicmd_c_gclicmdarg_in_ary;   //   false  membership flag
    // func:gcli.FGclicmdarg..AssignOp
    inline gcli::FGclicmdarg& operator =(const gcli::FGclicmdarg &rhs) = delete;
    // func:gcli.FGclicmdarg..CopyCtor
    inline               FGclicmdarg(const gcli::FGclicmdarg &rhs) = delete;
private:
    // func:gcli.FGclicmdarg..Ctor
    inline               FGclicmdarg() __attribute__((nothrow));
    // func:gcli.FGclicmdarg..Dtor
    inline               ~FGclicmdarg() __attribute__((nothrow));
    friend gcli::FGclicmdarg&   gclicmdarg_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend gcli::FGclicmdarg*   gclicmdarg_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 gclicmdarg_RemoveAll() __attribute__((nothrow));
    friend void                 gclicmdarg_RemoveLast() __attribute__((nothrow));
};
```

#### gcli.FGclicmdc - 
<a href="#gcli-fgclicmdc"></a>

#### gcli.FGclicmdc Fields
<a href="#gcli-fgclicmdc-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gcli.FGclicmdc.gclicmdc|[algo.Smallstr250](/txt/protocol/algo/README.md#algo-smallstr250)|[Val](/txt/exe/amc/reftypes.md#val)|||
|*gcli.FGclicmdc.gclicmd (/LL)*|[gclidb.Gclicmd](/txt/ssimdb/gclidb/gclicmd.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|||
|*gcli.FGclicmdc.ctype (/LR)*|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|gcli.FGclicmdc.c_gclicmdf|[gcli.FGclicmdf](/txt/exe/gcli/internals.md#gcli-fgclicmdf)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||

#### Struct FGclicmdc
<a href="#struct-fgclicmdc"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/gcli_gen.h](/include/gen/gcli_gen.h)
```
struct FGclicmdc { // gcli.FGclicmdc
    gcli::FGclicmdc*    ind_gclicmdc_next;           // hash next
    u32                 ind_gclicmdc_hashval;        // hash value
    algo::Smallstr250   gclicmdc;                    //
    gcli::FGclicmdf**   c_gclicmdf_elems;            // array of pointers
    u32                 c_gclicmdf_n;                // array of pointers
    u32                 c_gclicmdf_max;              // capacity of allocated array
    bool                gclicmd_c_gclicmdc_in_ary;   //   false  membership flag
    // reftype Ptrary of gcli.FGclicmdc.c_gclicmdf prohibits copy
    // func:gcli.FGclicmdc..AssignOp
    inline gcli::FGclicmdc& operator =(const gcli::FGclicmdc &rhs) = delete;
    // reftype Ptrary of gcli.FGclicmdc.c_gclicmdf prohibits copy
    // func:gcli.FGclicmdc..CopyCtor
    inline               FGclicmdc(const gcli::FGclicmdc &rhs) = delete;
private:
    // func:gcli.FGclicmdc..Ctor
    inline               FGclicmdc() __attribute__((nothrow));
    // func:gcli.FGclicmdc..Dtor
    inline               ~FGclicmdc() __attribute__((nothrow));
    friend gcli::FGclicmdc&     gclicmdc_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend gcli::FGclicmdc*     gclicmdc_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 gclicmdc_RemoveAll() __attribute__((nothrow));
    friend void                 gclicmdc_RemoveLast() __attribute__((nothrow));
};
```

#### gcli.FGclicmdf - fields
<a href="#gcli-fgclicmdf"></a>

#### gcli.FGclicmdf Fields
<a href="#gcli-fgclicmdf-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gcli.FGclicmdf.gclicmdf|[algo.Smallstr250](/txt/protocol/algo/README.md#algo-smallstr250)|[Val](/txt/exe/amc/reftypes.md#val)|||
|*gcli.FGclicmdf.gclicmd (/RL)*|[gclidb.Gclicmd](/txt/ssimdb/gclidb/gclicmd.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|||
|*gcli.FGclicmdf.field (/RR)*|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|||
|*gcli.FGclicmdf.name (.RR)*|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|gcli.FGclicmdf.uval|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|*gcli.FGclicmdf.gclicmdc (.RL)*|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FGclicmdf
<a href="#struct-fgclicmdf"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/gcli_gen.h](/include/gen/gcli_gen.h)
```
struct FGclicmdf { // gcli.FGclicmdf: fields
    gcli::FGclicmdf*    ind_gclicmdf_next;            // hash next
    u32                 ind_gclicmdf_hashval;         // hash value
    algo::Smallstr250   gclicmdf;                     //
    algo::cstring       uval;                         //
    bool                gclicmdc_c_gclicmdf_in_ary;   //   false  membership flag
    // func:gcli.FGclicmdf..AssignOp
    inline gcli::FGclicmdf& operator =(const gcli::FGclicmdf &rhs) = delete;
    // func:gcli.FGclicmdf..CopyCtor
    inline               FGclicmdf(const gcli::FGclicmdf &rhs) = delete;
private:
    // func:gcli.FGclicmdf..Ctor
    inline               FGclicmdf() __attribute__((nothrow));
    // func:gcli.FGclicmdf..Dtor
    inline               ~FGclicmdf() __attribute__((nothrow));
    friend gcli::FGclicmdf&     gclicmdf_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend gcli::FGclicmdf*     gclicmdf_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 gclicmdf_RemoveAll() __attribute__((nothrow));
    friend void                 gclicmdf_RemoveLast() __attribute__((nothrow));
};
```

#### gcli.FGclicmdf2j - mapping field to json key
<a href="#gcli-fgclicmdf2j"></a>

#### gcli.FGclicmdf2j Fields
<a href="#gcli-fgclicmdf2j-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gcli.FGclicmdf2j.base|[gclidb.Gclicmdf2j](/txt/ssimdb/gclidb/gclicmdf2j.md)|[Base](/txt/ssimdb/gclidb/gclicmdf2j.md)|||
|gcli.FGclicmdf2j.p_gclicmd|[gcli.FGclicmd](/txt/exe/gcli/internals.md#gcli-fgclicmd)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FGclicmdf2j
<a href="#struct-fgclicmdf2j"></a>
*Note:* field ``gcli.FGclicmdf2j.base`` has reftype ``base`` so the fields of [gclidb.Gclicmdf2j](/txt/ssimdb/gclidb/gclicmdf2j.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/gcli_gen.h](/include/gen/gcli_gen.h)
```
struct FGclicmdf2j { // gcli.FGclicmdf2j: mapping field to json key
    algo::Smallstr250   gclicmdf2j;                    //
    bool                dup;                           //   false
    algo::Comment       comment;                       //
    gcli::FGclicmd*     p_gclicmd;                     // reference to parent row
    bool                gclicmd_c_gclicmdf2j_in_ary;   //   false  membership flag
    // x-reference on gcli.FGclicmdf2j.p_gclicmd prevents copy
    // func:gcli.FGclicmdf2j..AssignOp
    gcli::FGclicmdf2j&   operator =(const gcli::FGclicmdf2j &rhs) = delete;
    // x-reference on gcli.FGclicmdf2j.p_gclicmd prevents copy
    // func:gcli.FGclicmdf2j..CopyCtor
    FGclicmdf2j(const gcli::FGclicmdf2j &rhs) = delete;
private:
    // func:gcli.FGclicmdf2j..Ctor
    inline               FGclicmdf2j() __attribute__((nothrow));
    // func:gcli.FGclicmdf2j..Dtor
    inline               ~FGclicmdf2j() __attribute__((nothrow));
    friend gcli::FGclicmdf2j&   gclicmdf2j_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend gcli::FGclicmdf2j*   gclicmdf2j_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 gclicmdf2j_RemoveAll() __attribute__((nothrow));
    friend void                 gclicmdf2j_RemoveLast() __attribute__((nothrow));
};
```

#### gcli.FGclicmdj2f - 
<a href="#gcli-fgclicmdj2f"></a>

#### gcli.FGclicmdj2f Fields
<a href="#gcli-fgclicmdj2f-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gcli.FGclicmdj2f.gclicmdj2f|[algo.Smallstr250](/txt/protocol/algo/README.md#algo-smallstr250)|[Val](/txt/exe/amc/reftypes.md#val)|||
|*gcli.FGclicmdj2f.gclicmd (/LL)*|[gclidb.Gclicmd](/txt/ssimdb/gclidb/gclicmd.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|||
|*gcli.FGclicmdj2f.jkey (/LR)*|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|gcli.FGclicmdj2f.select|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|gcli.FGclicmdj2f.dup|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|gcli.FGclicmdj2f.c_gclicmdf|[gcli.FGclicmdf](/txt/exe/gcli/internals.md#gcli-fgclicmdf)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||

#### Struct FGclicmdj2f
<a href="#struct-fgclicmdj2f"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/gcli_gen.h](/include/gen/gcli_gen.h)
```
struct FGclicmdj2f { // gcli.FGclicmdj2f
    gcli::FGclicmdj2f*   ind_gclicmdj2f_next;      // hash next
    u32                  ind_gclicmdj2f_hashval;   // hash value
    algo::Smallstr250    gclicmdj2f;               //
    bool                 select;                   //   false
    bool                 dup;                      //   false
    gcli::FGclicmdf**    c_gclicmdf_elems;         // array of pointers
    u32                  c_gclicmdf_n;             // array of pointers
    u32                  c_gclicmdf_max;           // capacity of allocated array
    // reftype Ptrary of gcli.FGclicmdj2f.c_gclicmdf prohibits copy
    // func:gcli.FGclicmdj2f..AssignOp
    inline gcli::FGclicmdj2f& operator =(const gcli::FGclicmdj2f &rhs) = delete;
    // reftype Ptrary of gcli.FGclicmdj2f.c_gclicmdf prohibits copy
    // func:gcli.FGclicmdj2f..CopyCtor
    inline               FGclicmdj2f(const gcli::FGclicmdj2f &rhs) = delete;
private:
    // func:gcli.FGclicmdj2f..Ctor
    inline               FGclicmdj2f() __attribute__((nothrow));
    // func:gcli.FGclicmdj2f..Dtor
    inline               ~FGclicmdj2f() __attribute__((nothrow));
    friend gcli::FGclicmdj2f&   gclicmdj2f_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend gcli::FGclicmdj2f*   gclicmdj2f_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 gclicmdj2f_RemoveAll() __attribute__((nothrow));
    friend void                 gclicmdj2f_RemoveLast() __attribute__((nothrow));
};
```

#### gcli.FGclicmdt - Internal test
<a href="#gcli-fgclicmdt"></a>

#### gcli.FGclicmdt Fields
<a href="#gcli-fgclicmdt-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gcli.FGclicmdt.base|[gclidb.Gclicmdt](/txt/ssimdb/gclidb/gclicmdt.md)|[Base](/txt/ssimdb/gclidb/gclicmdt.md)|||
|gcli.FGclicmdt.p_gclicmd|[gcli.FGclicmd](/txt/exe/gcli/internals.md#gcli-fgclicmd)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|gcli.FGclicmdt.p_gtype|[gcli.FGtype](/txt/exe/gcli/internals.md#gcli-fgtype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FGclicmdt
<a href="#struct-fgclicmdt"></a>
*Note:* field ``gcli.FGclicmdt.base`` has reftype ``base`` so the fields of [gclidb.Gclicmdt](/txt/ssimdb/gclidb/gclicmdt.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/gcli_gen.h](/include/gen/gcli_gen.h)
```
struct FGclicmdt { // gcli.FGclicmdt
    gcli::FGclicmdt*    ind_gclicmdt_next;      // hash next
    u32                 ind_gclicmdt_hashval;   // hash value
    algo::Smallstr250   gclicmdt;               //
    algo::Smallstr50    gmethod;                //   "GET"
    algo::Comment       uri;                    //
    algo::Comment       comment;                //
    gcli::FGclicmd*     p_gclicmd;              // reference to parent row
    gcli::FGtype*       p_gtype;                // reference to parent row
    // x-reference on gcli.FGclicmdt.p_gclicmd prevents copy
    // x-reference on gcli.FGclicmdt.p_gtype prevents copy
    // func:gcli.FGclicmdt..AssignOp
    inline gcli::FGclicmdt& operator =(const gcli::FGclicmdt &rhs) = delete;
    // x-reference on gcli.FGclicmdt.p_gclicmd prevents copy
    // x-reference on gcli.FGclicmdt.p_gtype prevents copy
    // func:gcli.FGclicmdt..CopyCtor
    inline               FGclicmdt(const gcli::FGclicmdt &rhs) = delete;
private:
    // func:gcli.FGclicmdt..Ctor
    inline               FGclicmdt() __attribute__((nothrow));
    // func:gcli.FGclicmdt..Dtor
    inline               ~FGclicmdt() __attribute__((nothrow));
    friend gcli::FGclicmdt&     gclicmdt_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend gcli::FGclicmdt*     gclicmdt_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 gclicmdt_RemoveAll() __attribute__((nothrow));
    friend void                 gclicmdt_RemoveLast() __attribute__((nothrow));
};
```

#### gcli.FGfld - Gitlab/github field name
<a href="#gcli-fgfld"></a>

#### gcli.FGfld Fields
<a href="#gcli-fgfld-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gcli.FGfld.base|[gclidb.Gfld](/txt/ssimdb/gclidb/gfld.md)|[Base](/txt/ssimdb/gclidb/gfld.md)|||
|gcli.FGfld.c_gtblactfld|[gcli.FGtblactfld](/txt/exe/gcli/internals.md#gcli-fgtblactfld)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||

#### Struct FGfld
<a href="#struct-fgfld"></a>
*Note:* field ``gcli.FGfld.base`` has reftype ``base`` so the fields of [gclidb.Gfld](/txt/ssimdb/gclidb/gfld.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/gcli_gen.h](/include/gen/gcli_gen.h)
```
struct FGfld { // gcli.FGfld
    gcli::FGfld*          ind_gfld_next;        // hash next
    u32                   ind_gfld_hashval;     // hash value
    algo::Smallstr50      gfld;                 //
    algo::Comment         comment;              //
    gcli::FGtblactfld**   c_gtblactfld_elems;   // array of pointers
    u32                   c_gtblactfld_n;       // array of pointers
    u32                   c_gtblactfld_max;     // capacity of allocated array
    bool                  c_gfld_in_ary;        //   false  membership flag
    // reftype Ptrary of gcli.FGfld.c_gtblactfld prohibits copy
    // func:gcli.FGfld..AssignOp
    inline gcli::FGfld&  operator =(const gcli::FGfld &rhs) = delete;
    // reftype Ptrary of gcli.FGfld.c_gtblactfld prohibits copy
    // func:gcli.FGfld..CopyCtor
    inline               FGfld(const gcli::FGfld &rhs) = delete;
private:
    // func:gcli.FGfld..Ctor
    inline               FGfld() __attribute__((nothrow));
    // func:gcli.FGfld..Dtor
    inline               ~FGfld() __attribute__((nothrow));
    friend gcli::FGfld&         gfld_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend gcli::FGfld*         gfld_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 gfld_RemoveAll() __attribute__((nothrow));
    friend void                 gfld_RemoveLast() __attribute__((nothrow));
};
```

#### gcli.FGithost - 
<a href="#gcli-fgithost"></a>

#### gcli.FGithost Fields
<a href="#gcli-fgithost-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gcli.FGithost.githost|[algo.Smallstr250](/txt/protocol/algo/README.md#algo-smallstr250)|[Val](/txt/exe/amc/reftypes.md#val)|||
|gcli.FGithost.ip|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|gcli.FGithost.id|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|gcli.FGithost.id_file|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|gcli.FGithost.port|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|gcli.FGithost.user|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|gcli.FGithost.ssh_config|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|gcli.FGithost.ssh_cmd|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FGithost
<a href="#struct-fgithost"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/gcli_gen.h](/include/gen/gcli_gen.h)
```
struct FGithost { // gcli.FGithost
    gcli::FGithost*     ind_githost_next;      // hash next
    u32                 ind_githost_hashval;   // hash value
    algo::Smallstr250   githost;               //
    algo::cstring       ip;                    //
    algo::cstring       id;                    //
    algo::cstring       id_file;               //
    algo::cstring       port;                  //
    algo::cstring       user;                  //
    algo::cstring       ssh_config;            //
    algo::cstring       ssh_cmd;               //
    // func:gcli.FGithost..AssignOp
    inline gcli::FGithost& operator =(const gcli::FGithost &rhs) = delete;
    // func:gcli.FGithost..CopyCtor
    inline               FGithost(const gcli::FGithost &rhs) = delete;
private:
    // func:gcli.FGithost..Ctor
    inline               FGithost() __attribute__((nothrow));
    // func:gcli.FGithost..Dtor
    inline               ~FGithost() __attribute__((nothrow));
    friend gcli::FGithost&      githost_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend gcli::FGithost*      githost_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 githost_RemoveAll() __attribute__((nothrow));
    friend void                 githost_RemoveLast() __attribute__((nothrow));
};
```

#### gcli.FGmethod - HTTP method list
<a href="#gcli-fgmethod"></a>

#### gcli.FGmethod Fields
<a href="#gcli-fgmethod-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gcli.FGmethod.base|[gclidb.Gmethod](/txt/ssimdb/gclidb/gmethod.md)|[Base](/txt/ssimdb/gclidb/gmethod.md)|||

#### Struct FGmethod
<a href="#struct-fgmethod"></a>
*Note:* field ``gcli.FGmethod.base`` has reftype ``base`` so the fields of [gclidb.Gmethod](/txt/ssimdb/gclidb/gmethod.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/gcli_gen.h](/include/gen/gcli_gen.h)
```
struct FGmethod { // gcli.FGmethod
    gcli::FGmethod*    ind_gmethod_next;      // hash next
    u32                ind_gmethod_hashval;   // hash value
    algo::Smallstr50   gmethod;               //
    u32                val;                   //   0
    algo::Comment      comment;               //
    // func:gcli.FGmethod..AssignOp
    inline gcli::FGmethod& operator =(const gcli::FGmethod &rhs) = delete;
    // func:gcli.FGmethod..CopyCtor
    inline               FGmethod(const gcli::FGmethod &rhs) = delete;
private:
    // func:gcli.FGmethod..Ctor
    inline               FGmethod() __attribute__((nothrow));
    // func:gcli.FGmethod..Dtor
    inline               ~FGmethod() __attribute__((nothrow));
    friend gcli::FGmethod&      gmethod_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend gcli::FGmethod*      gmethod_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 gmethod_RemoveAll() __attribute__((nothrow));
    friend void                 gmethod_RemoveLast() __attribute__((nothrow));
};
```

#### gcli.FGrepo - Repo table (acr grepo -in ~/.ssim)
<a href="#gcli-fgrepo"></a>

#### gcli.FGrepo Fields
<a href="#gcli-fgrepo-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gcli.FGrepo.base|[gclidb.Grepo](/txt/ssimdb/gclidb/grepo.md)|[Base](/txt/ssimdb/gclidb/grepo.md)|||
|gcli.FGrepo.select|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|gcli.FGrepo.gitremote_cmd|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FGrepo
<a href="#struct-fgrepo"></a>
*Note:* field ``gcli.FGrepo.base`` has reftype ``base`` so the fields of [gclidb.Grepo](/txt/ssimdb/gclidb/grepo.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/gcli_gen.h](/include/gen/gcli_gen.h)
```
struct FGrepo { // gcli.FGrepo
    gcli::FGrepo*       ind_grepo_next;      // hash next
    u32                 ind_grepo_hashval;   // hash value
    algo::Smallstr250   grepo;               // gcli host/full name
    algo::cstring       name;                // gcli full name
    algo::cstring       token;               // gitlab project or repo access token
    algo::cstring       default_branch;      //   "origin"  gcli remote used in the workflow
    algo::cstring       keyid;               // gcli project id or repo
    algo::cstring       ssh_repo;            //
    algo::cstring       web_url;             //
    algo::cstring       descr;               //
    bool                active;              //   true  set to false to ignore the line but keep it in the file
    algo::cstring       priv;                //
    bool                select;              //   false
    algo::cstring       gitremote_cmd;       //
    // func:gcli.FGrepo..AssignOp
    gcli::FGrepo&        operator =(const gcli::FGrepo &rhs) = delete;
    // func:gcli.FGrepo..CopyCtor
    FGrepo(const gcli::FGrepo &rhs) = delete;
private:
    // func:gcli.FGrepo..Ctor
    inline               FGrepo() __attribute__((nothrow));
    // func:gcli.FGrepo..Dtor
    inline               ~FGrepo() __attribute__((nothrow));
    friend gcli::FGrepo&        grepo_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend gcli::FGrepo*        grepo_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 grepo_RemoveAll() __attribute__((nothrow));
    friend void                 grepo_RemoveLast() __attribute__((nothrow));
};
```

#### gcli.FGstatet - Internal
<a href="#gcli-fgstatet"></a>

#### gcli.FGstatet Fields
<a href="#gcli-fgstatet-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gcli.FGstatet.base|[gclidb.Gstatet](/txt/ssimdb/gclidb/gstatet.md)|[Base](/txt/ssimdb/gclidb/gstatet.md)|||

#### Struct FGstatet
<a href="#struct-fgstatet"></a>
*Note:* field ``gcli.FGstatet.base`` has reftype ``base`` so the fields of [gclidb.Gstatet](/txt/ssimdb/gclidb/gstatet.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/gcli_gen.h](/include/gen/gcli_gen.h)
```
struct FGstatet { // gcli.FGstatet
    gcli::FGstatet*    ind_gstatet_next;      // hash next
    u32                ind_gstatet_hashval;   // hash value
    algo::Smallstr50   gstatet;               //
    algo::cstring      state;                 //
    // func:gcli.FGstatet..AssignOp
    inline gcli::FGstatet& operator =(const gcli::FGstatet &rhs) = delete;
    // func:gcli.FGstatet..CopyCtor
    inline               FGstatet(const gcli::FGstatet &rhs) = delete;
private:
    // func:gcli.FGstatet..Ctor
    inline               FGstatet() __attribute__((nothrow));
    // func:gcli.FGstatet..Dtor
    inline               ~FGstatet() __attribute__((nothrow));
    friend gcli::FGstatet&      gstatet_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend gcli::FGstatet*      gstatet_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 gstatet_RemoveAll() __attribute__((nothrow));
    friend void                 gstatet_RemoveLast() __attribute__((nothrow));
};
```

#### gcli.FGtbl - Supported gcli tables
<a href="#gcli-fgtbl"></a>

#### gcli.FGtbl Fields
<a href="#gcli-fgtbl-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gcli.FGtbl.base|[gclidb.Gtbl](/txt/ssimdb/gclidb/gtbl.md)|[Base](/txt/ssimdb/gclidb/gtbl.md)|||
|gcli.FGtbl.id|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|gcli.FGtbl.set_as_dflt|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|gcli.FGtbl.tail_dot|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|gcli.FGtbl.c_gtblact|[gcli.FGtblact](/txt/exe/gcli/internals.md#gcli-fgtblact)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||

#### Struct FGtbl
<a href="#struct-fgtbl"></a>
*Note:* field ``gcli.FGtbl.base`` has reftype ``base`` so the fields of [gclidb.Gtbl](/txt/ssimdb/gclidb/gtbl.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/gcli_gen.h](/include/gen/gcli_gen.h)
```
struct FGtbl { // gcli.FGtbl
    gcli::FGtbl*        ind_gtbl_next;      // hash next
    u32                 ind_gtbl_hashval;   // hash value
    algo::Smallstr250   gtbl;               //
    algo::Comment       comment;            //
    algo::cstring       id;                 //
    bool                set_as_dflt;        //   false
    bool                tail_dot;           //   false
    gcli::FGtblact**    c_gtblact_elems;    // array of pointers
    u32                 c_gtblact_n;        // array of pointers
    u32                 c_gtblact_max;      // capacity of allocated array
    // reftype Ptrary of gcli.FGtbl.c_gtblact prohibits copy
    // func:gcli.FGtbl..AssignOp
    inline gcli::FGtbl&  operator =(const gcli::FGtbl &rhs) = delete;
    // reftype Ptrary of gcli.FGtbl.c_gtblact prohibits copy
    // func:gcli.FGtbl..CopyCtor
    inline               FGtbl(const gcli::FGtbl &rhs) = delete;
private:
    // func:gcli.FGtbl..Ctor
    inline               FGtbl() __attribute__((nothrow));
    // func:gcli.FGtbl..Dtor
    inline               ~FGtbl() __attribute__((nothrow));
    friend gcli::FGtbl&         gtbl_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend gcli::FGtbl*         gtbl_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 gtbl_RemoveAll() __attribute__((nothrow));
    friend void                 gtbl_RemoveLast() __attribute__((nothrow));
};
```

#### gcli.FGtblact - static combination of gcli tables and actions
<a href="#gcli-fgtblact"></a>

#### gcli.FGtblact Fields
<a href="#gcli-fgtblact-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gcli.FGtblact.base|[gclidb.Gtblact](/txt/ssimdb/gclidb/gtblact.md)|[Base](/txt/ssimdb/gclidb/gtblact.md)|||
|gcli.FGtblact.id|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|gcli.FGtblact.step|[gcli.FGtblact](/txt/exe/gcli/internals.md#gcli-fgtblact)|[Hook](/txt/exe/amc/reftypes.md#hook)|||
|gcli.FGtblact.c_gtblactfld|[gcli.FGtblactfld](/txt/exe/gcli/internals.md#gcli-fgtblactfld)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|gcli.FGtblact.select|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|gcli.FGtblact.aliascmd|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FGtblact
<a href="#struct-fgtblact"></a>
*Note:* field ``gcli.FGtblact.base`` has reftype ``base`` so the fields of [gclidb.Gtblact](/txt/ssimdb/gclidb/gtblact.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/gcli_gen.h](/include/gen/gcli_gen.h)
```
struct FGtblact { // gcli.FGtblact
    gcli::FGtblact*           ind_gtblact_next;        // hash next
    u32                       ind_gtblact_hashval;     // hash value
    gcli::FGtblact*           zd_gtblact_next;         // zslist link; -1 means not-in-list
    gcli::FGtblact*           zd_gtblact_prev;         // previous element
    algo::Smallstr50          gtblact;                 //
    bool                      t;                       //   false
    bool                      e;                       //   false
    bool                      auth;                    //   false
    algo::cstring             id;                      //
    gcli::gtblact_step_hook   step;                    //   NULL  Pointer to a function
    gcli::FGtblactfld**       c_gtblactfld_elems;      // array of pointers
    u32                       c_gtblactfld_n;          // array of pointers
    u32                       c_gtblactfld_max;        // capacity of allocated array
    bool                      select;                  //   false
    algo::cstring             aliascmd;                //
    bool                      gtbl_c_gtblact_in_ary;   //   false  membership flag
    // reftype Hook of gcli.FGtblact.step prohibits copy
    // reftype Ptrary of gcli.FGtblact.c_gtblactfld prohibits copy
    // func:gcli.FGtblact..AssignOp
    gcli::FGtblact&      operator =(const gcli::FGtblact &rhs) = delete;
    // reftype Hook of gcli.FGtblact.step prohibits copy
    // reftype Ptrary of gcli.FGtblact.c_gtblactfld prohibits copy
    // func:gcli.FGtblact..CopyCtor
    FGtblact(const gcli::FGtblact &rhs) = delete;
private:
    // func:gcli.FGtblact..Ctor
    inline               FGtblact() __attribute__((nothrow));
    // func:gcli.FGtblact..Dtor
    inline               ~FGtblact() __attribute__((nothrow));
    friend gcli::FGtblact&      gtblact_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend gcli::FGtblact*      gtblact_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 gtblact_RemoveAll() __attribute__((nothrow));
    friend void                 gtblact_RemoveLast() __attribute__((nothrow));
};
```

#### gcli.FGtblactfld - List of available for each table & action combination
<a href="#gcli-fgtblactfld"></a>

#### gcli.FGtblactfld Fields
<a href="#gcli-fgtblactfld-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gcli.FGtblactfld.base|[gclidb.Gtblactfld](/txt/ssimdb/gclidb/gtblactfld.md)|[Base](/txt/ssimdb/gclidb/gtblactfld.md)|||
|gcli.FGtblactfld.uval|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|gcli.FGtblactfld.uval2|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|gcli.FGtblactfld.select|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|gcli.FGtblactfld.present|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|gcli.FGtblactfld.update|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|gcli.FGtblactfld.p_gfld|[gcli.FGfld](/txt/exe/gcli/internals.md#gcli-fgfld)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FGtblactfld
<a href="#struct-fgtblactfld"></a>
*Note:* field ``gcli.FGtblactfld.base`` has reftype ``base`` so the fields of [gclidb.Gtblactfld](/txt/ssimdb/gclidb/gtblactfld.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/gcli_gen.h](/include/gen/gcli_gen.h)
```
struct FGtblactfld { // gcli.FGtblactfld
    gcli::FGtblactfld*   ind_gtblactfld_next;           // hash next
    u32                  ind_gtblactfld_hashval;        // hash value
    algo::Smallstr50     gtblactfld;                    //
    algo::cstring        dflt;                          //
    bool                 field_name_dflt;               //   false
    bool                 optional;                      //   false
    bool                 regx;                          //   false
    algo::Comment        comment;                       //
    algo::cstring        uval;                          //
    algo::cstring        uval2;                         //
    bool                 select;                        //   false
    bool                 present;                       //   false
    bool                 update;                        //   false
    gcli::FGfld*         p_gfld;                        // reference to parent row
    bool                 gfld_c_gtblactfld_in_ary;      //   false  membership flag
    bool                 gtblact_c_gtblactfld_in_ary;   //   false  membership flag
    // x-reference on gcli.FGtblactfld.p_gfld prevents copy
    // func:gcli.FGtblactfld..AssignOp
    gcli::FGtblactfld&   operator =(const gcli::FGtblactfld &rhs) = delete;
    // x-reference on gcli.FGtblactfld.p_gfld prevents copy
    // func:gcli.FGtblactfld..CopyCtor
    FGtblactfld(const gcli::FGtblactfld &rhs) = delete;
private:
    // func:gcli.FGtblactfld..Ctor
    inline               FGtblactfld() __attribute__((nothrow));
    // func:gcli.FGtblactfld..Dtor
    inline               ~FGtblactfld() __attribute__((nothrow));
    friend gcli::FGtblactfld&   gtblactfld_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend gcli::FGtblactfld*   gtblactfld_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 gtblactfld_RemoveAll() __attribute__((nothrow));
    friend void                 gtblactfld_RemoveLast() __attribute__((nothrow));
};
```

#### gcli.FGtype - Platform type (ghp=github; glpat=gitlab)
<a href="#gcli-fgtype"></a>

#### gcli.FGtype Fields
<a href="#gcli-fgtype-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gcli.FGtype.base|[gclidb.Gtype](/txt/ssimdb/gclidb/gtype.md)|[Base](/txt/ssimdb/gclidb/gtype.md)|||
|gcli.FGtype.c_gtypeh|[gcli.FGtypeh](/txt/exe/gcli/internals.md#gcli-fgtypeh)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|gcli.FGtype.c_gtypeprefix|[gcli.FGtypeprefix](/txt/exe/gcli/internals.md#gcli-fgtypeprefix)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||

#### Struct FGtype
<a href="#struct-fgtype"></a>
*Note:* field ``gcli.FGtype.base`` has reftype ``base`` so the fields of [gclidb.Gtype](/txt/ssimdb/gclidb/gtype.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/gcli_gen.h](/include/gen/gcli_gen.h)
```
struct FGtype { // gcli.FGtype
    gcli::FGtype*          ind_gtype_next;        // hash next
    u32                    ind_gtype_hashval;     // hash value
    algo::Smallstr50       gtype;                 //
    algo::cstring          default_url;           //
    algo::Comment          comment;               //
    gcli::FGtypeh**        c_gtypeh_elems;        // array of pointers
    u32                    c_gtypeh_n;            // array of pointers
    u32                    c_gtypeh_max;          // capacity of allocated array
    gcli::FGtypeprefix**   c_gtypeprefix_elems;   // array of pointers
    u32                    c_gtypeprefix_n;       // array of pointers
    u32                    c_gtypeprefix_max;     // capacity of allocated array
    // reftype Ptrary of gcli.FGtype.c_gtypeh prohibits copy
    // reftype Ptrary of gcli.FGtype.c_gtypeprefix prohibits copy
    // func:gcli.FGtype..AssignOp
    inline gcli::FGtype& operator =(const gcli::FGtype &rhs) = delete;
    // reftype Ptrary of gcli.FGtype.c_gtypeh prohibits copy
    // reftype Ptrary of gcli.FGtype.c_gtypeprefix prohibits copy
    // func:gcli.FGtype..CopyCtor
    inline               FGtype(const gcli::FGtype &rhs) = delete;
private:
    // func:gcli.FGtype..Ctor
    inline               FGtype() __attribute__((nothrow));
    // func:gcli.FGtype..Dtor
    inline               ~FGtype() __attribute__((nothrow));
    friend gcli::FGtype&        gtype_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend gcli::FGtype*        gtype_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 gtype_RemoveAll() __attribute__((nothrow));
    friend void                 gtype_RemoveLast() __attribute__((nothrow));
};
```

#### gcli.FGtypeh - HTTP heaaderes for user with platform
<a href="#gcli-fgtypeh"></a>

#### gcli.FGtypeh Fields
<a href="#gcli-fgtypeh-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gcli.FGtypeh.base|[gclidb.Gtypeh](/txt/ssimdb/gclidb/gtypeh.md)|[Base](/txt/ssimdb/gclidb/gtypeh.md)|||

#### Struct FGtypeh
<a href="#struct-fgtypeh"></a>
*Note:* field ``gcli.FGtypeh.base`` has reftype ``base`` so the fields of [gclidb.Gtypeh](/txt/ssimdb/gclidb/gtypeh.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/gcli_gen.h](/include/gen/gcli_gen.h)
```
struct FGtypeh { // gcli.FGtypeh
    algo::Smallstr250   gtypeh;                  //
    algo::cstring       header;                  //
    bool                gtype_c_gtypeh_in_ary;   //   false  membership flag
    // func:gcli.FGtypeh..AssignOp
    inline gcli::FGtypeh& operator =(const gcli::FGtypeh &rhs) = delete;
    // func:gcli.FGtypeh..CopyCtor
    inline               FGtypeh(const gcli::FGtypeh &rhs) = delete;
private:
    // func:gcli.FGtypeh..Ctor
    inline               FGtypeh() __attribute__((nothrow));
    // func:gcli.FGtypeh..Dtor
    inline               ~FGtypeh() __attribute__((nothrow));
    friend gcli::FGtypeh&       gtypeh_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend gcli::FGtypeh*       gtypeh_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 gtypeh_RemoveAll() __attribute__((nothrow));
    friend void                 gtypeh_RemoveLast() __attribute__((nothrow));
};
```

#### gcli.FGtypeprefix - Token prefix mapping to platform
<a href="#gcli-fgtypeprefix"></a>

#### gcli.FGtypeprefix Fields
<a href="#gcli-fgtypeprefix-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gcli.FGtypeprefix.base|[gclidb.Gtypeprefix](/txt/ssimdb/gclidb/gtypeprefix.md)|[Base](/txt/ssimdb/gclidb/gtypeprefix.md)|||
|gcli.FGtypeprefix.p_gtype|[gcli.FGtype](/txt/exe/gcli/internals.md#gcli-fgtype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FGtypeprefix
<a href="#struct-fgtypeprefix"></a>
*Note:* field ``gcli.FGtypeprefix.base`` has reftype ``base`` so the fields of [gclidb.Gtypeprefix](/txt/ssimdb/gclidb/gtypeprefix.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/gcli_gen.h](/include/gen/gcli_gen.h)
```
struct FGtypeprefix { // gcli.FGtypeprefix
    algo::Smallstr50   gtypeprefix;                  //
    algo::Comment      comment;                      //
    gcli::FGtype*      p_gtype;                      // reference to parent row
    bool               gtype_c_gtypeprefix_in_ary;   //   false  membership flag
    // x-reference on gcli.FGtypeprefix.p_gtype prevents copy
    // func:gcli.FGtypeprefix..AssignOp
    inline gcli::FGtypeprefix& operator =(const gcli::FGtypeprefix &rhs) = delete;
    // x-reference on gcli.FGtypeprefix.p_gtype prevents copy
    // func:gcli.FGtypeprefix..CopyCtor
    inline               FGtypeprefix(const gcli::FGtypeprefix &rhs) = delete;
private:
    // func:gcli.FGtypeprefix..Ctor
    inline               FGtypeprefix() __attribute__((nothrow));
    // func:gcli.FGtypeprefix..Dtor
    inline               ~FGtypeprefix() __attribute__((nothrow));
    friend gcli::FGtypeprefix&  gtypeprefix_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend gcli::FGtypeprefix*  gtypeprefix_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 gtypeprefix_RemoveAll() __attribute__((nothrow));
    friend void                 gtypeprefix_RemoveLast() __attribute__((nothrow));
};
```

#### gcli.FHttp - HTTP request
<a href="#gcli-fhttp"></a>

#### gcli.FHttp Fields
<a href="#gcli-fhttp-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gcli.FHttp.request_uri|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||URI of requested resource|
|gcli.FHttp.request_content_type|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Request content type|
|gcli.FHttp.request_body|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Request body|
|gcli.FHttp.response_status_line|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Status line|
|gcli.FHttp.response_status_code|u32|[Val](/txt/exe/amc/reftypes.md#val)||Numeric status code|
|gcli.FHttp.request_method|u32|[Val](/txt/exe/amc/reftypes.md#val)|gcli_FHttp_request_method_GET|Request method|
|gcli.FHttp.response_reason_phrase|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Status message|
|gcli.FHttp.response_header|[algo.cstring](/txt/protocol/algo/cstring.md)|[Tary](/txt/exe/amc/reftypes.md#tary)||Array of request headers|
|gcli.FHttp.response_content_type|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Response content type|
|gcli.FHttp.response_body|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Response body|
|gcli.FHttp.response_content_type_json|bool|[Val](/txt/exe/amc/reftypes.md#val)||Flag if response is JSON|
|gcli.FHttp.response_json_parser|[lib_json.FParser](/txt/lib/lib_json/README.md#lib_json-fparser)|[Val](/txt/exe/amc/reftypes.md#val)||Json parser to parse response body on the fly|
|gcli.FHttp.request_body_sent_bytes|u32|[Val](/txt/exe/amc/reftypes.md#val)||Sent bytes in request body|

#### Struct FHttp
<a href="#struct-fhttp"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/gcli_gen.h](/include/gen/gcli_gen.h)
```
struct FHttp { // gcli.FHttp: HTTP request
    algo::cstring       request_uri;                  // URI of requested resource
    algo::cstring       request_content_type;         // Request content type
    algo::cstring       request_body;                 // Request body
    algo::cstring       response_status_line;         // Status line
    u32                 response_status_code;         //   0  Numeric status code
    u32                 request_method;               //   gcli_FHttp_request_method_GET  Request method
    algo::cstring       response_reason_phrase;       // Status message
    algo::cstring*      response_header_elems;        // pointer to elements
    u32                 response_header_n;            // number of elements in array
    u32                 response_header_max;          // max. capacity of array before realloc
    algo::cstring       response_content_type;        // Response content type
    algo::cstring       response_body;                // Response body
    bool                response_content_type_json;   //   false  Flag if response is JSON
    lib_json::FParser   response_json_parser;         // Json parser to parse response body on the fly
    u32                 request_body_sent_bytes;      //   0  Sent bytes in request body
    // value field gcli.FHttp.response_json_parser is not copiable
    // func:gcli.FHttp..AssignOp
    gcli::FHttp&         operator =(const gcli::FHttp &rhs) = delete;
    // func:gcli.FHttp..Ctor
    inline               FHttp() __attribute__((nothrow));
    // func:gcli.FHttp..Dtor
    inline               ~FHttp() __attribute__((nothrow));
    // value field gcli.FHttp.response_json_parser is not copiable
    // func:gcli.FHttp..CopyCtor
    FHttp(const gcli::FHttp &rhs) = delete;
};
```

#### gcli.FIssue - Gitlab project issue
<a href="#gcli-fissue"></a>

#### gcli.FIssue Fields
<a href="#gcli-fissue-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gcli.FIssue.base|[gclidb.Issue](/txt/ssimdb/gclidb/issue.md)|[Base](/txt/ssimdb/gclidb/issue.md)|||
|gcli.FIssue.select|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|gcli.FIssue.p_mr_open|[gcli.FMr](/txt/exe/gcli/internals.md#gcli-fmr)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|gcli.FIssue.c_mrjob|[gcli.FMrjob](/txt/exe/gcli/internals.md#gcli-fmrjob)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|gcli.FIssue.c_issuenote|[gcli.FIssuenote](/txt/exe/gcli/internals.md#gcli-fissuenote)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||

#### Struct FIssue
<a href="#struct-fissue"></a>
*Note:* field ``gcli.FIssue.base`` has reftype ``base`` so the fields of [gclidb.Issue](/txt/ssimdb/gclidb/issue.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/gcli_gen.h](/include/gen/gcli_gen.h)
```
struct FIssue { // gcli.FIssue
    gcli::FIssue*        ind_issue_next;      // hash next
    u32                  ind_issue_hashval;   // hash value
    algo::Smallstr50     issue;               // Identifier. must be in form project.iid
    algo::Smallstr50     assignee;            // User the issue is assigned to
    algo::cstring        labels;              // issue labels - comma-separated list
    algo::cstring        milestone_iid;       // issue milestone, blank if none
    algo::cstring        milestone_num;       // issue milestone, blank if none
    algo::cstring        milestone;           // issue milestone name, blank if none
    algo::cstring        title;               // issue title
    algo::cstring        author;              // issue author
    algo::cstring        state;               // issue state
    algo::cstring        mr;                  // mr issue
    algo::cstring        description;         // issue description
    bool                 select;              //   false
    gcli::FMr*           p_mr_open;           // optional pointer
    gcli::FMrjob**       c_mrjob_elems;       // array of pointers
    u32                  c_mrjob_n;           // array of pointers
    u32                  c_mrjob_max;         // capacity of allocated array
    gcli::FIssuenote**   c_issuenote_elems;   // array of pointers
    u32                  c_issuenote_n;       // array of pointers
    u32                  c_issuenote_max;     // capacity of allocated array
    // reftype Ptrary of gcli.FIssue.c_mrjob prohibits copy
    // reftype Ptrary of gcli.FIssue.c_issuenote prohibits copy
    // func:gcli.FIssue..AssignOp
    gcli::FIssue&        operator =(const gcli::FIssue &rhs) = delete;
    // reftype Ptrary of gcli.FIssue.c_mrjob prohibits copy
    // reftype Ptrary of gcli.FIssue.c_issuenote prohibits copy
    // func:gcli.FIssue..CopyCtor
    FIssue(const gcli::FIssue &rhs) = delete;
private:
    // func:gcli.FIssue..Ctor
    inline               FIssue() __attribute__((nothrow));
    // func:gcli.FIssue..Dtor
    inline               ~FIssue() __attribute__((nothrow));
    friend gcli::FIssue&        issue_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend gcli::FIssue*        issue_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 issue_RemoveAll() __attribute__((nothrow));
    friend void                 issue_RemoveLast() __attribute__((nothrow));
};
```

#### gcli.FIssuenote - Issue note
<a href="#gcli-fissuenote"></a>

#### gcli.FIssuenote Fields
<a href="#gcli-fissuenote-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gcli.FIssuenote.base|[gclidb.Issuenote](/txt/ssimdb/gclidb/issuenote.md)|[Base](/txt/ssimdb/gclidb/issuenote.md)|||
|gcli.FIssuenote.select|bool|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FIssuenote
<a href="#struct-fissuenote"></a>
*Note:* field ``gcli.FIssuenote.base`` has reftype ``base`` so the fields of [gclidb.Issuenote](/txt/ssimdb/gclidb/issuenote.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/gcli_gen.h](/include/gen/gcli_gen.h)
```
struct FIssuenote { // gcli.FIssuenote
    gcli::FIssuenote*   ind_issuenote_next;         // hash next
    u32                 ind_issuenote_hashval;      // hash value
    algo::Smallstr250   issuenote;                  // Identifier. must be in form issue.id
    algo::cstring       note;                       // Body of issue note
    algo::cstring       system;                     // System or user note (glab only)
    algo::cstring       author;                     // Authtor of issue note
    bool                select;                     //   false
    bool                issue_c_issuenote_in_ary;   //   false  membership flag
    // func:gcli.FIssuenote..AssignOp
    inline gcli::FIssuenote& operator =(const gcli::FIssuenote &rhs) = delete;
    // func:gcli.FIssuenote..CopyCtor
    inline               FIssuenote(const gcli::FIssuenote &rhs) = delete;
private:
    // func:gcli.FIssuenote..Ctor
    inline               FIssuenote() __attribute__((nothrow));
    // func:gcli.FIssuenote..Dtor
    inline               ~FIssuenote() __attribute__((nothrow));
    friend gcli::FIssuenote&    issuenote_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend gcli::FIssuenote*    issuenote_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 issuenote_RemoveAll() __attribute__((nothrow));
    friend void                 issuenote_RemoveLast() __attribute__((nothrow));
};
```

#### gcli.FMilestone - Gclidb project milestone
<a href="#gcli-fmilestone"></a>

#### gcli.FMilestone Fields
<a href="#gcli-fmilestone-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gcli.FMilestone.base|[gclidb.Milestone](/txt/ssimdb/gclidb/milestone.md)|[Base](/txt/ssimdb/gclidb/milestone.md)|||
|gcli.FMilestone.select|bool|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FMilestone
<a href="#struct-fmilestone"></a>
*Note:* field ``gcli.FMilestone.base`` has reftype ``base`` so the fields of [gclidb.Milestone](/txt/ssimdb/gclidb/milestone.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/gcli_gen.h](/include/gen/gcli_gen.h)
```
struct FMilestone { // gcli.FMilestone
    gcli::FMilestone*   ind_milestone_next;      // hash next
    u32                 ind_milestone_hashval;   // hash value
    algo::Smallstr200   milestone;               // Milestone id
    algo::cstring       description;             // Milestone description
    algo::cstring       title;                   // Milestone title
    bool                select;                  //   false
    // func:gcli.FMilestone..AssignOp
    inline gcli::FMilestone& operator =(const gcli::FMilestone &rhs) = delete;
    // func:gcli.FMilestone..CopyCtor
    inline               FMilestone(const gcli::FMilestone &rhs) = delete;
private:
    // func:gcli.FMilestone..Ctor
    inline               FMilestone() __attribute__((nothrow));
    // func:gcli.FMilestone..Dtor
    inline               ~FMilestone() __attribute__((nothrow));
    friend gcli::FMilestone&    milestone_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend gcli::FMilestone*    milestone_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 milestone_RemoveAll() __attribute__((nothrow));
    friend void                 milestone_RemoveLast() __attribute__((nothrow));
};
```

#### gcli.FMr - Gitlab project mr
<a href="#gcli-fmr"></a>

#### gcli.FMr Fields
<a href="#gcli-fmr-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gcli.FMr.base|[gclidb.Mr](/txt/ssimdb/gclidb/mr.md)|[Base](/txt/ssimdb/gclidb/mr.md)|||
|gcli.FMr.c_mrnote|[gcli.FMrnote](/txt/exe/gcli/internals.md#gcli-fmrnote)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|gcli.FMr.c_mrjob|[gcli.FMrjob](/txt/exe/gcli/internals.md#gcli-fmrjob)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|gcli.FMr.select|bool|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FMr
<a href="#struct-fmr"></a>
*Note:* field ``gcli.FMr.base`` has reftype ``base`` so the fields of [gclidb.Mr](/txt/ssimdb/gclidb/mr.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/gcli_gen.h](/include/gen/gcli_gen.h)
```
struct FMr { // gcli.FMr
    gcli::FMr*          ind_mr_next;       // hash next
    u32                 ind_mr_hashval;    // hash value
    algo::Smallstr150   mr;                // Identifier. must be in form project.iid
    algo::cstring       state;             // Mr state
    algo::cstring       title;             // Mr title
    algo::cstring       description;       // Mr description
    algo::cstring       source_branch;     // Source branch
    algo::cstring       source_repo;       // Source branch
    algo::cstring       reviewer;          // Reviewer username
    algo::cstring       author;            // Author username
    algo::Smallstr20    pipeline_status;   // Pipeline status
    algo::cstring       pipeline_id;       // Pipeline id
    algo::cstring       draft;             // Some draft status
    algo::cstring       node_id;           // Node id for graphQL
    gcli::FMrnote**     c_mrnote_elems;    // array of pointers
    u32                 c_mrnote_n;        // array of pointers
    u32                 c_mrnote_max;      // capacity of allocated array
    gcli::FMrjob**      c_mrjob_elems;     // array of pointers
    u32                 c_mrjob_n;         // array of pointers
    u32                 c_mrjob_max;       // capacity of allocated array
    bool                select;            //   false
    // reftype Ptrary of gcli.FMr.c_mrnote prohibits copy
    // reftype Ptrary of gcli.FMr.c_mrjob prohibits copy
    // func:gcli.FMr..AssignOp
    gcli::FMr&           operator =(const gcli::FMr &rhs) = delete;
    // reftype Ptrary of gcli.FMr.c_mrnote prohibits copy
    // reftype Ptrary of gcli.FMr.c_mrjob prohibits copy
    // func:gcli.FMr..CopyCtor
    FMr(const gcli::FMr &rhs) = delete;
private:
    // func:gcli.FMr..Ctor
    inline               FMr() __attribute__((nothrow));
    // func:gcli.FMr..Dtor
    inline               ~FMr() __attribute__((nothrow));
    friend gcli::FMr&           mr_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend gcli::FMr*           mr_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 mr_RemoveAll() __attribute__((nothrow));
    friend void                 mr_RemoveLast() __attribute__((nothrow));
};
```

#### gcli.FMrjob - Mr job
<a href="#gcli-fmrjob"></a>

#### gcli.FMrjob Fields
<a href="#gcli-fmrjob-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gcli.FMrjob.base|[gclidb.Mrjob](/txt/ssimdb/gclidb/mrjob.md)|[Base](/txt/ssimdb/gclidb/mrjob.md)|||
|gcli.FMrjob.select|bool|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FMrjob
<a href="#struct-fmrjob"></a>
*Note:* field ``gcli.FMrjob.base`` has reftype ``base`` so the fields of [gclidb.Mrjob](/txt/ssimdb/gclidb/mrjob.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/gcli_gen.h](/include/gen/gcli_gen.h)
```
struct FMrjob { // gcli.FMrjob
    gcli::FMrjob*       ind_mrjob_next;         // hash next
    u32                 ind_mrjob_hashval;      // hash value
    algo::Smallstr250   mrjob;                  // Identifier. must be in form mr.id
    algo::Smallstr150   ref;                    // Mr portion of mr job
    algo::cstring       id;                     // Job ID
    algo::cstring       name;                   // Job name
    algo::cstring       pipeline_id;            // Pipeline id
    algo::cstring       status;                 //
    algo::cstring       runner;                 //
    bool                select;                 //   false
    bool                issue_c_mrjob_in_ary;   //   false  membership flag
    bool                mr_c_mrjob_in_ary;      //   false  membership flag
    // func:gcli.FMrjob..AssignOp
    gcli::FMrjob&        operator =(const gcli::FMrjob &rhs) = delete;
    // func:gcli.FMrjob..CopyCtor
    FMrjob(const gcli::FMrjob &rhs) = delete;
private:
    // func:gcli.FMrjob..Ctor
    inline               FMrjob() __attribute__((nothrow));
    // func:gcli.FMrjob..Dtor
    inline               ~FMrjob() __attribute__((nothrow));
    friend gcli::FMrjob&        mrjob_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend gcli::FMrjob*        mrjob_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 mrjob_RemoveAll() __attribute__((nothrow));
    friend void                 mrjob_RemoveLast() __attribute__((nothrow));
};
```

#### gcli.FMrnote - Issue note
<a href="#gcli-fmrnote"></a>

#### gcli.FMrnote Fields
<a href="#gcli-fmrnote-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gcli.FMrnote.base|[gclidb.Mrnote](/txt/ssimdb/gclidb/mrnote.md)|[Base](/txt/ssimdb/gclidb/mrnote.md)|||
|gcli.FMrnote.select|bool|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FMrnote
<a href="#struct-fmrnote"></a>
*Note:* field ``gcli.FMrnote.base`` has reftype ``base`` so the fields of [gclidb.Mrnote](/txt/ssimdb/gclidb/mrnote.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/gcli_gen.h](/include/gen/gcli_gen.h)
```
struct FMrnote { // gcli.FMrnote
    gcli::FMrnote*      ind_mrnote_next;      // hash next
    u32                 ind_mrnote_hashval;   // hash value
    algo::cstring       system;               // System or user note (glab only)
    algo::Smallstr250   mrnote;               // Identifier. must be in form issue.id
    algo::cstring       note;                 // Body of mr note
    algo::cstring       author;               // Authtor of mr note
    bool                select;               //   false
    bool                mr_c_mrnote_in_ary;   //   false  membership flag
    // func:gcli.FMrnote..AssignOp
    inline gcli::FMrnote& operator =(const gcli::FMrnote &rhs) = delete;
    // func:gcli.FMrnote..CopyCtor
    inline               FMrnote(const gcli::FMrnote &rhs) = delete;
private:
    // func:gcli.FMrnote..Ctor
    inline               FMrnote() __attribute__((nothrow));
    // func:gcli.FMrnote..Dtor
    inline               ~FMrnote() __attribute__((nothrow));
    friend gcli::FMrnote&       mrnote_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend gcli::FMrnote*       mrnote_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 mrnote_RemoveAll() __attribute__((nothrow));
    friend void                 mrnote_RemoveLast() __attribute__((nothrow));
};
```

#### gcli.FTuples - 
<a href="#gcli-ftuples"></a>

#### gcli.FTuples Fields
<a href="#gcli-ftuples-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gcli.FTuples.tuples|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FTuples
<a href="#struct-ftuples"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/gcli_gen.h](/include/gen/gcli_gen.h)
```
struct FTuples { // gcli.FTuples
    algo::cstring   tuples;   //
private:
    // func:gcli.FTuples..Ctor
    inline               FTuples() __attribute__((nothrow));
    friend gcli::FTuples&       tuples_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend gcli::FTuples*       tuples_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 tuples_RemoveAll() __attribute__((nothrow));
    friend void                 tuples_RemoveLast() __attribute__((nothrow));
};
```

#### gcli.FUser - 
<a href="#gcli-fuser"></a>

#### gcli.FUser Fields
<a href="#gcli-fuser-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gcli.FUser.base|[gclidb.User](/txt/ssimdb/gclidb/user.md)|[Base](/txt/ssimdb/gclidb/user.md)|||
|gcli.FUser.select|bool|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FUser
<a href="#struct-fuser"></a>
*Note:* field ``gcli.FUser.base`` has reftype ``base`` so the fields of [gclidb.User](/txt/ssimdb/gclidb/user.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/gcli_gen.h](/include/gen/gcli_gen.h)
```
struct FUser { // gcli.FUser
    gcli::FUser*       ind_user_next;      // hash next
    u32                ind_user_hashval;   // hash value
    algo::Smallstr50   user;               //
    u32                id;                 //   0
    algo::cstring      name;               //
    bool               select;             //   false
    // func:gcli.FUser..AssignOp
    inline gcli::FUser&  operator =(const gcli::FUser &rhs) = delete;
    // func:gcli.FUser..CopyCtor
    inline               FUser(const gcli::FUser &rhs) = delete;
private:
    // func:gcli.FUser..Ctor
    inline               FUser() __attribute__((nothrow));
    // func:gcli.FUser..Dtor
    inline               ~FUser() __attribute__((nothrow));
    friend gcli::FUser&         user_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend gcli::FUser*         user_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 user_RemoveAll() __attribute__((nothrow));
    friend void                 user_RemoveLast() __attribute__((nothrow));
};
```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Imdb -->


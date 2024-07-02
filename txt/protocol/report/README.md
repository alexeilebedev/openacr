## report - Command reports
<a href="#report"></a>
Report lines are produced by various tools, usually as the last line of output,
summarizing the work performed. Compare with [command](/txt/protocol/command/README.md) namespace.

### Ctypes
<a href="#ctypes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Ctypes -->
Other ctypes in this namespace which don't have own readme files

#### report.abt - Report line for [abt](/txt/exe/abt/README.md)
<a href="#report-abt"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|n_target|u16|[Val](/txt/exe/amc/reftypes.md#val)||Number of build targets considered|
|time|[algo.UnDiff](/txt/protocol/algo/UnDiff.md)|[Val](/txt/exe/amc/reftypes.md#val)||Total time spent|
|hitrate|[algo.Smallstr20](/txt/protocol/algo/README.md#algo-smallstr20)|[Val](/txt/exe/amc/reftypes.md#val)||Compiler cache hit rate|
|pch_hitrate|[algo.Smallstr20](/txt/protocol/algo/README.md#algo-smallstr20)|[Val](/txt/exe/amc/reftypes.md#val)||Precompiled header cache hit rate|
|n_warn|u32|[Val](/txt/exe/amc/reftypes.md#val)||Number of compiler warnings|
|n_err|u32|[Val](/txt/exe/amc/reftypes.md#val)||Number of compiler errors|
|n_install|u16|[Val](/txt/exe/amc/reftypes.md#val)||Number of executabes linked to bin/|

#### report.acr - Report line for [acr](/txt/exe/acr/README.md)
<a href="#report-acr"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|n_select|u32|[Val](/txt/exe/amc/reftypes.md#val)||Number of records selected|
|n_insert|u32|[Val](/txt/exe/amc/reftypes.md#val)||Number of records inserted|
|n_delete|u32|[Val](/txt/exe/amc/reftypes.md#val)||Number of records deleted|
|n_ignore|u32|[Val](/txt/exe/amc/reftypes.md#val)||Number of input records ignored|
|n_update|u32|[Val](/txt/exe/amc/reftypes.md#val)||Number of records updated|
|n_file_mod|u32|[Val](/txt/exe/amc/reftypes.md#val)||Number of files modified|

#### report.acr_check - 
<a href="#report-acr_check"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|records|u32|[Val](/txt/exe/amc/reftypes.md#val)|0|Number of records checked|
|errors|u32|[Val](/txt/exe/amc/reftypes.md#val)|0|Number of errors reported|

#### report.amc - Report line for [amc](/txt/exe/amc/README.md)
<a href="#report-amc"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|n_cppfile|u32|[Val](/txt/exe/amc/reftypes.md#val)||Number of c++ files genreated|
|n_cppline|u32|[Val](/txt/exe/amc/reftypes.md#val)||Number of c++ lines generated|
|n_ctype|u32|[Val](/txt/exe/amc/reftypes.md#val)||Total number of ctypes|
|n_func|u32|[Val](/txt/exe/amc/reftypes.md#val)||Total number of functions|
|n_xref|u32|[Val](/txt/exe/amc/reftypes.md#val)||Number of x-references built|
|n_filemod|u32|[Val](/txt/exe/amc/reftypes.md#val)||Number of files written|

#### report.atf_comp - Report line for [atf_comp](/txt/exe/atf_comp/README.md)
<a href="#report-atf_comp"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ntest|i32|[Val](/txt/exe/amc/reftypes.md#val)||Total number of tests|
|nselect|i32|[Val](/txt/exe/amc/reftypes.md#val)||Number of tests selected for execution|
|npass|i32|[Val](/txt/exe/amc/reftypes.md#val)||Number of tests succeeded|
|nskip|i32|[Val](/txt/exe/amc/reftypes.md#val)||Number of tests failed|
|nrun|i32|[Val](/txt/exe/amc/reftypes.md#val)||Number of tests run|
|nwrite|i32|[Val](/txt/exe/amc/reftypes.md#val)||Number of files written|
|nerr|i32|[Val](/txt/exe/amc/reftypes.md#val)||Number of errors|
|ninsert|i32|[Val](/txt/exe/amc/reftypes.md#val)||Number of tests inserted|
|success|bool|[Val](/txt/exe/amc/reftypes.md#val)||Success flag|

#### report.atf_unit - Report line for [atf_unit](/txt/exe/atf_unit/README.md)
<a href="#report-atf_unit"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|n_test_total|u32|[Val](/txt/exe/amc/reftypes.md#val)||Total number of tests|
|success|bool|[Val](/txt/exe/amc/reftypes.md#val)||Success flag|
|n_test_run|u64|[Val](/txt/exe/amc/reftypes.md#val)||Number of tests invoked|
|n_err|u64|[Val](/txt/exe/amc/reftypes.md#val)||Number of errors|

#### report.gcache - Report line for [gcache](/txt/exe/gcache/README.md)
<a href="#report-gcache"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|starttime|[algo.UnTime](/txt/protocol/algo/UnTime.md)|[Val](/txt/exe/amc/reftypes.md#val)||Beginning of run|
|elapsed_sec|double|[Val](/txt/exe/amc/reftypes.md#val)||End of run|
|preproc_size|i32|[Val](/txt/exe/amc/reftypes.md#val)||Size of preprocessed filed|
|hit|bool|[Val](/txt/exe/amc/reftypes.md#val)||Cache hit - compilation avoided|
|cached_file|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||location of cached file|
|copy_file_range|bool|[Val](/txt/exe/amc/reftypes.md#val)||kernel-side copy succeeded|
|pch_hit|bool|[Val](/txt/exe/amc/reftypes.md#val)||PCH hit - precompiled header reused|
|pch_file|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||location of PCH file|
|source|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Source file|
|pch_source|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||PCH source file|

#### report.gcache_hitrate - 
<a href="#report-gcache_hitrate"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|hitrate|[algo.Smallstr20](/txt/protocol/algo/README.md#algo-smallstr20)|[Val](/txt/exe/amc/reftypes.md#val)||Compiler hit rate|
|pch_hitrate|[algo.Smallstr20](/txt/protocol/algo/README.md#algo-smallstr20)|[Val](/txt/exe/amc/reftypes.md#val)||Precompiled header hit rate|

#### report.src_func - Report line for [src_func](/txt/exe/src_func/README.md)
<a href="#report-src_func"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|n_func|u32|[Val](/txt/exe/amc/reftypes.md#val)|0|Number of functions|
|n_line|u32|[Val](/txt/exe/amc/reftypes.md#val)|0|Total number of lines|
|n_static|u32|[Val](/txt/exe/amc/reftypes.md#val)|0|Number of static functions|
|n_inline|u32|[Val](/txt/exe/amc/reftypes.md#val)|0|Number of inline functions|
|n_mysteryfunc|u32|[Val](/txt/exe/amc/reftypes.md#val)|0|Number of undocumented functions|
|n_filemod|u32|[Val](/txt/exe/amc/reftypes.md#val)|0|Number of files written|
|n_baddecl|u32|[Val](/txt/exe/amc/reftypes.md#val)|0|Number of unparseable (by src_func) declarations|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Ctypes -->


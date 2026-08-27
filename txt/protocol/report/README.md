## report - Command reports
<a href="#report"></a>
Report lines are produced by various tools, usually as the last line of output,
summarizing the work performed. Compare with [command](/txt/protocol/command/README.md) namespace.

### Table Of Contents
<a href="#table-of-contents"></a>
<!-- abt_md.toc_beg -->
<!-- abt_md.toc_end -->

### Ctypes
<a href="#ctypes"></a>
Other ctypes in this namespace which don't have own readme files

#### report.abt - Report line for [abt](/txt/exe/abt/README.md)
<a href="#report-abt"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|n_target|u16|[Val](/txt/exe/amc/reftype/Val.md)||Number of build targets considered|
|time|[algo.UnDiff](/txt/protocol/algo/UnDiff.md)|[Val](/txt/exe/amc/reftype/Val.md)||Total time spent|
|hitrate|[algo.Smallstr20](/txt/protocol/algo/README.md#algo-smallstr20)|[Val](/txt/exe/amc/reftype/Val.md)||Compiler cache hit rate|
|pch_hitrate|[algo.Smallstr20](/txt/protocol/algo/README.md#algo-smallstr20)|[Val](/txt/exe/amc/reftype/Val.md)||Precompiled header cache hit rate|
|n_warn|u32|[Val](/txt/exe/amc/reftype/Val.md)||Number of compiler warnings|
|n_err|u32|[Val](/txt/exe/amc/reftype/Val.md)||Number of compiler errors|
|n_install|u16|[Val](/txt/exe/amc/reftype/Val.md)||Number of executabes linked to bin/|

#### report.acr - Report line for [acr](/txt/exe/acr/README.md)
<a href="#report-acr"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|n_select|u32|[Val](/txt/exe/amc/reftype/Val.md)||Number of records selected|
|n_insert|u32|[Val](/txt/exe/amc/reftype/Val.md)||Number of records inserted|
|n_delete|u32|[Val](/txt/exe/amc/reftype/Val.md)||Number of records deleted|
|n_ignore|u32|[Val](/txt/exe/amc/reftype/Val.md)||Number of input records ignored|
|n_update|u32|[Val](/txt/exe/amc/reftype/Val.md)||Number of records updated|
|n_file_mod|u32|[Val](/txt/exe/amc/reftype/Val.md)||Number of files modified|
|n_badline|u32|[Val](/txt/exe/amc/reftype/Val.md)||Input lines dropped: parse failure or unknown ctype|

#### report.acr_check
<a href="#report-acr_check"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|records|u32|[Val](/txt/exe/amc/reftype/Val.md)|0|Number of records checked|
|n_err|u32|[Val](/txt/exe/amc/reftype/Val.md)|0|Number of errors reported|

#### report.amc - Report line for [amc](/txt/exe/amc/README.md)
<a href="#report-amc"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|n_cppfile|u32|[Val](/txt/exe/amc/reftype/Val.md)||Number of c++ files genreated|
|n_cppline|u32|[Val](/txt/exe/amc/reftype/Val.md)||Number of c++ lines generated|
|n_ctype|u32|[Val](/txt/exe/amc/reftype/Val.md)||Total number of ctypes|
|n_func|u32|[Val](/txt/exe/amc/reftype/Val.md)||Total number of functions|
|n_xref|u32|[Val](/txt/exe/amc/reftype/Val.md)||Number of x-references built|
|n_filemod|u32|[Val](/txt/exe/amc/reftype/Val.md)||Number of files written|

#### report.ams_sendtest - Verdict of one ams_sendtest run, one row per process
<a href="#report-ams_sendtest"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|proc|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)||Process this row reports on|
|n_msg|u64|[Val](/txt/exe/amc/reftype/Val.md)||Messages the run is to move|
|n_msg_send|u64|[Val](/txt/exe/amc/reftype/Val.md)||Messages sent|
|n_msg_recv|u64|[Val](/txt/exe/amc/reftype/Val.md)||Messages received|
|n_write_wait|u64|[Val](/txt/exe/amc/reftype/Val.md)||Sends the lane or board refused|
|woff|u64|[Val](/txt/exe/amc/reftype/Val.md)||Lane write offset at exit|
|roff|u64|[Val](/txt/exe/amc/reftype/Val.md)||Lane read offset at exit|
|latency_ns|double|[Val](/txt/exe/amc/reftype/Val.md)||Average receive latency, nanoseconds|
|success|bool|[Val](/txt/exe/amc/reftype/Val.md)||Process moved every message it was given|

#### report.atf_cov - Extent of one coverage measurement
<a href="#report-atf_cov"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|n_covtarget|u32|[Val](/txt/exe/amc/reftype/Val.md)||Number of targets measured|
|n_tgtcov|u32|[Val](/txt/exe/amc/reftype/Val.md)||Number of targets carrying a coverage floor|
|exe|u32|[Val](/txt/exe/amc/reftype/Val.md)||Executable lines measured|
|hit|u32|[Val](/txt/exe/amc/reftype/Val.md)||Exercised lines measured|

#### report.atf_unit - Report line for [atf_unit](/txt/exe/atf_unit/README.md)
<a href="#report-atf_unit"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|n_test_total|u32|[Val](/txt/exe/amc/reftype/Val.md)||Total number of tests|
|success|bool|[Val](/txt/exe/amc/reftype/Val.md)||Success flag|
|n_test_run|u64|[Val](/txt/exe/amc/reftype/Val.md)||Number of tests invoked|
|n_err|u64|[Val](/txt/exe/amc/reftype/Val.md)||Number of errors|

#### report.gcache - Report line for [gcache](/txt/exe/gcache/README.md)
<a href="#report-gcache"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|starttime|[algo.UnTime](/txt/protocol/algo/UnTime.md)|[Val](/txt/exe/amc/reftype/Val.md)||Beginning of run|
|elapsed_sec|double|[Val](/txt/exe/amc/reftype/Val.md)||End of run|
|preproc_size|i32|[Val](/txt/exe/amc/reftype/Val.md)||Size of preprocessed filed|
|hit|bool|[Val](/txt/exe/amc/reftype/Val.md)||Cache hit - compilation avoided|
|cached_file|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)||location of cached file|
|copy_file_range|bool|[Val](/txt/exe/amc/reftype/Val.md)||kernel-side copy succeeded|
|pch_hit|bool|[Val](/txt/exe/amc/reftype/Val.md)||PCH hit - precompiled header reused|
|pch_file|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)||location of PCH file|
|source|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)||Source file|
|pch_source|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype/Val.md)||PCH source file|

#### report.gcache_hitrate
<a href="#report-gcache_hitrate"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|hitrate|[algo.Smallstr20](/txt/protocol/algo/README.md#algo-smallstr20)|[Val](/txt/exe/amc/reftype/Val.md)||Compiler hit rate|
|pch_hitrate|[algo.Smallstr20](/txt/protocol/algo/README.md#algo-smallstr20)|[Val](/txt/exe/amc/reftype/Val.md)||Precompiled header hit rate|

#### report.llmtool_model - Cost of llm work charged to one model
<a href="#report-llmtool_model"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|model|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype/Val.md)||Model id as the transcript spells it|
|n_request|u32|[Val](/txt/exe/amc/reftype/Val.md)||Requests charged to this model|
|input|u64|[Val](/txt/exe/amc/reftype/Val.md)||Fresh input tokens|
|output|u64|[Val](/txt/exe/amc/reftype/Val.md)||Output tokens|
|cache_read|u64|[Val](/txt/exe/amc/reftype/Val.md)||Tokens read from cache|
|cache_write|u64|[Val](/txt/exe/amc/reftype/Val.md)||Tokens written to cache, every duration together|
|usd|[algo.I64Dec4](/txt/protocol/algo/README.md#algo-i64dec4)|[Val](/txt/exe/amc/reftype/Val.md)||Dollars charged to this model|
|priced|bool|[Val](/txt/exe/amc/reftype/Val.md)||The price list has a row for this model; when N the dollars read zero|

#### report.spnx_check - What one reference-graph check of a site examined and rejected
<a href="#report-spnx_check"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|n_page|u32|[Val](/txt/exe/amc/reftype/Val.md)|0|Number of pages the site would publish|
|n_orphan|u32|[Val](/txt/exe/amc/reftype/Val.md)|0|Pages the site publishes that no toctree holds|
|n_badfence|u32|[Val](/txt/exe/amc/reftype/Val.md)|0|Fenced blocks naming a language spnxdb.fence does not carry|

#### report.src_func - Report line for [src_func](/txt/exe/src_func/README.md)
<a href="#report-src_func"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|n_func|u32|[Val](/txt/exe/amc/reftype/Val.md)|0|Number of functions|
|n_line|u32|[Val](/txt/exe/amc/reftype/Val.md)|0|Total number of lines|
|n_static|u32|[Val](/txt/exe/amc/reftype/Val.md)|0|Number of static functions|
|n_inline|u32|[Val](/txt/exe/amc/reftype/Val.md)|0|Number of inline functions|
|n_mysteryfunc|u32|[Val](/txt/exe/amc/reftype/Val.md)|0|Number of undocumented functions|
|n_filemod|u32|[Val](/txt/exe/amc/reftype/Val.md)|0|Number of files written|
|n_baddecl|u32|[Val](/txt/exe/amc/reftype/Val.md)|0|Number of unparseable (by src_func) declarations|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

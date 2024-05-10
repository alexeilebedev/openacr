## command - Command line descriptions
<a href="#command"></a>
The command protocol describes command lines of all executables in the system.
Each command line is described by a struct named `command.<namespace>`.
These are attached to executables via [fcmdline](/txt/ssimdb/dmmeta/fcmdline.md)

### Table Of Contents
<a href="#table-of-contents"></a>
* [Ctypes](#ctypes)

### Ctypes
<a href="#ctypes"></a>
Other ctypes in this namespace which don't have own readme files

#### command.abt - Command line for [abt](/txt/exe/abt/README.md)
<a href="#command-abt"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|target|[dev.Target](/txt/ssimdb/dev/target.md)|RegxSql|""|Regx of target name|
|in|algo.cstring|Val|"data"|Root of input ssim dir|
|out_dir|algo.cstring|Val|""|Output directory|
|cfg|[dev.Cfg](/txt/ssimdb/dev/cfg.md)|Pkey|""|Set config|
|compiler|[dev.Compiler](/txt/ssimdb/dev/compiler.md)|Pkey|""|Set compiler.|
|uname|[dev.Uname](/txt/ssimdb/dev/uname.md)|Pkey|""|Set uname (default: guess)|
|arch|[dev.Arch](/txt/ssimdb/dev/arch.md)|Pkey|""|Set architecture (default: guess)|
|ood|bool|Val||List out-of-date source files|
|list|bool|Val||List target files|
|listincl|bool|Val||List includes|
|build|bool|Val||If set, build specified target (all necessary steps)|
|preproc|bool|Val||Preprocess file, produce .i file|
|clean|bool|Val||Delete all output files|
|dry_run|bool|Val||Print actions, do not perform|
|maxjobs|i32|Val|0|Maximum number of child build processes. 0=pick good default|
|printcmd|bool|Val||Print commands. Do not execute|
|force|bool|Val||Assume all files are out-of-date|
|install|bool|Val|false|Update soft-link under bin/|
|coverity|bool|Val|false|Run abt in coverity mode|
|package|algo.cstring|Val|""|Package tag|
|maxerr|u32|Val|100|Max failing commands before rest of pipeline is forced to fail|
|disas|algo.cstring|RegxSql|""|Regex of function to disassemble|
|report|bool|Val|true|Print final report|
|jcdb|algo.cstring|Val|""|Create JSON compilation database in specified file|
|cache|u8|Val||Cache mode|

#### command.abt_md - Command line for [abt_md](/txt/exe/abt_md/README.md)
<a href="#command-abt_md"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|algo.cstring|Val|"data"|Input directory or filename, - for stdin|
|readme|[dev.Readme](/txt/ssimdb/dev/readme.md)|RegxSql|"%"|Regx of readme to process/show (empty=all)|
|ns|[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)|RegxSql|""|(overrides -readme) Process readmes for this namespace|
|section|algo.cstring|RegxSql|"%"|Select specific section to process|
|update|bool|Val|true|(action) Update mode: Re-generate mdfiles|
|check|bool|Val|false|(action) Check mode: Check syntax and links|
|link|bool|Val|false|(with -print) Print links|
|anchor|bool|Val|false|(with -print) Print anchors|
|print|bool|Val|false|(action) Query mode: Print .md section without evaluating|
|dry_run|bool|Val|false|Do not write changes to disk|

#### command.acr - Command line for [acr](/txt/exe/acr/README.md)
<a href="#command-acr"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|query|algo.cstring|Val|""|Regx to match record|
|where|algo.cstring|Tary||Additional key:value pairs to match|
|in|algo.cstring|Val|"data"|Input directory or filename, - for stdin|
|del|bool|Val||Delete found item|
|sel|bool|Val||Read stdin and select records|
|insert|bool|Val||Read stdin and insert tuples|
|replace|bool|Val||Read stdin and replace tuples|
|update|bool|Val||Read stdin and update attributes of existing tuples|
|merge|bool|Val||Combination of -update and -insert|
|unused|bool|Val|false|Only select records which are not referenced.|
|trunc|bool|Val|false|Truncate table on first write|
|check|bool|Val||Run cross-reference check on selection|
|selerr|bool|Val|true|(with -check): Select error records|
|maxshow|i32|Val|100|Limit number of errors per table|
|write|bool|Val||Write data back to disk.|
|rename|algo.cstring|Val|""|Change value of found item|
|nup|i32|Val|0|Number of levels to go up|
|ndown|i32|Val|0|Number of levels to go down|
|l|bool|Val||Go down via pkeys only|
|xref|bool|Val|false|Short for -nup 100 -ndown 100|
|fldfunc|bool|Val|false|Evaluate fldfunc when printing tuple|
|maxgroup|i32|Val|25|Max. items per group|
|pretty|bool|Val|true|Align output in blocks|
|tree|bool|Val|false|Print as tree|
|loose|bool|Val|false|Allow printing a record before its references (used with -e)|
|my|bool|Val|false|Invoke acr_my -e (using acr_my directly is faster)|
|schema|algo.cstring|Val|"data"|Directory for initializing acr meta-data|
|e|bool|Val|false|Open selection in editor, write back when done.|
|t|bool|Val|false|Short for -tree -xref -loose|
|g|bool|Val||Trigger git commands for changes in dev.gitfile table|
|x|bool|Val||Propagate select/rename/delete to ssimreq records|
|rowid|bool|Val|false|Always print acr.rowid attribute|
|cmt|bool|Val|false|Print comments for all columns referenced in output|
|report|bool|Val|true|Show final report|
|print|bool|Val|true|Print selected records|
|cmd|algo.cstring|Val|""|Print script with command execution for each selected row|
|field|algo.cstring|Tary||Fields to select|
|regxof|algo.cstring|Val|""|Single field: output regx of matching field values|
|meta|bool|Val||Select meta-data for selected records|

#### command.acr_compl - Command line for [acr_compl](/txt/exe/acr_compl/README.md)
<a href="#command-acr_compl"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|data|algo.cstring|Val|"data"|Source for completions (dir or file or -)|
|schema|algo.cstring|Val|"data"|Source for schema information|
|line|algo.cstring|Val|""|Simulates COMP_LINE (debug)|
|point|algo.cstring|Val|""|Simulates COMP_POINT (debug). default: whole line|
|type|algo.cstring|Val|"9"|Simulates COMP_TYPE (debug)|
|install|bool|Val||Produce bash commands to install the handler|
|debug_log|algo.cstring|Val|""|Log file for debug information, overrides ACR_COMPL_DEBUG_LOG|

#### command.acr_dm - Command line for [acr_dm](/txt/exe/acr_dm/README.md)
<a href="#command-acr_dm"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|algo.cstring|Val|"data"|Input directory or filename, - for stdin|
|arg|algo.cstring|Tary||Files to merge: older ours theirs...|
|write_ours|bool|Val||Write result to ours file|
|msize|u8|Val|7|Conflict marker size|
|rowid|bool|Val||Output 'ours' rowid for merging into original ssimfiles|

#### command.acr_ed - Command line for [acr_ed](/txt/exe/acr_ed/README.md)
<a href="#command-acr_ed"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|algo.cstring|Val|"data"|Input directory or filename, - for stdin|
|create|bool|Val|false|Create new entity (-finput, -target, -ctype, -field)|
|del|bool|Val|false|Delete mode|
|rename|algo.cstring|Val|""|Rename to something else|
|finput|bool|Val|false|Create in-memory table based on ssimfile|
|foutput|bool|Val||Declare field as an output|
|srcfile|algo.cstring|Val|""|Create/Rename/Delete a source file|
|gstatic|bool|Val||Like -finput, but data is loaded at compile time|
|indexed|bool|Val|false|(with -finput) Add hash index|
|target|[dev.Target](/txt/ssimdb/dev/target.md)|Pkey|""|Create/Rename/Delete target|
|nstype|[dmmeta.Nstype](/txt/ssimdb/dmmeta/nstype.md)|Pkey|"exe"|(with -create -target): exe,lib,etc.|
|ctype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|Pkey|""|Create/Rename/Delete ctype|
|pooltype|[dmmeta.Reftype](/txt/ssimdb/dmmeta/reftype.md)|Pkey|""| Pool reftype (Lary,Lpool etc) for finput/ctype|
|ssimfile|[dmmeta.Ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|Pkey|""|  Ssimfile for new ctype|
|subset|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|Pkey|""|  Primary key is a subset of this ctype|
|subset2|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|Pkey|""|  Primary key is also a subset of this ctype|
|separator|algo.cstring|Val|"."|    Key separator|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|Pkey|""|Create field|
|arg|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|Pkey|""|  Field type (e.g. u32, etc), (with -ctype) add the base field|
|dflt|algo.cstring|Val|""|  Field default value|
|anon|bool|Val|false|  Anonymous field (use with command lines)|
|bigend|bool|Val|false|  Big-endian field|
|cascdel|bool|Val|false|  Field is cascdel|
|before|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|Pkey|""|  Place field before this one|
|substr|[dmmeta.Substr](/txt/ssimdb/dmmeta/substr.md)|Pkey|""|  New field is a substring|
|alias|bool|Val||Create alias field (requires -srcfield)|
|srcfield|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|Pkey|""|  Source field for bitfld/substr|
|fstep|[dmmeta.Fstep](/txt/ssimdb/dmmeta/fstep.md)|Pkey|""|  Add fstep record|
|inscond|algo.cstring|Val|"true"|  Insert condition (for xref)|
|reftype|[dmmeta.Reftype](/txt/ssimdb/dmmeta/reftype.md)|Pkey|""|  Reftype (e.g. Val, Thash, Llist, etc)|
|hashfld|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|Pkey|""|    (-reftype:Thash) Hash field|
|sortfld|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|Pkey|""|    (-reftype:Bheap) Sort field|
|unittest|algo.cstring|Val|""|Create unit test, <ns>.<functionname>|
|citest|algo.cstring|Val|""|Create CI test|
|cppfunc|algo.cstring|Val|""|Field is a cppfunc, pass c++ expression as argument|
|xref|bool|Val|false|    X-ref with field type|
|via|algo.cstring|Val|""|      X-ref argument (index, pointer, or index/key)|
|write|bool|Val|false|Commit output to disk|
|e|bool|Val|false| (with -create -unittest) Edit new testcase|
|comment|algo.cstring|Val|""|Comment for new entity|
|sandbox|bool|Val|false|Make changes in sandbox|
|test|bool|Val|false|Build resulting changes, run tests|
|showcpp|bool|Val||(With -sandbox), show resulting diff|
|msgtype|algo.cstring|Val|""|(with -ctype) use this msgtype as type|
|anonfld|bool|Val||Create anonfld|

#### command.acr_in - Command line for [acr_in](/txt/exe/acr_in/README.md)
<a href="#command-acr_in"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ns|[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)|RegxSql|""|Regx of matching namespace|
|data|bool|Val|false|List ssimfile contents|
|sigcheck|bool|Val|true|Output sigcheck records for schema version mismatch detection|
|list|bool|Val|false|List ssimfile names|
|t|bool|Val||(with -list) Tree mode|
|data_dir|algo.cstring|Val|"data"|Directory with ssimfiles|
|schema|algo.cstring|Val|"data"|
|related|algo.cstring|Val|""|Select only tuples related to specified acr key|
|notssimfile|[dmmeta.Ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|RegxSql|""|Exclude ssimfiles matching regx|
|checkable|bool|Val||Ensure output passes acr -check|
|r|[dmmeta.Ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|RegxSql|""|Reverse lookup of target by ssimfile|

#### command.acr_my - Command line for [acr_my](/txt/exe/acr_my/README.md)
<a href="#command-acr_my"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|nsdb|[dmmeta.Nsdb](/txt/ssimdb/dmmeta/nsdb.md)|RegxSql|""|Regx of ssim namespace (dmmeta.nsdb) to select|
|in|algo.cstring|Val|"data"|Input directory or filename, - for stdin|
|schema|algo.cstring|Val|"data"|Input directory or filename, - for stdin|
|fldfunc|bool|Val|false|Evaluate fldfunc when printing tuple|
|fkey|bool|Val||Enable foreign key constraints|
|e|bool|Val|false|Alias for -start -shell -stop|
|start|bool|Val|false|Start local mysql server|
|stop|bool|Val|false|Stop local mysql server, saving data|
|abort|bool|Val|false|Abort local mysql server, losing data|
|shell|bool|Val|false|Connect to local mysql server|
|serv|bool|Val|false|Start mysql with TCP/IP service enabled|

#### command.amc - Command line for [amc](/txt/exe/amc/README.md)
<a href="#command-amc"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in_dir|algo.cstring|Val|"data"|Root of input ssim dir|
|query|algo.cstring|Val|""|Query mode: generate code for specified object|
|out_dir|algo.cstring|Val|"."|Root of output cpp dir|
|proto|bool|Val|false|Print prototype|
|report|bool|Val|true|Final report|
|e|bool|Val|false|Open matching records in editor|
|trace|algo.cstring|RegxSql|""|Regx of something to trace code generation|

#### command.amc_gc - Command line for [amc_gc](/txt/exe/amc_gc/README.md)
<a href="#command-amc_gc"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|target|[dev.Target](/txt/ssimdb/dev/target.md)|RegxSql|"%"|Target to test-build|
|key|algo.cstring|RegxSql|""|ACR query selecting records to eliminate, e.g. dmmeta.ctype:amc.%|
|include|bool|Val||Garbage collect includes for specified target|
|in|algo.cstring|Val|"data"|Input directory or filename, - for stdin|

#### command.amc_vis - Command line for [amc_vis](/txt/exe/amc_vis/README.md)
<a href="#command-amc_vis"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ctype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|RegxSql|"%"|Ctype regexp to compute access path diagram|
|in|algo.cstring|Val|"data"|Input directory or filename, - for stdin|
|dot|algo.cstring|Val|""|Save dot file with specified filename|
|xref|bool|Val|false|Include all ctypes referenced by selected ones|
|xns|bool|Val|false|Cross namespace boundaries|
|noinput|bool|Val||Deselect module inputs|
|check|bool|Val||Check model for dependency problems|
|render|bool|Val|true|Produce an ascii drawing|

#### command.ams_cat - Command line for [ams_cat](/txt/exe/ams_cat/README.md)
<a href="#command-ams_cat"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|algo.cstring|Val|"data"|Input directory or filename, - for stdin|

#### command.ams_sendtest - Command line for [ams_sendtest](/txt/exe/ams_sendtest/README.md)
<a href="#command-ams_sendtest"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|algo.cstring|Val|"data"|Input directory or filename, - for stdin|
|id|i32|Val||Process index (0=parent)|
|file_prefix|algo.cstring|Val|""|Use file_prefix|
|nchild|i32|Val|1|Number of stream readers|
|blocking|bool|Val|false|Use blocking send mode|
|nmsg|i32|Val|1000|Number of messages to send/receive|
|trace|algo.cstring|RegxSql|""|Regx expression specifying what to trace|
|timeout|i32|Val|30|Time limit for the send|
|recvdelay_ns|i64|Val||Pause nanoseconds between messages|
|senddelay_ns|i64|Val||Pause nanoseconds between messages|
|msgsize_min|i32|Val|64|Minimum message length|
|msgsize_max|i32|Val|1024|Maximum message length|
|bufsize|i32|Val|32768|Shared memory buffer size|
|recvdelay|i64|Val||Pause nanoseconds between messages|

#### command.apm - Command line for [apm](/txt/exe/apm/README.md)
<a href="#command-apm"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|algo.cstring|Val|"data"|Input directory or filename, - for stdin|
|pkgdata|algo.cstring|Val|""|Load package definitions from here|
|package|[dev.Package](/txt/ssimdb/dev/package.md)|RegxSql|""|Regx of package|
|ns|[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)|RegxSql|""|Operate on specified namespace only|
|install|bool|Val||Install new package (specify -origin)|
|update|bool|Val||Update package (-origin)|
|list|bool|Val||List installed packages|
|diff|bool|Val||Diff package with respect to installed version|
|push|bool|Val||Evaluate package diff and push it to origin|
|check|bool|Val||Consistency check|
|remove|bool|Val||Remove specified package|
|origin|algo.Smallstr200|Val|""|Upstream URL of new package|
|ref|algo.Smallstr50|Val|""|(with -create) Gitref or branch to fetch|
|dry_run|bool|Val||Do not execute transaction|
|showrec|bool|Val||Show records belonging to package|
|showfile|bool|Val||List package files (gitfile records)|
|R|bool|Val||reverse the diff direction|
|l|bool|Val||Use local package definition on the remote side|
|reset|bool|Val||Reset package baseref/origin to those provided by the command line|
|checkclean|bool|Val|true|Ensure that changes are applied to a clean directory|
|t|bool|Val||Select parent packages for operation|
|stat|bool|Val||(with -diff) show stats|
|annotate|algo.cstring|Val|""|Read file and annotate each input tuple with package(s) it belongs to|
|data_in|algo.cstring|Val|"data"|Dataset from which package records are loaded|
|e|bool|Val||Open selected records in editor|
|binpath|algo.cstring|Val|"bin"|(internal use)|

#### command.aqlite - Command line for [aqlite](/txt/exe/aqlite/README.md)
<a href="#command-aqlite"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|algo.cstring|Val|"data"|Input directory or filename, - for stdin for schema|
|data|algo.cstring|Val|"data"|Input directory for data|
|cmd|algo.cstring|Val||Sql Query to run|

#### command.atf_amc - Command line for [atf_amc](/txt/exe/atf_amc/README.md)
<a href="#command-atf_amc"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|algo.cstring|Val|"data"|Input directory or filename, - for stdin|
|amctest|[atfdb.Amctest](/txt/ssimdb/atfdb/amctest.md)|RegxSql|"%"|SQL regex, selecting test to run|
|dofork|bool|Val|true|Use fork|
|q|bool|Val||Quiet mode|

#### command.atf_ci - Command line for [atf_ci](/txt/exe/atf_ci/README.md)
<a href="#command-atf_ci"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|algo.cstring|Val|"data"|Input directory or filename, - for stdin|
|citest|[atfdb.Citest](/txt/ssimdb/atfdb/citest.md)|RegxSql|"%"|Regx of tests to run|
|maxerr|i32|Val|0|Exit after this many errors|
|cijob|[atfdb.Cijob](/txt/ssimdb/atfdb/cijob.md)|RegxSql|"%"|
|capture|bool|Val||Capture the output of the test|

#### command.atf_cmdline - Command line for [atf_cmdline](/txt/exe/atf_cmdline/README.md)
<a href="#command-atf_cmdline"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|algo.cstring|Val|"data"|Input directory or filename, - for stdin|
|exec|bool|Val||Execv itself|
|astr|algo.cstring|Val||Required anon string|
|anum|i32|Val||Anon number|
|adbl|double|Val||Anon double|
|aflag|bool|Val||Anon flag|
|str|algo.cstring|Val||Required string|
|num|i32|Val||Required Number|
|dbl|double|Val||Required double|
|flag|bool|Val||Required flag|
|dstr|algo.cstring|Val|"blah"|Predefined string|
|dnum|i32|Val|-33|Predefined number|
|ddbl|double|Val|0.0001|Predefined double|
|dflag|bool|Val|true|Predefined flag|
|mstr|algo.cstring|Tary||String array|
|mnum|i32|Tary||Number array|
|mdbl|double|Tary||Double array|
|amnum|i32|Tary||Anon number array|
|fconst|u8|Val||Fconst for field|
|cconst|algo.Month|Val||Fconst for arg ctype|
|dregx|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|RegxSql|"%"|Predefined regx|
|dpkey|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|Pkey|""|Predefined pkey|

#### command.atf_comp - Command line for [atf_comp](/txt/exe/atf_comp/README.md)
<a href="#command-atf_comp"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|algo.cstring|Val|"data"|Input directory or filename, - for stdin|
|comptest|[atfdb.Comptest](/txt/ssimdb/atfdb/comptest.md)|RegxSql|"%"|Select comptest (SQL regex)|
|mdbg|bool|Val||(action) Run component test under debugger|
|run|bool|Val|true|(action) Run selected component tests|
|capture|bool|Val||(action) Re-capture test results|
|print|bool|Val||(action) Print testcase|
|printinput|bool|Val||(action) Print input of test case|
|e|bool|Val||(action) Open selected testcases in an editor|
|normalize|bool|Val||(action) Renumber and normalize tmsgs|
|covcapture|bool|Val||(action) Capture new coverage percentages and save back|
|covcheck|bool|Val||(action) Check coverage percentages against tgtcov table|
|compdir|algo.cstring|Val|""|Component image directory (exe)|
|cfg|[dev.Cfg](/txt/ssimdb/dev/cfg.md)|Pkey|"release"|Set config|
|check_untracked|bool|Val|true|Check for untracked file before allowing test to run|
|maxerr|i32|Val|1|Exit after this many errors|
|build|bool|Val|false|Build given cfg before test|
|ood|bool|Val|false|Check given cfg for ood before test|
|memcheck|bool|Val||Run under memory checker (valgrind)|
|force|bool|Val||(With -memcheck) run suppressed memcheck|
|callgrind|bool|Val||Run under callgrind profiler (valgrind)|
|maxjobs|i32|Val|0|Maximum number of tests run in parallel|
|stream|bool|Val||prints component's output|
|i|bool|Val||Read and execute testcase from stdin|
|write|bool|Val|true|(implied with -e) Write any changes back to ssim tables|
|report|bool|Val||Print final report|
|b|algo.cstring|Val|""|Breakpoint passed to mdbg as-is|

#### command.atf_cov - Command line for [atf_cov](/txt/exe/atf_cov/README.md)
<a href="#command-atf_cov"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|algo.cstring|Val|"data"|Input directory or filename, - for stdin|
|covdir|algo.cstring|Val|"temp/covdata"|Output directory to save coverage data|
|logfile|algo.cstring|Val|""|Log file|
|runcmd|algo.cstring|Val|""|command to run|
|exclude|[dev.Gitfile](/txt/ssimdb/dev/gitfile.md)|RegxSql|"(extern|include/gen|cpp/gen)/%"|Exclude gitfiles (external, generated)|
|mergepath|algo.cstring|Val|""|colon-separated dir list to load .cov.ssim files from|
|gcov|bool|Val||run gcov|
|ssim|bool|Val||write out ssim files|
|report|bool|Val||write out all reports|
|capture|bool|Val||Write coverage information into tgtcov table|
|xmlpretty|bool|Val||Generate pretty-formatted XML|
|summary|bool|Val|true|Show summary figures|
|check|bool|Val||Check coverage information against tgtcov table|

#### command.atf_fuzz - Command line for [atf_fuzz](/txt/exe/atf_fuzz/README.md)
<a href="#command-atf_fuzz"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|reprofile|algo.cstring|Val|"temp/atf_fuzz.repro"|File where repros are stored|
|target|[dev.Target](/txt/ssimdb/dev/target.md)|Pkey|""|Target to fuzz|
|args|algo.cstring|Val|""|Additional arguments to target|
|inputfile|algo.cstring|Val|""|File with input tuples.|
|fuzzstrat|[atfdb.Fuzzstrat](/txt/ssimdb/atfdb/fuzzstrat.md)|RegxSql|"%"|Strategy to choose|
|in|algo.cstring|Val|"data"|Input directory or filename, - for stdin|
|seed|i32|Val|0|Random seed|
|testprob|double|Val|1|Run each case with this probability|

#### command.atf_gcli - Command line for [atf_gcli](/txt/exe/atf_gcli/README.md)
<a href="#command-atf_gcli"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|algo.cstring|Val|"data"|Input directory or filename, - for stdin|
|gtblacttst|[gclidb.Gtblacttst](/txt/ssimdb/gclidb/gtblacttst.md)|RegxSql|"%"|gcli tests regx|
|id|algo.cstring|Val|""|Use this issue for $ISSUE substitutions|
|mr|algo.cstring|Val|""|Use this issue for $MR substitutions|
|note|algo.cstring|Val|""|Use this issue for $NOTE substitutions|
|capture|bool|Val||Capture test command output into gverbtout|
|skip_init|bool|Val||Skip setting local files - already set|
|skip_git_init|bool|Val||Skip setting local files - already set|
|dry_run|bool|Val||Print actions, do not perform|

#### command.atf_nrun - Command line for [atf_nrun](/txt/exe/atf_nrun/README.md)
<a href="#command-atf_nrun"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|algo.cstring|Val|"data"|Input directory or filename, - for stdin|
|maxjobs|i32|Val|2|Number of simultaneous jobs|
|ncmd|i32|Val|6|

#### command.atf_unit - Command line for [atf_unit](/txt/exe/atf_unit/README.md)
<a href="#command-atf_unit"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|unittest|[atfdb.Unittest](/txt/ssimdb/atfdb/unittest.md)|RegxSql|"%"|SQL regex, selecting test to run|
|nofork|bool|Val||Do not fork for destructive tests|
|arg|algo.cstring|Val|""|Argument to pass to tool|
|data_dir|algo.cstring|Val|"data"|Data directory|
|mdbg|bool|Val|0|Break at testcase in debugger|
|perf_secs|double|Val|1.0|# Of seconds to run perf tests for|
|pertest_timeout|u32|Val|900|Max runtime of any individual unit test|
|report|bool|Val|true|Print final report|
|capture|bool|Val||Re-capture test results|
|check_untracked|bool|Val|true|Check for untracked file before allowing test to run|

#### command.bash - 
<a href="#command-bash"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|c|algo.cstring|Val|""|Shell command to execute|

#### command.bash2html - Command line for [bash2html](/txt/exe/bash2html/README.md)
<a href="#command-bash2html"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|algo.cstring|Val|"data"|Input directory or filename, - for stdin|
|test|bool|Val||Produce Test Output|

#### command.bash_proc - 
<a href="#command-bash_proc"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|bash|command.bash|Exec|"bash"|Must be bash to support $'' for string quoting|

#### command.gcache - Command line for [gcache](/txt/exe/gcache/README.md)
<a href="#command-gcache"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|algo.cstring|Val|"data"|Input directory or filename, - for stdin|
|cmd|algo.cstring|Tary||Command to execute|
|install|bool|Val||Create gcache directory and enable gcache|
|stats|bool|Val||Show cache stats|
|enable|bool|Val||Create .gcache link to enable gcache use|
|disable|bool|Val||Remove .gcache link to disable gcache|
|gc|bool|Val||Clean old files from .gcache|
|clean|bool|Val||Clean the entire cache|
|dir|algo.cstring|Val|"/tmp/gcache"|(With -install,-enable) cache directory|
|hitrate|bool|Val||Report hit rate (specify start time with -after)|
|after|algo.UnTime|Val||Start time for reporting|
|report|bool|Val||Show end-of-run report|
|force|bool|Val||Force recompile and update cache|

#### command.gcli - Command line for [gcli](/txt/exe/gcli/README.md)
<a href="#command-gcli"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|algo.cstring|Val|"data"|Input directory or filename, - for stdin|
|selector|[gclidb.Gtbl](/txt/ssimdb/gclidb/gtbl.md)|Pkey|"issue:%"|table:key, where table is issue,repo,mr, etc. and key is a regex.|
|fields|algo.cstring|Tary|""|additional key:value pairs for use with -create, -list, -update|
|accept|bool|Val|false|(action) accept a slected merge request|
|start|bool|Val|false|(action) start working on a selected issue|
|list|bool|Val|false|(action) show selected table|
|create|bool|Val|false|(action) create a table record(s)|
|update|bool|Val|false|(action) update fields of a selected issue or mergereq|
|approve|bool|Val|false|(action) remove draft desiognation from mergereq|
|needs_work|bool|Val|false|(action) reopen an issue or put a draft designation on mergereq|
|stop|bool|Val|false|(action) closes an issue, or remove mr branch after review|
|t|bool|Val||Tree view: expand issue description|
|e|bool|Val||edit the input|
|authdir|algo.cstring|Val|".ssim"|(setup) Input directory for auth data|
|dry_run|bool|Val||Print actions, do not perform|
|gitdir|algo.cstring|Val|""|(setup) Change directory of git repository|
|show_gitlab_system_notes|bool|Val||(misc) Show issue and mr notes created by gitlab|

#### command.mdbg - Command line for [mdbg](/txt/exe/mdbg/README.md)
<a href="#command-mdbg"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|target|[dev.Target](/txt/ssimdb/dev/target.md)|Pkey||Executable name|
|in|algo.cstring|Val|"data"|Input directory or filename, - for stdin|
|args|algo.cstring|Tary|""|Additional module args|
|cfg|[dev.Cfg](/txt/ssimdb/dev/cfg.md)|Pkey|"debug"|Configuration to use|
|disas|bool|Val||Show disassembly (use F12)|
|attach|bool|Val|false|Attach to a running process|
|b|algo.cstring|Tary|""|Set breakpoint, e.g. 'a.cpp:123 if cond1', 'func#3'|
|catchthrow|bool|Val|true|Stop on exceptions|
|tui|bool|Val||Use gdb -tui as the debugger|
|bcmd|algo.cstring|Val|""|Evaluate command at breakpoint|
|emacs|bool|Val|true|Use emacs environment as the debugger|
|manywin|bool|Val|false|Use gdb-many-windows emacs mode|
|follow_child|bool|Val|
|py|bool|Val||Enable python scripting|
|dry_run|bool|Val||Print commands but don't execute|

#### command.mysql2ssim - Command line for [mysql2ssim](/txt/exe/mysql2ssim/README.md)
<a href="#command-mysql2ssim"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|writessimfile|bool|Val||Write to ssimfile directly|
|url|algo.cstring|Val||user:pass@host/db or sock:///filename/db|
|tables|algo.cstring|Val|""|comma-separated list of tables. Default is all tables|
|schema|bool|Val||Generate ssim type definition|
|in|algo.cstring|Val|"data"|Input directory or filename, - for stdin|
|pretty|bool|Val|false|Format output for the screen|
|nologo|bool|Val||Don't show copyright notice|
|baddbok|bool|Val||Don't claim if bad database|

#### command.orgfile - Command line for [orgfile](/txt/exe/orgfile/README.md)
<a href="#command-orgfile"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|algo.cstring|Val|"data"|Input directory or filename, - for stdin|
|move|algo.cstring|Val|""|Read stdin, rename files based on pattern|
|dedup|algo.cstring|RegxSql|""|Only allow deleting files that match this regx|
|commit|bool|Val||Apply changes|
|undo|bool|Val||Read previous orgfile output, undoing movement|
|hash|algo.cstring|Val|"sha1"|Hash command to use for deduplication|

#### command.samp_regx - Command line for [samp_regx](/txt/exe/samp_regx/README.md)
<a href="#command-samp_regx"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|algo.cstring|Val|"data"|Input directory or filename, - for stdin|
|expr|algo.cstring|Val||Expression|
|style|u8|Val|0|Regx style|
|match|bool|Val||Match a string|
|string|algo.cstring|Val|""|String to match|
|show|bool|Val|false|Show regx innards|

#### command.sandbox - Command line for [sandbox](/txt/exe/sandbox/README.md)
<a href="#command-sandbox"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|algo.cstring|Val|"data"|Input directory or filename, - for stdin|
|name|[dev.Sandbox](/txt/ssimdb/dev/sandbox.md)|RegxSql||Sandbox name|
|create|bool|Val||Create new sandbox and register in dev.sandbox|
|list|bool|Val||List existing sandboxes|
|reset|bool|Val||Reset sandbox to match current directory|
|clean|bool|Val||Remove sandbox contents to save space|
|shell|bool|Val||Open interactive shell inside sandbox|
|del|bool|Val||Permanently delete sandbox|
|gc|bool|Val||Run git gc in target dir|
|cmd|algo.cstring|Tary||Command to execute in sandbox|
|diff|bool|Val||Show diff after running command|
|files|algo.cstring|Tary||Shell regx to diff|
|refs|algo.cstring|Val|"HEAD"|Refs to fetch into sandbox|
|q|bool|Val||Quiet mode|

#### command.src_func - Command line for [src_func](/txt/exe/src_func/README.md)
<a href="#command-src_func"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|algo.cstring|Val|"data"|Input directory or filename, - for stdin|
|target|[dev.Target](/txt/ssimdb/dev/target.md)|RegxSql|"%"|Visit these targets|
|name|algo.cstring|RegxSql|"%"|Match function name|
|body|algo.cstring|RegxSql|"%"|Match function body|
|targsrc|[dev.Targsrc](/txt/ssimdb/dev/targsrc.md)|RegxSql|""|Visit these sources (optional)|
|func|algo.cstring|RegxSql|"%"|(with -listfunc) Match function prototype|
|comment|algo.cstring|RegxSql|"%"|(with -listfunc) Match function comment|
|nextfile|[dev.Gitfile](/txt/ssimdb/dev/gitfile.md)|Pkey|""|Print name of next srcfile in targsrc list|
|other|bool|Val|false|(with -nextfile), name of previous file|
|updateproto|bool|Val|false|Update prototypes in headers|
|listfunc|bool|Val|false|Show functions in target|
|iffy|bool|Val|false|Select functions that may contain errors|
|proto|bool|Val|false|(with -listfunc) List prototypes only|
|gen|bool|Val|false|(with -listfunc) Visit generated files|
|showloc|bool|Val|true|(with -listfunc) Show file location|
|showstatic|bool|Val|true|(with -listfunc) Show static functions|
|showsortkey|bool|Val|false|(with -listfunc) Display function sortkey|
|sortname|bool|Val|false|(with -listfunc) Sort functions by name|
|e|bool|Val|false|(with -listfunc) Edit found functions|
|baddecl|bool|Val||Report and fail on bad declarations|
|report|bool|Val|

#### command.src_hdr - Command line for [src_hdr](/txt/exe/src_hdr/README.md)
<a href="#command-src_hdr"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|algo.cstring|Val|"data"|Input directory or filename, - for stdin|
|targsrc|[dev.Targsrc](/txt/ssimdb/dev/targsrc.md)|RegxSql|""|Regx of targsrc to update|
|write|bool|Val||Update files in-place|
|indent|bool|Val||Indent source files|
|update_copyright|bool|Val||Update copyright year for current company|
|scriptfile|[dev.Scriptfile](/txt/ssimdb/dev/scriptfile.md)|RegxSql|""|Regx of scripts to update header|

#### command.src_lim - Command line for [src_lim](/txt/exe/src_lim/README.md)
<a href="#command-src_lim"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|algo.cstring|Val|"data"|Input directory or filename, - for stdin|
|linelim|bool|Val||Check various file limits (line length, function length, etc)|
|srcfile|[dev.Gitfile](/txt/ssimdb/dev/gitfile.md)|RegxSql|"%"|Filter for source files to process|
|strayfile|bool|Val||Check for unregistered source files|
|capture|bool|Val||Generate new dev.linelim records|
|write|bool|Val||Update ssim database (with -capture)|
|badchar|bool|Val||Check for bad chars in source files|
|badline|[dev.Badline](/txt/ssimdb/dev/badline.md)|RegxSql|""|Check badline (acr badline)|

#### command.ssim2csv - Command line for [ssim2csv](/txt/exe/ssim2csv/README.md)
<a href="#command-ssim2csv"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|expand|algo.cstring|Val|""|
|ignoreQuote|bool|Val|false|

#### command.ssim2mysql - Command line for [ssim2mysql](/txt/exe/ssim2mysql/README.md)
<a href="#command-ssim2mysql"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|url|algo.cstring|Val|""|URL of mysql server. user:pass@hostb or sock://filename; Empty -> stdout|
|data_dir|algo.cstring|Val|"data"|Load dmmeta info from this directory|
|maxpacket|i32|Val|100000|Max Mysql packet size|
|replace|bool|Val|true|use REPLACE INTO instead of INSERT INTO|
|trunc|bool|Val||Truncate target table|
|dry_run|bool|Val||Print SQL commands to the stdout|
|fldfunc|bool|Val|false|create columns for fldfuncs|
|in|algo.cstring|Val|"-"|Input directory or filename, - for stdin|
|db|algo.cstring|Val|""|Optional database name|
|createdb|bool|Val||Emit CREATE DATABASE code for namespace specified with <db>|
|fkey|bool|Val||Enable foreign key constraints (uses InnoDB storage engine)|

#### command.ssimfilt - Command line for [ssimfilt](/txt/exe/ssimfilt/README.md)
<a href="#command-ssimfilt"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|algo.cstring|Val|"data"|Input directory or filename, - for stdin|
|typetag|algo.cstring|RegxSql|"%"|(filter) Match typetag. ^=first encountered typetag|
|match|algo.cstring|Tary||(filter) Select input tuple if value of key matches value (regx:regx)|
|field|algo.cstring|Tary||(project) Select fields for output (regx)|
|format|u8|Val|0|Output format for selected tuples|
|t|bool|Val|false|Alias for -format:table|
|cmd|algo.cstring|Val|""|Command to output|

#### command.strconv - Command line for [strconv](/txt/exe/strconv/README.md)
<a href="#command-strconv"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|str|algo.cstring|Val||String parameter|
|tocamelcase|bool|Val|false|Convert string to camel case|
|tolowerunder|bool|Val|false|Convert string to lower-under|
|in|algo.cstring|Val|"data"|Input directory or filename, - for stdin|
|pathcomp|algo.Smallstr100|Val|""|Extract path component from string|

#### command.sv2ssim - Command line for [sv2ssim](/txt/exe/sv2ssim/README.md)
<a href="#command-sv2ssim"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|algo.cstring|Val|"data"|Input directory or filename, - for stdin|
|fname|algo.cstring|Val||Input file, use - for stdin|
|separator|char|Val|','|Input field separator|
|outseparator|algo.cstring|Val|""|Output separator. Default: ssim|
|header|bool|Val|true|File has header line|
|ctype|algo.cstring|Val|""|Type tag for output tuples|
|ssimfile|algo.cstring|Val|""|(with -schema) Create ssimfile definition|
|schema|bool|Val||(output)Generate schema from input file|
|field|algo.cstring|RegxSql|"%"|(output) Print selected fields|
|data|bool|Val||(output) Convert input file to ssim tuples|
|report|bool|Val|true|Print final report|
|prefer_signed|bool|Val||Prefer signed types when given a choice|


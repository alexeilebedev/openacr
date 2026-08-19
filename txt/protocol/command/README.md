## command - Command line descriptions
<a href="#command"></a>
The command protocol describes command lines of all executables in the system.
Each command line is described by a struct named `command.<namespace>`.
These are attached to executables via [ccmdline](/txt/ssimdb/dmmeta/ccmdline.md)

### Table Of Contents
<a href="#table-of-contents"></a>
<!-- abt_md.toc_beg -->
<!-- abt_md.toc_end -->

### Ctypes
<a href="#ctypes"></a>
Other ctypes in this namespace which don't have own readme files

#### command.abt - Algo Build Tool - build & link C++ targets
<a href="#command-abt"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|target|[dev.Target](/txt/ssimdb/dev/target.md)|[RegxSql](/txt/exe/amc/reftype.md#regxsql)|""|Regx of target name|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"data"|Root of input ssim dir|
|cfg|[dev.Cfg](/txt/ssimdb/dev/cfg.md)|[RegxSql](/txt/exe/amc/reftype.md#regxsql)|""|Set config|
|compiler|[dev.Compiler](/txt/ssimdb/dev/compiler.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)|""|Set compiler.|
|uname|[dev.Uname](/txt/ssimdb/dev/uname.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)|""|Set uname (default: guess)|
|arch|[dev.Arch](/txt/ssimdb/dev/arch.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)|""|Set architecture (default: guess)|
|ood|bool|[Val](/txt/exe/amc/reftype.md#val)||List out-of-date source files|
|list|bool|[Val](/txt/exe/amc/reftype.md#val)||List target files|
|listincl|bool|[Val](/txt/exe/amc/reftype.md#val)||List includes|
|build|bool|[Val](/txt/exe/amc/reftype.md#val)||If set, build specified target (all necessary steps)|
|preproc|bool|[Val](/txt/exe/amc/reftype.md#val)||Preprocess file, produce .i file|
|srcfile|[dev.Gitfile](/txt/ssimdb/dev/gitfile.md)|[RegxSql](/txt/exe/amc/reftype.md#regxsql)|"%"|Build/disassemble/preprocess specific file|
|clean|bool|[Val](/txt/exe/amc/reftype.md#val)||Delete all output files|
|dry_run|bool|[Val](/txt/exe/amc/reftype.md#val)||Print actions, do not perform|
|maxjobs|i32|[Val](/txt/exe/amc/reftype.md#val)|0|Maximum number of child build processes. 0=pick good default|
|printcmd|bool|[Val](/txt/exe/amc/reftype.md#val)||Print commands. Do not execute|
|force|bool|[Val](/txt/exe/amc/reftype.md#val)||Assume all files are out-of-date|
|install|bool|[Val](/txt/exe/amc/reftype.md#val)|false|Update soft-link under bin/|
|coverity|bool|[Val](/txt/exe/amc/reftype.md#val)|false|Run abt in coverity mode|
|package|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|""|Package tag|
|maxerr|u32|[Val](/txt/exe/amc/reftype.md#val)|100|Max failing commands before rest of pipeline is forced to fail|
|disas|[algo.cstring](/txt/protocol/algo/cstring.md)|[RegxSql](/txt/exe/amc/reftype.md#regxsql)|""|Regex of function to disassemble|
|report|bool|[Val](/txt/exe/amc/reftype.md#val)|true|Print final report|
|jcdb|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|""|Create JSON compilation database in specified file|
|cache|u8|[Val](/txt/exe/amc/reftype.md#val)||Cache mode|
|shortlink|bool|[Val](/txt/exe/amc/reftype.md#val)||Try to shorten sort link if possible|

#### command.abt_md - Tool to generate markdown documentation
<a href="#command-abt_md"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"data"|Input directory or filename, - for stdin|
|readmefile|[dev.Readmefile](/txt/ssimdb/dev/readmefile.md)|[RegxSql](/txt/exe/amc/reftype.md#regxsql)|"%"|Regx of readme to process/show (empty=all)|
|ns|[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)|[RegxSql](/txt/exe/amc/reftype.md#regxsql)|""|(overrides -readme) Process readmes for this namespace|
|section|[algo.cstring](/txt/protocol/algo/cstring.md)|[RegxSql](/txt/exe/amc/reftype.md#regxsql)|"%"|Select specific section to process|
|update|bool|[Val](/txt/exe/amc/reftype.md#val)|true|(action) Update mode: Re-generate mdfiles|
|check|bool|[Val](/txt/exe/amc/reftype.md#val)|false|(action) Check mode: Check syntax and links|
|link|bool|[Val](/txt/exe/amc/reftype.md#val)|false|(with -print) Print links|
|anchor|bool|[Val](/txt/exe/amc/reftype.md#val)|false|(with -print) Print anchors|
|print|bool|[Val](/txt/exe/amc/reftype.md#val)|false|(action) Query mode: Print .md section without evaluating|
|dry_run|bool|[Val](/txt/exe/amc/reftype.md#val)|false|Do not write changes to disk|
|external|bool|[Val](/txt/exe/amc/reftype.md#val)||Check external links as well (may fail if no internet connection)|
|evalcmd|bool|[Val](/txt/exe/amc/reftype.md#val)|true|Execute inline-commands|

#### command.acr - Algo Cross-Reference - ssimfile database & update tool
<a href="#command-acr"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|query|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|""|Regx to match record|
|where|[algo.cstring](/txt/protocol/algo/cstring.md)|[Tary](/txt/exe/amc/reftype.md#tary)||Additional key:value pairs to match|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"data"|Input directory or filename, - for stdin|
|del|bool|[Val](/txt/exe/amc/reftype.md#val)||Delete found item|
|sel|bool|[Val](/txt/exe/amc/reftype.md#val)||Read stdin and select records|
|insert|bool|[Val](/txt/exe/amc/reftype.md#val)||Read stdin and insert tuples|
|replace|bool|[Val](/txt/exe/amc/reftype.md#val)||Read stdin and replace tuples|
|update|bool|[Val](/txt/exe/amc/reftype.md#val)||Read stdin and update attributes of existing tuples|
|merge|bool|[Val](/txt/exe/amc/reftype.md#val)||Combination of -update and -insert|
|unused|bool|[Val](/txt/exe/amc/reftype.md#val)|false|Only select records which are not referenced.|
|trunc|bool|[Val](/txt/exe/amc/reftype.md#val)|false|Truncate table on first write|
|check|bool|[Val](/txt/exe/amc/reftype.md#val)||Run cross-reference check on selection|
|selerr|bool|[Val](/txt/exe/amc/reftype.md#val)|true|(with -check): Select error records|
|maxshow|i32|[Val](/txt/exe/amc/reftype.md#val)|100|Limit number of errors per table|
|write|bool|[Val](/txt/exe/amc/reftype.md#val)||Write data back to disk.|
|rename|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|""|Change value of found item|
|nup|i32|[Val](/txt/exe/amc/reftype.md#val)|0|Number of levels to go up|
|ndown|i32|[Val](/txt/exe/amc/reftype.md#val)|0|Number of levels to go down|
|l|bool|[Val](/txt/exe/amc/reftype.md#val)||Go down via pkeys only|
|xref|bool|[Val](/txt/exe/amc/reftype.md#val)|false|Short for -nup 100 -ndown 100|
|fldfunc|bool|[Val](/txt/exe/amc/reftype.md#val)|false|Evaluate fldfunc when printing tuple|
|maxgroup|i32|[Val](/txt/exe/amc/reftype.md#val)|25|Max. items per group|
|pretty|bool|[Val](/txt/exe/amc/reftype.md#val)|true|Align output in blocks|
|tree|bool|[Val](/txt/exe/amc/reftype.md#val)|false|Print as tree|
|loose|bool|[Val](/txt/exe/amc/reftype.md#val)|false|Allow printing a record before its references (used with -e)|
|my|bool|[Val](/txt/exe/amc/reftype.md#val)|false|Invoke acr_my -e (using acr_my directly is faster)|
|schema|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"data"|Directory for initializing acr meta-data|
|e|bool|[Val](/txt/exe/amc/reftype.md#val)|false|Open selection in editor, write back when done.|
|t|bool|[Val](/txt/exe/amc/reftype.md#val)|false|Short for -tree -xref -loose|
|g|bool|[Val](/txt/exe/amc/reftype.md#val)||Trigger git commands for changes in dev.gitfile table|
|x|bool|[Val](/txt/exe/amc/reftype.md#val)||Propagate select/rename/delete to ssimreq records|
|rowid|bool|[Val](/txt/exe/amc/reftype.md#val)|false|Always print acr.rowid attribute|
|cmt|bool|[Val](/txt/exe/amc/reftype.md#val)|false|Print comments for all columns referenced in output|
|report|bool|[Val](/txt/exe/amc/reftype.md#val)|true|Show final report|
|print|bool|[Val](/txt/exe/amc/reftype.md#val)|true|Print selected records|
|cmd|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|""|Print script with command execution for each selected row|
|field|[algo.cstring](/txt/protocol/algo/cstring.md)|[Tary](/txt/exe/amc/reftype.md#tary)||Fields to select|
|regxof|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|""|Single field: output regx of matching field values|
|meta|bool|[Val](/txt/exe/amc/reftype.md#val)||Select meta-data for selected records|

#### command.acr_compl - ACR shell auto-complete for all targets
<a href="#command-acr_compl"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|data|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"data"|Source for completions (dir or file or -)|
|schema|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"data"|Source for schema information|
|line|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|""|Simulates COMP_LINE (debug)|
|point|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|""|Simulates COMP_POINT (debug). default: whole line|
|type|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"9"|Simulates COMP_TYPE (debug)|
|install|bool|[Val](/txt/exe/amc/reftype.md#val)||Produce bash commands to install the handler|
|debug_log|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|""|Log file for debug information, overrides ACR_COMPL_DEBUG_LOG|
|check|bool|[Val](/txt/exe/amc/reftype.md#val)|false|Check command line validity|
|check_batch|bool|[Val](/txt/exe/amc/reftype.md#val)||Batch mode: read acr_compl.checkreq from stdin, emit acr_compl.checkerr per failure|

#### command.acr_dm - ACR Diff/Merge
<a href="#command-acr_dm"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"data"|Input directory or filename, - for stdin|
|arg|[algo.cstring](/txt/protocol/algo/cstring.md)|[Tary](/txt/exe/amc/reftype.md#tary)||Files to merge: older ours theirs...|
|write_ours|bool|[Val](/txt/exe/amc/reftype.md#val)||Write result to ours file|
|msize|u8|[Val](/txt/exe/amc/reftype.md#val)|7|Conflict marker size|
|anchor|bool|[Val](/txt/exe/amc/reftype.md#val)||Print each row's anchor, the row it was placed after|
|rowid|bool|[Val](/txt/exe/amc/reftype.md#val)||Print acr.rowid, each row's position in the merged file|

#### command.acr_ed - Script generator for common dev tasks
<a href="#command-acr_ed"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"data"|Input directory or filename, - for stdin|
|create|bool|[Val](/txt/exe/amc/reftype.md#val)|false|Create new entity (-finput, -target, -ctype, -field)|
|del|bool|[Val](/txt/exe/amc/reftype.md#val)|false|Delete mode|
|rename|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|""|Rename to something else|
|finput|bool|[Val](/txt/exe/amc/reftype.md#val)|false|Create in-memory table based on ssimfile|
|foutput|bool|[Val](/txt/exe/amc/reftype.md#val)||Declare field as an output|
|srcfile|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|""|Create/Rename/Delete a source file|
|gstatic|bool|[Val](/txt/exe/amc/reftype.md#val)||Like -finput, but data is loaded at compile time|
|indexed|bool|[Val](/txt/exe/amc/reftype.md#val)|false|(with -finput) Add hash index|
|target|[dev.Target](/txt/ssimdb/dev/target.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)|""|Create/Rename/Delete target|
|nstype|[dmmeta.Nstype](/txt/ssimdb/dmmeta/nstype.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)|"exe"|(with -create -target): exe,lib,etc.|
|ctype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)|""|Create/Rename/Delete ctype|
|ssimfile|[dmmeta.Ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)|""|  Ssimfile for new ctype|
|subset|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)|""|  Primary key is a subset of this ctype|
|subset2|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)|""|  Primary key is also a subset of this ctype|
|separator|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"."|    Key separator|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)|""|Create field|
|arg|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)|""|  Field type (e.g. u32, etc), (with -ctype) add the base field|
|dflt|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|""|  Field default value|
|anon|bool|[Val](/txt/exe/amc/reftype.md#val)|false|  Anonymous field (use with command lines)|
|bigend|bool|[Val](/txt/exe/amc/reftype.md#val)|false|  Big-endian field|
|cascdel|bool|[Val](/txt/exe/amc/reftype.md#val)|false|  Field is cascdel|
|before|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)|""|  Place field before this one|
|substr|[dmmeta.Substr](/txt/ssimdb/dmmeta/substr.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)|""|  New field is a substring|
|alias|bool|[Val](/txt/exe/amc/reftype.md#val)||Create alias field (requires -srcfield)|
|srcfield|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)|""|  Source field for bitfld/substr|
|inscond|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"true"|  Insert condition (for xref)|
|reftype|[dmmeta.Reftype](/txt/ssimdb/dmmeta/reftype.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)|""|  Reftype (e.g. Val, Thash, Llist, etc)|
|hashfld|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)|""|    (-reftype:Thash) Hash field|
|sortfld|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)|""|    (-reftype:Bheap) Sort field|
|unittest|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|""|Create unit test, <ns>.<functionname>|
|citest|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|""|Create CI test|
|cppfunc|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|""|Field is a cppfunc, pass c++ expression as argument|
|xref|bool|[Val](/txt/exe/amc/reftype.md#val)|false|    X-ref with field type|
|via|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|""|      X-ref argument (index, pointer, or index/key)|
|write|bool|[Val](/txt/exe/amc/reftype.md#val)|false|Commit output to disk|
|e|bool|[Val](/txt/exe/amc/reftype.md#val)|false| (with -create -unittest) Edit new testcase|
|comment|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|""|Comment for new entity|
|sandbox|bool|[Val](/txt/exe/amc/reftype.md#val)|false|Make changes in sandbox|
|showcpp|bool|[Val](/txt/exe/amc/reftype.md#val)||(With -sandbox), show resulting diff|
|msgtype|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|""|(with -ctype) use this msgtype as type|
|anonfld|bool|[Val](/txt/exe/amc/reftype.md#val)||Create anonfld|
|license|[dev.License](/txt/ssimdb/dev/license.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)|"GPL"|License for new source/script file|
|fstep|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)|""|Add fstep record on existing field (use with -create)|
|steptype|[dmmeta.Steptype](/txt/ssimdb/dmmeta/steptype.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)|"Inline"|Steptype for -create -fstep|
|fcurs|[dmmeta.Fcurs](/txt/ssimdb/dmmeta/fcurs.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)|""|Add fcurs record (-create); pkey is <field>/<curstype-name>|
|dispatch_msg|[dmmeta.DispatchMsg](/txt/ssimdb/dmmeta/dispatch_msg.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)|""|Add dispatch_msg record (-create); pkey is <dispatch>/<msgtype>|

#### command.acr_in - ACR Input - compute set of ssimfiles or tuples used by a specific target
<a href="#command-acr_in"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ns|[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)|[RegxSql](/txt/exe/amc/reftype.md#regxsql)|""|Regx of matching namespace|
|data|bool|[Val](/txt/exe/amc/reftype.md#val)|false|List ssimfile contents|
|sigcheck|bool|[Val](/txt/exe/amc/reftype.md#val)|true|Output sigcheck records for schema version mismatch detection|
|list|bool|[Val](/txt/exe/amc/reftype.md#val)|false|List ssimfile names|
|t|bool|[Val](/txt/exe/amc/reftype.md#val)||(with -list) Tree mode|
|data_dir|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"data"|Directory with ssimfiles|
|schema|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"data"||
|related|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|""|Select only tuples related to specified acr key|
|notssimfile|[dmmeta.Ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|[RegxSql](/txt/exe/amc/reftype.md#regxsql)|""|Exclude ssimfiles matching regx|
|checkable|bool|[Val](/txt/exe/amc/reftype.md#val)||Ensure output passes acr -check|
|r|[dmmeta.Ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|[RegxSql](/txt/exe/amc/reftype.md#regxsql)|""|Reverse lookup of target by ssimfile|

#### command.acr_my - ACR <-> MariaDB adaptor
<a href="#command-acr_my"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|nsdb|[dmmeta.Nsdb](/txt/ssimdb/dmmeta/nsdb.md)|[RegxSql](/txt/exe/amc/reftype.md#regxsql)|""|Regx of ssim namespace (dmmeta.nsdb) to select|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"data"|Input directory or filename, - for stdin|
|schema|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"data"|Input directory or filename, - for stdin|
|fldfunc|bool|[Val](/txt/exe/amc/reftype.md#val)|false|Evaluate fldfunc when printing tuple|
|fkey|bool|[Val](/txt/exe/amc/reftype.md#val)||Enable foreign key constraints|
|e|bool|[Val](/txt/exe/amc/reftype.md#val)|false|Alias for -start -shell -stop|
|start|bool|[Val](/txt/exe/amc/reftype.md#val)|false|Start local mysql server|
|stop|bool|[Val](/txt/exe/amc/reftype.md#val)|false|Stop local mysql server, saving data|
|abort|bool|[Val](/txt/exe/amc/reftype.md#val)|false|Abort local mysql server, losing data|
|shell|bool|[Val](/txt/exe/amc/reftype.md#val)|false|Connect to local mysql server|
|serv|bool|[Val](/txt/exe/amc/reftype.md#val)|false|Start mysql with TCP/IP service enabled|

#### command.amc - Algo Model Compiler: generate code under include/gen and cpp/gen
<a href="#command-amc"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in_dir|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"data"|Root of input ssim dir|
|query|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|""|Query mode: generate code for specified object|
|out_dir|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"."|Root of output cpp dir|
|proto|bool|[Val](/txt/exe/amc/reftype.md#val)|false|Print prototype|
|showcomment|bool|[Val](/txt/exe/amc/reftype.md#val)|true|Show generated comments|
|report|bool|[Val](/txt/exe/amc/reftype.md#val)|true|Final report|
|e|bool|[Val](/txt/exe/amc/reftype.md#val)|false|Open matching records in editor|
|trace|[algo.cstring](/txt/protocol/algo/cstring.md)|[RegxSql](/txt/exe/amc/reftype.md#regxsql)|""|Regx of something to trace code generation|
|derive|bool|[Val](/txt/exe/amc/reftype.md#val)|false|Derive and write the amc-owned tables; generate no source|

#### command.amc_gc - Garbage collector for in-memory databases
<a href="#command-amc_gc"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|target|[dev.Target](/txt/ssimdb/dev/target.md)|[RegxSql](/txt/exe/amc/reftype.md#regxsql)|"%"|Target to test-build|
|key|[algo.cstring](/txt/protocol/algo/cstring.md)|[RegxSql](/txt/exe/amc/reftype.md#regxsql)|""|ACR query selecting records to eliminate, e.g. dmmeta.ctype:amc.%|
|include|bool|[Val](/txt/exe/amc/reftype.md#val)||Garbage collect includes for specified target|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"data"|Input directory or filename, - for stdin|

#### command.amc_vis - Draw access path diagrams
<a href="#command-amc_vis"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ctype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[RegxSql](/txt/exe/amc/reftype.md#regxsql)|"%"|Ctype regexp to compute access path diagram|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"data"|Input directory or filename, - for stdin|
|dot|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|""|Save dot file with specified filename|
|xref|bool|[Val](/txt/exe/amc/reftype.md#val)|false|Include all ctypes referenced by selected ones|
|xns|bool|[Val](/txt/exe/amc/reftype.md#val)|true|Cross namespace boundaries|
|noinput|bool|[Val](/txt/exe/amc/reftype.md#val)||Deselect module inputs|
|check|bool|[Val](/txt/exe/amc/reftype.md#val)||Check model for dependency problems|
|render|bool|[Val](/txt/exe/amc/reftype.md#val)|true|Produce an ascii drawing|

#### command.ams_sendtest - Algo Messaging System test tool
<a href="#command-ams_sendtest"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"data"|Input directory or filename, - for stdin|
|id|i32|[Val](/txt/exe/amc/reftype.md#val)||Process index (0=parent)|
|file_prefix|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|""|Use file_prefix|
|nchild|i32|[Val](/txt/exe/amc/reftype.md#val)|1|Number of stream readers|
|blocking|bool|[Val](/txt/exe/amc/reftype.md#val)|false|Use blocking send mode|
|nmsg|i32|[Val](/txt/exe/amc/reftype.md#val)|100000|Number of messages to send/receive|
|trace|[algo.cstring](/txt/protocol/algo/cstring.md)|[RegxSql](/txt/exe/amc/reftype.md#regxsql)|""|Regx expression specifying what to trace|
|timeout|i32|[Val](/txt/exe/amc/reftype.md#val)|30|Time limit for the send|
|recvdelay_ns|i64|[Val](/txt/exe/amc/reftype.md#val)||Pause nanoseconds between messages|
|senddelay_ns|i64|[Val](/txt/exe/amc/reftype.md#val)||Pause nanoseconds between messages|
|msgsize_min|i32|[Val](/txt/exe/amc/reftype.md#val)|64|Minimum message length|
|msgsize_max|i32|[Val](/txt/exe/amc/reftype.md#val)|256|Maximum message length|
|bufsize|i32|[Val](/txt/exe/amc/reftype.md#val)|655360|Shared memory buffer size|
|recvdelay|i64|[Val](/txt/exe/amc/reftype.md#val)||Pause nanoseconds between messages|
|signaled|bool|[Val](/txt/exe/amc/reftype.md#val)|false|Enable signaled mode|
|board|bool|[Val](/txt/exe/amc/reftype.md#val)|false|Carry messages too big for the ring on the message board|
|board_pin|i32|[Val](/txt/exe/amc/reftype.md#val)|4|Board slots one reader may pin at once (board mode)|
|uc|bool|[Val](/txt/exe/amc/reftype.md#val)|false|Unicast: one lane per reader instead of one shared lane|

#### command.amsspy - List ams sessions and monitor traffic on host
<a href="#command-amsspy"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"data"|Input directory or filename, - for stdin|
|session|[algo.cstring](/txt/protocol/algo/cstring.md)|[RegxSql](/txt/exe/amc/reftype.md#regxsql)|"%"|Session regex|
|list|bool|[Val](/txt/exe/amc/reftype.md#val)|false|List sessions|
|shm|bool|[Val](/txt/exe/amc/reftype.md#val)|false|List shms|
|spy|[algo.cstring](/txt/protocol/algo/cstring.md)|[RegxSql](/txt/exe/amc/reftype.md#regxsql)|""|Spy on named shared memory segment|
|f|u8|[Val](/txt/exe/amc/reftype.md#val)|0|(output) Output format|
|clean|bool|[Val](/txt/exe/amc/reftype.md#val)||Unlink orphaned /dev/shm ams segments and exit|
|dump|bool|[Val](/txt/exe/amc/reftype.md#val)|false|Dump every segment's header + reader offsets/lag (out-of-band) and exit|

#### command.apm - Algo Package Manager
<a href="#command-apm"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"data"|Input directory or filename, - for stdin|
|pkgdata|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|""|Load package definitions from here|
|package|[dev.Package](/txt/ssimdb/dev/package.md)|[RegxSql](/txt/exe/amc/reftype.md#regxsql)|""|Regx of package|
|ns|[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)|[RegxSql](/txt/exe/amc/reftype.md#regxsql)|""|Operate on specified namespace only|
|install|bool|[Val](/txt/exe/amc/reftype.md#val)||Install new package (specify -origin)|
|update|bool|[Val](/txt/exe/amc/reftype.md#val)||Update package (-origin)|
|list|bool|[Val](/txt/exe/amc/reftype.md#val)||List installed packages|
|diff|bool|[Val](/txt/exe/amc/reftype.md#val)||Diff package with respect to installed version|
|push|bool|[Val](/txt/exe/amc/reftype.md#val)||Evaluate package diff and push it to origin|
|check|bool|[Val](/txt/exe/amc/reftype.md#val)||Consistency check|
|remove|bool|[Val](/txt/exe/amc/reftype.md#val)||Remove specified package|
|origin|[algo.Smallstr200](/txt/protocol/algo/README.md#algo-smallstr200)|[Val](/txt/exe/amc/reftype.md#val)|""|Upstream URL of new package|
|ref|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype.md#val)|""|(with -create) Gitref or branch to fetch|
|dry_run|bool|[Val](/txt/exe/amc/reftype.md#val)||Do not execute transaction|
|showrec|bool|[Val](/txt/exe/amc/reftype.md#val)||Show records belonging to package|
|showfile|bool|[Val](/txt/exe/amc/reftype.md#val)||List package files (gitfile records)|
|generate|bool|[Val](/txt/exe/amc/reftype.md#val)||Generate package records into apm/gen/<package>.ssim|
|R|bool|[Val](/txt/exe/amc/reftype.md#val)||reverse the diff direction|
|l|bool|[Val](/txt/exe/amc/reftype.md#val)||Use local package definition on the remote side|
|reset|bool|[Val](/txt/exe/amc/reftype.md#val)||Reset package baseref/origin to those provided by the command line|
|checkclean|bool|[Val](/txt/exe/amc/reftype.md#val)|true|Ensure that changes are applied to a clean directory|
|t|bool|[Val](/txt/exe/amc/reftype.md#val)||Select parent packages for operation|
|stat|bool|[Val](/txt/exe/amc/reftype.md#val)||(with -diff) show stats|
|annotate|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|""|Read file and annotate each input tuple with package(s) it belongs to|
|data_in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"data"|Dataset from which package records are loaded|
|e|bool|[Val](/txt/exe/amc/reftype.md#val)||Open selected records in editor|
|binpath|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"bin"|(internal use)|

#### command.aqlite - Runs sqlite queries against ssim files
<a href="#command-aqlite"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"data"|Input directory or filename, - for stdin|
|schema|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"data"|Schema dir|
|cmd|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||Sql Query to run|
|ns|[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)|[RegxSql](/txt/exe/amc/reftype.md#regxsql)|"dmmeta"|Regx of databases to attach|

#### command.atf_amc - Unit tests for amc (see amctest table)
<a href="#command-atf_amc"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"data"|Input directory or filename, - for stdin|
|amctest|[atfdb.Amctest](/txt/ssimdb/atfdb/amctest.md)|[RegxSql](/txt/exe/amc/reftype.md#regxsql)|"%"|SQL regex, selecting test to run|
|dofork|bool|[Val](/txt/exe/amc/reftype.md#val)|true|Use fork|
|q|bool|[Val](/txt/exe/amc/reftype.md#val)||Quiet mode|

#### command.atf_ci - Normalization tests (see citest table)
<a href="#command-atf_ci"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"data"|Input directory or filename, - for stdin|
|citest|[atfdb.Citest](/txt/ssimdb/atfdb/citest.md)|[RegxSql](/txt/exe/amc/reftype.md#regxsql)|"%"|Regx of tests to run|
|maxerr|i32|[Val](/txt/exe/amc/reftype.md#val)|0|Exit after this many errors|
|cijob|[atfdb.Cijob](/txt/ssimdb/atfdb/cijob.md)|[RegxSql](/txt/exe/amc/reftype.md#regxsql)|"%"||
|capture|bool|[Val](/txt/exe/amc/reftype.md#val)||Capture the output of the test|
|check_clean|bool|[Val](/txt/exe/amc/reftype.md#val)|true|Check for modifications after each test|

#### command.atf_cmdline - Test tool for command line parsing
<a href="#command-atf_cmdline"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"data"|Input directory or filename, - for stdin|
|exec|bool|[Val](/txt/exe/amc/reftype.md#val)||Execv itself|
|astr|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||Required anon string|
|anum|i32|[Val](/txt/exe/amc/reftype.md#val)||Anon number|
|adbl|double|[Val](/txt/exe/amc/reftype.md#val)||Anon double|
|aflag|bool|[Val](/txt/exe/amc/reftype.md#val)||Anon flag|
|str|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||Required string|
|num|i32|[Val](/txt/exe/amc/reftype.md#val)||Required Number|
|dbl|double|[Val](/txt/exe/amc/reftype.md#val)||Required double|
|flag|bool|[Val](/txt/exe/amc/reftype.md#val)||Required flag|
|dstr|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"blah"|Predefined string|
|dnum|i32|[Val](/txt/exe/amc/reftype.md#val)|-33|Predefined number|
|ddbl|double|[Val](/txt/exe/amc/reftype.md#val)|0.0001|Predefined double|
|dflag|bool|[Val](/txt/exe/amc/reftype.md#val)|true|Predefined flag|
|mstr|[algo.cstring](/txt/protocol/algo/cstring.md)|[Tary](/txt/exe/amc/reftype.md#tary)||String array|
|mnum|i32|[Tary](/txt/exe/amc/reftype.md#tary)||Number array|
|mdbl|double|[Tary](/txt/exe/amc/reftype.md#tary)||Double array|
|amnum|i32|[Tary](/txt/exe/amc/reftype.md#tary)||Anon number array|
|fconst|u8|[Val](/txt/exe/amc/reftype.md#val)||Fconst for field|
|cconst|[algo.Month](/txt/protocol/algo/Month.md)|[Val](/txt/exe/amc/reftype.md#val)||Fconst for arg ctype|
|dregx|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[RegxSql](/txt/exe/amc/reftype.md#regxsql)|"%"|Predefined regx|
|dpkey|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)|""|Predefined pkey|

#### command.atf_comp - Command line for [atf_comp](/txt/exe/atf_comp/README.md)
<a href="#command-atf_comp"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"data"|Input directory or filename, - for stdin|
|comptest|[atfdb.Comptest](/txt/ssimdb/atfdb/comptest.md)|[RegxSql](/txt/exe/amc/reftype.md#regxsql)|"%"|Select comptest (SQL regex)|
|mode|u8|[Val](/txt/exe/amc/reftype.md#val)|0|Test mode|
|capture|bool|[Val](/txt/exe/amc/reftype.md#val)||Alias for -mode:capture|
|ee|bool|[Val](/txt/exe/amc/reftype.md#val)||Alias for -mode:editsource|
|e|bool|[Val](/txt/exe/amc/reftype.md#val)||Alias for -mode:edit|
|cfg|[dev.Cfg](/txt/ssimdb/dev/cfg.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)|"release"|Configuration (determines bindir)|
|maxerr|i32|[Val](/txt/exe/amc/reftype.md#val)|3|Exit after this many errors|

#### command.atf_cov - Line coverage
<a href="#command-atf_cov"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"data"|Input directory or filename, - for stdin|
|covdir|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"temp/covdata"|Output directory to save coverage data|
|logfile|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|""|Log file|
|runcmd|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|""|command to run|
|exclude|[dev.Gitfile](/txt/ssimdb/dev/gitfile.md)|[RegxSql](/txt/exe/amc/reftype.md#regxsql)|"(extern|include/gen|cpp/gen)/%"|Exclude gitfiles (external, generated)|
|mergepath|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|""|colon-separated dir list to load .cov.ssim files from|
|gcov|bool|[Val](/txt/exe/amc/reftype.md#val)||run gcov|
|ssim|bool|[Val](/txt/exe/amc/reftype.md#val)||write out ssim files|
|report|bool|[Val](/txt/exe/amc/reftype.md#val)||write out all reports|
|capture|bool|[Val](/txt/exe/amc/reftype.md#val)||Write coverage information into tgtcov table|
|xmlpretty|bool|[Val](/txt/exe/amc/reftype.md#val)||Generate pretty-formatted XML|
|summary|bool|[Val](/txt/exe/amc/reftype.md#val)|true|Show summary figures|
|check|bool|[Val](/txt/exe/amc/reftype.md#val)||Check coverage information against tgtcov table|
|incremental|bool|[Val](/txt/exe/amc/reftype.md#val)||Keep *.gcda files from previous run|

#### command.atf_fuzz - Generator of bad inputs for targets
<a href="#command-atf_fuzz"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|reprofile|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"temp/atf_fuzz.repro"|File where repros are stored|
|target|[dev.Target](/txt/ssimdb/dev/target.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)|""|Target to fuzz|
|args|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|""|Additional arguments to target|
|inputfile|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|""|File with input tuples.|
|fuzzstrat|[atfdb.Fuzzstrat](/txt/ssimdb/atfdb/fuzzstrat.md)|[RegxSql](/txt/exe/amc/reftype.md#regxsql)|"%"|Strategy to choose|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"data"|Input directory or filename, - for stdin|
|seed|i32|[Val](/txt/exe/amc/reftype.md#val)|0|Random seed|
|testprob|double|[Val](/txt/exe/amc/reftype.md#val)|1|Run each case with this probability|

#### command.atf_nrun - Run N subprocesses in parallel
<a href="#command-atf_nrun"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"data"|Input directory or filename, - for stdin|
|maxjobs|i32|[Val](/txt/exe/amc/reftype.md#val)|2|Number of simultaneous jobs|
|ncmd|i32|[Val](/txt/exe/amc/reftype.md#val)|6||

#### command.atf_unit - Unit tests (see unittest table)
<a href="#command-atf_unit"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|unittest|[atfdb.Unittest](/txt/ssimdb/atfdb/unittest.md)|[RegxSql](/txt/exe/amc/reftype.md#regxsql)|"%"|SQL regex, selecting test to run|
|nofork|bool|[Val](/txt/exe/amc/reftype.md#val)||Do not fork for destructive tests|
|arg|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|""|Argument to pass to tool|
|data_dir|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"data"|Data directory|
|mdbg|bool|[Val](/txt/exe/amc/reftype.md#val)|0|Break at testcase in debugger|
|perf_secs|double|[Val](/txt/exe/amc/reftype.md#val)|1.0|# Of seconds to run perf tests for|
|pertest_timeout|u32|[Val](/txt/exe/amc/reftype.md#val)|900|Max runtime of any individual unit test|
|report|bool|[Val](/txt/exe/amc/reftype.md#val)|true|Print final report|
|capture|bool|[Val](/txt/exe/amc/reftype.md#val)||Re-capture test results|
|check_untracked|bool|[Val](/txt/exe/amc/reftype.md#val)|true|Check for untracked file before allowing test to run|

#### command.bash
<a href="#command-bash"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|c|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|""|Shell command to execute|

#### command.bash2html - Convert bash output and colours to html
<a href="#command-bash2html"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"data"|Input directory or filename, - for stdin|
|test|bool|[Val](/txt/exe/amc/reftype.md#val)||Produce Test Output|

#### command.bash_proc
<a href="#command-bash_proc"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|bash|[command.bash](/txt/protocol/command/README.md#command-bash)|[Exec](/txt/exe/amc/reftype.md#exec)|"bash"|Must be bash to support $'' for string quoting|

#### command.gcache - Compiler cache
<a href="#command-gcache"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"data"|Input directory or filename, - for stdin|
|cmd|[algo.cstring](/txt/protocol/algo/cstring.md)|[Tary](/txt/exe/amc/reftype.md#tary)||Command to execute|
|install|bool|[Val](/txt/exe/amc/reftype.md#val)||Create gcache directory and enable gcache|
|stats|bool|[Val](/txt/exe/amc/reftype.md#val)||Show cache stats|
|enable|bool|[Val](/txt/exe/amc/reftype.md#val)||Create .gcache link to enable gcache use|
|disable|bool|[Val](/txt/exe/amc/reftype.md#val)||Remove .gcache link to disable gcache|
|gc|bool|[Val](/txt/exe/amc/reftype.md#val)||Clean old files from .gcache|
|clean|bool|[Val](/txt/exe/amc/reftype.md#val)||Clean the entire cache|
|dir|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"/tmp/gcache"|(With -install,-enable) cache directory|
|maxmb|i64|[Val](/txt/exe/amc/reftype.md#val)|10240|Cache size budget in MB; GC evicts oldest entries past it|
|hitrate|bool|[Val](/txt/exe/amc/reftype.md#val)||Report hit rate (specify start time with -after)|
|after|[algo.UnTime](/txt/protocol/algo/UnTime.md)|[Val](/txt/exe/amc/reftype.md#val)||Start time for reporting|
|report|bool|[Val](/txt/exe/amc/reftype.md#val)||Show end-of-run report|
|force|bool|[Val](/txt/exe/amc/reftype.md#val)||Force recompile and update cache|

#### command.generic - Any generic subprocess
<a href="#command-generic"></a>

#### command.generic_proc
<a href="#command-generic_proc"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|name|[command.generic](/txt/protocol/command/README.md#command-generic)|[Exec](/txt/exe/amc/reftype.md#exec)|""||

#### command.jkv - JSON <-> key-value mapping tool
<a href="#command-jkv"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"data"|Input directory or filename, - for stdin|
|file|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||Filename (use - for stdin)|
|kv|[algo.cstring](/txt/protocol/algo/cstring.md)|[Tary](/txt/exe/amc/reftype.md#tary)||JSON Keyvals|
|r|bool|[Val](/txt/exe/amc/reftype.md#val)||Reverse (json keyvals -> JSON) mapping|
|write|bool|[Val](/txt/exe/amc/reftype.md#val)||Write the modified file back|
|output|u8|[Val](/txt/exe/amc/reftype.md#val)|0|Output format|
|pretty|u32|[Val](/txt/exe/amc/reftype.md#val)|2|Pretty-printer (0-compact; 1=algo style; 2=jq style)|

#### command.mdbg - Gdb front-end
<a href="#command-mdbg"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|target|[dev.Target](/txt/ssimdb/dev/target.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)||Executable name|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"data"|Input directory or filename, - for stdin|
|args|[algo.cstring](/txt/protocol/algo/cstring.md)|[Tary](/txt/exe/amc/reftype.md#tary)|""|Additional module args|
|cfg|[dev.Cfg](/txt/ssimdb/dev/cfg.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)|"debug"|Configuration to use|
|disas|bool|[Val](/txt/exe/amc/reftype.md#val)||Show disassembly (use F12)|
|attach|bool|[Val](/txt/exe/amc/reftype.md#val)|false|Attach to a running process|
|pid|i32|[Val](/txt/exe/amc/reftype.md#val)|0|(with -attach) Pid, if omitted mdbg will guess|
|b|[algo.cstring](/txt/protocol/algo/cstring.md)|[Tary](/txt/exe/amc/reftype.md#tary)|""|Set breakpoint, e.g. 'a.cpp:123 if cond1', 'func#3'|
|catchthrow|bool|[Val](/txt/exe/amc/reftype.md#val)|true|Stop on exceptions|
|tui|bool|[Val](/txt/exe/amc/reftype.md#val)||Use gdb -tui as the debugger|
|bcmd|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|""|Evaluate command at breakpoint|
|emacs|bool|[Val](/txt/exe/amc/reftype.md#val)|true|Use emacs environment as the debugger|
|manywin|bool|[Val](/txt/exe/amc/reftype.md#val)|false|Use gdb-many-windows emacs mode|
|follow_child|bool|[Val](/txt/exe/amc/reftype.md#val)||When forking, follow child (default is parent)|
|py|bool|[Val](/txt/exe/amc/reftype.md#val)||Enable python scripting|
|dry_run|bool|[Val](/txt/exe/amc/reftype.md#val)||Print commands but don't execute|
|nonstop|bool|[Val](/txt/exe/amc/reftype.md#val)||Debug a process tree: keep every spawned process under gdb and let them run|

#### command.mysql2ssim - mysql -> ssim conversion tool
<a href="#command-mysql2ssim"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|writessimfile|bool|[Val](/txt/exe/amc/reftype.md#val)||Write to ssimfile directly|
|url|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||user:pass@host/db or sock:///filename/db|
|tables|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|""|comma-separated list of tables. Default is all tables|
|schema|bool|[Val](/txt/exe/amc/reftype.md#val)||Generate ssim type definition|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"data"|Input directory or filename, - for stdin|
|pretty|bool|[Val](/txt/exe/amc/reftype.md#val)|false|Format output for the screen|
|nologo|bool|[Val](/txt/exe/amc/reftype.md#val)||Don't show copyright notice|
|baddbok|bool|[Val](/txt/exe/amc/reftype.md#val)||Don't claim if bad database|

#### command.orgfile - Organize and deduplicate files by timestamp and by contents
<a href="#command-orgfile"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"data"|Input directory or filename, - for stdin|
|move|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|""|Read stdin, rename files based on pattern|
|dedup|[algo.cstring](/txt/protocol/algo/cstring.md)|[RegxSql](/txt/exe/amc/reftype.md#regxsql)|""|Only allow deleting files that match this regx|
|commit|bool|[Val](/txt/exe/amc/reftype.md#val)||Apply changes|
|undo|bool|[Val](/txt/exe/amc/reftype.md#val)||Read previous orgfile output, undoing movement|
|hash|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"sha1"|Hash command to use for deduplication|

#### command.samp_make - sample program for Makefile management
<a href="#command-samp_make"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"data"|Input directory or filename, - for stdin|
|target|[sampdb.Target](/txt/ssimdb/sampdb/target.md)|[RegxSql](/txt/exe/amc/reftype.md#regxsql)|"%"|Create Makefile for selected targets|
|parse_make|bool|[Val](/txt/exe/amc/reftype.md#val)||Parse extern/gnumake/Simple-Makefile|
|makefile|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"extern/gnumake/Simple-Makefile"|(with parse_make) makefile to parse|
|write|bool|[Val](/txt/exe/amc/reftype.md#val)||P(with parse_make) write ssimfiles, otherwise print them|

#### command.samp_meng - Sample matching engine
<a href="#command-samp_meng"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|proc|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|""|<proc_id>[,<prefix>[,<in_grp>,<out_grp>]]|

#### command.samp_regx - Test tool for regular expressions
<a href="#command-samp_regx"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"data"|Input directory or filename, - for stdin|
|expr|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||Expression|
|style|[algo_lib.RegxStyle](/txt/gen/algo_lib/algo_lib.md#algo_lib-regxstyle)|[Val](/txt/exe/amc/reftype.md#val)|2|Regx style|
|trace|bool|[Val](/txt/exe/amc/reftype.md#val)|false|Trace regx innards|
|capture|bool|[Val](/txt/exe/amc/reftype.md#val)|false|Use capture groups|
|full|bool|[Val](/txt/exe/amc/reftype.md#val)|true|Match full string|
|f|bool|[Val](/txt/exe/amc/reftype.md#val)|false|<string> is a filename, grep the lines|
|match|bool|[Val](/txt/exe/amc/reftype.md#val)||Match a string, exit code represnts success|
|string|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|""|String to match|

#### command.src_func - Access / edit functions
<a href="#command-src_func"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"data"|Input directory or filename, - for stdin|
|targsrc|[dev.Targsrc](/txt/ssimdb/dev/targsrc.md)|[RegxSql](/txt/exe/amc/reftype.md#regxsql)|"%"|(scan) Limit scanning to these sources only|
|acrkey|[algo.cstring](/txt/protocol/algo/cstring.md)|[RegxSql](/txt/exe/amc/reftype.md#regxsql)|"%"|Select function by acr key that caused it|
|func|[dev.Target](/txt/ssimdb/dev/target.md)|[RegxSql](/txt/exe/amc/reftype.md#regxsql)|"%"|Target.function regex|
|nextfile|[dev.Gitfile](/txt/ssimdb/dev/gitfile.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)|""|(action) Print name of next srcfile in targsrc list|
|other|bool|[Val](/txt/exe/amc/reftype.md#val)|false|    (with -nextfile), name of previous file|
|list|bool|[Val](/txt/exe/amc/reftype.md#val)|false|(action) List matching functions|
|updateproto|bool|[Val](/txt/exe/amc/reftype.md#val)|false|(action) Update prototypes in headers|
|createmissing|bool|[Val](/txt/exe/amc/reftype.md#val)||(action) Create missing functions declared in userfunc table|
|iffy|bool|[Val](/txt/exe/amc/reftype.md#val)|false|(filter) Select functions that may contain errors|
|gen|bool|[Val](/txt/exe/amc/reftype.md#val)|false|(scan) Scan generated files|
|showloc|bool|[Val](/txt/exe/amc/reftype.md#val)|false|(output) Show file location|
|f|bool|[Val](/txt/exe/amc/reftype.md#val)|false|(output) -sortname -showcomment -showbody|
|showstatic|bool|[Val](/txt/exe/amc/reftype.md#val)|true|(filter) Allow static functions|
|matchproto|[algo.cstring](/txt/protocol/algo/cstring.md)|[RegxSql](/txt/exe/amc/reftype.md#regxsql)|"%"|(filter) Match function prototype|
|matchbody|[algo.cstring](/txt/protocol/algo/cstring.md)|[RegxSql](/txt/exe/amc/reftype.md#regxsql)|"%"|(filter) Match function body|
|matchcomment|[algo.cstring](/txt/protocol/algo/cstring.md)|[RegxSql](/txt/exe/amc/reftype.md#regxsql)|"%"|(filter) Match function comment|
|showsortkey|bool|[Val](/txt/exe/amc/reftype.md#val)|false|(output) Display function sortkey|
|showcomment|bool|[Val](/txt/exe/amc/reftype.md#val)|false|(output) Display function comment|
|showbody|bool|[Val](/txt/exe/amc/reftype.md#val)|false|(output) Print function body|
|sortname|bool|[Val](/txt/exe/amc/reftype.md#val)|false|(output) Sort functions by name|
|printssim|bool|[Val](/txt/exe/amc/reftype.md#val)|false|(output) Print ssim tuples|
|e|bool|[Val](/txt/exe/amc/reftype.md#val)|false|Edit found functions|
|baddecl|bool|[Val](/txt/exe/amc/reftype.md#val)||Report and fail on bad declarations|
|report|bool|[Val](/txt/exe/amc/reftype.md#val)||Print final report|

#### command.src_hdr - Manage source copyright+license header in source files and scripts
<a href="#command-src_hdr"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"data"|Input directory or filename, - for stdin|
|targsrc|[dev.Targsrc](/txt/ssimdb/dev/targsrc.md)|[RegxSql](/txt/exe/amc/reftype.md#regxsql)|""|Regx of targsrc to update|
|write|bool|[Val](/txt/exe/amc/reftype.md#val)||Update files in-place|
|indent|bool|[Val](/txt/exe/amc/reftype.md#val)||Indent source files|
|update_copyright|bool|[Val](/txt/exe/amc/reftype.md#val)||Update copyright year for current company|
|scriptfile|[dev.Scriptfile](/txt/ssimdb/dev/scriptfile.md)|[RegxSql](/txt/exe/amc/reftype.md#regxsql)|""|Regx of scripts to update header|

#### command.src_lim - Enforce line length, function length, and indentation limits
<a href="#command-src_lim"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"data"|Input directory or filename, - for stdin|
|linelim|bool|[Val](/txt/exe/amc/reftype.md#val)||Check various file limits (line length, function length, etc)|
|srcfile|[dev.Gitfile](/txt/ssimdb/dev/gitfile.md)|[RegxSql](/txt/exe/amc/reftype.md#regxsql)|"%"|Filter for source files to process|
|strayfile|bool|[Val](/txt/exe/amc/reftype.md#val)||Check for unregistered source files|
|capture|bool|[Val](/txt/exe/amc/reftype.md#val)||Generate new dev.linelim records|
|write|bool|[Val](/txt/exe/amc/reftype.md#val)||Update ssim database (with -capture)|
|badchar|bool|[Val](/txt/exe/amc/reftype.md#val)||Check for bad chars in source files|
|badline|[dev.Badline](/txt/ssimdb/dev/badline.md)|[RegxSql](/txt/exe/amc/reftype.md#regxsql)|""|Check badline (acr badline)|

#### command.ssim2csv - Ssim -> csv conversion tool
<a href="#command-ssim2csv"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|expand|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|""||
|ignoreQuote|bool|[Val](/txt/exe/amc/reftype.md#val)|false||

#### command.ssim2mysql - Ssim -> mysql
<a href="#command-ssim2mysql"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|url|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|""|URL of mysql server. user:pass@hostb or sock://filename; Empty -> stdout|
|data_dir|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"data"|Load dmmeta info from this directory|
|maxpacket|i32|[Val](/txt/exe/amc/reftype.md#val)|100000|Max Mysql packet size|
|replace|bool|[Val](/txt/exe/amc/reftype.md#val)|true|use REPLACE INTO instead of INSERT INTO|
|trunc|bool|[Val](/txt/exe/amc/reftype.md#val)||Truncate target table|
|dry_run|bool|[Val](/txt/exe/amc/reftype.md#val)||Print SQL commands to the stdout|
|fldfunc|bool|[Val](/txt/exe/amc/reftype.md#val)|false|create columns for fldfuncs|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"-"|Input directory or filename, - for stdin|
|db|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|""|Optional database name|
|createdb|bool|[Val](/txt/exe/amc/reftype.md#val)||Emit CREATE DATABASE code for namespace specified with <db>|
|fkey|bool|[Val](/txt/exe/amc/reftype.md#val)||Enable foreign key constraints (uses InnoDB storage engine)|

#### command.ssimfilt - Tuple utility
<a href="#command-ssimfilt"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"data"|Input directory or filename, - for stdin|
|typetag|[algo.cstring](/txt/protocol/algo/cstring.md)|[RegxSql](/txt/exe/amc/reftype.md#regxsql)|"%"|(filter) Match typetag. ^=first encountered typetag|
|match|[algo.cstring](/txt/protocol/algo/cstring.md)|[Tary](/txt/exe/amc/reftype.md#tary)||(filter) Select input tuple if value of key matches value (regx:regx)|
|field|[algo.cstring](/txt/protocol/algo/cstring.md)|[Tary](/txt/exe/amc/reftype.md#tary)||(project) Select fields for output (regx)|
|format|u8|[Val](/txt/exe/amc/reftype.md#val)|0|Output format for selected tuples|
|t|bool|[Val](/txt/exe/amc/reftype.md#val)|false|Alias for -format:table|
|cmd|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|""|Command to output|
|f|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|""|Alias for -field:<f> -format:field|

#### command.strconv - A simple string utility
<a href="#command-strconv"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|str|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||String parameter|
|tocamelcase|bool|[Val](/txt/exe/amc/reftype.md#val)|false|Convert string to camel case|
|tolowerunder|bool|[Val](/txt/exe/amc/reftype.md#val)|false|Convert string to lower-under|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"data"|Input directory or filename, - for stdin|
|pathcomp|[algo.Smallstr100](/txt/protocol/algo/README.md#algo-smallstr100)|[Val](/txt/exe/amc/reftype.md#val)|""|Extract path component from string|

#### command.sv2ssim - sv2ssim - Separated Value file processor
<a href="#command-sv2ssim"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"data"|Input directory or filename, - for stdin|
|fname|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||Input file, use - for stdin|
|separator|char|[Val](/txt/exe/amc/reftype.md#val)|','|Input field separator|
|outseparator|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|""|Output separator. Default: ssim|
|header|bool|[Val](/txt/exe/amc/reftype.md#val)|true|File has header line|
|ctype|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|""|Type tag for output tuples|
|ssimfile|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|""|(with -schema) Create ssimfile definition|
|schema|bool|[Val](/txt/exe/amc/reftype.md#val)||(output)Generate schema from input file|
|field|[algo.cstring](/txt/protocol/algo/cstring.md)|[RegxSql](/txt/exe/amc/reftype.md#regxsql)|"%"|(output) Print selected fields|
|data|bool|[Val](/txt/exe/amc/reftype.md#val)||(output) Convert input file to ssim tuples|
|report|bool|[Val](/txt/exe/amc/reftype.md#val)|true|Print final report|
|prefer_signed|bool|[Val](/txt/exe/amc/reftype.md#val)||Prefer signed types when given a choice|

#### command.wt - Worktree manager - reset, run, diff, delete
<a href="#command-wt"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"data"|Input directory or filename, - for stdin|
|name|[dev.Sandbox](/txt/ssimdb/dev/sandbox.md)|[RegxSql](/txt/exe/amc/reftype.md#regxsql)||Sandbox name|
|create|bool|[Val](/txt/exe/amc/reftype.md#val)||Create new sandbox and register in dev.sandbox|
|b|bool|[Val](/txt/exe/amc/reftype.md#val)||Branch worktree: create on new branch NAME; git-registered, no dev.sandbox row|
|cow|bool|[Val](/txt/exe/amc/reftype.md#val)||Sandbox is a copy-on-write farm (with -create/anon name)|
|list|bool|[Val](/txt/exe/amc/reftype.md#val)||List existing sandboxes|
|reset|bool|[Val](/txt/exe/amc/reftype.md#val)||Reset sandbox to match current directory|
|claudesess|bool|[Val](/txt/exe/amc/reftype.md#val)||Start a background claude session named after the worktree|
|clean|bool|[Val](/txt/exe/amc/reftype.md#val)||Remove sandbox contents to save space|
|shell|bool|[Val](/txt/exe/amc/reftype.md#val)||Open interactive shell inside sandbox|
|del|bool|[Val](/txt/exe/amc/reftype.md#val)||Permanently delete sandbox|
|cmd|[algo.cstring](/txt/protocol/algo/cstring.md)|[Tary](/txt/exe/amc/reftype.md#tary)||Command to execute in sandbox|
|diff|bool|[Val](/txt/exe/amc/reftype.md#val)||Show diff after running command|
|files|[algo.cstring](/txt/protocol/algo/cstring.md)|[Tary](/txt/exe/amc/reftype.md#tary)||Shell regx to diff|
|ref|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|"HEAD"|Reset to this ref|
|q|bool|[Val](/txt/exe/amc/reftype.md#val)||Quiet mode|
|pull|bool|[Val](/txt/exe/amc/reftype.md#val)||Pull changes from sandbox to main repo|

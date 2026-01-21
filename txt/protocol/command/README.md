## command - Command line descriptions
<a href="#command"></a>
The command protocol describes command lines of all executables in the system.
Each command line is described by a struct named `command.<namespace>`.
These are attached to executables via [fcmdline](/txt/ssimdb/dmmeta/fcmdline.md)

### Ctypes
<a href="#ctypes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Ctypes -->
Other ctypes in this namespace which don't have own readme files

#### command.abt - Command line for [abt](/txt/exe/abt/README.md)
<a href="#command-abt"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|target|[dev.Target](/txt/ssimdb/dev/target.md)|[RegxSql](/txt/exe/amc/reftypes.md#regxsql)|""|Regx of target name|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"data"|Root of input ssim dir|
|cfg|[dev.Cfg](/txt/ssimdb/dev/cfg.md)|[RegxSql](/txt/exe/amc/reftypes.md#regxsql)|""|Set config|
|compiler|[dev.Compiler](/txt/ssimdb/dev/compiler.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|""|Set compiler.|
|uname|[dev.Uname](/txt/ssimdb/dev/uname.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|""|Set uname (default: guess)|
|arch|[dev.Arch](/txt/ssimdb/dev/arch.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|""|Set architecture (default: guess)|
|ood|bool|[Val](/txt/exe/amc/reftypes.md#val)||List out-of-date source files|
|list|bool|[Val](/txt/exe/amc/reftypes.md#val)||List target files|
|listincl|bool|[Val](/txt/exe/amc/reftypes.md#val)||List includes|
|build|bool|[Val](/txt/exe/amc/reftypes.md#val)||If set, build specified target (all necessary steps)|
|preproc|bool|[Val](/txt/exe/amc/reftypes.md#val)||Preprocess file, produce .i file|
|srcfile|[dev.Gitfile](/txt/ssimdb/dev/gitfile.md)|[RegxSql](/txt/exe/amc/reftypes.md#regxsql)|"%"|Build/disassemble/preprocess specific file|
|clean|bool|[Val](/txt/exe/amc/reftypes.md#val)||Delete all output files|
|dry_run|bool|[Val](/txt/exe/amc/reftypes.md#val)||Print actions, do not perform|
|maxjobs|i32|[Val](/txt/exe/amc/reftypes.md#val)|0|Maximum number of child build processes. 0=pick good default|
|printcmd|bool|[Val](/txt/exe/amc/reftypes.md#val)||Print commands. Do not execute|
|force|bool|[Val](/txt/exe/amc/reftypes.md#val)||Assume all files are out-of-date|
|install|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Update soft-link under bin/|
|coverity|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Run abt in coverity mode|
|package|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|Package tag|
|maxerr|u32|[Val](/txt/exe/amc/reftypes.md#val)|100|Max failing commands before rest of pipeline is forced to fail|
|disas|[algo.cstring](/txt/protocol/algo/cstring.md)|[RegxSql](/txt/exe/amc/reftypes.md#regxsql)|""|Regex of function to disassemble|
|report|bool|[Val](/txt/exe/amc/reftypes.md#val)|true|Print final report|
|jcdb|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|Create JSON compilation database in specified file|
|cache|u8|[Val](/txt/exe/amc/reftypes.md#val)||Cache mode|
|shortlink|bool|[Val](/txt/exe/amc/reftypes.md#val)||Try to shorten sort link if possible|

#### command.abt_md - Command line for [abt_md](/txt/exe/abt_md/README.md)
<a href="#command-abt_md"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"data"|Input directory or filename, - for stdin|
|readmefile|[dev.Readmefile](/txt/ssimdb/dev/readmefile.md)|[RegxSql](/txt/exe/amc/reftypes.md#regxsql)|"%"|Regx of readme to process/show (empty=all)|
|ns|[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)|[RegxSql](/txt/exe/amc/reftypes.md#regxsql)|""|(overrides -readme) Process readmes for this namespace|
|section|[algo.cstring](/txt/protocol/algo/cstring.md)|[RegxSql](/txt/exe/amc/reftypes.md#regxsql)|"%"|Select specific section to process|
|update|bool|[Val](/txt/exe/amc/reftypes.md#val)|true|(action) Update mode: Re-generate mdfiles|
|check|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|(action) Check mode: Check syntax and links|
|link|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|(with -print) Print links|
|anchor|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|(with -print) Print anchors|
|print|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|(action) Query mode: Print .md section without evaluating|
|dry_run|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Do not write changes to disk|
|external|bool|[Val](/txt/exe/amc/reftypes.md#val)||Check external links as well (may fail if no internet connection)|
|evalcmd|bool|[Val](/txt/exe/amc/reftypes.md#val)|true|Execute inline-commands|

#### command.acr - Command line for [acr](/txt/exe/acr/README.md)
<a href="#command-acr"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|query|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|Regx to match record|
|where|[algo.cstring](/txt/protocol/algo/cstring.md)|[Tary](/txt/exe/amc/reftypes.md#tary)||Additional key:value pairs to match|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"data"|Input directory or filename, - for stdin|
|del|bool|[Val](/txt/exe/amc/reftypes.md#val)||Delete found item|
|sel|bool|[Val](/txt/exe/amc/reftypes.md#val)||Read stdin and select records|
|insert|bool|[Val](/txt/exe/amc/reftypes.md#val)||Read stdin and insert tuples|
|replace|bool|[Val](/txt/exe/amc/reftypes.md#val)||Read stdin and replace tuples|
|update|bool|[Val](/txt/exe/amc/reftypes.md#val)||Read stdin and update attributes of existing tuples|
|merge|bool|[Val](/txt/exe/amc/reftypes.md#val)||Combination of -update and -insert|
|unused|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Only select records which are not referenced.|
|trunc|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Truncate table on first write|
|check|bool|[Val](/txt/exe/amc/reftypes.md#val)||Run cross-reference check on selection|
|selerr|bool|[Val](/txt/exe/amc/reftypes.md#val)|true|(with -check): Select error records|
|maxshow|i32|[Val](/txt/exe/amc/reftypes.md#val)|100|Limit number of errors per table|
|write|bool|[Val](/txt/exe/amc/reftypes.md#val)||Write data back to disk.|
|rename|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|Change value of found item|
|nup|i32|[Val](/txt/exe/amc/reftypes.md#val)|0|Number of levels to go up|
|ndown|i32|[Val](/txt/exe/amc/reftypes.md#val)|0|Number of levels to go down|
|l|bool|[Val](/txt/exe/amc/reftypes.md#val)||Go down via pkeys only|
|xref|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Short for -nup 100 -ndown 100|
|fldfunc|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Evaluate fldfunc when printing tuple|
|maxgroup|i32|[Val](/txt/exe/amc/reftypes.md#val)|25|Max. items per group|
|pretty|bool|[Val](/txt/exe/amc/reftypes.md#val)|true|Align output in blocks|
|tree|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Print as tree|
|loose|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Allow printing a record before its references (used with -e)|
|my|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Invoke acr_my -e (using acr_my directly is faster)|
|schema|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"data"|Directory for initializing acr meta-data|
|e|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Open selection in editor, write back when done.|
|t|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Short for -tree -xref -loose|
|g|bool|[Val](/txt/exe/amc/reftypes.md#val)||Trigger git commands for changes in dev.gitfile table|
|x|bool|[Val](/txt/exe/amc/reftypes.md#val)||Propagate select/rename/delete to ssimreq records|
|rowid|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Always print acr.rowid attribute|
|cmt|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Print comments for all columns referenced in output|
|report|bool|[Val](/txt/exe/amc/reftypes.md#val)|true|Show final report|
|print|bool|[Val](/txt/exe/amc/reftypes.md#val)|true|Print selected records|
|cmd|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|Print script with command execution for each selected row|
|field|[algo.cstring](/txt/protocol/algo/cstring.md)|[Tary](/txt/exe/amc/reftypes.md#tary)||Fields to select|
|regxof|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|Single field: output regx of matching field values|
|meta|bool|[Val](/txt/exe/amc/reftypes.md#val)||Select meta-data for selected records|

#### command.acr_compl - Command line for [acr_compl](/txt/exe/acr_compl/README.md)
<a href="#command-acr_compl"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|data|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"data"|Source for completions (dir or file or -)|
|schema|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"data"|Source for schema information|
|line|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|Simulates COMP_LINE (debug)|
|point|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|Simulates COMP_POINT (debug). default: whole line|
|type|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"9"|Simulates COMP_TYPE (debug)|
|install|bool|[Val](/txt/exe/amc/reftypes.md#val)||Produce bash commands to install the handler|
|debug_log|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|Log file for debug information, overrides ACR_COMPL_DEBUG_LOG|

#### command.acr_dm - Command line for [acr_dm](/txt/exe/acr_dm/README.md)
<a href="#command-acr_dm"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"data"|Input directory or filename, - for stdin|
|arg|[algo.cstring](/txt/protocol/algo/cstring.md)|[Tary](/txt/exe/amc/reftypes.md#tary)||Files to merge: older ours theirs...|
|write_ours|bool|[Val](/txt/exe/amc/reftypes.md#val)||Write result to ours file|
|msize|u8|[Val](/txt/exe/amc/reftypes.md#val)|7|Conflict marker size|
|rowid|bool|[Val](/txt/exe/amc/reftypes.md#val)||Output 'ours' rowid for merging into original ssimfiles|

#### command.acr_ed - Command line for [acr_ed](/txt/exe/acr_ed/README.md)
<a href="#command-acr_ed"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"data"|Input directory or filename, - for stdin|
|create|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Create new entity (-finput, -target, -ctype, -field)|
|del|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Delete mode|
|rename|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|Rename to something else|
|finput|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Create in-memory table based on ssimfile|
|foutput|bool|[Val](/txt/exe/amc/reftypes.md#val)||Declare field as an output|
|srcfile|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|Create/Rename/Delete a source file|
|gstatic|bool|[Val](/txt/exe/amc/reftypes.md#val)||Like -finput, but data is loaded at compile time|
|indexed|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|(with -finput) Add hash index|
|target|[dev.Target](/txt/ssimdb/dev/target.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|""|Create/Rename/Delete target|
|nstype|[dmmeta.Nstype](/txt/ssimdb/dmmeta/nstype.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|"exe"|(with -create -target): exe,lib,etc.|
|ctype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|""|Create/Rename/Delete ctype|
|pooltype|[dmmeta.Reftype](/txt/ssimdb/dmmeta/reftype.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|""| Pool reftype (Lary,Lpool etc) for finput/ctype|
|ssimfile|[dmmeta.Ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|""|  Ssimfile for new ctype|
|subset|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|""|  Primary key is a subset of this ctype|
|subset2|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|""|  Primary key is also a subset of this ctype|
|separator|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"."|    Key separator|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|""|Create field|
|arg|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|""|  Field type (e.g. u32, etc), (with -ctype) add the base field|
|dflt|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|  Field default value|
|anon|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|  Anonymous field (use with command lines)|
|bigend|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|  Big-endian field|
|cascdel|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|  Field is cascdel|
|before|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|""|  Place field before this one|
|substr|[dmmeta.Substr](/txt/ssimdb/dmmeta/substr.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|""|  New field is a substring|
|alias|bool|[Val](/txt/exe/amc/reftypes.md#val)||Create alias field (requires -srcfield)|
|srcfield|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|""|  Source field for bitfld/substr|
|fstep|[dmmeta.Fstep](/txt/ssimdb/dmmeta/fstep.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|""|  Add fstep record|
|inscond|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"true"|  Insert condition (for xref)|
|reftype|[dmmeta.Reftype](/txt/ssimdb/dmmeta/reftype.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|""|  Reftype (e.g. Val, Thash, Llist, etc)|
|hashfld|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|""|    (-reftype:Thash) Hash field|
|sortfld|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|""|    (-reftype:Bheap) Sort field|
|unittest|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|Create unit test, <ns>.<functionname>|
|citest|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|Create CI test|
|cppfunc|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|Field is a cppfunc, pass c++ expression as argument|
|xref|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|    X-ref with field type|
|via|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|      X-ref argument (index, pointer, or index/key)|
|write|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Commit output to disk|
|e|bool|[Val](/txt/exe/amc/reftypes.md#val)|false| (with -create -unittest) Edit new testcase|
|comment|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|Comment for new entity|
|sandbox|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Make changes in sandbox|
|test|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Build resulting changes, run tests|
|showcpp|bool|[Val](/txt/exe/amc/reftypes.md#val)||(With -sandbox), show resulting diff|
|msgtype|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|(with -ctype) use this msgtype as type|
|anonfld|bool|[Val](/txt/exe/amc/reftypes.md#val)||Create anonfld|
|amc|bool|[Val](/txt/exe/amc/reftypes.md#val)|true|Run amc if needed|

#### command.acr_in - Command line for [acr_in](/txt/exe/acr_in/README.md)
<a href="#command-acr_in"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ns|[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)|[RegxSql](/txt/exe/amc/reftypes.md#regxsql)|""|Regx of matching namespace|
|data|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|List ssimfile contents|
|sigcheck|bool|[Val](/txt/exe/amc/reftypes.md#val)|true|Output sigcheck records for schema version mismatch detection|
|list|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|List ssimfile names|
|t|bool|[Val](/txt/exe/amc/reftypes.md#val)||(with -list) Tree mode|
|data_dir|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"data"|Directory with ssimfiles|
|schema|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"data"||
|related|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|Select only tuples related to specified acr key|
|notssimfile|[dmmeta.Ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|[RegxSql](/txt/exe/amc/reftypes.md#regxsql)|""|Exclude ssimfiles matching regx|
|checkable|bool|[Val](/txt/exe/amc/reftypes.md#val)||Ensure output passes acr -check|
|r|[dmmeta.Ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|[RegxSql](/txt/exe/amc/reftypes.md#regxsql)|""|Reverse lookup of target by ssimfile|

#### command.acr_my - Command line for [acr_my](/txt/exe/acr_my/README.md)
<a href="#command-acr_my"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|nsdb|[dmmeta.Nsdb](/txt/ssimdb/dmmeta/nsdb.md)|[RegxSql](/txt/exe/amc/reftypes.md#regxsql)|""|Regx of ssim namespace (dmmeta.nsdb) to select|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"data"|Input directory or filename, - for stdin|
|schema|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"data"|Input directory or filename, - for stdin|
|fldfunc|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Evaluate fldfunc when printing tuple|
|fkey|bool|[Val](/txt/exe/amc/reftypes.md#val)||Enable foreign key constraints|
|e|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Alias for -start -shell -stop|
|start|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Start local mysql server|
|stop|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Stop local mysql server, saving data|
|abort|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Abort local mysql server, losing data|
|shell|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Connect to local mysql server|
|serv|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Start mysql with TCP/IP service enabled|

#### command.amc - Command line for [amc](/txt/exe/amc/README.md)
<a href="#command-amc"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in_dir|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"data"|Root of input ssim dir|
|query|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|Query mode: generate code for specified object|
|out_dir|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"."|Root of output cpp dir|
|proto|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Print prototype|
|showcomment|bool|[Val](/txt/exe/amc/reftypes.md#val)|true|Show generated comments|
|report|bool|[Val](/txt/exe/amc/reftypes.md#val)|true|Final report|
|e|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Open matching records in editor|
|trace|[algo.cstring](/txt/protocol/algo/cstring.md)|[RegxSql](/txt/exe/amc/reftypes.md#regxsql)|""|Regx of something to trace code generation|

#### command.amc_gc - Command line for [amc_gc](/txt/exe/amc_gc/README.md)
<a href="#command-amc_gc"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|target|[dev.Target](/txt/ssimdb/dev/target.md)|[RegxSql](/txt/exe/amc/reftypes.md#regxsql)|"%"|Target to test-build|
|key|[algo.cstring](/txt/protocol/algo/cstring.md)|[RegxSql](/txt/exe/amc/reftypes.md#regxsql)|""|ACR query selecting records to eliminate, e.g. dmmeta.ctype:amc.%|
|include|bool|[Val](/txt/exe/amc/reftypes.md#val)||Garbage collect includes for specified target|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"data"|Input directory or filename, - for stdin|

#### command.amc_vis - Command line for [amc_vis](/txt/exe/amc_vis/README.md)
<a href="#command-amc_vis"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ctype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[RegxSql](/txt/exe/amc/reftypes.md#regxsql)|"%"|Ctype regexp to compute access path diagram|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"data"|Input directory or filename, - for stdin|
|dot|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|Save dot file with specified filename|
|xref|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Include all ctypes referenced by selected ones|
|xns|bool|[Val](/txt/exe/amc/reftypes.md#val)|true|Cross namespace boundaries|
|noinput|bool|[Val](/txt/exe/amc/reftypes.md#val)||Deselect module inputs|
|check|bool|[Val](/txt/exe/amc/reftypes.md#val)||Check model for dependency problems|
|render|bool|[Val](/txt/exe/amc/reftypes.md#val)|true|Produce an ascii drawing|

#### command.ams_sendtest - Command line for [ams_sendtest](/txt/exe/ams_sendtest/README.md)
<a href="#command-ams_sendtest"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"data"|Input directory or filename, - for stdin|
|id|i32|[Val](/txt/exe/amc/reftypes.md#val)||Process index (0=parent)|
|file_prefix|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|Use file_prefix|
|nchild|i32|[Val](/txt/exe/amc/reftypes.md#val)|1|Number of stream readers|
|blocking|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Use blocking send mode|
|nmsg|i32|[Val](/txt/exe/amc/reftypes.md#val)|1000|Number of messages to send/receive|
|trace|[algo.cstring](/txt/protocol/algo/cstring.md)|[RegxSql](/txt/exe/amc/reftypes.md#regxsql)|""|Regx expression specifying what to trace|
|timeout|i32|[Val](/txt/exe/amc/reftypes.md#val)|30|Time limit for the send|
|recvdelay_ns|i64|[Val](/txt/exe/amc/reftypes.md#val)||Pause nanoseconds between messages|
|senddelay_ns|i64|[Val](/txt/exe/amc/reftypes.md#val)||Pause nanoseconds between messages|
|msgsize_min|i32|[Val](/txt/exe/amc/reftypes.md#val)|64|Minimum message length|
|msgsize_max|i32|[Val](/txt/exe/amc/reftypes.md#val)|1024|Maximum message length|
|bufsize|i32|[Val](/txt/exe/amc/reftypes.md#val)|32768|Shared memory buffer size|
|recvdelay|i64|[Val](/txt/exe/amc/reftypes.md#val)||Pause nanoseconds between messages|

#### command.apm - Command line for [apm](/txt/exe/apm/README.md)
<a href="#command-apm"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"data"|Input directory or filename, - for stdin|
|pkgdata|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|Load package definitions from here|
|package|[dev.Package](/txt/ssimdb/dev/package.md)|[RegxSql](/txt/exe/amc/reftypes.md#regxsql)|""|Regx of package|
|ns|[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)|[RegxSql](/txt/exe/amc/reftypes.md#regxsql)|""|Operate on specified namespace only|
|install|bool|[Val](/txt/exe/amc/reftypes.md#val)||Install new package (specify -origin)|
|update|bool|[Val](/txt/exe/amc/reftypes.md#val)||Update package (-origin)|
|list|bool|[Val](/txt/exe/amc/reftypes.md#val)||List installed packages|
|diff|bool|[Val](/txt/exe/amc/reftypes.md#val)||Diff package with respect to installed version|
|push|bool|[Val](/txt/exe/amc/reftypes.md#val)||Evaluate package diff and push it to origin|
|check|bool|[Val](/txt/exe/amc/reftypes.md#val)||Consistency check|
|remove|bool|[Val](/txt/exe/amc/reftypes.md#val)||Remove specified package|
|origin|[algo.Smallstr200](/txt/protocol/algo/README.md#algo-smallstr200)|[Val](/txt/exe/amc/reftypes.md#val)|""|Upstream URL of new package|
|ref|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)|""|(with -create) Gitref or branch to fetch|
|dry_run|bool|[Val](/txt/exe/amc/reftypes.md#val)||Do not execute transaction|
|showrec|bool|[Val](/txt/exe/amc/reftypes.md#val)||Show records belonging to package|
|showfile|bool|[Val](/txt/exe/amc/reftypes.md#val)||List package files (gitfile records)|
|R|bool|[Val](/txt/exe/amc/reftypes.md#val)||reverse the diff direction|
|l|bool|[Val](/txt/exe/amc/reftypes.md#val)||Use local package definition on the remote side|
|reset|bool|[Val](/txt/exe/amc/reftypes.md#val)||Reset package baseref/origin to those provided by the command line|
|checkclean|bool|[Val](/txt/exe/amc/reftypes.md#val)|true|Ensure that changes are applied to a clean directory|
|t|bool|[Val](/txt/exe/amc/reftypes.md#val)||Select parent packages for operation|
|stat|bool|[Val](/txt/exe/amc/reftypes.md#val)||(with -diff) show stats|
|annotate|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|Read file and annotate each input tuple with package(s) it belongs to|
|data_in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"data"|Dataset from which package records are loaded|
|e|bool|[Val](/txt/exe/amc/reftypes.md#val)||Open selected records in editor|
|binpath|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"bin"|(internal use)|

#### command.aqlite - Command line for [aqlite](/txt/exe/aqlite/README.md)
<a href="#command-aqlite"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"data"|Input directory or filename, - for stdin|
|schema|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"data"|Schema dir|
|cmd|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Sql Query to run|
|ns|[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)|[RegxSql](/txt/exe/amc/reftypes.md#regxsql)|"dmmeta"|Regx of databases to attach|

#### command.atf_amc - Command line for [atf_amc](/txt/exe/atf_amc/README.md)
<a href="#command-atf_amc"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"data"|Input directory or filename, - for stdin|
|amctest|[atfdb.Amctest](/txt/ssimdb/atfdb/amctest.md)|[RegxSql](/txt/exe/amc/reftypes.md#regxsql)|"%"|SQL regex, selecting test to run|
|dofork|bool|[Val](/txt/exe/amc/reftypes.md#val)|true|Use fork|
|q|bool|[Val](/txt/exe/amc/reftypes.md#val)||Quiet mode|

#### command.atf_ci - Command line for [atf_ci](/txt/exe/atf_ci/README.md)
<a href="#command-atf_ci"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"data"|Input directory or filename, - for stdin|
|citest|[atfdb.Citest](/txt/ssimdb/atfdb/citest.md)|[RegxSql](/txt/exe/amc/reftypes.md#regxsql)|"%"|Regx of tests to run|
|maxerr|i32|[Val](/txt/exe/amc/reftypes.md#val)|0|Exit after this many errors|
|cijob|[atfdb.Cijob](/txt/ssimdb/atfdb/cijob.md)|[RegxSql](/txt/exe/amc/reftypes.md#regxsql)|"%"||
|capture|bool|[Val](/txt/exe/amc/reftypes.md#val)||Capture the output of the test|
|check_clean|bool|[Val](/txt/exe/amc/reftypes.md#val)|true|Check for modifications after each test|

#### command.atf_cmdline - Command line for [atf_cmdline](/txt/exe/atf_cmdline/README.md)
<a href="#command-atf_cmdline"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"data"|Input directory or filename, - for stdin|
|exec|bool|[Val](/txt/exe/amc/reftypes.md#val)||Execv itself|
|astr|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Required anon string|
|anum|i32|[Val](/txt/exe/amc/reftypes.md#val)||Anon number|
|adbl|double|[Val](/txt/exe/amc/reftypes.md#val)||Anon double|
|aflag|bool|[Val](/txt/exe/amc/reftypes.md#val)||Anon flag|
|str|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Required string|
|num|i32|[Val](/txt/exe/amc/reftypes.md#val)||Required Number|
|dbl|double|[Val](/txt/exe/amc/reftypes.md#val)||Required double|
|flag|bool|[Val](/txt/exe/amc/reftypes.md#val)||Required flag|
|dstr|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"blah"|Predefined string|
|dnum|i32|[Val](/txt/exe/amc/reftypes.md#val)|-33|Predefined number|
|ddbl|double|[Val](/txt/exe/amc/reftypes.md#val)|0.0001|Predefined double|
|dflag|bool|[Val](/txt/exe/amc/reftypes.md#val)|true|Predefined flag|
|mstr|[algo.cstring](/txt/protocol/algo/cstring.md)|[Tary](/txt/exe/amc/reftypes.md#tary)||String array|
|mnum|i32|[Tary](/txt/exe/amc/reftypes.md#tary)||Number array|
|mdbl|double|[Tary](/txt/exe/amc/reftypes.md#tary)||Double array|
|amnum|i32|[Tary](/txt/exe/amc/reftypes.md#tary)||Anon number array|
|fconst|u8|[Val](/txt/exe/amc/reftypes.md#val)||Fconst for field|
|cconst|[algo.Month](/txt/protocol/algo/Month.md)|[Val](/txt/exe/amc/reftypes.md#val)||Fconst for arg ctype|
|dregx|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[RegxSql](/txt/exe/amc/reftypes.md#regxsql)|"%"|Predefined regx|
|dpkey|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|""|Predefined pkey|

#### command.atf_comp - Command line for [atf_comp](/txt/exe/atf_comp/README.md)
<a href="#command-atf_comp"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"data"|Input directory or filename, - for stdin|
|comptest|[atfdb.Comptest](/txt/ssimdb/atfdb/comptest.md)|[RegxSql](/txt/exe/amc/reftypes.md#regxsql)|"%"|Select comptest (SQL regex)|
|mdbg|bool|[Val](/txt/exe/amc/reftypes.md#val)||(action) Run component test under debugger|
|run|bool|[Val](/txt/exe/amc/reftypes.md#val)|true|(action) Run selected component tests|
|capture|bool|[Val](/txt/exe/amc/reftypes.md#val)||(action) Re-capture test results|
|print|bool|[Val](/txt/exe/amc/reftypes.md#val)||(action) Print testcase|
|printinput|bool|[Val](/txt/exe/amc/reftypes.md#val)||(action) Print input of test case|
|e|bool|[Val](/txt/exe/amc/reftypes.md#val)||(action) Open selected testcases in an editor|
|normalize|bool|[Val](/txt/exe/amc/reftypes.md#val)||(action) Renumber and normalize tmsgs|
|covcapture|bool|[Val](/txt/exe/amc/reftypes.md#val)||(action) Capture new coverage percentages and save back|
|covcheck|bool|[Val](/txt/exe/amc/reftypes.md#val)||(action) Check coverage percentages against tgtcov table|
|bindir|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|Directory with binaries (default: build/cfg)|
|cfg|[dev.Cfg](/txt/ssimdb/dev/cfg.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|"release"|Set config|
|check_untracked|bool|[Val](/txt/exe/amc/reftypes.md#val)|true|Check for untracked file before allowing test to run|
|maxerr|i32|[Val](/txt/exe/amc/reftypes.md#val)|3|Exit after this many errors|
|build|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Build given cfg before test|
|memcheck|bool|[Val](/txt/exe/amc/reftypes.md#val)||Run under memory checker (valgrind)|
|force|bool|[Val](/txt/exe/amc/reftypes.md#val)||(With -memcheck) run suppressed memcheck|
|callgrind|bool|[Val](/txt/exe/amc/reftypes.md#val)||Run under callgrind profiler (valgrind)|
|maxjobs|i32|[Val](/txt/exe/amc/reftypes.md#val)|0|Maximum number of tests run in parallel|
|stream|bool|[Val](/txt/exe/amc/reftypes.md#val)||prints component's output|
|i|bool|[Val](/txt/exe/amc/reftypes.md#val)||Read and execute testcase from stdin|
|write|bool|[Val](/txt/exe/amc/reftypes.md#val)|true|(implied with -e) Write any changes back to ssim tables|
|report|bool|[Val](/txt/exe/amc/reftypes.md#val)|true|Print final report|
|b|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|Breakpoint passed to mdbg as-is|
|covfast|bool|[Val](/txt/exe/amc/reftypes.md#val)|true|Speedup coverage processing|
|minrepeat|i32|[Val](/txt/exe/amc/reftypes.md#val)|0|Execute every comptest at least this many times|
|maxrepeat|i32|[Val](/txt/exe/amc/reftypes.md#val)|1|Don't repeat any individual comptest more than X times|

#### command.atf_cov - Command line for [atf_cov](/txt/exe/atf_cov/README.md)
<a href="#command-atf_cov"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"data"|Input directory or filename, - for stdin|
|covdir|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"temp/covdata"|Output directory to save coverage data|
|logfile|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|Log file|
|runcmd|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|command to run|
|exclude|[dev.Gitfile](/txt/ssimdb/dev/gitfile.md)|[RegxSql](/txt/exe/amc/reftypes.md#regxsql)|"(extern|include/gen|cpp/gen)/%"|Exclude gitfiles (external, generated)|
|mergepath|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|colon-separated dir list to load .cov.ssim files from|
|gcov|bool|[Val](/txt/exe/amc/reftypes.md#val)||run gcov|
|ssim|bool|[Val](/txt/exe/amc/reftypes.md#val)||write out ssim files|
|report|bool|[Val](/txt/exe/amc/reftypes.md#val)||write out all reports|
|capture|bool|[Val](/txt/exe/amc/reftypes.md#val)||Write coverage information into tgtcov table|
|xmlpretty|bool|[Val](/txt/exe/amc/reftypes.md#val)||Generate pretty-formatted XML|
|summary|bool|[Val](/txt/exe/amc/reftypes.md#val)|true|Show summary figures|
|check|bool|[Val](/txt/exe/amc/reftypes.md#val)||Check coverage information against tgtcov table|
|incremental|bool|[Val](/txt/exe/amc/reftypes.md#val)||Keep *.gcda files from previous run|

#### command.atf_exp - Command line for [atf_exp](/txt/exe/atf_exp/README.md)
<a href="#command-atf_exp"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"data"|Input directory or filename, - for stdin|
|trace|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|Regx of things to trace|
|bindir|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"bin"|Directory with binaries|
|comptest|[atfdb.Comptest](/txt/ssimdb/atfdb/comptest.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|""|Component test name|
|tempdir|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|Temporary directory for the component test|
|timeout|double|[Val](/txt/exe/amc/reftypes.md#val)|8.0|Timeout in seconds for a step|
|dbgshell|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Invoke bash with curr envs in case of timeout|

#### command.atf_fuzz - Command line for [atf_fuzz](/txt/exe/atf_fuzz/README.md)
<a href="#command-atf_fuzz"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|reprofile|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"temp/atf_fuzz.repro"|File where repros are stored|
|target|[dev.Target](/txt/ssimdb/dev/target.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|""|Target to fuzz|
|args|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|Additional arguments to target|
|inputfile|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|File with input tuples.|
|fuzzstrat|[atfdb.Fuzzstrat](/txt/ssimdb/atfdb/fuzzstrat.md)|[RegxSql](/txt/exe/amc/reftypes.md#regxsql)|"%"|Strategy to choose|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"data"|Input directory or filename, - for stdin|
|seed|i32|[Val](/txt/exe/amc/reftypes.md#val)|0|Random seed|
|testprob|double|[Val](/txt/exe/amc/reftypes.md#val)|1|Run each case with this probability|

#### command.atf_gcli - Command line for [atf_gcli](/txt/exe/atf_gcli/README.md)
<a href="#command-atf_gcli"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"data"|Input directory or filename, - for stdin|
|gtblacttst|[gclidb.Gtblacttst](/txt/ssimdb/gclidb/gtblacttst.md)|[RegxSql](/txt/exe/amc/reftypes.md#regxsql)|"%"|gcli tests regx|
|id|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|Use this issue for $ISSUE substitutions|
|mr|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|Use this issue for $MR substitutions|
|note|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|Use this issue for $NOTE substitutions|
|capture|bool|[Val](/txt/exe/amc/reftypes.md#val)||Capture test command output into gverbtout|
|skip_init|bool|[Val](/txt/exe/amc/reftypes.md#val)||Skip setting local files - already set|
|skip_git_init|bool|[Val](/txt/exe/amc/reftypes.md#val)||Skip setting local files - already set|
|dry_run|bool|[Val](/txt/exe/amc/reftypes.md#val)||Print actions, do not perform|

#### command.atf_nrun - Command line for [atf_nrun](/txt/exe/atf_nrun/README.md)
<a href="#command-atf_nrun"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"data"|Input directory or filename, - for stdin|
|maxjobs|i32|[Val](/txt/exe/amc/reftypes.md#val)|2|Number of simultaneous jobs|
|ncmd|i32|[Val](/txt/exe/amc/reftypes.md#val)|6||

#### command.atf_unit - Command line for [atf_unit](/txt/exe/atf_unit/README.md)
<a href="#command-atf_unit"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|unittest|[atfdb.Unittest](/txt/ssimdb/atfdb/unittest.md)|[RegxSql](/txt/exe/amc/reftypes.md#regxsql)|"%"|SQL regex, selecting test to run|
|nofork|bool|[Val](/txt/exe/amc/reftypes.md#val)||Do not fork for destructive tests|
|arg|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|Argument to pass to tool|
|data_dir|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"data"|Data directory|
|mdbg|bool|[Val](/txt/exe/amc/reftypes.md#val)|0|Break at testcase in debugger|
|perf_secs|double|[Val](/txt/exe/amc/reftypes.md#val)|1.0|# Of seconds to run perf tests for|
|pertest_timeout|u32|[Val](/txt/exe/amc/reftypes.md#val)|900|Max runtime of any individual unit test|
|report|bool|[Val](/txt/exe/amc/reftypes.md#val)|true|Print final report|
|capture|bool|[Val](/txt/exe/amc/reftypes.md#val)||Re-capture test results|
|check_untracked|bool|[Val](/txt/exe/amc/reftypes.md#val)|true|Check for untracked file before allowing test to run|

#### command.bash - 
<a href="#command-bash"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|c|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|Shell command to execute|

#### command.bash2html - Command line for [bash2html](/txt/exe/bash2html/README.md)
<a href="#command-bash2html"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"data"|Input directory or filename, - for stdin|
|test|bool|[Val](/txt/exe/amc/reftypes.md#val)||Produce Test Output|

#### command.bash_proc - 
<a href="#command-bash_proc"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|bash|[command.bash](/txt/protocol/command/README.md#command-bash)|[Exec](/txt/exe/amc/reftypes.md#exec)|"bash"|Must be bash to support $'' for string quoting|

#### command.gcache - Command line for [gcache](/txt/exe/gcache/README.md)
<a href="#command-gcache"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"data"|Input directory or filename, - for stdin|
|cmd|[algo.cstring](/txt/protocol/algo/cstring.md)|[Tary](/txt/exe/amc/reftypes.md#tary)||Command to execute|
|install|bool|[Val](/txt/exe/amc/reftypes.md#val)||Create gcache directory and enable gcache|
|stats|bool|[Val](/txt/exe/amc/reftypes.md#val)||Show cache stats|
|enable|bool|[Val](/txt/exe/amc/reftypes.md#val)||Create .gcache link to enable gcache use|
|disable|bool|[Val](/txt/exe/amc/reftypes.md#val)||Remove .gcache link to disable gcache|
|gc|bool|[Val](/txt/exe/amc/reftypes.md#val)||Clean old files from .gcache|
|clean|bool|[Val](/txt/exe/amc/reftypes.md#val)||Clean the entire cache|
|dir|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"/tmp/gcache"|(With -install,-enable) cache directory|
|hitrate|bool|[Val](/txt/exe/amc/reftypes.md#val)||Report hit rate (specify start time with -after)|
|after|[algo.UnTime](/txt/protocol/algo/UnTime.md)|[Val](/txt/exe/amc/reftypes.md#val)||Start time for reporting|
|report|bool|[Val](/txt/exe/amc/reftypes.md#val)||Show end-of-run report|
|force|bool|[Val](/txt/exe/amc/reftypes.md#val)||Force recompile and update cache|

#### command.gcli - Command line for [gcli](/txt/exe/gcli/README.md)
<a href="#command-gcli"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"data"|Input directory or filename, - for stdin|
|selector|[gclidb.Gtbl](/txt/ssimdb/gclidb/gtbl.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|"issue:%"|table:key, where table is issue,repo,mr, etc. and key is a regex.|
|fields|[algo.cstring](/txt/protocol/algo/cstring.md)|[Tary](/txt/exe/amc/reftypes.md#tary)|""|additional key:value pairs for use with -create, -list, -update|
|accept|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|(action) accept a selected merge request|
|start|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|(action) start working on a selected issue|
|list|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|(action) show selected table|
|create|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|(action) create a table record(s)|
|update|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|(action) update fields of a selected issue or mergereq|
|approve|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|(action) remove draft desiognation from mergereq|
|needs_work|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|(action) reopen an issue or put a draft designation on mergereq|
|stop|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|(action) closes an issue, or remove mr branch after review|
|t|bool|[Val](/txt/exe/amc/reftypes.md#val)||Tree view: expand issue description|
|e|bool|[Val](/txt/exe/amc/reftypes.md#val)||edit the input|
|authdir|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|".ssim"|(setup) Input directory for auth data|
|dry_run|bool|[Val](/txt/exe/amc/reftypes.md#val)||Print actions, do not perform|
|gitdir|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|(setup) Change directory of git repository|
|show_gitlab_system_notes|bool|[Val](/txt/exe/amc/reftypes.md#val)||(misc) Show issue and mr notes created by gitlab|

#### command.generic - Any generic subprocess
<a href="#command-generic"></a>

#### command.generic_proc - 
<a href="#command-generic_proc"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|name|[command.generic](/txt/protocol/command/README.md#command-generic)|[Exec](/txt/exe/amc/reftypes.md#exec)|""||

#### command.jkv - Command line for [jkv](/txt/exe/jkv/README.md)
<a href="#command-jkv"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"data"|Input directory or filename, - for stdin|
|file|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Filename (use - for stdin)|
|kv|[algo.cstring](/txt/protocol/algo/cstring.md)|[Tary](/txt/exe/amc/reftypes.md#tary)||JSON Keyvals|
|r|bool|[Val](/txt/exe/amc/reftypes.md#val)||Reverse (json keyvals -> JSON) mapping|
|write|bool|[Val](/txt/exe/amc/reftypes.md#val)||Write the modified file back|
|output|u8|[Val](/txt/exe/amc/reftypes.md#val)|0|Output format|
|pretty|u32|[Val](/txt/exe/amc/reftypes.md#val)|2|Pretty-printer (0-compact; 1=algo style; 2=jq style)|

#### command.mdbg - Command line for [mdbg](/txt/exe/mdbg/README.md)
<a href="#command-mdbg"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|target|[dev.Target](/txt/ssimdb/dev/target.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)||Executable name|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"data"|Input directory or filename, - for stdin|
|args|[algo.cstring](/txt/protocol/algo/cstring.md)|[Tary](/txt/exe/amc/reftypes.md#tary)|""|Additional module args|
|cfg|[dev.Cfg](/txt/ssimdb/dev/cfg.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|"debug"|Configuration to use|
|disas|bool|[Val](/txt/exe/amc/reftypes.md#val)||Show disassembly (use F12)|
|attach|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Attach to a running process|
|pid|i32|[Val](/txt/exe/amc/reftypes.md#val)|0|(with -attach) Pid, if omitted mdbg will guess|
|b|[algo.cstring](/txt/protocol/algo/cstring.md)|[Tary](/txt/exe/amc/reftypes.md#tary)|""|Set breakpoint, e.g. 'a.cpp:123 if cond1', 'func#3'|
|catchthrow|bool|[Val](/txt/exe/amc/reftypes.md#val)|true|Stop on exceptions|
|tui|bool|[Val](/txt/exe/amc/reftypes.md#val)||Use gdb -tui as the debugger|
|bcmd|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|Evaluate command at breakpoint|
|emacs|bool|[Val](/txt/exe/amc/reftypes.md#val)|true|Use emacs environment as the debugger|
|manywin|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Use gdb-many-windows emacs mode|
|follow_child|bool|[Val](/txt/exe/amc/reftypes.md#val)||When forking, follow child (default is parent)|
|py|bool|[Val](/txt/exe/amc/reftypes.md#val)||Enable python scripting|
|dry_run|bool|[Val](/txt/exe/amc/reftypes.md#val)||Print commands but don't execute|
|mp|bool|[Val](/txt/exe/amc/reftypes.md#val)||Multi-process debugging|

#### command.mysql2ssim - Command line for [mysql2ssim](/txt/exe/mysql2ssim/README.md)
<a href="#command-mysql2ssim"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|writessimfile|bool|[Val](/txt/exe/amc/reftypes.md#val)||Write to ssimfile directly|
|url|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||user:pass@host/db or sock:///filename/db|
|tables|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|comma-separated list of tables. Default is all tables|
|schema|bool|[Val](/txt/exe/amc/reftypes.md#val)||Generate ssim type definition|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"data"|Input directory or filename, - for stdin|
|pretty|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Format output for the screen|
|nologo|bool|[Val](/txt/exe/amc/reftypes.md#val)||Don't show copyright notice|
|baddbok|bool|[Val](/txt/exe/amc/reftypes.md#val)||Don't claim if bad database|

#### command.orgfile - Command line for [orgfile](/txt/exe/orgfile/README.md)
<a href="#command-orgfile"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"data"|Input directory or filename, - for stdin|
|move|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|Read stdin, rename files based on pattern|
|dedup|[algo.cstring](/txt/protocol/algo/cstring.md)|[RegxSql](/txt/exe/amc/reftypes.md#regxsql)|""|Only allow deleting files that match this regx|
|commit|bool|[Val](/txt/exe/amc/reftypes.md#val)||Apply changes|
|undo|bool|[Val](/txt/exe/amc/reftypes.md#val)||Read previous orgfile output, undoing movement|
|hash|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"sha1"|Hash command to use for deduplication|

#### command.samp_make - Command line for [samp_make](/txt/exe/samp_make/README.md)
<a href="#command-samp_make"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"data"|Input directory or filename, - for stdin|
|target|[sampdb.Target](/txt/ssimdb/sampdb/target.md)|[RegxSql](/txt/exe/amc/reftypes.md#regxsql)|"%"|Create Makefile for selected targets|
|parse_make|bool|[Val](/txt/exe/amc/reftypes.md#val)||Parse extern/gnumake/Simple-Makefile|
|makefile|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"extern/gnumake/Simple-Makefile"|(with parse_make) makefile to parse|
|write|bool|[Val](/txt/exe/amc/reftypes.md#val)||P(with parse_make) write ssimfiles, otherwise print them|

#### command.samp_meng - Command line for [samp_meng](/txt/exe/samp_meng/README.md)
<a href="#command-samp_meng"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"data"|Input directory or filename, - for stdin|

#### command.samp_regx - Command line for [samp_regx](/txt/exe/samp_regx/README.md)
<a href="#command-samp_regx"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"data"|Input directory or filename, - for stdin|
|expr|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Expression|
|style|[algo_lib.RegxStyle](/txt/lib/algo_lib/README.md#algo_lib-regxstyle)|[Val](/txt/exe/amc/reftypes.md#val)|2|Regx style|
|trace|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Trace regx innards|
|capture|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Use capture groups|
|full|bool|[Val](/txt/exe/amc/reftypes.md#val)|true|Match full string|
|f|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|<string> is a filename, grep the lines|
|match|bool|[Val](/txt/exe/amc/reftypes.md#val)||Match a string, exit code represnts success|
|string|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|String to match|

#### command.sandbox - Command line for [sandbox](/txt/exe/sandbox/README.md)
<a href="#command-sandbox"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"data"|Input directory or filename, - for stdin|
|name|[dev.Sandbox](/txt/ssimdb/dev/sandbox.md)|[RegxSql](/txt/exe/amc/reftypes.md#regxsql)||Sandbox name|
|create|bool|[Val](/txt/exe/amc/reftypes.md#val)||Create new sandbox and register in dev.sandbox|
|list|bool|[Val](/txt/exe/amc/reftypes.md#val)||List existing sandboxes|
|reset|bool|[Val](/txt/exe/amc/reftypes.md#val)||Reset sandbox to match current directory|
|clean|bool|[Val](/txt/exe/amc/reftypes.md#val)||Remove sandbox contents to save space|
|shell|bool|[Val](/txt/exe/amc/reftypes.md#val)||Open interactive shell inside sandbox|
|del|bool|[Val](/txt/exe/amc/reftypes.md#val)||Permanently delete sandbox|
|gc|bool|[Val](/txt/exe/amc/reftypes.md#val)||Run git gc in target dir|
|cmd|[algo.cstring](/txt/protocol/algo/cstring.md)|[Tary](/txt/exe/amc/reftypes.md#tary)||Command to execute in sandbox|
|diff|bool|[Val](/txt/exe/amc/reftypes.md#val)||Show diff after running command|
|files|[algo.cstring](/txt/protocol/algo/cstring.md)|[Tary](/txt/exe/amc/reftypes.md#tary)||Shell regx to diff|
|refs|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"HEAD"|Additional list of refs to fetch into sandbox|
|ref|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"HEAD"|Reset to this ref|
|q|bool|[Val](/txt/exe/amc/reftypes.md#val)||Quiet mode|

#### command.src_func - Command line for [src_func](/txt/exe/src_func/README.md)
<a href="#command-src_func"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"data"|Input directory or filename, - for stdin|
|targsrc|[dev.Targsrc](/txt/ssimdb/dev/targsrc.md)|[RegxSql](/txt/exe/amc/reftypes.md#regxsql)|"%"|(scan) Limit scanning to these sources only|
|acrkey|[algo.cstring](/txt/protocol/algo/cstring.md)|[RegxSql](/txt/exe/amc/reftypes.md#regxsql)|"%"|Select function by acr key that caused it|
|func|[dev.Target](/txt/ssimdb/dev/target.md)|[RegxSql](/txt/exe/amc/reftypes.md#regxsql)|"%"|Target.function regex|
|nextfile|[dev.Gitfile](/txt/ssimdb/dev/gitfile.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|""|(action) Print name of next srcfile in targsrc list|
|other|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|    (with -nextfile), name of previous file|
|list|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|(action) List matching functions|
|updateproto|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|(action) Update prototypes in headers|
|createmissing|bool|[Val](/txt/exe/amc/reftypes.md#val)||(action) Create missing functions declared in userfunc table|
|iffy|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|(filter) Select functions that may contain errors|
|gen|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|(scan) Scan generated files|
|showloc|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|(output) Show file location|
|f|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|(output) -sortname -showcomment -showbody|
|showstatic|bool|[Val](/txt/exe/amc/reftypes.md#val)|true|(filter) Allow static functions|
|matchproto|[algo.cstring](/txt/protocol/algo/cstring.md)|[RegxSql](/txt/exe/amc/reftypes.md#regxsql)|"%"|(filter) Match function prototype|
|matchbody|[algo.cstring](/txt/protocol/algo/cstring.md)|[RegxSql](/txt/exe/amc/reftypes.md#regxsql)|"%"|(filter) Match function body|
|matchcomment|[algo.cstring](/txt/protocol/algo/cstring.md)|[RegxSql](/txt/exe/amc/reftypes.md#regxsql)|"%"|(filter) Match function comment|
|showsortkey|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|(output) Display function sortkey|
|showcomment|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|(output) Display function comment|
|showbody|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|(output) Print function body|
|sortname|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|(output) Sort functions by name|
|printssim|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|(output) Print ssim tuples|
|e|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Edit found functions|
|baddecl|bool|[Val](/txt/exe/amc/reftypes.md#val)||Report and fail on bad declarations|
|report|bool|[Val](/txt/exe/amc/reftypes.md#val)||Print final report|

#### command.src_hdr - Command line for [src_hdr](/txt/exe/src_hdr/README.md)
<a href="#command-src_hdr"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"data"|Input directory or filename, - for stdin|
|targsrc|[dev.Targsrc](/txt/ssimdb/dev/targsrc.md)|[RegxSql](/txt/exe/amc/reftypes.md#regxsql)|""|Regx of targsrc to update|
|write|bool|[Val](/txt/exe/amc/reftypes.md#val)||Update files in-place|
|indent|bool|[Val](/txt/exe/amc/reftypes.md#val)||Indent source files|
|update_copyright|bool|[Val](/txt/exe/amc/reftypes.md#val)||Update copyright year for current company|
|scriptfile|[dev.Scriptfile](/txt/ssimdb/dev/scriptfile.md)|[RegxSql](/txt/exe/amc/reftypes.md#regxsql)|""|Regx of scripts to update header|

#### command.src_lim - Command line for [src_lim](/txt/exe/src_lim/README.md)
<a href="#command-src_lim"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"data"|Input directory or filename, - for stdin|
|linelim|bool|[Val](/txt/exe/amc/reftypes.md#val)||Check various file limits (line length, function length, etc)|
|srcfile|[dev.Gitfile](/txt/ssimdb/dev/gitfile.md)|[RegxSql](/txt/exe/amc/reftypes.md#regxsql)|"%"|Filter for source files to process|
|strayfile|bool|[Val](/txt/exe/amc/reftypes.md#val)||Check for unregistered source files|
|capture|bool|[Val](/txt/exe/amc/reftypes.md#val)||Generate new dev.linelim records|
|write|bool|[Val](/txt/exe/amc/reftypes.md#val)||Update ssim database (with -capture)|
|badchar|bool|[Val](/txt/exe/amc/reftypes.md#val)||Check for bad chars in source files|
|badline|[dev.Badline](/txt/ssimdb/dev/badline.md)|[RegxSql](/txt/exe/amc/reftypes.md#regxsql)|""|Check badline (acr badline)|

#### command.ssim2csv - Command line for [ssim2csv](/txt/exe/ssim2csv/README.md)
<a href="#command-ssim2csv"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|expand|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""||
|ignoreQuote|bool|[Val](/txt/exe/amc/reftypes.md#val)|false||

#### command.ssim2mysql - Command line for [ssim2mysql](/txt/exe/ssim2mysql/README.md)
<a href="#command-ssim2mysql"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|url|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|URL of mysql server. user:pass@hostb or sock://filename; Empty -> stdout|
|data_dir|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"data"|Load dmmeta info from this directory|
|maxpacket|i32|[Val](/txt/exe/amc/reftypes.md#val)|100000|Max Mysql packet size|
|replace|bool|[Val](/txt/exe/amc/reftypes.md#val)|true|use REPLACE INTO instead of INSERT INTO|
|trunc|bool|[Val](/txt/exe/amc/reftypes.md#val)||Truncate target table|
|dry_run|bool|[Val](/txt/exe/amc/reftypes.md#val)||Print SQL commands to the stdout|
|fldfunc|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|create columns for fldfuncs|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"-"|Input directory or filename, - for stdin|
|db|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|Optional database name|
|createdb|bool|[Val](/txt/exe/amc/reftypes.md#val)||Emit CREATE DATABASE code for namespace specified with <db>|
|fkey|bool|[Val](/txt/exe/amc/reftypes.md#val)||Enable foreign key constraints (uses InnoDB storage engine)|

#### command.ssimfilt - Command line for [ssimfilt](/txt/exe/ssimfilt/README.md)
<a href="#command-ssimfilt"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"data"|Input directory or filename, - for stdin|
|typetag|[algo.cstring](/txt/protocol/algo/cstring.md)|[RegxSql](/txt/exe/amc/reftypes.md#regxsql)|"%"|(filter) Match typetag. ^=first encountered typetag|
|match|[algo.cstring](/txt/protocol/algo/cstring.md)|[Tary](/txt/exe/amc/reftypes.md#tary)||(filter) Select input tuple if value of key matches value (regx:regx)|
|field|[algo.cstring](/txt/protocol/algo/cstring.md)|[Tary](/txt/exe/amc/reftypes.md#tary)||(project) Select fields for output (regx)|
|format|u8|[Val](/txt/exe/amc/reftypes.md#val)|0|Output format for selected tuples|
|t|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Alias for -format:table|
|cmd|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|Command to output|

#### command.strconv - Command line for [strconv](/txt/exe/strconv/README.md)
<a href="#command-strconv"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|str|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||String parameter|
|tocamelcase|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Convert string to camel case|
|tolowerunder|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Convert string to lower-under|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"data"|Input directory or filename, - for stdin|
|pathcomp|[algo.Smallstr100](/txt/protocol/algo/README.md#algo-smallstr100)|[Val](/txt/exe/amc/reftypes.md#val)|""|Extract path component from string|

#### command.sv2ssim - Command line for [sv2ssim](/txt/exe/sv2ssim/README.md)
<a href="#command-sv2ssim"></a>

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"data"|Input directory or filename, - for stdin|
|fname|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Input file, use - for stdin|
|separator|char|[Val](/txt/exe/amc/reftypes.md#val)|','|Input field separator|
|outseparator|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|Output separator. Default: ssim|
|header|bool|[Val](/txt/exe/amc/reftypes.md#val)|true|File has header line|
|ctype|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|Type tag for output tuples|
|ssimfile|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|(with -schema) Create ssimfile definition|
|schema|bool|[Val](/txt/exe/amc/reftypes.md#val)||(output)Generate schema from input file|
|field|[algo.cstring](/txt/protocol/algo/cstring.md)|[RegxSql](/txt/exe/amc/reftypes.md#regxsql)|"%"|(output) Print selected fields|
|data|bool|[Val](/txt/exe/amc/reftypes.md#val)||(output) Convert input file to ssim tuples|
|report|bool|[Val](/txt/exe/amc/reftypes.md#val)|true|Print final report|
|prefer_signed|bool|[Val](/txt/exe/amc/reftypes.md#val)||Prefer signed types when given a choice|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Ctypes -->


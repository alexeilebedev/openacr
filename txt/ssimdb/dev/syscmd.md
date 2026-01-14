## dev.syscmd - System command to execute


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Syscmd

* file:[data/dev/syscmd.ssim](/data/dev/syscmd.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|syscmd|i64|[Val](/txt/exe/amc/reftypes.md#val)||Step number|
|execkey|i64|[Val](/txt/exe/amc/reftypes.md#val)||Execution priority: sequence number, nprereq|
|command|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Command to execute|
|pid|i32|[Val](/txt/exe/amc/reftypes.md#val)||PID, if running|
|status|i32|[Val](/txt/exe/amc/reftypes.md#val)||Exit status (if command has completed)|
|nprereq|i32|[Val](/txt/exe/amc/reftypes.md#val)||Number of live pre-requisites|
|fail_prereq|bool|[Val](/txt/exe/amc/reftypes.md#val)||Set if one of pre-requisites fails|
|completed|bool|[Val](/txt/exe/amc/reftypes.md#val)||Completed?|
|maxtime|i32|[Val](/txt/exe/amc/reftypes.md#val)|0|Optional max running time (used to use SIGALRM)|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Subsets
<a href="#subsets"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Subsets -->
These ssimfiles are subsets of dev.syscmd

* [dev.Syscmddep](/txt/ssimdb/dev/syscmddep.md) - Dependency between two commands 

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Subsets -->

### Related
<a href="#related"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Related -->
These ssimfiles reference dev.syscmd

* [dev.syscmddep via parent](/txt/ssimdb/dev/syscmddep.md) - Dependency between two commands 

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Related -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [abt](/txt/exe/abt/internals.md) as [abt.FSyscmd](/txt/exe/abt/internals.md#abt-fsyscmd)
* [lib_exec](/txt/lib/lib_exec/README.md) as [lib_exec.FSyscmd](/txt/lib/lib_exec/README.md#lib_exec-fsyscmd)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->


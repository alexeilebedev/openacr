## dev.syscmd - System command to execute


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Syscmd

* file:[data/dev/syscmd.ssim](/data/dev/syscmd.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|syscmd|i64|[Val](/txt/exe/amc/reftype.md#val)||Step number|
|execkey|i64|[Val](/txt/exe/amc/reftype.md#val)||Execution priority: sequence number, nprereq|
|command|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||Command to execute|
|pid|i32|[Val](/txt/exe/amc/reftype.md#val)||PID, if running|
|status|i32|[Val](/txt/exe/amc/reftype.md#val)||Exit status (if command has completed)|
|nprereq|i32|[Val](/txt/exe/amc/reftype.md#val)||Number of live pre-requisites|
|fail_prereq|bool|[Val](/txt/exe/amc/reftype.md#val)||Set if one of pre-requisites fails|
|completed|bool|[Val](/txt/exe/amc/reftype.md#val)||Completed?|
|maxtime|i32|[Val](/txt/exe/amc/reftype.md#val)|0|Optional max running time (used to use SIGALRM)|

### Subsets
<a href="#subsets"></a>
These ssimfiles are subsets of dev.syscmd

* [dev.Syscmddep](/txt/ssimdb/dev/syscmddep.md) - Dependency between two commands

### Related
<a href="#related"></a>
These ssimfiles reference dev.syscmd

* [dev.syscmddep via parent](/txt/ssimdb/dev/syscmddep.md) - Dependency between two commands

### Used In Executables
<a href="#used-in-executables"></a>
* [abt](/txt/gen/abt/abt.md) as [abt.FSyscmd](/txt/gen/abt/abt.md#abt-fsyscmd)
* [lib_exec](/txt/gen/lib_exec/lib_exec.md) as [lib_exec.FSyscmd](/txt/gen/lib_exec/lib_exec.md#lib_exec-fsyscmd)

## dev.syscmd - System command to execute


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Syscmd

* file:[data/dev/syscmd.ssim](/data/dev/syscmd.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|syscmd|i64|Val||Step number|
|execkey|i64|Val||Execution priority: sequence number, nprereq|
|command|algo.cstring|Val||Command to execute|
|pid|i32|Val||PID, if running|
|status|i32|Val||Exit status (if command has completed)|
|nprereq|i32|Val||Number of live pre-requisites|
|fail_prereq|bool|Val||Set if one of pre-requisites fails|
|completed|bool|Val||Completed?|
|maxtime|i32|Val|0|Optional max running time (used to use SIGALRM)|

### Related
<a href="#related"></a>
These ssimfiles reference dev.syscmd

* [dev.syscmddep via child](/txt/ssimdb/dev/syscmddep.md) - Dependency between two commands 
* [dev.syscmddep via parent](/txt/ssimdb/dev/syscmddep.md) - Dependency between two commands

### Used In Executables
<a href="#used-in-executables"></a>
* [abt](/txt/exe/abt/README.md) as abt.FSyscmd
* [lib_exec](/txt/lib/lib_exec/README.md) as lib_exec.FSyscmd


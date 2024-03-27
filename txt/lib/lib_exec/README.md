## lib_exec - Build and run a dag of subprocesses with N parallel jobs


### Table Of Contents
<a href="#table-of-contents"></a>
* [Ctypes](#ctypes)
* [Functions](#functions)
* [Sources](#sources)
* [Inputs](#inputs)

### Ctypes
<a href="#ctypes"></a>
Other ctypes in this namespace which don't have own readme files

#### lib_exec.Cmdline - 
<a href="#lib_exec-cmdline"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|dry_run|bool|Val|
|q|bool|Val|true|Do not print node name|
|maxjobs|i32|Val|8|Maximum number of parallel jobs|
|complooo|bool|Val|false|

#### lib_exec.FDb - In-memory database for lib_exec
<a href="#lib_exec-fdb"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|_db|lib_exec.FDb|Global|
|cmdline|lib_exec.Cmdline|Val|
|syscmddep|lib_exec.FSyscmddep|Lary|
|next_syscmd|u64|Val|
|syscmd|lib_exec.FSyscmd|Lary|
|ind_running|lib_exec.FSyscmd|Thash||Running subprocesses, hashed by pid|
|bh_syscmd|lib_exec.FSyscmd|Bheap||Heap of all commands in dependency order|
|zd_started|lib_exec.FSyscmd|Llist|

#### lib_exec.FSyscmd - 
<a href="#lib_exec-fsyscmd"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|msghdr|[dev.Syscmd](/txt/ssimdb/dev/syscmd.md)|Base|
|c_prior|lib_exec.FSyscmddep|Ptrary||Set of dependencies where this command is the 'after'|
|c_next|lib_exec.FSyscmddep|Ptrary||Set of dependencies where this command is the 'before'|
|rowid|i32|Val|
|message|algo.cstring|Val||Message to print when command finishes|
|redir_out|bool|Val|true|Redirect stdout/stderr to temp file|
|show_out|bool|Val|true|Print stdout/stderr|
|stdout_fd|algo_lib.FFildes|Val||Temporary file containing stdout of subprocess|
|stderr_fd|algo_lib.FFildes|Val||Temporary file containing stderr of subprocess|
|signal|i32|Val||Signal received by process (if any)|

#### lib_exec.FSyscmddep - 
<a href="#lib_exec-fsyscmddep"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|msghdr|[dev.Syscmddep](/txt/ssimdb/dev/syscmddep.md)|Base|
|p_child|lib_exec.FSyscmd|Upptr||Command to execute after|
|p_parent|lib_exec.FSyscmd|Upptr||Command to execute before (p = parent, prior, prerequisite..)|

### Functions
<a href="#functions"></a>
Functions exported from this namespace:

```c++
i64 lib_exec::execkey_Get(lib_exec::FSyscmd &cmd) 
```

```c++
// Spawn process associated with command.
// If the command was started successfully, its pid can be
// looked up in ind_running, and the command is added to zd_started list.
void lib_exec::StartCmd(lib_exec::FSyscmd &cmd) 
```

```c++
// Run created command graph and return # of commands that failed.
// (successful run -> return value 0).
// Individual command status codes can be examined directly.
// To reset the graph, call syscmd_RemoveAll().
int lib_exec::SyscmdExecute() 
```

```c++
// Return a new command record which will be started after START,
// and is guaranteed to exit before END.
// This is the basic building block for creating commands.
lib_exec::FSyscmd &lib_exec::NewCmd(lib_exec::FSyscmd *start, lib_exec::FSyscmd *end) 
```

```c++
// Returns true if command was actually invoked and did exist successfully.
// If the command never ran, return false.
bool lib_exec::CompletedOKQ(lib_exec::FSyscmd &cmd) 
```

```c++
// Frees FDs for stdout and stderr
void lib_exec::RefurbishStd(lib_exec::FSyscmd &cmd) 
```

### Sources
<a href="#sources"></a>
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/gen/lib_exec_gen.cpp](/cpp/gen/lib_exec_gen.cpp)||
|[cpp/lib/lib_exec.cpp](/cpp/lib/lib_exec.cpp)||
|[include/gen/lib_exec_gen.h](/include/gen/lib_exec_gen.h)||
|[include/gen/lib_exec_gen.inl.h](/include/gen/lib_exec_gen.inl.h)||
|[include/lib_exec.h](/include/lib_exec.h)||

### Inputs
<a href="#inputs"></a>
`lib_exec` takes the following tables on input:
|ssimfile|comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|


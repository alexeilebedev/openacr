## lib_git - Helpful git wrappers


### Table Of Contents
<a href="#table-of-contents"></a>
* [Ctypes](#ctypes)
* [Functions](#functions)
* [Sources](#sources)
* [Inputs](#inputs)

### Ctypes
<a href="#ctypes"></a>
Other ctypes in this namespace which don't have own readme files

#### lib_git.FDb - In-memory database for lib_git
<a href="#lib_git-fdb"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|_db|lib_git.FDb|Global|

### Functions
<a href="#functions"></a>
Functions exported from this namespace:

```c++
// Return current git ref, e.g. ac18e6cb142a86136b58561f2f1d63e3ff4b749e
// Any error output is suppressed.
tempstr lib_git::CurrentRef() 
```

```c++
// Return symbolic name for current branch, e.g. AB-12345
// Any error output is suppressed.
tempstr lib_git::CurrentBranch() 
```

```c++
// Return date + gitref string describing most recent commit, e.g. 2017-05-24.21f8ce8
tempstr lib_git::ShortRef() 
```

```c++
// Return most recent log entry
// If there was a merge, this will describe 2 or more commits. If there was a straight
// commit, there will be just 1 entry (compare with git log -1)
tempstr lib_git::LastCommitLog() 
```

```c++
// Check that specified DIR is free of any git modifications.
// Return a string describing modified files, or an empty string if none
tempstr lib_git::GitModifiedFiles(strptr dir) 
```

```c++
// Check that specified DIR is free of any git modifications.
// If failed, throw exception with MSG.
void lib_git::CheckGitCleanX(strptr dir, strptr msg) 
```

```c++
// Return TRUE if REMOTE is a valid git remote
bool lib_git::GitRemoteExistsQ(strptr remote) 
```

```c++
// Return TRUE if BRANCH is a valid git branch
bool lib_git::GitBranchExistsQ(strptr branch) 
```

```c++
// Get configured user: name <email>
tempstr lib_git::GetUser() 
```

```c++
// Get year of latest commit of given file
u32 lib_git::GetLastCommitYear(strptr file) 
```

### Sources
<a href="#sources"></a>
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/gen/lib_git_gen.cpp](/cpp/gen/lib_git_gen.cpp)||
|[cpp/lib/lib_git.cpp](/cpp/lib/lib_git.cpp)||
|[include/gen/lib_git_gen.h](/include/gen/lib_git_gen.h)||
|[include/gen/lib_git_gen.inl.h](/include/gen/lib_git_gen.inl.h)||
|[include/lib_git.h](/include/lib_git.h)||

### Inputs
<a href="#inputs"></a>
`lib_git` takes the following tables on input:
|ssimfile|comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|


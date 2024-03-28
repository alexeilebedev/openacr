## atf_nrun - Run N subprocesses in parallel


### Table Of Contents
<a href="#table-of-contents"></a>
* [Syntax](#syntax)
* [Options](#options)
* [Sources](#sources)
* [Inputs](#inputs)

### Syntax
<a href="#syntax"></a>
```
atf_nrun: Run N subprocesses in parallel
Usage: atf_nrun [[-ncmd:]<int>] [options]
    OPTION      TYPE    DFLT    COMMENT
    -in         string  "data"  Input directory or filename, - for stdin
    -maxjobs    int     2       Number of simultaneous jobs
    [ncmd]      int     6
    -verbose    int             Verbosity level (0..255); alias -v; cumulative
    -debug      int             Debug level (0..255); alias -d; cumulative
    -help                       Print help and exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig

```

### Options
<a href="#options"></a>

#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

#### -maxjobs -- Number of simultaneous jobs
<a href="#-maxjobs"></a>

#### -ncmd -- 
<a href="#-ncmd"></a>

### Sources
<a href="#sources"></a>
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/atf/atf_nrun.cpp](/cpp/atf/atf_nrun.cpp)||
|[cpp/gen/atf_nrun_gen.cpp](/cpp/gen/atf_nrun_gen.cpp)||
|[include/atf_nrun.h](/include/atf_nrun.h)||
|[include/gen/atf_nrun_gen.h](/include/gen/atf_nrun_gen.h)||
|[include/gen/atf_nrun_gen.inl.h](/include/gen/atf_nrun_gen.inl.h)||

### Inputs
<a href="#inputs"></a>
`atf_nrun` takes the following tables on input:
|ssimfile|comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|


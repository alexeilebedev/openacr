## ams_cat - Algo Messaging System sample tool


### Table Of Contents
<a href="#table-of-contents"></a>
* [Syntax](#syntax)
* [Options](#options)
* [Sources](#sources)
* [Inputs](#inputs)

### Syntax
<a href="#syntax"></a>
```
ams_cat: Algo Messaging System sample tool
Usage: ams_cat [options]
    OPTION      TYPE    DFLT    COMMENT
    -in         string  "data"  Input directory or filename, - for stdin
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

### Sources
<a href="#sources"></a>
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/ams/ams_cat.cpp](/cpp/ams/ams_cat.cpp)||
|[cpp/gen/ams_cat_gen.cpp](/cpp/gen/ams_cat_gen.cpp)||
|[include/ams_cat.h](/include/ams_cat.h)||
|[include/gen/ams_cat_gen.h](/include/gen/ams_cat_gen.h)||
|[include/gen/ams_cat_gen.inl.h](/include/gen/ams_cat_gen.inl.h)||

### Inputs
<a href="#inputs"></a>
`ams_cat` takes the following tables on input:
|ssimfile|comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|


## mysql2ssim - mysql -> ssim conversion tool


### Table Of Contents
<a href="#table-of-contents"></a>
* [Syntax](#syntax)
* [Options](#options)
* [Sources](#sources)
* [Inputs](#inputs)

### Syntax
<a href="#syntax"></a>
```
mysql2ssim: mysql -> ssim conversion tool
Usage: mysql2ssim [-url:]<string> [[-tables:]<string>] [options]
    OPTION          TYPE    DFLT    COMMENT
    -writessimfile                  Write to ssimfile directly
    [url]           string          user:pass@host/db or sock:///filename/db
    [tables]        string  ""      comma-separated list of tables. Default is all tables
    -schema                         Generate ssim type definition
    -in             string  "data"  Input directory or filename, - for stdin
    -pretty                         Format output for the screen
    -nologo                         Don't show copyright notice
    -baddbok                        Don't claim if bad database
    -verbose        int             Verbosity level (0..255); alias -v; cumulative
    -debug          int             Debug level (0..255); alias -d; cumulative
    -help                           Print help and exit; alias -h
    -version                        Print version and exit
    -signature                      Show signatures and exit; alias -sig

```

### Options
<a href="#options"></a>

#### -writessimfile -- Write to ssimfile directly
<a href="#-writessimfile"></a>

#### -url -- user:pass@host/db or sock:///filename/db
<a href="#-url"></a>

#### -tables -- comma-separated list of tables. Default is all tables
<a href="#-tables"></a>

#### -schema -- Generate ssim type definition
<a href="#-schema"></a>

#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

#### -pretty -- Format output for the screen
<a href="#-pretty"></a>

#### -nologo -- Don't show copyright notice
<a href="#-nologo"></a>

#### -baddbok -- Don't claim if bad database
<a href="#-baddbok"></a>

### Sources
<a href="#sources"></a>
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/gen/mysql2ssim_gen.cpp](/cpp/gen/mysql2ssim_gen.cpp)||
|[cpp/mysql2ssim.cpp](/cpp/mysql2ssim.cpp)||
|[include/gen/mysql2ssim_gen.h](/include/gen/mysql2ssim_gen.h)||
|[include/gen/mysql2ssim_gen.inl.h](/include/gen/mysql2ssim_gen.inl.h)||

### Inputs
<a href="#inputs"></a>
`mysql2ssim` takes the following tables on input:
|ssimfile|comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|


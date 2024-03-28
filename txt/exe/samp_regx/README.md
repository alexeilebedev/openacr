## samp_regx - Test tool for regular expressions


### Table Of Contents
<a href="#table-of-contents"></a>
* [Syntax](#syntax)
* [Options](#options)
* [Sources](#sources)
* [Inputs](#inputs)

### Syntax
<a href="#syntax"></a>
```
samp_regx: Test tool for regular expressions
Usage: samp_regx [-expr:]<string> [[-string:]<string>] [options]
    OPTION      TYPE    DFLT    COMMENT
    -in         string  "data"  Input directory or filename, - for stdin
    [expr]      string          Expression
    -style      int     acr     Regx style (acr|shell|classic|literal)
                                    acr  ACR-style regx
                                    shell  Shell-style regx
                                    classic  Classic regx
                                    literal  Literal string
    -match                      Match a string
    [string]    string  ""      String to match
    -show                       Show regx innards
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

#### -expr -- Expression
<a href="#-expr"></a>

#### -style -- Regx style
<a href="#-style"></a>

#### -match -- Match a string
<a href="#-match"></a>

#### -string -- String to match
<a href="#-string"></a>

#### -show -- Show regx innards
<a href="#-show"></a>

### Sources
<a href="#sources"></a>
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/gen/samp_regx_gen.cpp](/cpp/gen/samp_regx_gen.cpp)||
|[cpp/samp/samp_regx.cpp](/cpp/samp/samp_regx.cpp)||
|[include/gen/samp_regx_gen.h](/include/gen/samp_regx_gen.h)||
|[include/gen/samp_regx_gen.inl.h](/include/gen/samp_regx_gen.inl.h)||
|[include/samp_regx.h](/include/samp_regx.h)||

### Inputs
<a href="#inputs"></a>
`samp_regx` takes the following tables on input:
|ssimfile|comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|


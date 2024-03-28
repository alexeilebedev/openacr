## src_lim - Enforce line length, function length, and indentation limits


### Table Of Contents
<a href="#table-of-contents"></a>
* [Syntax](#syntax)
* [Options](#options)
* [Sources](#sources)
* [Inputs](#inputs)

### Syntax
<a href="#syntax"></a>
```
src_lim: Enforce line length, function length, and indentation limits
Usage: src_lim [options]
    OPTION      TYPE    DFLT    COMMENT
    -in         string  "data"  Input directory or filename, - for stdin
    -linelim                    Check various file limits (line length, function length, etc)
    -srcfile    regx    "%"     Filter for source files to process
    -strayfile                  Check for unregistered source files
    -capture                    Generate new dev.linelim records
    -write                      Update ssim database (with -capture)
    -badchar                    Check for bad chars in source files
    -badline    regx    ""      Check badline (acr badline)
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

#### -linelim -- Check various file limits (line length, function length, etc)
<a href="#-linelim"></a>

#### -srcfile -- Filter for source files to process
<a href="#-srcfile"></a>

#### -strayfile -- Check for unregistered source files
<a href="#-strayfile"></a>

#### -capture -- Generate new dev.linelim records
<a href="#-capture"></a>

#### -write -- Update ssim database (with -capture)
<a href="#-write"></a>

#### -badchar -- Check for bad chars in source files
<a href="#-badchar"></a>

#### -badline -- Check badline (acr badline)
<a href="#-badline"></a>

### Sources
<a href="#sources"></a>
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/gen/src_lim_gen.cpp](/cpp/gen/src_lim_gen.cpp)||
|[cpp/src_lim/lim.cpp](/cpp/src_lim/lim.cpp)||
|[include/gen/src_lim_gen.h](/include/gen/src_lim_gen.h)||
|[include/gen/src_lim_gen.inl.h](/include/gen/src_lim_gen.inl.h)||

### Inputs
<a href="#inputs"></a>
`src_lim` takes the following tables on input:
|ssimfile|comment|
|---|---|
|[dev.gitfile](/txt/ssimdb/dev/gitfile.md)|File managed by git|
|[dev.targsrc](/txt/ssimdb/dev/targsrc.md)|List of sources for target|
|[dev.linelim](/txt/ssimdb/dev/linelim.md)||
|[dev.include](/txt/ssimdb/dev/include.md)|A site where one file includes another|
|[dev.badline](/txt/ssimdb/dev/badline.md)|Regex of a dubious source code line|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|


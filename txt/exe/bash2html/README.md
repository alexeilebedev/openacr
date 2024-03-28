## bash2html - Convert bash output and colours to html


### Table Of Contents
<a href="#table-of-contents"></a>
* [Syntax](#syntax)
* [Options](#options)
* [Sources](#sources)
* [Inputs](#inputs)

### Syntax
<a href="#syntax"></a>
```
bash2html: Convert bash output and colours to html
Usage: bash2html [options]
    OPTION      TYPE    DFLT    COMMENT
    -in         string  "data"  Input directory or filename, - for stdin
    -test                       Produce Test Output
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

#### -test -- Produce Test Output
<a href="#-test"></a>

### Sources
<a href="#sources"></a>
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/bash2html.cpp](/cpp/bash2html.cpp)||
|[cpp/gen/bash2html_gen.cpp](/cpp/gen/bash2html_gen.cpp)||
|[include/bash2html.h](/include/bash2html.h)||
|[include/gen/bash2html_gen.h](/include/gen/bash2html_gen.h)||
|[include/gen/bash2html_gen.inl.h](/include/gen/bash2html_gen.inl.h)||

### Inputs
<a href="#inputs"></a>
`bash2html` takes the following tables on input:
|ssimfile|comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|


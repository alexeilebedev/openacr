## strconv - A simple string utility


### Table Of Contents
<a href="#table-of-contents"></a>
* [Syntax](#syntax)
* [Options](#options)
* [Sources](#sources)
* [Inputs](#inputs)

### Syntax
<a href="#syntax"></a>
```
strconv: A simple string utility
Usage: strconv [-str:]<string> [options]
    OPTION         TYPE    DFLT    COMMENT
    [str]          string          String parameter
    -tocamelcase                   Convert string to camel case
    -tolowerunder                  Convert string to lower-under
    -in            string  "data"  Input directory or filename, - for stdin
    -pathcomp      string  ""      Extract path component from string
    -verbose       int             Verbosity level (0..255); alias -v; cumulative
    -debug         int             Debug level (0..255); alias -d; cumulative
    -help                          Print help and exit; alias -h
    -version                       Print version and exit
    -signature                     Show signatures and exit; alias -sig

```

### Options
<a href="#options"></a>

#### -str -- String parameter
<a href="#-str"></a>

#### -tocamelcase -- Convert string to camel case
<a href="#-tocamelcase"></a>

#### -tolowerunder -- Convert string to lower-under
<a href="#-tolowerunder"></a>

#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

#### -pathcomp -- Extract path component from string
<a href="#-pathcomp"></a>

### Sources
<a href="#sources"></a>
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/gen/strconv_gen.cpp](/cpp/gen/strconv_gen.cpp)||
|[cpp/strconv/strconv.cpp](/cpp/strconv/strconv.cpp)||
|[include/gen/strconv_gen.h](/include/gen/strconv_gen.h)||
|[include/gen/strconv_gen.inl.h](/include/gen/strconv_gen.inl.h)||
|[include/strconv.h](/include/strconv.h)||

### Inputs
<a href="#inputs"></a>
`strconv` takes the following tables on input:
|ssimfile|comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|


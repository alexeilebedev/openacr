## src_func - Access / edit functions


### Table Of Contents
<a href="#table-of-contents"></a>
* [Syntax](#syntax)
* [Options](#options)
* [Examples](#examples)
* [Sources](#sources)
* [Inputs](#inputs)

### Syntax
<a href="#syntax"></a>
```
src_func: Access / edit functions
Usage: src_func [[-target:]<regx>] [[-name:]<regx>] [[-body:]<regx>] [options]
    OPTION        TYPE    DFLT    COMMENT
    -in           string  "data"  Input directory or filename, - for stdin
    [target]      regx    "%"     Visit these targets
    [name]        regx    "%"     Match function name
    [body]        regx    "%"     Match function body
    -targsrc      regx    ""      Visit these sources (optional)
    -func         regx    "%"     (with -listfunc) Match function prototype
    -comment      regx    "%"     (with -listfunc) Match function comment
    -nextfile     string  ""      Print name of next srcfile in targsrc list
    -other                        (with -nextfile), name of previous file
    -updateproto                  Update prototypes in headers
    -listfunc                     Show functions in target
    -iffy                         Select functions that may contain errors
    -proto                        (with -listfunc) List prototypes only
    -gen                          (with -listfunc) Visit generated files
    -showloc              Y       (with -listfunc) Show file location
    -showstatic           Y       (with -listfunc) Show static functions
    -showsortkey                  (with -listfunc) Display function sortkey
    -sortname                     (with -listfunc) Sort functions by name
    -e                            (with -listfunc) Edit found functions
    -baddecl                      Report and fail on bad declarations
    -report
    -verbose      int             Verbosity level (0..255); alias -v; cumulative
    -debug        int             Debug level (0..255); alias -d; cumulative
    -help                         Print help and exit; alias -h
    -version                      Print version and exit
    -signature                    Show signatures and exit; alias -sig

```

### Options
<a href="#options"></a>

#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

#### -target -- Visit these targets
<a href="#-target"></a>

#### -name -- Match function name
<a href="#-name"></a>

#### -body -- Match function body
<a href="#-body"></a>

#### -targsrc -- Visit these sources (optional)
<a href="#-targsrc"></a>

#### -func -- (with -listfunc) Match function prototype
<a href="#-func"></a>

#### -comment -- (with -listfunc) Match function comment
<a href="#-comment"></a>

#### -nextfile -- Print name of next srcfile in targsrc list
<a href="#-nextfile"></a>

#### -other -- (with -nextfile), name of previous file
<a href="#-other"></a>

#### -updateproto -- Update prototypes in headers
<a href="#-updateproto"></a>

#### -listfunc -- Show functions in target
<a href="#-listfunc"></a>

#### -iffy -- Select functions that may contain errors
<a href="#-iffy"></a>

#### -proto -- (with -listfunc) List prototypes only
<a href="#-proto"></a>

#### -gen -- (with -listfunc) Visit generated files
<a href="#-gen"></a>

#### -showloc -- (with -listfunc) Show file location
<a href="#-showloc"></a>

#### -showstatic -- (with -listfunc) Show static functions
<a href="#-showstatic"></a>

#### -showsortkey -- (with -listfunc) Display function sortkey
<a href="#-showsortkey"></a>

#### -sortname -- (with -listfunc) Sort functions by name
<a href="#-sortname"></a>

#### -e -- (with -listfunc) Edit found functions
<a href="#-e"></a>

#### -baddecl -- Report and fail on bad declarations
<a href="#-baddecl"></a>

#### -report -- 
<a href="#-report"></a>

### Examples
<a href="#examples"></a>
* Re-scan source files for function prototypes

    src_func -updateproto

* List functions in target:

    src_func amc

* List function prototypes in target:

    src_func amc -proto

* Edit functions in target matching regex:

    src_func amc %X% -e
    <use F4, shift-F4 to visit locations>

* Show functions that are not documented:

    src_func amc -listiffy

* Edit all functions in program:

     src_func amc -e

* Search all hand-written source for pattern:

     src_func % % "%pattern%"

* Search all generated source for pattern:

     src_func % % "%pattern%" -gen

* Search hand-written source for pattern:

     src_func amc % "%pattern%"

* Search source of specific target for pattern and open in editor:

     src_func amc % "%pattern%" -e

* Search function arguments (i.e. first line, including return type and arguments):

     src_func % % -func:%amc::FCtype%

* Show target functions, sort by name:

     src_func amc -sortname

### Sources
<a href="#sources"></a>
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/gen/src_func_gen.cpp](/cpp/gen/src_func_gen.cpp)||
|[cpp/src_func/edit.cpp](/cpp/src_func/edit.cpp)|Implementation of -e|
|[cpp/src_func/fileloc.cpp](/cpp/src_func/fileloc.cpp)|Location in file, for each function|
|[cpp/src_func/header.cpp](/cpp/src_func/header.cpp)|Generate prototypes in header|
|[cpp/src_func/iffy.cpp](/cpp/src_func/iffy.cpp)|Check iffy function prototypes|
|[cpp/src_func/list.cpp](/cpp/src_func/list.cpp)|Print list of functions|
|[cpp/src_func/main.cpp](/cpp/src_func/main.cpp)|Main file|
|[cpp/src_func/nextfile.cpp](/cpp/src_func/nextfile.cpp)|Find next file in target|
|[cpp/src_func/scan.cpp](/cpp/src_func/scan.cpp)|Scan sources to collect functions|
|[include/gen/src_func_gen.h](/include/gen/src_func_gen.h)||
|[include/gen/src_func_gen.inl.h](/include/gen/src_func_gen.inl.h)||
|[include/src_func.h](/include/src_func.h)||

### Inputs
<a href="#inputs"></a>
`src_func` takes the following tables on input:
|ssimfile|comment|
|---|---|
|[dev.target](/txt/ssimdb/dev/target.md)|Build target|
|[dev.targsrc](/txt/ssimdb/dev/targsrc.md)|List of sources for target|
|[dmmeta.gstatic](/txt/ssimdb/dmmeta/gstatic.md)|Load entries for this table at startup time|
|[dmmeta.fstep](/txt/ssimdb/dmmeta/fstep.md)|Generate a main loop step to be executed whenever a field is non-empty|
|[dmmeta.dispatch](/txt/ssimdb/dmmeta/dispatch.md)|Generate code for a multi-way branch|
|[dmmeta.ctypelen](/txt/ssimdb/dmmeta/ctypelen.md)|Size of Ctype|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|


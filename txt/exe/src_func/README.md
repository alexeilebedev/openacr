## src_func - Access / edit functions


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Toc -->
&nbsp;&nbsp;&bull;&nbsp;  [Syntax](#syntax)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Examples](#examples)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Options](#options)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
&#128196; [src_func - Internals](/txt/exe/src_func/internals.md)<br/>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Toc -->

### Syntax
<a href="#syntax"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Syntax -->
```
src_func: Access / edit functions
Usage: src_func [[-func:]<regx>] [options]
    OPTION          TYPE    DFLT    COMMENT
    -in             string  "data"  Input directory or filename, - for stdin
    -targsrc        regx    "%"     (scan) Limit scanning to these sources only
    -acrkey         regx    "%"     Select function by acr key that caused it
    [func]          regx    "%"     Target.function regex
    -nextfile       string  ""      (action) Print name of next srcfile in targsrc list
    -other                              (with -nextfile), name of previous file
    -list                           (action) List matching functions
    -updateproto                    (action) Update prototypes in headers
    -createmissing                  (action) Create missing functions declared in userfunc table
    -iffy                           (filter) Select functions that may contain errors
    -gen                            (scan) Scan generated files
    -showloc                        (output) Show file location
    -f                              (output) -sortname -showcomment -showbody
    -showstatic             Y       (filter) Allow static functions
    -matchproto     regx    "%"     (filter) Match function prototype
    -matchbody      regx    "%"     (filter) Match function body
    -matchcomment   regx    "%"     (filter) Match function comment
    -showsortkey                    (output) Display function sortkey
    -showcomment                    (output) Display function comment
    -showbody                       (output) Print function body
    -sortname                       (output) Sort functions by name
    -printssim                      (output) Print ssim tuples
    -e                              Edit found functions
    -baddecl                        Report and fail on bad declarations
    -report                         Print final report
    -verbose        flag            Verbosity level (0..255); alias -v; cumulative
    -debug          flag            Debug level (0..255); alias -d; cumulative
    -help                           Print help and exit; alias -h
    -version                        Print version and exit
    -signature                      Show signatures and exit; alias -sig

```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Syntax -->

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

### Options
<a href="#options"></a>

<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Options -->
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

#### -targsrc -- (scan) Limit scanning to these sources only
<a href="#-targsrc"></a>

#### -acrkey -- Select function by acr key that caused it
<a href="#-acrkey"></a>

#### -func -- Target.function regex
<a href="#-func"></a>

#### -nextfile -- (action) Print name of next srcfile in targsrc list
<a href="#-nextfile"></a>

#### -other --     (with -nextfile), name of previous file
<a href="#-other"></a>

#### -list -- (action) List matching functions
<a href="#-list"></a>

#### -updateproto -- (action) Update prototypes in headers
<a href="#-updateproto"></a>

#### -createmissing -- (action) Create missing functions declared in userfunc table
<a href="#-createmissing"></a>

#### -iffy -- (filter) Select functions that may contain errors
<a href="#-iffy"></a>

#### -gen -- (scan) Scan generated files
<a href="#-gen"></a>

#### -showloc -- (output) Show file location
<a href="#-showloc"></a>

#### -f -- (output) -sortname -showcomment -showbody
<a href="#-f"></a>

#### -showstatic -- (filter) Allow static functions
<a href="#-showstatic"></a>

#### -matchproto -- (filter) Match function prototype
<a href="#-matchproto"></a>

#### -matchbody -- (filter) Match function body
<a href="#-matchbody"></a>

#### -matchcomment -- (filter) Match function comment
<a href="#-matchcomment"></a>

#### -showsortkey -- (output) Display function sortkey
<a href="#-showsortkey"></a>

#### -showcomment -- (output) Display function comment
<a href="#-showcomment"></a>

#### -showbody -- (output) Print function body
<a href="#-showbody"></a>

#### -sortname -- (output) Sort functions by name
<a href="#-sortname"></a>

#### -printssim -- (output) Print ssim tuples
<a href="#-printssim"></a>

#### -e -- Edit found functions
<a href="#-e"></a>

#### -baddecl -- Report and fail on bad declarations
<a href="#-baddecl"></a>

#### -report -- Print final report
<a href="#-report"></a>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Options -->

### Inputs
<a href="#inputs"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Inputs -->
`src_func` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|
|[dmmeta.ctypelen](/txt/ssimdb/dmmeta/ctypelen.md)|Size of Ctype|
|[dev.target](/txt/ssimdb/dev/target.md)|Build target|
|[dev.targsrc](/txt/ssimdb/dev/targsrc.md)|List of sources for target|
|[dmmeta.userfunc](/txt/ssimdb/dmmeta/userfunc.md)||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Inputs -->


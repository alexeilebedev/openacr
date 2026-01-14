## sv2ssim - sv2ssim - Separated Value file processor


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Toc -->
&nbsp;&nbsp;&bull;&nbsp;  [Syntax](#syntax)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Description](#description)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Test cases](#test-cases)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Examples](#examples)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Options](#options)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
&#128196; [sv2ssim - Internals](/txt/exe/sv2ssim/internals.md)<br/>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Toc -->

### Syntax
<a href="#syntax"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Syntax -->
```
sv2ssim: sv2ssim - Separated Value file processor
Usage: sv2ssim [-fname:]<string> [options]
    OPTION          TYPE    DFLT    COMMENT
    -in             string  "data"  Input directory or filename, - for stdin
    [fname]         string          Input file, use - for stdin
    -separator      string  ','     Input field separator
    -outseparator   string  ""      Output separator. Default: ssim
    -header                 Y       File has header line
    -ctype          string  ""      Type tag for output tuples
    -ssimfile       string  ""      (with -schema) Create ssimfile definition
    -schema                         (output)Generate schema from input file
    -field          regx    "%"     (output) Print selected fields
    -data                           (output) Convert input file to ssim tuples
    -report                 Y       Print final report
    -prefer_signed                  Prefer signed types when given a choice
    -verbose        flag            Verbosity level (0..255); alias -v; cumulative
    -debug          flag            Debug level (0..255); alias -d; cumulative
    -help                           Print help and exit; alias -h
    -version                        Print version and exit
    -signature                      Show signatures and exit; alias -sig

```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Syntax -->

### Description
<a href="#description"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Description -->

sv2ssim is a tool for extracting schema from tab-,comma-, and other delimiter-separated files (sv-files).
The tool can process files with and without a header line (`-header`), extract schema by observing
the column width and value range of values, and generate ssim schema on output.

The tool can read files or stdin (with "-" as filename argument).
It can convert between sv-formats as well as convert sv-files to ssim tuples;
It handles double-quote escape characters in CSV files.

The main configuration comes from the `svtype` table, which helps the tool map observed values to ssim types.
When more than one svtype matches, it selects one with the lowest size (`maxwid`). If all else fails, `algo.cstring`
is chosen as the type.

```
inline-command: acr svtype -report:N
dmmeta.svtype  ctype:algo.Smallstr10   maxwid:10          fixedwid1:0  fixedwid2:0  comment:""
dmmeta.svtype  ctype:algo.Smallstr100  maxwid:100         fixedwid1:0  fixedwid2:0  comment:""
dmmeta.svtype  ctype:algo.Smallstr150  maxwid:150         fixedwid1:0  fixedwid2:0  comment:""
dmmeta.svtype  ctype:algo.Smallstr200  maxwid:200         fixedwid1:0  fixedwid2:0  comment:""
dmmeta.svtype  ctype:algo.Smallstr249  maxwid:249         fixedwid1:0  fixedwid2:0  comment:""
dmmeta.svtype  ctype:algo.Smallstr25   maxwid:25          fixedwid1:0  fixedwid2:0  comment:""
dmmeta.svtype  ctype:algo.Smallstr250  maxwid:250         fixedwid1:0  fixedwid2:0  comment:""
dmmeta.svtype  ctype:algo.Smallstr255  maxwid:255         fixedwid1:0  fixedwid2:0  comment:""
dmmeta.svtype  ctype:algo.Smallstr50   maxwid:10          fixedwid1:0  fixedwid2:0  comment:""
dmmeta.svtype  ctype:algo.cstring      maxwid:1000000000  fixedwid1:0  fixedwid2:0  comment:""
dmmeta.svtype  ctype:bool              maxwid:1           fixedwid1:0  fixedwid2:0  comment:""
dmmeta.svtype  ctype:char              maxwid:1           fixedwid1:0  fixedwid2:0  comment:""
dmmeta.svtype  ctype:double            maxwid:64          fixedwid1:0  fixedwid2:0  comment:""
dmmeta.svtype  ctype:i32               maxwid:31          fixedwid1:0  fixedwid2:0  comment:""
dmmeta.svtype  ctype:i64               maxwid:63          fixedwid1:0  fixedwid2:0  comment:""
dmmeta.svtype  ctype:u32               maxwid:32          fixedwid1:0  fixedwid2:0  comment:""
dmmeta.svtype  ctype:u64               maxwid:64          fixedwid1:0  fixedwid2:0  comment:""
```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Description -->

### Test cases
<a href="#test-cases"></a>

```
inline-command: acr comptest:sv2ssim.%
atfdb.comptest  comptest:sv2ssim.Convert1         timeout:10  memcheck:Y  coverage:Y  exit_code:0  ncore:1  repeat:1  comment:""
atfdb.comptest  comptest:sv2ssim.Convert1Signed   timeout:10  memcheck:Y  coverage:Y  exit_code:0  ncore:1  repeat:1  comment:""
atfdb.comptest  comptest:sv2ssim.Convert2         timeout:10  memcheck:Y  coverage:Y  exit_code:0  ncore:1  repeat:1  comment:""
atfdb.comptest  comptest:sv2ssim.Convert2Tsv      timeout:10  memcheck:Y  coverage:Y  exit_code:0  ncore:1  repeat:1  comment:""
atfdb.comptest  comptest:sv2ssim.UniqueFieldName  timeout:10  memcheck:Y  coverage:Y  exit_code:0  ncore:1  repeat:1  comment:""
report.acr  n_select:5  n_insert:0  n_delete:0  n_ignore:0  n_update:0  n_file_mod:0
```

### Examples
<a href="#examples"></a>

```
inline-command: cat test/csv/2.csv; echo; sv2ssim test/csv/2.csv -ctype a.B -schema -data -prefer_signed
"Month", "1958", "1959", "1960"
"JAN",  340,  360,  417
"FEB",  318,  342,  391
"MAR",  362,  406,  419
"APR",  348,  396,  461
"MAY",  363,  420,  472
"JUN",  435,  472,  535
"JUL",  491,  548,  622
"AUG",  505,  559,  606
"SEP",  404,  463,  508
"OCT",  359,  407,  461
"NOV",  310,  362,  390
"DEC",  337,  405,  432

a.B  Month:JAN  _1958:340  _1959:360  _1960:417
a.B  Month:FEB  _1958:318  _1959:342  _1960:391
a.B  Month:MAR  _1958:362  _1959:406  _1960:419
a.B  Month:APR  _1958:348  _1959:396  _1960:461
a.B  Month:MAY  _1958:363  _1959:420  _1960:472
a.B  Month:JUN  _1958:435  _1959:472  _1960:535
a.B  Month:JUL  _1958:491  _1959:548  _1960:622
a.B  Month:AUG  _1958:505  _1959:559  _1960:606
a.B  Month:SEP  _1958:404  _1959:463  _1960:508
a.B  Month:OCT  _1958:359  _1959:407  _1960:461
a.B  Month:NOV  _1958:310  _1959:362  _1960:390
a.B  Month:DEC  _1958:337  _1959:405  _1960:432
sv2ssim.report  n_line:13  n_nonempty_line:13  n_field:4  n_wideline:1
dmmeta.ctype  ctype:a.B  comment:""
dmmeta.cfmt  cfmt:a.B.String  printfmt:Tuple  read:Y  print:Y  sep:""  genop:Y  comment:""
dmmeta.field  field:a.B.Month  arg:algo.Smallstr10  reftype:Val  dflt:""  comment:Month
dmmeta.field  field:a.B._1958  arg:i32  reftype:Val  dflt:""  comment:_1958
dmmeta.field  field:a.B._1959  arg:i32  reftype:Val  dflt:""  comment:_1959
dmmeta.field  field:a.B._1960  arg:i32  reftype:Val  dflt:""  comment:_1960
```

### Options
<a href="#options"></a>

<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Options -->
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

#### -fname -- Input file, use - for stdin
<a href="#-fname"></a>

#### -separator -- Input field separator
<a href="#-separator"></a>

#### -outseparator -- Output separator. Default: ssim
<a href="#-outseparator"></a>

#### -header -- File has header line
<a href="#-header"></a>

#### -ctype -- Type tag for output tuples
<a href="#-ctype"></a>

#### -ssimfile -- (with -schema) Create ssimfile definition
<a href="#-ssimfile"></a>

#### -schema -- (output)Generate schema from input file
<a href="#-schema"></a>

#### -field -- (output) Print selected fields
<a href="#-field"></a>

#### -data -- (output) Convert input file to ssim tuples
<a href="#-data"></a>

#### -report -- Print final report
<a href="#-report"></a>

#### -prefer_signed -- Prefer signed types when given a choice
<a href="#-prefer_signed"></a>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Options -->

### Inputs
<a href="#inputs"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Inputs -->
`sv2ssim` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|
|[amcdb.bltin](/txt/ssimdb/amcdb/bltin.md)|Specify properties of a C built-in type|
|[dmmeta.svtype](/txt/ssimdb/dmmeta/svtype.md)|Table for determining ctype from separated value file|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Inputs -->


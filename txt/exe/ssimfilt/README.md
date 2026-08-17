## ssimfilt - Tuple utility


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- abt_md.toc_beg -->
&nbsp;&nbsp;&bull;&nbsp;  [Internals](#internals)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Syntax](#syntax)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Operation](#operation)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Examples](#examples)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Options](#options)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
<!-- abt_md.toc_end -->

### Internals
<a href="#internals"></a>
&#128196; [ssimfilt - Internals](/txt/gen/ssimfilt/ssimfilt.md)<br/>

### Syntax
<a href="#syntax"></a>
```
ssimfilt: Tuple utility
Usage: ssimfilt [[-typetag:]<regx>] [[-match:]<string>] [options]
    OPTION      TYPE    DFLT    COMMENT
    -in         string  "data"  Input directory or filename, - for stdin
    [typetag]   regx    "%"     (filter) Match typetag. ^=first encountered typetag
    [match]...  string          (filter) Select input tuple if value of key matches value (regx:regx)
    -field...   string          (project) Select fields for output (regx)
    -format     enum    ssim    Output format for selected tuples (ssim|csv|field|cmd|json|stablefld|table|mdtable)
    -t                          Alias for -format:table
    -cmd        string  ""      Command to output
    -f          string  ""      Alias for -field:<f> -format:field
    -verbose    flag            Verbosity level (0..255); alias -v; cumulative
    -debug      flag            Debug level (0..255); alias -d; cumulative
    -help                       Print help and exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig
```

### Operation
<a href="#operation"></a>
Ssimfilt reads stdin, filtering and outputing tuples. It is schema-less, requiring only ssim encoding
of attributes on stdin. "Ssimfilt a b:c d:e ..." matches only tuples with typetag "a", and where attribute "b" matches "c"
and "d" matches "e". a,b,c,d,e are all regular expressions. If typetag is "^", it is locked to the first typetag
that appears in the input and doesn't change after that.

This is is for input & filtering. For output, user can choose which fields to select for printing with "-field:..."
and the format. With `-format:ssim` (default), ssim tuples are printed back. With `-format:field`, raw field values are printed.
With `-cmd:...`, ssimfilt makes all tuple attributes available to the specified command as variables; pipe the output
of ssimfilt through bash to execute arbitrary command for each tuple.

### Examples
<a href="#examples"></a>
#### Example: Format ssim input as table
<a href="#example-format-ssim-input-as-table"></a>

```
inline-command: acr field:command.ssimfilt.% | head | ssimfilt ^ -t
FIELD                     ARG           REFTYPE  DFLT    COMMENT
command.ssimfilt.in       algo.cstring  Val      "data"  Input directory or filename, - for stdin
command.ssimfilt.typetag  algo.cstring  RegxSql  "%"     (filter) Match typetag. ^=first encountered typetag
command.ssimfilt.match    algo.cstring  Tary             (filter) Select input tuple if value of key matches value (regx:regx)
command.ssimfilt.field    algo.cstring  Tary             (project) Select fields for output (regx)
command.ssimfilt.format   u8            Val      0       Output format for selected tuples
command.ssimfilt.t        bool          Val      false   Alias for -format:table
command.ssimfilt.cmd      algo.cstring  Val      ""      Command to output
command.ssimfilt.f        algo.cstring  Val      ""      Alias for -field:<f> -format:field

```

#### Example: Extract field
<a href="#example-extract-field"></a>

```
inline-command: echo $'blah a:b\nblah a:c' | ssimfilt -field a -format field
b
c
```

#### Example: Convert ssim to Json
<a href="#example-convert-ssim-to-json"></a>

```
inline-command: echo $'blah a:b\nblah a:c' | ssimfilt  -format json
{"@type":"blah","a":"b"}
{"@type":"blah","a":"c"}
```

#### Example: Convert ssim to Markdown table
<a href="#example-convert-ssim-to-markdown-table"></a>

```
inline-command: echo $'blah a:b\nblah a:c' | ssimfilt  -format mdtable
|A|
|---|
|b|
|c|

```

#### Example: Find other uses in documentation
<a href="#example-find-other-uses-in-documentation"></a>

```
grep -R 'command.*ssimfilt' txt/
```

### Options
<a href="#options"></a>
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

#### -typetag -- (filter) Match typetag. ^=first encountered typetag
<a href="#-typetag"></a>

An input tuple is first matched as a regx against the typetag to determine if it should be printed.
The special value "^" picks the first input tuple as the filter. This is useful with csv
output format which is rectangular in nature, i.e. record types cannot be mixed.

#### -match -- (filter) Select input tuple if value of key matches value (regx:regx)
<a href="#-match"></a>

This option can be specified multiple tuples. It is a pair of regexes, one for key and one for value
Each input tuple must match each `-match` argument for it to be selected for output.

#### -field -- (project) Select fields for output (regx)
<a href="#-field"></a>

Select which fields from the input tuple to print (regx).

#### -format -- Output format for selected tuples
<a href="#-format"></a>

Select an output format.
Available formats are
- ssim: print as ssim tuple (default)
- csv: print as csv file. first line of input determines which header to print, and locks in the typetag.
- field: print selected field values, one per line
- cmd: output a script suitable for piping to bash. the `-cmd` option specifies which command to include.
All field values are available to the command as bash variables
- table: print a nicely aligned ascii table
- mdtable: print a pipe delimited markdown table with the header

#### -t -- Alias for -format:table
<a href="#-t"></a>

#### -cmd -- Command to output
<a href="#-cmd"></a>

E.g.
```
acr field | ssimfilt -cmd 'echo $field/$arg' | bash
```

#### -f -- Alias for -field:<f> -format:field
<a href="#-f"></a>

### Inputs
<a href="#inputs"></a>
`ssimfilt` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|
|[amcdb.bltin](/txt/ssimdb/amcdb/bltin.md)|Specify properties of a C built-in type|
|[dmmeta.cdflt](/txt/ssimdb/dmmeta/cdflt.md)|Specify default value for single-value types that lack fields|
|[dmmeta.cfmt](/txt/ssimdb/dmmeta/cfmt.md)|Specify options for printing/reading ctypes into multiple formats|
|[dmmeta.cppfunc](/txt/ssimdb/dmmeta/cppfunc.md)|Value of field provided by this expression|
|[dmmeta.ctype](/txt/ssimdb/dmmeta/ctype.md)|Struct|
|[dmmeta.fconst](/txt/ssimdb/dmmeta/fconst.md)|Specify enum value (integer + string constant) for a field|
|[dmmeta.field](/txt/ssimdb/dmmeta/field.md)|Specify field of a struct|
|[dmmeta.ftuple](/txt/ssimdb/dmmeta/ftuple.md)||
|[dmmeta.sqltype](/txt/ssimdb/dmmeta/sqltype.md)|Mapping of ctype -> SQL expression|
|[dmmeta.ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|File with ssim tuples|
|[dmmeta.substr](/txt/ssimdb/dmmeta/substr.md)|Specify that the field value is computed from a substring of another field|
|[dev.unstablefld](/txt/ssimdb/dev/unstablefld.md)|Fields that should be stripped from component test output because they contain timestamps etc.|
|[dev.unstablefld](/txt/ssimdb/dev/unstablefld.md)|Fields that should be stripped from component test output because they contain timestamps etc.|

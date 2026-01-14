## ssimfilt - Tuple utility


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Toc -->
&nbsp;&nbsp;&bull;&nbsp;  [Syntax](#syntax)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Options](#options)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
&#128196; [ssimfilt - Internals](/txt/exe/ssimfilt/internals.md)<br/>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Toc -->

### Syntax
<a href="#syntax"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Syntax -->
```
ssimfilt: Tuple utility
Usage: ssimfilt [[-typetag:]<regx>] [[-match:]<string>] [options]
    OPTION      TYPE    DFLT    COMMENT
    -in         string  "data"  Input directory or filename, - for stdin
    [typetag]   regx    "%"     (filter) Match typetag. ^=first encountered typetag
    [match]...  string          (filter) Select input tuple if value of key matches value (regx:regx)
    -field...   string          (project) Select fields for output (regx)
    -format     enum    ssim    Output format for selected tuples (ssim|csv|field|cmd|json|stablefld|table|mdtable)
                                    ssim  Print selected/filtered tuples
                                    csv  First tuple determines header. CSV quoting is used. Newlines are removed
                                    field  Print selected fields, one per line
                                    cmd  Emit command for each tuple (implied if -cmd is set)
                                    json  Print JSON object for each tuple
                                    stablefld  Filter unstable fields, leave the rest intact
                                    table  ASCII table for each group of tuples
                                    mdtable  ASCII Markdown table with | separators for each group of tuples
    -t                          Alias for -format:table
    -cmd        string  ""      Command to output
    -verbose    flag            Verbosity level (0..255); alias -v; cumulative
    -debug      flag            Debug level (0..255); alias -d; cumulative
    -help                       Print help and exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig

```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Syntax -->

### Options
<a href="#options"></a>

<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Options -->
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

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Options -->

### Inputs
<a href="#inputs"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Inputs -->
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

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Inputs -->


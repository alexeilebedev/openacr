## ssimfilt - Tuple utility


### Table Of Contents
<a href="#table-of-contents"></a>
* [Syntax](#syntax)
* [Options](#options)
* [Sources](#sources)
* [Inputs](#inputs)
* [Tests](#tests)

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
    -format     int     ssim    Output format for selected tuples (ssim|csv|field|cmd|json|table)
                                    ssim  Print selected/filtered tuples
                                    csv  First tuple determines header. CSV quoting is used. Newlines are removed
                                    field  Print selected fields, one per line
                                    cmd  Emit command for each tuple (implied if -cmd is set)
                                    json  Print JSON object for each tuple
                                    table  ASCII table for each group of tuples
    -t                          Alias for -format:table
    -cmd        string  ""      Command to output
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

#### -t -- Alias for -format:table
<a href="#-t"></a>

#### -cmd -- Command to output
<a href="#-cmd"></a>

E.g.
```
acr field | ssimfilt -cmd 'echo $field/$arg' | bash
```

### Sources
<a href="#sources"></a>
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/gen/ssimfilt_gen.cpp](/cpp/gen/ssimfilt_gen.cpp)||
|[cpp/ssimfilt.cpp](/cpp/ssimfilt.cpp)||
|[include/gen/ssimfilt_gen.h](/include/gen/ssimfilt_gen.h)||
|[include/gen/ssimfilt_gen.inl.h](/include/gen/ssimfilt_gen.inl.h)||
|[include/ssimfilt.h](/include/ssimfilt.h)||

### Inputs
<a href="#inputs"></a>
`ssimfilt` takes the following tables on input:
|ssimfile|comment|
|---|---|
|[dmmeta.ctype](/txt/ssimdb/dmmeta/ctype.md)|Struct|
|[dmmeta.field](/txt/ssimdb/dmmeta/field.md)|Specify field of a struct|
|[dev.unstablefld](/txt/ssimdb/dev/unstablefld.md)|Fields that should be stripped from component test output because they contain timestamps etc.|
|[dmmeta.substr](/txt/ssimdb/dmmeta/substr.md)|Specify that the field value is computed from a substring of another field|
|[dmmeta.ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|File with ssim tuples|
|[dmmeta.sqltype](/txt/ssimdb/dmmeta/sqltype.md)|Mapping of ctype -> SQL expression|
|[dmmeta.ftuple](/txt/ssimdb/dmmeta/ftuple.md)||
|[dmmeta.fconst](/txt/ssimdb/dmmeta/fconst.md)|Specify enum value (integer + string constant) for a field|
|[dmmeta.cppfunc](/txt/ssimdb/dmmeta/cppfunc.md)|Value of field provided by this expression|
|[dmmeta.cfmt](/txt/ssimdb/dmmeta/cfmt.md)|Specify options for printing/reading ctypes into multiple formats|
|[dmmeta.cdflt](/txt/ssimdb/dmmeta/cdflt.md)|Specify default value for single-value types that lack fields|
|[amcdb.bltin](/txt/ssimdb/amcdb/bltin.md)|Specify properties of a C built-in type|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|

### Tests
<a href="#tests"></a>
The following component tests are defined for `ssimfilt`.
These can be executed with `atf_comp <comptest> -v`
|COMPTEST|COMMENT|
|---|---|
|ssimfilt.Csv||
|ssimfilt.CsvField|CSV + field selection|
|ssimfilt.FirstTag|Lock typetag to first input tuple|
|ssimfilt.Json|Convert test data to json|
|ssimfilt.JsonRecursive|Print Recursive json object|
|ssimfilt.MatchField|Select fields by value|
|ssimfilt.MatchTag|Match typetag|
|ssimfilt.SelectField|Select several fields|
|ssimfilt.Table|Tabular output|


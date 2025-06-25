## aqlite - Runs sqlite queries against ssim files


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Toc -->
&nbsp;&nbsp;&bull;&nbsp;  [Syntax](#syntax)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Options](#options)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Sources](#sources)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Tests](#tests)<br/>
&#128196; [aqlite - Internals](/txt/exe/aqlite/internals.md)<br/>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Toc -->

### Syntax
<a href="#syntax"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Syntax -->
```
aqlite: Runs sqlite queries against ssim files
Usage: aqlite [-cmd:]<string> [options]
    OPTION      TYPE    DFLT    COMMENT
    -in         string  "data"  Input directory or filename, - for stdin
    -schema     string  "data"  Schema dir
    [cmd]       string          Sql Query to run
    -verbose    int             Verbosity level (0..255); alias -v; cumulative
    -debug      int             Debug level (0..255); alias -d; cumulative
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

#### -schema -- Schema dir
<a href="#-schema"></a>

#### -cmd -- Sql Query to run
<a href="#-cmd"></a>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Options -->

### Inputs
<a href="#inputs"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Inputs -->
`aqlite` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dmmeta.ns](/txt/ssimdb/dmmeta/ns.md)|Namespace (for in-memory database, protocol, etc)|
|[dmmeta.ctype](/txt/ssimdb/dmmeta/ctype.md)|Struct|
|[dmmeta.field](/txt/ssimdb/dmmeta/field.md)|Specify field of a struct|
|[dmmeta.substr](/txt/ssimdb/dmmeta/substr.md)|Specify that the field value is computed from a substring of another field|
|[dmmeta.ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|File with ssim tuples|
|[dmmeta.sqltype](/txt/ssimdb/dmmeta/sqltype.md)|Mapping of ctype -> SQL expression|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Inputs -->

### Sources
<a href="#sources"></a>
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/aqlite/aqlite.cpp](/cpp/aqlite/aqlite.cpp)||
|[cpp/gen/aqlite_gen.cpp](/cpp/gen/aqlite_gen.cpp)||
|[include/aqlite.h](/include/aqlite.h)||
|[include/gen/aqlite_gen.h](/include/gen/aqlite_gen.h)||
|[include/gen/aqlite_gen.inl.h](/include/gen/aqlite_gen.inl.h)||

### Tests
<a href="#tests"></a>
The following component tests are defined for `aqlite`.
These can be executed with `atf_comp <comptest> -v`
|COMPTEST|COMMENT|
|---|---|
|aqlite.Number||
|aqlite.Smoke||


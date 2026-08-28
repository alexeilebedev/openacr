## aqlite - Runs sqlite queries against ssim files


### Syntax
<a href="#syntax"></a>
```usage
aqlite: Runs sqlite queries against ssim files
Usage: aqlite [-cmd:]<string> [options]
    OPTION      TYPE    DFLT      COMMENT
    -in         string  "data"    Input directory or filename, - for stdin
    -schema     string  "data"    Schema dir
    [cmd]       string            Sql Query to run
    -ns         regx    "dmmeta"  Regx of databases to attach
    -verbose    flag              Verbosity level (0..255); alias -v; cumulative
    -debug      flag              Debug level (0..255); alias -d; cumulative
    -help                         Print help and exit; alias -h
    -version                      Print version and exit
    -signature                    Show signatures and exit; alias -sig
```

### Tests
<a href="#tests"></a>
The following component tests are defined for `aqlite`.
These can be executed with `atf_comp <comptest> -v`
|COMPTEST|COMMENT|
|---|---|
|aqlite.Number||
|aqlite.Smoke||

### Options
<a href="#options"></a>
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

#### -schema -- Schema dir
<a href="#-schema"></a>

#### -cmd -- Sql Query to run
<a href="#-cmd"></a>

#### -ns -- Regx of databases to attach
<a href="#-ns"></a>

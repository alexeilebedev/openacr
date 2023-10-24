## ssimfilt: Tuple utility



### Syntax

```
ssimfilt: Tuple utility
Usage: ssimfilt [[-typetag:]<regx>] [[-match:]<string>] [options]
    OPTION      TYPE    DFLT    COMMENT
    -in         string  "data"  Input directory or filename, - for stdin
    [typetag]   regx    "%"     (filter) Match typetag. ^=first encountered typetag
    [match]...  string          (filter) Select input tuple if value of key matches value (regx:regx)
    -field...   string          (project) Select fields for output (regx)
    -format     int     ssim    Output format for selected tuples (ssim|csv|field|cmd|table)
                                    ssim  Print selected/filtered tuples
                                    csv  First tuple determines header. CSV quoting is used. Newlines are removed
                                    field  Print selected fields, one per line
                                    cmd  Emit command for each tuple (implied if -cmd is set)
                                    table  ASCII table for each group of tuples
    -t                          Alias for -format:table
    -cmd        string  ""      Command to output
    -verbose    int             Verbosity level (0..255); alias -v; cumulative
    -debug      int             Debug level (0..255); alias -d; cumulative
    -help                       Print help and exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig

```

### Inputs

`ssimfilt` takes the following tables on input:
```
CTYPE                COMMENT
dmmeta.Dispsigcheck  Check signature of input data against executable's version
```

### Tests

The following component tests are defined for `ssimfilt`.
These can be executed with `atf_comp <comptest> -v`
```
COMPTEST              COMMENT
ssimfilt.Csv
ssimfilt.FirstTag     Lock typetag to first input tuple
ssimfilt.MatchField
ssimfilt.MatchTag
ssimfilt.SelectField
ssimfilt.Table



```


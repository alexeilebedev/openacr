## amc_gc: Empirically eliminate unused records



### Syntax

```
amc_gc: Empirically eliminate unused records
Usage: amc_gc [options]
    OPTION      TYPE    DFLT    COMMENT
    -target     regx    "%"     Target to test-build
    -key        regx    ""      ACR query selecting records to eliminate, e.g. dmmeta.ctype:amc.%
    -include                    Garbage collect includes for specified target
    -in         string  "data"  Input directory or filename, - for stdin
    -verbose    int             Verbosity level (0..255); alias -v; cumulative
    -debug      int             Debug level (0..255); alias -d; cumulative
    -help                       Print help an exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig

```


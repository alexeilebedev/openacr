## samp_regx:



### Syntax

```
Usage: samp_regx [-expr:]<string> [[-string:]<string>] [options]
    OPTION      TYPE    DFLT    COMMENT
    -in         string  "data"  Input directory or filename, - for stdin
    [expr]      string          Expression
    -style      int     acr     Regx style (acr|shell|classic|literal)
                                    acr  ACR-style regx
                                    shell  Shell-style regx
                                    classic  Classic regx
                                    literal  Literal string
    -match                      Match a string
    [string]    string  ""      String to match
    -show                       Show regx innards
    -verbose    int             Verbosity level (0..255); alias -v; cumulative
    -debug      int             Debug level (0..255); alias -d; cumulative
    -help                       Print help and exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig

```

### Inputs

`samp_regx` takes the following tables on input:
```
CTYPE                COMMENT
dmmeta.Dispsigcheck  Check signature of input data against executable's version
```


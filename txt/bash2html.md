## bash2html: Convert bash output and colours to html



### Syntax

```
bash2html: Convert bash output and colours to html
Usage: bash2html [options]
    OPTION      TYPE    DFLT    COMMENT
    -in         string  "data"  Input directory or filename, - for stdin
    -test                       Produce Test Output
    -verbose    int             Verbosity level (0..255); alias -v; cumulative
    -debug      int             Debug level (0..255); alias -d; cumulative
    -help                       Print help and exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig

```

### Inputs

`bash2html` takes the following tables on input:
```
CTYPE                COMMENT
dmmeta.Dispsigcheck  Check signature of input data against executable's version
```


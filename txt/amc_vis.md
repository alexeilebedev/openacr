## amc_vis: amc: draw access path diagrams



### Syntax

```
amc_vis: amc: draw access path diagrams
Usage: amc_vis [[-ctype:]<regx>] [options]
    OPTION      TYPE    DFLT    COMMENT
    [ctype]     regx    "%"     Ctype regexp to compute access path diagram
    -in         string  "data"  Input directory or filename, - for stdin
    -dot        string  ""      Save dot file with specified filename
    -xref                       Include all ctypes referenced by selected ones
    -xns                        Cross namespace boundaries
    -noinput                    Deselect module inputs
    -check                      Check model for dependency problems
    -render             Y       Produce an ascii drawing
    -verbose    int             Verbosity level (0..255); alias -v; cumulative
    -debug      int             Debug level (0..255); alias -d; cumulative
    -help                       Print help and exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig

```

### Inputs

`amc_vis` takes the following tables on input:
```
CTYPE                COMMENT
dmmeta.Reftype
dmmeta.Ctype         Conceptual type (or C type)
dmmeta.Field         Specify field of a struct
dmmeta.Finput        Describe input table of a program
dmmeta.Dispsigcheck  Check signature of input data against executable's version
```


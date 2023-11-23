## atf_nrun: Run N subprocesses in parallel



### Syntax

```
atf_nrun: Run N subprocesses in parallel
Usage: atf_nrun [[-ncmd:]<int>] [options]
    OPTION      TYPE    DFLT    COMMENT
    -in         string  "data"  Input directory or filename, - for stdin
    -maxjobs    int     2       Number of simultaneous jobs
    [ncmd]      int     6
    -verbose    int             Verbosity level (0..255); alias -v; cumulative
    -debug      int             Debug level (0..255); alias -d; cumulative
    -help                       Print help and exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig

```

### Inputs

`atf_nrun` takes the following tables on input:
```
CTYPE                COMMENT
dmmeta.Dispsigcheck  Check signature of input data against executable's version
```


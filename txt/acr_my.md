## acr_my: ACR <-> MariaDB adaptor



### Syntax

```
acr_my: ACR <-> MariaDB adaptor
Usage: acr_my [[-nsdb:]<regx>] [options]
    OPTION      TYPE    DFLT    COMMENT
    [nsdb]      regx    ""      Regx of ssim namespace (dmmeta.nsdb) to select
    -in         string  "data"  Input directory or filename, - for stdin
    -schema     string  "data"  Input directory or filename, - for stdin
    -fldfunc                    Evaluate fldfunc when printing tuple
    -fkey                       Enable foreign key constraints
    -e                          Alias for -start -shell -stop
    -start                      Start local mysql server
    -stop                       Stop local mysql server, saving data
    -abort                      Abort local mysql server, losing data
    -shell                      Connect to local mysql server
    -serv                       Start mysql with TCP/IP service enabled
    -verbose    int             Verbosity level (0..255); alias -v; cumulative
    -debug      int             Debug level (0..255); alias -d; cumulative
    -help                       Print help and exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig

```

### Inputs

`acr_my` takes the following tables on input:
```
CTYPE                COMMENT
dmmeta.Dispsigcheck  Check signature of input data against executable's version
dmmeta.Nsdb          Annotate ssimdb namespaces
dmmeta.Ssimfile      Ssim tuple name for structure
```


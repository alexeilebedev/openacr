## atf_gcli: test harness for gcli



### Syntax

```
atf_gcli: test harness for gcli
Usage: atf_gcli [options]
    OPTION          TYPE    DFLT    COMMENT
    -in             string  "data"  Input directory or filename, - for stdin
    -gtblacttst     regx    "%"     gcli tests regx
    -id             string  ""      Use this issue for $ISSUE substitutions
    -mr             string  ""      Use this issue for $MR substitutions
    -note           string  ""      Use this issue for $NOTE substitutions
    -capture                        Capture test command output into gverbtout
    -skip_init                      Skip setting local files - already set
    -skip_git_init                  Skip setting local files - already set
    -dry_run                        Print actions, do not perform
    -verbose        int             Verbosity level (0..255); alias -v; cumulative
    -debug          int             Debug level (0..255); alias -d; cumulative
    -help                           Print help an exit; alias -h
    -version                        Print version and exit
    -signature                      Show signatures and exit; alias -sig

```

### Inputs

`atf_gcli` takes the following tables on input:
```
CTYPE                 COMMENT
gclidb.Gclienv
gclidb.Gtblact
gclidb.Gtblacttst
gclidb.Gtblacttstout
gclidb.Gclienvsub
```


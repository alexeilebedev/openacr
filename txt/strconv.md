## strconv: A simple string utility



### Syntax

```
strconv: A simple string utility
Usage: strconv [-str:]<string> [options]
    OPTION         TYPE    DFLT    COMMENT
    [str]          string          String parameter
    -tocamelcase                   Convert string to camel case
    -tolowerunder                  Convert string to lower-under
    -in            string  "data"  Input directory or filename, - for stdin
    -pathcomp      string  ""      Extract path component from string
    -verbose       int             Verbosity level (0..255); alias -v; cumulative
    -debug         int             Debug level (0..255); alias -d; cumulative
    -help                          Print help an exit; alias -h
    -version                       Print version and exit
    -signature                     Show signatures and exit; alias -sig

```


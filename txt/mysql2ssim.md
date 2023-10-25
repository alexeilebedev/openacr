## mysql2ssim: mysql -> ssim conversion tool



### Syntax

```
mysql2ssim: mysql -> ssim conversion tool
Usage: mysql2ssim [-url:]<string> [[-tables:]<string>] [options]
    OPTION          TYPE    DFLT    COMMENT
    -writessimfile                  Write to ssimfile directly
    [url]           string          user:pass@host/db or sock:///filename/db
    [tables]        string  ""      comma-separated list of tables. Default is all tables
    -schema                         Generate ssim type definition
    -in             string  "data"  Input directory or filename, - for stdin
    -pretty                         Format output for the screen
    -nologo                         Don't show copyright notice
    -baddbok                        Don't claim if bad database
    -verbose        int             Verbosity level (0..255); alias -v; cumulative
    -debug          int             Debug level (0..255); alias -d; cumulative
    -help                           Print help an exit; alias -h
    -version                        Print version and exit
    -signature                      Show signatures and exit; alias -sig

```


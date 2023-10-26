## atf_ws: Component test driver for lib_ws



### Syntax

```
atf_ws: Component test driver for lib_ws
Usage: atf_ws [options]
    OPTION            TYPE    DFLT    COMMENT
    -in               string  "data"  Input directory or filename, - for stdin
    -randomize_ports          Y       Randomize port numbers
    -verbose          int             Verbosity level (0..255); alias -v; cumulative
    -debug            int             Debug level (0..255); alias -d; cumulative
    -help                             Print help an exit; alias -h
    -version                          Print version and exit
    -signature                        Show signatures and exit; alias -sig

```

### Tests

The following component tests are defined for `atf_ws`:
```
atf_ws.ServerStartStop	
atf_ws.Smoke	
```


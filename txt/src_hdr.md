## src_hdr: Update source file / copyright header



### Syntax

```
src_hdr: Update source file / copyright header
Usage: src_hdr [options]
    OPTION             TYPE    DFLT    COMMENT
    -in                string  "data"  Input directory or filename, - for stdin
    -targsrc           regx    ""      Regx of targsrc to update
    -write                             Update files in-place
    -indent                            Indent source files
    -update_copyright  string  ""      Update copyright notice for specified company with current year
    -scriptfile        regx    ""      Regx of scripts to update header
    -verbose           int             Verbosity level (0..255); alias -v; cumulative
    -debug             int             Debug level (0..255); alias -d; cumulative
    -help                              Print help and exit; alias -h
    -version                           Print version and exit
    -signature                         Show signatures and exit; alias -sig

```

### Inputs

`src_hdr` takes the following tables on input:
```
CTYPE                COMMENT
dev.License
dmmeta.Ns            Namespace (for in-memory database, protocol, etc)
dev.Target           Build target
dev.Targsrc          List of sources for target
dev.Scriptfile
dmmeta.Nsx           Control code-generation and exception handling options for process/library
dmmeta.Dispsigcheck  Check signature of input data against executable's version
```


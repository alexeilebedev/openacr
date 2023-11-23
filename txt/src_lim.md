## src_lim: Enforce line length, function length, and indentation limits



### Syntax

```
src_lim: Enforce line length, function length, and indentation limits
Usage: src_lim [options]
    OPTION      TYPE    DFLT    COMMENT
    -in         string  "data"  Input directory or filename, - for stdin
    -linelim                    Check various file limits (line length, function length, etc)
    -srcfile    regx    "%"     Filter for source files to process
    -strayfile                  Check for unregistered source files
    -capture                    Generate new dev.linelim records
    -write                      Update ssim database (with -capture)
    -badchar                    Check for bad chars in source files
    -badline    regx    ""      Check badline (acr badline)
    -verbose    int             Verbosity level (0..255); alias -v; cumulative
    -debug      int             Debug level (0..255); alias -d; cumulative
    -help                       Print help and exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig

```

### Inputs

`src_lim` takes the following tables on input:
```
CTYPE                COMMENT
dev.Gitfile
dev.Targsrc          List of sources for target
dev.Linelim
dev.Include          A site where one file includes another
dev.Badline
dmmeta.Dispsigcheck  Check signature of input data against executable's version
```


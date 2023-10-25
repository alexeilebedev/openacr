## atf_ci: Normalization tests (see citest table)



### Syntax

```
atf_ci: Normalization tests (see citest table)
Usage: atf_ci [[-citest:]<regx>] [options]
    OPTION      TYPE    DFLT    COMMENT
    -in         string  "data"  Input directory or filename, - for stdin
    [citest]    regx    "%"     Regx of tests to run
    -maxerr     int     0       Exit after this many errors
    -cijob      regx    "%"
    -capture                    Capture the output of the test
    -verbose    int             Verbosity level (0..255); alias -v; cumulative
    -debug      int             Debug level (0..255); alias -d; cumulative
    -help                       Print help an exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig

```

### Inputs

`atf_ci` takes the following tables on input:
```
CTYPE            COMMENT
dmmeta.Ns        Namespace (for in-memory database, protocol, etc)
dmmeta.Ctype     Conceptual type (or C type)
dmmeta.Field     Specify field of a struct
dev.Unstablefld
dmmeta.Substr    Specify that the field value is computed from a substring of another field
dmmeta.Ftuple
dmmeta.Fconst    Specify enum value (integer + string constant) for a field
dmmeta.Cppfunc   Value of field provided by this expression
dmmeta.Cfmt      Specify options for printing/reading ctypes into multiple formats
dmmeta.Cdflt     Specify default value for single-value types that lack fields
dev.Gitfile
dev.Targsrc      List of sources for target
dmmeta.Ssimfile  Ssim tuple name for structure
dev.Ssimfs
dev.Scriptfile
dev.Readme
dev.Noindent
dev.Msgfile
dclidb.Dctr      Container
dev.Cfg
dev.Builddir     Directory where object files/executables go. Determines compile/link options
```


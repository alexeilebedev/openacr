## ssim2mysql: Ssim -> mysql



### Syntax

```
ssim2mysql: Ssim -> mysql
Usage: ssim2mysql [options]
    OPTION      TYPE    DFLT    COMMENT
    -url        string  ""      URL of mysql server. user:pass@hostb or sock://filename; Empty -> stdout
    -data_dir   string  "data"  Load dmmeta info from this directory
    -maxpacket  int     100000  Max Mysql packet size
    -replace            Y       use REPLACE INTO instead of INSERT INTO
    -trunc                      Truncate target table
    -dry_run                    Print SQL commands to the stdout
    -fldfunc                    create columns for fldfuncs
    -in         string  "-"     Input directory or filename, - for stdin
    -db         string  ""      Optional database name
    -createdb                   Emit CREATE DATABASE code for namespace specified with <db>
    -fkey                       Enable foreign key constraints (uses InnoDB storage engine)
    -verbose    int             Verbosity level (0..255); alias -v; cumulative
    -debug      int             Debug level (0..255); alias -d; cumulative
    -help                       Print help and exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig

```

### Inputs

`ssim2mysql` takes the following tables on input:
```
CTYPE                COMMENT
dmmeta.Ns            Namespace (for in-memory database, protocol, etc)
dmmeta.Ctype         Conceptual type (or C type)
dmmeta.Field         Specify field of a struct
dmmeta.Substr        Specify that the field value is computed from a substring of another field
dmmeta.Ssimfile      Ssim tuple name for structure
dmmeta.Sqltype
dmmeta.Dispsigcheck  Check signature of input data against executable's version
```


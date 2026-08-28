## ssim2mysql - Ssim -> mysql


### Syntax
<a href="#syntax"></a>
```usage
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
    -verbose    flag            Verbosity level (0..255); alias -v; cumulative
    -debug      flag            Debug level (0..255); alias -d; cumulative
    -help                       Print help and exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig
```

### Options
<a href="#options"></a>
#### -url -- URL of mysql server. user:pass@hostb or sock://filename; Empty -> stdout
<a href="#-url"></a>

#### -data_dir -- Load dmmeta info from this directory
<a href="#-data_dir"></a>

#### -maxpacket -- Max Mysql packet size
<a href="#-maxpacket"></a>

#### -replace -- use REPLACE INTO instead of INSERT INTO
<a href="#-replace"></a>

#### -trunc -- Truncate target table
<a href="#-trunc"></a>

#### -dry_run -- Print SQL commands to the stdout
<a href="#-dry_run"></a>

#### -fldfunc -- create columns for fldfuncs
<a href="#-fldfunc"></a>

#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

#### -db -- Optional database name
<a href="#-db"></a>

#### -createdb -- Emit CREATE DATABASE code for namespace specified with <db>
<a href="#-createdb"></a>

#### -fkey -- Enable foreign key constraints (uses InnoDB storage engine)
<a href="#-fkey"></a>

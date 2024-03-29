## ssim2mysql - Ssim -> mysql


### Table Of Contents
<a href="#table-of-contents"></a>
* [Syntax](#syntax)
* [Options](#options)
* [Sources](#sources)
* [Inputs](#inputs)

### Syntax
<a href="#syntax"></a>
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

### Sources
<a href="#sources"></a>
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/gen/ssim2mysql_gen.cpp](/cpp/gen/ssim2mysql_gen.cpp)||
|[cpp/ssim2mysql.cpp](/cpp/ssim2mysql.cpp)||
|[include/gen/ssim2mysql_gen.h](/include/gen/ssim2mysql_gen.h)||
|[include/gen/ssim2mysql_gen.inl.h](/include/gen/ssim2mysql_gen.inl.h)||

### Inputs
<a href="#inputs"></a>
`ssim2mysql` takes the following tables on input:
|ssimfile|comment|
|---|---|
|[dmmeta.ns](/txt/ssimdb/dmmeta/ns.md)|Namespace (for in-memory database, protocol, etc)|
|[dmmeta.ctype](/txt/ssimdb/dmmeta/ctype.md)|Struct|
|[dmmeta.field](/txt/ssimdb/dmmeta/field.md)|Specify field of a struct|
|[dmmeta.substr](/txt/ssimdb/dmmeta/substr.md)|Specify that the field value is computed from a substring of another field|
|[dmmeta.ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|File with ssim tuples|
|[dmmeta.sqltype](/txt/ssimdb/dmmeta/sqltype.md)|Mapping of ctype -> SQL expression|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|


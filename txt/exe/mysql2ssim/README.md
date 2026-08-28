## mysql2ssim - mysql -> ssim conversion tool


### Syntax
<a href="#syntax"></a>
```usage
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
    -verbose        flag            Verbosity level (0..255); alias -v; cumulative
    -debug          flag            Debug level (0..255); alias -d; cumulative
    -help                           Print help and exit; alias -h
    -version                        Print version and exit
    -signature                      Show signatures and exit; alias -sig
```

### Options
<a href="#options"></a>
#### -writessimfile -- Write to ssimfile directly
<a href="#-writessimfile"></a>

#### -url -- user:pass@host/db or sock:///filename/db
<a href="#-url"></a>

#### -tables -- comma-separated list of tables. Default is all tables
<a href="#-tables"></a>

#### -schema -- Generate ssim type definition
<a href="#-schema"></a>

#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

#### -pretty -- Format output for the screen
<a href="#-pretty"></a>

#### -nologo -- Don't show copyright notice
<a href="#-nologo"></a>

#### -baddbok -- Don't claim if bad database
<a href="#-baddbok"></a>

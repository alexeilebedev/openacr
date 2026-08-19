## mysql2ssim - mysql -> ssim conversion tool


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- abt_md.toc_beg -->
&nbsp;&nbsp;&bull;&nbsp;  [Internals](#internals)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Syntax](#syntax)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Options](#options)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
<!-- abt_md.toc_end -->

### Internals
<a href="#internals"></a>
&#128196; [mysql2ssim - Internals](/txt/gen/mysql2ssim/mysql2ssim.md)<br/>

### Syntax
<a href="#syntax"></a>
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

### Inputs
<a href="#inputs"></a>
`mysql2ssim` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|

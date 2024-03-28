## ssim2csv - Ssim -> csv conversion tool


### Table Of Contents
<a href="#table-of-contents"></a>
* [Syntax](#syntax)
* [Options](#options)
* [Sources](#sources)
* [Inputs](#inputs)

### Syntax
<a href="#syntax"></a>
```
ssim2csv: Ssim -> csv conversion tool
Usage: ssim2csv [options]
    OPTION        TYPE    DFLT  COMMENT
    -expand       string  ""
    -ignoreQuote
    -verbose      int           Verbosity level (0..255); alias -v; cumulative
    -debug        int           Debug level (0..255); alias -d; cumulative
    -help                       Print help and exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig

```

### Options
<a href="#options"></a>

#### -expand -- 
<a href="#-expand"></a>

#### -ignoreQuote -- 
<a href="#-ignorequote"></a>

### Sources
<a href="#sources"></a>
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/gen/ssim2csv_gen.cpp](/cpp/gen/ssim2csv_gen.cpp)||
|[cpp/ssim2csv.cpp](/cpp/ssim2csv.cpp)||
|[include/gen/ssim2csv_gen.h](/include/gen/ssim2csv_gen.h)||
|[include/gen/ssim2csv_gen.inl.h](/include/gen/ssim2csv_gen.inl.h)||

### Inputs
<a href="#inputs"></a>
`ssim2csv` takes the following tables on input:
|ssimfile|comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|


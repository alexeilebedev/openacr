## ssim2csv - Ssim -> csv conversion tool


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
&#128196; [ssim2csv - Internals](/txt/gen/ssim2csv/ssim2csv.md)<br/>

### Syntax
<a href="#syntax"></a>
```
ssim2csv: Ssim -> csv conversion tool
Usage: ssim2csv [options]
    OPTION        TYPE    DFLT  COMMENT
    -expand       string  ""
    -ignoreQuote
    -verbose      flag          Verbosity level (0..255); alias -v; cumulative
    -debug        flag          Debug level (0..255); alias -d; cumulative
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

### Inputs
<a href="#inputs"></a>
`ssim2csv` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|

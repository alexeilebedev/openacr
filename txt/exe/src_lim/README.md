## src_lim - Enforce line length, function length, and indentation limits


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Toc -->
&nbsp;&nbsp;&bull;&nbsp;  [Syntax](#syntax)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Options](#options)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
&#128196; [src_lim - Internals](/txt/exe/src_lim/internals.md)<br/>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Toc -->

### Syntax
<a href="#syntax"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Syntax -->
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
    -verbose    flag            Verbosity level (0..255); alias -v; cumulative
    -debug      flag            Debug level (0..255); alias -d; cumulative
    -help                       Print help and exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig

```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Syntax -->

### Options
<a href="#options"></a>

<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Options -->
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

#### -linelim -- Check various file limits (line length, function length, etc)
<a href="#-linelim"></a>

#### -srcfile -- Filter for source files to process
<a href="#-srcfile"></a>

#### -strayfile -- Check for unregistered source files
<a href="#-strayfile"></a>

#### -capture -- Generate new dev.linelim records
<a href="#-capture"></a>

#### -write -- Update ssim database (with -capture)
<a href="#-write"></a>

#### -badchar -- Check for bad chars in source files
<a href="#-badchar"></a>

#### -badline -- Check badline (acr badline)
<a href="#-badline"></a>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Options -->

### Inputs
<a href="#inputs"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Inputs -->
`src_lim` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|
|[dev.badline](/txt/ssimdb/dev/badline.md)|Regex of a dubious source code line|
|[dev.gitfile](/txt/ssimdb/dev/gitfile.md)|File managed by git|
|[dev.include](/txt/ssimdb/dev/include.md)|A site where one file includes another|
|[dev.linelim](/txt/ssimdb/dev/linelim.md)||
|[dev.targsrc](/txt/ssimdb/dev/targsrc.md)|List of sources for target|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Inputs -->


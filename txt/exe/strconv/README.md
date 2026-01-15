## strconv - A simple string utility


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Toc -->
&nbsp;&nbsp;&bull;&nbsp;  [Syntax](#syntax)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Options](#options)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
&#128196; [strconv - Internals](/txt/exe/strconv/internals.md)<br/>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Toc -->

### Syntax
<a href="#syntax"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Syntax -->
```
strconv: A simple string utility
Usage: strconv [-str:]<string> [options]
    OPTION         TYPE    DFLT    COMMENT
    [str]          string          String parameter
    -tocamelcase                   Convert string to camel case
    -tolowerunder                  Convert string to lower-under
    -in            string  "data"  Input directory or filename, - for stdin
    -pathcomp      string  ""      Extract path component from string
    -verbose       flag            Verbosity level (0..255); alias -v; cumulative
    -debug         flag            Debug level (0..255); alias -d; cumulative
    -help                          Print help and exit; alias -h
    -version                       Print version and exit
    -signature                     Show signatures and exit; alias -sig

```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Syntax -->

### Options
<a href="#options"></a>

<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Options -->
#### -str -- String parameter
<a href="#-str"></a>

#### -tocamelcase -- Convert string to camel case
<a href="#-tocamelcase"></a>

#### -tolowerunder -- Convert string to lower-under
<a href="#-tolowerunder"></a>

#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

#### -pathcomp -- Extract path component from string
<a href="#-pathcomp"></a>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Options -->

### Inputs
<a href="#inputs"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Inputs -->
`strconv` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Inputs -->


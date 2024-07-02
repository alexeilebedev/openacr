## acr_compl - ACR shell auto-complete for all targets


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Toc -->
&nbsp;&nbsp;&bull;&nbsp;  [Syntax](#syntax)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Options](#options)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
&#128196; [acr_compl - Internals](/txt/exe/acr_compl/internals.md)<br/>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Toc -->

### Syntax
<a href="#syntax"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Syntax -->
```
acr_compl: ACR shell auto-complete for all targets
Usage: acr_compl [options]
    OPTION      TYPE    DFLT    COMMENT
    -data       string  "data"  Source for completions (dir or file or -)
    -schema     string  "data"  Source for schema information
    -line       string  ""      Simulates COMP_LINE (debug)
    -point      string  ""      Simulates COMP_POINT (debug). default: whole line
    -type       string  "9"     Simulates COMP_TYPE (debug)
    -install                    Produce bash commands to install the handler
    -debug_log  string  ""      Log file for debug information, overrides ACR_COMPL_DEBUG_LOG
    -verbose    int             Verbosity level (0..255); alias -v; cumulative
    -debug      int             Debug level (0..255); alias -d; cumulative
    -help                       Print help and exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig

```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Syntax -->

### Options
<a href="#options"></a>

<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Options -->
#### -data -- Source for completions (dir or file or -)
<a href="#-data"></a>

#### -schema -- Source for schema information
<a href="#-schema"></a>

#### -line -- Simulates COMP_LINE (debug)
<a href="#-line"></a>

#### -point -- Simulates COMP_POINT (debug). default: whole line
<a href="#-point"></a>

#### -type -- Simulates COMP_TYPE (debug)
<a href="#-type"></a>

#### -install -- Produce bash commands to install the handler
<a href="#-install"></a>

#### -debug_log -- Log file for debug information, overrides ACR_COMPL_DEBUG_LOG
<a href="#-debug_log"></a>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Options -->

### Inputs
<a href="#inputs"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Inputs -->
`acr_compl` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dmmeta.ns](/txt/ssimdb/dmmeta/ns.md)|Namespace (for in-memory database, protocol, etc)|
|[dmmeta.ctype](/txt/ssimdb/dmmeta/ctype.md)|Struct|
|[dmmeta.field](/txt/ssimdb/dmmeta/field.md)|Specify field of a struct|
|[dmmeta.ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|File with ssim tuples|
|[dmmeta.fflag](/txt/ssimdb/dmmeta/fflag.md)|Options for command-line flags|
|[dmmeta.fconst](/txt/ssimdb/dmmeta/fconst.md)|Specify enum value (integer + string constant) for a field|
|[dmmeta.fcmdline](/txt/ssimdb/dmmeta/fcmdline.md)|Annotate field that holds process command line|
|[dmmeta.falias](/txt/ssimdb/dmmeta/falias.md)|Alias field|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|
|[dmmeta.argvtype](/txt/ssimdb/dmmeta/argvtype.md)|Customize parsing of command lines (rarely used)|
|[dmmeta.anonfld](/txt/ssimdb/dmmeta/anonfld.md)|Omit field name where possible (command line, enums, constants)|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Inputs -->


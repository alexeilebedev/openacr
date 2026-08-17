## acr_compl - ACR shell auto-complete for all targets


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- abt_md.toc_beg -->
&nbsp;&nbsp;&bull;&nbsp;  [Internals](#internals)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Options](#options)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
<!-- abt_md.toc_end -->

### Internals
<a href="#internals"></a>
&#128196; [acr_compl - Internals](/txt/gen/acr_compl/acr_compl.md)<br/>

### Options
<a href="#options"></a>
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

#### -check -- Check command line validity
<a href="#-check"></a>

#### -check_batch -- Batch mode: read acr_compl.checkreq from stdin, emit acr_compl.checkerr per failure
<a href="#-check_batch"></a>

### Inputs
<a href="#inputs"></a>
`acr_compl` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|
|[dmmeta.anonfld](/txt/ssimdb/dmmeta/anonfld.md)|Omit field name where possible (command line, enums, constants)|
|[dmmeta.argvtype](/txt/ssimdb/dmmeta/argvtype.md)|Customize parsing of command lines (rarely used)|
|[dmmeta.ccmdline](/txt/ssimdb/dmmeta/ccmdline.md)|tie cmdline to basecmdline|
|[dmmeta.ctype](/txt/ssimdb/dmmeta/ctype.md)|Struct|
|[dmmeta.falias](/txt/ssimdb/dmmeta/falias.md)|Alias field|
|[dmmeta.fconst](/txt/ssimdb/dmmeta/fconst.md)|Specify enum value (integer + string constant) for a field|
|[dmmeta.fflag](/txt/ssimdb/dmmeta/fflag.md)|Options for command-line flags|
|[dmmeta.field](/txt/ssimdb/dmmeta/field.md)|Specify field of a struct|
|[dmmeta.finsertwhen](/txt/ssimdb/dmmeta/finsertwhen.md)|Switch -<flag> to insert-completion mode when <when> flag is also set|
|[dmmeta.floadtuples](/txt/ssimdb/dmmeta/floadtuples.md)|Request that process automatically load any input tables on startup|
|[dmmeta.ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|File with ssim tuples|
|[dmmeta.substr](/txt/ssimdb/dmmeta/substr.md)|Specify that the field value is computed from a substring of another field|

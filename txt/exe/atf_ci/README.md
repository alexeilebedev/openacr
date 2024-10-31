## atf_ci - Normalization tests (see citest table)


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Toc -->
&nbsp;&nbsp;&bull;&nbsp;  [Syntax](#syntax)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Options](#options)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
&#128196; [atf_ci - Internals](/txt/exe/atf_ci/internals.md)<br/>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Toc -->

### Syntax
<a href="#syntax"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Syntax -->
```
atf_ci: Normalization tests (see citest table)
Usage: atf_ci [[-citest:]<regx>] [options]
    OPTION      TYPE    DFLT    COMMENT
    -in         string  "data"  Input directory or filename, - for stdin
    [citest]    regx    "%"     Regx of tests to run
    -maxerr     int     0       Exit after this many errors
    -cijob      regx    "%"
    -capture                    Capture the output of the test
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
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

#### -citest -- Regx of tests to run
<a href="#-citest"></a>

#### -maxerr -- Exit after this many errors
<a href="#-maxerr"></a>

#### -cijob -- 
<a href="#-cijob"></a>

#### -capture -- Capture the output of the test
<a href="#-capture"></a>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Options -->

### Inputs
<a href="#inputs"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Inputs -->
`atf_ci` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dev.gitfile](/txt/ssimdb/dev/gitfile.md)|File managed by git|
|[dmmeta.ns](/txt/ssimdb/dmmeta/ns.md)|Namespace (for in-memory database, protocol, etc)|
|[dmmeta.ctype](/txt/ssimdb/dmmeta/ctype.md)|Struct|
|[dmmeta.field](/txt/ssimdb/dmmeta/field.md)|Specify field of a struct|
|[dmmeta.substr](/txt/ssimdb/dmmeta/substr.md)|Specify that the field value is computed from a substring of another field|
|[dmmeta.ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|File with ssim tuples|
|[dmmeta.sqltype](/txt/ssimdb/dmmeta/sqltype.md)|Mapping of ctype -> SQL expression|
|[dmmeta.ftuple](/txt/ssimdb/dmmeta/ftuple.md)||
|[dmmeta.fconst](/txt/ssimdb/dmmeta/fconst.md)|Specify enum value (integer + string constant) for a field|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|
|[dmmeta.cppfunc](/txt/ssimdb/dmmeta/cppfunc.md)|Value of field provided by this expression|
|[dmmeta.cfmt](/txt/ssimdb/dmmeta/cfmt.md)|Specify options for printing/reading ctypes into multiple formats|
|[dmmeta.cdflt](/txt/ssimdb/dmmeta/cdflt.md)|Specify default value for single-value types that lack fields|
|[dev.unstablefld](/txt/ssimdb/dev/unstablefld.md)|Fields that should be stripped from component test output because they contain timestamps etc.|
|[dev.targsrc](/txt/ssimdb/dev/targsrc.md)|List of sources for target|
|[dev.scriptfile](/txt/ssimdb/dev/scriptfile.md)|Known script file|
|[dev.readme](/txt/ssimdb/dev/readme.md)|File containing documentation|
|[dev.noindent](/txt/ssimdb/dev/noindent.md)|Indicates that a file should not be automatically indented|
|[dev.msgfile](/txt/ssimdb/dev/msgfile.md)|File in repo containing messages that should be normalized during CI|
|[dev.cfg](/txt/ssimdb/dev/cfg.md)|Compiler configuration|
|[dev.builddir](/txt/ssimdb/dev/builddir.md)|Directory where object files/executables go. Determines compile/link options|
|[atfdb.cipackage](/txt/ssimdb/atfdb/cipackage.md)||
|[amcdb.bltin](/txt/ssimdb/amcdb/bltin.md)|Specify properties of a C built-in type|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Inputs -->


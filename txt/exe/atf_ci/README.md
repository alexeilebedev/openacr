## atf_ci - Normalization tests (see citest table)


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- abt_md.toc_beg -->
&nbsp;&nbsp;&bull;&nbsp;  [Internals](#internals)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Options](#options)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
<!-- abt_md.toc_end -->

### Internals
<a href="#internals"></a>
&#128196; [atf_ci - Internals](/txt/gen/atf_ci/atf_ci.md)<br/>

### Options
<a href="#options"></a>
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

#### -check_clean -- Check for modifications after each test
<a href="#-check_clean"></a>

### Inputs
<a href="#inputs"></a>
`atf_ci` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|
|[amcdb.bltin](/txt/ssimdb/amcdb/bltin.md)|Specify properties of a C built-in type|
|[dmmeta.cdflt](/txt/ssimdb/dmmeta/cdflt.md)|Specify default value for single-value types that lack fields|
|[dmmeta.cfmt](/txt/ssimdb/dmmeta/cfmt.md)|Specify options for printing/reading ctypes into multiple formats|
|[dmmeta.cppfunc](/txt/ssimdb/dmmeta/cppfunc.md)|Value of field provided by this expression|
|[dmmeta.ctype](/txt/ssimdb/dmmeta/ctype.md)|Struct|
|[dmmeta.fconst](/txt/ssimdb/dmmeta/fconst.md)|Specify enum value (integer + string constant) for a field|
|[dmmeta.field](/txt/ssimdb/dmmeta/field.md)|Specify field of a struct|
|[dmmeta.ftuple](/txt/ssimdb/dmmeta/ftuple.md)||
|[dmmeta.sqltype](/txt/ssimdb/dmmeta/sqltype.md)|Mapping of ctype -> SQL expression|
|[dmmeta.ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|File with ssim tuples|
|[dmmeta.substr](/txt/ssimdb/dmmeta/substr.md)|Specify that the field value is computed from a substring of another field|
|[dev.unstablefld](/txt/ssimdb/dev/unstablefld.md)|Fields that should be stripped from component test output because they contain timestamps etc.|
|[dev.builddir](/txt/ssimdb/dev/builddir.md)|Directory where object files/executables go. Determines compile/link options|
|[dev.cfg](/txt/ssimdb/dev/cfg.md)|Compiler configuration|
|[atfdb.cipackage](/txt/ssimdb/atfdb/cipackage.md)||
|[dev.gitfile](/txt/ssimdb/dev/gitfile.md)|File managed by git|
|[dev.msgfile](/txt/ssimdb/dev/msgfile.md)|File in repo containing messages that should be normalized during CI|
|[dev.noindent](/txt/ssimdb/dev/noindent.md)|Indicates that a file should not be automatically indented|
|[dmmeta.ns](/txt/ssimdb/dmmeta/ns.md)|Namespace (for in-memory database, protocol, etc)|
|[dev.package](/txt/ssimdb/dev/package.md)|OpenACR package|
|[dev.pkggen](/txt/ssimdb/dev/pkggen.md)||
|[dev.readmefile](/txt/ssimdb/dev/readmefile.md)|File containing documentation|
|[dev.scriptfile](/txt/ssimdb/dev/scriptfile.md)|Known script file|
|[dmmeta.ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|File with ssim tuples|
|[dev.targsrc](/txt/ssimdb/dev/targsrc.md)|List of sources for target|

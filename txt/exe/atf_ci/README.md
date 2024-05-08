## atf_ci - Normalization tests (see citest table)


### Table Of Contents
<a href="#table-of-contents"></a>
* [Syntax](#syntax)
* [Options](#options)
* [Sources](#sources)
* [Inputs](#inputs)

### Syntax
<a href="#syntax"></a>
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

### Sources
<a href="#sources"></a>
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/atf/ci/acr_ed.cpp](/cpp/atf/ci/acr_ed.cpp)||
|[cpp/atf/ci/apm.cpp](/cpp/atf/ci/apm.cpp)||
|[cpp/atf/ci/bootstrap.cpp](/cpp/atf/ci/bootstrap.cpp)||
|[cpp/atf/ci/cppcheck.cpp](/cpp/atf/ci/cppcheck.cpp)||
|[cpp/atf/ci/main.cpp](/cpp/atf/ci/main.cpp)||
|[cpp/atf/ci/src.cpp](/cpp/atf/ci/src.cpp)|source file (cpp/, include/) normalizations including amc|
|[cpp/atf/ci/ssim.cpp](/cpp/atf/ci/ssim.cpp)|ssim database normalizations|
|[cpp/gen/atf_ci_gen.cpp](/cpp/gen/atf_ci_gen.cpp)||
|[include/atf_ci.h](/include/atf_ci.h)||
|[include/gen/atf_ci_gen.h](/include/gen/atf_ci_gen.h)||
|[include/gen/atf_ci_gen.inl.h](/include/gen/atf_ci_gen.inl.h)||

### Inputs
<a href="#inputs"></a>
`atf_ci` takes the following tables on input:
|ssimfile|comment|
|---|---|
|[dmmeta.ns](/txt/ssimdb/dmmeta/ns.md)|Namespace (for in-memory database, protocol, etc)|
|[dmmeta.ctype](/txt/ssimdb/dmmeta/ctype.md)|Struct|
|[dmmeta.field](/txt/ssimdb/dmmeta/field.md)|Specify field of a struct|
|[dev.unstablefld](/txt/ssimdb/dev/unstablefld.md)|Fields that should be stripped from component test output because they contain timestamps etc.|
|[dmmeta.substr](/txt/ssimdb/dmmeta/substr.md)|Specify that the field value is computed from a substring of another field|
|[dmmeta.sqltype](/txt/ssimdb/dmmeta/sqltype.md)|Mapping of ctype -> SQL expression|
|[dmmeta.ftuple](/txt/ssimdb/dmmeta/ftuple.md)||
|[dmmeta.fconst](/txt/ssimdb/dmmeta/fconst.md)|Specify enum value (integer + string constant) for a field|
|[dmmeta.cppfunc](/txt/ssimdb/dmmeta/cppfunc.md)|Value of field provided by this expression|
|[dmmeta.cfmt](/txt/ssimdb/dmmeta/cfmt.md)|Specify options for printing/reading ctypes into multiple formats|
|[dmmeta.cdflt](/txt/ssimdb/dmmeta/cdflt.md)|Specify default value for single-value types that lack fields|
|[amcdb.bltin](/txt/ssimdb/amcdb/bltin.md)|Specify properties of a C built-in type|
|[dev.gitfile](/txt/ssimdb/dev/gitfile.md)|File managed by git|
|[dev.targsrc](/txt/ssimdb/dev/targsrc.md)|List of sources for target|
|[dmmeta.ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|File with ssim tuples|
|[dev.scriptfile](/txt/ssimdb/dev/scriptfile.md)|Known script file|
|[dev.readme](/txt/ssimdb/dev/readme.md)|File containing documentation|
|[dev.noindent](/txt/ssimdb/dev/noindent.md)|Indicates that a file should not be automatically indented|
|[dev.msgfile](/txt/ssimdb/dev/msgfile.md)|File in repo containing messages that should be normalized during CI|
|[atfdb.cipackage](/txt/ssimdb/atfdb/cipackage.md)||
|[dev.cfg](/txt/ssimdb/dev/cfg.md)|Compiler configuration|
|[dev.builddir](/txt/ssimdb/dev/builddir.md)|Directory where object files/executables go. Determines compile/link options|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|


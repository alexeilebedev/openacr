## atf_unit - Unit tests (see unittest table)


### Table Of Contents
<a href="#table-of-contents"></a>
* [Syntax](#syntax)
* [Options](#options)
* [Sources](#sources)
* [Inputs](#inputs)

### Syntax
<a href="#syntax"></a>
```
atf_unit: Unit tests (see unittest table)
Usage: atf_unit [[-unittest:]<regx>] [options]
    OPTION            TYPE    DFLT    COMMENT
    [unittest]        regx    "%"     SQL regex, selecting test to run
    -nofork                           Do not fork for destructive tests
    -arg              string  ""      Argument to pass to tool
    -data_dir         string  "data"  Data directory
    -mdbg                     0       Break at testcase in debugger
    -perf_secs        double  1.0     # Of seconds to run perf tests for
    -pertest_timeout  int     900     Max runtime of any individual unit test
    -report                   Y       Print final report
    -capture                          Re-capture test results
    -check_untracked          Y       Check for untracked file before allowing test to run
    -verbose          int             Verbosity level (0..255); alias -v; cumulative
    -debug            int             Debug level (0..255); alias -d; cumulative
    -help                             Print help and exit; alias -h
    -version                          Print version and exit
    -signature                        Show signatures and exit; alias -sig

```

### Options
<a href="#options"></a>

#### -unittest -- SQL regex, selecting test to run
<a href="#-unittest"></a>

#### -nofork -- Do not fork for destructive tests
<a href="#-nofork"></a>

#### -arg -- Argument to pass to tool
<a href="#-arg"></a>

#### -data_dir -- Data directory
<a href="#-data_dir"></a>

#### -mdbg -- Break at testcase in debugger
<a href="#-mdbg"></a>

#### -perf_secs -- # Of seconds to run perf tests for
<a href="#-perf_secs"></a>

#### -pertest_timeout -- Max runtime of any individual unit test
<a href="#-pertest_timeout"></a>

#### -report -- Print final report
<a href="#-report"></a>

#### -capture -- Re-capture test results
<a href="#-capture"></a>

#### -check_untracked -- Check for untracked file before allowing test to run
<a href="#-check_untracked"></a>

### Sources
<a href="#sources"></a>
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/atf_unit/acr.cpp](/cpp/atf_unit/acr.cpp)||
|[cpp/atf_unit/algo_fmt.cpp](/cpp/atf_unit/algo_fmt.cpp)||
|[cpp/atf_unit/algo_lib.cpp](/cpp/atf_unit/algo_lib.cpp)||
|[cpp/atf_unit/ams.cpp](/cpp/atf_unit/ams.cpp)||
|[cpp/atf_unit/bash.cpp](/cpp/atf_unit/bash.cpp)||
|[cpp/atf_unit/charset.cpp](/cpp/atf_unit/charset.cpp)||
|[cpp/atf_unit/decimal.cpp](/cpp/atf_unit/decimal.cpp)||
|[cpp/atf_unit/fm.cpp](/cpp/atf_unit/fm.cpp)||
|[cpp/atf_unit/lib_ams.cpp](/cpp/atf_unit/lib_ams.cpp)||
|[cpp/atf_unit/lib_exec.cpp](/cpp/atf_unit/lib_exec.cpp)||
|[cpp/atf_unit/lib_json.cpp](/cpp/atf_unit/lib_json.cpp)||
|[cpp/atf_unit/lib_sql.cpp](/cpp/atf_unit/lib_sql.cpp)||
|[cpp/atf_unit/line.cpp](/cpp/atf_unit/line.cpp)||
|[cpp/atf_unit/lockfile.cpp](/cpp/atf_unit/lockfile.cpp)||
|[cpp/atf_unit/main.cpp](/cpp/atf_unit/main.cpp)||
|[cpp/atf_unit/parsenum.cpp](/cpp/atf_unit/parsenum.cpp)||
|[cpp/atf_unit/regx.cpp](/cpp/atf_unit/regx.cpp)||
|[cpp/atf_unit/string.cpp](/cpp/atf_unit/string.cpp)||
|[cpp/atf_unit/time.cpp](/cpp/atf_unit/time.cpp)||
|[cpp/atf_unit/tuple.cpp](/cpp/atf_unit/tuple.cpp)||
|[cpp/gen/atf_unit_gen.cpp](/cpp/gen/atf_unit_gen.cpp)||
|[include/atf_unit.h](/include/atf_unit.h)||
|[include/gen/atf_unit_gen.h](/include/gen/atf_unit_gen.h)||
|[include/gen/atf_unit_gen.inl.h](/include/gen/atf_unit_gen.inl.h)||

### Inputs
<a href="#inputs"></a>
`atf_unit` takes the following tables on input:
|ssimfile|comment|
|---|---|
|[fmdb.alm_objtype](/txt/ssimdb/fmdb/alm_objtype.md)|Alarm resource type|
|[fmdb.alm_code](/txt/ssimdb/fmdb/alm_code.md)||
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|


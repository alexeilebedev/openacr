## atf_unit - Unit tests (see unittest table)


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Toc -->
&nbsp;&nbsp;&bull;&nbsp;  [Syntax](#syntax)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Options](#options)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
&#128196; [atf_unit - Internals](/txt/exe/atf_unit/internals.md)<br/>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Toc -->

### Syntax
<a href="#syntax"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Syntax -->
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
    -verbose          flag            Verbosity level (0..255); alias -v; cumulative
    -debug            flag            Debug level (0..255); alias -d; cumulative
    -help                             Print help and exit; alias -h
    -version                          Print version and exit
    -signature                        Show signatures and exit; alias -sig

```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Syntax -->

### Options
<a href="#options"></a>

<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Options -->
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

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Options -->

### Inputs
<a href="#inputs"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Inputs -->
`atf_unit` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|
|[fmdb.alm_code](/txt/ssimdb/fmdb/alm_code.md)||
|[fmdb.alm_objtype](/txt/ssimdb/fmdb/alm_objtype.md)|Alarm resource type|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Inputs -->


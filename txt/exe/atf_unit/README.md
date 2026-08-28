## atf_unit - Unit tests (see unittest table)


### Syntax
<a href="#syntax"></a>
```usage
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

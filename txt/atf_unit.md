## atf_unit: Unit tests (see unittest table)



### Syntax

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
    -help                             Print help an exit; alias -h
    -version                          Print version and exit
    -signature                        Show signatures and exit; alias -sig

```

### Inputs

`atf_unit` takes the following tables on input:
```
CTYPE            COMMENT
fmdb.AlmObjtype  Alarm resource type
fmdb.AlmCode
```


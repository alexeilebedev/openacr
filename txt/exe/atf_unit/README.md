## atf_unit - Unit tests (see unittest table)
<a href="#atf_unit"></a>
`atf_unit` runs the unit tests, which call library and tool code directly from
C++ and check the results.  Each test is one function compiled into
`atf_unit`, and the runner starts every selected test in a child process of
its own.  A test passes when its function returns without an error.

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
    -trace            string  ""      Trace expression: category[:filter],...; also payload_lim:N, verbose, debug, timestamps
    -help                             Print help and exit; alias -h
    -version                          Print version and exit
    -signature                        Show signatures and exit; alias -sig
```

### Description
<a href="#description"></a>

#### Where the tests live
<a href="#where-the-tests-live"></a>

Each unit test is a row of `atfdb.unittest`, named `<ns>.<name>` after the
namespace it tests, and there are ~440 of them, most under `algo_lib` and
`lib_json`.  The test's function is `unittest_<ns>_<name>` in
`cpp/atf_unit/<ns>.cpp`.  The table is compiled into `atf_unit`, so a new row
reaches the runner after `amc` and a rebuild.
`acr_ed -create -unittest <ns>.<name> -write` does all of it: it adds the row
and a stub function, runs `amc`, rebuilds `atf_unit` and runs the new test (see
[acr_ed](/txt/exe/acr_ed/README.md)).

A test asserts with `TESTCMP(a,b)` or `vrfy`, and any error it throws fails it.
A few tests write a file and compare it with a reference file under
`test/atf_unit/`, which `-capture` rewrites.

#### Running the tests
<a href="#running-the-tests"></a>

Before it starts, `atf_unit` refuses to run when the working tree holds
untracked files, and after each test it fails the test if the test left any
behind.  It then runs each selected test as a child `atf_unit -nofork`,
as many at once as there are CPUs and at least four.  Each test prints an
`atf_unit.begin` line and an `atf_unit.unittest` line with its verdict.  At the
end the runner repeats every failed test with a `repro` command and prints a
`report.atf_unit` line.  The exit code is the number of failed tests.

A test that measures speed wraps its loop in `DO_PERF_TEST`, which repeats the
loop for `-perf_secs` seconds and prints an `atf_unit.perf` line with the time
per iteration.

### Examples
<a href="#examples"></a>

```bash
atf_unit                             # run every unit test
atf_unit 'algo_lib.%'                # run the algo_lib tests
atf_unit algo_lib.Abs -nofork        # run one test in this process
atf_unit algo_lib.Abs -mdbg          # run one test in the debugger
atf_unit 'lib_json.%' -perf_secs:0   # run the lib_json tests with the shortest perf loops
acr unittest:acr.%                   # list the acr unit tests
```

A passing run of one test:

```
atf_unit.begin  unittest:algo_lib.Abs
atf_unit.unittest  unittest:algo_lib.Abs  success:Y  comment:""
report.atf_unit  n_test_total:844  success:Y  n_test_run:1  n_err:0
```

### Caveats
<a href="#caveats"></a>

- An untracked file anywhere in the tree stops the whole run with
  `atf_unit.untracked_files`.  Commit or delete the file, or pass
  `-check_untracked:N`.
- A pattern that matches no test prints `atf_uniq.regex_warning` and exits
  zero, so a mistyped name reads like a pass.  Check that `n_test_run` is
  nonzero.
- `n_test_total` counts every test twice, so it reads double the size of the
  table.

### Options
<a href="#options"></a>
#### -unittest -- SQL regex, selecting test to run
<a href="#-unittest"></a>

Selects the tests to run by name, as an SQL regex such as `'algo_lib.%'`.  The
default `%` runs all of them.

#### -nofork -- Do not fork for destructive tests
<a href="#-nofork"></a>

Runs the selected tests one after another inside this process, with no child
per test.  A test that crashes then ends the run, but the process is easy to
follow in a debugger or a profiler.  `-mdbg` uses it on its own.

#### -arg -- Argument to pass to tool
<a href="#-arg"></a>

No unit test reads this value, so setting it has no effect.

#### -data_dir -- Data directory
<a href="#-data_dir"></a>

The directory `atf_unit` loads the ssim tables from that some tests read, such
as `dmmeta.dispsigcheck`.  The default is `data`.

#### -mdbg -- Break at testcase in debugger
<a href="#-mdbg"></a>

Runs the first selected test under [mdbg](/txt/exe/mdbg/README.md) with
`-nofork`, with breakpoints at the test's function and at the point where a
`TESTCMP` fails.  Select one test with it.

#### -perf_secs -- # Of seconds to run perf tests for
<a href="#-perf_secs"></a>

How long each `DO_PERF_TEST` loop repeats before it reports its time.  A value
of zero runs each loop once, a thousand iterations, which is what CI passes.

#### -pertest_timeout -- Max runtime of any individual unit test
<a href="#-pertest_timeout"></a>

The number of seconds one test may run before an alarm kills it, which fails
that test.  The default is fifteen minutes.

#### -report -- Print final report
<a href="#-report"></a>

Prints the failed tests with their `repro` commands and the `report.atf_unit`
line at the end of the run.  The runner passes `-report:N` to its children so
the report appears once.

#### -capture -- Re-capture test results
<a href="#-capture"></a>

Makes the tests that compare output with a reference file copy their output
over that file under `test/atf_unit/`.  A test that has no reference file runs
as usual.

#### -check_untracked -- Check for untracked file before allowing test to run
<a href="#-check_untracked"></a>

Refuses to start when the tree holds untracked files, and fails any test that
leaves one behind.  Pass `-check_untracked:N` to run in a tree with work in
progress, at the cost of missing a test that litters.

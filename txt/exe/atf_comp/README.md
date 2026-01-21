## atf_comp - Algo Test Framework - Component test execution


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Toc -->
&nbsp;&nbsp;&bull;&nbsp;  [Syntax](#syntax)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Description](#description)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Running tests](#running-tests)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Managing executable path](#managing-executable-path)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Keeping target executables up to date](#keeping-target-executables-up-to-date)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Capturing test output](#capturing-test-output)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Checking untracked files](#checking-untracked-files)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Continue testing after failure](#continue-testing-after-failure)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Run tests under memory checker](#run-tests-under-memory-checker)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Run tests under profiler](#run-tests-under-profiler)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Measuring test coverage](#measuring-test-coverage)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Finding artifacts from last run](#finding-artifacts-from-last-run)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Printing testcase](#printing-testcase)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Editing testcase](#editing-testcase)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inserting testcases](#inserting-testcases)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Normalizing testcases](#normalizing-testcases)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Debugging testcases](#debugging-testcases)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Examples](#examples)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Options](#options)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
&#128196; [atf_comp - Internals](/txt/exe/atf_comp/internals.md)<br/>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Toc -->

### Syntax
<a href="#syntax"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Syntax -->
```
atf_comp: Algo Test Framework - Component test execution
Usage: atf_comp [[-comptest:]<regx>] [options]
    OPTION            TYPE    DFLT       COMMENT
    -in               string  "data"     Input directory or filename, - for stdin
    [comptest]        regx    "%"        Select comptest (SQL regex)
    -mdbg                                (action) Run component test under debugger
    -run                      Y          (action) Run selected component tests
    -capture                             (action) Re-capture test results
    -print                               (action) Print testcase
    -printinput                          (action) Print input of test case
    -e                                   (action) Open selected testcases in an editor
    -normalize                           (action) Renumber and normalize tmsgs
    -covcapture                          (action) Capture new coverage percentages and save back
    -covcheck                            (action) Check coverage percentages against tgtcov table
    -bindir           string  ""         Directory with binaries (default: build/cfg)
    -cfg              string  "release"  Set config
    -check_untracked          Y          Check for untracked file before allowing test to run
    -maxerr           int     3          Exit after this many errors
    -build                               Build given cfg before test
    -memcheck                            Run under memory checker (valgrind)
    -force                               (With -memcheck) run suppressed memcheck
    -callgrind                           Run under callgrind profiler (valgrind)
    -maxjobs          int     0          Maximum number of tests run in parallel
    -stream                              prints component's output
    -i                                   Read and execute testcase from stdin
    -write                    Y          (implied with -e) Write any changes back to ssim tables
    -report                   Y          Print final report
    -b                string  ""         Breakpoint passed to mdbg as-is
    -covfast                  Y          Speedup coverage processing
    -minrepeat        int     0          Execute every comptest at least this many times
    -maxrepeat        int     1          Don't repeat any individual comptest more than X times
    -verbose          flag               Verbosity level (0..255); alias -v; cumulative
    -debug            flag               Debug level (0..255); alias -d; cumulative
    -help                                Print help and exit; alias -h
    -version                             Print version and exit
    -signature                           Show signatures and exit; alias -sig

```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Syntax -->

### Description
<a href="#description"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Description -->

Unlike unit test, which deals with source code testing,
component test deals with black-box testing of ready-made
software component, which is an executable target.

Atf_comp is a test driver, which runs target executable,
feeds it with input, and gathers output to a file,
then compares file content with canned one.

ams targets support expect-like interactive mode, waiting for specific
output before accepting additional input. This is achieved with t2.ExpectMsg message.

Atf_comp facilitates various development activities
running tests in companion with other tools (NOT YET ALL IMPLEMENTED):
- debugging;
- memory checking;
- profiling;
- coverage measurement;
- automatic fuzzing.

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Description -->

### Running tests
<a href="#running-tests"></a>

To run all tests, simply do:
```
atf_comp
```

To run specific test use SQL pattern for comptest, for example, run all tests for target *target*:

```
atf_comp target.%
```

### Managing executable path
<a href="#managing-executable-path"></a>

Without arguments, atf_comp always runs release targets under `build/release` directory.
In order to change configuration use `-cfg` option,
e.g. to test debug executables under `build/debug`, use:

```
atf_comp -cfg:debug
```

In order to specify completely different directory use `-compdir` option, e.g.:
```
atf_comp -compdir:other_dir
```

### Keeping target executables up to date
<a href="#keeping-target-executables-up-to-date"></a>

In order to ensure that target executabes are up-to-date, use `-ood` and/or `-build` options.
Once specified, atf_comp runs abt with given options and configuration selected with `-cfg`.

### Capturing test output
<a href="#capturing-test-output"></a>

For first run of newly developed testcase or rewriting existing expected output file
(to be compared with) from actual run, use `-capture`.
Outputs are kept in files named `test/atf_comp/<COMPTEST>.out`.
The first line of each output file is a command that can be used to reproduce the rest of the file.

### Checking untracked files
<a href="#checking-untracked-files"></a>

By default, atf_comp check untracked files before runing tests.
This can be avoided with `-check-untracked:N`.

### Continue testing after failure
<a href="#continue-testing-after-failure"></a>

By default, Atf_comp stops testing after first failed test.
In order to continue testing up to N failed tests,
specify it with `-maxerr`, e.g.: `-maxerr:10` - stop after 10 test failures.

### Run tests under memory checker
<a href="#run-tests-under-memory-checker"></a>

Atf_cov supports valgrind memory checker, use `-memcheck`:

```
atf_cov -memcheck
```

Once memory errors have been detected, they are displayed, and test failed.

To suppress memcheck for particular comptest, set `memcheck:N`.
To run suppresed memcheck, use `-memcheck -force`.

### Run tests under profiler
<a href="#run-tests-under-profiler"></a>

Atf_cov supports callgrind profiler, use `-callgrind`:

```
atf_cov -callgrind
```

In this mode, callgrind output file is generated. To open callgrind file,
use GUI analyzer.

On Linux, run **KCacheGrind**:

```
kcachegrind temp/atf_comp/comptest.callgrind.out
```

On MacOS, download output file and run **QCacheGrind**

```
qcachegrind path_to/comptest.callgrind.out
```

Replace *comptest* with comptest id.
Replace *path_to* with the actual file location.

### Measuring test coverage
<a href="#measuring-test-coverage"></a>

When specified `-cfg coverage`, atf_comp automatically runs tests under atf_cov,
with dedicated coverage directory for each comtest.
After all tests have been run, it merges data from all runs, and generates results on `temp/cov.d`.

For details refer to atf_cov section.

### Finding artifacts from last run
<a href="#finding-artifacts-from-last-run"></a>

One may find test artifacts from last run under directory `temp/atf_comp/`.
This is useful for debugging, and for achiving CI job results.

Files are:
- *comptest*.in - test input file (unfiltered)
- *comptest*.out - test output file (unfiltered)
- *comptest*.out.filt - test output file (filtered)
- *comptest*.memcheck.log - valgrind memcheck log
- *comptest*.callgrind.log - callgrind log
- *comptest*.callgrind.out - callgrind output file (to be loaded by analyzer)
- *comptest*.cov.d - coverage data directory (comptest)
- cov.d - coverage data directory (merged)

File names are prefixed with test case name (*comptest*).

**NOTE:**

It is not possible to combine several tools in singe run, and thus not all files may be present.

### Printing testcase
<a href="#printing-testcase"></a>

Use `-print` to print a testcase to stdout in a human-readable way (display format).
Use `-printinput` to print testcase input,
which can be fed directly to the target process using bash pipe.

### Editing testcase
<a href="#editing-testcase"></a>

Use `-e` to open selected testcases in an editor, using `atf_comp` display format.
Any changes made in the editor are read, applied and saved back to the source dataset.

### Inserting testcases
<a href="#inserting-testcases"></a>

New testcases in display format can be inserted into the dataset with `atf_comp -i`.

### Normalizing testcases
<a href="#normalizing-testcases"></a>

Testcases can be renumbered, and all messages read back (using `lib_ctype`), and written
back in normalized field order, using `atf_comp -normalize`.
This runs in `atf_ci comptest` to ensure typos, unknown fields, unknown messages
don't make it into the comptest database.

### Debugging testcases
<a href="#debugging-testcases"></a>

Testcases can be debugged by running the target command under `gdb`.
To do this, invoked `atf_comp` with `-mdbg` option. 

The command line produced by atf_comp can be used for stand-alone testing,
omitting atf_comp altogether.
Use `-v` to see the command line being generated.

### Examples
<a href="#examples"></a>

#### Testcase
<a href="#testcase"></a>

Testcases are described on `atfdb.comptest` table.

Test id format is:

*target*.*testname*

where *target* is the name of the target (executable) to test, *testname* is the name of test case for this target.

One record corresponds to one run of target executable.

Positive value for *timeout* parameter sets the timeout for this test.
If target executable does not finish in time, it will be killed and test failed then.

*memcheck* parameter allows suppressing memory checker for this test case.

**NOTE:**

For stability, recommended timeout value shall be set to 2x..3x from elapsed time
for typical run of release target without any tool.
Running instrumented target or running in companion with tool may greatly increase elapsed time,
for example, debug executable slows down by factor of 3..4, memory checker - 20..30.
Atf_comp will adjust timeout automatically depending on selected configuration and selected tool.

#### Command line arguments for target executable
<a href="#command-line-arguments-for-target-executable"></a>

Command line arguments for target executable being run are placed on the table `atfdb.targs`

#### Test script
<a href="#test-script"></a>

Test script is described on `atf.tmsg` table.

Messages are ordered with integer rank.

#### Filtering output
<a href="#filtering-output"></a>

Test output may contain time-dependent or uninitersting data, such as clock values or heartbeats.
One may filter it out with `atfdb.tfilt` record. Filter expression is runnable OS command,
which accepts unfilterred data on stdin, and puts filtered data on stdout.
Unix commands like sed, awk, grep and cat fit.
For complex filter, dedicated executable is possible.

### Options
<a href="#options"></a>

<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Options -->
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

#### -comptest -- Select comptest (SQL regex)
<a href="#-comptest"></a>

#### -mdbg -- (action) Run component test under debugger
<a href="#-mdbg"></a>

#### -run -- (action) Run selected component tests
<a href="#-run"></a>

#### -capture -- (action) Re-capture test results
<a href="#-capture"></a>

#### -print -- (action) Print testcase
<a href="#-print"></a>

#### -printinput -- (action) Print input of test case
<a href="#-printinput"></a>

#### -e -- (action) Open selected testcases in an editor
<a href="#-e"></a>

#### -normalize -- (action) Renumber and normalize tmsgs
<a href="#-normalize"></a>

#### -covcapture -- (action) Capture new coverage percentages and save back
<a href="#-covcapture"></a>

#### -covcheck -- (action) Check coverage percentages against tgtcov table
<a href="#-covcheck"></a>

#### -bindir -- Directory with binaries (default: build/cfg)
<a href="#-bindir"></a>

#### -cfg -- Set config
<a href="#-cfg"></a>

#### -check_untracked -- Check for untracked file before allowing test to run
<a href="#-check_untracked"></a>

#### -maxerr -- Exit after this many errors
<a href="#-maxerr"></a>

#### -build -- Build given cfg before test
<a href="#-build"></a>

#### -memcheck -- Run under memory checker (valgrind)
<a href="#-memcheck"></a>

#### -force -- (With -memcheck) run suppressed memcheck
<a href="#-force"></a>

#### -callgrind -- Run under callgrind profiler (valgrind)
<a href="#-callgrind"></a>

#### -maxjobs -- Maximum number of tests run in parallel
<a href="#-maxjobs"></a>

#### -stream -- prints component's output
<a href="#-stream"></a>

#### -i -- Read and execute testcase from stdin
<a href="#-i"></a>

#### -write -- (implied with -e) Write any changes back to ssim tables
<a href="#-write"></a>

#### -report -- Print final report
<a href="#-report"></a>

#### -b -- Breakpoint passed to mdbg as-is
<a href="#-b"></a>

#### -covfast -- Speedup coverage processing
<a href="#-covfast"></a>

#### -minrepeat -- Execute every comptest at least this many times
<a href="#-minrepeat"></a>

#### -maxrepeat -- Don't repeat any individual comptest more than X times
<a href="#-maxrepeat"></a>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Options -->

### Inputs
<a href="#inputs"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Inputs -->
`atf_comp` takes the following tables on input:
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
|[atfdb.comptest](/txt/ssimdb/atfdb/comptest.md)||
|[atfdb.targs](/txt/ssimdb/atfdb/targs.md)||
|[atfdb.tfilt](/txt/ssimdb/atfdb/tfilt.md)||
|[atfdb.tmsg](/txt/ssimdb/atfdb/tmsg.md)||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Inputs -->


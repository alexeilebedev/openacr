## atf_comp: A component test framework

### Syntax

```
inline-command: atf_comp -h
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
    -compdir          string  ""         Component image directory (exe)
    -cfg              string  "release"  Set config
    -check_untracked          Y          Check for untracked file before allowing test to run
    -maxerr           int     1          Exit after this many errors
    -build                               Build given cfg before test
    -ood                                 Check given cfg for ood before test
    -memcheck                            Run under memory checker (valgrind)
    -force                               (With -memcheck) run suppressed memcheck
    -callgrind                           Run under callgrind profiler (valgrind)
    -maxjobs          int     1          Maximum number of tests run in parallel
    -stream                              prints component's output
    -i                                   Read and execute testcase from stdin
    -write                    Y          (implied with -e) Write any changes back to ssim tables
    -report                              Print final report
    -b                string  ""         Breakpoint passed to mdbg as-is
    -verbose          int                Verbosity level (0..255); alias -v; cumulative
    -debug            int                Debug level (0..255); alias -d; cumulative
    -help                                Print help an exit; alias -h
    -version                             Print version and exit
    -signature                           Show signatures and exit; alias -sig

```

### Description

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

### Data model

#### Testcase

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

Command line arguments for target executable being run are placed on the table `atfdb.targs`

#### Test script

Test script is described on `atf.tmsg` table.

Messages are ordered with integer rank.

#### Filtering output

Test output may contain time-dependent or uninitersting data, such as clock values or heartbeats.
One may filter it out with `atfdb.tfilt` record. Filter expression is runnable OS command,
which accepts unfilterred data on stdin, and puts filtered data on stdout.
Unix commands like sed, awk, grep and cat fit.
For complex filter, dedicated executable is possible.

### Running tests

To run all tests, simply do:
```
atf_comp
```

To run specific test use SQL pattern for comptest, for example, run all tests for target *target*:

```
atf_comp target.%
```

### Managing executable path

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

In order to ensure that target executabes are up-to-date, use `-ood` and/or `-build` options.
Once specified, atf_comp runs abt with given options and configuration selected with `-cfg`.

### Capturing test output

For first run of newly developed testcase or rewriting existing expected output file
(to be compared with) from actual run, use `-capture`.
Outputs are kept in files named `test/atf_comp/<COMPTEST>.out`.
The first line of each output file is a command that can be used to reproduce the rest of the file.

### Checking untracked files

By default, atf_comp check untracked files before runing tests.
This can be avoided with `-check-untracked:N`.

### Continue testing after failure

By default, Atf_comp stops testing after first failed test.
In order to continue testing up to N failed tests,
specify it with `-maxerr`, e.g.: `-maxerr:10` - stop after 10 test failures.

### Run tests under memory checker

Atf_cov supports valgrind memory checker, use `-memcheck`:

```
atf_cov -memcheck
```

Once memory errors have been detected, they are displayed, and test failed.

To suppress memcheck for particular comptest, set `memcheck:N`.
To run suppresed memcheck, use `-memcheck -force`.


### Run tests under profiler

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

When specified `-cfg coverage`, atf_comp automatically runs tests under atf_cov,
with dedicated coverage directory for each comtest.
After all tests have been run, it merges data from all runs, and generates results on `temp/cov.d`.

For details refer to atf_cov section.

### Finding artifacts from last run

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

Use `-print` to print a testcase to stdout in a human-readable way (display format).
Use `-printinput` to print testcase input,
which can be fed directly to the target process using bash pipe.

### Editing testcase

Use `-e` to open selected testcases in an editor, using `atf_comp` display format.
Any changes made in the editor are read, applied and saved back to the source dataset.

### Inserting testcases

New testcases in display format can be inserted into the dataset with `atf_comp -i`.

### Normalizing testcases

Testcases can be renumbered, and all messages read back (using `lib_ctype`), and written
back in normalized field order, using `atf_comp -normalize`.
This runs in `atf_ci comptest` to ensure typos, unknown fields, unknown messages
don't make it into the comptest database.

### Debugging testcases

Testcases can be debugged by running the target command under `gdb`.
To do this, invoked `atf_comp` with `-mdbg` option. 

The command line produced by atf_comp can be used for stand-alone testing,
omitting atf_comp altogether.
Use `-v` to see the command line being generated.


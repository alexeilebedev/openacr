## atf_comp - Algo Test Framework - Component test execution
<a href="#atf_comp"></a>

Procedural component test runner. Each component test is a C++ function
loaded via gstatic from the `atfdb.comptest` table. Test functions use
a process API to spawn subprocesses, feed them input, and capture output
into a log. The log is compared against a reference file stored in
`test/atf_comp/<testname>`.

### Table Of Contents
<a href="#table-of-contents"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Toc -->
&nbsp;&nbsp;&bull;&nbsp;  [Syntax](#syntax)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Process API](#process-api)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Output filtering](#output-filtering)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Modes](#modes)<br/>
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
    OPTION            TYPE    DFLT             COMMENT
    -in               string  "data"           Input directory or filename, - for stdin
    [comptest]        regx    "%"              Select comptest (SQL regex)
    -mode             enum    run              Test mode (run|capture|covcheck|covcapture|memcheck|valgrind|mdbg|edit|editsource|print|printinput|del)
                                                   run  Run tests and compare output
                                                   capture  Re-capture test results
                                                   covcheck  Check coverage against tgtcov
                                                   covcapture  Capture new coverage percentages
                                                   memcheck  Run under valgrind memcheck
                                                   valgrind  Run under valgrind (general)
                                                   mdbg  Debug single test under mdbg
                                                   edit  Edit test definition (acr -t -e)
                                                   editsource  Edit test function source code
                                                   print  Print reference output
                                                   printinput  Print test input lines
                                                   del  Delete selected comptests
    -mdbg                                      (action) Run component test under debugger
    -run                      Y                (action) Run selected component tests
    -capture                                   Alias for -mode:capture
    -ee                                        Alias for -mode:editsource
    -e                                         Alias for -mode:edit
    -print                                     (action) Print testcase
    -cfg              string  "release"        Configuration (determines bindir)
    -printinput                                (action) Print input of test case
    -maxerr           int     3                Exit after this many errors
    -normalize                                 (action) Renumber and normalize tmsgs
    -covcapture                                (action) Capture new coverage percentages and save back
    -covcheck                                  (action) Check coverage percentages against tgtcov table
    -bindir           string  ""               Directory with binaries (default: build/cfg)
    -tempdir          string  "temp/atf_comp"  Temp directory
    -testdir          string  "test/atf_comp"  Test data directory
    -check_untracked          Y                Check for untracked file before allowing test to run
    -build                                     Build given cfg before test
    -memcheck                                  Run under memory checker (valgrind)
    -force                                     (With -memcheck) run suppressed memcheck
    -callgrind                                 Run under callgrind profiler (valgrind)
    -maxjobs          int     0                Maximum number of tests run in parallel
    -stream                                    prints component's output
    -i                                         Read and execute testcase from stdin
    -write                    Y                (implied with -e) Write any changes back to ssim tables
    -report                   Y                Print final report
    -b                string  ""               Breakpoint passed to mdbg as-is
    -covfast                  Y                Speedup coverage processing
    -minrepeat        int     0                Execute every comptest at least this many times
    -maxrepeat        int     1                Don't repeat any individual comptest more than X times
    -verbose          flag                     Verbosity level (0..255); alias -v; cumulative
    -debug            flag                     Debug level (0..255); alias -d; cumulative
    -help                                      Print help and exit; alias -h
    -version                                   Print version and exit
    -signature                                 Show signatures and exit; alias -sig

```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Syntax -->

### Process API
<a href="#process-api"></a>

Each test function uses these calls to manage child processes:

- `ProcStart(cmd)` -- spawn a subprocess via `bash -c`. Apply $-substitution
  for variables like `$bindir`, `$tempdir`. Derive a unique process
  name from the command basename. Return a reference to FProc.
- `ProcWrite(proc, msg)` -- write a line to the process stdin.
- `ProcWriteEof(proc)` -- close stdin (signal EOF to the child).
- `ProcRead(proc, until)` -- read stdout until `until` appears (or EOF if empty).
  Use poll with 1-second timeout; fail the test if the comptest timeout is exceeded.
  Save data past the match line into a readahead buffer for the next ProcRead.
- `ProcWait(proc)` -- close stdin, drain stdout, waitpid.
- `ProcKill(proc, signal)` -- send signal to the child process.

All process output is logged as `<procname> -> <line>`, input as `<procname> <- <line>`.
FProc holds a `command::bash_proc` field; destroying FProc kills the subprocess.

### Output filtering
<a href="#output-filtering"></a>

Two mechanisms stabilize non-deterministic output before comparison:

**unstableattr** (`atfdb.unstableattr`): mask ssim tuple attributes whose values
vary between runs (timestamps, random ports, UUIDs). When `stablefld:Y` is set
on a comptest, each output line is parsed as an ssim tuple and matching attributes
are replaced with `***`. Entries can be exact (`samp_meng.NewOrderMsg.time`) or
wildcard (`%.timestamp`, `%.port`). The lookup check both `<head>.<attr>` and
`%.<attr>`.

**tfilt** (`atfdb.tfilt`): per-test output filter applied once on the complete log.
Specify a shell command that reads stdin and write stdout, e.g.
`sed -E -f test/filt.sed`. Use for non-ssim patterns (free-text timestamps,
IP:port in log messages, line deletions). The filter runs in both capture and
run modes.

### Modes
<a href="#modes"></a>

    atf_comp 'acr.%'                  # run all acr tests, compare with reference
    atf_comp acr.BadInsert -capture   # re-capture reference output
    atf_comp acr.BadInsert -v         # verbose: print log as it's generated
    atf_comp acr.BadInsert -e         # edit test definition (acr comptest -t -e)
    atf_comp acr.BadInsert -ee        # edit test function source (src_func -e)
    atf_comp acr.BadInsert -mode:mdbg # debug single test under mdbg
    atf_comp -mode:print 'acr.%'     # print reference files
    atf_comp -mode:printinput 'acr.%' # print input lines only
    atf_comp -mode:memcheck           # run under valgrind memcheck
    atf_comp -mode:valgrind           # run under valgrind (general)
    atf_comp -mode:covcheck           # check coverage against tgtcov
    atf_comp -mode:covcapture         # capture new coverage percentages

### Options
<a href="#options"></a>

<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Options -->
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

#### -comptest -- Select comptest (SQL regex)
<a href="#-comptest"></a>

#### -mode -- Test mode
<a href="#-mode"></a>

#### -mdbg -- (action) Run component test under debugger
<a href="#-mdbg"></a>

#### -run -- (action) Run selected component tests
<a href="#-run"></a>

#### -capture -- Alias for -mode:capture
<a href="#-capture"></a>

#### -ee -- Alias for -mode:editsource
<a href="#-ee"></a>

#### -e -- Alias for -mode:edit
<a href="#-e"></a>

#### -print -- (action) Print testcase
<a href="#-print"></a>

#### -cfg -- Configuration (determines bindir)
<a href="#-cfg"></a>

#### -printinput -- (action) Print input of test case
<a href="#-printinput"></a>

#### -maxerr -- Exit after this many errors
<a href="#-maxerr"></a>

#### -normalize -- (action) Renumber and normalize tmsgs
<a href="#-normalize"></a>

#### -covcapture -- (action) Capture new coverage percentages and save back
<a href="#-covcapture"></a>

#### -covcheck -- (action) Check coverage percentages against tgtcov table
<a href="#-covcheck"></a>

#### -bindir -- Directory with binaries (default: build/cfg)
<a href="#-bindir"></a>

#### -tempdir -- Temp directory
<a href="#-tempdir"></a>

#### -testdir -- Test data directory
<a href="#-testdir"></a>

#### -check_untracked -- Check for untracked file before allowing test to run
<a href="#-check_untracked"></a>

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
|[atfdb.tfilt](/txt/ssimdb/atfdb/tfilt.md)||
|[atfdb.unstableattr](/txt/ssimdb/atfdb/unstableattr.md)||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Inputs -->


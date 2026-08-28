## atf_comp - Component test runner: spawn processes and diff the log against a reference
<a href="#atf_comp"></a>

Procedural component test runner. Each component test is a C++ function
loaded via gstatic from the `atfdb.comptest` table. Test functions use
a process API to spawn subprocesses, feed them input, and capture output
into a log. The log is compared against a reference file stored in
`test/atf_comp/<testname>`.

### Syntax
<a href="#syntax"></a>
```usage
atf_comp: Component test runner: spawn processes and diff the log against a reference
Usage: atf_comp [[-comptest:]<regx>] [options]
    OPTION      TYPE    DFLT       COMMENT
    -in         string  "data"     Input directory or filename, - for stdin
    [comptest]  regx    "%"        Select comptest (SQL regex)
    -mode       enum    run        Test mode (run|capture|memcheck|valgrind|mdbg|edit|editsource|print|printinput|mdbgall|del)
    -capture                       Alias for -mode:capture
    -ee                            Alias for -mode:editsource
    -e                             Alias for -mode:edit
    -cfg        string  "release"  Configuration (determines bindir)
    -maxerr     int     3          Exit after this many errors
    -verbose    flag               Verbosity level (0..255); alias -v; cumulative
    -debug      flag               Debug level (0..255); alias -d; cumulative
    -help                          Print help and exit; alias -h
    -version                       Print version and exit
    -signature                     Show signatures and exit; alias -sig
```

### Process API
<a href="#process-api"></a>

Each test function uses these calls to manage child processes:

- `ProcStart(cmd)` -- spawn a subprocess via `bash -c`. Apply $-substitution
  for variables like `$bindir` and `$tempdir`. Derive a unique process
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
    atf_comp -cfg:coverage            # run the coverage comptest subset (atf_ci owns gcov/merge)

The namespaces that have comptests are whatever `acr comptest` reports; each
names the tool or the subsystem its tests drive.

### Options
<a href="#options"></a>
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

#### -comptest -- Select comptest (SQL regex)
<a href="#-comptest"></a>

#### -mode -- Test mode
<a href="#-mode"></a>

#### -capture -- Alias for -mode:capture
<a href="#-capture"></a>

#### -ee -- Alias for -mode:editsource
<a href="#-ee"></a>

#### -e -- Alias for -mode:edit
<a href="#-e"></a>

#### -cfg -- Configuration (determines bindir)
<a href="#-cfg"></a>

#### -maxerr -- Exit after this many errors
<a href="#-maxerr"></a>

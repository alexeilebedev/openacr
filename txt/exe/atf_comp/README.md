## atf_comp - Component test runner: spawn processes and diff the log against a reference
<a href="#atf_comp"></a>

Procedural component test runner. Each component test is a C++ function
loaded via gstatic from the `atfdb.comptest` table. Test functions use
a process API to spawn subprocesses, feed them input, and capture output
into a log. The log is compared against a reference file stored in
`test/atf_comp/<testname>`.

### Table Of Contents
<a href="#table-of-contents"></a>
<!-- abt_md.toc_beg -->
&nbsp;&nbsp;&bull;&nbsp;  [Internals](#internals)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Process API](#process-api)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Output filtering](#output-filtering)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Modes](#modes)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Options](#options)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
<!-- abt_md.toc_end -->

### Internals
<a href="#internals"></a>
&#128196; [atf_comp - Internals](/txt/gen/atf_comp/atf_comp.md)<br/>

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

### Inputs
<a href="#inputs"></a>
`atf_comp` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|
|[atfdb.testenv](/txt/ssimdb/atfdb/testenv.md)||
|[atfdb.tfilt](/txt/ssimdb/atfdb/tfilt.md)||
|[atfdb.unstableattr](/txt/ssimdb/atfdb/unstableattr.md)||

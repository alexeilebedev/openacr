## atf_comp - Component test runner: spawn processes and diff the log against a reference
<a href="#atf_comp"></a>

`atf_comp` runs the component tests, which check a tool from the outside by
running it as a process.  A comptest is a C++ function that starts processes,
writes their input and reads what they print.  The runner records all of that
in a log and compares the log with a reference file committed under
`test/atf_comp/`, and the test passes when the two agree.

### Syntax
<a href="#syntax"></a>
```usage
atf_comp: Component test runner: spawn processes and diff the log against a reference
Usage: atf_comp [[-comptest:]<regx>] [options]
    OPTION      TYPE    DFLT       COMMENT
    -in         string  "data"     Input directory or filename, - for stdin
    [comptest]  regx    "%"        Select comptest (SQL regex)
    -cijob      regx    "%"        (with -mode:memcheck) run only comptests whose memcheck cijob matches
    -mode       enum    run        Test mode (run|capture|memcheck|valgrind|mdbg|edit|editsource|print|printinput|mdbgall|del)
    -capture                       Alias for -mode:capture
    -ee                            Alias for -mode:editsource
    -e                             Alias for -mode:edit
    -cfg        string  "release"  Configuration (determines bindir)
    -maxerr     int     3          Exit after this many errors
    -verbose    flag               Verbosity level (0..255); alias -v; cumulative
    -debug      flag               Debug level (0..255); alias -d; cumulative
    -trace      string  ""         Trace expression: category[:filter],...; also payload_lim:N, verbose, debug, timestamps
    -help                          Print help and exit; alias -h
    -version                       Print version and exit
    -signature                     Show signatures and exit; alias -sig
```

### Description
<a href="#description"></a>

#### Where the tests live
<a href="#where-the-tests-live"></a>

Each comptest is a row of `atfdb.comptest`, named `<ns>.<name>` after the
namespace it drives, and there are ~1000 of them.  The row gives the test's
timeout in seconds, the memcheck cijob that runs it under valgrind, whether the
coverage run includes it, and `stablefld`, which turns on masking of unstable
attributes.  The test's function is `comptest_<ns>_<name>` in
`cpp/atf_comp/<ns>.cpp`, and its reference file is `test/atf_comp/<ns>.<name>`.
`acr comptest` lists the tests, and `atf_comp` carries the rows it was built
with, since the table is compiled in.

#### Running a test and reading its log
<a href="#running-a-test-and-reading-its-log"></a>

For each selected comptest, `atf_comp` empties the directory
`temp/atf_comp/<name>`, calls the test's function, and waits for every process
the function started.  It then compares the log with the reference and prints
the colored diff when they differ.  A test that throws prints its log instead.
Each test reports an `atf_comp.begin` and an `atf_comp.end` line, each failure
adds an `atf_comp.fail` line naming its reason, and a final `report.atf_comp`
line counts the run.  The exit code is nonzero when any test failed.

A command line names the build under test and the test's own directory through
variables: `$bindir` is `build/<cfg>`, `$tempdir` is `temp/atf_comp/<name>`, and
`$comptest` and `$timeout` are the test's name and budget.  Every row of
`atfdb.testenv` goes into the environment as well.

Here is `acr.BadInsert`, which starts `acr -insert` and writes two records
into it:

```cpp
void atf_comp::comptest_acr_BadInsert() {
    atf_comp::FProc &proc = atf_comp::ProcStart("$bindir/acr -insert ns:ns%");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns1        nstype:exe      license:xx   comment:\"This record will be inserted\"");
    atf_comp::ProcWrite(proc, "dmmeta.ns  ns:ns1        nstype:lib      license:yy   comment:\"This record will not\"");
}
```

Its reference file `test/atf_comp/acr.BadInsert` is the log that run produces:

```text
# start acr cmd:"$bindir/acr -insert ns:ns%"
acr <- dmmeta.ns  ns:ns1        nstype:exe      license:xx   comment:"This record will be inserted"
acr <- dmmeta.ns  ns:ns1        nstype:lib      license:yy   comment:"This record will not"
# eof acr
acr -> acr.insert  dmmeta.ns  ns:ns1  nstype:exe  license:xx  comment:"This record will be inserted"
acr -> report.acr  n_select:1  n_insert:1  n_delete:0  n_ignore:1  n_update:0  n_file_mod:0  n_badline:0
# exit acr code:0
```

A line marked `<-` is input the test wrote, and a line marked `->` is output the
process printed.  Each process takes the name of the program it runs, and a
second process of the same program carries the name with `-2` appended.  The command
lines keep their variables unreplaced, so the reference reads the same on every
machine.

#### Writing a comptest
<a href="#writing-a-comptest"></a>

A test function drives its processes with these calls:

- `ProcStart(cmd)` starts `cmd` under bash, after `$`-substitution, and returns
  the process.
- `ScriptStart(name, script)` writes a script, one command per line, to
  `name.sh` in the tempdir and runs bash on it.  The log shows each line as
  `# name.sh: <line>`.
- `ProcWrite(proc, line)` writes one line to the process's stdin.  An optional
  third argument is the line terminator, for a protocol that wants `"\r\n"`.
- `ProcWriteEof(proc)` closes the process's stdin.
- `ProcRead(proc, until)` reads the process's output until the text `until`
  appears, or to end of file when `until` is empty, and returns what it read.
- `ProcWait(proc, code)` closes stdin, reads the rest of the output, and fails
  the test unless the process exited with `code`, which defaults to zero.  A
  process killed with `ProcKill(proc, signal)` reports -1.
- `SetVar(name, value)` defines `$name` for later command lines.
- `SetEnv(name, value)` and `UnsetEnv(name)` change the environment of every
  process started after them, and `ClearEnv()` restores the runner's own.  The
  runner also restores it after every test.
- `Stage(name)` logs `# stage name`, so a reader can tell one run of the tool
  from the next.
- `TcpStart(addr, keep)` connects to `host:port` and returns the connection
  as a process: `ProcWrite` sends a line, `ProcRead` reads the server's lines
  without their carriage returns, and `ProcWait` closes it at once.  `keep`, a
  `|`-separated list of line prefixes, drops every other line.
- `ProcQuiet(proc, on)` keeps a process's writes and reads out of the log, for
  a probe repeated until the cluster answers it.
- `Sleep(secs)` waits, scaled by the build, and logs `# sleep secs`.
- `Part(name)` waits for every process the previous part started and logs
  `# part name`, so several checks can share one cluster.  A test with parts
  runs and captures as a whole; there is no selector for one part.
- `Check(name, value)` logs `# check name:value`, and `CheckFile(name, file)`
  logs whether a file under the tempdir is present, empty, a directory or absent.

A process that the test never waits for is still drained when the function
returns, and the runner logs its exit code without checking it.

#### Keeping the log stable
<a href="#keeping-the-log-stable"></a>

A timestamp, a port or a generated id changes from run to run, and four tables
keep such values out of the comparison.

- `atfdb.unstableattr` masks an attribute of an ssim tuple to `***`, for a
  comptest with `stablefld:Y`.  An entry names one tuple head, as in
  `gcache.error.err`, or every head, as in `%.builddate`.
- `atfdb.unstableline` drops every line whose tuple head it names.  Use it for
  a line that appears on some hosts and not on others.
- `atfdb.sortline` sorts each run of adjacent lines with the head it names,
  on both sides of the comparison, for events that arrive in no promised order.
- `atfdb.tfilt` gives one comptest a shell filter that reads the whole log on
  stdin and writes the filtered log on stdout.  `atf_cmdline.Sig` uses
  `sed -E -f test/filt.sed`.  The filter runs in capture mode as well.

#### Memory checking
<a href="#memory-checking"></a>

`-mode:memcheck` runs the comptests whose `memcheck` column names a cijob,
optionally narrowed by `-cijob`, and skips those tagged `none`.  It runs the
first process, and every process running the tool the test is named after,
under valgrind memcheck with a leak check.  A memory error or a definite leak
adds a `# memcheck errors:` line to the log, so the test fails against its
reference.  This mode multiplies timeouts by 30.

### Examples
<a href="#examples"></a>

```bash
atf_comp 'acr.%'                     # run every acr comptest
atf_comp acr.BadInsert               # run one comptest
atf_comp acr.BadInsert -v            # print the log as the test runs
atf_comp acr.BadInsert -capture      # rewrite the reference from this run
atf_comp -mode:print acr.BadInsert   # print the reference file, without running
atf_comp -mode:printinput 'acr.%'    # print only the input lines the tests write
atf_comp acr.BadInsert -e            # edit the atfdb.comptest row
atf_comp acr.BadInsert -ee           # open the test function in the editor
atf_comp acr.BadInsert -mode:mdbg    # run one test under the debugger
atf_comp -mode:memcheck -cfg:memcheck -cijob:memcheckC 'acr.%'  # memcheck one shard
acr comptest:amc.%                   # list the amc comptests
```

A passing run prints a progress line and a pair of lines per test, a separator
after each test, and a summary.  atf_x2 prints the same progress line and
separator:

```
test 1/1 (100%), elapsed:00:00:00
atf_comp.begin  comptest:acr.BadInsert
atf_comp.end  comptest:acr.BadInsert  success:Y  nlines:7  duration:0.0243471
--------------------------------------------------------------------------------

report.atf_comp  nrun:1  npass:1  nerr:0
```

### Caveats
<a href="#caveats"></a>

- A captured reference cannot validate a change in behavior.  `-capture`
  records whatever the binary does now, so a run afterwards compares the change
  with itself and passes.  Read the diff that `-capture` prints, and ask of
  every changed line whether you meant it.
- `-mode:print` does not run the test.  Its output matches the reference by
  construction, so it is no evidence that a test passes.
- The command a test runs is in `cpp/atf_comp/<ns>.cpp`.  The reference opens
  with a `# start` line that looks like the script, but editing it changes only
  what the test expects.
- A new `atfdb.comptest` row runs only after `amc` and a rebuild of
  `atf_comp`.  A comptest with no reference file fails with
  `atf_comp.noref` until you capture one.
- A regex names the tests you thought of.  After a change that alters what many
  tests print, such as a new field in a ctype whose rows many references hold, run
  `atf_ci -citest:atf_comp`, which is what the `comp` job runs.
- Masking reaches ssim tuples only.  No table can mask a column of a
  human-readable table, so add `-ssim` to the command where the tool has it.
- The run stops after three failures by default.  Raise `-maxerr` to see them
  all.

### Options
<a href="#options"></a>
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

The directory `atf_comp` reads its filter tables from: `atfdb.tfilt`,
`atfdb.unstableattr`, `atfdb.unstableline`, `atfdb.sortline` and
`atfdb.testenv`.  The comptests themselves are compiled into the binary and do
not come from here.

#### -comptest -- Select comptest (SQL regex)
<a href="#-comptest"></a>

Selects the comptests to run by name, as an SQL regex such as `'acr.%'`.  The
default `%` selects all of them, and a pattern that matches nothing fails with
`atf_comp.nomatch`.

#### -cijob -- (with -mode:memcheck) run only comptests whose memcheck cijob matches
<a href="#-cijob"></a>

Narrows a memcheck run to the comptests whose `memcheck` column matches this
regex, such as `memcheckA`.  CI splits the memcheck tests into shards this way.
Other modes ignore it.

#### -mode -- Test mode
<a href="#-mode"></a>

Picks what `atf_comp` does with the selected tests.  `run` runs them and
compares each log with its reference, and `capture` writes the log over the
reference instead.  `memcheck` runs them under valgrind memcheck, and
`valgrind` runs each test's first process under plain valgrind with timeouts
scaled by 50.  `print` prints the reference files, and `printinput` prints only
the lines the tests wrote to their processes.  `edit` and `editsource` open the
test's row or its function in the editor.  `mdbg` runs one test in the debugger,
stopped at its function, and `mdbgall` runs it without stopping.  `del` deletes
the selected `atfdb.comptest` rows and the rows that depend on them.

#### -capture -- Alias for -mode:capture
<a href="#-capture"></a>

Runs the tests and writes each log to its reference file, printing the diff
against the old reference first.  `atf_comp` adds a new reference file to git, and
`update-gitfile` runs at the end.  Use it after a change to what a test says,
and read the diff before you commit it.

#### -ee -- Alias for -mode:editsource
<a href="#-ee"></a>

Opens the functions of the selected comptests in the editor, through `src_func
-e`.

#### -e -- Alias for -mode:edit
<a href="#-e"></a>

Opens the selected `atfdb.comptest` rows in the editor, through `acr -t -e`, so
the related rows such as a test's `atfdb.tfilt` come along.

#### -cfg -- Configuration (determines bindir)
<a href="#-cfg"></a>

Picks the build under test, so `$bindir` becomes `build/<cfg>`.  Timeouts are
scaled by four under `debug` and `coverage`, and `coverage` also narrows the
selection to the comptests marked `coverage:Y`.  Pass `-cfg:memcheck` with
`-mode:memcheck` so valgrind can see leaks inside amc pools; the debugger modes
switch to `debug` on their own.

#### -maxerr -- Exit after this many errors
<a href="#-maxerr"></a>

Stops the run once this many tests have failed, and prints `atf_comp.maxerr`.
The default of three suits a quick check; a run meant to list every failure
wants a large value.

## atf_ci - Normalization tests (see citest table)
<a href="#atf_ci"></a>
`atf_ci` runs the checks that CI runs, one citest after another.  A citest is a
step compiled into `atf_ci`: it may regenerate files and require that nothing
changed, run a whole test suite such as `atf_comp`, or exercise a tool in a
sandbox.  Each citest belongs to a cijob, and a CI job is one `atf_ci` run over
one cijob.

### Syntax
<a href="#syntax"></a>
```usage
atf_ci: Normalization tests (see citest table)
Usage: atf_ci [[-citest:]<regx>] [options]
    OPTION        TYPE    DFLT    COMMENT
    -in           string  "data"  Input directory or filename, - for stdin
    [citest]      regx    "%"     Regx of tests to run
    -maxerr       int     0       Exit after this many errors
    -cijob        regx    "%"
    -capture                      Capture the output of the test
    -check_clean          Y       Check for modifications after each test
    -verbose      flag            Verbosity level (0..255); alias -v; cumulative
    -debug        flag            Debug level (0..255); alias -d; cumulative
    -trace        string  ""      Trace expression: category[:filter],...; also payload_lim:N, verbose, debug, timestamps
    -help                         Print help and exit; alias -h
    -version                      Print version and exit
    -signature                    Show signatures and exit; alias -sig
```

### Description
<a href="#description"></a>

#### Where the citests live
<a href="#where-the-citests-live"></a>

Each citest is a row of `atfdb.citest`, and there are ~85 of them.  The row
names the cijob the test belongs to, whether it runs in a sandbox, its timeout
in seconds, and whether a failure ends the run (`failfast`).  The cijobs are
the rows of `atfdb.cijob`: `normalize` is the fast pre-merge gate, `comp` runs
the unit and component tests, the `memcheck` shards and `coverage` run the
comptests under valgrind and gcov.  The test's function is `citest_<name>`, in
the file under `cpp/atf_ci/` named after its cijob.  `acr_ed -create -citest`
adds a row to the `normalize` job.

#### Running the citests
<a href="#running-the-citests"></a>

`atf_ci` runs, in table order, each citest whose name matches `-citest` and
whose cijob matches `-cijob`.  A lock file, `lock/atf_ci`, keeps two runs in one
checkout from overlapping.  Each citest prints an `atf_ci.begin` line and an
`atf_ci.citest` line with its runtime and verdict, and the run ends with a
`report.atf_ci` line.  The exit code is nonzero when any citest failed.

With `-check_clean`, which is on by default, the run refuses to start in a tree
with modified files.  After each citest it lists the files that test modified,
as `atf_ci.modified_files`, and fails the test.  This is how the `normalize`
job works: its citests regenerate files, formatting and generated code in
place, and a citest that changes anything tells you the committed tree was not
normalized.  Read the diff, commit the change if it is right, and run again.

A citest marked `sandbox:Y` runs in the `atf_ci` sandbox, `wt/atf_ci`.  The
first such test in a run resets the sandbox, the later
ones clean it, and the clean check skips them.  A citest in the `coverage` job
writes its gcov data to `temp/cov/<citest>.d`.

A citest that runs past its timeout ends the whole run with an
`atf_ci.timeout` line naming it and exit code 124.  The citests after it never
ran, so the run says nothing about them.

#### Where a tracked file may live
<a href="#where-a-tracked-file-may-live"></a>

`dev.gitpath` lists the places a tracked file may live, one pattern per row:
`cpp/%.cpp`, `include/%.h`, `bin/%`, `extern/%`.  The `gitfile` citest
regenerates `dev.gitfile` from git and fails on a file that no pattern
matches, printing `atf_ci.stray_file`.  Put a new kind of file where an
existing pattern allows it, or insert a row for its new place:

```bash
echo 'dev.gitpath  gitpath:python/%.py  comment:"Python programs"' | acr -insert -write
```

`bin/normalize` is `ai` followed by `atf_ci -maxerr:1000 -cijob:normalize`, and
it takes a cijob name as its argument.

### Examples
<a href="#examples"></a>

```bash
bin/normalize                        # build, then run the normalize job and report every failure
bin/normalize comp                   # the same for the comp job
atf_ci -citest:atf_comp              # run the comptests the way the comp job does
atf_ci amc                           # run one citest: regenerate code with amc and check the tree
atf_ci -cijob:normalize -maxerr:100  # run the normalize job without stopping at the first failure
acr atfdb.citest.cijob:comp -field citest   # list the citests of the comp job
acr citest:amc                       # show one citest's row
```

### Caveats
<a href="#caveats"></a>

- The `normalize` citests always write their results into the tree, with or
  without `-capture`.  Run them in a clean tree, and read what they changed.
- A dirty tree stops the run before it starts, with `atf_ci.dirty_tree`.
  Commit the work first, or pass `-check_clean:N` when you only want the
  test's verdict.
- The run stops at the first failure by default.  `bin/normalize` passes
  `-maxerr:1000` so a single run reports them all.
- A timeout ends the run and leaves every later citest unrun, so exit code 124
  is an incomplete verdict.
- `atf_ci -citest:atf_comp` is the component-test gate.  An `atf_comp` run with
  a pattern tests only what the pattern names.

### Options
<a href="#options"></a>
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

The directory `atf_ci` loads its ssim tables from, such as `dev.gitfile`,
`dev.targsrc` and `dmmeta.ssimfile`, which some citests check against the tree.
The default is `data`.

#### -citest -- Regx of tests to run
<a href="#-citest"></a>

Selects citests by name, as an SQL regex, and combines with `-cijob`.  A
pattern that matches nothing fails with `atf_ci.nomatch` and prints the list of
citests.

#### -maxerr -- Exit after this many errors
<a href="#-maxerr"></a>

The number of failed citests the run tolerates before it stops.  The default of
zero stops at the first failure; a `failfast` citest stops the run whatever
this value is.

#### -cijob -- 
<a href="#-cijob"></a>

Selects citests by their cijob, as an SQL regex such as `normalize` or
`'memcheck%'`.  This is how a CI job names its share of the citests, and it
combines with `-citest`.

#### -capture -- Capture the output of the test
<a href="#-capture"></a>

Passes `-capture` to the suites a citest runs, so `atf_unit` and `atf_comp`
rewrite their reference files.  Under the `coverage` job, `atf_cov` then
updates `dev.tgtcov` and skips its check.  The `normalize` citests capture with
or without this flag.

#### -check_clean -- Check for modifications after each test
<a href="#-check_clean"></a>

Refuses to start in a tree with modified files, and fails a citest that
modifies any.  Pass `-check_clean:N` to run a citest in a tree with work in
progress; its verdict then covers only its own step.

## atf_cov - Line coverage
<a href="#atf_cov"></a>

`atf_cov` measures which source lines a test run executed.  It runs a binary
built with GCC's coverage instrumentation, turns the profile data the binary
leaves behind into per-line hit counts with `gcov`, and reports coverage per
file, per target and in total.  It can also compare each target's coverage with
the floor recorded in `dev.tgtcov`, which is how a test suite keeps coverage
from dropping.

### Syntax
<a href="#syntax"></a>
```usage
atf_cov: Line coverage
Usage: atf_cov [options]
    OPTION        TYPE    DFLT                              COMMENT
    -in           string  "data"                            Input directory or filename, - for stdin
    -covdir       string  "temp/covdata"                    Output directory to save coverage data
    -logfile      string  ""                                Log file
    -runcmd       string  ""                                command to run
    -exclude      regx    "(extern|include/gen|cpp/gen)/%"  Exclude gitfiles (external, generated)
    -mergepath    string  ""                                colon-separated dir list to load .cov.ssim files from
    -gcov                                                   run gcov
    -ssim                                                   write out ssim files
    -report                                                 write out all reports
    -capture                                                Write coverage information into tgtcov table
    -xmlpretty                                              Generate pretty-formatted XML
    -summary              Y                                 Show summary figures
    -check                                                  Check coverage information against tgtcov table
    -incremental                                            Keep *.gcda files from previous run
    -verbose      flag                                      Verbosity level (0..255); alias -v; cumulative
    -debug        flag                                      Debug level (0..255); alias -d; cumulative
    -trace        string  ""                                Trace expression: category[:filter],...; also payload_lim:N, verbose, debug, timestamps
    -help                                                   Print help and exit; alias -h
    -version                                                Print version and exit
    -signature                                              Show signatures and exit; alias -sig
```

### Description
<a href="#description"></a>

A run of `atf_cov` goes through up to four stages, and each option turns one of
them on.  `-runcmd` runs the instrumented command, `-gcov` turns its profile
data into line counts, `-ssim` saves those counts, and `-report` writes the
reports.  Every stage reads and writes the coverage directory given by
`-covdir`.  Run it from the top of the checkout, since the profile files name
their sources relative to it.

#### What a line's coverage means
<a href="#what-a-line-s-coverage-means"></a>

A line is executable when the compiler emitted code for it.  Comments, blank
lines, declarations, templates nobody instantiated and inline functions nobody
called are non-executable.  A line that holds several branches, such as a
one-line `if` or a `&&` chain, is partially executed when only some of them ran.
It still counts as executed.

A file's coverage is its executed lines divided by its executable lines.  A
target's coverage adds up the files `dev.targsrc` lists for it, and the total
adds up every target.  A source file that belongs to no target is never counted,
and neither is one matching `-exclude`, which by default drops third-party and
generated code.

#### Building and running an instrumented binary
<a href="#building-and-running-an-instrumented-binary"></a>

`abt -cfg:coverage` builds with g++'s coverage flags, and the result lands under
`build/coverage/`.  Such a binary counts the lines it runs.  When it exits, it
writes one `.gcda` file per object file into the directory named by the
`GCC_PROFILE_DIR` environment variable.

`-runcmd` sets `GCC_PROFILE_DIR` to the coverage directory and runs the command
through bash.  Before it runs, it deletes every output of an earlier run from
that directory, so each run starts clean.  `-incremental` keeps the earlier
profile data, and the new run adds its counts to it.  To
measure several test cases separately, give each one its own `-covdir` and merge
them afterwards.

#### Turning profile data into line counts
<a href="#turning-profile-data-into-line-counts"></a>

`-gcov` pairs each `.gcda` file with the `.gcno` program graph that the compiler
wrote beside the object, and runs `gcov` over them.  It moves the `.gcov` files
into the coverage directory and loads their hit counts into memory.  A line that
several runs executed adds up their counts.

`-ssim` writes the in-memory counts into one `.cov.ssim` file per source, with
each `/` of the path spelled `#`.  A row is a `dev.covline`:

```ssim
inline-command: acr dmmeta.field:dev.Covline.%
dmmeta.field  field:dev.Covline.covline  arg:algo.cstring  reftype:Val   dflt:""     comment:"Key: file:line"
dmmeta.field  field:dev.Covline.src      arg:dev.Gitfile   reftype:Pkey  dflt:""     comment:"Source file"
dmmeta.field  field:dev.Covline.line     arg:u32           reftype:Val   dflt:""     comment:"Source line"
dmmeta.field  field:dev.Covline.flag     arg:char          reftype:Val   dflt:"'N'"  comment:Flag
dmmeta.field  field:dev.Covline.hit      arg:u32           reftype:Val   dflt:""     comment:"Number of hits"
dmmeta.field  field:dev.Covline.text     arg:algo.cstring  reftype:Val   dflt:""     comment:"Line text"
report.acr  n_select:6  n_insert:0  n_delete:0  n_ignore:0  n_update:0  n_file_mod:0  n_badline:0
```

The flag says what kind of line it is:

```ssim
inline-command: acr dmmeta.fconst:dev.Covline.%
dmmeta.fconst  fconst:dev.Covline.flag/N  value:"'N'"  comment:Non-executable
dmmeta.fconst  fconst:dev.Covline.flag/E  value:"'E'"  comment:Executable
dmmeta.fconst  fconst:dev.Covline.flag/P  value:"'P'"  comment:"Executable, partially executed"
report.acr  n_select:3  n_insert:0  n_delete:0  n_ignore:0  n_update:0  n_file_mod:0  n_badline:0
```

A later run with neither `-gcov` nor `-mergepath` loads these files back from
the coverage directory.  That lets you produce reports again without rerunning
the command or `gcov`.

#### Merging several runs
<a href="#merging-several-runs"></a>

`-mergepath` names a colon-separated list of coverage directories and adds up
the counts from all of them.  Without `-gcov` it reads each directory's
`.cov.ssim` files.  With `-gcov` it runs `gcov` in each directory over the raw
`.gcda` files.  The outputs go to `-covdir`.

Each directory reports what it gave, on stderr:

```ssim
atf_cov.merge  covdir:temp/cov/atf_comp_cov.d  n_gcda:812  n_gcov:1104  n_fail:0  success:Y  comment:""
```

`n_gcda` counts the profile files found and `n_gcov` the `gcov` outputs they
produced.  `n_fail` counts `gcov` commands that failed, and an
`atf_cov.gcov_fail` line above names the file `gcov` refused.  A directory
holding profile data that yields no coverage reports `success:N` and fails the
run.

#### Reports
<a href="#reports"></a>

`-report` writes `report.ssim`, `report.txt` and `cobertura.xml` into the
coverage directory, along with `summary.txt` and `uncovfunc.ssim`.  The reports
list the total, then each target, then each file of that target.  A target row
is a `dev.covtarget`, and the total uses the target name `TOTAL`:

```ssim
inline-command: acr dmmeta.field:dev.Covtarget.%
dmmeta.field  field:dev.Covtarget.covtarget  arg:dev.Target    reftype:Pkey  dflt:""  comment:Target
dmmeta.field  field:dev.Covtarget.total      arg:u32           reftype:Val   dflt:""  comment:"Total lines"
dmmeta.field  field:dev.Covtarget.nonexe     arg:u32           reftype:Val   dflt:""  comment:"Non-executable lines"
dmmeta.field  field:dev.Covtarget.exe        arg:u32           reftype:Val   dflt:""  comment:"Executable lines"
dmmeta.field  field:dev.Covtarget.exer       arg:algo.U32Dec2  reftype:Val   dflt:""  comment:"Percentage of executable lines"
dmmeta.field  field:dev.Covtarget.hit        arg:u32           reftype:Val   dflt:""  comment:"Exercised lines"
dmmeta.field  field:dev.Covtarget.cov        arg:algo.U32Dec2  reftype:Val   dflt:""  comment:"Line coverage"
report.acr  n_select:7  n_insert:0  n_delete:0  n_ignore:0  n_update:0  n_file_mod:0  n_badline:0
```

A file row is a `dev.covfile`, with the same columns keyed by source file:

```ssim
inline-command: acr dmmeta.field:dev.Covfile.%
dmmeta.field  field:dev.Covfile.covfile  arg:dev.Gitfile   reftype:Pkey  dflt:""  comment:"Source file"
dmmeta.field  field:dev.Covfile.total    arg:u32           reftype:Val   dflt:""  comment:"Total lines"
dmmeta.field  field:dev.Covfile.nonexe   arg:u32           reftype:Val   dflt:""  comment:"Non-executable lines"
dmmeta.field  field:dev.Covfile.exe      arg:u32           reftype:Val   dflt:""  comment:"Executable lines"
dmmeta.field  field:dev.Covfile.exer     arg:algo.U32Dec2  reftype:Val   dflt:""  comment:"Percentage of executable lines"
dmmeta.field  field:dev.Covfile.hit      arg:u32           reftype:Val   dflt:""  comment:"Exercised lines"
dmmeta.field  field:dev.Covfile.cov      arg:algo.U32Dec2  reftype:Val   dflt:""  comment:"Line coverage"
report.acr  n_select:7  n_insert:0  n_delete:0  n_ignore:0  n_update:0  n_file_mod:0  n_badline:0
```

`report.txt` is the same data as a table.  `cobertura.xml` follows the
[Cobertura DTD](http://cobertura.sourceforge.net/xml/coverage-04.dtd), which
GitLab CI reads to mark covered lines in a merge request.  `uncovfunc.ssim`
lists every function whose executable lines all went unhit, as `dev.uncovfunc`
rows.  `atf_cov` gets the function extents from `src_func`.

The summary goes to stdout, or to the `-logfile` file when there is one, unless
you pass `-summary:N`.  It shows each target's
executable lines, hit lines and coverage, then the change against the target's
floor in `dev.tgtcov`.  Under the table sits a line that says how far the run
reached:

```ssim
report.atf_cov  n_covdir:6  n_covdir_empty:0  n_covtarget:77  n_tgtcov:145  n_unmeasured:0  exe:89643  hit:69420
```

`n_covdir` counts the directories merged, and `n_covdir_empty` those whose
profile data yielded nothing.  `n_covtarget` counts the targets measured, and
`n_tgtcov` the targets that carry a floor.  `n_unmeasured` counts targets with a
floor above zero that produced no data.

#### Checking and capturing floors
<a href="#checking-and-capturing-floors"></a>

`-check` compares every measured target with its `dev.tgtcov` floor.  It fails
with `atf_cov.coverage_lowered` when coverage falls more than five points under
the floor.  It judges the run as a whole first.  When a directory came back
empty or a target with a floor produced no data, it fails once, and it compares
no target:

```ssim
atf_cov.coverage_lost  n_covdir_empty:1  n_covtarget:18  n_unmeasured:56  success:N  target:"abt_md acr acr_compl amc ..."  comment:"run lost coverage data; no target is judged against its floor"
```

The targets named there are the ones the missing data would have measured.  Run
the job again.  The [coverage_lost
recipe](/txt/rule/openacr.md#the-coverage-cijob-fails-with-atf_cov-coverage_lost)
says how to find which directory lost its data.

`-capture` writes each measured coverage into `dev.tgtcov` as the target's new
floor.  It also rewrites `dev.uncovfunc` with this run's list of unhit
functions.  A run that lost data is refused with `atf_cov.capture_refused`, and
the floors stay as they were.

### Examples
<a href="#examples"></a>

```bash
abt acr -cfg:coverage                                                   # build an instrumented acr
atf_cov -runcmd "build/coverage/acr dmmeta.ns:amc" -gcov -ssim -report  # measure one command and report
atf_cov -report                                                         # report again from the saved .cov.ssim files
atf_cov -covdir temp/cov1 -runcmd "build/coverage/acr -check" -gcov -ssim   # measure a second case in its own directory
atf_cov -mergepath temp/covdata:temp/cov1 -covdir temp/merged -report   # merge two saved runs
atf_cov -mergepath temp/covdata:temp/cov1 -covdir temp/merged -gcov -report  # merge from raw .gcda files
atf_ci -cijob:coverage                                                  # run the whole suite and check every floor
atf_ci -cijob:coverage -capture                                         # run the whole suite and record new floors
```

To wrap a command that sits in a pipeline, send the log to a file so the
command's own output stays clean:

```bash
producer | atf_cov -runcmd command -logfile atf_cov.log | consumer
```

### Caveats
<a href="#caveats"></a>

- A process that is killed writes no coverage data.  Its counters live in memory
  until it exits, so a `SIGKILL` loses every line it ran.  Code that only a
  kill test reaches scores zero, and the target's floor sits well below its
  neighbors.  The `dev.tgtcov` row's comment says when that is the reason.
- `-check` and `-capture` only make sense on a run of the whole suite.  A run
  that measures one command leaves most targets with floors unmeasured, so
  `-check` reports `atf_cov.coverage_lost`.
- Use an empty directory for `-covdir`.  `atf_cov` deletes its earlier outputs
  there and writes thousands of files with long names, so a directory such as
  the checkout root or your home directory becomes hard to clean.
- A floor comes down only when someone edits `data/dev/tgtcov.ssim` by hand.  A
  target whose last test was deleted keeps its floor and produces no data, so
  every `-check` fails until that row is lowered.

### Options
<a href="#options"></a>
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

The dataset `atf_cov` reads its tables from: `dev.gitfile`, `dev.target`,
`dev.targsrc` and `dev.tgtcov`.  Leave it at `data` unless you are measuring
against another checkout's tables.

#### -covdir -- Output directory to save coverage data
<a href="#-covdir"></a>

The directory every stage reads and writes.  `-runcmd` points the binary's
profile output here, `-ssim` and `-report` write here, and a run with neither
`-gcov` nor `-mergepath` loads its counts from here.  `atf_cov` creates it when
it is missing.

#### -logfile -- Log file
<a href="#-logfile"></a>

Send the progress log and `gcov`'s output to this file.  Errors and verdicts,
such as `atf_cov.merge` and `atf_cov.coverage_lost`, still go to stderr.  Use it
when `-runcmd` sits in a pipeline, so the log does not mix with the command's
output.

#### -runcmd -- command to run
<a href="#-runcmd"></a>

Run this bash command with `GCC_PROFILE_DIR` set to `-covdir`.  Quote it when it
has arguments.  It first deletes the profile data and every derived output an
earlier run left in `-covdir`, and `-incremental` spares the profile data.

#### -exclude -- Exclude gitfiles (external, generated)
<a href="#-exclude"></a>

An SQL regex over source paths.  `-gcov` skips a matching source when it loads
line counts, so the file counts toward no target.  The default drops `extern/`
and the code `amc` generates.

#### -mergepath -- colon-separated dir list to load .cov.ssim files from
<a href="#-mergepath"></a>

Add up the coverage from each listed directory.  Without `-gcov` it reads their
`.cov.ssim` files, and with `-gcov` it runs `gcov` in each one.  Outputs go to
`-covdir`, which should be a separate directory.

#### -gcov -- run gcov
<a href="#-gcov"></a>

Run `gcov` over the profile data in `-covdir`, or in each `-mergepath`
directory, and load the line counts it produces.  It first clears the `.gcno`,
`.gcov`, `.cov.ssim` and report files a previous run left there.

#### -ssim -- write out ssim files
<a href="#-ssim"></a>

Write the loaded line counts into `-covdir` as one `.cov.ssim` file per source.
A later run can merge or report from these files without the profile data.

#### -report -- write out all reports
<a href="#-report"></a>

Write `report.ssim`, `report.txt`, `cobertura.xml`, `summary.txt` and
`uncovfunc.ssim` into `-covdir`.  It runs `src_func` to find function extents
for the unhit-function list.

#### -capture -- Write coverage information into tgtcov table
<a href="#-capture"></a>

Record each measured target's coverage as its floor in `dev.tgtcov`, and replace
`dev.uncovfunc` with this run's unhit functions.  It writes both ssimfiles
through `acr`.  It refuses a run that lost data.  Use it through `atf_ci
-cijob:coverage -capture`, which runs the whole suite first.

#### -xmlpretty -- Generate pretty-formatted XML
<a href="#-xmlpretty"></a>

Indent `cobertura.xml` so a person can read it.  It applies with `-report`.

#### -summary -- Show summary figures
<a href="#-summary"></a>

Print the per-target summary and the `report.atf_cov` line to stdout, or to the
`-logfile` file when there is one.  It is on by default, and `-summary:N` turns it off.  `-report` writes the same
table to `summary.txt` either way.

#### -check -- Check coverage information against tgtcov table
<a href="#-check"></a>

Fail when a target's coverage falls more than five points under its
`dev.tgtcov` floor.  A run that lost data fails once with
`atf_cov.coverage_lost`, and no target is compared.

#### -incremental -- Keep *.gcda files from previous run
<a href="#-incremental"></a>

Keep the profile data already in `-covdir` when `-runcmd` starts.  The
instrumented binary then adds its counts to the existing files, so several
commands accumulate into one measurement.

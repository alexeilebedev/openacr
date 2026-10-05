## amc_gc - Garbage collector for in-memory databases
<a href="#amc_gc"></a>
`amc_gc` finds schema records and `#include` lines that a target builds without,
and deletes them.  It tries each candidate in a sandbox, one at a time, and a
candidate whose removal still lets the target build is removed from your tree
as well.  Less schema means less generated code, which is what the report
measures.

### Syntax
<a href="#syntax"></a>
```usage
amc_gc: Garbage collector for in-memory databases
Usage: amc_gc [options]
    OPTION      TYPE    DFLT    COMMENT
    -target     regx    "%"     Target to test-build
    -key        regx    ""      ACR query selecting records to eliminate, e.g. dmmeta.ctype:amc.%
    -include                    Garbage collect includes for specified target
    -in         string  "data"  Input directory or filename, - for stdin
    -verbose    flag            Verbosity level (0..255); alias -v; cumulative
    -debug      flag            Debug level (0..255); alias -d; cumulative
    -trace      string  ""      Trace expression: category[:filter],...; also payload_lim:N, verbose, debug, timestamps
    -help                       Print help and exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig
```

### Description
<a href="#description"></a>

#### Removing records
<a href="#removing-records"></a>

`amc_gc -target:<regx> -key:<query>` runs `acr <query>` to list the candidate
records.  It runs `amc` in the current tree to count the generated lines, and it
resets the `amc_gc` sandbox to match the current tree.  Then it takes each
candidate in turn.  It resets the sandbox to that starting state and deletes the
record there with `acr -del -write`.  It runs `amc` and builds the target with
`abt`, stopping at the first compile error.  When the build succeeds, `amc_gc`
deletes the same record in your tree.

The final `report.amc_gc` line counts the records matched and deleted, and the
generated lines the deletions saved.

#### Removing #include lines
<a href="#removing-include-lines"></a>

`amc_gc -target:<regx> -include` works the same way over the `#include` lines of
the target's hand-written source files, the ones `dev.targsrc` lists outside
`gen/` and `extern/`.  It tries the lines from the last to the first, so a
deletion leaves the line numbers of the lines still to come unchanged.  An
include the target builds without is removed from the file in your tree.

Each trial writes its `amc` and `abt` output to `temp/amc_gc.build`, and the
first line `amc_gc` prints names a `watch` command to follow it.

### Examples
<a href="#examples"></a>

```bash
amc_gc -target:sample -key:ctype:sample.%       # drop the ctypes sample builds without
amc_gc -target:abt -include                     # drop the includes abt builds without
watch head -50 temp/amc_gc.build                # follow the current trial's build
```

A worked example makes a target that loads a table and never uses it:

```
$ acr_ed -create -target sample -write
$ acr_ed -create -finput -target sample -ssimfile dmmeta.ns -write
```

`sample` still compiles when its `ns` table is gone, so `amc_gc` removes
`sample.FNs`:

```
$ amc_gc -target:sample -key:ctype:sample.%
amc_gc.begin  tot_rec:2  n_cppline:...  watch_cmd:"watch head -50 temp/amc_gc.build"
amc_gc.analyze  query:dmmeta.ctype:sample.FDb  eliminate:N  rec_no:1  tot_rec:2  n_del:0  ...
amc_gc.analyze  query:dmmeta.ctype:sample.FNs  eliminate:Y  rec_no:2  tot_rec:2  n_del:1  ...  n_cppline_del:461
report.amc_gc  key:ctype:sample.%  n_match:2  n_del:1  n_cppline:...  n_cppline_del:461
```

Delete the target once you are done with it:

```
$ acr_ed -del -target sample -write
```

### Caveats
<a href="#caveats"></a>

- `amc_gc` edits your tree, so commit first and read `git diff` afterwards.  A
  record goes the way `acr -del -write` deletes it, together with the records
  that reference it.
- A successful build is the only test.  A record the code needs only at run
  time, such as a row the program looks up by name, can still be deleted, so run
  the tests after `amc_gc`.
- Each candidate is tried alone against the starting tree.  Two records that
  can each go while the other stays are both deleted, and the target may then
  fail to build.  Build the target again once `amc_gc` finishes.
- Every candidate costs an `amc` run and a build.  The default `-target:%`
  builds every target on each trial, so name the target you care about.

### Options
<a href="#options"></a>
#### -target -- Target to test-build
<a href="#-target"></a>

The targets `abt` builds in each trial, as a regx of `dev.target` names.  A
candidate is deleted only when all of them build.  With `-include`, it also
picks the source files whose includes are tried.

#### -key -- ACR query selecting records to eliminate, e.g. dmmeta.ctype:amc.%
<a href="#-key"></a>

The `acr` query that lists the candidate records, such as `ctype:sample.%` or
`dmmeta.field:sample.FDb.%`.  Each matching record is tried on its own.
`-include` ignores it.

#### -include -- Garbage collect includes for specified target
<a href="#-include"></a>

Try the `#include` lines of the target's hand-written sources in place of
schema records.  Each trial rebuilds with `abt` and skips the `amc` run, since
no record changed.

#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

The standard input-directory option.  `amc_gc` loads no table of its own, and
its `acr` and `amc` runs read `data/`, so the option has no effect.

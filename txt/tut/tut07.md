## Tutorial 7: Tests
<a href="#tutorial-7-tests"></a>

> **Run this in a sandbox.**  The commands below create a unit
> test and a citest and insert the rows that register them, so
> they change the schema and the source tree of whichever
> checkout runs them.  `wt tut_make -reset "bash -l"` gives you
> a shell in a sandbox, reset to match your current directory,
> and nothing you do there reaches your working tree — see
> [wt](/txt/exe/wt/README.md).

OpenACR has three concrete test runners — and one umbrella
driver that wraps them all.

| Tool       | What it runs                                                          |
|------------|-----------------------------------------------------------------------|
| `atf_unit` | Single-function **unit tests** linked into `atf_unit`'s own binary.   |
| `atf_comp` | **Component tests** — scripted invocations of real binaries, captured stdout/stderr diffed against reference files. |
| `atf_amc`  | **amc-generator tests** — assert that amc emits the expected C++ from given ssim inputs. |
| `atf_ci`   | The umbrella.  Runs **CI tests** (`citest` rows) grouped into **cijobs**.  Some citests are simple wrappers around `atf_unit`/`atf_comp`/`atf_amc`; others are custom logic (regenerate gitfile tables, check copyright headers, scan readmes, …). |

`bin/normalize` is just an alias for `atf_ci -cijob:normalize`
— the pre-merge gate.

### CI jobs (`cijob`)
<a href="#ci-jobs-cijob-"></a>

The full list of cijobs:

| `cijob`     | Purpose                                                                |
|-------------|------------------------------------------------------------------------|
| `normalize` | The pre-merge gate.  Fast invariants: gitfile/scanreadme/ssimfile checks, copyright headers, line endings, source-code police, generated-code freshness for selected targets, ssimdb round-trip via MariaDB, … |
| `comp`      | Unit tests + all component tests + amc tests + sandboxed editor tests. |
| `memcheck`  | Re-runs component tests under Valgrind memcheck.  Citests with `memcheck:N` are skipped. |
| `coverage`  | Re-runs component tests under `gcov`, checks each target's coverage against `dev.tgtcov` thresholds. |

Each cijob is a regex over `atfdb.citest.cijob`.  Run a single
cijob:

    $ atf_ci -cijob:comp

…or just one citest from any cijob:

    $ atf_ci memcheck                   # exact name
    $ atf_ci -citest:%spnx%             # SQL regex

`bin/normalize` is `atf_ci -cijob:normalize` plus syntax
highlighting.  Every commit on the integration branch should
pass it.

### Anatomy of a citest
<a href="#anatomy-of-a-citest"></a>

A citest is one row in `atfdb.citest` plus one C++ function:

```
atfdb.citest  citest:gitfile     cijob:normalize  sandbox:N  comment:"Update gitfile tables by scanning filesystem"
atfdb.citest  citest:atf_unit    cijob:comp       sandbox:N  comment:"Run unit tests"
atfdb.citest  citest:atf_comp    cijob:comp       sandbox:N  comment:"Run component tests"
atfdb.citest  citest:apm         cijob:comp       sandbox:Y  comment:"Test APM"
```

amc generates a prototype for each citest as
`void atf_ci::citest_<name>()`.  The implementation lives in
the matching cijob's `.cpp` file under `cpp/atf_ci/` (e.g.,
`cpp/atf_ci/normalize.cpp` for `atfdb.cijob:normalize`).

A citest with `sandbox:Y` runs inside a fresh sandboxed
checkout — useful for tests that create files, mutate the
schema, or call `acr_ed -create`.  `sandbox:N` runs in place
and is expected to be read-only or self-cleaning.

Some citests are very thin wrappers:

- `citest_atf_unit` just spawns `atf_unit` and reports its
  exit code.
- `citest_atf_comp` does the same for `atf_comp`.
- `citest_atf_amc` does the same for `atf_amc`.

Others are full implementations:

- `citest_gitfile` walks the filesystem and reconciles
  `dev.gitfile` with what's actually checked into git.
- `citest_src_lim` runs the "source code police" — line length,
  brace style, copyright presence, etc.
- `citest_normalize_acr_my` round-trips every ssim table
  through a temporary MariaDB instance.

The cascade is intentional: instead of "every test runner has
its own entry point," everything funnels through one umbrella
that produces uniform `atf_ci.citest success:Y/N` log lines.

### Adding a unit test
<a href="#adding-a-unit-test"></a>

Unit tests live in `cpp/atf/unit/<ns>.cpp` and are named in
`atfdb.unittest`.  List them with `acr unittest`.  Create one:

    $ acr_ed -create -unittest algo_lib.Test1 -write
    ...

This adds a row to `atfdb.unittest` and a stub to
`cpp/atf/unit/algo_lib.cpp`:

```c++
void atf_unit::unittest_algo_lib_Test1() {
    // test code goes here
}
```

The same can be done by hand:

    $ echo atfdb.unittest unittest:algo_lib.Test1 | acr -insert -write
    $ amc

Run it:

    $ atf_unit algo_lib.Test1
    $ atf_unit algo_lib.%      # all unit tests in a namespace
    $ atf_unit                 # everything

Debug a single test:

    $ atf_unit algo_lib.Test1 -mdbg

That breaks at the first line of the test function under
`mdbg`.

### Adding a component test
<a href="#adding-a-component-test"></a>

Component tests are scripted invocations of a real binary,
with expected output captured against a reference file.  Each
comptest is one row in `atfdb.comptest` plus one file at
`test/atf_comp/<comptest>`.

```
atfdb.comptest  comptest:acr.Insert  timeout:10  memcheck:Y  coverage:Y  stablefld:N  comment:"Insert a few records"
```

Per-test fields:

| Field        | Meaning                                                         |
|--------------|-----------------------------------------------------------------|
| `comptest`   | `<target>.<name>` — primary key.  `target` substring picks the binary to test. |
| `timeout`    | Per-process timeout in seconds (10 by default).  Auto-scaled in slow modes — see below. |
| `memcheck`   | Y = include in `-mode:memcheck` runs.  N = skip (e.g., expected to leak by design). |
| `coverage`   | Y = include in the `-cijob:coverage` run (`atf_comp -cfg:coverage`). |
| `stablefld`  | Y = replace volatile fields (timestamps, PIDs) with `***` before diff. |

The C++ entry point is `atf_comp::comptest_<name>()` (with the
target prefix dropped); it's typically a small script in
`cpp/atf_comp/<target>.cpp` that uses `proc_Alloc` /
`SetVar` / `WriteLine` to drive the subprocess.

The test definition file at `test/atf_comp/<comptest>`
interleaves three line kinds:

```text
# start acr cmd:"$bindir/acr -insert ns:ns%"
acr <- dmmeta.ns  ns:ns1  nstype:exe  license:xx  comment:"Will be inserted"
acr <- dmmeta.ns  ns:ns1  nstype:lib  license:yy  comment:"Will be ignored (dup pkey)"
# eof acr
acr -> acr.insert  dmmeta.ns  ns:ns1  nstype:exe  license:xx  ...
acr -> report.acr  n_select:1  n_insert:1  n_delete:0  n_ignore:1  n_update:0  n_file_mod:0
# exit acr code:0
```

- `# start <name> cmd:"…"` — spawn a subprocess named `<name>`.
- `<name> <- <line>` — send `<line>` to the subprocess's stdin.
- `# eof <name>` — close stdin.
- `<name> -> <line>` — assert this line appears on the
  subprocess's stdout/stderr.
- `# exit <name> code:N` — assert exit code.

Variables substituted by the framework:
`$bindir` (the build's bin dir), `$tempdir` (per-test scratch),
`$comptest` (the comptest name), `$timeout`.

### Component-test modes
<a href="#component-test-modes"></a>

`atf_comp -mode:` is the most-used option.  The modes:

| `-mode:`     | Effect                                                                       |
|--------------|------------------------------------------------------------------------------|
| `run`        | Default.  Run each test, diff stdout/stderr against reference, report mismatches. |
| `capture`    | Re-capture references.  Diffs old vs new before overwriting; `git add` new files. |
| `memcheck`   | Run under Valgrind memcheck.  Filters out tests with `memcheck:N`.  Timeout × 30. |
| `valgrind`   | Run under generic Valgrind (helgrind, drd, callgrind — picked via env).  Timeout × 50. |
| `mdbg`       | Debug a single test under [mdbg](/txt/exe/mdbg/README.md).  Requires exactly one matching comptest.  Uses the debug config. |
| `mdbgall`    | Like `mdbg`, but also wraps every child process the test spawns — multi-process debugging.  Use when the bug is in a subprocess, not in `atf_comp` itself. |
| `print`      | Print the expected output for the matched tests (no execution).              |
| `printinput` | Print just the stdin lines.                                                  |
| `edit`       | Open the matching comptest rows in `$EDITOR` via `acr -t -e`.                |
| `editsource` | Open the C++ functions implementing the matching tests.  Alias: `-ee`.       |
| `del`        | Delete the matching comptests (rows, reference files, C++ stubs).            |

Common combos:

    $ atf_comp acr.Insert                          # run one
    $ atf_comp 'acr\.%'                            # all acr.* tests
    $ atf_comp 'acr\.%' -mode:capture              # accept new output as the new reference
    $ atf_comp 'acr\.%' -mode:memcheck             # memcheck just the acr suite
    $ atf_comp acr.BadInsert -mode:mdbg            # debug one test
    $ atf_comp <ns>.<Test> -mode:mdbgall          # debug a test that spawns a process tree
    $ atf_ci -cijob:coverage                       # coverage gate (builds, runs, gcov-merges)

Timeouts auto-scale: 1× for release, 4× for debug/coverage,
30× for memcheck, 50× for valgrind.

### Output filters (`tfilt`)
<a href="#output-filters-tfilt-"></a>

Some comptests produce output that's stable in shape but
volatile in content (timestamps, build identifiers, optimizer
warnings that vary by compiler).  A `dmmeta.tfilt` row attaches
a shell filter that's applied to the captured log before
diffing:

```
atfdb.tfilt  comptest:acr.BadNs  filter:"sed -E 's/^Valid values.*$/Valid values ***/'"
```

There's also `atfdb.tifilt` for input-side filtering.

### Adding a CI test
<a href="#adding-a-ci-test"></a>

A CI test is a custom citest that doesn't fit the unit/comptest
mould — typically because it operates on the filesystem or on
the schema rather than on a single binary.

    $ acr_ed -create -citest mycheck -write
    ...

By hand:

    $ echo atfdb.citest citest:mycheck cijob:normalize | acr -insert -write
    $ amc

Then implement `atf_ci::citest_mycheck()` in
`cpp/atf_ci/normalize.cpp` (or whichever cijob `.cpp` matches).
The function returns `void` and signals failure by setting
`algo_lib::_db.exit_code` or by `prerr`-ing an error message.

Pick the cijob carefully — `normalize` is fast and runs every
commit, while `comp` includes the full comptest suite and takes
about five minutes.

### Debugging a failing test
<a href="#debugging-a-failing-test"></a>

The fastest paths:

- **Unit test:** `atf_unit <name> -mdbg`
- **Component test (single process):** `atf_comp <name> -mode:mdbg`
- **Component test (multi-process):** `atf_comp <name> -mode:mdbgall` —
  wraps every subprocess the test spawns under `mdbg` so you can
  break in whichever one is misbehaving.
- **Memcheck failure:** `atf_comp <name> -mode:memcheck -v` —
  the Valgrind report is the relevant signal.
- **Output diff:** when a comptest fails in `run` mode, the
  printed diff is between the new output and the reference.
  If the new output is *what you want*, re-run with
  `-mode:capture` to update the reference (then commit the
  change).

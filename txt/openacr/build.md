## Build and Test
<a href="#build-and-test"></a>

How to build the project and run the test suites.  All commands are run from
the top of the repo, with `bin/` on `PATH`.

`abt` replaces make and cmake.  It reads the same ssim database everything else
reads, so it already knows which sources belong to which target and what each
target links; a build is therefore selected with one regex over target names
rather than described in a makefile.

| Tool | Purpose |
|------|---------|
| `abt` | Build the targets a regex matches; installs into `bin/`. |
| `ai` | Bootstrap: build `abt` if missing, then `abt %`. |
| `abt_ts` | The same for the TypeScript packages under `ts/`. |
| `atf_unit` | Unit-test runner. |
| `atf_amc` | Code-generator unit tests. |
| `atf_comp` | Component tests: run a scripted process, diff its output against a stored reference. |
| `atf_ci` | CI driver — runs a whole `cijob` made of unit, comp, memcheck and coverage steps. |
| `bin/normalize` | `atf_ci -cijob:normalize`, the pre-merge gate. |

### Build commands
<a href="#build-commands"></a>

```bash
ai                           # bootstrap + build everything
abt %                        # build every target
abt '(x|y|z)'                # build several targets
abt <target> -install        # build one target and install it into bin/
abt -cfg:debug -install %    # a different configuration
make clean                   # rm build/ and temp/, forcing a full rebuild via ai
```

Use `ai` rather than `abt` after pulling or rebasing onto work that changed the
build tools themselves.  A tool cannot bootstrap a feature it predates, and
[/txt/rule/openacr.md](/txt/rule/openacr.md) explains what that looks like when
it happens.

### TypeScript
<a href="#typescript"></a>

```bash
abt_ts -normalize           # pnpm install in each package, then typecheck
abt_ts -build               # implies -normalize, then the bundle build
abt_ts -clean               # drop node_modules and the pnpm caches
```

A fresh checkout has no `ts/*/node_modules`, and that is a missing build rather
than a broken environment, exactly as an unbuilt `bin/` symlink is.  `ts/gen`
holds the TypeScript `amc` emits.  Prettier settings are single quotes, 120
columns, two-space indent, no semicolons, es5 trailing commas.

### External build-time dependencies
<a href="#external-build-time-dependencies"></a>

- **mariadb** / **libmariadbd** — for `acr_my`, `ssim2mysql`, `mysql2ssim`.
- **libssl** / **libcrypto** — SHA1, TLS.
- **libcurl** — HTTP client.
- **liblz4** — compression.
- **cppcheck**, **valgrind** — static analysis and memory check, used by CI.

### Tests
<a href="#tests"></a>

```bash
# C++ unit tests
atf_unit
atf_amc                                  # amc code-generator unit tests

# Component tests
acr comptest:<ns>.%                      # list comptests for namespace
atf_comp <ns>.<name>                     # run one
atf_comp <ns>.<name> -capture            # re-record reference (test/atf_comp/<name>)
atf_comp -mode:printinput <ns>.<name>    # print test input lines
cat ... | atf_comp -i                    # create a comptest interactively

# CI jobs
atf_ci -cijob:comp                       # compilation
atf_ci -cijob:memcheck                   # valgrind memcheck
atf_ci -cijob:coverage                   # coverage; with -capture, updates
                                         # data/dev/tgtcov.ssim
bin/normalize                            # main CI gate
```

A comptest command line runs from the root of the checkout, so every path in it
is written relative to that root: `$bindir/doc test/doc/sample.md -in:data`.
Reaching the checkout through `$OLDPWD` instead works only when the command line
performs its own `cd` first, because a `cd` is what sets `OLDPWD`.  With no `cd`
the value is inherited from whichever shell started the run, and on a developer's
machine that is usually the checkout, so the test passes locally and the captured
reference records that machine's absolute paths.  On a runner `OLDPWD` is the
runner's home, and the symptom is a whole namespace of comptests failing at once
with

```bash
bash -> bash: line 1: /home/gitlab-runner/build/release/<tool>: No such file or directory
```

which reads like a missing build and is a path bug.  A capture is worth checking
for the same reason: `grep /home/ test/atf_comp/<name>` should find nothing.

A component test is a stored conversation: the runner starts a process with a
scripted command line, captures everything it writes, and diffs that against
`test/atf_comp/<name>`.  So the reference file is the assertion, and
recapturing it is how a test stops testing.  The rules for reading a comptest
diff, and for what to do about a value that legitimately varies between runs,
are in [/txt/rule/openacr.md](/txt/rule/openacr.md).

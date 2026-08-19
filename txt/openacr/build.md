## Build and Test
<a href="#build-and-test"></a>

How to build the project and run the test suites.  All commands are
run from the top of the repo with `bin/` on `PATH`.

Tools covered here:

| Tool | Purpose |
|------|---------|
| `abt` | Build one or more targets (regex selection); installs into `bin/`. |
| `ai` | Bootstrap: builds `abt` if missing, then runs `abt %`. |
| `atf_unit` | Unit-test runner. |
| `atf_comp` | Component-test runner — scripted process invocation, stdout/stderr captured and diffed against a reference. |
| `atf_ci` | CI driver — runs an entire job (`-cijob:<name>`) made up of unit, comp, memcheck, coverage etc. |
| `bin/normalize` | Alias for `atf_ci -cijob:normalize` — the main pre-merge gate. |

### Build commands
<a href="#build-commands"></a>

`abt` takes one regex; multi-target via `abt '(x|y|z)'`.  `abt %`
builds all targets.  `ai` bootstraps `abt` if needed and then
builds everything — use it for a clean tree.

```bash
ai                           # bootstrap + build all
abt -cfg:release -install %  # all targets in release config
abt -cfg:debug   -install %  # all targets in debug config
abt <target> -install        # one target
make clean                   # rm build/ and temp/, force a full rebuild via ai
```

### A build directory older than the tools
<a href="#a-build-directory-older-than-the-tools"></a>

Suppose `abt <target>` compiles nothing at all, prints `abt can't find
build/gitinfo.h`, and dies.  Neither `PATH` nor the checkout is at fault.  The
library that every executable links includes the build-identity header that
`abt` writes at the start of a run, and the `abt` binary sitting in `build/` was
compiled before that header existed, so its own dependency scan now demands a
file it does not know how to write.  A tool cannot bootstrap a feature it
predates.  `ai` is what resolves it: it plants an empty `build/gitinfo.h` before
building, the build then succeeds unversioned, and the fresh `abt` it installs
replaces the file with the real stamp on the next run.  So after pulling or
rebasing onto work that changed the build tools, start with `ai` rather than
`abt`.

### One build at a time per checkout
<a href="#one-build-at-a-time-per-checkout"></a>

A generated `txt/ssimdb/` page once reached a commit with an `abt.config
builddir:…` line spliced into the middle of a C++ prototype, the rest of that
prototype and the comment after it gone.  Nothing generates that line but a
running `abt`, and nothing writes that page but a readme regeneration, so the
two ran at the same time in one checkout and one tool's output landed inside the
other's captured output.  The damage is silent: the regeneration reports
success, the corrupt page is committed, and the next regeneration is what
finally reports the file as modified.  Run one build or documentation tool at a
time in a checkout, and put concurrent work in its own worktree — see
[/txt/exe/wt/README.md](/txt/exe/wt/README.md).

### Component-test failure workflow
<a href="#component-test-failure-workflow"></a>

On failure, `atf_comp` prints a colored diff between captured
output (`temp/atf_comp/<name>`) and reference
(`test/atf_comp/<name>`).  Show the diff to the user and read it
yourself before declaring breakage.  Two shapes:

- **Real regression** — diff is consistent across reruns.  Fix the
  code.
- **Non-determinism** — diff varies between runs, or names
  volatile fields (timestamps, ports, PIDs, IDs).  Fix it by adding
  the field to `data/atfdb/unstableattr.ssim` (which masks the
  value with `***`) or by writing a `dmmeta.tfilt` rule — *not* by
  blessing the broken output with `-capture`.

Re-run 3–5 times when in doubt.  An unstable diff is
non-determinism, not a regression.

`-mode:print` does not run the test.  It prints the reference file
`test/atf_comp/<name>` as it stands on disk, so its output matches that
file by construction whatever the code does.  "A fresh `-mode:print`
capture matches the committed baseline" therefore says nothing about
whether the test passes, and it is not evidence that a failure was a
flake — the argument is circular, and it reads as a real check, which is
what makes it worth naming.  To ask whether the test passes, run it:
plain `atf_comp <ns>.<name>` exits non-zero and prints the diff.

The masking that `unstableattr` performs applies to **ssim tuple output
only** — a line is parsed as a tuple and matching attrs are replaced.  A
human table (what `ls -l` prints without `-ssim`) is not a tuple, so no
rule can mask a column of it, and a volatile value there has to be kept
out of the assertion instead.  Adding `-ssim` to the command is often
enough, since it turns the same data into a tuple the rules can reach.

An entry in `unstableattr` may be scoped to one tuple head
(`report.abt.time`) or apply to every head (`%.time`).  Prefer
the scoped form: the wildcard reaches every comptest in the tree, and
masking a field that some other baseline asserts deliberately turns that
assertion into `***` without anyone noticing.

### A test shortened for speed no longer covers what it was written for
<a href="#a-test-shortened-for-speed-no-longer-covers-what-it-was-written-for"></a>

A test runs for four minutes and the suite is slow, so it is narrowed: the
number of streams drops from sixteen to one, the cluster from three nodes to
one, the repetition count from five to one.  Wall time falls, the suite stays
green, and the diff reads as a pure speedup.

What left with the wall time is the breadth.  A publish test that fanned out
across three nodes exercised the path where two partitions accept writes at
once; the one-node version exercises a single partition, and the defect class
the test was built to catch can no longer occur in the shape it now runs.  The
test still passes, because it still asserts what it always asserted — over a
smaller set of cases.

The loss is invisible because a test reports only on what it ran.  Nothing in
the suite compares a test against the intent it was written with, so the
narrowed test and the original produce the same output: `npass` goes up by one
either way.  A green suite after the change is therefore not evidence that the
change was safe.  It is the same evidence the suite gave before, and it would
read identically if the test had been deleted outright.

So before shortening a test, name the failure it is supposed to turn red on,
and keep the dimension that carries that distinction.  Cut the dimensions that
only cost time — a warmup, a message count, a settle wait that was budgeted
generously — and leave the one the assertion depends on.  Where the expensive
dimension *is* the distinguishing one, split the test rather than shrink it:
a fast case for the common path, and a second that keeps the breadth under a
timeout budgeted for it.

Then confirm the shortened test can still fail.  Reintroduce the defect it was
written for, or an approximation of it, and watch the test go red before
keeping the change.  This is the rule AGENTS.md states as "a check that finds
nothing has not passed until you know it can fail", applied to a test that was
made cheaper rather than to a grep.

### Modifying `amc` itself
<a href="#modifying-amc-itself"></a>

Don't run `amc` in the live tree while editing `cpp/amc/...` — a
broken generator pollutes `cpp/gen/` and the next build can't
bootstrap.  Workflow:

1. Edit `cpp/amc/...`.
2. `wt amc -reset -- amc` — regen inside `wt/amc/`,
   a fresh copy of the live tree.
3. `wt amc -- ai` — compile inside the sandbox.
4. Read sandbox errors and tighten the generator until the sandbox
   build is clean.
5. Only then run `amc && ai` in the live tree.

### Rebasing onto a master that moved the toolchain
<a href="#rebasing-onto-a-master-that-moved-the-toolchain"></a>

A rebase updates the tree and leaves the built tools a version behind,
and the two failures that follow are each mistaken for something else.

The silent one comes from `amc`.  Some of `amc`'s inputs are also its
outputs: `data/dmmeta/ctypelen.ssim` records every ctype's computed
length, and `data/dmmeta/dispsig.ssim` records every dispatch's
signature.  A generator built before a new kind of row was declared does
not know to emit it, so running it *deletes* that row, along with the
generated line that loaded the signature at boot.  The run still exits
0, and `acr -check % -x` still passes, because a missing row is not an
inconsistency — it is a smaller database.  The only tell is the diff: a
generated row that disappears right after a rebase means the generator
is older than the schema it was asked to emit.  Rebuild the generator
and regenerate, and never accept the deletion as the new truth.

The loud one comes from `abt`.  `abt` scans sources for the headers they
include, and a master commit can add an include of a header that `abt`
itself writes into `build/`.  An `abt` predating that stamp cannot
resolve the include, so it fails on it and crashes —
`cpp/lib/algo/arg.cpp:36: abt can't find build/gitinfo.h`, then a
segfault.  That reads like a broken checkout and is a stale binary.

`ai` resolves both, and it handles the second case explicitly by
creating an empty `build/gitinfo.h` when none exists, so an `abt` from
before the stamp cannot fail on the include.  The order after a rebase
that moved the toolchain is therefore `ai`, then `amc`, then
`abt % -build -install`.

### TypeScript
<a href="#typescript"></a>

`abt_ts` is to `ts/` what `ai` and `abt` are to `cpp/`: the bootstrap
plus build step, run on demand rather than once per machine.

```bash
abt_ts -normalize           # pnpm install in each package, then typecheck
abt_ts -build               # implies -normalize, then the bundle build
abt_ts -clean               # drop node_modules and the pnpm caches
```

A fresh checkout has no `ts/*/node_modules`, and that is a missing build
rather than a broken environment — the same as an unbuilt `bin/` symlink.
Which packages exist and what scripts each offers is in its own
`package.json`; `ts/gen` holds the TypeScript amc emits.

Prettier: single quotes, 120-char width, 2-space indent, no
semicolons, trailing commas (es5).

### External build-time dependencies
<a href="#external-build-time-dependencies"></a>

- **mariadb** / **libmariadbd** — for `acr_my`, `ssim2mysql`,
  `mysql2ssim`.
- **libssl** / **libcrypto** — SHA1, TLS.
- **libcurl** — HTTP client.
- **liblz4** — compression.
- **cppcheck**, **valgrind** — static analysis, memory check (CI).

### The first `bin/normalize` in a fresh worktree
<a href="#the-first-bin-normalize-in-a-fresh-worktree"></a>
The first `bin/normalize` in a new worktree can stop at the `readme` gate
with `atf_ci.timeout  citest:readme`, while every gate before it reports
`success:Y`.  Running that one gate again immediately afterwards passes in
about four minutes.  Nothing in the branch changed between the two runs.

The `readme` gate re-generates the markdown under `txt/`, and the tutorial
files hold inline commands that are executed rather than quoted.  Those
commands create a target, generate its code, and compile and link it.  They
run inside the worktree's own copy-on-write `abt_md` sandbox, which `acr
sandbox` lists along with the other registered sandboxes.  A fresh
worktree's sandbox holds no object files, so the first run compiles the
whole dependency tree there before the tutorial produces its first output,
and that exceeds the gate's timeout budget — `acr citest:readme` shows it
as 600 seconds.  Every later run reuses the objects and stays well inside
the budget.

Two consequences follow.  The timeout is a property of the sandbox's state
rather than of the branch, so it happens on `master` as readily as on a
topic branch.  And a gate that exceeds its budget ends the job, which
leaves every gate after it unrun: the run exits 124 and its verdict is
incomplete, so the outcome cannot be read as one gate failing and the rest
passing.

The remedy is to run `bin/normalize` again.  To tell this apart from a gate
that is genuinely stuck, watch the sandbox's build directory while the gate
runs — a cold first run fills it steadily, and a stuck gate does not.

### The `apm_gen` gate and its diff
<a href="#the-apm_gen-gate-and-its-diff"></a>
The `apm_gen` gate regenerates `apm/gen/<package>.ssim` for every
`dev.pkggen` entry.  That file is a manifest of the package's entire ssim
schema — its ctypes, fields, messages, dispatch signatures, struct lengths
and command flags — so it restates, in one place, whatever the branch did to
the schema.  Any branch that adds a field, retires a message, or gives a
command a new flag therefore leaves it out of date, and the gate reports the
drift as a failure.

Do not read that diff.  It carries no information the branch's own ssim diff
does not already carry, and inspecting it is the same review twice.  Accept
it: run `apm -package:<pkg> -generate`, `git add` the result, and amend.

Do not re-run `bin/normalize` afterwards either.  The regeneration is
confined to `apm/gen/`, no other gate reads that directory, and the gates are
independent of one another, so the ones that already reported `success:Y` in
that run still hold.  A second full pass costs several minutes and can only
confirm what the first one established.

### The anchor under a heading is generated, not written
<a href="#the-anchor-under-a-heading-is-generated-not-written"></a>

The section above was added to this file by hand, heading first and then the
`<a href="…"></a>` line beneath it, copied from the pattern its neighbours use.
The anchor written that way read `#the-apm-gen-gate-and-its-diff`.  Running
`abt_md -check` reported no error and `bin/normalize` had already passed, so
the file looked finished; the CI `quickreadme` gate then failed on that one
line, because the anchor it computes is `#the-apm_gen-gate-and-its-diff`.  The
underscore inside `` `apm_gen` `` survives into the slug, and the guess that
every non-word character becomes a hyphen does not hold.

The general point is that the anchor line is generated output that happens to
live in the same file as the prose around it.  Writing one by hand is editing a
generated file, and it fails the way editing `cpp/gen/` by hand fails — the
generator wins at the next run, and the gate that runs the generator is where
the disagreement surfaces.

So write the heading and leave the anchor alone: `abt_md -evalcmd:N` inserts a
missing one and rewrites a wrong one, and it is exactly what `quickreadme`
runs, so a clean run of it locally is what clears that gate.  `abt_md -check`
is a different mode and answers a different question — it exits 0 on a wrong
anchor, which is what made the bad line look reviewed.

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
bin/normalize                            # main CI gate; bootstraps then
                                         # atf_ci -cijob:normalize
```

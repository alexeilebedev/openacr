## wt - Worktree manager - reset, run, diff, delete
<a href="#wt"></a>

The wt tool creates and manages named copies of the checkout under the
top-level `wt/` directory:

* A **sandbox** is registered in the `dev.sandbox` table and can be reset to
  a baseline that matches the current directory, including any currently
  modified files. Tools (`atf_ci`, `amc_gc`, `atf_fuzz`, `apm`, `acr_ed`,
  `abt_md`) reference sandboxes by name, so the names are checked-in data.
  The row's `cow` flag selects the sandbox's materialization: a git worktree
  (the default), or a copy-on-write farm — see
  [Copy-on-write sandboxes](#copy-on-write-sandboxes).
* A **branch worktree** (`-b`) is a durable checkout of its own branch, used
  for working on a task in isolation. It is registered only in git
  (`wt -list -b` shows all of them) and is never auto-reset.

Every operation executes its commands directly, in order; `-verbose` echoes
each command as it runs. The exit code of a command executed inside a
sandbox becomes wt's exit code.

### Syntax
<a href="#syntax"></a>
```usage
wt: Worktree manager - reset, run, diff, delete
Usage: wt [-name:]<regx> [[-cmd:]<string>] [options]
    OPTION       TYPE    DFLT    COMMENT
    -in          string  "data"  Input directory or filename, - for stdin
    [name]       regx            Sandbox name
    -create                      Create new sandbox and register in dev.sandbox
    -b                           Branch worktree: create on new branch NAME; git-registered, no dev.sandbox row
    -cow                         Sandbox is a copy-on-write farm (with -create/anon name)
    -list                        List existing sandboxes
    -reset                       Reset sandbox to match current directory
    -claudesess                  Start a background claude session named after the worktree
    -clean                       Remove sandbox contents to save space
    -shell                       Open interactive shell inside sandbox
    -del                         Permanently delete sandbox
    [cmd]...     string          Command to execute in sandbox
    -diff                        Show diff after running command
    -files...    string          Shell regx to diff
    -ref         string  "HEAD"  Reset to this ref
    -q                           Quiet mode
    -pull                        Pull changes from sandbox to main repo
    -verbose     flag            Verbosity level (0..255); alias -v; cumulative
    -debug       flag            Debug level (0..255); alias -d; cumulative
    -help                        Print help and exit; alias -h
    -version                     Print version and exit
    -signature                   Show signatures and exit; alias -sig
```

### Operation
<a href="#operation"></a>

With `-create`, a new entry is added to `dev.sandbox` table and this implies `-reset`.
`-reset` brings the sandbox in correspondence with the current directory:
the worktree is checked out (detached) at `-ref` (default `HEAD`), modified
files from the current directory are copied in, and the result is committed
as a `baseline` commit, so `git diff` inside the sandbox shows only what
changed since the reset. Files listed in the `dev.sbpath` table (the
`build/` directory, the build cache link `.gcache`) are also copied in, so
the sandbox starts with working executables.

Actions are performed in the following order: `-create`, `-reset`,
`-claudesess`, `-clean`, execute command in sandbox, `-diff`, `-del`. This
means you can create a sandbox, run a command inside it, show the
difference, and delete the sandbox in one line.

A single command argument is a shell command line and runs via `bash -c`;
multiple arguments form the argv of the command directly.

### Copy-on-write sandboxes
<a href="#copy-on-write-sandboxes"></a>

Some sandbox uses need no git history: `abt_md` evaluates the inline
commands of a tutorial in a sandbox, and the commands only need a directory
that looks exactly like the checkout and absorbs writes.  Materializing
such a sandbox as a git worktree costs a checkout of the whole tree plus a
copy of the build directory — over ten seconds per reset.  A sandbox whose
`dev.sandbox` row says `cow:Y` (or an unregistered name used with `-cow`)
is materialized instead as a *farm*: a hardlink copy of the current
directory that costs well under a second to reset.

The farm is built by `rsync --link-dest`: every file is a hardlink to the
corresponding file of the checkout, so the copy consumes directory entries
but no data blocks.  A hardlink alone does not isolate writes: opening the
shared inode for writing would modify the checkout's copy of the content.
A command therefore runs with `libcowdancer` (from the `cowdancer`
package) preloaded; the library copies a protected file before any
in-place write, breaking the hardlink.  The list of protected inodes,
`.ilist`, is computed at reset, since resets are the only operation that
creates hardlinks.  Files created by the command are ordinary files of the
farm and need no protection.  An overlayfs mount would give the same
isolation without preloading, but mounting one requires privileges that
the containers development happens in do not grant; the
hardlink-plus-preload scheme needs none.

On a host without the cowdancer package a `cow:Y` sandbox is materialized
as an ordinary git worktree: the same name, the same directory, the same
behavior for the caller, only a slower reset.  A full physical copy of the
checkout would also isolate writes, but copying every data block takes
over half a minute on a CI runner, while a worktree reset finishes in a
few seconds.  The `.ilist` file marks a hardlink farm, so wt refuses to
run a command in one when libcowdancer is missing (the preload is what
keeps such a farm safe); `-reset` then rebuilds the sandbox as a worktree.

Three parts of the checkout are not copied into a farm.  `wt/` is excluded
because it contains the farm itself, along with the other worktrees.
`.git` is excluded and replaced by a fresh empty repository, so a command
that stages files (for example `acr_ed -create -target`, which runs
`git add`) operates on a private index and can never touch the real repo.
`temp/` is recreated empty.  Because the farm's git repo has no baseline
commit, `-diff` and `-clean` behave differently: `-diff` is not supported,
and `-clean` removes the farm contents (the next use rebuilds it).

The protection mechanism is `LD_PRELOAD`, which sets its limits: only
dynamically linked programs that write through libc are intercepted.
Every tool in this repo qualifies, as do the compilers and linkers; a
static binary would bypass the protection and write through the hardlink.
The links are shared with the checkout, but only commands inside the farm
run with the preload: in practice writers on the checkout side replace
files rather than rewriting them in place — the repo's tools write a temp
file and rename it, and the build toolchain gives each output a new inode
— which breaks the link and leaves the farm holding the old version.  The
farm therefore goes stale as the checkout changes, never corrupt, and the
next `-reset` re-syncs it.

### Tools that use wt:
<a href="#tools-that-use-wt"></a>

* `amc_gc` uses a sandbox to test changes to process in-memory database composition, and to clean up
includes
* `acr_ed` uses a sandbox when invoked with `-sandbox` option
* `atf_fuzz` uses a sandbox to test crash behavior of corrupted inputs
* `atf_ci` uses a sandbox to run tests marked `sandbox:Y`
* `abt_md` evaluates readmes marked `sandbox:Y` inside the cow sandbox
`abt_md`: one reset per readme file, then one invocation per inline
command, so state accumulates within a tutorial and side effects die at
the next reset

### Example: create a new sandbox
<a href="#example-create-a-new-sandbox"></a>

```bash
wt test -create
```

This initializes a new sandbox. The sandbox directory is `wt/test`
and includes any files that are modified in the current directory,
committed with the comment 'baseline'. The sandbox can be reset to match
the current directory with `wt test -reset`, and brought back to the
baseline state with `wt test -clean`.

### Example: run command in sandbox
<a href="#example-run-command-in-sandbox"></a>

```bash
$ wt test "acr sandbox:test"
dev.sandbox  sandbox:test  comment:""
```

### Example: show difference from baseline
<a href="#example-show-difference-from-baseline"></a>

```bash
$ wt amc -reset "echo test >> cpp/wt/wt.cpp" -diff
wt.reset  sandbox:amc  dir:wt/amc
diff --git a/cpp/wt/wt.cpp b/cpp/wt/wt.cpp
index 0889b61..20d42ef 100644
--- a/cpp/wt/wt.cpp
+++ b/cpp/wt/wt.cpp
@@ -173,3 +173,4 @@ void wt::Main() {
         }ind_end;
    }
}
+test
```

### Example: list sandboxes and their current state
<a href="#example-list-sandboxes-and-their-current-state"></a>

```bash
$ wt % -list
Sandbox   Size  Clean  Path         Comment
acr_ed    656M  Y      wt/acr_ed    sandbox for testing acr_ed changes
amc       659M  Y      wt/amc       sandbox for running amc commands
amc_gc    664M  N      wt/amc_gc    sandbox for garbage collection tool
atf_ci    664M  N      wt/atf_ci    sandbox for CI testing
atf_fuzz  1.3G  Y      wt/atf_fuzz  sandbox for fuzzing
```

### Example: reset all sandboxes
<a href="#example-reset-all-sandboxes"></a>

```bash
$ wt % -reset
```

### Example: test amc changes inside sandbox
<a href="#example-test-amc-changes-inside-sandbox"></a>

This resets the sandbox to match current directory, runs amc in it, and rebuilds everything.
It can be used to test changes that might break compilation and put you in a dead-end situation.

```bash
$ wt amc -reset "amc && ai"
...
```

### Example: run interactive shell inside sandbox
<a href="#example-run-interactive-shell-inside-sandbox"></a>

```bash
$ wt amc -reset "bash -l"
```

### Example: run a command in a copy-on-write sandbox
<a href="#example-run-a-command-in-a-copy-on-write-sandbox"></a>

```bash
$ wt test1 -cow 'acr_ed -create -ssimfile dev.zz -write && acr ssimfile:dev.zz'
```

The schema edit happens in the farm; the checkout is unchanged.  A
subsequent `wt test1 ...` command sees the new ssimfile, because the farm
accumulates state until the next `-reset`.

### Example: create a branch worktree
<a href="#example-create-a-branch-worktree"></a>

```bash
$ wt 2047-my-task -create -b
wt.branch  branch:2047-my-task  dir:wt/2047-my-task
```

This creates `wt/2047-my-task` on new branch `2047-my-task` at `-ref`
(default `HEAD`), plants a `.branch` symlink to the shared branch-control
directory, gives the worktree local empty `build/<cfg>` directories with
`abt` and `gcache` seeded from the main checkout, and enables the shared
compiler cache, so the first build inside the worktree is cache-served.
No `dev.sandbox` row is written; git is the registry. `wt <name> -del -b`
removes the directory and prunes the registration, leaving the branch.

With `-claudesess`, wt additionally starts a background claude session
inside the worktree, named after it. The session boots by reading
`CLAUDE.md` and the branch control file `.branch/<name>.md`, then stands
by; it registers as a background agent, so `claude agents` lists it and
`claude --resume <name>` (or `claude attach`) opens it. The resolved
branch-control directory is passed with `--add-dir`, since the `.branch`
symlink points outside the worktree and the boot read would otherwise
stall on a permission prompt.

### Options
<a href="#options"></a>
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

#### -name -- Sandbox name
<a href="#-name"></a>

#### -create -- Create new sandbox and register in dev.sandbox
<a href="#-create"></a>

#### -b -- Branch worktree: create on new branch NAME; git-registered, no dev.sandbox row
<a href="#-b"></a>

#### -cow -- Sandbox is a copy-on-write farm (with -create/anon name)
<a href="#-cow"></a>

#### -list -- List existing sandboxes
<a href="#-list"></a>

#### -reset -- Reset sandbox to match current directory
<a href="#-reset"></a>

#### -claudesess -- Start a background claude session named after the worktree
<a href="#-claudesess"></a>

#### -clean -- Remove sandbox contents to save space
<a href="#-clean"></a>

#### -shell -- Open interactive shell inside sandbox
<a href="#-shell"></a>

#### -del -- Permanently delete sandbox
<a href="#-del"></a>

#### -cmd -- Command to execute in sandbox
<a href="#-cmd"></a>

#### -diff -- Show diff after running command
<a href="#-diff"></a>

#### -files -- Shell regx to diff
<a href="#-files"></a>

#### -ref -- Reset to this ref
<a href="#-ref"></a>

#### -q -- Quiet mode
<a href="#-q"></a>

#### -pull -- Pull changes from sandbox to main repo
<a href="#-pull"></a>

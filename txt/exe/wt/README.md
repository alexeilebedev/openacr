## wt - Worktree manager - reset, run, diff, delete
<a href="#wt"></a>

wt makes and manages named copies of the checkout under the top-level `wt/`
directory.  Use a sandbox to run a command against a throwaway copy of your
tree, and use a branch worktree to work on a task on its own branch.  wt runs
its commands one after another, and `-verbose` echoes each as it runs.

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
    -i                           Read the command script from stdin, one command per line
    [cmd]...     string          Command to execute in sandbox
    -diff                        Show diff after running command
    -files...    string          Shell regx to diff
    -ref         string  "HEAD"  Reset to this ref
    -q                           Quiet mode
    -pull                        Pull changes from sandbox to main repo
    -verbose     flag            Verbosity level (0..255); alias -v; cumulative
    -debug       flag            Debug level (0..255); alias -d; cumulative
    -trace       string  ""      Trace expression: category[:filter],...; also payload_lim:N, verbose, debug, timestamps
    -help                        Print help and exit; alias -h
    -version                     Print version and exit
    -signature                   Show signatures and exit; alias -sig
```

### Description
<a href="#description"></a>

wt knows two kinds of copy, and both live at `wt/<name>`.

A **sandbox** is a copy you can reset to your current tree at any time.  Its
name is a row of `dev.sandbox`, and tools such as `atf_ci`, `amc_gc`,
`atf_fuzz`, `apm`, `acr_ed` and `abt_md` find their sandboxes by those names.
A name that matches no row makes an anonymous sandbox, which behaves the same
and leaves no row behind.

A **branch worktree** (`-b`) is a durable checkout of its own branch.  git is
its only registry, and wt never resets it.

#### Resetting a sandbox to the current tree
<a href="#resetting-a-sandbox-to-the-current-tree"></a>

A reset checks the sandbox out, detached, at `-ref`.  It then copies in every
file you have modified and the paths listed in `dev.sbpath`, among them
`build/` and `.gcache`, so the sandbox starts with working executables.  The
result is committed as `baseline`, and `git diff` inside the sandbox shows what
changed since the reset.  A sandbox resets on `-reset`, on `-create`, and on
first use when its directory does not exist yet; any other command runs on the
state the last one left.

#### Running a command in a sandbox
<a href="#running-a-command-in-a-sandbox"></a>

Give one command argument and wt runs it as a shell line under `bash -c`.
Give several and they are the argv of the command.  The command's exit code
becomes wt's exit code.

wt performs the actions of one invocation in this order: `-create`, `-reset`,
`-claudesess`, `-clean`, the command, `-diff`, `-pull`, `-del`.  So one line
can make a sandbox, run a command in it, show the diff and delete it.

#### Copy-on-write sandboxes
<a href="#copy-on-write-sandboxes"></a>

A sandbox whose `dev.sandbox` row says `cow:Y`, or an anonymous one named with
`-cow`, is a *farm*: a hardlink copy of the current directory that resets in
well under a second.  A worktree reset takes over ten seconds.  `abt_md` uses
the `abt_md` farm to evaluate the inline commands of a readme marked
`sandbox:Y`.  It resets once per readme and runs each command in its own
invocation, so state accumulates within a tutorial and dies at the next reset.

A command in a farm runs with `libcowdancer`, from the `cowdancer` package,
preloaded.  The library copies a shared file before anything writes to it in
place, so the checkout's copy stays untouched.  Three parts of the tree are
left out of a farm.  `wt/` holds the farm itself, `.git` is replaced by an
empty repository of its own, and `temp/` starts empty.

On a host without `cowdancer`, a `cow:Y` sandbox is an ordinary worktree at
the same path, and only the reset is slower.

#### Branch worktrees
<a href="#branch-worktrees"></a>

`wt <name> -create -b` creates `wt/<name>` on a new branch `<name>`, starting
at `-ref`.  It plants a `.branch` symlink to the shared branch-control
directory when the checkout has one.  It gives the worktree its own empty
`build/<cfg>` directories, with `abt`, `gcache` and `llmtool` linked from the
checkout, and enables the shared compiler cache, so the first build is served
from the cache.  The `llmtool` link gives a session started in the worktree a
status line before anything is built there.  It copies the cppcheck build directories under `temp/`, so the first
`bin/normalize` there analyzes only what the branch changes.  And when the
checkout has an inventory attached under `run/`, the worktree gets a symlink
to the same dataset.

`wt <name> -del -b` removes the directory and prunes git's registration.  The
branch stays.

### Examples
<a href="#examples"></a>

```bash
wt % -list                              # every sandbox, its size and whether it is clean
wt % -list -b                           # every git worktree of this checkout
wt test -create                         # register sandbox test and reset it to this tree
wt test "acr sandbox:test"              # run a command in sandbox test
wt amc -reset "amc && ai"               # try amc and a full build on a copy of this tree
wt amc -reset -shell                    # a login shell in a fresh sandbox
wt % -reset                             # reset every sandbox
wt scratch -cow 'acr_ed -create -ssimfile dev.zz -write'  # a schema edit in an anonymous farm
wt 2047-my-task -create -b -ref:origin/master             # a branch worktree for a task
wt 2047-my-task -create -b -claudesess -ref:origin/master # the same, with a claude session in it
wt 2047-my-task -del -b                 # remove the worktree and keep the branch
```

The listing is tab-separated, and `wt 'abt_md|amc' -list` prints:

```
Sandbox  Cow  Size  Clean  Path       Comment
abt_md   Y    390M  N      wt/abt_md  Sandbox for compiling readmes
amc      N    N/A   Y      wt/amc     sandbox for running amc commands
```

`N/A` is a sandbox whose directory does not exist yet.

`-diff` shows what a command changed since the reset:

```bash
$ wt amc -reset "echo test >> cpp/wt/wt.cpp" -diff
wt.reset  sandbox:amc  dir:wt/amc
diff --git a/cpp/wt/wt.cpp b/cpp/wt/wt.cpp
...
+test
```

A farm keeps what a command wrote until its next reset, so a later `wt scratch
...` sees the ssimfile the example above created, and the checkout never does.

### Caveats
<a href="#caveats"></a>

- `-create` and `-del` on a sandbox write `data/dev/sandbox.ssim`, which is
  checked in.  For a throwaway copy, use a name no row has and skip `-create`.
- `-create -b` and `-claudesess` refuse to run inside a worktree, with `wt:
  refusing to nest a worktree inside a worktree`.  Run them from the main
  checkout.  Sandboxes work from anywhere.
- The branch starts at `-ref`, which defaults to `HEAD`.  In the main checkout
  that is often a local `master` behind `origin/master`, so pass
  `-ref:origin/master` for fresh work.
- When the branch named already exists, `-create -b` checks it out as it is
  and ignores `-ref`.
- `-clean` on a branch worktree discards its uncommitted changes.
- A farm goes stale as the checkout changes, and `-reset` brings it up to
  date.  A farm has no baseline commit, so `-diff` prints `wt.diff` and shows
  nothing.
- A farm is safe only under the preload.  A static binary run inside it would
  write through the hardlinks into the checkout.  wt refuses to run a command
  in a farm when `libcowdancer` is missing, with `wt.nocowdancer`, and
  `-reset` then rebuilds the sandbox as a worktree.
- wt reads stdin only under `-i`.

### Options
<a href="#options"></a>
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

The directory wt reads `dev.sandbox` and `dev.sbpath` from.  Leave it at `data`
unless you are testing wt against another data set.

#### -name -- Sandbox name
<a href="#-name"></a>

A regx over sandbox names, so `%` selects every sandbox and `'abt_md|amc'`
selects two.  A name that matches no `dev.sandbox` row makes an anonymous
sandbox of that name, and wt prints `wt.anon`.  With `-b` the name is also the
branch name.

#### -create -- Create new sandbox and register in dev.sandbox
<a href="#-create"></a>

Register a new sandbox in `dev.sandbox` and reset it, which writes
`data/dev/sandbox.ssim`.  With `-b` it creates a branch worktree and writes no
row.

#### -b -- Branch worktree: create on new branch NAME; git-registered, no dev.sandbox row
<a href="#-b"></a>

Work on a branch worktree.  With `-create` it makes `wt/<name>` on a new branch
`<name>`; with `-del` it removes the worktree and keeps the branch; with `-list`
it prints `git worktree list`.

#### -cow -- Sandbox is a copy-on-write farm (with -create/anon name)
<a href="#-cow"></a>

Make the sandbox a copy-on-write farm.  With `-create` it sets `cow:Y` on the
new row, and on an anonymous name it applies to that run.  A registered sandbox
takes its `cow` flag from its row.

#### -list -- List existing sandboxes
<a href="#-list"></a>

Print the selected sandboxes after every other action has run: name, cow flag,
size, whether `git diff` is empty, path and comment.  With `-b` it prints every
git worktree of the checkout, whatever the name.

#### -reset -- Reset sandbox to match current directory
<a href="#-reset"></a>

Reset the sandbox to match the current directory at `-ref`, discarding whatever
it held.  `-create` implies it, and a sandbox with no directory resets on first
use without it.

#### -claudesess -- Start a background claude session named after the worktree
<a href="#-claudesess"></a>

Start a background claude session in the worktree, named after it.  The session
reads `CLAUDE.md` and `.branch/<name>.md` and stands by, and `claude agents`
lists it.  Combine it with `-create -b` to hand a task to a helper session; it
refuses to run from inside a worktree.

#### -clean -- Remove sandbox contents to save space
<a href="#-clean"></a>

Return a worktree sandbox to its baseline by discarding every change since the
reset.  A farm has no baseline, so its directory is removed and the next use
rebuilds it.  On a branch worktree it discards uncommitted work.

#### -shell -- Open interactive shell inside sandbox
<a href="#-shell"></a>

Open an interactive login shell inside the sandbox in place of a command.  It
runs after any `-reset` given on the same line.

#### -del -- Permanently delete sandbox
<a href="#-del"></a>

Delete the sandbox directory and prune git's worktree list.  On a registered
sandbox it also removes the `dev.sandbox` row; with `-b` it keeps the branch.

#### -i -- Read the command script from stdin, one command per line
<a href="#-i"></a>

Read the command script from stdin, one command per line, and run it in the
sandbox as one `bash -c` script.  wt reads stdin under this flag alone.

#### -cmd -- Command to execute in sandbox
<a href="#-cmd"></a>

The command to run in the sandbox.  One argument is a shell line run by `bash
-c`, and several arguments are the argv of the command.  Its exit code becomes
wt's.

#### -diff -- Show diff after running command
<a href="#-diff"></a>

Print `git diff` of the sandbox after the command runs, which is every change
since the reset.  A farm has no baseline, so it prints `wt.diff` there and shows
nothing.

#### -files -- Shell regx to diff
<a href="#-files"></a>

wt accepts this flag and does nothing with it today, so `-diff` always covers
the whole sandbox.  To narrow a diff, run `git diff -- <paths>` as the command.

#### -ref -- Reset to this ref
<a href="#-ref"></a>

The commit a reset checks the sandbox out at, and the commit a new branch
worktree starts from.  Any git ref works, and `-ref:origin/master` is the usual
one for fresh work.

#### -q -- Quiet mode
<a href="#-q"></a>

Suppress wt's own progress lines, `wt.reset`, `wt.branch`, `wt.claudesess` and
`wt.anon`.  The command's output is unaffected.

#### -pull -- Pull changes from sandbox to main repo
<a href="#-pull"></a>

Run `git pull` of the sandbox's `HEAD` into the current checkout after the
command.  Use it to bring back commits a command made inside the sandbox.

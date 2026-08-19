## amc Reftype: Exec
<a href="#amc-reftype-exec"></a>

`Exec` is a **subprocess controller**.  Declaring a field as
`Exec` of some `command.X` ctype gives you a struct ready to
fork/exec/wait/kill that subprocess, with full control of its
stdin/stdout/stderr, timeout, exit status, and command-line
construction.  Every amc-generated executable gets a
`command.X_proc` companion ctype with an Exec field, so any
program can drive another binary as a child with two lines of
C++.

```
dmmeta.field  field:acr_ed.FDb.abt  arg:command.abt  reftype:Exec  dflt:""  comment:""
```

The field name is the prefix of every generated function;
`arg:` is the command struct that the subprocess understands
(typically auto-generated for every amc executable).

### What it generates
<a href="#what-it-generates"></a>

Pre-amc phase `amcdb.gen:newfield_exec` adds a set of companion
fields to the parent struct:

| Field           | Type             | Meaning                              |
|-----------------|------------------|--------------------------------------|
| `<name>_path`   | `algo::cstring`  | path to the executable, default `"bin/<cmdname>"` |
| `<name>_cmd`    | `command::X`     | the typed command line               |
| `<name>_fstdin` | `algo::cstring`  | shell-style stdin redirect (`<file`, `<&fd`, `"|"`, …) |
| `<name>_fstdout`| `algo::cstring`  | stdout redirect (`>file`, `>&fd`, `"|"`, …) |
| `<name>_fstderr`| `algo::cstring`  | stderr redirect (`>file`, `>&fd`, `"|"`; use `">&1"` to merge into the stdout pipe) |
| `<name>_to_stdin`   | `algo::Fildes` | write end of the stdin pipe (valid when `_fstdin=="|"`) |
| `<name>_from_stdout`| `algo::Fildes` | read end of the stdout pipe (valid when `_fstdout=="|"`) |
| `<name>_from_stderr`| `algo::Fildes` | read end of the stderr pipe (valid when `_fstderr=="|"`) |
| `<name>_pid`    | `pid_t`          | child PID; 0 when not running        |
| `<name>_timeout`| `i32`            | seconds before `alarm` fires (0 = no timeout) |
| `<name>_status` | `i32`            | last `wait` status                   |

With `anonfld:Y` on the field, the storage fields are flattened
into the parent without the `<name>_` prefix — used when a
ctype is *itself* a subprocess wrapper (e.g., `command.X_proc`).

### Ssim inputs
<a href="#ssim-inputs"></a>

Required:

- `dmmeta.field` with `reftype:Exec`.  `arg:` must be a `command.X`
  ctype that has a `cfmt:Argv` with `print:Y` — otherwise amc
  refuses with `amc.noargv`.

That is the entire requirement.  The companion `_path`, `_pid`,
`_status` etc. fields and the eight tfunc functions come for
free.

### Generated functions
<a href="#generated-functions"></a>

Source: `cpp/amc/exec.cpp`.

| Tfunc              | Generated function                                  | Effect |
|--------------------|-----------------------------------------------------|--------|
| `Exec.Init`        | (none)                                              | Trivial — the companion fields default to their own defaults. |
| `Exec.Uninit`      | (folded into parent uninit)                         | Calls `<name>_Kill` so a partially-destroyed parent doesn't leak a zombie. |
| `Exec.Start`       | `int <name>_Start(P&)`                              | If `_pid==0`, create any pipes (for redirects set to `"|"`), fork, redirect stdin/stdout/stderr, set alarm, exec.  Calls `DieWithParent` in the child.  Returns 0 on success or `errno` on fork failure. |
| `Exec.Wait`        | `void <name>_Wait(P&)`                              | Block in `waitpid(_pid)`, restarting on EINTR.  Writes the result to `_status`, clears `_pid`, and closes the pipe fds (`_to_stdin`, `_from_stdout`, `_from_stderr`). |
| `Exec.Kill`        | `void <name>_Kill(P&)`                              | `kill(_pid, SIGKILL)` then `_Wait`.  No-op if not running. |
| `Exec.Exec`        | `int <name>_Exec(P&)`                               | `_Start` + `_Wait`; return `_status`. |
| `Exec.ExecX`       | `void <name>_ExecX(P&)`                             | `_Exec`; throw a human-readable `algo_lib.exec` error if the exit code is non-zero (uses `DescribeWaitStatus` for the comment). |
| `Exec.ToCmdline`   | `algo::tempstr <name>_ToCmdline(P&)`                | Print the full command line as a shell-quoted string (used for logging). |
| `Exec.ToArgv`      | `void <name>_ToArgv(P&, StringAry& args)`           | Build the `argv` array in `args` (skipping defaulted fields).  Used internally by `Execv`. |
| `Exec.Execv`       | `int <name>_Execv(P&)`                              | Construct argv via `ToArgv` and call `execv`.  Runs only in the child. |

### How it works
<a href="#how-it-works"></a>

`Start` does the unix dance:

```
fork() -> 0:                       // child
  algo_lib::DieWithParent();       // prctl(PR_SET_PDEATHSIG) on linux
  if (timeout > 0) alarm(timeout);
  apply_redirect(fstdin , 0);
  apply_redirect(fstdout, 1);
  apply_redirect(fstderr, 2);
  Execv(parent);                   // build argv from cmd struct + execv
  _exit(127);                      // if execv returns, failed
fork() -> pid:                     // parent
  _pid = pid;
```

The redirect syntax in `_fstdin/_fstdout/_fstderr` follows the
shell:

- `"file"` — same as `>file` for stdout, `<file` for stdin
- `">file"` — overwrite
- `">>file"` — append
- `">&fd"` — duplicate from fd

`DieWithParent` ensures the child is reaped if the parent
crashes — no run-away processes.

For amc-generated commands (target has `ccmdline`), `ToArgv`
uses the compact `-name:value` form.  For plain CLI commands
without a `ccmdline`, it falls back to the two-token
`-name value` form.

### Pitfalls
<a href="#pitfalls"></a>

- **The wrapped ctype needs `cfmt:Argv`.**  amc errors out
  with `amc.noargv` if there is no Argv printer for the
  command type.
- **`Uninit` kills the child.**  Letting a `_proc` struct go
  out of scope while the subprocess is running terminates the
  subprocess with SIGKILL.  This is usually what you want
  (no leaks), but if you intend to fire-and-forget, decouple
  the lifetimes (e.g., via `posix_spawn` directly).
- **Defaults are stripped from argv.**  `ToArgv` only emits
  flags whose value differs from the field default.  If you
  set a value equal to the default, it will not appear on
  the wire.  Defaults inherited by the target are
  responsible for the same default.
- **Pipe redirects.**  Set a redirect to `"|"` and `_Start`
  creates a pipe and exposes the parent end on the struct:
  `_fstdin="|"` → write to `_to_stdin`; `_fstdout="|"` → read
  from `_from_stdout`; `_fstderr="|"` → read from `_from_stderr`.
  To merge stderr into the stdout pipe, set `_fstdout="|"` and
  `_fstderr=">&1"`.  The fds are closed by `_Wait` (and thus by
  `_Kill`/destruction), so no manual cleanup is needed.  Drain
  the read ends before `_Wait` and close `_to_stdin` (or call
  `_Wait`) to send EOF — pipes have finite buffers and can
  deadlock otherwise.  Unix only (`dospawn` on WIN32 ignores
  `"|"`).
- **`Wait` blocks.**  Read from `_from_stdout` in a loop, or use
  an async I/O loop, if you need to do other work while the
  child runs.
- **`Execv` uses `bin/<cmdname>`.**  `_path` defaults to
  `bin/<cmdname>` — adjust before `_Start` if you need a
  different binary.
- **Stdin defaults to inheriting.**  An unset `_fstdin` leaves
  the child's stdin attached to the parent's; redirect
  explicitly to `"</dev/null"` if you want it closed.

### See also
<a href="#see-also"></a>

- [Reftypes index](/txt/exe/amc/reftype.md)
- [Subprocess invocation (feature page)](/txt/exe/amc/exec.md)
- [Command-line parsing](/txt/exe/amc/cmdline.md) — the `cfmt:Argv` requirement
- Source: `cpp/amc/exec.cpp`
- Tfunc records: `acr 'tfunc:Exec.%'`

### Example
<a href="#example"></a>

Driving the `abt` build tool from a parent process:

```c++
command::abt_proc abt;
abt.cmd.target = "atf_unit";
abt.cmd.build  = true;
abt_ExecX(abt);                    // throws on non-zero exit
```

Capturing the output of a subprocess line-by-line:

```c++
command::abt_proc abt;
abt.cmd.target = "%";
abt.cmd.list   = true;
abt.fstdout    = "|";              // capture stdout through a pipe
abt_Start(abt);
ind_beg(algo::FileLine_curs, line, abt.from_stdout) {
    out1 << line << eol;
} ind_end;
abt_Wait(abt);
```

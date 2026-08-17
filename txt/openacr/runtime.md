## Runtime Patterns
<a href="#runtime-patterns"></a>

How OpenACR-generated processes are expected to behave at runtime —
step functions, the natural-exit model, and subprocess invocation.

### Step functions (fstep)
<a href="#step-functions-fstep-"></a>

In `ns::<field>_Step()`, the field's list / heap is guaranteed
non-empty.  `zd_*` and `bh_*` first-element accesses don't need a
NULL check.

Avoid hot-polling: if a `_Step()` condition will never resolve for
an element, remove that element from the list so the list drains.
Add a one-line comment explaining why the removal is safe — the
heuristic isn't always obvious.

**An `InlineRecur` step with no delay stops the main loop from ever
sleeping.**  `MainLoop` repeats while `next_loop < limit`, and the
generated `Call` for a recurring step pins `next_loop` to
`clock + <field>_delay`.  A delay left at its zero default therefore
pins `next_loop` to the current clock on every pass, the loop's
condition never goes false, and the process spins at a full core.
Nothing reports it: the program is correct, only hot.  So a recurring
step sets its cadence explicitly with `<field>_SetDelay(...)` — and
the enable flag defaults to `false` when the step has nothing to do
until some setup runs, since the pin happens whenever the flag is on,
whether the step body does any work or not.

The cost lands on whoever links the code, which makes this sharper in
a **library** than in an executable.  A spinning executable wastes one
core; the same step moved into a library, enabled by default, spins
every client that links it.  What surfaced was not a CPU report but an unrelated test
failing — a retention test whose publisher could no longer keep up
with its own budget, reported as "too little was published".  A test
that starts failing only when run alongside others, and passes alone,
is the signature.

### Natural exit model
<a href="#natural-exit-model"></a>

A *server* process — one whose purpose is to stay up servicing
peers, not to run a fixed task and exit — must exit only by
*natural exit*: `algo_lib::MainLoop` ends when the next scheduling
cycle is at infinity, i.e. there are no FIohooks registered, no
non-empty fstep lists, and no scheduled timehooks.  Do not call
`algo_lib::ReqExitMainLoop()` to force shutdown; it papers over a
state bug.

To shut down a server cleanly, drain the state that's keeping
MainLoop alive:

- **Inbound shm** — mark `c_shmhdr->eof = true` (writer death) or
  `cd_poll_read_Remove(shm)` (peer signaled end-of-stream, e.g.
  `ams.TerminateMsg`).  `cd_poll_read_Step` drains remaining
  messages and drops the shm.
- **Outbound shm** — write-only shms aren't polled; they don't
  keep MainLoop alive — no action needed.
- **Signaled mode** — `lib_ams::SetSignaledMode(true)` arms a
  signalfd and registers it as an FIohook, so that a peer's
  SIGRTMIN wakes the loop.  That hook stays registered for as long
  as the process stays in signaled mode, and a registered FIohook
  is one of the three things natural exit requires the absence of.
  A process in signaled mode therefore has no natural exit at all:
  it runs until `algo_lib::_db.limit`, whatever else it has
  drained.  Leaving signaled mode removes the hook, and
  `lib_ams::Uninit` does that as its first act — but `Uninit` runs
  after MainLoop, so a server that must exit on its own has to
  call `lib_ams::SetSignaledMode(false)` as part of the drain.
- **Stdin (`fdin`)** — `fdin_RemoveAll()` on EOF.  If a stdio-mode
  loopback shm is in `cd_poll_read`, drop it too
  (`lib_ams::_db.c_loopback_shm`).
- **Forked children** — a recurring `algo_lib::FTimehook` (e.g.
  1 s) calls `waitpid(-1, ..., WNOHANG)` in a loop, calls
  `lib_ams::ProcExit(pid, status)` per reap; when waitpid returns
  `< 0` (ECHILD), `bh_timehook_Remove(th)` so the bheap empties.
  SIGCHLD handler does only `algo_lib::ThScheduleIn(th, 0)` to wake
  the loop.  **Install the SIGCHLD handler and schedule the
  timehook *before* forking** — a fast-exiting child can race the
  default ignore-handler.  `lib_ams::ProcExit` cascades: it marks
  shms tied to the dead proc as eof so `cd_poll_read` drains them.
- **FCmd Fbuf (e.g. ams_bridge pipe mode)** — in-fbuf EOF triggers
  `cd_cmd_eof_Step`; remove the FCmd and any shm reads that no
  longer have a producer.

`ReqExit` is acceptable in fixed-task CLI tools where the work is
bounded and exit is the normal terminus.  It is not acceptable in
servers; if you find yourself reaching for it, find the source
that's keeping MainLoop alive and drain it.

### Subprocess invocation
<a href="#subprocess-invocation"></a>

To invoke another amc-generated exe, use its
`command::<target>_proc` struct and `<target>_ExecX`:

```cpp
command::acr_proc acr_cmd;
acr_cmd.cmd.query   = tempstr() << "amsdb.proctype:" << proctype;
acr_cmd.cmd.print   = true;
acr_ExecX(acr_cmd);   // throws on non-zero exit
```

- `_Exec` returns the exit code.
- `_ExecX` throws `algo_lib::ErrorX` on non-zero exit.
- Setting `_fstdout="|"` makes `_Start` create a pipe and expose
  the read end as `_from_stdout` for line-by-line stdout reads
  (closed by `_Wait`).  `apm::CreateMergeFiles` in `cpp/apm/update.cpp`
  reads a child's output that way with a `FileLine_curs`.

### State lives on `FDb`
<a href="#state-lives-on-fdb-"></a>

Never use file-scope `static` mutable state.  Add a Val field to
`<ns>.FDb` and access it as `_db.foo`.  See the
[`codestyle` skill](/.agents/skills/codestyle/SKILL.md) for the full
rule and the `report.<name>` pattern.

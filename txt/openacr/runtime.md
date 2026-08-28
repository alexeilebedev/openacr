## Runtime Patterns
<a href="#runtime-patterns"></a>

Every OpenACR executable is one thread running one loop.  There are no locks
and no background threads, because there is nothing to lock against: all state
lives in the namespace's in-memory database, and only the loop touches it.

### Steps
<a href="#steps"></a>

The loop's body is a fixed sequence of *steps*.  A step is a C++ function bound
by a `dmmeta.fstep` record to a list, a heap or a timer, and the loop calls it
when that collection has something in it or that timer is due.  So control flow
is the step order rather than the call order, and a program of this shape is
not read top to bottom.

Two properties follow, and they are what make the shape worth using.  A step
returns after a bounded quantum of work, so every other step runs at a rate
that can be reasoned about.  And because the loop never waits on anyone, real
time is packed densely, which is where the throughput comes from.

A step therefore never blocks, and a step that finds it cannot act right now
does not spin: it takes the element off its collection and arranges to be armed
again when the condition clears.  What arms it has to be named, because a wait
with nothing to end it is invisible -- the program builds, passes its tests,
and simply stops moving that data.

### Natural exit
<a href="#natural-exit"></a>

A server exits when it has nothing left to do, and by no other route.
`algo_lib::MainLoop` ends when the next scheduling cycle is at infinity: no
I/O hook registered, no non-empty step collection, no scheduled timer.  Shutting
one down is therefore a matter of draining whatever is holding the loop open,
rather than of asking it to stop.

Reaching for `algo_lib::ReqExitMainLoop()` in a server papers over a state bug,
and the bug is that something is still registered which should not be.
[/txt/rule/openacr.md](/txt/rule/openacr.md) lists what can hold the loop open
and how each is drained.  A fixed-task CLI tool is a different case: its work
is bounded and exiting is the normal terminus.

### State lives on FDb
<a href="#state-lives-on-fdb"></a>

Each namespace has one singleton, `FDb`, and every pool, index and global value
hangs off it.  A file-scope `static` would do the same job invisibly, and that
is the objection: state on `FDb` is a field with a `dmmeta.field` row, so `acr`
can print it, `amc` can generate its reader and printer, and an xref can reach
it.  A hidden global takes part in none of that.

Values that belong together get their own ctype and one `FDb` field of that
type, which is how a report or a parse buffer is held.

### Calling another tool
<a href="#calling-another-tool"></a>

Every generated executable comes with a `command::<target>_proc` struct that
starts it, waits for it and kills it.  Filling in the fields and calling
`<target>_ExecX` is how one tool runs another, and it beats assembling a
command line because the arguments are typed and the quoting is the generator's
problem rather than yours.

```cpp
command::acr_proc acr;
acr.cmd.query = "dmmeta.ns:acr%";
acr.cmd.print = true;
acr_ExecX(acr);              // throws algo_lib::ErrorX on a non-zero exit
```

`_Exec` returns the exit code instead of throwing, and setting `_fstdout="|"`
before `_Start` exposes the child's stdout as a readable stream.

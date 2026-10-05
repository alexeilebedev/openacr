## amc Runtime
<a href="#amc-runtime"></a>

amc shapes RAM with pools, records, and indexes; the runtime
chapter is about the *other* dimension of a program — time.  Here
amc has less to say than it does about layout: the body of any
work-doing function is hand-written C++.  What amc *does* manage
is the structure that calls those functions, the units that count
as "doing work," and the smooth handoff between hot-polling and
blocking on the OS.

A namespace's runtime surface is a small fixed cast:

- **`main()`** — entry point.  amc emits it.  Parses argv into
  `command.<ns>`, initializes algo_lib, loads finputs, and
  invokes the user-implemented `<ns>::Main`.
- **`<ns>::Main()`** — user-implemented function.  Typically calls
  `<ns>::MainLoop()`.
- **`<ns>::MainLoop()`** — amc-emitted scheduling loop.  Repeats
  one `Steps()` call after another until `next_loop ≥ limit`.
- **`<ns>::Steps()`** — amc-emitted sequence of step calls, emitted
  for an executable only.  It calls every step of every namespace
  the executable links, the executable's own first and then each
  library in `dev.targdep` order.  A library emits no step function
  of its own.
- **`<ns>::<field>_Step()`** — *user-implemented* work function,
  one per `fstep` record.

Everything below is what amc emits between those layers and how
the **scheduler clock** (`algo_lib::_db.next_loop`) lets the same
mechanism drive a 200ns hot loop and a long-blocking idle process.

### Table Of Contents
<a href="#table-of-contents"></a>
&nbsp;&nbsp;&bull;&nbsp;  [The main loop and `next_loop`](#the-main-loop-and-next_loop-)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Smoothly merging blocking and hot-polling](#smoothly-merging-blocking-and-hot-polling)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Defining a step](#defining-a-step)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [The control field](#the-control-field)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Step types](#step-types)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inline](#inline)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [InlineOnce — deadline scheduling](#inlineonce-deadline-scheduling)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [InlineRecur — fixed-period polling](#inlinerecur-fixed-period-polling)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Scaled steps — spread the load](#scaled-steps-spread-the-load)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [TimeHookRecur / TimeHookOnce](#timehookrecur-timehookonce)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Callback and Extern](#callback-and-extern)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [`_FirstChanged` — index-state notification](#-_firstchanged-index-state-notification)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Steps as collaborative threads](#steps-as-collaborative-threads)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Program startup](#program-startup)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Pitfalls](#pitfalls)<br/>

### The main loop and `next_loop`
<a href="#the-main-loop-and-next_loop-"></a>

amc emits `MainLoop` from `tfunc_Global_MainLoop`
(`cpp/amc/global.cpp`):

```c++
void <ns>::MainLoop() {
    algo::SchedTime time(algo::get_cycles());
    algo_lib::_db.clock = time;
    do {
        algo_lib::_db.next_loop.value = algo_lib::_db.limit;  // = ∞
        <ns>::Steps();
    } while (algo_lib::_db.next_loop < algo_lib::_db.limit);
}
```

Three `algo_lib`-level fields drive the loop:

| Field                   | Meaning                                                                 |
|-------------------------|-------------------------------------------------------------------------|
| `algo_lib::_db.clock`   | Most recent `rdtsc` capture (refreshed by `_UpdateCycles`).             |
| `algo_lib::_db.next_loop` | "Earliest time some step wants CPU again."  Reset to `limit` each iteration. |
| `algo_lib::_db.limit`   | Loop-exit sentinel.  Default `0x7fff…ffff` (effectively infinity).      |

Each scheduling cycle starts by **raising `next_loop` to
infinity**.  Then `Steps()` runs through every step's `_Call`
wrapper.  Each wrapper **lowers** `next_loop` according to its
own scheduling needs — to `clock` for hot work, to `clock+delay`
for periodic work, to a future deadline for time-bounded work.
At the bottom of the cycle, any time between `clock` and
`next_loop` is **slack** — time that no step needs.

If a step explicitly sets `next_loop = limit` (or the user code
calls `algo_lib::ExitMainLoop()` which does the same), the
`while` condition fails and `MainLoop` returns.

### Smoothly merging blocking and hot-polling
<a href="#smoothly-merging-blocking-and-hot-polling"></a>

That slack is the key: the same code drives a microsecond-level
tight loop *and* a daemon that blocks on a socket for an hour.
The mechanism is `algo_lib::giveup_time_Step` in
`cpp/lib/algo/iohook.cpp`:

```c++
void algo_lib::giveup_time_Step() {
    u64 limit       = algo_lib::_db.limit;
    u64 slack       = u64_SubClip(u64_Min(_db.next_loop, limit), _db.clock);
    u64 wait_clocks = _db.last_sleep_clocks == 0 ? 0 : slack;
    if (_db.n_iohook > 0) {
        _IohookWaitClocks(wait_clocks);   // epoll_wait or kevent
        _db.next_loop = _db.clock;        // we slept, so come back through
    } else if (_db.next_loop < limit) {
        SleepClocks(wait_clocks);         // nanosleep
    }
    _db.last_sleep_clocks = slack;
}
```

The shape:

- If any step lowered `next_loop` to `clock` (i.e., "I have work
  *right now*"), `slack = 0` and `wait_clocks = 0` — `epoll_wait`
  returns immediately, the loop spins.  This is the **hot path**.
- If every step is content to wait `delay` cycles, `next_loop =
  clock + delay`, `slack = delay`, and the OS gets that long.
  `epoll_wait(fd, …, delay_ms)` blocks until either an fd
  becomes ready *or* the delay elapses — whichever fires first.
- With **no I/O hooks** but a pending periodic step, fall through
  to `nanosleep` of the same duration.
- **Two-cycle anti-deadlock rule**: `wait_clocks = 0` whenever
  `last_sleep_clocks == 0`.  After any sleep, the next pass is
  guaranteed to be a no-sleep pass.  This means one step that
  enqueues work for *another* step occurring later in the same
  cycle never deadlocks: even if `next_loop` stayed high, the
  next iteration runs through everything before sleeping again.

The same binary thus seamlessly:

| Workload                                   | Behavior                              |
|--------------------------------------------|---------------------------------------|
| All steps hot (`next_loop = clock`)        | Spin tight; no syscall overhead.      |
| Some periodic, some hot                    | Spin while hot work pending, then sleep until next periodic deadline. |
| All quiet, fds open                        | Block in `epoll_wait` up to next deadline. |
| All quiet, no fds                          | `nanosleep` to next deadline.         |

You don't pick blocking vs polling; you describe the work, and
the scheduler picks based on what each step asked for *this
cycle*.

### Defining a step
<a href="#defining-a-step"></a>

A step is a `dmmeta.fstep` row attached to a **global** field —
that is, a field on `<ns>.FDb`.  Each step generates exactly one
extern user-implemented function `<field>_Step()`.

```ssim
dmmeta.ctype  ctype:atf_amc.FDb
  dmmeta.field  field:atf_amc.FDb.cd_in_msg  arg:atf_amc.Msgbuf  reftype:Llist
    dmmeta.fstep  fstep:atf_amc.FDb.cd_in_msg  steptype:Inline
```bash

amc emits:

```c++
// User implements:
void atf_amc::cd_in_msg_Step();

// amc-generated dispatch wrapper:
inline static void atf_amc::cd_in_msg_Call() {
    if (!atf_amc::cd_in_msg_EmptyQ()) {           // control field non-empty
        atf_amc::cd_in_msg_Step();                // user code
        cd_in_msg_UpdateCycles();                 // refresh clock + ftrace
        algo_lib::_db.next_loop = algo_lib::_db.clock;  // "more work, no sleep"
    }
}

// amc-generated Steps() invocation list:
void atf_amc::Steps() {
    // ...
    atf_amc::cd_in_msg_Call(); // fstep:atf_amc.FDb.cd_in_msg  stepband:work
    // ...
    algo_lib::bh_timehook_Call(); // fstep:algo_lib.FDb.bh_timehook  stepband:work
    algo_lib::bh_timehook_idle_Call(); // fstep:algo_lib.FDb.bh_timehook_idle  stepband:idle
    algo_lib::giveup_time_Call(); // fstep:algo_lib.FDb.giveup_time  stepband:yield
}
```

The `_Call` wrapper enforces three universal contracts: the
non-emptiness predicate (so user code never runs against an
empty queue), the cycle-count update (for `ftrace`), and the
`next_loop` adjustment (so the scheduler responds appropriately).

### The control field
<a href="#the-control-field"></a>

The field carrying the `fstep` is the **control field**, and its
"non-emptiness" controls whether the step body runs.  amc picks
the right predicate based on the reftype (`GetStepCond` in
`cpp/amc/step.cpp`):

| Field type                                 | Predicate                              |
|--------------------------------------------|----------------------------------------|
| `Llist`, `Atree`, `Bheap`, `Tary`, `Inlary`, `Ptrary`, `Thash` | `!<name>_EmptyQ()` |
| `Val` (typically `bool`), `Ptr`, `Upptr`   | `<ns>::_db.<name>` (zero-test)         |
| `ZSListMT`                                 | `<name>_DestructiveFirst() != NULL`    |
| `Global`                                   | `true` (always run)                    |

**This is why a `bool` on FDb works as a step trigger.**
There's no special "trigger" reftype — a `reftype:Val arg:bool`
field with an `fstep` row gets the zero-test predicate and
becomes a flag you set when you want the step to fire and clear
when you're done:

```ssim
dmmeta.field  field:net.FDb.do_compact  arg:bool  reftype:Val
  dmmeta.fstep  fstep:net.FDb.do_compact  steptype:Inline
```

```c++
void net::do_compact_Step() {
    PerformCompaction();
    net::_db.do_compact = false;   // clear the flag so the step stops firing
}
```

Combined with the wrapper's automatic `next_loop = clock` adjust,
this gives one-shot work units that fire on demand and idle the
process cleanly after.

### Step types
<a href="#step-types"></a>

The `dmmeta.steptype` table lists every supported variant:

| `steptype`     | Comment                                                                 |
|----------------|-------------------------------------------------------------------------|
| `Inline`       | Inline; call step func every cycle.                                     |
| `InlineOnce`   | Inline; call step func at given time (first element of Bheap).          |
| `InlineRecur`  | Inline; call step func with given period (`fdelay`).                    |
| `TimeHookOnce` | Routed through `bh_timehook` Bheap; call at given time.                 |
| `TimeHookRecur`| Routed through `bh_timehook` Bheap; call with given period.             |
| `Callback`     | Inline; call step func always; does not update `next_loop`.             |
| `Extern`       | User provides scheduling by implementing `_FirstChanged`.               |

Direct steps (`Inline*`, `Callback`, `Extern`) inline into
`Steps()` and take a handful of instructions each.  TimeHook
steps live behind a generic Bheap (`algo_lib::_db.bh_timehook`),
which adds O(log N) scheduling cost but doesn't burn cycles on
every loop.

### Inline
<a href="#inline"></a>

The simplest variant.  Run the user function whenever the
control field is non-empty.

```c++
if (!<name>_EmptyQ()) {
    <ns>::<name>_Step();
    <name>_UpdateCycles();
    algo_lib::_db.next_loop = algo_lib::_db.clock;   // hot path
}
```

`next_loop = clock` keeps the scheduler hot for as long as the
index has work.  The step is expected to drain *some* work each
call — typically one item — and let the loop spin.  When the
index empties, the predicate becomes false, no `next_loop`
update happens, and the scheduler can finally sleep.

### InlineOnce — deadline scheduling
<a href="#inlineonce-deadline-scheduling"></a>

`InlineOnce` requires the control field to be a Bheap; one of the
record's fields (designated by `dmmeta.sortfld`) is treated as a
**deadline**.  Each cycle, amc-generated code walks the heap and
fires the step for every expired record:

```c++
algo_lib::_db.step_limit = algo_lib::_db.clock;
while (FRow *row = <name>_First()) {            // top of heap
    algo::SchedTime expire = row-><sortfld>;
    if (expire < algo_lib::_db.step_limit) {
        <name>_Step();                          // user code consumes row
        <name>_UpdateCycles();
        algo_lib::_db.next_loop = step_limit;   // there may be more, come back
    } else {
        algo_lib::_db.next_loop = u64_Min(expire, _db.next_loop);  // sleep until then
        break;
    }
}
```

This *is* deadline scheduling.  The first element of the heap is
the earliest deadline (`Bheap` is min-heap by `sortfld`), so:
- Expired records fire in deadline order.
- If the head record's deadline is in the future, `next_loop`
  drops to that deadline and the scheduler sleeps exactly that
  long.

The user is responsible for **removing the processed record from
the heap** — failing to do so makes the same record fire next
cycle and the next, hot-spinning forever.

`step_limit` (frozen at loop entry) gives a stable "this cycle"
horizon: even if `clock` advances during the loop, only entries
that were already expired *when the cycle started* run.  This
prevents starving other steps when an InlineOnce step contains
many tightly-clustered deadlines.

### InlineRecur — fixed-period polling
<a href="#inlinerecur-fixed-period-polling"></a>

`InlineRecur` runs every `_delay` cycles regardless of when work
arrived.  amc adds two variables to FDb:

| Variable            | Meaning                                  |
|---------------------|------------------------------------------|
| `<name>_delay`      | Inter-call delay, in `algo::SchedTime`.  |
| `<name>_next`       | Time of next scheduled invocation.       |

The wrapper:

```c++
if (<LoopCond>) {                                       // control non-empty
    if (<ns>::_db.<name>_next < algo_lib::_db.clock) {
        <ns>::_db.<name>_next = _db.clock + _db.<name>_delay;
        <ns>::<name>_Step();
        <name>_UpdateCycles();
    }
    algo_lib::_db.next_loop = u64_Min(<name>_next, _db.next_loop);
}
```

The step thus fires every `_delay` cycles, and in between
`next_loop` is lowered to `<name>_next` so the scheduler sleeps
exactly long enough.

The delay is set either statically via `dmmeta.fdelay.delay`
(loaded in the generated `_Init`) or dynamically via
`<name>_SetDelay(algo::SchedTime delay)` — amc emits this setter
(`cpp/amc/step.cpp`) and it shifts `_next` accordingly so a
larger delay doesn't fire too soon and a smaller delay doesn't
have to wait out the old larger one.

### Scaled steps — spread the load
<a href="#scaled-steps-spread-the-load"></a>

`dmmeta.fdelay.scale:Y` changes the *interpretation* of `delay`
from "every N cycles" to **"go through the whole list in N
cycles, at uniform intervals."**  amc divides the delay by the
current list size on every call:

```c++
u64 effective_delay = <ns>::_db.<name>_delay / u64_Max(1, <name>_N());
<ns>::_db.<name>_next = _db.clock + algo::SchedTime(effective_delay);
```

So if `<name>_delay = 1s` and the list has 100 entries, the step
fires every 10ms — once per cycle around the ring you visit every
record.  If the list grows to 1000 entries, it fires every 1ms.
This is the canonical pattern for **heartbeats**: send one
heartbeat per second total, distributed evenly across all
connected peers regardless of count.

The combination of a circular Llist + `cd_mylist_RotateFirst()`
inside the step + `scale:Y` gives you "visit every record once
per second" in a few lines:

```c++
dmmeta.field  field:gate.FDb.cd_session  arg:gate.FSession  reftype:Llist
  dmmeta.fstep  fstep:gate.FDb.cd_session  steptype:InlineRecur
    dmmeta.fdelay  fstep:gate.FDb.cd_session  delay:1.0  scale:Y
```

```c++
void gate::cd_session_Step() {
    gate::FSession *s = cd_session_First();
    SendHeartbeat(*s);
    cd_session_RotateFirst();   // ring advances by one
}
```

Note `scale:Y` is rejected on `TimeHookRecur` (only `InlineRecur`
supports it; `cpp/amc/step.cpp`).

### TimeHookRecur / TimeHookOnce
<a href="#timehookrecur-timehookonce"></a>

The TimeHook variants take the same semantics as `InlineRecur` /
`InlineOnce` but route the callback through a shared Bheap of
time hooks (`algo_lib::_db.bh_timehook`).  amc declares
`<parent>.th_<name>` (an `algo_lib::FTimehook` value) and emits
the binding code in `_Init`:

```c++
hook_Set0(<parent>.th_<name>, <ns>::<name>_Call);
ThInitRecur(<parent>.th_<name>, algo::SchedTime());
```

The trade is:

- TimeHook steps cost nothing in the main loop when idle —
  they're scheduled via the timehook heap, which only the
  scheduler's heap-top check touches each cycle.
- TimeHook adds ~tens of cycles of overhead per fire (heap
  reposition) and a small amount of scheduling jitter relative
  to Inline variants.

Use Inline for tight pipelines where determinism matters; use
TimeHook for large numbers of slow periodic tasks.

### Callback and Extern
<a href="#callback-and-extern"></a>

Two auxiliary variants for cases the standard model doesn't
cover.

- **`Callback`**: the user function runs every cycle when the
  control field is non-empty, but `next_loop` is *not* updated.
  Used when the step itself is responsible for `_UpdateCycles`
  bookkeeping (e.g., it integrates an external event source
  whose timing isn't tied to the index state) or doesn't want to
  prevent the scheduler from sleeping.
- **`Extern`**: the user implements not just `_Step` but also
  `_FirstChanged` (the index-state notification, see next
  section).  amc emits no scheduling logic — the user wires it
  themselves.  Use for unusual fields (e.g. a field whose state
  depends on an external counter).

### A step on another namespace's list
<a href="#a-step-on-another-namespace-s-list"></a>

A library can put rows on a list that an executable drains, and
an executable can replace a step a library declares.  Declare an
`Alias` field on the executable's `FDb` that names the library's
list, and put the `fstep` on the alias:

```
dmmeta.field   field:<exe>.FDb.cs_<list>  arg:<lib>.F<Row>  reftype:Alias
  dmmeta.falias  field:<exe>.FDb.cs_<list>  srcfield:<lib>.FDb.cs_<list>
dmmeta.fstep   fstep:<exe>.FDb.cs_<list>  steptype:Inline
```

The alias step tests the library's list for pending work and
calls `<exe>::cs_<list>_Step()`, which takes rows off the
list with the library's own accessors.  The alias has no storage
and no accessors of its own.

When the list has an `fstep` of its own, the alias step overrides
it.  `Steps()` calls the alias step in the slot the library's
step held, and never calls the library's.  The overriding step
may call `lib_x2::cs_eof_changed_Step()` itself when it wants the
library's behavior too.  A process whose executable declares no
alias step runs the library's step.  When the list has no step of
its own, the alias step is an ordinary step of the namespace that
declares it.

amc refuses four shapes: a source list that is not a global of
another namespace; a steptype other than `Inline`, `InlineRecur`
or `Callback`, since a time hook is armed by the list's
`_FirstChanged`, which lives in the list's own namespace; an
override of a library step that runs from a time hook, which
never enters `Steps()`; and two alias steps on one list inside
one executable's closure, which would race for its rows.

### Step bands — the order of the pass
<a href="#step-bands-the-order-of-the-pass"></a>

Take a process whose shm read step sits in a library that comes last
in `dev.targdep` order.  A message it reads reaches the module's list
in this pass, and the module's step already ran, so the reaction waits
for the next pass.  Every hop along a chain of steps can cost a pass
that way.

Each `fstep` names a band, a row of `amcdb.stepband`, and `Steps()`
runs the bands in rank order: `input` (reads from fds, shm rings and
the fabric), `work` (the default), `output`
(outflow of conns, fabric interfaces and shm rings), `idle`, and
`yield`, which holds only `algo_lib`'s `giveup_time`.  Within a band
the order is the dependency order of the namespaces, the executable's
own first, and each namespace's steps in declaration order.  So a read
in any library runs before the module's work in the same pass, and the
bytes that work wrote leave the process at the end of it.  A step that
composes a message, a heartbeat or a serve, is work: it writes into a
buffer, and the output band flushes that buffer in the same pass.

Background steps whose latency no reader waits on sit in `idle`:
metric and alarm publishing, volume readings, and the retries of an archive
upload or fetch.  An idle step that drains a queue empties it in one call, so its
rate governs how often the queue is visited and not how fast it moves.

```
dmmeta.fstep  fstep:lib_ams.FDb.cd_poll_read  steptype:Inline  stepband:input
```

The order is a latency hint and nothing more.  Each step is a
transaction that leaves the database consistent, so no step may rely
on another having run earlier in the same pass.

### Step rates — a fractional for-loop
<a href="#step-rates-a-fractional-for-loop"></a>

A step that loops over its list inside one call trades fairness for a
warm cache: it runs up to N entries while it has them.  Now take a
step that should run *less* than once a pass, a background task that
must not take a share of a busy loop.  The same for-loop with a limit
below one would run once for every several calls.

A `dmmeta.fsteprate` row gives an `Inline` step that rate: up to
`ncall` calls every `npass` passes.  Each pass earns the step `ncall`
credit, and each call spends `npass`.  The credit is capped at
`ncall+npass-1`, which is the most a pass can hold after spending what it
could, so a step with work never loses credit it earned, and an empty
stretch banks less than one call.  With no row the rate is one call per
pass, or one call in eight passes for a step in the `idle` band.  `3/2`
calls the step three times over two passes, `2/3` twice over three, and
`1/10` once in ten.

```
dmmeta.fsteprate  fstep:yyy.FDb.zd_bg  ncall:1  npass:10
```

A step in the `idle` band also runs once in any pass that earned it no
call, when that pass has time to give up (`next_loop` stands past the
clock at the step's slot).  So an idle step at `1/10` runs at once when
the process has nothing else to do, and once every ten passes when it
is busy, and an idle step with no row does the same at `1/8`.  A rate applies to `Inline` steps only; the other steptypes
keep their own clocks, and in the idle band they run late in the pass.

A `TimeHookOnce` or `TimeHookRecur` step never appears in `Steps()`: it
fires from a heap of time hooks in `algo_lib`.  There is one heap per
band a hook may fire in.  `bh_timehook` holds the hooks of `work`
steps and runs in the work band, and `bh_timehook_idle` holds the hooks
of `idle` steps and runs after the output band.  A step's band picks the
heap its hook is armed on, and amc refuses a time hook step in any
other band.  So a metric poll or a retry backoff that comes due fires
after the pass has flushed its work.

```
dmmeta.fstep  fstep:lib_ams.FDb.bh_metric_poll  steptype:TimeHookOnce  stepband:idle
```

### `_FirstChanged` — index-state notification
<a href="#-_firstchanged-index-state-notification"></a>

When the **first element of an indexed control field changes**
(an item is inserted at the front, or the previous front is
removed), amc may need to react: the schedule the step is keyed
to has shifted.  amc emits `<name>_FirstChanged` on demand (see
`tfunc_Step_FirstChanged` in `cpp/amc/step.cpp`) — but only
for step types that need it:

| `steptype`     | What `_FirstChanged` does                                                 |
|----------------|--------------------------------------------------------------------------|
| `TimeHookRecur`| If index empty → deschedule timehook; else → `bh_timehook_Reheap`.       |
| `TimeHookOnce` | If index empty → deschedule; else → update timehook time to new head's deadline, reheap. |
| `Inline`       | No-op (the inline loop will discover the change next cycle).             |
| `Extern`       | User-provided.  amc emits only the extern prototype.                     |

The index reftype's `Insert`/`Remove`/`Reheap` operations are
generated to call `_FirstChanged` whenever they observe a
head-change, automatically.  This is invisible to the user — they
just modify the index and the timehook schedule follows.

For example, after inserting a record with an earlier deadline
into a `TimeHookOnce`-controlled Bheap:

```c++
gate::pq_msg_Insert(msg);    // record with deadline T
// generated code inside Insert calls pq_msg_FirstChanged
//   which updates th_pq_msg.time = T and reheaps bh_timehook
```

The next pass through the scheduler discovers the timehook is
now scheduled for `T`, sleeps until then, and fires.

### Steps as collaborative threads
<a href="#steps-as-collaborative-threads"></a>

The step model is functionally a **cooperative threading
discipline** — each step is a thread that runs to completion
before the next one starts.  This buys you a property C++ has to
fight for elsewhere:

> Between any two lines inside a step function, **no other step
> runs, no other thread mutates state, no callback fires**.

Concretely:

- **No locks.**  Two steps that share a queue don't need a mutex
  on it; they can't run simultaneously.  A producer step puts
  items on a list, a consumer step takes them off, both touch
  the same head pointer without a `std::mutex` in sight.
- **Read-modify-write is automatic.**  `++counter;` is atomic in
  every meaningful sense because nothing else gets the CPU
  between the read and the write.
- **Invariants hold across step boundaries, not within.**  Step
  bodies may briefly violate ctype invariants (an item in
  flight, half-populated), as long as they're restored before
  return.

The cost: a step body that blocks (waits on a lock, calls
`sleep(2)`, does a slow syscall) stops *every* step in the
namespace.  The discipline is to break long work into many
short step invocations — each returning quickly with state
parked on a list — so blocking is impossible.

Where you do need actual threads (e.g., offloading CPU-heavy
work), the canonical pattern is one writer thread pushing onto
a [ZSListMT](/txt/exe/amc/reftype/ZSListMT.md) (a lock-free
single-producer/multi-consumer linked list), and a step on the
main thread draining it via `DestructiveFirst`.  That's the only
place locks intrude on the step model, and it's a single CAS per
insert.

### Program startup
<a href="#program-startup"></a>

`main()` is generated by amc (see `tfunc_Global_Main` in
`cpp/amc/global.cpp`) and does, in order:

1. Save `argc`/`argv` to `algo_lib::_db.argc` / `argv`.
2. Initialize all linked namespaces (`<ns>::Init` for each — the
   one that does pool defaults, reflection registration, etc.).
3. Parse argv into `command.<ns>` per the `dmmeta.ccmdline`
   record.
4. Auto-load every `dmmeta.finput` table for the namespace
   (`floadtuples:Y` controls; usually defaulted on).
5. Call user-implemented `<ns>::Main`.
6. On exception, log `<ns>.error` and set
   `algo_lib::_db.exit_code = 1`.

The user's `Main` is then free to be as minimal as
`MainLoop();` or do its own work and exit.  Sub-namespaces
linked via `dmmeta.main.ismodule:Y` get their `MainLoop` called
implicitly from the parent's generated main as well.

### Pitfalls
<a href="#pitfalls"></a>

- **Steps must drain or clear.**  Inline + non-empty control
  field + step that doesn't make progress = hot-spin forever.
  Always either pop something off the index or clear the flag.
- **Bool steps clear themselves.**  A `Val arg:bool` step that
  doesn't clear the flag in the step body fires every cycle.
- **InlineOnce demands a Bheap.**  Other indexed reftypes don't
  support deadline semantics — amc rejects the combination.
- **`scale:Y` only with `InlineRecur`.**  Scaled delay needs the
  per-call divisor, which TimeHook steps don't see.
- **Two-cycle anti-deadlock relies on `next_loop`.**  Don't set
  `algo_lib::_db.last_sleep_clocks` directly from user code.
- **Inline steps starve TimeHook steps if they never yield.**
  An Inline step that always reports work pending keeps
  `next_loop = clock`, so the giveup_time step never gets to
  fire the timehook heap.  This is by design (Inline = hot path
  takes priority), but be aware.
- **Bool field with no fstep is fine, just unused.**  The
  scheduler ignores it; only an `fstep` row makes the field
  participate.
- **Steps in module namespaces fire through the parent's
  `MainLoop`.**  `dmmeta.main.ismodule:Y` chains the calls;
  modules don't have their own loop.

### See also
<a href="#see-also"></a>

- [Reflection](/txt/exe/amc/reflection.md) — Imdb's `Step` and
  `MainLoop` function pointers expose this machinery
- [Trace counters](/txt/exe/amc/trace.md) — `ftrace` on a step
  field adds `step_<name>` + `step_<name>_cycles`
- [Dispatches](/txt/exe/amc/dispatch.md) — common payload for
  Inline message-processing steps
- [Reftypes / Bheap](/txt/exe/amc/reftype/Bheap.md) — the
  required control type for InlineOnce
- [Reftypes / ZSListMT](/txt/exe/amc/reftype/ZSListMT.md) — the
  cross-thread escape hatch
- Source: `cpp/amc/step.cpp`, `cpp/amc/global.cpp` (MainLoop /
  Steps generation), `cpp/lib/algo/iohook.cpp`
  (`giveup_time_Step`).
- Spec tables: `acr 'dmmeta.fstep:%'`, `acr 'dmmeta.fdelay:%'`,
  `acr 'dmmeta.steptype:%'`.

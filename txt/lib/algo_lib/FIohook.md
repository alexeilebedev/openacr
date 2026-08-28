## algo_lib.FIohook -
<a href="#algo_lib-fiohook"></a>

`Iohook` is the single mechanism by which an amc-generated
process learns that a file descriptor is ready for I/O.  Each
hook represents one fd plus a callback.  The runtime adds the
fd to a kernel event queue (`epoll` on Linux, `kqueue` on
FreeBSD/macOS) and, on each scheduling cycle, asks the kernel
which fds are ready — and only those.

The same machinery lets a process **drive a tight engine
loop** (no fds, no syscalls per iteration) and **block politely
on idle** (sleep in `epoll_wait` until either an fd is ready
or the next periodic step is due).  See
[Runtime](/txt/exe/amc/runtime.md#smoothly-merging-blocking-and-hot-polling)
for the scheduler-side picture; this page is the iohook side.

### Table Of Contents
<a href="#table-of-contents"></a>
&nbsp;&nbsp;&bull;&nbsp;  [The `FIohook` record](#the-fiohook-record)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [API: Add, Remove, Init](#api-add-remove-init)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [The callback model](#the-callback-model)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Edge-triggered semantics](#edge-triggered-semantics)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Lifecycle and fd ownership](#lifecycle-and-fd-ownership)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Cross-platform behavior](#cross-platform-behavior)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example: line-at-a-time reader](#example-line-at-a-time-reader)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Interaction with steps and giveup_time](#interaction-with-steps-and-giveup_time)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Common consumers](#common-consumers)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Pitfalls](#pitfalls)<br/>

### The `FIohook` record
<a href="#the-fiohook-record"></a>

`algo_lib::FIohook` is a small ctype, ~32 bytes:

| Field        | Type              | Meaning                                              |
|--------------|-------------------|------------------------------------------------------|
| `callback`   | `Hook`            | User-supplied callback fired on event readiness.     |
| `fildes`     | `algo::Fildes`    | The file descriptor.  Closed at destruction unless `nodelete:Y`. |
| `evt_flags`  | `algo::IOEvtFlags`| What the fd is subscribed to: `read`, `write`.       |
| `flags`      | `algo::IOEvtFlags`| What fired *this* call: `read`, `write`, `eof`, `err`. |
| `in_epoll`   | `bool`            | Currently registered with epoll/kqueue.              |
| `nodelete`   | `bool`            | Don't `close(fd)` when the FIohook is destroyed.     |

Embed an `FIohook` field in any ctype that owns an fd
(`reftype:Val arg:algo_lib.FIohook`).  Once you call
`IohookAdd`, the scheduler will route events back to your
callback automatically — there is no per-cycle polling code to
write.

```ssim
dmmeta.field  field:gate.FGwport.iohook  arg:algo_lib.FIohook  reftype:Val
```

### API: Add, Remove, Init
<a href="#api-add-remove-init"></a>

Three functions in `algo_lib`:

```c++
// Initialize the epoll/kqueue fd (once, at startup).
// amc calls this for you from algo_lib::Init.
void algo_lib::IohookInit();

// Subscribe to read/write events on iohook.fildes.
// OK to call twice with different flags — the second call replaces the first.
void algo_lib::IohookAdd(algo_lib::FIohook& iohook, algo::IOEvtFlags inflags);

// Stop watching this fd.  Safe to call when not registered.
void algo_lib::IohookRemove(algo_lib::FIohook& iohook);
```

`IOEvtFlags` is a small bitfield with `read`, `write`, `eof`,
`err`.  To subscribe to read events:

```c++
algo::IOEvtFlags flags;
read_Set(flags, true);
algo_lib::IohookAdd(_db.read, flags);
```

`IohookAdd` is **edge-triggered**: subsequent fires only happen
when the fd state *changes* (e.g., readable→not-readable→
readable).  This is required for correctness with shared event
queues — see [below](#edge-triggered-semantics).

### The callback model
<a href="#the-callback-model"></a>

The `callback` field is a [Hook](/txt/exe/amc/reftype/Hook.md):
a typed function-pointer plus a context.  Bind it before
calling `IohookAdd`:

```c++
static void OnReadable(algo_lib::FIohook& h) {
    // h.flags tells us what fired:
    //   read_Get(h.flags)  — fd is readable
    //   write_Get(h.flags) — fd is writable
    //   eof_Get(h.flags)   — peer closed (EPOLLHUP / EV_EOF)
    //   err_Get(h.flags)   — error (EPOLLERR)
    char buf[4096];
    int rc;
    do {
        rc = read(h.fildes.value, buf, sizeof buf);
        if (rc > 0) {
            ProcessBytes(buf, rc);
        }
    } while (rc > 0);
    if (rc == 0 || (rc < 0 && errno != EAGAIN)) {
        IohookRemove(h);   // EOF or fatal error
    }
}

// elsewhere:
callback_Set0(_db.read, OnReadable);   // or callback_Set1 for context binding
```

The callback runs **synchronously inside `giveup_time_Step`**:
the scheduler's epoll/kqueue wait returns N events, and the
runtime invokes one callback per event before the next step
runs.  Callbacks are therefore subject to the same cooperative
rules as steps — no blocking, no locks needed, finite work per
call, leave the data on a Llist/Bheap so a step picks it up.

### Edge-triggered semantics
<a href="#edge-triggered-semantics"></a>

On Linux, `IohookAdd` registers with `EPOLLET` (edge-triggered);
on BSD, `kqueue` is used with `EV_CLEAR` (the equivalent).  This
matters:

- **You must drain the fd until `EAGAIN`** (or `EWOULDBLOCK`)
  inside the callback.  A single `read()` of one byte leaves
  data unread, and the kernel will not fire again until the fd
  goes from "had data" → "no data" → "has data" — which may
  never happen if the peer sends one big burst.
- **Set the fd non-blocking** before adding it.  Use
  `algo::SetBlockingMode(fd, false)`.  Without this, the drain
  loop will hang on the first incomplete read.

Edge-triggered mode is the right choice for this scheduler: it
avoids spurious wakeups when one cycle didn't fully consume the
data, and matches the "callback fires when state *transitions*"
model of the rest of amc's runtime.

### Lifecycle and fd ownership
<a href="#lifecycle-and-fd-ownership"></a>

By default, `FIohook` **owns** the file descriptor.  The
`fcleanup` on `fildes` arranges for it to be closed when the
FIohook is destroyed (e.g., when its pool row is `Delete`'d).
Set `nodelete:Y` when the fd is shared (a `dup`'d pipe, a
duplicated socket, etc.):

```c++
_db.shared_input.fildes = fd;
_db.shared_input.nodelete = true;   // somebody else closes fd
```

Calling `IohookRemove` does **not** close the fd — it only
deregisters from epoll/kqueue.  Calling it twice in a row is
safe (the second call sees `in_epoll == false` and returns).

`IohookAdd` updates an existing registration if the iohook is
already in epoll — change the subscribed flags by calling it
again.

### Cross-platform behavior
<a href="#cross-platform-behavior"></a>

Iohook abstracts the platform's event mechanism (see
`cpp/lib/algo/iohook.cpp`):

| Platform           | Mechanism            | Notes                                            |
|--------------------|----------------------|--------------------------------------------------|
| Linux              | `epoll_pwait2`/`epoll_wait` | `epoll_pwait2` used when available (sub-ms sleep precision). |
| FreeBSD / macOS    | `kqueue` + `kevent`  | Edge-clear (`EV_CLEAR`) gives the same semantics as `EPOLLET`. |
| Cygwin             | Immediate callback   | No kernel event queue; `IohookAdd` invokes the callback directly. |
| Windows            | Immediate callback   | Same as Cygwin; full event support is a TODO.    |

On Linux, a regular file (not a pipe/socket/eventfd) cannot be
added to epoll — the kernel returns `EPERM`.  `IohookAdd`
detects this and **fires the callback immediately**, treating
the fd as always-ready.  This lets the same code work for
sockets *and* files transparently.

### Interaction with steps and giveup_time
<a href="#interaction-with-steps-and-giveup_time"></a>

At the bottom of each scheduling cycle, `algo_lib::giveup_time_Step`
decides how to spend the slack:

- **`_db.n_iohook > 0`**: call into `_IohookWaitClocks` →
  `epoll_pwait2` / `kevent` with a timeout equal to the slack
  computed from `next_loop - clock`.  Returns when the timeout
  expires *or* when any iohook fires.  Each fired iohook's
  `callback` runs before control returns to `MainLoop`.
- **`_db.n_iohook == 0`** and `next_loop < limit`: plain
  `nanosleep` for the same duration.
- **`_db.n_iohook == 0`** and `next_loop == limit`: no sleep,
  loop exits.

`IohookAdd` increments `_db.n_iohook`; `IohookRemove` decrements
it.  So the runtime tracks "is there anything to wait for?" with
a single counter — no list walks per cycle.

Inside the kernel-wait branch, `giveup_time_Step` resets
`next_loop = clock` after the wait completes — guaranteeing one
more pass through `Steps()` (with a fresh clock value) before
the loop can exit.  This is the same two-cycle anti-deadlock
rule [described in Runtime](/txt/exe/amc/runtime.md#smoothly-merging-blocking-and-hot-polling).

There's also a small heuristic to avoid calling `epoll` on every
iteration when steps are very cheap: with `wait_clocks == 0`,
`giveup_count` cycles between skipping and calling epoll on
alternating iterations.  This matters for kernel-bypass
workloads (vma_lib) where the epoll call itself costs ~650 cycles
versus ~200 cycles per step.

### Common consumers
<a href="#common-consumers"></a>

Iohook is the lowest level; most code uses it indirectly:

- **[Fbuf](/txt/exe/amc/fbuf.md)**: a message-oriented byte
  buffer with `BeginRead`/`BeginWrite` that internally creates
  and manages an iohook.  Use this for protocol traffic — you
  almost never want to read bytes one at a time yourself.
- **`lib_ams::signal_iohook`**: routes Unix signal delivery
  through a `signalfd`, so signals reach the program through
  the same scheduler path as everything else.
- **`lib_rl::iohook`**: GNU readline integration — the
  terminal fd is registered, and readline characters drive an
  interactive prompt without blocking other steps.
- **`<ns>::FGwport.iohook`**: network connections in a
  gateway.

If your fd carries length-delimited messages, reach for `Fbuf`.
If it carries something stranger (a signalfd, a timerfd, an
eventfd, a control pipe), use Iohook directly.

### Pitfalls
<a href="#pitfalls"></a>

- **Not draining = silent stall.**  Edge-triggered.  Always loop
  until `read`/`write` returns `EAGAIN` or `EWOULDBLOCK`.
- **Forgetting `SetBlockingMode(fd, false)`.**  Blocking reads
  in the drain loop hang the whole process.
- **Calling expensive work in the callback.**  The callback runs
  inside `giveup_time_Step` — every cycle it blocks delays every
  other step.  Push parsed records onto a Llist, do the work in
  a step.
- **Forgetting `IohookRemove` on EOF.**  An EOF'd fd keeps
  triggering events.  Remove the hook so `n_iohook` drops and
  the process can idle.
- **Closing the fd twice.**  If you `close(fd)` manually before
  destroying the iohook, the iohook's destructor closes it
  again unless `nodelete:Y` is set.  Either set `nodelete:Y` *or*
  let the iohook own the close.
- **One fd in two iohooks.**  Don't.  epoll's per-fd state is
  shared; both will fire and the bookkeeping breaks.  If you
  need two views of the same fd, `dup` it and set `nodelete:Y`
  on the duplicate.
- **`flags` is read-only inside the callback.**  Don't mutate
  `flags` — the runtime sets it from the kernel event each
  call.  Mutate `evt_flags` (via a fresh `IohookAdd`) to change
  what you're subscribed to.

### See also
<a href="#see-also"></a>

- [Runtime / giveup_time](/txt/exe/amc/runtime.md#smoothly-merging-blocking-and-hot-polling)
- [Fbuf — buffered framed I/O](/txt/exe/amc/fbuf.md)
- [Hook reftype](/txt/exe/amc/reftype/Hook.md) — the callback field
- Source: `cpp/lib/algo/iohook.cpp`
- Schema: `acr ctype:algo_lib.FIohook`,
  `acr ctype:algo.IOEvtFlags`

### Example: line-at-a-time reader
<a href="#example-line-at-a-time-reader"></a>

Read characters from stdin, print each with a timestamp, exit on
EOF:

```c++
dmmeta.field  field:sample.FDb.read  arg:algo_lib.FIohook  reftype:Val
```

```c++
static void DoRead(algo_lib::FIohook& h) {
    char buf[1];
    int rc;
    do {
        rc = read(h.fildes.value, buf, 1);
        if (rc == 1) {
            tempstr out;
            char_PrintCppSingleQuote(buf[0], out);
            prlog(CurrUnTime() << " " << out);
        } else if (rc == -1 && errno == EAGAIN) {
            // edge-triggered: drained for now
        } else {
            IohookRemove(h);   // EOF (rc==0) or fatal error
        }
    } while (rc > 0);
}

void sample::Main() {
    _db.read.fildes = algo::Fildes(0);          // stdin
    algo::SetBlockingMode(_db.read.fildes, false);
    callback_Set0(_db.read, DoRead);
    algo::IOEvtFlags flags;
    read_Set(flags, true);
    algo_lib::IohookAdd(_db.read, flags);
    sample::MainLoop();
}
```

Running it:

```bash
$ sample
hello
2025-04-12T16:26:52.95788 'h'
2025-04-12T16:26:52.95792 'e'
2025-04-12T16:26:52.95794 'l'
2025-04-12T16:26:52.95796 'l'
2025-04-12T16:26:52.95798 'o'
2025-04-12T16:26:52.95800 '\n'
^D
$
```

The drain-loop inside `DoRead` handles edge-triggered mode; the
`IohookRemove` on EOF makes `n_iohook` drop to zero and the next
`giveup_time` returns straight away, so `MainLoop` exits.

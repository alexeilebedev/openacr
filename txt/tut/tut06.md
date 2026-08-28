## Tutorial 6: Fbuf — buffered framed I/O
<a href="#tutorial-6-fbuf-buffered-framed-i-o"></a>

`Fbuf` is the protocol-aware byte buffer used everywhere in
OpenACR that a process reads or writes length-delimited
messages.  It sits on top of [Iohook](/txt/lib/algo_lib/FIohook.md):
Iohook hands the runtime a "the fd is ready" event; Fbuf turns
that into a fully-framed, statically-typed message that arrives
at a step.

This tutorial walks through the Fbuf pattern.  Read
[Tutorial 3](/txt/tut/tut03.md) first for the main loop and
steps, and the [Iohook reference](/txt/lib/algo_lib/FIohook.md)
for the lower-level fd-event API.

### When to reach for Fbuf
<a href="#when-to-reach-for-fbuf"></a>

Use Fbuf when **all four** of these hold:

- The fd carries **length-delimited messages** (a TLV header,
  Kafka frames, FIX, your own wire format).
- You want incoming messages to be **typed** — landing in an
  inbox table as fully-constructed C++ records, ready for a
  dispatch.
- You want the buffer to integrate with the main loop **without
  per-cycle polling code**.
- The fd is non-blocking (sockets, pipes, eventfds).

For raw byte streams that aren't framed (a TTY, a log tail), use
Iohook directly — see [iohook.md](/txt/lib/algo_lib/FIohook.md).

### The Fbuf field
<a href="#the-fbuf-field"></a>

An Fbuf is a `reftype:Fbuf` field on a ctype, with a
`dmmeta.fbuf` row that describes its direction (`in`/`out`),
the header ctype, and the index that receives parsed messages.

```ssim
dmmeta.field  field:gate.FConn.in_buf  arg:gate.MsgHdr  reftype:Fbuf
  dmmeta.fbuf   field:gate.FConn.in_buf  fbufdir:in
  dmmeta.fcond  fcond:gate.FConn.in_buf/ready  ins:gate.FDb.cd_in_msg  rem:Y
```

The `fcond` row binds the buffer's `ready` condition to an index:
every complete message
that arrives in `in_buf` should be allocated as a row in
`cd_in_msg` for a step to pick up.  The step is a normal
`fstep:Inline` on `cd_in_msg` — Fbuf's job ends when the
message is on the inbox list.

The full generated-function reference (`BeginRead`,
`BeginWrite`, `GetMsg`, `SkipMsg`, all the buffer-state fields)
is in [amc Fbuf](/txt/exe/amc/fbuf.md).

### End-to-end pattern
<a href="#end-to-end-pattern"></a>

Suppose `gate.MsgHdr` is a 4-byte header with a `len` field
(declared via `dmmeta.lenfld`) and a `type` discriminator
(declared via `dmmeta.typefld`).  The ssim wiring:

```ssim
dmmeta.ctype  ctype:gate.FConn
  dmmeta.field  field:gate.FConn.in_buf  arg:gate.MsgHdr  reftype:Fbuf
    dmmeta.fbuf  field:gate.FConn.in_buf  fbufdir:in
    dmmeta.fcond  fcond:gate.FConn.in_buf/ready  ins:gate.FDb.cd_in_msg  rem:Y

dmmeta.fstep  fstep:gate.FDb.cd_in_msg  steptype:Inline
```

The Main:

```c++
void gate::Main() {
    gate::FConn &conn = gate::conn_Alloc();
    in_buf_BeginRead(conn, algo::Fildes(0));   // bind fd, register iohook
    gate::MainLoop();
}
```

The step that consumes parsed messages:

```c++
void gate::cd_in_msg_Step() {
    gate::FMsg &msg = *cd_in_msg_RemoveFirst();
    InDispatch(msg.base, msg.base.length);     // dispatch on msg.type
    msg_Delete(msg);
}
```

That's the whole thing.  Behind the scenes:

1. `BeginRead` configures the underlying iohook (read,
   edge-triggered, non-blocking) and adds it to epoll/kqueue.
2. When the fd becomes readable, the iohook callback fires
   inside `giveup_time_Step`.  Fbuf's callback drains the fd,
   parses complete messages, and inserts each into
   `cd_in_msg`.
3. The next pass of the scheduler runs `cd_in_msg_Step` once
   per message in the inbox.
4. EOF on the fd is recorded as `in_buf_eof = true`.  Your
   step (or a cleanup step) can check this and tear down the
   connection.

### What Fbuf does that you don't have to
<a href="#what-fbuf-does-that-you-don-t-have-to"></a>

- **Edge-triggered drain.**  Reads until `EAGAIN` on every
  iohook event.  Partial messages are kept in the buffer for
  the next read.
- **Length-delimited framing.**  Splits incoming bytes at the
  `lenfld` boundary of the declared header type.  No state
  machine to maintain.
- **Backpressure-aware writes.**  `BeginWrite` registers a
  writable iohook; writes that block (`EAGAIN`) are queued and
  flushed automatically when the kernel signals writable.
- **Per-buffer trace counters** when `dmmeta.ftrace` is set —
  `<name>_n_read_byte`, `<name>_n_read_msg`, `<name>_n_eagain`,
  etc.  Useful for spotting slow consumers or stuck writers.
- **Zero-copy mode** (`<name>_zerocopy = true`) lets the
  writer hand bytes straight to the kernel without an
  intermediate buffer copy.

### When Fbuf isn't enough
<a href="#when-fbuf-isn-t-enough"></a>

Some traffic doesn't fit the framed-byte-stream pattern: a TTY
in raw mode (one keystroke at a time), a `signalfd`, a control
pipe carrying single bytes as wakeups.  For these, register an
Iohook directly and do the parsing in the callback.  See
the [Iohook reference](/txt/lib/algo_lib/FIohook.md) for the API
and the canonical "read characters from stdin" example.

### See also
<a href="#see-also"></a>

- [Iohook reference](/txt/lib/algo_lib/FIohook.md) — the fd-event
  layer Fbuf builds on
- [amc Fbuf feature page](/txt/exe/amc/fbuf.md) — generated
  functions and field reference
- [Fbuf reftype](/txt/exe/amc/reftype/Fbuf.md) — schema-level
  reference
- [Tutorial 3: Scheduling and main loop](/txt/tut/tut03.md) —
  steps and `next_loop`
- [amc Dispatches](/txt/exe/amc/dispatch.md) — typed message
  dispatch

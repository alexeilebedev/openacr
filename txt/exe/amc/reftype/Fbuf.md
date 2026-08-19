## amc Reftype: Fbuf
<a href="#amc-reftype-fbuf"></a>

`Fbuf` is a **file-descriptor-backed byte buffer with message
framing and edge-triggered epoll integration**.  It binds a
buffer (`elems`, `start`, `end`, `max`) to a file descriptor
via an `algo_lib::FIohook` and a "ready" linked-list so that:

- when the fd has data, the buffer's parent record is auto-
  inserted into the ready list;
- a step (or your own loop) drains the list, calls `GetMsg`,
  processes the message, calls `SkipMsg`, and loops.

Fbuf is the basis of every network/Unix-socket protocol in
X2.  It supports line, byte, and length-delimited message
framing, plus an extern hook for custom protocols.

```
dmmeta.field  field:atf_amc.Msgbuf.in_buf  arg:atf_amc.MsgHeader  reftype:Fbuf  dflt:""  comment:"Message-based buffer"
  dmmeta.fbuf  field:atf_amc.Msgbuf.in_buf  fbufdir:in  max:64  fbuftype:Msgbuf  iotype:standard
  dmmeta.fcond  fcond:atf_amc.Msgbuf.in_buf/ready  ins:atf_amc.FDb.cd_in_msg  via:""  rem:Y
```

The feature page [Fbuf](/txt/exe/amc/fbuf.md) goes through the
data-flow patterns in detail; this page is the field-level
reference.

### What it generates
<a href="#what-it-generates"></a>

A dozen storage fields are added to the parent ctype
(see `tclass_Fbuf` in `cpp/amc/fbuf.cpp`):

| Field                  | Type             | Always | Meaning                                  |
|------------------------|------------------|--------|------------------------------------------|
| `<name>_elems`         | `u8*`            | yes    | data buffer                              |
| `<name>_max`           | `u32`            | yes    | allocated size (0 = unbounded, resizes)  |
| `<name>_start`         | `i32`            | yes    | offset of first valid byte               |
| `<name>_end`           | `i32`            | yes    | offset past last valid byte              |
| `<name>_msglen`        | `i32`            | yes    | length of currently-detected message     |
| `<name>_err`           | `algo::Errcode`  | yes    | last OS-level error code                 |
| `<name>_eof`           | `bool`           | yes    | no more bytes will arrive                |
| `<name>_msgvalid`      | `bool`           | yes    | a valid message starts at `_start`       |
| `<name>_epoll_enable`  | `bool`           | yes    | use epoll vs. always-call (for kernel-bypass) |
| `<name>_zerocopy`      | `bool`           | output only, with fd | attempt `write` immediately on enqueue |
| `<name>_n_eagain`      | `u64`            | output only, with fd | EAGAIN counter           |
| `<name>_iohook`        | `algo_lib::FIohook` | with fd | epoll registration handle             |
| `<name>_ssl`           | `SSL*`           | iotype:openssl | OpenSSL connection                |

Plus tracing counters (`<name>_n_read_byte`, `<name>_n_read_msg`,
or `<name>_n_write_byte`, `<name>_n_write_msg`) if
`dmmeta.ftrace` is set on the field.

### Ssim inputs
<a href="#ssim-inputs"></a>

Required:

- `dmmeta.field` with `reftype:Fbuf`.  `arg:` is the element
  type:
  - `char` or `u8` — line/byte buffers.
  - a struct with `dmmeta.lenfld` — message buffers (length-
    delimited framing).
- `dmmeta.fbuf`:
  - `fbufdir` — `in` for read buffers, `out` for write.
  - `max` — fixed buffer size in bytes (0 = grow dynamically).
  - `fbuftype` — `Linebuf` / `Bytebuf` / `Msgbuf`.
  - `iotype` — `standard` (read/write), `openssl` (SSL_read/
    SSL_write), `nofd` (in-memory only, no fd).

Optional, one `dmmeta.fcond` row per condition the buffer reports.
The key is `<field>/<condition>`, and `ins` names the index the
buffer's parent record enters:

  - `ready` — a message is available (input), or there is data
    to flush (output).
  - `eof` — the buffer will never carry another message.
  - `space` — a congested out buffer drained past its low-water
    mark, so a producer that was refused can write again.

A condition with no `fcond` row is simply not reported.

`Msgbuf` requires `arg` to have a `lenfld`.  `Linebuf` requires
a non-empty `dflt` (the line-terminator character).

### Fbuftype variants
<a href="#fbuftype-variants"></a>

| fbuftype       | Framing                                              |
|----------------|------------------------------------------------------|
| `Linebuf`      | One line per "message"; framing on `dflt` character (usually `'\n'`). |
| `Bytebuf`      | No framing; user reads bytes via `SkipBytes`.        |
| `Msgbuf`       | Length-prefixed: `_msglen` is read from the element's `lenfld`. |

### Custom framing (ScanMsg ffunc)
<a href="#custom-framing-scanmsg-ffunc-"></a>

`ScanMsg` is the internal function that sets `_msglen`/`_msgvalid` for
the next message; amc generates it per `fbuftype`.  To frame messages
yourself, override it with an
[ffunc](/txt/exe/amc/backend/tclass-tfunc.md#ffunc):

```
dmmeta.ffunc  ffunc:<field>.ScanMsg  extrn:Y  comment:"custom framing"
```

amc then emits only the `void <name>_ScanMsg(P&)` prototype and you
implement it, setting `_msglen` and `_msgvalid` on the buffer.  `GetMsg`
calls it as usual.  (This replaced the former `fbuftype:BytebufExtern`,
which is now just `Bytebuf` plus this ffunc.)

### Generated functions
<a href="#generated-functions"></a>

Source: `cpp/amc/fbuf.cpp`.

#### Lifecycle
<a href="#lifecycle"></a>

| Tfunc         | Generated function                  | Effect |
|---------------|-------------------------------------|--------|
| `Fbuf.Init`   | `<name>_Init(P&)`                   | NULL `_elems`, zero counters, allocate fixed-size buffer if `max>0`. |
| `Fbuf.Uninit` | `<name>_Uninit(P&)`                 | Free `_elems`, detach from epoll. |
| `Fbuf.Realloc`| `void <name>_Realloc(P&, int new_max)` | Grow / shrink buffer; preserves data. |

#### Capacity / counters
<a href="#capacity-counters"></a>

| Tfunc        | Generated function           | Effect |
|--------------|------------------------------|--------|
| `Fbuf.Max`   | `i32 <name>_Max(P&)`         | Allocated size. |
| `Fbuf.N`     | `i32 <name>_N(P&)`           | `_end - _start` — valid byte count. |

#### Reading (input buffers)
<a href="#reading-input-buffers-"></a>

| Tfunc                | Generated function                              | Effect |
|----------------------|-------------------------------------------------|--------|
| `Fbuf.BeginRead`     | `void <name>_BeginRead(P&, Fildes fd)`          | Attach `fd`, register edge-triggered with epoll, set the on-ready callback that pushes the parent into the `ready` index. |
| `Fbuf.EndRead`       | `void <name>_EndRead(P&)`                       | Set `_eof = true`. |
| `Fbuf.Refill`        | `bool <name>_Refill(P&)`                        | `read()` into the buffer; returns false at EOF. |
| `Fbuf.GetMsg`        | `<Ret> <name>_GetMsg(P&)`                       | Scan for the next message via `ScanMsg`; Refill if needed.  Returns `MsgHeader*` for Msgbuf, `aryptr<char>` for line / byte buffers, or NULL when no message available. |
| `Fbuf.SkipMsg`       | `void <name>_SkipMsg(P&)`                       | Advance `_start` past the current message; clears `_msgvalid`. |
| `Fbuf.SkipBytes`     | `void <name>_SkipBytes(P&, int n)`              | Advance `_start` by `n` raw bytes (byte/line buffers). |
| `Fbuf.ScanMsg`       | (internal) `<name>_ScanMsg(P&)`                 | Set `_msglen`/`_msgvalid` based on the framing rules.  User-overridable via `ffunc:<field>.ScanMsg extrn:Y` (see [Custom framing](#custom-framing-scanmsg-ffunc-)). |
| `Fbuf.Shift`         | (internal) `<name>_Shift(P&)`                   | Move valid bytes to offset 0 when the head has been consumed. |
| `Fbuf.RemoveAll`     | `void <name>_RemoveAll(P&)`                     | Reset `_start = _end = 0`. |

#### Writing (output buffers)
<a href="#writing-output-buffers-"></a>

| Tfunc                  | Generated function                             | Effect |
|------------------------|------------------------------------------------|--------|
| `Fbuf.BeginWrite`      | `void <name>_BeginWrite(P&, Fildes fd)`        | Attach `fd`, register write hook.  Not generated for a paired write buffer. |
| `Fbuf.EndWrite`        | `void <name>_EndWrite(P&)`                     | Send EOF (zero-byte write); release fd. |
| `Fbuf.WriteAll`        | `bool <name>_WriteAll(P&, u8 *in, i32 n)`      | Append `n` bytes; returns false if the buffer is fixed-size and would overflow. |
| `Fbuf.WriteReserve`    | `void <name>_WriteReserve(P&, u8 *in, i32 n)`  | Append, growing the buffer if necessary (fatal on OOM). |
| `Fbuf.WriteMsg`        | `bool <name>_WriteMsg(P&, T& msg)`             | Append one length-prefixed message (Msgbuf only). |
| `Fbuf.Outflow`         | `void <name>_Outflow(P&)`                      | Drain bytes to the fd via `write()`; tracks EAGAIN. |

#### Streaming allocators (Bytebuf only)
<a href="#streaming-allocators-bytebuf-only-"></a>

| Tfunc                          | Generated function                                   | Effect |
|--------------------------------|------------------------------------------------------|--------|
| `Fbuf.GetAlloc`                | `algo::Alloc <name>_GetAlloc(P&)`                    | Handle that clears, then fills. |
| `Fbuf.GetAllocReserve`         | `algo::Alloc <name>_GetAllocReserve(P&)`             | Same but auto-grows. |
| `Fbuf.BeginAlloc` / `BeginAllocReserve` | internals used by the above                 |        |

### Pitfalls
<a href="#pitfalls"></a>

- **Edge-triggered epoll**.  If you read fewer bytes than
  available, you won't wake up again until the next event.
  `GetMsg` + `SkipMsg` keep draining until the buffer is
  empty or a partial message remains — don't return from your
  step early on the first message.
- **Same fd in & out**.  One descriptor carries one epoll
  registration, so two Fbufs cannot each register their own.
  Declare the pair — `dmmeta.fbuf pair:<read buffer>` on the
  write buffer — and attach it with the generated
  `<read>_BeginReadWrite`.  `dup` is not an alternative: under
  a kernel-bypass stack a dup'd descriptor carries no data.
  See [Fbuf feature page](/txt/exe/amc/fbuf.md).
- **Msgbuf needs `lenfld`**.  amc refuses without it.
- **Linebuf needs `dflt`**.  The default char is the line
  terminator.
- **Zero-copy writes**.  When `_zerocopy:true` and the buffer
  is empty, `WriteAll` attempts an immediate `write()` to skip
  buffering.  Performance win on kernel-bypass paths; can hurt
  on regular paths because each call is its own syscall.
- **`Refill` is fatal on real I/O errors.**  Only EAGAIN /
  EINTR / EOF are recoverable.
- **`_max=0` means dynamic.**  `Realloc` may move data — never
  hold a pointer into `_elems` across an `Outflow`/`WriteAll`.

### See also
<a href="#see-also"></a>

- [Reftypes index](/txt/exe/amc/reftype.md)
- [Fbuf feature page](/txt/exe/amc/fbuf.md) — usage patterns
- [Runtime / steps](/txt/exe/amc/runtime.md) — wiring Fbufs to a step
- Source: `cpp/amc/fbuf.cpp`
- Tfunc records: `acr 'tfunc:Fbuf.%'`
- Spec table: `acr 'dmmeta.fbuf:%'`

### Example
<a href="#example"></a>

An input message buffer wired to a "ready" linked list:

```c++
atf_amc::Msgbuf msgbuf;
in_buf_BeginRead(msgbuf, fd);      // attach fd, register epoll

// per-cycle (driven by fstep on cd_in_msg):
while (atf_amc::MsgHeader *m = in_buf_GetMsg(msgbuf)) {
    process(*m);
    in_buf_SkipMsg(msgbuf);
}
```

Output side:

```c++
atf_amc::Outbuf out;
out_buf_BeginWrite(out, fd);
out_buf_WriteAll(out, payload, payload_len);
// fstep runs on the next cycle:
out_buf_Outflow(out);
```

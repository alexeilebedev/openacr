## amc - fbuf
<a href="#amc"></a>
Fbuf is a field reftype that implements a file i/o buffer.

### Syntax
<a href="#syntax"></a>
- Set reftype:Fbuf
- Add dmmeta.fbuf record
- Add one dmmeta.fcond record per condition the buffer reports

```
dmmeta.field  field:atf_amc.Msgbuf.in_buf  arg:atf_amc.MsgHeader  reftype:Fbuf  dflt:""  comment:"Message-based buffer"
  dmmeta.fbuf   field:atf_amc.Msgbuf.in_buf  max:64  fbuftype:Msgbuf  iotype:standard  comment:""
  dmmeta.fcond  fcond:atf_amc.Msgbuf.in_buf/ready  ins:atf_amc.FDb.cd_in_msg  via:""  rem:Y  comment:""
```

- dmmeta.field.arg: Type of data in buffer; Can be char, u8 or a message header type
- dmmeta.field.reftype: must be `Fbuf`
- dmmeta.fbuf.max: Maximum size of buffer. If 0, the size is set dynamically at runtime with `_Resize`.
- dmmeta.fbuf.fbuftype: one of `Msgbuf`, `Linebuf`, `Bytebuf`

### Conditions
<a href="#conditions"></a>

A buffer's state changes for reasons no field assignment expresses: a socket
becomes readable, a write drains, a peer hangs up. The Fbuf tclass therefore
declares the conditions it can report as `amcdb.tcond` records, and each one
is bound to an index by a `dmmeta.fcond` record keyed `<field>/<condition>`.
When the condition becomes true, the generated code inserts the buffer's
parent record into the `ins` index; a condition with no `fcond` record is not
reported at all.

| condition | meaning |
|---|---|
| `ready` | An input buffer potentially holds a message to read; an output buffer holds data to write out. |
| `eof` | The buffer reached end of life and will never carry another message. |
| `space` | A congested output buffer drained past its low-water mark (see [Producer backpressure](#producer-backpressure)).  Output buffers only. |

`rem:Y` on the `ready` record means the buffer also takes the parent record
back off the index when the condition stops holding — the socket returns
EAGAIN, or the output buffer empties. `eof` and `space` have no falling edge,
so their records are `rem:N` and their step drains the index.

The `ready` record leaves `via` empty, because the same insert function is
installed as the file descriptor's readiness callback and the kernel hands
that callback the buffer's parent record and nothing else. `eof` and `space`
may redirect through `via` to a related record.

### Data direction
<a href="#data-direction"></a>
The buffer is uni-directional: either in or out. Input buffer names must be prefixed with `in_`;
Output buffer names must be prefixed with `out_`.

An input buffer is considered `ready` when it potentially contains a message to be read.
An output buffer is considered `ready` when it has some data that may need to be written out to a file descriptor.

The buffer is associated with a file descriptor; the file descriptor is added to the global epoll fd (algo_lib.FDb.epoll)
with `BeginRead` or `BeginWrite`. Only edge-triggered epoll is used.

When a buffer is marked `ready`, it is inserted into that condition's index, which may be a linked list or a binary
heap, or any other structure; But most commonly it's a linked list with a step attached to it. The step is executed
on the next scheduling cycle and is a user-implemented function. For input buffers, the user calls `GetMsg` to access
the next message. The `GetMsg` function does framing:
- For line buffers (`Linebuf`), the function scans until newline character, or whatever character is specified in `dmmeta.field.dflt`.
- For byte buffers (`Bytebuf`), the function returns whatever is in the buffer. User reads bytes by calling `SkipBytes` but must be careful
to call `Refill` as appropriate.
- For custom framed buffers (a `Bytebuf` with a `ScanMsg` ffunc), the user implements `ScanMsg` function (see below) and calls `GetMsg` / `SkipMsg` as with `Msgbuf`
- For message buffers (`Msgbuf`), the function determines whether the buffer can fit the message and returns the pointer
to the message (which is contained in the buffer)

### Custom Framing
<a href="#custom-framing"></a>
To override the `ScanMsg` function, declare `ffunc:<field>.ScanMsg  extrn:Y` (on a `Bytebuf`) and implement the `ScanMsg` function
The function may set the following variables in the parent record (assuming field name `in_buf`):
```
in_buf_msglen: length of detected message
in_buf_msgvalid: true if message is found
in_buf_eof: true if message cannot be read because it would exceed buffer size
in_buf_err: if message too big, use E2BIG 
```

The user-specified `ScanMsg` function can choose to extend the buffer based on some user-defined strategy
but watch out not to extend the buffer indefinitely because this might crash the process.

### Refilling Input buffer
<a href="#refilling-input-buffer"></a>
When the `GetMsg` function needs more data to find a complete message in the buffer, it calls `Refill`. `Refill` calls `read`,
refilling the buffer from file descriptor. If `read` returns -1, nothing more is done because edge-triggered epoll will be used
to re-insert the buffer into the `ready` index when the file descriptor is reported by the next call to epoll.
If `read` yields some data, `ScanMsg` is reattempted.

### Writing Data
<a href="#writing-data"></a>
Data is written to the output buffer with WriteAll function. This function either writes all bytes to the output
buffer or writes no bytes. If the buffer is dynamically sized but not big enough, it is user's responsibility to resize it
(no automatic resizing is done by generated functions). When the data is added to the buffer, its parent record
is inserted into the `ready` index for outflow.

A fixed-size buffer refuses the write once it is full, and `WriteAll` returns false. Declare the `space`
condition to be told when the buffer has room again, rather than re-attempting the write from your own step — see
[Producer backpressure](#producer-backpressure).

If the `zerocopy` flag is set and the buffer is empty, the `write` call is attempted immediately. This yields a performance
improvement when using kernel bypass. Otherwise, zerocopy should be left `false` so that multiple writes accumulate data in the
buffer, which will be outflown on the next scheduling step.

### Outflowing
<a href="#outflowing"></a>
For output buffers, a buffer is considered ready when there is some data in it. User's step typically rotates through
the list of ready output buffers and calls `Outflow` function, which simply attempts to write bytes to fd. Because the ready index
may be a priority queue, output priority may be implemented.

If the file descriptor is full (write return -1 with EAGAIN), the ready function no longer gets called until epoll
reports that the file descriptor can accept data. At this point, the buffer's containing record is re-inserted into the ready
index and outflowing begins again.

### Producer backpressure
<a href="#producer-backpressure"></a>

Consider a gateway sending records to a client that stops reading its socket.
The socket's send buffer fills, `Outflow` gets EAGAIN, and the output buffer
stops draining. The buffer reaches its `max` and `WriteAll` starts returning
false. What should the producer do with that false?

Polling is the wrong answer, and it is the tempting one. A producer that keeps
its message and re-attempts the write from its own step re-attempts it on every
scheduling cycle, because nothing in that cycle changes the buffer's occupancy.
One stalled client then consumes the whole event loop, and the other work that
loop owns — the other connections, the timers, the flow-control accounting that
would eventually unblock this very client — is starved by a write that cannot
succeed. Growing the buffer instead (`WriteReserve`) removes the refusal but
also removes the bound, so a client that never reads is absorbed by memory
rather than by backpressure.

The insight is that an output buffer is a bounded queue with two independent
flow-control edges, and each edge needs its own resume event. On the consumer
edge the file descriptor refuses bytes, and the resume event is the epoll
writable edge; that is what the `ready` condition and the iohook implement, described
above. On the producer edge the buffer refuses bytes, and the resume event is
the buffer draining. Room in the buffer is not something the producer's step
can bring about, so waiting for it there is waiting in the wrong time domain.
The buffer is what knows when it drains.

The `space` condition carries that event. It is declared only on output
buffers, since an input buffer's app is the consumer and is served by `ready`.
The generated code maintains it with a high-water and a low-water mark:

- `WriteAll` sets `<name>_congested` when the buffer is at or above 75% full,
  and also whenever it actually refuses a message that could ever fit. A
  message longer than the whole buffer never latches, because no drain will
  make it fit; that caller must resize.
- `SkipBytes` — the only place bytes leave an output buffer — clears the latch
  and reports `space` once occupancy falls to 25% or
  below. `RemoveAll` does the same, having emptied the buffer outright.

The two marks are what make this one event rather than a stream of them. A
single mark would insert the parent on every write that crossed it, so a
producer running at the drain rate would be woken per message; the gap between
75% and 25% means a congestion episode produces exactly one wake, delivered
when there is room for a full message rather than for one byte.

The client app writes one step on the `space` index, and in it retries
whatever it parked. The step is the mirror image of the `ready` step: where
that one moves bytes from the buffer to the descriptor, this one lets the
producer move bytes into the buffer again.

### A read and a write buffer over one file descriptor
<a href="#a-read-and-a-write-buffer-over-one-file-descriptor"></a>

A duplex connection reads and writes one socket, and the record that holds it
carries an input buffer and an output buffer.  Both buffers want to know when
the descriptor is ready.

An epoll registration is keyed by the descriptor, not by the watcher: one
descriptor admits one set of event flags and one callback pointer.  A second
registration on the same descriptor is refused, so two buffers cannot each
register their own.  Nor can the second buffer simply take a copy of the
descriptor from `dup`: a kernel-bypass stack holds the connection in user
space, and a dup'd descriptor refers to the kernel-side socket, which carries
no data.

The two buffers are therefore declared as a pair, on the write buffer, naming
the read buffer it shares the descriptor with:

```
dmmeta.fbuf  field:<ns>.FConn.out  ...  pair:<ns>.FConn.in
```

The read buffer then owns the descriptor and the one registration.  The write
buffer has no `FIohook` field of its own, and `BeginWrite` is not generated for
it; the pair is attached in one call, which subscribes to read and write
together:

```
in_BeginReadWrite(conn, fd);
```

Whenever the descriptor reports readiness, the generated `in_PairReady` fans
each reported direction out to the buffer that owns it -- readable arms the read
buffer's `ready`, writable arms the write buffer's -- and an error or hangup,
which arrives as neither, ends the connection through the read buffer's `eof`.

Because the registration is real, the write buffer keeps `epoll_enable` true:
`Outflow` retires the buffer from its ready list when a write moves no bytes,
and the descriptor's write edge brings it back once the socket drains.

amc rejects a pairing that cannot work: declared on the read buffer instead of
the write buffer, naming a field of another ctype or one that is not an fbuf,
naming a buffer that is not a read buffer, either side without a descriptor, or
the two sides disagreeing on `iotype`.

A pair is a property of the ctype, so it fits only a record whose two buffers
*always* share one descriptor.  A ctype attached to two separate pipes in one
mode and to a single descriptor in another cannot be paired.

### Generated fields
<a href="#generated-fields"></a>
- `in_buf_elems`: if fbuf.max is 0, this is a pointer to buffer elements; otherwise it's an inline array of `max` size.
- `in_buf_max`: maximum size of buffer (if fbuf.max is set to a value other than 0, this is defined as an enum)
- `in_buf_start`: current offset, after which valid data begins
- `in_buf_end`: end of valid data; everything between `start..end` corresponds to unread / unwritten data.
- `in_buf_eof`: true if no more data will be added to the buffer. If this variable is set, `ScanMsg` will not attempt
to read more data.
- `in_buf_err`: User-set variable indicating error code
- `in_buf_msgvalid`: True if a valid message has been found in the buffer (at offset `start`)
- `in_buf_msglen`: Length of current message
- `in_buf_iohook`: An iohook associated with the buffer. This field is created for any buffer with a file descriptor
- `in_buf_zerocopy`: Enable zero-copy writing optimization
- `in_buf_n_eagain`: Incremented each time `read` or `write` returns EAGAIN.
- `in_buf_epoll_enable`: May be set before `BeginRead`/`BeginWrite` to disable epoll

### Generated Functions
<a href="#generated-functions"></a>
```
inline-command: src_func atf_amc.% -matchcomment:%atf_amc.Msgbuf.in_buf% -gen -showcomment
// --- atf_amc.Msgbuf.in_buf.BeginRead
// Attach fbuf to Iohook for reading
// Attach file descriptor and begin reading using edge-triggered epoll.
// File descriptor becomes owned by atf_amc::Msgbuf.in_buf via FIohook field.
// Whenever the file descriptor becomes readable, insert msgbuf into cd_in_msg.
void atf_amc::in_buf_BeginRead(atf_amc::Msgbuf& msgbuf, algo::Fildes fd) 
// --- atf_amc.Msgbuf.in_buf.EndRead
// Set EOF flag
void atf_amc::in_buf_EndRead(atf_amc::Msgbuf& msgbuf) 
// --- atf_amc.Msgbuf.in_buf.GetMsg
// Detect incoming message in buffer and return it
// Look for valid message at current position in the buffer.
// If message is already there, return a pointer to it. Do not skip message (call SkipMsg to do that).
// If there is no message, read once from underlying file descriptor and try again.
// The message is length-delimited based on field length field
// 
atf_amc::MsgHeader* atf_amc::in_buf_GetMsg(atf_amc::Msgbuf& msgbuf) 
// --- atf_amc.Msgbuf.in_buf.Realloc
// Set buffer size.
// Unconditionally reallocate buffer to have size NEW_MAX
// If the buffer has data in it, NEW_MAX is adjusted so that the data is not lost
// (best to call this before filling the buffer)
void atf_amc::in_buf_Realloc(atf_amc::Msgbuf& msgbuf, int new_max) 
// --- atf_amc.Msgbuf.in_buf.Refill
// Refill buffer. Return false if no further refill possible (input buffer exhausted)
bool atf_amc::in_buf_Refill(atf_amc::Msgbuf& msgbuf) 
// --- atf_amc.Msgbuf.in_buf.RemoveAll
// Empty bfufer
// Discard contents of the buffer.
void atf_amc::in_buf_RemoveAll(atf_amc::Msgbuf& msgbuf) 
// --- atf_amc.Msgbuf.in_buf.ScanMsg
// Internal function to scan for a message
// 
static void atf_amc::in_buf_ScanMsg(atf_amc::Msgbuf& msgbuf) 
// --- atf_amc.Msgbuf.in_buf.Shift
// Internal function to shift data left
// Shift existing bytes over to the beginning of the buffer
static void atf_amc::in_buf_Shift(atf_amc::Msgbuf& msgbuf) 
// --- atf_amc.Msgbuf.in_buf.SkipMsg
// Skip current message, if any
// Skip current message, if any.
void atf_amc::in_buf_SkipMsg(atf_amc::Msgbuf& msgbuf) 
// --- atf_amc.Msgbuf.in_buf.WriteAll
// Attempt to write buffer contents to fbuf, return success
// Write bytes to the buffer. If the entire block is accepted, return true,
// Otherwise return false.
// Bytes in the buffer are potentially shifted left to make room for the message.
// 
bool atf_amc::in_buf_WriteAll(atf_amc::Msgbuf& msgbuf, u8 *in, i32 in_n) 
// --- atf_amc.Msgbuf.in_buf.WriteReserve
// Write buffer contents to fbuf, reallocate as needed
// Write bytes to the buffer. The entire block is always written or the program exits.
void atf_amc::in_buf_WriteReserve(atf_amc::Msgbuf& msgbuf, u8 *in, i32 in_n) 
// --- atf_amc.Msgbuf.in_buf.Max
// Return max. number of bytes in the buffer.
inline i32 atf_amc::in_buf_Max(atf_amc::Msgbuf& msgbuf) 
// --- atf_amc.Msgbuf.in_buf.N
// Return number of bytes in the buffer.
inline i32 atf_amc::in_buf_N(atf_amc::Msgbuf& msgbuf) 
```

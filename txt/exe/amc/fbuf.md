## amc - fbuf
<a href="#amc"></a>
Fbuf is a field reftype that implements a file i/o buffer.

### Syntax
<a href="#syntax"></a>
- Set reftype:Fbuf
- Add dmmeta.fbuf record

```
dmmeta.field  field:atf_amc.Msgbuf.in_buf  arg:atf_amc.MsgHeader  reftype:Fbuf  dflt:""  comment:"Message-based buffer"
  dmmeta.fbuf  field:atf_amc.Msgbuf.in_buf  max:64  fbuftype:Msgbuf  insready:atf_amc.FDb.cd_in_msg  inseof:""  comment:""
```

- dmmeta.field.arg: Type of data in buffer; Can be char, u8 or a message header type
- dmmeta.field.reftype: must be `Fbuf`
- dmmeta.fbuf.max: Maximum size of buffer. If 0, the size is set dynamically at runtime with `_Resize`.
- dmmeta.fbuf.fbuftype: one of `Msgbuf`, `Linebuf`, `Bytebuf`
- dmmeta.fbuf.insready: Index where the parent record is inserted when the buffer is "ready" (see below). If the value
of this attribute is empty, nothing is done when the buffer is `ready`.
- dmmeta.fbuf.inseof: Index where the parent record is inserted at end of life. If the value of this attribute
is empty, nothing is done when the `eof` is set.

### Data direction
<a href="#data-direction"></a>
The buffer is uni-directional: either in or out. Input buffer names must be prefixed with `in_`;
Output buffer names must be prefixed with `out_`.

An input buffer is considered `ready` when it potentially contains a message to be read.
An output buffer is considered `ready` when it has some data that may need to be written out to a file descriptor.

The buffer is associated with a file desecriptor; The file descriptor is added to global epoll fd (algo_lib.FDb.epoll)
with `BeginRead` or `BeginWrite`. Only edge-triggered epoll is used.

When a buffer is marked `ready`, it is inserted into the `insready` index, which may be a linked list or a binary
heap, or any other structure; But most commonly it's a linked list with a step attached to it. The step is executed
on the next scheduling cycle and is a user-implemented function. For input buffers, the user calls `GetMsg` to access
the next message. The `GetMsg` function does framing:
- For line buffers (`Linebuf`), the function scans until newline character, or whatever character is specified in `dmmeta.field.dflt`.
- For byte buffers (`Bytebuf`), the function returns whatever is in the buffer. User reads bytes by calling `SkipBytes` but must be careful
to call `Refill` as appropriate.
- For custom framed buffers (`BytebufExtern`), the user implements `ScanMsg` function (see below) and calls `GetMsg` / `SkipMsg` as with `Msgbuf`
- For message buffers (`Msgbuf`), the function determins whether the buffer can fit the message and returns the pointer
to the message (which is contained in the buffer)

### Custom Framing
<a href="#custom-framing"></a>
To override the `ScanMsg` function, use fbuftype:BytebufExtern and implement the `ScanMsg` function
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
to re-insert the buffer into the `insready` index when the file descriptor is reported by the next call to epoll.
If `read` yields some data, `ScanMsg` is reattempted.

### Writing Data
<a href="#writing-data"></a>
Data is written to the output buffer with WriteAll function. This function either writes all bytes to the output
buffer or writes no bytes. If the buffer is dynamically sized but not big enough, it is user's responsibility to resize it
(no automatic resizsing is done by generated functions). When the data is added to the buffer, its parent record
is inserted into the `insready` index for outflow.

If the `zerocopy` flag is set and the buffer is empty, the `write` call is attempted immediately. This yields a performance
improvement when using kernel bypass. Otherwise, zerocopy should be left `false` so that multiple writes accumulate data in the
buffer, which will be outflown on the next scheduling step.

### Outflowing
<a href="#outflowing"></a>
For output buffers, a buffer is considered ready when there is some data in it. User's step typically rotates through
the list of ready outout buffers and calls `Outflow` function, which simply attempts to write bytes to fd. Because the ready index
may be a priority queue, output priority may be implemented.

If the file descriptor is full (write return -1 with EAGAIN), the ready function no longer gets called until epoll
reports that the file descriptor can accept data. At this point, the buffer's containing record is re-inserted into the ready
index and outflowing begins again.

### Input and Output buffers with same file descriptor
<a href="#input-and-output-buffers-with-same-file-descriptor"></a>
An input and an output buffer cannot be handed the same file descriptor, since they independently interact with epoll.
Use `dup` to avoid this issue. However some kernel bypass libraries such as `lib_vma` do not support `dup`. In this case,
hand the same file descriptor to the output buffer as you do to the input buffer, but set `epoll_enable` to `false` in advance
to avoid using epoll. In this case, the ready step will be called on every scheduling cycle as long as there is data in the buffer.

### Generated fields
<a href="#generated-fields"></a>
- `in_buf_elems`: if fbuf.max is 0, this is a pointer to buffer elements; otherwise it's an inline array of `max` size.
- `in_buf_max`: maximum size of buffer (if fbuf.max is set to a value other than 0, this is defined as an enum)
- `in_buf_start`: current offset, after which valid data begins
- `in_buf_end`: end of valid data; everything between `start..end` corresponds to unread / unwritten data.
- `in_buf_eof`: true if no more data will be added to the buffer. If this variable is set, `Scanmsg` will not attempt
to read more data.
- `in_buf_err`: User-set variable indicating error code
- `in_buf_msgvalid`: True if a valid message has been found in the buffer (at off set `start`)
- `in_buf_msglen`: Length of current message
- `in_buf_iohook`: An iohook associated with the buffer. This field is created if `insready` is a valid index
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
// Write bytes to the buffer. If the entire block is written, return true,
// Otherwise return false.
// Bytes in the buffer are potentially shifted left to make room for the message.
// 
bool atf_amc::in_buf_WriteAll(atf_amc::Msgbuf& msgbuf, u8 *in, i32 in_n) 
// --- atf_amc.Msgbuf.in_buf.WriteReserve
// Write buffer contents to fbuf, reallocate as needed
// Write bytes to the buffer. The entire block is always written
void atf_amc::in_buf_WriteReserve(atf_amc::Msgbuf& msgbuf, u8 *in, i32 in_n) 
// --- atf_amc.Msgbuf.in_buf.XrefMaybe
// Insert row into all appropriate indices. If error occurs, store error
// in algo_lib::_db.errtext and return false. Caller must Delete or Unref such row.
bool atf_amc::in_buf_XrefMaybe(atf_amc::MsgHeader &row) 
// --- atf_amc.Msgbuf.in_buf.Max
// Return max. number of bytes in the buffer.
inline i32 atf_amc::in_buf_Max(atf_amc::Msgbuf& msgbuf) 
// --- atf_amc.Msgbuf.in_buf.N
// Return number of bytes in the buffer.
inline i32 atf_amc::in_buf_N(atf_amc::Msgbuf& msgbuf) 
```


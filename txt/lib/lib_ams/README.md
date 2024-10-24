## lib_ams - Library for AMS middleware, supporting file format & messaging
<a href="#lib_ams"></a>

AMS is a shared memory transport for communication between single-threaded processes.
Every process is a single-threaded process.

When invoked with default options, each process reads messages from stdin and writes all of its outputs to stdout.
Some of the inputs (see list of ams messages, specifically message `ams.OpenMsg`)
may instruct the process to start reading / writing shared memory and turn off reading of messages from stdin.

AMS communication is based on streams. A stream is a named append-only sequence of messages.
Every stream has only a single writer process, but may have multiple readers.
All stream messages are sequenced and gaps are not allowed.
Writers wait until readers consume data before overwriting old messages.
Stream readers are registered with the control process which watches for liveness and keeps track of overall state.

A stream ID is a 32-bit entity consisting of four 8-bit fields: process type, process id, stream type, and stream index.
- ProcType: 8-bit value, see table proctype
- ProcId: 16-bit value, ProcType + index
- StreamType: 8-bit value, see table streamtype
- StreamId: proc_id + stream_type + stream_idx

Process types are registered in table `amsdb.proctype`. The standard types are:

```
inline-command: acr proctype | ssimfilt ^ -t
PROCTYPE  ID  NS            COMMENT
0         0                 No process
amstest   5   ams_sendtest  Ams testing

```

Stream types are registered in table `amsdb.streamtype`. The standard types are:

```
inline-command: acr streamtype
amsdb.streamtype  streamtype:0      id:0  comment:"No stream"
amsdb.streamtype  streamtype:ctl    id:1  comment:"AMS control messages"
amsdb.streamtype  streamtype:io     id:5  comment:"Messages intended for file output"
amsdb.streamtype  streamtype:out    id:2  comment:"Log messages, alarms"
amsdb.streamtype  streamtype:trace  id:6  comment:"Trace messages"
report.acr  n_select:5  n_insert:0  n_delete:0  n_ignore:0  n_update:0  n_file_mod:0
```

Together, stream id is formatted like this: `proctype-index.streamtype-index`.
For instance, `proc1-0.md-0`.

Proc types and stream types can be extended for the given system being implemented.
For every stream that a process reads or writes, it publishes one `ams.StreamHb` message to its `ctl`
output stream. The `ctl` output stream is different from standard output because it is non-deterministic
(contains time-based messages such as heartbeats).

### Table Of Contents
<a href="#table-of-contents"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Toc -->
&nbsp;&nbsp;&bull;&nbsp;  [AMS Library](#ams-library)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [List of AMS Messages](#list-of-ams-messages)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [`ams.ProcAdd`: Create process](#-ams-procadd--create-process)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [`ams.ProcRemove`: Destroy process](#-ams-procremove--destroy-process)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [`ams.Seqmsg`: A sequenced message](#-ams-seqmsg--a-sequenced-message)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [`ams.OpenMsg`: Open stream for reading/writing](#-ams-openmsg--open-stream-for-reading-writing)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [`ams.StreamHb`: Stream heartbeat](#-ams-streamhb--stream-heartbeat)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [In-memory stream format; Flow control](#in-memory-stream-format--flow-control)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Performance](#performance)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [AMS Stream Tracing](#ams-stream-tracing)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Functions](#functions)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Sources](#sources)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Dependencies](#dependencies)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [In Memory DB](#in-memory-db)<br/>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Toc -->

## AMS Library
<a href="#ams-library"></a>

Ams logic is implemented in the library `lib_ams`.
Each process knows what to do with certain streams; For instance, each process will direct
disk IO to the io stream if it's configured.

## List of AMS Messages
<a href="#list-of-ams-messages"></a>

All ams messages are derived from `ams.MsgHeader`, which is a 32-bit header with type
and length.

### `ams.ProcAdd`: Create process
<a href="#-ams-procadd--create-process"></a>

```
inline-command: acr ctype:ams.ProcAddMsg -t | awk -v RS='' '/dmmeta.ctype/'
dev.license  license:GPL  comment:""
dmmeta.nstype  nstype:protocol  comment:"Protocol (not a target)"
  dmmeta.ns  ns:ams  nstype:protocol  license:GPL  comment:"Algo Messaging System protocol"
    dmmeta.ctype  ctype:ams.ProcAddMsg  comment:"Add process to process group"
      dmmeta.cpptype  ctype:ams.ProcAddMsg  ctor:Y  dtor:N  cheap_copy:N
      dmmeta.msgtype  ctype:ams.ProcAddMsg  type:4
      dmmeta.pack  ctype:ams.ProcAddMsg  comment:""
      dmmeta.field  field:ams.ProcAddMsg.base     arg:ams.MsgHeader  reftype:Base  dflt:""  comment:""
      dmmeta.field  field:ams.ProcAddMsg.proc_id  arg:ams.ProcId  reftype:Val  dflt:""  comment:"ID of process being added"
      dmmeta.cfmt  cfmt:ams.ProcAddMsg.String  printfmt:Tuple  read:Y  print:Y  sep:""  genop:Y  comment:""
      dmmeta.ctypelen  ctype:ams.ProcAddMsg  len:10  alignment:1  padbytes:0  plaindata:Y
```

Control process creates a new subprocess.
The new subprocess reads main control stream starting with latest sequence number.
The control stream reads output of the child process. By default, the output is directed to stdout.

### `ams.ProcRemove`: Destroy process
<a href="#-ams-procremove--destroy-process"></a>

```
inline-command: acr ctype:ams.ProcRemoveMsg -t | awk -v RS='' '/dmmeta.ctype/'
dev.license  license:GPL  comment:""
dmmeta.nstype  nstype:protocol  comment:"Protocol (not a target)"
  dmmeta.ns  ns:ams  nstype:protocol  license:GPL  comment:"Algo Messaging System protocol"
    dmmeta.ctype  ctype:ams.ProcRemoveMsg  comment:"Remove process from process group"
      dmmeta.cpptype  ctype:ams.ProcRemoveMsg  ctor:Y  dtor:N  cheap_copy:N
      dmmeta.msgtype  ctype:ams.ProcRemoveMsg  type:5
      dmmeta.pack  ctype:ams.ProcRemoveMsg  comment:""
      dmmeta.field  field:ams.ProcRemoveMsg.base     arg:ams.MsgHeader  reftype:Base  dflt:""  comment:""
      dmmeta.field  field:ams.ProcRemoveMsg.proc_id  arg:ams.ProcId  reftype:Val  dflt:""  comment:"ID of process being removed"
      dmmeta.cfmt  cfmt:ams.ProcRemoveMsg.String  printfmt:Tuple  read:Y  print:Y  sep:""  genop:Y  comment:""
      dmmeta.ctypelen  ctype:ams.ProcRemoveMsg  len:10  alignment:1  padbytes:0  plaindata:Y
```

Control process destroys the subprocess and removes its streams.

### `ams.Seqmsg`: A sequenced message
<a href="#-ams-seqmsg--a-sequenced-message"></a>

All shared memory messages have this header.
Each Seqmsg is identified with `seqmsg_id`, a 96-bit quantity formatted as `stream_id@seqno`.
Each Seqmsg has a `tsc` field, which holds the value of CPU's TSC register, filled out at the time
of creation.

```
inline-command: acr ctype:ams.Seqmsg -t | awk -v RS='' '/dmmeta.ctype/'
dev.license  license:GPL  comment:""
dmmeta.nstype  nstype:protocol  comment:"Protocol (not a target)"
  dmmeta.ns  ns:ams  nstype:protocol  license:GPL  comment:"Algo Messaging System protocol"
    dmmeta.ctype  ctype:ams.Seqmsg  comment:"Sequenced stream message"
      dmmeta.cpptype  ctype:ams.Seqmsg  ctor:Y  dtor:N  cheap_copy:N
      dmmeta.msgtype  ctype:ams.Seqmsg  type:10
      dmmeta.pack  ctype:ams.Seqmsg  comment:""
      dmmeta.field  field:ams.Seqmsg.base       arg:ams.MsgHeader  reftype:Base  dflt:""  comment:""
      dmmeta.field  field:ams.Seqmsg.seqmsg_id  arg:ams.SeqmsgId   reftype:Val  dflt:""  comment:"Message ID"
      dmmeta.field  field:ams.Seqmsg.tsc        arg:u64            reftype:Val  dflt:""  comment:"TSC of sender"
      dmmeta.field  field:ams.Seqmsg.payload    arg:ams.MsgHeader  reftype:Opt  dflt:""  comment:"Actual Payload"
      dmmeta.cfmt  cfmt:ams.Seqmsg.String  printfmt:Tuple  read:Y  print:Y  sep:""  genop:Y  comment:""
      dmmeta.ctypelen  ctype:ams.Seqmsg  len:32  alignment:1  padbytes:0  plaindata:N
```

### `ams.OpenMsg`: Open stream for reading/writing
<a href="#-ams-openmsg--open-stream-for-reading-writing"></a>

```
inline-command: acr ctype:ams.OpenMsg -t | awk -v RS='' '/dmmeta.ctype/'
dev.license  license:GPL  comment:""
dmmeta.nstype  nstype:protocol  comment:"Protocol (not a target)"
  dmmeta.ns  ns:ams  nstype:protocol  license:GPL  comment:"Algo Messaging System protocol"
    dmmeta.ctype  ctype:ams.OpenMsg  comment:"Open stream for reading"
      dmmeta.cpptype  ctype:ams.OpenMsg  ctor:Y  dtor:N  cheap_copy:N
      dmmeta.msgtype  ctype:ams.OpenMsg  type:7
      dmmeta.pack  ctype:ams.OpenMsg  comment:""
      dmmeta.field  field:ams.OpenMsg.base    arg:ams.MsgHeader  reftype:Base  dflt:""  comment:""
      dmmeta.field  field:ams.OpenMsg.member  arg:ams.Member     reftype:Val  dflt:""  comment:"Target process_id and stream_id"
      dmmeta.field  field:ams.OpenMsg.pos     arg:ams.StreamPos  reftype:Val  dflt:""  comment:"Start reading here"
      dmmeta.cfmt  cfmt:ams.OpenMsg.String  printfmt:Tuple  read:Y  print:Y  sep:""  genop:Y  comment:""
      dmmeta.ctypelen  ctype:ams.OpenMsg  len:31  alignment:1  padbytes:0  plaindata:Y
```

Begin reading/writing stream identified by `streamid` from shared memory region
For a shared memory region, reading begins at sequence number 1 and proceeds with no gaps.
Writing begins at sequence number 1.
Begin publishing a heartbeat describing current reading state.
Only one process can write to a given shared memory area, but multiple processes can read from it

### `ams.StreamHb`: Stream heartbeat
<a href="#-ams-streamhb--stream-heartbeat"></a>

```
inline-command: acr ctype:ams.StreamHbMsg -t | awk -v RS='' '/dmmeta.ctype/'
dev.license  license:GPL  comment:""
dmmeta.nstype  nstype:protocol  comment:"Protocol (not a target)"
  dmmeta.ns  ns:ams  nstype:protocol  license:GPL  comment:"Algo Messaging System protocol"
    dmmeta.ctype  ctype:ams.StreamHbMsg  comment:"Stream heartbeat"
      dmmeta.cpptype  ctype:ams.StreamHbMsg  ctor:Y  dtor:N  cheap_copy:N
      dmmeta.msgtype  ctype:ams.StreamHbMsg  type:3
      dmmeta.pack  ctype:ams.StreamHbMsg  comment:""
      dmmeta.field  field:ams.StreamHbMsg.base     arg:ams.MsgHeader  reftype:Base  dflt:""  comment:""
      dmmeta.field  field:ams.StreamHbMsg.member   arg:ams.Member     reftype:Val  dflt:""  comment:"Primary key"
      dmmeta.field  field:ams.StreamHbMsg.pos      arg:ams.StreamPos  reftype:Val  dflt:""  comment:"Read/write position"
      dmmeta.field  field:ams.StreamHbMsg.wbudget  arg:u32            reftype:Val  dflt:""  comment:"Max. offset for writing"
      dmmeta.cfmt  cfmt:ams.StreamHbMsg.String  printfmt:Tuple  read:Y  print:Y  sep:""  genop:Y  comment:""
      dmmeta.ctypelen  ctype:ams.StreamHbMsg  len:35  alignment:1  padbytes:0  plaindata:Y
```

For each stream being either read or written, each process outputs a StreamHb message.
For a reading process, the message declares how many messages have been read.
For a writing process (`write:Y`), the message declares how many messages have been written
For a process writing to disk, `commit:Y` is specified

## In-memory stream format; Flow control
<a href="#in-memory-stream-format--flow-control"></a>

The contents of a stream can be thought of as a sequence of back-to-back `Seqmsg` messages.
The first message is at offset 0, the second one has offset msg0.length, etc.
The messages are published through a shared memory region of size 2^k.
The region, plus a 4Kb helper region are mapped into memory of both reading and writing process.
A message is written contiguously to memory at offset `offset & ((1<<k)-1)`.
The message is written in such a way that the length field is populated last. Thus the reader just
checks the length field at a known offset to poll for message receive.

Looking at the memory region it's not possible to tell where the next message starts.
One has to follow the progress of the stream to know the offsets.
The heartbeat message written to ctl stream reports a {sequence,offset} pair
that can be used to start reading at that location.
However, randomly reading a shared memory region isn't possible either, as the writer process
could overwrite a region at any time. One has to be registered as a reader first. The writer
will avoid overwriting any memory not read by the reader yet.

Readers are announced to the writer on input, and are added to the table of streams.
All readers must read all messages before the those message bytes can be overwritten.
When writing a message, the length is saved to memory last. Zero length means the message is not yet
published. Writer process calls `sfence()` before writing length, reader process calls `lfence()`
after reading non-zero length.

Reading process polls current offset until it sees a non-zero length.
The message can be safely read without fear of being overwritten.
Control process kills child processes that do not send heartbeats (each process may have its own heartbeat
setting, disk write process is given at least 60 seconds). After killing a child process, control
process publishes an appropriate messages so that all writers can remove the dead reader from their table.

### Performance
<a href="#performance"></a>

Use of single writer and multiple readers, with strict flow control,
increases effective size for shared CPU cache. This is the closest
one can get to multicast messaging (the most scalable messaging method) within a single machine.
So, if delivering a single message to one recipient is done within 60 nanoseconds,
so is delivering a single message to 4 recipients.

The following tests were done without any thread pinning.
Latency of first 10,000 
On a 3GHz machine, With single reader, single writer, average one-way latency to send 2,000,000 messages
through a small (256KB) window is about 150 cycles (best case) and about 220 cycles on average.

With one reader and 8 writers, one-way latency is about 1,000-1,600 cycles. On a 3.3 GHz machine
this means that one process can send a 64-byte message, and 160 nanoseconds later, 8 different processes can read it.

```
$ sudo chrt -r 99 ./atf_unit -check_untracked:N -nofork lib_ams.SendData8
Set dflt shmem size to 2101248
Parent: writing messages...
parent: wrote all 2000000 messages
amstest-1: avg recv latency 978 clocks
amstest-2: avg recv latency 977 clocks
amstest-5: avg recv latency 1344 clocks
amstest-4: avg recv latency 1340 clocks
amstest-3: avg recv latency 1338 clocks
amstest-8: avg recv latency 1374 clocks
amstest-7: avg recv latency 1384 clocks
amstest-6: avg recv latency 1391 clocks
amstest-0: atf_unit.AmsSendTest  n_msg_send:2000000  n_msg_recv:0  n_write_wait:0  n_msg_limit:2000000  sum_recv_latency:0  n_recv_latency:0  n_write_block_spin:0
atf_unit.unittest  unittest:lib_ams.SendData8  success:Y  comment:""
report.atf_unit  n_test_total:526  success:Y  n_test_run:1  n_err:0
```

### AMS Stream Tracing
<a href="#ams-stream-tracing"></a>

When the traace expression is applied using the function `lib_ams::ApplyTrace`,
both log categories and AMS streams are considered.

The trace expression is also applied to each each stream as follows:
The following string is matched against the regex:
- proc_id/stream_id       enable all tracing in ascii
- proc_id/stream_id,r     enable tracing of read messages in ascii
- proc_id/stream_id,w     enable tracing of written messages in ascii
- proc_id/stream_id,rb    enable tracing of read messages in binary
- proc_id/stream_id,wb    enable tracing of written messages in binary

Messages are considered "written" to a stream when the process calls `lib_ams::WriteMsg`.
This is when tracing happens.

Notably, `WriteMsg` is called when a messages is read from stdin and posted to a stream.

Message is considered read from a stream when it is skipped using `lib_ams::SkipMsg`
This happens after all of the outputs produced while handling the message.
Examples of ams stream trace expressions:

- %/%.out-0                        read+write, output from all processes in the tree
- %/%.md-0,w                       all messages written to md-0 stream
- (bbox-0/bbox-0.out-0,r|...)      explicit list

### Functions
<a href="#functions"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Functions -->
Functions exported from this namespace:

```c++
// In-process `expect` implementation allows pausing reading of inputs until
// a certain prlog output is produced
// Motivating example:
// InputMsg1
// InputMsg2
// Expect text:blah
// InputMsg3
// InputMsg3 is not processed until blah is printed.
// Implementation:
// When reading an input message:
// clear output buffer
// if read an ExpectMsg:
// set expect string to the input data
// check output buffer (match any outputs since last input against expect string)
// set expect timeout
// Expect timeout:
// exit process with error
// When printing:
// add to output buffer (limit: 100K).
// if expect mode, check output buffer
void lib_ams::ExpectEnable(strptr str) 
```

```c++
// Replace input reading function with one that pauses output
// to support ExpectMsg functionality
void lib_ams::ExpectAttach(lib_ams::FStream &stream) 
```

```c++
// Function that replaces uses-provided message hook for a given stream
// Having no cost until the first call to ExpectEnable, the purpose
// is to block processing of any messages until expect_Str is empty
void lib_ams::ExpectHook(lib_ams::FStream &stream, ams::MsgHeader &msg) 
```

```c++
// Save output to possibly check against expect string later
// Keep last 50k-100k of output
void lib_ams::ExpectSaveOutput(algo::strptr text) 
```

```c++
void lib_ams::ExpectTimeout() 
```

```c++
// Check saved output buffer against expect string
// WARNING: calling prlog() from this function will cause an infinite loop
void lib_ams::ExpectCheck() 
```

```c++
// Whether write file is in good state - valid fd and no failure
bool lib_ams::OkQ(lib_ams::FWritefile &wf) 
```

```c++
// Check for failure. This is intended for checking Close errors.
// After Close, OkQ could not be used for this purpose,
// but failure flag is preserved, and could be retrieved with FailQ.
bool lib_ams::FailQ(lib_ams::FWritefile &wf) 
```

```c++
// Open file for appending messages
void lib_ams::OpenAppend(lib_ams::FWritefile &wf, algo::strptr filename) 
```

```c++
// Flush data to file
void lib_ams::zd_flush_Step() 
```

```c++
// Write message (buffered)
void lib_ams::WriteMsg(lib_ams::FWritefile &wf, ams::MsgHeader &msg) 
```

```c++
// Write buffer as message block
void lib_ams::Flush(lib_ams::FWritefile &wf) 
```

```c++
// Compress buffer - form full message in cbuf
void lib_ams::Compress(lib_ams::FWritefile &wf) 
```

```c++
// Write message block to file
void lib_ams::WriteMsgBlock(lib_ams::FWritefile &wf) 
```

```c++
// starts new message block - discard buffer, adjust seqno
void lib_ams::StartNextMsgBlock(lib_ams::FWritefile &wf) 
```

```c++
// Close written file
void lib_ams::Close(lib_ams::FWritefile &wf) 
```

```c++
// cleanup function
void lib_ams::fd_Cleanup(lib_ams::FWritefile& parent) 
```

```c++
// Whether read file is in good state - valid fd and no failure.
// EOF is not a failure.
bool lib_ams::OkQ(lib_ams::FReadfile &rf) 
```

```c++
// Check for failure after close.
// EOF is not a failure.
bool lib_ams::FailQ(lib_ams::FReadfile &rf) 
```

```c++
// Check for EOF
bool lib_ams::EofQ(lib_ams::FReadfile &rf) 
```

```c++
// Whether can read from file - file is good and no EOF
bool lib_ams::CanReadQ(lib_ams::FReadfile &rf) 
```

```c++
// Open file for reading messages
void lib_ams::OpenRead(lib_ams::FReadfile &rf, algo::strptr filename) 
```

```c++
// Read message by sequence number
ams::MsgHeader *lib_ams::ReadMsg(lib_ams::FReadfile &rf, u64 seqno) 
```

```c++
// Scan file up to end
void lib_ams::ScanToEnd(lib_ams::FReadfile &rf) 
```

```c++
// Scan up to seqno, rewind if needed
void lib_ams::ScanToSeqno(lib_ams::FReadfile &rf, u64 seqno) 
```

```c++
// Scan and block load engine.
// if seqno is NULL, scans to end, fills block with last block.
// If seqno is present, scans up to given seqno, reads cbuf with payload
// Note is does only forward scan.
void lib_ams::Scan(lib_ams::FReadfile &rf,const u64 *seqno) 
```

```c++
// Skip bytes
void lib_ams::Skip(lib_ams::FReadfile &rf,u32 skip) 
```

```c++
// Read bytes
bool lib_ams::Read(lib_ams::FReadfile &rf,void *buf,u32 n) 
```

```c++
// Uncompress
void lib_ams::Uncompress(lib_ams::FReadfile &rf) 
```

```c++
// Reindex
void lib_ams::Reindex(lib_ams::FReadfile &rf) 
```

```c++
// Close read file
void lib_ams::Close(lib_ams::FReadfile &wf) 
```

```c++
// cleanup function
void lib_ams::fd_Cleanup(lib_ams::FReadfile& parent) 
```

```c++
// The length field of a message must reside entirely within one cache line,
// or the sfence/lfence instructions won't behave as expected.
// So, we force each message to start at an address that'a a multiple of machine
// cache line size.
inline u64 AddOffset(u64 offset, int n) 
```

```c++
inline ams::Seqmsg *MsgAtOffset(lib_ams::FStream &stream, u64 offset) 
```

```c++
inline void UpdateAckOffset(lib_ams::FStream &stream) 
```

```c++
// Scan /dev/shm for stale stream files and delete them
// A file is stale if it's flockable (i.e. no process has locked it for writing)
// and it's at least 1 hour old
void lib_ams::CleanOldStreamFiles() 
```

```c++
// return TRUE if shared memory region is attached to stream STREAM.
bool lib_ams::ShmemOpenQ(lib_ams::FStream &stream) 
```

```c++
// Open shared memory for reading/writing (as specified in FLAGS)
// and return resulting shared memory region size.
// Fields initialized:
// - stream.filename
// - stream.shm_handle (windows)
// - stream.shm_file (linux)
// - stream.shm_region, if mapped successfully
i64 lib_ams::OpenShmem(lib_ams::FStream &stream, ams::StreamFlags flags) 
```

```c++
// Open stream for reading or writing (but not both)
// If shared memory mode (lib_ams::_db.shmem_mode) is set,
// The file creator sets stream size.
// Writer creates & locks the file.
// Reader determines file from file size
// If shared memory mode is not set (stdio mode),
// the stream is attached to a private memory block of default size.
// When opening for writing:
// The next message written to the stream will have sequence SEQ and offset OFFSET.
// Default is to use sequence 1, offset 0.
// (However a newly spawned child process can be provided a known sequence & offset by the parent.)
// When opening for reading:
// begin hot-polling for new messages on the stream
bool lib_ams::OpenStream(lib_ams::FStream &stream, ams::StreamFlags flags, ams::StreamPos pos) 
```

```c++
bool lib_ams::OpenStream(lib_ams::FStream &stream, ams::StreamFlags flags) 
```

```c++
// TBD not sure if this function is needed in non-fork() environment
bool lib_ams::ReopenForReading(lib_ams::FStream &stream) 
```

```c++
// Update budget for stream STREAM
// Return TRUE if the stream is potentially unblocked for
// writing due to WRITELIMIT being increased.
// (WRITELIMIT is the point beyond which no message can be written
// because doing so would overwrite data not yet consumed by one of the read members.)
// TODO: do not update budget for a stream that is not opened for writing
bool lib_ams::UpdateBudget(lib_ams::FStream &stream) 
```

```c++
// Update stream budgets
void lib_ams::UpdateBudgets() 
```

```c++
// Register a bad, unskippable incoming message
// on STREAM, report it, and stop reading.
void lib_ams::WriterError(lib_ams::FStream &stream, ams::Seqmsg *msg) 
```

```c++
// If the stream is open for reading, check to see if a message
// is available.
// If it is available, return pointer to message.
ams::Seqmsg *lib_ams::PeekMsg(lib_ams::FStream &stream) 
```

```c++
// Called by the client
// to aoivd reading current message
void lib_ams::StopReading(lib_ams::FStream &stream) 
```

```c++
// Mark current message as read and move to the next message.
void lib_ams::SkipMsg(lib_ams::FStream &stream) 
```

```c++
int lib_ams::WriteBudget(lib_ams::FStream &stream) 
```

```c++
// Send heartbeat to control stream
// Update
void lib_ams::SendHb(lib_ams::FStream &stream) 
```

```c++
// Publish stream heartbeats to c_stream_ctl stream
void lib_ams::cd_stream_hb_Step() 
```

```c++
// Check all streams (that are not already readable) for readability.
// If readable, call h_msg hook.
void lib_ams::cd_poll_read_Step() 
```

```c++
void lib_ams::PollCtlIn() 
```

```c++
// Begin writing message of length LENGTH
// Used by WriteMsg and with amc's pnew (acr pnew) for zero-copy sends.
// If successful, return pointer to region of size LENGTH whre the mssage can be written,
// otherwise return NULL.
void *lib_ams::BeginWrite(lib_ams::FStream &stream, int length) 
```

```c++
// Finish writing message of length LENGTH,
// and send the result.
void lib_ams::EndWrite(lib_ams::FStream &stream, void *ptr, int len) 
```

```c++
// If the stream is attached to a writable shared memory segment,
// write sequenced message to stream STREAM, and increment sequence number
// and stream offset.
// Message must be within max. message size
// If the stream is not configured for writingIf an output fd is associated with the stream, hthe stream, write
bool lib_ams::WriteMsg(lib_ams::FStream &stream, ams::MsgHeader &msg) 
```

```c++
// Write message MSG to output, either in binary or text mode.
// Return success status
// If ISREAD flag is set, this is a copy of a message that has just been read
// from another stream. Otherwise, it's a copy of a message that's just been written.
// In stdin mode, where messages are read from ascii input and posted to the
void lib_ams::TraceMsg(lib_ams::FStream &stream, ams::MsgHeader &msg, bool isread) 
```

```c++
// Initialize stream library
// FILE_PREFIX: unique filesystem prefix for all subsequently created streams
// Also clean any stale (unlocked) stream files.
// (this can be disabled by setting _db.stream_files_cleaned to true)
bool lib_ams::Init(algo::strptr file_prefix, ams::ProcId proc_id) 
```

```c++
void lib_ams::Uninit() 
```

```c++
void lib_ams::shm_handle_Cleanup(lib_ams::FStream &stream) 
```

```c++
void lib_ams::shm_file_Cleanup(lib_ams::FStream &stream) 
```

```c++
// Process joined the group
void lib_ams::CtlMsg_ProcAddMsg(ams::ProcAddMsg &msg) 
```

```c++
// A process has exited
// Remove any read/write members contributed by the process and
// potentially unblock some write streams
void lib_ams::CtlMsg_ProcRemoveMsg(ams::ProcRemoveMsg &msg) 
```

```c++
void lib_ams::CtlMsg_DumpStreamTableMsg(ams::DumpStreamTableMsg &msg) 
```

```c++
void lib_ams::CtlMsg_StreamHbMsg(ams::StreamHbMsg &msg) 
```

```c++
void lib_ams::ReadCtlMsg(lib_ams::FStream &, ams::MsgHeader &msg) 
```

```c++
// Read next input line from stdin
void lib_ams::cd_fdin_read_Step() 
```

```c++
// Stop reading stdin
void lib_ams::cd_fdin_eof_Step() 
```

```c++
// Begin reading ams control messages from stdin
void lib_ams::BeginReadStdin() 
```

```c++
lib_ams::FStream &lib_ams::ind_stream_GetOrCreate(ams::StreamId stream_id) 
```

```c++
void lib_ams::DumpStreamTableDflt() 
```

```c++
void lib_ams::DumpStreamTableVisual() 
```

```c++
void lib_ams::DumpStreamTable(int format = 0) 
```

```c++
// Close stream.
// If stream is opened for writing, remove its file.
// BUG: I think if the stream is opened for reading, the mmap() leaks
void lib_ams::Close(lib_ams::FStream &stream) 
```

```c++
void lib_ams::SetDfltShmSize(u32 size) 
```

```c++
ams::StreamPos lib_ams::ReadStreamPos(algo::strptr val) 
```

```c++
// Register PROC_ID as reader of stream STREAM_ID starting at offset 0
// This immediately limits the write budget of STREAM_ID
// (Subsequent attempt to overwrite data not yet consumed by PROC_ID causes
// either reject or blocking wait)
void lib_ams::AddReadMember(ams::ProcId proc_id, ams::StreamId stream_id) 
```

```c++
// Open stream STREAM for reading and add it to the list of streams
// which lib_ams scans for messages (i.e. control streams).
// Attach lib_ams message dispatch to the stream
bool lib_ams::AddCtlIn(lib_ams::FStream &stream) 
```

```c++
void lib_ams::CloseAllStreams() 
```

```c++
// Match trace expression REGX against stream STREAM.
// If it matches, enable/disable tracing as indicated by ENABLE
// For full description see txt/trace.md
bool lib_ams::ApplyTrace(lib_ams::FStream &stream, algo_lib::Regx &regx, bool enable) 
```

```c++
// Enable or disable logcat tracing based on regex WHAT
// If TRACE is specified, logcats matching regex are enabled; otherwise
// they are disabled.
// This affects all future `prlog_cat` calls.
// Return number of matches
int lib_ams::ApplyTrace(algo::strptr what, bool enable) 
```

```c++
// Same as ApplyTrace, but
// If expression doesn't match anything, print a helpful message for the user
void lib_ams::ApplyTraceV(algo::strptr what, bool enable) 
```

```c++
// prlog hook for ams application, could be used for sending log messages to output stream
void lib_ams::Prlog(algo_lib::FLogcat *logcat, algo::SchedTime tstamp, strptr str) 
```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Functions -->

### Inputs
<a href="#inputs"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Inputs -->
`lib_ams` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Inputs -->

### Sources
<a href="#sources"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Sources -->
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/gen/lib_ams_gen.cpp](/cpp/gen/lib_ams_gen.cpp)||
|[cpp/lib_ams/expect.cpp](/cpp/lib_ams/expect.cpp)|Expect implementation|
|[cpp/lib_ams/file.cpp](/cpp/lib_ams/file.cpp)||
|[cpp/lib_ams/stream.cpp](/cpp/lib_ams/stream.cpp)||
|[include/gen/lib_ams_gen.h](/include/gen/lib_ams_gen.h)||
|[include/gen/lib_ams_gen.inl.h](/include/gen/lib_ams_gen.inl.h)||
|[include/lib_ams.h](/include/lib_ams.h)||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Sources -->

### Dependencies
<a href="#dependencies"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Dependencies -->
The build target depends on the following libraries
|Target|Comment|
|---|---|
|[algo_lib](/txt/lib/algo_lib/README.md)|Support library for all executables|
|[lib_prot](/txt/lib/lib_prot/README.md)|Library covering all protocols|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Dependencies -->

### In Memory DB
<a href="#in-memory-db"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Imdb -->
`lib_ams` generated code creates the tables below.
All allocations are done through global `lib_ams::_db` [lib_ams.FDb](#lib_ams-fdb) structure
|Ctype|Ssimfile|Create|Access|
|---|---|---|---|
|[lib_ams.FDb](#lib_ams-fdb)||FDb._db (Global)|
|[lib_ams.FFdin](#lib_ams-ffdin)||FDb.fdin (Lary)|fdin (Lary, by rowid)|cd_fdin_eof (Llist)|cd_fdin_read (Llist)|
|[lib_ams.FMember](#lib_ams-fmember)||FDb.member (Tpool)|ind_member (Thash, hash field member)|
||||FProc.zd_member_byproc (Llist)|
||||FStream.zd_member_bystream (Llist)|
|[lib_ams.FProc](#lib_ams-fproc)||FDb.proc (Tpool)|ind_proc (Thash, hash field proc_id)|zd_proc (Llist)|
||||FMember.p_proc (Upptr)|
|[lib_ams.FReadfile](#lib_ams-freadfile)||
|[lib_ams.FStream](#lib_ams-fstream)||FDb.stream (Lary)|stream (Lary, by rowid)|ind_stream (Thash, hash field stream_id)|cd_stream_hb (Llist)|cd_poll_read (Llist)|zd_ctlin (Llist)|c_stream_ctl (Ptr)|c_stream_out (Ptr)|
||||FMember.p_stream (Upptr)|
|[lib_ams.FStreamType](#lib_ams-fstreamtype)|[amsdb.streamtype](/txt/ssimdb/amsdb/streamtype.md)|FDb.streamtype (Lary)|**static**|streamtype (Lary, by rowid)|ind_streamtype (Thash, hash field id)|
||||FStream.p_streamtype (Upptr)|
|[lib_ams.FWritefile](#lib_ams-fwritefile)||FDb.writefile (Cppstack)|zd_flush (Llist)|

#### lib_ams.FDb - In-memory database for lib_ams
<a href="#lib_ams-fdb"></a>

#### lib_ams.FDb Fields
<a href="#lib_ams-fdb-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_ams.FDb.fdin|[lib_ams.FFdin](/txt/lib/lib_ams/README.md#lib_ams-ffdin)|[Lary](/txt/exe/amc/reftypes.md#lary)||ams control messages on stdin|
|lib_ams.FDb.cd_fdin_eof|[lib_ams.FFdin](/txt/lib/lib_ams/README.md#lib_ams-ffdin)|[Llist](/txt/exe/amc/reftypes.md#llist)||End reading from stdin|
|lib_ams.FDb.cd_fdin_read|[lib_ams.FFdin](/txt/lib/lib_ams/README.md#lib_ams-ffdin)|[Llist](/txt/exe/amc/reftypes.md#llist)||Read next message from file descriptor|
|lib_ams.FDb._db|[lib_ams.FDb](/txt/lib/lib_ams/README.md#lib_ams-fdb)|[Global](/txt/exe/amc/reftypes.md#global)|||
|lib_ams.FDb.writefile|[lib_ams.FWritefile](/txt/lib/lib_ams/README.md#lib_ams-fwritefile)|[Cppstack](/txt/exe/amc/reftypes.md#cppstack)||Provides default name for variables of this type|
|lib_ams.FDb.zd_flush|[lib_ams.FWritefile](/txt/lib/lib_ams/README.md#lib_ams-fwritefile)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|lib_ams.FDb.stream|[lib_ams.FStream](/txt/lib/lib_ams/README.md#lib_ams-fstream)|[Lary](/txt/exe/amc/reftypes.md#lary)||Table of streams|
|lib_ams.FDb.ind_stream|[lib_ams.FStream](/txt/lib/lib_ams/README.md#lib_ams-fstream)|[Thash](/txt/exe/amc/reftypes.md#thash)||Index of streams by stream id|
|lib_ams.FDb.cd_stream_hb|[lib_ams.FStream](/txt/lib/lib_ams/README.md#lib_ams-fstream)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|lib_ams.FDb.proc_id|[ams.ProcId](/txt/protocol/ams/ProcId.md)|[Val](/txt/exe/amc/reftypes.md#val)||Process id, e.g. amstest-0|
|lib_ams.FDb.shmem_size|i32|[Val](/txt/exe/amc/reftypes.md#val)|32768|Default stream shared memory size|
|lib_ams.FDb.max_msg_size|i32|[Val](/txt/exe/amc/reftypes.md#val)|4096|Maximum message size|
|lib_ams.FDb.cd_poll_read|[lib_ams.FStream](/txt/lib/lib_ams/README.md#lib_ams-fstream)|[Llist](/txt/exe/amc/reftypes.md#llist)||Hot poll input stream for reading|
|lib_ams.FDb.file_prefix|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|File prefix for all streams|
|lib_ams.FDb.proc|[lib_ams.FProc](/txt/lib/lib_ams/README.md#lib_ams-fproc)|[Tpool](/txt/exe/amc/reftypes.md#tpool)|||
|lib_ams.FDb.ind_proc|[lib_ams.FProc](/txt/lib/lib_ams/README.md#lib_ams-fproc)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|lib_ams.FDb.member|[lib_ams.FMember](/txt/lib/lib_ams/README.md#lib_ams-fmember)|[Tpool](/txt/exe/amc/reftypes.md#tpool)|||
|lib_ams.FDb.ind_member|[lib_ams.FMember](/txt/lib/lib_ams/README.md#lib_ams-fmember)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|lib_ams.FDb.streamtype|[lib_ams.FStreamType](/txt/lib/lib_ams/README.md#lib_ams-fstreamtype)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|lib_ams.FDb.ind_streamtype|[lib_ams.FStreamType](/txt/lib/lib_ams/README.md#lib_ams-fstreamtype)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|lib_ams.FDb.zd_ctlin|[lib_ams.FStream](/txt/lib/lib_ams/README.md#lib_ams-fstream)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|lib_ams.FDb.stream_files_cleaned|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_ams.FDb.shmem_mode|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_ams.FDb.fdin_buf|[algo.ByteAry](/txt/protocol/algo/README.md#algo-byteary)|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_ams.FDb.dflt_stream_id|[ams.StreamId](/txt/protocol/ams/StreamId.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_ams.FDb.zd_proc|[lib_ams.FProc](/txt/lib/lib_ams/README.md#lib_ams-fproc)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|lib_ams.FDb.c_stream_ctl|[lib_ams.FStream](/txt/lib/lib_ams/README.md#lib_ams-fstream)|[Ptr](/txt/exe/amc/reftypes.md#ptr)||Ctl output stream (process events)|
|lib_ams.FDb.expect_buf|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Buffer containing outputs produced since last input|
|lib_ams.FDb.expect_str|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||If non-empty, pause reading inputs until this string is non-empty|
|lib_ams.FDb.th_expect|[algo_lib.FTimehook](/txt/lib/algo_lib/README.md#algo_lib-ftimehook)|[Val](/txt/exe/amc/reftypes.md#val)||Timeout for expect message|
|lib_ams.FDb.expect_pos|i32|[Val](/txt/exe/amc/reftypes.md#val)||Match position within expect buf|
|lib_ams.FDb.expect_timeout|double|[Val](/txt/exe/amc/reftypes.md#val)|10.0|Default expect timeout|
|lib_ams.FDb.fmt_buf|[algo.ByteAry](/txt/protocol/algo/README.md#algo-byteary)|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_ams.FDb.regx_trace|[algo.cstring](/txt/protocol/algo/cstring.md)|[RegxSql](/txt/exe/amc/reftypes.md#regxsql)||Regx of streams to trace|
|lib_ams.FDb.c_stream_out|[lib_ams.FStream](/txt/lib/lib_ams/README.md#lib_ams-fstream)|[Ptr](/txt/exe/amc/reftypes.md#ptr)||Output stream|

#### Struct FDb
<a href="#struct-fdb"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_ams_gen.h](/include/gen/lib_ams_gen.h)
```
struct FDb { // lib_ams.FDb: In-memory database for lib_ams
    lib_ams::FFdin*          fdin_lary[32];                  // level array
    i32                      fdin_n;                         // number of elements in array
    lib_ams::FFdin*          cd_fdin_eof_head;               // zero-terminated doubly linked list
    i32                      cd_fdin_eof_n;                  // zero-terminated doubly linked list
    lib_ams::FFdin*          cd_fdin_read_head;              // zero-terminated doubly linked list
    i32                      cd_fdin_read_n;                 // zero-terminated doubly linked list
    lib_ams::FWritefile*     zd_flush_head;                  // zero-terminated doubly linked list
    i32                      zd_flush_n;                     // zero-terminated doubly linked list
    lib_ams::FWritefile*     zd_flush_tail;                  // pointer to last element
    algo::SchedTime          zd_flush_next;                  // lib_ams.FDb.zd_flush                                               Next invocation time
    algo::SchedTime          zd_flush_delay;                 // lib_ams.FDb.zd_flush                                               Delay between invocations
    lib_ams::FStream*        stream_lary[32];                // level array
    i32                      stream_n;                       // number of elements in array
    lib_ams::FStream**       ind_stream_buckets_elems;       // pointer to bucket array
    i32                      ind_stream_buckets_n;           // number of elements in bucket array
    i32                      ind_stream_n;                   // number of elements in the hash table
    lib_ams::FStream*        cd_stream_hb_head;              // zero-terminated doubly linked list
    i32                      cd_stream_hb_n;                 // zero-terminated doubly linked list
    algo::SchedTime          cd_stream_hb_next;              // lib_ams.FDb.cd_stream_hb                                           Next invocation time
    algo::SchedTime          cd_stream_hb_delay;             // lib_ams.FDb.cd_stream_hb                                           Delay between invocations
    ams::ProcId              proc_id;                        // Process id, e.g. amstest-0
    i32                      shmem_size;                     //   32768  Default stream shared memory size
    i32                      max_msg_size;                   //   4096  Maximum message size
    lib_ams::FStream*        cd_poll_read_head;              // zero-terminated doubly linked list
    i32                      cd_poll_read_n;                 // zero-terminated doubly linked list
    algo::cstring            file_prefix;                    //   ""  File prefix for all streams
    u64                      proc_blocksize;                 // # bytes per block
    lib_ams::FProc*          proc_free;                      //
    lib_ams::FProc**         ind_proc_buckets_elems;         // pointer to bucket array
    i32                      ind_proc_buckets_n;             // number of elements in bucket array
    i32                      ind_proc_n;                     // number of elements in the hash table
    u64                      member_blocksize;               // # bytes per block
    lib_ams::FMember*        member_free;                    //
    lib_ams::FMember**       ind_member_buckets_elems;       // pointer to bucket array
    i32                      ind_member_buckets_n;           // number of elements in bucket array
    i32                      ind_member_n;                   // number of elements in the hash table
    lib_ams::FStreamType*    streamtype_lary[32];            // level array
    i32                      streamtype_n;                   // number of elements in array
    lib_ams::FStreamType**   ind_streamtype_buckets_elems;   // pointer to bucket array
    i32                      ind_streamtype_buckets_n;       // number of elements in bucket array
    i32                      ind_streamtype_n;               // number of elements in the hash table
    lib_ams::FStream*        zd_ctlin_head;                  // zero-terminated doubly linked list
    i32                      zd_ctlin_n;                     // zero-terminated doubly linked list
    lib_ams::FStream*        zd_ctlin_tail;                  // pointer to last element
    bool                     stream_files_cleaned;           //   false
    bool                     shmem_mode;                     //   false
    algo::ByteAry            fdin_buf;                       //
    ams::StreamId            dflt_stream_id;                 //
    lib_ams::FProc*          zd_proc_head;                   // zero-terminated doubly linked list
    i32                      zd_proc_n;                      // zero-terminated doubly linked list
    lib_ams::FProc*          zd_proc_tail;                   // pointer to last element
    lib_ams::FStream*        c_stream_ctl;                   // Ctl output stream (process events). optional pointer
    algo::cstring            expect_buf;                     // Buffer containing outputs produced since last input
    algo::cstring            expect_str;                     // If non-empty, pause reading inputs until this string is non-empty
    algo_lib::FTimehook      th_expect;                      // Timeout for expect message
    i32                      expect_pos;                     //   0  Match position within expect buf
    double                   expect_timeout;                 //   10.0  Default expect timeout
    algo::ByteAry            fmt_buf;                        //
    algo_lib::Regx           regx_trace;                     // Sql Regx
    lib_ams::FStream*        c_stream_out;                   // Output stream. optional pointer
    lib_ams::trace           trace;                          //
};
```

#### lib_ams.FFdin - FD input (normally stdin)
<a href="#lib_ams-ffdin"></a>

#### lib_ams.FFdin Fields
<a href="#lib_ams-ffdin-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_ams.FFdin.iohook|[algo_lib.FIohook](/txt/lib/algo_lib/README.md#algo_lib-fiohook)|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_ams.FFdin.in|char|[Fbuf](/txt/exe/amc/reftypes.md#fbuf)|'\n'||

#### Struct FFdin
<a href="#struct-ffdin"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_ams_gen.h](/include/gen/lib_ams_gen.h)
```
struct FFdin { // lib_ams.FFdin: FD input (normally stdin)
    lib_ams::FFdin*     cd_fdin_eof_next;    // zslist link; -1 means not-in-list
    lib_ams::FFdin*     cd_fdin_eof_prev;    // previous element
    lib_ams::FFdin*     cd_fdin_read_next;   // zslist link; -1 means not-in-list
    lib_ams::FFdin*     cd_fdin_read_prev;   // previous element
    algo_lib::FIohook   iohook;              //
    u8                  in_elems[8192];      // pointer to elements of inline array
    i32                 in_start;            // beginning of valid bytes (in bytes)
    i32                 in_end;              // end of valid bytes (in bytes)
    bool                in_eof;              // no more data will be written to buffer
    algo::Errcode       in_err;              // system error code
    bool                in_msgvalid;         // current message is valid
    i32                 in_msglen;           // current message length
    algo_lib::FIohook   in_iohook;           // edge-triggered hook for refilling buffer
    bool                in_epoll_enable;     // use epoll?
    enum { in_max = 8192 };
    // value field lib_ams.FFdin.iohook is not copiable
    // field lib_ams.FFdin.in prevents copy
    // func:lib_ams.FFdin..AssignOp
    inline lib_ams::FFdin& operator =(const lib_ams::FFdin &rhs) = delete;
    // value field lib_ams.FFdin.iohook is not copiable
    // field lib_ams.FFdin.in prevents copy
    // func:lib_ams.FFdin..CopyCtor
    inline               FFdin(const lib_ams::FFdin &rhs) = delete;
private:
    // func:lib_ams.FFdin..Ctor
    inline               FFdin() __attribute__((nothrow));
    // func:lib_ams.FFdin..Dtor
    inline               ~FFdin() __attribute__((nothrow));
    friend lib_ams::FFdin&      fdin_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend lib_ams::FFdin*      fdin_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 fdin_RemoveAll() __attribute__((nothrow));
    friend void                 fdin_RemoveLast() __attribute__((nothrow));
};
```

#### lib_ams.FMember - 
<a href="#lib_ams-fmember"></a>

#### lib_ams.FMember Fields
<a href="#lib_ams-fmember-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_ams.FMember.member|[ams.Member](/txt/protocol/ams/Member.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_ams.FMember.pos|[ams.StreamPos](/txt/protocol/ams/StreamPos.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_ams.FMember.last_hb|[algo.UnTime](/txt/protocol/algo/UnTime.md)|[Val](/txt/exe/amc/reftypes.md#val)||Time last heartbeat received|
|lib_ams.FMember.budget|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_ams.FMember.p_stream|[lib_ams.FStream](/txt/lib/lib_ams/README.md#lib_ams-fstream)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|lib_ams.FMember.p_proc|[lib_ams.FProc](/txt/lib/lib_ams/README.md#lib_ams-fproc)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FMember
<a href="#struct-fmember"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_ams_gen.h](/include/gen/lib_ams_gen.h)
```
struct FMember { // lib_ams.FMember
    lib_ams::FMember*   member_next;               // Pointer to next free element int tpool
    lib_ams::FMember*   ind_member_next;           // hash next
    ams::Member         member;                    //
    ams::StreamPos      pos;                       //
    algo::UnTime        last_hb;                   // Time last heartbeat received
    u32                 budget;                    //   0
    lib_ams::FStream*   p_stream;                  // reference to parent row
    lib_ams::FProc*     p_proc;                    // reference to parent row
    lib_ams::FMember*   zd_member_byproc_next;     // zslist link; -1 means not-in-list
    lib_ams::FMember*   zd_member_byproc_prev;     // previous element
    lib_ams::FMember*   zd_member_bystream_next;   // zslist link; -1 means not-in-list
    lib_ams::FMember*   zd_member_bystream_prev;   // previous element
    // x-reference on lib_ams.FMember.p_stream prevents copy
    // x-reference on lib_ams.FMember.p_proc prevents copy
    // func:lib_ams.FMember..AssignOp
    inline lib_ams::FMember& operator =(const lib_ams::FMember &rhs) = delete;
    // x-reference on lib_ams.FMember.p_stream prevents copy
    // x-reference on lib_ams.FMember.p_proc prevents copy
    // func:lib_ams.FMember..CopyCtor
    inline               FMember(const lib_ams::FMember &rhs) = delete;
private:
    // func:lib_ams.FMember..Ctor
    inline               FMember() __attribute__((nothrow));
    // func:lib_ams.FMember..Dtor
    inline               ~FMember() __attribute__((nothrow));
    friend lib_ams::FMember&    member_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend lib_ams::FMember*    member_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 member_Delete(lib_ams::FMember &row) __attribute__((nothrow));
};
```

#### lib_ams.FProc - 
<a href="#lib_ams-fproc"></a>

#### lib_ams.FProc Fields
<a href="#lib_ams-fproc-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_ams.FProc.proc_id|[ams.ProcId](/txt/protocol/ams/ProcId.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_ams.FProc.zd_member_byproc|[lib_ams.FMember](/txt/lib/lib_ams/README.md#lib_ams-fmember)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|lib_ams.FProc.pid|i32|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_ams.FProc.status|i32|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_ams.FProc.logbuf|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_ams.FProc.hbtimeout|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_ams.FProc.critical|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_ams.FProc.realtime|bool|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FProc
<a href="#struct-fproc"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_ams_gen.h](/include/gen/lib_ams_gen.h)
```
struct FProc { // lib_ams.FProc
    lib_ams::FProc*     proc_next;               // Pointer to next free element int tpool
    lib_ams::FProc*     ind_proc_next;           // hash next
    lib_ams::FProc*     zd_proc_next;            // zslist link; -1 means not-in-list
    lib_ams::FProc*     zd_proc_prev;            // previous element
    ams::ProcId         proc_id;                 //
    lib_ams::FMember*   zd_member_byproc_head;   // zero-terminated doubly linked list
    i32                 zd_member_byproc_n;      // zero-terminated doubly linked list
    lib_ams::FMember*   zd_member_byproc_tail;   // pointer to last element
    i32                 pid;                     //   0
    i32                 status;                  //   0
    algo::cstring       logbuf;                  //
    u32                 hbtimeout;               //   0
    bool                critical;                //   false
    bool                realtime;                //   false
    // reftype Llist of lib_ams.FProc.zd_member_byproc prohibits copy
    // func:lib_ams.FProc..AssignOp
    inline lib_ams::FProc& operator =(const lib_ams::FProc &rhs) = delete;
    // reftype Llist of lib_ams.FProc.zd_member_byproc prohibits copy
    // func:lib_ams.FProc..CopyCtor
    inline               FProc(const lib_ams::FProc &rhs) = delete;
private:
    // func:lib_ams.FProc..Ctor
    inline               FProc() __attribute__((nothrow));
    // func:lib_ams.FProc..Dtor
    inline               ~FProc() __attribute__((nothrow));
    friend lib_ams::FProc&      proc_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend lib_ams::FProc*      proc_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 proc_Delete(lib_ams::FProc &row) __attribute__((nothrow));
};
```

#### lib_ams.FReadfile - 
<a href="#lib_ams-freadfile"></a>

#### lib_ams.FReadfile Fields
<a href="#lib_ams-freadfile-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_ams.FReadfile.filename|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_ams.FReadfile.fd|[algo.Fildes](/txt/protocol/algo/Fildes.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_ams.FReadfile.eof|bool|[Val](/txt/exe/amc/reftypes.md#val)||EOF flag|
|lib_ams.FReadfile.fail|bool|[Val](/txt/exe/amc/reftypes.md#val)||Failure flag|
|lib_ams.FReadfile.buf|u8|[Tary](/txt/exe/amc/reftypes.md#tary)||Message buffer|
|lib_ams.FReadfile.cbuf|u8|[Tary](/txt/exe/amc/reftypes.md#tary)||Compressed buffer|
|lib_ams.FReadfile.block|[ams.MsgBlock](/txt/protocol/ams/MsgBlock.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_ams.FReadfile.offset|u32|[Tary](/txt/exe/amc/reftypes.md#tary)|||

#### Struct FReadfile
<a href="#struct-freadfile"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_ams_gen.h](/include/gen/lib_ams_gen.h)
```
struct FReadfile { // lib_ams.FReadfile
    algo::cstring   filename;       //
    algo::Fildes    fd;             //
    bool            eof;            //   false  EOF flag
    bool            fail;           //   false  Failure flag
    u8*             buf_elems;      // pointer to elements
    u32             buf_n;          // number of elements in array
    u32             buf_max;        // max. capacity of array before realloc
    u8*             cbuf_elems;     // pointer to elements
    u32             cbuf_n;         // number of elements in array
    u32             cbuf_max;       // max. capacity of array before realloc
    ams::MsgBlock   block;          //
    u32*            offset_elems;   // pointer to elements
    u32             offset_n;       // number of elements in array
    u32             offset_max;     // max. capacity of array before realloc
    // func:lib_ams.FReadfile..AssignOp
    lib_ams::FReadfile&  operator =(const lib_ams::FReadfile &rhs) __attribute__((nothrow));
    // func:lib_ams.FReadfile..Ctor
    inline               FReadfile() __attribute__((nothrow));
    // func:lib_ams.FReadfile..Dtor
    inline               ~FReadfile() __attribute__((nothrow));
    // func:lib_ams.FReadfile..CopyCtor
    FReadfile(const lib_ams::FReadfile &rhs) __attribute__((nothrow));
};
```

#### lib_ams.FStream - Stream record
<a href="#lib_ams-fstream"></a>

#### lib_ams.FStream Fields
<a href="#lib_ams-fstream-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_ams.FStream.stream_id|[ams.StreamId](/txt/protocol/ams/StreamId.md)|[Val](/txt/exe/amc/reftypes.md#val)||Stream ID (primary key)|
|lib_ams.FStream.filename|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Name of the file (if opened)|
|lib_ams.FStream.shm_file|[algo_lib.FFildes](/txt/lib/algo_lib/README.md#algo_lib-ffildes)|[Val](/txt/exe/amc/reftypes.md#val)||Associated file|
|lib_ams.FStream.flags|[ams.StreamFlags](/txt/protocol/ams/StreamFlags.md)|[Val](/txt/exe/amc/reftypes.md#val)||Is stream open for writing|
|lib_ams.FStream.byteary|[algo.ByteAry](/txt/protocol/algo/README.md#algo-byteary)|[Val](/txt/exe/amc/reftypes.md#val)||Substitute for shm_region|
|lib_ams.FStream.shm_handle|u8|[Ptr](/txt/exe/amc/reftypes.md#ptr)||Associated memory segment handle (windows only)|
|lib_ams.FStream.rpos|[ams.StreamPos](/txt/protocol/ams/StreamPos.md)|[Val](/txt/exe/amc/reftypes.md#val)||Read position|
|lib_ams.FStream.wpos|[ams.StreamPos](/txt/protocol/ams/StreamPos.md)|[Val](/txt/exe/amc/reftypes.md#val)||Write position|
|lib_ams.FStream.shm_region|[algo.memptr](/txt/protocol/algo/memptr.md)|[Val](/txt/exe/amc/reftypes.md#val)||Shared memory region|
|lib_ams.FStream.limit|u64|[Val](/txt/exe/amc/reftypes.md#val)||Updated with ackoff, limit for nextoff|
|lib_ams.FStream.offset_mask|u64|[Val](/txt/exe/amc/reftypes.md#val)||Mask for byte eof|
|lib_ams.FStream.cur_msg|[ams.Seqmsg](/txt/protocol/ams/Seqmsg.md)|[Ptr](/txt/exe/amc/reftypes.md#ptr)||Current message|
|lib_ams.FStream.h_msg|[ams.MsgHeader](/txt/protocol/ams/MsgHeader.md)|[Hook](/txt/exe/amc/reftypes.md#hook)||Message processing hook|
|lib_ams.FStream.h_msg_orig|[ams.MsgHeader](/txt/protocol/ams/MsgHeader.md)|[Hook](/txt/exe/amc/reftypes.md#hook)||Saved message processing hook (EXPECT)|
|lib_ams.FStream.zd_member_bystream|[lib_ams.FMember](/txt/lib/lib_ams/README.md#lib_ams-fmember)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|lib_ams.FStream.next_ackoff|u64|[Val](/txt/exe/amc/reftypes.md#val)|1023|Force StreamHb after reaching this offset|
|lib_ams.FStream.p_streamtype|[lib_ams.FStreamType](/txt/lib/lib_ams/README.md#lib_ams-fstreamtype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|lib_ams.FStream.n_write_block|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_ams.FStream.burst|i32|[Val](/txt/exe/amc/reftypes.md#val)|20|Max number of messages processed at once|
|lib_ams.FStream.writer_error|bool|[Val](/txt/exe/amc/reftypes.md#val)||Writer error was detected|

#### Struct FStream
<a href="#struct-fstream"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_ams_gen.h](/include/gen/lib_ams_gen.h)
```
struct FStream { // lib_ams.FStream: Stream record
    lib_ams::FStream*                 ind_stream_next;           // hash next
    lib_ams::FStream*                 cd_stream_hb_next;         // zslist link; -1 means not-in-list
    lib_ams::FStream*                 cd_stream_hb_prev;         // previous element
    lib_ams::FStream*                 cd_poll_read_next;         // zslist link; -1 means not-in-list
    lib_ams::FStream*                 cd_poll_read_prev;         // previous element
    lib_ams::FStream*                 zd_ctlin_next;             // zslist link; -1 means not-in-list
    lib_ams::FStream*                 zd_ctlin_prev;             // previous element
    ams::StreamId                     stream_id;                 // Stream ID (primary key)
    algo::cstring                     filename;                  // Name of the file (if opened)
    algo_lib::FFildes                 shm_file;                  // Associated file
    ams::StreamFlags                  flags;                     // Is stream open for writing
    algo::ByteAry                     byteary;                   // Substitute for shm_region
    u8*                               shm_handle;                // Associated memory segment handle (windows only). optional pointer
    ams::StreamPos                    rpos;                      // Read position
    ams::StreamPos                    wpos;                      // Write position
    algo::memptr                      shm_region;                // Shared memory region
    u64                               limit;                     //   0  Updated with ackoff, limit for nextoff
    u64                               offset_mask;               //   0  Mask for byte eof
    ams::Seqmsg*                      cur_msg;                   // Current message. optional pointer
    lib_ams::stream_h_msg_hook        h_msg;                     //   NULL  Pointer to a function
    u64                               h_msg_ctx;                 //   0  Callback context
    lib_ams::stream_h_msg_orig_hook   h_msg_orig;                //   NULL  Pointer to a function
    u64                               h_msg_orig_ctx;            //   0  Callback context
    lib_ams::FMember*                 zd_member_bystream_head;   // zero-terminated doubly linked list
    i32                               zd_member_bystream_n;      // zero-terminated doubly linked list
    lib_ams::FMember*                 zd_member_bystream_tail;   // pointer to last element
    u64                               next_ackoff;               //   1023  Force StreamHb after reaching this offset
    lib_ams::FStreamType*             p_streamtype;              // reference to parent row
    u32                               n_write_block;             //   0
    i32                               burst;                     //   20  Max number of messages processed at once
    bool                              writer_error;              //   false  Writer error was detected
    // user-defined fcleanup on lib_ams.FStream.shm_handle prevents copy
    // reftype Hook of lib_ams.FStream.h_msg prohibits copy
    // reftype Hook of lib_ams.FStream.h_msg_orig prohibits copy
    // reftype Llist of lib_ams.FStream.zd_member_bystream prohibits copy
    // x-reference on lib_ams.FStream.p_streamtype prevents copy
    // func:lib_ams.FStream..AssignOp
    lib_ams::FStream&    operator =(const lib_ams::FStream &rhs) = delete;
    // user-defined fcleanup on lib_ams.FStream.shm_handle prevents copy
    // reftype Hook of lib_ams.FStream.h_msg prohibits copy
    // reftype Hook of lib_ams.FStream.h_msg_orig prohibits copy
    // reftype Llist of lib_ams.FStream.zd_member_bystream prohibits copy
    // x-reference on lib_ams.FStream.p_streamtype prevents copy
    // func:lib_ams.FStream..CopyCtor
    FStream(const lib_ams::FStream &rhs) = delete;
private:
    // func:lib_ams.FStream..Ctor
    inline               FStream() __attribute__((nothrow));
    // func:lib_ams.FStream..Dtor
    inline               ~FStream() __attribute__((nothrow));
    friend lib_ams::FStream&    stream_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend lib_ams::FStream*    stream_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 stream_RemoveAll() __attribute__((nothrow));
    friend void                 stream_RemoveLast() __attribute__((nothrow));
};
```

#### lib_ams.FStreamType - 
<a href="#lib_ams-fstreamtype"></a>

#### lib_ams.FStreamType Fields
<a href="#lib_ams-fstreamtype-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_ams.FStreamType.base|[amsdb.StreamType](/txt/ssimdb/amsdb/streamtype.md)|[Base](/txt/ssimdb/amsdb/streamtype.md)|||

#### Struct FStreamType
<a href="#struct-fstreamtype"></a>
*Note:* field ``lib_ams.FStreamType.base`` has reftype ``base`` so the fields of [amsdb.StreamType](/txt/ssimdb/amsdb/streamtype.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_ams_gen.h](/include/gen/lib_ams_gen.h)
```
struct FStreamType { // lib_ams.FStreamType
    lib_ams::FStreamType*   ind_streamtype_next;   // hash next
    algo::Smallstr50        streamtype;            //
    ams::StreamType         id;                    //
    algo::Comment           comment;               //
    // func:lib_ams.FStreamType..AssignOp
    inline lib_ams::FStreamType& operator =(const lib_ams::FStreamType &rhs) = delete;
    // func:lib_ams.FStreamType..CopyCtor
    inline               FStreamType(const lib_ams::FStreamType &rhs) = delete;
private:
    // func:lib_ams.FStreamType..Ctor
    inline               FStreamType() __attribute__((nothrow));
    // func:lib_ams.FStreamType..Dtor
    inline               ~FStreamType() __attribute__((nothrow));
    friend lib_ams::FStreamType& streamtype_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend lib_ams::FStreamType* streamtype_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 streamtype_RemoveAll() __attribute__((nothrow));
    friend void                 streamtype_RemoveLast() __attribute__((nothrow));
};
```

#### lib_ams.FWritefile - 
<a href="#lib_ams-fwritefile"></a>

#### lib_ams.FWritefile Fields
<a href="#lib_ams-fwritefile-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_ams.FWritefile.filename|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_ams.FWritefile.fd|[algo.Fildes](/txt/protocol/algo/Fildes.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_ams.FWritefile.fail|bool|[Val](/txt/exe/amc/reftypes.md#val)||Failure flag|
|lib_ams.FWritefile.buf|u8|[Tary](/txt/exe/amc/reftypes.md#tary)||Message buffer|
|lib_ams.FWritefile.cbuf|u8|[Tary](/txt/exe/amc/reftypes.md#tary)||Compressed buffer|
|lib_ams.FWritefile.buf_thr|u32|[Val](/txt/exe/amc/reftypes.md#val)|1024*64|Buffer write threshold|
|lib_ams.FWritefile.block|[ams.MsgBlock](/txt/protocol/ams/MsgBlock.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FWritefile
<a href="#struct-fwritefile"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_ams_gen.h](/include/gen/lib_ams_gen.h)
```
struct FWritefile { // lib_ams.FWritefile
    lib_ams::FWritefile*   zd_flush_next;   // zslist link; -1 means not-in-list
    lib_ams::FWritefile*   zd_flush_prev;   // previous element
    algo::cstring          filename;        //
    algo::Fildes           fd;              //
    bool                   fail;            //   false  Failure flag
    u8*                    buf_elems;       // pointer to elements
    u32                    buf_n;           // number of elements in array
    u32                    buf_max;         // max. capacity of array before realloc
    u8*                    cbuf_elems;      // pointer to elements
    u32                    cbuf_n;          // number of elements in array
    u32                    cbuf_max;        // max. capacity of array before realloc
    u32                    buf_thr;         //   1024*64  Buffer write threshold
    ams::MsgBlock          block;           //
    // func:lib_ams.FWritefile..AssignOp
    lib_ams::FWritefile& operator =(const lib_ams::FWritefile &rhs) = delete;
    // func:lib_ams.FWritefile..Ctor
    inline               FWritefile() __attribute__((nothrow));
    // func:lib_ams.FWritefile..Dtor
    inline               ~FWritefile() __attribute__((nothrow));
    // func:lib_ams.FWritefile..CopyCtor
    FWritefile(const lib_ams::FWritefile &rhs) = delete;
};
```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Imdb -->


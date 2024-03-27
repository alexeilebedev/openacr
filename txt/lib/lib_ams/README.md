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
PROCTYPE  ID  COMMENT
0         0   No process
amstest   5   Ams testing

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
* [AMS Library](#ams-library)
* [List of AMS Messages](#list-of-ams-messages)
* [`ams.ProcAdd`: Create process](#-ams-procadd--create-process)
* [`ams.ProcRemove`: Destroy process](#-ams-procremove--destroy-process)
* [`ams.Seqmsg`: A sequenced message](#-ams-seqmsg--a-sequenced-message)
* [`ams.OpenMsg`: Open stream for reading/writing](#-ams-openmsg--open-stream-for-reading-writing)
* [`ams.StreamHb`: Stream heartbeat](#-ams-streamhb--stream-heartbeat)
* [In-memory stream format; Flow control](#in-memory-stream-format--flow-control)
* [Performance](#performance)
* [AMS Stream Tracing](#ams-stream-tracing)
* [Ctypes](#ctypes)
* [Functions](#functions)
* [Sources](#sources)
* [Inputs](#inputs)

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
      dmmeta.ctypelen  ctype:ams.ProcAddMsg  len:10  alignment:1  padbytes:0
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
      dmmeta.ctypelen  ctype:ams.ProcRemoveMsg  len:10  alignment:1  padbytes:0
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
      dmmeta.ctypelen  ctype:ams.Seqmsg  len:32  alignment:1  padbytes:0
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
      dmmeta.ctypelen  ctype:ams.OpenMsg  len:31  alignment:1  padbytes:0
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
      dmmeta.ctypelen  ctype:ams.StreamHbMsg  len:35  alignment:1  padbytes:0
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

### Ctypes
<a href="#ctypes"></a>
Other ctypes in this namespace which don't have own readme files

#### lib_ams.FDb - In-memory database for lib_ams
<a href="#lib_ams-fdb"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|fdin|lib_ams.FFdin|Lary||ams control messages on stdin|
|cd_fdin_eof|lib_ams.FFdin|Llist||End reading from stdin|
|cd_fdin_read|lib_ams.FFdin|Llist||Read next message from file descriptor|
|_db|lib_ams.FDb|Global|
|writefile|lib_ams.FWritefile|Cppstack||Provides default name for variables of this type|
|zd_flush|lib_ams.FWritefile|Llist|
|stream|lib_ams.FStream|Lary||Table of streams|
|ind_stream|lib_ams.FStream|Thash||Index of streams by stream id|
|cd_stream_hb|lib_ams.FStream|Llist|
|proc_id|ams.ProcId|Val||Process id, e.g. amstest-0|
|shmem_size|i32|Val|32768|Default stream shared memory size|
|max_msg_size|i32|Val|4096|Maximum message size|
|cd_poll_read|lib_ams.FStream|Llist||Hot poll input stream for reading|
|file_prefix|algo.cstring|Val|""|File prefix for all streams|
|proc|lib_ams.FProc|Tpool|
|ind_proc|lib_ams.FProc|Thash|
|member|lib_ams.FMember|Tpool|
|ind_member|lib_ams.FMember|Thash|
|streamtype|lib_ams.FStreamType|Lary|
|ind_streamtype|lib_ams.FStreamType|Thash|
|zd_ctlin|lib_ams.FStream|Llist|
|stream_files_cleaned|bool|Val|
|shmem_mode|bool|Val|
|fdin_buf|algo.ByteAry|Val|
|dflt_stream_id|ams.StreamId|Val|
|zd_proc|lib_ams.FProc|Llist|
|c_stream_ctl|lib_ams.FStream|Ptr||Ctl output stream (process events)|
|expect_buf|algo.cstring|Val||Buffer containing outputs produced since last input|
|expect_str|algo.cstring|Val||If non-empty, pause reading inputs until this string is non-empty|
|th_expect|algo_lib.FTimehook|Val||Timeout for expect message|
|expect_pos|i32|Val||Match position within expect buf|
|expect_timeout|double|Val|10.0|Default expect timeout|
|fmt_buf|algo.ByteAry|Val|
|regx_trace|algo.cstring|RegxSql||Regx of streams to trace|
|c_stream_out|lib_ams.FStream|Ptr||Output stream|

#### lib_ams.FFdin - FD input (normally stdin)
<a href="#lib_ams-ffdin"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|iohook|algo_lib.FIohook|Val|
|in|char|Fbuf|'\n'|

#### lib_ams.FMember - 
<a href="#lib_ams-fmember"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|member|ams.Member|Val|
|pos|ams.StreamPos|Val|
|last_hb|algo.UnTime|Val||Time last heartbeat received|
|budget|u32|Val|
|p_stream|lib_ams.FStream|Upptr|
|p_proc|lib_ams.FProc|Upptr|

#### lib_ams.FProc - 
<a href="#lib_ams-fproc"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|proc_id|ams.ProcId|Val|
|zd_member_byproc|lib_ams.FMember|Llist|
|pid|i32|Val|
|status|i32|Val|
|logbuf|algo.cstring|Val|
|hbtimeout|u32|Val|
|critical|bool|Val|
|realtime|bool|Val|

#### lib_ams.FReadfile - 
<a href="#lib_ams-freadfile"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|filename|algo.cstring|Val|
|fd|algo.Fildes|Val|
|eof|bool|Val||EOF flag|
|fail|bool|Val||Failure flag|
|buf|u8|Tary||Message buffer|
|cbuf|u8|Tary||Compressed buffer|
|block|ams.MsgBlock|Val|
|offset|u32|Tary|

#### lib_ams.FStream - Stream record
<a href="#lib_ams-fstream"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|stream_id|ams.StreamId|Val||Stream ID (primary key)|
|filename|algo.cstring|Val||Name of the file (if opened)|
|shm_file|algo_lib.FFildes|Val||Associated file|
|flags|ams.StreamFlags|Val||Is stream open for writing|
|byteary|algo.ByteAry|Val||Substitute for shm_region|
|shm_handle|u8|Ptr||Associated memory segment handle (windows only)|
|rpos|ams.StreamPos|Val||Read position|
|wpos|ams.StreamPos|Val||Write position|
|shm_region|algo.memptr|Val||Shared memory region|
|limit|u64|Val||Updated with ackoff, limit for nextoff|
|offset_mask|u64|Val||Mask for byte eof|
|cur_msg|ams.Seqmsg|Ptr||Current message|
|h_msg|ams.MsgHeader|Hook||Message processing hook|
|h_msg_orig|ams.MsgHeader|Hook||Saved message processing hook (EXPECT)|
|zd_member_bystream|lib_ams.FMember|Llist|
|next_ackoff|u64|Val|1023|Force StreamHb after reaching this offset|
|p_streamtype|lib_ams.FStreamType|Upptr|
|n_write_block|u32|Val|
|burst|i32|Val|20|Max number of messages processed at once|
|writer_error|bool|Val||Writer error was detected|

#### lib_ams.FStreamType - 
<a href="#lib_ams-fstreamtype"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[amsdb.StreamType](/txt/ssimdb/amsdb/streamtype.md)|Base|

#### lib_ams.FWritefile - 
<a href="#lib_ams-fwritefile"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|filename|algo.cstring|Val|
|fd|algo.Fildes|Val|
|fail|bool|Val||Failure flag|
|buf|u8|Tary||Message buffer|
|cbuf|u8|Tary||Compressed buffer|
|buf_thr|u32|Val|1024*64|Buffer write threshold|
|block|ams.MsgBlock|Val|

### Functions
<a href="#functions"></a>
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
// TODO remove: There is no simple way to do it on Win
// Scan stream/ directory for existing streams
// Create a stream record for each.
void lib_ams::DiscoverStreams() 
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
// (This message could be unnecessary)
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
void lib_ams::DumpStreamTable() 
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

### Sources
<a href="#sources"></a>
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

### Inputs
<a href="#inputs"></a>
`lib_ams` takes the following tables on input:
|ssimfile|comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|


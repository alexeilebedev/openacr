## lib_ams - Library for AMS middleware, supporting file format & messaging
<a href="#lib_ams"></a>

AMS is a shared memory transport for communication between single-threaded processes.
Every process is a single-threaded process.

When invoked with default options, each process reads messages from stdin and writes all of its outputs to stdout.
Some of the inputs (see list of ams messages)
may instruct the process to start reading / writing shared memory and turn off reading of messages from stdin.

AMS communication is based on shms. A shm is a named append-only sequence of messages.
Every shm has only a single writer process, but may have multiple readers.
All shm messages are sequenced and gaps are not allowed.
Writers wait until readers consume data before overwriting old messages.
Shm readers are registered with the control process which watches for liveness and keeps track of overall state.

A shm ID is a 32-bit entity consisting of four 8-bit fields: process type, process id, shm type, and shm index.
- ProcType: 8-bit value, see table proctype
- ProcId: 16-bit value, ProcType + index
- Shmtype: 8-bit value, see table shmtype
- ShmId: proc_id + shm_type + shm_idx

Process types are registered in table `amsdb.proctype`. The standard types are:

```
inline-command: acr proctype | ssimfilt ^ -t
PROCTYPE      ID  NS            COMMENT
              0                 No process
ams_sendtest  5   ams_sendtest  Ams testing

```

Shm types are registered in table `amsdb.shmtype`. The standard types are:

```
inline-command: acr shmtype -report:N
amsdb.shmtype  shmtype:ctl  id:1  nonblock:Y  comment:"AMS control messages"
amsdb.shmtype  shmtype:log  id:9  nonblock:N  comment:"Log messages"
amsdb.shmtype  shmtype:out  id:2  nonblock:N  comment:"Log messages, alarms"
```

Together, shm id is formatted like this: `proctype-index.shmtype-index`.
For instance, `proc1-0.md-0`.

Proc types and shm types can be extended for the given system being implemented.
For every shm that a process reads or writes, it publishes one `ams.ShmHb` message to its `ctl`
output shm. The `ctl` output shm is different from standard output because it is non-deterministic
(contains time-based messages such as heartbeats).

### Table Of Contents
<a href="#table-of-contents"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Toc -->
&nbsp;&nbsp;&bull;&nbsp;  [AMS Library](#ams-library)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [List of AMS Messages](#list-of-ams-messages)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [`ams.Shmmsg`: Shm message frame](#-ams-shmmsg--shm-message-frame)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [`ams.ShmHb`: Shm heartbeat](#-ams-shmhb--shm-heartbeat)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [In-memory shm format; Flow control](#in-memory-shm-format--flow-control)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Performance](#performance)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [AMS Shm Tracing](#ams-shm-tracing)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Functions](#functions)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Sources](#sources)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Dependencies](#dependencies)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [In Memory DB](#in-memory-db)<br/>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Toc -->

## AMS Library
<a href="#ams-library"></a>

Ams logic is implemented in the library `lib_ams`.
Each process knows what to do with certain shms; For instance, each process will direct
disk IO to the io shm if it's configured.

## List of AMS Messages
<a href="#list-of-ams-messages"></a>

All ams messages are derived from `ams.MsgHeader`, which is a 32-bit header with type
and length.

### `ams.Shmmsg`: Shm message frame
<a href="#-ams-shmmsg--shm-message-frame"></a>
All shared memory messages have this header.
Each Shmmsg is identified with `shm_id` and `offset`.

```
inline-command: acr ctype:ams.Shmmsg -t -l
dev.license  license:GPL  comment:""
dmmeta.nstype  nstype:protocol  comment:"Protocol (not a target)"
  dmmeta.ns  ns:ams  nstype:protocol  license:GPL  comment:"Algo Messaging System protocol"
    dmmeta.ctype  ctype:ams.Shmmsg  comment:"Sequenced message on a shm"
      dmmeta.cpptype  ctype:ams.Shmmsg  ctor:Y  dtor:N  cheap_copy:N
      dmmeta.msgtype  ctype:ams.Shmmsg  type:10
      dmmeta.pack  ctype:ams.Shmmsg  comment:""
      dmmeta.field  field:ams.Shmmsg.base     arg:ams.MsgHeader  reftype:Base  dflt:""  comment:""
      dmmeta.field  field:ams.Shmmsg.shm_id   arg:ams.ShmId      reftype:Val  dflt:""  comment:""
      dmmeta.field  field:ams.Shmmsg.off      arg:u64            reftype:Val  dflt:0   comment:""
      dmmeta.field  field:ams.Shmmsg.tstamp   arg:u64            reftype:Val  dflt:0   comment:""
      dmmeta.field  field:ams.Shmmsg.payload  arg:ams.MsgHeader  reftype:Opt  dflt:""  comment:"Actual Payload"
      dmmeta.cfmt  cfmt:ams.Shmmsg.String  printfmt:Tuple  read:Y  print:Y  sep:""  genop:Y  comment:""
      dmmeta.ctypelen  ctype:ams.Shmmsg  len:30  alignment:1  padbytes:0  plaindata:N
report.acr  n_select:14  n_insert:0  n_delete:0  n_ignore:0  n_update:0  n_file_mod:0
```

### `ams.ShmHb`: Shm heartbeat
<a href="#-ams-shmhb--shm-heartbeat"></a>

```
inline-command: acr ctype:ams.ShmHbMsg -t | awk -v RS='' '/dmmeta.ctype/'
dev.license  license:GPL  comment:""
dmmeta.nstype  nstype:protocol  comment:"Protocol (not a target)"
  dmmeta.ns  ns:ams  nstype:protocol  license:GPL  comment:"Algo Messaging System protocol"
    dmmeta.ctype  ctype:ams.ShmHbMsg  comment:"Shm heartbeat"
      dmmeta.cpptype  ctype:ams.ShmHbMsg  ctor:Y  dtor:N  cheap_copy:N
      dmmeta.msgtype  ctype:ams.ShmHbMsg  type:3
      dmmeta.pack  ctype:ams.ShmHbMsg  comment:""
      dmmeta.field  field:ams.ShmHbMsg.base      arg:ams.MsgHeader   reftype:Base  dflt:""  comment:""
      dmmeta.field  field:ams.ShmHbMsg.shmember  arg:ams.ShmemberId  reftype:Val  dflt:""  comment:"Primary key"
      dmmeta.field  field:ams.ShmHbMsg.off       arg:u64             reftype:Val  dflt:""  comment:"Read/write position"
      dmmeta.field  field:ams.ShmHbMsg.wbudget   arg:u32             reftype:Val  dflt:""  comment:"Max. offset for writing"
      dmmeta.cfmt  cfmt:ams.ShmHbMsg.String  printfmt:Tuple  read:Y  print:Y  sep:""  genop:Y  comment:""
      dmmeta.ctypelen  ctype:ams.ShmHbMsg  len:32  alignment:1  padbytes:0  plaindata:Y
```

For each shm being either read or written, each process outputs a ShmHb message.
For a reading process, the message declares how many messages have been read.
For a writing process (`write:Y`), the message declares how many messages have been written
For a process writing to disk, `commit:Y` is specified

## In-memory shm format; Flow control
<a href="#in-memory-shm-format--flow-control"></a>

The contents of a shm can be thought of as a sequence of back-to-back `Seqmsg` messages.
The first message is at offset 0, the second one has offset msg0.length, etc.
The messages are published through a shared memory region of size 2^k.
The region, plus a 4Kb helper region are mapped into memory of both reading and writing process.
A message is written contiguously to memory at offset `offset & ((1<<k)-1)`.
The message is written in such a way that the length field is populated last. Thus the reader just
checks the length field at a known offset to poll for message receive.

Looking at the memory region it's not possible to tell where the next message starts.
One has to follow the progress of the shm to know the offsets.
The heartbeat message written to ctl shm reports a {sequence,offset} pair
that can be used to start reading at that location.
However, randomly reading a shared memory region isn't possible either, as the writer process
could overwrite a region at any time. One has to be registered as a reader first. The writer
will avoid overwriting any memory not read by the reader yet.

Readers are announced to the writer on input, and are added to the table of shms.
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

### AMS Shm Tracing
<a href="#ams-shm-tracing"></a>

All messages read from the AMS bus can be traced with the `ams` logcat.

### Functions
<a href="#functions"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Functions -->
Functions exported from this namespace:

```c++
// Open shm SHM for reading and add it to the list of shms
// which lib_ams scans for messages (i.e. control shms).
// Attach lib_ams message dispatch to the shm
bool lib_ams::AddCtlIn(lib_ams::FShm &shm) 
```

```c++
inline u64 lib_ams::AddOffset(u64 offset, int n) 
```

```c++
// Register PROC_ID as reader of shm SHM_ID starting at offset 0
// This immediately limits the write budget of SHM_ID
// (Subsequent attempt to overwrite data not yet consumed by PROC_ID causes
// either reject or blocking wait)
void lib_ams::AddReadShmember(ams::ShmId shm_id, ams::ProcId proc_id) 
```

```c++
// Begin reading ams control messages from stdin
void lib_ams::BeginReadStdin() 
```

```c++
// Begin writing message of length LENGTH
// Used by WriteMsg and with amc's pnew (acr pnew) for zero-copy sends.
// If successful, return pointer to region of size LENGTH whre the mssage can be written,
// otherwise return NULL.
void *lib_ams::BeginWrite(lib_ams::FShm &shm, int length) 
```

```c++
// Whether can read from file - file is good and no EOF
bool lib_ams::CanReadQ(lib_ams::FReadfile &rf) 
```

```c++
// Scan /dev/shm for stale shm files and delete them
// A file is stale if it's flockable (i.e. no process has locked it for writing)
// and it's at least 15 seconds old
void lib_ams::CleanOldShmFiles() 
```

```c++
// Close read file
void lib_ams::Close(lib_ams::FReadfile &wf) 
```

```c++
// Close shm.
// If shm is opened for writing, remove its file.
// BUG: I think if the shm is opened for reading, the mmap() leaks
void lib_ams::Close(lib_ams::FShm &shm) 
```

```c++
// Close written file
void lib_ams::Close(lib_ams::FWritefile &wf) 
```

```c++
void lib_ams::CloseAllShms() 
```

```c++
// Compress buffer - form full message in cbuf
void lib_ams::Compress(lib_ams::FWritefile &wf) 
```

```c++
void lib_ams::CtlMsg_ShmHbMsg(ams::ShmHbMsg &msg) 
```

```c++
// Print table of shms in lib_ams, using ssim format
// if MEMBER is specified, print member information as well
void lib_ams::DumpShmTableDflt(algo_lib::Regx &regx, bool member) 
```

```c++
// Print table of shms in lib_ams, using a more readable layout
void lib_ams::DumpShmTableVisual(algo_lib::Regx &regx) 
```

```c++
// Finish writing message of length LENGTH,
// and send the result.
void lib_ams::EndWrite(lib_ams::FShm &shm, void *ptr, int len) 
```

```c++
bool lib_ams::EnsureOutput(lib_ams::FShm &outshm, int budget) 
```

```c++
// Check for EOF
bool lib_ams::EofQ(lib_ams::FReadfile &rf) 
```

```c++
// Check for failure after close.
// EOF is not a failure.
bool lib_ams::FailQ(lib_ams::FReadfile &rf) 
```

```c++
// Check for failure. This is intended for checking Close errors.
// After Close, OkQ could not be used for this purpose,
// but failure flag is preserved, and could be retrieved with FailQ.
bool lib_ams::FailQ(lib_ams::FWritefile &wf) 
```

```c++
// Write buffer as message block
void lib_ams::Flush(lib_ams::FWritefile &wf) 
```

```c++
inline u64 lib_ams::GetWriteBudget(lib_ams::FShm &shm) 
```

```c++
// Initialize shm library
// FILE_PREFIX: unique filesystem prefix for all subsequently created shms.
// This is copied to lib_ams::_db.file_prefix.
// if FILE_PREFIX is empty, we are running in NON-SHMEM mode. In this mode,
// the library begins reading STDIN and posting any input messages to the default
// input shm.
// PROC_ID: process ID for this app. This is copied to lib_ams::_db.proc_id.
// The PROC_ID is <proctype>.<nodeindex>-<index> -- enough to identify any number of processes within
// a cluster.
// If the variable _db.shm_files_cleaned is FALSE (which is the default),
// clean any stale (unlocked) shm files in /dev/shm/*.ams
// In SHMEM mode, all current and future memory is locked using mlockall.
bool lib_ams::Init(algo::strptr file_prefix, ams::ProcId proc_id) 
```

```c++
inline ams::ProcId lib_ams::MakeProcId(ams::Proctype proctype, int node, int index) 
```

```c++
inline ams::Shmmsg *lib_ams::MsgAtOffset(lib_ams::FShm &shm, u64 offset) 
```

```c++
// Whether read file is in good state - valid fd and no failure.
// EOF is not a failure.
bool lib_ams::OkQ(lib_ams::FReadfile &rf) 
```

```c++
// Whether write file is in good state - valid fd and no failure
bool lib_ams::OkQ(lib_ams::FWritefile &wf) 
```

```c++
// Open file for appending messages
void lib_ams::OpenAppend(lib_ams::FWritefile &wf, algo::strptr filename) 
```

```c++
// Create a read shmember for shm SHM
// Begin hot-polling the shm
bool lib_ams::OpenRead(lib_ams::FShm &shm, u64 off) 
```

```c++
// Open file for reading messages
void lib_ams::OpenRead(lib_ams::FReadfile &rf, algo::strptr filename) 
```

```c++
// Open shm for reading or writing (or both)
// The file creator sets shm size.
// Writer creates & locks the file.
// Reader determines file from file size
// When opening for writing:
// The next message written to the shm will have OFFSET as determined by POS
// Initial value of offset is 0
// (However a newly spawned child process can be provided a known sequence & offset by the parent.)
// When opening for reading:
// begin hot-polling for new messages on the shm
bool lib_ams::OpenShm(lib_ams::FShm &shm, ams::ShmFlags flags, u64 pos = 0) 
```

```c++
// Create a write shmember for shm SHM
bool lib_ams::OpenWrite(lib_ams::FShm &shm, u64 off) 
```

```c++
// If the shm is open for reading, check to see if a message
// is available. If it is available, return pointer to message.
ams::Shmmsg *lib_ams::PeekMsg(lib_ams::FShmember &shmember) 
```

```c++
void lib_ams::PollCtlIn() 
```

```c++
// Print message MSG to string OUT according to format FMT
// if FMT.STRIP > 0, strip this many outer "layers";
// if FMT.BIN, the message is printed as pure binary; otherwise, convert to text
// if FMT.PRETTY, every next layer / payload is printed on a new line with indent
// for readability.
// FMT.PAYLOAD_LEN limits maximum printed payload length, allowing to fit one messge per
// screen even if payload is 10MB
// Finally, if FMT.SHOWLEN is true, message length is included in output.
void lib_ams::PrintMsg(lib_ams::MsgFmt &fmt, ams::MsgHeader &msg, cstring &out) 
```

```c++
// Read bytes
bool lib_ams::Read(lib_ams::FReadfile &rf,void *buf,u32 n) 
```

```c++
void lib_ams::ReadCtlMsg(lib_ams::FShm &, ams::MsgHeader &msg) 
```

```c++
// Read message by sequence number
ams::MsgHeader *lib_ams::ReadMsg(lib_ams::FReadfile &rf, u64 seqno) 
```

```c++
// Reindex
void lib_ams::Reindex(lib_ams::FReadfile &rf) 
```

```c++
// Scan and block load engine.
// if seqno is NULL, scans to end, fills block with last block.
// If seqno is present, scans up to given seqno, reads cbuf with payload
// Note is does only forward scan.
void lib_ams::Scan(lib_ams::FReadfile &rf,const u64 *seqno) 
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
// Send heartbeat to control shm
void lib_ams::SendHb(lib_ams::FShmember &shmember) 
```

```c++
void lib_ams::SetDfltShmSize(u32 size) 
```

```c++
// Register a bad, unskippable incoming message
// on SHM, report it, and stop reading.
void lib_ams::ShmError(lib_ams::FShm &shm, ams::Shmmsg *msg, const char *text) 
```

```c++
// return TRUE if shared memory region is attached to shm SHM.
bool lib_ams::ShmemFdOpenQ(lib_ams::FShm &shm) 
```

```c++
// Open shared memory for reading/writing (as specified in FLAGS)
// and return success status
// Fields initialized:
// - shm.filename
// - shm.shm_handle (windows)
// - shm.shm_file (linux)
// - shm.shm_region, if mapped successfully
// If the segment is opened for writing, it is flocked.
bool lib_ams::ShmemOpenFile(lib_ams::FShm &shm, ams::ShmFlags flags) 
```

```c++
// Skip bytes
void lib_ams::Skip(lib_ams::FReadfile &rf,u32 skip) 
```

```c++
// Mark current message as read and move to the next message.
void lib_ams::SkipMsg(lib_ams::FShmember &shmember) 
```

```c++
// starts new message block - discard buffer, adjust seqno
void lib_ams::StartNextMsgBlock(lib_ams::FWritefile &wf) 
```

```c++
// Called by the client
// to avoid reading current message
void lib_ams::StopReading(lib_ams::FShm &shm) 
```

```c++
// Convert message MSG to string in a way suitable for debugging
// (some information is lost in exchange for readability)
tempstr lib_ams::ToDbgString(ams::MsgHeader &msg) 
```

```c++
tempstr lib_ams::ToString(ams::MsgHeader &msg) 
```

```c++
// This function should be called if the ams logcat is enabled
// It prints the given MSG to ams logcat using pretty format.
// Heartbeats (ShmHb, MemberHb, PubMetric) are skipped unless verbose is on
void lib_ams::TraceMsg(ams::Shmmsg *msg, ams::MsgHeader *payload) 
```

```c++
// Uncompress
void lib_ams::Uncompress(lib_ams::FReadfile &rf) 
```

```c++
void lib_ams::Uninit() 
```

```c++
void lib_ams::UnreadMsg() 
```

```c++
// next_ackoff is the read offset at which an unconditional
// Shmhb is sent out. It is enabled when the shm is being read.
inline void lib_ams::UpdateAckOffset(lib_ams::FShm &shm, lib_ams::FShmember &shmember) 
```

```c++
// Update budget for shm SHM
// Return TRUE if the shm is potentially unblocked for
// writing due to WRITELIMIT being increased.
// (WRITELIMIT is the point beyond which no message can be written
// because doing so would overwrite data not yet consumed by one of the read members.)
bool lib_ams::UpdateWriteLimit(lib_ams::FShm &shm) 
```

```c++
// If the shm is attached to a writable shared memory segment,
// write sequenced message to shm SHM, and increment sequence number
// and shm offset.
// Message must be within max. message size
// If the shm is not configured for writingIf an output fd is associated with the shm, hthe shm, write
bool lib_ams::WriteMsg(lib_ams::FShm &shm, ams::MsgHeader &msg) 
```

```c++
// Write message (buffered)
void lib_ams::WriteMsg(lib_ams::FWritefile &wf, ams::MsgHeader &msg) 
```

```c++
// Write message block to file
void lib_ams::WriteMsgBlock(lib_ams::FWritefile &wf) 
```

```c++
// Read up to N messages from the message heap
// Messages are processed in the order in which they were posted to the shms.
void lib_ams::bh_shmember_read_Step() 
```

```c++
// Stop reading stdin
void lib_ams::cd_fdin_eof_Step() 
```

```c++
// Read next input line from stdin
// The line is parsed as an AMS message. It could be a Shmmsg or any other msg.
// If the line doesn't look like anything, it is converted to ams.InputLineMsg
// Then, look up a shm where to post the message.
// If we see a Shmmsg, find shm where this message is intended to go and write the message there.
// Otherwise, post the message to the default shm (lib_ams::_db.dflt_shm_id)
// If the shm where we are posting the message is full (won't accept the message)
// then reading of fdin is stopped and will resume after the shm has room.
// If there is nowhere to post the message because no target shm is found, the counter
// trace.n_fdin_drop_notgt is incremented and a message is printed in verbose mode.
void lib_ams::cd_fdin_read_Step() 
```

```c++
// Publish shm heartbeats to c_shm_ctl shm
void lib_ams::cd_hb_Step() 
```

```c++
// Check all shms (that are not already readable) for readability and
// transfer readable shms to the read heap with correct sort key.
// If powersave is enabled, then non-readable shms
// are transferred to the slow_poll_read list where they are polled less frequently
void lib_ams::cd_poll_read_Step() 
```

```c++
void lib_ams::cd_slow_poll_read_Step() 
```

```c++
// cleanup function
void lib_ams::fd_Cleanup(lib_ams::FReadfile& parent) 
```

```c++
// cleanup function
void lib_ams::fd_Cleanup(lib_ams::FWritefile& parent) 
```

```c++
lib_ams::FShm &lib_ams::ind_shm_GetOrCreate(ams::ShmId shm_id) 
```

```c++
void lib_ams::shm_file_Cleanup(lib_ams::FShm &shm) 
```

```c++
// Flush data to file
void lib_ams::zd_flush_Step() 
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
|[cpp/lib_ams/dump.cpp](/cpp/lib_ams/dump.cpp)||
|[cpp/lib_ams/file.cpp](/cpp/lib_ams/file.cpp)||
|[cpp/lib_ams/shm.cpp](/cpp/lib_ams/shm.cpp)||
|[cpp/lib_ams/shmember.cpp](/cpp/lib_ams/shmember.cpp)||
|[include/gen/lib_ams_gen.h](/include/gen/lib_ams_gen.h)||
|[include/gen/lib_ams_gen.inl.h](/include/gen/lib_ams_gen.inl.h)||
|[include/lib_ams.h](/include/lib_ams.h)||
|[include/lib_ams.inl.h](/include/lib_ams.inl.h)||

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
|[lib_ams.FProc](#lib_ams-fproc)||FDb.proc (Tpool)|ind_proc (Thash, hash field proc_id)|zd_proc (Llist)|c_thisproc (Ptr)|
||||FShmember.p_proc (Upptr)|
|[lib_ams.FReadfile](#lib_ams-freadfile)||
|[lib_ams.FShm](#lib_ams-fshm)||FDb.shm (Lary)|shm (Lary, by rowid)|ind_shm (Thash, hash field shm_id)|c_shm_ctl (Ptr)|
||||FProc.c_shm (Ptrary)|
||||FShmember.p_shm (Upptr)|
|[lib_ams.FShmember](#lib_ams-fshmember)||FDb.shmember (Tpool)|cd_hb (Llist)|cd_poll_read (Llist)|cd_slow_poll_read (Llist)|ind_shmember (Thash, hash field shmember)|c_ctlin (Ptrary)|c_cur_shmember (Ptr)|bh_shmember_read (Bheap, sort field sortkey)|
||||FProc.c_shmember (Ptrary)|
||||FShm.c_shmember (Ptrary)|
||||FShm.c_read (Ptr)|
||||FShm.c_write (Ptr)|
|[lib_ams.FShmtype](#lib_ams-fshmtype)|[amsdb.shmtype](/txt/ssimdb/amsdb/shmtype.md)|FDb.shmtype (Lary)|**static**|shmtype (Lary, by rowid)|ind_shmtype (Thash, hash field id)|
||||FShm.p_shmtype (Upptr)|
|[lib_ams.FWritefile](#lib_ams-fwritefile)||FDb.writefile (Cppstack)|zd_flush (Llist)|
|[lib_ams.MsgFmt](#lib_ams-msgfmt)||
||||MsgFmt.h_convert (Hook)|

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
|lib_ams.FDb.shm|[lib_ams.FShm](/txt/lib/lib_ams/README.md#lib_ams-fshm)|[Lary](/txt/exe/amc/reftypes.md#lary)||Table of streams|
|lib_ams.FDb.ind_shm|[lib_ams.FShm](/txt/lib/lib_ams/README.md#lib_ams-fshm)|[Thash](/txt/exe/amc/reftypes.md#thash)||Index of streams by stream id|
|lib_ams.FDb.cd_hb|[lib_ams.FShmember](/txt/lib/lib_ams/README.md#lib_ams-fshmember)|[Llist](/txt/exe/amc/reftypes.md#llist)||Send heartbeats (per-member)|
|lib_ams.FDb.proc_id|[ams.ProcId](/txt/protocol/ams/ProcId.md)|[Val](/txt/exe/amc/reftypes.md#val)||Process id, e.g. amstest-0|
|lib_ams.FDb.shmem_size|i32|[Val](/txt/exe/amc/reftypes.md#val)|32768|Default stream shared memory size|
|lib_ams.FDb.max_msg_size|i32|[Val](/txt/exe/amc/reftypes.md#val)|4096|Max. size|
|lib_ams.FDb.cd_poll_read|[lib_ams.FShmember](/txt/lib/lib_ams/README.md#lib_ams-fshmember)|[Llist](/txt/exe/amc/reftypes.md#llist)||Hot poll input stream for reading|
|lib_ams.FDb.cd_slow_poll_read|[lib_ams.FShmember](/txt/lib/lib_ams/README.md#lib_ams-fshmember)|[Llist](/txt/exe/amc/reftypes.md#llist)||Cold poll input stream for reading|
|lib_ams.FDb.file_prefix|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|File prefix for all streams|
|lib_ams.FDb.proc|[lib_ams.FProc](/txt/lib/lib_ams/README.md#lib_ams-fproc)|[Tpool](/txt/exe/amc/reftypes.md#tpool)|||
|lib_ams.FDb.ind_proc|[lib_ams.FProc](/txt/lib/lib_ams/README.md#lib_ams-fproc)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|lib_ams.FDb.shmember|[lib_ams.FShmember](/txt/lib/lib_ams/README.md#lib_ams-fshmember)|[Tpool](/txt/exe/amc/reftypes.md#tpool)|||
|lib_ams.FDb.ind_shmember|[lib_ams.FShmember](/txt/lib/lib_ams/README.md#lib_ams-fshmember)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|lib_ams.FDb.shmtype|[lib_ams.FShmtype](/txt/lib/lib_ams/README.md#lib_ams-fshmtype)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|lib_ams.FDb.ind_shmtype|[lib_ams.FShmtype](/txt/lib/lib_ams/README.md#lib_ams-fshmtype)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|lib_ams.FDb.c_ctlin|[lib_ams.FShmember](/txt/lib/lib_ams/README.md#lib_ams-fshmember)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|lib_ams.FDb.shm_files_cleaned|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_ams.FDb.shmem_mode|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_ams.FDb.fdin_buf|[algo.ByteAry](/txt/protocol/algo/README.md#algo-byteary)|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_ams.FDb.dflt_shm_id|[ams.ShmId](/txt/protocol/ams/README.md#ams-shmid)|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_ams.FDb.zd_proc|[lib_ams.FProc](/txt/lib/lib_ams/README.md#lib_ams-fproc)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|lib_ams.FDb.c_shm_ctl|[lib_ams.FShm](/txt/lib/lib_ams/README.md#lib_ams-fshm)|[Ptr](/txt/exe/amc/reftypes.md#ptr)||Ctl output shm (process events)|
|lib_ams.FDb.expect_buf|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Buffer containing outputs produced since last input|
|lib_ams.FDb.expect_str|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||If non-empty, pause reading inputs until this string is non-empty|
|lib_ams.FDb.expect_pos|i32|[Val](/txt/exe/amc/reftypes.md#val)||Match position within expect buf|
|lib_ams.FDb.expect_timeout|double|[Val](/txt/exe/amc/reftypes.md#val)|10.0|Default expect timeout|
|lib_ams.FDb.fmt_buf|[algo.ByteAry](/txt/protocol/algo/README.md#algo-byteary)|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_ams.FDb.c_cur_shmember|[lib_ams.FShmember](/txt/lib/lib_ams/README.md#lib_ams-fshmember)|[Ptr](/txt/exe/amc/reftypes.md#ptr)||lib_ams message read callback context|
|lib_ams.FDb.bh_shmember_read|[lib_ams.FShmember](/txt/lib/lib_ams/README.md#lib_ams-fshmember)|[Bheap](/txt/exe/amc/reftypes.md#bheap)|||
|lib_ams.FDb.c_thisproc|[lib_ams.FProc](/txt/lib/lib_ams/README.md#lib_ams-fproc)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|lib_ams.FDb.powersave|bool|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FDb
<a href="#struct-fdb"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_ams_gen.h](/include/gen/lib_ams_gen.h)
```
struct FDb { // lib_ams.FDb: In-memory database for lib_ams
    lib_ams::FFdin*        fdin_lary[32];                // level array
    i32                    fdin_n;                       // number of elements in array
    lib_ams::FFdin*        cd_fdin_eof_head;             // zero-terminated doubly linked list
    i32                    cd_fdin_eof_n;                // zero-terminated doubly linked list
    lib_ams::FFdin*        cd_fdin_read_head;            // zero-terminated doubly linked list
    i32                    cd_fdin_read_n;               // zero-terminated doubly linked list
    lib_ams::FWritefile*   zd_flush_head;                // zero-terminated doubly linked list
    i32                    zd_flush_n;                   // zero-terminated doubly linked list
    lib_ams::FWritefile*   zd_flush_tail;                // pointer to last element
    algo::SchedTime        zd_flush_next;                // lib_ams.FDb.zd_flush                                               Next invocation time
    algo::SchedTime        zd_flush_delay;               // lib_ams.FDb.zd_flush                                               Delay between invocations
    lib_ams::FShm*         shm_lary[32];                 // level array
    i32                    shm_n;                        // number of elements in array
    lib_ams::FShm**        ind_shm_buckets_elems;        // pointer to bucket array
    i32                    ind_shm_buckets_n;            // number of elements in bucket array
    i32                    ind_shm_n;                    // number of elements in the hash table
    lib_ams::FShmember*    cd_hb_head;                   // zero-terminated doubly linked list
    i32                    cd_hb_n;                      // zero-terminated doubly linked list
    algo::SchedTime        cd_hb_next;                   // lib_ams.FDb.cd_hb                                                  Next invocation time
    algo::SchedTime        cd_hb_delay;                  // lib_ams.FDb.cd_hb                                                  Delay between invocations
    ams::ProcId            proc_id;                      // Process id, e.g. amstest-0
    i32                    shmem_size;                   //   32768  Default stream shared memory size
    i32                    max_msg_size;                 //   4096  Max. size
    lib_ams::FShmember*    cd_poll_read_head;            // zero-terminated doubly linked list
    i32                    cd_poll_read_n;               // zero-terminated doubly linked list
    lib_ams::FShmember*    cd_slow_poll_read_head;       // zero-terminated doubly linked list
    i32                    cd_slow_poll_read_n;          // zero-terminated doubly linked list
    algo::SchedTime        cd_slow_poll_read_next;       // lib_ams.FDb.cd_slow_poll_read                                      Next invocation time
    algo::SchedTime        cd_slow_poll_read_delay;      // lib_ams.FDb.cd_slow_poll_read                                      Delay between invocations
    algo::cstring          file_prefix;                  //   ""  File prefix for all streams
    u64                    proc_blocksize;               // # bytes per block
    lib_ams::FProc*        proc_free;                    //
    lib_ams::FProc**       ind_proc_buckets_elems;       // pointer to bucket array
    i32                    ind_proc_buckets_n;           // number of elements in bucket array
    i32                    ind_proc_n;                   // number of elements in the hash table
    u64                    shmember_blocksize;           // # bytes per block
    lib_ams::FShmember*    shmember_free;                //
    lib_ams::FShmember**   ind_shmember_buckets_elems;   // pointer to bucket array
    i32                    ind_shmember_buckets_n;       // number of elements in bucket array
    i32                    ind_shmember_n;               // number of elements in the hash table
    lib_ams::FShmtype*     shmtype_lary[32];             // level array
    i32                    shmtype_n;                    // number of elements in array
    lib_ams::FShmtype**    ind_shmtype_buckets_elems;    // pointer to bucket array
    i32                    ind_shmtype_buckets_n;        // number of elements in bucket array
    i32                    ind_shmtype_n;                // number of elements in the hash table
    lib_ams::FShmember**   c_ctlin_elems;                // array of pointers
    u32                    c_ctlin_n;                    // array of pointers
    u32                    c_ctlin_max;                  // capacity of allocated array
    bool                   shm_files_cleaned;            //   false
    bool                   shmem_mode;                   //   false
    algo::ByteAry          fdin_buf;                     //
    ams::ShmId             dflt_shm_id;                  //
    lib_ams::FProc*        zd_proc_head;                 // zero-terminated doubly linked list
    i32                    zd_proc_n;                    // zero-terminated doubly linked list
    lib_ams::FProc*        zd_proc_tail;                 // pointer to last element
    lib_ams::FShm*         c_shm_ctl;                    // Ctl output shm (process events). optional pointer
    algo::cstring          expect_buf;                   // Buffer containing outputs produced since last input
    algo::cstring          expect_str;                   // If non-empty, pause reading inputs until this string is non-empty
    i32                    expect_pos;                   //   0  Match position within expect buf
    double                 expect_timeout;               //   10.0  Default expect timeout
    algo::ByteAry          fmt_buf;                      //
    lib_ams::FShmember*    c_cur_shmember;               // lib_ams message read callback context. optional pointer
    lib_ams::FShmember**   bh_shmember_read_elems;       // binary heap by sortkey
    i32                    bh_shmember_read_n;           // number of elements in the heap
    i32                    bh_shmember_read_max;         // max elements in bh_shmember_read_elems
    lib_ams::FProc*        c_thisproc;                   // optional pointer
    bool                   powersave;                    //   false
    lib_ams::trace         trace;                        //
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
    u8*                 in_elems;            //   NULL  pointer to elements of indirect array
    u32                 in_max;              //   0  current length of allocated array
    i32                 in_start;            // beginning of valid bytes (in bytes)
    i32                 in_end;              // end of valid bytes (in bytes)
    i32                 in_msglen;           // current message length
    algo::Errcode       in_err;              // system error code
    algo_lib::FIohook   in_iohook;           // edge-triggered hook for the buffer
    bool                in_eof;              // no more data will be written to buffer
    bool                in_msgvalid;         // current message is valid
    bool                in_epoll_enable;     // use epoll?
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

#### lib_ams.FProc - 
<a href="#lib_ams-fproc"></a>

#### lib_ams.FProc Fields
<a href="#lib_ams-fproc-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_ams.FProc.proc_id|[ams.ProcId](/txt/protocol/ams/ProcId.md)|[Val](/txt/exe/amc/reftypes.md#val)||Unique ID|
|lib_ams.FProc.c_shm|[lib_ams.FShm](/txt/lib/lib_ams/README.md#lib_ams-fshm)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|lib_ams.FProc.c_shmember|[lib_ams.FShmember](/txt/lib/lib_ams/README.md#lib_ams-fshmember)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)||List of AMS shm readers/writers at this proc|
|lib_ams.FProc.pid|i32|[Val](/txt/exe/amc/reftypes.md#val)||Current PID|
|lib_ams.FProc.status|i32|[Val](/txt/exe/amc/reftypes.md#val)||Exit status|
|lib_ams.FProc.hbtimeout|u32|[Val](/txt/exe/amc/reftypes.md#val)||Heartbeat timeout|
|lib_ams.FProc.critical|bool|[Val](/txt/exe/amc/reftypes.md#val)||Exit if process dies?|

#### Struct FProc
<a href="#struct-fproc"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_ams_gen.h](/include/gen/lib_ams_gen.h)
```
struct FProc { // lib_ams.FProc
    lib_ams::FProc*        proc_next;          // Pointer to next free element int tpool
    lib_ams::FProc*        ind_proc_next;      // hash next
    u32                    ind_proc_hashval;   // hash value
    lib_ams::FProc*        zd_proc_next;       // zslist link; -1 means not-in-list
    lib_ams::FProc*        zd_proc_prev;       // previous element
    ams::ProcId            proc_id;            // Unique ID
    lib_ams::FShm**        c_shm_elems;        // array of pointers
    u32                    c_shm_n;            // array of pointers
    u32                    c_shm_max;          // capacity of allocated array
    lib_ams::FShmember**   c_shmember_elems;   // array of pointers
    u32                    c_shmember_n;       // array of pointers
    u32                    c_shmember_max;     // capacity of allocated array
    i32                    pid;                //   0  Current PID
    i32                    status;             //   0  Exit status
    u32                    hbtimeout;          //   0  Heartbeat timeout
    bool                   critical;           //   false  Exit if process dies?
    // reftype Ptrary of lib_ams.FProc.c_shm prohibits copy
    // reftype Ptrary of lib_ams.FProc.c_shmember prohibits copy
    // func:lib_ams.FProc..AssignOp
    inline lib_ams::FProc& operator =(const lib_ams::FProc &rhs) = delete;
    // reftype Ptrary of lib_ams.FProc.c_shm prohibits copy
    // reftype Ptrary of lib_ams.FProc.c_shmember prohibits copy
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

#### lib_ams.FShm - Shm record
<a href="#lib_ams-fshm"></a>

#### lib_ams.FShm Fields
<a href="#lib_ams-fshm-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_ams.FShm.shm_id|[ams.ShmId](/txt/protocol/ams/README.md#ams-shmid)|[Val](/txt/exe/amc/reftypes.md#val)||Stream ID (primary key)|
|lib_ams.FShm.c_shmember|[lib_ams.FShmember](/txt/lib/lib_ams/README.md#lib_ams-fshmember)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)||List of all read/write members|
|lib_ams.FShm.filename|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Name of the file (if opened)|
|lib_ams.FShm.shm_file|[algo_lib.FFildes](/txt/lib/algo_lib/README.md#algo_lib-ffildes)|[Val](/txt/exe/amc/reftypes.md#val)||Associated file|
|lib_ams.FShm.flags|[ams.ShmFlags](/txt/protocol/ams/README.md#ams-shmflags)|[Val](/txt/exe/amc/reftypes.md#val)||Is stream open for writing|
|lib_ams.FShm.shm_handle|u8|[Ptr](/txt/exe/amc/reftypes.md#ptr)||Associated memory segment handle (windows only)|
|lib_ams.FShm.c_read|[lib_ams.FShmember](/txt/lib/lib_ams/README.md#lib_ams-fshmember)|[Ptr](/txt/exe/amc/reftypes.md#ptr)||This process|
|lib_ams.FShm.c_write|[lib_ams.FShmember](/txt/lib/lib_ams/README.md#lib_ams-fshmember)|[Ptr](/txt/exe/amc/reftypes.md#ptr)||This process|
|lib_ams.FShm.shm_region|[algo.memptr](/txt/protocol/algo/memptr.md)|[Val](/txt/exe/amc/reftypes.md#val)||Shared memory region|
|lib_ams.FShm.writelimit|u64|[Val](/txt/exe/amc/reftypes.md#val)||Write limit, based on all read members|
|lib_ams.FShm.offset_mask|u64|[Val](/txt/exe/amc/reftypes.md#val)||Mask for byte eof|
|lib_ams.FShm.h_amsmsg|[ams.MsgHeader](/txt/protocol/ams/MsgHeader.md)|[Hook](/txt/exe/amc/reftypes.md#hook)||Message processing hook|
|lib_ams.FShm.h_msg_orig|[ams.MsgHeader](/txt/protocol/ams/MsgHeader.md)|[Hook](/txt/exe/amc/reftypes.md#hook)||Saved message processing hook (EXPECT)|
|lib_ams.FShm.next_ackoff|u64|[Val](/txt/exe/amc/reftypes.md#val)|1023|Force StreamHb after reaching this offset|
|lib_ams.FShm.p_shmtype|[lib_ams.FShmtype](/txt/lib/lib_ams/README.md#lib_ams-fshmtype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|lib_ams.FShm.n_write_block|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_ams.FShm.error|bool|[Val](/txt/exe/amc/reftypes.md#val)||Error was detected|
|lib_ams.FShm.created|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_ams.FShm.schedbias|i64|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FShm
<a href="#struct-fshm"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_ams_gen.h](/include/gen/lib_ams_gen.h)
```
struct FShm { // lib_ams.FShm: Shm record
    lib_ams::FShm*                 ind_shm_next;       // hash next
    u32                            ind_shm_hashval;    // hash value
    ams::ShmId                     shm_id;             // Stream ID (primary key)
    lib_ams::FShmember**           c_shmember_elems;   // array of pointers
    u32                            c_shmember_n;       // array of pointers
    u32                            c_shmember_max;     // capacity of allocated array
    algo::cstring                  filename;           // Name of the file (if opened)
    algo_lib::FFildes              shm_file;           // Associated file
    ams::ShmFlags                  flags;              // Is stream open for writing
    u8*                            shm_handle;         // Associated memory segment handle (windows only). optional pointer
    lib_ams::FShmember*            c_read;             // This process. optional pointer
    lib_ams::FShmember*            c_write;            // This process. optional pointer
    algo::memptr                   shm_region;         // Shared memory region
    u64                            writelimit;         //   0  Write limit, based on all read members
    u64                            offset_mask;        //   0  Mask for byte eof
    lib_ams::shm_h_amsmsg_hook     h_amsmsg;           //   NULL  Pointer to a function
    u64                            h_amsmsg_ctx;       //   0  Callback context
    lib_ams::shm_h_msg_orig_hook   h_msg_orig;         //   NULL  Pointer to a function
    u64                            h_msg_orig_ctx;     //   0  Callback context
    u64                            next_ackoff;        //   1023  Force StreamHb after reaching this offset
    lib_ams::FShmtype*             p_shmtype;          // reference to parent row
    u32                            n_write_block;      //   0
    bool                           error;              //   false  Error was detected
    bool                           created;            //   false
    i64                            schedbias;          //   0
    i32                            proc_c_shm_idx;     //   -1  Array index (-1 = not in array)
    // reftype Ptrary of lib_ams.FShm.c_shmember prohibits copy
    // reftype Hook of lib_ams.FShm.h_amsmsg prohibits copy
    // reftype Hook of lib_ams.FShm.h_msg_orig prohibits copy
    // x-reference on lib_ams.FShm.p_shmtype prevents copy
    // func:lib_ams.FShm..AssignOp
    lib_ams::FShm&       operator =(const lib_ams::FShm &rhs) = delete;
    // reftype Ptrary of lib_ams.FShm.c_shmember prohibits copy
    // reftype Hook of lib_ams.FShm.h_amsmsg prohibits copy
    // reftype Hook of lib_ams.FShm.h_msg_orig prohibits copy
    // x-reference on lib_ams.FShm.p_shmtype prevents copy
    // func:lib_ams.FShm..CopyCtor
    FShm(const lib_ams::FShm &rhs) = delete;
private:
    // func:lib_ams.FShm..Ctor
    inline               FShm() __attribute__((nothrow));
    // func:lib_ams.FShm..Dtor
    inline               ~FShm() __attribute__((nothrow));
    friend lib_ams::FShm&       shm_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend lib_ams::FShm*       shm_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 shm_RemoveAll() __attribute__((nothrow));
    friend void                 shm_RemoveLast() __attribute__((nothrow));
};
```

#### lib_ams.FShmember - 
<a href="#lib_ams-fshmember"></a>

#### lib_ams.FShmember Fields
<a href="#lib_ams-fshmember-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_ams.FShmember.shmember|[ams.ShmemberId](/txt/protocol/ams/README.md#ams-shmemberid)|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_ams.FShmember.off|u64|[Val](/txt/exe/amc/reftypes.md#val)||Current offset|
|lib_ams.FShmember.rate_off|[algo.I64Rate](/txt/protocol/algo/README.md#algo-i64rate)|[Val](/txt/exe/amc/reftypes.md#val)||Rate of change of offset|
|lib_ams.FShmember.sortkey|u64|[Val](/txt/exe/amc/reftypes.md#val)||Sort key for reading|
|lib_ams.FShmember.last_hb|[algo.SchedTime](/txt/protocol/algo/SchedTime.md)|[Val](/txt/exe/amc/reftypes.md#val)||Time last heartbeat received|
|lib_ams.FShmember.budget|u64|[Val](/txt/exe/amc/reftypes.md#val)||Write budget (if applicable)|
|lib_ams.FShmember.p_shm|[lib_ams.FShm](/txt/lib/lib_ams/README.md#lib_ams-fshm)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|lib_ams.FShmember.c_cur_msg|[ams.Shmmsg](/txt/protocol/ams/README.md#ams-shmmsg)|[Ptr](/txt/exe/amc/reftypes.md#ptr)||Current message|
|lib_ams.FShmember.p_proc|[lib_ams.FProc](/txt/lib/lib_ams/README.md#lib_ams-fproc)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FShmember
<a href="#struct-fshmember"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_ams_gen.h](/include/gen/lib_ams_gen.h)
```
struct FShmember { // lib_ams.FShmember
    lib_ams::FShmember*   cd_hb_next;               // zslist link; -1 means not-in-list
    lib_ams::FShmember*   cd_hb_prev;               // previous element
    lib_ams::FShmember*   cd_poll_read_next;        // zslist link; -1 means not-in-list
    lib_ams::FShmember*   cd_poll_read_prev;        // previous element
    lib_ams::FShmember*   cd_slow_poll_read_next;   // zslist link; -1 means not-in-list
    lib_ams::FShmember*   cd_slow_poll_read_prev;   // previous element
    lib_ams::FShmember*   shmember_next;            // Pointer to next free element int tpool
    lib_ams::FShmember*   ind_shmember_next;        // hash next
    u32                   ind_shmember_hashval;     // hash value
    i32                   bh_shmember_read_idx;     // index in heap; -1 means not-in-heap
    ams::ShmemberId       shmember;                 //
    u64                   off;                      //   0  Current offset
    algo::I64Rate         rate_off;                 // Rate of change of offset
    u64                   sortkey;                  //   0  Sort key for reading
    algo::SchedTime       last_hb;                  // Time last heartbeat received
    u64                   budget;                   //   0  Write budget (if applicable)
    lib_ams::FShm*        p_shm;                    // reference to parent row
    ams::Shmmsg*          c_cur_msg;                // Current message. optional pointer
    lib_ams::FProc*       p_proc;                   // reference to parent row
    i32                   c_ctlin_idx;              //   -1  Array index (-1 = not in array)
    i32                   proc_c_shmember_idx;      //   -1  Array index (-1 = not in array)
    i32                   shm_c_shmember_idx;       //   -1  Array index (-1 = not in array)
    // x-reference on lib_ams.FShmember.p_shm prevents copy
    // x-reference on lib_ams.FShmember.p_proc prevents copy
    // func:lib_ams.FShmember..AssignOp
    lib_ams::FShmember&  operator =(const lib_ams::FShmember &rhs) = delete;
    // x-reference on lib_ams.FShmember.p_shm prevents copy
    // x-reference on lib_ams.FShmember.p_proc prevents copy
    // func:lib_ams.FShmember..CopyCtor
    FShmember(const lib_ams::FShmember &rhs) = delete;
private:
    // func:lib_ams.FShmember..Ctor
    inline               FShmember() __attribute__((nothrow));
    // func:lib_ams.FShmember..Dtor
    inline               ~FShmember() __attribute__((nothrow));
    friend lib_ams::FShmember&  shmember_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend lib_ams::FShmember*  shmember_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 shmember_Delete(lib_ams::FShmember &row) __attribute__((nothrow));
};
```

#### lib_ams.FShmtype - 
<a href="#lib_ams-fshmtype"></a>

#### lib_ams.FShmtype Fields
<a href="#lib_ams-fshmtype-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_ams.FShmtype.base|[amsdb.Shmtype](/txt/ssimdb/amsdb/shmtype.md)|[Base](/txt/ssimdb/amsdb/shmtype.md)|||

#### Struct FShmtype
<a href="#struct-fshmtype"></a>
*Note:* field ``lib_ams.FShmtype.base`` has reftype ``base`` so the fields of [amsdb.Shmtype](/txt/ssimdb/amsdb/shmtype.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_ams_gen.h](/include/gen/lib_ams_gen.h)
```
struct FShmtype { // lib_ams.FShmtype
    lib_ams::FShmtype*   ind_shmtype_next;      // hash next
    u32                  ind_shmtype_hashval;   // hash value
    algo::Smallstr50     shmtype;               //
    ams::Shmtype         id;                    //
    bool                 nonblock;              //   false  Non-blocking (lossy) stream
    algo::Comment        comment;               //
    // func:lib_ams.FShmtype..AssignOp
    inline lib_ams::FShmtype& operator =(const lib_ams::FShmtype &rhs) = delete;
    // func:lib_ams.FShmtype..CopyCtor
    inline               FShmtype(const lib_ams::FShmtype &rhs) = delete;
private:
    // func:lib_ams.FShmtype..Ctor
    inline               FShmtype() __attribute__((nothrow));
    // func:lib_ams.FShmtype..Dtor
    inline               ~FShmtype() __attribute__((nothrow));
    friend lib_ams::FShmtype&   shmtype_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend lib_ams::FShmtype*   shmtype_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 shmtype_RemoveAll() __attribute__((nothrow));
    friend void                 shmtype_RemoveLast() __attribute__((nothrow));
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

#### lib_ams.MsgFmt - 
<a href="#lib_ams-msgfmt"></a>

#### lib_ams.MsgFmt Fields
<a href="#lib_ams-msgfmt-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_ams.MsgFmt.payload_lim|i32|[Val](/txt/exe/amc/reftypes.md#val)|1024000|Print up to this many chars from payload|
|lib_ams.MsgFmt.indent|i32|[Val](/txt/exe/amc/reftypes.md#val)|0|Print indentation (with -pretty)|
|lib_ams.MsgFmt.format|u8|[Val](/txt/exe/amc/reftypes.md#val)|0|Output format|
|lib_ams.MsgFmt.strip|i32|[Val](/txt/exe/amc/reftypes.md#val)||Number of headers to strip|
|lib_ams.MsgFmt.pretty|bool|[Val](/txt/exe/amc/reftypes.md#val)|true|Use structured and indented printing|
|lib_ams.MsgFmt.showlen|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Show message length|
|lib_ams.MsgFmt.h_convert|[lib_ams.MsgFmt](/txt/lib/lib_ams/README.md#lib_ams-msgfmt)|[Hook](/txt/exe/amc/reftypes.md#hook)|||
|lib_ams.MsgFmt.convert_key|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||conversion key|
|lib_ams.MsgFmt.convert_val|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||conversion value - overwritten by the return|

#### Struct MsgFmt
<a href="#struct-msgfmt"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_ams_gen.h](/include/gen/lib_ams_gen.h)
```
struct MsgFmt { // lib_ams.MsgFmt
    i32                              payload_lim;     //   1024000  Print up to this many chars from payload
    i32                              indent;          //   0  Print indentation (with -pretty)
    u8                               format;          //   0  Output format
    i32                              strip;           //   0  Number of headers to strip
    bool                             pretty;          //   true  Use structured and indented printing
    bool                             showlen;         //   false  Show message length
    lib_ams::parent_h_convert_hook   h_convert;       //   NULL  Pointer to a function
    u64                              h_convert_ctx;   //   0  Callback context
    algo::cstring                    convert_key;     // conversion key
    algo::cstring                    convert_val;     // conversion value - overwritten by the return
    // func:lib_ams.MsgFmt..Ctor
    inline               MsgFmt() __attribute__((nothrow));
};
```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Imdb -->


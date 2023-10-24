## AMS: Algo Mesaging System

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

## AMS Library

Ams logic is implemented in the library `lib_ams`.
Each process knows what to do with certain streams; For instance, each process will direct
disk IO to the io stream if it's configured.

## List of AMS Messages

All ams messages are derived from `ams.MsgHeader`, which is a 32-bit header with type
and length.

### `ams.ProcAdd`: Create process

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

### Tests

- `atf_unit lib_ams.SendData`: This test uses `fork()` and sends 1 million messages through a small
shared memory window, measuring receive latency


## lib_ams - Library for AMS middleware, supporting file format & messaging
<a href="#lib_ams"></a>

AMS is a shared memory transport for communication between single-threaded processes.
Every process is a single-threaded process.

When invoked with default options, each process reads messages from stdin and writes all of its outputs
to stdout. Some of the inputs (see list of ams messages) may instruct the process to start reading /
writing shared memory and turn off reading of messages from stdin.

AMS communication is based on shms. A shm is a named append-only ring buffer of messages
backed by a memory-mapped file under `/dev/shm/` (Linux) or a named section (Windows).
Every shm has only a single writer process, but may have multiple readers.
Writers track the minimum read offset across all readers and block (or drop in lossy mode)
when writing would overwrite data that a reader has not yet consumed.
Shm readers are registered with the control process which watches for liveness and keeps track
of overall state.

A shm ID is a 32-bit entity consisting of four fields: process type, process id, grptype, and group index.
- ProcType: 8-bit value, see table proctype
- ProcId: 24-bit value, ProcType + nodeidx (node index) + procidx (process index on node)
- Grptype: 8-bit value, see table grptype
- ShmId: proc_id + grptype + grpidx

Process types are registered in table `amsdb.proctype`. The standard types are:

```
inline-command: acr proctype | ssimfilt ^ -t
PROCTYPE      ID  NS            OVERHEADMB  HUGEMB  HBTIMEOUT  COMMENT
              0                 0           0       30         No process
ams_sendtest  5   ams_sendtest  0           4295    30         Ams testing
ext           22                0           4295    30         External client connection
samp_meng     23  samp_meng     0           4295    30         Matching engine
user          21                0           4295    30         User process launched via userproc

```

Grptypes are registered in table `amsdb.grptype`. Grptype describes both a shared memory segment type and
a multicast group type. The standard types are:

```
inline-command: acr grptype -report:N
amsdb.grptype  grptype:board    id:24  comment:"Message board: large payloads referenced from lane rings"
amsdb.grptype  grptype:log      id:9   comment:"Log messages"
amsdb.grptype  grptype:netall   id:4   comment:"Net process to all processes on node"
amsdb.grptype  grptype:nethb    id:22  comment:"Net process to one process (unicast heartbeats)"
amsdb.grptype  grptype:netpr    id:3   comment:"Net process to any process (unicast)"
amsdb.grptype  grptype:out      id:2   comment:"Log messages, alarms"
amsdb.grptype  grptype:prmon    id:18  comment:"Process to monitor"
amsdb.grptype  grptype:prnet    id:5   comment:"Process to net"
amsdb.grptype  grptype:prsup    id:12  comment:"Process to supervisor"
amsdb.grptype  grptype:prtxn    id:10  comment:"Process to txn"
amsdb.grptype  grptype:pruser   id:21  comment:"Parent process to user process"
amsdb.grptype  grptype:supall   id:11  comment:"Supervisor to all"
amsdb.grptype  grptype:txncmt   id:13  comment:"Net to commit gapfill fallback"
amsdb.grptype  grptype:txnpr    id:23  comment:"Txn to process (per-partition send credit grants)"
amsdb.grptype  grptype:userpr   id:20  comment:"User process to parent process"
amsdb.grptype  grptype:xbcast   id:16  comment:"(x-node) Any to all"
amsdb.grptype  grptype:xinctl   id:8   comment:"(x-node) Directed control: command and response envelopes"
amsdb.grptype  grptype:xindata  id:6   comment:"(x-node) Directed data delivery to one node"
amsdb.grptype  grptype:xingf    id:7   comment:"(x-node) Gapfill asks directed at a serving node, and its re-serves"
amsdb.grptype  grptype:xpart    id:1   comment:"(x-node) Partition messages"
```

Together, shm id is formatted like this: `proctype-nodeidx-procidx.grptype-grpidx`.
For instance, `proc1-0-0.md-0`.

Proc types and grptypes can be extended for the given system being implemented.

### Table Of Contents
<a href="#table-of-contents"></a>
<!-- abt_md.toc_beg -->
&nbsp;&nbsp;&bull;&nbsp;  [Internals](#internals)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [AMS Library](#ams-library)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [In-memory shm format; Flow control](#in-memory-shm-format-flow-control)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Shmhdr](#shmhdr)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Writing](#writing)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Reading](#reading)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Flow control (budget)](#flow-control-budget-)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Message board](#message-board)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Two-tier polling](#two-tier-polling)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Heartbeats and liveness](#heartbeats-and-liveness)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Performance](#performance)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Message board performance](#message-board-performance)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [AMS Shm Tracing](#ams-shm-tracing)<br/>
<!-- abt_md.toc_end -->

### Internals
<a href="#internals"></a>
&#128196; [lib_ams - Internals](/txt/gen/lib_ams/lib_ams.md)<br/>

## AMS Library
<a href="#ams-library"></a>

Ams logic is implemented in the library `lib_ams`.
Each process knows what to do with certain shms; For instance, each process will direct
disk IO to the io shm if it's configured.

## In-memory shm format; Flow control
<a href="#in-memory-shm-format-flow-control"></a>

A shm is a memory-mapped ring buffer. The file has a 4 KB control header (`ams.Shmhdr`)
followed by a data region whose usable size is rounded down to a power of two.
Messages are written contiguously at `offset & offset_mask` within the data region.

### Shmhdr
<a href="#shmhdr"></a>

The shared header contains:
- `woff` — absolute write offset, updated by the writer after `sfence()`
- `offset_mask` — power-of-two mask for mapping absolute offsets to the ring
- `n_shmember` / `max_shmember` — current and max reader count
- `writer_pid` — PID of the writer (used by flock for mutual exclusion)
- `nnobudget` / `nblock` — flow-control statistics

The first `max_shmember` entries of `ams.Shmember` follow the header.
Each shmember tracks `offset` (absolute read offset), `pid`, and heartbeat info.

### Writing
<a href="#writing"></a>

`BeginWrite(shm, length)` check write budget and return a pointer into the ring.
After the caller copies payload into that region, `EndWrite(shm, ptr, len)`:
1. Compute new write offset: `woff += len`
2. Issue `sfence()` — ensure all payload bytes are globally visible
3. Store `shmhdr->woff = woff`
4. If a reader exists on this shm, insert the shm into `cd_poll_read` so the
   next poll step processes it

In blocking mode (the default for most grptypes), the writer spins in `WaitBudget`
until at least one reader advances. In lossy mode (`wlossy` flag), the write is dropped
instead of blocking.

### Reading
<a href="#reading"></a>

Reader discovery is polling-based. `PeekMsg(shm)` compare the reader's `offset` against
`cached_woff`. If the cache is stale, it issues `lfence()` and reloads `shmhdr->woff`.
If `reader_offset + msg->length <= woff`, a complete message is available and a pointer is
returned.

### Flow control (budget)
<a href="#flow-control-budget-"></a>

`UpdateBudget(shm)` scan all active shmembers for the minimum read offset and set
`writelimit = min_roff + ring_size - 2*max_msg_size`.
The writer cannot advance `woff` past `writelimit`. This guarantees that the writer
never overwrites data any reader still needs.

### Message board
<a href="#message-board"></a>

A lane has to reserve room for the largest message that may cross it. The budget
formula above subtracts two `max_msg_size` from the ring, so a ring must be
several times the largest message before the writer has any room to run ahead of
its readers. A writer that fans a 64 KB message out to eight readers therefore
holds eight lanes sized for 64 KB messages, and copies the payload into each one.

The **message board** is a second segment per writing process, holding
fixed-size payload slots. A message too large for a lane's ring goes to the
board once, and each recipient's lane carries an `ams::BoardrefMsg` — a byte
offset into the board and a payload length — in the position the message would
have occupied. Messages that fit ride the lanes inline exactly as before, so the
only thing that decides the path is the message's size against the lane's
`max_msg_size`.

A reader cannot tell which way a message arrived. `cd_poll_read_Step` resolves a
reference through `BoardResolve` and hands the *payload* to the handler the
inline path would have called.

The slot a message occupies is charged to every recipient, and returns to the
free list when the last of them releases it. Each lane-and-reader pair owns a
queue of the references outstanding to it; the writer sweeps that queue from the
head as the reader's lane position passes each entry, so reclaim is the writer's
alone and a board reader writes nothing to the segment at all. A reader's queue
length is its allowance, which is what makes the sizing rule exact: a board at
least as large as the sum of its readers' allowances can never fail to place a
message, because every reader's outstanding set fits at once by construction. A
reader that stops consuming spends its own allowance and no one else's.

A board is inert to the read path, and it is inert because nothing ever asks it
for a message. It is not placed on the read poll list, and it has no members: a
member slot records a reader's consume position, and a board reader has none, so
the member table is sized to zero and refuses any later registration. Those two
facts are the whole of the guarantee. Its `woff` does stay zero for the life of
the segment, but that is a property of a board rather than a second line of
defence — a reader's offset is read before any offset comparison is made, so a
board that reached the poll list would not read as empty. `amsspy -dump` reports a board as its
slot count and slot size rather than as a ring, and reports the write budget it
does not have as `n/a`.

### Two-tier polling
<a href="#two-tier-polling"></a>

`cd_poll_read_Step()` is the main read loop. For each shm on the hot-poll list,
it calls `PeekMsg` and, if a message is available, process up to `burst` (default 10)
messages before rotating to the next shm for fairness.

When `signaled` is enabled, idle shms (no message found) are moved to
`cd_slow_poll_read`, which is checked with a longer delay (`cpu_hz / 1024` cycles).
As soon as a slow-polled shm has data, it is promoted back to hot poll.

### Heartbeats and liveness
<a href="#heartbeats-and-liveness"></a>

The control process monitors child process liveness and kills unresponsive children
(disk-writing processes are given a longer grace period). After killing a child,
the control process publishes a message so writers can remove the dead reader from
their shmember table.

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

### Message board performance
<a href="#message-board-performance"></a>

The two paths can only be told apart when the writer holds a lane per reader. A
lane every reader shares costs one write however many readers consume it, so a
board saves nothing there. `ams_sendtest -uc` gives every reader a lane of its
own, which is the shape a real fan-out has:

```
bin/ams_sendtest -uc -nchild:<N> -nmsg:500000 -msgsize_min:65536 -msgsize_max:65537 [-board]
```

Measured on an AMD EPYC 7702P at 3.3 GHz with no thread pinning, one warmup run
and then the best of three — the first run of a shape is measurably slower than
the rest, so a table without the warmup understates every figure. Throughput is
payload bytes delivered to every reader over wall time; latency is each reader's
average one-way delivery time, averaged over the readers. The writer is refused a
few dozen times in 200,000 sends at either path, so these figures measure copy
bandwidth and not flow control.

64 KB messages:

|readers|ring MB/s|board MB/s|ring latency|board latency|
|---|---|---|---|---|
|1|17759|12334|2.6 us|4.4 us|
|2|20863|23910|3.6 us|4.4 us|
|4|26558|40071|5.7 us|5.4 us|
|8|28594|73112|10.1 us|5.8 us|

8 KB messages, with `-nmsg:2000000 -msgsize_min:8192 -msgsize_max:8193`:

|readers|ring MB/s|board MB/s|ring latency|board latency|
|---|---|---|---|---|
|1|7896|8319|1.1 us|1.2 us|
|2|11671|13449|1.3 us|1.4 us|
|4|11107|16700|2.1 us|2.0 us|
|8|11078|21732|3.6 us|2.7 us|

The board carries one fixed extra cost, and it is a single payload copy: the
inline path formats in place through the zero-copy `LogMsg_FmtShm`, while the
board path formats into a scratch buffer and copies that into the slot. At one
reader at 64 KB nothing offsets that copy, and the board is 31% slower. From two
readers up the inline path pays a payload copy per recipient while the board pays
one whatever the fan-out, so the board pulls ahead and keeps pulling: at eight
readers it moves 2.6 times the ring's aggregate and its latency is 43% lower. At
8 KB the copy costs proportionally less, and the board is level at one reader and
twice as fast at eight.

Shared memory for the same fan-out, sampled from `/dev/shm` while the run is in
flight. The ring path must size every lane for 64 KB messages. The board path
sizes its lanes for the references they carry — that is what `-bufsize:32768`
says — and the payloads live once in the board:

|readers|ring bytes|board bytes (lanes + board)|
|---|---|---|
|1|1,118,273|307,460|
|2|2,236,546|610,824|
|4|4,473,092|1,217,552|
|8|8,946,184|2,431,008|

A board grows with its readers as well, because it is sized for the sum of their
allowances, so the ratio stays near 3.7x rather than widening with the fan-out.
What the board removes is the room every lane had to reserve for a message it
now never carries.

### AMS Shm Tracing
<a href="#ams-shm-tracing"></a>

All messages read from the AMS bus can be traced with the `ams` logcat.

## ams_sendtest - Algo Messaging System test tool


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- abt_md.toc_beg -->
&nbsp;&nbsp;&bull;&nbsp;  [Internals](#internals)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Options](#options)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
<!-- abt_md.toc_end -->

### Internals
<a href="#internals"></a>
&#128196; [ams_sendtest - Internals](/txt/gen/ams_sendtest/ams_sendtest.md)<br/>

### Options
<a href="#options"></a>
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

#### -id -- Process index (0=parent)
<a href="#-id"></a>

#### -file_prefix -- Use file_prefix
<a href="#-file_prefix"></a>

#### -nchild -- Number of stream readers
<a href="#-nchild"></a>

#### -blocking -- Use blocking send mode
<a href="#-blocking"></a>

#### -nmsg -- Number of messages to send/receive
<a href="#-nmsg"></a>

#### -trace -- Regx expression specifying what to trace
<a href="#-trace"></a>

#### -timeout -- Time limit for the send
<a href="#-timeout"></a>

#### -recvdelay_ns -- Pause nanoseconds between messages
<a href="#-recvdelay_ns"></a>

#### -senddelay_ns -- Pause nanoseconds between messages
<a href="#-senddelay_ns"></a>

#### -msgsize_min -- Minimum message length
<a href="#-msgsize_min"></a>

#### -msgsize_max -- Maximum message length
<a href="#-msgsize_max"></a>

#### -bufsize -- Shared memory buffer size
<a href="#-bufsize"></a>

#### -recvdelay -- Pause nanoseconds between messages
<a href="#-recvdelay"></a>

#### -signaled -- Enable signaled mode
<a href="#-signaled"></a>

#### -board -- Carry messages too big for the ring on the message board
<a href="#-board"></a>

#### -board_pin -- Board slots one reader may pin at once (board mode)
<a href="#-board_pin"></a>

#### -uc -- Unicast: one lane per reader instead of one shared lane
<a href="#-uc"></a>

### Inputs
<a href="#inputs"></a>
`ams_sendtest` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|

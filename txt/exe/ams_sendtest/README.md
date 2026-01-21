## ams_sendtest - Algo Messaging System test tool


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Toc -->
&nbsp;&nbsp;&bull;&nbsp;  [Syntax](#syntax)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Options](#options)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
&#128196; [ams_sendtest - Internals](/txt/exe/ams_sendtest/internals.md)<br/>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Toc -->

### Syntax
<a href="#syntax"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Syntax -->
```
ams_sendtest: Algo Messaging System test tool
Usage: ams_sendtest [options]
    OPTION         TYPE    DFLT    COMMENT
    -in            string  "data"  Input directory or filename, - for stdin
    -id            int     0       Process index (0=parent)
    -file_prefix   string  ""      Use file_prefix
    -nchild        int     1       Number of stream readers
    -blocking                      Use blocking send mode
    -nmsg          int     1000    Number of messages to send/receive
    -trace         regx    ""      Regx expression specifying what to trace
    -timeout       int     30      Time limit for the send
    -recvdelay_ns  int     0       Pause nanoseconds between messages
    -senddelay_ns  int     0       Pause nanoseconds between messages
    -msgsize_min   int     64      Minimum message length
    -msgsize_max   int     1024    Maximum message length
    -bufsize       int     32768   Shared memory buffer size
    -recvdelay     int     0       Pause nanoseconds between messages
    -verbose       flag            Verbosity level (0..255); alias -v; cumulative
    -debug         flag            Debug level (0..255); alias -d; cumulative
    -help                          Print help and exit; alias -h
    -version                       Print version and exit
    -signature                     Show signatures and exit; alias -sig

```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Syntax -->

### Options
<a href="#options"></a>

<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Options -->
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

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Options -->

### Inputs
<a href="#inputs"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Inputs -->
`ams_sendtest` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Inputs -->


## ams_sendtest - Algo Messaging System test tool


### Table Of Contents
<a href="#table-of-contents"></a>
* [Syntax](#syntax)
* [Options](#options)
* [Sources](#sources)
* [Inputs](#inputs)

### Syntax
<a href="#syntax"></a>
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
    -verbose       int             Verbosity level (0..255); alias -v; cumulative
    -debug         int             Debug level (0..255); alias -d; cumulative
    -help                          Print help and exit; alias -h
    -version                       Print version and exit
    -signature                     Show signatures and exit; alias -sig

```

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

### Sources
<a href="#sources"></a>
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/ams_sendtest.cpp](/cpp/ams_sendtest.cpp)||
|[cpp/gen/ams_sendtest_gen.cpp](/cpp/gen/ams_sendtest_gen.cpp)||
|[include/ams_sendtest.h](/include/ams_sendtest.h)||
|[include/gen/ams_sendtest_gen.h](/include/gen/ams_sendtest_gen.h)||
|[include/gen/ams_sendtest_gen.inl.h](/include/gen/ams_sendtest_gen.inl.h)||

### Inputs
<a href="#inputs"></a>
`ams_sendtest` takes the following tables on input:
|ssimfile|comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|


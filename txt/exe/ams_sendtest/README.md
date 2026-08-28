## ams_sendtest - Algo Messaging System test tool


### Syntax
<a href="#syntax"></a>
```usage
ams_sendtest: Algo Messaging System test tool
Usage: ams_sendtest [options]
    OPTION         TYPE    DFLT    COMMENT
    -in            string  "data"  Input directory or filename, - for stdin
    -id            int     0       Process index (0=parent)
    -file_prefix   string  ""      Use file_prefix
    -nchild        int     1       Number of stream readers
    -blocking                      Use blocking send mode
    -nmsg          int     100000  Number of messages to send/receive
    -trace         regx    ""      Regx expression specifying what to trace
    -timeout       int     30      Time limit for the send
    -recvdelay_ns  int     0       Pause nanoseconds between messages
    -senddelay_ns  int     0       Pause nanoseconds between messages
    -msgsize_min   int     64      Minimum message length
    -msgsize_max   int     256     Maximum message length
    -bufsize       int     655360  Shared memory buffer size
    -recvdelay     int     0       Pause nanoseconds between messages
    -signaled                      Enable signaled mode
    -board                         Carry messages too big for the ring on the message board
    -board_pin     int     4       Board slots one reader may pin at once (board mode)
    -uc                            Unicast: one lane per reader instead of one shared lane
    -verbose       flag            Verbosity level (0..255); alias -v; cumulative
    -debug         flag            Debug level (0..255); alias -d; cumulative
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

#### -signaled -- Enable signaled mode
<a href="#-signaled"></a>

#### -board -- Carry messages too big for the ring on the message board
<a href="#-board"></a>

#### -board_pin -- Board slots one reader may pin at once (board mode)
<a href="#-board_pin"></a>

#### -uc -- Unicast: one lane per reader instead of one shared lane
<a href="#-uc"></a>

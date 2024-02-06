## ams_sendtest: Algo Messaging System test tool



### Syntax

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

#### -in -- Input directory or filename, - for stdin


#### -id -- Process index (0=parent)


#### -file_prefix -- Use file_prefix


#### -nchild -- Number of stream readers


#### -blocking -- Use blocking send mode


#### -nmsg -- Number of messages to send/receive


#### -trace -- Regx expression specifying what to trace


#### -timeout -- Time limit for the send


#### -recvdelay_ns -- Pause nanoseconds between messages


#### -senddelay_ns -- Pause nanoseconds between messages


#### -msgsize_min -- Minimum message length


#### -msgsize_max -- Maximum message length


#### -bufsize -- Shared memory buffer size


#### -recvdelay -- Pause nanoseconds between messages

### Inputs

`ams_sendtest` takes the following tables on input:
```
CTYPE                COMMENT
dmmeta.Dispsigcheck  Check signature of input data against executable's version
```


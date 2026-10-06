## amsspy - List ams sessions and monitor traffic on host
<a href="#amsspy"></a>
amsspy inspects the [AMS](/txt/lib/lib_ams/README.md) shared memory segments on this host.
It lists them, prints the flow-control state of each one, and prints the messages written
to a segment as they arrive.  It reads the segments without joining them as a reader, so
it never slows a writer down.  It can also remove the segments that crashed processes left
behind.

### Syntax
<a href="#syntax"></a>
```usage
amsspy: List ams sessions and monitor traffic on host
Usage: amsspy [options]
    OPTION      TYPE    DFLT    COMMENT
    -in         string  "data"  Input directory or filename, - for stdin
    -session    regx    "%"     Session regex
    -list                       List sessions
    -shm                        List shms
    -spy        regx    ""      Spy on named shared memory segment
    -f          enum    decode  (output) Output format (decode|raw|text|json)
    -clean                      Unlink orphaned /dev/shm ams segments and exit
    -dump                       Dump every segment's header + reader offsets/lag (out-of-band) and exit
    -verbose    flag            Verbosity level (0..255); alias -v; cumulative
    -debug      flag            Debug level (0..255); alias -d; cumulative
    -trace      string  ""      Trace expression: category[:filter],...; also payload_lim:N, verbose, debug, timestamps
    -help                       Print help and exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig
```

### Description
<a href="#description"></a>

Each AMS segment is a file `/dev/shm/<shm>.ams`.  Its name starts with the file prefix
its owner was given, and amsspy calls the processes sharing one prefix a session.  A run
of [ams_sendtest](/txt/exe/ams_sendtest/README.md) with `-file_prefix:demo` is the session
`demo`, and its lane is the segment `demo-ams_sendtest-0-0.log-0`.

amsspy does one of four things:

- `-list` prints the sessions, and with `-shm` the segments of each.  It is also what
  amsspy does when no other mode is given.
- `-dump` prints one `amsspy.dump` line per segment and one indented line per reader.  The
  segment line carries the write offset, the room the writer has left, and the writer's
  pid.  Each reader line carries the reader's offset and its lag behind the writer, so
  the reader with the largest lag is the one holding the writer back.  One indented
  `channel` line per shm channel follows: its key and reader, the bytes written and
  read on it, the limit the reader set and the window it follows the read count by,
  the writer's room under that limit, and the bytes written and not yet read.
- `-spy` prints every message written to the matching segments from now on, until you
  stop it.
- `-clean` removes the segments whose writer is gone, and prints how many it removed.

### Examples
<a href="#examples"></a>

```bash
amsspy                                  # list the sessions on this host
amsspy -list -shm                       # list sessions with their segments
amsspy -dump -session:demo              # offsets and reader lag of session demo
amsspy -spy:'demo%'                     # print messages as session demo writes them
amsspy -spy:'demo%' -f:text -v          # type, length and bytes, with segment and offset
amsspy -clean                           # remove segments left by dead writers
```

While `ams_sendtest -file_prefix:demo -nchild:2 -recvdelay_ns:1000000 -slowreader` runs,
the dump shows reader 1 lagging behind reader 2:

```
amsspy.dump  shm:demo-ams_sendtest-0-0.log-0  woff:644672  budget:617792  nchunk:0  chunk_size:0  nnobudget:0  nblock:0  writer_pid:1369372  eof:N  n_reader:2
    reader  grpmember:ams_sendtest-0-0.log-0/ams_sendtest-0-1,r  pid:1369373  offset:214528  lag:430144  sleeping:0
    reader  grpmember:ams_sendtest-0-0.log-0/ams_sendtest-0-2,r  pid:1369374  offset:644672  lag:0  sleeping:0
```

### Caveats
<a href="#caveats"></a>

- `-spy` starts at the segment's current end, so it shows only messages written after it
  starts.  When the writer laps it, amsspy prints `amsspy.spy_overrun` and skips ahead.
- `-f:json` prints nothing.
- `-session` filters `-dump` alone.  `-list` always prints every session.

### Options
<a href="#options"></a>
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

amsspy reads no ssim data, so this option has no effect.

#### -session -- Session regex
<a href="#-session"></a>

With `-dump`, an SQL-style regx of the sessions to print, `%` by default.

#### -list -- List sessions
<a href="#-list"></a>

Prints the name of every session on the host.  amsspy lists sessions whenever `-spy`
matches no segment, so plain `amsspy` does the same.

#### -shm -- List shms
<a href="#-shm"></a>

With `-list`, prints the segments of each session, indented under its name.

#### -spy -- Spy on named shared memory segment
<a href="#-spy"></a>

An SQL-style regx of segment names.  amsspy maps every matching segment read-only and
prints each new message in the `-f` format until you stop it.  Messages from several
segments interleave in the output.

#### -f -- (output) Output format
<a href="#-f"></a>

The format `-spy` prints messages in:
- `decode`: one ssim tuple per message (the default).
- `text`: the message type, its length and its bytes as an ssim string.
- `raw`: the message bytes, unchanged, for piping into another AMS tool.
- `json`: prints nothing.

#### -clean -- Unlink orphaned /dev/shm ams segments and exit
<a href="#-clean"></a>

Removes every segment whose writer has exited without removing it.  The line it prints,
`amsspy.clean`, counts the segments before and after.  A segment whose writer still
holds it is left alone.

#### -dump -- Dump every segment's header + reader offsets/lag (out-of-band) and exit
<a href="#-dump"></a>

Prints the flow-control state of every segment in the sessions `-session` selects.  It
reads the segments directly, so it works when the processes that own them have stopped
answering.  A message board segment has no ring, so its `budget` reads `n/a` and it
reports its chunk count instead.

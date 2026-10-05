## ams_sendtest - Algo Messaging System test tool
<a href="#ams_sendtest"></a>
ams_sendtest tests and measures [AMS](/txt/lib/lib_ams/README.md) shared memory transport.
One writer process sends a numbered stream of messages to several reader processes, and
each reader checks every message it gets.  Each process prints a report with its counts and
its latency or send rate, and the run fails when any message is lost or corrupted.

### Syntax
<a href="#syntax"></a>
```usage
ams_sendtest: Algo Messaging System test tool
Usage: ams_sendtest [options]
    OPTION           TYPE    DFLT    COMMENT
    -in              string  "data"  Input directory or filename, - for stdin
    -id              int     0       Process index (0=parent)
    -file_prefix     string  ""      Use file_prefix
    -nchild          int     1       Number of stream readers
    -blocking                        Use blocking send mode
    -nmsg            int     100000  Number of messages to send/receive
    -timeout         int     30      Time limit for the send
    -recvdelay_ns    int     0       Pause nanoseconds between messages
    -senddelay_ns    int     0       Pause nanoseconds between messages
    -msgsize_min     int     64      Minimum message length
    -msgsize_max     int     256     Maximum message length
    -bufsize         int     655360  Shared memory buffer size
    -recvdelay       int     0       Pause nanoseconds between messages
    -slowreader                      Only the first reader takes recvdelay_ns
    -signaled                        Enable signaled mode
    -board                           Carry every message on the message board, one reference per lane
    -board_chunkref  int     8       Chunks one lane may reference at once (board mode)
    -board_bufsize   int     655360  Message board body size in bytes (board mode)
    -chunk_size      int     65536   Board chunk size in bytes (board mode)
    -uc                              Unicast: one lane per reader instead of one shared lane
    -channel                         Pace each reader's messages by a shm channel it grants
    -channel_window  int     4096    Bytes a reader grants past what it has read (channel mode)
    -parkwrite                       Send from a step that sleeps when refused; only a reader's wake resumes it
    -verbose         flag            Verbosity level (0..255); alias -v; cumulative
    -debug           flag            Debug level (0..255); alias -d; cumulative
    -trace           string  ""      Trace expression: category[:filter],...; also payload_lim:N, verbose, debug, timestamps
    -help                            Print help and exit; alias -h
    -version                         Print version and exit
    -signature                       Show signatures and exit; alias -sig
```

### Description
<a href="#description"></a>

You start the parent, and the parent does the rest.  It creates a shared memory segment
under `/dev/shm`, named after `-file_prefix`, and registers `-nchild` readers on it.  It
then starts one copy of itself per reader, with `-id` set to that reader's index.  Once
every reader is attached, the parent writes `-nmsg` `ams.LogMsg` messages, each numbered
and padded to a length between `-msgsize_min` and `-msgsize_max`.

Each reader checks that every message carries the next number and intact padding.  It
also measures how long each message took to arrive.  A reader stops once it has read
every message, and the parent waits for all of them and removes the segments it created.

Three layouts of the transport can be tested:

- By default all readers share one lane, a ring buffer that the parent writes once.
- With `-uc` each reader gets a lane of its own, and the parent writes every message to
  each of them.
- With `-board` the parent writes each message once into a message board, and the lanes
  carry only references to it.  Combine it with `-uc` to measure what the board saves.

`-channel` adds a shm channel per reader on the lane it reads, keyed by the reader's
index.  Message n belongs to reader n mod nchild + 1 and is written on that reader's
channel; on any other lane it goes on the lane's base channel.  Each reader sets its
channel a window of `-channel_window` bytes past what it has read, so the parent is
paced by each reader's reading.  At the end every channel must hold `nwrite == nread`
and `nwrite <= wlim`, and the parent prints the channel table.  The window must exceed
the ring's largest message, `-msgsize_max` plus 64, or the run is refused at the start
with `ams_sendtest.badwindow`.  `-channel` does not combine with `-board`:

```
ams_sendtest -channel -nchild:3 -nmsg:20000
```

`-parkwrite` turns a run into a test of the writer's wake.  The parent sends from a step
that keeps its loop awake only while it makes progress; refused for room, on the lane
or on a channel, it parks and sleeps, and only a reader's wake signal runs it again.
It needs `-signaled`.  Behind a slow reader the writer parks many times, so a wake
that goes missing leaves it asleep until `-timeout`, and the run fails:

```
ams_sendtest -signaled -parkwrite -recvdelay_ns:200000 -bufsize:32768 -nmsg:2000
ams_sendtest -signaled -parkwrite -recvdelay_ns:200000 -channel -channel_window:1024 -nmsg:2000
```

Every process prints one `report.ams_sendtest` line.  A reader reports its average
latency in `latency_ns`.  The parent reports its send rate in `msg_per_s` and `mb_per_s`,
`n_write_wait`, the number of times a lane had no room, and `n_limit_wait`, the number of
times a channel's limit refused a message every lane had room for, and `n_writer_park`,
the times the writer parked and slept until a reader woke it.  `success:Y` in the parent's
report means that every message was sent and every reader exited 0, and the exit code
says the same.

### Examples
<a href="#examples"></a>

```bash
ams_sendtest -nmsg:1000 -nchild:2                  # quick check with two readers
ams_sendtest -nchild:4 -uc -board                  # four lanes fed from a message board
ams_sendtest -nchild:4 -uc                         # the same without the board, to compare
ams_sendtest -nchild:2 -recvdelay_ns:1000000 -slowreader -nmsg:3000   # one reader lags
ams_sendtest -nmsg:1000 -nchild:2 | grep report    # just the verdicts
```

The last command prints one line per process:

```
report.ams_sendtest  proc:ams_sendtest-0-1  n_msg:1000  n_msg_send:0  n_msg_recv:1000  n_write_wait:0  woff:214848  roff:214848  latency_ns:3920.77  success:Y  ...
report.ams_sendtest  proc:ams_sendtest-0-2  n_msg:1000  n_msg_send:0  n_msg_recv:1000  n_write_wait:0  woff:214848  roff:214848  latency_ns:3943.98  success:Y  ...
report.ams_sendtest  proc:ams_sendtest-0-0  n_msg:1000  n_msg_send:1000  n_msg_recv:0  n_write_wait:0  woff:214848  roff:0  latency_ns:0  success:Y  send_s:8.96802e-05  msg_per_s:1.11507e+07  mb_per_s:1776.76
```

While a run is in progress, [amsspy](/txt/exe/amsspy/README.md) `-dump` shows each
reader's lag.

### Caveats
<a href="#caveats"></a>

- A run that has not finished by `-timeout` seconds fails, and on failure every process
  prints a dump of its segments.  Raise `-timeout` for large `-nmsg` with slow readers.
- A parent that is killed leaves its segments in `/dev/shm`.  `amsspy -clean` removes
  them.

### Options
<a href="#options"></a>
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

ams_sendtest reads no ssim data, so this option has no effect.

#### -id -- Process index (0=parent)
<a href="#-id"></a>

The index of this process, 0 for the parent.  The parent starts each reader with its own
index from 1 up, so you only ever run the parent yourself.

#### -file_prefix -- Use file_prefix
<a href="#-file_prefix"></a>

The prefix of the shared memory file names.  The parent defaults it to
`ams_sendtest_<pid>`, so runs do not collide.  A reader needs it, and the parent passes it
on.

#### -nchild -- Number of stream readers
<a href="#-nchild"></a>

The number of reader processes the parent starts, 1 by default.

#### -blocking -- Use blocking send mode
<a href="#-blocking"></a>

Makes the parent wait for room in the lane before each send.  Without it a send that finds
no room is retried on the next pass and counted in `n_write_wait`.

#### -nmsg -- Number of messages to send/receive
<a href="#-nmsg"></a>

The number of messages the parent sends and every reader must receive, 100000 by default.

#### -timeout -- Time limit for the send
<a href="#-timeout"></a>

The time limit of each process, in seconds.  A process that has not finished by then
stops, and its report says `success:N`.

#### -recvdelay_ns -- Pause nanoseconds between messages
<a href="#-recvdelay_ns"></a>

Makes each reader spin for this many nanoseconds after every message.  Use it to make the
readers slower than the writer, so the lane fills up.

#### -senddelay_ns -- Pause nanoseconds between messages
<a href="#-senddelay_ns"></a>

The pause between the parent's sends, in nanoseconds.  A long pause keeps a run going long
enough to watch it with amsspy.

#### -msgsize_min -- Minimum message length
<a href="#-msgsize_min"></a>

The shortest message the parent sends, in bytes.  Lengths vary between this and
`-msgsize_max`, and the same message number always gets the same length.

#### -msgsize_max -- Maximum message length
<a href="#-msgsize_max"></a>

The longest message the parent sends, in bytes.  It is raised to `-msgsize_min` plus one
when it is not larger.

#### -bufsize -- Shared memory buffer size
<a href="#-bufsize"></a>

The size of each lane in bytes, 640 KB by default.  A smaller lane makes the writer wait
on the readers sooner.

#### -recvdelay -- Pause nanoseconds between messages
<a href="#-recvdelay"></a>

Has no effect.  Use `-recvdelay_ns`.

#### -slowreader -- Only the first reader takes recvdelay_ns
<a href="#-slowreader"></a>

Applies `-recvdelay_ns` to reader 1 alone, so one reader lags while the others keep up.
Use it to see how one slow reader holds back the writer.

#### -signaled -- Enable signaled mode
<a href="#-signaled"></a>

Runs every process in AMS signaled mode, where a reader with nothing to read sleeps until
the writer wakes it.  Without it readers poll.

#### -board -- Carry every message on the message board, one reference per lane
<a href="#-board"></a>

Writes each message once into a message board segment and puts only a reference to it in
each lane.  Readers resolve the reference and check the message as usual.

#### -board_chunkref -- Chunks one lane may reference at once (board mode)
<a href="#-board_chunkref"></a>

With `-board`, the number of board chunks one lane may hold references into at once, 8 by
default.  It bounds how much of the board one stopped reader can pin.

#### -board_bufsize -- Message board body size in bytes (board mode)
<a href="#-board_bufsize"></a>

With `-board`, the size of the board in bytes, 640 KB by default.  It plays the part that
`-bufsize` plays for a lane: it is how far readers may fall behind before the writer
waits.

#### -chunk_size -- Board chunk size in bytes (board mode)
<a href="#-chunk_size"></a>

With `-board`, the size of one board chunk in bytes, 64 KB by default.  The board is
divided into chunks of this size.

#### -uc -- Unicast: one lane per reader instead of one shared lane
<a href="#-uc"></a>

Gives each reader a lane of its own, which the parent writes separately.  This is the
layout of a real fan-out, and the one in which `-board` makes a difference.

#### -channel -- Pace each reader's messages by a shm channel it grants
<a href="#-channel"></a>

#### -channel_window -- Bytes a reader grants past what it has read (channel mode)
<a href="#-channel_window"></a>

#### -parkwrite -- Send from a step that sleeps when refused; only a reader's wake resumes it
<a href="#-parkwrite"></a>

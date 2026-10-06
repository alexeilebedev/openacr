## atf_nrun - Run N subprocesses in parallel
<a href="#atf_nrun"></a>
`atf_nrun` is a small sample program that runs a number of child jobs with a
cap on how many run at once.  It exists to show how a program built on
generated steps schedules work, and
[Tutorial 5](/txt/tut/tut05.md) walks through its source.

### Syntax
<a href="#syntax"></a>
```usage
atf_nrun: Run N subprocesses in parallel
Usage: atf_nrun [[-ncmd:]<int>] [options]
    OPTION      TYPE    DFLT    COMMENT
    -in         string  "data"  Input directory or filename, - for stdin
    -maxjobs    int     2       Number of simultaneous jobs
    [ncmd]      int     6
    -verbose    flag            Verbosity level (0..255); alias -v; cumulative
    -debug      flag            Debug level (0..255); alias -d; cumulative
    -trace      string  ""      Trace expression: category[:filter],...; also payload_lim:N, verbose, debug, timestamps
    -help                       Print help and exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig
```

### Description
<a href="#description"></a>

`atf_nrun` makes `-ncmd` jobs, where job number `i` is the shell command
`echo i; sleep 1`.  It starts jobs until `-maxjobs` are running, and each time a
child exits it starts the next one.  It prints an `atf_nrun.spawn` line for
every job it starts and an `atf_nrun.sigchild` line with the exit status of
every child it collects.  It exits when every job has finished.  It reads no
tables and writes no files.

### Examples
<a href="#examples"></a>

```bash
atf_nrun                  # six jobs, two at a time: about three seconds
atf_nrun 3 -maxjobs:2     # three jobs, two at a time
atf_nrun 10 -maxjobs:10   # ten jobs at once: about one second
```

The second command prints:

```
atf_nrun.spawn  command:"echo 0; sleep 1"  pid:1355008  ntodo:2  nrunning:1
atf_nrun.spawn  command:"echo 1; sleep 1"  pid:1355009  ntodo:1  nrunning:2
0
1
SIGCHLD
atf_nrun.sigchild  pid:1355008  status:0
atf_nrun.spawn  command:"echo 2; sleep 1"  pid:1355035  ntodo:0  nrunning:2
atf_nrun.sigchild  pid:1355009  status:0
2
SIGCHLD
atf_nrun.sigchild  pid:1355035  status:0
```

### Options
<a href="#options"></a>
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

The data directory every generated tool accepts.  `atf_nrun` loads no tables
from it.

#### -maxjobs -- Number of simultaneous jobs
<a href="#-maxjobs"></a>

The most jobs that run at once.  With a value of one the jobs run one after
another.

#### -ncmd -- 
<a href="#-ncmd"></a>

The number of jobs to run, given by position or as `-ncmd:N`.  The default is
six.

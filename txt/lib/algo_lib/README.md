## algo_lib - Support library for all executables


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- abt_md.toc_beg -->
&nbsp;&nbsp;&bull;&nbsp;  [Internals](#internals)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Printing / Process output](#printing-process-output)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Printing to stderr](#printing-to-stderr)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Log categories](#log-categories)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Verblog](#verblog)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Dbglog](#dbglog)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Environment variables](#environment-variables)<br/>
&#128196; [algo_lib.FIohook -](/txt/lib/algo_lib/FIohook.md)<br/>
<!-- abt_md.toc_end -->

### Internals
<a href="#internals"></a>
&#128196; [algo_lib - Internals](/txt/gen/algo_lib/algo_lib.md)<br/>

### Printing / Process output
<a href="#printing-process-output"></a>

The standard way to print something to `stdout` is with the use of `prlog` macro.
Example
```
prlog("hello, world");
```

Prlog uses a temporary string, so that convenient `<<` expressions are possible:

```
prlog("hello, world, my name is "<< _db.cmdline.name);
```

prlog automatically appends a newline; to omit the newline, use the variant `prlog_`.
Prlog will automatically prefix each printed line with a nanosecond-level timestamp,
if the variable `algo_lib::_db.show_tstamp` is set.

Additionally, all `*log` macros ultimately call a function through the global pointer
`algo_lib::_db.Prlog`, which can be overridden to redirect output to some other channel.

### Printing to stderr
<a href="#printing-to-stderr"></a>

The prlog equivalent for stderr output is `prerr`.
prerr automatically appends a newline; to omit the newline, use the variant `prerr_`.

### Log categories
<a href="#log-categories"></a>

`algo_lib` has a convenient notion of `log category`, controlled by table `dev.logcat`.
Each log category has a name and an `enable` flag, which can be switched at runtime.
Printing to a log category xyz is performed with

```
prcat(xyz, "some output");
```

And is equivalent to

```
if (algo_lib_logcat.xyz.enable) {
   prlog("some output");
}
```

Stdout and stderr are generalized as log categories.
The table of log categories is given below:

```
inline-command: acr logcat:'(stdout|stderr|debug|verbose|verbose2)' -report:N | ssimfilt -t
LOGCAT    ENABLED  BUILTIN  STDOUT  MAXMSG  WINDOW  SUPPRESS  COMMENT
debug     N        N        N       0       0       N         Debug messages
stderr    Y        Y        N       0       0       N         Standard error
stdout    Y        Y        Y       0       0       N         Standard output
verbose   N        N        N       0       0       N         Verbose messages go here
verbose2  N        N        N       0       0       N         Extra verbose messages go here

```

### Verblog
<a href="#verblog"></a>

The macro `verblog(x)` is equivlent to `if (algo_lib::_db.cmdline.verbose) { prlog(x); }`

Verbose mode is enabled with flags `-v` or `-verbose`.
Multiple `-v` flags increment the verbose level to be higher.

When spawning a known amc-compatible subprocess using amc-generated code, the verbosity level
is automatically decremented by one. Thus, running a process with `-v -v` passes `-v` to each spawned
subprocess, which can be very useful to look "deeper" into the run.

### Dbglog
<a href="#dbglog"></a>

The macro `dbglog(x)` is equivlent to `if (algo_lib::_db.cmdline.debug) { prlog(x); }`

Debug mode is enabled with flags `-d` or `-debug
Multiple `-d` flags increment the debug level to be higher.
Debug level is similar to verbose level and is passed down to child processes on command line,
without being decremented.

### Environment variables
<a href="#environment-variables"></a>

We specifically avoid using the environment block to pass verbose/debug flags, in order to maintain
an exact correspondence between the command line and the process behavior.

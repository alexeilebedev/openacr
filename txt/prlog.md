## Printing / Process output

The standard way to print something to `stdout` is with the use of `prlog` macro.
Example
```
prlog("hello, world");
```

Prlog constructs a temporary string, so that convenient `<<` expressions
are possible

```
prlog("hello, world, my name is "<< _db.cmdline.name);
```

### Printing to stderr

The prlog equivalent for stderr output is `prerr`.

### Log categories

`algo_lib` has a convenient notion of `log category`, controlled by table `dev.logcat`.
Each log category has a name and an `enable` flag, which can be switched at runtime.
Printing to a log category xyz is performed with

```
prlog_cat(algo_lib_logcat_xyz, "some output");
```

And is equivalent to

```
if (algo_lib_logcat.xyz.enable) {
   prlog("some output");
}
```

Log categories can be enabled with a SQL regular expression passed via the `-trace` command line option

### The table of log categories is given below:

```
inline-command: acr logcat
dmmeta.logcat  logcat:expect  enabled:N  builtin:N  comment:"lib_ams expect implementation"
dmmeta.logcat  logcat:stderr  enabled:Y  builtin:Y  comment:"Standard error (cannot be disabled)"
dmmeta.logcat  logcat:stdout  enabled:Y  builtin:Y  comment:"Standard output (cannot be disabled)"
report.acr  n_select:3  n_insert:0  n_delete:0  n_ignore:0  n_update:0  n_file_mod:0
```

### Verblog

The macro `verblog(x)` is equivlent to `if (algo_lib::_db.cmdline.verbose) { prlog(x); }`

Verbose mode is enabled with flags `-v` or `-verbose`.
Multiple `-v` flags increment the verbose level to be higher.

When spawning a known amc-compatible subprocess using amc-generated code, the verbosity level
is automatically decremented by one. Thus, running a process with `-v -v` passes `-v` to each spawned
subprocess, which can be very useful to look "deeper" into the run.

### Dbglog

The macro `dbglog(x)` is equivlent to `if (algo_lib::_db.cmdline.debug) { prlog(x); }`

Debug mode is enabled with flags `-d` or `-debug
Multiple `-d` flags increment the debug level to be higher.

Debug level is similar to verbose level and is passed down to child processes on command line.

### Design / Bugs

We specifically avoid using the environment block to pass verbose/debug flags, in order to maintain
an exact correspondence between the command line and the process behavior.

There is unfortunately no clear rule that says whether process output should go to
stdout or stderr. 

Log category names are a mess


## mdbg: Gdb front-end



### Syntax

```
mdbg: Gdb front-end
Usage: mdbg [-target:]<string> [[-args:]<string>] [options]
    OPTION         TYPE    DFLT     COMMENT
    [target]       string           Executable name
    -in            string  "data"   Input directory or filename, - for stdin
    [args]...      string           Additional module args
    -cfg           string  "debug"  Configuration to use
    -disas                          Show disassembly (use F12)
    -attach                         Attach to a running process
    -b...          string           Set breakpoint, e.g. 'a.cpp:123 if cond1', 'func#3'
    -catchthrow            Y        Stop on exceptions
    -tui                            Use gdb -tui as the debugger
    -bcmd          string  ""       Evaluate command at breakpoint
    -emacs                 Y        Use emacs environment as the debugger
    -manywin                        Use gdb-many-windows emacs mode
    -follow_child
    -py                             Enable python scripting
    -dry_run                        Print commands but don't execute
    -verbose       int              Verbosity level (0..255); alias -v; cumulative
    -debug         int              Debug level (0..255); alias -d; cumulative
    -help                           Print help and exit; alias -h
    -version                        Print version and exit
    -signature                      Show signatures and exit; alias -sig

```

### Description

This is a wrapper for automating the invocation of `gdb` from command line.

Mdbg uses abt to build a debug version of the debug target (By default,
this means `debug`, but the option can be customized with `-cfg <cfg>`, for instance
specifying `-cfg profile`).

Then, mdbg prepares a debugging environment by writing two script files: one
for the debugger, `temp/mdbg.gdb`, and one for the editor, `temp/mdbg.el`.
The default, and really the most supported editor, is emacs; specifying `-tui`
will switch to the gdb's Text User Interface mode instead.

Mdbg then runs the debug target in the debug environment. It can optionally set a regular
or conditional breakpoint in a function other than Main with the `-b` option.

Let's consider an example:

    $ acr_ed -create -target sample -write
    ...
    $ mdbg sample
    
Mdbg will print a reminder of the shortcuts it has equipped the target editor with...

    mdbg.note  Debug script saved to temp/mdbg.el
    mdbg.note  Invoking gdb under emacs. Make sure to link ~/.emacs -> conf/emacs.el, ~/elisp -> conf/elisp
    mdbg.note  Standard shortcuts:
    mdbg.note               F7  recompile and restart executable
    mdbg.note              F11  step into
    mdbg.note        Shift-F11  step out
    mdbg.note              F10  step over
    mdbg.note         Ctrl-F10  run to cursor
    mdbg.note              F12  restore windows
    mdbg.note              F9   set breakpoint
    mdbg.note         Ctrl-F9   clear breakpoint
    mdbg.note              F6   other window
    mdbg.note    Alt-Up Arrow   Go up the call stack
    mdbg.note  Alt-Down Arrow   Go down the call stack

and run the target program under the debugger, stopping at Main.

      // --- sample...main
      int main(int argc, char **argv) {
          try {
    B =>      algo_lib::FDb_Init();
              sample::FDb_Init();
              algo_lib::_db.argc = argc;
      ...

The `-manywin` option enables emacs' gdb's `gud-many-windows` mode, which brings up
a debugger layout reminiscent of Visual Studio or Eclipse, with locals, breakpoints, 
threads subwindows, etc.
Additionally specifying the `-disas` option enables the disassembly window.

### Disambiguation

There exists a microsoft-provided tool named mdbg. Please make sure
this tool is first in PATH in order to avoid confusion.

### Specifying arguments

The first argument to mdbg is the debug target name. Mdbg must know it so it can build it.
To pass arguments to the debug target, specify them in a single string like this:
 
    $ mdbg sample " -in:filename"

Always include a space in front of the arguments: since Bash will strip the quotes, mdbg
needs to know that the specified option is not an mdbg option but in fact an option for the debug
target. To avoid any ambiguity, you could write `-args:-in:filename`, but it seems less 
convenient than passing the arguments directly.

### The -tui option

To use gdb's text UI, use `mdbg -tui`

### Specifying Breakpoints

The default breakpoint is Main, but others can be specified, including gdb's conditional breakpoints.
For instance, in the example above, we might invoke mdbg with a custom breakpoint.

    $ mdbg sample -b algo::Prlog 
    ...

      void algo::Prlog(int fd, cstring &str, int start, bool eol) {
          try {
    B =>      if (eol) {
                  str << '\n';
              }
    
Or, if we're interested only in output that goes to stderr,

    $ mdbg sample -b "algo::Prlog if fd==2"
    
Multiple breakpoints can be specified by comma-separating them. For instance `-b "A, B if c"`, etc.
To execute a command at a breakpoint, use `-bcmd` option:

    $ mdbg sample -b "algo::Prlog if fd==2" -bcmd "print str.ch_n"

These options preconfigure gdb via the `mdbg.gdb` file so that 
there is less typing later.

### Edit-and-retry

Mdbg sets up the standard shortcut F7 for edit-and-retry: the target is rebuilt
and reinvoked with the same options as initially. This can save a lot of set up, because
the gdb instance is not restarted; only the debug target.

### The -follow_child otption

This controlls gdb's `follow child` so that the debugger traces the child process instead
of the parent. By default, only the parent process is debugged.
This option is really most useful for debugging `fork()` calls; With fork/exec, it makes
the most sense to isolate the child process command line and debug it directly without
messing with the parent first.

### Exceptions

By default, gdb is configured to catch exceptions. To ignore them instead, specify `-catchthrow:N`

### Gdb Python Interface

To enable gdb python scriptability, specify `-py` option.

### Bugs

Because gdb and emacs don't form a standardized environment, it's possible that mdbg's integration
is broken in some way. Please file an issue, describing your config, so that this combination
can be tried out and fixed.

### Inputs

`mdbg` takes the following tables on input:
```
CTYPE                COMMENT
dev.Cfg
dev.Builddir         Directory where object files/executables go. Determines compile/link options
dmmeta.Dispsigcheck  Check signature of input data against executable's version
```

### Tests

The following component tests are defined for `mdbg`.
These can be executed with `atf_comp <comptest> -v`
```
COMPTEST             COMMENT
mdbg.OutOfOrderArgs
mdbg.Smoke
mdbg.SmokeBreak
mdbg.SmokeBreak2



```


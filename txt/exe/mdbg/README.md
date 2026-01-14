## mdbg - Gdb front-end


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Toc -->
&nbsp;&nbsp;&bull;&nbsp;  [Syntax](#syntax)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Description](#description)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Disambiguation](#disambiguation)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Specifying arguments](#specifying-arguments)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Edit-and-retry](#edit-and-retry)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Exceptions](#exceptions)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Bugs](#bugs)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Options](#options)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
&#128196; [mdbg - Internals](/txt/exe/mdbg/internals.md)<br/>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Toc -->

### Syntax
<a href="#syntax"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Syntax -->
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
    -pid           int     0        (with -attach) Pid, if omitted mdbg will guess
    -b...          string           Set breakpoint, e.g. 'a.cpp:123 if cond1', 'func#3'
    -catchthrow            Y        Stop on exceptions
    -tui                            Use gdb -tui as the debugger
    -bcmd          string  ""       Evaluate command at breakpoint
    -emacs                 Y        Use emacs environment as the debugger
    -manywin                        Use gdb-many-windows emacs mode
    -follow_child                   When forking, follow child (default is parent)
    -py                             Enable python scripting
    -dry_run                        Print commands but don't execute
    -mp                             Multi-process debugging
    -verbose       flag             Verbosity level (0..255); alias -v; cumulative
    -debug         flag             Debug level (0..255); alias -d; cumulative
    -help                           Print help and exit; alias -h
    -version                        Print version and exit
    -signature                      Show signatures and exit; alias -sig

```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Syntax -->

### Description
<a href="#description"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Description -->

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

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Description -->

### Disambiguation
<a href="#disambiguation"></a>

There exists a microsoft-provided tool named mdbg. Please make sure
this tool is first in PATH in order to avoid confusion.

### Specifying arguments
<a href="#specifying-arguments"></a>

The first argument to mdbg is the debug target name. Mdbg must know it so it can build it.
To pass arguments to the debug target, specify them in as anonymous arguments:a single string like this:
 
    $ mdbg sample -- -in:filename

### Edit-and-retry
<a href="#edit-and-retry"></a>

Mdbg sets up the standard shortcut F7 for edit-and-retry: the target is rebuilt
and reinvoked with the same options as initially. This can save a lot of set up, because
the gdb instance is not restarted; only the debug target.

### Exceptions
<a href="#exceptions"></a>

By default, gdb is configured to catch exceptions. To ignore them instead, specify `-catchthrow:N`

### Bugs
<a href="#bugs"></a>

Because gdb and emacs don't form a standardized environment, it's possible that mdbg's integration
is broken in some way. Please file an issue, describing your config, so that this combination
can be tried out and fixed.

### Options
<a href="#options"></a>

<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Options -->
#### -target -- Executable name
<a href="#-target"></a>

#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

#### -args -- Additional module args
<a href="#-args"></a>

#### -cfg -- Configuration to use
<a href="#-cfg"></a>

#### -disas -- Show disassembly (use F12)
<a href="#-disas"></a>

This option configures the environment to show disassembly when running

#### -attach -- Attach to a running process
<a href="#-attach"></a>

#### -pid -- (with -attach) Pid, if omitted mdbg will guess
<a href="#-pid"></a>

#### -b -- Set breakpoint, e.g. 'a.cpp:123 if cond1', 'func#3'
<a href="#-b"></a>

If this argument is omitted, the process will initially stop at `Main`.
Multiple breakpoints can be specified by using `-b` multiple times.

    $ mdbg sample -b algo::Prlog 
    ...

      void algo::Prlog(int fd, cstring &str, int start, bool eol) {
          try {
    B =>      if (eol) {
                  str << '\n';
              }
    
To use conditional breakpoints, use the following syntax:

    $ mdbg sample -b "algo::Prlog if fd==2"

#### -catchthrow -- Stop on exceptions
<a href="#-catchthrow"></a>

#### -tui -- Use gdb -tui as the debugger
<a href="#-tui"></a>

#### -bcmd -- Evaluate command at breakpoint
<a href="#-bcmd"></a>

This option evaluates a command at the first triggered breakpoint

#### -emacs -- Use emacs environment as the debugger
<a href="#-emacs"></a>

#### -manywin -- Use gdb-many-windows emacs mode
<a href="#-manywin"></a>

Enable emacs' gdb's `gud-many-windows` mode, which brings up
a debugger layout reminiscent of Visual Studio or Eclipse, with locals, breakpoints, 
threads subwindows, etc. See also `-disas`

#### -follow_child -- When forking, follow child (default is parent)
<a href="#-follow_child"></a>

This controlls gdb's `follow child` so that the debugger traces the child process instead
of the parent. By default, only the parent process is debugged.

This option is really most useful for debugging `fork()` calls; With fork/exec, it makes
the most sense to isolate the child process command line and debug it directly without
messing with the parent first.

#### -py -- Enable python scripting
<a href="#-py"></a>

Configure gdb to enable gdb python scriptability

#### -dry_run -- Print commands but don't execute
<a href="#-dry_run"></a>

#### -mp -- Multi-process debugging
<a href="#-mp"></a>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Options -->

### Inputs
<a href="#inputs"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Inputs -->
`mdbg` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|
|[dev.builddir](/txt/ssimdb/dev/builddir.md)|Directory where object files/executables go. Determines compile/link options|
|[dev.cfg](/txt/ssimdb/dev/cfg.md)|Compiler configuration|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Inputs -->


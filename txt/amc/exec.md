## amc: Subprocess Invocation

Using field `reftype:Exec` allows you to generate a subprocess control structure
for any target. A default one is already provided for each possible target T, called
`command.T_proc`.

Here is one such generated struct for `amc` itself:

```
inline-command: amc command.amc_proc | grep -B 20 '};'

// --- command.amc_proc
struct amc_proc { // command.amc_proc: Subprocess: Algo Model Compiler: generate code under include/gen and cpp/gen
    algo::cstring   path;      //   "bin/amc"  path for executable
    command::amc    cmd;       // command line for child process
    algo::cstring   fstdin;    // redirect for stdin
    algo::cstring   fstdout;   // redirect for stdout
    algo::cstring   fstderr;   // redirect for stderr
    pid_t           pid;       //   0  pid of running child process
    i32             timeout;   //   0  optional timeout for child process
    i32             status;    //   0  last exit status of child process
    amc_proc();
    ~amc_proc();
private:
    // reftype Exec of command.amc_proc.amc prohibits copy
    // value field command.amc_proc.cmd is not copiable
    amc_proc(const amc_proc&){ /*disallow copy constructor */}
    void operator =(const amc_proc&){ /*disallow direct assignment */}
};
```

The generated fields are:
- `path`: path to the executable. Defaults to `bin/target` but can be customized
- `cmd`: the command line, to be filled out before executing
- `fstdin`, `fstdout`, `fstderr`: strings describing redirects. By default, empty.
If non-empty, can be
-- `filename`
-- `>filename`
-- `>>filename`
-- `<filename`
-- `>&fd`
- `pid`: if the subprocess is running (see `Start()`), this contains the subprocess pid
- timeout: maximum subprocess runtime, can be specified before starting (produces `alarm()` when hit)
- status: exit status of subprocess, if it has exited.

The generated functions are:
```
* _Start(): If `pid` is zero, start the subprocess using specified command line, timeout, and redirects.
 After the process is started, it will run until
-- it exits by itself
-- timeout occurs. Child process calls `alarm` after `fork()` and before `exec()` if the timeout is non-zero.
-- Kill is called. This occurs when the field is destroyed or the parent process exits. All child processes
call `algo_lib::DieWithParent()` after `fork()` and before `exec()` to ensure the child dies when the parent dies.
This prevents run away processes.

* _StartRead(): Like `Start`, but takes a `FFildes` argument and can be used to conveniently read output from subprocess
as follows:
```
    algo_lib::FFildes read;
    ind_beg(algo::FileLine_curs,line,proc_StartRead(proc,read)) {
        out1 << line << eol;
    }ind_end;
```

* _Kill(): Kill the subprocess with signal 9.

* _Wait(): Wait for the subprocess to exit

* _Exec(): Equivalent to `_Start(), _Wait();`

* _ExecX(): Execute and throw exception if exit status is non-zero

* _ToCmdline: Return intended subprocess command line as a string

### Example

Here is a full example of invoking a subprocess:

```
// create json compile database
command::abt_proc abt;
Regx_ReadSql(abt.cmd.target,"%",true);
abt.cmd.uname       = uname;
abt.cmd.cfg         = dev_Cfg_cfg_release;
abt.cmd.compiler    = compiler;
abt.cmd.arch        = "x86_64";
abt.cmd.jcdb        = project;
abt_ExecX(abt);
```

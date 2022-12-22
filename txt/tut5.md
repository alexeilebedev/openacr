## Tutorial 5: Running multiple subprocesses

In this tutorial, which is related to `tut3: Scheduling and Main Loop`,
we describe a simple program called `atf_nrun`, which runs N parallel child jobs,
spawning new jobs and polling for completions as necessary, using steps (i.e.
described in real time domain, not logical time domain).

This tutorial is written with use of `inline-command`, which means all
code samples are actual code samples taken from the source code of the program.
The source of the program is in these files:

```
inline-command: acr targsrc:atf_nrun/% -report:N
dev.targsrc  targsrc:atf_nrun/cpp/atf/atf_nrun.cpp            comment:""
dev.targsrc  targsrc:atf_nrun/cpp/gen/atf_nrun_gen.cpp        comment:""
dev.targsrc  targsrc:atf_nrun/include/atf_nrun.h              comment:""
dev.targsrc  targsrc:atf_nrun/include/gen/atf_nrun_gen.h      comment:""
dev.targsrc  targsrc:atf_nrun/include/gen/atf_nrun_gen.inl.h  comment:""
```

### Syntax

```
inline-command: atf_nrun -h


Usage: atf_nrun [options]
    -in       string  Input directory or filename, - for stdin. default: "data"
    -maxjobs  int     Number of simultaneous jobs. default: 2
    [ncmd]    int     . default: 6
    -verbose          Enable verbose mode
    -debug            Enable debug mode
    -version          Show version information
    -sig              Print SHA1 signatures for dispatches
    -help             Print this screen and exit

```

### Description

How do we run and manage N parallel child jobs?
We need the following two sets:
1. A set of todos
2. A set of running jobs

With this description of program state, there are 4 possible changes that can be performed:
1. Insert todo
2. Delete todo
3. Create running job
4. Delete running job

Since this is a sample, we'll use just one table of records called `FEntry`,
which we will create once in Main and never delete.
The `todo` set will be represented by the `zd_todo` index.
The `running job` set will be represented by the `ind_running` hash on the `pid` field of `FEntry`.

When both of these indexes are empty, we have no jobs left to spawn and no
running children to wait for, so the program should exit.
Here is the visualization of this structure:

```
inline-command: amc_vis atf_nrun.%
                                          
                                          
    / atf_nrun.FDb                        
    |                                     
    |Lary fentry-------->/ atf_nrun.FEntry
    |Thash ind_running-->|                
    |Llist zd_todo------>|                
    -                    |                
                         |                
                         -                
```

Since this is a sample, there are no inputs. We will creates all the `FEntry` records
in Main, and then enter main loop.

```
inline-command: src_func atf_nrun Main -report:N -showloc:N 
void atf_nrun::Main() {
    struct sigaction sigact;
    sigact.sa_handler = SignalHandler;
    sigemptyset(&sigact.sa_mask);
    sigact.sa_flags = SA_RESTART;
    (void)sigaction(SIGCHLD, &sigact, 0);

    for (int i=0; i<_db.cmdline.ncmd; i++) {
        FEntry &entry = atf_nrun::fentry_Alloc();
        entry.command << "echo "<<i<<"; sleep 1";
        fentry_XrefMaybe(entry);
        zd_todo_Insert(entry);
    }
    atf_nrun::MainLoop();
}

```

Let's look at the defined steps.
```
inline-command: acr fstep:atf_nrun.% -tree -report:N
dmmeta.fstep  fstep:atf_nrun.FDb.ind_running  steptype:InlineRecur  comment:""
dmmeta.fstep  fstep:atf_nrun.FDb.zd_todo      steptype:InlineRecur  comment:""
```

The `InlineRecur` step type, which would be better named `InlineDelay`, is sampled once
through each iteration of main loop, and calls the user-defined function at specified time
whenever the index is non-empty, after which it is rescheduled for current time + delay.
The delay can be adjusted dynamically with generated SetDelay function

Here is the implementation of the step function for `zd_todo`.
In this function, we spawn the next child if our internal parallelism limit allows
for it. If we have reached the maximum number of running jobs, we set the delay at 'infinity'
(60 seconds) to avoid exiting main loop. Our program will never actually wait 60 seconds before
spawning a child process.

```
inline-command: src_func atf_nrun zd_todo_Step -report:N -showloc:N 
// Attempted every minute until zd_todo is empty
// Increases # of jobs
void atf_nrun::zd_todo_Step() {
    if (atf_nrun::ind_running_N() <atf_nrun::_db.cmdline.maxjobs) {
        atf_nrun::FEntry &next = *atf_nrun::zd_todo_RemoveFirst();
        next.job_path = "bash";// for some reason amc supplies 'bin/bash' by default
        next.job_cmd.c = next.command;// command to execute
        job_Start(next);// spawn it
        next.pid=next.job_pid;// amc can't index by job_pid field, must copy to a user-defined field
        if (next.pid != -1) {
            atf_nrun::ind_running_InsertMaybe(next);// index it
        }
        prlog("atf_nrun.spawn"
              <<Keyval("command",next.command)
              <<Keyval("pid",next.pid)
              <<Keyval("ntodo",atf_nrun::zd_todo_N())
              <<Keyval("nrunning",atf_nrun::ind_running_N()));
    } else {
        zd_todo_SetDelay(algo::ToSchedTime(60.0));
    }
}

```

Here is the implementation of the step function for `ind_running`. Conceptually
this runs in an infinite loop, but since we don't want to call `waitpid` repeatedly,
we delay the next call by the usual 1 minute unless a `SIGCHLD` signal arrives, in which
case we schedule the `ind_running_Step` to occur immediately.
Here is the code. As you can see, we just call waitpid, and then schedule `zd_todo`
to run immediately since it becomes possible to spawn more jobs after a child exits.
The signal handler is installed in the `Main` function (see above).

```
inline-command: src_func atf_nrun '(%Signal%|ind_running_Step)' -report:N -showloc:N 
// Attempted whenever we suspect that waitpid()
// will return something interesting
// Decreases # of jobs
void atf_nrun::ind_running_Step() {
    int status=0;
    int pid = waitpid(-1,&status,WNOHANG);//nonblocking wait
    if (pid > 0) {
        if (FEntry *entry = ind_running_Find(pid)) {
            ind_running_Remove(*entry);// no longer running
            entry->pid = -1;
            entry->job_pid = -1;// clear pid
            entry->job_status = status;// save status
            // cause zd_todo to execute immediately
            zd_todo_SetDelay(algo::SchedTime());
        }
        prlog("atf_nrun.sigchild"
              <<Keyval("pid",pid)
              <<Keyval("status",status)
              );
    } else {
        // let child processes run until next sigchild
        ind_running_SetDelay(algo::ToSchedTime(60.0));
    }
}

// trigger waitpid call
static void SignalHandler(int sig) {
    (void)sig;
    atf_nrun::ind_running_SetDelay(algo::SchedTime());// cause it to execute immediately
    prlog("SIGCHLD");
}

```

For completeness, here are the ssim lines defining the `atf_nrun` process, minus
some less useful ones:

```
inline-command: acr ns:atf_nrun -t -report:N | egrep -v '(dev.targsrc|dmmeta.ctypelen)'
dmmeta.nstype  nstype:exe  comment:Executable
  dmmeta.ns  ns:atf_nrun  nstype:exe  comment:""
    dev.target  target:atf_nrun  license:""  compat:%-%.%-%
      dev.targdep  targdep:atf_nrun.algo_lib  comment:""
      dev.targdep  targdep:atf_nrun.algo_pch  comment:""
      dev.tgtcov  target:atf_nrun  cov_min:0.00  maxerr:0.00  comment:""

    dmmeta.ctype  ctype:atf_nrun.FDb  comment:""
      dmmeta.field  field:atf_nrun.FDb._db      arg:atf_nrun.FDb      reftype:Global  dflt:""  comment:""
      dmmeta.field  field:atf_nrun.FDb.cmdline  arg:command.atf_nrun  reftype:Val     dflt:""  comment:""
        dmmeta.fcmdline  field:atf_nrun.FDb.cmdline  read:Y  comment:""

      dmmeta.field  field:atf_nrun.FDb.fentry       arg:atf_nrun.FEntry  reftype:Lary   dflt:""  comment:"List of jobs"
      dmmeta.field  field:atf_nrun.FDb.ind_running  arg:atf_nrun.FEntry  reftype:Thash  dflt:""  comment:"Running job"
        dmmeta.fstep  fstep:atf_nrun.FDb.ind_running  steptype:InlineRecur  comment:""
        dmmeta.thash  field:atf_nrun.FDb.ind_running  hashfld:atf_nrun.FEntry.pid  unique:Y  comment:""
        dmmeta.xref  field:atf_nrun.FDb.ind_running  inscond:false  via:""

      dmmeta.field  field:atf_nrun.FDb.zd_todo  arg:atf_nrun.FEntry  reftype:Llist  dflt:""  comment:"Remaining to dos"
        dmmeta.llist  field:atf_nrun.FDb.zd_todo  havetail:Y  havecount:Y  comment:""
        dmmeta.fstep  fstep:atf_nrun.FDb.zd_todo  steptype:InlineRecur  comment:""
        dmmeta.xref  field:atf_nrun.FDb.zd_todo  inscond:false  via:""


    dmmeta.ctype  ctype:atf_nrun.FEntry  comment:""
      dmmeta.field  field:atf_nrun.FEntry.command  arg:algo.cstring  reftype:Val   dflt:""  comment:"Command to execute"
      dmmeta.field  field:atf_nrun.FEntry.pid      arg:i32           reftype:Val   dflt:""  comment:"Pid of running bash job"
      dmmeta.field  field:atf_nrun.FEntry.job      arg:command.bash  reftype:Exec  dflt:""  comment:Subprocess

    dmmeta.main  ns:atf_nrun  ismodule:N
    dmmeta.nsx  ns:atf_nrun  genthrow:Y  correct_getorcreate:Y  pool:algo_lib.FDb.malloc  sortxref:Y  pack:N  fldoffset_asserts:N  comment:""
```

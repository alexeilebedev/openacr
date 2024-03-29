## Tutorial 3: Scheduling And Main Loop
<a href="#tutorial-3-scheduling-and-main-loop"></a>
The are two main types of applications that `amc` provides support for.

One type is RPC-style applications, i.e. client applications, written in logical time domain.
This style is characterized by blocking calls.

The second is real-time, cooperative threaded applications ("engines"), written in real-time
domain.
In the first instance, one line of code corresponds to one logical step of the algorithm
being implemented. In the second instance, each line corresponds to a few CPU instructions.
This style is fully asynchronous and no blocking calls are typically allowed.

In a full kernel bypass application, not even system calls are allowed. Both amc and `algo_lib`
support this use case.

It is also possible to mix these two paradigms, but the distinction is useful to keep
in mind. If you keep RPC applications and engines separate, you get the best of both worlds:
the high throughput of engines, which can spend every available CPU cycle doing useful work,
and never block for any reason, and the logical simplicity of applications, where you don't
have to worry about every conceivable interleaving of incoming events.

### Sample App With Main Loop
<a href="#sample-app-with-main-loop"></a>
Let's begin by creating a sample app and add a main loop to it.

```
inline-command: acr_ed -create -target samp_tut3 -write
acr_ed.create_target  target:samp_tut3
report.acr_check  records:***  errors:0
acr.insert  dev.gitfile  gitfile:bin/samp_tut3
acr.insert  dev.gitfile  gitfile:cpp/gen/samp_tut3_gen.cpp
acr.insert  dev.gitfile  gitfile:cpp/samp_tut3/samp_tut3.cpp
acr.insert  dev.gitfile  gitfile:include/gen/samp_tut3_gen.h
acr.insert  dev.gitfile  gitfile:include/gen/samp_tut3_gen.inl.h
acr.insert  dev.gitfile  gitfile:include/samp_tut3.h
acr.insert  dev.gitfile  gitfile:txt/exe/samp_tut3/README.md
  acr.insert  dev.readme  gitfile:txt/exe/samp_tut3/README.md      inl:N   sandbox:N   filter:""  comment:""

acr.insert  dmmeta.ns  ns:samp_tut3  nstype:exe  license:GPL  comment:""
  acr.insert  dev.target  target:samp_tut3  compat:Linux-%.%-%
    acr.insert  dev.targdep  targdep:samp_tut3.algo_lib  comment:""
    acr.insert  dev.targdep  targdep:samp_tut3.lib_prot  comment:""
    acr.insert  dev.targsrc  targsrc:samp_tut3/cpp/gen/samp_tut3_gen.cpp        comment:""
    acr.insert  dev.targsrc  targsrc:samp_tut3/cpp/samp_tut3/samp_tut3.cpp      comment:""
    acr.insert  dev.targsrc  targsrc:samp_tut3/include/gen/samp_tut3_gen.h      comment:""
    acr.insert  dev.targsrc  targsrc:samp_tut3/include/gen/samp_tut3_gen.inl.h  comment:""
    acr.insert  dev.targsrc  targsrc:samp_tut3/include/samp_tut3.h              comment:""

  acr.insert  dmmeta.ctype  ctype:samp_tut3.FDb  comment:""
    acr.insert  dmmeta.field  field:samp_tut3.FDb._db      arg:samp_tut3.FDb      reftype:Global  dflt:""  comment:""
    acr.insert  dmmeta.field  field:samp_tut3.FDb.cmdline  arg:command.samp_tut3  reftype:Val     dflt:""  comment:""
      acr.insert  dmmeta.fcmdline  field:samp_tut3.FDb.cmdline  read:Y  basecmdline:algo_lib.FDb.cmdline  comment:""

  acr.insert  dmmeta.main  ns:samp_tut3  ismodule:N
  acr.insert  dmmeta.nscpp  ns:samp_tut3  comment:""
  acr.insert  dmmeta.nsx  ns:samp_tut3  genthrow:Y  correct_getorcreate:Y  pool:algo_lib.FDb.malloc  sortxref:Y  pack:N  comment:""

acr.insert  dmmeta.ctype  ctype:command.samp_tut3  comment:""
  acr.insert  dmmeta.field  field:command.samp_tut3.in  arg:algo.cstring  reftype:Val  dflt:'"data"'  comment:"Input directory or filename, - for stdin"
    acr.insert  dmmeta.floadtuples  field:command.samp_tut3.in  comment:""

  acr.insert  dmmeta.cfmt  cfmt:command.samp_tut3.Argv  printfmt:Tuple  read:Y  print:Y  sep:""  genop:Y  comment:""
report.acr  ***
report.amc  ***
abt.config  config:Linux-g++.release-x86_64  cache:***  out_dir:build/release
abt.outofdate  ***
report.abt  ***
please execute $(acr_compl -install) to add completions support for new target
report.amc  ***
```

Since `amc` always generates a main loop in case the program wants it, we don't need to
do anything special except modify the `samp_tut3::Main` function:

```
inline-command: sed -i 's/prlog.*/samp_tut3::MainLoop();/' cpp/samp_tut3/samp_tut3.cpp
```

```
inline-command: sed '/{/,/}/p;d' cpp/samp_tut3/samp_tut3.cpp
void samp_tut3::Main() {
    samp_tut3::MainLoop();
}
```

Build:

```
inline-command: ai samp_tut3
abt.config  config:Linux-g++.release-x86_64  cache:***  out_dir:build/release
abt.outofdate  ***
report.abt  ***
```

Run:

```
inline-command: samp_tut3
```

When you run the newly modified samp_tut3, you will notice that it exits right away. That's
because `samp_tut3::MainLoop` has detected that no actions are possible,
and has exited. Let's take a closer look at the generated `samp_tut3::MainLoop`:

```
inline-command: sed '/--- samp_tut3.FDb._db.MainLoop/,/^}/p;d' cpp/gen/samp_tut3_gen.cpp
// --- samp_tut3.FDb._db.MainLoop
// Main loop.
void samp_tut3::MainLoop() {
    algo::SchedTime time(algo::get_cycles());
    algo_lib::_db.clock          = time;
    do {
        algo_lib::_db.next_loop.value = algo_lib::_db.limit;
        samp_tut3::Steps();
    } while (algo_lib::_db.next_loop < algo_lib::_db.limit);
}
```

The main loop algorithm is controlled by the following three main variables:

* `algo_lib::_db.clock`: Current value of the clock. Updated on every scheduling cycle from CPU time source (get_cycles())
* `algo_lib::_db.limit`: When `clock` reaches `limit`, the loop exits. This makes it possible to run `MainLoop` for short amounts
of time if necessary by adjusting `limit`. By default, limit is set to the max possible u64 value.
* `algo_lib::_db.next_loop`: At the top of each each main loop cycle, this variable is set to limit. Then, some number
of `Steps` are performed, one per namespace.
Each namespace's step calls zero or more of its `fsteps` (field-level steps), which are just functions.
Each `fstep` is allowed to revise `next_loop` to the value
when this step needs to be revisited. Thus, we compute the soonest time we are going to next need some CPU time.
At the bottom of the loop, inside `algo_lib::Step`, we have
`giveup_time_Step()`, which yields the unneeded time to the OS, either by calling a direct sleep function, or,
if some `Iohooks` are defined, calling `kevent` (BSD systems) or `evoll_wait` (Linux systems) with a timeout.
If `next_loop` is found to be equal to `limit` at the end of the cycle, this proves that there is no possible
way the application has anything left to do, so we just exit the loop and the process naturally ends.

In a kernel bypass (e.g. hot polling, latency-critical) application, there are no `Iohooks`, but there is usually some step that
is set up to be called on every scheduling cycle. This tells `giveup_time_Step` not to sleep, and so we get
good performance and no system calls. But should an `Iohook` show up after all in one of the
namespaces that comprise the app, the main loop automatically begins to poll it with no special action needed.

### Adding A Step
<a href="#adding-a-step"></a>
Now let's modify our app so that it prints numbers `0..10`, asynchronously, then exits.
There are many ways to do it, so we'll start with the most general one: a table.

```
inline-command: acr_ed -create -ctype samp_tut3.Value -subset i32 -pooltype Tpool -write
report.acr_check  records:***  errors:0
acr.insert  dmmeta.ctype  ctype:samp_tut3.Value  comment:""
  acr.insert  dmmeta.field  field:samp_tut3.Value.value  arg:i32  reftype:Val  dflt:""  comment:""

acr.insert  dmmeta.field  field:samp_tut3.FDb.value  arg:samp_tut3.Value  reftype:Tpool  dflt:""  comment:""
report.acr  ***
report.amc  ***
```

```
inline-command: acr_ed -create -field samp_tut3.FDb.zd_value -fstep Inline -write
report.acr_check  records:***  errors:0
acr.insert  dmmeta.field  field:samp_tut3.FDb.zd_value  arg:samp_tut3.Value  reftype:Llist  dflt:""  comment:""
  acr.insert  dmmeta.llist  field:samp_tut3.FDb.zd_value  havetail:Y  havecount:Y  comment:""
  acr.insert  dmmeta.fstep  fstep:samp_tut3.FDb.zd_value  steptype:Inline  comment:""
  acr.insert  dmmeta.xref  field:samp_tut3.FDb.zd_value  inscond:true  via:""
report.acr  ***
report.amc  ***
```

The above commands, which are best practiced interactively, add the following records to our database:

```
inline-command: acr ctype:samp_tut3.Value -t
dev.license  license:GPL  comment:""
dmmeta.nstype  nstype:exe  comment:Executable
  dmmeta.ns  ns:samp_tut3  nstype:exe  license:GPL  comment:""
    dmmeta.ctype  ctype:samp_tut3.Value  comment:""
      dmmeta.field  field:samp_tut3.Value.value  arg:i32  reftype:Val  dflt:""  comment:""
      dmmeta.ctypelen  ctype:samp_tut3.Value  len:32  alignment:8  padbytes:4

dmmeta.field  field:samp_tut3.FDb.value     arg:samp_tut3.Value  reftype:Tpool  dflt:""  comment:""
dmmeta.field  field:samp_tut3.FDb.zd_value  arg:samp_tut3.Value  reftype:Llist  dflt:""  comment:""
  dmmeta.llist  field:samp_tut3.FDb.zd_value  havetail:Y  havecount:Y  comment:""
  dmmeta.fstep  fstep:samp_tut3.FDb.zd_value  steptype:Inline  comment:""
  dmmeta.xref  field:samp_tut3.FDb.zd_value  inscond:true  via:""
report.acr  ***
```

To summarize:
* We have a new table of type `samp_tut3.Value` (`samp_tut3::Value` in C++), where new elements
can be created as `value_Alloc`.
* During xref, each new element in `value` is automatically added to a zero-terminated, global doubly linked list
called `zd_value`.
* There is a step defined on `zd_value` of type `Inline`.

Now let's modify the samp_tut3 and run it:

```
inline-command: cp conf/samp_tut3.txt cpp/samp_tut3/samp_tut3.cpp
```

```
inline-command: sed '/zd_value_Step/,$p;d' cpp/samp_tut3/samp_tut3.cpp
void samp_tut3::zd_value_Step() {
    Value &value = *zd_value_First();
    prlog(value.value);
    value_Delete(value);
}

void samp_tut3::Main() {
    for (int i=0; i < 10; i++) {
        Value &value=value_Alloc();
        value.value=i;
        value_XrefMaybe(value);
    }
    samp_tut3::MainLoop();
}
```

```
inline-command: ai samp_tut3
abt.config  config:Linux-g++.release-x86_64  cache:***  out_dir:build/release
abt.outofdate  ***
report.abt  ***
```

```
inline-command: samp_tut3
0
1
2
3
4
5
6
7
8
9
```

The program has printed 10 numbers and exited. We already know that a program exits
when it can be proven that it has nothing remaining to do. We also have the beginnings
of an asynchronous program written in engine style.

Let's proceed slowly and understand what code got generated, and what we can do with it.
First, let's check the new `MainLoop`;

```
inline-command: amc samp_tut3.%.MainLoop

// --- samp_tut3.FDb._db.MainLoop
// Main loop.
void samp_tut3::MainLoop() {
    algo::SchedTime time(algo::get_cycles());
    algo_lib::_db.clock          = time;
    do {
        algo_lib::_db.next_loop.value = algo_lib::_db.limit;
        samp_tut3::Steps();
    } while (algo_lib::_db.next_loop < algo_lib::_db.limit);
}

report.amc  ***
```

We see that `samp_tut3::Step` is now called. That's because we defined the `fstep` record on `zd_value`.
Let's drill down to `Step`:

```
inline-command: amc samp_tut3.%.Step

// --- samp_tut3.FDb._db.Step
// Main step
void samp_tut3::Step() {
    zd_value_Call();
}
// func:samp_tut3.FDb.zd_value.Step
// this function is 'extrn' and implemented by user
void                 zd_value_Step() __attribute__((nothrow));

report.amc  ***
```

Almost nothing interesting here... Let's check `zd_value_Call`:

```
inline-command: amc samp_tut3.%.zd_value.Call

// --- samp_tut3.FDb.zd_value.Call
inline static void samp_tut3::zd_value_Call() {
    if (!samp_tut3::zd_value_EmptyQ()) { // fstep:samp_tut3.FDb.zd_value
        samp_tut3::zd_value_Step(); // steptype:Inline: call function on every step
        zd_value_UpdateCycles();
        algo_lib::_db.next_loop = algo_lib::_db.clock;
    }
}

report.amc  ***
```

Finally, the interesting bit. We see that the main `Step` checks to see if `zd_value` is empty.
If not, it calls `zd_value_Step`, which is a user-defined function. The function can rely
on the fact that the list is never empty when it's invoked, so it's safe to dereference the head
pointer. We also see that `zd_value_Call` calls `UpdateCycles`, which is an accounting function
that counts total cycles spent in `zd_value_Step`, and brings `next_loop` all the way down to
`_db.clock`. This means there will be no sleep at the end of the scheduling cycle. The rule
of thumb is that if we did any work on a given scheduling cycle, we must immediately check to
see if more work is necessary, so sleep must be zero.

### Step With Delay
<a href="#step-with-delay"></a>
Now let's modify the step function so that it gets executed only once per second.
After all, if we wanted to print 10 numbers with no delay, we didn't need to go asynchronous for that.

We'll do it all without touching the editor. Unfortunately, `acr_ed` presently doesn't have a `-delay` option
for configuring steps, so we insert the record manually.

    $ echo dmmeta.fstep fstep:samp_tut3.FDb.zd_value steptype:InlineRecur | acr -merge -write
    ...
    $ echo dmmeta.fdelay fstep:samp_tut3.FDb.zd_value delay:1.000000000 scale:N | acr -insert -write
    ...

Here, we change the type of the step to `InlineRecur` with `delay:1` and `scale:N`.
We should also update `samp_tut3` to print the wall time, because otherwise its output as shown in this
tutorial would be indistinguishable from the previous example.

    void samp_tut3::zd_value_Step() {
        Value &value = *zd_value_First();
        prlog(CurrUnTime() << " "<< value.value);
        value_Delete(value);
    }
    ...
    $ ai samp_tut3
    ...
    $ samp_tut3
    2019-05-17T14:49:30.602566 0
    2019-05-17T14:49:31.665902 1
    2019-05-17T14:49:32.729908 2
    2019-05-17T14:49:33.793814 3
    2019-05-17T14:49:34.857718 4
    2019-05-17T14:49:35.921621 5
    2019-05-17T14:49:36.950129 6
    2019-05-17T14:49:38.014028 7
    2019-05-17T14:49:39.077919 8
    2019-05-17T14:49:40.141812 9

Magic! It worked. Notice that timestamps use ISO 8601 format, which is a standard.
Let's check to see how the implementation of `zd_value_Call` has changed.

     // --- samp_tut3.FDb.zd_value.Call
     inline static void samp_tut3::zd_value_Call() {
         if (!samp_tut3::zd_value_EmptyQ()) { // fstep:samp_tut3.FDb.zd_value
             if (samp_tut3::_db.zd_value_next < algo_lib::_db.clock) {
                 samp_tut3::_db.zd_value_next = algo_lib::_db.clock + samp_tut3::_db.zd_value_delay;
                 samp_tut3::zd_value_Step(); // steptype:InlineRecur: call function every N clock cycles
                 zd_value_UpdateCycles();
             }
             algo_lib::_db.next_loop.value = u64_Min(samp_tut3::_db.zd_value_next, algo_lib::_db.next_loop);
         }
     }

We now have two new variables called `samp_tut3::_db.zd_value_next` and `samp_tut3::_db.zd_value_delay`: it holds the clock value
when `zd_value_Call` should be called next; Initially it's zero. Whenever `zd_value_Step()` (the user-defined
function) is invoked, we update `next` to be `clock + delay`, thus "re-scheduling" the call.

If we trace the `samp_tut3` process, we will see that between write's , there is a call to `nanosleep`.
This is `algo_lib` giving up time to the OS (I'm on FreeBSD, so I use `truss`, on Linux `strace` would
be used):

    $ truss samp_tut3
    ...
    write(1,"2019-05-17T14:53:54.168237 6\n",29)	 = 29 (0x1d)
    nanosleep({ 0.999901488 })			 = 0 (0x0)
    2019-05-17T14:53:55.231717 7
    write(1,"2019-05-17T14:53:55.231717 7\n",29)	 = 29 (0x1d)
    nanosleep({ 0.999898795 })			 = 0 (0x0)
    2019-05-17T14:53:56.294986 8
    write(1,"2019-05-17T14:53:56.294986 8\n",29)	 = 29 (0x1d)
    ...

### Scaled Delay
<a href="#scaled-delay"></a>
In the example above, we used `scale:N`. In some situations, such as when implementing a `heartbeat` step
for network connections, we want each element of the list to be visited once per `delay`.
In this case, we set `scale:Y` and the delay will be automatically divided by the number of elements
in the list.

### Dynamic Updates: Deletion
<a href="#dynamic-updates-deletion"></a>
So far so good, but the same exact thing could be written in a couple of lines of C, if we just call
a `nanosleep` and `printf` in a loop. It's time to attempt to do something that an RPC process can't do:
interleave two logical threads of actions inside of one single-threaded program.

We are going to add a new hash table called `ind_value`, and a new step that executes 60 times a second.
Each time, it picks a random number from 0 to 10, finds a `value` using this number as the key,
and if found, deletes it. This will hopefully simulate some asynchronous activity.

Let's proceed.
First, adding an extra hash:

    $ acr_ed -create -field samp_tut3.FDb.ind_value -write

Now, let's add the other step:

    $ acr_ed -create -field samp_tut3.FDb.update -arg bool -fstep InlineRecur -dflt true -write
    ...
    $ echo dmmeta.fdelay fstep:samp_tut3.FDb.update delay:0.018 scale:N | acr -insert -write
    ...

And implement it. We use the current cpu cycles value as a proxy for a random number.
An fstep attached to a variable of type bool executes whenever the bool is true. So, to deschedule it,
we must set the bool to false when we know that no more work will be performed.
Otherwise, the program will run forever.

    void samp_tut3::update_Step() {
        int num=get_cycles() % 10;
        if (Value *value=ind_value_Find(num)) {
            prlog("deleted "<<value->value);
            value_Delete(*value);
        }
        if (ind_value_N()==0) {
            _db.update=false;
        }
    }

Now let's compile and run the program:

    $ samp_tut3
    2019-05-17T15:06:37.752202 0
    deleted 8
    deleted 4
    deleted 2
    deleted 6
    2019-05-17T15:06:38.752804 1
    2019-05-17T15:06:39.753405 3
    2019-05-17T15:06:40.754008 5
    2019-05-17T15:06:41.754611 7
    2019-05-17T15:06:42.755214 9

And indeed, modifying the linked list of values was never even an issue, because amc automatically
removed any deleted value from the list.
Now is a good time to see that happens.

### Auto Unref
<a href="#auto-unref"></a>
Whenever a `value` is deleted, amc invokes the `Unref` function.
The function's job is to remove any references to the value from known places in the system.
Any references held by user code via raw pointers, or some other trickery, are of course not visible
to amc. (Amc cannot save you from committing suicide).

    $ amc samp_tut3.Value.%.Unref

    // --- samp_tut3.Value..Unref
    // Remove row from all global and cross-reference indices
    // Row may be only partially x-referenced with any parents.
    static void samp_tut3::Value_Unref(samp_tut3::Value &row) { // internal, automatically inlined
        zd_value_Remove(row); // remove value from index zd_value
        ind_value_Remove(row); // remove value from index ind_value
    }

Here, amc knows that Value has two x-refs: `zd_value` and `ind_value`.
Finally, let's look at the definition of value itself.

     // --- samp_tut3.Value
     // create: samp_tut3.FDb.value (Tpool)
     // global access: zd_value (Llist)
     // global access: ind_value (Thash)
     struct Value { // samp_tut3.Value
         samp_tut3::Value*   value_next;       // Pointer to next free element int tpool
         samp_tut3::Value*   zd_value_next;    // zslist link; -1 means not-in-list
         samp_tut3::Value*   zd_value_prev;    // previous element
         samp_tut3::Value*   ind_value_next;   // hash next
         i32              value;            //   0
     private:
         friend samp_tut3::Value&       value_Alloc() __attribute__((__warn_unused_result__, nothrow));
         friend samp_tut3::Value*       value_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
         friend void                 value_Delete(samp_tut3::Value &row) __attribute__((nothrow));
         Value();
         ~Value();
         Value(const Value&){ /*disallow copy constructor */}
         void operator =(const Value&){ /*disallow direct assignment */}
     };
     ...

Here, we see a few things of note:
* Above the struct declaration, a list of places that `amc` considers access paths to value.
This includes both x-refs and pools.
* Field `_next` to support Tpool. With free-lists, it is sometimes customary to overwrite
the first 8 bytes of a deleted element, because it's deleted anyway, who cares? But it's unsafe
to do so -- use-after-free is an insiduous bug that's hard to find, and programs that "use
just a little bit" after free are strictly speaking correct, so `amc` takes the safe way and
introduces an extra field just to link dead elements together. This field is unused as long as
`Value` is alive, and it can be used to detect double-freeing as well.
* Fields `zd_value_next`, `zd_value_prev`: link fields for the doubly linked list.
* Field `ind_value_next`: Next element in the hash bucket collision list.
* Privately declared functions `Alloc`, `AllocMaybe`, private constructor, copy constructor,
  and `operator =`. Whenever `amc` sees an access path, it has disallows copying.
  And whenever `amc` sees a pool, it disallows creating of the corresponding record
  on the stack. (The reason is simple: how does one distinguish between a record allocated via
  a Tpool and a record on the stack? One would be free to call `_Delete` on either, yielding
  unpredictable results.)

### Larger Programs
<a href="#larger-programs"></a>
This method of interleaving multiple little steps in a larger program scales very well,
and gives one fine control over the various activities. The step functions should be viewed
as cooperatively scheduled threads. After each step, we return to the scheduler,
and we aren't holding any pointers on the stack, making the overall process easier to reason
about.

### Iohooks
<a href="#iohooks"></a>
In a non-blocking single-threaded application, there can be only one situation where time is
yielded to the system: when that time is not needed. That's how we avoid taking 100% CPU
while continuing to call ourselves `non-blocking`.
The `Iohook` mechanism is a way to attach (hook?) file descriptor polling to the main loop.

Let's begin by deleting the old program and creating a new one:

    $ acr_ed -del -target samp_tut3 -write
    $ acr_ed -create -target samp_tut3 -write
    ...

The program will read characters from stdin and print them as c++-quoted characters
with timestamp.
First, we'll need a field for the Iohook:

    $ acr_ed -create -field samp_tut3.FDb.read -arg algo_lib.FIohook -write
    ...

The concept is simple. We register interest in either reading or writing a file descriptor,
attach a callback, and insert the iohook into the system-specific event mechanism. On
BSD systems (FreeBSD and MacOS (Mach)) it's `kevent`, on Linux it's `epoll`.

    static void DoRead() {
        char buf;
        int rc=0;
        do {
            rc=read(samp_tut3::_db.read.fildes.value,&buf,1);
            if (rc==1) {
                tempstr out;
                char_PrintCppSingleQuote(buf,out);
                prlog(CurrUnTime()<<" "<<out);
            } else if (rc==-1 && errno==EAGAIN) {
            } else {
                IohookRemove(samp_tut3::_db.read);
            }
        } while (rc>0);
    }

    void samp_tut3::Main() {
        _db.read.fildes=algo::Fildes(0);
        algo::SetBlockingMode(_db.read.fildes,false);
        callback_Set0(_db.read,DoRead);
        IOEvtFlags flags;
        read_Set(flags,true);
        IohookAdd(_db.read,flags);
        MainLoop();
    }

Let's run the program:

    $ samp_tut3
    5
    2019-05-17T16:26:52.95788 '5'
    2019-05-17T16:26:52.95818 '\n'
    ^K
    2019-05-17T16:26:56.931429 '\013'
    2019-05-17T16:26:56.931444 '\n'
    ^D
    <exits>

Our primitive non-blocking program does what we intended.
This code could be combined with other non-blocking code,
and the behaviors the two will be combined.


## AMC Runtime
<a href="#amc-runtime"></a>
AMC offers many ways to structure RAM (into pools, records, indexes, etc). The other dimension
of a program is *time*. Here, AMC doesn't have as much to say; The user implements their
program in C++. Still, there are several ways amc affects control flow.
First, amc implements the `main()` function, which initializes libraries, loads input tuples,
parses the command line, and passes the control to `ns::Main`.

Second, `amc` generates a `MainLoop` function, which the user can call. The `MainLoop` repeatedly
executes `steps`, which are user-implemented functions.

### Program Startup & Main
<a href="#program-startup---main"></a>

The command line for each target `x` is represented by the struct `command.x`.

The `fcmdline` record generates code to parse the command line from argc/argv,
and generates the syntax string.

Positional arguments are marked with `anonfld`.

When the program runs, entry function `main()` saves argc/argv to variables
`algo_lib.FDb.argc` and `algo_lib.FDb.argv` and calls `Main`:
User implements `Main`.
One can step through all of these functions with [mdbg](/txt/exe/mdbg/README.md)

### Steps
<a href="#steps"></a>

Steps are a scheduling construct to associate actions to fields. A step is defined by adding
a `dmmeta.fstep` record; `fstep` is a subset of field, and specifically only global fields
can have steps associated with them. When you define a step, you get a user-defined function
which you can implement. The function is called each time the process goes through the main loop,
(amc-generated function `$ns::MainLoop()`).
and it's called only if the underlying field is considered non-empty (for bool/integer fields this
just means non-zero).

Using steps you can implement a pipeline for data processing. Since the state of any program
can be represented as a set of tables, the only possible actions the program does throughout its
life are insertion and deletion of records in these tables. Steps allow you to trigger these actions
in a well-understood and very flexible way.

The main loop executes until the value of CPU clock (rdtsc)
reaches some limit (typically 2^64-1). The scheduling cycle begins by setting next\_loop to the limit,
then executing all the steps. the steps adjust next\_loop down depending on their scheduling needs.
At the end of the scheduling cycle, unused remaining time is given up to the OS via epoll or nanosleep
(if there are no file descriptors open). This way, both hot-spinning and blocking processes are covered
with minimal overhead.

The following step types are defined: `Inline`, `InlineRecur`, `TimeHookRecur`, `Callback`
To define a step that is performed periodically on a timer, use `fdelay`.

### Defining Steps
<a href="#defining-steps"></a>

To define a step, create an fstep record. It must be defined on a global field (FDb),
since each step corresponds to 1 user-written function.
Example:

```
dmmeta.ctype  ctype:atf_amc.FDb  comment:""
  dmmeta.field  field:atf_amc.FDb.cd_in_msg  arg:atf_amc.Msgbuf  reftype:Llist  dflt:""  comment:""
    dmmeta.fstep  fstep:atf_amc.FDb.cd_in_msg  steptype:Inline  comment:""

```

This declares the external (to be implemented by you) function `cd_in_msg_Step()`,
generates the `cd_in_msg_Call()` function which checks conditions before calling the user-generated
function, and modifies the generated `atf_amc::Step` to invoke the new step:

```
// forward declaration for user-defined function
void cd_in_msg_Step() __attribute__((nothrow));

// Main function that calls all steps in atf_amc namespace
void atf_amc::Step() {
    // ... other steps
    cd_in_msg_Call();
}

// Function to invoke user -generated _Step function
// if the conditions are right, and  modify scheduling parameters
inline static void atf_amc::cd_in_msg_Call() {
    if (!atf_amc::cd_in_msg_EmptyQ()) { // fstep:atf_amc.FDb.cd_in_msg
        atf_amc::cd_in_msg_Step(); // steptype:Inline: call function on every step
        cd_in_msg_UpdateCycles();
        algo_lib::_db.next_loop = algo_lib::_db.clock;
    }
}

```

All steps have to satisfy the condition that underlying control field (index or boolean field) is non-empty
before calling the user-defined function.

### Step Types
<a href="#step-types"></a>

There are several ways to get to the `_Step` function.
* We can call the user-defined function on every cycle through the main loop. This step type is called `Inline`.
The only check performed is that the control field is non-empty.
* We can call the user-defined function when the `algo_lib::_db.clock` reaches a certain value. This
step type is `InlineOnce`. The value is defined next to the field, with suffix `_next`.
* We can call the user-defined function every `_delay` clock units. This step type is called `InlineRecur`.
Two control variables are defined, `_next` and `_delay` (of type u64). The delay can be controlled at
runtime with `_SetDelay` function. Since the step type is still `Inline`, the cost of adding such step is a few
instructions being added to the main loop.
* We can call the user-defined function every `_delay` clock units, but scheduled as a `Timehook` via a binary
heap (this is a built-in index called `algo_lib.FDb.bh_timehook`. This is of course the most scalable
and universal scheduling
method, since checking the top entry in the heap is O(1) and rescheduling is O(log N), but if you want to design
a tuned engine with optimal pipelining behavior, you will need to use `Inline` variants.


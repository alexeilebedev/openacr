## amc - Algo Model Compiler

AMC reads ssim tables, mechanically generates
C++ in-memory database code as determined by these tables, and outputs .h and .cpp files.
By default, ssim tables are loaded from directory "data", and output
is written to directories cpp/gen and include/gen.

Amc generates hash tables, arrays,
linked lists, dequeues, binary heaps, and tree structures. AMC
generates hierarchical, region-based memory allocators, including
free-list, fifo, best-fit, linear array, indirect array. The resulting
code is compiled into a library, a ``database of source code'',
containing a superset of functions that will be used by the final
application. AMC does not modify or reverse-engineer user code. The
application developer uses generated code as a library.

AMC reads about 100 ssim tables. The full list can be obtained with
acr\_in amc. The exact actual amc input can be printed with acr_in amc -data.
About 20% of these tables are responsible for 80% of the generated code, the rest deal with finer details.

AMC was initially coded by hand, but once its capabilities became powerful enough, it was used to
generate data structures for its next version. As a result, all of AMC's internal data structures,
both input, and computational, are defined as ssim tuples and can be queried with ``acr ns:amc -xref''.

The output of amc is a "database of code". AMC generates verbose, user-readable code with comments.
You almost never need to go to assembly level with this code.

An in-memory database is a data structure with specific record types, well-defined custom memory pools,
referential integrity constraints, and functions to create, read, update and delete (CRUD) the records,
built-in joins between tables (called cross-references), and other operations.

Programs using amc include the generated header then make calls to provided functions.
A large number of primitives is implemented, spanning the range from almost trivial to
quite sophisticated, that the human can use to build programs.

The premise of AMC is that reusable code doesn't work, because of two things:
- layers of glue and cruft that get added to accomodate the possible use scenarios
- eventual loss of either compatibility or flexibility and abandonment

As a result, all high-performance systems are hand-coded due to highly
specific requirements. AMC solves this dilemma.
Since it generates readable source code that we then keep the under source control, we can control the changes tightly.
Since the performance of AMC itself is irrelevant (it generates 1M+ lines of code
per second, which is good enough for all purposes), we can keep backward
compatibility at the cost of slightly lower performance whenever we face this trade-off.
AMC allows customization, down to bitfield-level, of the memory layout of all attributes.
Best-in-class algorithms for most day-to-day problems are known.
The problem is supplying them to the application without losing the performance to glue code and data format
mismatch. Since AMC generates a custom set of routines for each application, it can be tuned without fear
of adversely affecting other users.

One killer feature of AMC is that it generates xrefs -- automatic joins between
in-memory structures, eliminating hard-to-find pointer errors and
leaving the developer to focus on the application, not pointer
management.

Studies estimate the defect rate to grow as an exponential function of
number of source lines. The ratio of manual to AMC-generated code for
a typical application is 1 to 20, significantly lowering the number of
potential bugs. The code AMC generates is also the kind that is the
most time-consuming and error-prone to write.

### Running Amc

To run amc, just type amc.
The default invocation takes no arguments, and prints something like this:

    $ amc
    report.amc  n_cppfile:123  n_cppline:258301  n_ctype:970  n_func:22524  n_xref:460  n_filemod:0

### Query mode

When amc is given an argument, it runs in query mode.
Instead of modifying source files it simply prints to stdout all internal code sections whose key matches
the specified regex (typically it's a ctype name or a function name).

This is the fastest way to check how a certain function is implemented.

Example:

    amc amc.%

This would generate all functions which are used to compile amc itself. The apparent circularity
is because at some point, those functions were written by hand, and then amc was modified to generate them
and save them to cpp/gen/amc_gen.cpp

### References

If a record of type A presupposes the existence of record of type B, then A contains some field
  that is a *reference* to B. If we want to main a list of all As that refer to a given B,
then we can collect that information using a *cross-reference. Reference types in amc are
Pkey, Upptr, RegxSql. Cross-reference types are Ptr, Ptrary, Thash, Atree, Bheap, Llist, Count, and others.

### Sandbox mode

The sandbox mode is provided to help add features to amc without creating a dead-end (where
  generated code is broken, but cannot be re-generated because amc needs to be rebuilt)

abt -testgen compiles a debug version of amc, creates the sandbox directory .testgen, runs amc in the .testgen
directory, shows a diff between current and sandboxed changes, builds all unit tests inside the sandbox
and runs the tests. The command succeeds if all of the steps succeed.

### Reading Code of Amc Itself
Amc source code is located under cpp/amc/*.cpp. The list of all the source files and headers
can also be retrieved with acr targsrc:amc/%
If you want to browse all the functions in amc concerned with generating hash tables, run
errlist src_func amc Thash -proto

Gentle reminder to read amc code and the tables in data/amcdb and data/dmmeta, since
they're quite readable, and ultimately that's where all the information is. This documentation
is more of a conceptual aid.

### Example Show generated C++ struct

    amc amc.FCtype

### Example: Show prototypes of all generated functions matching pattern

    amc amc.FCtype.% -proto

### Example: Count how many linked lists there are in the code database

    acr llist -print:N

### Example: Find all references to a record

    acr ctype:acmdb.Device -ndown 100

### Example: Edit 2 Ctypes

    acr ctype:acr.FCtype\|acr.FField -t -e

### Main Input Tables
The main tables Amc uses as input are ns, ctype and field.
Ns maps to a C++ namespace.
Ctype corresponds to a struct.
Field corresponds to a struct member or some derived value.

Namespaces, ctypes and fields lead useful lives outside of world of source code.
For instance, every ssim database has a namespace defined for it; its records
  are described with ctype and field. But acr mostly cares about the field names
since ssimfiles are text files, and for acr, values are strings.

The main attributes of a field are its name, arg, and reftype.
Arg is the type, and reftype is a modifier, or 'type constructor'.

    dmmeta.field  field:amc.FDb.ind_ns        arg:amc.FNs        reftype:Thash   dflt:""  comment:""
    dmmeta.field  field:dmmeta.Field.field     arg:algo.Smallstr100  reftype:Val  dflt:"  comment:"

### Steps
Steps are a scheduling construct to associate actions to fields.

A step can be thought of as a cooperative thread -- a function that's assigned to
some top-level variable (pointer, bool, list or heap) that is invoked whenever that variable is non-empty,
and advances the process towards its goal.

The amc model for a server process is a top-level while loop, which calls
a top-level Step function for each namespace linked into the process.
The namespace Step function then performs some action for each fstep defined in that namespace.

The main loop is defined in terms of real time: it executes until the value of CPU clock (rdtsc)
reaches some limit (typically 2^64-1). The scheduling cycle begins by setting next\_loop to the limit,
then executing all the steps. the steps adjust next\_loop down depending on their scheduling needs.
At the end of the scheduling cycle, unused remaining time is given up to the OS via epoll or nanosleep
(if there are no file descriptors open). This way, both hot-spinning and blocking processes are covered
with minimal overhead.

The following step types are defined: Inline, InlineRecur, TimeHookRecur, Callback
To define a step that is performed periodically on a timer, use fdelay.

Inline step is the simplest: on every pass,
the empty condition is checked on the underlying field, and a user-provided Step function
is called if the field is non-empty.

InlineRecur step requires an fdelay record specifying the initial delay between steps.
The logic is the same as Inline, with a time-based delay between steps.

### Cross-references
In a relational data model, individual records are represented by tuples (ctype).
Each attribute (field) is either a raw type, or a reference to some other record.
The reference can take several forms -- it could be a direct pointer (Upptr),
a primary key (Pkey), a regular expression (RegxSql). In either case, the whole
reason we write C++ programs with amc instead of solving all problems using SQL and MariaDB,
is that reversing the direction of reference lookup -- finding all records
that point to a given record -- is expensive. This is where x-references come in.
In database terms, a cross-reference would be a join or a group-by.
Except a cross-reference is an incremental
join that is efficiently and automatically maintained as records are added or removed from tables.

A cross-reference is established by use of an xref record. Here is an example

    dmmeta.ctype  ctype:amc.FTarget  comment:"
      dmmeta.field  field:amc.FTarget.msghdr     arg:dev.Target    reftype:Base    dflt:"  comment:"
      dmmeta.field  field:amc.FTarget.c_targdep  arg:amc.FTargdep  reftype:Ptrary  dflt:"  comment:"
        dmmeta.ptrary  field:amc.FTarget.c_targdep  unique:Y
        dmmeta.xref  field:amc.FTarget.c_targdep  inscond:true  via:amc.FDb.ind_target/dev.Targdep.target

This says: whenever a Targdep record is inserted, find an instance of Target by using global
index ind_target with key dev.Targdep.target (a computed attribute of targdep),
and insert Targdep into an array of pointers rooted in Target. Whenever a Targdep record is deleted,
automatically remove it from the list.

The main x-reference types are Ptr, Ptrary, Llist, Thash, Bheap and Atree.

There can be any number of x-references between any two tables. So, if you want a record to be
a member of ten linked lists and eleven heaps -- you're free to do so. This is similar to RDBMS indexes,
of which you can have any number.

To visualize x-references, it may be useful to use amc_vis.

A pool is a memory source for new records.
Pools are declared as fields, using reftype to specify pool type.

Amc always creates custom Alloc/Delete functions for each type of record. Memory allocation
is thus record-specific. There is no generic ``malloc'' and there is no casting of return types.

### Chaining
Pools can be chained by specifying a basepool record. Basepool is where a pool gets more memory
when its own supplies are exhausted. The algo_lib.FDb.sbrk pool is usually at the root of each chain.

### Default Namespace Pool
Each namespace has a default pool, declared via nsx record. This is where all memory for the
 namespace ultimately comes from.

### Tracing
Ftrace record can be used to enable counting of alloc/delete calls for each pool.


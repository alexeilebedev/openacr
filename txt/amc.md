## amc: Algo Model Compiler

### Syntax

```
inline-command: amc -h


amc: Algo Model Compiler: generate code under include/gen and cpp/gen
Usage: amc [options]
    -in_dir   string  Root of input ssim dir. default: "data"
    [query]   string  Query mode: generate code for specified object
    -out_dir  string  Root of output cpp dir. default: "."
    -proto            Print prototype. default: false
    -report           Final report. default: true
    -e                Open matching records in editor. default: false
    -trace    string  Regx of something to trace code generation
    -verbose          Enable verbose mode
    -debug            Enable debug mode
    -version          Show version information
    -sig              Print SHA1 signatures for dispatches
    -help             Print this screen and exit

```

### Input tables

```
inline-command: acr ctype:$(acr_in amc | acr -in:- % -regxof:ctype)
dmmeta.ctype  ctype:amcdb.Bltin         comment:"Specify properties of a C built-in type"
dmmeta.ctype  ctype:amcdb.Tcurs         comment:"Cursor template"
dmmeta.ctype  ctype:dev.Targdep         comment:"Dependency between targets"
dmmeta.ctype  ctype:dev.Target          comment:"Build target"
dmmeta.ctype  ctype:dmmeta.Anonfld      comment:"Omit field name where possible (command line, enums, constants)"
dmmeta.ctype  ctype:dmmeta.Argvtype     comment:"Customize parsing of command lines (rarely used)"
dmmeta.ctype  ctype:dmmeta.Basepool     comment:"Specify pool to be used for allocating elements of a type"
dmmeta.ctype  ctype:dmmeta.Bitfld       comment:"Specify offset/width of a bitfield within another field"
dmmeta.ctype  ctype:dmmeta.Cafter       comment:"Control amc processing order for unrelated types (used in rare situations)"
dmmeta.ctype  ctype:dmmeta.Cascdel      comment:"Request cascading delete of referenced rows"
dmmeta.ctype  ctype:dmmeta.Ccmp         comment:"Generate hash function"
dmmeta.ctype  ctype:dmmeta.Cdflt        comment:"Specify default value for single-value types that lack fields"
dmmeta.ctype  ctype:dmmeta.Cextern      comment:"Externally defined ctype (a struct from system header, or from a c++ library)"
dmmeta.ctype  ctype:dmmeta.Cfmt         comment:"Specify options for printing/reading ctypes into multiple formats"
dmmeta.ctype  ctype:dmmeta.Cget         comment:"Generate state functions for these ctypes"
dmmeta.ctype  ctype:dmmeta.Charset      comment:"Generate functions to determine if a character is a member of a set"
dmmeta.ctype  ctype:dmmeta.Chash        comment:"Generate hash function"
dmmeta.ctype  ctype:dmmeta.Cppfunc      comment:"Value of field provided by this expression"
dmmeta.ctype  ctype:dmmeta.Cpptype      comment:"Specify whether a ctype can be passed by value, and other c++ options"
dmmeta.ctype  ctype:dmmeta.Csize        comment:"Specify size/alignment for built-in C++ types"
dmmeta.ctype  ctype:dmmeta.Cstr         comment:"Specify that type behaves like a string"
dmmeta.ctype  ctype:dmmeta.Ctype        comment:"Conceptual type (or C type)"
dmmeta.ctype  ctype:dmmeta.Dispatch     comment:"Generate code for a multi-way branch"
dmmeta.ctype  ctype:dmmeta.DispatchMsg  comment:"Add message to a dispatch"
dmmeta.ctype  ctype:dmmeta.Dispctx      comment:"Use context with dispatch"
dmmeta.ctype  ctype:dmmeta.Dispfilter   comment:"Generate filter function on dispatch"

dmmeta.ctype  ctype:dmmeta.Disptrace    comment:"Generate trace fields (cycles, counts) for all dispatch branches"
dmmeta.ctype  ctype:dmmeta.Fbase        comment:"Customize imported Base fields"
dmmeta.ctype  ctype:dmmeta.Fbigend      comment:"Annotate field as having big-endian storage"
dmmeta.ctype  ctype:dmmeta.Fbitset      comment:"Generate bitset functions over integer field or array"
dmmeta.ctype  ctype:dmmeta.Fbuf         comment:"Buffer for reading/writing messages, works with Iohook"
dmmeta.ctype  ctype:dmmeta.Fcast        comment:"Generate implicit conversion from field to c++ expression"
dmmeta.ctype  ctype:dmmeta.Fcleanup     comment:"Request user-implemented function to be called at Uninit time for a field"
dmmeta.ctype  ctype:dmmeta.Fcmap        comment:"Bidirectional mapping between fconst values"
dmmeta.ctype  ctype:dmmeta.Fcmdline     comment:"Annotate field that holds process command line"
dmmeta.ctype  ctype:dmmeta.Fcmp         comment:"Request versionsort or case-insensitive sort for field"
dmmeta.ctype  ctype:dmmeta.Fcompact     comment:"Request compaction upon removal from index"
dmmeta.ctype  ctype:dmmeta.Fconst       comment:"Specify enum value (integer + string constant) for a field"
dmmeta.ctype  ctype:dmmeta.Fcurs        comment:"Request generation of custom cursor"
dmmeta.ctype  ctype:dmmeta.Fdec         comment:"Specify that field has an implied # of decimal places and specify formatting options"
dmmeta.ctype  ctype:dmmeta.Fdelay       comment:"Control elapsed time between execution of a step"
dmmeta.ctype  ctype:dmmeta.Field        comment:"Specify field of a struct"
dmmeta.ctype  ctype:dmmeta.Findrem      comment:"Request generation of FindRemove function"
dmmeta.ctype  ctype:dmmeta.Finput       comment:"Describe input table of a program"
dmmeta.ctype  ctype:dmmeta.Fldoffset    comment:"Assert field offset - will result in compile-time error if violated"
dmmeta.ctype  ctype:dmmeta.Floadtuples  comment:"Request that process automatically load any input tables on startup"
dmmeta.ctype  ctype:dmmeta.Fnoremove    comment:"Omit any functions for removing elements from table; Table is append-only"
dmmeta.ctype  ctype:dmmeta.Foutput      comment:"Generate function to save index/table back to disk"
dmmeta.ctype  ctype:dmmeta.Fprefix      comment:"Mapping between field prefix and Reftype"
dmmeta.ctype  ctype:dmmeta.Fregx        comment:"Specify options for command-line regx field"
dmmeta.ctype  ctype:dmmeta.Fsort        comment:"Generate custom sort function for array field"
dmmeta.ctype  ctype:dmmeta.Fstep        comment:"Generate a main loop step to be executed whenever a field is non-empty"

dmmeta.ctype  ctype:dmmeta.Ftrace      comment:"Generate cycle/step counting fields for a step"
dmmeta.ctype  ctype:dmmeta.Funique     comment:"This field must be unique in the table. Not needed for primary key"
dmmeta.ctype  ctype:dmmeta.Fuserinit   comment:"Add user-defined initialization function for field (see fcleanup)"
dmmeta.ctype  ctype:dmmeta.Fwddecl     comment:"Request forward declaration of a field"
dmmeta.ctype  ctype:dmmeta.Gconst      comment:"Import ssim table columns as fconst for a field"
dmmeta.ctype  ctype:dmmeta.Gstatic     comment:"Load entries for this table at startup time"
dmmeta.ctype  ctype:dmmeta.Gsymbol     comment:"Create C++ symbols from entries in ssim table"
dmmeta.ctype  ctype:dmmeta.Hook        comment:"Required on Hook fields"
dmmeta.ctype  ctype:dmmeta.Inlary      comment:"Generate inline array of fixed or variable length (all entries fit within parent struct)"
dmmeta.ctype  ctype:dmmeta.Lenfld      comment:"Specify which gives length of varlen portion in bytes"
dmmeta.ctype  ctype:dmmeta.Listtype    comment:"Specify structure of linked list based on field prefix"
dmmeta.ctype  ctype:dmmeta.Llist       comment:"Options for Llist field"
dmmeta.ctype  ctype:dmmeta.Main        comment:"Generate Main function for namespace"
dmmeta.ctype  ctype:dmmeta.Msgtype     comment:"Specify message type for each eligible message, controls dispatch"
dmmeta.ctype  ctype:dmmeta.Nocascdel   comment:"Remove cascade delete for this index"
dmmeta.ctype  ctype:dmmeta.Nossimfile  comment:"Indicates that ssimfile does not exist for this ssimdb ctype"
dmmeta.ctype  ctype:dmmeta.Noxref      comment:"Explicitly specify that no x-ref exists between tables (don't use unless forced)"
dmmeta.ctype  ctype:dmmeta.Ns          comment:"Namespace (for in-memory database, protocol, etc)"
dmmeta.ctype  ctype:dmmeta.Nsdb        comment:"Annotate ssimdb namespaces"
dmmeta.ctype  ctype:dmmeta.Nsinclude   comment:"Explicitly specify a C++ include file for namespace"
dmmeta.ctype  ctype:dmmeta.Nsproto     comment:"Annotate protocol namespace (collection of types, no state)"
dmmeta.ctype  ctype:dmmeta.Nsx         comment:"Control code-generation and exception handling options for process/library"
dmmeta.ctype  ctype:dmmeta.Numstr      comment:"Add functions to read numbers out of a string field"
dmmeta.ctype  ctype:dmmeta.Pack        comment:"Request byte-packing of structure fields"
dmmeta.ctype  ctype:dmmeta.Pmaskfld    comment:"Specify which fields holds presence mask bits (one for each field in struct)"
dmmeta.ctype  ctype:dmmeta.Pnew        comment:"Generate custom constructor (placement new), for use with binary protocols"

dmmeta.ctype  ctype:dmmeta.Ptrary        comment:"Required for fields with reftype:Ptrary"
dmmeta.ctype  ctype:dmmeta.Rowid         comment:"Initialize field to row id of element"
dmmeta.ctype  ctype:dmmeta.Smallstr      comment:"Generated fixed-length padded or length-delimited string field"
dmmeta.ctype  ctype:dmmeta.Sortfld       comment:"Specify what field an index (Bheap,Atree) is sorted on"
dmmeta.ctype  ctype:dmmeta.Ssimfile      comment:"Ssim tuple name for structure"
dmmeta.ctype  ctype:dmmeta.Ssimvolatile  comment:"Types based on ssim files marked this way cannot be cross-referenced"
dmmeta.ctype  ctype:dmmeta.Substr        comment:"Specify that the field value is computed from a substring of another field"
dmmeta.ctype  ctype:dmmeta.Tary          comment:"Indirect linear dynamically alocated array (used for strings, arrays of data, etc)"
dmmeta.ctype  ctype:dmmeta.Thash         comment:"Hash index, required for fields with reftype Thash"
dmmeta.ctype  ctype:dmmeta.Typefld       comment:"Specifies which field of a message carries the type"
dmmeta.ctype  ctype:dmmeta.Usertracefld  comment:"Add custom user trace fields to process's trace struct"
dmmeta.ctype  ctype:dmmeta.Xref          comment:"Specify how to cross-reference (i.e. project, or group-by) one record with another"
report.acr  n_select:90  n_insert:0  n_delete:0  n_update:0  n_file_mod:0
```

### Introduction

Amc is an extensible generator of source code from ssimfiles.

Amc reads ssim tables, creates C++ in-memory database code as determined by these tables,
and outputs .h and .cpp files.
By default, ssim tables are loaded from directory "data", and output
is written to directories `cpp/gen` and `include/gen`. With option `-in_dir`,
the location of the input dataset can be specified. 
With option `-out_dir`, the location of output directory can be specified. The default
is `.`.

Amc generates hash tables, arrays, linked lists, dequeues, binary heaps; trees;
hierarchical, region-based memory allocators, including
single and powers-of-two freelists, fifo, linear array (vector),
indirect array (with permanent pointers); inline arrays and fixed-size arrays;
Functions to convert any struct to/from a string or a bash command line; Enum support,
both for integer and string values; presence masks; big-endian fields;
sort functions on custom fields; incremental group-by indexes; tracking of pointers
with automatic cascade delete; protection against linear scanning when deleting
elements of singly linked lists; scheduling constructs (for real-time modules); cycle accounting ('traces').
Generation of C++ symbols from ssimfile columns; Statically loaded tables; Subprocess invocation;
Asynchronous I/O. Bitsets on top of any array type. Char sets;
Fixed string types (Pascal strings, Left-padded strings, Right-padded
strings, Fixed-length strings with numeric conversion); Scaled decimal types;
Bidirectional mappings between enumerated types;
Dispatches (any group of ctypes), whether sharing a common header with a type field, or not.
Printing, reading, calling dispatches given both binary and text input.
Uniform cursor (iterator) interfaces for bheap, hash, tree, array,
lines, files in directory, messages in a memory region, and more.

For each program, these things are generated in-place and
from scratch, and can be iteratively customized.
The resulting code forms a *database of source code*,
containing a superset of functions that will be used by the final
application. The generated code is verbose, user-readable, properly commented,
is intended to be readable by a human, corresponds directly to the final assembly
code, and intentionally uses only a small, conservative subset of C++. 
`Amc` does not modify or reverse-engineer user code, so it's not a framework
where you have to "plug in" anything. User always controls what functions will be called
(dispatches and steps are an exception, but they are also explicitly controllable).
`amc` is a tool for constructing robust software based on your specifications.

The authors have spent many years using template-based libraries and
class hierarchies, and found them to be unmaintainable in the long
run.  amc represents a step forward by stepping away from these
concepts; The templates, which are a form of compile-time code
generation, are fully superseded by explicit code generation, which
can perform deductive reaasoning on closed sets, something that's not
available to templates. Templates, which are based on the idea of
substitution of expressions, cannot in princple insert or remove
fields at runtime, or delete certain functions, or create multiple
symbols, and are hard to debug because they break the mapping between
lines of code and assembly, which is required for debugging system
code. Annotated, generated code overcomes this while eliminating an
entire complex sublanguage.  Virtual functions, or function pointers,
can be useful and even unavoidable, (and are implemented in amc as
Hooks), but they negatively affect process reliability. Dijkstra's
predicate transformer, which is the only available theoretical tool
for reasoning about imperative software, requires that each action has
known pre- and post-conditions. A virtual call always leaves the
possibility open that the target function will do something that
violates the post-condition, which makes the calling code statically
unanalyzable. That's why our approach is to construct lists or
priority queues of "todo" items in a process, and then process them
with a well-defined loop or step.  This forces distinct cases to be
enumerated by some state field or an enum, and collects the code for
processing the distinct cases into one place, making it possible to think
about post-conditions again.

`Amc` loads about 100 ssim tables. The full list can be obtained with
`acr_in amc`. The exact actual `amc` input can be printed with `acr_in
amc -data`.  About 20% of these tables are responsible for 80% of the
generated code, the rest deal with finer details.

`Amc` was initially coded by hand, but once its capabilities became
powerful enough, it was used to generate data structures for its next
version. As a result, all of `Amc`'s internal data structures, both
input, and computational (internal), are defined as ssim tuples and can be
queried with `acr ns:amc -xref`. The tool is thus unique in that it
generates most of its own source code. Previous approaches focused
either on interpreting the interpreter (LISP) or compiling the
compiler. Generating a source-generator is a first as far as we know.

### Why Generate?

Good algorithms and data structures for most problems are known.
The problem is attaching them to an application. Usually the costs associated
with using algorithms are:

* Performance cost and complexity cost when using libraries.
* Difficulty understanding symbol renamings (this happens with the C++ templates or when using macro preprocessors).
  Programmer's attention is a finite resource. When the programmer spends this attention in order
  to understand what a program does, he can no longer confirm its correctness, and the program's
  runtime behavior becomes more surprising. Another word for surprise is bug. So when a program doesn't
  look trivial, this leads to more bugs.
* Debugging and reliability cost when hand-coding algorithms.
* Maintenance cost due to having too many lines of code.
* Unexpected changes in upstream generic libraries.

#### Libraries Vs. Custom Code

The motivation for writing generators is that writing code for reusability doesn't work.
The reason it doesn't work is that the definition of correctness doesn't lie with
the piece of reusable code -- it lies with the final application. And so the reusable
code always offers to the application too much and at the same time not enough.
You need a singly linked list, but the library uses a doubly linked list. You need an extra index,
but the library author didn't anticipate it. You have your own strategy for memory
management, but the library insists on its own. And you can't customize the library, since
for every feature you need to change, there is already some user out there who needs it to stay the same.
When you update to the next version of the library, you get, almost by definition, features you didn't ask for.
(Since you were already making do with the previous set of features).
So, code written for reusability never reaches its intended potential in terms
of either performance or utility.

Leaving aside reusability for a moment, as can be seen from real life examples,
all high-performance systems are hand-coded due to highly
specific requirements, and because it allows writing only what's needed, debugging that,
and leaving it to run and do its job indefinitely. Yet hand-coding is difficult and
requires a lot of debugging and chasing of memory errors and dangling pointers.

All of this may not matter when the problems are small and requirements negligible, but it
really starts to matter when you are pushing against the limits of a single machine. The difference
between code that runs in 1 millisecond and 10 milliseconds is eventually the difference between
10 servers and 100 servers.

Thus we have a mini-max problem, which is the first sign of a problem worth solving.
On one hand, we want maximally customizable code that does only what we want.
On the other hand, we want to write and debug the minimal amount of code.

#### Exponential Cost Of Software

Software complexity models such as [COCOMO](https://en.wikipedia.org/wiki/COCOMO)
estimate the defect rate to grow as an
exponential function of number of source lines. This exponent should not be underestimated.
It is about 1.2 for a national stock exchange with real-time and embedded constraints, meaning that
to write 1,000 lines of code, it costs you 3,981 units of effort.
And if you write 100,000 lines of code, you pay a full 1,000,000 units. This exponential
nature of the cost of lines of code is closely related to the cost of borrowing money.
You basically have to pay back with interest, making every line of code
 in a very real sense "technical debt".

Massive code bases can slow development to a crawl and destroy projects.
I don't want to present a macabre listing of all the software projects that failed
because they had too much code; I will just assume that we all agree on the validity
of the following equation:

    (Project Goodness) = W*(Features) - A*(Amount of Code Written)^B

In other words, we like the features of our system, with some weight `W`,
and we pay with interest `B` and weight `A` for the amount
of code we wrote to get these features.

With that, let's see how far we are able to go in solving this problem, and what kinds
of cool tricks we can make our generator do.

### Running Amc

To run `amc`, just type `amc`. All of its inputs come from ssimfiles, so no arguments
are necessary. The default invocation prints something like this:

    $ amc
    report.amc  n_cppfile:123  n_cppline:258301  n_ctype:970  n_func:22524  n_xref:460  n_filemod:0

With respect to generator performance, `amc` process generates about 1M LOC per second.
Of course, this performance is not reflected in
the final executable, which means that adding new checks or generator features to amc is effectively free.

Amc outputs are versioned in git, so we can trace the origin
of any change (with git annotate), and continue to make changes to amc
without breaking existing code.

### Query mode

When amc is given an argument, it runs in query mode.
Instead of modifying source files it simply prints to stdout all internal code
sections whose key matches the specified regex (typically it's a ctype name or a function name).
This is the fastest way to check how a certain function is implemented.

Example:
    amc amc.%

This generates all functions which are used to compile amc itself. The apparent circularity
exists because at some point, functions implementing amc were written by hand,
and then amc was modified to generate them and save them to `cpp/gen/amc_gen.cpp`.
Please see [The Algorithm For Generating Any Code](#the-algorithm-for-generating-any-code)

To limit amc's output to prototypes only, specify `-proto`:

    amc amc.% -proto

### Ratio of Generated To Manual LOC

On average, `amc` generates 15-25 lines of code per 1 line of ssimfile specification.
It is easy enough to check this claim:

    $ acr ns:amc -t | wc -l
    2010
    $ amc amc.% | wc -l
    47431

The specification can be edited manually and adjusted frequently with Unix tools such as
sed and perl, or by issuing `acr_ed` commands. The compression factor, and the fact
that ssim lines are relatively independent of each other makes the cost of
ssim specifications much lower than the cost of regular code.

### Sandbox Mode

The sandbox mode is provided to help add features to amc without creating a dead-end (where
generated code is broken, but cannot be re-generated because amc needs to be rebuilt)

`acr_ed -showcpp -sandbox` runs amc in sandbox, shows resulting change using `git diff`,
and then compiles and runs unit tests. The command succeeds if all of the steps succeed.
This can be very useful when modifying amc itself, since introducing an error into a generator
phase may break compiling of amc and thus prevent fixing of such error without resorting to
`git reset --hard`.

### Reading Code of Amc Itself

Amc source code is located under cpp/amc. The list of all the source files and headers
can also be examined with `acr targsrc:amc/%`

amc input tables are in `data/amcdb` and `data/dmmeta`; The full list can be obtained
with `acr_in amc`.

### What is a Cross-reference?

The [Relational Model](https://en.wikipedia.org/wiki/Relational_model) is a universal
way to represent knowledge, first described by Edgar Codd.
It is related to [Zermelo–Fraenkel set theory](https://en.wikipedia.org/wiki/Zermelo%E2%80%93Fraenkel_set_theory).

It is a good foundation. In a relational data model, all data stored in a set of tables.
Each table has a set of records. Each record's first field is its key, and all other fields
are either basic types (like integer or string), or references to other records.

The reference can take several forms -- it could be a direct pointer (Upptr),
or a primary key (Pkey). In either case, the whole
reason we write compiled language programs instead of solving all problems using SQL and MariaDB,
is that reversing the direction of reference lookup --
*finding all records that point to a given record* -- is expensive.
Cheaper cross-references is really the reason why most programs exist in the first place.
In database terms, a cross-reference is called a group-by;
In amc, cross-reference is
always incremental -- automatically updated as new records are added or removed from tables.

A cross-reference is established by use of an xref record. Here is a random example from amc, which
needs to keep track of targets and their dependencies.

    dmmeta.field  field:amc.FTarget.c_targdep  arg:amc.FTargdep  reftype:Ptrary  dflt:"  comment:"
      dmmeta.ptrary  field:amc.FTarget.c_targdep  unique:Y
        dmmeta.xref  field:amc.FTarget.c_targdep  inscond:true  via:amc.FDb.ind_target/dev.Targdep.target

This says: Dear amc, whenever a `targdep` record is inserted in my program, find an instance
of `target` by using global index `ind_target` with key `dev.Targdep.target` as the key, and
insert `targdep` into an array of pointers rooted in `target`.
Whenever a `targdep` record is deleted, automatically remove it from the list.
(Removing from a Ptrary is expensive but the last part won't be needed).

The main xref types supported by amc are Ptr, Ptrary, Llist, Thash, Bheap, Atree and Count.

Xrefs can be easily added and removed either by hand (with `acr -e` or by editing ssimfiles
directly), or using `acr_ed`. In the example above, `acr_ed -create -field amc.FTarget.c_targdep -write`
would be enough to establish the xref.

There can be any number of xrefs between any two tables. So, if you want a record to be
a member of ten linked lists and eleven binary heaps -- you're free to do so.
Xrefs are exactly analogous to RDBMS indexes; except xrefs can be established between any two
tables, so they are also `partitioned indexes` and incremental `group by`s at the same time.

I would re-state here the fact that these xrefs, or indexes, are secondary to the data.
The information needed to establish the xrefs must be contained in the data itself. This typically means
extra information in the leaves.

It doesn't make sense to say "hash of X" or "tree of Y". Elements of X and Y are primary, and exist even
if you remove all meaningful access paths to them. Instead, we speak of "access to X". The indexes
are roads, and there can be many roads to the same place.

To visualize xrefs, it may be useful to use `amc_vis`.

### Main Input Tables

The main tables Amc uses as input are ns, ctype and field.
Ns maps to a C++ namespace.
Ctype corresponds to a struct.
Field corresponds to a struct member or some derived value.

The main attributes of a field are its name, arg, and reftype.
Arg is the type, and reftype is a modifier, or 'type constructor'.

    dmmeta.field  field:amc.FDb.ind_ns        arg:amc.FNs        reftype:Thash   dflt:""  comment:""
    dmmeta.field  field:dmmeta.Field.field     arg:algo.Smallstr100  reftype:Val  dflt:"  comment:"

### String types and how to use them:

Throughout the code base, you will see several string types in use.
They are fairly straightforward, and they're all described below.

* `algo::strptr`: Length-delimited string pointer.
There are two fields in this struct: `char *elems, int n_elems`;
The string is just n chars starting at elems;
No null-termination is assumed or allowed.
It's safe to use strptr in function prototypes when the argument isn't
  modified by the function -- all other string types can be converted
  to such temporary strptr and passed to a function.
amc's hash find uses strptr as argument type whenever the key type is some
kind of string.

* `algo::cstring`: Tary of char. Contents can be cast to strptr.
Fields are: `char *ch, int ch_n, int ch_max`.
operator = is defined for cstring, so these can be assigned like values

* `algo::tempstr`: cstring, to be used only as the return type of a function,
in order to avoid copying data. Has the property that when assigned to
cstring, the contents are moved instead ("move constructor semantics").

- DO NOT assign tempstr, cstring, or a temporary variable to a strptr, since strptr
is just a pointer.
- DO NOT pass cstring& to a function when strptr is sufficient.
- DO NOT return cstring from functions, it will result in extra alloc/copy/delete.
 Return tempstr instead.

In addition to these string types, which are fully sufficient for all practical purposes,
there are many possible fixed-length string types that are generated by amc, mostly
for protocol-handling and database interoperability purposes.

#### Amc-generated String Types

A lot of these are are already defined, but new ones can be built as  needed.
use "acr smallstr" for the full list. Brief explanation below.

* `algo::SmallstrN`: pascal string with N characters. Both data and length
are part of the struct. Don't send this over the wire, because unused
portions of the string may contain garbage from the stack.

* `algo::RspaceStr{1..255}`: Fixed-length string field padded on the right with spaces.
Can be sent over the wire (included in protocol types)
* `algo::RnullStr{1..255}`
* `algo::LspaceStr{1..255}`: similar to the above

* `algo::LnumStr{1..N}_U{32,64}`: Fixed-length string field, padded on the left with '0'
(ascii character '0', not the NUL character). Includes functions to convert to/from a u32/u64.
Number cannot be negative, because left-padding with 0 prevents that.

* `LnumStr{1..N}_U{32,64}_Base{B}`: Same as above, but different base.

### Field Name Prefix

The functions amc generates are all global functions. I.e. they are not member
functions of any class. The function name is usually built from the name of the related
field, plus a name. For example, a big-endian u32 called `value` will  cause a function
named `value_Get` to be generated. A linked list field called `zd_target` will cause
amc to create functions `zd_target_Insert`, `zd_target_Prev`, etc. These function names
are readable because the field name contains a hint about its type.

The table `dmmeta.fprefix` defines a constraint between field prefix and a field reftype,
as interpreted by amc. If the field's name begins with the prefix (such as zd), followed
by an underscore, then the field must have the specified reftype.

The defined prefixes are:

```
inline-command: acr fprefix -report:N -cmd 'printf "%5s %10s\n" $fprefix $reftype' |sh
   bh      Bheap
    c     Ptrary
   cd      Llist
  cdl      Llist
  cnt      Count
   cs      Llist
  csl      Llist
  ind      Thash
    p      Upptr
   tr      Atree
   zd      Llist
  zdl      Llist
   zs      Llist
  zsl      Llist
```

Additional prefixes may be defined by the user.

### The Algorithm For Generating Code

The main algorithm for generating any code (not just with amc) is simple:

1. Manually write the code that you want to generate and make sure it works.
2. Put a print statement around each line.
3. Move the resulting code to the appropriate place in your generator.
4. Run your generator. Now you have 2 copies of your code: one you started with, and the
  other that was printed by the generator. If you did everygthing right, you should get a
  link error now.
5. Delete the manually written code.
6. Parameterize the generator so that it can generate a whole family of implementations that
  look similar.

It is usually not a good idea to start programming new features in amc itself.
It is very tiresome to debug such code. The code should already have been written
by hand, possibly a couple of times, to the point where the duplication occurs, but the
different implementations cannot be unified because of either unacceptable performance costs,
or too many dependencies. Such code is to be lifted into a generator.
## Dispatch



~TBD~

## Amc Features

### Scaled Decimal Types

A scaled decimal type is an integer with an implied decimal point. For instance, a number
with two implied decimal places can be constructed with an `fdec` record, specifying `nplace:2`.

Attribute `fixedfmt` controls whether trailing zeros are left in place when printing.
With `fixedfmt:Y`, 0 is printed as `0.00`, otherwise it is just printed as 0.

Example:

    dmmeta.field  field:algo.U64Dec2.value  arg:u64  reftype:Val  dflt:""  comment:""
      dmmeta.fdec  field:algo.U64Dec2.value  nplace:2  fixedfmt:Y  comment:"0 prints as 0.00"

The scaled decimal type generates functions to convert between the stored (scaled) value
and a numeric value: `value_qSetDouble`, `value_SetDoubleMaybe`, `value_GetDouble`, `value_GetInt`.

    // Set value of field value.
    // The value is rounded to the nearest integer.
    // This ensures that truncation of a near-integer value does not occur.
    // Example: 1.3 cannot be represented exactly as a double, the actual
    // stored value will be 1.29999999. when we apply C truncation,
    // we want to end up with 1.3, not 1.2.
    void                 value_qSetDouble(algo::U64Dec2& parent, double val) __attribute__((nothrow));

    double               value_GetDouble(algo::U64Dec2& parent) __attribute__((nothrow));

    // Return integer portion (divide number by 100)
    u64                  value_GetInt(algo::U64Dec2& parent) __attribute__((nothrow));

    // Return constant 100
    u64                  U64Dec2_GetScale() __attribute__((nothrow));

    // Set value of field value, using rounding.
    // If value is out of range for the target type, return false.
    bool                 value_SetDoubleMaybe(algo::U64Dec2& parent, double val) __attribute__((nothrow));

Functions to print and read the value are also generated.
You can query the generated source for `value_Print` with `amc algo.U64Dec2.value.Print`

    bool                 value_ReadStrptrMaybe(algo::U64Dec2& parent, algo::strptr in) __attribute__((nothrow));
    void                 value_Print(algo::U64Dec2& parent, cstring &outstr) __attribute__((nothrow));

### Big-Endian Fields

Adding an `fbigend` record to a field indicates that the in-memory byte order is big-endian.

    dmmeta.field  field:atf_amc.TypeBE16.value  arg:u16  reftype:Val  dflt:""  comment:""
      dmmeta.fbigend  field:atf_amc.TypeBE16.value  comment:""

Amc generates values `value_Get` and `value_Set` to access the value.

    u16                  value_Get(const atf_amc::TypeBE16& parent) __attribute__((__warn_unused_result__, nothrow));
    void                 value_Set(atf_amc::TypeBE16& parent, u16 rhs) __attribute__((nothrow));

When generating the C++ struct, amc adds `_be` to the field name so that the programmer
becomes aware, should they choose to access it manually, that the field may be encoded.

    $ amc atf_amc.TypeBE16%
    // --- atf_amc.TypeBE16
    struct TypeBE16 { // atf_amc.TypeBE16
        u16   value_be;   //   0
        TypeBE16();
    };

    ...

    // --- atf_amc.TypeBE16.value.Get
    inline u16 atf_amc::value_Get(const atf_amc::TypeBE16& parent) {
        return be16toh(parent.value_be); // read big-endian value from memory
    }

    // --- atf_amc.TypeBE16.value.Set
    inline void atf_amc::value_Set(atf_amc::TypeBE16& parent, u16 rhs) {
        parent.value_be = htobe16(rhs); // write big-endian value to memory
    }

    ...

### Bitfields

Amc supports bitfields. In the example below, the field `bits5` is defined
as bits 5..10 of the value of the field called `value`. It is important that
the source field is explicitly specified. This eliminates any ambiguity about where
in memory the bitfield may be. The source field can be any integer type.

Bitfields also work identically between big- and little-endian source fields.
The reason is that we define the bitfield in terms of a shift- and mask- operation
on the source field. It is purely an arithmetic operation that produces the value of the
bitfield.

If you've looked at Linux system headers for network protocols, you've seen ungainly `#ifdef` blocks.
They are needed in C because C bitfields are defined against memory, not against the value of
a source field.

    dmmeta.field  field:atf_amc.BitfldType1.value  arg:u64  reftype:Val     dflt:""  comment:""
    dmmeta.field  field:atf_amc.BitfldType1.bits5   arg:u64  reftype:Bitfld  dflt:""  comment:""
      dmmeta.bitfld  field:atf_amc.BitfldType1.bits5   offset:5  width:10  srcfield:atf_amc.BitfldType1.value  comment:""

A bitfield can have any type - `bool`, `u64`, or a custom type that wraps an integer.

    $ amc atf_amc.BitfldType1.bits5.% -proto

    // Retrieve bitfield from value of field value
    //    10 bits starting at bit 5.
    u64                  bits5_Get(const atf_amc::BitfldType1& parent) __attribute__((__warn_unused_result__, nothrow));

    // Set bitfield in value of field 'value'
    //    10 bits starting at bit 5.
    void                 bits5_Set(atf_amc::BitfldType1& parent, u64 rhs) __attribute__((nothrow));

The implementation is straightforward:

    $ amc atf_amc.BitfldType1.bits5.Set

    // --- atf_amc.BitfldType1.bits5.Set
    // Set bitfield in value of field 'value'
    //    10 bits starting at bit 5.
    inline void atf_amc::bits5_Set(atf_amc::BitfldType1& parent, u64 rhs) {
        u64 t1    = u64(0x3ff) << 5;
        u64 t2    = (u64(rhs) & 0x3ff) << 5;
        parent.value = u64((parent.value & ~t1) | t2);
    }

bitfields can be read from string tuples just like other fields.
When printing a ctype containing bitfields to an ssim tuple, amc does not print the source field;
instead, all the bitfields are printed instead.

When printing a ctype containing bitfields using the `Raw` format, only the source field is printed,
and bitfields are skipped.

It is an error to specify a combination of offset and width that is out of bounds for the source type.
Amc will flag this. It is also an error to have two bitfields overlap. All of the `offset+width`
ranges must be disjoint.

Here is an example of the code amc generates when the source field is big-endian.
(Endianness tends to be confusing, because languages try hard to erase the boundary between
memory and values, so the user gets an extra warning).

    $ amc atf_amc.TypeBE64sf.bit63.Get

    // --- atf_amc.TypeBE64sf.bit63.Get
    // Retrieve bitfield from value of field value
    //    1 bits starting at bit 63.
    //    NOTE: bits correspond to the the integer value of the field.
    //    The value is obtained by reading bytes from memory and swapping them.
    inline u64 atf_amc::bit63_Get(const atf_amc::TypeBE64sf& parent) {
        return u64((value_Get(parent) >> 63) & 0x01);
    }

In order to facilitate flexible handling of named bits within bitsets, 
special enum constants are generated for bitfld with width=1 and bool arg.

### Bitset

Main goal of bitsets is compact string representation of set of named bits.

Bitset is special print/read format for ctype. It deals with boolean fields only, other field types are ignored.
It does not matter how boolean values are grouped - free ctype fields, or bitfields distributed among one 
or more integer storage fields. All the same, it forms single space of named bits.

The format is comma-separated list of field names those are set to `true`.
For example, ctype has tree boolean fields: f1, f2, f3. Assume f1=true, f2=false, f3=true.
Print result: `f1,f3`. Read function for this ctype recognizes such format and sets fields f1 and f3 to true.

### Steps

Steps are a scheduling construct to associate actions to fields. A step is defined by adding
a `dmmeta.fstep` record; `fstep is a subset of field, and specifically only global fields
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

#### Defining a Step

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

#### Step Types

There are several ways to get to the `_Step` function.
1. We can call the user-defined function on every cycle through the main loop. This step type is called `Inline`.
The only check performed is that the control field is non-empty.
2. We can call the user-defined function when the `algo_lib::_db.clock` reaches a certain value. This
step type is `InlineOnce`. The value is defined next to the field, with suffix `_next`.
3. We can call the user-defined function every `_delay` clock units. This step type is called `InlineRecur`.
Two control variables are defined, `_next` and `_delay` (of type u64). The delay can be controlled at
runtime with `_SetDelay` function. Since the step type is still `Inline`, the cost of adding such step is a few
instructions being added to the main loop.
4. We can call the user-defined function every `_delay` clock units, but scheduled as a `Timehook` via a binary
heap (this is a built-in index called `algo_lib.FDb.bh_timehook`. This is of course the most scalable
and universal scheduling
method, since checking the top entry in the heap is O(1) and rescheduling is O(log N), but if you want to design
a tuned engine with optimal pipelining behavior, you will need to use `Inline` variants.

### Tracing

*UNDER CONSTRUCTION*.
Ftrace record can be used to enable counting of alloc/delete calls for each pool.

### Base: Mixin

*UNDER CONSTRUCTION*.

The Base reftype copies all fields from one arg to field's parent ctype.
There are two main use cases.

1. Protocol messages, where Base is used to declare a common message header for all message types.
Amc generates functions to go back and forth between header and specific type using Castdown / Castbase functions.

2. In-memory tables based on ssimfiles, which inherit all of the fields defined
in the ssimfile. These do not allow casting back and forth since memory layouts differ, there
are extra pointers in the in-memory version, some attributes are being used for cross-references (joins), etc.

### Bheap: Binary Heap

*UNDER CONSTRUCTION*.

The binary heap is implemented as an flat array of pointers (e.g. a Ptrary).

### Bitset: Bitset over an array

Amc can generate code to get / set bits in any field that is an integer (`u8`, `u16`,
`u32`, `u64`, or an array of integers (`Inlary`, `Tary`).

Here is an example:

```
inline-command: acr -t field:atf_unit.Bitset.fld8 | egrep Bitset
    dmmeta.ctype  ctype:atf_unit.Bitset  comment:"Test bitset"
      dmmeta.field  field:atf_unit.Bitset.fld8  arg:u8  reftype:Val  dflt:""  comment:"Bitset field"
        dmmeta.fbitset  field:atf_unit.Bitset.fld8  comment:""
```

The generated functions are as follows:

```
inline-command: amc -report:N -proto atf_unit.Bitset.fld8.%
// Return constant 1
int                  fld8_N(atf_unit::Bitset& parent) __attribute__((__warn_unused_result__, nothrow, pure));
// Access value
u8&                  fld8_qFind(atf_unit::Bitset& parent, int) __attribute__((__warn_unused_result__, nothrow));
// Get max # of bits in the bitset
// Return max. number of bits supported by array
int                  fld8_Nbits(atf_unit::Bitset& parent) __attribute__((__warn_unused_result__, nothrow));
// Retrieve value of bit #BIT_IDX in bit set. No bounds checking
bool                 fld8_qGetBit(atf_unit::Bitset& parent, u32 bit_idx) __attribute__((__warn_unused_result__, nothrow));
// Retrieve value of bit #BIT_IDX in bit set. If bit index is out of bounds, return 0.
bool                 fld8_GetBit(atf_unit::Bitset& parent, u32 bit_idx) __attribute__((__warn_unused_result__, nothrow));
// Check if all the bits in the bitset are equal to zero
bool                 fld8_BitsEmptyQ(atf_unit::Bitset& parent) __attribute__((__warn_unused_result__, nothrow));
u64                  fld8_Sum1s(atf_unit::Bitset& parent) __attribute__((__warn_unused_result__, nothrow));
// Clear bit # BIT_IDX in bit set. No bounds checking
void                 fld8_qClearBit(atf_unit::Bitset& parent, u32 bit_idx) __attribute__((nothrow));
// Clear bit # BIT_IDX in bit set. If bit index is out of bounds, do nothing
void                 fld8_ClearBit(atf_unit::Bitset& parent, u32 bit_idx) __attribute__((nothrow));
// Set bit # BIT_IDX in bit set. No bounds checking
void                 fld8_qSetBit(atf_unit::Bitset& parent, u32 bit_idx) __attribute__((nothrow));
// Set bit # BIT_IDX in bit set. If bit index is out of bounds, do nothing.
void                 fld8_SetBit(atf_unit::Bitset& parent, u32 bit_idx) __attribute__((nothrow));
// Set bit # BIT_IDX in bit set. No bounds checking
void                 fld8_qSetBitVal(atf_unit::Bitset& parent, u32 bit_idx, bool val) __attribute__((nothrow));
// Or bit # BIT_IDX in bit set. No bounds checking
void                 fld8_qOrBitVal(atf_unit::Bitset& parent, u32 bit_idx, bool val) __attribute__((nothrow));
// Set all bits of array to zero.
// Note: this does not change what NBits will return.
void                 fld8_ClearBitsAll(atf_unit::Bitset& parent) __attribute__((nothrow));
// Zero in PARENT any bits that are set in RHS.
void                 fld8_ClearBits(atf_unit::Bitset& parent, atf_unit::Bitset &rhs) __attribute__((nothrow));
// Set PARENT to union of two bitsets.
// (This function is not named Set.. to avoid triple entendre).
void                 fld8_OrBits(atf_unit::Bitset& parent, atf_unit::Bitset &rhs) __attribute__((nothrow));
// Return smallest number N such that indexes of all 1 bits are below N
i32                  fld8_Sup(atf_unit::Bitset& parent) __attribute__((__warn_unused_result__, nothrow));

```
### Count: Count elements

*UNDER CONSTRUCTION*.

Count is a xref type that simply keeps track of the number of child
elements referring to a given parent. The elements themselves are not accessible
via this field.

### Inlary: Inline array

When `amc` sees an `inlary` record, it generates code to turn a single value field
into an inline array of up to N values. An inline array's memory is reserve directly in the
record which houses it. There is no dynamic allocation. There are two types of `Inlary`:
ones where `min = max`, in this case a C++ array is generated and we talk of a `fixed` array.
And the case where `min < max`. In this case, `amc` declares an array of `u8` of appropriate size,
and generates generates usual pool functions `Alloc`, `RemoveLast`, `RemoveAll`.

```
inline-command: acr -t field:algo_lib.FDb.temp_strings | egrep algo_lib.FDb
    dmmeta.ctype  ctype:algo_lib.FDb  comment:""
      dmmeta.field  field:algo_lib.FDb.temp_strings  arg:algo.cstring  reftype:Inlary  dflt:""  comment:"* initialization order is important *"
        dmmeta.inlary  field:algo_lib.FDb.temp_strings  min:8  max:8  comment:""
```

Here is an example of functions generated for a fixed array:

```
inline-command: amc -report:N -proto algo_lib.FDb.temp_strings.%
// Set all elements of fixed array to value RHS
void                 temp_strings_Fill(const algo::cstring &rhs) __attribute__((nothrow));
// Look up row by row id. Return NULL if out of range
algo::cstring*       temp_strings_Find(u64 t) __attribute__((__warn_unused_result__, nothrow));
// Access fixed array temp_strings as aryptr.
algo::aryptr<algo::cstring> temp_strings_Getary() __attribute__((nothrow));
// Return max number of items in the array
i32                  temp_strings_Max() __attribute__((nothrow));
// Return number of items in the array
i32                  temp_strings_N() __attribute__((__warn_unused_result__, nothrow, pure));
// Set contents of fixed array to RHS; Input length is trimmed as necessary
void                 temp_strings_Setary(const algo::aryptr<algo::cstring> &rhs) __attribute__((nothrow));
// 'quick' Access row by row id. No bounds checking in release.
algo::cstring&       temp_strings_qFind(u64 t) __attribute__((nothrow));

```
```
  dmmeta.field  field:algo_lib.FDb.temp_strings  arg:algo.cstring  reftype:Inlary  dflt:""  comment:""
    dmmeta.inlary  field:algo_lib.FDb.temp_strings  min:8  max:8  comment:""
```

And here are the functions generated for a dynamically sized inline array:

```
inline-command: amc -report:N -proto atf_amc.FTypeA.typec.%
// Allocate memory for new default row.
// If out of memory, process is killed.
atf_amc::FTypeC&     typec_Alloc(atf_amc::FTypeA& typea) __attribute__((__warn_unused_result__, nothrow));
// Allocate memory for new element. If out of memory, return NULL.
atf_amc::FTypeC*     typec_AllocMaybe(atf_amc::FTypeA& typea) __attribute__((__warn_unused_result__, nothrow));
// Create new row from struct.
// Return pointer to new element, or NULL if insertion failed (due to out-of-memory, duplicate key, etc)
atf_amc::FTypeC*     typec_InsertMaybe(atf_amc::FTypeA& typea, const atf_amc::TypeC &value) __attribute__((nothrow));
// Allocate space for one element. If no memory available, return NULL.
void*                typec_AllocMem(atf_amc::FTypeA& typea) __attribute__((__warn_unused_result__, nothrow));
// Return true if index is empty
bool                 typec_EmptyQ(atf_amc::FTypeA& typea) __attribute__((nothrow));
// Look up row by row id. Return NULL if out of range
atf_amc::FTypeC*     typec_Find(atf_amc::FTypeA& typea, u64 t) __attribute__((__warn_unused_result__, nothrow));
// Return array pointer by value
algo::aryptr<atf_amc::FTypeC> typec_Getary(atf_amc::FTypeA& typea) __attribute__((nothrow));
// Return constant 10 -- max. number of items in the pool
i32                  typec_Max(atf_amc::FTypeA& typea) __attribute__((nothrow));
// Return number of items in the array
i32                  typec_N(const atf_amc::FTypeA& typea) __attribute__((__warn_unused_result__, nothrow, pure));
// Destroy all elements of Inlary
void                 typec_RemoveAll(atf_amc::FTypeA& typea) __attribute__((nothrow));
// Delete last element of array. Do nothing if array is empty.
void                 typec_RemoveLast(atf_amc::FTypeA& typea) __attribute__((nothrow));
// 'quick' Access row by row id. No bounds checking in release.
atf_amc::FTypeC&     typec_qFind(atf_amc::FTypeA& typea, u64 t) __attribute__((nothrow));
// Compute row id of element given element's address
u64                  typec_rowid_Get(atf_amc::FTypeA& typea, atf_amc::FTypeC &row) __attribute__((nothrow));
// Insert row into all appropriate indices. If error occurs, store error
// in algo_lib::_db.errtext and return false. Caller must Delete or Unref such row.
bool                 typec_XrefMaybe(atf_amc::FTypeC &row);

```

### Inlary With Gstatic

One particularly cool use of Inlary is with gstatic. Whenever gstatic is specified, the contents of an ssim
table are ``statically'' included into the generated source file. This is done by reading the table
at generating time and then inserting its elements, as strings, into the target table, as part of FDb's constructor.

When gstatic is combined with Inlary and the field is global (i.e. in `FDb`), the individual addresses
of elements in the table are compile-time constants. Amc celebrates this fact by declaring a C++ compiler symbol
for each row of the table. By the time the user code starts executing, all of the elements have been inserted
in the array and properly cross-referenced, and the user code can use the generated globals to access table rows.

Amc itself uses gstatic in several places. Try `acr gstatic:amc.%` to check the uses.

### Gstatic With Hook

Tables marked `gstatic` have known contents. This enables associating entries of such tables with C functions.
Consider this example from amc:
```
  dmmeta.field  field:amc.FDb.gen  arg:amc.FGen  reftype:Lary  dflt:""  comment:""
    dmmeta.gstatic  field:amc.FDb.gen  comment:""

  dmmeta.field  field:amc.FGen.step  arg:""  reftype:Hook  dflt:""  comment:""
      dmmeta.hook  field:amc.FGen.step  comment:""
```
Here, a static table `gen` has been declared, and its contents are populated with entires from
`amcdb.gen` in amc's FDb constructor. The table can have a hash index by primary key and there
is not restriction on any other cross-references. For this pattern of declaration, amc forward-declares one
function foreach element of the table, and assigns its address to the `hook` field of `amc::FGen` upon construction.
If the function is not defined in the user process, a link error will result.

This construct is very useful for things like unit tests, consistency checks, alerts, and other situations where
an extensible list of functions is needed together with meta-information about them.
Each function has an associated record which can be looked up by name, so it is possible to invoke all
functions matching a regex passed on command line, or describe them with comments.

Example:
Show first 10 entries from amcdb.gen:

```
inline-command: acr amcdb.gen | sort -k 2 | head



amcdb.gen  gen:basepool         perns:N  comment:"Create basepools based on defaults"
amcdb.gen  gen:bitfldenum         perns:N  comment:"Generate fconsts from bool bitfield"
amcdb.gen  gen:cget             perns:N  comment:"Generate helpful Q functions based on fconsts and bools"
amcdb.gen  gen:check_basefield    perns:N  comment:"Check Base usage"
amcdb.gen  gen:check_basepool   perns:N  comment:"Check basepool order"
amcdb.gen  gen:check_bigend     perns:N  comment:"Check big-endians"
amcdb.gen  gen:check_bitfld     perns:N  comment:"Check that bitfields don't overlap"
```

Select first 10 functions from amc source code:
```
inline-command: src_func amc gen_% -proto -comment: | sort -k 3 | head
cpp/amc/gen.cpp:220: void amc::gen_basepool() 
cpp/amc/cget.cpp:123: void amc::gen_cget()
cpp/amc/gen.cpp:577: void amc::gen_check_bigend() 
cpp/amc/gen.cpp:296: void amc::gen_check_bitfld() 
cpp/amc/gen.cpp:112: void amc::gen_check_cascdel() 
cpp/amc/gen.cpp:460: void amc::gen_check_cpptype() 
cpp/amc/gen.cpp:1365: void amc::gen_check_fcurs() 
cpp/amc/gen.cpp:205: void amc::gen_check_prefix() 
cpp/amc/gen.cpp:395: void amc::gen_check_reftype() 
cpp/amc/gen.cpp:161: void amc::gen_check_static() 
```

In language-centric systems, one would start by writing some functions, and then use "reflection" to
scrape the functions from the code (usually at runtime) and do things with them. Starting with a plain text file
on disk, and then generating C++ functions and in-memory tables from it is a lot more elegant. No "reflection"
is needed, everything is forward-looking.

### Lary: Level array

*UNDER CONSTRUCTION*.

### Llist: Linked list

*UNDER CONSTRUCTION*.

Amc generates 32 flavors of linked lists, almost as many as Heinz for ketchup.
Linked lists can be:

Singly or doubly linked (as indicated by letter s or d in the prefix)
Zero-terminated or circular (as indicated by letter z or c in the prefix)
Fifo or lifo (as indicated by presence of letter l in the prefix, l for lifo)
With or without count
With or without tail pointer

Circular linked lists are often used with steps, because it is convenient to call RotateFirst
to both grab an element off the head of the list, and move this element to the back.

### Opt: Optional last field in variable-length struct

*UNDER CONSTRUCTION*.

Opt corresponds to 0 or 1 values at the end of a struct. This type is frequently used by protocols
to specify optional payload. Amc provides functions that can validate and access the payload.

### Pmask: Presence mask

*UNDER CONSTRUCTION*.

Amc provides convenient presence mask support. If any field in a ctype is marked Pmask,
then amc allocates 1 bit in the pmask for each field in the struct. The bits are initially zero.
Amc then generates a Set, PresentQ, and SetPresent functions for each field, unless one already exists.
Whenever the Set function is called on a field, the corresponding pmask bit is set to 1.
When reading from a string, the pmask bits are populated for each scanned attribute.
When printing, fields with zero pmask bit are not printed.

Regardless of pmask status, all fields of a struct are initialized to default values upon construction,
so there is no speed to be gained from using pmask. Access to fields is not protected in any way --
they can be accessed as if the pmask didn't exist. The generated PresentQ function can be used to check
if a field was previously Set.

### Ptr

*UNDER CONSTRUCTION*.

Ptr is a cross-reference type used when there can be 0 or 1 child records for each parent record.

### Ptrary

*UNDER CONSTRUCTION*.

Tary of Ptrs.

### RegxSql:

*UNDER CONSTRUCTION*.

When using this field type, amc ignores the field arg and inserts an algo_lib.Regx into
the parent structure. The expression intended to match the primary key of the target type.
This reftype is very useful in command line arguments.

### Smallstr

*UNDER CONSTRUCTION*.

Smallstr is a fixed-length character field. Memory is reserved inline in the parent struct.
Strings can be length-suffixed (Rpascal), left-padded or right-padded.
For padded strings, the string value is calculated by discarding the pad characters from the edge.
Any smallstr is castable to strptr.

### Thash: hash table

*UNDER CONSTRUCTION*.

Hash table, implemented as a Tary of pointers.
Collisions are implemented as a singly linked list. Hash tables grow automatically.
Whenever the number of entries in the hash table is greater
than the number of buckets (pointers), the hash table size is doubled.
The size of the array of pointers is always a power of 2. This means that a hash function has
to be good. Amc can generate hash functions for any ctype, typically CRC32 is used.

Hash tables can be unique of non-unique. A unique hash table rejects insertions of duplicate keys.
A non-unique hash table allows them.

For records that have only one hash access path defined for them, amc generates a GetOrCreate function
which is a convenient way to force creation of an element when you know its key.

### Upptr:

*UNDER CONSTRUCTION*.

Upptr is a pointer to a pre-existing record, usually non-null.
Upptr is a reference. By contrast, Ptr is a cross-reference.
I.e. a Ptr becomes non-null when another record starts pointing at the parent of the Ptr.
Upptr becomes non-null when a lookup in an index is performed.
amc_vis will complain if there are circular dependencies implied by Upptr (i.e. A has Upptr to B,
B has Upptr to A). Circular dependencies between Ptr fields are OK.

### Varlen: variable-length tail portion of a struct

*UNDER CONSTRUCTION*.

Varlen corresponds to a (possibly empty) array of fixed-size records appended to the end of a struct.
the parent ctype must have a length field defined. varlen fields cane be read from a string or
iterated over. This type is frequently used by wire protocols to specify a repeated section of a message.

## amc_gc: AMC garbage collector

`amc_gc` is a tool for removing unused records from the dmmeta database.

`amc_gc` takes a target regex and an acr query as an argument. It finds all records
matching the query, then it tries to remove them one by one, and rebuilds the targets
(using abt) in a sandbox dir. If build succeeds with one of the records deleted
it means it wasn't needed in the first place.

Let's illustrate `amc_gc` by creating a new program and inputting a table.

    $ acr_ed -create -target sample -write
    $ acr_ed -create -finput -target sample -ssimfile dmmeta.ns -write

Since the `ns` table is unused, `sample` will compile even if we remove it. This is the
case that `amc_gc` detects, and can remove the table:

    $ amc_gc -target:sample -key:ctype:sample.%
    amc_gc.begin  tot_rec:2  n_cppline:259802  watch_cmd:"watch head -50 temp/amc_gc.build"
    amc_gc.analyze  query:dmmeta.ctype:sample.FDb  eliminate:N  rec_no:1  tot_rec:2  n_del:0  n_cppline:259802  n_cppline_del:0
    amc_gc.analyze  query:dmmeta.ctype:sample.FNs  eliminate:Y  rec_no:2  tot_rec:2  n_del:1  n_cppline:259341  n_cppline_del:461
    report.amc_gc  key:ctype:sample.%  n_match:2  n_del:1  n_cppline:259341  n_cppline_del:461

And indeed, `amc_gc` successfully garbage collects the table.
Let's finish by deleting the unused target

    $ acr_ed -del -target sample -write
## Hello Meta World

Follow the steps below to create a new sample program.

The program will print the names of all of its own structures, and
their fields, cross-referenced twice: first, by membership and
then by computing all back-references.

This seems like an appropriately self-referential way to say hello
using the tools at our disposal. Having a program print its own data
structure is also mind-boggling if you think about it for too long.

Use this as a starting point, or to get motivated to read one of the tutorials.

~~~
acr_ed -create -target hi -write
cat > cpp/samp/hi.cpp << EOF
#include "include/hi.h"
void hi::Main() {
    prlog("Hello Meta World!");
    ind_beg(hi::_db_ctype_curs,ctype,hi::_db) {
        if (ns_Get(ctype) == dmmeta_Ns_ns_hi) {
            prlog("ctype "<<ctype.ctype);
            ind_beg(hi::ctype_zd_field_curs,field,ctype) {
                prlog("    has field "<<field.field<<" of type "<<field.arg<<" reftype:"<<field.reftype);
            }ind_end;
            ind_beg(hi::ctype_zd_arg_curs,arg,ctype) {
                prlog("    is referred to by field "<<arg.field<<" using "<<arg.reftype);
            }ind_end;
        }
    }ind_end;
}
EOF
acr_ed -create -finput -target hi -ssimfile:dmmeta.ctype -indexed -write
acr_ed -create -finput -target hi -ssimfile:dmmeta.field -write
acr_ed -create -field:hi.FField.p_ctype  -arg:hi.FCtype -xref -via:hi.FDb.ind_ctype/dmmeta.Field.ctype -write
acr_ed -create -field:hi.FCtype.zd_field -arg:hi.FField -xref -via:hi.FDb.ind_ctype/dmmeta.Field.ctype  -write
acr_ed -create -field:hi.FCtype.zd_arg   -arg:hi.FField -xref -via:hi.FDb.ind_ctype/dmmeta.Field.arg    -write
abt -install hi
hi
~~~
## Amc: Memory Pools

For each `ctype`, instances of which can be dynamically allocated
(i.e. not on the stack), amc generates two functions: `Alloc` and `Delete`.

This section will explain all of the available pool types, how to chain them
so that one pool allocates its own memory from another, and provide examples.

In `amc`, memory pools are fields with specific reftypes. They are given names and
they can be referred to.

### Types of Memory Pools

A memory pool can be thought of as any mechanism by which new instances of records can be allocated.
Amc provides the following base set of memory pools, listed here
in alphabetical order.

* Blkpool - A mostly-fifo allocator based on refcounted blocks.
* Delptr - Indirect Val; A single value, always freed in destructor.
* Inlary - A piece of memory for min..max elements inside the parent struct.
 When `min == max`, there is no `Alloc` function, and it behaves like N `Val`s.
 When `min < max`, new elements can be allocated. Only the last element can be freed.
* Lary - Effectively an Inlary of 32 Tarys, each of size 2^k. Has permanent pointers.
* Lpool - 32 Tpools, each for elements of size up to 2^k.
* Malloc - Simply calls malloc() / free().
* Sbrk - Calls sbrk() to get memory.
* Tary - Pointer to a resizable array (typically growing by powers of 2). Similar to std::vector.
* Tpool - A singly linked list of free elements; Gets new memory from base pool, frees
  elements into the list.
* Val - A single value, automatically allocated in constructor, and freed in destructor.

### Declaring a Pool

A pool is declared like a field.

    dmmeta.field  field:acr.FDb.file  arg:acr.FFile  reftype:Lary  dflt:""  comment:"List of all loaded files"

This provides a hook for `amc` to generate the functions initializing
and maintaining the state of the pool.

Amc keeps track of all the pools which claim to be able to provide memory for a given ctype.
In the example above, `amc` would generate functions `file_Alloc` and `file_Delete`.
The names of these functions are not derived from the ctype; They are derived
from the pool name. It is the pool's state that gets altered when one of these functions
is called.

### Base Pools

For any pool, you can specify where to get memory from. This is called a basepool,
and is specified with a `dmmeta.basepool` record.
In the example below, the `Lpool` `algo_lib.FDb.lpool` calls `sbrk_AllocMem`
whenever it needs more memory.

    dmmeta.field  field:algo_lib.FDb.sbrk   arg:u8  reftype:Sbrk   dflt:""  comment:"Base allocator for everything"
    dmmeta.field  field:algo_lib.FDb.lpool  arg:u8  reftype:Lpool  dflt:""  comment:"private memory pool"
      dmmeta.basepool  field:algo_lib.FDb.lpool  base:algo_lib.FDb.sbrk

The basepool is a really great feature. Imagine a realtime program with strict
memory requirements; Everything must be pre-allocated and system calls are not allowed during runtime,
since they would cause latency spikes 1000x of what's allowed. The configuration of memory
pools can be done entirely outside of the source code base. One can write straightforward bash scripts that try
different memory pools and measure their relative performance.

To avoid having to specify a base pool for every single pool in a given
namespace, you can (actually, you must) specify a default namespace pool in the `dmmeta.nsx` record.

    dmmeta.nsx  ns:algo_lib   genthrow:N  correct_getorcreate:Y  pool:algo_lib.FDb.lpool   sortxref:N  pack:N  fldoffset_asserts:N  comment:""

### Running Out Of Memory

There are two ways to run out of memory: voluntarily (because you decide a table got too big)
and involuntarily (the OS refuses to give you more memory). The first method is on the user --
`amc` does not support pool limits. When the OS is out of memory, the function either exits
the calling process (it's a fatal error) or returns NULL.  A function
with an unambiguous name such as `Alloc` will kill the process when an out-of-memory condition occurs.
A function ending in `Maybe`, such as `AllocMaybe`, will return NULL.

One of the use cases of `amc` is to generate deterministic processes. Think of two programs, running
in parallel on two different physical hosts, processing the same sequence of messages. The output
of both of these processes is sent to the same destination and de-duplicated based on sequence numbers.
This is a hot-hot redundancy scenario with great latency characteristics (the faster of the two messages
becomes the output, shaving off some latency spikes). In any case, the output of the two proceses
must be identical and depend only on the input. This means that a process is *not allowed* to strategize
around low-memory conditions. That's why exiting on out-of-memory is a valid, in fact the only possible
strategy.

### Blkpool

The block pool is a free-list of large-ish blocks. Memory is allocated from the current block,
with others serving as reserve. Allocated elements contain a back-pointer to the beginning
of the block, and increment a refcount on the block. Elements are allocated back-to-back
starting at the beginning of the block until the block is exhausted, respecting the necessary
alignment.

When freeing memory, the refcount is decremented but memory cannot be reused (yet). Only when
the block's refcount goes to zero, the entire block goes back to the free list. This allocator
is suitable for messages that are being processed using a fifo strategy. It is not good
for random access patterns because one unfreed message can hold an entire block in memory,
eventually exhausting memory.

### Delptr: Optional Value

Delptr is an indirect Val. The parent struct gets a pointer which is initially `NULL`.
When you call `_Access`, and the point is still `NULL`, a new value is allocated.
When the the struct is deleted, the value is freed.

Here is an example of a `Delptr` field:

    dmmeta.ctype  ctype:atf_amc.DelType1  comment:"Delptr test 1"
      dmmeta.field  field:atf_amc.DelType1.u32val  arg:u32  reftype:Delptr  dflt:34  comment:""

Here is the generated struct:

    // --- atf_amc.DelType1
    struct DelType1 { // atf_amc.DelType1: Delptr test 1
        u32*   u32val;   // Private pointer to value
        DelType1();
        ~DelType1();
    private:
        // reftype of atf_amc.DelType1.u32val prohibits copy
        DelType1(const DelType1&){ /*disallow copy constructor */}
        void operator =(const DelType1&){ /*disallow direct assignment */}
    };

And here is the implementation of `_Access`.
The underlying memory pool used is the one associated with its namespace
via the `nsx` record.

    // --- atf_amc.DelType1.u32val.Access
    // Get or Create
    // Access value, creating it if necessary. Process dies if not successful.
    u32& atf_amc::u32val_Access(atf_amc::DelType1& parent) {
        u32 *ret=parent.u32val;
        if (!ret) {
            ret = (u32*)algo_lib::malloc_AllocMem(sizeof(u32));
            if (!ret) {
                FatalErrorExit("out of memory allocating u32 (in atf_amc::DelType1.u32val)");
            }
            new(ret) u32(34);
            parent.u32val = ret;
        }
        return *ret;
    }

### Inlary: Inline Array

*Tbd*

### Lary: Level array

Lary is implemented as 32 pointers in the parent struct. Level k holds as pointer
to a block of elements of length 2^k. Indexed lookup involves just 1 memory access,
because amc uses BitScanReverse to find which level the element lives on. When a level is exhausted,
another level, 2x the size, is allocated. Since none of the previous levels need to be
reallocated, the pointers returned by Lary are stable and so elements can be freely cross-referenced.
Lary is the most common pool.

The permanent pointer promise is the main reason for Lary's existence. Lary is the default
pool for most records.
Here is an example of one:

    dmmeta.field  field:ssim2mysql.FDb.ctype  arg:ssim2mysql.FCtype  reftype:Lary  dflt:""  comment:""

And here are the generated functions.

#### Alloc
    // Allocate memory for new default row.
    // If out of memory, process is killed.
    ssim2mysql::FCtype&  ctype_Alloc() __attribute__((__warn_unused_result__, nothrow));

#### AllocMaybe

    // Allocate memory for new element. If out of memory, return NULL.
    ssim2mysql::FCtype*  ctype_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));

#### InsertMaybe

    // Create new row from struct.
    // Return pointer to new element, or NULL if insertion failed (due to out-of-memory, duplicate key, etc)
    ssim2mysql::FCtype*  ctype_InsertMaybe(const dmmeta::Ctype &value) __attribute__((nothrow));

#### AllocMem

    // Allocate space for one element. If no memory available, return NULL.
    void*                ctype_AllocMem() __attribute__((__warn_unused_result__, nothrow));

#### EmptyQ

    // Return true if index is empty
    bool                 ctype_EmptyQ() __attribute__((nothrow));

#### Find

    // Look up row by row id. Return NULL if out of range
    ssim2mysql::FCtype*  ctype_Find(u64 t) __attribute__((__warn_unused_result__, nothrow));

#### Last

    // Return pointer to last element of array, or NULL if array is empty
    ssim2mysql::FCtype*  ctype_Last() __attribute__((nothrow, pure));

#### N

    // Return number of items in the pool
    i32                  ctype_N() __attribute__((__warn_unused_result__, nothrow, pure));

#### RemoveLast

    // Delete last element of array. Do nothing if array is empty.
    void                 ctype_RemoveLast() __attribute__((nothrow));

#### qFind

    // 'quick' Access row by row id. No bounds checking.
    ssim2mysql::FCtype&  ctype_qFind(u64 t) __attribute__((nothrow));

#### XrefMaybe

    // Insert row into all appropriate indices. If error occurs, store error
    // in algo_lib::_db.errtext and return false. Caller must Delete or Unref such row.
    bool                 ctype_XrefMaybe(ssim2mysql::FCtype &row);

### Lpool: Level pool

Lpool is 32 Tpools, one for each allocation size. When allocating memory, the request
is bumped up to the nearest power of 2 and from there Tpool logic is followed.

Here is an example of an Lpool declaration:

    dmmeta.field  field:algo_lib.FDb.lpool  arg:u8  reftype:Lpool  dflt:""  comment:"private memory pool"
      dmmeta.basepool  field:algo_lib.FDb.lpool  base:algo_lib.FDb.sbrk

And here are the generated functions. As always, the actual code
of these functions can be queried with `amc algo_lib.FDb.lpool.%`

#### FreeMem

    // Free block of memory previously returned by Lpool.
    void                 lpool_FreeMem(void *mem, u64 size) __attribute__((nothrow));

#### AllocMem

    // Allocate new piece of memory at least SIZE bytes long.
    // If not successful, return NULL
    // The allocated block is 16-byte aligned
    void*                lpool_AllocMem(u64 size) __attribute__((__warn_unused_result__, nothrow));

#### ReserveBuffers

    // Add N buffers of some size to the free store
    bool                 lpool_ReserveBuffers(int nbuf, u64 bufsize) __attribute__((nothrow));

#### ReallocMem

    // Allocate new block, copy old to new, delete old.
    // New memory is always allocated (i.e. size reduction is not a no-op)
    // If no memory, return NULL: old memory untouched
    void*                lpool_ReallocMem(void *oldmem, u64 old_size, u64 new_size) __attribute__((nothrow));

### Malloc

Pass-through to libc's malloc / free.

### Sbrk

*Tbd*

### Tary: Flat indirect array (vector)

Tary is a dynamically allocated resizable array of values. A single block of memory
is used for all elements.
Taking the address of a Tary element is not allowed, although it cannot be prevented
Records allocated with Tary cannot be cross-referenced, this is enforced by `amc`.
`algo.ByteAry` is defined as Tary of u8. `algo.cstring` is defined as Tary of char.
When growing a full Tary (such as from Reserve or Alloc functions),
the size is always at least doubled.

Here is an example of a Tary field:

    dmmeta.field  field:algo.LineBuf.buf  arg:char  reftype:Tary  dflt:""  comment:""
      dmmeta.tary  field:algo.LineBuf.buf  aliased:Y  comment:""

#### Aliasing

The `aliased` attribute of the `tary` record specifies whether functions involving
`aryptr` will be generated.
When they are, it is possible that the `aryptr` being passed to `Addary` or `Setary` is
a subrange of the array itself. A check is inserted for this condition, and it's a fatal
program error if the check fails. Even though `amc` could adjust the incoming pointer
before and after calling `Reserve`, the caller still has a bad `aryptr` on their hands,
which means there is a program error.

The following functions are generated:

#### Addary

    // Reserve space (this may move memory). Insert N element at the end.
    // Return aryptr to newly inserted block.
    // If the RHS argument aliases the array (refers to the same memory), exit program with fatal error.
    algo::aryptr<char>   buf_Addary(algo::LineBuf& parent, algo::aryptr<char> rhs) __attribute__((nothrow));

#### Alloc

    // Reserve space. Insert element at the end
    // The new element is initialized to a default value
    char&                buf_Alloc(algo::LineBuf& parent) __attribute__((__warn_unused_result__, nothrow));

#### AllocAt

    // Reserve space for new element, reallocating the array if necessary
    // Insert new element at specified index. Index must be in range or a fatal error occurs.
    char&                buf_AllocAt(algo::LineBuf& parent, int at) __attribute__((__warn_unused_result__, nothrow));

#### AllocN

    // Reserve space. Insert N elements at the end of the array, return pointer to array
    algo::aryptr<char>   buf_AllocN(algo::LineBuf& parent, int n_elems) __attribute__((__warn_unused_result__, nothrow));

#### EmptyQ

    // Return true if index is empty
    bool                 buf_EmptyQ(algo::LineBuf& parent) __attribute__((nothrow));

#### Find

    // Look up row by row id. Return NULL if out of range
    char*                buf_Find(algo::LineBuf& parent, u64 t) __attribute__((__warn_unused_result__, nothrow));

#### Getary

    // Return array pointer by value
    algo::aryptr<char>   buf_Getary(algo::LineBuf& parent) __attribute__((nothrow));

#### Last

    // Return pointer to last element of array, or NULL if array is empty
    char*                buf_Last(algo::LineBuf& parent) __attribute__((nothrow, pure));

#### Max

    // Return max. number of items in the array
    i32                  buf_Max(algo::LineBuf& parent) __attribute__((nothrow));

#### N

    // Return number of items in the array
    i32                  buf_N(const algo::LineBuf& parent) __attribute__((__warn_unused_result__, nothrow, pure));

#### Remove

    // Remove item by index. If index outside of range, do nothing.
    void                 buf_Remove(algo::LineBuf& parent, u32 i) __attribute__((nothrow));

#### RemoveAll

    void                 buf_RemoveAll(algo::LineBuf& parent) __attribute__((nothrow));

#### RemoveLast

    // Delete last element of array. Do nothing if array is empty.
    void                 buf_RemoveLast(algo::LineBuf& parent) __attribute__((nothrow));

#### Reserve

    // Make sure N *more* elements will fit in array. Process dies if out of memory
    void                 buf_Reserve(algo::LineBuf& parent, int n) __attribute__((nothrow));

#### AbsReserve

    // Make sure N elements fit in array. Process dies if out of memory
    void                 buf_AbsReserve(algo::LineBuf& parent, int n) __attribute__((nothrow));

#### Setary (copy one Tary to another)

    // Copy contents of RHS to PARENT.
    void                 buf_Setary(algo::LineBuf& parent, algo::LineBuf &rhs) __attribute__((nothrow));

#### Setary

    // Copy specified array into buf, discarding previous contents.
    // If the RHS argument aliases the array (refers to the same memory), throw exception.
    void                 buf_Setary(algo::LineBuf& parent, const algo::aryptr<char> &rhs) __attribute__((nothrow));

#### qFind

    // 'quick' Access row by row id. No bounds checking.
    char&                buf_qFind(algo::LineBuf& parent, u64 t) __attribute__((nothrow));

#### qLast

    // Return reference to last element of array. No bounds checking
    char&                buf_qLast(algo::LineBuf& parent) __attribute__((nothrow));

### Tpool

This pool type only supports fixed-size allocation. Free elements area stored in a singly
linked list. if the list is empty, tpool uses the base allocator
(or the namespace default allocator)
to fulfill the request. The free list can be refilled with ReserveMem.
The memory obtained by Tpool from the base allocator is never returned.
Tpools can only be global (otherwise, memory leaks would occur).
This is the fastest allocator, because it only takes a couple of instructions to peel a free element
off of the free list.

Here is an example of a `Tpool`:

    dmmeta.field  field:ssim2mysql.FDb.cmd     arg:ssim2mysql.FCmd     reftype:Tpool   dflt:""  comment:""

And here is the generated code:

#### Alloc

    // Allocate memory for new default row.
    // If out of memory, process is killed.
    ssim2mysql::FCmd&    cmd_Alloc() __attribute__((__warn_unused_result__, nothrow));

#### AllocMaybe

    // Allocate memory for new element. If out of memory, return NULL.
    ssim2mysql::FCmd*    cmd_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));

#### Delete

    // Remove row from all global and cross indices, then deallocate row
    void                 cmd_Delete(ssim2mysql::FCmd &row) __attribute__((nothrow));

#### AllocMem

    // Allocate space for one element
    // If no memory available, return NULL.
    void*                cmd_AllocMem() __attribute__((__warn_unused_result__, nothrow));

#### FreeMem

    // Remove mem from all global and cross indices, then deallocate mem
    void                 cmd_FreeMem(ssim2mysql::FCmd &row) __attribute__((nothrow));

#### Reserve

    // Preallocate memory for N more elements
    // Return number of elements actually reserved.
    u64                  cmd_Reserve(u64 n_elems) __attribute__((nothrow));

#### ReserveMem

    // Allocate block of given size, break up into small elements and append to free list.
    // Return number of elements reserved.
    u64                  cmd_ReserveMem(u64 size) __attribute__((nothrow));

#### XrefMaybe

    // Insert row into all appropriate indices. If error occurs, store error
    // in algo_lib::_db.errtext and return false. Call Unref or Delete to cleanup partially inserted row.
    bool                 cmd_XrefMaybe(ssim2mysql::FCmd &row);

### Val: Value

Val is the simplest reftype. A field of type val becomes a regular struct member
on output.
Here is an example of a `Val` field:

    dmmeta.ctype  ctype:algo.UnTime  comment:"unix time * 1e9 + nanoseconds"
      dmmeta.field  field:algo.UnTime.value  arg:i64  reftype:Val  dflt:""  comment:""

And here is what `amc` produces on output:

    // --- algo.UnTime
    struct UnTime { // algo.UnTime: unix time * 1e9 + nanoseconds
        i64   value;   //   0
        UnTime();
    };
## Reflection

`amc` bakes some information about a process into the process itself.
For each each namespace linked into a process, possessing an in-memory database, an `algo_lib.FImdb`
entry is defined, and added to the `algo_lib.FDb.imdb` table. This table is indexed
with `algo_lib.FDb.ind_imdb`.

Without loading any ssimfiles, a process can access the `imdb` records.
Each record has the following fields:

* `imdb`: primary key. (e.g. `algo_lib` or `sample`)
* `InsertStrptrMaybe`: pointer to a function that takes a strptr and inserts it into this namespace's in-memory database.
* `Step`: pointer to a function that executes a single scheduler cycle.
* `MainLoop`: pointer to the main loop function, if defined
* `GetTrace`: pointer to a function that reads all trace variables from the process

In addition, the `algo_lib.FDb.imtable` table contains the list of all in-memory tables.
`Imtable` is also indexed, with `ind_imtable`, and has the followin fields (`amc -e algo.Imtable`):

* `imtable`: primary key
* `elem_type`: A string holding the ctype pkey of the records in this table
* `c_RowidFind`: Optional pointer to a function to find an element by rowid
* `XrefX`: Optional function to x-reference the record
* `NItems`: Optional function that returns the number of records in the table
* `Print`: Optional function to print one element as a string
* `size`: With of each record
* `ssimfile`: Pkey of ssimfile, if one is associated with the record.

Reflection is generally considered a very powerful mechanism, but in the OpenACR world
it's not used that frequently. While the ability to insert records dynamically (i.e. outside
of a pre-declared code-path) and knowing the list of namespace linked into a given program is useful,
it's not as useful as simply loading the ssimtables that describe *all* processes in the
given universe, and doing something with them. An analogy would be surgery on yourself,
especially brain surgery. Powerful? Yes. Best practice? Hardly.

## amc_vis: Visualize Ctype Dependencies and Access Paths

The single parameter to `amc_vis` is a ctype regex,
and whatever ctypes are matched by the regex will be shown
via ASCII art.

Here is an example:

    $ amc_vis amc.FCtype\|amc.FField


                  / amc.FCtype
                  |
                  |                   / amc.FField
                  |Ptrary c_field---->|
                  |Ptr c_pkeyfield--->|
                  |Ptrary c_datafld-->|
                  |Llist zd_inst----->|
                  |<------------------|Upptr p_arg
                  |Ptr c_optfld------>|
                  |<------------------|Upptr p_ctype
                  |Llist zd_access--->|
                  |Ptr c_varlenfld--->|
                  -                   |
                                      |
                                      -

amc_vis can also output an dot file, which can then be viewed in a browser:

    $ amc_vis -xref:N amc.FCtype\|amc.FField -dot xyz.dot
        amc_vis.dot  out_dot:xyz.dot  out_svg:xyz.svg

    $ firefox xyz.svg

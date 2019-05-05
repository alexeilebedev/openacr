## amc: Algo Model Compiler

### Introduction

Amc is an extensible generator of source code from ssimfiles.

Amc reads ssim tables, creates C++ in-memory database code as determined by these tables, 
and outputs .h and .cpp files.
By default, ssim tables are loaded from directory "data", and output
is written to directories `cpp/gen` and `include/gen`.

Amc generates hash tables, arrays, linked lists, dequeues, binary heaps; trees;
hierarchical, region-based memory allocators, including
single and powers-of-two freelists, fifo, linear array (vector), 
indirect array (with permanent pointers); inline arrays and fixed-size arrays;
Functions to convert any struct to/from a string or a bash command line; Enum support,
both for integer and string values; presence masks; big-endian fields;
sort functions on custom fields; incremental group-by indexes; tracking of pointers
with automatic cascade delete; protection against linear scanning when deleting
elements; scheduling constructs (for real-time modules); cycle accounting ('traces').
C++ symbols from ssimfile columns; Statically loaded tables; Subprocess invocation.
Asynchronous I/O. Bitsets on top of any array type. Char sets; 
Fixed string types (Pascal strings, Left-padded strings, Right-padded
strings, Fixed-length strings with numeric conversion); Scaled decimal types; 
Dispatches (any group of ctypes), sharing a common header with a type field, or not.
Printing, reading, calling dispatches given both binary and text input.
Uniform cursor (iterator) interfaces for bheap, hash, tree, array,
lines, files in directory, and more. I'm sure I'm forgetting something.

For each program, these things are generated in-place and
from scratch, and can be iteratively customized.
The resulting code forms a *database of source code*,
containing a superset of functions that will be used by the final
application. The generated code is verbose, user-readable, properly commented,
is intended to be readable by a human, corresponds directly to the final assembly
code, and uses only a small, conservative subset of C++. 
`Amc` does not modify or reverse-engineer user code, so it's not a framework
where you have to "plug in" anything. It is a tool for constructing software
based on your specifications.

`Amc` loads about 100 ssim tables. The full list can be obtained with
`acr_in amc`. The exact actual `amc` input can be printed with `acr_in amc -data`.
About 20% of these tables are responsible for 80% of the generated code, the rest deal with finer details.

`Amc` was initially coded by hand, but once its capabilities became powerful enough, it was used to
generate data structures for its next version. As a result, all of `Amc`'s internal data structures,
both input, and computational, are defined as ssim tuples and can be queried with `acr ns:amc -xref`.

### Why Generate?

Good algorithms and data structures for most problems are known. 
The problem is attaching them to an application. Usually the costs associated 
with using algorithms are:

* Performance cost and complexity cost when using libraries.
* Complexity cost due to symbol renamings (what happens with the C++ template sublanguage or with macro preprocessors). 
* Debugging and reliability cost when hand-coding algorithms.
* Maintenance cost due to having too many lines of code (technical debt).
* Unexpected changes in upstream generic libraries.

#### Libraries Vs. Custom Code

The motivation for writing generators is that writing code for reusability doesn't work.
The reason it doesn't work is that the definition of correctness doesn't lie with
the piece of reusable code -- it lies with the final application. And so the reusable 
code always offers to the application too much and at the same time not enough.
You need a singly linked list, but the library uses a doubly linked list. You need an extra index,
but the library author didn't anticipate it. You have your own strategy for memory
management, but the library insists on its own. And you can't customize the library, since
for every feature, there is already some user out there who needs it to stay the same. 
And when you update to the next version of the library, you get tons of features you didn't ask for.
Code written for reusability rarely reaches its intended potential in terms
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

`Amc` processes generates about 1M LOC per second.
Of course, this performance is not reflected in 
the final executable, which means that adding new checks to amc is effectively free.

It is important that amc outputs are versioned in git, so we can trace the origin
of any change (with git annotate), and continue to make changes to amc
without breaking existing code.

### Query mode

When amc is given an argument, it runs in query mode.
Instead of modifying source files it simply prints to stdout all internal code
sections whose key matches the specified regex (typically it's a ctype name or a function name).

This is the fastest way to check how a certain function is implemented.

    amc amc.%

This would generate all functions which are used to compile amc itself. The apparent circularity
exists because at some point, functions implementing amc were written by hand, 
and then amc was modified to generate them and save them to `cpp/gen/amc_gen.cpp`.
Please see [The Algorithm For Generating Any Code](#the-algorithm-for-generating-any-code)

To limit amc's output to prototypes only, specify `-proto`:

    amc amc.% -proto
    
### Ratio of Generated To Manual LOC

It generates roughly 25 lines of code per 1 line of ssimfile specification. In fact
I will check that claim right now:

    $ acr ns:amc -t | wc -l
    2010
    $ amc amc.% | wc -l
    47431

The specification can be edited manually and adjusted frequently with Unix tools such as
sed and perl, or by issuing `acr_ed` commands. This makes the cost of
ssim specifications lower than the cost of regular code.

### Sandbox Mode

The sandbox mode is provided to help add features to amc without creating a dead-end (where
generated code is broken, but cannot be re-generated because amc needs to be rebuilt)

`abt -testgen` compiles a debug version of amc, creates the sandbox directory .testgen, runs amc in the .testgen
directory, shows a diff between current and sandboxed changes, builds all unit tests inside the sandbox
and runs the tests. The command succeeds if all of the steps succeed.

### Reading Code of Amc Itself

Amc source code is located under cpp/amc. The list of all the source files and headers
can also be examined with `acr targsrc:amc/%`

amc inpt tables are in `data/amcdb` and `data/dmmeta`; The full list can be obtained 
with `acr_in amc`. 

### What is a Cross-reference?

The [Relational Model](https://en.wikipedia.org/wiki/Relational_model) is a universal
way to represent knowledge, first described by Edgar Codd.
It is related to [Zermelo–Fraenkel set theory](https://en.wikipedia.org/wiki/Zermelo%E2%80%93Fraenkel_set_theory).

It is a good foundation. In a relational data model, individual records are represented by tuples
(what we call a ctype). Each attribute (what we call a field) is either a basic type, or
a reference to some other tuple.

The reference can take several forms -- it could be a direct pointer (Upptr),
or a primary key (Pkey). In either case, the whole
reason we write compiled language programs instead of solving all problems using SQL and MariaDB,
is that reversing the direction of reference lookup -- 
*finding all records that point to a given record* -- is expensive. 
Cheaper cross-references is really the reason why most programs exist in the first place.
In database terms, a cross-reference is a group-by; in amc, cross-reference is
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

* `bh` -> Bheap
* `c` -> Ptr or Ptrary
* `cd, cdl, cs, csl, zd, zdl, zs, zsl` -> Llist
* `cnt` -> Count
* `ind` -> Thash
* `p` -> Upptr
* `tr` -> Atree

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

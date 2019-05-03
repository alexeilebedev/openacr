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

The motivation for writing generators is that writing code for reusability doesn't work.
The reason it doesn't work is that the definition of correctness doesn't lie with
the piece of reusable code -- it lies with the final application. And so the reusable 
code always offers to the application too much and at the same time not enough.
You need a singly linked list, but the library uses a doubly linked list. You don't need
a count of elements, but the library maintains that. You have your own strategy for memory
management, but the library insists on its own. And you can't customize the library, since
somebody else is using it, and they might need the feature. And when you update to the next
version of the library, you get tons of features you didn't ask for.
Code written for reusability rarely reaches its intended potential in terms
of either performance or utility.

Leaving aside reusability for a moment, as can be seen from real life examples,
all high-performance systems are hand-coded due to highly
specific requirements, and because it allows writing only what's needed, debugging that,
and leaving it to run and do its job indefinitly. But hand-coding is difficult and 
requires a lot of debugging and chasing of memory errors and dangling pointers. 
Good algorithms and data structures for most problems are known.
The problem is attaching them to the application without losing the performance to 
glue code, data format translations (such as what happens with libraries), or layers of obfuscation
and symbol renamings (what happens with the C++ template sublanguage or with macro preprocessors). 

Software complexity models such as [COCOMO](https://en.wikipedia.org/wiki/COCOMO)
estimate the defect rate to grow as an
exponential function of number of source lines. This exponent should not be underestimated.
It is about 1.2 for a national exchange with real-time and embedded constraints, meaning that 
to write 1,000 lines of code, it costs you 3,981 units of effort.
And if you write 100,000 lines of code, you pay a full 1,000,000 units. This exponential
nature of the cost of lines of code is closely related to the cost of borrowing money.
You basically have to pay back with interest, making every line of code 
 in a very real sense "technical debt". 
 
Massive code bases can slow development to a crawl and destroy projects.
I don't want to present a macabre listing of all the software projects that failed 
because they had too much code; I will just assume that we all agree on the validity
of the following equation:

    (Project Goodness) = (Features)*W - A*(Amount of Code Written)^B

In other words, we like the features of our system, with some weight `W`,
and we pay with interest `B` and weight `A` for the amount
of code we wrote, to get these features. 

So we have a mini-max problem, which is the first sign of a problem worth solving. 
On one hand, we want maximally customizable code that does only what we want.
On the other hand, we want to write and debug the minimal amount of code.

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
way to represent knowledge, first described by Edgar Codd (he is our Jesus).
It is related to [Zermelo–Fraenkel set theory](https://en.wikipedia.org/wiki/Zermelo%E2%80%93Fraenkel_set_theory)

It is a good foundation. In a relational data model, individual records are represented by tuples (ctype).
Each attribute (field) is either a raw type, or a reference to some other record.

The reference can take several forms -- it could be a direct pointer (Upptr),
or a primary key (Pkey). In either case, the whole
reason we write compiled language programs instead of solving all problems using SQL and MariaDB,
is that reversing the direction of reference lookup -- 
*finding all records that point to a given record* -- is expensive. 
Cheap cross-references are ultimately why we write programs in the first place.
In database terms, a cross-reference is a group-by; in amc, cross-reference is
maintained as an incremental join -- automatically updated as records are added or removed from tables.

A cross-reference is established by use of an xref record. Here is an example

    dmmeta.ctype  ctype:amc.FTarget  comment:"
      dmmeta.field  field:amc.FTarget.msghdr     arg:dev.Target    reftype:Base    dflt:"  comment:"
      dmmeta.field  field:amc.FTarget.c_targdep  arg:amc.FTargdep  reftype:Ptrary  dflt:"  comment:"
        dmmeta.ptrary  field:amc.FTarget.c_targdep  unique:Y
        dmmeta.xref  field:amc.FTarget.c_targdep  inscond:true  via:amc.FDb.ind_target/dev.Targdep.target

This says: Dear amc, whenever a `targdep` record is inserted in my program, find an instance
of `target` by using global index `ind_target` with key `dev.Targdep.target`  (a fldfunc of `targdep`),
as the key, and insert `targdep` into an array of pointers rooted in `target`. 
Whenever a `targdep` record is deleted, automatically remove it from the list.

The main xref types supported by amc are Ptr, Ptrary, Llist, Thash, Bheap, Atree and Count.

Xrefs can be easily added and removed either by hand (with `acr -e` or by editing ssimfiles 
directly), or using `acr_ed`. In the example above, `acr_ed -create -field amc.FTarget.c_targdep -write`
would be enough to establish the x-ref.

There can be any number of xrefs between any two tables. So, if you want a record to be
a member of ten linked lists and eleven heaps -- you're free to do so. 
Thus xrefs are exactly analogous to RDBMS indexes; except xrefs can be between any two
tables, so xrefs are also `partitioned indexes` and incremental `group by`s at the same time.

I would re-state here the fact that these x-refs, or indexes, are secondary to the data.
It doesn't make sense to say "hash of X" or "tree of Y". Sets of X and Y are primary, and exist even 
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

### Chaining
Pools can be chained by specifying a basepool record. Basepool is where a pool gets more memory
when its own supplies are exhausted. The algo_lib.FDb.sbrk pool is usually at the root of each chain.

### Default Namespace Pool
Each namespace has a default pool, declared via nsx record. This is where all memory for the
 namespace ultimately comes from.

### Tracing
Ftrace record can be used to enable counting of alloc/delete calls for each pool.

### Base: Mixin

The Base reftype copies all fields from one arg to field's parent ctype.
There are two main use cases.

1. Protocol messages, where Base is used to declare a common message header for all message types.
Amc generates functions to go back and forth between header and specific type using Castdown / Castbase functions.

2. In-memory tables based on ssimfiles, which inherit all of the fields defined
in the ssimfile. These do not allow casting back and forth since memory layouts differ, there
are extra pointers in the in-memory version, some attributes are being used for cross-references (joins), etc.

### Bheap: Binary Heap

The binary heap is implemented as an flat array of pointers (e.g. a Ptrary).

### Bitset: Bitset over an array

Bitsets can be created on top of any integer field (e.g. u8 to u128) or array field (Inlary, Tary).
Amc generates functions to provide indexed access to bits of the underlying field.

### Blkpool: Mostly fifo memory allocator

The block pool is a free-list of large-ish blocks. Memory is allocated from the current block,
with others serving as reserve. Allocated elements contain a back-pointer to the beginning
of the block, and increment a refcount on the block. Elements are allocated back-to-back
starting at the beginning of the block until the block is exhausted.
When freeing memory, the refcount is decremented but memory cannot be reused (yet). When
the block's refcount goes to zero, the entire block goes back to the free list. This allocator
is suitable for messages that are being processed using a fifo strategy. It is not good
for random access patterns because one unfreed message can hold an entire block in memory,
eventually exhausting memory.

### Count: Count elements

Count is a xref type that simply keeps track of the number of child
elements referring to a given parent. The elements themselves are not accessible
via this field.

### Dec: Scaled decimals
This reftype is not specified explicitly. It is applied when fdec record appears.

### Fconst: Enumerated type
This reftype is not specified explicitly. It is applied when fconst record appears.
Each fconst record names a symbol and a C++ expression (value). The symbol is the string representation
of the vaule. Amc creates an enum type with values, and creates to-string and from-string functions
that translate between values and symbols.

A related record is Gconst, which can be used in place of fconst to create an enum type out of an entire table.

When printing, if there is no symbol associated with the numeric value of the field being printed, the
value is printed as a number. When reading, if input symbol doesn't map to any known value, it is parsed
as an integer instead.

### Inlary: Inline array

Inlary uses memory reserved directly inside the parent struct. A dmmeta.inlary record
is required, specifying min,max elements. If min=max, it is considered a fixed array.
Fixed array has neither Alloc nor Delete functions, and there is no count of elements.
If min < max, then the first min elements are created when the parent is constructed, and the rest
can be dynamically allocated. The Inlary supports random access because it is an array.

One particularly cool use of Inlary is with gstatic. Whenever gstatic is specified, the contents of an ssim
table are ``statically'' included into the generated source file. When gstatic is combined with Inlary,
amc creates a C++ compiler symbol with a name derived from the primary key of the source table.
The symbol is a reference whose value is a compile-time constant since the offset to the variable
is known. The symbol can be used at runtime to access the record, which is guaranteed to exist
and be properly cross-referenced.

### Lary: Level array

Lary is implemented as 32 pointers in the parent struct. Level k holds as pointer
to a block of elements of length 2^k. Indexed lookup involves just 1 memory access,
because amc uses BitScanReverse to find which level the element lives on. When a level is exhausted,
another level 2x the size, is allocated. Since none of the previous levels need to be
reallocated, the pointers returned by Lary are stable and so elements can be freely cross-referenced.
Lary is the most common pool.

### Llist: Linked list

Amc generates 32 flavors of linked lists, almost as many as Heinz for ketchup.
Linked lists can be:

Singly or doubly linked (as indicated by letter s or d in the prefix)
Zero-terminated or circular (as indicated by letter z or c in the prefix)
Fifo or lifo (as indicated by presence of letter l in the prefix, l for lifo)
With or without count
With or without tail pointer

Circular linked lists are often used with steps, because it is convenient to call RotateFirst
to both grab an element off the head of the list, and move this element to the back.

### Lpool: Level pool

Lpool is 32 Tpools, one for each allocation size. When allocating memory, the request
is bumped up to the nearest power of 2 and from there Tpool logic is followed.

### Malloc

Pass-through to libc's malloc / free.

### Opt: Optional last field in variable-length struct

Opt corresponds to 0 or 1 values at the end of a struct. This type is frequently used by protocols
to specify optional payload. Amc provides functions that can validate and access the payload.

### Pmask: Presence mask

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

Ptr is a cross-reference type used when there can be 0 or 1 child records for each parent record.

### Ptrary

Tary of Ptrs.

### RegxSql:

When using this field type, amc ignores the field arg and inserts an algo_lib.Regx into
the parent structure. The expression intended to match the primary key of the target type.
This reftype is very useful in command line arguments.

### Sbrk

This is just a pass-through to the sbrk() system call. The pool does not support deletion.
Ultimately all system memory requests are satisfied by this pool, because it sits at the
end of every basepool chain.

### Smallstr

Smallstr is a fixed-length character field. Memory is reserved inline in the parent struct.
Strings can be length-suffixed (Rpascal), left-padded or right-padded.
For padded strings, the string value is calculated by discarding the pad characters from the edge.
Any smallstr is castable to strptr.

### Tary: Flat array of records

Tary is a dynamically allocated resizable array of values. A single block of memory
is used for all elements, so taking the address of a Tary element is not allowed. Records
allocated with Tary cannot be cross-referenced. ByteAry and cstring use Tary as the underlying type.
When growing a full Tary (such as from Reserve or Alloc functions), the size is always at least doubled.

### Thash: hash table

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

### Tpool: singly linked free-list

This pool type only supports fixed-size allocation. free elements are stored in a singly
linked list. if the list is empty, tpool uses the base allocator (or the namespace default allocator)
to fulfill the request. the free list can be refilled with reservemem. This is the
fastest allocator, because it only takes a couple of instructions to peel a free element
off of the free list.

### Upptr:

Upptr is a pointer to a pre-existing record, usually non-null.
Upptr is a reference. By contrast, Ptr is a cross-reference.
I.e. a Ptr becomes non-null when another record starts pointing at the parent of the Ptr.
Upptr becomes non-null when a lookup in an index is performed.
amc_vis will complain if there are circular dependencies implied by Upptr (i.e. A has Upptr to B,
B has Upptr to A). Circular dependencies between Ptr fields are OK.

### Val: Value

Val is the simplest field type, just a struct member.
It is an inline field that is initialized whenever the parent
is initialized, and destroyed when the parent is destroyed.

### Varlen: variable-length tail portion of a struct

Varlen corresponds to a (possibly empty) array of fixed-size records appended to the end of a struct.
the parent ctype must have a length field defined. varlen fields cane be read from a string or
iterated over. This type is frequently used by wire protocols to specify a repeated section of a message.

### String types and how to use them:

algo::strptr: Length-delimited string pointer.
There are two fields in this struct: char *elems, int n\_elems;
The string is just n chars starting at elems;
No null-termination is assumed or allowed.
It's safe to use strptr in function prototypes when the argument isn't
  modified by the function -- all other string types can be converted
  to such temporary strptr and passed to a function.
amc's hash find uses strptr as argument type whenever the key type is some
kind of string.

algo::cstring: Tary of char. Contents can be cast to strptr.
Fields are: char *ch, int ch\_n, int ch\_max.
operator = is defined for cstring, so these can be assigned like values

algo::tempstr: cstring, to be used only as the return type of a function,
in order to avoid copying data. Has the property that when assigned to
cstring, the contents are moved instead ("move constructor semantics").

DO NOT assign tempstr, cstring, or a temporary variable to a strptr.
DO NOT pass cstring& to a function when strptr is sufficient.
DO NOT return cstring from functions, it will result in extra alloc/copy/delete

Amc-generated types:
A lot of these are are already defined, but new ones can be built as  needed.
use "acr smallstr" for the full list. Brief explanation below.

algo::SmallstrN: pascal string with N characters. Both data and length
are part of the struct. Don't send this over the wire, because unused
portions of the string may contain garbage from the stack.

algo::RspaceStrN: Fixed-length string field padded on the right with spaces.
Can be sent over the wire (included in protocol types)
algo::RnullStrN
algo::LspaceStrN: similar to the above

algo::LnumStrN\_U{32,64}: Fixed-length string field, padded on the left with '0' (numeric 0)
Includes functions to convert to/from a u32/u64. Number cannot be negative,
because left-padding with 0 prevents that.

LnumStr13\_U64\_Base36: Same as above, but different base.

### The Algorithm For Generating Any Code

The main algorithm for generating any code (not just with amc) is simple:

1. Manually write the code that you want to generate and make sure it works.
2. Put a print statement around each line.
3. Move the resulting code to the appropriate place in your generator.
4. Run your generator. Now you have 2 copies of your code: one you started with, and the 
  other that was printed by the generator. If you did everygthing right, you should get a
  link error now.
5. Delete the manually written code.
6. Parameterize the generator so that it can generate a whole family of implementations that
  share look like the code.

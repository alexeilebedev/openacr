## Table of Contents
<!--ts-->
   * [Table of Contents](#table-of-contents)
   * [About](#about)
   * [Setup and Installation](#setup-and-installation)
         * [Tests](#tests)
   * [Directory Structure](#directory-structure)
         * [Binaries](#binaries)
         * [Editor configuration files](#editor-configuration-files)
   * [Command Line Options](#command-line-options)
   * [Bash Command Completion](#bash-command-completion)
         * [Example: Create New Ssimfile And Test Command Completion](#example-create-new-ssimfile-and-test-command-completion)
   * [Tutorial: Hello Meta World](#tutorial-hello-meta-world)
   * [Ssim files](#ssim-files)
         * [Structured Key Normal Form](#structured-key-normal-form)
         * [History of SKNF](#history-of-sknf)
   * [acr - Auto Cross Reference](#acr---auto-cross-reference)
         * [Example: Print all records in Ssim database](#example-print-all-records-in-ssim-database)
         * [Example: Format ssim table](#example-format-ssim-table)
         * [Example: Delete unnecessary entries from a dependency table](#example-delete-unnecessary-entries-from-a-dependency-table)
         * [Example: Round-trip custom type through Mariadb without loss](#example-round-trip-custom-type-through-mariadb-without-loss)
         * [Example: Recursively rename a key](#example-recursively-rename-a-key)
         * [Example: Move field from one ctype to another](#example-move-field-from-one-ctype-to-another)
         * [Example: Recursively delete a key](#example-recursively-delete-a-key)
         * [Example: Use SQL statements to refactor code](#example-use-sql-statements-to-refactor-code)
         * [Example: Specify bool options on command line](#example-specify-bool-options-on-command-line)
         * [Field Funcs](#field-funcs)
   * [acr_in: Show input tuples for target](#acr_in-show-input-tuples-for-target)
   * [abt - A Build Tool](#abt---a-build-tool)
   * [amc - Algo Model Compiler](#amc---algo-model-compiler)
         * [Running Amc](#running-amc)
         * [Query mode](#query-mode)
         * [References](#references)
         * [Sandbox mode](#sandbox-mode)
         * [Reading Code of Amc Itself](#reading-code-of-amc-itself)
         * [Example Show generated C   struct](#example-show-generated-c-struct)
         * [Example: Show prototypes of all generated functions matching pattern](#example-show-prototypes-of-all-generated-functions-matching-pattern)
         * [Example: Count how many linked lists there are in the code database](#example-count-how-many-linked-lists-there-are-in-the-code-database)
         * [Example: Find all references to a record](#example-find-all-references-to-a-record)
         * [Example: Edit 2 Ctypes](#example-edit-2-ctypes)
         * [Main Input Tables](#main-input-tables)
         * [Steps](#steps)
         * [Cross-references](#cross-references)
         * [Chaining](#chaining)
         * [Default Namespace Pool](#default-namespace-pool)
         * [Tracing](#tracing)
   * [Bootstrapping Magic](#bootstrapping-magic)
   * [Reftypes (field types)](#reftypes-field-types)
         * [Base: Mixin](#base-mixin)
         * [Bheap: Binary Heap](#bheap-binary-heap)
         * [Bitset: Bitset over an array](#bitset-bitset-over-an-array)
         * [Blkpool: Mostly fifo memory allocator](#blkpool-mostly-fifo-memory-allocator)
         * [Count: Count elements](#count-count-elements)
         * [Dec: Scaled decimals](#dec-scaled-decimals)
         * [Fconst: Enumerated type](#fconst-enumerated-type)
         * [Inlary: Inline array](#inlary-inline-array)
         * [Lary: Level array](#lary-level-array)
         * [Llist: Linked list](#llist-linked-list)
         * [Lpool: Level pool](#lpool-level-pool)
         * [Malloc](#malloc)
         * [Opt: Optional last field in variable-length struct](#opt-optional-last-field-in-variable-length-struct)
         * [Pmask: Presence mask](#pmask-presence-mask)
         * [Ptr](#ptr)
         * [Ptrary](#ptrary)
         * [RegxSql:](#regxsql)
         * [Sbrk](#sbrk)
         * [Smallstr](#smallstr)
         * [Tary: Flat array of records](#tary-flat-array-of-records)
         * [Thash: hash table](#thash-hash-table)
         * [Tpool: singly linked free-list](#tpool-singly-linked-free-list)
         * [Upptr:](#upptr)
         * [Val: Value](#val-value)
         * [Varlen: variable-length tail portion of a struct](#varlen-variable-length-tail-portion-of-a-struct)
      * [String types and how to use them:](#string-types-and-how-to-use-them)
   * [acr_ed](#acr_ed)
   * [Coding Style](#coding-style)
         * [Spaces, Indentation](#spaces-indentation)
         * [Variable Names](#variable-names)
         * [Predicate Functions](#predicate-functions)
         * [Curly Braces](#curly-braces)
         * [Split Conditionals](#split-conditionals)
         * [Curly Braces around Conditionals are Non-Optional](#curly-braces-around-conditionals-are-non-optional)
         * [Use of semi-colon forces a new line](#use-of-semi-colon-forces-a-new-line)
         * [Keep code separate from data](#keep-code-separate-from-data)
         * [Use query-command separation](#use-query-command-separation)
         * [Keep it single-threaded](#keep-it-single-threaded)
         * [Use Single Entry, Single Exit (SESE) style](#use-single-entry-single-exit-sese-style)
         * [Single File Static Assignment](#single-file-static-assignment)
         * [Document all non-static functions](#document-all-non-static-functions)
         * [All rules allow exceptions](#all-rules-allow-exceptions)
   * [amc_vis](#amc_vis)
   * [amc_gc: AMC garbage collector](#amc_gc-amc-garbage-collector)
   * [MariaDB integration](#mariadb-integration)
   * [Working with source files &amp; targets](#working-with-source-files--targets)
         * [Listing Files](#listing-files)
         * [Creating Source Files](#creating-source-files)
         * [Functions &amp; Headers](#functions--headers)
         * [Program Text](#program-text)
   * [atf_unit: Unit Tests](#atf_unit-unit-tests)
   * [Links](#links)


<!--te-->

## Reftypes (field types)

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

Count is a x-reference type that simply keeps track of the number of child
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

## String types and how to use them:

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

AMC-generated types:
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

## acr_ed

acr_ed (``acr editor'') generates sequences of commands
that implement common developer tasks such as
 create target,
 create ctype,
 create ssimfile,
 etc.

By default, acr_ed spits out an executable script to stdout. With
-write, the script is executed (this has the same effect as piping output
to sh)

The main commands in acr_ed are:

~~~
-ctype:X -create : Create new ctype. Suboptions: -ssimfile, -cbase
-ctype:X -rename:Y: Rename ctype X to Y
-ctype:X -create -subset:Y -pooltype:Z -indexed: Create a new indexed in-memory table

-ssimfile:X -create : Create new ssimfile. Suboptions: -subset, -subset2, -separator, -ctype
-ssimfile:X -rename:Y: Rename ssimfile (to fully rename an ssimfile, first rename the ssimfile, then the ctype)

-field:X -create: Create field or access path.
  Suboptions: -comment, -arg, -dflt, -reftype, -before, -via, -xref, -fstep, -substr, -anon, -bigend
  Some of these can be guessed by acr_ed. For instance, if program x has tables y and z, then using
    acr_ed -create -field x.FY.zd_z will automatically add a cross-reference ("group-by") of z by y,
    figuring out the projection of z on y and any necessary index through which to find a y record
    given a z.
-finput -create  -ssimfile:X -target:Y: Add ssimfile as finput for a target. Suboptions: -indexed

-target:X -create: Create new target. Issues necessary git commands as well.
-target:X -rename:Y: Rename target. Issues necessary git commands as well.
-target:X -del: Delete a target

-srcfile:X -create -target:Y: Create new source file
-srcfile:X -rename:Y: Rename source file (automatically updates headers)
-srcfile:X -del: Delete source file
~~~

Create new target:

       acr_ed -create -target xyz

Delete target:

       acr_ed -del -target xyz

Create new ssimfile.

	 acr_ed -ssimfile dmmeta.nsref -create

Create new ssimfile as a subset of 2 other ctypes, using separator for the key

	 acr_ed -create -ssimfile dmmeta.nsref -subset dmmeta.Ns -subset2 dmmeta.Reftype -separator /

Create new table for in-memory database (manual).
	 Parameter -reftype Thash is guessed based on field name
	 Parameter -hashfld is guessed based on primary key

	 acr_ed -create -ctype amc.FCtype -cbase dmmeta.Ctype
	 acr_ed -create -field amc.FDb.ctype -arg amc.FCtype  -reftype Lary
	 acr_ed -create -field amc.FDb.ind_ctype -arg amc.FCtype  -xref

Create new table for in-memory database, based on an ssimfile

	 acr_ed -create -finput -target amc -ssimfile dmmeta.ctype

Create new table for in-memory database, based on an ssimfile, add a hash index

	 acr_ed -create -finput -target amc -ssimfile dmmeta.ctype -indexed

Add a pointer from ssimfile -> ctype for target acr_ed
Parameters -reftype, -via are guessed.

Conditional cross-reference

	 acr_ed -create -field acr_ed.FSsimfile.p_ctype -arg acr_ed.FCtype -xref -inscond false

Create new source file:

       acr_ed -create -srcfile cpp/amc/blah.cpp -write

Create new header:

       acr_ed -create -srcfile include/amc/blah.h -target amc -write

## Coding Style

### Spaces, Indentation

Use spaces, use 4 space indent. We use cpp-indent to normalize all source files,
so this policy is enforced automatically.

### Variable Names

Variable names use lower_under style.

### Predicate Functions

Functions that return bool end in Q: VisibleQ, ExistsQ, etc. Q means Query or Question mark.

### Curly Braces

Curly braces start on the same line both for functions and for control flow operators.
'Else' starts on the same line as the previous closing curly brace. The reason is that
without this rule, the if-block that that ends at page boundary can be
incorrectly perceived as lacking the else clause, inadvertently changing its meaning.

    if (x) {
    }

    if (x) {
    } else if (y) {
    } else {
    }

    void Proc() {
    }

### Split Conditionals

If you need to split a long line, the connecting operator goes to the new line.

Example:

    if (xyz
        && abc) {
    }

This formatting rule can be derived from principle of incremental construction.

The expression if (xyz) becomes if (xyz && abc)
by adding && abc, so logically the operator groups with the later code.
The side benefit is that commenting out or deleting the additional code is easier.

### Curly Braces around Conditionals are Non-Optional

If statements and loops always use curly braces, even if the body is empty.
The reason is again due to incremental construction: adding an extra statement
to an if statement shouldn't entail insertion of three lines. The curly braces
should already be there.

Control structures must be formatted on multiple lines
This rule is important for debugging. A statement like if (x) a=b; requires
going to assembly level to determine if the conditional was taken, or to place a breakpoint
at the assignment.

### Use of semi-colon forces a new line

    x;
    y;

The reason here is again that debuggers are line-oriented.
All code is versioned in git, so it wouldn't be nice to create spurious
diffs just because someone needed to debug a piece of code.

### Keep code separate from data

Structure space with ssim files and amc.
Strucutre time with plain C++.

### Use query-command separation

Functions can be divided into two types: queries, which are read-only functions that compute
a value; and commands, which are procedures that perform some in-memory update.
Queries are a lot easier to write and debug, since there is no post-condition to prove.
Keep as much code as possible in query form, and keep it separate from command-type functions.
This will reduce debugging time.

### Keep it single-threaded

We can easily task thousands of cores with independent tasks, and they will all execute at once.
But let's not task a human with trying to understand two tasks simltaneously. Keep programs
single-threaded, and as deterministic (independent of wall clock) as possible,
and your debug times will be finite.

### Use Single Entry, Single Exit (SESE) style

Do not use the continue statement (break is OK, it is sometimes necessary to terminate a search).
Use return only at the end of a function. SESE has several sound reasons behind it.
One is the desire to be able to extend the program with minimal effort.
It is natural to replace a program that does X with a program that does X, then Y.
Only SESE style makes this possible by writing ``X; Y''. For this, X cannot have hidden exits.
SESE is also a normal form, i.e. a solution that two developers can agree on without communicating,
so it is scalable.
Finally, SESE dictates that code of the program is simply a text representation of its control
flow graph. All arcs in the graph are made visible, and this facilitates reasoning about program
correctness.

### Single File Static Assignment

Single Static Assignment (SSA) is when  there is only one location in the source tree
where each variable is assigned to. This is preferable but not required.

All assignments to a given variable must be in the same source file.
This is necessary for reasoning about correctness, debugging, and enforcing post-conditions
of given variable assignment.

### Document all non-static functions

Non-static functions are API functions. Each one needs to be documented, explaining
the reason for its existence, side effects, pre- and post-conditions,
and any other rules for caller/callee not enforceable at the language level.

### All rules allow exceptions

Given a sufficiently good reason, any rule can be replaced. But let's try to keep a consistent
whole by following these rules whenever possible.

## amc_vis

amc_vis is a tool for visualization access paths between tables.
The parameter is a ctype regex, and whatever ctypes are matched by the regex will be shown
via ASCII art.

Visualize access paths between two records

    $ amc_vis amc.FCtype\|amc.FField -xref:N


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

## amc_gc: AMC garbage collector

amc_gc is a tool for removing unused records from the dmmeta database.

amc_gc takes a target regex and an acr query as an argument. It finds all records
matching the query, then it tries to remove them one by one, and rebuilds the targets
(using abt) in a sandbox dir. If build succeeds with one of the records deleted
it means it wasn't needed in the first place.

Eliminate all ctypes in amc without which amc can be built

    $ amc_gc -target:amc -key:ctype:amc.%

## MariaDB integration

All ssim tables are compatible with MariaDB. The namespace (ns) corresponds to a database,
and ssimfile corresponds to a table.
Tools mysql2ssim and ssim2mysql perform conversion to/from ssim schema to SQL, and
acr_my can set up a local MariaDB server process and iteract with
it via a socket.

Here are some useful commands:
Start a local MariaDB server:

    acr_my -start dmmeta

Connect to the local server on the command line:

    acr_my -shell

From here, you can issue SQL commands:

    select count(*) from dmmeta.field where arg ='u8';

When you exit from the shell, the server keeps running.
You can stop the server and save changes:

    acr_my -stop

Or stop the server, discarding changes:

    acr_my -abort

You can also use acr -my as a shortcut for acr_my -start -shell -stop:

    echo 'update thash set unique='N' where field like "acr.%"' | acr -my -fldfunc dmmeta.%
    amc

The table dmmeta.sqltype allows ssim2mysql to map ctypes to SQL types so that
round tripping can occur without loss.

## Working with source files & targets

### Listing Files

List all targets:

     acr target

List all libraries:

     acr dmmeta.ns.nstype:lib

List all sources for target T:

     acr targsrc:T/%

Re-build copyright blocks in files:

     src_hdr -write

### Creating Source Files
It is perfectly possible to never use any of these commands, and just do everything by hand.
But the muse of automation requires that working with source files should be automated.

All of these commands perform necessary git commands as well.
Running acr_ed without -write outputs a shell script. -write simply runs the script.

Create new source file (target is picked automatically):

       acr_ed -create -srcfile cpp/lib/algo/x.cpp -write -comment "X functions"

Delete source file:

       acr_ed -del -srcfile cpp/lib/algo/x.cpp -write

Rename source file:

       acr_ed -srcfile cpp/lib/algo/x.cpp -rename cpp/lib/algo/y.cpp -write

Create new header:

       acr_ed -srcfile include/x.h -target amc -write -comment "blah"

### Functions & Headers

Re-scan source files for function prototypes

    src_func -updateproto

List functions in target:

    src_func amc

List function prototypes in target:

    src_func amc -proto

Edit functions in target matching regex:

    src_func amc %X% -e
    <use F4, shift-F4 to visit locations>

Show functions that are not documented:

    src_func amc -listiffy

### Program Text

Edit all functions in program:

     src_func amc -e

Search all hand-written source for pattern:

     src_func % % "%pattern%"

Search all generated source for pattern:

     src_func % % "%pattern%" -gen

Search hand-written source for pattern:

     src_func amc % "%pattern%"

Search source of specific target for pattern and open in editor:

     src_func amc % "%pattern%" -e

Search function arguments (i.e. first line, including return type and arguments):

     src_func % % -func:%amc::FCtype%

Show target functions, sort by name:

     src_func amc -sortname

## atf_unit: Unit Tests

atf\_unit runs single-function tests which are linked into atf\_unit.
The test source code is under cpp/atf/unit, and the names of all defined tests are in the table
atfdb.unittest.

Example: Create new unit test

    $ acr_ed -create -unittest amc.Test1

Example: Create and edit a new unit test

    $ acr_ed -create -unittest amc.Test1 -e

Example: Run a single unit test

    $ atf_unit http_lib.Http09

Example: Run several unit tests

    $ atf_unit http_lib.%

Example: Debug a unit test

    $ atf_unit http_lib.Http09 -debug


## Links

For an explanation of the directory structure, see [txt/directories.txt]
See [txt/self-similar.txt] for an explanation of the self-similar concept and philosophy.

ssim (super-simple) is the data format
See [txt/ssim.txt] for more.
You can browse ssim files with 'find data' or list them with 'acr ssimfile'.
Ssim files form a plain-text relational database and possess just 1 normal form,
called 'structured key normal form'.
See [txt/sknf.txt] for more info.

acr is a tool for creating and managing ssimfiles.
It integrates with the UNIX file-system and bash.
See [txt/acr.txt] for more.

abt is A Build Tool.
See [txt/abt.txt] for more.

For a walk-through of creating a new program with x-referenced tables,
see [txt/new-program.txt]

To set up shell-based auto-complete, see [txt/autocomplete.txt]

For an illustration of round-tripping data through MariaDB, or modifying
source code from bash command line using SQL statements, see [txt/sql.txt]

Enjoy,

Alexei Lebedev
Chief Software Architect
NYSE | Intercontinental Exchange
April 29, 2019

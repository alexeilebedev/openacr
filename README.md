This file was created with 'atf_norm readme' from txt/*.md -- *do not edit*

## Table Of Contents
   * [About](#about)
   * [Setup and Installation](#setup-and-installation)
      * [Tests](#tests)
   * [Directory Structure](#directory-structure)
      * [Binaries](#binaries)
      * [Editor configuration files](#editor-configuration-files)
   * [Command Lines](#command-lines)
      * [Bash Command Completion](#bash-command-completion)
      * [Example: Create New Ssimfile And Test Command Completion](#example-create-new-ssimfile-and-test-command-completion)
   * [Tutorial: Hello Meta World](#tutorial-hello-meta-world)
   * [Ssim files](#ssim-files)
      * [Structured Key Normal Form](#structured-key-normal-form)
      * [History of SKNF](#history-of-sknf)
   * [acr - Auto Cross Reference](#acr--auto-cross-reference)
      * [Querying](#querying)
      * [Creating A New Table](#creating-a-new-table)
      * [Inserting Data](#inserting-data)
      * [Generating Shell Scripts](#generating-shell-scripts)
      * [Inserting a Column](#inserting-a-column)
      * [Creating a Subset Table](#creating-a-subset-table)
      * [Following References Up](#following-references-up)
      * [Following References Down](#following-references-down)
      * [Deleting Records](#deleting-records)
      * [Manual Editing](#manual-editing)
      * [Checking Referential Integrity](#checking-referential-integrity)
      * [Renaming](#renaming)
      * [Cross-Product Types](#cross-product-types)
      * [Fldfunc fields](#fldfunc-fields)
      * [Querying On Non-Primary Key](#querying-on-non-primary-key)
      * [MySQL Integration](#mysql-integration)
      * [Specifying Flags On Command Line](#specifying-flags-on-command-line)
   * [acr_in: Show input tuples for target](#acr_in-show-input-tuples-for-target)
   * [acr_in: Show input tuples for target](#acr_in-show-input-tuples-for-target)
   * [abt - A Build Tool](#abt--a-build-tool)
   * [amc - Algo Model Compiler](#amc--algo-model-compiler)
      * [Running Amc](#running-amc)
      * [Query mode](#query-mode)
      * [References](#references)
      * [Sandbox mode](#sandbox-mode)
      * [Reading Code of Amc Itself](#reading-code-of-amc-itself)
      * [Example Show generated C++ struct](#example-show-generated-c--struct)
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
   * [Reftypes (field types)](#reftypes-field-types-)
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
      * [RegxSql:](#regxsql-)
      * [Sbrk](#sbrk)
      * [Smallstr](#smallstr)
      * [Tary: Flat array of records](#tary-flat-array-of-records)
      * [Thash: hash table](#thash-hash-table)
      * [Tpool: singly linked free-list](#tpool-singly-linked-free-list)
      * [Upptr:](#upptr-)
      * [Val: Value](#val-value)
      * [Varlen: variable-length tail portion of a struct](#varlen-variable-length-tail-portion-of-a-struct)
   * [String types and how to use them:](#string-types-and-how-to-use-them-)
   * [Table of Contents](#table-of-contents)
   * [Reftypes (field types)](#reftypes-field-types-)
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
      * [RegxSql:](#regxsql-)
      * [Sbrk](#sbrk)
      * [Smallstr](#smallstr)
      * [Tary: Flat array of records](#tary-flat-array-of-records)
      * [Thash: hash table](#thash-hash-table)
      * [Tpool: singly linked free-list](#tpool-singly-linked-free-list)
      * [Upptr:](#upptr-)
      * [Val: Value](#val-value)
      * [Varlen: variable-length tail portion of a struct](#varlen-variable-length-tail-portion-of-a-struct)
   * [String types and how to use them:](#string-types-and-how-to-use-them-)
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
   * [Working with source files & targets](#working-with-source-files--targets)
      * [Listing Files](#listing-files)
      * [Creating Source Files](#creating-source-files)
      * [Functions & Headers](#functions--headers)
      * [Program Text](#program-text)
   * [atf_unit: Unit Tests](#atf_unit-unit-tests)
   * [Links](#links)
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
   * [Working with source files & targets](#working-with-source-files--targets)
      * [Listing Files](#listing-files)
      * [Creating Source Files](#creating-source-files)
      * [Functions & Headers](#functions--headers)
      * [Program Text](#program-text)
   * [atf_unit: Unit Tests](#atf_unit-unit-tests)


## About

This is OpenACR, an open-source version of acr, which stands for Auto Cross
Reference, and its related tools.
It is the result of over 10 years of development and
production use. The tools were initially created by Alexei Lebedev at AlgoEngineering
with the goal of helping write low latency software of higher quality.
Subsequently they were licensed to Intercontinental Exchange as part
of a project to rewrite all of NYSE's electronic exchanges.

As of this writing, there are several national electronic markets
written in acr/amc, from matching engines to development tools,
with amc responsible for over 95% of all executable code (>4 million LOC)
and acr handling all configurations.

Since the tools are domain-agonostic, it was decided that open-sourcing
them would be the best way to ensure their longevity and value, and
also a way to give back to the community. After
all, if it weren't for Linux, gcc and the shell, this project wouldn't have
been possible in the first place. And so here we are.

Enjoy,

Alexei Lebedev
Chief Software Architect
NYSE | Intercontinental Exchange
April 29, 2019

## Setup and Installation

Presently, the project only builds on CentOS Linux. It has been tested on

* RHEL 7.0
* RHEL 7.3
* CentOS 7.6

The MariaDB packages are required in order to build mysql2ssim and ssim2mysql tools.

    yum install -y mariadb mariadb-devel mariadb-server
    
All commands can be issued from this, top-level directory.
Just add the relative path bin/ to your path.

    set PATH=$PATH:bin/

To build everything, you can run make (provided for convenience)
or the bootstapped version of abt called ai:

    ai

This should build abt using a bootstrapped shell script, then switch to abt
and build the rest. If any of this fails, you may need to file a bug report.

The home for this project is at https://github.com/alexeilebedev/openacr

After you run ai, you will see a couple of directories appear:

    drwxr-xr-x  2 alexei users  4096 Apr 29 10:57 dflt.debug-x86_64
    drwxr-xr-x  2 alexei users 12288 Apr 30 17:52 dflt.release-x86_64

These contain all object, library and executable files -- outputs of abt.
Each directory is just a flat set of files. Object file names are created by
replacing '/' with '.' in the relative path to the source:

    $ ls -l dflt.release-x86_64/ | grep abt
    -rwxr-xr-x 1 alexei users   335848 Apr 30 17:51 abt
    -rw-r--r-- 1 alexei users    14952 Apr 30 17:51 cpp.abt.build.o
    -rw-r--r-- 1 alexei users     9248 Apr 30 17:51 cpp.abt.disas.o
    -rw-r--r-- 1 alexei users    71528 Apr 30 17:51 cpp.abt.main.o
    -rw-r--r-- 1 alexei users    12776 Apr 30 17:51 cpp.abt.opt.o
    -rw-r--r-- 1 alexei users   232360 Apr 30 17:51 cpp.gen.abt_gen.o

These temporary directories shouldn't be in the path, instead
just bin/ is in the path. When compiling any executable with -install flag, abt creates
a soft link under bin/ pointing to one of these directories. This allows 'installing'
debug binaries, or profile binaries, or binaries built with another compiler.
At the time of writing, gcc and clang are supported as compilers.
To undo any non-standard installation, type 'ai'. This will reinstall all binaries
using the release configuration.

### Tests

There are two kinds of tests in OpenACR, unit tests and normalization tests.
Unit tests just check that some functions do what they should, and they can be invoked
with

    atf_unit

Normalization tests enforce various invariants on this repository, such as checking
that all files have an entry in the gitfile table, or round-tripping all data values
through a temporary MariaDB instance to ensure portability. These are invoked with

    atf_norm

All tests together are died in the script called normalize

    normalize

If this script succeeds, your latest changes are OK (relatively speaking).


## Directory Structure

* `bin`         All executable files. Add relative path 'bin/' to your PATH
* `conf`        Miscellaneous config files for emacs, vi
* `cpp`         All files ending in .cpp; cpp/gen contains outputs from amc
* `data`        All ssim files -- configuration tables etc.
* `diff`        Patch files
* `extern`      Files whose original home is not here.
* `include`     All files ending in .h; include/gen contains outputs from amc
* `lock`        Runtime lock files
* `temp`        Temp dir
* `txt`         .txt files

Intermediate binaries are kept in dflt.debug-x86_64 or dflt.release-x86_64

### Binaries

All executable files are in bin (both scripts and binary executables).
Binaries are compiled with abt -build and installed with abt -install.

### Editor configuration files

See files in conf/ for sample config files that provide ssim syntax highlighting, etc.
Here are some commands to get set it up:

    ln -sf $PWD/conf/emacs.el ~/.emacs
    ln -sf $PWD/conf/elisp ~/elisp
    ln -sf $PWD/conf/vimrc ~/.vimrc
    ln -sf $PWD/conf/vim ~/.vim


## Command Lines

All tools treat space after option and `:` interchangeably:
`-option:value` is the same as `-option value`.
I don't particularly like Java, but the first variant is preferrable,
because it's clear at the syntax level, without knowing what the command
expects, that `value` is a value and not a stand-alone option or a positional argument.

All tools accept standard options -verbose, -help, -debug. 
Verbosity is cumulative, each subprocess process inherits a lower verbosity level; it's a form of tracing.
Running a process with -v -v will execute most immediate subprocesses with -v.
The command line of any tool is represented by a ctype in the command namespace. 
Try this:

    acr ctype:cmdline.acr -t

### Bash Command Completion

To configure completion of all commands, do

    eval $(acr_compl -install)

Then, try auto-completing acr command-line options:

    $ acr -pr<tab>
    -pretty:<flag>  -print:<flag>

This auto-completes works with ssimfiles, and can auto-complete from any table.

    $ acr ctype:acr.FC<tab>
    acr.FCdflt    acr.FCheck    acr.FCppfunc  acr.FCtype

### Example: Create New Ssimfile And Test Command Completion

In fact, let's create and populate a brand new table, and add a dummy acr 
command line option referencing that table, just to see if auto-complete works.

* Step 1: create new ssimfile

    acr_ed -create -ssimfile dmmeta.mytable -write

* Step 2: populate it

    for X in abcd{10..99}; do echo "dmmeta.mytable mytable:$X"; done | acr -insert -write

* Step 3: create a new command line option

    acr_ed -create -field command.acr.mytable -arg dmmeta.Mytable -reftype Pkey -comment "A very useful option" -write

* Step 4: test

    $ acr -mytable:abcd2<tab>
    abcd20  abcd21  abcd22  abcd23  abcd24  abcd25  abcd26  abcd27  abcd28  abcd29

Inside acr, the field is accessible as _db.cmdline.mytable. Of course, manually
editing acr source code would be required to do something useful with this new option.

Since we don't really need these changes, we can undo them:

    $ git reset --hard

Or

    $ acr_ed -del -ssimfile dmmeta.mytable -write
    $ acr field:command.acr.mytable -del -write


## Tutorial: Hello Meta World

Follow the steps below to create a new sample program.
The program will load ctype and field tables, and cross-reference
them in two ways.
It will list all data structures in all executables, and compute all
back-references, i.e. for each struct (ctype), it will show all the
places where the struct is referenced.

Use this as a starting point for creating your own data model analysis tool.

~~~
acr_ed -create -target samp_hi -write
cat > cpp/samp/samp_hi.cpp << EOF
#include "include/algo.h"
#include "include/gen/samp_hi_gen.h"
#include "include/gen/samp_hi_gen.inl.h"
// Load ctype and field, print all fields and use sites
void samp_hi::Main() {
    prlog("Hello Meta World!");
    ind_beg(samp_hi::_db_ctype_curs,ctype,samp_hi::_db) {
        prlog("ctype "<<ctype.ctype);
        ind_beg(samp_hi::ctype_zd_field_curs,field,ctype) {
            prlog("    has field "<<field.field<<" of type "<<field.arg<<" reftype:"<<field.reftype);
        }ind_end;
        ind_beg(samp_hi::ctype_zd_arg_curs,arg,ctype) {
            prlog("    is referred to by field "<<arg.field<<" using "<<arg.reftype);
        }ind_end;
    }ind_end;
}
EOF
acr_ed -create -finput -target samp_hi -ssimfile:dmmeta.ctype -indexed -write
acr_ed -create -finput -target samp_hi -ssimfile:dmmeta.field -write
acr_ed -create -field:samp_hi.FField.p_ctype  -arg:samp_hi.FCtype -xref -via:samp_hi.FDb.ind_ctype/dmmeta.Field.ctype -write
acr_ed -create -field:samp_hi.FCtype.zd_field -arg:samp_hi.FField -xref -via:samp_hi.FDb.ind_ctype/dmmeta.Field.ctype  -write
acr_ed -create -field:samp_hi.FCtype.zd_arg   -arg:samp_hi.FField -xref -via:samp_hi.FDb.ind_ctype/dmmeta.Field.arg    -write
amc
abt -install samp_hi
acr ns:samp_hi -t
samp_hi
~~~


## Ssim files

Ssim is a Super-Simple line-oriented text format for
storing configuration data in the form of tables of tuples. Each tuple consists
of a type tag and key-value pairs called attributes. The first
key-value pair is a primary key.

    $ acr ctype:amc.% | head
    dmmeta.ctype  ctype:amc.BltinId       comment:""
    dmmeta.ctype  ctype:amc.CppkeywordId  comment:""
    dmmeta.ctype  ctype:amc.Enumstr       comment:Key
    dmmeta.ctype  ctype:amc.FAnonfld      comment:""
    ^^type tag          ^^primary key

Every line is treated as an element of a set.  There are no headers or
footers or other file markers, although lines can be commented out with #.
Any concatenation, permutation, or subset of two ssim files is a
valid ssim file, just like you would expect with sets.

Both keys and values may be arbitrary byte sequences. A string
containing non-obvious characters and be enclosed either with single
or double quotes (there being no difference between these types of quotes),
and inside the quotes, C++ string rules exactly apply. So "\t":"\001" is a valid
key-value pair.

A ssimfile maps directly to a relational table, and each line corresponds to a record
in a database.

In a given dataset,
The list of all ssim files is provided by "acr ssimfile".
The list of all attrbitutes is provided by "acr field"

Ssim tuples is typically held in data sets, (with one directory per
namespace, and one file per table) or in a single file.  One can use
grep, sed, awk, and other line-oriented tools to access these records.

All amc-supported commands support the -in argument which specifies the input
dataset for the tool -- either a file or a directory. By default it's "data"

### Structured Key Normal Form

There is a lot of literature on how to construct database schema so that it doesn't
have anomalies, and how to create primary keys; There are 6 or 7 'normal forms' -- invariants
that have to hold if you want certain anomalies to be absent from your schema.

Here I will describe the Structured Key Normal Form, or SKNF, which all ssim
schemas use. SKNF has no anomalies by construction and requires only one normal form.
All it boils down to is this:
a single field, the first field of a table, is the primary key, and it is either a simple type,
or a cross product of two other keys (which is the same thing if you allow for an empty set).

All other columns are non-NULL, and are also either simple types, or must refer to a key of some other
table. acr treats the key as a regular string.
When the primary key is a cross-product of two other sets, for instance dmmeta.Ctype, where dmmeta refers to ns:dmmeta
and ctype Ctype is a string, we use a separator, in this case '.'.

If you need some column to be NULLable, you delete
the column, and create a new ssimfile which is a subset of the original file.
Deleting the rows from this new ssimfile is equivalent to NULLing the original fields.

Let's look at an example:

    $ acr ctype:dmmeta.Ctype
    dmmeta.ctype  ctype:dmmeta.Ctype  comment:"C structure"

Here, the primary key is the string 'dmmeta.Ctype'.

But the key is restricted by a foreign key constraint.
Let's try acr with -fldfunc argument:

    $ acr ctype:dmmeta.Ctype -fldfunc
    dmmeta.ctype  ctype:dmmeta.Ctype  ns:dmmeta  name:Ctype  comment:"C structure"

If we were to rename this element to dmm.Ctype, we'd get an acr -check
error, since dmm is not a valid namespace.

dmmeta.Ctype.ns is an attribute of dmmeta.Ctype, and it's defined as a function of other fields,
or 'fldfunc' for short. The acr option -fldfunc expands all fldfuncs when showing output.
The 'ns' field in the query above is defined like this:

      dmmeta.field  field:dmmeta.Ctype.ns  arg:dmmeta.Ns  reftype:Pkey  dflt:""  comment:"translates to c++ namespace"
        dmmeta.substr  field:dmmeta.Ctype.ns  expr:.RL  srcfield:dmmeta.Ctype.ctype

The substr expression consists of triples of characters, where the first character is
what to look for in the string (in this case '.'), the second character is where to start (in this case
start at the right side of the string, 'R'), and the third character specifies what part of the
string to take after the match is found, or the search stops. If the match is not found, search stops
at the opposite end of the string. So, the expression .RL means "Scan from the right until you find the
character or hit the left side of the string. Then, return everything to the right of the current position".
These triples can be repeated as many times as necessary to "bite off" pieces of the string, for example
.RL.RL removes the 2 trailing dots from a string. The corresponding C++ function that implements this
search is called algo::Pathcomp.

There are no constraints other than foreign key constraints in ssim databases.
(acr -check can detect some errors such as strings that are too long for the underlying specified type,
but that's not a constraint per se)

### History of SKNF

In his seminal paper (https://www.seas.upenn.edu/~zives/03f/cis550/codd.pdf)
Codd explained that attaching an access path to a data set
results in anomalies which lie at the root of most data modeling problems,
and so must be avoided. Here for the first time, way after languages
such as Algol were designed and published, we have the most important
simplication -- the factoring of a state space into a set of sets, and a set
of access paths between them. Data structure is replaced with access structure
and a set.

Whenever we talk of "linked list of X" or "hash table of X", we are introducing
invisible anomalies by presupposing that all X are reachable via said access path.
To avoid this anomaly, we analyze X as a set by itself, and figure out what
information must be included in the element of each set in order to distinguish
two elements from each other. Only after we have done that, do we begin to attach
indexing structures, or access paths, so that we can retrieve elements of X
from the data store (in-memory or on-disk) faster.

If we view each element of a set as a struct with several fields, then the set of
fields which can be used to distinguish this element from others is called a key.
Many such keys are possible. For instance, we could generate a globally unique ID (GUID)
or get a sequence number from some service, and attach it to the elements of our
set as a key; This would be called a 'surrogate key'. In fact, most relational databases
blindly use surrogate keys (a field called 'id') for most purposes. There is even
an argument that surrogate keys are good since they protect the user from having
to know the schema. But the problem with surrogate keys is that they are not guessable,
and so two people cooperating on constructing the same table without communicating
with each other will run into a conflict: they will certainly include duplicate elements
into the table, marked with different surrogate keys.

So, surrogate keys don't solve the problem of constructing the set
For that, you have to describe the cardinality of
the set, either being an integer, or a subset of another set, or a cross product of two sets.

Codd was much of in favor of simple domains, where each column (field of a struct)
is either an integer or a string, and the primary key is described as a concatenation
of several such fields. This principle is not scalable, because if you use simple domains,
you get very complicated joins which are very sensitive to all layers of the schema.
And it's violated by SQL's timestamp type itself, which is a complex domain composed of year,
month, day, hour, minute, second and nanosecond. If we followed Codd blindly, we'd use 7
fields every time we needed a timestamp.

Codd was also in favor of the 4-valued boolean logic, where you have values
"yes", "no", "NULL but doesn't matter", and "NULL and it does matter". He had a lot
of trouble convincing people to implement this 4-valued logic, which was necessary
for consistency. Most database implementors ignored the 4-valued logic, sayng
"NULL is NULL, OK?", making Codd very upset. But most database
users heeded the "simple domain" rule. It's possible that if it weren't for
this unfortuante decision, SQL would be a lot more popular today.

If it weren't for the simple domain rule and NULLs, the proliferation of normal forms
wouldn't exist. And so SKNF represents a very simple but stable point in the space of
all possible schema schemas (meta-schemas), where you don't have NULLs and every key is
just a single composite value. It scales indefinitely, and every join takes just 2 values.

SKNF is not a name recognized in the industry. In the clade of DBMS construction philosophies,
the closest analog would be DKNF (Domain Key Normal Form).


## acr - Auto Cross Reference

Acr is a query and editing tool for a ssim (super-simple) database..
Acr is not concerned with big data; it's about small but highly cross-referenced data,
essentially configurations and source code. A rule of thumb is that acr will
process 1 million records/sec and the data sets must fit in memory.

### Querying

The default data directory "data", can be specified with -in option.
Assuming you've successfully built acr, you can type

    $ acr %
    amcdb.curs  curs:curs       comment:""
    amcdb.curs  curs:unordcurs  comment:""
    < about 10,000 more records are printed >

This will dump all records in the data directory, about 10,000 of them.
Of course, once you start growing a real project around this initial deposit, 
3,000 fields will seem like nothing.
A large project may easily get 10,000 ctypes, 1,000 ssimfiles and 30,000 fields,
and a few million records.

The query parameter for acr is a SQL regular expression.
SQL regexes are very convenient to use on the command line because they don't
use the whilecard character is `%` instead of `*`, and thus quoting is mostly unnecessary.

You could achieve a similar result to `acr %` by running

    $ find data -name "*.ssim" | xargs cat
    ...
    
### Creating A New Table

To show the rest of the commands, let's start with a couple of fresh tables
and proceed interactively. We will choose the `dev` namespace; this choice is quite arbitrary.

    $ acr_ed -create -ssimfile dev.a -write
    report.acr_check  records:13  errors:0
    report.acr  n_select:0  n_insert:6  n_delete:0  n_update:6  n_file_mod:1
    report.acr  n_select:624  n_insert:1  n_delete:0  n_update:1  n_file_mod:1
    report.amc  n_cppfile:123  n_cppline:258669  n_ctype:943  n_func:22556  n_xref:460  n_filemod:6

This will create a new empty table data/dev/a.ssim. The ctype for a has a single string field
named 'a'. The fact that this ssimfile was created was recorded in the dmmeta database.
The dmmeta database is how acr knows anything.

    $ acr ssimfile:dev.a
    dmmeta.ssimfile  ssimfile:dev.a  ctype:dev.A
    report.acr  n_select:1  n_insert:0  n_delete:0  n_update:0  n_file_mod:0

### Inserting Data

We can now populate this table with some data. `acr -insert -write` reads values from standard
input, inserts them into the database, and saves everything to disk when done. Duplicate keys 
will cause errors. If we need to ignore duplicates, we use `acr -replace -write`. 

    $ for X in {0..10}; do echo dev.a a:a$X; done | acr -insert -write 
    acr.insert  dev.a  a:a0   comment:""
    acr.insert  dev.a  a:a1   comment:""
    acr.insert  dev.a  a:a2   comment:""
    acr.insert  dev.a  a:a3   comment:""
    acr.insert  dev.a  a:a4   comment:""
    acr.insert  dev.a  a:a5   comment:""
    acr.insert  dev.a  a:a6   comment:""
    acr.insert  dev.a  a:a7   comment:""
    acr.insert  dev.a  a:a8   comment:""
    acr.insert  dev.a  a:a9   comment:""
    acr.insert  dev.a  a:a10  comment:""
    report.acr  n_select:11  n_insert:11  n_delete:0  n_update:11  n_file_mod:1

Now let's query this table. Here, we use a SQL regex and ask for values a2, a3 and a4.

    $ acr a:a'(2|3|4)'
    dev.a  a:a2   comment:""
    dev.a  a:a3   comment:""
    dev.a  a:a4   comment:""
    report.acr  n_select:3  n_insert:0  n_delete:0  n_update:0  n_file_mod:0

If we wanted to use these values in a shell script, there are a few flags that help with that.
First, the `-field` option:

    $ acr a:a'(2|3|4)' -field:a
    a2
    a3
    a4
    
Alternatively, we could ask for a regex of the values:

    $ acr a:a'(2|3|4)' -regxof:a
    (a2|a3|a4)
    
### Generating Shell Scripts

Finally, the `-cmd` option produces an executable shell script which can be piped to `sh`. script

    $ acr a:a1 -cmd 'echo ==== $a ===='
    acr_tuple=$'dev.a  a:a1  comment:""'
    acr_head=dev.a
    acr_rowid=1
    a=a1
    comment=''
    echo ==== $a ====
    
Piping through sh produces the desired result:

    $ acr a:a1 -cmd 'echo ==== $a ====' | sh
    ==== a1 ====
    
### Inserting a Column

We now might want to add a column to the `a` table.

    $ acr_ed -create -field dev.A.b -arg u32 -write
    ...

Let's update a few values with `acr -merge`:

    $ echo 'dev.a a:a1 b:55' | acr -merge -write
    report.acr  n_select:0  n_insert:0  n_delete:0  n_update:1  n_file_mod:1
    
Let's check if the b column is there:

    $ acr a | head -3
    dev.a  a:a0   b:0   comment:""
    dev.a  a:a1   b:55  comment:""
    dev.a  a:a10  b:0   comment:""

Yes, it is. What if we just look in the file itself?

    $ head -3 data/dev/a.ssim
    dev.a  a:a0  b:0  comment:""
    dev.a  a:a1  b:55  comment:""
    dev.a  a:a10  b:0  comment:""

We see largely the same result, except the columns are not aligned. By default,
acr flag `-pretty` is set to true, which aligns columns in blocks of 25.

### Creating a Subset Table

Now let's create another table which will be a subset of the first, and populate it.
When we specify the -subset parameter, we must include the ctype (not ssimfile).

    $ acr_ed -create -ssimfile:dev.b -subset dev.A -write
    ...

Let's quickly check how `B`'s fields were defined:

    $ acr field:dev.B.%
    dmmeta.field  field:dev.B.a        arg:dev.A         reftype:Pkey  dflt:""  comment:""
    dmmeta.field  field:dev.B.comment  arg:algo.Comment  reftype:Val   dflt:""  comment:""
    report.acr  n_select:2  n_insert:0  n_delete:0  n_update:0  n_file_mod:0

We see that the field dev.B.a was created, with `arg:dev.A` and `reftype:Pkey`.
`reftype:Pkey` means that the type of the field in question is the same as the type 
of the first field of the arg. In this case, `algo.Smallstr50`. It also means, as far as acr 
is concerned, that the value of this field must correspond to some existing value in `a`.
Notice that the new table's primary key has the name `a`, not `b`. This
is because of the convention that a foreign key reference uses the name of the target table.
We'll use values a3 through a7.

    $ for X in {3..7}; do echo dev.b a:a$X; done | acr -insert -write
    acr.insert  dev.b  a:a3  comment:""
    acr.insert  dev.b  a:a4  comment:""
    acr.insert  dev.b  a:a5  comment:""
    acr.insert  dev.b  a:a6  comment:""
    acr.insert  dev.b  a:a7  comment:""
    report.acr  n_select:5  n_insert:5  n_delete:0  n_update:5  n_file_mod:1
    
### Following References Up

Now let's see if acr recognizes that these tables are related. We'll use the `-nup 1`
argument to ask acr the following: after locating all records that match the initial query,
follow any of the references 1 level up, extending the selection. At the end, print the result.

    $ acr b:a3 -nup 1
    dev.a  a:a3  b:0  comment:""

    dev.b  a:a3  comment:""
    report.acr  n_select:2  n_insert:0  n_delete:0  n_update:0  n_file_mod:0

Yes! It worked. What if we try the same thing when querying a?

    $ acr a:a3 -nup 1
    dev.a  a:a3  b:0  comment:""
    report.acr  n_select:1  n_insert:0  n_delete:0  n_update:0  n_file_mod:0
    
Since `a` doesn't refer to anything, there is nothing to discover.

### Following References Down

But we *can* discover the b record by following from a:

    $ acr a:a4 -ndown  1
    dev.a  a:a4  b:0  comment:""

    dev.b  a:a4  comment:""
    report.acr  n_select:2  n_insert:0  n_delete:0  n_update:0  n_file_mod:0

Notice that in the output, `a` is always printed out before `b`. That's because 
acr sorts the output so that when it's scanned linearly, referential integrity is
maintained. Acr has a useful shortcut `-xref`, which stands for `-nup 100 -ndown 100`.
And we'll use the `-tree` option, which groups any dependent records underneath the
nearest parent, using indentation. For ssimfiles, indentation is irrelevant when parsing.
Leading whitespace is ignored (very much unlike Python). So the leading whitespace is purely
for human comprehension:

    $ acr b -xref -tree
    dev.a  a:a3  b:0  comment:""
      dev.b  a:a3  comment:""

    dev.a  a:a4  b:0  comment:""
      dev.b  a:a4  comment:""

    dev.a  a:a5  b:0  comment:""
      dev.b  a:a5  comment:""

    dev.a  a:a6  b:0  comment:""
      dev.b  a:a6  comment:""

    dev.a  a:a7  b:0  comment:""
      dev.b  a:a7  comment:""
    report.acr  n_select:10  n_insert:0  n_delete:0  n_update:0  n_file_mod:0
          
Since `-xref -tree` is very frequently used, command line option `-t` is an alias for it.

    $ acr b:a5 -t
    dev.a  a:a5  b:0  comment:""
      dev.b  a:a5  comment:""
      report.acr  n_select:2  n_insert:0  n_delete:0  n_update:0  n_file_mod:0
  
What about deleting records? Let's delete something.

    $ acr a:a4 -del -write
    acr.delete  dev.a  a:a4  b:0  comment:""

    acr.delete  dev.b  a:a4  comment:""
    report.acr  n_select:2  n_insert:0  n_delete:2  n_update:0  n_file_mod:1

As you can see, acr followed the reference and deleted the offending b record as well as the
matching a record.

### Deleting Records

What if we wanted to see what `-del` does without modifying the database?
We could omit the `-write` option. Notice that acr prints the records that *would*
be deleted, but in the final report, `n_file_mod:0` so we know that nothing was written back.

    $ acr a:a5 -del
    acr.delete  dev.a  a:a5  b:0  comment:""
    
    acr.delete  dev.b  a:a5  comment:""
    report.acr  n_select:2  n_insert:0  n_delete:2  n_update:0  n_file_mod:0

`-del` works with any number of records. You could delete the entire database with
`acr % -del -write`.

### Manual Editing

The option -e launches an editor (as specified with the environment
variable EDITOR), allowing you to edit the selected set. When the editor
exits, whatever tuples were in the original selection are deleted from
the ssim files and replaced with the edited output. To abort editing,
kill the editor and remove temporary file temp/acr.ssim.

### Checking Referential Integrity

Let's insert a record into the `b` table which has no corresponding record in the `a` table.

    $ echo 'dev.b a:xyz' | acr -insert -write
    acr.insert  dev.b  a:xyz  comment:""
    report.acr  n_select:1  n_insert:1  n_delete:0  n_update:1  n_file_mod:1

Now we have a record referring to a non-existing `xyz`. This is clearly a foreign
key constraint violation, but because we've been running acr with `-check:N` (the default),
the violation was allowed. The reason `acr -check` is off by default is that it's 
virtually impossible to interactively build a nice dataset without it being `broken` at 
one point or another. It's like if your editor disallowed syntactically incorrect programs -- 
you wouln't be able to even type up "hello world". Usually, it's applications that load
and cross-reference records are the ones that care deeply about constraint violation.

    $ acr -check b
    data/dev/b.ssim:5: Invalid value a:xyz
    dev.b  a:xyz  comment:""

    Type               dev.A
    Valid values       a0, a1, a10, a2, a3, a5, a6, a7, a8, a9

    acr.badrefs  ctype:dev.B  nbad:1

    report.acr_check  records:9  errors:3
    report.acr  n_select:1  n_insert:0  n_delete:0  n_update:0  n_file_mod:0

We wee that acr has detected the error and given us some suggestions on how to fix it.
We can run acr with `-check -e` and delete the record in question.
But we could also delete the offending record, rename it, or insert a missing `a` record.

(Notice that `-check -del` doesn't work as you would expect (this might be fixed later)).

### Renaming

Speaking of renaming, we can rename whole clusters of records with one command:

    $ acr a:a3 -rename a99 -write
    report.acr  n_select:0  n_insert:0  n_delete:0  n_update:2  n_file_mod:1

We can check that the dependencies were followed during the rename.

    $ acr a:a99 -t
    dev.a  a:a99  b:0  comment:""
      dev.b  a:a99  comment:""
      report.acr  n_select:2  n_insert:0  n_delete:0  n_update:0  n_file_mod:0
  
### Cross-Product Types

Now it's time to do something more interesting. We'll create a table `c`, which will be
an independent table; we will populate it with a few colors:

    $ acr_ed -create -ssimfile:dev.c -write
    ...

    $ for X in red green blue; do echo dev.c c:$X; done | acr -insert -write
    acr.insert  dev.c  c:red    comment:""
    acr.insert  dev.c  c:green  comment:""
    acr.insert  dev.c  c:blue   comment:""
    report.acr  n_select:3  n_insert:3  n_delete:0  n_update:3  n_file_mod:1

Now let's create a table `d`, whose key is a cross product of `b` and `c`.

    $ acr_ed -create -ssimfile dev.d -subset dev.B -subset2 dev.C -separator . -write
    ...
    
    $ for B in a5 a6 a7; do for C in red green blue; do echo dev.d d:$B.$C; done; done | acr -insert -write
    acr.insert  dev.d  d:a5.red    comment:""
    acr.insert  dev.d  d:a5.green  comment:""
    acr.insert  dev.d  d:a5.blue   comment:""
    acr.insert  dev.d  d:a6.red    comment:""
    acr.insert  dev.d  d:a6.green  comment:""
    acr.insert  dev.d  d:a6.blue   comment:""
    acr.insert  dev.d  d:a7.red    comment:""
    acr.insert  dev.d  d:a7.green  comment:""
    acr.insert  dev.d  d:a7.blue   comment:""
    report.acr  n_select:9  n_insert:9  n_delete:0  n_update:9  n_file_mod:1

Now the most interesting part is, how does acr see the resulting structure?
Below we see that acr has followed the schema and discovered all the necessary dependencies,
grouping `d` under `b`, which groups under `a`. All the referenced colors from `c` were
included in the output, but didn't become part of the tree, because only one parent is the *main*
parent, and acr chooses the leftmost parent.

    $ acr d -t
    dev.c  c:blue   comment:""
    dev.c  c:green  comment:""
    dev.c  c:red    comment:""
    dev.a  a:a5  b:0  comment:""
      dev.b  a:a5  comment:""
        dev.d  d:a5.blue   comment:""
        dev.d  d:a5.green  comment:""
        dev.d  d:a5.red    comment:""

    dev.a  a:a6  b:0  comment:""
      dev.b  a:a6  comment:""
        dev.d  d:a6.blue   comment:""
        dev.d  d:a6.green  comment:""
        dev.d  d:a6.red    comment:""

    dev.a  a:a7  b:0  comment:""
      dev.b  a:a7  comment:""
        dev.d  d:a7.blue   comment:""
        dev.d  d:a7.green  comment:""
        dev.d  d:a7.red    comment:""
    report.acr  n_select:17  n_insert:0  n_delete:0  n_update:0  n_file_mod:0

Let's look at the definition of `dev.D` type. The reason I have been holding off
on showing the definitions of the types `dev.A`, `dev.B`, and `dev.C`.
What you see here is the self-similar aspect of `acr`: the definitions
are nothing more than records that were inserted by acr_ed into the appropriate
databases (using, just like us, `acr -insert -write`), and -t discovers and shows
their relationships.

    $ acr ctype:dev.D -t
    dmmeta.nstype  nstype:ssimdb
      dmmeta.ns  ns:dev  nstype:ssimdb  comment:""
        dmmeta.ctype  ctype:dev.D  comment:""
          dmmeta.field  field:dev.D.d  arg:algo.Smallstr50  reftype:Val   dflt:""  comment:""
          dmmeta.field  field:dev.D.b  arg:dev.B            reftype:Pkey  dflt:""  comment:""
            dmmeta.substr  field:dev.D.b  expr:.RL  srcfield:dev.D.d

          dmmeta.field  field:dev.D.c  arg:dev.C  reftype:Pkey  dflt:""  comment:""
            dmmeta.substr  field:dev.D.c  expr:.RR  srcfield:dev.D.d

          dmmeta.field  field:dev.D.comment  arg:algo.Comment  reftype:Val  dflt:""  comment:""
          dmmeta.cfmt  cfmt:dev.D.String  printfmt:Tuple  read:Y  print:Y  sep:""  genop:Y  comment:""
          dmmeta.ctypelen  ctype:dev.D  len:204  alignment:1  padbytes:0

    dmmeta.ssimfile  ssimfile:dev.d  ctype:dev.D
      dmmeta.ssimsort  ssimfile:dev.d  sortfld:dev.D.d  comment:""
    report.acr  n_select:13  n_insert:0  n_delete:0  n_update:0  n_file_mod:0

### Fldfunc fields

One thing we see in the print-out above, is that the fields `dev.D.b` and `deb.D.c` are not data
fields. They are defined as subswtrings of `dev.D.d` (the primary key), using expressions `.RL`
and `.RR`. Such computed fields are called fldfuncs. Acr treats fldfunc fields in the same way 
as all other fields, but doesn't display them by default unless `-fldfunc` has been specified.
Let's ask acr to show us the fldfunc values:

    $ acr d -fldfunc
    dev.d  d:a5.blue   b:a5  c:blue   comment:""
    dev.d  d:a5.green  b:a5  c:green  comment:""
    dev.d  d:a5.red    b:a5  c:red    comment:""
    dev.d  d:a6.blue   b:a6  c:blue   comment:""
    dev.d  d:a6.green  b:a6  c:green  comment:""
    dev.d  d:a6.red    b:a6  c:red    comment:""
    dev.d  d:a7.blue   b:a7  c:blue   comment:""
    dev.d  d:a7.green  b:a7  c:green  comment:""
    dev.d  d:a7.red    b:a7  c:red    comment:""
    report.acr  n_select:9  n_insert:0  n_delete:0  n_update:0  n_file_mod:0

We can also use `-regx` to quickly confirm which values are present:

    $ acr d -regxof:c
    (blue|green|red)

There are two other types of fldfuncs in addition to substr: bitfields and externally-defined values.
These will be described later.

The expression such as '.RL' is not limited to 3 characters. It can be any number.
For example, '.RL/RR' first drops everything to the right of the last dot, then selects
everything to the right of the last `/`.
The funciton that parses these expressions is called Pathcomp, and we can easily show its source
code right on the comman line:

    $ src_func % Pathcomp

### Querying On Non-Primary Key

It is possible to ask acr to select by a non-primary column. For that, we have to include the full
field name:

    $ acr dev.D.c:blue
    dev.d  d:a5.blue  comment:""
    dev.d  d:a6.blue  comment:""
    dev.d  d:a7.blue  comment:""
    report.acr  n_select:3  n_insert:0  n_delete:0  n_update:0  n_file_mod:0

The query string you pass to acr applies not just to the value part, but to the key as well.
So, if we wanted to search both `b` and `c` tables, we'd write:

    $ acr '(c|b)'
    dev.c  c:blue   comment:""
    dev.c  c:green  comment:""
    dev.c  c:red    comment:""

    dev.b  a:a99  comment:""
    dev.b  a:a5  comment:""
    dev.b  a:a6  comment:""
    dev.b  a:a7  comment:""
    report.acr  n_select:7  n_insert:0  n_delete:0  n_update:0  n_file_mod:0

### MySQL Integration

The option -my launches an instance of mariadb and imports all namespaces that cover selected records
as databases into mariadb. It then drops you into a mariadb shell, where you can apply needed transformations.
Upon exit, the data is downloaded from the database back to disk. -my implies -write.

### Specifying Flags On Command Line

Whenever a bool field is present in the command line, it can be specified in a few different ways:

    acr % -print:N       # OK
    acr % -print:Y       # database-style -- OK
    acr % -print:true    # OK
    acr % -print         # defaults to true

## acr_in: Show input tuples for target

acr_in computes the names and the order of ssimfiles
which constitute target's declared input. Input fields are simply
those fields for which finput records exist, e.g. for target t:

    acr finput:t.%

but taken recursively over all libraries that the target uses.

With -data argument, acr_in also loads the specified ssimfiles in memory
and prints out their contents in the order in which it is safe to load.

The order of ssimfiles is determined as a transitive closure on Pkey
references, and is independent of the target itself. This means that
acr_in can be called with an regex of target names (e.g. %), and the
resulting input can be fed into any one of the targets implied by the
regex, without error.

Example: Create canned input file for a given tool:

    acr_in amc -data > tempfile
    amc -in:tempfile
    # this is exactly the same as running amc -in:data



## acr_in: Show input tuples for target

acr_in computes the names and the order of ssimfiles
which constitute target's declared input. Input fields are simply
those fields for which finput records exist, e.g. for target t:

    acr finput:t.%

but taken recursively over all libraries that the target uses.

With -data argument, acr_in also loads the specified ssimfiles in memory
and prints out their contents in the order in which it is safe to load.

The order of ssimfiles is determined as a transitive closure on Pkey
references, and is independent of the target itself. This means that
acr_in can be called with an regex of target names (e.g. %), and the
resulting input can be fed into any one of the targets implied by the
regex, without error.

Example: Create canned input file for a given tool:

    acr_in amc -data > tempfile
    amc -in:tempfile
    # this is exactly the same as running amc -in:data



## abt - A Build Tool

ABT is a build tool. The argument to abt is a target name regex.
ABT reads some ssim files (use acr_in abt to find out which ones);
Builds a dependency dag based on #includes; Invokes build commands.

With -cfg, abt can select different option sets.
Cfg can be release, profile, coverage, debug, or some custom config.

The list of available targets is loaded from dev.target ("acr dev.target" to list).
List of source files for each target is in by dev.targsrc.
To view the configuration for a given target, use acr dev.target:amc -t -e

Using configuration cfg:release, source file
cpp/amc/main.cpp compiles to dflt.release-x86_64/cpp.amc.main.o

Example: Build all

         $ abt -install %

Example: Re-build all, verbose mode

         $ abt -clean -install %

Example: Disassemble one function

         $ abt -cfg release amc -disas:%Main%

Example: Find out what files are out of date for a target

         $ abt -ood amc

Example: Install debug version of binaries into bin/

         $ abt % -install -cfg:debug

Example: Add a library to link with an executable

         echo dev.targdep:amc.json_lib | acr -replace -write



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


## Bootstrapping Magic

Let's consider the three main tools in this repository: acr, amc and abt.

The three tools are interdependent:
 acr, abt and amc all use acr to manage and validate their inputs.
 acr, abt and amc's in-memory structures are all generated with amc
 acr, abt and amc are all build using abt

There are several examples of such systems in the world.
The most famous one is the LISP interpreter as expressed in LISP itself.
Another example is gcc after it was compiled by gcc that was compiled by gcc.

Humbly joining this hallowed group is amc, which generates most
of its own source code as C++ and lets you modify this source code with
perl, sed and MariaDB from the bash command line, without introducing
a new language or an interpreter.

Why is this important?
You see, when a LISP interpreter interprets LISP, the new interpreter is
qualitatively different from the old one: it is slower. It can only
run smaller jobs than its parent.

Similarly, the C++ template language is less powerful than the parent language.
If you go back one step and extended C++ itself by changing its grammar,
and update the compiler accordingly, you get a new language that's not C++, and
cannot be mixed with C++.

So, after one cycle of application of these tools (LISP interpretation, C++ templates),
you get to a new and better place, but that place is
temporary, and you don't lock in the gains. Yet *it is* possible to
lock in the gains. For that, we need a tool whose input is about equally readable and writable
by both human and the machine, and where most of the source code is derived
from this input. That way, we skip the slow interpretation layer, and
get to lock in any iterative improvements.

And we want to avoid creating a new language, at all costs, because creating a new
language is such an expensive proposition. You now need glue libraries between your
language and all other languages; and you need debuggers, and multi-platform support
just for starters.
But a conservative subset of C++ is all you need to write anything.
C++ with an extra library is still C++.

In the world where data is kept in a machine-readable format, and most of the source code
is generated, the tools become universal: any tool works with almost any other tool. 

    'abt acr' builds acr.
    'abt abt' builds itself.

    'acr field:dmmeta.Field.field' describes its own primary key.
    'acr ctype:dmmeta.Ctype' describes the struct type (C type).

    'amc acr.%' generates from scratch (most of) the source code for acr
    'amc amc.%' generates from scratch the source code of itself.

    'src_func abt' shows the hand-written source code of abt.
    'src_func src_func' shows the hand-written source code of itself.

    'acr_in -data amc' shows all of the inputs that amc takes
    'acr_in -data acr_in' shows all of the inputs that it takes.

    'acr_compl -line amc' shows bash completions for amc
    'acr_compl -line acr_compl' shows bash completions for acr_compl itself

    'acr ns:abt -t' shows the definitions of all abt structures
    'acr ns:acr -t' shows the definitions of its own structures

And of course, the point of the tools is not to compile themselves; 
The idea is that this repo is extended with new commands and tools specific to some project,
maintaining the same directory structure and basic conventions.

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



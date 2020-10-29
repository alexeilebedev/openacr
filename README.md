This file was created with 'atf_norm readme' from files in [txt/](txt/) -- *do not edit*

## Table Of Contents
   * [About](#about)
   * [Why Use It](#why-use-it)
   * [Setup and Installation](#setup-and-installation)
      * [Pre-requisites: CentOS:](#pre-requisites-centos)
; [Pre-requisites: Ubuntu/Debian](#pre-requisites-ubuntu-debian); [Pre-requisites: MacOS](#pre-requisites-macos); [Path](#path); [Building](#building); [Cygwin Support](#cygwin-support); [Editor configuration files](#editor-configuration-files); [Environment Variables](#environment-variables); [Known Issues](#known-issues)
   * [Directory Structure](#directory-structure)
      * [Binaries](#binaries)
; [Intermediate Files](#intermediate-files)
   * [Tutorial 1: Parts And Projects Tutorial](#tutorial-1-parts-and-projects-tutorial)
   * [Tutorial 2: Parts And Projects in SQL](#tutorial-2-parts-and-projects-in-sql)
   * [Tutorial 3: Scheduling And Main Loop](#tutorial-3-scheduling-and-main-loop)
   * [Tutorial 4: Command Lines](#tutorial-4-command-lines)
   * [Structured Keys](#structured-keys)
   * [acr: Auto Cross Reference](#acr-auto-cross-reference)
   * [acr_in: Show input tuples for target](#acr_in-show-input-tuples-for-target)
   * [abt: A Build Tool](#abt-a-build-tool)
   * [acr_ed: Acr Editor](#acr_ed-acr-editor)
   * [amc: Algo Model Compiler](#amc-algo-model-compiler)
   * [Tests](#tests)
   * [Mdbg: Gdb front-end](#mdbg-gdb-front-end)
   * [Scriptlets](#scriptlets)
   * [Coding Style](#coding-style)
   * [Testimonials](#testimonials)


## About

The home for this project is at https://github.com/alexeilebedev/openacr

This is OpenACR, an open-source version of acr and amc, powerful
and extensible tools for creating programs and writing code as data.

It is the result of over 10 years of development and
production use. The tools were initially written with the goal of formalizing
construction of low-latency & realtime programs,
but they ended up being suitable for all sorts of other things, and the code generation
part took on a life of its own, eventually generating most of its own source code.

Since the tools are domain-agnostic, it was decided that open-sourcing
them would be the best way to ensure their longevity and value, and
also a way to give back to the community. After
all, if it weren't for other open-source projects like Linux, gcc and the shell, 
this project wouldn't have been possible in the first place. And so here we are.

OpenACR is in an interesting position of having been co-developed in connection with a large
software project, and later extracted back from it. Its usability
on small projects is assumed but not proven. I believe its ideal use case is realized
when it is taken as a kernel, and an ecosystem of commands and corresponding
configuration data files are grown in-place around it. Thus, it is meant to be used
a core of a project.

Enjoy,

Alexei Lebedev
April 29, 2019


## Why Use It

Most of software design, or rather design of libraries, looks like this:
library author decides what type of data to represent and manipulate in memory;
Without exception, the library declares some number of *records*, each containing
some fields, and a set of functions that manipulate sets of these records while
maintaining some library-wide invariants.
At a higher level, the library author is said to pick some *data structure*
that offers efficient/economical creation,
modification, updates, and querying. Update operations range from allocation and 
handling of memory-related conditions and errors, to business logic (the most
application-specific part).

Designing abstract data structures is difficult because of the need to track  various 
pointers and cross-references in the resulting data structures, and subsequently 
rewriting these pointers without causing corruption. To be robust and to avoid leaks, 
the library must also track resources that were allocated.
This difficulty, and the corresponding debugging effort, which usually exceeds
the initial coding time by 10- or 100x, means that once a library has been built, it
is not updated frequently. 
Sometimes, the choice of the data structure itself makes adding additional
indexes impractical. For instance, if we think of our main data structure as being 
a "hash table of X", we have committed to a key-value map as the main access structure,
and all other operations are operations other than lookup by key are made less efficient.
And if we have decided to use "a binary tree of X", then we'll use the binary tree lookup
and not consider also hashing the items. This type of thinking characterizes "data structure" 
design, and is taught in schools. 
The very term "data structure" presupposes that we take our data and then structure
it somehow. The metaphor is some sort of data origame, or moving data around for 
better access. There is no concept of access as being separate from the data.
This is assumed to be the price to pay if we want to write good algorithms.

On the other hand, there is a different approach to handling data -- the database approach.
If the data structure approach blends data with indexing, the database approach maximally separates
them. The data which we want to handle is first split (factored) into various tables. 
A table contains a number of records, and each record has some fields (also known as attributes).
We then create *indexes on attributes*, so that we can quickly locate
any record. Typically, any number of such indexes are allowed by a database management system.
Then, we write a query in some special query language, and a mechanism known as the query
planner decides which indexes to use to answer the query (so it's not just a multiply-nested
for-loop over all tables) and hopefully completes the query. But not *that*
quickly. It is an accepted fact that you pay for the generality of this approach with
correspondingly low runtime performance. When you really want performance, you use classical
methods to get it.

And yet, we know that all programming can be viewed as manipulation of in-memory databases.
Thus, the main motivation of *amc* is to give you the flexibility of crafting your own 
in-memory database with any number of tables and indexes, without having to implement
all the operations that implement creation, replacement, update, deletion and querying
of these records.

As a software architect and user of amc, you decide on the schema to use for your in-memory
database, and amc follows the constraints in your schema to implement cascading deletes, 
automatic group-bys, fast allocation of records using chained memory pools, and many other things.
The number and the variety of structures generated by amc is more similar to that of
a standard C++ library than an RDBMS.

Amc generates roughly 20 lines of code for each line of input. The code it generates 
is usually of the most error-prone and performance-critical kind, and you can account
for the generated code at the assembly instruction level. The generated code is documented,
readable and debuggable, and is provided in a standard target language: a conservative subset of C++ 
(circa C++ 1997), eschewing all modern constructs. 
This value proposition has immense consequences:
it means that a large and expensive software project can become a medium-sized software project,
and a medium-sized software project can become a small software project. Ultimately, 
this is what makes amc worth learning

When you co-evolve the schema together with your application, you can easily add and
remove indexes on the fly, and fine-tune them later.
At the same time, you can co-evolve amc itself with the project.
That's because amc isn't built and installed on the host system as a stand-alone, 
binary package. It is provided in source code form and versioned together with the
project it supports, forming a single eco-system.


## Setup and Installation

Presently, this project has been tested with the following distributions / compilers:

* clang on MacOS (a.k.a. Darwin)
* clang on FreeBSD
* clang on Linux (RHEL, CentOS, Ubuntu, Debian)
* g++ on Linux
* g++-9 on Linux

The MariaDB and OpenSSL packages are the main external dependencies.
MariaDB (formerly MySQL) is not really required, but it's used by `acr_my`, `ssim2mysql` and
`mysql2ssim` utilities. OpenSSL provides `libcrypto` which is for SHA1 functions.

### Pre-requisites: CentOS:

    yum install -y mariadb mariadb-devel mariadb-server

### Pre-requisites: Ubuntu/Debian

    apt install -y mariadb-client mariadb-common mariadb-server libmariadb-dev libmariadbd-dev libssl-dev
    apt install llvm llvm-dev  # to enable abt -compiler llvm

You may need the following hacks as well. This is because mysql hasn't been fully renamed
to mariadb yet.

    (cd /usr/lib/x86_64-linux-gnu && ln -sf libmariadbclient.a libmysqlclient.a)
    (cd /usr/lib/x86_64-linux-gnu && ln -sf libmariadbclient.so libmysqlclient.so)
    (cd /usr/include && ln -sf mariadb mysql)
    
### Pre-requisites: MacOS

Install brew. Then,

    brew install mariadb openssl
    ln -s /usr/local/opt/openssl/lib/libcrypto.a /usr/local/lib/
    ln -s /usr/local/opt/openssl/lib/libssl.a /usr/local/lib/

### Path

Add the relative path bin to your path.
All commands are issued from this, top-level directory. This normalizes all 
pathnames to a single form.

    set PATH=$PATH:bin

### Building

To build everything, you can run make (provided for convenience)
or the bootstapped version of abt called ai:

    ai

`ai` will choose a bootstrap file by looking at what kind of system you're running
on and what compilers you have. It won't install pre-requisites, that fun exercise
is left to the reader. See the section on [abt](#txt/abt-a-build-tool) for complete
documentation of this process.

`ai` should build abt using a canned script, then switch to abt
and build the rest. If any of this fails, you may need to file a bug report.

### Cygwin Support

Support of Windows (Cygwin) is needed, so if you'd like to help, please send pull requests.

### Editor configuration files

See files in `conf/` for sample config files that provide ssim syntax highlighting, etc.
Here are some commands to get set it up. You may already have some of these files, so you'll need
to decide whether you want to merge provided configs with yours, replace or do something else.

    ln -s $PWD/conf/emacs.el ~/.emacs
    ln -s $PWD/conf/elisp ~/elisp
    ln -s $PWD/conf/vimrc ~/.vimrc
    ln -s $PWD/conf/vim ~/.vim
    ln -s $PWD/conf/alexei/bash_profile ~/.bash_profile

### Environment Variables

* EDITOR - standard environment variable specifying which editor to use.

### Known Issues

Currently, optimization levels `-O2` and higher cannot be used with gcc 8 and higher,
due to the way the optimizer results in corruptions.


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
* `txt`         .txt and .md (markdown) files

### Binaries

All executable files are accessible from bin (both scripts and binary executables).
For each binary, there is a soft link from bin to the directory where the binary really sits.

Binaries are compiled with `abt -build` and the soft link in `bin/` can be updated with `abt -install`.

Binaries are not kept in git, only the soft links in bin/ are stored.
To make these links valid, the targets must be built with `ai`, which is a bootstrap script
that will build `abt` if it doesn't yet exist.

### Intermediate Files

Object files, libraries and executables are kept in `build/release/` directory.
Build directories can be wiped with `make clean`. 
After that, the `abt` binary will no longer exist, and it would have to be re-created with `ai`

The intermediate directory has a flat structure. The name of each object file is the 
path to the .cpp file with `/`s replaced by `.`s.
    
    $ ls build/release/
    abc                    cpp.amc.ctype.read.o     cpp.amc.regx.o          cpp.atf.amc.strconv.o ...
    abt                    cpp.amc.delptr.o         cpp.amc.sbrk.o          cpp.atf.amc.tary.o
    acr                    cpp.amc.dflt.o           cpp.amc.signature.o     cpp.atf.amc.varlen.o
    acr_compl              cpp.amc.disp.call.o      cpp.amc.size.o          cpp.atf.amc.zdlist.o
    ...


## Tutorial 1: Parts And Projects Tutorial
[See txt/tut1.md](txt/tut1.md)

## Tutorial 2: Parts And Projects in SQL
[See txt/tut2.md](txt/tut2.md)

## Tutorial 3: Scheduling And Main Loop
[See txt/tut3.md](txt/tut3.md)

## Tutorial 4: Command Lines
[See txt/tut4.md](txt/tut4.md)

## Structured Keys
[See txt/sknf.md](txt/sknf.md)

## acr: Auto Cross Reference
[See txt/acr.md](txt/acr.md)

## acr_in: Show input tuples for target
[See txt/acr_in.md](txt/acr_in.md)

## abt: A Build Tool
[See txt/abt.md](txt/abt.md)

## acr_ed: Acr Editor
[See txt/acr_ed.md](txt/acr_ed.md)

## amc: Algo Model Compiler
[See txt/amc.md](txt/amc.md)

## Tests
[See txt/tests.md](txt/tests.md)

## Mdbg: Gdb front-end
[See txt/mdbg.md](txt/mdbg.md)

## Scriptlets
[See txt/script.md](txt/script.md)

## Coding Style
[See txt/coding-style.md](txt/coding-style.md)

## Testimonials
[See txt/testimonials.md](txt/testimonials.md)

This file was created with 'abt_md' from files in [txt/](txt/) -- *do not edit*

## Table Of Contents
   * [About](#about)
   * [Why Use It](#why-use-it)
   * [Setup and Installation](#setup-and-installation)
      * [Pre-requisites: CentOS:](#pre-requisites-centos)
; [Pre-requisites: Ubuntu/Debian](#pre-requisites-ubuntu-debian); [Pre-requisites: MacOS](#pre-requisites-macos); [Path](#path); [Building](#building); [Cygwin Support](#cygwin-support); [Editor configuration files](#editor-configuration-files); [Environment Variables](#environment-variables); [Known Issues](#known-issues)
   * [Directory Structure](#directory-structure)
      * [Binaries](#binaries)
; [Intermediate Files](#intermediate-files)
   * [Writing Readmes](#writing-readmes)
   * [orgfile: organize and deduplicate files by timestamp and by contents](#orgfile-organize-and-deduplicate-files-by-timestamp-and-by-contents)
   * [Tutorial 1: Parts And Projects Tutorial](#tutorial-1-parts-and-projects-tutorial)
   * [Tutorial 2: Parts And Projects in SQL](#tutorial-2-parts-and-projects-in-sql)
   * [Tutorial 3: Scheduling And Main Loop](#tutorial-3-scheduling-and-main-loop)
   * [Tutorial 4: Command Lines](#tutorial-4-command-lines)
   * [Tutorial 5: Running multiple subprocesses](#tutorial-5-running-multiple-subprocesses)
   * [Structured Keys](#structured-keys)
   * [acr: Algo Cross-Reference - ssimfile database & update tool](#acr-algo-cross-reference---ssimfile-database---update-tool)
   * [acr_in: ACR Input - compute set of ssimfiles or tuples used by a specific target](#acr_in-acr-input---compute-set-of-ssimfiles-or-tuples-used-by-a-specific-target)
   * [abt: Algo Build Tool - build & link C++ targets](#abt-algo-build-tool---build---link-c---targets)
   * [acr_ed: ACR Editor Set of useful recipes, uses acr, abt, git, and other tools](#acr_ed-acr-editor-set-of-useful-recipes--uses-acr--abt--git--and-other-tools)
   * [amc: Algo Model Compiler: generate code under include/gen and cpp/gen](#amc-algo-model-compiler-generate-code-under-include-gen-and-cpp-gen)
   * [Describing Protocols in AMC](#describing-protocols-in-amc)
   * [Reading and Printing](#reading-and-printing)
   * [Fconst/Gconst: Enumerated Types](#fconst-gconst-enumerated-types)
   * [Amc: Size and field offset assertions](#amc-size-and-field-offset-assertions)
   * [Tests](#tests)
   * [mdbg: Gdb front-end](#mdbg-gdb-front-end)
   * [Scriptlets](#scriptlets)
   * [Coding Style](#coding-style)
   * [Testimonials](#testimonials)
   * [amc: Subprocess Invocation](#amc-subprocess-invocation)
   * [amc: Program Startup & Main](#amc-program-startup---main)
   * [AMS: Algo Mesaging System](#ams-algo-mesaging-system)
   * [atf_comp: Algo Test Framework - Component test execution](#atf_comp-algo-test-framework---component-test-execution)
   * [atf_cov: Line coverage](#atf_cov-line-coverage)
   * [Tracing](#tracing)
   * [Printing / Process output](#printing---process-output)
   * [Fault Management](#fault-management)
   * [Troubleshooting build errors](#troubleshooting-build-errors)
   * [atf_fuzz: Generator of bad inputs for targets](#atf_fuzz-generator-of-bad-inputs-for-targets)
   * [sv2ssim: sv2ssim - Separated Value file processor](#sv2ssim-sv2ssim---separated-value-file-processor)
   * [gcli: Gcli - gitlab/github command-line client](#gcli-gcli---gitlab-github-command-line-client)
   * [sandbox: Sandbox - sandbox management tool](#sandbox-sandbox---sandbox-management-tool)
   * [acr_dm: ACR Diff/Merge](#acr_dm-acr-diff-merge)
   * [abt_md: Tool to generate markdown documentation](#abt_md-tool-to-generate-markdown-documentation)
   * [acr_compl: ACR shell auto-complete for all targets](#acr_compl-acr-shell-auto-complete-for-all-targets)
   * [acr_my: ACR <-> MariaDB adaptor](#acr_my-acr-----mariadb-adaptor)
   * [amc_gc: Empirically eliminate unused records](#amc_gc-empirically-eliminate-unused-records)
   * [amc_vis: amc: draw access path diagrams](#amc_vis-amc-draw-access-path-diagrams)
   * [ams_cat: Algo Messaging System sample tool](#ams_cat-algo-messaging-system-sample-tool)
   * [ams_sendtest: Algo Messaging System test tool](#ams_sendtest-algo-messaging-system-test-tool)
   * [atf_amc: Unit tests for amc (see amctest table)](#atf_amc-unit-tests-for-amc--see-amctest-table-)
   * [atf_ci: Normalization tests (see citest table)](#atf_ci-normalization-tests--see-citest-table-)
   * [atf_cmdline: Test tool for command line parsing](#atf_cmdline-test-tool-for-command-line-parsing)
   * [atf_nrun: Run N subprocesses in parallel](#atf_nrun-run-n-subprocesses-in-parallel)
   * [atf_unit: Unit tests (see unittest table)](#atf_unit-unit-tests--see-unittest-table-)
   * [bash2html: Convert bash output and colours to html](#bash2html-convert-bash-output-and-colours-to-html)
   * [mysql2ssim: mysql -> ssim conversion tool](#mysql2ssim-mysql----ssim-conversion-tool)
   * [atf_gcli: test harness for gcli](#atf_gcli-test-harness-for-gcli)
   * [src_func: Access / edit functions](#src_func-access---edit-functions)
   * [src_hdr: Update source file / copyright header](#src_hdr-update-source-file---copyright-header)
   * [src_lim: Enforce line length, function length, and indentation limits](#src_lim-enforce-line-length--function-length--and-indentation-limits)
   * [ssim2csv: Ssim -> csv conversion tool](#ssim2csv-ssim----csv-conversion-tool)
   * [ssim2mysql: Ssim -> mysql](#ssim2mysql-ssim----mysql)
   * [ssimfilt: Tuple utility](#ssimfilt-tuple-utility)
   * [strconv: A simple string utility](#strconv-a-simple-string-utility)


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

    apt install -y mariadb-server mariadb-client libmariadb-dev libmariadbd-dev \
        libssl-dev libcurl4-openssl-dev liblz4-dev cppcheck
    apt install llvm llvm-dev  # to enable abt -compiler llvm

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


## Writing Readmes

The top-level `README.md` file is created by processing 
specific .md files under txt/.
These files are described in the readme table (`acr readme`).
Readme files can be in-line or included via a link. This is configured by the `inl` flag:

```
inline-command: acr readme | head -1
dev.readme  gitfile:txt/about.md                   inl:Y  sandbox:N  filter:""               comment:""
```

The top-level README.md file is created with `make readme` or, in reality, `atf_ci readme`.
It is a by-product of normalization and has to be up-to-date for every commit.

In addition, output of arbitrary commands can be embedded inside readme files using
the inline-command directive. Everything after this directive and before the next triple-backtick
is replaced with the output of specified command. An example of such a command can be seen above


## orgfile: organize and deduplicate files by timestamp and by contents
[See txt/orgfile.md](txt/orgfile.md)

## Tutorial 1: Parts And Projects Tutorial
[See txt/tut1.md](txt/tut1.md)

## Tutorial 2: Parts And Projects in SQL
[See txt/tut2.md](txt/tut2.md)

## Tutorial 3: Scheduling And Main Loop
[See txt/tut3.md](txt/tut3.md)

## Tutorial 4: Command Lines
[See txt/tut4.md](txt/tut4.md)

## Tutorial 5: Running multiple subprocesses
[See txt/tut5.md](txt/tut5.md)

## Structured Keys
[See txt/sknf.md](txt/sknf.md)

## acr: Algo Cross-Reference - ssimfile database & update tool
[See txt/acr.md](txt/acr.md)

## acr_in: ACR Input - compute set of ssimfiles or tuples used by a specific target
[See txt/acr_in.md](txt/acr_in.md)

## abt: Algo Build Tool - build & link C++ targets
[See txt/abt.md](txt/abt.md)

## acr_ed: ACR Editor Set of useful recipes, uses acr, abt, git, and other tools
[See txt/acr_ed.md](txt/acr_ed.md)

## amc: Algo Model Compiler: generate code under include/gen and cpp/gen
[See txt/amc.md](txt/amc.md)

## Describing Protocols in AMC
[See txt/amc/protocol.md](txt/amc/protocol.md)

## Reading and Printing
[See txt/amc/print.md](txt/amc/print.md)

## Fconst/Gconst: Enumerated Types
[See txt/amc/fconst.md](txt/amc/fconst.md)

## Amc: Size and field offset assertions
[See txt/amc/csize.md](txt/amc/csize.md)

## Tests
[See txt/tests.md](txt/tests.md)

## mdbg: Gdb front-end
[See txt/mdbg.md](txt/mdbg.md)

## Scriptlets
[See txt/script.md](txt/script.md)

## Coding Style
[See txt/coding-style.md](txt/coding-style.md)

## Testimonials
[See txt/testimonials.md](txt/testimonials.md)

## amc: Subprocess Invocation
[See txt/amc/exec.md](txt/amc/exec.md)

## amc: Program Startup & Main
[See txt/amc/main.md](txt/amc/main.md)

## AMS: Algo Mesaging System
[See txt/ams.md](txt/ams.md)

## atf_comp: Algo Test Framework - Component test execution
[See txt/atf_comp.md](txt/atf_comp.md)

## atf_cov: Line coverage
[See txt/atf_cov.md](txt/atf_cov.md)

## Tracing
[See txt/trace.md](txt/trace.md)

## Printing / Process output
[See txt/prlog.md](txt/prlog.md)

## Fault Management
[See txt/fm.md](txt/fm.md)

## Troubleshooting build errors
[See txt/abt-troubleshoot.md](txt/abt-troubleshoot.md)

## atf_fuzz: Generator of bad inputs for targets
[See txt/atf_fuzz.md](txt/atf_fuzz.md)

## sv2ssim: sv2ssim - Separated Value file processor
[See txt/sv2ssim.md](txt/sv2ssim.md)

## gcli: Gcli - gitlab/github command-line client
[See txt/gcli.md](txt/gcli.md)

## sandbox: Sandbox - sandbox management tool
[See txt/sandbox.md](txt/sandbox.md)

## acr_dm: ACR Diff/Merge
[See txt/acr_dm.md](txt/acr_dm.md)

## abt_md: Tool to generate markdown documentation
[See txt/abt_md.md](txt/abt_md.md)

## acr_compl: ACR shell auto-complete for all targets
[See txt/acr_compl.md](txt/acr_compl.md)

## acr_my: ACR <-> MariaDB adaptor
[See txt/acr_my.md](txt/acr_my.md)

## amc_gc: Empirically eliminate unused records
[See txt/amc_gc.md](txt/amc_gc.md)

## amc_vis: amc: draw access path diagrams
[See txt/amc_vis.md](txt/amc_vis.md)

## ams_cat: Algo Messaging System sample tool
[See txt/ams_cat.md](txt/ams_cat.md)

## ams_sendtest: Algo Messaging System test tool
[See txt/ams_sendtest.md](txt/ams_sendtest.md)

## atf_amc: Unit tests for amc (see amctest table)
[See txt/atf_amc.md](txt/atf_amc.md)

## atf_ci: Normalization tests (see citest table)
[See txt/atf_ci.md](txt/atf_ci.md)

## atf_cmdline: Test tool for command line parsing
[See txt/atf_cmdline.md](txt/atf_cmdline.md)

## atf_nrun: Run N subprocesses in parallel
[See txt/atf_nrun.md](txt/atf_nrun.md)

## atf_unit: Unit tests (see unittest table)
[See txt/atf_unit.md](txt/atf_unit.md)

## bash2html: Convert bash output and colours to html
[See txt/bash2html.md](txt/bash2html.md)

## mysql2ssim: mysql -> ssim conversion tool
[See txt/mysql2ssim.md](txt/mysql2ssim.md)

## atf_gcli: test harness for gcli
[See txt/atf_gcli.md](txt/atf_gcli.md)

## src_func: Access / edit functions
[See txt/src_func.md](txt/src_func.md)

## src_hdr: Update source file / copyright header
[See txt/src_hdr.md](txt/src_hdr.md)

## src_lim: Enforce line length, function length, and indentation limits
[See txt/src_lim.md](txt/src_lim.md)

## ssim2csv: Ssim -> csv conversion tool
[See txt/ssim2csv.md](txt/ssim2csv.md)

## ssim2mysql: Ssim -> mysql
[See txt/ssim2mysql.md](txt/ssim2mysql.md)

## ssimfilt: Tuple utility
[See txt/ssimfilt.md](txt/ssimfilt.md)

## strconv: A simple string utility
[See txt/strconv.md](txt/strconv.md)

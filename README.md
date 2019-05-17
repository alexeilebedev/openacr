This file was created with 'atf_norm readme' from files in [txt/](txt/) -- *do not edit*

## Table Of Contents
   * [About](#about)
      * [Contributors](#contributors)
   * [Setup and Installation](#setup-and-installation)
      * [Pre-requisites: CentOS:](#pre-requisites-centos)
; [Pre-requisites: Ubuntu/Debian](#pre-requisites-ubuntu-debian); [Pre-requisites: MacOS](#pre-requisites-macos); [Path](#path); [Building](#building); [Platform Short List](#platform-short-list); [Editor configuration files](#editor-configuration-files); [Environment Variables](#environment-variables); [Known Issues](#known-issues)
   * [Directory Structure](#directory-structure)
      * [Binaries](#binaries)
; [Intermediate Files](#intermediate-files)
   * [History & Intro](#history---intro)
      * [A Motivating Example](#a-motivating-example)
; [Creating Some Tuples](#creating-some-tuples); [Describing The Tuple Schema](#describing-the-tuple-schema); [Describing The Columns](#describing-the-columns); [Adding Tools](#adding-tools); [Describing The Tools](#describing-the-tools); [Representation & Manipulation](#representation---manipulation)
   * [Ssim files](#ssim-files)
      * [Ssim Data Sets](#ssim-data-sets)
; [Structured Key Normal Form](#structured-key-normal-form); [Decomposing A Domain Into Sets](#decomposing-a-domain-into-sets); [History of Database Design](#history-of-database-design); [Cardinality Analysis](#cardinality-analysis); [The Curse Of Simple Domains](#the-curse-of-simple-domains); [Remember 4-valued logic?](#remember-4-valued-logic-); [Structured Key Normal Form](#structured-key-normal-form)
   * [Bootstrapping Magic](#bootstrapping-magic)
   * [Hello Meta World](#hello-meta-world)
   * [acr: Auto Cross Reference](#acr-auto-cross-reference)
      * [Querying](#querying)
; [Creating A New Table](#creating-a-new-table); [Inserting Data](#inserting-data)
         * [The -replace option](#the--replace-option)
; [The -merge option](#the--merge-option); [The -trunc option](#the--trunc-option)
      * [Generating Shell Scripts](#generating-shell-scripts)
; [Deleting Records From Stdin](#deleting-records-from-stdin); [Inserting a Column](#inserting-a-column)
         * [The -before option](#the--before-option)
      * [Creating a Subset Table](#creating-a-subset-table)
; [Following References Up](#following-references-up); [Following References Down](#following-references-down); [Deleting Records](#deleting-records); [Manual Editing](#manual-editing); [Checking Referential Integrity](#checking-referential-integrity); [Renaming](#renaming); [Cross-Product Types](#cross-product-types); [Fldfunc fields](#fldfunc-fields); [Querying On Non-Primary Key](#querying-on-non-primary-key); [Sorting & RowIDs](#sorting---rowids); [Creating A New Data Set](#creating-a-new-data-set); [Using A File As A Data Set](#using-a-file-as-a-data-set); [The -schema option](#the--schema-option); [The -meta option](#the--meta-option); [The -cmt option](#the--cmt-option); [MySQL Integration](#mysql-integration)
   * [Command Lines](#command-lines)
      * [Boolean Options](#boolean-options)
; [Default Values](#default-values); [Integer Options](#integer-options); [Anonymous Options](#anonymous-options); [Other Data Types](#other-data-types); [Bash Command Completion](#bash-command-completion); [Completing From Table](#completing-from-table); [Inputting A Table](#inputting-a-table); [Regx Options](#regx-options); [The -version flag](#the--version-flag); [The -sig flag](#the--sig-flag); [Printing Command Lines](#printing-command-lines); [Subprocesses With Command Line](#subprocesses-with-command-line); [Verbosity](#verbosity)
   * [acr_in: Show input tuples for target](#acr_in-show-input-tuples-for-target)
      * [The -data option](#the--data-option)
; [The -checkable option](#the--checkable-option); [The -related option](#the--related-option)
   * [abt: A Build Tool](#abt-a-build-tool)
      * [Input Tables](#input-tables)
; [Output Directory](#output-directory); [Bootstrap System](#bootstrap-system); [The -install option](#the--install-option); [Target Definition](#target-definition); [Customizing Options](#customizing-options); [Disassembling](#disassembling); [Specifying a different compiler](#specifying-a-different-compiler); [The -ood option](#the--ood-option); [The -listincl option](#the--listincl-option); [Debugging the build](#debugging-the-build); [Compiler Cache](#compiler-cache)
   * [amc: Algo Model Compiler](#amc-algo-model-compiler)
      * [Introduction](#introduction)
; [Why Generate?](#why-generate-)
         * [Libraries Vs. Custom Code](#libraries-vs--custom-code)
; [Exponential Cost Of Software](#exponential-cost-of-software)
      * [Running Amc](#running-amc)
; [Query mode](#query-mode); [Ratio of Generated To Manual LOC](#ratio-of-generated-to-manual-loc); [Sandbox Mode](#sandbox-mode); [Reading Code of Amc Itself](#reading-code-of-amc-itself); [What is a Cross-reference?](#what-is-a-cross-reference-); [Main Input Tables](#main-input-tables); [String types and how to use them:](#string-types-and-how-to-use-them)
         * [Amc-generated String Types](#amc-generated-string-types)
      * [Field Name Prefix](#field-name-prefix)
; [The Algorithm For Generating Code](#the-algorithm-for-generating-code)
   * [Amc: Memory Pools](#amc-memory-pools)
      * [Types of Memory Pools](#types-of-memory-pools)
; [Declaring a Pool](#declaring-a-pool); [Base Pools](#base-pools); [Running Out Of Memory](#running-out-of-memory); [Blkpool](#blkpool); [Delptr: Optional Value](#delptr-optional-value); [Inlary: Inline Array](#inlary-inline-array); [Lary: Level array](#lary-level-array)
         * [Alloc](#alloc)
; [AllocMaybe](#allocmaybe); [InsertMaybe](#insertmaybe); [AllocMem](#allocmem); [EmptyQ](#emptyq); [Find](#find); [Last](#last); [N](#n); [RemoveLast](#removelast); [qFind](#qfind); [XrefMaybe](#xrefmaybe)
      * [Lpool: Level pool](#lpool-level-pool)
         * [FreeMem](#freemem)
; [AllocMem](#allocmem); [ReserveBuffers](#reservebuffers); [ReallocMem](#reallocmem)
      * [Malloc](#malloc)
; [Sbrk](#sbrk); [Tary: Flat indirect array (vector)](#tary-flat-indirect-array--vector-)
         * [Aliasing](#aliasing)
; [Addary](#addary); [Alloc](#alloc); [AllocAt](#allocat); [AllocN](#allocn); [EmptyQ](#emptyq); [Find](#find); [Getary](#getary); [Last](#last); [Max](#max); [N](#n); [Remove](#remove); [RemoveAll](#removeall); [RemoveLast](#removelast); [Reserve](#reserve); [AbsReserve](#absreserve); [Setary (copy one Tary to another)](#setary--copy-one-tary-to-another-); [Setary](#setary); [qFind](#qfind); [qLast](#qlast)
      * [Tpool](#tpool)
         * [Alloc](#alloc)
; [AllocMaybe](#allocmaybe); [Delete](#delete); [AllocMem](#allocmem); [FreeMem](#freemem); [Reserve](#reserve); [ReserveMem](#reservemem); [XrefMaybe](#xrefmaybe)
      * [Val: Value](#val-value)
   * [Amc Features](#amc-features)
      * [Scaled Decimal Types](#scaled-decimal-types)
; [Big-Endian Fields](#big-endian-fields); [Bitfields](#bitfields); [Steps](#steps); [Tracing](#tracing); [Base: Mixin](#base-mixin); [Bheap: Binary Heap](#bheap-binary-heap); [Bitset: Bitset over an array](#bitset-bitset-over-an-array); [Count: Count elements](#count-count-elements); [Fconst: Enumerated type](#fconst-enumerated-type); [Inlary: Inline array](#inlary-inline-array); [Lary: Level array](#lary-level-array); [Llist: Linked list](#llist-linked-list); [Opt: Optional last field in variable-length struct](#opt-optional-last-field-in-variable-length-struct); [Pmask: Presence mask](#pmask-presence-mask); [Ptr](#ptr); [Ptrary](#ptrary); [RegxSql:](#regxsql); [Smallstr](#smallstr); [Thash: hash table](#thash-hash-table); [Upptr:](#upptr); [Varlen: variable-length tail portion of a struct](#varlen-variable-length-tail-portion-of-a-struct)
   * [acr_ed: Acr Editor](#acr_ed-acr-editor)
      * [Targets](#targets)
         * [Create Target](#create-target)
; [Rename Target](#rename-target); [Delete Target](#delete-target)
      * [Ssimfiles](#ssimfiles)
         * [Create Ssimfile](#create-ssimfile)
; [Create Ssimfile as a Subset of Another Ssimfile](#create-ssimfile-as-a-subset-of-another-ssimfile); [Create Ssimfile as a Cross Product of 2 Other Ssimfiles](#create-ssimfile-as-a-cross-product-of-2-other-ssimfiles); [Rename Ssimfile](#rename-ssimfile); [Delete Ssimfile](#delete-ssimfile)
      * [Source Files](#source-files)
         * [Create Source File](#create-source-file)
; [Rename Source File](#rename-source-file); [Delete Source File](#delete-source-file)
      * [Ctypes](#ctypes)
         * [Create A Ctype](#create-a-ctype)
; [Rename A Ctype](#rename-a-ctype); [Delete A Ctype](#delete-a-ctype)
      * [Add An Input To Program](#add-an-input-to-program)
; [Create A New In-Memory Table](#create-a-new-in-memory-table); [Create An Index](#create-an-index)
         * [Create A Hash Table](#create-a-hash-table)
; [Create A Binary Heap](#create-a-binary-heap); [Create An AVL Tree](#create-an-avl-tree)
      * [Conditional X-Ref](#conditional-x-ref)
   * [Coding Style](#coding-style)
      * [Spaces, Indentation](#spaces--indentation)
; [Variable Names](#variable-names); [Member Functions](#member-functions); [Predicate Functions](#predicate-functions); [Curly Braces](#curly-braces); [Split Conditionals](#split-conditionals); [Curly Braces around Conditionals are Non-Optional](#curly-braces-around-conditionals-are-non-optional); [Use of semi-colon forces a new line](#use-of-semi-colon-forces-a-new-line); [Keep code separate from data](#keep-code-separate-from-data); [No Code In Headers](#no-code-in-headers); [Use query-command separation](#use-query-command-separation); [Keep it single-threaded](#keep-it-single-threaded); [Use Single Entry, Single Exit (SESE) style](#use-single-entry--single-exit--sese--style); [Single File Static Assignment](#single-file-static-assignment); [Document all non-static functions](#document-all-non-static-functions); [All rules allow exceptions](#all-rules-allow-exceptions)
   * [amc_vis: Visualize Ctype Dependencies and Access Paths](#amc_vis-visualize-ctype-dependencies-and-access-paths)
   * [amc_gc: AMC garbage collector](#amc_gc-amc-garbage-collector)
   * [MariaDB integration](#mariadb-integration)
   * [Working with source files & targets](#working-with-source-files---targets)
      * [Listing Files](#listing-files)
; [Creating Source Files](#creating-source-files); [Functions & Headers](#functions---headers); [Program Text](#program-text)
   * [Tests](#tests)
      * [Unit Tests](#unit-tests)
; [Normalization Checks](#normalization-checks); [Debugging](#debugging)
   * [Mdbg: Gdb front-end](#mdbg-gdb-front-end)
      * [Specifying arguments](#specifying-arguments)
; [The -tui option](#the--tui-option); [Specifying Breakpoints](#specifying-breakpoints); [Edit-and-retry](#edit-and-retry); [The -follow_child otption](#the--follow_child-otption); [Exceptions](#exceptions); [Gdb Python Interface](#gdb-python-interface); [Bugs](#bugs)
   * [Scriptlets](#scriptlets)
      * [Hilite: inline highlighter](#hilite-inline-highlighter)
         * [Blotter Mode](#blotter-mode)
; [Inline Specification](#inline-specification); [Diff mode](#diff-mode)
      * [Grephunk: patch file filter](#grephunk-patch-file-filter)
   * [FF - find files and in files](#ff---find-files-and-in-files)
         * [Selecting Files And Directories](#selecting-files-and-directories)
; [Negative Patterns](#negative-patterns); [Matching Options](#matching-options); [Editing Matches](#editing-matches); [Highlighting](#highlighting)
   * [Testimonials](#testimonials)
      * [1](#1)
; [2](#2); [3](#3); [4](#4)

## About

The home for this project is at https://github.com/alexeilebedev/openacr

This is OpenACR, an open-source version of acr and amc, powerful
and extensible tools for handling configuration data and creating programs.
(By configuration data, I mean perhaps a thousand tables and a
few million records).

It is the result of over 10 years of development and
production use. The tools were initially created by Alexei Lebedev at AlgoEngineering
with the goal of formalizing construction of low-latency & realtime programs,
but they ended up being suitable for all sorts of other things, and the code generation
part took on a life of its own, eventually generating most of its own source code.

The tools were subsequently licensed to Intercontinental Exchange.

Since the tools are domain-agnostic, it was decided that open-sourcing
them would be the best way to ensure their longevity and value, and
also a way to give back to the community. After
all, if it weren't for other open-source projects like Linux, gcc and the shell, 
this project wouldn't have been possible in the first place. And so here we are.

OpenACR is in an interesting position of having been co-developed with a large
software project, and later extracted back from it. Its usability
on small projects is assumed but not proven. I believe its ideal use case is realized
when it is taken as a kernel, and an ecosystem of commands and corresponding
configuration data files are grown in-place around it. Thus, it is meant to be used
in-vivo.

The above statements are do not represent ICE views or constitute an endorsement.

### Contributors

Here are some of the names of git authors who contributed code to this project,
in alphabetical order by first name. 

* Alexey Polovinkin
* Ara Aslyan
* Ezequiel Fernandes
* Hank Wang
* Hayk Mkrtchyan
* Jeffrey Wang
* Jeremy Xue
* John Brzezniak
* Jonathan Joshua
* Luke Huang
* Shreejith Billenahallilokegowda
* Vladimir Parizhsky

There were also many others who helped immensely with
discussions and influenced various design decisions.

Enjoy,

Alexei Lebedev
April 29, 2019


## Setup and Installation

Presently, this project has been tested on the following distributions / compilers:

* OS: RHEL, CentOS, Ubuntu
* g++: 4.8, 8.3, 9
* clang: 3.4.2
* MacOS: LLVM 10.0.1

The MariaDB and OpenSSL packages are required in order to build mysql2ssim and ssim2mysql tools.

### Pre-requisites: CentOS:

    yum install -y mariadb mariadb-devel mariadb-server

### Pre-requisites: Ubuntu/Debian

    apt install -y mariadb libmariadb-dev libmariadbd-dev libssl-dev
    apt install llvm llvm-dev  # to enable abt -compiler llvm

### Pre-requisites: MacOS

Install brew.
Then,

    brew install mariadb openssl
    ln -s /usr/local/opt/openssl/lib/libcrypto.a /usr/local/lib/
    ln -s /usr/local/opt/openssl/lib/libssl.a /usr/local/lib/

Please note that mariadb, not mysql should be installed.
Since MariaDB is the successor to `mysql`, there shouldn't be a problem.

### Path

All commands can be issued from this, top-level directory.
Add the relative path bin/ to your path.

    set PATH=$PATH:bin/

### Building

To build everything, you can run make (provided for convenience)
or the bootstapped version of abt called ai:

    ai

The default compiler abt uses is g++ prior to verison 9.
You will need to set the environment variable `COMPILER` to `g++-9` or `clang++` if 
you want to use those compilers. See the section on [abt](#txt/abt-a-build-tool) for more information.

This should build abt using a bootstrapped shell script, then switch to abt
and build the rest. If any of this fails, you may need to file a bug report.

### Platform Short List

Support FreeBSD and Windows (Cygwin) 
is very must desired, so if you'd like to help, please send pull requests.

### Editor configuration files

See files in conf/ for sample config files that provide ssim syntax highlighting, etc.
Here are some commands to get set it up.

    ln -s $PWD/conf/emacs.el ~/.emacs
    ln -s $PWD/conf/elisp ~/elisp
    ln -s $PWD/conf/vimrc ~/.vimrc
    ln -s $PWD/conf/vim ~/.vim
    ln -s $PWD/conf/alexei/bash_profile ~/.bash_profile

### Environment Variables

* EDITOR - standard environment variable specifying which editor to use

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

Object files, libraries and executables are kept in `dflt.debug-x86_64` or `dflt.release-x86_64`.
Here, `dflt` is the compiler name. Other possible names are `clang`.

Build directories can be wiped with `make clean`. After that, the `abt` binary will no longer exist,
and it would have to be re-created with `ai` or `abt-bootstrap`.

The intermediate directory has a flat structure. The name of each object file is the 
path to the .cpp file with `/`s replaced by `.`s.
    
    $ ls dflt.release-x86_64/
    abc                    cpp.amc.ctype.read.o     cpp.amc.regx.o          cpp.atf.amc.strconv.o ...
    abt                    cpp.amc.delptr.o         cpp.amc.sbrk.o          cpp.atf.amc.tary.o
    acr                    cpp.amc.dflt.o           cpp.amc.signature.o     cpp.atf.amc.varlen.o
    acr_compl              cpp.amc.disp.call.o      cpp.amc.size.o          cpp.atf.amc.zdlist.o
    ...


## History & Intro

This chapter introduces the concepts of ctypes, fields, namespaces, ssimfiles,
and with the assumption that you know some SQL.

### A Motivating Example

Let's begin with a classic modeling example involving projects and parts.
This is the example from Codd 1970's
[A Relational Model of Data For Large Shared Data Banks](https://www.seas.upenn.edu/~zives/03f/cis550/codd.pdf)

Codd considers 5 different database schemas, first four of which have access path anomalies --
i.e. they are unable to represent conditions such as an ownership of a part that's not associated
with any project, or a project with no parts, etc.

Let's use a MariaDB interactive shell as an example. You can start one manually, or you can use a provided
tool `acr_my` (assuming you successfully built everything), to create a local networkless instance.
Here is provide the namespace name `dmmeta` because

    $ acr_my -start -shell

I will omit MariaDB prompts to make it easier to copy/paste commands into your shell.
Let's create a new database.

    create database test;
    use test;

Let's create Codd's project schema. Instead of the name `commit`, which Codd used for the assignment
of parts to projects, we'll use the name `projpart`.

    create table project (project varchar(50));
    create table part (part varchar(50));
    create table partproj (part varchar(50), project varchar(50));

Now let's set up the primary keys

    alter table project add primary key project (project);
    alter table part add primary key part (part);
    alter table partproj add primary key part (part);

And the foreign key constraints

    alter table partproj add foreign key  (part) references part(part);
    alter table partproj add foreign key (project) references project(project);

And populate some data.

    insert into part (part) values ("part1"), ("part2");
    insert into project (project) values ("project1"), ("project2");
    insert into partproj (part,project) values ("part1","project1"), ("part2","project1");

Let's check if the projects are there:

    MariaDB [test]> select * from project;
    +----------+
    | project  |
    +----------+
    | project1 |
    | project2 |
    +----------+
    2 rows in set (0.00 sec)

So far so good. Now let's check the parts:

    MariaDB [test]> select * from part;
    +-------+
    | part  |
    +-------+
    | part1 |
    | part2 |
    +-------+
    2 rows in set (0.00 sec)

Also good. And now the part-to-project assignment.

    MariaDB [test]> select * from partproj;
    +-------+----------+
    | part  | project  |
    +-------+----------+
    | part1 | project1 |
    | part2 | project1 |
    +-------+----------+
    2 rows in set (0.00 sec)

By now that we have a database instance and some data in it, we can do all the things one does with the database --
connect to it, submit various queries, and serve up the results. Not only is it extremely useful
(which is why everybody does it), but the table-based approach is universal. The
[Zermelo-Fraenkel Set Theory](https://en.wikipedia.org/wiki/Zermelo%E2%80%93Fraenkel_set_theory),
first proposed in 1908, essentially says that all of constructible mathematical facts can be represented by N
sets, either simple ones, or constructed from other sets (i.e. by taking a cross-product of some other 2 sets).

At the same time, when presenting his 4 schemas with anomalies, Codd is
basically describing what we, programmers, know as data structure. He is pointing out a fatal flaw that lies in the
idea of subordination of one set to another: by introducing an access path dependence into your data model,
you lose the ability to represent conditions when they lie outside of your world view. It is hard to
overestimate the importance of this idea.

So it would be very nice to apply the relational model to all config files and to the actual programs as well.
Because the concept of anomalies is fundamental, if anomalies cause bugs when modeling projects and parts, 
they also cause bugs in configs, and bugs in programs. We are pretty much forced into it.. But can it be done?

OpenACR says that yes, not only can it be done, but you get an insanely extensible, compact system with almost
 magical properties. Let's begin by building this same schema using `acr`.
 
### Creating Some Tuples

The first thing we want is a way to move database records down to the level of text lines, so we can work with 
them outside of MariaDB or any other database. Perl and sed and grep are great tools, and so is git. 
We want to keep records in plain text files, version them, and treat them like source code. 
We'll need a tool that manipulates these text records and does various useful things with them.

We introduce the idea of a super-simple tuple, or ssim tuple, that consists
of key-value pairs on a single line and a type tag.

    test.project  project:project1

Let's look at the first word, `test.project`. This is the tuple head, or type tag.
If you think of a shell command line, it's the name of the command being invoked. 
But here, `test` is the database name, and `project` is the table name. In ssim, there is no 
concept of a database, there is only namespace, or ns. So `test` is the namespace. 

We are going to keep these, and only these records in the file

    data/test/project.ssim
    
Similarly, we will be keeping parts and projparts in files

    data/test/part.ssim
    data/test/projpart.ssim

And so we are going to place the following text lines in the corresponding files.

    $ cat > data/test/project.ssim << EOF
    test.project  project:project1
    test.project  project:project2
    EOF
    
    $ cat > data/test/part.ssim << EOF
    test.part  part:part1
    test.part  part:part2
    EOF

    $ cat > data/test/partproj.ssim << EOF
    test.partproj  part:part1  project:project1
    test.partproj  part:part2  project:project1
    EOF

And we're done.

### Describing The Tuple Schema

Now here comes the interesting part.

If we want our query tool to find this data, we would need to specify exactly 
which ssimfiles and namespaces we want in our system. We also need a description 
of the various columns, and what they refer to.

We will place the list of ssimfiles in the ssimfile called `data/dmmeta/ssimfile.ssim` 
and in it, by convention, each tuple will start work the word `ssimfile`. 
The word `dmmeta` is short for *Data Model Meta-information*
While we're at it, we will include the `ssimfile` record as well.
The entries will be sorted alphabetically.

    $ cat > data/dmmeta/ssimfile.ssim << EOF
    dmmeta.ssimfile  ssimfile:dmmeta.ssimfile
    dmmeta.ssimfile  ssimfile:test.part
    dmmeta.ssimfile  ssimfile:test.partproj
    dmmeta.ssimfile  ssimfile:test.project
    EOF


Similarly, we will place the list of known namespaces in the
table `data/dmmeta/ns.ssim` and these will all be tagged as `dmmeta.ns`.
But because we already have a namespace `dmmeta`, we will include its definition.

    $ cat > data/dmmeta/ns.ssim << EOF
    dmmeta.ns  ns:dmmeta
    dmmeta.ns  ns:test
    EOF

It things get even more interesting.

Since we want to write a C++ programs with these tuples, we will need a name for the type 
to use in C++. In addition, we anticipate having C types that have no associated ssimfiles.

Using the CamelCase convention for C++ types, we create a new table `dmmeta.ctype`.
We must include ctypes for all ctypes mentioned so far, this includes `ns` and `ctype`.

    $ cat > data/dmmeta/ctype.ssim << EOF
    dmmeta.ctype  ctype:dmmeta.Ctype
    dmmeta.ctype  ctype:dmmeta.Ns 
    dmmeta.ctype  ctype:test.Part
    dmmeta.ctype  ctype:test.Partproj 
    dmmeta.ctype  ctype:test.Project
    EOF

We then modify the ssimfile table so that each ssimfile has related ctype.
This is analogous to the `partproj` relation. 

    $ cat > data/dmmeta/ssimfile.ssim << EOF
    dmmeta.ssimfile  ssimfile:dmmeta.ssimfile ctype:dmmeta.Ssimfile
    dmmeta.ssimfile  ssimfile:test.part       ctype:test.Part
    dmmeta.ssimfile  ssimfile:test.partproj   ctype:test.Partproj
    dmmeta.ssimfile  ssimfile:test.project    ctype:test.Project
    EOF

We decide that it is not necessary to align the columns in a ssimfile, but it looks nicer.
Also, leading and trailing whitespace on each line will be ignored. And the order of the lines 
shouldn't matter either. 

And now the only thing that's missing is some description of the columns.

### Describing The Columns

We will need a new table, with one record for every type of column we have used so far.
We will call it `dmmeta.field`, so now we need to add `ssimfile` and `ctype` records to 
the appropriate ssmifiles:

    $ echo 'dmmeta.ctype  ctype:dmmeta.Field' >> data/dmmeta/ctype.ssim
    $ echo 'dmmeta.ssimfile  ssimfile:dmmeta.field  ctype:dmmeta.Field' >> data/dmmeta/ctype.ssim

The primary key for a field will be formed by concatenating the parent `ctype` with the 
field name, with a dot in between, e.g. `dmmeta.Ns.ns` will be the name of the field
describing the primary key of `ns`. We decide that whenever we need to look for a primary
key, we will just take the first field from the relevant ctype. This convention means that
we don't have to annotate the fields as being primary keys or not.

In addition, as we describe the field, we will need to address the concept of a constraint.
In our schema, there are two types of fields: a value type, and a reference to a primary key
of another table. We will call these `reftypes`. We can easily enter the description of the 
reftypes, and some reftype records, using the tools we have so far:

    $ echo 'dmmeta.ctype  ctype:dmmeta.Reftype ' >> data/dmmeta/ctype.ssim
    $ echo 'dmmeta.ctype  ctype:dmmeta.Reftype ' >> data/dmmeta/ctype.ssim
    $ cat > data/dmmeta/reftype.ssim <<EOF
    dmmeta.reftype  reftype:Val
    dmmeta.reftype  reftype:Pkey
    EOF
    
In order to describe value types, we will add a single type called `algo.Smallstr50`.
This will be our equivalent of varchar(50).
We create a new namespace called `algo`, because we want the `dmmeta`
namespace to be used exclusively for ctypes that have ssimfiles associated with them.

    $ echo 'dmmeta.ns ns:algo' >> data/dmmeta/ns.ssim
    $ echo 'dmmeta.ctype ctype:algo.Smallstr50' >> data/dmmeta/ctype.ssim

And now we are ready to describe the fields. We include descriptions of all known fields.

    $ cat > data/dmmeta/field.ssim << EOF
    dmmeta.field  field:dmmeta.Ctype.ctype        arg:algo.Smallstr50  reftype:Val 
    dmmeta.field  field:dmmeta.Field.arg          arg:dmmeta.Ctype     reftype:Pkey
    dmmeta.field  field:dmmeta.Field.field        arg:algo.Smallstr50  reftype:Val 
    dmmeta.field  field:dmmeta.Field.reftype      arg:dmmeta.Reftype  reftype:Pkey
    dmmeta.field  field:dmmeta.Ns.ns              arg:algo.Smallstr50  reftype:Val 
    dmmeta.field  field:dmmeta.Reftype.reftype    arg.algo.Smallstr50  reftype:Val
    dmmeta.field  field:dmmeta.Ssimfile.ctype     arg:dmmeta.Ctype     reftype:Pkey
    dmmeta.field  field:dmmeta.Ssimfile.ssimfile  arg:algo.Smallstr50  reftype:Val 
    dmmeta.field  field:test.Part.part            arg:algo.Smallstr50  reftype:Val
    dmmeta.field  field:test.Partproj.part        arg:test.Part        reftype:Pkey
    dmmeta.field  field:test.Partproj.project     arg:test.Project     reftype:Pkey
    dmmeta.field  field:test.Project.project      arg:algo.Smallstr50  reftype:Val
    EOF
    
If your head is not at least a little bit exploding when you see 
the line:

    dmmeta.field  field:dmmeta.Field.reftype      arg:dmmeta.Reftype  reftype:Pkey

Or the line

    dmmeta.field  field:dmmeta.Field.field        arg:algo.Smallstr50  reftype:Val 

Then you need to read the above text again. 
All of our moves so far have been forced. We are simply adding descriptions
of things as they pop up.

### Adding Tools

At this point, it is clear what kinds of actions we are performing on these data sets.
So we need to introduce some tools.

First, we need a query tool that can fetch records of a given type, and then find all related
records as a transitive closure over the Pkey references. Because the tool will be 
automatically cross-references all the tables in our system, we will call it `Auto Cross Reference`
or `acr`. We will equip `acr` with options like `-check` so that it can find any broken
Pkey references, and other options to make it useful.

Second, we will need a tool that takes some command lines and prints shell scripts of the 
sort we used above, which we can then pipe through `sh` in order to make the edits. 
We will call this tool `acr_ed`, for `editor`.

Third, since we already have all the descriptions of our records nicely arranged in tables,
we don't want to repeat ourselves. We actually want to generate the C++ structs, with fields and 
everything, by reading the `ctype`, `field` and `ns` tables, and creating C++ headers and source
files with correspondin structs. Then, as we load these tables, we will map them onto the structs
and everything will be nice and strictly typed. This model compiler will be called `amc`, or 
`A Model Compiler`. 

In order to compile all these tools, we will need `A Build Tool`, or `abt`. A regular Makefile-based
systrem won't do, because we already have a lot of data in `ssimfiles`, and we have the `ctypes` to handle
them in C++. We would be introducing anomalies if we were to go outside this model.

### Describing The Tools

Any running program is nothing more than a temporary database containing some number of tables.
At any given time, the program performs an instruction that either reads some field of some 
row from some in-memory table, or writes some field of some row to some in-memory table.

And so we can use the `ns` table to describe all the tools that we plan to introduce:

    $ cat >> data/dmmeta/ns.ssim << EOF
    dmmeta.ns ns:abt
    dmmeta.ns ns:amc
    dmmeta.ns ns:acr
    dmmeta.ns ns:acr_ed
    EOF

Now comes the tricky part: we want `amc` to generate the ctype called `Ctype` in namespace `dmmeta`,
by virtue of loading the `dmmeta.ctype  ctype:dmmeta.Ctype` record. There is a real dependency here,
of a different type than what we had with ssimfiles: with ssimfiles, even though they seemed to be
circular in their descriptions, they were not. The solution is to manually enter the definition of 
`dmmeta::Ctype` into `amc`'s source code, but only in order to read the `dmmeta.ctype` records into it.
After we do that, the output of amc will include `dmmeta::Ctype` simply because it loaded that record.
We then take the output of amc, and link it back to `amc`. At this point we will get a link error, 
since we already have the original, manually entered definition of `dmmeta::Ctype`. We can now delete
the manual version, making `amc`'s source code smaller. We repeat this many times, re-creating different
bits of `amc` carefully, always using `git commit` to checkpoint and rolling back when `amc` generates
code that no longer compiles.

Slowly but surely, we can cause about 95% of all the code we need to be generated from ssimfiles.
(The remaining 5% could also be generated `as-is`, just by `printf()`-ing the lines verbatim, but this 
would actually be counter-productive. It is only interesting to generate code that's used in more than one place.)

In subsequent chapters, I will descibre the `amc` memory model for an executable, and things like pools 
(for holding records) and x-refs (for creating group-by's and cross-references). 
The presence of `ssimfile` in our data set was dictated by the way we organized the data set.
The presence of `amc` in this model was dictated by our desire to use C++ to write the query tool `acr`.
There is a more general pattern here:

### Representation & Manipulation

This approach seems to scale indefinitely. We can represent any semantic concept as 
tuples, which are points in a multi-dimensional sparse space.
Whenever we extend our representable universe with
additional concepts, such as a physcial set of C++ files on disk, or a set of dev and production environments
with some configuration, we include its relational description, or map, as a set of tables in our data set. 
We then write tools that enforce a unidirectional or bidirectional correspondence between this new object and
the records that describe it. 

## Ssim files

Ssim is a super-simple line-oriented text format for
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

Leading and trailing whitespace is ignored, and may be used to aid legibility.
(For instance, it could be used to create a tree-like structure)

Both keys and values may be arbitrary byte sequences. A string
containing non-obvious characters and be enclosed either with single
or double quotes (there being no difference between these types of quotes),
and inside the quotes, C++ string rules exactly apply. So "\t":"\001" is a valid
key-value pair.

In database terms, a ssimfile maps directly to a table, 
and each line corresponds to a record.

### Ssim Data Sets

A ssimfile could be a part of a data set, or it could be a stand-alone file.
Both are called `ssimfiles` and use the extension `.ssim`.
There is one data set in this project, it is in the directory "data". 
In it, there is one directory per namespace, and one file per ssimfile.

In this data set, there is both data and meta-data. Meta-data is in the directory
`data/dmmeta`, where `dmmeta` stands for "data model meta". 

The list of all ssim files is provided by "acr ssimfile".
The list of all attrbitutes is provided by "acr field"

Ssim tuples can also be stored together in a file. Acr can read and write those
tuples. One can also use grep, sed, awk, and other line-oriented tools to access, edit,
and mutilate these records.

All amc-generated programs support the `-in` argument which specifies the input
data set for the tool -- either a file or a directory. By default it's `"data"`.

Acr additionally supports the `-schema` argument, specify the location from which it 
loads the schema. But default it's also `"data"`. This allows having many data-oriented
data sets which all use the same schema, and keeping the schema in the default location.

### Structured Key Normal Form

There is a lot of literature on how to construct database schema so that it doesn't
have anomalies, and how to create primary keys. There are 6 or 7 'normal forms' -- invariants
that have to hold if you want certain anomalies to be absent from your schema.
Anomalies are logical inconsistencies that arise as a result of operations such as join, update, insert
or delete.

Here I will describe the Structured Key Normal Form, or SKNF, which all OpenACR
schemas use. SKNF has no anomalies by construction and requires only one normal form.
All it boils down to is this:
a single field, the first field of a table, is the primary key, and it is either a simple type,
or a cross product of two other keys (which is the same thing if you allow for an empty set).

All other columns are non-NULL, and are also either simple types, or must refer to a key in some other table.
When the primary key is a cross-product of two other sets, for instance dmmeta.Ctype, 
where `dmmeta` refers to `ns:dmmeta` and ctype Ctype is a string, we use a separator, in this case '.'.

To decompose a domain into ssimfiles, perform cardinality analysis, meaning that you break the domain up
into sets, where each set has 1 or more values attached to a key, and the key is a structured
one as described above. 

If you need some column to be NULLable, there is no action available. Instead, delete
the column, and create a new table which is a subset of the original table.
Deleting the rows from this new table is equivalent to NULLing the original fields.

There are no constraints other than foreign key constraints in ssim databases,
and since they are kept in text files, the only storage type is string (for acr, all values
are just strings).

### Decomposing A Domain Into Sets

Let's consider the domain of programs, such as the one found here in OpenACR.
Since we need to attach various properties to these programs in order to do stuff with them,
we create a number of tables to describe them.

First, we have the set of all binaries. We can call it `target`, meaning "build target". 
Then, we have the set of all source files; We'll call it `gitfile`.
Notice that when naming a set, we don't use plurals. In OpenACR, we always use singular
when describing a set; (There is simply no benefit to using plurals when naming things).
Finally, to specify that a source file belongs to some target, we create a table `targsrc`
as a cross product of `target` and `gitfile`.
We make `target` a subset of `ns`, since all targets have namespaces describing them,
but not all namespaces become build targets (i.e. `command` doesn't have a target).

The resulting schema is shown below with `amc_vis`. Notice that all arrows point left.
This is very important. Left-pointing arrows are *references*, and a database without indexes 
consists only of references. 

    $ amc_vis dev.Target\|dmmeta.Ns\|dev.Targsrc\|dev.Gitfile

                    / dev.Gitfile
                    |
                / dmmeta.Ns
                |   |
                |   |              / dev.Targsrc
                |   |<-------------|Pkey src
                |   -              |
                |                  |
                |                  |
                |   / dev.Target   |
                |<--|Pkey target   |
                -   |              |
                    |              |
                    |<-------------|Pkey target
                    |              -
                    |
                    -

When we need to quickly answer the question
"which records point to this record?" do we introduce right-pointing *cross-references*, which are
computed from references. Here is an example of an in-memory database built specifically
for abt in accordance with the above schema.

    amc_vis abt.FDb\|abt.FTarget\|abt.FTargsrc

    / abt.FDb
    |
    |Lary targsrc---------------------------------->/ abt.FTargsrc
    |Thash ind_targsrc----------------------------->|
    |                                               |
    |Lary target--------------->/ abt.FTarget       |
    |Thash ind_target---------->|                   |
    |Llist zs_sel_target------->|                   |
    |Llist zsl_libdep_visited-->|                   |
    |Llist zsl_libdep---------->|                   |
    |Llist zs_origsel_target--->|                   |
    -                           |                   |
                                |                   |
                                |Ptrary c_targsrc-->|
                                |<------------------|Upptr p_target
                                |                   -
                                |
                                -

### History of Database Design

In his seminal paper (https://www.seas.upenn.edu/~zives/03f/cis550/codd.pdf)
Codd explained how to decompose a *data structure* into a *set of tables* and *access paths*,
where each access path is also a table.
This was a crucial insight. Attaching an access path to a data set and calling
it a data structure results in anomalies which lie at the root of many, many bugs,
and so must be avoided. An anomaly is basically anything that dangles. 
Any value that was supposed to be updated but wasn't is an anomaly. Anomalies are just bugs.

For the first time, way after languages
such as Algol were designed and published, the world of software construction
acquired one of the most important tools -- the factoring of a state space into a
set of sets, and a set of access paths between them. This decomposition is similar 
in importance, and in fact identical in princple, to the first successful axiomatization
of mathematics, the Zermelo–Fraenkel set theory.
Data structure was replaced by a set and an access path, and the access path
is itself a set.

Whenever we talk of "linked list of X" or "hash table of X", we are introducing
invisible anomalies by presupposing that all X are reachable via said access path.
To avoid this anomaly, we need to first think of X as a thing in itself, and only attach 
indexes later.

### Cardinality Analysis

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

So, surrogate keys don't solve the problem of constructing the set. What does?

Cardinality analysis does. The cardinality of each set is either either an integer, a string,
same as that of another set (i.e. a subset), or a cross product of two sets.

Decomposing your domain into sets based on the cardinality alone has the property of being
replicable -- if two people go into separate rooms and each design a schema for the same domain,
they will arrive at the same decomposition module spelling of names. This is important 
for collaborations as it simplifies merging.

### The Curse Of Simple Domains

Codd was much of in favor of simple domains, where each column (field of a struct)
is either an integer or a string, and the primary key is described as a concatenation
of several such fields. This principle is not scalable, because if you use simple domains,
you get very complicated joins which are very sensitive to all layers of the schema.
And it's violated by SQL's timestamp type itself, which is a complex domain composed of year,
month, day, hour, minute, second and nanosecond. If we followed Codd blindly, we'd use 7
fields every time we needed a timestamp.

### Remember 4-valued logic?

Simple domains weren't the only thing that Codd strongly advocated for.
He was also in favor of the 4-valued boolean logic, where the result of any expression has
"yes", "no", "NULL and doesn't matter", and "NULL and it does matter". He had a lot
of trouble convincing people to implement this 4-valued logic, which was necessary
for logical consistency in presence of NULLable columns. Codd was right. If you have NULLs
and don't use 4-valued logic, you have consistency issues. But 50 years later,
we have some hindsight: why not just throw away NULLs?
When you get rid of NULLs, you are naturally pushed toward columnar storage, 
since you still need to support missing values at various stages of your data set lifetime, 
but your missing values simply become missing rows.

A NULL is nothing more than a missing join!

### Structured Key Normal Form

If it weren't for the simple domain rule and NULLs, the proliferation of normal forms
wouldn't exist. 

And so SKNF represents a very simple but stable point in the space of
all possible schemas of schemas, where you don't have NULLs and every key is
just a single composite value. It scales indefinitely, and every join takes just 2 values.
It's guessable and easy to remember.

SKNF is not a name recognized in the industry. Perhaps in the clade of DBMS construction philosophies,
the closest analog would be DKNF (Domain Key Normal Form).

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

    'mdbg acr' debugs acr
    'mdbg mdbg' debugs the debugger
    
And of course, the point of the tools is not to compile themselves; 
The idea is that this repo is extended with new commands and tools specific to some project,
maintaining the same directory structure and basic conventions.

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

## acr: Auto Cross Reference

Acr is a query and editing tool for ssim (super-simple) databases.
These databases can be proper directories, or files, or stdin.
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

#### The -replace option

When `-replace` is specified, each new tuple completely overrides the previous tuple. 
Any fields that aren't specified are assigned default values. Let's illustrate by first
creating a temporary table:

    $ acr_ed -create -ssimfile dev.t -write
    ...
    $ acr_ed -create -field dev.T.val -arg u32 -write
    ...
    $ echo 'dev.t t:ggg val:3' | acr -insert -write
    acr.insert  dev.t  t:ggg  val:3  comment:""
    ...
    
Now let's try to insert another record with the value `ggg`:

    $ echo 'dev.t t:ggg' | acr -insert -write
    acr.duplicate_key  key:dev.T:ggg

That failed. Now let's replace instead. This will succeed, and the value `val`
will go back to the default:

    $ echo 'dev.t t:ggg' | acr -replace -write
    ...
    $ acr t
    dev.t  t:ggg  val:0  comment:""
    
#### The -merge option

The `-merge` option is like replace on a per-field basis. Let's illustrate
using the same `t` table. We'll need another column, call it `val2`, while 
set set `val` back to 3:

    $ acr_ed -create -field dev.T.val2 -arg u32 -write
    $ echo 'dev.t t:ggg val:3' | acr -replace -write
    ...
    $ acr t
    dev.t  t:ggg  val:3  val2:0   comment:""
    
So far so good. Now let's use -merge:

    $ echo 'dev.t t:ggg val2:4' | acr -merge -write
    ...
    $ acr t
    dev.t  t:ggg  val:3  val2:4  comment:""

#### The -trunc option

With this option, when the first change is made to the table, the table is first wiped.

    $ echo 'dev.t t:hhh' | acr -insert -trunc -write
    ...
    $ acr t
    dev.t  t:hhh  val:0  val2:0  comment:""


### Generating Shell Scripts

Finally, the `-cmd` option produces an executable shell script which can be piped to `sh`. 
For each record in the final selection, acr outputs variable assignment statements, giving the
shell script access to the values of all field attributes, the fldfuncs, the tuple itsef (`acr_tuple`)
the type tag (`acr_head`) and the rowid (`acr_rowid`). The script can then use whatever 
other Unix tools it needs to.

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

The beauty of `-cmd` is that it only creates one subprocess, the target shell. Executing
one command per output row would have been prohibitively expensive.

### Deleting Records From Stdin

The options `-insert`, `-replace`, `-merge` all enable reading of stdin
for a list of tuples. This input stream can contain special lines.

Whenever a line starts with `acr.delete`, the corresponding record is deleted.

Whenever a line starts with `acr.insert`, the corresponding record is inserted (subject to replace/merge semantics).

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

#### The -before option

When creating a new field with `acr_ed`, it can be inserted before another field
by specifying `-before <field>`. Alternatively, you can always call up `acr -t -e ctype:<ctype>`
and make the adjustment by hand, or just go into the `data/dmmeta/field.ssim` table
and swap two lines in a text editor. These are all equally valid approaches.

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

### Deleting Records

What about deleting records? Let's delete something.

    $ acr a:a4 -del -write
    acr.delete  dev.a  a:a4  b:0  comment:""

    acr.delete  dev.b  a:a4  comment:""
    report.acr  n_select:2  n_insert:0  n_delete:2  n_update:0  n_file_mod:1

As you can see, acr followed the reference and deleted the offending b record as well as the
matching a record.

What if we wanted to see what `-del` does without modifying the database?
We could omit the `-write` option. Notice that acr prints the records that *would*
be deleted, but in the final report, `n_file_mod:0` so we know that nothing was written back.

    $ acr a:a5 -del
    acr.delete  dev.a  a:a5  b:0  comment:""

    acr.delete  dev.b  a:a5  comment:""
    report.acr  n_select:2  n_insert:0  n_delete:2  n_update:0  n_file_mod:0

`-del` works with any number of records. You could delete the entire database with
`acr % -del -write`.

When deleting a field, acr automatically opens and rewrites the corresponding ssimfile.

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
virtually impossible to interactively build a nice data set without it being `broken` at
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
included in the output, but didn't become part of the tree, because only the leftmost
parent is the preferred one.

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
on showing the definitions of the types `dev.A`, `dev.B`, and `dev.C` is that 
this tutorial is about creating tables and records in these tables,
and type definitions are exactly that -- they're just records inserted by `acr_ed`
into some appropriate tables using `acr -insert -write`.
So it would have been circular to start with them, and I had to hide them.
Seemingly circular definitions occur all the time in acr.
Thankfully, `acr -t` discovers all forward and back references and shows their relationships:

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

The expression such as '.RL' is not limited to 3 characters. 

The substr expression consists of triples of characters, where the first character is
what to look for in the string (in this case '.'), the second character is where to start (in this case
start at the right side of the string, 'R'), and the third character specifies what part of the
string to take after the match is found, or the search stops. If the match is not found, search stops
at the opposite end of the string. So, the expression .RL means "Scan from the right until you find the
character or hit the left side of the string. Then, return everything to the right of the current position".
These triples can be repeated as many times as necessary to "bite off" pieces of the string, for example
.RL.RL removes the 2 trailing dots from a string. The corresponding C++ function that implements this
search is called algo::Pathcomp.

The funciton that parses these expressions is called Pathcomp, and we can easily view its source code:

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

### Sorting & RowIDs

Acr always saves files in sorted order. Sorting is controlled by the `ssimsort`
table, which is a subset of `ssimfile`. Sorting is optional. If `ssimsort` is missing,
the set is *order-dependent*. When sorting is enabled, it can be done on any fields,
including a fldfunc.

Let's take another look at our color table `c`:

    $ acr c
    dev.c  c:blue   comment:""
    dev.c  c:green  comment:""
    dev.c  c:red    comment:""
    report.acr  n_select:3  n_insert:0  n_delete:0  n_update:0  n_file_mod:0

The colors are sorted by name. Let's look at the ctype:

    $ acr ctype:dev.C -t
    dmmeta.ctype  ctype:dev.C  comment:""
      dmmeta.field  field:dev.C.c        arg:algo.Smallstr50  reftype:Val  dflt:""  comment:""
      dmmeta.field  field:dev.C.comment  arg:algo.Comment     reftype:Val  dflt:""  comment:""
      dmmeta.cfmt  cfmt:dev.C.String  printfmt:Tuple  read:Y  print:Y  sep:""  genop:Y  comment:""
      dmmeta.ctypelen  ctype:dev.C  len:204  alignment:1  padbytes:0

    dmmeta.ssimfile  ssimfile:dev.c  ctype:dev.C
      dmmeta.ssimsort  ssimfile:dev.c  sortfld:dev.C.c  comment:""

Now let's delete the ssimsort line.

    $ acr ssimsort:dev.c -del -write
    ...

If we now use `acr -e` to open `c` in an editor, we will see that each line now has an additional
attribute called `acr.rowid`. This is attribute ensures that the line will be saved back to the 
file in the same position.

    $ acr c -e
    <opens EDITOR>
    dev.c  c:blue   acr.rowid:0       comment:""
    dev.c  c:green  acr.rowid:1       comment:""
    dev.c  c:red    acr.rowid:2       comment:""

If we swap the lines for `blue` and `green`, and exit from the editor, no change will 
occur on disk. That's because the order of the records is not determined by their visual order
on the screen. It's determined by the rowid. To swap two records, we would need to swap their rowids.
The rowids are temporary, and exist only for the duration of the editing session. They are not 
saved to the ssimfile.

The type of this rowid is conveniently a `float`, which allows you to insert new lines at fractional positions,
something like this:

    dev.c  c:cyan  acr.rowid:0.5 ...
    ...
    
The very important `dmmeta.field` table is partially positional. It is sorted on the `ctype`
attribute, but not sorted by name. That's because the user needs to control the order of fields
in a structure. Most other ssimfiles are sorted on the primary key.

### Creating A New Data Set

So far, we have been using the default data set called `data`. Acr is not limited to it.
We can work with tuples in any data set, and that data set can be either a directory or a file.

To create a new directory-based data set, we just create an empty directory and then use the `-in`
option:

    $ mkdir x
    $ acr dmmeta.% | acr -in:x -insert -write -print:N
    ...
    $ ls -l x
    total 8
    drwxrwxr-x. 2 alexei alexei 4096 May  2 17:29 dmmeta

All records have now been copiled from `data` to `in`.
As long as we specify the `-in` option, acr will read and write from the specified directory.

### Using A File As A Data Set

We can do the same thing with a file

    $ rm -r x
    $ touch x
    $ acr dmmeta.% | acr -in:x -insert -write -print:N
    ...
    $ ls -l x
    -rw-rw-r--. 1 alexei alexei 735004 May  2 18:00 x

File `x` now contains ssim tuples, one per line, and it can be queried or edited
just like any other data set. Just by dumping some tuples in a file, we get a data set.

### The -schema option

Acr can use and query standard input just like it can a regular file.

    $ echo 'dmmeta.ctype ctype:xyz' | acr -in:- ctype:%
    dmmeta.ctype  ctype:xyz  comment:""

This shows a couple of interesting things. First, acr always displays tuples 
according to the known schema. When printing a tuple, `acr` loops over the 
non-fldfunc fields of the associated `ctype`, and evaluates each field in the context
of the given tuple. This means that acr normalizes data whenever the data is printed.

Second, you might wonder, how does `acr` know that `dmmeta.ctype` has an attribute 
`comment` under these conditions? Well, as it turns out, this is a common problem.
If `acr` loaded its meta-information from the same place it loaded the data, it would
be supremely inconvenient. That's why an additional attribute called `-schema`,
which is equal to `"data"` by default, specifies where to load the schema information.
If we pass an empty data set as schema, we expect nothing to be printed:

    $ rm -r x; touch x  # create a file
    $ echo 'dmmeta.ctype ctype:xyz' | acr -in:- ctype:% -schema:x
    report.acr  n_select:0  n_insert:0  n_delete:0  n_update:0  n_file_mod:0

Sure enough, acr doesn't recognize the input as a valid tuple, so the line is silently
skipped.

### The -meta option

Instead of writing `acr ctype:dev.C -t` we can write

    $ acr c -meta
    dmmeta.ctype  ctype:dev.C  comment:""
      dmmeta.field  field:dev.C.c        arg:algo.Smallstr50  reftype:Val  dflt:""  comment:""
      dmmeta.field  field:dev.C.comment  arg:algo.Comment     reftype:Val  dflt:""  comment:""
      dmmeta.cfmt  cfmt:dev.C.String  printfmt:Tuple  read:Y  print:Y  sep:""  genop:Y  comment:""
      dmmeta.ctypelen  ctype:dev.C  len:204  alignment:1  padbytes:0
        
The `-meta` option looks at the selected records, and de-selects them, and selects their
ctypes instead. `-meta` implies `-t`.

### The -cmt option

The -cmt option displays any comments associated with the current selection. Let's document the dev.C.c
field so that every time `-cmt` is used (and it's on by default whenever `-e` is specified), the 
users gets some help:

    $ echo 'dmmeta.field field:dev.C.c comment:"Name of the color (primary key)"' | acr -merge -write
    $ acr c -cmt
    dev.c  c:blue   comment:""
    dev.c  c:green  comment:""
    dev.c  c:red    comment:""
    
    # Field    Arg              Reftype  Comment
    # dev.C.c  algo.Smallstr50  Val      Name of the color (primary key)

### MySQL Integration

The option -my launches an instance of mariadb and imports all namespaces that cover selected records
as databases into mariadb. It then drops you into a mariadb shell, where you can apply needed transformations.
Upon exit, the data is downloaded from the database back to disk. -my implies -write.

## Command Lines

All programs created with amc come with full command line support. A command line is just a struct,
and options are described as fields. All one needs to do is keep adding fields, and amc and the rest
of programs will take care of all chores.

To start this tutorial, let's start with a fresh commit, and create a new executable.

    $ acr_ed -create -target abc -write
    ...

This will create a new program called `abc`:

    $ abc
    Hello, World!

The program already comes with some built-in options.

    $ abc -h


    Usage: abc [options]
        -in       string  Input directory or filename, - for stdin. default: "data"
        -verbose          Enable verbose mode
        -debug            Enable debug mode
        -version          Show version information
        -sig              Print SHA1 signatures for dispatches
        -help             Print this screen and exit

Amc knows that the ctype `command.abc` is the command line for abc because of the `fcmdline`
record inserted by `acr_ed`.

    $ acr fcmdline:abc.%
    dmmeta.fcmdline  field:abc.FDb.cmdline  read:Y  comment:""

The command line itself, as created by acr_ed, is a simple ctype:

    dmmeta.ctype  ctype:command.abc  comment:""
      dmmeta.field  field:command.acr.in  arg:algo.cstring  reftype:Val  dflt:'"data"'  comment:"Input directory or filename, - for stdin"
                  
Let's do something func. First, let's add a flag. Because what kind of command doesn't have a flag?

    $ acr_ed -create -field command.abc.flag -arg bool -write -comment "An important flag"
    ...
    $ ai abc
    ...
    $ abc -h

    Usage: abc [options]
        -in       string  Input directory or filename, - for stdin. default: "data"
        -flag             An important flag. default: false
        -verbose          Enable verbose mode
        -debug            Enable debug mode
        -version          Show version information
        -sig              Print SHA1 signatures for dispatches
        -help             Print this screen and exit

As you can see, the help screen has been updated. Let's modify `abc`'s main to print the flag.
It is an OpenACR convention to avoid displaying raw values. Output should be machine
readable and never susceptible to an injection attack. So we'll print the value
as a key-value pair (`Keyval` is a C++ template helper):

    void abc::Main() {
        prlog(Keyval("flag",_db.cmdline.flag));
    }

    $ ai abc
    ...
    $ abc -flag
    flag:Y

### Boolean Options

There are several ways to specify a flag on a command line: `abc -flag`, `abc -flag:Y`,
`abc -flag:true` as well as `-flag:yes` and `-flag:on` all work.
To specify a false value, we could either omit the option
altogether, or write `abc -flag:N` or `abc -flag:false` or `-flag:no` or `-flag:off`.
The OpenACR convention is to
print booleans in a datatabase-friendly way, using `Y` and `N`.

If a boolean option is specified more than one on the command line, the value
in memory is incremented by one each time. This allows counting how many times the value
was specified, which is useful for controlling verbosity and debug levels.

There is no bundling of options. Each option must be specified as a separate 
argument.

### Default Values

If the `dflt` attribute of the field describing a command line argumentn is non-empty,
that option may be omitted. Otherwise it is required.
Boolean options can always be omitted (i.e. omitting a boolean option, a.k.a. a flag
from the command line is never an error).

Here is an example of the default value, in this case `"data"`:

    $ acr field:command.%.in | head -1
    dmmeta.field  field:command.acr.in  arg:algo.cstring  reftype:Val  dflt:'"data"'  comment:"Input directory or filename, - for stdin"


### Integer Options

Let's continue. It's time to add an integer-valued option:

    $ acr_ed -create -field command.abc.val -dflt 0 -arg u32 -write -comment "A value"
    ...
    $ ai abc

Let's update the source code to print the value:

    void abc::Main() {
        prlog(Keyval("flag",_db.cmdline.flag));
        prlog(Keyval("val",_db.cmdline.val));
    }

    ...
    $ abc -val:4
    flag:N
    val:4

When parsing the command line, space after option and `:` are treated interchangeably:
`-val:4` is the same as `-val value`.

It may be a stylistic point, but when the option and its value form one word, it's clear at the syntax level,
without knowing what the command expects, that `value` is not a stand-alone option or a positional argument.
It can make error messages more legible when things go wrong.

### Anonymous Options

Let's say we want abc to be able to write `abc 4` without specifying the parameter name `-val`.
We can use `anonfld` for this. We add the appropriate record to the anonfld table, then re-generate
and rebuild.

    $ echo dmmeta.anonfld field:command.abc.val | acr -insert -write
    $ amc && abt -install abc
    $ abc 5
    flag:N
    val:5

### Other Data Types

A command line option can be of any data type whatsoever. Any type that can be read from 
a string is a valid type. For instance, to input an IPv4 address, use

    $ acr_ed -create -field command.abc.ip -arg ietf.Ipv4 -comment "Enter this IP" -write

Reading ctypes from a string is controlled by the `dmmeta.cfmt` table, where a user-provided
function may be specified with `extrn:Y`. This is described in depth in the chapter on string conversion.
    
### Bash Command Completion

OpenACR offers completion of command line parameters for any program in its domain.
The magic is implemented in the command `acr_compl`. To use, we instruct bash
to call `acr_compl` whenever a command line start with one of known command names.
In this case, abc is now a known name, so `acr_compl` already knows about it:

    $ abt -install acr_compl
    $ acr_compl -install
    # This is a script suitable for inclusion in .bash_profile
    complete -o default -o nospace -C acr_compl abc abt acr acr_compl acr_ed acr_in acr_my amc ...
                                                ^^^ -- it knows!

Let's manually hook it up:

    complete -o default -o nospace -C acr_compl abc

And test it:

    $ abc -v<tab>
    -val:<int>  -verbose    -version

### Completing From Table

Auto-complete works with the contents of any table, loading it as necessary to provide completions.

    $ acr ctype:acr.FC<tab>
    acr.FCdflt    acr.FCheck    acr.FCppfunc  acr.FCtype

Let's test it on our program. We'll create and populate a brand new table, and add an option to abc
which will be completed with values from that table.

    $ acr_ed -create -ssimfile dmmeta.mytable -write
    $ for X in abcd{10..99}; do echo "dmmeta.mytable mytable:$X"; done | acr -insert -write
    ...
    $ acr mytable -print:N
    report.acr  n_select:90  n_insert:0  n_delete:0  n_update:0  n_file_mod:0

So far so good, we have 90 values in our table.
Now let's create a new command line option:

    $ acr_ed -create -field command.abc.mytable -arg dmmeta.Mytable -reftype Pkey -dflt '""' -comment "A very useful option" -write
    ...
    $ ai abc

And now we're ready to test it:

    $ abc -mytable:abcd2<tab>
    abcd20  abcd21  abcd22  abcd23  abcd24  abcd25  abcd26  abcd27  abcd28  abcd29

It works! Bash now magically knows to fetch values from `mytable` whenever `abc` is being invoked.
As expected, inside abc this value is accessible as _db.cmdline.mytable. It has the type `algo::Smallstr50`,
because that's the type of the primary key of `mytable`.

### Inputting A Table

Now that we have our `abc` program and we're taking a `mytable` option, it would be nice to be able
to do a lookup and find the appropriate record.

To do that, we first create a table in the `abc` namespace, which is based on the `dmmeta.mytable` table.

    $ acr_ed -create -finput -target abc -ssimfile dmmeta.mytable -indexed -write

We verify that there are two access paths from `abc` to `abc.FMytable`, one via the memory pool `abc.FDb.mytable`,
the other via hash table `abc.FDb.ind_mytable`:

    / abc.FDb
    |
    |Lary mytable------->/ abc.FMytable
    |Thash ind_mytable-->|
    -                    |
                         |
                         -

We modify `abc` as follows:

    void abc::Main() {
        abc::FMytable *mytable=ind_mytable_Find(_db.cmdline.mytable);
        prlog((mytable ? "Yes, record found" : "No such record"));
    }

Then build and run:

    $ abc -mytable:tt
    No such record
    $ abc -mytable:abcd51
    Yes, record found

### Regx Options

We will go into more detail into these operations later, but first let's convert `-mytable`
to a Regx option, and modify abc to scan the `_db.mytable` array instead of using the hash table,
and print all matching records:

    $ echo dmmeta.field field:command.abc.mytable reftype:RegxSql | acr -merge -write
    $ amc

    ...
    void abc::Main() {
        ind_beg(_db_mytable_curs,mytable,_db) {
            if (Regx_Match(_db.cmdline.mytable,mytable.mytable)) {
                prlog(mytable.mytable);
            }
        }ind_end;
    }

Now we run the new abc to check the result:

    $ abc -mytable:abcd5%
    abcd50
    abcd51
    abcd52
    abcd53
    abcd54
    abcd55
    abcd56
    abcd57
    abcd58
    abcd59

Yes, it finds and prints the keys of the records it found.

Notice that we no longer need the hash table `abc.FDb.ind_mytable`, since we 
converted from a hash lookup to a linear scan. Let's delete the hash table:

    $ acr_ed -del -field abc.FDb.ind_mytable -write
    $ amc_vis abc.%

    / abc.FDb
    |
    |Lary mytable-->/ abc.FMytable
    -               |
                    |
                    -

Indeed, only one access path remains.

### The -version flag

All commands compiled with abt contain a special string called `gitinfo`. abt populates it
with the build date, first 6 characters of the last known git commit, last git author's e-mail,
and current configuration string: compiler, compiler version, config (e.g. release), OS name (e.g. Linux)
and architecture string (e.g. x86). All this is intended to help with debugging.
This information can be retrived with `strings` or by running the command with `-version` argument:

    $ abc -version
    dev.gitinfo  gitinfo:2019-05-02.309c6ba  author:alexei@lebe.dev  cfg:g++/4.8.5/release.Linux-x86_64  package:""

    $ strings dflt.release-x86_64/abc | grep gitinfo:
    dev.gitinfo  gitinfo:2019-05-02.309c6ba  author:alexei@lebe.dev  cfg:g++/4.8.5/release.Linux-x86_64  package:""

~AL~: this is no longer valid as gitinfo support via loader mechanism was non-portable.
Will be re-implemented via `amc` in a portable way

### The -sig flag

For each target that inputs some tables, amc computes a signature hash of that program's inputs.
This allows detecting schema changes and avoiding incompatibilities that might be dangerous.
The `-sig` option prints the target's signatures. A signature can be viewed as a de-facto version.

### Printing Command Lines

Just like there is support for reading command lines, amc generates the necessary code to convert
a command line struct to a properly escaped Bash command.
Let's modify `abc`'s source as follows:

    void abc::Main() {
        prlog(abc_ToCmdline(_db.cmdline));
    }

Now let's run abc with some options:

    $ abc
    bin/abc  0

abc converts its command line back to a string that can be passed to bash, and any strings get
correctly quoted and escaped as necessary:

    $ abc -val:33 -mytable:$'\nblah'
    bin/abc  33 -mytable:$'\nblah'

Since `-val` is an anonymous field, `abc_ToCmdline` omits the name.

### Subprocesses With Command Line

Before we end this tutorial, let's show one more feature, namely invoking a subprocess
in a strictly typed way. `amc` generates a subprocess invocation helper `command::abc_proc`
which we can use to avoid calling system(). system() is undesirable, mainly because
it disables SIGINT and SIGTERM, making processes built around system() essentially unkillable
from the command line. `amc`s subprocess implementation doesn't capture interrupts, and
uses `DieWithParent` to make sure the child process is killed whenever the parent dies.

Modify the source code of abc.cpp as follows:

    #include "include/gen/command_gen.h"
    #include "include/gen/command_gen.inl.h"

    void abc::Main() {
        prlog(_db.cmdline.val);
        if (_db.cmdline.val>0) {
            command::abc_proc abc;
            abc.cmd.val = _db.cmdline.val-1;
            abc_Exec(abc);
        }
    }

This will recursively invoke the `abc` subprocess (don't try it with large values!)

    $ abc 3
    3
    2
    1
    0

Here is the code amc generated for `amc_proc`:

    $ amc command.abc_proc

    // --- command.abc_proc
    struct abc_proc { // command.abc_proc: Subprocess:
        algo::cstring   path;      //   "bin/abc"  path for executable
        command::abc    cmd;       // command line for child process
        algo::cstring   stdin;     // redirect for stdin
        algo::cstring   stdout;    // redirect for stdout
        algo::cstring   stderr;    // redirect for stderr
        pid_t           pid;       //   0  pid of running child process
        i32             timeout;   //   0  optional timeout for child process
        i32             status;    //   0  last exit status of child process
        abc_proc();
        ~abc_proc();
    private:
        // reftype of command.abc_proc.abc prohibits copy
        abc_proc(const abc_proc&){ /*disallow copy constructor */}
        void operator =(const abc_proc&){ /*disallow direct assignment */}
    };
    ...

### Verbosity

You may have noticed the use of `prlog` to print things. `prlog` is one of a few C++ macros in OpenACR.
(All macros are defined in [include/define.h]).
Along with `prlog`, there is `prerr` that prints to stderr, `verblog`, which prints only
if the command was invoked with `-v` or `-verbose`, and `dbglog`, which prints if the command was
invoked with `-d` or `-debug`.

Whenever a command line is converted to a string with the `_ToCmdline` function,
it inherits a lower verbosity level than the parent. This allows tracing the process tree
by using an appropriate number of `-v`'s on the command line.

Don't forget to `git reset --hard` to clean up any local changes.

## acr_in: Show input tuples for target

acr_in computes the names and the order of ssimfiles
which constitute target's declared input.

To illustrate, let's create a new program and make it read a table.

    $ acr_ed -create -target abc -write
    ...
    $ acr_ed -create -finput -target abc -ssimfile dmmeta.ctype -write
    ...
    $ acr finput:abc.%
    dmmeta.finput  field:abc.FDb.gitfile  extrn:N  update:N  strict:Y  comment:""
    ...
    
For this target, finput is `dmmeta.ctype`. Let's see if acr_in knows that:

    $ acr_in abc
    dmmeta.Dispsigcheck  dispsig:abc.Input  signature:f162f70f9895c41909c2192722172e6d21fe5679
    dmmeta.Dispsigcheck  dispsig:algo_lib.Input  signature:ddc07e859e7056e1a824df1ad0e6d08e12e89849
    dmmeta.ssimfile  ssimfile:dmmeta.ctype  ctype:dmmeta.Ctype

The output is the list of ssimfiles needed by the target, plus any signatures
used by the target. We can ignore the signatures for now, but they can be used 
to detect schema changes between the compiled version of a program and the version 
of data set on which `acr_in` operates. The `-sigcheck` option can be used to omit these.

Let's now add, as an `finput` for `abc`, the `dmmeta.ns` table, which is lexicographically
after `dmmeta.ctype`, but logically before (since ctype depends on ns).

    $ acr_ed -create -finput -target abc -ssimfile dmmeta.ns -write
    
    $ acr_in abc -sigcheck:N
    dmmeta.ssimfile  ssimfile:dmmeta.ns  ctype:dmmeta.Ns
    dmmeta.ssimfile  ssimfile:dmmeta.ctype  ctype:dmmeta.Ctype
    
We see that `acr_in` has printed ns and ctype in the order of Pkey dependencies between them.
`acr_in` operates recursively over all libraries that are target uses.

The order of ssimfiles is determined as a transitive closure on Pkey
references, and is independent of the target itself. This means that
`acr_in` can be called with an regex of target names (e.g. %), and the
resulting input can be fed into any one of the targets implied by the
regex, without error.

### The -data option

With -data argument, acr_in also loads the specified ssimfiles in memory
and prints out their contents.

This can be used to create canned input files:

    acr_in abc -data > tempfile
    abc -in:tempfile
    # this is exactly the same as running abc -in:data

### The -checkable option

If we take the data from ssmifiles `ns` and `ctype`, they are now sufficient
to serve as inputs to the newly created `abc`. However, `acr -check` will fail on
this resulting dataset, because `ns` also depends on `nstype`, and in general there
may be unresolved Pkey references in the resulting output.

To recursively include any dependent ssimfiles, specify `-checkable`:

    $ acr_in abc -checkable -sigcheck:N
    dmmeta.ssimfile  ssimfile:dmmeta.nstype  ctype:dmmeta.Nstype
    dmmeta.ssimfile  ssimfile:dmmeta.ns  ctype:dmmeta.Ns
    dmmeta.ssimfile  ssimfile:dmmeta.dispsig  ctype:dmmeta.Dispsig
    dmmeta.ssimfile  ssimfile:dmmeta.ctype  ctype:dmmeta.Ctype

### The -related option

`acr_in` can optionally include only those tuples which are transitively
reachable from a certain set. For this, specify `-related`.
Here is an example where we constrain `abc`'s input to the `abc` namespace itself.

    $ acr_in abc -data -related:dmmeta.ns:abc -sigcheck:N
    dmmeta.ns  ns:abc  nstype:exe  comment:""
    dmmeta.ctype  ctype:abc.FCtype  comment:""
    dmmeta.ctype  ctype:abc.FDb  comment:""
    dmmeta.ctype  ctype:abc.FGitfile  comment:""
    dmmeta.ctype  ctype:abc.FNs  comment:""

In contrast, if we didn't specify `-related`, `-data` would fetch all records:

    $ acr_in abc -data | wc -l
    864

## abt: A Build Tool

Abt is a build tool. The argument to abt is a target name regex.
Here, target means 'build target'.

The ssimfiles `abt` reads describe which targets can be built,
what are the sources files that comprise the targets, and the various options to use.

Abt outputs are organized under the `build/` directory.
`abt` supports multiple compilers and architectures within the same directory.

When building, `abt` builds a dependency dag based on #includes; 
Invokes build commands, keeping up to N of them running at a time.
The simplest way to build everything is:

    $ abt %

Here we assume that `abt` itself has already been built with `ai`. For more information,
see the [Bootstrap](#Bootstrap) section.

Let's begin by creating a new target.

    $ acr_ed -create -target sample -write
    ...
    
This creates and builds `sample.` Let's clean sample. This cleans both the target
and all of the libraries that it might use. 

    $ abt -clean sample
    abt.config  config:Linux-clang++.release-x86_64  cache:none  out_dir:build/Linux-clang++.release-x86_64
    abt.outofdate  pch:1  src:38  lib:2  exe:1
    report.abt  n_target:4  time:00:00:00.037978981  n_warn:0  n_err:0  n_install:0

And rebuild it:

    $ abt sample
    abt.config  cfg:release  uname:Linux  arch:x86_64  compiler:g++  cache:none
    abt.config  config:Linux-clang++.release-x86_64  cache:none  out_dir:build/Linux-clang++.release-x86_64
    abt.outofdate  pch:1  src:38  lib:2  exe:1
    abt.build  line_n:48,413  built_n:13,511  job_n:3  complete:28
                ^^^^^ build in progress

In the example above, `job_n` indicates the number of jobs that are running at the moment. The default is picked
based on the number of processors in the system. It can be overriden by specifying `-maxjobs`.
The `config` line specifies the 4-tuple of uname, compiler, configuration and target architecture
that determine which build options to use. If a compiler cache is being used, it's also specified here.
See the [Compiler Cache](#compiler-cache) section.

The `outofdate` line shows what `abt` found to be out of date: 1 precompiled header,
38 source files, 2 libraries and 1 executable.

### Input Tables

Abt's main input tables come from the dev namespace of the default data set
(as specified with `-in`)

* dev.target         Buildable target
* dev.targdep        Pairwise dependencies between targets
* dev.targsrc        List of source files for each target.
* dev.tool_opt       List of options to use for compilation and linking,

There are also a few general tables.

* dev.uname          List of supported OS's (Linux, Darwin, Cygwin, etc)
* dev.compiler       List of supported compilers (clang++, g++, etc)
* dev.cfg            List of arbitrarily named configurations (release, profile, debug, etc.)
* dev.arch           List of target architectures (such as x86_64)

### Output Directory

The resulting object files are now in `build/release/sample`, 
or more specifically `build/Linux-clang++.release-x86_64/sample`,
since `build/release/sample` is a soft link.


    $ ls -l build/Linux-clang++.release-x86_64/*sample*
    -rwxrwxr-x. 1 alexei alexei 109128 May  3 18:35 dflt.release-x86_64/sample
    -rw-rw-r--. 1 alexei alexei   1912 May  3 18:34 dflt.release-x86_64/cpp.sample.sample.o
    -rw-rw-r--. 1 alexei alexei  24776 May  3 18:34 dflt.release-x86_64/cpp.gen.sample_gen.o

`abt` places all output files in the same output directory, with no subdirectories.
Source file paths are flattened, substituting `/` with `.`. So, `cpp/sample/sample.cpp`
becomes `cpp.sample.sample.o` in this directory.

The output directory can be overriden with `-out_dir` option. 
If not specified, `-cfg` defaults to `-release` and the output directory defaults to `build/$cfg`.
Typically, `build/$cfg` is a soft link installed by whatever bootstrap script you ran.

### Bootstrap System

The bootstrap files are prepared by `atf_norm bootstrap` by scanning the `builddir` table 
and invoking `abt` with `-build:N -printcmd` options. There is one bootstrap file for each tested
combination of uname, compiler and arch. Bootstrap files are prepared only for the release configuation.
The bootstrap files live in `bin/bootstrap`:

    $ ls -l bin/bootstrap
    total 104
    -rwxr-xr-x 1 alexei alexei 21519 May 17 01:19 Darwin-clang++.release-x86_64
    -rwxr-xr-x 1 alexei alexei 23442 May 17 01:19 Linux-clang++.release-x86_64
    -rwxr-xr-x 1 alexei alexei 28566 May 17 01:19 Linux-g++-9.release-x86_64
    -rwxr-xr-x 1 alexei alexei 26072 May 17 01:19 Linux-g++.release-x86_64

When you first run `ai`, it finds that `abt -version` returns an error. This indicates that `abt`
is not usable. `ai` then guesses at the values of `uname`, `compiler` and `arch` by invoking
the `uname` utility and checking if the compiler is installed, and finally runs one of the bootstrap scripts
from `bin/bootstrap`. The bootstrap script create the top-level `build` directory and creates
soft links under it. When `abt` is invoked, this soft link provides abt with the defaults
for each of the options `-uname`, `-compiler`, `-arch`.

The `build/$cfg` (by `$cfg` I mean any one of the supported cfg values) can be rewritten
manually, or by rerunning one of the bootstrap scripts. For instance, if `ai` picked `clang++`
and you wanted `g++-9`, you can simply run the corresponding bootstrap script, and `g++-9`
will become the default compiler.

### The -install option

By default, the resulting files are left in the output directory.
There are already soft links from `bin`/ to `../build/release`. If we want to re-point the default
binary to a different version, the `-install` option will rewrite the soft link to point 
to the new executable. For instance, we may want to install a debug sample version with
`abt -install sample -cfg debug`. This will rewrite the soft link `bin/sample` to be
`../build/Linux-clang++.debug-x86_64/sample`.

### Target Definition

We can view the definition of target `sample`, as created by `acr_ed`, with `acr`.
As usual, several viewing options are available, in this case we skip the tree mode
and use a block output.

    $ acr target:sample -ndown 10 > x
    dev.target  target:sample

    dev.targdep  targdep:sample.algo_lib  comment:""
    dev.targdep  targdep:sample.algo_pch  comment:""
    dev.targdep  targdep:sample.lib_prot  comment:""

    dev.targsrc  targsrc:sample/cpp/sample/sample.cpp            comment:""
    dev.targsrc  targsrc:sample/cpp/gen/sample_gen.cpp        comment:""
    dev.targsrc  targsrc:sample/include/sample.h              comment:""
    dev.targsrc  targsrc:sample/include/gen/sample_gen.h      comment:""
    dev.targsrc  targsrc:sample/include/gen/sample_gen.inl.h  comment:""
    report.acr  n_select:9  n_insert:0  n_delete:0  n_update:0  n_file_mod:0

As we can see, headers are considered sourdce files, and there are a couple of libraries
(`lib_prot` and `algo_lib`), and a precompiled header (`algo_pch`).

### Customizing Options

When debugging memory errors, we would use `abt -cfg:debug`; or 
`-cfg:coverage` when updating coverage.

It is possible to see what options `abt` will pass to the compiler under a certain configuration.
The slightly non-standard, non-SKNF table `dev.tool_opt` allows customizing these options
on a per-target, per-uname, per-compiler, per-cfg and per-arch basis:

    $ acr cfg:coverage -t
    dev.cfg  cfg:coverage  comment:"coverage measurement"
      dev.builddir  builddir:Linux.coverage-x86_64  comment:""

      dev.tool_opt  tool_opt:181  opt_type:CC_OPTS    opt:-ftest-coverage    target:""  uname:""  compiler:g++  cfg:coverage  arch:""  comment:""
      dev.tool_opt  tool_opt:182  opt_type:CC_OPTS    opt:-fprofile-arcs     target:""  uname:""  compiler:g++  cfg:coverage  arch:""  comment:""
      dev.tool_opt  tool_opt:183  opt_type:CC_OPTS    opt:"-fprofile-dir=."  target:""  uname:""  compiler:g++  cfg:coverage  arch:""  comment:""
      dev.tool_opt  tool_opt:184  opt_type:LINK_OPTS  opt:--coverage         target:""  uname:""  compiler:g++  cfg:coverage  arch:""  comment:""
      dev.tool_opt  tool_opt:185  opt_type:CC_OPTS    opt:-D_COVERAGE        target:""  uname:""  compiler:g++  cfg:coverage  arch:""  comment:""
      report.acr  n_select:7  n_insert:0  n_delete:0  n_update:0  n_file_mod:0
  
### Disassembling

Abt includes a convenient disassembly mode, which can be invoked with `-disas`.
The parameter is a regular expression that's matched against function names in the 
compiler's assembler output.

    $ abt sample -disas Main | head -15
    abt.config  config:Linux-clang++.release-x86_64  cache:none  out_dir:build/Linux-clang++.release-x86_64
    abt.outofdate  pch:0  src:0  lib:0  exe:0
    0000000000000000 <sample::Main()>:
       0:	53                   	push   %rbx
       1:	be 00 00 00 00       	mov    $0x0,%esi
       6:	48 83 ec 10          	sub    $0x10,%rsp
       a:	8b 1d 00 00 00 00    	mov    0x0(%rip),%ebx        # 10 <sample::Main()+0x10>
      10:	48 89 e7             	mov    %rsp,%rdi
      13:	48 c7 04 24 00 00 00 	movq   $0x0,(%rsp)
      1a:	00 
      1b:	c7 44 24 08 0d 00 00 	movl   $0xd,0x8(%rsp)
      22:	00 
      23:	e8 00 00 00 00       	callq  28 <sample::Main()+0x28>
      28:	89 da                	mov    %ebx,%edx
      2a:	b9 01 00 00 00       	mov    $0x1,%ecx

### Specifying a different compiler

The `-compiler` option tells `abt` to use a compiler other than the default. In addition
to `-arch`, `-uname`, and `-cfg` option, this mostly affects the choice of options from the `tool_opt` 
table. For each of the files it compiles, `abt` scans the `tool_opt` table in full, and includes
any options that apply. There is a handful of custom lines of code in abt for dealing specifically
with `g++` and `clang`, and other cases that cannot be handled by `tool_opt` alone.

### The -ood option

You can find out which files are out-of-date (I don't really see how this could be useful,
but the option is there) with `-ood`:

    $ touch include/sample.h
    $ abt -ood sample -build:N
    dev.target  target:sample
    dev.srcfile  srcfile:cpp/sample/sample.cpp
    abt.config  config:Linux-clang++.release-x86_64  cache:none  out_dir:build/Linux-clang++.release-x86_64
    abt.outofdate  pch:0  src:1  lib:0  exe:1
    report.abt  n_target:4  time:00:00:00.031850008  n_warn:0  n_err:0  n_install:0

### The -listincl option

To see the full list of include files, as discovered by abt, for a given target, use

    $ abt -listincl sample 
    dev.Include  include:cpp/sample/sample.cpp:include/algo.h  sys:N  comment:""
    dev.Include  include:cpp/sample/sample.cpp:include/sample.h  sys:N  comment:""
    dev.Include  include:cpp/gen/sample_gen.cpp:include/algo.h  sys:N  comment:""
    ...

### Debugging the build

Just like with other programs, the verbosity level `-v` can be used to trace the execution.
When run with `-v`, abt will show the commands that execute. Otherwise, only the commands that
either fail or produce output are echoed to the screen. By default, they are hidden to keep output
clean.

### Compiler Cache

`abt` supports two compiler caches. The `ccache` is an open-source tool, and `gcache` is a built-in version.

~TBD~

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


### Steps

*UNDER CONSTRUCTION*.

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

*UNDER CONSTRUCTION*.

Bitsets can be created on top of any integer field (e.g. u8 to u128) or array field (Inlary, Tary).
Amc generates functions to provide indexed access to bits of the underlying field.

### Count: Count elements

*UNDER CONSTRUCTION*.

Count is a xref type that simply keeps track of the number of child
elements referring to a given parent. The elements themselves are not accessible
via this field.

### Fconst: Enumerated type

*UNDER CONSTRUCTION*.
This reftype is not specified explicitly. It is applied when fconst record appears.
Each fconst record names a symbol and a C++ expression (value). The symbol is the string representation
of the vaule. Amc creates an enum type with values, and creates to-string and from-string functions
that translate between values and symbols.

A related record is Gconst, which can be used in place of fconst to create an enum type out of an entire table.

When printing, if there is no symbol associated with the numeric value of the field being printed, the
value is printed as a number. When reading, if input symbol doesn't map to any known value, it is parsed
as an integer instead.

### Inlary: Inline array

*UNDER CONSTRUCTION*.

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


## acr_ed: Acr Editor

`Acr_ed` started its life as a cheat sheet generator, but now it's an indispensable 
tool for common tasks such as creating, deleting, and renaming targets, ctypes, ssimfiles,
xrefs, and source files.

By default, `acr_ed` spits out an executable script to stdout. With
`-write`, the script is executed (this has the same effect as piping output
to sh)

~~~

-field:X -create: Create field or access path.
  Suboptions: -comment, -arg, -dflt, -reftype, -before, -via, -xref, -fstep, -substr, -anon, -bigend
  Some of these can be guessed by acr_ed. For instance, if program x has tables y and z, then using
    acr_ed -create -field x.FY.zd_z will automatically add a cross-reference ("group-by") of z by y,
    figuring out the projection of z on y and any necessary index through which to find a y record
    given a z.
-finput -create  -ssimfile:X -target:Y: Add ssimfile as finput for a target. Suboptions: -indexed

~~~

### Targets

#### Create Target

    $ acr_ed -create -target:X
    ...

#### Rename Target

    $ acr_ed -target:X -rename:Y
    ...

#### Delete Target

    $ acr_ed -del -target:X
    ...

### Ssimfiles

#### Create Ssimfile

    $ acr_ed -create -ssimfile ns.x 
    ...

#### Create Ssimfile as a Subset of Another Ssimfile

    $ acr_ed -create -ssimfile ns.x -subset ns.Y
    ...

#### Create Ssimfile as a Cross Product of 2 Other Ssimfiles

    $ acr_ed -create -ssimfile ns.x -subset ns.Y -subset2 ns.Z -separator .
    ...

Note that ssimfiles user lowercase names but ctypes use uppercase names.
Options `-subset`, `-subset2` take ctype names.

#### Rename Ssimfile

    $ acr_ed -ssimfile ns.x -rename ns2.y
    ...

#### Delete Ssimfile

    $ acr_ed -del -ssimfile ns.x
    ...

### Source Files

#### Create Source File
When creating a source file, `acr_ed` automatically which target
this file will belong to based on the the other files in the same directory.
If there is ambiguity, specify `-target ...` argument
Headers are considered source files.

    $ acr_ed -create -srcfile cpp/...path.cpp
    ...
    $ acr_ed -create -srcfile include/path.h

#### Rename Source File

    $ acr_ed -srcfile cpp/path.cpp -rename cpp/path2.cpp
    ...

#### Delete Source File

    $ acr_ed -del -srcfile cpp/path.cpp
    ...

### Ctypes

#### Create A Ctype

    $ acr_ed -create -ctype ns.Name
    ...

#### Rename A Ctype

    $ acr_ed -ctype ns.Name -rename ns2.Name2
    ...

#### Delete A Ctype

    $ acr_ed -del -ctype ns.Name
    ...
    
### Add An Input To Program

    $ acr_ed -create -finput -target ns -ssimfile ns2.name
    acr_ed.create_finput  target:abc  ssimfile:dev.gitfile
    bin/acr  '' -insert:Y -check:Y -write:Y -t:Y -rowid:Y << EOF
    dmmeta.ctype  ctype:abc.FGitfile  comment:""
    dmmeta.field  field:abc.FGitfile.base  arg:dev.Gitfile  reftype:Base  dflt:""  comment:""
    dmmeta.field  field:abc.FDb.gitfile  arg:abc.FGitfile  reftype:Lary  dflt:""  comment:""
    dmmeta.finput  field:abc.FDb.gitfile  extrn:N  update:N  strict:Y  comment:""
    #  Proposed change
    #
    #
    #     / abc.FDb
    #     |
    #     |Lary gitfile-->/ abc.FGitfile
    #     -               |
    #                     |
    #                     -
    EOF
    ...
    
With `-indexed` option, a hash index is thrown in. This is pretty common.

### Create A New In-Memory Table

To create a new table in a program, use `-create -ctype ... -subset ... -pooltype`.
This adds a global pool of a given type (typically `Tpool` or `Lary`).
To throw in a hash index, specify `-indexed`

    $ acr_ed -create -ctype abc.FTable -subset u32 -pooltype Tpool -indexed
    bin/acr  '' -insert:Y -check:Y -write:Y -t:Y -rowid:Y << EOF
    dmmeta.ctype  ctype:abc.FTable  comment:""
    dmmeta.field  field:abc.FTable.table  arg:u32  reftype:Val  dflt:""  comment:""
    dmmeta.field  field:abc.FDb.table  arg:abc.FTable  reftype:Tpool  dflt:""  comment:""
    dmmeta.field  field:abc.FDb.ind_table  arg:abc.FTable  reftype:Thash  dflt:""  comment:""
    dmmeta.thash  field:abc.FDb.ind_table  hashfld:abc.FTable.table  unique:Y  comment:""
    dmmeta.xref  field:abc.FDb.ind_table  inscond:true  via:""
    EOF

### Create An Index

#### Create A Hash Table

    $ acr_ed -create -field abc.FDb.ind_table
    $ acr_ed -create -field abc.FDb.ind_table -hashfld <fieldname>

#### Create A Binary Heap

    $ acr_ed -create -field abc.FDb.bh_table
    $ acr_ed -create -field abc.FDb.bh_table -sortfld <fieldname>

#### Create An AVL Tree

    $ acr_ed -create -field abc.FDb.tr_table
    $ acr_ed -create -field abc.FDb.tr_table -sortfld <fieldname>

### Conditional X-Ref

    $ acr_ed -create -field abc.FDb.ind_table -inscond false

## Coding Style

OpenACR is written with a certain uniform coding style. This section 
explains the reasoning behind it. The style is maintained by various tools.
First of all, amc generates all code in this style; 
Second, `indent-recent` and `cpp-indent` automatically indent 
source files, and should be used before any commit.

### Spaces, Indentation

Use spaces, use 4 space indent. We use `cpp-indent` to normalize all source files,
so this policy is enforced automatically.

### Variable Names

Variable names use `lower_under` style.

### Member Functions

There are no member functions. Functions describe actual blocks of instructions for 
CPU to execute; Structs don't even exist -- they are rules for calculating field
offsets and thereby structuring RAM. We maintain this separation.

### Predicate Functions

Functions that return bool end in Q: VisibleQ, ExistsQ, etc. 
Q means Query or Question mark. This is the Mathematica convention.

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
Structure time with plain C++.

### No Code In Headers

`.h` contain declarations and structure definitions only, never any code.
All inline code codes into `inl.h` headers (after all, it must go somewhere).

### Use query-command separation

Functions can be divided into two types: queries, which are read-only functions that compute
a value; and commands, which are procedures that perform some in-memory update.
Queries are a lot easier to write and debug, since there is no post-condition to prove.
Keep as much code as possible in query form, and keep it separate from command-type functions.
This will reduce debugging time. Bertrand Meyer writes eloquently about this.

### Keep it single-threaded

We can easily task thousands of cores with independent tasks, and they will all execute at once.
But let's not task a human with trying to understand two tasks simltaneously. Keep programs
single-threaded, and as deterministic (independent of wall clock) as possible,
and your debug times will be finite. Hoare's
[Communicating Sequential Processes](https://www.cs.cmu.edu/~crary/819-f09/Hoare78.pdf) should
serve as an inspiration.

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
correctness. Bertrand Meyer is a big proponent of this style.
See [Wiki Entry](https://en.wikipedia.org/wiki/Single-entry_single-exit)

### Single File Static Assignment

Single Static Assignment (SSA) is when  there is only one location in the source tree
where each variable is assigned to. This is preferable but not required.

All assignments to a given variable must be in the same source file.
This is necessary for reasoning about correctness, debugging, and enforcing post-conditions
of given variable assignment. See Andrew Appel's
[SSA Is Functional Programming](https://www.cs.princeton.edu/~appel/papers/ssafun.pdf)

### Document all non-static functions

Non-static functions are API functions. Each one needs to be documented, explaining
the reason for its existence, side effects, pre- and post-conditions,
and any other rules for caller/callee not enforceable at the language level.

### All rules allow exceptions

Given a sufficiently good reason, any rule can be replaced. But let's try to keep a consistent
whole by following these rules whenever possible.


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


## amc_gc: AMC garbage collector

`amc_gc` is a tool for removing unused records from the dmmeta database.

`amc_gc` takes a target regex and an acr query as an argument. It finds all records
matching the query, then it tries to remove them one by one, and rebuilds the targets
(using abt) in a sandbox dir. If build succeeds with one of the records deleted
it means it wasn't needed in the first place.

Let's illustrate `amc_gc` by creating a new program and inputting a table. 

    $ acr_ed -create -target abc -write
    $ acr_ed -create -finput -target abc -ssimfile dmmeta.ns -write

Since the `ns` table is unused, `abc` will compile even if we remove it. This is the 
case that `amc_gc` detects, and can remove the table:

    $ amc_gc -target:abc -key:ctype:abc.%
    amc_gc.begin  tot_rec:2  n_cppline:259802  watch_cmd:"watch head -50 temp/amc_gc.build"
    amc_gc.analyze  query:dmmeta.ctype:abc.FDb  eliminate:N  rec_no:1  tot_rec:2  n_del:0  n_cppline:259802  n_cppline_del:0
    amc_gc.analyze  query:dmmeta.ctype:abc.FNs  eliminate:Y  rec_no:2  tot_rec:2  n_del:1  n_cppline:259341  n_cppline_del:461
    report.amc_gc  key:ctype:abc.%  n_match:2  n_del:1  n_cppline:259341  n_cppline_del:461

And indeed, `amc_gc` successfully garbage collects the table.
Let's finish by deleting the unused target

    $ acr_ed -del -target abc -write

## MariaDB integration

All ssim tables are compatible with MariaDB. The namespace (ns) corresponds to a database,
and ssimfile corresponds to a table.
Tools mysql2ssim and ssim2mysql perform conversion to/from ssim schema to SQL, and
acr_my can set up a local MariaDB server process and iteract with
it via a socket.

Here are some useful commands:
Start a local MariaDB server:

    $ acr_my -start dmmeta

Connect to the local server on the command line:

    $ acr_my -shell

From here, you can issue SQL commands:

    > select count(*) from dmmeta.field where arg ='u8';

When you exit from the shell, the server keeps running.
You can stop the server and save changes:

    $ acr_my -stop

Or stop the server, discarding changes:

    $ acr_my -abort

You can also use acr -my as a shortcut for acr_my -start -shell -stop:

    $ echo 'update thash set unique='N' where field like "acr.%"' | acr -my -fldfunc dmmeta.%
    $ amc

The table dmmeta.sqltype allows ssim2mysql to map ctypes to SQL types so that
round tripping can occur without loss.


## Working with source files & targets

### Listing Files

List all targets:

     acr target

List all libraries:

     acr dmmeta.ns.nstype:lib

List all sources for target T, one filename per line:

     acr targsrc:T/% -field:src

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


## Tests

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

Every commit should satisfy `normalize`.

### Unit Tests

`atf_unit` runs single-function tests which are linked into `atf_unit`.
The test source code is under cpp/atf/unit, and the names of all defined tests are in the table
atfdb.unittest. You can see what all the tests are with `acr unittest`.

Let's begin by creating a new unit test:

    $ acr_ed -create -unittest algo_lib.Test1 -write

This creates a new entry in `atfdb.unittest`, and adds a stub to `cpp/atf/unit/algo_lib.cpp`.

    +// --------------------------------------------------------------------------------
    +
    +void atf_unit::unittest_algo_lib_Test1() {
    +    // test code goes here
    +}

After implementing the test, we can run it with `atf_unit algo_lib.Test`

We can run all unit tests from our namespace with 

    atf_unit algo.%
    
### Normalization Checks

We can create a normalization check with 

    acr_ed -create -normcheck <name> -write

### Debugging

If a test fails, the easiest way to debug it is to re-run `atf_unit` with 
`-debug` flag. It will use `mdbg` and automatically set a breakpoint at the first 
line of the test in question.

## Mdbg: Gdb front-end

This is a wrapper for automating the invocation of `gdb` from command line.

Mdbg uses abt to build a debug version of the debug target (By default,
this means `debug`, but the option can be customized with `-cfg <cfg>`, for instance
specifying `-cfg profile`).

Then, mdbg prepares a debugging environment by writing two script files: one
for the debugger, `temp/mdbg.gdb`, and one for the editor, `temp/mdbg.el`.
The default, and really the most supported editor, is emacs; specifying `-tui`
will switch to the gdb's Text User Interface mode instead.

Mdbg then runs the debug target in the debug environment. It can optionally set a regular
or conditional breakpoint in a function other than Main with the `-b` option.

Let's consider an example:

    $ acr_ed -create -target abc -write
    ...
    $ mdbg abc
    
Mdbg will print a reminder of the shortcuts it has equipped the target editor with...

    mdbg.note  Debug script saved to temp/mdbg.el
    mdbg.note  Invoking gdb under emacs. Make sure to link ~/.emacs -> conf/emacs.el, ~/elisp -> conf/elisp
    mdbg.note  Standard shortcuts:
    mdbg.note               F7  recompile and restart executable
    mdbg.note              F11  step into
    mdbg.note        Shift-F11  step out
    mdbg.note              F10  step over
    mdbg.note         Ctrl-F10  run to cursor
    mdbg.note              F12  restore windows
    mdbg.note              F9   set breakpoint
    mdbg.note         Ctrl-F9   clear breakpoint
    mdbg.note              F6   other window
    mdbg.note    Alt-Up Arrow   Go up the call stack
    mdbg.note  Alt-Down Arrow   Go down the call stack

and run the target program under the debugger, stopping at Main.

      // --- abc...main
      int main(int argc, char **argv) {
          try {
    B =>      algo_lib::FDb_Init();
              abc::FDb_Init();
              algo_lib::_db.argc = argc;
      ...

The `-manywin` option enables emacs' gdb's `gud-many-windows` mode, which brings up
a debugger layout reminiscent of Visual Studio or Eclipse, with locals, breakpoints, 
threads subwindows, etc.
Additionally specifying the `-disas` option enables the disassembly window.

### Specifying arguments

The first argument to mdbg is the debug target name. Mdbg must know it so it can build it.
To pass arguments to the debug target, specify them in a single string like this:
 
    $ mdbg abc " -in:filename"

Always include a space in front of the arguments: since Bash will strip the quotes, mdbg
needs to know that the specified option is not an mdbg option but in fact an option for the debug
target. To avoid any ambiguity, you could write `-args:-in:filename`, but it seems less 
convenient than passing the arguments directly.

### The -tui option

To use gdb's text UI, use `mdbg -tui`

### Specifying Breakpoints

The default breakpoint is Main, but others can be specified, including gdb's conditional breakpoints.
For instance, in the example above, we might invoke mdbg with a custom breakpoint.

    $ mdbg abc -b algo::Prlog 
    ...

      void algo::Prlog(int fd, cstring &str, int start, bool eol) {
          try {
    B =>      if (eol) {
                  str << '\n';
              }
    
Or, if we're interested only in output that goes to stderr,

    $ mdbg abc -b "algo::Prlog if fd==2"
    
Multiple breakpoints can be specified by comma-separating them. For instance `-b "A, B if c"`, etc.
To execute a command at a breakpoint, use `-bcmd` option:

    $ mdbg abc -b "algo::Prlog if fd==2" -bcmd "print str.ch_n"

These options preconfigure gdb via the `mdbg.gdb` file so that 
there is less typing later.

### Edit-and-retry

Mdbg sets up the standard shortcut F7 for edit-and-retry: the target is rebuilt
and reinvoked with the same options as initially. This can save a lot of set up, because
the gdb instance is not restarted; only the debug target.

### The -follow_child otption

This controlls gdb's `follow child` so that the debugger traces the child process instead
of the parent. By default, only the parent process is debugged.
This option is really most useful for debugging `fork()` calls; With fork/exec, it makes
the most sense to isolate the child process command line and debug it directly without
messing with the parent first.

### Exceptions

By default, gdb is configured to catch exceptions. To ignore them instead, specify `-catchthrow:N`

### Gdb Python Interface

To enable gdb python scriptability, specify `-py` option. 

### Bugs

Because gdb and emacs don't form a standardized environment, it's possible that mdbg's integration
is broken in some way. Please file an issue, describing your config, so that this combination
can be tried out and fixed.


## Scriptlets

OpenACR includes a number of little scripts to ease program development.
Some of them are useful as stand-alone utilities

### Hilite: inline highlighter

The hilite tool is a perl script for highlighting strings in some output.

Usage:

    <some command> | hilite <expr1> <expr2> ...

Each expr is a perl regex that is highlighted using a separate color.
colors are picked automatically from a default sequence.

To specify a color explicitly, use its name: `expr1:green expr2:yellow`
The list of available colors is black, red, green, yellow, blue, magenta, cyan, white, darkblack, 
darkred, darkgreen, darkyellow, darkblue, darkmagenta, darkcyan, darkwhite.

Omitting `expr` matches the entire line, so `hilite :green` makes all lines green.

#### Blotter Mode

To highlight every other line, use `:green%2`. This is useful with values of 5
when the output is wide, creating a blotter effect.

If multiple patterns match a given input line, patterns specified later
on the command line override previous patterns; Each pattern can match multiple
times within the line.

#### Inline Specification

Highlighting rules can be provided as part of the input.
If a line starts with the special word 'hilite.colors', e.g.

    hilite.colors <expr1> <expr2> ...

Then the previous rules are flushed,
and new exprs are processed as if they were read from the command line.
This allows commands to output hilite instructions that keep the text readable
yet support highlighting where appropriate.

#### Diff mode

If `<expr>` is `-diff` or `-d`, hilite loads diff rules that support either line-
or word-diffs. Example of colorizing a diff file:

   cat patchfile | hilite -d | less -r

Removed lines or words are colored red, added lines or words are green, and index
lines are blue.

Hilite is not intended to compete with the various syntax highlighters; it is an
interactive tool intended to ease readibility or terminal-based output.

### Grephunk: patch file filter

Grephunk is a perl script that scan hunks on stdin
and filter them according them to specified pattern. A `hunk` is the name patch file
people use to refer to sections of a patch file that look like this:

    diff --git a/include/gen/atf_amc_gen.inl.h b/include/gen/atf_amc_gen.inl.h
    index fbcd12a..5871940 100644
    --- a/include/gen/atf_amc_gen.inl.h
    +++ b/include/gen/atf_amc_gen.inl.h
    @@ -385,0 +386,14 @@ inline bool atf_amc::Ctype2Attr_Update(atf_amc::Ctype2Attr &lhs, atf_amc::Ctype2
    +    atf_amc::DelType1_Uninit(*this);
    @@ -426 +440 @@ inline atf_amc::DispFilter::DispFilter() {
    -inline int atf_amc::pmask_Nbits(atf_amc::DispFilter &parent) {
    +inline int atf_amc::pmask_Nbits(atf_amc::DispFilter& parent) {

Here, there are two hunks, one for each line starting with `@@`.
The `filename` of a hunk is the contents of the `diff` line.

The `header` is everything from the word `diff` to the first `@@`.
Hunk body is everything between `@@` and the next `@@` or end of file.

Grephunk allows specifying patterns for any of these entities.
Patterns can apply to filename, line, hunk header, or the entire hunk.

Pattern prefix specifies its scope:
* no prefix: applies to hunk body
* l:    line pattern (applies to content only)
* f:    filename pattern (applies to diff header line)
* a:    applies to 'a' file (line starts with ---)
* b:    applies to 'b' file (line starts with +++)
* h:    hunk pattern -- applies to the entire hunk, including any header lines

Any number of patterns must be specified on the command line.
Filename pattern is specified with `f:<regex>`.
For instance,

    <some command> | grephunk abcd f:cpp

Searches fort hunks that match string `abcd` where the filename also
matches `cpp`

A pattern can be positive or negative, negative patterns are preceded with `!`.
So,

    <some command> | grephunk f:\!/gen/ | grephunk blah

Excludes files with /gen/ in the pathname, and then searches for string `blah`.

Grephunk outputs any hunk that matches any of the patterns. So, "OR" is assumed.
If you want "AND", just chain several grephunk invocations one after another, or use
a `blah.*blah` pattern.

In the example below, hunk must contain the word amc anywhere in it,
and filename should not have pattern /gen/

    grephunk amc f:!/gen/ h:'}\s+$'

## FF - find files and in files

`Ff` is a file search utility.
The simplest usage is

    ff <term1> <term2>
    
Ff recursively searches for all text files starting from from the starting
point (default is `.`), and then searches inside each for for the specified pattern.
All terms must occur on the same, in effect `ff term1 term2` is equivalent to `ff term1.*term2`.

All terms are perl regexes.

#### Selecting Files And Directories

Ff can search inside a particular directory with `-d`; Multiple directories can be specified:

    ff -d /usr/include -d cpp <term>
    
The `-n` option excludes generated files (files with `/gen/` in the path).
The `-s` option adds `/usr/include` to the list of starting directories.

The `-g` option starts the search in the `ffroot` directory. This directory is determined
by walking upward from the current directory until encountering a directory with the file `.ffroot` in it.

`ff` automatically skips directory `temp/`.
The `-f` option can be used to select which files `ff` selects for searching.
There can be multiple `-f` options. They act as "OR". 

#### Negative Patterns

Negative patterns can be preceded with `!`.
Negative patterns are available both for filenames (`-f`) and file conrtents.
For instance, to exclude headers, you might use `ff -f '!\.h$'`. Make sure to quote `!` 
so it's not interpeted by the command shell as a history item

#### Matching Options

The `-t` option looks for typedefs or struct definitions (in a very loose sense).

The `-w` option matches whole words only. This is equivalent to wrapping terms with `\b...\b`
since all terms are Perl regexes.

The `-i` option enables case-insensitive search for all terms.

#### Editing Matches

With `-l` option, only the names of files that matched the terms are printed.

The `-c` option adds extra empty lines between multiple matches in the same file; for visual grouping.

The `-a` option prints all matches, even if there was more than match per line.
This can be useful with emacs macros, in order to visit all match locations and perform some action.

The output of `ff` is ready to consume as compilation buffer output in emacs, or via errlist.
To open the list of matches in an editor, do `errlist ff <term1>...`.
Integration of `ff` with emacs is provided in `conf/emacs.el`.

#### Highlighting

By default, `ff` highlights output. But if `-notty` is specified, or the output
is not a terminal, highlighting is skipped.

`ff` is similar to `grep -RHn`, but more specialized.

## Testimonials

These are some of the good things people have said about acr/amc.

### 1

    Alexei has built a tool set and framework that significantly improves
    the software development process with large percentage of the code
    generated with guaranteed consistency and quality. 

        Shreejith Billenahalli

### 2

    Alexei's ACR and AMC is a unique, transformative, and extendible tool
    that models and implements the relationships between data,
    configurations, components, and anything else that can be defined. The
    code that it generates leads to reduced development efforts, improved
    quality, excellent performance, and a single coding standard across
    the entire codebase.

        Jon Joshua

### 3

    Alexei has created a paradigm-shifting methodology for systems that
    require extreme stability while having a high-frequency release
    cycle. His model compiler in combination with pre-commit invariant
    checking has enabled a big team of developers to produce very
    high-quality products in very short periods of time while working on a
    single codebase

        Hayk Mkrtchyan
 
### 4

    Alexei envisioned a new HPC programming paradigm, wrote a programming
    language and compiler for it, and invented middleware with a
    blockchain-like transactional algorithm. The resulting unique and
    powerful programming environment, that I have the privilege using at
    AlgoEngineering and NYSE, formed the foundation for NYSE Pillar
    exchange architecture. NYSE Pillar currently powers 4 markets, with
    more on the way.

        Vladimir Parizhsky

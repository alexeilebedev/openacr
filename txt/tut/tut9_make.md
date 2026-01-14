## Tutorial on transitioning from Makefile to OpenACR build
<a href="#tutorial-on-transitioning-from-makefile-to-openacr-build"></a>
The goal of this tutorial is to show step-by-step how a ``build`` system utilizing a well known  ["GNU Make"](https://www.gnu.org/software/make/manual/make.html) can be transformed with the help of ["Openacr"](https://github.com/alexeilebedev/openacr). The purpose of the transformation is an illustration of ssim definitions, Primary Keys, Foreign Keys, composite keys with **fldfunc** and the resulting Referential Integrity. The Makefile here is an artificial example from GNU Make documentation, the actual build system will not have object files as targets etc...

We will first go through all data definitions, and after the data is defined, refer to cpp program that utilizes the data to deliver the Makefile. The sample program and the data definitions are described below. All definition details are also present via **generated** automatic documentation reference:

### Automatic Reference Set
<a href="#automatic-reference-set"></a>
* [samp_make - sample program for Makefile management](/txt/exe/samp_make/README.md)

* [sampdb.gitfile - samp_make list of gitfile sources](/txt/ssimdb/sampdb/gitfile.md)

* [sampdb.targdep - samp_make targets dependencies](/txt/ssimdb/sampdb/targdep.md)

* [sampdb.target - samp_make targets list](/txt/ssimdb/sampdb/target.md)

* [sampdb.targrec - samp_make recipe for a target](/txt/ssimdb/sampdb/targrec.md)

* [sampdb.targsrc - samp_make targets source dependencies](/txt/ssimdb/sampdb/targsrc.md)

### Problem Statement
<a href="#problem-statement"></a>
We want to transform "hand-written" process of writing the Makefile into a process of filling out some tables and getting Makefile as a result of a cpp program that uses the tables as the input and produces the Makefile. The tables have to have a very strong ***Referential Integrity***, so the data entry process can be verified at the entry point, guaranteeing the final result. The presentation emphasis is on the Primary Keys and the Referential Integrity.

``GNU Make`` is driven by its ``Makefile`` - [A Simple Makefile](https://www.gnu.org/software/make/manual/make.html#Simple-Makefile) from the documentation is given below:

```
edit : main.o kbd.o command.o display.o \
       insert.o search.o files.o utils.o
        cc -o edit main.o kbd.o command.o display.o \
                   insert.o search.o files.o utils.o

main.o : main.c defs.h
        cc -c main.c
kbd.o : kbd.c defs.h command.h
        cc -c kbd.c
command.o : command.c defs.h command.h
        cc -c command.c
display.o : display.c defs.h buffer.h
        cc -c display.c
insert.o : insert.c defs.h buffer.h
        cc -c insert.c
search.o : search.c defs.h buffer.h
        cc -c search.c
files.o : files.c defs.h buffer.h command.h
        cc -c files.c
utils.o : utils.c defs.h
        cc -c utils.c
clean :
        rm edit main.o kbd.o command.o display.o \
           insert.o search.o files.o utils.o
```

Let's create a list of **facts** that this ``Makefile`` contains, record them into the tables and write a [samp_make - sample program for Makefile management](/txt/exe/samp_make/README.md) program that will read the tables and (re)construct the Makefile. We'll also have additional properties when the facts are collected and the program is written.

We will record the facts into [ssim tuples](/txt/ssimdb/sampdb/README.md) dataset. By design the first column in any table in ``openacr`` must be unique - be the primary key.

### List of Targets
<a href="#list-of-targets"></a>
First, there is a list of targets - all left-aligned names in Makefile. Lets' define a table named [sampdb.target](/txt/ssimdb/sampdb/target.md). It will contain all targets from the Makefile. Targets are unique by definition, so the first column of our table is the primary key.

|TARGET|
|---|
|clean
|command.o
|display.o
|edit
|files.o
|insert.o
|kbd.o
|main.o
|search.o
|utils.o

### List of Targets combined with the ``precondition`` Sources
<a href="#list-of-targets-combined-with-the---precondition---sources"></a>
Next, we want to record what source files correspond to each target. Here we want to relate the fact that each target might have a list of source files as a precondition. So we want to express in the records that ``command.o``, the target, will have ``command.c`` source file as a precondition. To express it as a primary key we  combine ``target`` name with the ``source`` name, separated by a character, in this case "/". This table has a ``composite`` key that is split into target and source keys. The split is expressed the table definition below (copied here from [sampdb.targsrc](/txt/ssimdb/sampdb/targsrc.md) reference). There are two implied fields in the primary key targsrc field. They are tied to our [List of Targets](#list-of-targets), and defined below [List of Sources](#list-of-sources)

italicised fields: *target, src* are [*Ssim Fundamentals*](/txt/ssim.md) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|targsrc|algo.Smallstr50|Val|||
|*target*|*[sampdb.Target](/txt/ssimdb/sampdb/target.md)*|*Pkey*||*<br>/LL of targsrc*|
|*src*|*[sampdb.Gitfile](/txt/ssimdb/sampdb/gitfile.md)*|*Pkey*||*<br>/LR of targsrc*|

 The table [sampdb.targsrc](/txt/ssimdb/sampdb/targsrc.md) records will look like this:

|TARGSRC|
|---|
|command.o/command.c
|command.o/command.h
|command.o/defs.h
|display.o/buffer.h
|display.o/defs.h
|display.o/display.c
|files.o/buffer.h
|files.o/command.h
|files.o/defs.h
|files.o/files.c
|insert.o/buffer.h
|insert.o/defs.h
|insert.o/insert.c
|kbd.o/command.h
|kbd.o/defs.h
|kbd.o/kbd.c
|main.o/defs.h
|main.o/main.c
|search.o/buffer.h
|search.o/defs.h
|search.o/search.c
|utils.o/defs.h
|utils.o/utils.c

### List of Sources
<a href="#list-of-sources"></a>
We will have a table of all sources. We can match it to git controlled files: [samdb.gitfile](/txt/ssimdb/sampdb/gitfile.md). This table is the **foreign** key to [List of Targets with Sources](#list-of-targets-combined-with-the---precondition---sources) - it validates the **src** ***fldfunc*** key there.

|GITFILE|COMMENT|
|---|---|
|buffer.h||
|command.c||
|command.h||
|defs.h||
|display.c||
|files.c||
|insert.c||
|kbd.c||
|main.c||
|search.c||
|utils.c||

### List of Targets combined with ``precondition`` Targets
<a href="#list-of-targets-combined-with---precondition---targets"></a>
Targets dependance on each other, as in preconditions, will be expressed by a table [samp.targdep](/txt/ssimdb/sampdb/targdep.md).  Here we use the same **fldfunc** approach, with "." as the separator. Our **fdlfunc** here are *Target* and *Parent*, both referring to our [List of Targets](#list-of-targets). For the precise table definition see reference [samp.targdep](/txt/ssimdb/sampdb/targdep.md). 

|TARGDEP|
|---|
|clean.command.o
|clean.display.o
|clean.edit
|clean.files.o
|clean.insert.o
|clean.kbd.o
|clean.main.o
|clean.search.o
|clean.utils.o
|edit.command.o
|edit.display.o
|edit.files.o
|edit.insert.o
|edit.kbd.o
|edit.main.o
|edit.search.o
|edit.utils.o

### List of Recipes for each Target
<a href="#list-of-recipes-for-each-target"></a>
Finally, we will have a recipe table for each target: [sampdb.targrec](/txt/ssimdb/sampdb/targrec.md). Again we are tying [List of Targets](#list-of-targets) to the columns, this time directly, without **fldfunc**.

|TARGET|RECIPE|COMMENT|
|---|---|---|
|clean|rm||
|command.o|cc -c||
|display.o|cc -c||
|edit|cc -o edit||
|files.o|cc -c||
|insert.o|cc -c||
|kbd.o|cc -c||
|main.o|cc -c||
|search.o|cc -c||
|utils.o|cc -c||

### All data dependencies visualized
<a href="#all-data-dependencies-visualized"></a>
All these tables together represent the full extent of the information contained in the Makefile.

The information comes *cross-referenced* via Primary and Foreign key constraints. The constraints tie all records together into a [Transitive Closure](https://en.wikipedia.org/wiki/Transitive_closure) set. The actual dependencies and the set are fully visible via [acr](/txt/exe/acr/README.md) command, with the output below. The command returns all relations defined for each record in the target table [sampdb.target](/txt/ssimdb/sampdb/target.md) for a selected target, selected targets, or for the totality of it:

```
$ acr sampdb.target:edit -t
sampdb.target  target:edit  dflt:Y  comment:""
  sampdb.targdep  targdep:edit.command.o  rec:Y  pre:Y  comment:""
  sampdb.targdep  targdep:edit.display.o  rec:Y  pre:Y  comment:""
  sampdb.targdep  targdep:edit.files.o    rec:Y  pre:Y  comment:""
  sampdb.targdep  targdep:edit.insert.o   rec:Y  pre:Y  comment:""
  sampdb.targdep  targdep:edit.kbd.o      rec:Y  pre:Y  comment:""
  sampdb.targdep  targdep:edit.main.o     rec:Y  pre:Y  comment:""
  sampdb.targdep  targdep:edit.search.o   rec:Y  pre:Y  comment:""
  sampdb.targdep  targdep:edit.utils.o    rec:Y  pre:Y  comment:""
  sampdb.targrec  target:edit  recipe:"cc -o edit"  comment:""

sampdb.targdep  targdep:clean.edit  rec:Y  pre:N  comment:""
```

selected targets

```
$ acr sampdb.target:'edit|clean' -t
sampdb.target  target:clean  dflt:N  comment:""
  sampdb.targdep  targdep:clean.command.o  rec:Y  pre:N  comment:""
  sampdb.targdep  targdep:clean.display.o  rec:Y  pre:N  comment:""
  sampdb.targdep  targdep:clean.edit       rec:Y  pre:N  comment:""
  sampdb.targdep  targdep:clean.files.o    rec:Y  pre:N  comment:""
  sampdb.targdep  targdep:clean.insert.o   rec:Y  pre:N  comment:""
  sampdb.targdep  targdep:clean.kbd.o      rec:Y  pre:N  comment:""
  sampdb.targdep  targdep:clean.main.o     rec:Y  pre:N  comment:""
  sampdb.targdep  targdep:clean.search.o   rec:Y  pre:N  comment:""
  sampdb.targdep  targdep:clean.utils.o    rec:Y  pre:N  comment:""
  sampdb.targrec  target:clean  recipe:rm  comment:""

sampdb.target  target:edit  dflt:Y  comment:""
  sampdb.targdep  targdep:edit.command.o  rec:Y  pre:Y  comment:""
  sampdb.targdep  targdep:edit.display.o  rec:Y  pre:Y  comment:""
  sampdb.targdep  targdep:edit.files.o    rec:Y  pre:Y  comment:""
  sampdb.targdep  targdep:edit.insert.o   rec:Y  pre:Y  comment:""
  sampdb.targdep  targdep:edit.kbd.o      rec:Y  pre:Y  comment:""
  sampdb.targdep  targdep:edit.main.o     rec:Y  pre:Y  comment:""
  sampdb.targdep  targdep:edit.search.o   rec:Y  pre:Y  comment:""
  sampdb.targdep  targdep:edit.utils.o    rec:Y  pre:Y  comment:""
  sampdb.targrec  target:edit  recipe:"cc -o edit"  comment:""

```

or for all targets

```
$ acr sampdb.target -t

sampdb.target  target:clean  dflt:N  comment:""
  sampdb.targdep  targdep:clean.command.o  rec:Y  pre:N  comment:""
  sampdb.targdep  targdep:clean.display.o  rec:Y  pre:N  comment:""
  sampdb.targdep  targdep:clean.edit       rec:Y  pre:N  comment:""
  sampdb.targdep  targdep:clean.files.o    rec:Y  pre:N  comment:""
  sampdb.targdep  targdep:clean.insert.o   rec:Y  pre:N  comment:""
  sampdb.targdep  targdep:clean.kbd.o      rec:Y  pre:N  comment:""
  sampdb.targdep  targdep:clean.main.o     rec:Y  pre:N  comment:""
  sampdb.targdep  targdep:clean.search.o   rec:Y  pre:N  comment:""
  sampdb.targdep  targdep:clean.utils.o    rec:Y  pre:N  comment:""
  sampdb.targrec  target:clean  recipe:rm  comment:""

sampdb.target  target:command.o  dflt:N  comment:""
  sampdb.targrec  target:command.o  recipe:"cc -c"  comment:""
  sampdb.targsrc  targsrc:command.o/command.c  pre:Y  rec:Y  comment:""
  sampdb.targsrc  targsrc:command.o/command.h  pre:Y  rec:N  comment:""
  sampdb.targsrc  targsrc:command.o/defs.h     pre:Y  rec:N  comment:""

sampdb.target  target:display.o  dflt:N  comment:""
  sampdb.targrec  target:display.o  recipe:"cc -c"  comment:""
  sampdb.targsrc  targsrc:display.o/buffer.h   pre:Y  rec:N  comment:""
  sampdb.targsrc  targsrc:display.o/defs.h     pre:Y  rec:N  comment:""
  sampdb.targsrc  targsrc:display.o/display.c  pre:Y  rec:Y  comment:""

sampdb.target  target:edit  dflt:Y  comment:""
  sampdb.targdep  targdep:edit.command.o  rec:Y  pre:Y  comment:""
  sampdb.targdep  targdep:edit.display.o  rec:Y  pre:Y  comment:""
  sampdb.targdep  targdep:edit.files.o    rec:Y  pre:Y  comment:""
  sampdb.targdep  targdep:edit.insert.o   rec:Y  pre:Y  comment:""
  sampdb.targdep  targdep:edit.kbd.o      rec:Y  pre:Y  comment:""
  sampdb.targdep  targdep:edit.main.o     rec:Y  pre:Y  comment:""
  sampdb.targdep  targdep:edit.search.o   rec:Y  pre:Y  comment:""
  sampdb.targdep  targdep:edit.utils.o    rec:Y  pre:Y  comment:""
  sampdb.targrec  target:edit  recipe:"cc -o edit"  comment:""

sampdb.target  target:files.o  dflt:N  comment:""
  sampdb.targrec  target:files.o  recipe:"cc -c"  comment:""
  sampdb.targsrc  targsrc:files.o/buffer.h   pre:Y  rec:N  comment:""
  sampdb.targsrc  targsrc:files.o/command.h  pre:Y  rec:N  comment:""
  sampdb.targsrc  targsrc:files.o/defs.h     pre:Y  rec:N  comment:""
  sampdb.targsrc  targsrc:files.o/files.c    pre:Y  rec:Y  comment:""

sampdb.target  target:insert.o  dflt:N  comment:""
  sampdb.targrec  target:insert.o  recipe:"cc -c"  comment:""
  sampdb.targsrc  targsrc:insert.o/buffer.h  pre:Y  rec:N  comment:""
  sampdb.targsrc  targsrc:insert.o/defs.h    pre:Y  rec:N  comment:""
  sampdb.targsrc  targsrc:insert.o/insert.c  pre:Y  rec:Y  comment:""

sampdb.target  target:kbd.o  dflt:N  comment:""
  sampdb.targrec  target:kbd.o  recipe:"cc -c"  comment:""
  sampdb.targsrc  targsrc:kbd.o/command.h  pre:Y  rec:N  comment:""
  sampdb.targsrc  targsrc:kbd.o/defs.h  pre:Y  rec:N  comment:""
  sampdb.targsrc  targsrc:kbd.o/kbd.c   pre:Y  rec:Y  comment:""

sampdb.target  target:main.o  dflt:N  comment:""
  sampdb.targrec  target:main.o  recipe:"cc -c"  comment:""
  sampdb.targsrc  targsrc:main.o/defs.h  pre:Y  rec:N  comment:""
  sampdb.targsrc  targsrc:main.o/main.c  pre:Y  rec:Y  comment:""

sampdb.target  target:search.o  dflt:N  comment:""
  sampdb.targrec  target:search.o  recipe:"cc -c"  comment:""
  sampdb.targsrc  targsrc:search.o/buffer.h  pre:Y  rec:N  comment:""
  sampdb.targsrc  targsrc:search.o/defs.h    pre:Y  rec:N  comment:""
  sampdb.targsrc  targsrc:search.o/search.c  pre:Y  rec:Y  comment:""

sampdb.target  target:utils.o  dflt:N  comment:""
  sampdb.targrec  target:utils.o  recipe:"cc -c"  comment:""
  sampdb.targsrc  targsrc:utils.o/defs.h   pre:Y  rec:N  comment:""
  sampdb.targsrc  targsrc:utils.o/utils.c  pre:Y  rec:Y  comment:""
report.acr  n_select:60  n_insert:0  n_delete:0  n_ignore:0  n_update:0  n_file_mod:0
```

### Using the defined tables to generate Makefile
<a href="#using-the-defined-tables-to-generate-makefile"></a>
Now we can use this information in a cpp program [samp_make](/txt/exe/samp_make/README.md), and restore Makefile by iterating over these tables and reconstructing the Makefile. The result will look like this:

```
$samp_make -target:%

edit : command.o display.o files.o insert.o kbd.o main.o search.o 

utils.o
        cc -o edit  command.o display.o files.o insert.o kbd.o main.o search.o utils.o
command.o : command.c command.h defs.h
        cc -c  command.c
display.o : buffer.h defs.h display.c
        cc -c  display.c
files.o : buffer.h command.h defs.h files.c
        cc -c  files.c
insert.o : buffer.h defs.h insert.c
        cc -c  insert.c
kbd.o : command.h defs.h kbd.c
        cc -c  kbd.c
main.o : defs.h main.c
        cc -c  main.c
search.o : buffer.h defs.h search.c
        cc -c  search.c
utils.o : defs.h utils.c
        cc -c  utils.c
clean :
        rm command.o display.o edit files.o insert.o kbd.o main.o search.o utils.o
```

Now we can create new targets or amend this target by adding/removing the information into the tables and not dealing with the actual Makefile syntax. We can generate each individual target is we wish so: target parameter takes a ``regex`` of targets. 

```
$ samp_make -target:'clean|insert.o'
clean :
        rm command.o display.o edit files.o insert.o kbd.o main.o search.o utils.o
insert.o : buffer.h defs.h insert.c
        cc -c  insert.c

```


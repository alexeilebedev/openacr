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

    $ acr_my -start -shell

I will omit MariaDB prompts to make it easier to copy/paste commands into your shell.
Let's create a new database.

    create database test;
    use test;

Let's create Codd's project schema. Instead of the name `commit`, which Codd used for the assignment
of parts to projects, we'll use the name `partproj`.

    create table project (project varchar(50));
    create table part (part varchar(50));
    create table partproj (part varchar(50), project varchar(50));

Now let's set up the primary keys:

    alter table project add primary key project (project);
    alter table part add primary key part (part);
    alter table partproj add primary key part (part);

And the foreign key constraints:

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
you lose the ability to represent conditions that lie outside of your narrowed world view. It is hard to
overestimate the importance of this idea. 

If even a simple projects-and-parts example has 5 possible schemas that can represent it, out of which
4 have anomalies (design bugs with provably bad consequences), then what can be said about a larger software project?
Any software project is a veritable rat's nest of references, just a large database.
How many design bugs does a software project have that can be shown to exist from code organization alone,
before we even run it? 
We are forced into the idea of converting our software to relational form, fixing anomalies at that level, and then
re-creating source code from that representation. But can it be done?

OpenACR says that yes, not only can it be done, but you get an insanely extensible, compact system with almost
magical properties. Below I show how it gets started, from scarch.
 
### Creating Some Tuples

The first thing we want is a way to move database records down to the level of text lines, so we can work with 
them outside of MariaDB or any other database. Perl and sed and grep are great tools, and so is git. 
We want to keep records in plain text files, version them, and treat them like source code. 
We'll need a tool that manipulates these text records and does various useful things with them.

We introduce the idea of a super-simple tuple, or ssim tuple, that consists
of a type tag, and key-value pairs on a single line.

    test.project  project:project1

The first word, `test.project` is the tuple head, or type tag.
If you think of a shell command line, it's the name of the command being invoked. 
But here, `test` is the database name, and `project` is the table name. Instead of `database`, 
we will say `namespace`, or `ns`.

We are going to keep these, and only these records in the file

    data/test/project.ssim
    
Similarly, we will be keeping parts and partprojs in files

    data/test/part.ssim
    data/test/partproj.ssim

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

And that's it for now.

### Describing The Tuple Schema

Now here comes the interesting part.

If we want our query tool to find this data, we would need to specify exactly 
which ssimfiles and namespaces we want in our system. We would also need a description 
of the various columns, and what they refer to.

We will place the list of ssimfiles in the ssimfile called `data/dmmeta/ssimfile.ssim` 
and in it, by the above convention, each tuple will start work the word `dmmeta.ssimfile`. 
The ns `dmmeta` is short for *Data Model Meta-information*.
The entries will be sorted alphabetically.

    $ cat > data/dmmeta/ssimfile.ssim << EOF
    dmmeta.ssimfile  ssimfile:dmmeta.ssimfile
    dmmeta.ssimfile  ssimfile:test.part
    dmmeta.ssimfile  ssimfile:test.partproj
    dmmeta.ssimfile  ssimfile:test.project
    EOF

Notice that the `dmmeta.ssimfile` entry, referring to the file itself, is
part of that list.

We will then place the list of known namespaces in the
table `data/dmmeta/ns.ssim` and these will all be tagged as `dmmeta.ns`.
But because we already have a namespace `dmmeta`, we will include its definition.
So far we have two namespaces:

    $ cat > data/dmmeta/ns.ssim << EOF
    dmmeta.ns  ns:dmmeta
    dmmeta.ns  ns:test
    EOF

Since we want to write a C++ programs with these tuples, we will need a name for the type 
to use in C++. In addition, we anticipate having C types that have no associated ssimfiles.
Using the CamelCase convention for C++ type names, we create a new table `dmmeta.ctype`.
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

It is not necessary to align the columns in a ssimfile, but it looks nicer.
Also, leading and trailing whitespace on each line will be ignored. And the order of the lines 
shouldn't matter either. 

At this point, the only thing that's missing is some description of the columns.

### Describing The Columns

We will need a new table, with one record for every type of column we have used so far.
We will call it `dmmeta.field`, so now we need to add field's `ssimfile` and `ctype` lines to 
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
    $ echo 'dmmeta.ssimfile  ssimfile:dmmeta.reftype  ctype:dmmeta.Reftype ' >> data/dmmeta/ssimfile.ssim
    $ cat > data/dmmeta/reftype.ssim <<EOF
    dmmeta.reftype  reftype:Val
    dmmeta.reftype  reftype:Pkey
    EOF
    
In order to describe value types, we will add a single type called `algo.Smallstr50`.
This will be our equivalent of `varchar(50)`.
We create a new namespace called `algo`, because we want the `dmmeta`
namespace to be used exclusively for ctypes that have ssimfiles associated with them.

    $ echo 'dmmeta.ns ns:algo' >> data/dmmeta/ns.ssim
    $ echo 'dmmeta.ctype ctype:algo.Smallstr50' >> data/dmmeta/ctype.ssim

And now we are ready to describe the fields. The table below includes descriptions of
all fields we referenced so far.

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
    
If your head is not at least a little bit exploding when you see the line:

    dmmeta.field  field:dmmeta.Field.reftype      arg:dmmeta.Reftype  reftype:Pkey

Then you need to read the above text again. 
All of our moves so far have been forced. We are simply adding descriptions
of things as they pop up.

We are almost done. All that remains is adding a description of the constraint for substrings
of fields. For instance, a substring of the field's primary key must refer to a valid ctype,
and similarly a substring of ctype's primary key must refer to a namespace.
For this, we will add some records describing the substrings, and place them all 
into the appropriate ssimfiles:

    dmmeta.ctype  ctype:dmmeta.Substr  comment:"Computed field"
      dmmeta.field  field:dmmeta.Substr.field     arg:dmmeta.Field    reftype:Pkey  dflt:""  comment:""
      dmmeta.field  field:dmmeta.Substr.expr      arg:algo.Smallstr50 reftype:Val   dflt:""  comment:""
      dmmeta.field  field:dmmeta.Substr.srcfield  arg:dmmeta.Field    reftype:Pkey  dflt:""  comment:""

    dmmeta.ssimfile  ssimfile:dmmeta.substr  ctype:dmmeta.Substr

And now we can describe the fields which are implicitly contained in other fields:

    dmmeta.field  field:dmmeta.Field.ctype  arg:dmmeta.Ctype  reftype:Pkey  dflt:""  comment:"enclosing structure"
      dmmeta.substr  field:dmmeta.Field.ctype  expr:.RL  srcfield:dmmeta.Field.field

    dmmeta.field  field:dmmeta.Ctype.ns  arg:dmmeta.Ns  reftype:Pkey  dflt:""  comment:"translates to c++ namespace"
      dmmeta.substr  field:dmmeta.Ctype.ns  expr:.RL  srcfield:dmmeta.Ctype.ctype

And place these records into their corresponding ctypes as well.
The expression `.RL` means "scan for character . from the Right. then take everything to the Left of the found character".
We will also allow expressions with any number of triples and any search characters, e.g. `/RR@LL` or `.LR.LR.LR`.
We may want to describe additional substrings (such as field's name), but this should be sufficient
to show the process of building up the concepts.

### Adding Tools

At this point, it is clear what kinds of actions we are performing on these data sets. Let's introduce some tools.

First, we need a query tool that can fetch records of a given type, and then find all related
records as a transitive closure over the Pkey references. Because the tool 
automatically cross-references all the tables in our system, we will call it `Auto Cross Reference`
or `acr`. We will equip `acr` with options like `-check` so that it can find any broken
`Pkey` references, and other options to make it useful. We will describe all the command-line options
as fields of the ctype `command.acr`.

For shell command-line completion, we will write `acr_compl`, which will now be able to auto-complete
both arguments to acr, and even queries, by reading appropriate ssim files.

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

(The difference between `acr` and `dmmeta` is that `acr` is a database of structs in RAM
while `dmmeta` is a database of ssim files on disk).

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
The remaining 5% could also be generated `as-is`, just by `printf()`-ing the lines verbatim, but this 
would actually be counter-productive, and no more impressive than `tar cvf`-ing the whole thing. 
It is only interesting to generate code that's used in more than one place.

In subsequent chapters, I will describe the `amc` memory model for an executable, and things like pools 
(for holding records) and x-refs (for creating group-by's). 

The presence of `ssimfile` in our data set was dictated by the way we organized the data set.
The presence of `amc` in this model was dictated by our desire to use C++ to write the query tool `acr`,
which keeping the data description in a single place.

There is a more general pattern here:

### Representation & Manipulation

We can represent any concept as a point in a multi-dimensional sparse space of tuples.
Whenever we extend our representable universe with
additional concepts, such as a physcial set of C++ files on disk, or a set of dev and production environments
with some configuration, we include its relational description, or map, as a set of tables in our data set. 
We then write tools that enforce a unidirectional or bidirectional correspondence between this new object and
the records that describe it. In this way we make the object programmable and editable with the
same fundamental set of tools we use on the tuples themselves.

### Ordering Dependencies

There are many examples of systems in the software world
where the rules created by a system begin to apply to the system itself.
Let's briefly consider some such systems.

* The most famous one is the LISP interpreter as expressed in LISP itself,
as expressed by the famous top-level expression `(loop (print (eval (read))))`. 
* Another example is the [self-compiling compiler](#https://en.wikipedia.org/wiki/Bootstrapping_(compilers)).
All compiled languages have one (and *no* interpreted language has one).
* And a third example is the template meta-programming sublanguage of C++,
using which you can manipulate the very types from which the underlying
C++ program is written.

Joining this list is OpenACR, which is of a different kind: 
it not only generates most of its own source code and lets you modify this source code with
plain command-line tools like sed and awk, and even SQL, without introducing
any new language or an interpreter; but it serves as a sort of planter from which
you can grow other applications that share these same properties. 

Let's go back to our three examples and consider one cycle of application of them.

* When a LISP interpreter written in LISP interprets more LISP, it is
qualitatively different: it is slower. It can only
run smaller jobs than its parent. In order to be the same, the homoiconic interpreter
would have to be vastly different; At the very least it would have to contain a
memory model of the underlying computer and its file system, so it could then target them.
That's why no interpreted language today uses a self-hosting interpreter -- nobody
wants to pay for the slowdown.
* The output of a self-compiling compiler is an object file -- unreadable
for all practical purposes. So even though the compiler can compile itself, and the resulting
compiler can run even faster than the one before (the opposite of what happens in LISP),
this is a one-time gain.
* Finally, the C++ template sublanguage, our third example, is strictly less powerful
than its parent language; you can't loop over the fields of a struct, 
or check how many structs are defined, or if the name of a function contains an uppercase S.
Neither the C++ language, nor its template sublanguage contain words that 
describe themselves.

So, after one cycle of application, you get to a new and better place, but that place is
either inaccessible (e.g. object file), or built at some unmaintainable expense;
in either case, the gains are temporary. Yet *it is* possible to lock them in.
For that, we need tools whose input is readable and writable
by both human and the machine, and where the system of names applies equally well to the 
description of itself and the tools. 

When the input format is both machine and human-readable and most of the source code
is generated, any tool works with almost any other tool. 

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
    'mdbg "mdbg acr"' debugs the debugger debugging acr (in principle)
    
And of course, even though this is fun, 
the point of these tools is not to compile themselves; 
That economy is just a by-product of some naming conventions.
The point is to allow the creation of new applications,
using plain-text files to describe new domains while continuing
to apply the same small set of tools -- bash, acr, perl, etc. on each cycle.

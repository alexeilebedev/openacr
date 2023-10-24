## acr_ed: ACR Editor Set of useful recipes, uses acr, abt, git, and other tools



### Syntax

```
acr_ed: ACR Editor Set of useful recipes, uses acr, abt, git, and other tools
Usage: acr_ed [options]
    OPTION      TYPE    DFLT    COMMENT
    -in         string  "data"  Input directory or filename, - for stdin
    -create                     Create new entity (-finput, -target, -ctype, -field)
    -del                        Delete mode
    -rename     string  ""      Rename to something else
    -finput                     Create in-memory table based on ssimfile
    -foutput                    Declare field as an output
    -srcfile    string  ""          Create source file
    -gstatic                    Like -finput, but data is loaded at compile time
    -indexed                    (with -finput) Add hash index
    -target     string  ""      Create new target
    -nstype     string  "exe"   (with -create -target): exe,lib,etc.
    -ctype      string  ""      Create new ctype
    -pooltype   string  ""       Pool reftype (Lary,Lpool etc) for finput/ctype
    -ssimfile   string  ""        Ssimfile for new ctype
    -subset     string  ""        Primary key is a subset of this ctype
    -subset2    string  ""        Primary key is also a subset of this ctype
    -separator  string  "."         Key separator
    -field      string  ""      Create field
    -arg        string  ""        Field type (e.g. u32, etc), (with -ctype) add the base field
    -dflt       string  ""        Field default value
    -anon                         Anonymous field (use with command lines)
    -bigend                       Big-endian field
    -cascdel                      Field is cascdel
    -before     string  ""        Place field before this one
    -substr     string  ""        New field is a substring
    -alias                      Create alias field (requires -srcfield)
    -srcfield   string  ""        Source field for bitfld/substr
    -fstep      string  ""        Add fstep record
    -inscond    string  "true"    Insert condition (for xref)
    -reftype    string  ""        Reftype (e.g. Val, Thash, Llist, etc)
    -hashfld    string  ""          (-reftype:Thash) Hash field
    -sortfld    string  ""          (-reftype:Bheap) Sort field
    -unittest   string  ""      Create unit test, <ns>.<functionname>
    -citest     string  ""      Create CI test
    -cppfunc    string  ""      Field is a cppfunc, pass c++ expression as argument
    -xref                           X-ref with field type
    -via        string  ""            X-ref argument (index, pointer, or index/key)
    -write                      Commit output to disk
    -e                           (with -create -unittest) Edit new testcase
    -comment    string  ""      Comment for new entity
    -sandbox                    Make changes in sandbox
    -test                       Build resulting changes, run tests
    -showcpp                    (With -sandbox), show resulting diff
    -msgtype    string  ""      (with -ctype) use this msgtype as type
    -anonfld                    Create anonfld
    -verbose    int             Verbosity level (0..255); alias -v; cumulative
    -debug      int             Debug level (0..255); alias -d; cumulative
    -help                       Print help and exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig

```

### Description

`Acr_ed` is a cheat sheet generator, for common tasks such as creating
, deleting, and renaming targets, ctypes, ssimfiles, xrefs, and source files.

When invoked, `acr_ed` spits out an executable script to stdout. With
`-write`, the script is executed (this has the same effect as piping output
to sh).

Optionally, `acr_ed` can make all changes in a sandbox (with `-sandbox` flag), 
showing the diff between current and new versions. With `-test` flag, it builds 
all main executables and runs tests. With `-showcpp` flag, it shows a colored diff
between the current directory and the sandbox after executing the transaction in sandbox.

### Full list of actions

The following is the full list of actions acr_ed supports
```
inline-command: acr edaction -report:N | ssimfilt -t -field:edaction -field:comment
EDACTION         COMMENT
Create_Citest    -create -citest <citest>
Create_Ctype     -create -ctype <ctype> [-subset <ctype> [-subset2 <ctype2> -separator <char>]] [-pooltype <pooltype>] [-indexed]
Create_Field     -create -field <field> -arg <ctype> -reftype <reftype> [-xref [-via <via>]] [-anonfld] [-fbigend] ...
Create_Finput    -create -finput -target <target> -ssimfile <ssimfile>
Create_Srcfile   -create -srcfile <filename.(h|md|cpp)>
Create_Ssimfile  -create -ssimfile <ssimfile> [-subset <ctype> [-subset2 <ctype2> -separator <char>]]
Create_Target    -create -target <target>
Create_Unittest  -create -unittest <unittest>
Delete_Ctype     -del -ctype <ctype>
Delete_Field     -del -field <field>
Delete_Srcfile   -del -srcfile <srcfile>
Delete_Ssimfile  -del -ssimfile <ssimfile
Delete_Target    -del -target <target>
Rename_Ctype     -ctype <ctype> -rename <newname>
Rename_Field     -field <field> -rename <newname>
Rename_Srcfile   -srcfile <srcfile> -rename <newname>
Rename_Ssimfile  -ssimfile <ssimfile> -rename <newname>
Rename_Target    -target <target> -rename <newtarget>

```

More documentation is provided below

#### Creating a field

Syntax:
```
acr_ed -create -field:<field> -arg:<ctype>
```

Sub-options for field creation:
* -comment <comment>
* -dflt <c++ expression>
* -reftype <reftype>
* -before <field>
* -xref
* -via <field[/field]>
* -inscond <c++ expr>
* -fstep
* -substr:<pathcomp> -srcfield:<field>
* -anon
* -cascdel
* -bigend
* -hashfld <field> -- use with ind_ fields
* -sortfld <field> -- use with bh_ or tr_ fields
* -cppfunc <c++ expr> -- field is a computed field with specified c++ expression yielding the value

When creating a field, the Reftype can be guessed automatically based on the field name.
The following mappings are defined:

```
inline-command: acr fprefix -report:N | ssimfilt -t
FPREFIX  REFTYPE  COMMENT
bh       Bheap    Binary heap
c        Ptrary   Pointer array
cd       Llist    Circular doubly linked list
cdl      Llist    Circular doubly linked LIFO list
cnt      Count    Count of items
cs       Llist    Circular singly linked list
csl      Llist    Circular singly linked LIFO list
ind      Thash    Hash table (index)
p        Upptr    Up-pointer: equivalent of Pkey reference for in-memory model
tr       Atree    AVL tree
zd       Llist    Zero-terminated doubly linked list
zdl      Llist    Zero-terminated doubly linked LIFO list
zs       Llist    Zero-terminated singly linked list
zsl      Llist    Zero-terminated singly linked LIFO list

```

#### Create a command-line option

Command-line flags are regular fields. The command line for process `X` is `command.X`.
Thus, to add a new command-line option for X, use
```
acr_ed -create -field:command.X.fname -arg:<ctype>
```

* Specify `-anonfld` to create a nameless (positional) argument.
* Use `-reftype Tary` to create a flag that can be specified multiple times (an array).
* Use `-alias -srcfield:<field>` to create an option that is synonymous with another option.

To create an option that will select a key from an existing table `<ns.Table>`, use

```
acr_ed -create -field:command.X.t -arg:ns.Table -reftype:Pkey
```
After creating this field, tab-completion in the shell will automatically pick up
the change (since it reads the field table) and offer completions based on the table.
You can also use `reftype:RegxSql`.

A command-line option is considered required if the `dflt` expression of the field is empty.
Fields of type `bool` are never considered required.

#### Create a substring directly

To create a field named `field` that is extracted as a substring from another source field,
use `-substr:<pathcomp> -srcfield:<field>:

    $acr_ed -create -field ns.Name.field -substr .LL -srcfield ns.Name.field2

### Create A New In-Memory Table

To create a new table in a program, use `-create -ctype:<ctype> -pooltype:<pooltype>`.
This adds a pool of a given type (typically `Tpool` or `Lary`).
To throw in a hash index, specify `-indexed`.
Example:

    $ acr_ed -create -ctype sample.FTable -subset u32 -pooltype Tpool -indexed
    bin/acr  '' -insert:Y -check:Y -write:Y -t:Y -rowid:Y << EOF
    dmmeta.ctype  ctype:sample.FTable  comment:""
    dmmeta.field  field:sample.FTable.table  arg:u32  reftype:Val  dflt:""  comment:""
    dmmeta.field  field:sample.FDb.table  arg:sample.FTable  reftype:Tpool  dflt:""  comment:""
    dmmeta.field  field:sample.FDb.ind_table  arg:sample.FTable  reftype:Thash  dflt:""  comment:""
    dmmeta.thash  field:sample.FDb.ind_table  hashfld:sample.FTable.table  unique:Y  comment:""
    dmmeta.xref  field:sample.FDb.ind_table  inscond:true  via:""
    EOF

This is equivalent to executing the following commands in order:
```
acr_ed -create -ctype sample.FTable -write
acr_ed -create -field sample.FTable.table -arg i32 -write
acr_ed -create -field sample.FDb.table -arg sample.FTable -reftype Tpool -write
acr_ed -create -field sample.FDb.ind_table -arg sample.FTable -hashfld sample.FTable.table -xref -inscond:true -write
```

### Add An Input To Program

To add a table to the list of inputs of a program (as specified by the `finput` table)
use `acr_ed -create -finput -target:<ns> -ssimfile:<ssimfile>`.
This creates a new ctype in the target process, which copies all the fields from the ssimfile.
It also creates a pool for the ctype in the in-memory database (FDb).

Example:

    $ acr_ed -create -finput -target sample -ssimfile dev.gitfile
    acr_ed.create_finput  target:sample  ssimfile:dev.gitfile
    bin/acr  '' -insert:Y -check:Y -write:Y -t:Y -rowid:Y << EOF
    dmmeta.ctype  ctype:sample.FGitfile  comment:""
    dmmeta.field  field:sample.FGitfile.base  arg:dev.Gitfile  reftype:Base  dflt:""  comment:""
    dmmeta.field  field:sample.FDb.gitfile  arg:sample.FGitfile  reftype:Lary  dflt:""  comment:""
    dmmeta.finput  field:sample.FDb.gitfile  extrn:N  update:N  strict:Y  comment:""
    #  Proposed change
    #
    #
    #     / sample.FDb
    #     |
    #     |Lary gitfile-->/ sample.FGitfile
    #     -               |
    #                     |
    #                     -
    EOF
    ...
    
* With `-indexed` option, a hash index is thrown in
* With `-pooltype` option, the allocator for the new type can be specified.

### Create An Index

`acr_ed` can create both global and partitioned indexes. Global indexes are placed in the FDb (global struct).
Partitioned indexes are placed in any ctype. Indexes are fields.
Here is an example:
```
acr_ed -create -field:ns.FTable.ind_xyz -arg:ns.FXyz -hashfld:ns.FXyz.field
```

Often, you can simply specify the field name to create, and `acr_ed` automatically determines
which type it will refer to, based on the name.

`acr_ed` can also create an automatic x-reference by analyzing all the access paths between
the two tables. The rule is as follows:
You can create an index in table A of records of type B if there exists a function that can locate B given A.
Since the global database (`FDb`) is always accessible, you can always create a global index.
To create a partitioned index, `acr_ed` will perform a search over fields of B and over global hashes
to see if any of the keys can be used to find an instance of A. If no paths exist, it's an error.
If no paths, or more than one path exists, one must be specified via the `-via` argument.
`-via` can be a pointer field in B, or
an expression in the form `hash_field/key`. Examples below will illustrate the difference.

#### Create an Upptr (reference)

To illustrate the difference between an Upptr (reference) and a Ptr (any other point),
we need to think of the relational model first. The result of looking up a Pkey reference in the relational
model is called Upptr in the in-memory model. A cross-reference, i.e. a pointer, hash, or other index
that is updated *in response* to this reference being established is called a cross-reference or x-ref.

Let's start by creating a new executable with an in-memory database, called `samp_xref`. We'll
input two tables, `ns` and `ctype`. These are related because `ctype` key contains a reference to `ns`.

    $ acr_ed -create -target samp_xref -write
    $ acr_ed -create -finput -ssimfile dmmeta.ns -target samp_xref -indexed -write
    $ acr_ed -create -finput -ssimfile dmmeta.ctype -target samp_xref -write

Let's check the structure of the in-memory database:

```
    $ amc_vis samp_xref.%                                     
                                     
     / samp_xref.FDb                 
     |                               
     |Lary ctype-->/ samp_xref.FCtype
     |             -                 
     |                               
     |                               
     |Lary ns------->/ samp_xref.FNs 
     |Thash ind_ns-->|               
     -               |               
                     |               
                     -               
```
                                                                                       
We can now create a pointer from `ctype` to `ns` directly. This is called an `Upptr` because
`ns` logically is above ctype.

    $ acr_ed -create -field samp_xref.FCtype.p_ns

This is equivalent to writing:

    $ acr_ed -create -field samp_xref.FCtype.p_ns -via samp_xref.FDb.ind_ns/dmmeta.Ctype.ns
    
In this case, the `-via` parameter is omitted to `acr_ed` because the path is unique and can be guessed.
We can also create a Ptr reference from `ns` down to `ctype:

    $ acr_ed -create -field samp_xref.FNs.c_ctype

The resulting structure is as follows:
                                                     
```                                                     
    / samp_xref.FDb                                  
    |                                                
    |Lary ctype------------------->/ samp_xref.FCtype
    |                              |                 
    |Lary ns------->/ samp_xref.FNs|                 
    |Thash ind_ns-->|              |                 
    -               |              |                 
                    |              |                 
                    |Ptr c_ctype-->|                 
                    |<-------------|Upptr p_ns       
                    |              -                 
                    |                                
                    -                                

```

Other index types are available.

#### Create A Hash Table

    $ acr_ed -create -field sample.FDb.ind_table
    $ acr_ed -create -field sample.FDb.ind_table -hashfld <fieldname>

#### Create A Binary Heap

    $ acr_ed -create -field sample.FDb.bh_table
    $ acr_ed -create -field sample.FDb.bh_table -sortfld <fieldname>

#### Create a circular, doubly linked list

The structure of the linked list is described by the field prefix.
amc supports 32 types of linked lists: singly and doubly linked, circular or zero-terminated,
with default tail and head insertion, with and without a count, and with or without a tail pointer.

    $ acr_ed -create -field sample.FDb.cd_table
    $ acr_ed -create -field sample.FDb.cd_table

The full list of linked list types can be gleaned from this table:

```
inline-command: acr listtype | ssimfilt ^ -t
LISTTYPE  CIRCULAR  HAVEPREV  INSTAIL  COMMENT
cd        Y         Y         Y        Circular doubly-linked queue
cdl       Y         Y         N        Circular double-linked lifo (stack)
cs        Y         N         Y        Circular singly-linked queue
csl       Y         N         N        Circular singly-linked lifo (stack)
zd        N         Y         Y        Zero-terminated doubly-linked queue
zdl       N         Y         N        Zero-terminated doubly-linked lifo (stack)
zs        N         N         Y        Zero-terminated singly-linked queue
zsl       N         N         N        Zero-terminated singly-linked lifo (stack)

```

The arguments `havetail` and `havecount` are specified directly in the `llist` record which is required for a linked list.

#### Create An AVL Tree

    $ acr_ed -create -field sample.FDb.tr_table
    $ acr_ed -create -field sample.FDb.tr_table -sortfld <fieldname>

#### Create a Pointer array

    $ acr_ed -create -field sample.FDb.c_table
    $ acr_ed -create -field sample.FDb.c_table

#### Conditional X-Ref

There are two phases to the creation of each record in the in-memory databases created by amc.
One is to allocate the record and fill out its fields, and the other is to call `_XrefMaybe`.
The x-reference step inserts the record into any indexes, as described in the schema. The code for x-referencing
is generated by amc. By default, xrefs are always applied. But sometimes it's useful not to insert
a record into an index until later time (as determined by the programmer). In this case, an `-inscond` can be supplied,
which ends up in the `dmmeta.xref.inscond` field and becomes an if-statement to be tested before doing the xref.

    $ acr_ed -create -field sample.FDb.ind_table -inscond false

#### Create a source file

Syntax:
```
acr_ed -create -srcfile:<filename> [-target:<target>]
```

This option is used to create a .cpp, .h, or .md (readme) file.
When creating a source file, `acr_ed` automatically determines which target
this file will belong to based on the the other files in the same directory.
If there is ambiguity, specify `-target ...` argument
Headers are considered source files.

    $ acr_ed -create -srcfile cpp/...path.cpp
    ...
    $ acr_ed -create -srcfile include/path.h

With `-e` option, the resulting file is opened for editing.

#### Rename a source File

Syntax:
```
acr_ed -srcfile:<filename> -rename:<newfilename> [-target:<target>]
```

This syntax can be used to rename a source file or move it between targets (or both)

#### Delete a source File

To delete a source file, use

```
acr_ed -del -srcfile:<filename>
```

### Inputs

`acr_ed` takes the following tables on input:
```
CTYPE                COMMENT
dmmeta.Ns            Namespace (for in-memory database, protocol, etc)
dmmeta.Dispsigcheck  Check signature of input data against executable's version
dmmeta.Ctype         Conceptual type (or C type)
dmmeta.Field         Specify field of a struct
dmmeta.Typefld       Specifies which field of a message carries the type
dev.Target           Build target
dev.Targsrc          List of sources for target
dmmeta.Nsdb          Annotate ssimdb namespaces
dmmeta.Ssimfile      Ssim tuple name for structure
dev.Sbpath
dmmeta.Pack          Request byte-packing of structure fields
dmmeta.Fprefix       Mapping between field prefix and Reftype
dmmeta.Listtype      Specify structure of linked list based on field prefix
dmmeta.Cstr          Specify that type behaves like a string
dmmeta.Cpptype       Specify whether a ctype can be passed by value, and other c++ options
dmmeta.Cfmt          Specify options for printing/reading ctypes into multiple formats
```

### Tests

The following component tests are defined for `acr_ed`.
These can be executed with `atf_comp <comptest> -v`
```
COMPTEST                    COMMENT
acr_ed.CreateCtype
acr_ed.CreateSsimfileBadNs



```


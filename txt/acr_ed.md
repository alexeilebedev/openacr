## acr_ed: Acr Editor

### Syntax

```
inline-command: acr_ed -h


acr_ed: ACR Editor Set of useful recipes, uses acr, abt, git, and other tools
Usage: acr_ed [options]
    -in         string  Input directory or filename, - for stdin. default: "data"
    -create             Create new entity (-finput, -target, -ctype, -field). default: false
    -del                Delete mode. default: false
    -rename     string  Rename to something else
    -replace            Use acr -replace (default is -insert, fails on duplicate). default: false
    -finput             Create in-memory table based on ssimfile. default: false
    -foutput            Declare field as an output. default: false
    -srcfile    string      Create source file
    -gstatic            Like -finput, but data is loaded at compile time. default: false
    -indexed            (with -finput) Add hash index. default: false
    -target     string  Create new target
    -nstype     string  (with -create -target): exe,lib,etc.. default: "exe"
    -ctype      string  Create new ctype
    -pooltype   string   Pool reftype (Lary,Lpool etc) for finput/ctype
    -ssimfile   string    Ssimfile for new ctype
    -subset     string    Primary key is a subset of this ctype
    -subset2    string    Primary key is also a subset of this ctype
    -separator  string      Key separator. default: "."
    -field      string  Create field
    -arg        string    Field type (e.g. u32, etc), (with -ctype) add the base field
    -dflt       string    Field default value
    -anon                 Anonymous field (use with command lines). default: false
    -bigend               Big-endian field. default: false
    -cascdel              Field is cascdel. default: false
    -before     string    Place field before this one
    -substr     string    New field is a substring
    -srcfield   string    Source field for bitfld/substr
    -fstep      string    Add fstep record
    -inscond    string    Insert condition (for xref). default: "true"
    -reftype    string    Reftype (e.g. Val, Thash, Llist, etc)
    -hashfld    string      (-reftype:Thash) Hash field
    -sortfld    string      (-reftype:Bheap) Sort field
    -unittest   string  Create unit test, <ns>.<functionname>
    -citest     string  Create CI test
    -cppfunc    string  Field is a cppfunc, pass c++ expression as argument
    -xref                   X-ref with field type. default: false
    -via        string        X-ref argument (index, pointer, or index/key)
    -write              Commit output to disk. default: false
    -e                   (with -create -unittest) Edit new testcase. default: false
    -comment    string  Comment for new entity
    -sandbox            Make changes in sandbox. default: false
    -test               Build resulting changes, run tests. default: false
    -showcpp            (With -sandbox), show resulting diff. default: false
    -msgtype    string  (with -ctype) use this msgtype as type
    -anonfld            Create anonfld. default: false
    -verbose            Enable verbose mode
    -debug              Enable debug mode
    -version            Show version information
    -sig                Print SHA1 signatures for dispatches
    -help               Print this screen and exit

```

### Description

`Acr_ed` is a cheat sheet generator, for common tasks such as creating
, deleting, and renaming targets, ctypes, ssimfiles, xrefs, and source files.

By default, `acr_ed` spits out an executable script to stdout. With
`-write`, the script is executed (this has the same effect as piping output
to sh).

Optionally, `acr_ed` can make all changes in a sandbox (with `-sandbox` flag), 
showing the diff between current and new versions. With `-test` flag, it builds 
nall main executables and runs tests. With `-showcpp` flag, it shows a colored diff
between the current directory and the sandbox after executing the transaction in sandbox.

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

This option is used to create a .cpp, .h, or .md (readme) file.
When creating a source file, `acr_ed` automatically determines which target
this file will belong to based on the the other files in the same directory.
If there is ambiguity, specify `-target ...` argument
Headers are considered source files.

    $ acr_ed -create -srcfile cpp/...path.cpp
    ...
    $ acr_ed -create -srcfile include/path.h

With `-e` option, the resulting file is opened for editing.

#### Rename Source File

    $ acr_ed -srcfile cpp/path.cpp -rename cpp/path2.cpp
    ...

#### Delete Source File

    $ acr_ed -del -srcfile cpp/path.cpp
    ...

#### Create Readme file

    $ acr_ed -create -srcfile txt/abcd.md
    ...

### Ctypes

#### Create A Ctype

    $ acr_ed -create -ctype ns.Name
    ...

#### Rename A Ctype

    $ acr_ed -ctype ns.Name -rename ns2.Name2
    ...

#### Create a Ctype with a single field
If `-reftype` is omitted, it will be guessed
If the field refers to a relational table, `reftype:Pkey` is picked by default
If the field looks like a value, `reftype:Val` is used;

    $ acr_ed -create -ctype ns.Name -subset ns2.Name2
    ...

#### Create a string field that's a cross-product of two other keys
If field type is omitted, `algo.Smallstr50` is picked.
This creates three fields: the requested one, and two substring fields that are extracted
from the created field as `fldfunc`s (in C++, this is implemented with a `_Get` function).
The arguments to `-subset` and `-subset2` can be either relational or values.
If the argument is relational (i.e. the ctype has an associated ssimfile), then `reftype:Pkey` is picked.
Otherwise, `reftype:Val` is picked. You can use `-subset i32`, and you will get an integer-valued field,
but it will be parsed from the parent field on every access.

    $ acr_ed -create -field ns.Name.field -subset ns2.Name2 -subset2 ns.Name3 -separator .
    ...

#### Create a substring directly
Create a field named `field` that is extracted as a substring from another source field.
The `substr` specification is described in txt/acr.md (look for Pathcomp)

    $ acr_ed -create -field ns.Name.field -substr .LL -srcfield ns.Name.field2
    
#### Delete A Ctype

    $ acr_ed -del -ctype ns.Name
    ...
    
### Add An Input To Program

    $ acr_ed -create -finput -target ns -ssimfile ns2.name
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
    
With `-indexed` option, a hash index is thrown in. This is pretty common.

### Create A New In-Memory Table

To create a new table in a program, use `-create -ctype ... -subset ... -pooltype`.
This adds a global pool of a given type (typically `Tpool` or `Lary`).
To throw in a hash index, specify `-indexed`

    $ acr_ed -create -ctype sample.FTable -subset u32 -pooltype Tpool -indexed
    bin/acr  '' -insert:Y -check:Y -write:Y -t:Y -rowid:Y << EOF
    dmmeta.ctype  ctype:sample.FTable  comment:""
    dmmeta.field  field:sample.FTable.table  arg:u32  reftype:Val  dflt:""  comment:""
    dmmeta.field  field:sample.FDb.table  arg:sample.FTable  reftype:Tpool  dflt:""  comment:""
    dmmeta.field  field:sample.FDb.ind_table  arg:sample.FTable  reftype:Thash  dflt:""  comment:""
    dmmeta.thash  field:sample.FDb.ind_table  hashfld:sample.FTable.table  unique:Y  comment:""
    dmmeta.xref  field:sample.FDb.ind_table  inscond:true  via:""
    EOF

### Create An Index
`acr_ed` can create both global and partitioned indexes. The rule is as follows:
You can create an index in table A of records of type B if there exists a function that can locate B given A.
Since the global database (`FDb`) is always accessible, you can always create a global index.
To create a partitioned index, `acr_ed` will perform a search over fields of B and over global hashes
to see if any of the keys can be used to find an instance of A. If no paths exist, it's an error.
If more than one path exists, it is also an error but only as far as the guessing is concerned.
You can specify the path from B to A using `-via` argument. `-via` can be a pointer field in B, or
an expression in the form `hash_field/key`. Examples will show the difference

#### Create an Upptr (reference)
Let's start by creating a new executable with an in-memory database, called `samp_xref`. We'll
input two tables, `ns` and `ctype`. These are related because `ctype` key contains a reference to `ns`.

    $ acr_ed -create -target samp_xref -write
    $ acr_ed -create -finput -ssimfile dmmeta.ns -target samp_xref -indexed -write
    $ acr_ed -create -finput -ssimfile dmmeta.ctype -target samp_xref -write

Let's check the structure of the in-memory database:

```
    $ amc_vis sampe_xref.%                                     
                                     
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
inline-command: acr listtype
dmmeta.listtype  listtype:cd   circular:Y  haveprev:Y  instail:Y  comment:"Circular doubly-linked queue"
dmmeta.listtype  listtype:cdl  circular:Y  haveprev:Y  instail:N  comment:"Circular double-linked lifo (stack)"
dmmeta.listtype  listtype:cs   circular:Y  haveprev:N  instail:Y  comment:"Circular singly-linked queue"
dmmeta.listtype  listtype:csl  circular:Y  haveprev:N  instail:N  comment:"Circular singly-linked lifo (stack)"
dmmeta.listtype  listtype:zd   circular:N  haveprev:Y  instail:Y  comment:"Zero-terminated doubly-linked queue"
dmmeta.listtype  listtype:zdl  circular:N  haveprev:Y  instail:N  comment:"Zero-terminated doubly-linked lifo (stack)"
dmmeta.listtype  listtype:zs   circular:N  haveprev:N  instail:Y  comment:"Zero-terminated singly-linked queue"
dmmeta.listtype  listtype:zsl  circular:N  haveprev:N  instail:N  comment:"Zero-terminated singly-linked lifo (stack)"
report.acr  n_select:8  n_insert:0  n_delete:0  n_update:0  n_file_mod:0
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

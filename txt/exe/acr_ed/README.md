## acr_ed - Script generator for common dev tasks


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Toc -->
&nbsp;&nbsp;&bull;&nbsp;  [Syntax](#syntax)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Description](#description)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Options](#options)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
&#128196; [acr_ed - Internals](/txt/exe/acr_ed/internals.md)<br/>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Toc -->

### Syntax
<a href="#syntax"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Syntax -->
```
acr_ed: Script generator for common dev tasks
Usage: acr_ed [options]
    OPTION      TYPE    DFLT    COMMENT
    -in         string  "data"  Input directory or filename, - for stdin
    -create                     Create new entity (-finput, -target, -ctype, -field)
    -del                        Delete mode
    -rename     string  ""      Rename to something else
    -finput                     Create in-memory table based on ssimfile
    -foutput                    Declare field as an output
    -srcfile    string  ""      Create/Rename/Delete a source file
    -gstatic                    Like -finput, but data is loaded at compile time
    -indexed                    (with -finput) Add hash index
    -target     string  ""      Create/Rename/Delete target
    -nstype     string  "exe"   (with -create -target): exe,lib,etc.
    -ctype      string  ""      Create/Rename/Delete ctype
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

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Syntax -->

### Description
<a href="#description"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Description -->

`Acr_ed` is a script generator for common tasks such as creating,
deleting, and renaming targets, ctypes, ssimfiles, xrefs, and source files.

When invoked, `acr_ed` spits out an executable script to stdout. With
`-write`, the script is executed (this has the same effect as piping output
through bash).

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

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Description -->

### Options
<a href="#options"></a>

<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Options -->
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

#### -create -- Create new entity (-finput, -target, -ctype, -field)
<a href="#-create"></a>

#### -del -- Delete mode
<a href="#-del"></a>

The `-del` option can be used with `-ctype`, `-field`, `-srcfile`, `-ssimfile`, `-target`.

#### -rename -- Rename to something else
<a href="#-rename"></a>

The `-rename` option can be used with `-ctype`, `-field`, `-srcfile`, `-ssimfile`, `-target`.
When renaming a ssimfile, the corresponding ctype is also renamed to the CamelCase version of
the ssimfile name. In addition, in all programs that use the ssimfile as an finput,
the corresponding in-memory type and its pool are renamed.

When renaming a target, all of its source files are moved to the new location `cpp/<target>/`.
All component tests are renamed, along with the readme. `acr_ed` uses `sed` speculatively
to partially rename namespace-qualified references inside source files.
When renaming a target, `acr_ed` renames to corresponding `dmmeta.ns` entry as well.

#### -finput -- Create in-memory table based on ssimfile
<a href="#-finput"></a>

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

#### -foutput -- Declare field as an output
<a href="#-foutput"></a>

#### -srcfile -- Create/Rename/Delete a source file
<a href="#-srcfile"></a>

Syntax:

    acr_ed -create -srcfile:<filename> [-target:<target>]

This option is used to create a .cpp, .h, or .md (readme) file.
When creating a source file, `acr_ed` automatically determines which target
this file will belong to based on the the other files in the same directory.
If there is ambiguity, specify `-target ...` argument
Headers are considered source files.

    $ acr_ed -create -srcfile cpp/...path.cpp
    ...
    $ acr_ed -create -srcfile include/path.h

With `-e` option, the resulting file is opened for editing.

To rename a source file, use

    acr_ed -srcfile:<filename> -rename:<newfilename> [-target:<target>]

#### -gstatic -- Like -finput, but data is loaded at compile time
<a href="#-gstatic"></a>

#### -indexed -- (with -finput) Add hash index
<a href="#-indexed"></a>

#### -target -- Create/Rename/Delete target
<a href="#-target"></a>

#### -nstype -- (with -create -target): exe,lib,etc.
<a href="#-nstype"></a>

#### -ctype -- Create/Rename/Delete ctype
<a href="#-ctype"></a>

To create a new record type in a program, use `-create -ctype:<ctype> -pooltype:<pooltype>`.
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

#### -pooltype --  Pool reftype (Lary,Lpool etc) for finput/ctype
<a href="#-pooltype"></a>

#### -ssimfile --   Ssimfile for new ctype
<a href="#-ssimfile"></a>

#### -subset --   Primary key is a subset of this ctype
<a href="#-subset"></a>

#### -subset2 --   Primary key is also a subset of this ctype
<a href="#-subset2"></a>

#### -separator --     Key separator
<a href="#-separator"></a>

#### -field -- Create field
<a href="#-field"></a>

Syntax:
```
acr_ed -create -field:<field> -arg:<ctype>
```

Sub-options for field creation:
- -arg <ctype>
- -comment <comment>
- -dflt <c++ expression>
- -reftype <reftype>
- -before <field>
- -xref
- -via <field[/field]>
- -inscond <c++ expr>
- -fstep
- -substr:<pathcomp> -srcfield:<field>
- -anon
- -cascdel
- -bigend
- -hashfld <field> -- use with ind_ fields
- -sortfld <field> -- use with bh_ or tr_ fields
- -cppfunc <c++ expr> -- field is a computed field with specified c++ expression yielding the value

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

#### -arg --   Field type (e.g. u32, etc), (with -ctype) add the base field
<a href="#-arg"></a>

#### -dflt --   Field default value
<a href="#-dflt"></a>

#### -anon --   Anonymous field (use with command lines)
<a href="#-anon"></a>

#### -bigend --   Big-endian field
<a href="#-bigend"></a>

#### -cascdel --   Field is cascdel
<a href="#-cascdel"></a>

#### -before --   Place field before this one
<a href="#-before"></a>

#### -substr --   New field is a substring
<a href="#-substr"></a>

To create a field named `x` that is extracted as a substring from another field `y`,
use `-substr:<pathcomp> -srcfield:<field>:

    $ acr_ed -create -field ns.Name.x -substr .LL -srcfield ns.Name.y

#### -alias -- Create alias field (requires -srcfield)
<a href="#-alias"></a>

#### -srcfield --   Source field for bitfld/substr
<a href="#-srcfield"></a>

#### -fstep --   Add fstep record
<a href="#-fstep"></a>

#### -inscond --   Insert condition (for xref)
<a href="#-inscond"></a>

Create conditional x-reference. The arguent to the option is a c++ expression.
The x-reference becomes conditional on the expression.
For example:

    $ acr_ed -create -field sample.FDb.ind_table -inscond false

With this xref, new records added to `table` are not automatically inserted into the
`ind_table` hash. That step has to be done manually later with `ind_table_Insert(...);`.

#### -reftype --   Reftype (e.g. Val, Thash, Llist, etc)
<a href="#-reftype"></a>

- Hash tables

    $ acr_ed -create -field sample.FDb.ind_table
    $ acr_ed -create -field sample.FDb.ind_table -hashfld <fieldname>

- Binary Heaps

    $ acr_ed -create -field sample.FDb.bh_table
    $ acr_ed -create -field sample.FDb.bh_table -sortfld <fieldname>

- Linked lists

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

- AVL Trees

    $ acr_ed -create -field sample.FDb.tr_table
    $ acr_ed -create -field sample.FDb.tr_table -sortfld <fieldname>

- Pointer arrays

    $ acr_ed -create -field sample.FDb.c_table
    $ acr_ed -create -field sample.FDb.c_table

- Upptr (reference)

Pointer to a record that must previously exist. All indexing reftypes except Upptr are cross-reference types,
i.e. they are used in response to creation of some other record. Upptr is the result of a lookup of some key,
and so it is used in response to creation of the record containing the field.

To illustrate the difference between an Upptr (reference) and a Ptr (any other pointer),
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
`ns` logically is above ctype (it must exist when the ctype record is created).

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

#### -hashfld --     (-reftype:Thash) Hash field
<a href="#-hashfld"></a>

Specify a hash field when creating a hash. By default, the primary key of
the record is used.

#### -sortfld --     (-reftype:Bheap) Sort field
<a href="#-sortfld"></a>

Specify a sort field for Atree or Bheap. By default, the primary key of the record is used.

#### -unittest -- Create unit test, <ns>.<functionname>
<a href="#-unittest"></a>

#### -citest -- Create CI test
<a href="#-citest"></a>

#### -cppfunc -- Field is a cppfunc, pass c++ expression as argument
<a href="#-cppfunc"></a>

#### -xref --     X-ref with field type
<a href="#-xref"></a>

Create a x-reference between two in-memory records.

The syntax is

    acr_ed -create -field <field> -xref -via <via>
    
There are two phases to the creation of each record in the in-memory databases created by amc.
One is to allocate the record and fill out its fields using `rec_Alloc`, and the other is to call `rec_XrefMaybe`.
Also see `-via` and `-inscond`.

#### -via --       X-ref argument (index, pointer, or index/key)
<a href="#-via"></a>

The `-via` argument is either a single pointer field belonging to the child record,
or a `<hash>/<key>` pair. This is best illustrated by an example.
This is best illustrated by an example. Let's say the target x loads ssimfiles y.a, y.b and y.c, where
c is a cross product of a and b. Then, we may desire to x-reference (group-by) c records by a.
This would be achieved as follows:

    acr_ed -create -field x.FA.zd_c -arg x.FC -via x.FDb.ind_a/y.C.a

Here, x.FA is the ctype corresponding to an in-memory representation of a y.a record in target x.
zd_c is a doubly linked, zero-terminated list of c. Arg is x.FC, which is the in-memory record
corresponding to y.c. x.FDb.ind_a is a global hash of a. y.C.a is the field in ssimfile y.c containing
a pkey reference to a.

#### -write -- Commit output to disk
<a href="#-write"></a>

#### -e --  (with -create -unittest) Edit new testcase
<a href="#-e"></a>

#### -comment -- Comment for new entity
<a href="#-comment"></a>

Applies to `-field`, `-ctype`, or `-target`.

#### -sandbox -- Make changes in sandbox
<a href="#-sandbox"></a>

Make all changes in a sandbox, showing the diff between current and new versions.

#### -test -- Build resulting changes, run tests
<a href="#-test"></a>

Build all main executables and runs tests. Use with `-sandbox`.

#### -showcpp -- (With -sandbox), show resulting diff
<a href="#-showcpp"></a>

Show a colorized diff between the current directory and the sandbox after executing
transaction in sandbox.

#### -msgtype -- (with -ctype) use this msgtype as type
<a href="#-msgtype"></a>

#### -anonfld -- Create anonfld
<a href="#-anonfld"></a>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Options -->

### Inputs
<a href="#inputs"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Inputs -->
`acr_ed` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dev.gitfile](/txt/ssimdb/dev/gitfile.md)|File managed by git|
|[dmmeta.ns](/txt/ssimdb/dmmeta/ns.md)|Namespace (for in-memory database, protocol, etc)|
|[dmmeta.ctype](/txt/ssimdb/dmmeta/ctype.md)|Struct|
|[dmmeta.field](/txt/ssimdb/dmmeta/field.md)|Specify field of a struct|
|[dmmeta.typefld](/txt/ssimdb/dmmeta/typefld.md)|Specifies which field of a message carries the type|
|[dmmeta.nsdb](/txt/ssimdb/dmmeta/nsdb.md)|Annotate ssimdb namespaces|
|[dmmeta.ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|File with ssim tuples|
|[dmmeta.pack](/txt/ssimdb/dmmeta/pack.md)|Request byte-packing of structure fields|
|[dmmeta.fprefix](/txt/ssimdb/dmmeta/fprefix.md)|Mapping between field prefix and Reftype|
|[dmmeta.listtype](/txt/ssimdb/dmmeta/listtype.md)|Specify structure of linked list based on field prefix|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|
|[dmmeta.cstr](/txt/ssimdb/dmmeta/cstr.md)|Specify that type behaves like a string|
|[dmmeta.cpptype](/txt/ssimdb/dmmeta/cpptype.md)|Specify whether a ctype can be passed by value, and other c++ options|
|[dmmeta.cfmt](/txt/ssimdb/dmmeta/cfmt.md)|Specify options for printing/reading ctypes into multiple formats|
|[dev.target](/txt/ssimdb/dev/target.md)|Build target|
|[dev.targsrc](/txt/ssimdb/dev/targsrc.md)|List of sources for target|
|[dev.sbpath](/txt/ssimdb/dev/sbpath.md)|Extra files to copy into the sandbox|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Inputs -->


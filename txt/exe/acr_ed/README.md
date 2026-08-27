## acr_ed - Script generator for common dev tasks


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- abt_md.toc_beg -->
&nbsp;&nbsp;&bull;&nbsp;  [Internals](#internals)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Description](#description)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Quick reference](#quick-reference)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Options](#options)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
<!-- abt_md.toc_end -->

### Internals
<a href="#internals"></a>
&#128196; [acr_ed - Internals](/txt/gen/acr_ed/acr_ed.md)<br/>

### Description
<a href="#description"></a>

`acr_ed` is a standalone script generator for common schema tasks such as
creating, deleting, and renaming targets, ctypes, ssimfiles, xrefs, and
source files.  When invoked, `acr_ed` prints an executable script to
stdout.  With `-write`, it executes that script immediately (equivalent to
piping it through `bash`).

The generated scripts call `acr` with `-insert`, `-del` or `-rename` and
`-write` to modify the ssimfiles, then run `amc` to regenerate C++ from
the new schema.  `acr` itself has no dependency on `acr_ed` — `acr_ed` is
a convenience wrapper around the same `acr` + `amc` calls you could do by
hand.  Use `acr_ed` when modifying the schema; use `acr` directly for data
queries, bulk edits, and anything that doesn't require a schema change.

The following is the full list of actions acr_ed supports
```
inline-command: acr edaction -report:N | ssimfilt -t -field:edaction -field:comment
EDACTION            COMMENT
Create_Citest       -create -citest <citest>
Create_Ctype        -create -ctype <ctype> [-subset <ctype> [-subset2 <ctype2> -separator <char>]] [-reftype <reftype>] [-indexed]
Create_DispatchMsg  -create -dispatch_msg <dispatch>/<msgtype>
Create_Fcurs        -create -fcurs <field>/<curstype>
Create_Field        -create -field <field> -arg <ctype> -reftype <reftype> [-xref [-via <via>]] [-anonfld] [-fbigend] ...
Create_Finput       -create -finput -target <target> -ssimfile <ssimfile>
Create_Fstep        -create -fstep <field> [-steptype:<type>]
Create_Srcfile      -create -srcfile <filename.(h|md|cpp)>
Create_Ssimfile     -create -ssimfile <ssimfile> [-subset <ctype> [-subset2 <ctype2> -separator <char>]]
Create_Target       -create -target <target>
Create_Unittest     -create -unittest <unittest>
Delete_Ctype        -del -ctype <ctype>
Delete_Field        -del -field <field>
Delete_Srcfile      -del -srcfile <srcfile>
Delete_Ssimfile     -del -ssimfile <ssimfile
Delete_Target       -del -target <target>
Rename_Ctype        -ctype <ctype> -rename <newname>
Rename_Field        -field <field> -rename <newname>
Rename_Srcfile      -srcfile <srcfile> -rename <newname>
Rename_Ssimfile     -ssimfile <ssimfile> -rename <newname>
Rename_Target       -target <target> -rename <newtarget>

```

### Quick reference
<a href="#quick-reference"></a>

```bash
# New build target
acr_ed -create -target <target> -write

# New ssimfile input for a target
acr_ed -create -finput -target <target> -ssimfile <ns>.<name> -write
acr_ed -create -finput -target <target> -ssimfile <ns>.<name> -indexed -write

# Add a command-line option (accessible in C++ as _db.cmdline.<name>)
acr_ed -create -field command.<proc>.<name> -arg <ctype> -dflt '""' -comment "..." -write

# Add a plain field to a ctype
acr_ed -create -field <ns>.FCtype.<name> -arg <ctype> -write

# New ctype with a pool
acr_ed -create -ctype <ns>.FName -reftype Lary -write
acr_ed -create -ctype <ns>.FName -reftype Tpool -indexed -write

# Hash-index cross-reference
acr_ed -create -field <ns>.FParent.ind_child -arg <ns>.FChild \
       -reftype Thash -hashfld <ns>.FChild.key \
       -xref -via <ns>.FChild.p_parent -write

# Wire-protocol message (subset of a header ctype)
acr_ed -create -ctype ams.MyMsg -subset ams.MsgHeader -write

# Markdown source files
acr_ed -create -srcfile txt/.../xyz.md -write
acr_ed -del    -srcfile txt/.../xyz.md -write

# Rename
acr_ed -ctype    <ns>.Old      -rename <ns>.New      -write
acr_ed -field    <ns>.Ct.old   -rename <ns>.Ct.new   -write
acr_ed -ssimfile <ns>.old      -rename <ns>.new      -write
acr_ed -target   <old>         -rename <new>         -write

# Delete
acr_ed -del -field    <ns>.FCtype.<name> -write
acr_ed -del -ctype    <ns>.FName         -write
acr_ed -del -ssimfile <ns>.<name>        -write
acr_ed -del -target   <target>           -write
```

Notes:
- `-reftype` is auto-inferred from the field-name prefix: `ind_` → Thash,
  `cd_` → Llist, `p_` → Upptr, `c_` → Ptrary.  See `acr fprefix`.
- String defaults require inner quotes: `'""'` for empty, `'"value"'` for
  a non-empty default.
- `-xref` creates both the field row and its `dmmeta.xref` record.
- Never hand-edit `cpp/gen/` or `include/gen/` — always go through
  `acr_ed` / `amc`.

Inspect the schema with acr:
```bash
acr field:<ctype>.%              # all fields of a ctype
acr field:command.<proc>.%       # command-line fields for a process
acr fprefix                      # field-name prefix → reftype mapping
```

### Options
<a href="#options"></a>
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

#### -create -- Create new entity (-finput, -target, -ctype, -field)
<a href="#-create"></a>

#### -del -- Delete mode
<a href="#-del"></a>

The `-del` option can be used with `-ctype`, `-field`, `-srcfile`, `-ssimfile`, `-target`.

Deleting a field removes the values as well as the schema row.  `acr` ignores a
data attribute that names no field of the current ctype, so the values would
otherwise sit in the ssimfile with nothing to object to them, and `acr_ed`
follows the delete with a rewrite of every ssimfile whose rows carried them:

    $ acr_ed -del -field dmmeta.Ns.license
    set -e
    bin/acr  -query:'' -replace:Y -check:Y -selerr:N -write:Y -t:Y << EOF
    EOF

    bin/acr  -query:field:dmmeta.Ns.license -del:Y -write:Y
    bin/acr  -query:dmmeta.ns:% -write:Y -print:N
    bin/amc

A field on an in-memory ctype has no ssimfile behind it, so the script for one
of those carries the delete alone.

Deleting a ctype that has an ssimfile is redirected to the ssimfile delete,
which removes the ctype, its fields and the data file together.

#### -rename -- Rename to something else
<a href="#-rename"></a>

The `-rename` option can be used with `-ctype`, `-field`, `-srcfile`, `-ssimfile`, `-target`.
When renaming a ssimfile, the corresponding ctype is also renamed to the CamelCase version of
the ssimfile name. In addition, in all programs that use the ssimfile as an finput,
the corresponding in-memory type and its pool are renamed.

Renaming a **field** takes either a bare new name or the full pkey, and the two
are the same edit: `acr_ed -field <ns>.<Ct>.old -rename new` and
`acr_ed -field <ns>.<Ct>.old -rename <ns>.<Ct>.new` both rename within the ctype,
because a rename does not move a field between ctypes.  `acr` renames the column
in the ssimfile at the same time, so no further step is needed on the data.  Two
spellings are refused rather than guessed at.  A `:` in the new name is the query
form (`-rename field:<ns>.<Ct>.new`), which `acr` would read as naming the ctype
`field:<ns>.<Ct>`.  And a new name carrying a different ctype is refused when
either ctype has an ssimfile, since the old table's rows would keep a column no
field claims; move a field between ssim-backed tables with a delete and a create.

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
* With `-reftype` option, the allocator for the new type can be specified.

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

To create a new record type in a program, use `-create -ctype:<ctype> -reftype:<reftype>`.
This adds a pool of a given type (typically `Tpool` or `Lary`).
To throw in a hash index, specify `-indexed`.
Example:

    $ acr_ed -create -ctype sample.FTable -subset u32 -reftype Tpool -indexed
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
FPREFIX      REQUIRE  DFLT  COMMENT
bh.Bheap     Y        Y     Binary heap
c.Delptr     N        N     Owned pointer; c_ allowed but Delptr fields may use any name
c.Ptr        N        N     Pointer; c_ allowed but Ptr fields may use any name
c.Ptrary     Y        Y     Pointer array
cd.Llist     Y        Y     Circular doubly linked list
cdl.Llist    Y        Y     Circular doubly linked LIFO list
cnt.Count    Y        Y     Count of items
cs.Llist     Y        Y     Circular singly linked list
csl.Llist    Y        Y     Circular singly linked LIFO list
ind.Blkhash  Y        N     Block hash index for keys with a dense component
ind.Thash    Y        Y     Hash table (index)
p.Ptr        N        N     Pointer; p_ allowed but Ptr fields may use any name
p.Upptr      Y        Y     Up-pointer: equivalent of Pkey reference for in-memory model
tr.Atree     Y        Y     AVL tree
zd.Llist     Y        Y     Zero-terminated doubly linked list
zdl.Llist    Y        Y     Zero-terminated doubly linked LIFO list
zs.Llist     Y        Y     Zero-terminated singly linked list
zs.ZSListMT  Y        N     Zero-terminated singly linked list with atomic head (multithreaded)
zsl.Llist    Y        Y     Zero-terminated singly linked LIFO list

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

#### -showcpp -- (With -sandbox), show resulting diff
<a href="#-showcpp"></a>

Show a colorized diff between the current directory and the sandbox after executing
transaction in sandbox.

#### -msgtype -- (with -ctype) use this msgtype as type
<a href="#-msgtype"></a>

#### -anonfld -- Create anonfld
<a href="#-anonfld"></a>

#### -license -- License for new source/script file
<a href="#-license"></a>

#### -fstep -- Add fstep record on existing field (use with -create)
<a href="#-fstep"></a>

#### -steptype -- Steptype for -create -fstep
<a href="#-steptype"></a>

#### -fcurs -- Add fcurs record (-create); pkey is <field>/<curstype-name>
<a href="#-fcurs"></a>

#### -dispatch_msg -- Add dispatch_msg record (-create); pkey is <dispatch>/<msgtype>
<a href="#-dispatch_msg"></a>

### Inputs
<a href="#inputs"></a>
`acr_ed` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|
|[dmmeta.cfmt](/txt/ssimdb/dmmeta/cfmt.md)|Specify options for printing/reading ctypes into multiple formats|
|[dmmeta.cpptype](/txt/ssimdb/dmmeta/cpptype.md)|Ask amc for a fieldwise constructor|
|[dmmeta.cstr](/txt/ssimdb/dmmeta/cstr.md)|Specify that type behaves like a string|
|[dmmeta.ctype](/txt/ssimdb/dmmeta/ctype.md)|Struct|
|[dmmeta.field](/txt/ssimdb/dmmeta/field.md)|Specify field of a struct|
|[dmmeta.fprefix](/txt/ssimdb/dmmeta/fprefix.md)|Allowed pairing of field-name prefix and reftype|
|[dev.gitfile](/txt/ssimdb/dev/gitfile.md)|File managed by git|
|[dmmeta.listtype](/txt/ssimdb/dmmeta/listtype.md)|Specify structure of linked list based on field prefix|
|[dmmeta.msgtype](/txt/ssimdb/dmmeta/msgtype.md)|Specify message type for each eligible message, controls dispatch|
|[dmmeta.ns](/txt/ssimdb/dmmeta/ns.md)|Namespace (for in-memory database, protocol, etc)|
|[dmmeta.nsdb](/txt/ssimdb/dmmeta/nsdb.md)|Annotate ssimdb namespaces|
|[dmmeta.pack](/txt/ssimdb/dmmeta/pack.md)|Request byte-packing of structure fields|
|[dmmeta.reftype](/txt/ssimdb/dmmeta/reftype.md)|Field type constructor (e.g. reference type)|
|[dev.sbpath](/txt/ssimdb/dev/sbpath.md)|Extra files to copy into the sandbox|
|[dmmeta.ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|File with ssim tuples|
|[dev.target](/txt/ssimdb/dev/target.md)|Build target|
|[dev.targsrc](/txt/ssimdb/dev/targsrc.md)|List of sources for target|
|[dmmeta.typefld](/txt/ssimdb/dmmeta/typefld.md)|Specifies which field of a message carries the type|

## acr_ed - Script generator for common dev tasks
<a href="#acr_ed"></a>
`acr_ed` is the schema editor.  It creates, renames and deletes targets, ctypes,
fields, ssimfiles, cross-references and source files.  Each edit comes out as a
shell script of `acr` commands followed by `amc`, and `-write` runs that script.
Use it for any change to the schema, and use [acr](/txt/exe/acr/README.md) directly
for queries and for edits to data.

### Syntax
<a href="#syntax"></a>
```usage
acr_ed: Script generator for common dev tasks
Usage: acr_ed [options]
    OPTION         TYPE    DFLT      COMMENT
    -in            string  "data"    Input directory or filename, - for stdin
    -create                          Create new entity (-finput, -target, -ctype, -field)
    -del                             Delete mode
    -rename        string  ""        Rename to something else
    -finput                          Create in-memory table based on ssimfile
    -foutput                         Declare field as an output
    -srcfile       string  ""        Create/Rename/Delete a source file
    -gstatic                         Like -finput, but data is loaded at compile time
    -indexed                         (with -finput) Add hash index
    -target        string  ""        Create/Rename/Delete target
    -nstype        string  "exe"     (with -create -target): exe,lib,etc.
    -ctype         string  ""        Create/Rename/Delete ctype
    -ssimfile      string  ""          Ssimfile for new ctype
    -subset        string  ""          Primary key is a subset of this ctype
    -subset2       string  ""          Primary key is also a subset of this ctype
    -separator     string  "."           Key separator
    -field         string  ""        Create field
    -arg           string  ""          Field type (e.g. u32, etc), (with -ctype) add the base field
    -dflt          string  ""          Field default value
    -anon                              Anonymous field (use with command lines)
    -bigend                            Big-endian field
    -cascdel                           Field is cascdel
    -before        string  ""          Place field before this one
    -substr        string  ""          New field is a substring
    -alias                           Create alias field (requires -srcfield)
    -srcfield      string  ""          Source field for bitfld/substr
    -inscond       string  "true"      Insert condition (for xref)
    -reftype       string  ""          Reftype (e.g. Val, Thash, Llist, etc)
    -hashfld       string  ""            (-reftype:Thash) Hash field
    -sortfld       string  ""            (-reftype:Bheap) Sort field
    -unittest      string  ""        Create unit test, <ns>.<functionname>
    -citest        string  ""        Create CI test
    -cppfunc       string  ""        Field is a cppfunc, pass c++ expression as argument
    -xref                                X-ref with field type
    -via           string  ""              X-ref argument (index, pointer, or index/key)
    -write                           Commit output to disk
    -e                                (with -create -unittest) Edit new testcase
    -comment       string  ""        Comment for new entity
    -sandbox                         Make changes in sandbox
    -showcpp                         (With -sandbox), show resulting diff
    -msgtype       string  ""        (with -ctype) use this msgtype as type
    -anonfld                         Create anonfld
    -license       string  "Apache"  License for new source/script file
    -fstep         string  ""        Add fstep record on existing field (use with -create)
    -steptype      string  "Inline"  Steptype for -create -fstep
    -fcurs         string  ""        Add fcurs record (-create); pkey is <field>/<curstype-name>
    -dispatch_msg  string  ""        Add dispatch_msg record (-create); pkey is <dispatch>/<msgtype>
    -verbose       flag              Verbosity level (0..255); alias -v; cumulative
    -debug         flag              Debug level (0..255); alias -d; cumulative
    -trace         string  ""        Trace expression: category[:filter],...; also payload_lim:N, verbose, debug, timestamps
    -help                            Print help and exit; alias -h
    -version                         Print version and exit
    -signature                       Show signatures and exit; alias -sig
```

### Description
<a href="#description"></a>

Run `acr_ed` without `-write` and it prints the script it would run.  Add `-write`
and it runs the same script.  So the way to see what an edit does is to run it once
without `-write` and read the output.

The script starts with one `acr -replace -check -write -t`, which is fed the new
records on a heredoc.  The commands the action needs next follow it: `git add` or
`git mv` for files, a `sed` over sources, a rewrite of an ssimfile.  The script ends
with `bin/amc`, which regenerates the C++ under `cpp/gen/` and `include/gen/`.  The
three source-file actions change no schema and skip `amc`.

When the action creates a field, a finput, an fstep or an fcurs, the heredoc also
carries a drawing of the result from [amc_vis](/txt/exe/amc_vis/README.md), as `#` comment lines.  Read it to
check the structure before you write the edit.

`acr_ed` reads the database under `-in`, which is `data` by default.  It writes
through `acr`, so every ssimfile it touches stays sorted, and it takes the lock
`lock/acr_ed` while `-write` runs.

Each invocation performs one action.  `-create`, `-del` or `-rename` picks the verb,
and the entity option that carries a value picks the action.  The table lists them
all:

```ssim
inline-command: acr edaction -report:N | ssimfilt -t -field:edaction -field:comment
EDACTION            COMMENT
Create_Citest       -create -citest <citest>
Create_Ctype        -create -ctype <ctype> [-subset <ctype> [-subset2 <ctype2> -separator <char>]] [-reftype <reftype>] [-indexed]
Create_DispatchMsg  -create -dispatch_msg <dispatch>/<msgtype>
Create_Fcurs        -create -fcurs <field>/<curstype>
Create_Field        -create -field <field> -arg <ctype> -reftype <reftype> [-xref [-via <via>]] [-anonfld] [-bigend] ...
Create_Finput       -create -finput -target <target> -ssimfile <ssimfile>
Create_Fstep        -create -fstep <field> [-steptype:<type>]
Create_Srcfile      -create -srcfile <filename.(h|md|cpp)>
Create_Ssimfile     -create -ssimfile <ssimfile> [-subset <ctype> [-subset2 <ctype2> -separator <char>]]
Create_Target       -create -target <target>
Create_Unittest     -create -unittest <unittest>
Delete_Ctype        -del -ctype <ctype>
Delete_Field        -del -field <field>
Delete_Srcfile      -del -srcfile <srcfile>
Delete_Ssimfile     -del -ssimfile <ssimfile>
Delete_Target       -del -target <target>
Rename_Ctype        -ctype <ctype> -rename <newname>
Rename_Field        -field <field> -rename <newname>
Rename_Srcfile      -srcfile <srcfile> -rename <newname>
Rename_Ssimfile     -ssimfile <ssimfile> -rename <newname>
Rename_Target       -target <target> -rename <newtarget>

```

#### Creating a target
<a href="#creating-a-target"></a>

`-create -target <name>` adds a namespace and everything needed to build it.  The
records are the `dmmeta.ns` row, its `dev.target`, `dev.targsrc` and `dev.targdep`
rows, the `FDb` global, and for an executable a command line with an `-in` option.
For an executable or a library the script writes a starter header and source file,
runs `amc` and builds the target with `abt -install`.  An executable also gets its
link `bin/<name>` to the release build.  Every kind gets a README under `txt/`, its
copyright headers from `src_hdr` and its documentation from `abt_md`.

`-nstype` picks the kind of namespace, and a name that starts with `lib_` is always
a library.  An `ssimdb` namespace gets a `data/<name>/` directory and a `dmmeta.nsdb`
row, and its generated code goes into `lib_prot`.  For an executable or a
library, run `acr_compl -install` afterwards, as the script reminds you, so shell
completion learns the new name.

#### Creating a ctype
<a href="#creating-a-ctype"></a>

`-create -ctype <ns>.<Name>` adds a `dmmeta.ctype` row.  `-subset` gives it a first
field of that type, which is its primary key.  The key is named after the ctype in
lower case, `FThing` giving `thing`.  In an `ssimdb` namespace a key whose type is
another table takes that table's key name and becomes a `Pkey`, and any other key is
a `Val`.  A subset of a message header that has a type field becomes a `Base` field instead,
and the new ctype also gets the header's `dmmeta.pack`, a `dmmeta.msgtype` and its
string formats.

In an `ssimdb` namespace, or with `-indexed`, a ctype with no `-subset` gets an
`algo.Smallstr50` key.  `-subset` together with `-subset2` and `-separator` makes a
key that joins two other keys, and adds a substring field for each half.

`-reftype` with a pool reftype, such as `Lary` or `Tpool`, adds a pool of the new
ctype to the namespace's `FDb`.  `-indexed` adds a global hash index on the key.

#### Creating an ssimfile
<a href="#creating-an-ssimfile"></a>

`-create -ssimfile <ns>.<name>` creates a ctype as above and makes it a table.  The
ctype is named in CamelCase after the ssimfile, `dev.widget` becoming `dev.Widget`.
The records added are the `dmmeta.ssimfile` row, a `dmmeta.ssimsort` on the primary
key, a `comment` field and a string format.  The script creates and stages the empty
`data/<ns>/<name>.ssim`.  The namespace must already be an ssim database, which means
it has a `dmmeta.nsdb` row.

#### Loading an ssimfile into a program
<a href="#loading-an-ssimfile-into-a-program"></a>

`-create -finput -target <target> -ssimfile <ssimfile>` makes the program load a
table at startup.  It adds an in-memory ctype `<target>.F<Name>`, whose `Base` field
copies every field of the table's ctype.  It also adds a pool of those records to the
program's `FDb` and a `dmmeta.finput` row.  The pool is a `Lary` unless `-reftype`
names another one, and `-indexed` adds a hash index on the table's primary key:

```bash
$ acr_ed -create -finput -target acr_compl -ssimfile dev.license -indexed
acr_ed.create_finput  target:acr_compl  ssimfile:dev.license
set -e
bin/acr  -query:'' -replace:Y -check:Y -selerr:N -write:Y -t:Y << EOF
dmmeta.ctype  ctype:acr_compl.FLicense  comment:""
dmmeta.field  field:acr_compl.FLicense.base  arg:dev.License  reftype:Base  dflt:""  comment:""
dmmeta.field  field:acr_compl.FDb.license  arg:acr_compl.FLicense  reftype:Lary  dflt:""  comment:""
dmmeta.finput  field:acr_compl.FDb.license  update:N  strict:Y  comment:""

dmmeta.field  field:acr_compl.FDb.ind_license  arg:acr_compl.FLicense  reftype:Thash  dflt:""  comment:""
dmmeta.thash  field:acr_compl.FDb.ind_license  hashfld:dev.License.license  unique:Y  comment:""
dmmeta.xref  field:acr_compl.FDb.ind_license  inscond:true  via:""
#  Proposed change
# / acr_compl.FDb
# |Lary license-------->/ acr_compl.FLicense
# |Thash ind_license--->|
# -                     |
#                       -
EOF

bin/amc
```

With `-gstatic` in place of `-finput`, `amc` compiles the table's rows into the
program, which inserts them at startup without reading the ssimfile.  `-foutput`
also declares the pool an output, so `amc` generates the code to save it back.

#### Creating a field
<a href="#creating-a-field"></a>

`-create -field <ns>.<Ctype>.<name>` adds a `dmmeta.field` row, with its type in
`-arg`.  The new field goes at the end of its ctype, or just before the ctype's
`comment` field when that field is last.  `-before` places it anywhere else.  The
same command can also mark the field as a substring, an alias, a computed `cppfunc`,
a big-endian integer or a cascading delete, and each option says how.

The field name's prefix implies its reftype, and the table of prefixes is
`dmmeta.fprefix`:

```ssim
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

A field with no prefix is a `Pkey` when both its ctype and its arg are tables, and a
`Val` otherwise.  The name can imply the arg too: `<ns>.FCtype.p_ns` finds the pool
`<ns>.FDb.ns` and takes its type.  Name `-arg`, `-reftype` or `-via` to override any
of these guesses.

A field that needs a record of its own gets it in the same script.  A `Thash` gets a
`dmmeta.thash` and a `Bheap` or `Atree` a `dmmeta.sortfld`, each keyed on the arg's
primary key unless `-hashfld` or `-sortfld` names another field.  An `Llist` gets a
`dmmeta.llist`, a `Ptrary` a `dmmeta.ptrary` and a `Tary` a `dmmeta.tary`.

#### Adding a command-line option
<a href="#adding-a-command-line-option"></a>

A command-line option is a field of the ctype `command.<target>`, so it is created
like any other field.  The program reads it as `_db.cmdline.<name>`, and its comment
becomes the help text.  An option with an empty `-dflt` is required, except that a
`bool` option never is.  A `Tary` option can be given several times, `-anonfld`
makes an option positional, and `-alias -srcfield` makes one a synonym of another.

An option whose arg is a table and whose reftype is `Pkey` or `RegxSql` takes a key
of that table.  Shell completion reads the field table, so it offers the table's keys
for the new option without further work.

#### Creating a cross-reference
<a href="#creating-a-cross-reference"></a>

A cross-reference is a field of one in-memory record that indexes or points at
others: a hash, a list, a heap, a tree or a pointer array.  `amc` keeps it current as
records are created and deleted, and a `dmmeta.xref` row declares it.  A `Thash`,
`Llist`, `Bheap`, `Atree` or `Blkhash` field always has one, `-via` implies one, and
`-xref` asks for one on any other field.

An index in record A over records B needs a way to find the A given a B.  `FDb` is
always reachable, so a global index needs nothing more.  An index held by any other
ctype needs a path, and `acr_ed` searches for it among the fields of B and the global
hashes.  When it finds one path it uses it and prints it as `acr_ed.via_match1`.
When it finds none, or several, it stops and lists the candidates, and you choose one
with `-via`.

`-via` is either a pointer field of B, or a `<hash>/<key>` pair.  The hash is a
global index of A, and the key is the ssimfile field of B that holds A's key.  The
list types a prefix can name are these:

```ssim
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

The `dmmeta.llist` row that `acr_ed` adds for a list also sets `havetail` and
`havecount`, and you can edit both afterwards.

#### A worked example of a cross-reference
<a href="#a-worked-example-of-a-cross-reference"></a>

This example builds a program `samp_xref` that loads `dmmeta.ns` and `dmmeta.ctype`,
and links each ctype to its namespace.  A ctype's key starts with the key of its
namespace, so the namespace is the parent.

```bash
acr_ed -create -target samp_xref -write
acr_ed -create -finput -target samp_xref -ssimfile dmmeta.ns -indexed -write
acr_ed -create -finput -target samp_xref -ssimfile dmmeta.ctype -write
```

An `Upptr` from each ctype up to its namespace needs only its name.  `acr_ed` finds
the pool `samp_xref.FDb.ns`, and the path through the hash `ind_ns` and the field
`dmmeta.Ctype.ns`:

```bash
$ acr_ed -create -field samp_xref.FCtype.p_ns
acr_ed.via_match1  child:dmmeta.Ctype.ns  comment:"This child field is a possible via key candidate"
...
dmmeta.field  field:samp_xref.FCtype.p_ns  arg:samp_xref.FNs  reftype:Upptr  dflt:""  comment:""
dmmeta.xref  field:samp_xref.FCtype.p_ns  inscond:true  via:samp_xref.FDb.ind_ns/dmmeta.Ctype.ns
```

The same path serves an index in the other direction.  `c_ctype` gives each namespace
a pointer array of its ctypes, and `zd_ctype` would give it a list:

```bash
$ acr_ed -create -field samp_xref.FNs.c_ctype
...
dmmeta.field  field:samp_xref.FNs.c_ctype  arg:samp_xref.FCtype  reftype:Ptrary  dflt:""  comment:""
dmmeta.xref  field:samp_xref.FNs.c_ctype  inscond:true  via:samp_xref.FDb.ind_ns/dmmeta.Ctype.ns
dmmeta.ptrary  field:samp_xref.FNs.c_ctype  unique:Y  heaplike:N
```

With both fields written, `amc_vis` draws the two records and the links between them:

```text
$ amc_vis '(samp_xref.FNs|samp_xref.FCtype)'
                                / samp_xref.FCtype
              / samp_xref.FNs   |
              |<----------------|Upptr p_ns
              |Ptrary c_ctype-->|
              -                 |
                                -
```

#### Creating a source file
<a href="#creating-a-source-file"></a>

`-create -srcfile <path>` creates a `.cpp`, `.h`, README or script file with a
starter body, and registers it in `dev.gitfile`.  A `.cpp` or `.h` also gets a
`dev.targsrc` row, and its target is the one that owns the most files in the same
directory, unless `-target` names another.  A `.md` under `txt/` gets a
`dev.readmefile` row, and a file under `bin/` gets a `dev.scriptfile` row and a
README of its own.  The script then updates the copyright headers with `src_hdr`.
An existing file keeps its contents, and only the records are added.

`-srcfile <path> -rename <newpath>` moves the file with `git mv` and renames its
records.  The file also moves to the target that owns the new directory, or to
`-target`.  `-del -srcfile <path>` deletes the file and its records.

#### Creating tests
<a href="#creating-tests"></a>

`-create -unittest <ns>.<name>` adds an `atfdb.unittest` row and appends an empty
test function to `cpp/atf_unit/<ns>.cpp`.  `-create -citest <name>` adds an
`atfdb.citest` row to the `normalize` job, and after `amc` you write the function
`atf_ci::citest_<name>` it declares.

#### Adding steps, cursors and dispatch messages
<a href="#adding-steps-cursors-and-dispatch-messages"></a>

Three actions add one record to an existing field or dispatch.  `-create -fstep
<field>` adds a `dmmeta.fstep` row, so the program's main loop calls a step
function whenever that field is non-empty.  `-create -fcurs <field>/<name>` adds a custom cursor
over the field.  `-create -dispatch_msg <dispatch>/<msgtype>` routes one more message
ctype to a dispatch.

#### Renaming
<a href="#renaming"></a>

`-rename <new>` with `-ctype`, `-field`, `-ssimfile`, `-srcfile` or `-target` renames
that entity.  `acr` renames every record that refers to it, so the schema stays
consistent.  Hand-written C++ keeps the old names, except where a target rename
edits it.

A field rename also renames the column in the table's ssimfile.  The new name can be
bare, or the full `<ns>.<Ctype>.<name>`, and both mean the same edit.

An ssimfile rename moves the file with `git mv`.  When the key field and the ctype
are named after the ssimfile, it renames them too.  So does each program's
in-memory copy of the table, `<target>.F<Name>`, and its pool.

A target rename renames the namespace and every record keyed by it.  It moves the
target's sources into `cpp/<new>/` and `include/`, and renames the `bin/` link.  It
also rewrites `#include` lines and `<old>::` qualifiers in those sources with `sed`.
Anything else that spells the old name, such as another target's code, is yours to
fix.

#### Deleting
<a href="#deleting"></a>

`-del` with `-field`, `-ctype`, `-ssimfile`, `-srcfile` or `-target` deletes that
entity and every record that refers to it.  Deleting a field also rewrites each
ssimfile whose rows carried it, so the values go with the schema row:

```bash
$ acr_ed -del -field dev.Gitfile.comment
set -e
bin/acr  -query:'' -replace:Y -check:Y -selerr:N -write:Y -t:Y << EOF
EOF

bin/acr  -query:field:dev.Gitfile.comment -del:Y -write:Y
bin/acr  -query:dev.gitfile:% -write:Y -print:N
bin/amc
```

Deleting a ctype that has an ssimfile deletes the ssimfile, and the file goes with
`git rm`.  Deleting a target removes its namespace, its records and its files, then
updates the headers and the documentation.

#### Seeing the generated code
<a href="#seeing-the-generated-code"></a>

The printed script shows the records, and `-sandbox` shows what they generate.
`acr_ed` resets the `acr_ed` sandbox from the current tree, runs the whole script
there with `-write`, and prints the diff.  Inside the sandbox it also rebuilds and
runs `amc` from source, so the diff covers a change to `amc` itself.  The checkout
you work in is left alone.  See [wt](/txt/exe/wt/README.md) for sandboxes.

### Examples
<a href="#examples"></a>

```bash
# See what an edit would do, without doing it
acr_ed -create -field dmmeta.Ns.flag -arg bool

# Create an executable, a library and an ssim database namespace
acr_ed -create -target samp_tool -write
acr_ed -create -target lib_samp -write
acr_ed -create -target sampdb -nstype ssimdb -write

# Create a table whose key refers to dev.target
acr_ed -create -ssimfile dev.widget -subset dev.Target -comment "A widget" -write

# Create a table whose key joins a namespace and a license, as <ns>/<license>
acr_ed -create -ssimfile dev.nslicense -subset dmmeta.Ns -subset2 dev.License -separator / -write

# Make a program load dev.gitfile, with a hash index on its key
acr_ed -create -finput -target samp_tool -ssimfile dev.gitfile -indexed -write

# Create an in-memory record type with a pool and a global hash
acr_ed -create -ctype samp_tool.FThing -reftype Tpool -indexed -write

# Add a field, and one placed before an existing field
acr_ed -create -field dev.Widget.size -arg u32 -dflt 0 -comment "Size in bytes" -write
acr_ed -create -field dev.Widget.owner -arg algo.Smallstr50 -before dev.Widget.size -write

# Add a string option and a flag to a program's command line
acr_ed -create -field command.samp_tool.name -arg algo.cstring -dflt '""' -comment "Name to use" -write
acr_ed -create -field command.samp_tool.dry -arg bool -comment "Print, do not act" -write

# Add an option whose value is a key of dmmeta.ns, completed by the shell
acr_ed -create -field command.samp_tool.ns -arg dmmeta.Ns -reftype Pkey -dflt '""' -write

# Add a heap of the loaded files, ordered by extension
acr_ed -create -field samp_tool.FDb.bh_gitfile -arg samp_tool.FGitfile -sortfld dev.Gitfile.ext -write

# Add a list of ctypes to each namespace, naming the path explicitly
acr_ed -create -field samp_xref.FNs.zd_ctype -arg samp_xref.FCtype -via samp_xref.FDb.ind_ns/dmmeta.Ctype.ns -write

# Create a source file, a README and a unit test
acr_ed -create -srcfile cpp/samp_tool/parse.cpp -write
acr_ed -create -srcfile txt/exe/samp_tool/internals.md -comment "Internals of samp_tool" -write
acr_ed -create -unittest algo_lib.Widget -e -write

# Rename a field, a ctype, an ssimfile, a source file and a target
acr_ed -field dev.Widget.size -rename nbyte -write
acr_ed -ctype samp_tool.FThing -rename samp_tool.FItem -write
acr_ed -ssimfile dev.widget -rename dev.gadget -write
acr_ed -srcfile cpp/samp_tool/parse.cpp -rename cpp/samp_tool/read.cpp -write
acr_ed -target samp_tool -rename samp_util -write

# Delete a field and its values, an ssimfile, a source file and a target
acr_ed -del -field dev.Widget.owner -write
acr_ed -del -ssimfile dev.widget -write
acr_ed -del -srcfile cpp/samp_tool/read.cpp -write
acr_ed -del -target samp_tool -write

# See the generated code a schema change produces, in the sandbox
acr_ed -create -field dmmeta.Ns.flag -arg bool -sandbox
```

Inspect the schema with `acr` before and after an edit:

```bash
acr field:dev.Gitfile.%              # the fields of a ctype
acr field:command.acr_ed.%           # the command-line options of a program
acr fprefix                          # which reftype each field-name prefix implies
```

### Caveats
<a href="#caveats"></a>

- One invocation runs one action.  A command line that selects two, such as
  `-create -ctype X -field Y`, prints `More than one action selected` and the table of
  actions, and does nothing.
- The script stops at its first failing command, and nothing rolls back.  A failure
  midway leaves the tree as the commands before it left it, so check `git status`.
- A string default needs its C++ quotes inside the shell quotes: `-dflt '""'` for an
  empty string, `-dflt '"value"'` for another.
- A rename changes the schema and the generated code, and leaves hand-written C++
  alone.  The next build reports each old name that code still uses, as a missing
  member or type.
- A field rename refuses a new name with a `:` in it (`acr_ed.rename_prefix`), which
  is the query spelling `field:<ns>.<Ctype>.<name>`.  It also refuses to move a field
  to another ctype when either ctype has an ssimfile (`acr_ed.rename_ctype`).  Move
  such a field with `-del` and `-create`.
- `-create -finput` for a table the target already loads fails in the drawing step,
  with `amc_vis.duplicate_key`.
- After you create, rename or delete a target, run `acr_compl -install` so shell
  completion knows the change.
- Never edit `cpp/gen/` or `include/gen/` by hand.  `amc` rewrites them from the
  schema, which is what `acr_ed` edits.

### Options
<a href="#options"></a>
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

The ssim database `acr_ed` reads to plan the edit, `data` by default.  The script
itself always runs against the files in the current directory.

#### -create -- Create new entity (-finput, -target, -ctype, -field)
<a href="#-create"></a>

Selects the create verb.  The entity option with a value picks what is created:
`-target`, `-ctype`, `-ssimfile`, `-finput`, `-field`, `-srcfile`, `-unittest`,
`-citest`, `-fstep`, `-fcurs` or `-dispatch_msg`.

#### -del -- Delete mode
<a href="#-del"></a>

Selects the delete verb, with `-field`, `-ctype`, `-ssimfile`, `-srcfile` or
`-target`.  `-del -ctype` on a ctype that has an ssimfile deletes the ssimfile, see
[Deleting](#deleting).

#### -rename -- Rename to something else
<a href="#-rename"></a>

Renames the entity named by `-field`, `-ctype`, `-ssimfile`, `-srcfile` or `-target`
to this value.  For a field the value may be a bare name.  See
[Renaming](#renaming) for what each rename carries along.

#### -finput -- Create in-memory table based on ssimfile
<a href="#-finput"></a>

With `-create`, `-target` and `-ssimfile`, makes the target load the ssimfile at
startup, see [Loading an ssimfile into a program](#loading-an-ssimfile-into-a-program).
Combine it with `-indexed` for a hash index, and with `-reftype` for a pool other
than `Lary`.

#### -foutput -- Declare field as an output
<a href="#-foutput"></a>

With `-create -finput`, also adds a `dmmeta.foutput` row on the new pool.  `amc` then
generates the code the program uses to save the table back to its ssimfile.

#### -srcfile -- Create/Rename/Delete a source file
<a href="#-srcfile"></a>

Names the source file to create, rename or delete.  The kind of file follows from
its path: `.cpp` and `.h` are sources of a target, a `.md` under `txt/` is a README,
and a file under `bin/` is a script.  The directory must exist.

#### -gstatic -- Like -finput, but data is loaded at compile time
<a href="#-gstatic"></a>

Use it in place of `-finput`.  `amc` compiles the table's rows into the program,
which inserts them at startup.  The new ctype also gets a `step` hook, which `amc`
binds for each row to a function named after that row's key.

#### -indexed -- (with -finput) Add hash index
<a href="#-indexed"></a>

Adds a global hash `<ns>.FDb.ind_<name>`.  With `-create -finput` or `-create -ctype`
it hashes the new records on their key, and with `-create -field` on the new field.
With `-create -ctype` it also gives a ctype with no `-subset` an `algo.Smallstr50`
key.

#### -target -- Create/Rename/Delete target
<a href="#-target"></a>

Names the target to create, rename or delete.  With `-finput` it names the program
that loads the table.  With `-srcfile` it names the owning target, when the guess from
the directory is wrong.

#### -nstype -- (with -create -target): exe,lib,etc.
<a href="#-nstype"></a>

The kind of namespace a new target is, a key of `dmmeta.nstype`: `exe`, `lib`,
`protocol` or `ssimdb`.  A target whose name starts with `lib_` is a `lib` whatever
this says.

#### -ctype -- Create/Rename/Delete ctype
<a href="#-ctype"></a>

Names the ctype to create, rename or delete, as `<ns>.<Name>`.  With `-create`, see
[Creating a ctype](#creating-a-ctype) for the fields and pool it may add.

#### -ssimfile --   Ssimfile for new ctype
<a href="#-ssimfile"></a>

Names the ssimfile to create, rename or delete, as `<ns>.<name>`.  With `-finput` or
`-gstatic` it names the table the target loads.

#### -subset --   Primary key is a subset of this ctype
<a href="#-subset"></a>

With `-create -ctype` or `-create -ssimfile`, the type of the new ctype's key field.
With `-create -field` and no `-arg`, it stands for `-arg` and makes the field a
`Pkey` when the type is a table.

#### -subset2 --   Primary key is also a subset of this ctype
<a href="#-subset2"></a>

With `-subset`, makes the key a join of two keys, `<subset><separator><subset2>`.
The key becomes an `algo.Smallstr50`, and a substring field for each half is added
beside it.

#### -separator --     Key separator
<a href="#-separator"></a>

The character that joins the two halves of a `-subset2` key, `.` by default.  Pick
one that neither half's key can contain, such as `/`.

#### -field -- Create field
<a href="#-field"></a>

Names the field to create, rename or delete, as `<ns>.<Ctype>.<name>`.  With
`-create`, see [Creating a field](#creating-a-field) for how the name implies the
reftype and the arg.

#### -arg --   Field type (e.g. u32, etc), (with -ctype) add the base field
<a href="#-arg"></a>

The type of a new field, a key of `dmmeta.ctype`.  It can be left out when the field
name implies it or `-subset` stands for it.  `-create -ctype` ignores it, so use
`-subset` to give a new ctype its key.

#### -dflt --   Field default value
<a href="#-dflt"></a>

A C++ expression for the new field's default.  A string needs its C++ quotes, as in
`-dflt '""'`.  On a command-line option, an empty default makes the option required.

#### -anon --   Anonymous field (use with command lines)
<a href="#-anon"></a>

Makes a new command-line option positional, by adding a `dmmeta.anonfld` row.  It is
the same as `-anonfld`.

#### -bigend --   Big-endian field
<a href="#-bigend"></a>

Stores a new `Val` field in big-endian byte order, with a `dmmeta.fbigend` row.  Use
it for a field of a wire message.  Any other reftype is refused.

#### -cascdel --   Field is cascdel
<a href="#-cascdel"></a>

Adds a `dmmeta.cascdel` row, so deleting a record in memory also deletes the records
this field refers to.

#### -before --   Place field before this one
<a href="#-before"></a>

Places a new field just before the named field of the same ctype.  It sets the new
row's `acr.rowid`, and field order is the member order of the generated struct.

#### -substr --   New field is a substring
<a href="#-substr"></a>

Makes a new field a substring of another field, computed by the given path
expression.  The source is `-srcfield`, or the ctype's first field when that is
omitted.  `-arg` is still required:

```bash
acr_ed -create -field dmmeta.Ns.name -arg algo.Smallstr50 -substr .RR -srcfield dmmeta.Ns.ns
```

#### -alias -- Create alias field (requires -srcfield)
<a href="#-alias"></a>

Makes a new field a synonym of `-srcfield`, with a `dmmeta.falias` row.  On a command
line this gives an option a second name.

#### -srcfield --   Source field for bitfld/substr
<a href="#-srcfield"></a>

The field that `-substr` extracts from or that `-alias` renames.  For `-alias` it must
already exist.

#### -inscond --   Insert condition (for xref)
<a href="#-inscond"></a>

A C++ expression that decides whether a new record joins this cross-reference,
`true` by default.  With `-inscond false` no record is added automatically, and the
program inserts records itself, for instance with `ind_thing_InsertMaybe`.

#### -reftype --   Reftype (e.g. Val, Thash, Llist, etc)
<a href="#-reftype"></a>

The reftype of a new field, which is otherwise implied by its name.  With `-create
-ctype` it is the pool the new ctype gets in `FDb`, such as `Lary`, `Lpool` or
`Tpool`, and a reftype that is not a pool, such as `Tary`, adds none.  With `-finput` it replaces the
default `Lary` pool.

#### -hashfld --     (-reftype:Thash) Hash field
<a href="#-hashfld"></a>

The field a new `Thash` hashes on.  The default is the primary key of the indexed
records.

#### -sortfld --     (-reftype:Bheap) Sort field
<a href="#-sortfld"></a>

The field a new `Bheap` or `Atree` sorts on.  The default is the primary key of the
indexed records.

#### -unittest -- Create unit test, <ns>.<functionname>
<a href="#-unittest"></a>

With `-create`, adds a unit test named `<ns>.<name>` and appends its empty function
to `cpp/atf_unit/<ns>.cpp`.  The namespace must exist.  Add `-e` to open the file and
then build and run the test.

#### -citest -- Create CI test
<a href="#-citest"></a>

With `-create`, adds an `atfdb.citest` row in the `normalize` job, using `-comment`
as its description.  You then write the function `atf_ci::citest_<name>` that `amc`
declares.

#### -cppfunc -- Field is a cppfunc, pass c++ expression as argument
<a href="#-cppfunc"></a>

Makes a new field computed.  `amc` generates an accessor that returns the value of
this C++ expression, and the record stores no value for it.

#### -xref --     X-ref with field type
<a href="#-xref"></a>

Adds a `dmmeta.xref` row for the new field, so `amc` maintains it as records are
created and deleted.  It is implied by `-via` and by the reftypes that need it; see
[Creating a cross-reference](#creating-a-cross-reference).

#### -via --       X-ref argument (index, pointer, or index/key)
<a href="#-via"></a>

The path from a record to its parent in a cross-reference.  It is a pointer field of
the record, or `<hash>/<key>` with a global hash of the parent and the record's field
holding the parent's key.  Give it when `acr_ed` finds no path or several:

```bash
acr_ed -create -field samp_xref.FNs.zd_ctype -arg samp_xref.FCtype -via samp_xref.FDb.ind_ns/dmmeta.Ctype.ns
```

#### -write -- Commit output to disk
<a href="#-write"></a>

Runs the script.  Without it the script is printed and nothing changes, and on a
terminal the new names are highlighted.  `acr_ed` exits non-zero when a command in
the script fails.

#### -e --  (with -create -unittest) Edit new testcase
<a href="#-e"></a>

Opens the new file in `$EDITOR` at its end, after `-create -unittest` or `-create
-srcfile`.  For a unit test it also builds `atf_unit` and runs the test.  Without
`-e` those lines appear in the script as comments.

#### -comment -- Comment for new entity
<a href="#-comment"></a>

The `comment` of the new record: a target, ctype, field, source file, README, CI
test, step, cursor or dispatch message.  A target's comment is also its command
line's help text.

#### -sandbox -- Make changes in sandbox
<a href="#-sandbox"></a>

Runs the edit in the `acr_ed` sandbox and prints the diff it produced, see
[Seeing the generated code](#seeing-the-generated-code).  The sandbox is reset from
the current tree first, so each run starts clean.

#### -showcpp -- (With -sandbox), show resulting diff
<a href="#-showcpp"></a>

Has no effect.  `-sandbox` always prints the diff.

#### -msgtype -- (with -ctype) use this msgtype as type
<a href="#-msgtype"></a>

The type code of a new message ctype, one created with `-subset` of a header that
has a type field.  By default `acr_ed` takes one more than the largest code of the
messages that share that header.

#### -anonfld -- Create anonfld
<a href="#-anonfld"></a>

Makes a new command-line option positional, so it can be given without its name.  It
is the same as `-anon`.

#### -license -- License for new source/script file
<a href="#-license"></a>

The license of a new target's namespace, or of a new script under `bin/`, a key of
`dev.license`.  The default is `Apache`.

#### -fstep -- Add fstep record on existing field (use with -create)
<a href="#-fstep"></a>

With `-create`, adds a `dmmeta.fstep` row on an existing field.  `amc` then declares a
step function for it, which the main loop calls while the field is non-empty, on the
schedule set by `-steptype`.

#### -steptype -- Steptype for -create -fstep
<a href="#-steptype"></a>

How a new step is scheduled, a key of `dmmeta.steptype`.  `Inline` calls it on every
pass of the main loop; `InlineRecur` and `TimeHookRecur` call it at a fixed period.

#### -fcurs -- Add fcurs record (-create); pkey is <field>/<curstype-name>
<a href="#-fcurs"></a>

With `-create`, adds a `dmmeta.fcurs` row, a custom cursor over an existing field, such
as `samp_tool.FDb.ind_thing/curs`.  The field must exist.

#### -dispatch_msg -- Add dispatch_msg record (-create); pkey is <dispatch>/<msgtype>
<a href="#-dispatch_msg"></a>

With `-create`, adds a `dmmeta.dispatch_msg` row, so the named dispatch handles one
more message ctype.  The message ctype must exist.

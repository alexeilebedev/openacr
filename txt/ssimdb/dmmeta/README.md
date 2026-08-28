## dmmeta - Tables holding metadata (data model meta)


### dmmeta.anonfld
<a href="#dmmeta-anonfld"></a>

The `anonfld` record indicates that a field has an optional name.
It is used in several contexts. When used in a command line, it creates a positional argument.
For enums, it causes the name of the field to be stripped from the generated enum constant.
If there were no anonfld,
the enum's symbols would be named `algo_Bool_value_true`, etc. With anonfld, it becomes `algo_Bool_true`,
etc.

```ssim
    dmmeta.anonfld  field:algo.Bool.value  comment:""
```

### dmmeta.bitfld
<a href="#dmmeta-bitfld"></a>

Specify that the field is a computed field extracted from certain bits of the value of a source field.
In the example below, the field `bits5` is defined as bits 5..10 of the value of the field called `value`.
```ssim
    dmmeta.field  field:atf_amc.BitfldType1.value  arg:u64  reftype:Val     dflt:""  comment:""
    dmmeta.field  field:atf_amc.BitfldType1.bits5   arg:u64  reftype:Bitfld  dflt:""  comment:""
      dmmeta.bitfld  field:atf_amc.BitfldType1.bits5   offset:5  width:10  srcfield:atf_amc.BitfldType1.value  comment:""
```

Note that the AMC bitfld is endian-agnostic; it specifies a calculation to perform on the source field,
which is the same on all architectures. In contrast, C bitfields depend on machine endianness, because
they are not specified in relation to a source field.

Bitfld requires `reftype:Bitfld` in the parent field record. A Bitfld field can be of any type that
can be created from an integer, such as bool, any signed or unsigned integer type, or a wrapped enum type.

The accessors amc generates for it -- a getter and a setter for the bit range, and
the conversions to and from the field's string constants -- are in [the code amc
writes for that ctype](code:dmmeta.field:atf_amc.BitfldType1.bits5).

Bitfields can be read and printed from string tuples just like other fields.
When printing a ctype containing bitfields to an ssim tuple, amc does not print the source field;
instead, all the bitfields are printed instead.

When printing a ctype containing bitfields using the `Raw` format, only the source field is printed,
and bitfields are skipped.

It is an error to specify a combination of offset and width that is out of bounds for the source type.
Amc will flag this. It is also an error to have two bitfields overlap. All of the `offset+width`
ranges must be disjoint.

In order to facilitate flexible handling of named bits within bitsets,
special enum constants are generated for bitfld when its width is `1` and the field type is `bool`.

### dmmeta.blkhash
<a href="#dmmeta-blkhash"></a>

This record is required on fields with reftype:Blkhash.

A Blkhash is a unique hash index for keys with a dense integer component.
Consider an index of sequenced messages keyed by (stream id, seqno).
A regular hash function must spread all keys uniformly, so ten million
cached messages cost a ten-million-pointer bucket array that doubles and
rehashes as it grows, and neighboring seqnos land in unrelated cache lines.
But seqnos are consecutive: only the stream id needs spreading.

The blkhash record declares the dense component (`linfld`, a field of the
key ctype, possibly wrapped in a single-field type such as algo.SeqType)
and a width `linbits`.  The low `linbits` bits of the component select a
slot directly inside a block of 2^linbits entry pointers.  The rest of the
key, with those bits masked off, identifies the block and is hashed to
find it in a small bucket array of block chains.

A matched block plus a slot determines the full key exactly, so the index
is inherently unique, needs no collision chain, and adds no intrusive
fields to the row.  Consecutive keys of one stream occupy consecutive
slots of one block.  Blocks are fixed-size and come from the basepool;
a block whose last slot clears is freed and recycled for the next block
of the same index, so a rolling window of keys (append at the tail,
retire at the head) reaches a steady state with no allocation and no
rehashing.  Only the small bucket array (one pointer per resident block)
grows with population.

A resident block costs 2^linbits pointers regardless of occupancy, so
pick linbits at or below the logarithm of the typical resident run of
consecutive keys; sparse keys degrade the index into one block per key.

### dmmeta.csize
<a href="#dmmeta-csize"></a>

The `dmmeta.csize` table allows user to declare the intended size, in bytes, of each ctype,
and its alignment. For example:

```ssim
dmmeta.csize  ctype:algw1.AccountEnableMsg         size:8   alignment:1  comment:""
dmmeta.csize  ctype:algw1.AccountLimitsMsg         size:88  alignment:1  comment:""
dmmeta.csize  ctype:algw1.AccountNoLoadMsg         size:8   alignment:1  comment:""
```

This declaration has no effect on the code generation. 
`amc` generates a static (compile-time) assertion in the `StaticCheck`
function in the target namespace. 
E.g.

### dmmeta.fbigend
<a href="#dmmeta-fbigend"></a>

This record indicates that the memory byte order of a field is big-endian.
Here is an example:

```ssim
dmmeta.field  field:atf_amc.TypeBE16.value  arg:u16  reftype:Val  dflt:""  comment:""
    dmmeta.fbigend  field:atf_amc.TypeBE16.value  comment:""
```

Amc generates `value_Get` and `value_Set` to access the value, and it adds `_be` to
the member's name in the C++ struct.  The suffix is there so that a programmer who
reaches past those accessors sees that the value in memory is not the value of the
field.  Both are in [the code amc writes for that
ctype](code:dmmeta.ctype:atf_amc.TypeBE16).

Amc reads/writes big-endian fields just like all other fields.

### dmmeta.fbitset
<a href="#dmmeta-fbitset"></a>

The fbitset record (not to be confused with `bitfld`) can be attached to any
numeric field, an inline array (`Inlary`) or a `Tary`. It generates standard bitset functions
Here is an example:

```ssim
inline-command: acr -t field:atf_amc.Bitset.fld8 | egrep Bitset
    dmmeta.ctype  ctype:atf_amc.Bitset  comment:"Test bitset"
      dmmeta.field  field:atf_amc.Bitset.fld8  arg:u8  reftype:Val  dflt:""  comment:"Bitset field"
        dmmeta.fbitset  field:atf_amc.Bitset.fld8  comment:""
        dmmeta.fcurs  fcurs:atf_amc.Bitset.fld8/bitcurs  comment:""
```

The bitset functions it generates read, set and clear one bit, count the bits that
are set, and combine two bitsets; they are in [the code amc writes for that
ctype](code:dmmeta.field:atf_amc.Bitset.fld8).

### dmmeta.fcast
<a href="#dmmeta-fcast"></a>

The fcast record adds an implicit conversion operator to the parent record.
Use it sparingly.
Here is an example:

```ssim
    dmmeta.fcast  field:algo.Bool.value  expr:""  comment:""
```

This adds the following function to the `algo::Bool` type:

```bash
    inline operator algo_BoolEnum() const;
```

### dmmeta.fconst
<a href="#dmmeta-fconst"></a>

The `fconst` record associates a name/value pair with a field. When reading from string,
the name (or the numeric value) can be used. When printing, the appropriate name is printed
Here is an example:
```ssim
inline-command: acr field:lib_json.FParser.state -l -tree -ndown 1
dmmeta.field  field:lib_json.FParser.state  arg:u32  reftype:Val  dflt:""  comment:""
  dmmeta.fconst  fconst:lib_json.FParser.state/ws          value:0   comment:""
  dmmeta.fconst  fconst:lib_json.FParser.state/value       value:1   comment:""
  dmmeta.fconst  fconst:lib_json.FParser.state/number      value:2   comment:""
  dmmeta.fconst  fconst:lib_json.FParser.state/str         value:3   comment:""
  dmmeta.fconst  fconst:lib_json.FParser.state/str_esc     value:4   comment:""
  dmmeta.fconst  fconst:lib_json.FParser.state/str_uesc    value:5   comment:""
  dmmeta.fconst  fconst:lib_json.FParser.state/str_usurr1  value:6   comment:""
  dmmeta.fconst  fconst:lib_json.FParser.state/str_usurr2  value:7   comment:""
  dmmeta.fconst  fconst:lib_json.FParser.state/token       value:8   comment:""
  dmmeta.fconst  fconst:lib_json.FParser.state/sec_line    value:9   comment:"JSON security line"
  dmmeta.fconst  fconst:lib_json.FParser.state/err         value:99  comment:"parser error"
report.acr  n_select:12  n_insert:0  n_delete:0  n_ignore:0  n_update:0  n_file_mod:0  n_badline:0
```

NOTE: The mapping from strings to integers is done using an efficient `static hash`.

The functions it generates convert between the name and the value, and print the
field as its name; they are in [the code amc writes for that
ctype](code:dmmeta.field:lib_json.FParser.state).

### dmmeta.fcurs
<a href="#dmmeta-fcurs"></a>

Some AMC cursors are generated by default. Others have to be requested by the user.
Example:
```ssim
dmmeta.fcurs  fcurs:atf_amc.FDb.cd_typed/delcurs         comment:""
```

See [curstype](/txt/ssimdb/amcdb/README.md) for the list of known cursor types.

Which cursors are generated by default, and which only on request, is the `dflt`
column of [amcdb.tcurs](amcdb.tcurs:%).

### dmmeta.fdec
<a href="#dmmeta-fdec"></a>

Fdec specifies that a field is implicitly scaled by some power of 10.
Example:
```ssim
dmmeta.fdec  field:algo.I32Dec4.value  nplace:4  fixedfmt:N  comment:""
```

The functions it generates convert the fixed-point value to and from a double, read
it from a string and print it; they are in [the code amc writes for that
ctype](code:dmmeta.field:algo.I32Dec4.value).

The `fixedfmt` option controls whether scientific notation is allowed to be used when printing such field.
Set it to `Y` to disable scientific notation.

### dmmeta.fldoffset
<a href="#dmmeta-fldoffset"></a>

The `dmmeta.fldoffset` table is similar to csize, but declares field ofset within a struct.
This, too, has no effect on code generation and results in a static assert being added
to a function `StaticCheck` in the target namespace.

Example:

```ssim
dmmeta.fldoffset  field:algw1.AccountEnableMsg.account  offset:4
dmmeta.fldoffset  field:algw1.AccountEnableMsg.enable   offset:2
dmmeta.fldoffset  field:algw1.AccountEnableMsg.pad      offset:3
```

Generated code:

```c++
+    algo_assert(_offset_of(algw1::AccountEnableMsg, enable) == 2);
+    algo_assert(_offset_of(algw1::AccountEnableMsg, pad) == 3);
+    algo_assert(_offset_of(algw1::AccountEnableMsg, account) == 4);
```

### dmmeta.floadtuples
<a href="#dmmeta-floadtuples"></a>

A row names one command-line field that holds the path of a tuple source, so
`command.acr_ed.in` says that acr_ed reads its tables from the directory `-in`
points at.  A command may name more than one, because its rows do not all come
from the same place: a tool may read the source tree at `-in` and then the
infrastructure inventory at `-invdir`, and each field gets a row of its own.

Two attributes say what a reader may do with a source.  `optional` marks a
source that need not be there — a layer nobody has attached is the ordinary
state of a fresh checkout, so the process skips it rather than failing, and the
generated startup load reads only the sources without the mark.  `autoscan`
marks a source whose contents a user may name on a command line, which is what
`acr_compl` reads when it offers completions: a node key that lives only in the
inventory is offered from there exactly as a ctype key is offered from `data`.

### dmmeta.fprefix
<a href="#dmmeta-fprefix"></a>

Field names carry a light version of
[Hungarian Notation](https://en.wikipedia.org/wiki/Hungarian_notation):
a prefix such as `zd` or `ind`, followed by an underscore, announces the
field's reftype to the reader.  One prefix can be shared by several
reftypes (`ind` names both a Thash and a Blkhash index), and one reftype
can accept several prefixes, so the table holds one record per allowed
(prefix, reftype) pairing, keyed `prefix.reftype`.

The `require` flag controls the check amc performs.  If a reftype has any
pairing with `require:Y`, every field of that reftype must be named under
one of the reftype's registered prefixes.  A reftype whose pairings are
all `require:N` (Ptr, Delptr) also accepts arbitrary field names; its
rows only document the customary prefixes.

The `dflt` flag serves acr_ed: when `-reftype` is not given, acr_ed
infers it from the field-name prefix using the pairing marked `dflt:Y`
(at most one per prefix, checked by amc).

Additional pairings may be defined by the user.

### dmmeta.gconst
<a href="#dmmeta-gconst"></a>

The `gconst` record creates an enum type from two columns of an ssim file.
One column becomes the name, another becomes the value. If the target field is of string type,
then compile-time string constants are generated instead.
See [fconst](/txt/ssimdb/dmmeta/README.md#dmmeta-fconst) for more documentation.

### dmmeta.gstatic
<a href="#dmmeta-gstatic"></a>

Whenever gstatic is specified, the contents of an ssim
table are ``statically'' included into the output source file for the given namespace
The contents of the table are inserted into the in-memory database at startup,
as part of FDb's constructor.

When gstatic is combined with Inlary and the field is global (i.e. in `FDb`), the individual addresses
of elements in the table are compile-time constants. Amc celebrates this fact by declaring a C++ compiler symbol
for each row of the table. By the time the user code starts executing, all of the elements have been inserted
in the array and properly cross-referenced, and the user code can use the generated globals to access table rows.

Here is an example:
```ssim
dmmeta.field  field:atf_amc.FDb.listtype  arg:atf_amc.FListtype  reftype:Inlary  dflt:""  comment:""
    dmmeta.gstatic  field:atf_amc.FDb.listtype  comment:""
    dmmeta.inlary  field:atf_amc.FDb.listtype  min:0  max:10  comment:""
```

This generates the following compile-time references:
```c++
static atf_amc::FListtype &atf_amc_listtype_cd    = ((atf_amc::FListtype*)atf_amc::_db.listtype_data)[0];
static atf_amc::FListtype &atf_amc_listtype_cdl   = ((atf_amc::FListtype*)atf_amc::_db.listtype_data)[1];
static atf_amc::FListtype &atf_amc_listtype_cs    = ((atf_amc::FListtype*)atf_amc::_db.listtype_data)[2];
static atf_amc::FListtype &atf_amc_listtype_csl   = ((atf_amc::FListtype*)atf_amc::_db.listtype_data)[3];
```

Compare `gstatic` to [finput](/txt/ssimdb/dmmeta/README.md), which indicates that records can be dynamically loaded
into the process at runtime (and at startup, by reading an input file). By contrast, gstatic records become
part of the source code.

Tables marked `gstatic` have known contents. This enables associating entries of such tables with C functions.
Consider this example from amc:
```ssim
  dmmeta.field  field:amc.FDb.gen  arg:amc.FGen  reftype:Lary  dflt:""  comment:""
    dmmeta.gstatic  field:amc.FDb.gen  comment:""

  dmmeta.field  field:amc.FGen.step  arg:""  reftype:Hook  dflt:""  comment:""
      dmmeta.hook  field:amc.FGen.step  comment:""
```
Here, a static table `gen` has been declared, and its contents are populated with entires from
`amcdb.gen` in amc's FDb constructor. The table can have a hash index by primary key and there
is not restriction on any other cross-references. For this pattern of declaration, amc forward-declares one
function foreach element of the table, and assigns its address to the `hook` field of `amc::FGen` upon construction.
If the function is not defined in the user process, a link error will result.

This construct is very useful for things like unit tests, consistency checks, alerts, and other situations where
an extensible list of functions is needed together with meta-information about them.
Each function has an associated record which can be looked up by name, so it is possible to invoke all
functions matching a regex passed on command line, or describe them with comments.

The generator's own functions are the example: each one is a row of
[amcdb.gen](amcdb.gen:%), and the row is what makes amc call it -- so
[gen_cget](func:amc.gen_cget) runs because a record says it does.

In language-centric systems, one would start by writing some functions, and then use "reflection" to
scrape the functions from the code (usually at runtime) and do things with them. Starting with a plain text file
on disk, and then generating C++ functions and in-memory tables from it is a lot more elegant. No "reflection"
is needed, everything is forward-looking.

Gstatic also behaves in a special way when used with records containing `Hook` fields.
See [Hook](/txt/ssimdb/dmmeta/README.md#dmmeta-hook)

### dmmeta.hook
<a href="#dmmeta-hook"></a>

A Hook field is a pointer to a function.

When a record with Hook is created at process startup time using [gstatic](/txt/ssimdb/dmmeta/README.md#dmmeta-gstatic),
amc forward-declares one function for each record in the table; the name of the function is based
on the primary key of the record.
These are user-implemented functions, and their pointers are automatically assigned to hook fields
as part of process startup initialization.
Thus, one can associate one user-implemented function with each row of a ssim table.
There are no restrictions on the target table -- it can be indexed and cross-referenced in any way.
This is a very commonly used trick to structure processes.

### dmmeta.inlary
<a href="#dmmeta-inlary"></a>

The `Inlary` generates code to turn a single value field
into an inline array of up to N values. An inline array is located directly in the
record which houses it. There is no dynamic allocation. There are two types of `Inlary`:
ones where `min = max`, in this case a C++ array is generated.
And the case where `min < max`. In this case, `amc` declares an array of `u8` of appropriate size,
and generates generates usual pool functions `Alloc`, `RemoveLast`, `RemoveAll`.
Here is an example:

```ssim
inline-command: acr -t field:algo_lib.FDb.temp_strings | egrep algo_lib.FDb
    dmmeta.ctype  ctype:algo_lib.FDb  comment:"In-memory database for algo_lib"
      dmmeta.field  field:algo_lib.FDb.temp_strings  arg:algo.cstring  reftype:Inlary  dflt:""  comment:""
        dmmeta.inlary  field:algo_lib.FDb.temp_strings  min:8  max:8  comment:""
```

The two shapes generate different functions.  [A fixed array](code:dmmeta.field:algo_lib.FDb.temp_strings)
carries the array accessors alone, where [a non-fixed one](code:dmmeta.field:atf_amc.FTypeA.typec)
adds the pool functions that allocate an element and remove one.

### dmmeta.llist
<a href="#dmmeta-llist"></a>

Amc generates 32 flavors of linked lists, almost as many as Heinz for ketchup.
Linked lists can be:

Singly or doubly linked (as indicated by letter s or d in the prefix)
Zero-terminated or circular (as indicated by letter z or c in the prefix)
Fifo or lifo (as indicated by presence of letter l in the prefix, l for lifo)
With or without count
With or without tail pointer

Circular linked lists are often used with steps, because it is convenient to call RotateFirst
to both grab an element off the head of the list, and move this element to the back.

Here is an example:
```ssim
dmmeta.llist  field:atf_amc.FDb.zd_typed           havetail:Y  havecount:Y  comment:""
dmmeta.llist  field:atf_amc.FDb.cd_typed          havetail:N  havecount:Y  comment:""
```

The tail and count options come from the llist record.  The remaining options come
from its list type, and the list types are the rows of
[dmmeta.listtype](dmmeta.listtype:%).

The functions generated for a circular list are in [the code amc writes for that
ctype](code:dmmeta.field:atf_amc.FDb.cd_typed).

### dmmeta.pack
<a href="#dmmeta-pack"></a>

Using the `dmmeta.pack` record you can request that the generated struct be wrapped
with the `#pragma pack`:

```ssim
dmmeta.pack  ctype:algw1.AccountEnableMsg       comment:""
```

Here is the generated code:

```c++
// --- algw1.AccountEnableMsg
#pragma pack(push,1)
struct AccountEnableMsg { // algw1.AccountEnableMsg
    explicit AccountEnableMsg(bool                           in_enable
        ,i32                            in_account);
    AccountEnableMsg();
};
#pragma pack(pop)
```

Another way to make sure that ALL structs in a namespace are 1-byte packed
is to set the attribute `pack:Y` in the `nsx` (namespace extra) record:

```ssim
dmmeta.nsx  ns:algw1  genthrow:N  correct_getorcreate:Y  pool:algo_lib.FDb.lpool  sortxref:N  pack:Y  fldoffset_asserts:Y  comment:""
```

### dmmeta.pbsyntax
<a href="#dmmeta-pbsyntax"></a>

The protobuf syntax a message follows, named by
[dmmeta.cpbuf](/txt/ssimdb/dmmeta/README.md).  The two variants differ in how a
field's absence is expressed on the wire, which is what the generated encoder
has to decide for every field it writes.

The syntax governs one decision only: whether a scalar equal to its zero value
is skipped.  That is what `implicit_presence` names, and it is the attribute
the encoder reads — the syntax name itself does not appear in the generator, so
a further variant is a row here rather than a code change.  A syntax that has
no row is rejected: `acr -check` refuses the `cpbuf` row, and `amc` refuses the
schema it was handed, since a schema read straight out of a directory does not
pass through the reference check.  A field that carries a presence mask
([dmmeta.pmaskfld](/txt/ssimdb/dmmeta/README.md#dmmeta-pmaskfld)) is written when its present
bit is set, under either syntax, so `syntax:proto2` on a ctype without a
presence mask writes every field on every encode.

### dmmeta.pmaskfld
<a href="#dmmeta-pmaskfld"></a>

Indicate that the field holds a presence mask.
then amc allocates 1 bit in the pmask for each field in the struct. The bits are initially zero.
The field can be of any integer type, or an Inlary.

Amc then generates a Set, PresentQ, and SetPresent functions for each field, unless one already exists.
Whenever the Set function is called on a field, the corresponding pmask bit is set to 1.
When reading from a string, the pmask bits are populated for each scanned attribute.
When printing, fields with zero pmask bit are not printed.

Regardless of pmask status, all fields of a struct are initialized to default values upon construction,
so there is no speed to be gained from using pmask. Access to fields is not protected in any way --
they can be accessed as if the pmask didn't exist. The generated PresentQ function can be used to check
if a field was previously assigned a value with the `Set` function.

When reading a record from a string, `amc` uses the `Set` function if one exists. So, one can read
a tuple and know which attributes were present, by examining the pmask 

It is possible to have multiple `pmaskfld` fields in a struct, each for its own purpose.
In this case, use [pmaskfld_member](/txt/ssimdb/dmmeta/README.md#dmmeta-pmaskfld_member) to manually assign which fields
a given presence mask covers. If none are specified, then all fields are added to the presence mask.
If the `pmaskfld` field is named `present` or `pmask`, then `PresentQ`, `SetPresent` functions are generated.
If it's named `blah`, then the functions become `BlahQ`, `SetBlah`.

### dmmeta.pmaskfld_member
<a href="#dmmeta-pmaskfld_member"></a>

Manually assign a field to a presence mask.
Here is an example:
```ssim
dmmeta.pmaskfld_member  pmaskfld_member:atf_amc.PmaskMultiple.nullable/atf_amc.PmaskMultiple.value1                                comment:""
dmmeta.pmaskfld_member  pmaskfld_member:atf_amc.PmaskMultiple.nullable/atf_amc.PmaskMultiple.value3                                comment:""
dmmeta.pmaskfld_member  pmaskfld_member:atf_amc.PmaskMultiple.nullable/atf_amc.PmaskMultiple.value5                                comment:""
```
See [pmaskfld](/txt/ssimdb/dmmeta/README.md#dmmeta-pmaskfld) for more information.

### dmmeta.printfmt
<a href="#dmmeta-printfmt"></a>

List of supported print formats

### dmmeta.ptrary
<a href="#dmmeta-ptrary"></a>

This record is required for any field with reftype:Ptrary.
Example:
```bash
acr ptrary:atf_amc.% | head -1
dmmeta.ptrary  field:atf_amc.FCascdel.c_child_ptrary  unique:Y
```

The attribute `unique` controls whether only a pointer to one record
can be added to the array more than once. Without `unique:Y`, you could add a pointer
multiple times. Using `ptrary` with `xref` is only allowed if `unique:Y`.
A flag is added to the child record to record whether it's inserted in the array.
Removing a record from `ptrary` involves linear scanning back-to-front to find the element
and shifting the remaining elements over by one. Removing from a non-unique
ptrary only removes the first instance and is generally not recommended.

The functions it generates are in [the code amc writes for that
ctype](code:dmmeta.field:atf_amc.FCascdel.c_child_ptrary).

### dmmeta.reftype
<a href="#dmmeta-reftype"></a>

This table lists all available reftypes ("type constructors").
For more information, see [Reftypes](/txt/exe/amc/reftype.md).

### dmmeta.rowid
<a href="#dmmeta-rowid"></a>

Use rowid to automatically initialize the field's value to its index in the enclosing array.
This works if the field pool type is `Lary` or `Inlary`..

### dmmeta.smallstr
<a href="#dmmeta-smallstr"></a>

Smallstr is a fixed-length character field. Memory is reserved inline in the parent struct.
Strings can be length-suffixed (Rpascal), left-padded or right-padded.
For padded strings, the string value is calculated by discarding the pad characters from the edge.
Any smallstr is castable to strptr.
The small string types are the rows of [dmmeta.strtype](dmmeta.strtype:%).

See [Strings](/txt/exe/amc/string.md) for more information

### dmmeta.sorttype
<a href="#dmmeta-sorttype"></a>

List of supported sort types

### dmmeta.sqltype
<a href="#dmmeta-sqltype"></a>

Mapping of ctype to SQL expression.
This table is used when converting OpenACR schema to MariaDB schema.

### dmmeta.ssimfile
<a href="#dmmeta-ssimfile"></a>

This record indicates that a ssimfile exists for the given ctype.

### dmmeta.ssimsort
<a href="#dmmeta-ssimsort"></a>

This record specifies which field the ssimfile is sorted on.
It is optional (i.e. not all ssimfiles are sorted).
A ssimfile is considered fully sorted if the sort field is the primary key.
Otherwise, it's partially sorted. For instance, the `dmmeta.field`
table is only sorted on the `ctype` attribute, since the order of fields
in a structure is not lexicographical.

### dmmeta.steptype
<a href="#dmmeta-steptype"></a>

List of supported step types

### dmmeta.strfmt
<a href="#dmmeta-strfmt"></a>

This table describes various string-based formats for a ctype.

### dmmeta.strtype
<a href="#dmmeta-strtype"></a>

This table describes small string types
For more information, see [Strings](/txt/exe/amc/string.md)

### dmmeta.substr
<a href="#dmmeta-substr"></a>

This record indicates that the field value is calculated by taking a substring of another
field. The substring field is not necessarily itself a string. It can be any type, in which case
accessing the field (with the generated `_Get` function) involves reading the field value from
the string.

Substrings are used pervasively by acr to construct structured keys.

### dmmeta.svtype
<a href="#dmmeta-svtype"></a>

This table helps `sv2ssim` map CSV fields to ctypes.

### dmmeta.tary
<a href="#dmmeta-tary"></a>

This record is required on fields with [reftype:Tary](/txt/exe/amc/reftype/Tary.md)

### dmmeta.thash
<a href="#dmmeta-thash"></a>

This record is required on fields with [reftype:Thash](/txt/exe/amc/reftype/Thash.md)

A hash table is implemented as a Tary of pointers.
Collisions are implemented as a singly linked list.
Hash tables grow automatically.
Whenever the number of entries in the hash table is greater
than the number of buckets (pointers), the hash table size is doubled.
The size of the array of pointers is always a power of 2. This means that a hash function has
to be good. Amc can generate hash functions for any ctype, typically CRC32 is used.

Hash tables can be unique of non-unique. A unique hash table rejects insertions of duplicate keys.
A non-unique hash table allows them.

For records that have only one hash access path defined for them, amc generates a GetOrCreate function
which is a convenient way to force creation of an element when you know its key.

The following functions are generated:
```bash
amc -report:N atf_amc.FDb.ind_typea.% -proto
```

### dmmeta.typefld
<a href="#dmmeta-typefld"></a>

This record indicates that a field acts as a type field for a message.
It is widely used when describing [protocols](/txt/exe/amc/proto.md)

### dmmeta.xref
<a href="#dmmeta-xref"></a>

See [xref](/txt/exe/amc/xref.md)

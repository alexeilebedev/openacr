## dmmeta.fprefix - Mapping between field prefix and Reftype
<a href="#dmmeta-fprefix"></a>

The table `dmmeta.fprefix` defines a constraint between field prefix and a field reftype,
as interpreted by amc. If the field's name begins with the prefix (such as zd), followed
by an underscore, then the field must have the specified reftype. It's a light version of
[Hungarian Notation](https://en.wikipedia.org/wiki/Hungarian_notation)

The defined prefixes are:

```
inline-command: acr fprefix | ssimfilt ^ -t
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

Additional prefixes may be defined by the user.

### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Fprefix

* file:[data/dmmeta/fprefix.ssim](/data/dmmeta/fprefix.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|fprefix|[algo.Smallstr5](/txt/protocol/algo/README.md#algo-smallstr5)|[Val](/txt/exe/amc/reftypes.md#val)|||
|reftype|[dmmeta.Reftype](/txt/ssimdb/dmmeta/reftype.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Subsets
<a href="#subsets"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Subsets -->
These ssimfiles are subsets of dmmeta.fprefix

* [dmmeta.Listtype](/txt/ssimdb/dmmeta/listtype.md) - Specify structure of linked list based on field prefix 

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Subsets -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [acr_ed](/txt/exe/acr_ed/internals.md) as [acr_ed.FFprefix](/txt/exe/acr_ed/internals.md#acr_ed-ffprefix)
* [amc](/txt/exe/amc/internals.md) as [amc.FFprefix](/txt/exe/amc/internals.md#amc-ffprefix)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->


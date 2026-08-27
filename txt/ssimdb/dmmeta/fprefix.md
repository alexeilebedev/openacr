## dmmeta.fprefix - Allowed pairing of field-name prefix and reftype
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

The defined pairings are:

```
inline-command: acr fprefix | ssimfilt ^ -t
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

Additional pairings may be defined by the user.

### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Fprefix

* file:[data/dmmeta/fprefix.ssim](/data/dmmeta/fprefix.ssim)

italicised fields: *prefix, reftype* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|fprefix|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype/Val.md)||prefix.reftype pairing|
|*prefix*|*[algo.Smallstr5](/txt/protocol/algo/README.md#algo-smallstr5)*|*[Val](/txt/exe/amc/reftype/Val.md)*||*Field-name prefix<br>.LL of fprefix*|
|*reftype*|*[dmmeta.Reftype](/txt/ssimdb/dmmeta/reftype.md)*|*[Pkey](/txt/exe/amc/reftype/Pkey.md)*||*<br>.LR of fprefix*|
|require|bool|[Val](/txt/exe/amc/reftype/Val.md)|true|Reftype must use one of its registered prefixes|
|dflt|bool|[Val](/txt/exe/amc/reftype/Val.md)|true|Reftype acr_ed infers for this prefix; at most one per prefix|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [acr_ed](/txt/gen/acr_ed/acr_ed.md) as [acr_ed.FFprefix](/txt/gen/acr_ed/acr_ed.md#acr_ed-ffprefix)
* [amc](/txt/gen/amc/amc.md) as [amc.FFprefix](/txt/gen/amc/amc.md#amc-ffprefix)

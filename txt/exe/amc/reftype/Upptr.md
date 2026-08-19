## amc Reftype: Upptr
<a href="#amc-reftype-upptr"></a>

`Upptr` is a **back-pointer to a pre-existing record** — most
commonly the "parent" record of a hierarchical relationship.
The field is a plain `T*` in the row; its value is set by the
cross-reference machinery when the row is inserted into a
parent's index, and used downstream as the canonical pointer
from child up to parent.

Unlike [Ptr](/txt/exe/amc/reftype/Ptr.md), Upptr is not itself
an xref target — no other index inserts rows into an Upptr.
It is the receiving end of the **via:** path on an xref
declared elsewhere.

```
dmmeta.field  field:abt.FTargdep.p_parent  arg:abt.FTarget  reftype:Upptr  dflt:""  comment:"parent target"
```

By the project's prefix convention, Upptr field names begin
with `p_` (`p_parent`, `p_target`, …).

### What it generates
<a href="#what-it-generates"></a>

A single pointer in the parent ctype
(see `tclass_Upptr` in `cpp/amc/upptr.cpp`):

| Field        | Type    | Meaning                            |
|--------------|---------|------------------------------------|
| `<name>`     | `T*`    | NULL or pointer to the parent row  |

`tclass_Upptr` does no other work — Upptr has no `Insert`,
`Remove`, or `Cascdel`.  The pointer is populated by whichever
xref (often a [Thash](/txt/exe/amc/reftype/Thash.md) on the
parent's FDb) uses this Upptr as its `via:` target.

### Ssim inputs
<a href="#ssim-inputs"></a>

Required:

- `dmmeta.field` with `reftype:Upptr`.  `arg:` is the parent
  ctype.  The field name must start with `p_`
  ([field name prefixes](/txt/exe/amc/reftype.md#field-name-prefixes)).

That's it.  No `dmmeta.upptr` record, no spec parameters.

### Generated functions
<a href="#generated-functions"></a>

Source: `cpp/amc/upptr.cpp`.

| Tfunc          | Generated function                  | Effect |
|----------------|-------------------------------------|--------|
| `Upptr.Init`   | `<name>_Init(P&)` (inline, folded)  | `parent.<name> = NULL;` |

Upptr is the only reftype with exactly **one** tfunc.  Its
job is to declare the storage and let the rest of amc see it.

### Memory model
<a href="#memory-model"></a>

- One pointer in the row.
- Set/cleared exclusively by `XrefMaybe` / `Unref` via another
  reftype's `via:` chain.  Don't write to `<name>` directly.
- Parent must live in stable storage
  ([Lary](/txt/exe/amc/reftype/Lary.md) or similar) for the
  back-pointer to stay valid.

### Pitfalls
<a href="#pitfalls"></a>

- **Don't write directly.**  Upptr is a derived value
  maintained by the cross-reference engine.  Hand-writing the
  pointer makes the index believe a record is somewhere it
  isn't.
- **NULL means "not in the index".**  Until the xref's
  `_InsertMaybe` succeeds, the Upptr is NULL.  Check before
  dereferencing.
- **Parent must use a permanent-pointer pool.**  Tary parents
  invalidate Upptrs on grow — amc enforces this with the
  same "no xref on Tary" rule, but the failure is at xref
  time, not at field-declaration time.
- **No `dmmeta.upptr` spec record.**  Don't try to attach one;
  amc has none.

### See also
<a href="#see-also"></a>

- [Reftypes index](/txt/exe/amc/reftype.md)
- [Ptr](/txt/exe/amc/reftype/Ptr.md) — forward-pointer sibling
- [Pkey](/txt/exe/amc/reftype/Pkey.md) — text-key back-reference, resolved at load time
- [Cross-references](/txt/exe/amc/xref.md) — how `via:` paths populate Upptrs
- Source: `cpp/amc/upptr.cpp`
- Tfunc records: `acr 'tfunc:Upptr.%'`

### Example
<a href="#example"></a>

`abt` indexes targets by name in a Thash; every target
dependency has a Upptr back to its parent target:

```
dmmeta.field  field:abt.FTargdep.p_parent  arg:abt.FTarget  reftype:Upptr

dmmeta.field  field:abt.FTarget.c_targdep  arg:abt.FTargdep  reftype:Ptrary
  dmmeta.xref  field:abt.FTarget.c_targdep  inscond:true
              via:abt.FDb.ind_target/abt.FTargdep.target
```

The xref says "to find each targdep's parent target, look it
up in `ind_target` by `targdep.target`."  When the Ptrary
inserts a targdep into `c_targdep`, it also sets the
targdep's Upptr `p_parent` to the target row.  Now any
consumer can navigate with one pointer:

```c++
abt::FTargdep &td = ...;
abt::FTarget *parent = td.p_parent;   // single load, no lookup
```

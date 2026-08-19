## amc Reftype: Ptr
<a href="#amc-reftype-ptr"></a>

`Ptr` is the **simplest cross-reference index** — a single
pointer that holds at most one record at a time.  When wired
to a `dmmeta.xref`, `Ptr_InsertMaybe` accepts the first row to
match the xref and rejects subsequent ones until the slot is
cleared by `Remove`.  Use it for "this record optionally
points at one other record" relationships.

```
dmmeta.field  field:atf_amc.FCascdel.p_parent  arg:atf_amc.FCascdel  reftype:Ptr  dflt:""  comment:""
```

### What it generates
<a href="#what-it-generates"></a>

A single pointer in the parent ctype
(see `tclass_Ptr` in `cpp/amc/ptr.cpp`):

| Field        | Type    | Meaning                                |
|--------------|---------|----------------------------------------|
| `<name>`     | `T*`    | NULL or pointer to the indexed record  |

`Ptr` does **not** support a default value — amc rejects any
`dflt` on a Ptr field with `"Default value for Ptr is not
supported (it's always NULL)"`.

### Ptr vs Upptr
<a href="#ptr-vs-upptr"></a>

Both `Ptr` and `Upptr` are plain `T*` pointers, but they play
different roles in amc's cross-reference model:

| | **Ptr** | **Upptr** |
|---|---|---|
| Role | Forward pointer, set by xref insertion | Parent pointer, set when child is inserted into the parent |
| `_InsertMaybe` | Yes — Ptr can be a cross-reference target | No — Upptr is read-only |
| Auto-population | When `dmmeta.xref` matches | Set by another xref's `via:` path |
| Cardinality | One in-pointer per parent | One per child (back-pointer) |

### Ssim inputs
<a href="#ssim-inputs"></a>

Required:

- `dmmeta.field` with `reftype:Ptr`.  `arg:` is the target ctype.
  `c_` and `p_` are the conventional field-name prefixes but Ptr
  accepts any name
  ([field name prefixes](/txt/exe/amc/reftype.md#field-name-prefixes)).

Optional:

- `dmmeta.xref` — wires automatic insertion/removal.  Without
  an xref the pointer is purely user-managed.

### Generated functions
<a href="#generated-functions"></a>

Source: `cpp/amc/ptr.cpp`.

| Tfunc                  | Generated function                                                      | When emitted | Effect |
|------------------------|-------------------------------------------------------------------------|--------------|--------|
| `Ptr.Init`             | `<name>_Init(P&)` (inline, folded)                                      | always       | `parent.<name> = NULL;` |
| `Ptr.InsertMaybe`      | `bool <name>_InsertMaybe(P&, T& row)`                                   | when xref present | Set the pointer if it is NULL or already pointing at `row`; otherwise return false. |
| `Ptr.Remove`           | `void <name>_Remove(P&, T& row)`                                        | when xref present | If pointer is currently `&row`, NULL it. |
| `Ptr.Cascdel`          | (private)                                                               | when xref has `cascdel:Y` | Delete the pointed-to row, then NULL the pointer. |

### Pitfalls
<a href="#pitfalls"></a>

- **One in-pointer per parent.**  `_InsertMaybe` rejects any
  conflict.  If you need to index multiple children per
  parent, use [Ptrary](/txt/exe/amc/reftype/Ptrary.md) /
  [Thash](/txt/exe/amc/reftype/Thash.md) /
  [Llist](/txt/exe/amc/reftype/Llist.md) instead.
- **No default value.**  Always starts NULL.
- **Stable storage required.**  Like every xref, Ptr depends
  on the target row not moving.  Allocate targets via
  [Lary](/txt/exe/amc/reftype/Lary.md) or
  [Tpool](/txt/exe/amc/reftype/Tpool.md).
- **Computed Ptr (`FldfuncQ`) is allowed** — no storage in
  that case, no `Init`, no `Insert/Remove`.  The pointer is
  derived on the fly from another expression.

### See also
<a href="#see-also"></a>

- [Reftypes index](/txt/exe/amc/reftype.md)
- [Upptr](/txt/exe/amc/reftype/Upptr.md) — parent-direction sibling
- [Ptrary](/txt/exe/amc/reftype/Ptrary.md) — pointer array
- [Cross-references](/txt/exe/amc/xref.md)
- Source: `cpp/amc/ptr.cpp`
- Tfunc records: `acr 'tfunc:Ptr.%'`

### Example
<a href="#example"></a>

A cascade-delete test — every child points at its parent, and
deleting the parent cascades:

```
dmmeta.field  field:atf_amc.FCascdel.p_parent  arg:atf_amc.FCascdel  reftype:Ptr
  dmmeta.xref  field:atf_amc.FCascdel.p_parent  inscond:"row.parent != NULL"  via:atf_amc.FDb.ind_cascdel/atf_amc.FCascdel.parent
```

In C++:

```c++
atf_amc::FCascdel parent;
atf_amc::FCascdel child;
child.parent = parent.id;
if (cascdel_InsertMaybe(child)) {
    // child.p_parent is now &parent
}
```

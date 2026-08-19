## amc Reftype: Inlary
<a href="#amc-reftype-inlary"></a>

`Inlary` is a **fixed-bounded inline array** whose storage
lives directly inside the parent struct.  No heap allocation,
no extra pointer chasing — just an array of `max` elements with
an active count.  Two modes:

- **Fixed (`min == max`)** — the array always has `max`
  elements; behaves like an embedded C array.  No `_Alloc`,
  no `_n`.
- **Variable (`min < max`)** — `_n` tracks how many slots are
  in use; `_Alloc` extends it up to `max`, after which
  `AllocMem` returns NULL.

```
dmmeta.field   field:net.Ipv6.octet  arg:u8  reftype:Inlary  dflt:""  comment:""
  dmmeta.inlary  field:net.Ipv6.octet  min:16  max:16  comment:"fixed 16-byte address"
```

### What it generates
<a href="#what-it-generates"></a>

Storage in the **parent** ctype
(see `tclass_Inlary` in `cpp/amc/inlary.cpp`):

#### Fixed (`min == max`)
<a href="#fixed-min-max-"></a>

| Field            | Type        | Meaning             |
|------------------|-------------|---------------------|
| `<name>_elems[N]`| `T`         | `N` slots inline    |

No counter — the size is `max` and there are no `_Alloc`
operations.

#### Variable (`min < max`)
<a href="#variable-min-max-"></a>

| Field             | Type    | Meaning                                            |
|-------------------|---------|----------------------------------------------------|
| `<name>_data[]`   | `u8` or `u128` array | raw bytes sized for `max` elements; placement-new'd as needed |
| `<name>_n`        | `i32`   | current element count                               |
| `<name>_max`      | enum    | compile-time constant equal to `max`                |

Variable mode pulls in the `Pool` tclass — Inlary fields get
`Alloc` / `AllocMaybe` / `Delete` / `InsertMaybe` etc. on top
of the primitives below.

The `u128` row size for unpacked element types avoids the
alignment surprise that an `u8`-backed array would cause for
larger elements.

### Ssim inputs
<a href="#ssim-inputs"></a>

Required:

- `dmmeta.field` with `reftype:Inlary`.  `arg:` is the element
  type.
- `dmmeta.inlary`:
  - `min` — minimum number of elements (often 0).
  - `max` — maximum number of elements (compile-time constant).

If you have a `dmmeta.gstatic` on the field, amc auto-bumps
`max` to the number of static rows, so the array can hold the
whole gstatic table.

### Generated functions
<a href="#generated-functions"></a>

Source: `cpp/amc/inlary.cpp`.  Many tfuncs are emitted only in
the variable-size mode.

| Tfunc                  | Generated function                              | When emitted | Effect |
|------------------------|-------------------------------------------------|--------------|--------|
| `Inlary.Init`          | `<name>_Init(P&)` (folded into parent)          | always       | Set `_n=0` (variable) or default-init each slot (fixed). |
| `Inlary.Uninit`        | `<name>_Uninit(P&)`                             | always       | Destruct active elements. |
| `Inlary.N`             | `i32 <name>_N(P&)`                              | always       | `_n` (variable) or `max` (fixed). |
| `Inlary.Max`           | `i32 <name>_Max(P&)`                            | always       | `max`. |
| `Inlary.EmptyQ`        | `bool <name>_EmptyQ(P&)`                        | variable     | `_n == 0`. |
| `Inlary.AllocMem`      | `void* <name>_AllocMem(P&)`                     | variable     | Reserve next slot, return raw memory; NULL when full. |
| `Inlary.Find`          | `T* <name>_Find(P&, Rowid t)`                   | always       | Bounded find; NULL if `t >= _n`. |
| `Inlary.qFind`         | `T& <name>_qFind(P&, Rowid t)`                  | always       | Unchecked. |
| `Inlary.Getary`        | `aryptr<T> <name>_Getary(P&)`                   | always       | Span over the active elements. |
| `Inlary.Fill`          | `void <name>_Fill(P&, T value)`                 | fixed, copyable elements | Assign every slot. |
| `Inlary.Setary`        | `void <name>_Setary(P&, aryptr<T> rhs)`         | variable     | Copy from aryptr. |
| `Inlary.RemoveAll`     | `void <name>_RemoveAll(P&)`                     | variable     | Destruct each element, `_n = 0`. |
| `Inlary.RemoveLast`    | `void <name>_RemoveLast(P&)`                    | variable     | Pop; no-op if empty. |
| `Inlary.RowidFind`     | `<imrow helper>`                                | when `reflect:Y` | Reflection accessor. |
| `Inlary.rowid_Get`     | `u64 <name>_rowid_Get(P&, T& elem)`             | always       | Index of `elem` within the array. |
| `Inlary.Eq`            | `bool <name>_Eq(P&, P&)`                        | when elements comparable | Element-wise equality. |
| `Inlary.Cmp`           | `i32 <name>_Cmp(P&, P&)`                        | when elements ordered | Lexicographic. |
| `Inlary.curs`          | `<P>_<name>_curs` + accessors                   | always       | Forward cursor. |
| `Inlary.ReadStrptrMaybe`| `bool <name>_ReadStrptrMaybe(P&, strptr)`      | when separator defined or arg is char/u8 | Parse into the array. |

Variable Inlary also gets the Pool family (`Alloc`,
`AllocMaybe`, etc.) since it is `inst:Y` and `hasalloc:N` but
chains via `GenTclass(amc_tclass_Pool)`.

### Memory model
<a href="#memory-model"></a>

- All storage is in the parent — sizeof(parent) includes the
  whole array up front.
- Variable mode placement-news new elements at `_data + n` on
  `_Alloc`.  Removal in `_RemoveLast` runs the destructor.
- No heap allocation, no fragmentation, no pointer
  invalidation.  Pointers into the array are stable for the
  lifetime of the parent.

### Pitfalls
<a href="#pitfalls"></a>

- **Storage cost is `max * sizeof(T)`** regardless of `_n`.
  Inlary is wasteful for sparsely-populated large arrays;
  use [Tary](/txt/exe/amc/reftype/Tary.md) or
  [Lary](/txt/exe/amc/reftype/Lary.md) for those.
- **`max` must be a compile-time constant.**  amc errors out
  if `max==0` (with no gstatic providing the count).
- **Separator collisions.**  When printing, the chosen
  separator must not collide with the element's printer's
  own separator — amc warns about common cases (`' '` vs.
  Tuple, etc.) but does not catch every misconfiguration.
- **Fixed mode has no `_Alloc`.**  All slots are always
  active; modify them in place via `_qFind`.
- **`_AllocMem` returns NULL on overflow.**  No fatal error,
  so the caller must check.
- **Computed fields not allowed.**  amc rejects
  `FldfuncQ(field)` for fixed Inlary.

### See also
<a href="#see-also"></a>

- [Reftypes index](/txt/exe/amc/reftype.md)
- [Tary](/txt/exe/amc/reftype/Tary.md) — heap-backed dynamic array
- [Lary](/txt/exe/amc/reftype/Lary.md) — heap-backed with permanent pointers
- [Hook](/txt/exe/amc/reftype/Hook.md) — Inlary of hooks is the gstatic pattern
- Source: `cpp/amc/inlary.cpp`
- Tfunc records: `acr 'tfunc:Inlary.%'`
- Spec table: `acr 'dmmeta.inlary:%'`

### Example
<a href="#example"></a>

Fixed-size IPv6 address (16 bytes inline):

```
dmmeta.ctype  ctype:net.Ipv6  comment:""
  dmmeta.field  field:net.Ipv6.octet  arg:u8  reftype:Inlary  dflt:""
    dmmeta.inlary  field:net.Ipv6.octet  min:16  max:16
```

Generates a `struct Ipv6 { u8 octet_elems[16]; ... };` with no
counter and no allocator — just the array.

Variable-size example (up to 8 hooks per record):

```
dmmeta.field  field:atf_amc.FAction.hooks  arg:atf_amc.FHook  reftype:Inlary
  dmmeta.inlary  field:atf_amc.FAction.hooks  min:0  max:8
```

In C++:

```c++
atf_amc::FAction act;
atf_amc::FHook *h = hooks_AllocMaybe(act); // returns NULL if 8 already
if (h) {
    new(h) atf_amc::FHook(...);
}
```

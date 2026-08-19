## amc Reftype: Ptrary
<a href="#amc-reftype-ptrary"></a>

`Ptrary` is a **dynamic array of pointers** used as a
cross-reference index.  Operations are amortized O(1):
`Insert` appends; `Remove` swaps the last pointer into the
freed slot.  Order is not preserved.  Ptrary backs both
[Bheap](/txt/exe/amc/reftype/Bheap.md) and many user-level
"list of children" relationships in amc.

Ptrary has three variants, picked by flags on
`dmmeta.ptrary`:

| `unique` | `heaplike` | Membership tracking | Allowed |
|---|---|---|---|
| N | N | None — duplicates allowed, `_InAryQ` not generated | yes (default; **cannot** be `cascdel`) |
| Y | N | `<xfname>_in_ary` bool on the element | yes |
| Y | Y | `<xfname>_idx` i32 on the element (-1 = not in) | yes (Bheap requires this) |
| N | Y | — | rejected: heaplike implies unique |

```
dmmeta.field  field:amc.FTarget.c_targdep  arg:amc.FTargdep  reftype:Ptrary  dflt:""  comment:""
  dmmeta.ptrary  field:amc.FTarget.c_targdep  unique:Y  heaplike:N
```

### What it generates
<a href="#what-it-generates"></a>

State in the **parent** ctype
(see `tclass_Ptrary` in `cpp/amc/ptrary.cpp`):

| Field          | Type   | Meaning                                |
|----------------|--------|----------------------------------------|
| `<name>_elems` | `T**`  | array of pointers; doubling growth     |
| `<name>_n`     | `u64`  | current count                          |
| `<name>_max`   | `u64`  | capacity in pointers                   |

State in the **element** ctype (only with `unique`/`heaplike`):

| Field              | Type    | `unique:Y` | `heaplike:Y` | Meaning |
|--------------------|---------|------------|--------------|---------|
| `<xfname>_in_ary`  | `bool`  | yes        | no           | True while the element is in the array. |
| `<xfname>_idx`     | `i32`   | no         | yes          | Current position in the array; -1 = not in array. |

Heaplike uses the index to support O(log N) Remove from the
backing array — that's why Bheap demands `heaplike:Y`.

Both variants are populated by amc's `gen_newfield_ptrary`
phase as part of code generation.

### Ssim inputs
<a href="#ssim-inputs"></a>

Required:

- `dmmeta.field` with `reftype:Ptrary`.  `arg:` is the element
  ctype.  The field name must start with `c_`
  ([field name prefixes](/txt/exe/amc/reftype.md#field-name-prefixes)).
- `dmmeta.ptrary`:
  - `unique:Y/N` — reject duplicate insertions.
  - `heaplike:Y/N` — track position via `<xfname>_idx`; implies
    `unique:Y`.
- `dmmeta.basepool` — Ptrary allocates the pointer array from
  here.

Optional:

- `dmmeta.xref` — auto-population.
- `dmmeta.cascdel` on the xref — *requires* `unique:Y`
  (`Ptrary with unique:N cannot be cascdel`).

For `unique:Y`, the element ctype must live in the **same
namespace** as the Ptrary's parent — amc adds a flag field to
the element type and refuses to modify a foreign namespace.

### Generated functions
<a href="#generated-functions"></a>

Source: `cpp/amc/ptrary.cpp`.

| Tfunc                  | Generated function                                 | Variant gating | Effect |
|------------------------|----------------------------------------------------|----------------|--------|
| `Ptrary.Init`          | `<name>_Init(P&)` (inline)                         | always         | Zero pointer, size, capacity. |
| `Ptrary.Reserve`       | `void <name>_Reserve(P&, u64 n)`                   | always         | Grow capacity to ≥ `_n+n`. |
| `Ptrary.N`             | `u64 <name>_N(P&)`                                 | always         | Count. |
| `Ptrary.EmptyQ`        | `bool <name>_EmptyQ(P&)`                           | always         | `_n == 0`. |
| `Ptrary.First`         | `T* <name>_First(P&)`                              | always         | First pointer (or NULL). |
| `Ptrary.Last`          | `T* <name>_Last(P&)`                               | always         | Last pointer (or NULL). |
| `Ptrary.qLast`         | `T& <name>_qLast(P&)`                              | always         | Unchecked. |
| `Ptrary.Find`          | `T* <name>_Find(P&, Rowid t)`                      | always         | Bounded find by index; NULL if `t >= _n`. |
| `Ptrary.qFind`         | `T& <name>_qFind(P&, Rowid t)`                     | always         | Unchecked. |
| `Ptrary.Getary`        | `aryptr<T*> <name>_Getary(P&)`                     | always         | Span over the pointer array. |
| `Ptrary.InAryQ`        | `bool <xfname>_InAryQ(T& row)`                     | `unique:Y`     | Membership predicate: reads `_in_ary` or `_idx`. |
| `Ptrary.Insert`        | `void <name>_Insert(P&, T& row)`                   | always         | Append unconditionally (caller's responsibility to avoid duplicates). |
| `Ptrary.InsertMaybe`   | `bool <name>_InsertMaybe(P&, T& row)`              | always         | Append if not present; sets the membership flag.  Returns false on duplicate (`unique:Y`). |
| `Ptrary.ScanInsertMaybe` | (private)                                        | as needed      | Helper used by InsertMaybe. |
| `Ptrary.Remove`        | `void <name>_Remove(P&, T& row)`                   | `unique:Y`     | Swap-with-last; clears the membership flag. |
| `Ptrary.RemoveFirst`   | `T* <name>_RemoveFirst(P&)`                        | always         | Pop and return head. |
| `Ptrary.RemoveLast`    | `void <name>_RemoveLast(P&)`                       | always         | Pop tail. |
| `Ptrary.RemoveAll`     | `void <name>_RemoveAll(P&)`                        | always         | Clear flags on each element; set `_n=0`. |
| `Ptrary.Cascdel`       | (private)                                          | `cascdel:Y`    | Delete every referenced row; requires `unique:Y`. |

Plus two cursors.  `Ptrary.curs` generates `<P>_<name>_curs` for every
Ptrary field and walks the array in order.  `Ptrary.oncecurs` generates
`<P>_<name>_oncecurs`, which *drains* the array: `_Reset` captures the
elements and sets the count to zero, and each `_Next` clears the walked
element's membership marker, so a work queue can be emptied and
refilled in one pass.  oncecurs is opt-in — add a `dmmeta.fcurs` row
naming the field and `oncecurs`, see [cursors](/txt/exe/amc/reftype.md#cursors).

### Memory model
<a href="#memory-model"></a>

- The pointer array is owned by Ptrary; growth doubles capacity
  via the base pool's `ReallocMem`.
- The *targets* of the pointers must live in stable memory —
  Lary or Tpool is the right choice.
- `Remove` swaps-with-last, so iteration order is not stable
  across removals.
- For `heaplike:Y`, each move updates the swapped element's
  `<xfname>_idx`.

### Pitfalls
<a href="#pitfalls"></a>

- **`unique:N` cannot be `cascdel`.**  Without uniqueness amc
  cannot delete each pointed-to row safely (the same row may
  appear multiple times).
- **`heaplike:Y` implies `unique:Y`.**  amc refuses otherwise.
- **Element namespace match.**  `unique:Y` requires the
  element ctype to share the namespace because amc adds a
  flag field to it.  Cross-namespace `unique:N` Ptrary works,
  but you lose `_InAryQ` and `_Remove`.
- **`Remove` does swap-with-last.**  Iteration order changes
  on remove — don't expect insertion order to survive.
- **Element's `_Init` must run.**  The flag/idx defaults must
  be set correctly before insertion; bypassing `_Init`
  corrupts membership.

### See also
<a href="#see-also"></a>

- [Reftypes index](/txt/exe/amc/reftype.md)
- [Bheap](/txt/exe/amc/reftype/Bheap.md) — depends on `heaplike:Y`
- [Thash](/txt/exe/amc/reftype/Thash.md) — for keyed lookup
- [Llist](/txt/exe/amc/reftype/Llist.md) — for insertion-ordered traversal
- [Cross-references](/txt/exe/amc/xref.md)
- Source: `cpp/amc/ptrary.cpp`; phase: `cpp/amc/gen.cpp` `gen_newfield_ptrary`
- Tfunc records: `acr 'tfunc:Ptrary.%'`
- Spec table: `acr 'dmmeta.ptrary:%'`

### Example
<a href="#example"></a>

A unique Ptrary of dependencies per target:

```
dmmeta.field  field:amc.FTarget.c_targdep  arg:amc.FTargdep  reftype:Ptrary
  dmmeta.ptrary  field:amc.FTarget.c_targdep  unique:Y  heaplike:N
  dmmeta.xref    field:amc.FTarget.c_targdep  ...
```

In C++:

```c++
if (c_targdep_InAryQ(td)) {            // already in?
    return;
}
c_targdep_Insert(*target, td);

ind_beg(amc::FTarget_c_targdep_curs, td, *target) {
    process(td);
} ind_end;
```

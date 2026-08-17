## amc Reftype: Atree
<a href="#amc-reftype-atree"></a>

`Atree` is an **ordered** cross-reference index backed by an
AVL tree (height-balanced binary search tree).  All operations
— insert, remove, lookup, iteration in order, range scans —
are O(log N).  Use Atree when you need any of the following:

- iteration in **sorted order** (use `First` / `Next`)
- **range queries** by key (use `FirstGe` / `LastLt`)
- **predecessor / successor** of a given row

If you only need keyed lookup, [Thash](/txt/exe/amc/reftype/Thash.md)
is faster.  If you only need a single min/max, use
[Bheap](/txt/exe/amc/reftype/Bheap.md).

```
dmmeta.field  field:atf_unit.FDb.tr_number  arg:atf_unit.FNumber  reftype:Atree  dflt:""  comment:""
  dmmeta.sortfld  field:atf_unit.FDb.tr_number  sortfld:atf_unit.FNumber.value  comment:""
```

### What it generates
<a href="#what-it-generates"></a>

State in the **parent** ctype (see `tclass_Atree` in
`cpp/amc/avl.cpp`):

| Field (parent)   | Type   | Meaning                            |
|------------------|--------|------------------------------------|
| `<name>_root`    | `T*`   | root of the AVL tree; NULL if empty |
| `<name>_n`       | `i32`  | number of elements in the tree     |

State in the **element** ctype (per indexed field; an element
can be in multiple Atrees at once):

| Field (element)    | Type   | Meaning                                          |
|--------------------|--------|--------------------------------------------------|
| `<xfname>_up`      | `T*`   | parent; `(T*)-1` means "not in tree"; NULL = root |
| `<xfname>_left`    | `T*`   | left child (smaller keys)                        |
| `<xfname>_right`   | `T*`   | right child (larger keys)                        |
| `<xfname>_depth`   | `i32`  | subtree height; used by AVL rebalance            |

The element's `Init` is augmented to set
`up = (T*)-1, left=NULL, right=NULL, depth=0`.

### How it works
<a href="#how-it-works"></a>

Standard AVL tree (Adelson-Velsky & Landis):

- Tree is a binary search tree ordered by the `sortfld`'s `<`.
  Duplicates are allowed (a row can be inserted multiple times
  only by sharing keys; identity is checked separately).
- After every insert and remove, amc walks up to the root via
  `<xfname>_up`, refreshing each node's depth and rotating
  whenever the balance factor exceeds ±1 (`Rebalance`,
  `Turn` — single and double rotations).
- The `_up` back-pointer makes traversal stateless: `Next(row)`
  finds the successor by going right-then-leftmost, or upward
  until it comes from a left child — no stack needed.

The comparator is generated as `<name>_ElemLt(a,b)`:

- If `dmmeta.fcmp` is defined on the sortfld, the comparator
  calls `<sortfld>_Lt(a, b)`.
- Otherwise, it does `a.<sortfld> < b.<sortfld>` directly.

This is the same convention as [Bheap](/txt/exe/amc/reftype/Bheap.md).

### Sort field
<a href="#sort-field"></a>

Required.  Atree refuses to generate without a `dmmeta.sortfld`:

```
dmmeta.sortfld  field:atf_unit.FDb.tr_number  sortfld:atf_unit.FNumber.value
```

The sortfld must be a field of the element ctype (or of a base
type the element inherits).  Use `dmmeta.fcmp` for multi-field
or custom ordering.

### Ssim inputs
<a href="#ssim-inputs"></a>

Required:

- `dmmeta.field` with `reftype:Atree`.  `arg:` is the element
  ctype.  The field name must start with `tr_`
  ([field name prefixes](/txt/exe/amc/reftype.md#field-name-prefixes)).
- `dmmeta.sortfld` — which element field provides the key.

Optional:

- `dmmeta.xref` to populate the tree automatically on row
  insertion/removal.  Without an xref the tree is manual:
  call `Insert` / `Remove` yourself.
- `dmmeta.fcmp` on the sortfld for custom ordering.

Atree allocates nothing — every link lives inside element rows.

### Generated functions
<a href="#generated-functions"></a>

Source: `cpp/amc/avl.cpp`.

#### Lifecycle
<a href="#lifecycle"></a>

| Tfunc            | Generated function                          | Effect |
|------------------|---------------------------------------------|--------|
| `Atree.Init`     | `<name>_Init(P&)` (macro)                   | Set `_root=NULL`, `_n=0`.  No allocation. |
| `Atree.RemoveAll`| `void <name>_RemoveAll(P&)`                 | Walk every element, clear its `up`/`left`/`right`, mark not-in-tree; reset `_root` and `_n`.  Rows themselves are not deleted. |
| `Atree.Cascdel`  | (private)                                   | Pop and `Delete` every element. Emitted when xref has `cascdel:Y`. |

#### Membership & access
<a href="#membership-access"></a>

| Tfunc            | Generated function                          | Effect |
|------------------|---------------------------------------------|--------|
| `Atree.EmptyQ`   | `bool <name>_EmptyQ(P&)`                    | `_root == NULL`. |
| `Atree.InTreeQ`  | `bool <xfname>_InTreeQ(T& row)`             | `row.<xfname>_up != (T*)-1`. |
| `Atree.First`    | `T* <name>_First(P&)`                       | Smallest element; NULL if empty. |
| `Atree.Last`     | `T* <name>_Last(P&)`                        | Largest element; NULL if empty. |
| `Atree.Next`     | `T* <xfname>_Next(T& row)`                  | In-order successor (or NULL). |
| `Atree.Prev`     | `T* <xfname>_Prev(T& row)`                  | In-order predecessor (or NULL). |
| `Atree.FirstGe`  | `T* <name>_FirstGe(P&, const K& key)`       | Smallest element with `sortfld ≥ key`; NULL if none.  Useful for ranged scans. |
| `Atree.LastLt`   | `T* <name>_LastLt(P&, const K& key)`        | Largest element with `sortfld ≤ key` (note: the comment says "smaller or equal"). |

#### Mutation
<a href="#mutation"></a>

| Tfunc              | Generated function                          | Effect |
|--------------------|---------------------------------------------|--------|
| `Atree.Insert`     | `void <name>_Insert(P&, T& row)`            | Insert `row` at the position dictated by its current sortfld.  No-op if already in this tree.  O(log N). |
| `Atree.InsertImpl` | (private)                                   | The body of Insert — descends to a leaf, sets `_up`/`_left`/`_right`, then propagates depth + rebalance. |
| `Atree.Remove`     | `void <name>_Remove(P&, T& row)`            | Remove `row`; rebalances the parent chain.  No-op if not in tree. |
| `Atree.RemoveFirst`| `T* <name>_RemoveFirst(P&)`                 | Unlink and return the smallest element. |
| `Atree.Reinsert`   | `void <name>_Reinsert(P&, T& row)`          | When the sortfld of `row` has changed in place: Remove then Insert (semantics of `Reheap` in [Bheap](/txt/exe/amc/reftype/Bheap.md)). |

#### Internal helpers (private)
<a href="#internal-helpers-private-"></a>

These are emitted but marked private — they implement the AVL
machinery.  You shouldn't call them directly.

| Tfunc                | Effect |
|----------------------|--------|
| `Atree.ElemLt`       | Comparator (`a < b` by sortfld). |
| `Atree.FirstImpl` / `LastImpl` | Walk leftmost / rightmost of a subtree. |
| `Atree.Connect` / `Disconnect` | Wire a child into / out of a parent. |
| `Atree.Turn`         | Rotate (left or right). |
| `Atree.UpdateDepth`  | Recompute a node's `depth` from its children. |
| `Atree.TallerChild`  | Return the deeper child (NULL if equal). |
| `Atree.Balance`      | Balance factor of a node. |
| `Atree.Rebalance`    | Apply rotations if balance > ±1. |
| `Atree.Propagate`    | After insert/remove, walk up rebalancing every ancestor. |
| `Atree.RemoveAllImpl`| Recursive helper for `RemoveAll`. |

#### Iteration
<a href="#iteration"></a>

| Tfunc          | Generated function                                  | Effect |
|----------------|-----------------------------------------------------|--------|
| `Atree.curs`   | `<P>_<name>_curs` + `_Reset/_ValidQ/_Next/_Access`  | In-order forward cursor; uses `First` + `Next` internally, so no auxiliary stack. |

### Memory model
<a href="#memory-model"></a>

- **No allocation.**  The tree is entirely link-based; all
  storage is in the rows.
- Rows must live in stable memory ([Lary](/txt/exe/amc/reftype/Lary.md)
  or similar) — moving a row would orphan its `up`/`left`/
  `right` neighbors.
- A row can be in multiple Atrees at once, one set of links
  per Atree (driven by `xfname`).
- Removal does not free anything; `RemoveAll` just unlinks.

### Pitfalls
<a href="#pitfalls"></a>

- **Stable element storage required.**  Atree's links are
  pointers — rows must not move.  Use Lary (or another
  permanent-pointer pool) for rows that participate in an
  Atree.
- **`sortfld` writes need `Reinsert`.**  Atree, unlike Bheap,
  does not generate a `<sortfld>_Set` helper.  If you change
  the key, you must call `<name>_Reinsert(row)` yourself.
- **Duplicates are allowed.**  Atree does not enforce
  uniqueness on the sortfld — duplicate keys insert next to
  each other.  Use a `dmmeta.fcmp` that includes a tiebreaker
  if you need a strict total order.
- **Element's `_Init` must run** (sets `up=(T*)-1`).  Bypassing
  it leaves `up=NULL`, which `InTreeQ` interprets as "in tree
  at root" — corrupting both the tree and your sanity.
- **No `Maybe`-flavored Insert.**  Atree allocates nothing, so
  insertion cannot fail.  Logical conflicts (duplicate keys
  under a stricter ordering policy) must be detected by the
  user before calling `Insert`.

### See also
<a href="#see-also"></a>

- [Reftypes index](/txt/exe/amc/reftype.md)
- [Bheap](/txt/exe/amc/reftype/Bheap.md) — when only min/max is needed
- [Thash](/txt/exe/amc/reftype/Thash.md) — keyed lookup without ordering
- [Llist](/txt/exe/amc/reftype/Llist.md) — insertion-ordered traversal
- [Cross-references](/txt/exe/amc/xref.md) — auto-insertion model
- Source: `cpp/amc/avl.cpp` (the file is named `avl.cpp` but the reftype is `Atree`)
- Tfunc records: `acr 'tfunc:Atree.%'`

### Example
<a href="#example"></a>

`atf_unit.FNumber` is sorted numerically in an Atree to detect
duplicates and iterate in order:

```c++
atf_unit::FNumber *n = number_Alloc();
n->value = 42;
tr_number_Insert(*n);             // O(log N)

// in-order iteration:
ind_beg(atf_unit::_db_tr_number_curs, row, atf_unit::_db) {
    prlog(row.value);             // ascending
} ind_end;

// range scan [10, 20):
for (atf_unit::FNumber *r = tr_number_FirstGe(10);
     r && r->value < 20;
     r = value_Next(*r)) {
    // ...
}

// pop smallest:
while (atf_unit::FNumber *r = tr_number_RemoveFirst()) {
    process(*r);
}
```

If the key changes after insertion, call `Reinsert` so the tree
order stays consistent:

```c++
n->value = 100;
tr_number_Reinsert(*n);           // Remove + Insert under the hood
```

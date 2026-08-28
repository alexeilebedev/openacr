## amc Reftype: Bheap
<a href="#amc-reftype-bheap"></a>

`Bheap` is a **binary heap** cross-reference — a priority queue
of pointers ordered by a designated `sortfld`.  `First()` is
O(1); `Insert` / `Remove` / `Reheap` are O(log N).  The binding
to a specific sort field is what makes it a heap rather than a
plain array of pointers: amc generates the comparator from
`sortfld_Lt` (or `<` on the field value) and re-sifts
automatically whenever an element's key changes.

```ssim
dmmeta.field  field:abt.FDb.bh_syscmd  arg:abt.FSyscmd  reftype:Bheap  dflt:""  comment:""
  dmmeta.sortfld  field:abt.FDb.bh_syscmd  sortfld:abt.FSyscmd.starttime  comment:""
```

The heap is a **min-heap by default**: the smallest sortfld
value is at index 0.  For a max-heap, use a sortfld whose `<`
is reversed (e.g. via a `dmmeta.fcmp`).

### What it generates
<a href="#what-it-generates"></a>

State in the **parent** ctype (see `tclass_Bheap` in
`cpp/amc/bheap.cpp`):

| Field (parent)       | Type   | Meaning                                                |
|----------------------|--------|--------------------------------------------------------|
| `<name>_elems`       | `T**`  | array of pointers ordered as a binary heap by `sortfld` |
| `<name>_n`           | `i32`  | number of elements in the heap                          |
| `<name>_max`         | `i32`  | capacity in pointers before realloc                     |

State in the **element** ctype:

| Field (element)   | Type | Meaning                                              |
|-------------------|------|------------------------------------------------------|
| `<xfname>_idx`    | `i32`| current position in the heap array; -1 = not in heap |

The element's `Init` is augmented to set `idx=-1`.  The back-
pointer makes `Remove` and `Reheap` O(log N) — you locate the
element directly without scanning the heap.

### How it works
<a href="#how-it-works"></a>

A standard array-backed binary heap:

- Parent of index `i` is `(i-1)/2`; children are `2i+1` and
  `2i+2`.
- **Insert** appends at the end, then sifts up
  (`<name>_Upheap`) until the heap property is restored.
- **RemoveFirst** swaps the last element into slot 0, then
  sifts down (`<name>_Downheap`).
- **Remove(row)** finds the row in O(1) via `<xfname>_idx`,
  swaps it with the last element, then either sifts up or
  down depending on which direction restores the heap.
- **Reheap(row)** is the workhorse: if `row` is not in the
  heap, append + sift up; if it is, try sifting up first, then
  down — exactly one direction will move it.  Used by
  `sortfld_Set` to keep the heap consistent when a key
  changes.

The comparator is generated as `<name>_ElemLt(a,b)` — calls
`sortfld_Lt(a,b)` if a custom `dmmeta.fcmp` exists, otherwise
falls back to `a.sortfld < b.sortfld`.

### Sort field
<a href="#sort-field"></a>

Required.  Bheap ICE's at code-gen time if the field has no
`dmmeta.sortfld`:

```ssim
dmmeta.sortfld  field:abt.FDb.bh_syscmd  sortfld:abt.FSyscmd.starttime
```

The sortfld must be a field of the element ctype (or of a base
type that the element inherits).  Multi-field comparison is
done via `dmmeta.fcmp` on the sortfld.

### Ssim inputs
<a href="#ssim-inputs"></a>

Required:

- `dmmeta.field` with `reftype:Bheap`.  `arg:` is the element
  ctype.  The field name must start with `bh_`
  ([field name prefixes](/txt/exe/amc/reftype.md#field-name-prefixes)).
- `dmmeta.sortfld` — which element field provides the key.
- `dmmeta.basepool` (or a namespace default) — Bheap allocates
  the pointer array from this pool.

Optional:

- `dmmeta.xref` to populate the heap automatically when rows are
  inserted (and clear when removed).
- `dmmeta.fcmp` on the sortfld for custom ordering (descending,
  versionsort, case-insensitive, multi-field).
- `dmmeta.fstep` on the field if you want a step function to
  fire whenever the heap top changes (`FirstChanged` hook).

### Generated functions
<a href="#generated-functions"></a>

Source: `cpp/amc/bheap.cpp`.

| Tfunc                  | Generated function                                       | Effect |
|------------------------|----------------------------------------------------------|--------|
| `Bheap.Init`           | `<name>_Init(P&)` (macro)                                | Zero `_elems`/`_n`/`_max`.  No initial allocation. |
| `Bheap.Uninit`         | `<name>_Uninit(P&)` (macro)                              | Free the pointer array (no-op for global FDb). |
| `Bheap.N`              | `i32 <name>_N(const P&)`                                 | Count. |
| `Bheap.EmptyQ`         | `bool <name>_EmptyQ(P&)`                                 | `_n == 0`. |
| `Bheap.First`          | `T* <name>_First(P&)`                                    | Pointer to top (min) element; NULL if empty. |
| `Bheap.RemoveFirst`    | `T* <name>_RemoveFirst(P&)`                              | Pop the top, sift down; return removed pointer. |
| `Bheap.Insert`         | `void <name>_Insert(P&, T& row)`                         | Add `row`; no-op if already in heap. |
| `Bheap.Remove`         | `void <name>_Remove(P&, T& row)`                         | Remove `row` via its back-pointer; no-op if not in heap. |
| `Bheap.RemoveAll`      | `void <name>_RemoveAll(P&)`                              | Mark every element `idx=-1`, set `_n=0`.  Keeps the array. |
| `Bheap.InBheapQ`       | `bool <xfname>_InBheapQ(T& row)`                         | `row.<xfname>_idx != -1`. |
| `Bheap.Reheap`         | `i32 <name>_Reheap(P&, T& row)`                          | Insert or re-sift `row` to its correct position; returns new index. |
| `Bheap.ReheapFirst`    | `i32 <name>_ReheapFirst(P&)`                             | Re-sift the top element after its key changed in place; UB if empty. |
| `Bheap.Set`            | `void <sortfld>_Set(P&, T& row, K new_key)`              | Write `row.<sortfld>=new_key`, then `Reheap` (or `Remove` if the xref's `inscond` is now false). |
| `Bheap.SetIfBetter`    | `void <sortfld>_SetIfBetter(P&, T& row, K new_key)`      | Same as Set, but only writes when `new_key` is strictly better than the current key. |
| `Bheap.Cascdel`        | (private)                                                | Pop from the back and `Delete` each row.  Emitted when xref has `cascdel:Y`. |
| `Bheap.Reserve`        | `void <name>_Reserve(P&, i32 n)`                         | Grow the pointer array so that `n` *more* elements would fit; doubling growth. |
| `Bheap.Compact`        | `void <name>_Compact(P&)`                                | Shrink the pointer array down to `_n`, returning unused memory to the base pool. |
| `Bheap.Dealloc`        | (private)                                                | Free the pointer array unconditionally. |
| `Bheap.Upheap`         | (private) `<name>_Upheap(P&, T&, i32)`                   | Sift-up helper. |
| `Bheap.Downheap`       | (private) `<name>_Downheap(P&, T&, i32)`                 | Sift-down helper. |
| `Bheap.ElemLt`         | (private) `<name>_ElemLt(P&, T&, T&)`                    | Comparator: `a < b` by sortfld. |
| `Bheap.ElemLtval`      | (private) `<name>_ElemLtval(P&, T&, const K&)`           | Compare row's key against a raw value (used by SetIfBetter). |
| `Bheap.curs`           | `<P>_<name>_curs` + `_Reset/_ValidQ/_Next/_Access`       | Cursor that pops the heap in sorted order — destructive! |
| `Bheap.unordcurs`      | `<P>_<name>_unordcurs` + accessors *(opt-in)*            | Cursor that iterates the underlying array in arbitrary order — does **not** modify the heap.  Request it with a `dmmeta.fcurs` row, see [cursors](/txt/exe/amc/reftype.md#cursors). |

### Memory model
<a href="#memory-model"></a>

- Single pointer array, doubling growth.
- The heap stores **pointers**, so an Insert/Remove never moves
  rows.  Rows must live in stable storage (e.g.,
  [Lary](/txt/exe/amc/reftype/Lary.md)) — the heap's
  `<xfname>_idx` back-pointer would be invalidated otherwise.
- `RemoveAll` keeps the pointer array.  `Compact` shrinks it.
  `Uninit` frees it (skipped for global FDb).
- Allocation failure during `Reserve` is fatal — `Bheap` has
  no `Maybe`-flavored insert.

### Pitfalls
<a href="#pitfalls"></a>

- **Stable element storage required.**  The heap stores `T*`
  back to rows; if those rows ever move (Tary grow, stack-
  allocated rows, etc.), the heap becomes corrupt.
- **`sortfld` writes must go through `<sortfld>_Set`**.  Direct
  writes to the field do not reheap the row, leaving it in the
  wrong slot.  Use `<sortfld>_Set` (or `<sortfld>_SetIfBetter`).
- **`ReheapFirst` requires non-empty heap.**  Calling it on an
  empty heap is undefined behavior.
- **`curs` is destructive.**  The forward cursor pops elements
  off the heap to yield them in order.  Use `unordcurs` if you
  want to iterate without modifying.
- **No `Maybe`-flavored Insert.**  OOM on the pointer array is
  fatal — `Reserve` calls `FatalErrorExit`.
- **Element's `_Init` must run** (sets `idx=-1`).  Bypassing
  it leaves `idx=0`, which `InBheapQ` would interpret as "in
  heap at the top" — disaster.

### See also
<a href="#see-also"></a>

- [Reftypes index](/txt/exe/amc/reftype.md)
- [Atree](/txt/exe/amc/reftype/Atree.md) — ordered index when you need range queries, not just min/max
- [Thash](/txt/exe/amc/reftype/Thash.md) — for key lookup without ordering
- [Llist](/txt/exe/amc/reftype/Llist.md) — for FIFO/LIFO without ordering
- [Runtime](/txt/exe/amc/runtime.md) — for `TimeHook` Bheaps wired to `fstep`
- Source: `cpp/amc/bheap.cpp`
- Tfunc records: `acr 'tfunc:Bheap.%'`

### Example
<a href="#example"></a>

`abt` keeps a queue of subprocesses to wait on, ordered by
expected end-time:

```c++
abt::FSyscmd &cmd = syscmd_Alloc();
cmd.starttime = algo::CurrUnTime();
bh_syscmd_Insert(cmd);    // O(log N)

// re-prioritize when state changes:
starttime_Set(cmd, new_time); // writes field + Reheap

// pop the earliest:
abt::FSyscmd *next = bh_syscmd_RemoveFirst();
```

Iteration without mutating the heap (for printing, etc.) uses
the **unordered** cursor:

```c++
ind_beg(abt::_db_bh_syscmd_unordcurs, cmd, abt::_db) {
    prlog(cmd.command);
} ind_end;
```

The default `curs` pops elements one at a time — useful when
you really want to drain the heap in order.

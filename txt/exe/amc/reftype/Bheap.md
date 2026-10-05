## amc Reftype: Bheap
<a href="#amc-reftype-bheap"></a>

`Bheap` is a **binary heap** cross-reference — a priority queue
of pointers ordered by a designated `sortfld`, optionally with a
[sorted head](#a-sorted-head).  `First()` is
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
| `<name>_n`           | `i32`  | number of elements in the heap; with a head, `_nrun` and `_nheap` |
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

### A sorted head
<a href="#a-sorted-head"></a>

Take an order book kept as a heap of price levels.  Nearly all of its traffic is at the
best few levels: the best level fills and goes away, a new level appears one tick away, a
level two ticks back is canceled.  A plain binary heap pays a full sift for each of these.
A pop moves the last row into the root and walks it down, and a cancel near the top does
the same from the canceled slot.  A market-data snapshot that walks the best ten levels
builds a helper heap and sorts them again every time.

The heap order only needs the head to be cheap.  So a Bheap can keep its smallest rows as
a sorted run, and the run's largest row is the root of an ordinary heap below it.  A
`dmmeta.bheap` row sets the run's capacity:

```ssim
dmmeta.bheap  field:atf_amc.FDb.bh_heap_elem_head  nhead:32  comment:""
```

Without the row, `nhead` is 1: the run is the root alone, and amc generates the plain heap
described above, with the parent fields `_elems`, `_n` and `_max`.  With `nhead` of 2 or
more, the parent holds `_elems`, `_nrun` (rows in the run), `_nheap` (rows below the root)
and `_max`.  The run is right-aligned in slots 0 to nhead-1, so it ends at the root in
slot R = nhead-1, and the root's heap gives slot J the children 2J-R+1 and 2J-R+2.  Every
run row is at most the root, and the root is at most every heap row.

While the run holds two or more rows, a pop takes its first row and moves nothing else.
An insert or a cancel among the best rows shifts a few pointers inside the run.  Inserting
into a full run demotes the root into the heap, and removing the root while the run holds
other rows lets the next largest run row take its place.  Only rows that enter or leave
through the root pay a full sift.

The run refills only through inserts below the root and through `fillcurs` walks.  Once pops
have drained it down to the root, each pop sifts the last heap row into the root slot, as
a heap without a head does, so a queue that drains more than it inserts near its front
gets plain heap pops.

The sorted cursor walks the run in place, so a walk of the best rows reads them in order
with no allocation.  Past the root it continues through a helper heap, as a heap without
a head does, and it never writes to the Bheap.

Once pops have drained the run, every walk past the root builds that helper heap.  Ask
for `fillcurs` with a `dmmeta.fcurs` row to refill the run as you walk:

```ssim
dmmeta.fcurs  fcurs:atf_amc.FDb.bh_heap_elem_head/fillcurs  comment:""
```

When `fillcurs` reaches the root while the run has room, it promotes the root's smaller
child into the run, so a walk of K rows leaves up to K rows sorted in the run for the
pops and walks that follow.  On a 16-row heap whose run the pops keep draining, a walk of
the best 10 rows costs about 75 ns with `fillcurs` and about 225 ns with `curs`.  A
`fillcurs` walk changes the layout, so nothing else may walk the Bheap while one is
active.  amc refuses `fillcurs` on a Bheap without a head.

`atf_amc -amctest:PerfBheapHead` runs one scenario against a Bheap, a Bheap with a head of
32 rows, and an Atree.  With four million rows, on an AMD EPYC 7702P:

|Workload|nhead 1|nhead 32|
|---|---|---|
|book: 16 levels below all others, churned by pops and cancels|~125 ns|~45 ns|
|depth: the book workload plus a walk of the best 10 rows|~275 ns|~75 ns|
|pop the first row, push it back with a later key|~1.1 µs|~1.1 µs|
|change a random row's key, Reheap|~290 ns|~290 ns|

A head helps when the traffic concentrates on a band of the best rows that fits in it, as
in an order book or a deadline queue whose near deadlines keep moving, so pick an `nhead`
larger than that band.  On other traffic a Bheap with a head costs about what one without costs.

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
- `dmmeta.bheap` to give the heap a [sorted head](#a-sorted-head)
  of `nhead` rows.

### Generated functions
<a href="#generated-functions"></a>

Source: `cpp/amc/bheap.cpp`.

| Tfunc                  | Generated function                                       | Effect |
|------------------------|----------------------------------------------------------|--------|
| `Bheap.Init`           | `<name>_Init(P&)` (macro)                                | Zero `_elems`, the counts and `_max`.  No initial allocation. |
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
| `Bheap.Compact`        | `void <name>_Compact(P&)`                                | Halve the pointer array while the rows fill less than a quarter of it, returning memory to the base pool. |
| `Bheap.Dealloc`        | (private)                                                | Free the pointer array unconditionally. |
| `Bheap.Upheap`         | (private) `<name>_Upheap(P&, T&, i32)`                   | Sift-up helper. |
| `Bheap.Downheap`       | (private) `<name>_Downheap(P&, T&, i32)`                 | Sift-down helper. |
| `Bheap.ElemLt`         | (private) `<name>_ElemLt(P&, T&, T&)`                    | Comparator: `a < b` by sortfld. |
| `Bheap.ElemLtval`      | (private) `<name>_ElemLtval(P&, T&, const K&)`           | Compare row's key against a raw value (used by SetIfBetter). |
| `Bheap.curs`           | `<P>_<name>_curs` + `_Reset/_ValidQ/_Next/_Access`       | Cursor that returns the heap in sorted order through a helper heap; the heap is unchanged.  With a head, it walks the run in place first. |
| `Bheap.unordcurs`      | `<P>_<name>_unordcurs` + accessors *(opt-in)*            | Cursor that iterates the underlying array in arbitrary order — does **not** modify the heap.  Request it with a `dmmeta.fcurs` row, see [cursors](/txt/exe/amc/reftype.md#cursors). |
| `Bheap.fillcurs`       | `<P>_<name>_fillcurs` + accessors *(opt-in, head only)*  | Sorted cursor that promotes heap rows into the run as it walks; nothing else may walk the heap meanwhile. |
| `Bheap.RunInsert`, `RunRemove`, `RemoveRoot`, `Demote`, `InsertImpl`, `RemoveImpl` | (private) | The run's helpers; generated only with a head.  `Promote` exists only beside a `fillcurs`. |

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
- **`curs` allocates.**  The sorted cursor keeps a helper heap
  that grows with the walk's frontier.  Use `unordcurs` when order
  does not matter.
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

Iteration in arbitrary order (for printing, etc.) uses the
**unordered** cursor, which allocates nothing:

```c++
ind_beg(abt::_db_bh_syscmd_unordcurs, cmd, abt::_db) {
    prlog(cmd.command);
} ind_end;
```

The default `curs` returns the rows in key order and leaves the
heap as it was.  To drain the heap, loop on `RemoveFirst`.

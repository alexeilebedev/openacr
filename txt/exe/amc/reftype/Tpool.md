## amc Reftype: Tpool
<a href="#amc-reftype-tpool"></a>

`Tpool` is a **fixed-size freelist allocator**.  All
allocations share the same size — the element type — and freed
elements are threaded onto a singly-linked freelist for instant
reuse.  When the freelist is empty, Tpool asks its base pool
for a fresh block (`blocksize` bytes, default ~64 elements
worth) and chops it into individual elements.

Tpool is the fastest amc pool: `Alloc` is "pop head of free
list", `Delete` is "push head of free list" — two pointer
operations apiece.  It is the right choice when many rows of
the same type are allocated and individually deleted (sessions,
connections, request contexts, …).

```ssim
dmmeta.field  field:ssim2mysql.FDb.cmd  arg:ssim2mysql.FCmd  reftype:Tpool  dflt:""  comment:""
  dmmeta.basepool  field:ssim2mysql.FDb.cmd  base:algo_lib.FDb.lpool
```

### What it generates
<a href="#what-it-generates"></a>

State in the **parent** ctype
(see `tclass_Tpool` in `cpp/amc/tpool.cpp`):

| Field                  | Type         | Meaning                                       |
|------------------------|--------------|-----------------------------------------------|
| `<name>_blocksize`     | `u64`        | bytes per refill request (default: power-of-2 ≥ 64 × sizeof T) |
| `<name>_free`          | `T*`         | freelist head; NULL when empty                 |

State in the **element** ctype:

| Field           | Type | Meaning                                                |
|-----------------|------|--------------------------------------------------------|
| `<name>_next`   | `T*` | next free element; `(T*)-1` while in-use (sentinel)    |

The element's `_Init` sets `_next = (T*)-1` so that double-free
can be detected by `_FreeMem`.

Tpool pulls in the `Pool` tclass, so the field gets all
`Pool.*` tfuncs (Alloc, AllocMaybe, InsertMaybe, Delete, …)
on top of the primitives below.

### Constraints
<a href="#constraints"></a>

**Must be on FDb.**  amc rejects non-global Tpools because the
pool never returns memory to its base pool — a non-global
instance would silently leak on destruction.

### Ssim inputs
<a href="#ssim-inputs"></a>

Required:

- `dmmeta.field` with `reftype:Tpool`.  `arg:` is the element
  type.
- `dmmeta.basepool` — must point to the underlying allocator
  (usually `<ns>.FDb.lpool` or `<ns>.FDb.sbrk`).

No spec table.

### Generated functions
<a href="#generated-functions"></a>

Source: `cpp/amc/tpool.cpp`.

| Tfunc                  | Generated function                                  | Effect |
|------------------------|-----------------------------------------------------|--------|
| `Tpool.Init`           | `<name>_Init(P&)` (folded)                          | Empty freelist; set `_blocksize` to a sensible power of 2. |
| `Tpool.Reserve`        | `u64 <name>_Reserve(P&, u64 n)`                     | Preallocate at least `n` elements onto the freelist. |
| `Tpool.ReserveMem`     | `u64 <name>_ReserveMem(P&, u64 size)`               | Ask base pool for `size` bytes, slice into elements. |
| `Tpool.AllocMem`       | `void* <name>_AllocMem(P&)`                         | Pop the freelist (refilling if empty); NULL on OOM. |
| `Tpool.FreeMem`        | `void <name>_FreeMem(P&, T& row)`                   | Push `row` onto the freelist.  Aborts on double-free (via the `_next == (T*)-1` sentinel). |

Plus the full `Pool.*` family.

### Memory model
<a href="#memory-model"></a>

- Fixed element size — element type drives `sizeof`.  No padding,
  no wasted space.
- Refill from the base pool happens in `blocksize` chunks (rounded
  up to a power of 2 ≥ `64 × sizeof(T)`).
- `_FreeMem` does **not** return the block to the base pool.
  Memory grows monotonically (in `blocksize` increments) until
  process exit.
- Double-free is fatal: `_next` is set to `(T*)-1` at Init and
  remains a sentinel until the element is freed.  Re-freeing
  triggers `FatalErrorExit("tpool_double_delete")`.

### Pitfalls
<a href="#pitfalls"></a>

- **FDb only.**  Local Tpools are rejected.
- **No return to OS.**  Memory grows; `_FreeMem` keeps elements
  in the freelist.  Fine for steady-state workloads.
- **Double-free detection.**  Aborts the process — there is no
  recovery.  Don't manually `_FreeMem(*row)` then keep using
  `row`.
- **`_blocksize` is a runtime tunable.**  Bump it for very small
  or very large element types if you want a different refill
  granularity.
- **`Pool.Delete` calls `_FreeMem` after running the element's
  destructor.**  Don't use `_FreeMem` directly unless you have
  already destroyed the row.

### See also
<a href="#see-also"></a>

- [Reftypes index](/txt/exe/amc/reftype.md)
- [Lpool](/txt/exe/amc/reftype/Lpool.md) — variable-size sibling
- [Lary](/txt/exe/amc/reftype/Lary.md) — when permanent pointers matter
- [Memory pools](/txt/exe/amc/pool.md)
- Source: `cpp/amc/tpool.cpp`
- Tfunc records: `acr 'tfunc:Tpool.%'`

### Example
<a href="#example"></a>

A pool of command structs:

```c++
ssim2mysql::FCmd *cmd = cmd_Alloc();   // pop freelist (or refill)
cmd->name = "compile";
...
cmd_Delete(*cmd);                       // push to freelist
```

Pre-allocating to avoid mid-run base-pool calls:

```c++
cmd_Reserve(_db, 1024);                 // get ~1024 free elements up front
```

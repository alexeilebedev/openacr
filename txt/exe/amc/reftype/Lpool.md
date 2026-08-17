## amc Reftype: Lpool
<a href="#amc-reftype-lpool"></a>

`Lpool` is a **variable-size pool** implemented as 36 freelists
indexed by power-of-2 size class.  Allocations are rounded up
to the next power of two (minimum 16 bytes), and freed memory
is pushed back onto the matching freelist for reuse.  When a
freelist is empty, Lpool requests a large block from its base
pool and splits it into halves until it reaches the target
size — buddy-style allocation.

```
dmmeta.field  field:algo_lib.FDb.lpool  arg:u8  reftype:Lpool  dflt:""  comment:"private memory pool"
  dmmeta.basepool  field:algo_lib.FDb.lpool  base:algo_lib.FDb.sbrk
```

Lpool is the **default general-purpose allocator** for most
amc namespaces.  Its `arg` is conventionally `u8` because Lpool
is used for raw varlen byte blocks; the size classes range
from 16 B (2^4) to 1 TB (2^40).

### What it generates
<a href="#what-it-generates"></a>

State in the **parent** ctype
(see `tclass_Lpool` in `cpp/amc/lpool.cpp`):

| Field                | Type             | Always | Meaning                                |
|----------------------|------------------|--------|----------------------------------------|
| `<name>_free[36]`    | `<name>_Lpblock*[36]` | yes | freelist heads, one per size class |
| `<name>_n`           | `i64`            | when `arg != u8` | live allocation count          |

A `struct <name>_Lpblock { <name>_Lpblock* next; };` is also
emitted in the namespace header for the freelist nodes.

The 36 levels start at `1 << 4 = 16` bytes (level 0) and double
up to `1 << 40 = 1 TB` (level 35) — see `$minlevels:4`,
`$nlevels:36` in `tclass_Lpool`.

### How it works
<a href="#how-it-works"></a>

- **AllocMem(size)**:
  1. Round `size` up to at least 16 bytes.
  2. Compute cell `bsr(size-1) + 1 - 4` — the smallest power-
     of-2 class that fits.
  3. Pop from `_free[cell]`.  If empty, walk upward looking
     for any larger free block — pop, then repeatedly split
     the upper half off into smaller freelists until the
     block matches the requested cell.
  4. If still empty, ask the base pool for a big block (at
     least 2 MiB, so huge pages can be used) and split it
     down.  Return NULL if the base pool also failed.

- **FreeMem(mem, size)**: round size up, push the block onto
  `_free[cell]`.  No coalescing — adjacent free blocks stay
  separate.

- **ReallocMem(oldmem, old_size, new_size)**: allocate, copy,
  free.

Lpool also pulls in the `Pool` tclass, so a field gets all the
`Pool.*` tfuncs (Alloc, AllocMaybe, Delete, AllocVarlen, …)
on top of its primitives.

### Ssim inputs
<a href="#ssim-inputs"></a>

Required:

- `dmmeta.field` with `reftype:Lpool`.  `arg:` is conventionally
  `u8` for raw bytes; with a typed `arg`, Lpool keeps a count.
- `dmmeta.basepool` — Lpool needs a larger pool to fall back
  to (typically `sbrk`).

There is no `dmmeta.lpool` record.

### Generated functions
<a href="#generated-functions"></a>

Source: `cpp/amc/lpool.cpp`.

| Tfunc                  | Generated function                                                      | Effect |
|------------------------|-------------------------------------------------------------------------|--------|
| `Lpool.Init`           | `<name>_Init(P&)` (folded)                                              | NULL each freelist head, zero `_n`. |
| `Lpool.N`              | `i64 <name>_N(P&)`                                                      | Live alloc count (only when `arg != u8`). |
| `Lpool.AllocMem`       | `void* <name>_AllocMem(P&, u64 size)`                                   | Variable-size allocation; rounds up to power of 2. |
| `Lpool.FreeMem`        | `void <name>_FreeMem(P&, void* mem, u64 size)`                          | Push back onto the matching freelist. |
| `Lpool.ReallocMem`     | `void* <name>_ReallocMem(P&, void* old, u64 old_size, u64 new_size)`    | AllocMem + memcpy + FreeMem.  Returns NULL on OOM, leaving `old` intact. |
| `Lpool.ReserveBuffers` | `bool <name>_ReserveBuffers(P&, u64 nbuf, u64 bufsize)`                 | Pull `nbuf` buffers of `bufsize` from the base pool up front. |

Plus the full `Pool.*` family (Alloc, AllocMaybe, AllocVarlen,
Delete, …).

### Memory model
<a href="#memory-model"></a>

- Allocations are power-of-2 sizes from 16 B to 1 TB.
- Freed memory **stays in the pool** — never returned to the
  base pool or OS.
- No coalescing: a 32 B block freed back to the pool is
  available for 32 B requests only.  Lpool is good for
  workloads where size distribution is roughly stable;
  pathological cases with very few small allocations and many
  large frees can fragment.
- First-time growth allocates at least 2 MiB from the base
  pool so the kernel can hand back huge pages.

### Pitfalls
<a href="#pitfalls"></a>

- **Rounds up to power of two.**  A 1025-byte request uses a
  2048-byte block.  Memory usage can be up to ~2x the bytes
  you actually need.
- **No coalescing.**  Mixed-size workloads can fragment the
  pool over time.  If that bites, use [Blkpool](/txt/exe/amc/reftype/Blkpool.md)
  (FIFO) or [Tpool](/txt/exe/amc/reftype/Tpool.md) (fixed-size).
- **`_FreeMem` requires the original size.**  The caller must
  remember (or re-derive) the size used at allocation time.
  The `Pool.Delete` wrapper handles this for typed elements;
  raw memory needs manual bookkeeping.
- **No return-to-OS.**  Memory grows monotonically.  This
  matches X2's typical pattern (long-running processes,
  steady-state workloads) but is a poor fit for short-lived
  burst allocations.
- **Counter only for typed args.**  When `arg:u8`, no `_n`
  field is generated — Lpool is treated as a raw allocator.

### See also
<a href="#see-also"></a>

- [Reftypes index](/txt/exe/amc/reftype.md)
- [Memory pools](/txt/exe/amc/pool.md) — allocator catalog
- [Tpool](/txt/exe/amc/reftype/Tpool.md) — fixed-size freelist
- [Blkpool](/txt/exe/amc/reftype/Blkpool.md) — FIFO refcounted pool
- [Sbrk](/txt/exe/amc/reftype/Sbrk.md) — bottom-level allocator
- Source: `cpp/amc/lpool.cpp`
- Tfunc records: `acr 'tfunc:Lpool.%'`

### Example
<a href="#example"></a>

The standard `algo_lib.FDb.lpool` pool — backbone of most
namespaces' allocations.  Generated `_AllocMem` / `_FreeMem`
are usually accessed through the `Pool.*` tfuncs of fields
that name `lpool` as their basepool:

```
dmmeta.field      field:abt.FDb.target  arg:abt.FTarget  reftype:Lary
  dmmeta.basepool  field:abt.FDb.target  base:algo_lib.FDb.lpool
```

Direct use:

```c++
void *p = lpool_AllocMem(_db, 1234);   // rounds up to 2048
lpool_FreeMem(_db, p, 1234);           // back to freelist[7]
```

## amc Reftype: Malloc
<a href="#amc-reftype-malloc"></a>

`Malloc` is the thinnest possible pool — a pass-through to
libc `malloc` / `free` / `realloc`.  It has no state in the
parent struct (its `_AllocMem` doesn't even need a parent
argument).  Use it when you want amc-style allocation
ergonomics (`_Alloc`, `_Delete`, `_AllocMaybe`) but the
underlying memory should come straight from libc.

```
dmmeta.field  field:lib_kafka.FDb.heap  arg:u8  reftype:Malloc  dflt:""  comment:""
```

### What it generates
<a href="#what-it-generates"></a>

No storage in the parent — Malloc is stateless.  `tclass_Malloc`
calls `GenTclass(amc_tclass_Pool)` so the field gets the full
`Pool.*` family on top of the libc primitives.

A trace counter (`algo_lib::_db.trace.alloc__db_malloc` /
`del__db_malloc`) is bumped on every alloc/free so the process-
wide malloc traffic is visible.

### Ssim inputs
<a href="#ssim-inputs"></a>

Required:

- `dmmeta.field` with `reftype:Malloc`.  `arg:` is conventionally
  `u8` for raw bytes.

No spec table; Malloc has no parameters.

### Generated functions
<a href="#generated-functions"></a>

Source: `cpp/amc/malloc.cpp`.

| Tfunc                  | Generated function                                                  | Effect |
|------------------------|---------------------------------------------------------------------|--------|
| `Malloc.AllocMem`      | `void* <name>_AllocMem(size_t n)`                                   | `malloc(n)`; returns NULL on failure. |
| `Malloc.ReallocMem`    | `void* <name>_ReallocMem(void* mem, size_t old, size_t new)`        | `realloc(mem, new)`; NULL leaves `mem` untouched. |
| `Malloc.FreeMem`       | `void <name>_FreeMem(void* mem, size_t n)`                          | `free(mem)`; the size is taken but unused. |

Plus the full `Pool.*` family (Alloc, AllocMaybe, AllocVarlen,
Delete, …).

### Memory model
<a href="#memory-model"></a>

- Whatever libc decides — typically a thread-safe heap with
  per-thread arenas (jemalloc, tcmalloc, glibc malloc).
- Each `_AllocMem` is a system-allocator call; no batching.
- `_FreeMem` ignores its `size` parameter (it's only there so
  Malloc has the same signature as Lpool/Blkpool — saves the
  caller from special-casing the allocator).

### Pitfalls
<a href="#pitfalls"></a>

- **Goes through libc's locks.**  Multi-threaded allocators
  contend on shared state; for hot paths use a per-thread
  Lpool instead.
- **No size tracking.**  `_FreeMem` accepts a size argument
  for API uniformity but does not validate or use it.
- **Not a great base pool.**  Other pools (Lpool, Blkpool)
  prefer `Sbrk` as a base because they want big contiguous
  blocks; Malloc handles many small allocations efficiently
  but is wasted as a wholesale provider.
- **Counters are global** (`algo_lib::_db.trace.alloc__db_malloc`)
  — multiple Malloc fields all bump the same counter.

### See also
<a href="#see-also"></a>

- [Reftypes index](/txt/exe/amc/reftype.md)
- [Sbrk](/txt/exe/amc/reftype/Sbrk.md) — bottom-level pool with huge-page support
- [Lpool](/txt/exe/amc/reftype/Lpool.md) — power-of-2 freelists on top of a base pool
- [Memory pools](/txt/exe/amc/pool.md)
- Source: `cpp/amc/malloc.cpp`
- Tfunc records: `acr 'tfunc:Malloc.%'`

### Example
<a href="#example"></a>

A Kafka client's working heap can use plain malloc to avoid
fragmenting the namespace's primary Lpool:

```
dmmeta.field  field:lib_kafka.FDb.heap  arg:u8  reftype:Malloc
```

```c++
void *buf = heap_AllocMem(1<<20);
...
heap_FreeMem(buf, 1<<20);
```

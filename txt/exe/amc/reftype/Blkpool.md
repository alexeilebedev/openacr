## amc Reftype: Blkpool
<a href="#amc-reftype-blkpool"></a>

`Blkpool` is a **bump-allocator with refcounted block reuse**,
tuned for FIFO traffic.  It carves variable-size allocations
sequentially from a large buffer, and only returns a buffer to
the free list once **every** element allocated from it has
been freed.  This is ideal for streaming work where messages
have similar lifetimes (e.g., a parser/processor pipeline) and
fatal for random-access workloads where one long-lived element
can pin a multi-MB buffer indefinitely.

```ssim
dmmeta.field  field:atf_unit.FDb.msg  arg:atf_unit.FMsg  reftype:Blkpool  dflt:""  comment:""
  dmmeta.basepool  field:atf_unit.FDb.msg  base:algo_lib.FDb.sbrk
```

### What it generates
<a href="#what-it-generates"></a>

Blkpool is a **varlen-capable pool**.  It pulls in the `Pool`
tclass (`GenTclass(amc_tclass_Pool)` at line 40), so a
Blkpool field gets all the standard `Pool.*` tfuncs
(`Alloc`, `AllocMaybe`, `AllocVarlen`, `Delete`, …) on top of
its own primitives.

Storage in the **parent** ctype (see `tclass_Blkpool` in
`cpp/amc/blkpool.cpp`):

| Field (parent)             | Type             | Meaning                                         |
|----------------------------|------------------|-------------------------------------------------|
| `<name>_free`              | `<name>_Bpbuf*`  | head of the free-buffer list (unused buffers)   |
| `<name>_cur`               | `<name>_Bpbuf*`  | buffer currently being filled                   |
| `<name>_pos`               | `u32`            | next free byte offset inside `_cur`             |
| `<name>_buf_dflt_size`     | `u32`            | size of the next buffer to allocate (default 4 KiB) |

A back-pointer is also added to the **element** ctype so the
allocator can locate the owning buffer at free time:

| Field (element)   | Type             | Meaning                               |
|-------------------|------------------|---------------------------------------|
| `<name>_buf`      | `<name>_Bpbuf*`  | enclosing buffer; refcount target     |

Each buffer is a `<name>_Bpbuf` struct (declared at the top of
the namespace header) carrying `next`, `refcount`, and `size`.

### How it works
<a href="#how-it-works"></a>

- **AllocMem(size)**:
  - Round `size` up to 16-byte alignment.
  - If the current buffer has room, bump `_pos` forward and
    return a pointer into it.  Increment the buffer's
    `refcount`.
  - Otherwise pull a buffer from `_free`; if no suitable
    buffer exists, call `ReserveBuffers(1, max(size+hdr, dflt_size))`
    which asks the base pool for fresh memory.
  - Stamp the back-pointer (`elem->_buf = curbuf`) so the
    deallocator can find the buffer.
  - **Subsequent constructors must not overwrite this field.**

- **FreeMem(elem)**:
  - Decrements `elem->_buf->refcount`.
  - When the count reaches zero, if the buffer is still the
    current one, just rewind `_pos`; otherwise prepend the
    buffer back to `_free`.
  - Calling `FreeMem` on an already-freed buffer triggers a
    `FatalErrorExit("blkpool_double_free")`.

- **No compaction** — memory inside a buffer is never moved.
  Holes left by freed elements are reclaimed only when the
  whole buffer empties.

### Ssim inputs
<a href="#ssim-inputs"></a>

Required:

- `dmmeta.field` with `reftype:Blkpool`.  `arg:` is the element
  type.
- `dmmeta.basepool` — Blkpool gets its raw memory from the base
  pool; typically that's `<ns>.FDb.sbrk` or another low-level
  allocator.

There is no `dmmeta.blkpool` record — Blkpool is configured at
runtime via `SetBufferSize`.

### Generated functions
<a href="#generated-functions"></a>

Source: `cpp/amc/blkpool.cpp`.

| Tfunc                    | Generated function                                          | Effect |
|--------------------------|-------------------------------------------------------------|--------|
| `Blkpool.Init`           | `<name>_Init(P&)`                                           | Set defaults: 4 KiB buffer size, empty free list, no current buffer. |
| `Blkpool.SetBufferSize`  | `void <name>_SetBufferSize(P&, u32 nbytes)`                 | Override the size of *future* buffers; does not affect already-allocated ones. |
| `Blkpool.ReserveBuffers` | `bool <name>_ReserveBuffers(P&, u32 nbuf, u32 size)`        | Prefetch `nbuf` buffers of `size` bytes onto the free list.  Returns false on OOM. |
| `Blkpool.AllocMem`       | `T* <name>_AllocMem(P&, size_t size)`                       | The core primitive: returns a 16-byte-aligned block of at least `size` bytes, or NULL. |
| `Blkpool.FreeMem`        | `void <name>_FreeMem(P&, <name>_Bpbuf* buf)`                | Decrement refcount; recycle the buffer when it goes to zero. |
| `Blkpool.UsedBytes`      | `u64 <name>_UsedBytes(P&)`                                  | Bytes reserved from the base pool minus the free list.  Whole buffers count, so a buffer pinned by one live element counts in full — this is the pool's address-space cost, which live-byte accounting understates. |

Plus the full set of `Pool.*` tfuncs (`Alloc`, `AllocMaybe`,
`AllocVarlen`, `AllocVarlenMaybe`, `InsertMaybe`, `Delete`,
…).  Because the element has the `_buf` back-pointer, every
`Delete` automatically calls `FreeMem`.

### Memory model
<a href="#memory-model"></a>

- Memory is allocated in chunks from the base pool, never freed
  back to it (until process exit).  Buffers cycle through
  `_cur` ↔ `_free`.
- The default buffer size is 4 KiB; **call `SetBufferSize` at
  startup** if your messages are bigger, or first-touch
  allocations will trigger a fresh allocation per oversize
  message.
- Pointers handed out by `AllocMem` are stable until `FreeMem`
  is called on the element.  The back-pointer makes the
  element-to-buffer lookup O(1).
- `Uninit` is a no-op — Blkpool does not free its buffers
  individually.  Buffers are released only at process exit
  (when the base pool tears down).
- **Reuse pattern**: when the current buffer is fully drained
  (refcount=0), `_pos` is rewound to just past the header.
  This is the FIFO sweet spot — allocations cluster in time
  and free in roughly the same order.

### Pitfalls
<a href="#pitfalls"></a>

- **One pinned element holds the whole buffer.**  Blkpool is
  unsafe for workloads with mixed long/short lifetimes — a
  rarely-freed element can pin megabytes.
- **No compaction.**  Free holes inside a buffer are wasted
  until the entire buffer empties.
- **Element ctors must preserve `<name>_buf`.**  If your
  constructor `memset`s the row or does placement-new with a
  fresh value, the back-pointer must be set *after*.  amc's
  generated `Alloc` does this correctly; manual placement-new
  needs care.
- **`SetBufferSize` only affects future buffers.**  Reserved or
  in-flight buffers stay the size they were allocated at.
- **Default 4 KiB is small.**  Many production allocators bump
  this to 64 KiB or 1 MiB at startup.
- **Double-free is fatal.**  `FreeMem` on an already-freed
  buffer aborts the process with `blkpool_double_free`.
- **AllocMem returns NULL on oversize requests.**  If your
  request exceeds the configured buffer size, Blkpool does
  not silently allocate a bigger buffer — caller gets NULL.

### See also
<a href="#see-also"></a>

- [Reftypes index](/txt/exe/amc/reftype.md)
- [Memory pools](/txt/exe/amc/pool.md) — allocator catalog
- [Lpool](/txt/exe/amc/reftype/Lpool.md) — varlen pool with per-size-class freelists (better random-access fit)
- [Tpool](/txt/exe/amc/reftype/Tpool.md) — fixed-size freelist
- Source: `cpp/amc/blkpool.cpp`
- Tfunc records: `acr 'tfunc:Blkpool.%'`

### Example
<a href="#example"></a>

A test harness that allocates many small messages:

```c++
atf_unit::FMsg *m = msg_Alloc();   // pulls from msg_AllocMem
m->kind = atf_unit_MsgKind_Note;
m->text = "...";
...
msg_Delete(*m);                    // returns memory to the buffer
```

To preallocate buffers for low-latency hot paths:

```c++
msg_SetBufferSize(_db, 1<<20);     // 1 MiB buffers
msg_ReserveBuffers(_db, 8, 1<<20); // 8 of them, up front
```

## amc Reftype: Sbrk
<a href="#amc-reftype-sbrk"></a>

`Sbrk` is the **bottom of the allocator stack**.  It serves
big allocations from the kernel directly: small requests go to
`sbrk(2)`, large ones (≥ 2 MiB on Linux) go to `mmap(2)` with
optional huge-page (`MAP_HUGETLB`) backing.  It is the only
amc-provided pool that talks to the kernel for memory, and it
is almost always the base pool for an `Lpool`/`Lary`/`Blkpool`
chain.

```ssim
dmmeta.field  field:algo_lib.FDb.sbrk  arg:u8  reftype:Sbrk  dflt:""  comment:"Base allocator for everything"
```

### What it generates
<a href="#what-it-generates"></a>

Three tunables in the **parent** ctype
(see `tclass_Sbrk` in `cpp/amc/sbrk.cpp`):

| Field                | Type   | Meaning                                                              |
|----------------------|--------|----------------------------------------------------------------------|
| `<name>_huge_limit`  | `u64`  | Max bytes that may use huge pages; 0 disables huge pages.  Must be 0 across `fork()`. |
| `<name>_huge_alloc`  | `u64`  | Bytes currently mapped with huge pages (running total).              |
| `<name>_zeromem`     | `bool` | If true, freshly-returned memory is zeroed (`memset(0)`) before return. |

Init sets all three to 0/false; tune them before any allocation
to enable huge pages.

### How it works
<a href="#how-it-works"></a>

On Linux/glibc:

- **Allocation `size >= 2 MiB`**: round to a 2 MiB multiple,
  call `mmap` with `MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE`.
  If `huge_alloc + size < huge_limit`, try `MAP_HUGETLB` first.
  After mapping huge pages, call `mlock` to detect kernel
  shortfall — if `mlock` fails the allocation is unmapped and
  Sbrk falls back to regular pages (and disables huge pages for
  the rest of the process).  Increments `algo_lib::_db.trace.n_mmap`
  once per block mapped, by whichever of the two routes mapped it.
- **Allocation `size < 2 MiB`**: call `sbrk(size)`.  Increments
  `algo_lib::_db.trace.n_sbrk`.
- After either path, if `zeromem` is true, touch every byte
  with `memset(0)`.

On macOS / FreeBSD / Cygwin / Windows the implementation falls
back to plain `malloc`/`free`.

### Free behavior
<a href="#free-behavior"></a>

- **`size >= 2 MiB`**: `munmap`.  Big allocations are returned
  to the kernel.
- **`size < 2 MiB`**: **no-op**.  `sbrk` cannot shrink the
  break in the presence of intermixed allocations, so small
  allocations are kept by the process for its lifetime.

### Ssim inputs
<a href="#ssim-inputs"></a>

Required:

- `dmmeta.field` with `reftype:Sbrk`.  `arg:` is conventionally
  `u8`.

No spec table.

### Generated functions
<a href="#generated-functions"></a>

Source: `cpp/amc/sbrk.cpp`.

| Tfunc            | Generated function                                          | Effect |
|------------------|-------------------------------------------------------------|--------|
| `Sbrk.Init`      | `<name>_Init(P&)` (folded)                                  | Zero tunables. |
| `Sbrk.AllocMem`  | `void* <name>_AllocMem(P&, u64 size)`                       | sbrk / mmap (huge-page-aware). |
| `Sbrk.FreeMem`   | `void <name>_FreeMem(P&, void* mem, u64 size)`              | munmap for big allocs; no-op for small. |

Plus the full `Pool.*` family.

### Memory model
<a href="#memory-model"></a>

- Small allocations are sticky — Sbrk grows monotonically for
  anything under 2 MiB.
- Big allocations may use huge pages (2 MiB on x86_64); pre-
  faulted via `MAP_POPULATE` + `mlock` so the first touch never
  takes a page fault.
- Memory is zeroed *by Sbrk* only if `_zeromem:true` — kernel
  pages are already zeroed, so the extra memset is mostly a
  precaution for reused `sbrk` space.
- `_huge_limit` must be reset to 0 before `fork()` — child
  processes inherit the mapping but their `huge_alloc` counter
  would be stale.

### Pitfalls
<a href="#pitfalls"></a>

- **No real `_FreeMem` for small allocations.**  Anything
  under 2 MiB is permanently held by the process.  This is
  fine — Sbrk is only the basepool for higher-level pools that
  do their own reuse.
- **`_huge_limit` must be zeroed before fork.**  Failing to do
  so causes the child to over-account huge pages.
- **Cross-platform fallbacks.**  macOS / FreeBSD / Cygwin /
  Windows redirect to `malloc`/`free`; the tunables are
  cosmetic on those platforms.
- **Huge-page failure is silent (but disabling).**  If
  `MAP_HUGETLB` fails, Sbrk falls back to regular pages **and
  sets `_huge_limit = 0`** to skip future attempts.  Reset it
  manually if you fix the underlying configuration.
- **Tracing**.  `algo_lib::_db.trace.n_sbrk` counts every
  small allocation; `n_mmap` counts every block actually
  mapped, and a mapping that fails counts as nothing.  Both are
  shared counters across all Sbrk fields in a process, and both
  count operations rather than reporting live bytes — the
  running huge-page total is `<name>_huge_alloc`.

### See also
<a href="#see-also"></a>

- [Reftypes index](/txt/exe/amc/reftype.md)
- [Lpool](/txt/exe/amc/reftype/Lpool.md) — typically built on Sbrk
- [Lary](/txt/exe/amc/reftype/Lary.md) — same
- [Blkpool](/txt/exe/amc/reftype/Blkpool.md) — same
- [Memory pools](/txt/exe/amc/pool.md)
- Source: `cpp/amc/sbrk.cpp`
- Tfunc records: `acr 'tfunc:Sbrk.%'`

### Example
<a href="#example"></a>

Tuning the standard `algo_lib::FDb.sbrk` for a big in-memory
table:

```c++
sbrk_huge_limit = 8ULL << 30;   // up to 8 GiB on huge pages
sbrk_zeromem    = false;         // saves a memset on first touch
```

Once configured, every Lpool / Lary / Blkpool that chains to
`sbrk` benefits automatically.

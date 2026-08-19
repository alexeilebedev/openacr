## amc Reftype: Lary
<a href="#amc-reftype-lary"></a>

`Lary` ("level array") is an indirect array of values that grows
without ever moving the elements that are already in it.  Once
a pointer to a Lary element is taken, it remains valid for the
lifetime of the row — which is exactly what is needed for any
record that is the **target** of a cross-reference (`Ptr`,
`Ptrary`, `Llist`, `Thash`, `Atree`, `Bheap`, ...).  It is
amc's go-to allocator for records that are not deleted
individually.

```
dmmeta.field  field:ssim2mysql.FDb.ctype  arg:ssim2mysql.FCtype  reftype:Lary  dflt:""  comment:""
```

### What it generates
<a href="#what-it-generates"></a>

Two storage fields are inserted into the parent ctype
(see `tclass_Lary` in `cpp/amc/lary.cpp`):

| Field            | C++ type    | Meaning                                          |
|------------------|-------------|--------------------------------------------------|
| `<name>_lary[36]`| `T*[36]`    | 36 level pointers — `lary[k]` points to a block of `2^k` elements |
| `<name>_n`       | `i64`       | total number of elements across all levels       |

Level `k` holds at most `2^k` elements.  Level 0 has 1 slot,
level 1 has 2 slots, …, level 35 has 2^35 slots, for a hard
maximum of `2^36 - 1` (≈ 68 billion) elements.  Indexed lookup
uses one `BitScanReverse` to compute the level and the offset
within the level in O(1).

The first 4 levels are **pre-allocated as a single block** at
`Init` time (see `tfunc_Lary_Init` line 110), so a Lary that
never grows past 31 elements only ever does one allocation from
the base pool.  After that, each new level is allocated lazily
as needed.

Because the level pointers themselves live in the parent struct
and are never moved, `&<name>_lary[k][i]` is a permanent
pointer.

Lary also pulls in the **Pool** tclass
(`GenTclass(amc_tclass_Pool)` at line 39) — so every `Pool.*`
tfunc (such as `Alloc`, `AllocMaybe`, `InsertMaybe`,
`Delete`, …) is generated on top of the Lary primitives.

### Ssim inputs
<a href="#ssim-inputs"></a>

Required:

- `dmmeta.field` with `reftype:Lary`.  `arg:` is the element
  type.
- `dmmeta.basepool` (or a namespace default) — Lary calls
  `<basepool>_AllocMem(...)` for each level.

Constraint: the **parent ctype must be a global (FDb)**.
Local Lary fields are rejected at code-gen time with
`"Lary <arg> requires global FDb._db in namespace <ns>"`
(`tclass_Lary` line 42).  This is because Lary records have no
owning row — they are flat instances of the in-memory database
table.

There is no `dmmeta.lary` record — Lary takes no per-field
options.  The `nlevels=36` and `prealloc=4` constants are
hard-coded in amc.

### Memory model
<a href="#memory-model"></a>

- 36 level pointers in the parent struct, NULL until the level
  is first reached.
- `Init` allocates one block large enough for the first 4 levels
  (1+2+4+8 = 15 slots) and parcels it across `lary[0..3]`.
  Out-of-memory in `Init` is fatal.
- `AllocMem` returns the next slot, lazily creating its level
  the first time the level is touched.  Returns NULL if the
  base pool is exhausted (which means `AllocMaybe` from the
  Pool tclass returns NULL; the unsuffixed `Alloc` kills the
  process).
- Levels are never freed during the lifetime of the table.
  `RemoveAll` / `RemoveLast` decrement `_n` and run destructors,
  but the level blocks stay allocated — they remain available
  for the next `Alloc`.  Memory is only returned to the OS at
  process exit (via `Uninit`, which walks `lary[]` and frees
  each non-NULL block).
- **Pointers, references and cursors are permanent** for the
  lifetime of the row.  This is the property that makes Lary
  the right place to hold any record that gets cross-referenced.

### Generated functions
<a href="#generated-functions"></a>

Lary's own tfuncs (declared in `cpp/amc/lary.cpp`):

| Tfunc            | Generated function                              | Effect |
|------------------|-------------------------------------------------|--------|
| `Lary.Init`      | `<name>_Init(P&)` (macro)                       | Zero `_n` and `_lary[]`, preallocate first 4 levels. |
| `Lary.Uninit`    | `<name>_Uninit(P&)` (macro)                     | Destruct each element, then free every non-NULL `lary[k]`. |
| `Lary.AllocMem`  | `void* <name>_AllocMem(P&)`                     | Reserve one slot, return raw memory (no constructor). Returns NULL on OOM. |
| `Lary.N`         | `i64 <name>_N(const P&)`                        | Element count. |
| `Lary.EmptyQ`    | `bool <name>_EmptyQ(P&)`                        | `_n == 0`. |
| `Lary.Find`      | `T* <name>_Find(P&, Rowid t)`                   | Bounded find; NULL if `t >= _n`.  Inline; one `BitScanReverse`. |
| `Lary.qFind`     | `T& <name>_qFind(P&, Rowid t)`                  | "Quick" find — no bounds check. |
| `Lary.Last`      | `T* <name>_Last(P&)`                            | Pointer to last element, or NULL if empty. |
| `Lary.RemoveAll` | `void <name>_RemoveAll(P&)`                     | Destruct every element, set `_n=0`.  Keeps level blocks. |
| `Lary.RemoveLast`| `void <name>_RemoveLast(P&)`                    | Destruct last element, decrement `_n`.  No-op if empty. |
| `Lary.RowidFind` | Internal `ImrowPtr` helper for reflection       | Only emitted if `field.reflect` is set. |
| `Lary.curs`      | `<P>_<name>_curs` + `_Reset/_ValidQ/_Next/_Access` | Forward cursor used by `ind_beg`. |

Because the **Pool** tclass is also instantiated, the field
additionally gets:

| Tfunc                    | Generated function                                    | Effect |
|--------------------------|-------------------------------------------------------|--------|
| `Pool.Alloc`             | `T& <name>_Alloc()`                                   | Allocate one default-constructed element. Kills process on OOM. |
| `Pool.AllocMaybe`        | `T* <name>_AllocMaybe()`                              | Same, returning NULL on OOM. |
| `Pool.InsertMaybe`       | `T* <name>_InsertMaybe(const T& value)`               | Allocate + assign from value + XrefMaybe. |
| `Pool.UpdateMaybe`       | `T* <name>_UpdateMaybe(const T& value)`               | Update existing row by pkey, or insert if missing. |
| `Pool.AllocExtra`        | `T& <name>_AllocExtra()` (varlen elements)            | Variant for varlen element types. |
| `Pool.AllocVarlen` / `Pool.AllocVarlenMaybe` | varlen element allocation             | See [Varlen](/txt/exe/amc/reftype/Varlen.md). |
| `Pool.Delete`            | (omitted)                                             | Lary does **not** support `Delete`. |

Note the absence of `Delete`: an individual row cannot be
removed from a Lary without invalidating cross-references to
later rows.  Use `RemoveLast` / `RemoveAll` when you really
want to tear the whole table down.

### Pitfalls
<a href="#pitfalls"></a>

- **No `Delete`.** Per-row deletion is unsafe in a Lary because
  shifting later rows back would invalidate pointers.  If you
  need to delete individual rows, use a [Tpool](/txt/exe/amc/reftype/Tpool.md)
  or [Lpool](/txt/exe/amc/reftype/Lpool.md) instead.
- **FDb-only.** Lary fields are only allowed on the per-namespace
  global ctype (FDb).  This is enforced by amc at code-gen time.
- **Memory never returns to the OS** (until process exit).
  `RemoveAll` keeps level blocks for the next round of growth.
  This is usually what you want — but if the table peaks high and
  stays low, the high-water capacity sticks around.
- **OOM in `Init` is fatal.**  The pre-allocation of the first 4
  levels is unconditional and aborts the process if the base
  pool cannot satisfy it.
- **Max size = 2^36 − 1.**  Lary cannot grow beyond 36 levels
  (≈ 68 G elements).  Any `AllocMem` past that returns NULL.

### See also
<a href="#see-also"></a>

- [Reftypes index](/txt/exe/amc/reftype.md)
- [Tary](/txt/exe/amc/reftype/Tary.md) — std::vector-like, but moves on grow (no xrefs)
- [Tpool](/txt/exe/amc/reftype/Tpool.md) — freelist allocator for individually-deleted rows
- [Lpool](/txt/exe/amc/reftype/Lpool.md) — variable-size pool over 36 Tpools
- [Memory pools](/txt/exe/amc/pool.md) — base pool model
- Source: `cpp/amc/lary.cpp`
- Tfunc records: `acr 'tfunc:Lary.%'`

### Example
<a href="#example"></a>

The amc compiler itself uses Lary for everything that other
records point at — for instance, every C-type loaded by amc
lives in a Lary in `amc::FDb`:

```
dmmeta.field  field:amc.FDb.ctype  arg:amc.FCtype  reftype:Lary  dflt:""  comment:""
```

Insertion at runtime:

```c++
amc::FCtype *ctype = amc::ctype_InsertMaybe(in);
if (!ctype) { /* duplicate pkey or OOM */ }
// &*ctype is stable for the rest of the process lifetime
```

Iteration:

```c++
ind_beg(amc::_db_ctype_curs, ctype, amc::_db) {
    prlog(ctype.ctype);
} ind_end;
```

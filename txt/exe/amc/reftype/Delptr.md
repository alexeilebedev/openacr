## amc Reftype: Delptr
<a href="#amc-reftype-delptr"></a>

`Delptr` is a **lazily-allocated owning pointer** to a single
value.  The field is a pointer in the parent struct that starts
as `NULL`; the first call to `<name>_Access` allocates the
value from the namespace's default pool.  When the parent
struct is destructed (or `<name>_Delete` is called), the
allocation is freed.

Think of it as `std::unique_ptr<T>` with lazy construction —
ideal for optional sub-objects that are usually absent and
expensive to construct.

```ssim
dmmeta.field  field:acr.FRec.oldpkey  arg:algo.cstring  reftype:Delptr  dflt:""  comment:"Old pkey (if renamed)"
```

### What it generates
<a href="#what-it-generates"></a>

A single pointer field is added to the parent ctype
(see `tclass_Delptr` in `cpp/amc/delptr.cpp`):

| Field        | Type   | Meaning                              |
|--------------|--------|--------------------------------------|
| `<name>`     | `T*`   | NULL or owns one heap-allocated `T`  |

Memory comes from the **namespace default pool**
(`dmmeta.nsx.pool`).  Without a default pool, amc rejects the
field with `"namespace needs a default pool"`.

### Ssim inputs
<a href="#ssim-inputs"></a>

Required:

- `dmmeta.field` with `reftype:Delptr`.  `arg:` is the value
  type.  `c_` is the conventional field-name prefix but Delptr
  accepts any name
  ([field name prefixes](/txt/exe/amc/reftype.md#field-name-prefixes)).
- A `dmmeta.nsx.pool` default pool in the namespace.

The `dflt:` value, if any, is passed to the placement-new
constructor on the first `_Access` call.

### Generated functions
<a href="#generated-functions"></a>

Source: `cpp/amc/delptr.cpp`.

| Tfunc            | Generated function                          | Effect |
|------------------|---------------------------------------------|--------|
| `Delptr.Init`    | `<name>_Init(P&)` (inline)                  | `parent.<name> = NULL;` |
| `Delptr.Uninit`  | `<name>_Uninit(P&)`                         | Calls `<name>_Delete(parent)`. |
| `Delptr.Access`  | `T& <name>_Access(P&)`                      | Returns reference to the value, creating it via placement new (initialized to `dflt`) on first call.  OOM is fatal. |
| `Delptr.Delete`  | `void <name>_Delete(P&)`                    | If the pointer is non-NULL, destruct the value and free its memory to the default pool, then set NULL.  Idempotent. |

### Memory model
<a href="#memory-model"></a>

- One pointer's worth of overhead in the parent regardless of
  whether the value has been allocated.
- First `_Access` performs `AllocMem(sizeof(T))` on the default
  pool, then `new (ret) T(dflt)`.  OOM is fatal — there is no
  `_AccessMaybe`.
- Destruction is automatic via the parent's `_Uninit`.
- The allocation lives in the namespace's default pool — for
  most projects that's a `Lpool` or `Lary` chain rooted at
  `<ns>.FDb.lpool` / `sbrk`.

### Pitfalls
<a href="#pitfalls"></a>

- **Namespace default pool required.**  amc refuses to generate
  Delptr without `dmmeta.nsx.pool`.  Add a default pool to the
  namespace first.
- **`_Access` is non-const and can allocate.**  Calling it on a
  read-only path is wasteful and can trigger OOM on a value
  the caller didn't actually want.  Test the raw pointer
  (`parent.<name> != NULL`) when you only want to read.
- **OOM is fatal.**  `_Access` kills the process if allocation
  fails — there is no Maybe-flavored access path.
- **Not an xref target.**  Delptr is a *value*, not a
  cross-reference.  You cannot have a `Ptr` or `Llist` whose
  target is held in a Delptr.
- **Field-function form not supported.**  amc explicitly
  refuses `FldfuncQ(field)` on Delptr.

### See also
<a href="#see-also"></a>

- [Reftypes index](/txt/exe/amc/reftype.md)
- [Val](/txt/exe/amc/reftype/Val.md) — always-present embedded value
- [Opt](/txt/exe/amc/reftype/Opt.md) — optional trailing field for protocols
- [Memory pools](/txt/exe/amc/pool.md) — the default pool model
- Source: `cpp/amc/delptr.cpp`
- Tfunc records: `acr 'tfunc:Delptr.%'`

### Example
<a href="#example"></a>

`acr` keeps the old pkey of a renamed record only when a rename
actually happens:

```ssim
dmmeta.field  field:acr.FRec.oldpkey  arg:algo.cstring  reftype:Delptr  dflt:""
```

In C++:

```c++
acr::FRec rec;
// 99% of records never get an old pkey — no allocation:
process(rec);

// On rename:
oldpkey_Access(rec) = "the.old.name";  // first call: allocates
oldpkey_Access(rec) = "the.old.name2"; // subsequent: same allocation
```

When `rec` goes out of scope (or is removed from its pool), the
old `cstring` is destructed and freed automatically.

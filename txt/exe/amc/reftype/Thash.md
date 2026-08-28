## amc Reftype: Thash
<a href="#amc-reftype-thash"></a>

`Thash` is a **cross-reference** hash index keyed on one field
of the element ctype.  It is the standard way to look up a row
by primary or secondary key.  Like every xref, Thash doesn't
own its elements — it threads them through a single
collision-chain pointer that amc adds to the element type.
Operations are amortized O(1); the bucket array is power-of-2
sized and grows by doubling.

```ssim
dmmeta.field  field:abt.FDb.ind_target  arg:abt.FTarget  reftype:Thash  dflt:""  comment:""
  dmmeta.thash  field:abt.FDb.ind_target  hashfld:abt.FTarget.target  unique:Y  comment:""
```

### What it generates
<a href="#what-it-generates"></a>

State in the **parent** ctype (see `tclass_Thash` in
`cpp/amc/thash.cpp`):

| Field (parent)            | Type     | Meaning                                              |
|---------------------------|----------|------------------------------------------------------|
| `<name>_buckets_elems`    | `T**`    | bucket array, length is a power of two               |
| `<name>_buckets_n`        | `i32`    | number of buckets (always a power of two)            |
| `<name>_n`                | `i32`    | number of indexed elements                           |

State in the **element** ctype:

| Field (element)        | Type    | Meaning                                                 |
|------------------------|---------|---------------------------------------------------------|
| `<xfname>_next`        | `T*`    | next element in this bucket's collision chain; `(T*)-1` means "not in hash" |
| `<xfname>_hashval`     | `u32`   | cached hash value, used for cheap rehash on grow        |

The element's `Init` is augmented to set `next=(T*)-1` and
`hashval=0`.

### How it works
<a href="#how-it-works"></a>

- `_buckets_n` starts at 4 and doubles whenever
  `_n` would exceed `_buckets_n`.  That keeps the load factor
  ≤ 1.0; collision chains stay short in practice.
- Each bucket is a singly-linked list threaded through the
  element's `<xfname>_next` field.  Insertion pushes at the
  head of the bucket; removal walks the chain and rewrites
  the predecessor's pointer.
- Looking up a key:
  1. compute `hash = <ElemType>_Hash(0, key)`
  2. `index = hash & (buckets_n - 1)`
  3. walk the bucket chain comparing `(*row).<hashfld>` to
     `key` until either matched or the chain ends.
- The element's cached `<xfname>_hashval` makes growth cheap:
  rehash never recomputes the hash, just remasks
  `hashval & (new_buckets_n - 1)`.

### Hash variants
<a href="#hash-variants"></a>

The `chash.hashtype` field on the **element** ctype selects
the hash function:

| Hashtype | Behavior |
|----------|----------|
| `CRC32`  | Default.  Uses the algo CRC32 mix.  Bucket chains are walked on Find/Insert. |
| `Linear` | Identity hash — the hash value *is* the index, no collisions.  `_buckets` becomes a sparse direct-address table; `Find` is a single load.  Grows via `ReallocMem`, preserving slots. |
| `Extern` | User provides `<ElemType>_Hash`.  Treated like CRC32 for the rest of the layout. |

The Linear variant is intended for small dense integer keys
(e.g. enum values) — the buckets array is sized to at least
`max_key + 1`, so a sparse keyspace blows up memory.  amc
selects Linear via the `Find`/`Insert` code paths
(`HashLinearQ()` in `cpp/amc/thash.cpp`).

### Ssim inputs
<a href="#ssim-inputs"></a>

Required:

- `dmmeta.field` with `reftype:Thash`.  `arg:` is the element
  ctype.  The field name must start with `ind_`
  ([field name prefixes](/txt/exe/amc/reftype.md#field-name-prefixes)).

- `dmmeta.thash`:
  - `hashfld:<Type>.<field>` — which element field is the hash
    key.  Often the pkey, but any field works.
  - `unique:Y/N` — when `Y`, `InsertMaybe` rejects duplicates and
    returns false.  When `N`, the hash is a multimap (multiple
    rows can share a key; `Find` returns the first match).

- `dmmeta.chash` on the element ctype to set the hash function
  (defaults to CRC32 if absent).

- `dmmeta.xref` if you want the index to be populated
  automatically by row `XrefMaybe`.  Without an xref the hash
  is manual.

- `dmmeta.basepool` (or a namespace default) — Thash allocates
  the bucket array from this pool.

### Generated functions
<a href="#generated-functions"></a>

Source: `cpp/amc/thash.cpp`.

| Tfunc                  | Generated function                                              | Effect |
|------------------------|-----------------------------------------------------------------|--------|
| `Thash.Init`           | `<name>_Init(P&)` (macro)                                       | Allocate the initial 4-bucket array; OOM is fatal. |
| `Thash.Uninit`         | `<name>_Uninit(P&)` (macro)                                     | Free the bucket array (no-op for global FDb). |
| `Thash.N`              | `i32 <name>_N(const P&)`                                        | Number of indexed elements. |
| `Thash.EmptyQ`         | `bool <name>_EmptyQ(P&)`                                        | `_n == 0`. |
| `Thash.Find`           | `T* <name>_Find(P&, key)`                                       | Look up by key; NULL if not found. |
| `Thash.FindX`          | `T& <name>_FindX(P&, key)`                                      | Look up by key; throw on miss. |
| `Thash.FindRemove`     | `T* <name>_FindRemove(P&, key)` *(opt-in, see below)*           | Find + unlink in one walk. |
| `Thash.GetOrCreate`    | `T& <name>_GetOrCreate(P&, key)` or `T* …`                      | Find; on miss, allocate the underlying row, set its key field, and `XrefMaybe`.  Reference-return when amc can prove the xref always succeeds; pointer-return otherwise.  Only emitted if the element type has an instantiating pool (so the row can be created). |
| `Thash.InsertMaybe`    | `bool <name>_InsertMaybe(P&, T& row)`                           | Add `row` to the index using its current `hashfld`.  Returns false if `unique:Y` and the key is already present.  No-op if `row` is already in this index. |
| `Thash.Remove`         | `void <name>_Remove(P&, T& row)`                                | Unlink `row` from its bucket; no-op if not in the hash. |
| `Thash.Cascdel`        | (private)                                                       | Walk every bucket and `Delete` each element. Emitted when the xref has `cascdel:Y`. |
| `Thash.Reserve`        | `void <name>_Reserve(P&, i32 n)`                                | Grow buckets so that `n` *more* elements would fit. |
| `Thash.AbsReserve`     | `void <name>_AbsReserve(P&, i32 n)`                             | Grow buckets so that `n` absolute elements would fit.  Reallocs bucket array, rehashes every element (or, for `Linear`, just clears the new tail). |
| `Thash.curs`           | `<P>_<name>_curs` + `_Reset/_ValidQ/_Next/_Access` *(opt-in)*   | Forward cursor — walks each bucket head-to-tail, so rows come out in no key order.  Request it with a `dmmeta.fcurs` row, see [cursors](/txt/exe/amc/reftype.md#cursors). |

### FindRemove (opt-in)
<a href="#findremove-opt-in-"></a>

`FindRemove` (atomic find-and-unlink by key) is not generated by default.
Request it with an
[ffunc](/txt/exe/amc/backend/tclass-tfunc.md#ffunc) (`extrn:N` — amc
generates the body):

```ssim
dmmeta.ffunc  ffunc:<field>.FindRemove  extrn:N  comment:""
```

(This replaced the former `dmmeta.findrem` table.)

### Memory model
<a href="#memory-model"></a>

- The bucket array is the only allocation; everything else is
  stored in the element rows themselves.
- Growth is O(n) (every element gets rehashed for non-Linear
  hashes) but amortized O(1) per insertion.
- Bucket allocation failure during `Init` or `AbsReserve` is
  **fatal** — there is no graceful path for an out-of-memory
  hash because every other operation assumes a non-empty bucket
  array.
- Removal does not shrink the bucket array.
- The cached `<xfname>_hashval` makes rehash cheap, so even
  a very high-load Thash grows fast.

### Pitfalls
<a href="#pitfalls"></a>

- **Element's `_Init` must run** before the row is inserted.
  Bypassing it (e.g. `memset` on a stack temporary) leaves
  `next=NULL` instead of the "not in hash" sentinel, and the
  row will look like it is in the index.
- **`Linear` hashtype is a sparse direct-address table.**  Keys
  index into the bucket array directly, so a key of value `K`
  allocates `K+1` bucket slots.  Use only with small dense
  enums.
- **No multimap iteration helper.** With `unique:N`, `Find`
  returns the first matching row — to walk all matches you
  must follow `<xfname>_next` yourself (and stop at the next
  row whose `hashfld` differs).
- **OOM in `Init` is fatal.** Initial bucket allocation cannot
  fail gracefully.
- **`GetOrCreate` is only generated when amc can find an
  appropriate creating pool.** If your element type has no
  pool to allocate from, you'll only get `Find` /
  `InsertMaybe`.

### See also
<a href="#see-also"></a>

- [Reftypes index](/txt/exe/amc/reftype.md)
- [Atree](/txt/exe/amc/reftype/Atree.md) — ordered index, range queries
- [Ptrary](/txt/exe/amc/reftype/Ptrary.md) — when you only need iteration, not lookup
- [Cross-references](/txt/exe/amc/xref.md) — auto-insertion model
- Source: `cpp/amc/thash.cpp`
- Tfunc records: `acr 'tfunc:Thash.%'`

### Example
<a href="#example"></a>

```c++
abt::FTarget &t = target_GetOrCreate("abt_md");
// equivalent of:
abt::FTarget *t = ind_target_Find("abt_md");
if (!t) {
    t = &target_Alloc();
    t->target = "abt_md";
    if (!target_XrefMaybe(*t)) {
        target_Delete(*t);
        t = NULL;
    }
}
```

Iteration over the table (cursor walks buckets in order):

```c++
ind_beg(abt::_db_ind_target_curs, target, abt::_db) {
    prlog(target.target);
} ind_end;
```

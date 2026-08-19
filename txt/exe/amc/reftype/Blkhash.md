## amc Reftype: Blkhash
<a href="#amc-reftype-blkhash"></a>

`Blkhash` is a **cross-reference** hash index for keys with a dense
integer component.  Consider an index of sequenced messages keyed by
(stream id, seqno).  A regular hash function must spread all keys
uniformly, so ten million cached messages cost a ten-million-pointer
bucket array that doubles and rehashes as it grows, and neighboring
seqnos land in unrelated cache lines.  But seqnos are consecutive:
only the stream id needs spreading.

Blkhash splits the key accordingly.  The low `linbits` bits of the
dense component (`linfld`) select a slot directly inside a fixed
block of 2^linbits entry pointers; the key with those bits masked
off identifies the block, and is hashed to find it in a small bucket
array of block chains.  A matched block plus a slot determines the
full key exactly, so the index is inherently unique, needs no
collision chain, and adds no intrusive fields to the element.
Consecutive keys of one stream occupy consecutive slots of one block.

```
dmmeta.field  field:<ns>.FDb.ind_seqmsg_id  arg:<ns>.FSeqmsgId  reftype:Blkhash  dflt:""  comment:""
  dmmeta.blkhash  field:<ns>.FDb.ind_seqmsg_id  hashfld:<ns>.FSeqmsgId.seqmsg_id  linfld:<proto>.SeqmsgId.seq  linbits:12  comment:""
```

The API matches [Thash](/txt/exe/amc/reftype/Thash.md) (`Find`,
`FindX`, `InsertMaybe`, `Remove`, `N`, `EmptyQ`, cursor), so
converting a unique Thash over a suitable key to Blkhash is a
data-only change: flip the field's reftype and replace the
`dmmeta.thash` row with a `dmmeta.blkhash` row.

### What it generates
<a href="#what-it-generates"></a>

State in the **parent** ctype (see `tclass_Blkhash` in
`cpp/amc/blkhash.cpp`):

| Field (parent)            | Type      | Meaning                                        |
|---------------------------|-----------|------------------------------------------------|
| `<name>_buckets_elems`    | `Blk**`   | bucket array — chains of blocks                |
| `<name>_buckets_n`        | `i32`     | number of buckets (always a power of two)      |
| `<name>_nblk`             | `i32`     | number of resident blocks                      |
| `<name>_n`                | `i32`     | number of indexed elements                     |

Per index, one block struct `<Parname>_<name>_Blk` is emitted:

| Field (block)   | Type     | Meaning                                                 |
|-----------------|----------|---------------------------------------------------------|
| `key`           | key type | key with the low `linbits` linfld bits zeroed           |
| `next`          | `Blk*`   | bucket chain                                            |
| `elem`          | `T**`    | 2^linbits slot pointers; slot = linfld & (2^linbits-1)  |
| `hashval`       | `u32`    | hash of the masked key, for cheap re-chain on growth    |
| `n`             | `u32`    | occupied slots; the block is freed when it reaches zero |

The **element** ctype gets no new fields — membership is recorded
only in the block's slot pointer.

### How it works
<a href="#how-it-works"></a>

- Lookup masks the low `linbits` bits off the key, hashes the masked
  key (`CRC32`) into the bucket array, walks the block chain to the
  matching block, and loads `blk->elem[linfld & mask]`.  One short
  chain walk and one direct load — no per-entry compares.
- Insertion finds or allocates the block, then claims the slot.  A
  slot occupied by a *different* row means a duplicate key:
  `InsertMaybe` returns false (the index is inherently `unique:Y`).
- Removal clears the slot; a block whose last slot clears is unlinked
  and its memory returned to the basepool, ready for the next block
  of the same index.  A rolling key window (append at the tail,
  retire at the head) therefore reaches steady state with no
  allocation and no rehashing.
- The bucket array doubles when `nblk` would exceed it, re-chaining
  blocks by their stored `hashval` — it holds one pointer per
  resident *block*, so it is a factor 2^linbits smaller than a
  Thash's bucket array over the same population.

### Ssim inputs
<a href="#ssim-inputs"></a>

Required:

- `dmmeta.field` with `reftype:Blkhash`.  `arg:` is the element
  ctype.  The field name must start with `ind_`, a prefix shared
  with Thash — which means `acr_ed` infers Thash from the name and
  Blkhash has to be named explicitly
  ([field name prefixes](/txt/exe/amc/reftype.md#field-name-prefixes)).

- `dmmeta.blkhash`:
  - `hashfld:<Type>.<field>` — which element field is the key.
  - `linfld:<KeyType>.<field>` — the dense integer component; a
    field of hashfld's arg, possibly wrapped in a single-field ctype
    (e.g. `algo.SeqType.value`).
  - `linbits` — a block holds 2^linbits slots; the low linbits bits
    of linfld select the slot directly.

- `dmmeta.xref` if you want the index populated automatically by row
  `XrefMaybe`.  Without an xref the index is manual.

- `dmmeta.basepool` (or a namespace default) — blocks and the bucket
  array are allocated from this pool.

### Generated functions
<a href="#generated-functions"></a>

Source: `cpp/amc/blkhash.cpp`.

| Tfunc                    | Generated function                                    | Effect |
|--------------------------|-------------------------------------------------------|--------|
| `Blkhash.Init`           | `<name>_Init(P&)` (macro)                             | Allocate the initial 4-bucket array; OOM is fatal. |
| `Blkhash.Uninit`         | `<name>_Uninit(P&)` (macro)                           | Free every resident block and the bucket array. |
| `Blkhash.N`              | `i32 <name>_N(const P&)`                              | Number of indexed elements. |
| `Blkhash.EmptyQ`         | `bool <name>_EmptyQ(P&)`                              | `_n == 0`. |
| `Blkhash.Find`           | `T* <name>_Find(P&, key)`                             | Look up by key; NULL if not found. |
| `Blkhash.FindX`          | `T& <name>_FindX(P&, key)`                            | Look up by key; throw on miss. |
| `Blkhash.FindRemove`     | `T* <name>_FindRemove(P&, key)` *(opt-in via `dmmeta.ffunc`, as for Thash)* | Find + clear the slot in one walk. |
| `Blkhash.InsertMaybe`    | `bool <name>_InsertMaybe(P&, T& row)`                 | Claim the row's slot.  Returns false if a different row holds the key.  No-op if `row` is already in the index. |
| `Blkhash.Remove`         | `void <name>_Remove(P&, T& row)`                      | Clear the row's slot; no-op if the row is not in the index.  Frees the block when its last slot clears. |
| `Blkhash.Cascdel`        | (private)                                             | Delete every element reachable through the index.  Emitted when the xref has `cascdel:Y`. |
| `Blkhash.curs`           | `<P>_<name>_curs` + `_Reset/_ValidQ/_Next/_Access` *(opt-in)* | Forward cursor — walks each bucket's blocks and each block's occupied slots.  Request it with a `dmmeta.fcurs` row, see [cursors](/txt/exe/amc/reftype.md#cursors). |

### Memory model
<a href="#memory-model"></a>

- A resident block costs 2^linbits pointers **regardless of
  occupancy** (32KB at linbits:12).  Pick linbits at or below the
  logarithm of the typical resident run of consecutive keys; sparse
  keys degrade the index into one nearly-empty block per key.
- Freed blocks return to the basepool, so a rolling window recycles
  the same memory indefinitely.
- Bucket-array growth is O(nblk) (blocks re-chain by cached hashval,
  no element is touched) and the array is tiny — one pointer per
  block.
- Bucket or block allocation failure is **fatal**, as for Thash.
- Removal does not shrink the bucket array.

### Pitfalls
<a href="#pitfalls"></a>

- **The dense component must actually be dense.**  Keys whose linfld
  values are scattered allocate one block each — memory per key
  becomes 2^linbits pointers and every lookup still pays the chain
  walk.  Use Thash for uncorrelated keys.
- **The index is always unique.**  A slot holds one pointer; there is
  no multimap variant.
- **No `GetOrCreate`.**  Create the row and let `XrefMaybe` insert
  it, or call `InsertMaybe` manually.
- **linbits is a per-index memory knob.**  An index instanced per
  parent record (e.g. per stream) holds at least one block per
  non-empty parent; account for 2^linbits pointers each.

### See also
<a href="#see-also"></a>

- [Reftypes index](/txt/exe/amc/reftype.md)
- [Thash](/txt/exe/amc/reftype/Thash.md) — general-purpose hash index for uncorrelated keys
- [Cross-references](/txt/exe/amc/xref.md) — auto-insertion model
- [dmmeta.blkhash](/txt/ssimdb/dmmeta/blkhash.md) — the declaring ssimfile
- Source: `cpp/amc/blkhash.cpp`
- Tfunc records: `acr 'tfunc:Blkhash.%'`

### Example
<a href="#example"></a>

```c++
// <ns>.FDb.ind_seqmsg_id: message cache index over (stream_id, seq)
<ns>::FSeqmsgId *seqmsg = <ns>::ind_seqmsg_id_Find(seqmsg_id);
```

Iteration (cursor walks blocks bucket by bucket, occupied slots in
slot order within each block):

```c++
ind_beg(<ns>::_db_ind_seqmsg_id_curs, seqmsg, <ns>::_db) {
    prlog(seqmsg.seqmsg_id);
} ind_end;
```

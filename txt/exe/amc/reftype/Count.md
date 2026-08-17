## amc Reftype: Count
<a href="#amc-reftype-count"></a>

`Count` is the **minimal cross-reference** — it keeps a running
total of "how many rows of type `arg` are flagged as belonging
to this index" without storing any pointers to them.  Use it
when you need a fast count but don't need iteration or lookup.

```
dmmeta.field  field:abt.FDb.cnt_dirty  arg:abt.FTarget  reftype:Count  dflt:""  comment:"Number of dirty targets"
```

### What it generates
<a href="#what-it-generates"></a>

A pre-amc phase (`gen_newfield_count` in `cpp/amc/gen.cpp`)
synthesizes two ordinary `Val` fields:

| Field                         | Type   | Lives on        | Meaning                              |
|-------------------------------|--------|-----------------|--------------------------------------|
| `<name>_n`                    | `u32`  | parent ctype    | current count                        |
| `<arg>.<name>_value`          | `bool` | element ctype   | whether *this row* is currently counted |

The boolean per-row flag makes `Insert` and `Remove` idempotent
— calling `Insert` twice does not over-count, and `Remove` of a
not-counted row is a no-op.

`tclass_Count` itself is empty (`cpp/amc/count.cpp`); all
the work is done by the auto-generated `_n` and `_value`
fields plus the three tfuncs below.

### Ssim inputs
<a href="#ssim-inputs"></a>

Required:

- `dmmeta.field` with `reftype:Count`.  `arg:` is the element
  ctype.  By convention, field name starts with `cnt_`
  (`dmmeta.fprefix:cnt.Count`).

Optional:

- `dmmeta.xref` if you want the count maintained automatically
  via `XrefMaybe` on insertion / `Unref` on deletion.  Without
  an xref the count is manual.

No `dmmeta.count` record exists.  Count takes no parameters.

### Generated functions
<a href="#generated-functions"></a>

Source: `cpp/amc/count.cpp`.

| Tfunc           | Generated function                              | Effect |
|-----------------|-------------------------------------------------|--------|
| `Count.Insert`  | `void <name>_Insert(P&, T& row)`                | If `row.<name>_value == false`, set it true and increment `<name>_n`.  Idempotent. |
| `Count.Remove`  | `void <name>_Remove(P&, T& row)`                | Clear `<name>_value`; decrement `<name>_n` if it was set. |
| `Count.N`       | `u32 <name>_N(P&)`                              | Return `<name>_n`. |

### Memory model
<a href="#memory-model"></a>

- One `u32` in the parent, one `bool` per element row.  No
  pointers, no allocation.
- The per-row boolean costs a byte (or however the compiler
  packs it) but in exchange the operations are O(1) and
  exactly-once.

### Pitfalls
<a href="#pitfalls"></a>

- **No iteration.**  Count gives you the number, not the rows.
  If you need both, pair it with a [Ptrary](/txt/exe/amc/reftype/Ptrary.md)
  or [Llist](/txt/exe/amc/reftype/Llist.md).
- **One Count per (parent, element) pair.**  The per-row
  boolean is named after the count field, so multiple Counts
  on the same element type each get their own bool.
- **The bool defaults to false.**  As long as the element's
  `_Init` runs (i.e., row is allocated via a pool), Count is
  consistent.  Bypassing `_Init` can corrupt the count.

### See also
<a href="#see-also"></a>

- [Reftypes index](/txt/exe/amc/reftype.md)
- [Cross-references](/txt/exe/amc/xref.md) — auto-maintenance via xref
- [Ptrary](/txt/exe/amc/reftype/Ptrary.md) — when you want count + iteration
- Source: `cpp/amc/count.cpp`; field synthesis in `cpp/amc/gen.cpp` `gen_newfield_count`
- Tfunc records: `acr 'tfunc:Count.%'`

### Example
<a href="#example"></a>

```
dmmeta.field  field:abt.FDb.cnt_dirty  arg:abt.FTarget  reftype:Count
  dmmeta.xref   field:abt.FDb.cnt_dirty  inscond:"row.dirty"  via:...
```

Once the xref is wired, every target whose `dirty` flag is set
automatically counts itself; clearing the flag (via the xref
remove path) decrements.  `cnt_dirty_N()` is always live.

Manual usage:

```c++
cnt_dirty_Insert(_db, target);
cnt_dirty_Remove(_db, target);
prlog(cnt_dirty_N(_db));
```

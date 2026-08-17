## amc Reftype: Varlen
<a href="#amc-reftype-varlen"></a>

`Varlen` declares a **variable-length array of fixed-size
records appended past the end of a struct**.  The struct's
`sizeof` covers only the fixed portion; the Varlen elements
live in the bytes that follow.  This is the standard wire-
format pattern for length-prefixed messages: a fixed header
plus an array of fixed-size payload entries whose count is
derived from a length field.

```
dmmeta.ctype  ctype:prot.DataMsg
  dmmeta.field  field:prot.DataMsg.data  arg:u8  reftype:Varlen
```

A Varlen field must be the **last** field of its ctype.
Multiple Varlen fields are allowed (e.g., a message with two
trailing arrays); each one before the last gets an `_end`
offset field added.

### What it generates
<a href="#what-it-generates"></a>

Storage shape depends on whether the parent has a
`dmmeta.lenfld` (length field) and whether this Varlen is the
last one (see `tclass_Varlen` in `cpp/amc/varlen.cpp`):

- **Single trailing Varlen + parent's `lenfld`**: no extra
  storage.  `_N` derives the element count from
  `(lenfld_value - sizeof(parent)) / sizeof(T)`.
- **Multiple Varlens** (or none-have-lenfld): every Varlen
  except the last gets a `<name>_end` field carrying its byte
  offset relative to the end of the fixed portion.

Either way, no pointer is stored — the array's address is
just `(u8*)&parent + sizeof(parent) [+ prev_varlen_end]`.

amc requires:

- an element type with no destructor — a Varlen element is not
  visible to the parent's destructor, so anything it owned would
  never be released,
- the field is the last in the ctype (preceded by all
  non-Varlen fields),
- a varlen-capable pool basepool (Blkpool, Lpool, Malloc) —
  amc auto-creates one via `EnsureVarlenPool` if missing.

### Ssim inputs
<a href="#ssim-inputs"></a>

Required:

- `dmmeta.field` with `reftype:Varlen`.  `arg:` is the element
  type — frequently `u8` or `char` for raw bytes, or a small
  struct without destructor for typed trailing entries.

Optional:

- `dmmeta.lenfld` on a fixed field of the parent — names the
  field that holds the total message length.  Without it,
  `_N` becomes user-extern.

### Generated functions
<a href="#generated-functions"></a>

Source: `cpp/amc/varlen.cpp`.

| Tfunc                  | Generated function                              | Effect |
|------------------------|-------------------------------------------------|--------|
| `Varlen.Init`          | (folded)                                        | Set `<name>_end = 0` if applicable. |
| `Varlen.Addr`          | `T* <name>_Addr(P&)`                            | Return `(u8*)&parent + sizeof(parent) [+ prev_end]`. |
| `Varlen.Getary`        | `aryptr<T> <name>_Getary(P&)`                   | Span (`Addr`, `N`). |
| `Varlen.N`             | `u32 <name>_N(const P&)`                        | Element count.  Computed from `lenfld` (with adjustments for `_end` of any preceding Varlens) or user-extern when no `lenfld`. |
| `Varlen.Print`         | `void <name>_Print(P&, cstring& out)`           | For `char`/`u8`: print as string. |
| `Varlen.ReadStrptrMaybe`| `bool <name>_ReadStrptrMaybe(P&, strptr in)`   | Append from a span (typically used during ssim load). |
| `Varlen.curs`          | `<P>_<name>_curs` + accessors                   | Forward cursor over the trailing array. |

Allocation of the parent + trailing bytes is done via the
[Pool](/txt/exe/amc/reftype.md) family's `AllocVarlen` /
`AllocVarlenMaybe` tfuncs on the parent's pool field.

### Memory model
<a href="#memory-model"></a>

- The trailing bytes are part of the same allocation as the
  fixed header.  Use `AllocVarlen(extra_bytes)` to reserve
  room for them at construction time.
- Elements have no destructors — Varlen does not iterate to
  destruct, because the parent's `_Uninit` has no notion of
  the trailing array's element type.
- For length-derived `_N`, the lenfld is the source of truth.
  If lenfld is wrong, `_N` is wrong.

### Pitfalls
<a href="#pitfalls"></a>

- **Must be the last field** of the ctype.  amc errors with
  `amc.varlen_last` otherwise.
- **Cannot coexist with an Opt field.**  Both begin at the end
  of the fixed portion, so the generated writers would overlay
  one on the other; amc rejects the ctype with
  `amc.trail_overlay`.
- **Element type cannot have a destructor.**  amc rejects such an
  element with `amc.opt_dtor`.  Whether a type has a destructor
  follows from what it contains — an owning field, a Cleanup
  callback, a cascdel dependent, or an index it must unlink from.
- **No automatic resizing.**  The trailing region is sized at
  allocation; growing it means a fresh `AllocVarlen` and
  copying.
- **`_N` may be user-extern.**  Without a `lenfld` on the
  parent and without being the last Varlen, amc emits the
  prototype and expects you to provide the count function
  (cross-references: extern in non-amc-friendly messages).
- **Mixing fixed-size and varlen elements**: amc supports
  multiple Varlens but adds a `_end` offset per preceding one;
  watch sizeof carefully.

### See also
<a href="#see-also"></a>

- [Reftypes index](/txt/exe/amc/reftype.md)
- [Opt](/txt/exe/amc/reftype/Opt.md) — 0/1 trailing element variant
- [Varlen feature page](/txt/exe/amc/varlen.md) — protocol patterns
- [Protocols](/txt/exe/amc/proto.md) — typical use case
- Source: `cpp/amc/varlen.cpp`
- Tfunc records: `acr 'tfunc:Varlen.%'`

### Example
<a href="#example"></a>

A protobuf-style data message:

```
dmmeta.ctype  ctype:prot.DataMsg
  dmmeta.field  field:prot.DataMsg.length  arg:u8  reftype:Val
    dmmeta.lenfld  field:prot.DataMsg.length  extra:0
  dmmeta.field  field:prot.DataMsg.type    arg:u8  reftype:Val
    dmmeta.typefld field:prot.DataMsg.type
  dmmeta.field  field:prot.DataMsg.data    arg:u8  reftype:Varlen
```

In C++:

```c++
// allocate header + 100 bytes of payload:
prot::DataMsg *msg = msg_AllocVarlen(_db, 100);
msg->length = sizeof(prot::DataMsg) + 100;
memcpy(data_Addr(*msg), payload, 100);

// iterate later:
ind_beg(prot::DataMsg_data_curs, byte, *msg) {
    process(byte);
} ind_end;
```

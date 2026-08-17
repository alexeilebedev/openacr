## amc Reftype: Base
<a href="#amc-reftype-base"></a>

`Base` implements **single inheritance** between ctypes —
without C++'s `: public Base` syntax.  When ctype `C` has a
field of `reftype:Base` pointing at `B`, amc copies every
field of `B` into `C`'s layout, in the same order, with the
same names.  The result is binary-compatible with `B` for the
prefix of its fields, which is exactly what wire protocols
need.

```
dmmeta.ctype  ctype:prot.HeartbeatMsg
  dmmeta.field  field:prot.HeartbeatMsg.base  arg:prot.MsgHeader  reftype:Base
```

After this, every field of `prot.MsgHeader` (`type`, `length`,
…) is also a field of `prot.HeartbeatMsg`, sitting at the same
offsets.  No C++ inheritance, no virtual-pointer overhead.

### What it generates
<a href="#what-it-generates"></a>

`tclass_Base` (`cpp/amc/base.cpp`) does nothing on its own —
the heavy lifting happens in earlier amc phases that copy the
base's fields into the derived ctype as plain `Val` fields
(see `amcdb.gen:newfield_cbase`).  Only four tfuncs are emitted by
the Base reftype itself:

- `CopyOut` and `CopyIn` — between the derived type and the
  base type, when the field topology supports a faithful copy.
- `Castbase` — a reference to the base portion of a derived
  row.
- `Castdown` — given a base, attempt the reverse cast based on
  the discriminant (`typefld`) and length field.

### Ssim inputs
<a href="#ssim-inputs"></a>

Required:

- `dmmeta.field` with `reftype:Base`.  `arg:` is the **base**
  ctype.  The field name is conventionally `base` but any name
  works.

Optional (for the casting tfuncs to fire):

- `dmmeta.msgtype` on the derived ctype — gives the
  discriminant value.
- `dmmeta.typefld` on the base ctype — names the discriminant
  field.
- `dmmeta.lenfld` on the base — names the length field (so
  `Castdown` can also validate that the buffer is big enough).

One column is not copied verbatim.  A base's `algo.Comment`
column is instantiated in the derived ctype as an
`algo.cstring`, so the derived record spends 24 bytes on the
comment and allocates nothing until one is set, where the
fixed-size string costs 152 bytes on every record.  Nothing
declares this; it applies to every base that carries a comment.

### Generated functions
<a href="#generated-functions"></a>

Source: `cpp/amc/base.cpp`.

| Tfunc            | Generated function                                          | When emitted | Effect |
|------------------|-------------------------------------------------------------|--------------|--------|
| `Base.CopyOut`   | `void <basename>_CopyOut(P& row, Base& out)`                | always       | Copies each field of the base out of the derived row.  Handles `Val`, `Tary`, `Regx`, computed fields, and fields the derived does not carry. |
| `Base.CopyIn`    | `void <basename>_CopyIn(P& row, Base& in)`                  | when derived is copyable | Reverse direction. |
| `Base.Castbase`  | `Base& Castbase(P& row)`                                    | when layouts agree (see `AllowCastbaseQ`) | `reinterpret_cast<Base&>(row)` — zero-cost. |
| `Base.Castdown`  | `Derived* <Derived>_Castdown(Base& hdr)`                    | when derived has `msgtype` + base has `typefld` | Return `&hdr` cast to derived only if `hdr.typefld == msgtype` (and the length check, if `lenfld` is present); otherwise NULL. |

The two cast operations are the wire-protocol bread and butter:
`Castdown` is the *type tag dispatch*, `Castbase` is the no-op
upcast.

### Memory model
<a href="#memory-model"></a>

- The base is **inlined** into the derived ctype.  There is no
  separate base record, no pointer, no allocation.  Sizeof
  derived = sizeof base + sizeof derived-only fields.
- Because the layout is shared, the derived's first
  `sizeof(base)` bytes are a valid base instance.  This is what
  `Castbase` exploits.
- A base that carries an `algo.Comment` column breaks the
  binary compatibility, because the derived holds that column
  as an `algo.cstring`.  `Castbase` is not emitted for such a
  base, and `CopyOut` constructs the base's fixed-size string
  from the derived's cstring explicitly.

### Pitfalls
<a href="#pitfalls"></a>

- **No virtual functions.**  amc deliberately avoids them; if
  you need polymorphism, generate a `dispatch` over the set of
  ctypes (see [Dispatches](/txt/exe/amc/dispatch.md)).
- **Field-strip breaks `Castbase`.**  `fbase` is exposed for
  rare cases where a derived type omits a base field; doing so
  prevents `Castbase` from being emitted because the layouts
  no longer align.
- **`Castdown` needs `msgtype` + `typefld`.**  Without those,
  amc only emits `CopyOut/CopyIn/Castbase` — no down-cast.
- **No chained `Base`** through more than one level is special-
  cased — amc supports it, but each link must be a `Base`
  field; multiple inheritance is not supported.

### See also
<a href="#see-also"></a>

- [Reftypes index](/txt/exe/amc/reftype.md)
- [Protocols](/txt/exe/amc/proto.md) — the original motivation
- [Inheritance](/txt/exe/amc/inheritance.md) — overview of inheritance-like patterns
- [Dispatches](/txt/exe/amc/dispatch.md) — polymorphism via tag-dispatch instead of vtables
- Source: `cpp/amc/base.cpp`
- Tfunc records: `acr 'tfunc:Base.%'`

### Example
<a href="#example"></a>

Two messages sharing a header:

```
dmmeta.ctype  ctype:prot.MsgHeader  comment:""
  dmmeta.field  field:prot.MsgHeader.type    arg:u8  reftype:Val
    dmmeta.typefld  field:prot.MsgHeader.type
  dmmeta.field  field:prot.MsgHeader.length  arg:u8  reftype:Val
    dmmeta.lenfld   field:prot.MsgHeader.length  extra:0

dmmeta.ctype  ctype:prot.HeartbeatMsg
  dmmeta.msgtype  ctype:prot.HeartbeatMsg  type:1
  dmmeta.field    field:prot.HeartbeatMsg.base  arg:prot.MsgHeader  reftype:Base
```

In C++:

```c++
prot::MsgHeader &hdr = ...;        // raw header from the wire
if (prot::HeartbeatMsg *m = HeartbeatMsg_Castdown(hdr)) {
    // m and &hdr alias the same bytes; m is just the right type
}

prot::HeartbeatMsg msg;
prot::MsgHeader &header = Castbase(msg); // upcast, zero cost
header.type = 1;
```

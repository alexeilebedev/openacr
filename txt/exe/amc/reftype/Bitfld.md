## amc Reftype: Bitfld
<a href="#amc-reftype-bitfld"></a>

`Bitfld` is a **sub-byte field carved out of another field's
storage**.  The field has no storage of its own; reads and
writes mask/shift through the host field
(`dmmeta.bitfld.srcfield`).  Use it to pack flags / small enums
into wire-format integers without duplicating the bit-twiddling
boilerplate in every protocol.

```
dmmeta.field   field:algo.Errcode.errgrp  arg:i32  reftype:Bitfld  dflt:""  comment:""
  dmmeta.bitfld  field:algo.Errcode.errgrp  name:errgrp  offset:24  width:8  srcfield:algo.Errcode.value
```

Above declares an 8-bit `errgrp` slot inside the 32-bit
`algo.Errcode.value` host field, starting at bit 24.

### What it generates
<a href="#what-it-generates"></a>

No new storage in the parent ctype — the field is virtual.
amc:

- Generates `_Get` / `_Set` for the bitfield that shift+mask
  through the source field's full storage path.  Uses an
  unsigned value of width `srcfield`-type for the mask
  arithmetic (`StripWrappers(srcfield)`).
- Generates an `_Init` that calls `_Set(parent, dflt)` *only*
  if a non-empty default is specified — otherwise the bitfield
  inherits whatever the source field's `_Init` left in those
  bits (typically zero).
- Plays nicely with `fbigend`: if the host is big-endian, the
  bit numbering refers to the host's **integer** value, not
  its in-memory byte layout.  amc inserts a comment noting
  this.

Maximum width: 64 bits (`vrfy` at line 36).

### Ssim inputs
<a href="#ssim-inputs"></a>

Required:

- `dmmeta.field` with `reftype:Bitfld`.  `arg:` is the
  bitfield's *value* type — typically a small integer (`u8`,
  `i32`) or an enum struct.
- `dmmeta.bitfld`:
  - `name` — name of the bitfield (matches the field name).
  - `offset` — bit position within the source field (0 = LSB).
  - `width` — number of bits.
  - `srcfield` — the host field this bitfield reads/writes.

The source field is usually a plain `Val` of unsigned integer
type, but `Bitfld` also handles structs that wrap one — amc
follows the `StripWrappers` chain to find the underlying
storage type.

### Generated functions
<a href="#generated-functions"></a>

Source: `cpp/amc/bitfld.cpp`.

| Tfunc          | Generated function                              | Effect |
|----------------|-------------------------------------------------|--------|
| `Bitfld.Init`  | (folded into parent's `_Init`)                  | Calls `_Set(parent, dflt)` when a default is specified; otherwise emits nothing. |
| `Bitfld.Get`   | `T <name>_Get(const P&)`                        | `return T((srcfldval >> offset) & mask);` |
| `Bitfld.Set`   | `void <name>_Set(P&, T rhs)`                    | `srcfldval = (srcfldval & ~(mask<<offset)) | ((rhs & mask) << offset);` |

For single-field struct values (e.g., a typed wrapper around a
`u8`), `_Set` automatically extracts the underlying scalar
before masking.

### Memory model
<a href="#memory-model"></a>

- Zero added storage.  All reads/writes go through the host
  field.
- The mask is computed at amc time (`(1 << width) - 1`), so the
  generated code is just one shift + one mask + one OR — the
  compiler can usually constant-fold the whole thing.
- Big-endian host fields: bit positions are interpreted on the
  *integer* value (after a host-byte-order swap), not on the
  in-memory bytes.

### Pitfalls
<a href="#pitfalls"></a>

- **Width capped at 64 bits.** amc aborts at code-gen time
  otherwise.
- **No overlap check at the amc level.** Two Bitflds can
  collide on the same host bits — amc accepts it silently.
  If you need overlap detection, run `acr -check` or eyeball
  the offsets.
- **`_Set` is a read-modify-write.** Not atomic.  For multi-
  threaded access, serialize or use atomics on the host field.
- **Defaults are applied after the host's default.** If the
  host field has its own non-zero default, that runs first; the
  Bitfld then overwrites its slice.  Order is the order amc
  emits field initializers in the parent's constructor.
- **`fbigend` source — bit positions are integer-relative.**
  Don't try to reason about bit positions in terms of wire
  byte order; reason about them in terms of the unswapped
  integer value.

### See also
<a href="#see-also"></a>

- [Reftypes index](/txt/exe/amc/reftype.md)
- [Bitsets](/txt/exe/amc/bitset.md) — for bit-vector indexing
- [Big-endian fields](/txt/exe/amc/bigendian.md) — interaction with `fbigend`
- [Protocols](/txt/exe/amc/proto.md) — typical usage
- Source: `cpp/amc/bitfld.cpp`
- Tfunc records: `acr 'tfunc:Bitfld.%'`
- Spec table: `acr 'dmmeta.bitfld:%'`

### Example
<a href="#example"></a>

Packing five flags into a single `u8`:

```
dmmeta.ctype  ctype:net.TcpFlags
  dmmeta.field    field:net.TcpFlags.bits  arg:u8  reftype:Val

  dmmeta.field    field:net.TcpFlags.fin   arg:bool  reftype:Bitfld
    dmmeta.bitfld  field:net.TcpFlags.fin   name:fin  offset:0  width:1  srcfield:net.TcpFlags.bits
  dmmeta.field    field:net.TcpFlags.syn   arg:bool  reftype:Bitfld
    dmmeta.bitfld  field:net.TcpFlags.syn   name:syn  offset:1  width:1  srcfield:net.TcpFlags.bits
  dmmeta.field    field:net.TcpFlags.rst   arg:bool  reftype:Bitfld
    dmmeta.bitfld  field:net.TcpFlags.rst   name:rst  offset:2  width:1  srcfield:net.TcpFlags.bits
  ...
```

In C++:

```c++
net::TcpFlags fl;
fin_Set(fl, true);
syn_Set(fl, true);
if (fin_Get(fl) && !rst_Get(fl)) { ... }
```

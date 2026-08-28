## amc Feature: pbuf codec (protobuf)
<a href="#amc-feature-pbuf-codec-protobuf-"></a>

Many systems X2 needs to talk to use Google Protobuf as their
wire format.  Rather than depend on `protoc` and the upstream
C++ library, amc reads protobuf field metadata from ssim and
generates a small, self-contained codec.

The ssim descriptions are produced from `.proto` files by an
out-of-band converter (`pb2ssim` or similar) and committed
alongside the regular schema.  Once they're in, the codec
regenerates whenever any ctype or field changes.

```ssim
dmmeta.cpbuf   ctype:pbtest.ScalarTypes  syntax:proto3

dmmeta.field   field:pbtest.ScalarTypes.int32_val  arg:i32  reftype:Val
  dmmeta.fpbuf  field:pbtest.ScalarTypes.int32_val  field_number:1  pb_type:int32  packed:N
```

### Table Of Contents
<a href="#table-of-contents"></a>
&nbsp;&nbsp;&bull;&nbsp;  [Ssim inputs](#ssim-inputs)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [pb_type & wire type](#pb_type-wire-type)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [proto2 vs proto3](#proto2-vs-proto3)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Repeated & packed fields](#repeated-packed-fields)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Decoding into a record that already holds data](#decoding-into-a-record-that-already-holds-data)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Schema checks](#schema-checks)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [oneof groups](#oneof-groups)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Generated functions](#generated-functions)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Pitfalls](#pitfalls)<br/>

### Ssim inputs
<a href="#ssim-inputs"></a>

**`dmmeta.cpbuf  ctype:<T>  syntax:proto2|proto3`** — opt the
ctype into protobuf codec generation.  `syntax` names a row of
[dmmeta.pbsyntax](/txt/ssimdb/dmmeta/README.md#dmmeta-pbsyntax), the table of
syntax variants, and that row's `implicit_presence` is what the
encoder reads.  A value that is not one of them is rejected —
by `acr -check` when the row goes through the database, and by
amc when it reads the schema out of a directory — rather than
selecting the explicit-presence emission by default.  Behavior
of default values and "presence" differs between the two
variants.

**`dmmeta.fpbuf  field:<full>  field_number:<n>  pb_type:<T>  packed:Y/N`**
— per-field record:

| Attribute      | Meaning |
|----------------|---------|
| `field_number` | The integer tag from the `.proto` file (after `=`).  Must be in 1..2^29-1, and outside 19000..19999, the span the protobuf format reserves for its own implementation. |
| `pb_type`      | Protobuf type name — see the table below. |
| `packed`       | For repeated scalar fields, use packed encoding.  Only a repeated field of a varint or fixed-width pb_type has a packed form; `packed:Y` anywhere else is rejected. |

**`dmmeta.fpbufcase  fpbufcase:<field>/<oneof>`** — membership of
`<field>` in the `oneof` group named `<oneof>`.  See "oneof groups"
below.

### pb_type & wire type
<a href="#pb_type-wire-type"></a>

Each `pb_type` names a row of
[amcdb.pbtype](/txt/ssimdb/amcdb/README.md).  That table binds each
protobuf type to its wire type and to the `lib_pb` encode/decode
routine the codec calls for it:

| pb_type                        | Wire form        | Notes |
|--------------------------------|------------------|-------|
| `int32`, `int64`, `uint32`, `uint64`, `bool`, `enum` | varint     | Standard zig-zag-less integer encoding. |
| `sint32`, `sint64`             | varint (zigzag)  | Better for small negative numbers. |
| `fixed32`, `sfixed32`, `float` | 32-bit fixed     | Little-endian 4 bytes. |
| `fixed64`, `sfixed64`, `double`| 64-bit fixed     | Little-endian 8 bytes. |
| `string`, `bytes`, `message`   | length-delimited | Length prefix (varint) + raw bytes. |

The pbtype row also carries the value expressions the routine is
called with, so each field's encode and decode call is fixed when
the codec is generated and no runtime polymorphism is involved.

### proto2 vs proto3
<a href="#proto2-vs-proto3"></a>

Set by `cpbuf.syntax`.  Differences amc honors:

- **proto2** has explicit "presence": whether a field was set is
  recorded apart from its value, which on the amc side is a
  presence mask (`dmmeta.pmaskfld`).  The syntax itself adds no
  default-value rule, so a proto2 ctype without a presence mask
  emits every scalar field whatever its value.
- **proto3** drops presence for scalar fields: default-valued
  scalars are not emitted (you can't distinguish "absent"
  from "set to default"), but message-typed fields and
  `repeated`s still encode emptiness as "no bytes."

Under proto3 the encoder guards each scalar field with the
`nondflt` expression of its pbtype row — the "should I emit this
field?" check.  A field carrying a presence mask
(`dmmeta.pmaskfld`) is guarded by its present bit instead, under
either syntax.

### Repeated & packed fields
<a href="#repeated-packed-fields"></a>

A `repeated` proto field maps to a `Tary` on the amc side;
`fpbuf.packed:Y` controls whether the wire encoding is the
packed form (one length prefix + all elements concatenated) or
the unpacked form (one tag per element).  proto3 defaults
`packed:true` for primitive repeated fields; proto2 defaults
`packed:false`.

A length-delimited element — `string`, `bytes`, `message` —
carries its own length, so the format defines no packed form
for it, and a singular field has nothing to pack.  `packed:Y`
in either position is rejected rather than ignored.

Decode accepts both forms whichever way `packed` is declared: a
sender that packs a field this schema declares unpacked, or the
reverse, still reads correctly.

### Decoding into a record that already holds data
<a href="#decoding-into-a-record-that-already-holds-data"></a>

A reader that pulls messages off a socket usually keeps one
record and decodes each arriving message into it, to avoid
allocating a record per message.  Suppose the message has a
singular `name` and a repeated `tag`, and two messages arrive:
the first with `name:"a"` and tags `1,2`, the second with
`name:"b"` and tag `3`.  After the second decode the record
holds `name:"b"` — the value from the second message — and tags
`1,2,3`, the tags of both.

The two fields behave differently because the decoder implements
the protobuf format's merge order rather than a replacement.
Merge order is what the format requires of a decoder: two
serialized messages concatenated must read as one message, and a
submessage field that appears twice must read as the two
submessages merged.  Under that rule a singular field takes the
value of its last occurrence and a repeated field accumulates
every occurrence, which is exactly what the reader above
observed.

So a decode never clears anything, and a caller that treats
successive messages as independent resets the record itself before
each decode.  The repeated fields have to be cleared, or one
record accumulates the tags of every message that arrived.  The
singular fields have to be reset too whenever a field the message
omits must not be read as a value that arrived: the decoder
assigns a singular field only for an occurrence the message
carries, so a second message without `name` leaves `"a"` in place,
and nothing distinguishes it from a `name` the sender did send.
A reader that acts on whatever the record holds after the decode
therefore resets the whole record; one that reads only the fields
its sender always sends can leave the singular fields alone.

### Schema checks
<a href="#schema-checks"></a>

The codec substitutes each field access straight into a `lib_pb`
call, so a field whose shape does not match its declared `pb_type`
would produce generated C++ that does not compile, or a wire
stream a conformant peer rejects.  amc checks every `fpbuf` row
up front:

- The field's ctype needs a `cpbuf` row of its own.  Without one
  no codec is generated at all and the `fpbuf` row is dead.
- That `cpbuf` row's `syntax` must name a `dmmeta.pbsyntax` row.
- `pb_type` must name an `amcdb.pbtype` row.
- `field_number` must lie in 1..2^29-1, the range the wire tag
  holds, must fall outside the reserved 19000..19999 span, and
  must be unique within the ctype.
- The reftype must be `Val`, `Tary`, or `Bitfld`: a singular
  field, a `repeated` one, and a bitfield read and written
  through its `Get`/`Set` accessors.  No other reftype has a wire
  mapping.
- A singular field must be one the decoder can store into.  A field
  whose value has no member of its own is written through the `Set`
  that amc generates for it, and three shapes have a `Get` and no
  `Set`: a `substr`, a `falias`, and a `cppfunc` with `set:N`.  Those
  are rejected; `fbigend`, `Bitfld` and a `cppfunc` with `set:Y` are
  accepted, as is a repeated field of any shape, whose elements the
  decoder appends rather than stores.
- The field's `arg` must be the C++ type the pbtype row's codec
  functions accept (`pbtype.argtype`).  `pb_type:message` instead
  requires an arg carrying its own `cpbuf` row, and `pb_type:enum`
  an arg with an fconst-carrying `i32 value` field.
- `packed:Y` requires a repeated field of a varint or fixed-width
  pb_type, the one shape the format defines a packed form for.

- A `oneof` group's members must all be members of one presence
  mask, the mechanism that makes them mutually exclusive.  See
  "oneof groups" below.

A cpbuf ctype is checked for one thing beyond its own fields: a
`Base` field whose arg carries wire fields is rejected, because
the codec walks the ctype's own fields and the inherited ones
would appear in neither the encoder nor the decoder.

Each violation prints an `amc.pbuf_%` line naming the field; the
run continues so that further schema errors are reported in the
same pass, and exits nonzero.

### oneof groups
<a href="#oneof-groups"></a>

A protobuf `oneof` is a union: at most one of its members may
be set.  amc represents this with `dmmeta.fpbufcase` records, one
per member, keyed by the member field and the group name:

```ssim
dmmeta.fpbufcase  fpbufcase:mymsg.MyMsg.text/payload
dmmeta.fpbufcase  fpbufcase:mymsg.MyMsg.blob/payload
```

The union is carried by the presence mask.  Storing into one member
sets that member's present bit and clears the bit of every other
member of the same group, and the encoder writes a member only when
its present bit is set — so at most one member's bytes reach the
wire, and decoding a member's tag makes it the one that is present.

Every member of a group therefore has to be a member of one presence
mask that covers the whole group; amc rejects a group whose members
have no mask, or are split across two.  Without the shared mask each
member falls back to the proto3 default-value guard, and a record
with two nonzero members writes both of them.

### Generated functions
<a href="#generated-functions"></a>

Source: `cpp/amc/pbuf.cpp`.

| Tfunc                  | Generated function                                    | Effect |
|------------------------|-------------------------------------------------------|--------|
| `Ctype.PbufEncode`     | `void <T>_PbufEncode(algo::ByteAry& buf, <T>& parent)` | Append `parent` to `buf` in protobuf wire format. |
| `Ctype.PbufDecode`     | `bool <T>_PbufDecode(algo::memptr& buf, <T>& parent)` | Read `parent` from `buf`, advancing it past the bytes consumed; returns false on truncation or malformed wire.  An unknown tag is skipped, not an error. |

Each pb-typed field is encoded with `(tag << 3) | wire_type`
+ the field bytes.  Decode reads tags one at a time and
dispatches on (`field_number`, `wire_type`).

### Pitfalls
<a href="#pitfalls"></a>

- **proto3 has no "absent" for scalars.**  A field with its
  default value (0, empty string, false) is indistinguishable
  from "not set" on the wire.  Use proto2 if presence
  matters.
- **One field number can arrive twice.**  The format permits it,
  which is what makes concatenating two encoded messages equal
  merging them.  Decode of a second occurrence follows the format's
  rule per shape: a repeated field appends the new elements, a scalar
  takes the later value, and a singular `message` field merges — the
  second occurrence's members land on top of what the first one set,
  and members it does not carry keep the value they had.
- **Unknown fields**: proto3 ignores them on decode by
  default (forward-compatible).  amc's codec follows this —
  unknown tags are skipped, not preserved.  Roundtripping
  through a different schema may lose them.
- **packed vs unpacked**: `packed` governs only what the
  encoder writes.  The decoder of a repeated primitive field
  accepts either form, so data written under the other setting
  still reads back.
- **A message does not inherit wire fields.**  The codec
  writes and reads the ctype's own fields, so a `Base` field
  contributes nothing to the wire form.  A cpbuf ctype whose
  base carries `fpbuf` fields is rejected; repeat the fields
  on the derived ctype, or make the base a `message`-typed
  field of its own.
- **Field numbers are stable IDs.**  Don't ever change them
  without coordination — every existing wire message has
  hardcoded tag bytes.  The format assigns field numbers from 1
  to 536870911.  A tag naming field number 0 fails the decode:
  field 0 has no case in the switch, so it would be skipped as
  an unknown field, and the two bytes `00 00` would then read as
  a successfully decoded all-default record.
- **Integer widths are interchangeable on the wire.**  Protobuf
  declares `int32`, `int64`, `uint32`, `uint64` and `bool`
  wire-compatible with one another, and `sint32` with `sint64`,
  with the effect of a C++ cast.  A peer that declares a field
  wider than we do writes up to ten bytes of varint for it.  A
  field value is read at that width and narrowed to what the
  field holds, so the wider encoding does not by itself fail the
  message and the field carries the truncated value.  A varint
  that runs past ten bytes does not decode, and neither does a
  length or a tag naming a value above 4294967295.
- **`enum` is varint.**  But amc's `fconst` enums are
  internally `i32`/`u32`; treat them as such on the wire
  side.
- **oneof clearing**: setting a different oneof member
  doesn't free the previous one's storage.  Use C++-side
  cleanup if the previous member owned resources.

### See also
<a href="#see-also"></a>

- [Kafka](/txt/exe/amc/kafka.md) — sibling codec (Kafka wire)
- [FAST](/txt/exe/amc/fast.md) — sibling codec (FAST)
- [Protocols](/txt/exe/amc/proto.md) — overview of amc-driven protocol codecs
- [Dispatch](/txt/exe/amc/dispatch.md) — at-the-router message switching
- Source: `cpp/amc/pbuf.cpp`
- Spec tables: `acr 'dmmeta.cpbuf:%'`, `acr 'dmmeta.fpbuf:%'`, `acr 'dmmeta.fpbufcase:%'`

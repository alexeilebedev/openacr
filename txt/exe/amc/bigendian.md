## amc Feature: Big-endian fields — fbigend
<a href="#amc-feature-big-endian-fields-fbigend"></a>

Wire formats nearly universally store integers in network
(big-endian) byte order, while CPUs work in host order.  amc's
**`fbigend`** annotation handles the translation in a way that
keeps the C++ code readable: you declare a field of `arg:u32`
and tag it with `dmmeta.fbigend`, and amc generates a `_Get` /
`_Set` pair that swaps bytes through `htobe*` / `betoh*`.  The
underlying storage is renamed to `<name>_be` so a stray direct
read of the raw bytes will fail to compile rather than silently
produce a swapped value.

### Table Of Contents
<a href="#table-of-contents"></a>
&nbsp;&nbsp;&bull;&nbsp;  [What it generates](#what-it-generates)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Ssim inputs](#ssim-inputs)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Width selection](#width-selection)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Generated functions](#generated-functions)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example](#example)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Interaction with Bitfld](#interaction-with-bitfld)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Pitfalls](#pitfalls)<br/>

### What it generates
<a href="#what-it-generates"></a>

A normal `Val` of integer type `T` becomes the C++ field
`<name>` of type `T` initialized from `dflt`.  Annotated with
`fbigend`, the same declaration generates:

- the storage field as `<name>_be`,
- an inline `<name>_Get(const P&) -> T` that calls
  `beNNtoh(parent.<name>_be)`,
- an inline `<name>_Set(P&, T rhs)` that calls
  `parent.<name>_be = htobeNN(rhs)`.

`NN` is the integer width in bits, and amc reads it from the
type's `dmmeta.csize` row.  The width is a property of the
type, not of how the type's name is spelled, so an alias whose
name ends in different digits than its size still gets the
swap macro that matches its bytes.

Reading or writing the value through the typed accessors
performs the swap at each access.

### Ssim inputs
<a href="#ssim-inputs"></a>

Required:

- `dmmeta.field` with `reftype:Val` and `arg:` one of the
  built-in integer types listed under Width selection.  A
  Bitfld carved out of such a field is covered too, but the
  `fbigend` row goes on the host integer rather than on the
  Bitfld.
- `dmmeta.fbigend  field:<full>` — the marker record.  No
  parameters; the presence of the row is the entire signal.

The default initializer (`dflt`) — if any — is automatically
byte-swapped at construction so that the literal you write
in ssim is the **logical** value, not the wire-order
representation.

### Width selection
<a href="#width-selection"></a>

amc reads the width from the type's `dmmeta.csize` row and
picks the byteswap macro that matches it:

| `arg`  | byte-swap macro |
|--------|-----------------|
| `u16` / `i16` | `htobe16` / `be16toh` |
| `u32` / `i32` | `htobe32` / `be32toh` |
| `u64` / `i64` | `htobe64` / `be64toh` |

Those six are the types `amcdb.bltin` marks `bigendok:Y`, and
they are the ones the schema accepts.  An `arg` outside the
set — `u8`, `i8`, `bool`, `char`, `float`, `double`, `u128`,
or a ctype of your own — is refused with
`amc.bigend_deadend`.  A type marked `bigendok` whose width is
neither 16, 32 nor 64 is refused with `amc.bigend_width`, and
one whose `dmmeta.csize` row is missing with
`amc.bigend_size`.  An `fbigend` row on a field whose reftype
is not `Val` is refused with `amc.bad_bigend`.

### Generated functions
<a href="#generated-functions"></a>

| Function                       | Body |
|--------------------------------|------|
| `inline T <name>_Get(const P&)` | `return beNNtoh(parent.<name>_be);` |
| `inline void <name>_Set(P&, T rhs)` | `parent.<name>_be = htobeNN(rhs);` |

`Init` is folded into the parent ctype's constructor with
`parent.<name>_be = htobeNN(dflt);`, so a freshly constructed
record already holds its default in wire order.

### Interaction with Bitfld
<a href="#interaction-with-bitfld"></a>

[Bitfld](/txt/exe/amc/reftype/Bitfld.md) over a `fbigend`
host integer is supported: the bit numbering refers to the
*integer* value, not the in-memory bytes.  amc emits a comment
noting this in the generated `Get`/`Set` so a casual reader is
not confused.

Practically: declare the host integer first with `fbigend`,
then declare bitflds against it.  The bit layout is host-
order-conceptual, the wire is big-endian, and amc inserts the
swap on every access.

### Pitfalls
<a href="#pitfalls"></a>

- **Direct field access is silently wrong.**  Writing
  `parent.seq_be = 1234` stores `1234` *as host bytes*, which
  is `0xD2040000` on the wire instead of `0x000004D2`.  The
  `_be` suffix is intentional: it makes the wrong code visible.
- **`fbigend` + `cppfunc` is rejected.**  A computed field has
  no storage to swap; amc fails with `amc.bigend_fldfunc`.
- **The type carries the width, not the name.**  `arg:` names
  a ctype, and the swap macro follows that ctype's declared
  size, so a four-byte alias called `Seq16` gets `be32toh`.
  An alias with no `amcdb.bltin` row does not reach the
  generator at all: it is refused with `amc.bigend_deadend`.
- **Padding still required.**  Wire compatibility needs both
  byte order *and* layout — declare `dmmeta.pack` on the ctype
  (or `nsx.pack:Y` for the whole namespace) so the C++ struct
  matches the wire format.
- **`dflt:` is in logical (host) units.**  If you want the
  initial wire bytes to be `0xDEADBEEF`, set `dflt:` to that
  hex value; amc swaps it for you on `_Init`.

### See also
<a href="#see-also"></a>

- [Val](/txt/exe/amc/reftype/Val.md) — the underlying field reftype
- [Bitfld](/txt/exe/amc/reftype/Bitfld.md) — packing into a big-endian host integer
- [Protocols](/txt/exe/amc/proto.md) — typical context for fbigend usage
- [csize](/txt/exe/amc/csize.md) — pin layout assumptions
- Source: `cpp/amc/val.cpp` (`tfunc_Val_Get`, `tfunc_Val_Set`, `tfunc_Val_Init`)
- Spec table: `acr 'dmmeta.fbigend:%'`

### Example
<a href="#example"></a>

A TCP header excerpt with two big-endian fields:

```ssim
dmmeta.ctype     ctype:net.TcpHdr
dmmeta.pack      ctype:net.TcpHdr

dmmeta.field     field:net.TcpHdr.seq      arg:u32  reftype:Val  dflt:""
  dmmeta.fbigend  field:net.TcpHdr.seq

dmmeta.field     field:net.TcpHdr.ack      arg:u32  reftype:Val  dflt:""
  dmmeta.fbigend  field:net.TcpHdr.ack
```

Generated C++ (header):

```c++
#pragma pack(push,1)
struct TcpHdr {
    u32  seq_be;   // network order
    u32  ack_be;   // network order
    TcpHdr();
};
#pragma pack(pop)
```

…and inline accessors:

```c++
inline u32 net::seq_Get(const net::TcpHdr& parent) {
    return be32toh(parent.seq_be);
}
inline void net::seq_Set(net::TcpHdr& parent, u32 rhs) {
    parent.seq_be = htobe32(rhs);
}
```

Usage from C++:

```c++
net::TcpHdr hdr;
seq_Set(hdr, 1234);              // 1234 written as big-endian
u32 v = seq_Get(hdr);            // v == 1234

memcpy(&hdr, on_wire_bytes, sizeof(hdr));   // copy raw bytes
if (seq_Get(hdr) == expected) {  // swap is automatic
    ...
}
```

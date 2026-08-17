## amc Feature: Varlen trailing fields — Varlen / Opt / lenfld
<a href="#amc-feature-varlen-trailing-fields-varlen-opt-lenfld"></a>

A surprising number of wire formats end with "and then the
rest is a sequence of records of type X, length comes from
a header field."  amc has first-class support for this
pattern through three coordinated mechanisms:

| Concept     | Reftype                                        | Purpose                                  |
|-------------|------------------------------------------------|-------------------------------------------|
| **lenfld**  | (`dmmeta.lenfld` on a Val field)              | Names the field carrying total length    |
| **Varlen**  | [`reftype:Varlen`](/txt/exe/amc/reftype/Varlen.md) | 0..N trailing records                  |
| **Opt**     | [`reftype:Opt`](/txt/exe/amc/reftype/Opt.md) | Exactly 0 or 1 trailing record           |

Plus a fourth, **pnew** (`dmmeta.pnew`), which generates
placement-new constructors that allocate the parent + the
trailing region as a single contiguous block — the canonical
"construct a wire message in place" idiom.

This page covers the four together.

### Table Of Contents
<a href="#table-of-contents"></a>
&nbsp;&nbsp;&bull;&nbsp;  [The pattern](#the-pattern)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [lenfld — the length field](#lenfld-the-length-field)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Varlen — 0..N trailing](#varlen-0-n-trailing)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Opt — 0 or 1 trailing](#opt-0-or-1-trailing)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [pnew — placement-new constructors](#pnew-placement-new-constructors)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Multiple varlen fields](#multiple-varlen-fields)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Dispatch interaction](#dispatch-interaction)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Pitfalls](#pitfalls)<br/>

### The pattern
<a href="#the-pattern"></a>

The shape of a length-delimited message:

```
+----------------+-----------------+--------+--------+--------+
|    Header      |  fixed fields   |  rec_0 |  rec_1 | …      |
|  (length=N)    |                 |                          |
+----------------+-----------------+--------+--------+--------+
^ start
^ sizeof(parent) → here
                                   ^ Varlen.<name>_Addr(*msg)
^ length bytes total (length field's value, possibly scaled)
```

The lenfld lives in the header.  Everything after `sizeof(parent)`
up to `length` bytes is the Varlen (or Opt) tail.  amc
generates the offset arithmetic and access functions; you
write only `lenfld_Set(msg, total_len)` and the access goes
through `<varlen>_Getary` / `<opt>_Get`.

### lenfld — the length field
<a href="#lenfld-the-length-field"></a>

`dmmeta.lenfld  field:<full>  extra:<N>  scale:<S>  ctype:<T>`
declares a Val field on a ctype as the **length field**.  The
total message length in bytes is computed as:

```
total_bytes = (field.value * scale) + extra
```

| Attribute | Default | Meaning |
|-----------|---------|---------|
| `extra`   | 0       | Added to the field value.  Use to offset by sizeof(header) when the wire field counts trailing bytes only. |
| `scale`   | 1       | Multiplier.  Use when the wire field counts in 32-bit words (`scale:4`) etc. |
| `ctype`   | (auto)  | Usually inferred from the field's parent ctype. |

Without a lenfld, `Varlen._N` cannot derive the element
count and becomes user-extern; `Opt._Get` cannot decide
presence.  Always declare a lenfld for length-delimited
messages.

### Varlen — 0..N trailing
<a href="#varlen-0-n-trailing"></a>

See [Varlen reftype](/txt/exe/amc/reftype/Varlen.md) for the
field-level details.  Key points:

- Field must be the **last** of the ctype.
- `arg:` is the trailing element type.  `arg:u8` for raw
  bytes; a struct (without dtor) for typed records.
- `_N` = `(lenfld_value - sizeof(parent)) / sizeof(elem)`.
- `_Addr` = `(u8*)&parent + sizeof(parent)`.
- `_Getary` returns `aryptr<elem>(_Addr, _N)`.

### Opt — 0 or 1 trailing
<a href="#opt-0-or-1-trailing"></a>

See [Opt reftype](/txt/exe/amc/reftype/Opt.md).  Key
points:

- Field must be the **last** of the ctype.
- `_Get` returns `arg* | NULL`, presence derived from
  whether `lenfld_value >= sizeof(parent) + sizeof(arg)`.
- The optional element may itself have a `lenfld` —
  `_Get` then also validates that the optional's declared
  length fits inside the parent's remaining bytes.

### pnew — placement-new constructors
<a href="#pnew-placement-new-constructors"></a>

`dmmeta.pnew  pnew:<ns>/<ctype>.<buftype>  ns:<ns>
ctype:<ctype>  buftype:<kind>` generates a typed constructor
that:

1. Allocates `sizeof(parent) + sum(varlen-and-opt-lengths)`
   bytes in the chosen buffer kind.
2. Placement-news the parent struct into the buffer.
3. Initializes fields (including the lenfld with the total
   bytes, and any Varlen/Opt tail copied from arguments).
4. Returns a typed pointer.

The **buftype** decides where the allocation lives.  The
catalogue in `cpp/amc/pnew.cpp`:

| `buftype`  | Generated arg                                | Where the bytes go |
|------------|----------------------------------------------|---------------------|
| `Memptr`   | `algo::memptr &buf`                          | A pre-allocated byte span.  Advances the span past the constructed message. |
| `ByteAry`  | `algo::ByteAry &buf`                         | A `Tary<u8>`-style growable buffer; flushes existing contents first. |
| `Append`   | `algo::ByteAry &buf`                         | Same as ByteAry but appends rather than clearing. |
| `Alloc`    | `const algo::Alloc &alloc`                   | A typed allocator handle ([Tary GetAlloc](/txt/exe/amc/reftype/Tary.md), [Fbuf GetAlloc](/txt/exe/amc/reftype/Fbuf.md), …). |
| `Shm`      | `lib_ams::FShm &shm`                         | Shared-memory ring (used by lib_ams's hot path). |

Use `pnew` when you want to **build a wire message in one
shot** without separate allocate-and-fill steps.  The Alloc
variant pairs naturally with `Fbuf_GetAlloc` for streaming
encoders.

```c++
// Pnew variant: write straight into an output Fbuf
out_buf_Reserve(conn, 256);
algo::Alloc alloc = out_buf_GetAllocReserve(conn);
prot::HeartbeatMsg *msg = prot::HeartbeatMsg_Alloc(alloc, /* fields */);
// The fbuf now holds a complete wire message.
```

### Multiple varlen fields
<a href="#multiple-varlen-fields"></a>

A ctype may have **multiple Varlen fields** but all must be
contiguous and the last fields of the struct.  amc adds an
`<name>_end` offset field for each Varlen except the last,
recording the byte offset of the end of that field's region.
`_Addr` for each subsequent Varlen uses the previous one's
`_end`:

```
sizeof(parent) ──→ Varlen A ──→ A_end ──→ Varlen B ──→ B_end ──→ Varlen C
```

The lenfld covers the total — header + all varlens.

### Dispatch interaction
<a href="#dispatch-interaction"></a>

[Dispatch](/txt/exe/amc/dispatch.md) consumers of a Varlen-
bearing message header use the auto-generated `_curs` (see
[Dispatch / Message cursors](/txt/exe/amc/dispatch.md#message-cursors))
to walk byte buffers message-by-message.  Each iteration:

1. Reads the lenfld from the header.
2. Validates the bytes fit.
3. Hands a typed `MsgHeader*` to the dispatch.
4. Advances by `length` bytes.

The cursor + dispatch + pnew together cover the wire-format
read/write loop:

```c++
// reader:
ind_beg(prot::MsgHeader_curs, msg, recv_bytes) {
    InDispatch(*msg, msg->length);
} ind_end;

// writer:
auto alloc = out_buf_GetAllocReserve(conn);
prot::HeartbeatMsg_Alloc(alloc, /* fields */);
```

### Pitfalls
<a href="#pitfalls"></a>

- **Varlen must be the last field** — amc errors with
  `amc.varlen_last` otherwise.
- **Varlen and Opt are mutually exclusive** on one ctype --
  both begin at the end of the fixed portion; amc rejects the
  combination with `amc.trail_overlay`.
- **Element type must not have a destructor.**  The parent's
  `_Uninit` has no notion of the trailing region's typed
  content.
- **lenfld is the source of truth.**  Wrong lenfld → wrong
  count.  pnew sets lenfld correctly for you; manual
  construction must too.
- **`scale` rounds at the boundary.**  Wire formats that
  count in 32-bit words must use `scale:4`.  A misconfigured
  scale gives nonsense `_N` values.
- **pnew Memptr advances the buffer.**  Subsequent reads
  start at the new position; useful for stream encoders,
  surprising if you wanted to leave the cursor where it was.
- **pnew Shm requires `dmmeta.pack`.**  Pnew sets
  `req_pack=true` on the ctype so amc enforces the wire
  layout.

### See also
<a href="#see-also"></a>

- [Varlen reftype](/txt/exe/amc/reftype/Varlen.md)
- [Opt reftype](/txt/exe/amc/reftype/Opt.md)
- [Dispatch / message cursors](/txt/exe/amc/dispatch.md#message-cursors)
- [Protocols](/txt/exe/amc/proto.md)
- [Fbuf](/txt/exe/amc/fbuf.md) — Fbuf_GetAlloc for streaming-pnew encoding
- Source: `cpp/amc/varlen.cpp`, `cpp/amc/opt.cpp`, `cpp/amc/pnew.cpp`
- Spec tables: `acr 'dmmeta.lenfld:%'`, `acr 'dmmeta.pnew:%'`

## amc Feature: FAST presence codec
<a href="#amc-feature-fast-presence-codec"></a>

[FAST](https://en.wikipedia.org/wiki/FAST_protocol) is a
financial-industry wire format that encodes messages
extremely densely using:

- a per-message **presence map** (a bitset over the fields,
  serialized as stop-byte 7-bit groups), and
- a per-field **operator** (`none`, `constant`, `copy`,
  `default`, `delta`, `increment`, `tail`) that says how this
  field's value is derived from the previous message and the
  current bytes.

amc's FAST support is fully metadata-driven: declare the
templates and field instructions in ssim, and amc emits the
encoder, decoder, and a FIX-encoder fallback.  No hand-written
codec.

```
dmmeta.ctype     ctype:emdi12.Beacon
  dmmeta.cfast   ctype:emdi12.Beacon  id:109  encoding:template  reset:N

  dmmeta.field   field:emdi12.Beacon.MsgType  arg:algo.RspaceStr5  reftype:Val
    dmmeta.ffast  ffast:emdi12.Beacon.MsgType.field
                  name:MsgType id:35 pos:1 optional:N
                  encoding:string op:constant value:'"0"'
```

This page covers FAST specifically; for general wire-format
overview see [Protocols](/txt/exe/amc/proto.md).

### Table Of Contents
<a href="#table-of-contents"></a>
&nbsp;&nbsp;&bull;&nbsp;  [Ssim inputs](#ssim-inputs)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Encodings](#encodings)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Operators](#operators)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Generated functions](#generated-functions)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Presence map sizing](#presence-map-sizing)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Pitfalls](#pitfalls)<br/>

### Ssim inputs
<a href="#ssim-inputs"></a>

Two tables drive the codec, and a ctype carrying a
`dmmeta.cfast` row is what opts into codec generation:

- **`dmmeta.cfast`** on the ctype:
  - `id` — FAST template ID (mandatory for `encoding:template`).
  - `encoding` — one of `template`, `sequence`, `group`,
    `type` (enum/set), `string`, `unsigned`, `signed`,
    `scaled`, `byteVector`.
  - `reset:Y/N` — reset dictionary state at this message
    boundary.

- **`dmmeta.ffast`** on each field, one per instruction:
  - `pos` — field position within the template (1-based).
  - `id` — FAST tag id.
  - `name` — FAST tag name.
  - `optional:Y/N` — whether the field is presence-mapped.
  - `encoding` — one of the same set as cfast.
  - `op` — one of `none`/`constant`/`copy`/`default`/`delta`/
    `increment`/`tail`.
  - `value` — operator parameter (for `constant`, `default`,
    initial value of `copy`).
  - `fastinstr` — `field`, `length`, or `mantissa` (for
    multi-component encodings).

### Encodings
<a href="#encodings"></a>

| `encoding`   | Wire form                                                                 |
|--------------|----------------------------------------------------------------------------|
| `unsigned`   | Stop-byte 7-bit groups, big-endian; high bit of last byte set.            |
| `signed`     | Same, with twos-complement sign in the first group's high data bit.       |
| `string`     | 7-bit ASCII characters, each with stop bit; last byte's high bit set.     |
| `byteVector` | Length (unsigned) + raw bytes.                                            |
| `scaled`     | Two integers: exponent + mantissa (each unsigned/signed).                 |
| `sequence`   | Length + repeated body — generates a Tary/Varlen on decode.               |
| `group`      | Embedded sub-message; either inline or in a presence-mapped segment.      |
| `template`   | The outermost message-type wrapper.                                       |
| `type`       | Enum / set scalar.                                                        |

### Operators
<a href="#operators"></a>

| `op`         | Encoding behavior                                                          |
|--------------|----------------------------------------------------------------------------|
| `none`       | Field is sent every message (presence-mapped only if `optional:Y`).        |
| `constant`   | Value is fixed at `ffast.value` — only the presence bit is sent.           |
| `default`    | If the value equals `value`, omit it on the wire; otherwise send.          |
| `copy`       | Default to the previous message's value of the same field.                 |
| `delta`      | Send the difference from the previous message's value.                     |
| `increment`  | Implicit `prev + 1`; omit if matches.                                       |
| `tail`       | (Strings) Replace only the tail bytes that differ from the previous value. |

The operator determines whether the field consumes a presence
bit (`copy`/`default`/`tail`/`increment`/`delta`) or not
(`constant`/`none`).  amc computes the per-template presence-
map size at code-gen time and uses it to scope every per-
field branch.

### Generated functions
<a href="#generated-functions"></a>

Source: `cpp/amc/fast.cpp`.

| Tfunc                    | Generated function                                          | Effect |
|--------------------------|-------------------------------------------------------------|--------|
| `Ctype.FastEncode`       | `void <Msg>_FastEncode(<Msg>& msg, cstring& out)`           | Write the message in FAST wire form, respecting per-field operators and presence map. |
| `Ctype.FastDecode`       | `bool <Msg>_FastDecode(<Msg>& msg, strptr& in)`             | Inverse; advances `in` past the consumed bytes.  Returns false on truncation or schema mismatch. |
| `Ctype.FixEncode`        | `void <Msg>_FixEncode(<Msg>& msg, cstring& out)`            | FIX-tag fallback encoder for the same metadata — pairs FAST with a textual reference encoder for testing. |
| `gen_fast_presence` (phase) | (no user function — pre-pass)                            | Auto-creates a presence-bit set for any ctype with `optional:Y` ffast entries.  Runs in `amcdb.gen:fast_presence`. |
| `gen_ns_fast` (phase)    | `void <ns>_FastDecodeMsgs(...)`, `<ns>_FastEncodeMsgs(...)` | Top-level dispatch over an `Inlary` of messages; switches on template id. |

### Presence map sizing
<a href="#presence-map-sizing"></a>

amc's `PmapBitsN` (`cpp/amc/fast.cpp`) computes the number
of presence bits a template requires:

- One bit per field with an operator in
  `{copy, default, increment, delta, tail}` plus the message-
  type/template-id bit.
- `optional:Y` fields with `op:none` add a bit (present-or-
  not).
- `op:constant` fields with `optional:Y` add the bit too.

The pmap is serialized as 7-bit stop-byte groups starting just
after the template id.  amc reserves a buffer big enough for
the max pmap, writes the actual bits, and trims trailing zeros
on encode.

### Pitfalls
<a href="#pitfalls"></a>

- **Operators must match between peers.**  A `copy` on the
  sender and `none` on the receiver desynchronizes the
  dictionary — the next message becomes garbage.  Use
  [dispsig](/txt/exe/amc/dispatch.md#signatures-dispsig-dispsigcheck)
  to catch this at handshake.
- **`reset:Y` matters for stateful operators.**  `copy`,
  `delta`, `tail`, `increment` all carry state across
  messages; `reset:Y` zeroes that state.  Forgetting it on
  the first message of a snapshot is a classic FAST bug.
- **`scaled` encodes mantissa + exponent separately.**  Each
  needs its own ffast (with `fastinstr:mantissa` vs
  `field`).
- **`sequence` and `group` generate sub-codecs.**  The body
  of the sequence is its own template; declare its ffasts
  exactly as you would for a standalone message.
- **`Ctype.FastEncode` requires `dmmeta.cfast`** even if the
  template has no presence-mapped fields.  Without it amc
  doesn't generate a codec.
- **The FIX encoder is reference-only.**  Use it to dump a
  FAST decode in human-readable form (FIX `tag=value`
  pairs) — not for production interop.

### See also
<a href="#see-also"></a>

- [Protocols](/txt/exe/amc/proto.md)
- [Dispatches](/txt/exe/amc/dispatch.md) — message-type switch
- [Bitfld](/txt/exe/amc/reftype/Bitfld.md) — for in-message bit-packing
- [csize](/txt/exe/amc/csize.md) — pinning the on-wire layout
- Source: `cpp/amc/fast.cpp`
- Spec tables: `acr 'dmmeta.cfast:%'`, `acr 'dmmeta.ffast:%'`, `acr 'fastenc:%'`, `acr 'fastop:%'`, `acr 'fastinstr:%'`

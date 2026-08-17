## amc Feature: Kafka codec
<a href="#amc-feature-kafka-codec"></a>

The Kafka wire protocol is large (hundreds of message types,
five major encoding revisions over the years).  Maintaining
a hand-written codec is expensive; instead, amc takes
metadata generated from the Kafka project's JSON schema
files (one per request/response message), converts it to
ssim, and **generates the whole encoder/decoder**.

Two ssim records drive the codec:

- **`dmmeta.ckafka`** on each ctype: message-kind, root
  message name, valid-version range, flexible-version range.
- **`dmmeta.fkafka`** on each field: which versions it
  appears in, which versions it can be null, which versions
  treat it as "tagged" (Kafka's optional-field encoding).

Once a Kafka spec drop is converted to ssim, amc generates
a complete typed codec for every message — no per-message
hand-coding.

```
dmmeta.ckafka  ctype:kafka2.AbortedTransaction1a  kind:field  root:FetchResponse
              valid_versions:4-17  flexible_versions:12+

dmmeta.fkafka  field:kafka2.AbortedTransaction1a.producer_id  versions:4+
              nullable_versions:""  tagged_versions:""  tag:0
```

### Table Of Contents
<a href="#table-of-contents"></a>
&nbsp;&nbsp;&bull;&nbsp;  [Why this is metadata-driven](#why-this-is-metadata-driven)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Ssim inputs](#ssim-inputs)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Version ranges](#version-ranges)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Flexible (compact) vs legacy](#flexible-compact-vs-legacy)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Tagged fields](#tagged-fields)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Generated functions](#generated-functions)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example](#example)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Pitfalls](#pitfalls)<br/>

### Why this is metadata-driven
<a href="#why-this-is-metadata-driven"></a>

Kafka has 80+ request/response types, each with several
versions, each version adding or changing fields.  The wire
format evolved from "every field every time" (legacy) to
"flexible mode with compact lengths and tagged fields"
(modern).  A hand-written codec would have hundreds of
near-identical `if (version >= N) { ... }` blocks.

amc instead derives the codec from the message schemas
themselves: every field's `versions` and `nullable_versions`
become version-range checks in the generated encode/decode;
`flexible_versions` controls whether compact length prefixes
or fixed-size ones are used; `tagged_versions` controls
whether the field participates in the tag block.

The result is several megabytes of generated codec in
`cpp/gen/kafka2_gen.cpp` that exactly matches the schema for
every version.  Updates to the schema regenerate it; no
manual editing.

### Ssim inputs
<a href="#ssim-inputs"></a>

**`dmmeta.ckafka`** — per ctype:

| Attribute            | Meaning |
|----------------------|---------|
| `kind`               | KafkaTypeKind — `field`, `field_array`, `messageType`, etc. |
| `root`               | The outermost request or response message containing this type. |
| `valid_versions`     | Range of Kafka protocol versions in which this ctype appears (e.g., `4-17`). |
| `flexible_versions`  | Range of versions using compact (varint) encoding (e.g., `12+`). |

**`dmmeta.fkafka`** — per field:

| Attribute            | Meaning |
|----------------------|---------|
| `versions`           | Which Kafka versions include this field.  Outside the range, the codec skips it. |
| `nullable_versions`  | Versions in which the field may be null/absent. |
| `tagged_versions`    | Versions in which the field is encoded as a tagged optional rather than positionally. |
| `tag`                | The numeric tag id (when tagged). |

### Version ranges
<a href="#version-ranges"></a>

Versions are encoded as strings:

| String  | Meaning                              |
|---------|--------------------------------------|
| `0`     | Only version 0                       |
| `4`     | Only version 4                       |
| `4-17`  | Versions 4 through 17 inclusive     |
| `0+`    | Version 0 and later                  |
| `12+`   | Version 12 and later                 |
| `""`    | Always present (no version gating)   |

amc's `FirstVer` / `LastVer` helpers in `cpp/amc/kafka.cpp`
parse these and emit per-version branches in the encode and
decode paths.

### Flexible (compact) vs legacy
<a href="#flexible-compact-vs-legacy"></a>

Kafka's "flexible mode" switches three things:

1. **Length prefixes use varints (unsigned).**  A 5-element
   array uses 1 byte (`6`, off-by-one for null) instead of
   `INT32` (4 bytes).
2. **Strings get a compact varint length prefix.**
3. **Each record ends with a tag-block** for optional fields
   (see below).

amc reads `ckafka.flexible_versions` to pick between the two
encodings.  The same struct is encoded one way for
`version < first_flexible` and the other way for
`version >= first_flexible`.  No hand-written switch.

### Tagged fields
<a href="#tagged-fields"></a>

Flexible mode allows fields to be **optional** with a
self-describing tag.  Each tagged field is encoded as
`(tag_id, length, bytes)` and inserted into a per-record
"tag block" at the end of the message.  amc:

- omits the field from the positional encoding when the
  current version is in `tagged_versions`,
- emits the field into the tag block instead,
- on decode, reads the tag block, looks up each tag, and
  populates the struct accordingly.

Tagged encoding is how Kafka adds backwards-compatible
fields to a stable schema.

### Generated functions
<a href="#generated-functions"></a>

Source: `cpp/amc/kafka.cpp`.

| Tfunc                  | Generated function                                                  | Effect |
|------------------------|---------------------------------------------------------------------|--------|
| `Ctype.KafkaEncode`    | `void <Msg>_KafkaEncode(<Msg>& msg, i16 version, cstring& out)`     | Serialize `msg` for the requested protocol version. |
| `Ctype.KafkaDecode`    | `bool <Msg>_KafkaDecode(<Msg>& msg, i16 version, memptr in)`        | Deserialize bytes into `msg`; returns false on schema/truncation error. |

For dispatched messages (Kafka request/response root types),
amc also generates a top-level switch over the API key:

```c++
bool KafkaRequest_Decode(KafkaRequest& req, i16 api_key, i16 version, memptr buf);
```

driven by [Dispatch](/txt/exe/amc/dispatch.md) over the set of
request types — same machinery as any other amc dispatch.

### Pitfalls
<a href="#pitfalls"></a>

- **Version mismatch silently truncates.**  Encoding a v17
  struct as v0 just omits the fields that were added later.
  The receiver doesn't see them — verify version
  negotiation in your protocol handshake.
- **Tagged fields are not order-stable.**  Two encodings of
  the same struct may interleave the tag block differently;
  use logical equality on the struct, not byte equality.
- **`nullable_versions` matters.**  A non-nullable field that
  is missing on the wire is a decode error.  A nullable one
  is silently set to its default.
- **Schema updates regenerate megabytes of code.**  Don't
  hand-edit the generated `kafka2_gen.cpp`.  The schema
  ssim files are the source of truth.
- **The codec uses [Dispatch](/txt/exe/amc/dispatch.md) under
  the hood.**  Signature mismatches between client and server
  builds produce confusing decode errors; coordinate
  releases.

### See also
<a href="#see-also"></a>

- [Dispatch](/txt/exe/amc/dispatch.md) — root request/response switch
- [pbuf](/txt/exe/amc/pbuf.md) — sibling codec (Protobuf)
- [FAST](/txt/exe/amc/fast.md) — sibling codec (FAST)
- [Protocols](/txt/exe/amc/proto.md) — broader wire-format infrastructure
- Source: `cpp/amc/kafka.cpp`, `cpp/amc/disp/kafka_encode.cpp`, `cpp/amc/disp/kafka_decode.cpp`
- Spec tables: `acr 'dmmeta.ckafka:%'`, `acr 'dmmeta.fkafka:%'`
- Generated codec: `cpp/gen/kafka2_gen.cpp`

### Example
<a href="#example"></a>

A truncated FetchResponse:

```
dmmeta.ckafka  ctype:kafka2.FetchResponse  kind:messageType  root:FetchResponse
              valid_versions:0-17  flexible_versions:12+

dmmeta.field   field:kafka2.FetchResponse.throttle_time_ms  arg:i32  reftype:Val
  dmmeta.fkafka  field:kafka2.FetchResponse.throttle_time_ms  versions:1+
                 nullable_versions:""  tagged_versions:""  tag:0

dmmeta.field   field:kafka2.FetchResponse.error_code  arg:i16  reftype:Val
  dmmeta.fkafka  field:kafka2.FetchResponse.error_code  versions:7+
                 nullable_versions:""  tagged_versions:""  tag:0

dmmeta.field   field:kafka2.FetchResponse.session_id  arg:i32  reftype:Val
  dmmeta.fkafka  field:kafka2.FetchResponse.session_id  versions:7+
                 nullable_versions:""  tagged_versions:""  tag:0
```

In C++:

```c++
kafka2::FetchResponse resp;
resp.session_id  = 42;
resp.error_code  = 0;
algo::cstring out;
kafka2::FetchResponse_KafkaEncode(resp, /*version=*/12, out);
// out now contains a Kafka-v12 FetchResponse byte stream:
// flexible mode, compact arrays, optional tagged trailer.
```

A v0 encode of the same struct would write only fields whose
`versions` includes `0` (most of them not in this example).

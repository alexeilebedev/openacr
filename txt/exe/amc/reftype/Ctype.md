## amc Reftype: Ctype
<a href="#amc-reftype-ctype"></a>

`Ctype` is **not a user-facing reftype** the way `Tary` or
`Llist` are.  No user-written `dmmeta.field` has
`reftype:Ctype`.  Instead, `Ctype` is an *infrastructure
tclass* that runs once for **every** struct amc emits — it
contributes the ctype's own `Init`, `Uninit`, `Hash`, `Cmp`,
`Eq`, `Print`, comparison operators, command-line glue, and
all the wire-format encoder/decoder hooks that the rest of
amc depends on.

You will see Ctype's tfuncs in every namespace's generated
header — they are the C++ surface of "this is what a ctype
looks like".

### What it generates
<a href="#what-it-generates"></a>

`tclass_Ctype` (`cpp/amc/ctype.cpp`) itself is empty —
all of its work is done through the `Ctype.*` tfuncs below,
each of which gets invoked once per ctype amc is generating.
There is also a second pass, `Ctype2`, that runs after fields
are fully wired up (constructors, destructors, copy ctors).

Roughly the tfuncs fall into six groups:

- **Lifecycle** — `Init`, `Uninit`, plus the C++ `Ctor`,
  `Dtor`, `CopyCtor`, `AssignOp` emitted by Ctype2.
- **Comparison** — `Eq`, `Cmp`, `Lt` (+ `EqOp`, `NeOp`, `LtOp`,
  `GtOp`, `LeOp`, `GeOp` operator forms; `EqOpAryptr`).
- **Convenience reducers** — `Min`, `Max`, `Update`, `UpdateMin`,
  `UpdateMax` (only when `dmmeta.ccmp` requests them).
- **Print / Read** — `Print`, `Read`, `ToCmdline`, plus the
  `ReadFieldMaybe` dispatcher that reads tuple attributes.
- **Cross-reference glue** — `XrefMaybe` (insert into every
  index where this row should appear) and `Unref` (remove
  from all indexes when the row is deleted).
- **Wire formats** — `Hash`, `FastEncode/FastDecode`,
  `KafkaEncode/KafkaDecode`, `FixEncode`,
  `PbufEncode/PbufDecode`.  Only emitted when the ctype opts
  into the corresponding codec via `dmmeta.ccmp`,
  `dmmeta.ckafka`, `dmmeta.cpbuf`, `dmmeta.cfast`, etc.
- **Command-line / message glue** — `NArgs`, `GetAnon`,
  `GetMsgLength`, `GetMsgMemptr`, `EqEnum`.

### Ssim inputs
<a href="#ssim-inputs"></a>

Ctype is opted into per-ctype not by `dmmeta.field` but by:

- `dmmeta.ctype` — declares the struct itself.
- `dmmeta.cpptype` — asks for a fieldwise constructor.  Whether the
  ctype needs a destructor, and whether it is cheap enough to pass by
  value, amc computes from what the ctype contains.
- `dmmeta.ccmp` — declares whether comparison operators and
  reducers should be generated, with `order:Y/N` and
  `minmax:Y/N` switches.
- `dmmeta.chash` — declares the hash function used for `Thash`
  members.
- `dmmeta.cfmt` — declares which serialization formats (`String`,
  `Json`, `Tuple`, `Argv`) get conversion functions.
- `dmmeta.cfast`, `dmmeta.ckafka`, `dmmeta.cpbuf` — opt-in to
  the matching wire codec.

### Generated functions
<a href="#generated-functions"></a>

Source: `cpp/amc/ctype.cpp` and `cpp/amc/disp/` (for dispatches
that touch the type).

#### Lifecycle (pass 1: `Ctype`)
<a href="#lifecycle-pass-1-ctype-"></a>

| Tfunc            | Generated function                       | When emitted | Effect |
|------------------|------------------------------------------|--------------|--------|
| `Ctype.Init`     | `void <Name>_Init(<Type>& row)`          | when ctor needed | Initialize each field in declaration order. |
| `Ctype.Uninit`   | `void <Name>_Uninit(<Type>& row)`        | always        | Run `Cleanup` callbacks, `Cascdel` indexes, then per-field `Uninit` in **reverse** order. |
| `Ctype.XrefMaybe`| `bool <Name>_XrefMaybe(<Type>& row)`     | when ctype has xref destinations | Try to insert `row` into every index registered against it.  Returns false (and rolls back) if any insert fails. |
| `Ctype.Unref`    | `void <Name>_Unref(<Type>& row)`         | always        | Remove `row` from every index it currently sits in.  Called by `Delete`. |

#### Lifecycle (pass 2: `Ctype2`)
<a href="#lifecycle-pass-2-ctype2-"></a>

| Tfunc              | Generated function          | Effect |
|--------------------|------------------------------|--------|
| `Ctype2.Ctor`      | `<Type>::<Type>()`           | Calls `<Name>_Init(*this)`. |
| `Ctype2.Dtor`      | `<Type>::~<Type>()`          | Calls `<Name>_Uninit(*this)`. |
| `Ctype2.CopyCtor`  | `<Type>::<Type>(const <Type>&)` | Field-wise copy.  Emitted only when the ctype is safely copyable. |
| `Ctype2.AssignOp`  | `<Type>& operator=(...)`     | Field-wise assignment. |
| `Ctype2.FieldwiseCtor` | `<Type>::<Type>(field1, field2, ...)` | When `cpptype.ctor:Y`, a positional ctor. |
| `Ctype2.EnumCtor`  | `<Type>::<Type>(<ns>_<Name>Enum arg)` | Construct from the ctype's own enum type.  Emitted for a single-field ctype whose field carries `dmmeta.fconst`, which is what lets `algo::Bool(algo_Bool_Y)` compile. |

#### Comparison (`dmmeta.ccmp`)
<a href="#comparison-dmmeta-ccmp-"></a>

| Tfunc                | Emit predicate                  | Effect |
|----------------------|--------------------------------|--------|
| `Ctype.Eq` / `Ctype.EqOp` | `extrn:N`                   | Field-wise equality + `operator==`. |
| `Ctype.NeOp`         | with `EqOp`                    | `operator!=` complement. |
| `Ctype.Cmp` / `Ctype.Lt` | `order:Y`                   | Lexicographic 3-way and `<`. |
| `Ctype.LtOp/GtOp/LeOp/GeOp` | with `Lt` and `genop:Y`  | Operator forms. |
| `Ctype.Min` / `Ctype.Max` | `minmax:Y`                  | `<Type>_Min/Max(<Type> a, <Type> b)`. |
| `Ctype.Update`       | with `order:Y`                 | `Update(<Type>& a, <Type> b)` — set+return-changed. |
| `Ctype.UpdateMin` / `Ctype.UpdateMax` | `minmax:Y`        | Take the smaller / larger of two values. |
| `Ctype.EqOpAryptr`   | when ctype has a Tary of char/u8 | `operator==` against `algo::strptr`. |

#### Print / read (`dmmeta.cfmt`)
<a href="#print-read-dmmeta-cfmt-"></a>

| Tfunc                | Emit predicate                            | Effect |
|----------------------|-------------------------------------------|--------|
| `Ctype.Print`        | `cfmt.print:Y`                            | `void <Name>_Print(<Type>& row, cstring& out)`. |
| `Ctype.Read`         | `cfmt.read:Y`                             | `bool <Name>_ReadStrptrMaybe(<Type>& row, strptr in)`. |
| `Ctype.ReadFieldMaybe`| with `Read`, when ctype has fields       | Dispatcher invoked by `Read` for each tuple attribute. |
| `Ctype.ToCmdline`    | `cfmt:Argv`                                | Encode to a `cstring` as an argv-style command line. |
| `Ctype.NArgs`        | `cfmt:Argv` + has anonymous fields         | Count of mandatory positional args. |
| `Ctype.GetAnon`      | `cfmt:Argv` + has anonymous fields         | Accessor for the *i*-th positional field. |
| `Ctype.EqEnum`       | when ctype is an enum                      | Compare with `enum` constants. |

#### Wire codecs (opt-in)
<a href="#wire-codecs-opt-in-"></a>

| Tfunc                | Triggered by         | Effect |
|----------------------|----------------------|--------|
| `Ctype.Hash`         | `dmmeta.chash`       | `u32 <Name>_Hash(u32 prev, const <Type>&)` — the rehash entry point for `Thash`. |
| `Ctype.FastEncode` / `Ctype.FastDecode` | `dmmeta.cfast` | FAST presence codec. |
| `Ctype.FixEncode`    | `dmmeta.cfast` (FIX dialect) | FIX wire encoder. |
| `Ctype.KafkaEncode` / `Ctype.KafkaDecode` | `dmmeta.ckafka` | Kafka wire codec. |
| `Ctype.PbufEncode` / `Ctype.PbufDecode` | `dmmeta.cpbuf` | Protobuf wire codec. |

#### Message helpers (for `dmmeta.lenfld` ctypes)
<a href="#message-helpers-for-dmmeta-lenfld-ctypes-"></a>

| Tfunc                | Effect |
|----------------------|--------|
| `Ctype.GetMsgLength` | `u32 <Name>_GetMsgLength(const <Type>&)` — total length in bytes (uses the length field). |
| `Ctype.GetMsgMemptr` | `memptr <Name>_GetMsgMemptr(<Type>&)` — span covering the whole on-wire message. |

### Memory model
<a href="#memory-model"></a>

Ctype owns no storage — it is the *generator* that emits the
struct definition itself.  All actual fields come from the
field-level reftypes (`Val`, `Tary`, `Llist`, …).  Ctype's
job is to tie those together with a coherent lifecycle and
ABI.

### Pitfalls
<a href="#pitfalls"></a>

- **Not a user field reftype.**  You never write
  `reftype:Ctype` in a `dmmeta.field`.  If you find yourself
  trying to, you actually want [Val](/txt/exe/amc/reftype/Val.md)
  (for a value field) or [Base](/txt/exe/amc/reftype/Base.md)
  (for an inherited base).
- **Tfunc emission is opt-in.**  Most Ctype tfuncs are gated
  by a matching `c*` opt-in record.  If your struct lacks
  `operator==`, you forgot the `dmmeta.ccmp` (with `genop:Y`).
- **Uninit order matters.**  Fields are uninitialized in
  reverse order; if you depend on a destruction order, design
  the field order accordingly.
- **Two passes.**  Some emissions (the C++ ctor/dtor methods)
  happen in `Ctype2`, after every field's `_Init` has been
  fully built up.  If a tfunc fires "too early" and a field
  init isn't ready yet, that's the boundary.

### See also
<a href="#see-also"></a>

- [Reftypes index](/txt/exe/amc/reftype.md)
- [Backend / pipeline](/txt/exe/amc/backend/pipeline.md) — when Ctype runs in the gen table
- [Backend / tclass-tfunc](/txt/exe/amc/backend/tclass-tfunc.md) — the dispatch model
- [Strings](/txt/exe/amc/string.md) — cfmt-driven Print/Read paths
- [Protocols](/txt/exe/amc/proto.md) — Hash / msg-length helpers
- Source: `cpp/amc/ctype.cpp` (Ctype + Ctype2)
- Tfunc records: `acr 'tfunc:Ctype.%'  acr 'tfunc:Ctype2.%'`

### Example
<a href="#example"></a>

Most of what you see in `include/gen/<ns>_gen.h` for any
struct definition is Ctype output:

```c++
// --- algo.LineBuf
struct LineBuf {
    algo::cstring  buf;
    LineBuf();
    LineBuf(const LineBuf &rhs);
    ~LineBuf() __attribute__((nothrow));
    LineBuf& operator =(const LineBuf &rhs);
    bool operator ==(const LineBuf &rhs) const;
    bool operator <(const LineBuf &rhs) const;
};

// Ctype2.Ctor + Ctype.Init together:
inline algo::LineBuf::LineBuf() { LineBuf_Init(*this); }

// Ctype.Init body — calls every field's Init in order:
inline void algo::LineBuf_Init(algo::LineBuf& parent) {
    buf_Init(parent);
}
```

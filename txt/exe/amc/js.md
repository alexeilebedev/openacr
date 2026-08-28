## amc Feature: JavaScript / TypeScript emission
<a href="#amc-feature-javascript-typescript-emission"></a>

amc isn't C++-only.  Any namespace tagged with `dmmeta.nsjs`
gets a parallel TypeScript (or plain JavaScript) module
`ts/gen/<ns>_gen.ts`, with the same ctypes, fconsts,
enum unions, JSON interfaces, and serde helpers as the C++
side.  This is what lets the **X2 web UI** consume amc-defined
protocols without hand-maintaining a separate schema — the
TypeScript types are *the same types* the C++ engine knows
about.

```ssim
dmmeta.nsjs  ns:ams  typescript:Y  ifaceonly:N  comment:""
```

amc generates, per ns:

- A TypeScript module `ts/gen/<ns>_gen.ts` containing one
  declaration per ctype, plus encode / decode functions, an
  enum union for each fconst-bearing field, and metadata
  tables describing all fields for runtime introspection.
- (Optional) Plain JavaScript fallback if `typescript:N`.

The result is a fully-typed wire protocol on both sides of
the C++ ↔ browser boundary.

### Table Of Contents
<a href="#table-of-contents"></a>
&nbsp;&nbsp;&bull;&nbsp;  [Ssim inputs](#ssim-inputs)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [What gets emitted](#what-gets-emitted)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Built-in type mapping](#built-in-type-mapping)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Encode / decode](#encode-decode)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Which fields have a wire form](#which-fields-have-a-wire-form)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [API endpoint metadata](#api-endpoint-metadata)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example](#example)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Pitfalls](#pitfalls)<br/>

### Ssim inputs
<a href="#ssim-inputs"></a>

| Record         | Role |
|----------------|------|
| `dmmeta.nsjs  ns:<ns>  typescript:Y/N  ifaceonly:Y/N` | Turns on the JS backend for the namespace.  `ifaceonly:Y` emits the interfaces alone and no functions, so a namespace that wants codecs sets `ifaceonly:N`. |
| `dmmeta.jstype  ctype:<T>` | Mark this ctype as exportable to JS (most are by default; this opts out of skipping). |
| `dmmeta.cjsbltin  ctype:<T>  jsbltin:<B>` | Map a C++ built-in to a JS primitive: `u32 → number`, `Smallstr → string`, etc. |
| `dmmeta.jsbltin  jsbltin:<name>  cons:<ctor>  dflt:<expr>` | The catalog of JS primitives (TypeScript: `number`, `string`, `boolean`, `bigint`, `Uint8Array`, `Array`, `Map`, `object`, `undefined`). |
| `dmmeta.cjson  ctype:<T>  jsbltin:<custom>` | Customize the JSON representation of a ctype (e.g., emit a string instead of a struct). |

### What gets emitted
<a href="#what-gets-emitted"></a>

For each ctype in a `nsjs:Y` namespace, amc emits in
`ts/gen/<ns>_gen.ts`:

```typescript
// interface for the struct:
export interface MsgHeader {
    type: number;
    length: number;
}

// enum union for any fconst-bearing field:
export type MsgHeaderType =
    | 'Heartbeat'    // 1
    | 'Login'        // 2
    | 'Logout'       // 3
    ;

// JSON interface (when cfmt:Json is set on the ctype):
export interface MsgHeaderJson {
    type: number;
    length: number;
}

// encode / decode pair for wire formats:
export function MsgHeader_Encode(view: DataView, offset: number, parent: MsgHeader): number;
export function MsgHeader_Decode(view: DataView, offset: number): MsgHeader;

// field metadata (auto-generated runtime introspection):
export const MsgHeader_FieldMeta: FieldMeta[] = [
    { name: 'type',   type: 'number', size: 1 },
    { name: 'length', type: 'number', size: 1 },
];
```

For each namespace with REST endpoints, amc also emits
`ApiEndpoints` tables describing the URL → ctype mapping a
single-page app uses for typed `fetch` calls.

### Built-in type mapping
<a href="#built-in-type-mapping"></a>

The `dmmeta.cjsbltin` table maps each C++ built-in to a JS
primitive.  The default mapping:

| C++ type        | TypeScript type   | JS primitive (`jsbltin`) |
|-----------------|-------------------|--------------------------|
| `bool`          | `boolean`         | `boolean`                |
| `i8..i32`       | `number`          | `number`                 |
| `u8..u32`       | `number`          | `number`                 |
| `i64` / `u64`   | `bigint`          | `bigint`                 |
| `float` / `double` | `number`       | `number`                 |
| `algo.Smallstr*` | `string`         | `string`                 |
| `algo.Charset`  | `string` (set as chars) | `string`           |
| `algo.aryptr<u8>` | `Uint8Array`    | `Uint8Array`             |
| Tary of `T`     | `T[]`             | `Array`                  |
| Thash of `T`    | `Map<K, T>`       | `Map`                    |

`bigint` for 64-bit ints because JS numbers lose precision
beyond 2^53.  Tools that round-trip 64-bit values through
JSON must use `bigint` consistently.

`dmmeta.cjson` overrides this for JSON serialization
specifically — useful when the wire form differs from the
runtime form (e.g., a 64-bit value sent as a string in JSON
to survive `JSON.parse`).

### Encode / decode
<a href="#encode-decode"></a>

For ctypes that carry a `dmmeta.pack` row in a `typescript`
namespace, amc emits an encode/decode pair over a caller-supplied
`DataView`:

- `<Type>_Encode(view, offset, value)` → the frame's byte count,
  written at `offset` of `view`
- `<Type>_Decode(view, offset)` → the typed value read from that
  position

Neither direction emits a bound over the frame's fixed portion.
The fixed fields are written and read through `DataView`
accessors, and those raise a `RangeError` of their own when an
offset falls outside the view, so the fixed portion is bounded by
the view without amc emitting anything — but that error names
neither the ctype nor the field.

Encode emits two bounds of its own.  The first is over the frame's
length word.  A ctype whose frame length varies at runtime stores
that length in a word narrower than the byte count the encoder
computed, so a count the word cannot represent would be truncated
silently and the reader would frame the wrong number of bytes.
Encode therefore tests the count before storing it — against the
low end of the word's range, against its scale, and against the
high end — and amc emits only the terms whose failure is possible
for that word.  A ctype whose frame length is fixed is checked at
generation time instead, and gets no runtime test.

The second encode bound is over the tail.  A varlen or `Opt`
field's bytes are appended after the fixed portion, into a window
sized by the view's `byteLength`, so a tail longer than the window
that remains is refused rather than written past the view's end.
That is what makes a staging buffer holding several frames safe to
encode into.  Both encode refusals are a `RangeError` naming the
ctype: a frame-length refusal also names the computed length and
the bound it broke, a tail refusal names the field, and neither
names an offset.

Decode emits no bound.  It reads the frame's length word and takes
each tail as a window over the view's underlying `ArrayBuffer`,
sized from that word, so the tail is bounded by the buffer rather
than by the view.  A length word reaching past the frame's end
therefore yields a tail holding whatever bytes follow it in the
buffer, and nothing reports an error.  A caller that decodes
frames it did not encode has to bound the length word itself
before calling decode, which a reader whose view spans a whole
batch of received records has to do.

A frame longer than the fixed portion is not an error in either
direction: that is how a reader takes the prefix of a message a
newer sender extended.

These honor every per-field treatment: bitflds get
masked/shifted, fbigend fields get byte-swapped (via
`DataView`), `Smallstr` fields get trimmed/padded, fconst
fields map to their string aliases.

### Which fields have a wire form
<a href="#which-fields-have-a-wire-form"></a>

Consider a packed message whose second field is an inline
array of char, followed by a `u32` sequence number.  The
inline array of char is projected as a `string`, and a
string has no fixed-width byte form the decoder could read
out of the frame, so the decoder reads nothing for it — and,
reading nothing, it leaves its running position where the
array began.  The sequence number is then read from the
array's first four bytes, and every later field slides with
it.  The frame decodes, nothing reports an error, and the
values are wrong.

What makes that possible is a field admitted into the
layout with no wire form to give it.  The layout is one
derivation, shared by the encoder and the decoder, so a
field is placed identically in both or refused in both;
what amc must not do is admit a field one direction can
express and the other cannot.

So each field of a packed jstype ctype is classified once,
and a field that falls outside the table below is a schema
error, reported as `amc.jstype_wire` naming the field.  Every
offending field is named in one run, and a run that names any
of them writes no output at all, so a refused schema yields
no codec rather than a silently wrong one.

| reftype    | occupies                        | condition |
|------------|---------------------------------|-----------|
| `Base`     | nothing of its own              | the base's fields appear individually |
| `Bitfld`   | nothing of its own              | carried by the source word |
| `Val`      | the arg type's width            | the arg must carry no tail of its own |
| `Varlen`   | the frame's tail                | text tail for an arg of `char`, byte tail for an arg projecting as `Uint8Array` |
| `Opt`      | the frame's tail                | the arg must project as `Uint8Array` |
| `Inlary`   | `max` × the arg type's width    | the arg must project as `Uint8Array`, and the array must be fixed (`min` = `max`) |
| any other  | —                               | refused |

`Opt` and `Inlary` are byte copies in both directions, which
is why each requires a `Uint8Array` arg: an arg of `char`, or
of a `cstr` ctype such as `algo.Smallstr50`, projects as
`string` and has no byte-copy form.

`Varlen` has a text form as well as a byte form, so it takes
either projection — but the text form is what an arg of `char`
means.  An arg of a `cstr` ctype also projects as `string`,
while its tail on the wire is a run of that ctype's records
rather than encoded text, and neither form fits it.  So a
`Varlen` is accepted with an arg of `char` or with an arg
projecting as `Uint8Array`, and refused otherwise; the encoder
and the decoder then read the same projection and pick the
same form.

A `Val` field occupies the arg type's width, and that is the
whole of the field only while the arg type itself ends where
its fixed size ends.  An arg that declares a `Varlen` or an
`Opt` appends a tail past that point, and the nested encoder
returns a frame length the enclosing walk has no slot to
account for.  The enclosing walk would resume writing on top
of the nested tail, and the enclosing length word would
undercount the frame, so a `Val` of such a type is refused.
The shape it is reaching for is an `Opt`, whose bytes become
the enclosing frame's own tail.

An `Inlary` carries a second condition: the array has to be
fixed, meaning its `min` equals its `max`.  A variable inline
array stores a live element count after its elements, so it
occupies more than `max` elements' worth of bytes, and that
count has no member on the TypeScript side to carry it.
Placing such a field at `max` × the element width would leave
every later field four bytes low in the frame, so it is
refused instead.

The ctype as a whole carries one condition of its own.  A
`Varlen` or an `Opt` field's bytes are the frame's tail, and
the decoder learns where that tail ends from the frame's
length word, which lives in the ctype's length field.  A ctype
that declares a tail and no length field leaves the decoder
with no length to read, so it is refused as
`amc.jstype_lenfld` naming the ctype.

### API endpoint metadata
<a href="#api-endpoint-metadata"></a>

A namespace that declares REST API endpoints
(`dmmeta.apiendpoint` records pointing at request/response
ctype pairs) gets an `ApiEndpoints` table:

```typescript
export const ApiEndpoints: {[key: string]: ApiEndpointSpec} = {
    'GET /api/v1/users': {
        request:  null,
        response: 'UserList',
    },
    'POST /api/v1/users': {
        request:  'UserCreateReq',
        response: 'User',
    },
};
```

The X2 UI's fetch wrapper consumes this table for typed,
self-documenting REST calls — no hand-typing of URL strings
in the frontend.

### Pitfalls
<a href="#pitfalls"></a>

- **`i64` / `u64` become `bigint`.**  JS code that mixes
  numbers and bigints will throw `TypeError`.  Convert
  explicitly with `Number()` / `BigInt()` at the boundary.
- **JSON cannot natively round-trip bigints.**  amc emits
  them as strings; the decoder converts back.  Don't use
  `JSON.parse` directly on these payloads — go through
  `<Type>_Decode`.
- **Browsers don't have `Uint8Array.equals`.**  Comparison
  is opt-in via `algo`-style helpers if needed.
- **Namespace must opt in.**  No nsjs row, no JS output.
  This is intentional — most amc namespaces are
  server-internal and don't ship to the browser.
- **Cross-namespace imports** between generated TypeScript
  modules use relative paths.  Don't try to merge two ns's
  output files.
- **Default values matter.**  TypeScript interface fields
  are non-optional; the decoder fills in `dflt` for missing
  fields so the result is a fully-typed value.

### See also
<a href="#see-also"></a>

- [Strings — JSON cfmt](/txt/exe/amc/string.md) — the cfmt that drives JSON output
- [Dispatches](/txt/exe/amc/dispatch.md) — JS side of dispatch (Case enum, message types)
- [Protocols](/txt/exe/amc/proto.md) — shared with C++ side
- Source: `cpp/amc/js.cpp`
- Spec tables: `acr 'dmmeta.nsjs:%'`, `acr 'dmmeta.cjsbltin:%'`, `acr 'dmmeta.cjson:%'`, `acr 'jsbltin:%'`

### Example
<a href="#example"></a>

The namespace opts in first, and the row that produces
functions rather than interfaces alone is the one carrying
`ifaceonly:N`:

```ssim
dmmeta.nsjs  ns:ams  typescript:Y  ifaceonly:N
```

A ctype in that namespace gets a codec once it also carries a
`dmmeta.pack` row:

```ssim
dmmeta.pack  ctype:ams.LogMsg
```

Run amc, which writes `ts/gen/ams_gen.ts`.  In a frontend where
the `@gen` alias resolves to `ts/gen`:

```typescript
import { LogMsg, LogMsg_Encode, LogMsg_Decode } from '@gen/ams_gen'

const view = new DataView(new ArrayBuffer(4096))
const msg = new LogMsg()
msg.text = 'started'

// encode writes at a caller-supplied offset of a caller-supplied
// view, and returns the frame's byte count
try {
    const nbyte = LogMsg_Encode(view, 0, msg)
    console.log(`wrote ${nbyte} bytes`)
} catch (err) {
    // a tail that does not fit the view, or a frame length the
    // length word cannot hold, is refused as a RangeError naming
    // the ctype and the field
    if (!(err instanceof RangeError)) { throw err }
    console.error(err.message)
}

// decode reads from the same (view, offset) pair
const got = LogMsg_Decode(view, 0)
console.log(got.text)
```

Decode returns the value rather than a nullable, so there is no
null to test for.  It takes a view and an offset rather than a
`Uint8Array` because a caller usually holds several frames in
one buffer and walks it by advancing the offset by each frame's
byte count.

The `LogMsg` declaration, the codec, the enum unions and the
field metadata all come from the same ssim that produces the
C++ side.  Adding a field to the ctype updates both at once.

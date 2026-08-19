## amc Feature: Dispatches — dispatch, dispsig, dispctx, disptrace
<a href="#amc-feature-dispatches-dispatch-dispsig-dispctx-disptrace"></a>

A **dispatch** is amc's mechanism for "given a buffer that
might be any of N message types, do something appropriate."
It replaces the C++ virtual-function tax with a fixed,
amc-generated `switch` over a typed discriminant, plus
optional generated print / read / call / kafka-codec paths.
Dispatches sit at the heart of every wire-format consumer in
the project — they are what makes "decode N possible messages
from this byte buffer" a one-line operation.

This page covers the whole machinery:

- the `dispatch` record and its switches,
- the **Case** type that names every message in the dispatch,
- the **MsgHeader cursor** that scans a byte buffer message
  by message,
- the per-namespace **FieldId** and **TableId** enums
  amc auto-generates,
- the cryptographic **signatures** computed for each dispatch
  (`Dispsig`) and the runtime **signature check** that runs at
  startup (`Dispsigcheck`),
- **dispctx** for callback closures and **disptrace** for
  cycle / count instrumentation.

```
dmmeta.dispatch       dispatch:prot.In  unk:N  read:N  print:Y  haslen:Y  call:Y  strict:N
  dmmeta.dispatch_msg  dispatch_msg:prot.In/prot.HeartbeatMsg
  dmmeta.dispatch_msg  dispatch_msg:prot.In/prot.LoginMsg
```

### Table Of Contents
<a href="#table-of-contents"></a>
&nbsp;&nbsp;&bull;&nbsp;  [The Dispatch record](#the-dispatch-record)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Message types](#message-types)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Case discriminant types](#case-discriminant-types)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Generated functions](#generated-functions)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Dispctx — closure parameter](#dispctx-closure-parameter)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Disptrace — cycle and count tracing](#disptrace-cycle-and-count-tracing)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Dispfilter — pre-call filtering](#dispfilter-pre-call-filtering)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Message cursors](#message-cursors)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Namespace FieldId / TableId](#namespace-fieldid-tableid)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Signatures: Dispsig & Dispsigcheck](#signatures-dispsig-dispsigcheck)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example](#example)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Pitfalls](#pitfalls)<br/>

### The Dispatch record
<a href="#the-dispatch-record"></a>

`dmmeta.dispatch  dispatch:<ns>.<name>  ...flags...` declares a
named dispatch that lives in `<ns>` (but operates over ctypes
that may live elsewhere).  The dispatch is *just metadata* —
no storage in any struct — and what amc generates from it is
controlled by the flags:

| Flag      | Effect when `Y` |
|-----------|------------------|
| `call`    | Generate `<name>Dispatch(...)` that switches on the type and calls a per-message user-implemented callback. |
| `read`    | Generate `<name>_ReadStrptr(...)` that parses a text representation of any member message. |
| `print`   | Generate `<name>_Print(...)` that prints any member message. |
| `haslen`  | The wire bytes carry an explicit length (the dispatch's shared header must have a `lenfld`).  Adds `msg_len` to all signatures. |
| `strict`  | Refuse to dispatch when `msg_len` is longer than `sizeof(message)`.  Useful for fixed-size wire protocols. |
| `unk`     | Emit a `default:` case in the switch (do nothing, but reachable). |

`dmmeta.dispatch_msg  dispatch_msg:<dispatch>/<ctype>` adds one
ctype to the dispatch's switch.  Each member ctype must
either:
- share a common header — i.e., all members have a
  `dmmeta.field reftype:Base` pointing to the same base ctype,
  and that base has a `typefld` — or
- be unrelated, in which case amc generates a discriminated-
  union "Case" enum (see below).

### Message types
<a href="#message-types"></a>

Two related ssim concepts work together with Dispatch:

- **`dmmeta.typefld  field:<base>.<discriminant>`** marks the
  field that distinguishes message types.  Usually a `u8` or
  `u16` in the shared base header.
- **`dmmeta.msgtype  ctype:<msg>  type:<value>`** stamps each
  member ctype with the integer value of the discriminant.

A common pattern:

```
dmmeta.ctype    ctype:prot.MsgHeader
  dmmeta.field   field:prot.MsgHeader.type  arg:u8  reftype:Val
    dmmeta.typefld  field:prot.MsgHeader.type
  dmmeta.field   field:prot.MsgHeader.length  arg:u8  reftype:Val
    dmmeta.lenfld   field:prot.MsgHeader.length  extra:0

dmmeta.ctype    ctype:prot.HeartbeatMsg
  dmmeta.msgtype  ctype:prot.HeartbeatMsg  type:1
  dmmeta.field    field:prot.HeartbeatMsg.base  arg:prot.MsgHeader  reftype:Base
```

This makes `HeartbeatMsg` and `MsgHeader` binary-compatible
for their first `sizeof(MsgHeader)` bytes (see [Base](/txt/exe/amc/reftype/Base.md))
and tells amc the discriminant is `type` in the header.

### Case discriminant types
<a href="#case-discriminant-types"></a>

When messages share a common header (typefld + Base), the
discriminant *is* the typefld value.  When they don't —
typically a free-form `dispatch` over a set of unrelated
ctypes — amc creates a **Case enum** for the dispatch:

```c++
enum prot_InCase {
    prot_In_HeartbeatMsg,
    prot_In_LoginMsg,
    prot_In_unk,    // when unk:Y
};
```

Each dispatch member contributes one fconst.  The `_ReadStrptr`
output and `_Print` input use the Case enum as the typed
discriminant.

For dispatches with a shared header, amc also creates a
`<Hdrtype>MsgsCase` enum that maps the typefld values to the
dispatch members.  This is what `dispatch.haslen:Y dispatch
with strict:N` uses when reading messages of unknown type from
text input.

### Generated functions
<a href="#generated-functions"></a>

Source: `cpp/amc/disp/*.cpp` (one file per code-gen aspect:
`call.cpp`, `print.cpp`, `read.cpp`, `casetype.cpp`,
`filter.cpp`, `del.cpp`, `kafka_encode.cpp`, `kafka_decode.cpp`).

For a dispatch `<ns>.<name>` with shared header `Hdr`:

| Function                                           | Emitted when      | Effect |
|----------------------------------------------------|-------------------|--------|
| `int <name>Dispatch(Hdr&[, u32 len])`              | `call:Y`          | Switch on `hdr.type`; calls user-implemented `<ns>::<name>_<Msg>(...)`.  Returns the consumed byte count, or 0 if no case matched. |
| `int <name>DispatchRaw(<Case> type, u8 *msg, u32 len)` | `call:Y`      | Raw form taking a Case enum + bytes; used by `_ReadStrptr` after parsing. |
| `void <name>_Print(cstring& out, Hdr& msg[, u32 len])` | `print:Y`     | Switch on `hdr.type`; calls `<Msg>_Print` on the active arm. |
| `<Case> <name>_ReadStrptr(strptr in, ByteAry& buf)` | `read:Y`         | Parse tag from text, dispatch to the right `<Msg>_ReadStrptrMaybe`. |
| `void <name>_Delete(Hdr& msg)`                     | (dispatch + ctype has dtor) | Switch + dtor call. |
| `void <name>_KafkaEncode(...)` / `_KafkaDecode(...)` | when ctype has `ckafka` | Kafka wire encoders/decoders driven by the dispatch. |

User callbacks are extern forward-declarations: amc emits the
prototype, the user writes the body.

```c++
// auto-generated forward decls (one per dispatch_msg)
void In_HeartbeatMsg(prot::HeartbeatMsg& msg, u32 msg_len);
void In_LoginMsg    (prot::LoginMsg&     msg, u32 msg_len);
int  InDispatch(prot::MsgHeader& msg, u32 msg_len);  // amc generates this
```

### Dispctx — closure parameter
<a href="#dispctx-closure-parameter"></a>

`dmmeta.dispctx  dispatch:<disp>  ctype:<ctx>` adds a typed
context argument as the **first** parameter of every callback
(and of the dispatch function itself).  Used to thread state
through a stateless-by-default callback API:

```
dmmeta.dispctx  dispatch:omega.In  ctype:omega.Ctx
```

```c++
int OmegaDispatch(omega::Ctx& ctx, omega::Header& msg, u32 msg_len);
void Omega_OrderAdd(omega::Ctx& ctx, omega::OrderAdd& msg, u32 msg_len);
```

The context is opaque to amc — it's just a typed parameter
passed through the switch unchanged.  Use it for the session,
connection, or whatever per-call state the callbacks need.

### Disptrace — cycle and count tracing
<a href="#disptrace-cycle-and-count-tracing"></a>

`dmmeta.disptrace  dispatch:<disp>  cycle:Y/N` adds a trace
struct entry per message type to the namespace's
`FDb.trace` record.  Each callback gets wrapped with:

```c++
case 1:
    u64 t0 = algo::get_cycles();
    Omega_OrderAdd(...);
    ns::_db.trace.cycles_orderadd += algo::get_cycles() - t0;
    ns::_db.trace.count_orderadd  += 1;
    break;
```

`cycle:N` keeps only the count; `cycle:Y` adds the cycle
total.  Trace fields are visible to amc-reflection
(`-signature`, `-version`, `acr_in`).  See
[Trace](/txt/exe/amc/trace.md) for the broader tracing
mechanism.

### Dispfilter — pre-call filtering
<a href="#dispfilter-pre-call-filtering"></a>

`dmmeta.dispfilter  dispatch:<disp>  match_all:Y/N` injects a
filter predicate that runs *before* the user callback.  Use
case: skip messages the consumer doesn't care about (e.g.,
test-environment-only message types).  The filter itself is
user-implemented; amc only generates the call site.

### Message cursors
<a href="#message-cursors"></a>

For **every ctype with a `lenfld`** (i.e., every message
header), amc auto-creates a typed cursor `<Hdr>_curs` that
walks a byte buffer message by message.  Cursor generation
is the **`amcdb.gen:msgcurs`** phase (`cpp/amc/msgcurs.cpp`):

```c++
struct MsgHeader_curs {
    typedef MsgHeader *ChildType;
    MsgHeader  *msg;     // current message, or NULL when done
    u8         *bytes;   // remaining unread bytes
    i32         limit;   // bytes left
    i32         msglen;  // length of the current message
};

inline void MsgHeader_curs_Reset(MsgHeader_curs& curs, algo::memptr buf);
inline void MsgHeader_curs_Next (MsgHeader_curs& curs);
inline bool MsgHeader_curs_ValidQ(MsgHeader_curs& curs);
inline MsgHeader*& MsgHeader_curs_Access(MsgHeader_curs& curs);
```

It is `ind_beg`-compatible — drop-in for the standard cursor
loop:

```c++
algo::memptr buf = recv_bytes;
ind_beg(prot::MsgHeader_curs, msg, buf) {
    InDispatch(*msg, msg->length);
} ind_end;
```

The cursor uses the lenfld's `LengthExpr` (with `extra`,
`scale`) to compute the byte step on each `_Next`; messages
that would extend past `_limit` cause the cursor to
terminate.

This is the wire-decoder pattern for length-delimited
protocols: cursor + Dispatch + per-msg callback.

### Namespace FieldId / TableId
<a href="#namespace-fieldid-tableid"></a>

amc auto-creates two enum-typed helper structs per namespace:

- **`<ns>::FieldId`** — every field of every ctype in the
  namespace becomes an `fconst` of `FieldId.value`.  Used by
  the tuple parser to look up an attribute name in O(1)
  (`Read` paths walk `key:value` pairs and dispatch on the
  field ID).  Generated by `amcdb.gen:fieldid`
  (`cpp/amc/tableid.cpp`).

  ```c++
  enum <ns>_FieldId_Enum {
       <ns>_FieldId_<field0> = 0
      ,<ns>_FieldId_<field1> = 1
      ,...
  };
  ```

- **`<ns>::TableId`** — every ctype that has a `finput` becomes
  an `fconst` of `TableId.value`.  Used by
  `InsertStrptrMaybe` to dispatch the parsed tuple's tag to
  the right table.  Generated by `amcdb.gen:tableenum`
  (`cpp/amc/tableid.cpp`).

You don't write either of these by hand; amc populates them
based on the ctypes/fields present in the namespace.  They
show up in the generated header and exist primarily so that
the dispatcher code can be O(1) instead of a chain of string
comparisons.

### Signatures: Dispsig & Dispsigcheck
<a href="#signatures-dispsig-dispsigcheck"></a>

Wire compatibility is fragile: change a field's type, a
message's layout, or the discriminant value, and the protocol
silently breaks.  amc's signature mechanism builds a
cryptographic fingerprint of every dispatch's full type graph
and forces sender + receiver to agree.

**Generation** (`amcdb.gen:prep_signature` in
`cpp/amc/signature.cpp`):

1. For each ctype, compute a SHA-1 over field names, args,
   reftypes, widths, smallstr lengths, etc. — only the bits
   that affect on-wire layout.
2. Mix in each ctype's `fconst`s (unordered combination).
3. For each dispatch, mix the signatures of every member
   ctype (recursively) into a final dispatch signature.
4. Emit a `dmmeta.dispsig  dispsig:<disp>  signature:<sha1>`
   record per dispatch.

**Runtime check** (`Global.InitReflection` in
`cpp/amc/global.cpp`):

Every executable's startup inserts a tuple of the form
`dmmeta.Dispsigcheck  dispsig:<disp>  signature:<sha1>` into
`algo_lib::_db.dispsigcheck` *for every dispatch its
namespace owns*.  Library consumers (e.g., processes
linking lib_kafka and wanting to talk to a compatible peer)
walk that table at handshake time and compare signatures —
mismatch means "you compiled against a different version of
this protocol; refuse to peer."

The `dmmeta.dispsigcheck` ssimfile is shipped alongside
dispatches; running `-signature` on any amc-built executable
dumps its full signature table so other processes can verify.

### Pitfalls
<a href="#pitfalls"></a>

- **`typefld` must be reachable.**  amc reads
  `hdr.<typefld>` to switch — the header's discriminant field
  must be a plain Val (or a Bitfld that resolves to one).
- **`msgtype` collisions are not enforced** by amc beyond
  pkey uniqueness.  Two messages with the same `type:` value
  are dispatched by whichever comes first.
- **`strict:Y` is non-default.**  The default behavior accepts
  a buffer with more bytes than a message's `sizeof` — the
  cursor will eat the extra.  Set `strict:Y` to reject such
  buffers (matters for FIX-style wire formats).
- **Dispatchee ctypes need the right cfmt.**  Reading
  (`read:Y`) requires each member to have a `cfmt` with
  `read:Y`; printing similarly.  amc refuses otherwise.
- **Adding a dispatch_msg changes the signature.**  Existing
  peers will reject the new build until upgraded — that is
  by design.  Coordinate rollouts.
- **`dispctx` adds the parameter to every callback,**
  including ones that don't use it.  Keep contexts small or
  use pointers.
- **`dispatch.haslen:N` with `lenfld`-bearing messages** is
  inconsistent — the dispatcher won't know how far to advance
  the cursor.  Always use `haslen:Y` for length-delimited
  protocols.
- **Cross-namespace dispatches** are supported but the
  dispatch lives in *its* namespace, not the messages'.

### See also
<a href="#see-also"></a>

- [Protocols](/txt/exe/amc/proto.md) — broader protocol idioms
- [Base](/txt/exe/amc/reftype/Base.md) — shared-header inheritance
- [Varlen](/txt/exe/amc/varlen.md) / [Opt](/txt/exe/amc/reftype/Opt.md) — trailing payloads
- [Trace](/txt/exe/amc/trace.md) — broader instrumentation
- [Kafka](/txt/exe/amc/kafka.md) — dispatch-driven Kafka codecs
- [Global / reflection](/txt/exe/amc/reftype/Global.md) — where `dispsigcheck` rows get registered
- Source: `cpp/amc/disp/`, `cpp/amc/msgcurs.cpp`, `cpp/amc/tableid.cpp`, `cpp/amc/signature.cpp`
- Spec tables: `acr 'dmmeta.dispatch:%'`, `acr 'dmmeta.dispatch_msg:%'`, `acr 'dmmeta.dispctx:%'`, `acr 'dmmeta.disptrace:%'`, `acr 'dmmeta.dispsig:%'`, `acr 'dmmeta.dispsigcheck:%'`

### Example
<a href="#example"></a>

A protocol with a shared header and three messages:

```
dmmeta.ctype     ctype:prot.MsgHeader
  dmmeta.pack    ctype:prot.MsgHeader
  dmmeta.field   field:prot.MsgHeader.type    arg:u8  reftype:Val
    dmmeta.typefld  field:prot.MsgHeader.type
  dmmeta.field   field:prot.MsgHeader.length  arg:u8  reftype:Val
    dmmeta.lenfld   field:prot.MsgHeader.length  extra:0

dmmeta.ctype     ctype:prot.HeartbeatMsg
  dmmeta.msgtype  ctype:prot.HeartbeatMsg  type:1
  dmmeta.field    field:prot.HeartbeatMsg.base  arg:prot.MsgHeader  reftype:Base
  dmmeta.cfmt     cfmt:prot.HeartbeatMsg.String  printfmt:Tuple  read:Y  print:Y

dmmeta.ctype     ctype:prot.LoginMsg
  dmmeta.msgtype  ctype:prot.LoginMsg  type:2
  dmmeta.field    field:prot.LoginMsg.base  arg:prot.MsgHeader  reftype:Base
  dmmeta.field    field:prot.LoginMsg.user  arg:algo.RspaceStr16  reftype:Val

dmmeta.dispatch       dispatch:gate.In  unk:N  read:Y  print:Y  haslen:Y  call:Y  strict:N
  dmmeta.dispatch_msg  dispatch_msg:gate.In/prot.HeartbeatMsg
  dmmeta.dispatch_msg  dispatch_msg:gate.In/prot.LoginMsg
  dmmeta.dispctx       dispatch:gate.In  ctype:gate.Session
  dmmeta.disptrace     dispatch:gate.In  cycle:Y
```

In C++:

```c++
// user-implemented callbacks:
void gate::In_HeartbeatMsg(gate::Session& s, prot::HeartbeatMsg& msg, u32 len) { ... }
void gate::In_LoginMsg    (gate::Session& s, prot::LoginMsg&     msg, u32 len) { ... }

// reading bytes off the wire:
algo::memptr buf = recv_bytes;
ind_beg(prot::MsgHeader_curs, msg, buf) {
    gate::InDispatch(session, *msg, msg->length);
    // dispatch generates trace: gate::_db.trace.cycles_heartbeatmsg += ...
} ind_end;

// printing for logs:
algo::cstring out;
gate::In_Print(out, *msg, msg->length);
```

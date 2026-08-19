## amc Feature: Inheritance — Base, Castbase/Castdown, Pkey, Upptr
<a href="#amc-feature-inheritance-base-castbase-castdown-pkey-upptr"></a>

amc has **no virtual functions** and **no class inheritance**
in the C++ sense.  Instead it offers a small kit of explicit
relationships that together cover the same problem space —
shared layout, polymorphic message dispatch, foreign-key
references, parent pointers — without the runtime tax of
virtuals or the maintenance burden of multi-inheritance.

| Concept             | Reftype                                  | What it provides |
|---------------------|------------------------------------------|------------------|
| Shared prefix layout | [Base](/txt/exe/amc/reftype/Base.md)    | "Is-a" via field-copying; binary-compatible on the wire. |
| Down-cast by tag    | `Castdown` tfunc on Base                 | Safe runtime dispatch on a discriminant (`typefld`). |
| Foreign key by name | [Pkey](/txt/exe/amc/reftype/Pkey.md)    | Schema-only "use the target's pkey type"; rewritten to Val. |
| Foreign key by ptr  | [Upptr](/txt/exe/amc/reftype/Upptr.md)  | Runtime pointer set by an xref's `via:` path. |
| Forward pointer     | [Ptr](/txt/exe/amc/reftype/Ptr.md)      | Single-slot xref pointer. |

This page covers how they fit together.

### Table Of Contents
<a href="#table-of-contents"></a>
&nbsp;&nbsp;&bull;&nbsp;  [Why no virtuals?](#why-no-virtuals-)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Base: shared layout](#base-shared-layout)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Castbase / Castdown](#castbase-castdown)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Pkey: schema-only foreign keys](#pkey-schema-only-foreign-keys)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Upptr: pointer-based foreign keys](#upptr-pointer-based-foreign-keys)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Multi-level Base](#multi-level-base)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Picking the right mechanism](#picking-the-right-mechanism)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Pitfalls](#pitfalls)<br/>

### Why no virtuals?
<a href="#why-no-virtuals-"></a>

amc's design rejects C++ virtual functions for three reasons:

1. **Predictable post-conditions.**  A virtual call is opaque
   to the caller — Dijkstra's predicate-transformer reasoning
   stops at the indirection.  Explicit
   [Dispatches](/txt/exe/amc/dispatch.md) keep the set of
   possible behaviors visible in one place.
2. **Predictable cost.**  Virtual calls cost an indirect
   load + branch.  amc's switch dispatch is direct and the
   branches inline cleanly.
3. **No vtables on the wire.**  amc-generated structs are
   POD-compatible.  Binary serialization works by `memcpy`;
   there's no vtable pointer to skip or recompute.

When you want polymorphism, the workflow is:

- Define a discriminant field (`typefld`).
- Assign each message ctype a value via `msgtype`.
- Define a [Dispatch](/txt/exe/amc/dispatch.md) over the set.
- amc generates the switch.

Base is the layout half of that story; Dispatch is the
behavior half.

### Base: shared layout
<a href="#base-shared-layout"></a>

`reftype:Base` declares a field whose `arg` is another
ctype, and **inlines** every field of that ctype into the
parent at the same offsets.  The result is binary-compatible
with the base type for its entire prefix.

```
dmmeta.ctype  ctype:prot.HeartbeatMsg
  dmmeta.field  field:prot.HeartbeatMsg.base  arg:prot.MsgHeader  reftype:Base
```

After amc runs, `HeartbeatMsg` has every field of `MsgHeader`
as its own — `type`, `length`, anything else — and a pointer
to a `HeartbeatMsg` is castable to a `MsgHeader*` byte-for-
byte.

See [Base reftype](/txt/exe/amc/reftype/Base.md) for the
field-level details (when `Castbase`/`Castdown` are emitted,
what `fbase` does, etc.).

### Castbase / Castdown
<a href="#castbase-castdown"></a>

When the layouts agree (no `fbase` field-strip, base has a
non-trivial shape), amc emits:

- `Base& Castbase(Derived& row)` — zero-cost upcast,
  `reinterpret_cast` under the hood.
- `Derived* Derived_Castdown(Base& hdr)` — typed downcast
  that validates `hdr.<typefld> == <msgtype>` (and the length
  field if applicable) before returning the pointer; NULL if
  the message isn't actually of that type.

```c++
prot::MsgHeader &hdr = ...;      // raw bytes from the wire
if (prot::HeartbeatMsg *m = HeartbeatMsg_Castdown(hdr)) {
    // m and &hdr alias; m is typed
}

prot::HeartbeatMsg msg;
prot::MsgHeader &header = Castbase(msg);     // upcast
header.type = 1;
```

These two together replace the C++ `dynamic_cast` /
`static_cast` workflow with explicit, amc-validated casts.

### Pkey: schema-only foreign keys
<a href="#pkey-schema-only-foreign-keys"></a>

`reftype:Pkey  arg:<TargetCtype>` says **"this field holds the
primary-key text of a record of type `<TargetCtype>`"**.
amc resolves the pkey to the target's underlying string type
in a pre-pass (`amcdb.gen:lookuppkey`) and rewrites the field as a
plain `Val`.

By the time C++ is emitted, no `Pkey` fields exist — they
look like `Smallstr`s (or whatever the target's pkey type
resolves to).  But the **schema metadata still records the
relationship**, so `acr -check` enforces referential integrity
at load time.

```
dmmeta.field  field:my.Order.customer  arg:my.Customer  reftype:Pkey
```

Becomes:

```c++
struct Order {
    algo::Smallstr50 customer;   // pkey of my::Customer
    ...
};
```

Loading a row whose `customer` value doesn't match any
`my.Customer` record fails `acr -check`.  Pkey is the **text-
level** foreign key; Upptr is the **pointer-level** companion.

### Upptr: pointer-based foreign keys
<a href="#upptr-pointer-based-foreign-keys"></a>

`reftype:Upptr  arg:<TargetCtype>` is the runtime sibling.  It
declares a plain pointer that is populated by an xref's
`via:` path when the row is inserted:

```
dmmeta.ctype  ctype:abt.FTargdep
  dmmeta.field  field:abt.FTargdep.p_parent  arg:abt.FTarget  reftype:Upptr
```

By convention, Upptr field names start with `p_`.  The
**Pkey** field and the **Upptr** field together work like a
SQL foreign key: the Pkey is the textual reference loaded
from ssim, the Upptr is the in-memory pointer resolved by
the xref system.

```c++
abt::FTargdep &td = ...;
abt::FTarget *parent = td.p_parent;     // O(1) — set by Thash xref
prlog(parent->target);                  // walk up the tree
```

See [Upptr reftype](/txt/exe/amc/reftype/Upptr.md) for the
mechanics.

### Multi-level Base
<a href="#multi-level-base"></a>

amc supports chained Base — a ctype with a Base pointing at
another ctype that itself has a Base, etc.  Each level
inlines its predecessor's fields:

```
dmmeta.field  field:proto.MsgHeader.<base-fields>  ...

dmmeta.ctype    ctype:proto.RequestMsg
  dmmeta.field  field:proto.RequestMsg.hdr  arg:proto.MsgHeader  reftype:Base

dmmeta.ctype    ctype:proto.LoginRequest
  dmmeta.field  field:proto.LoginRequest.req  arg:proto.RequestMsg  reftype:Base
```

`LoginRequest` ends up with every MsgHeader field + every
RequestMsg-specific field + every LoginRequest-specific
field, in that order.  Castbase walks one level at a time;
Castdown validates the discriminant at the innermost base
with a `typefld`.

**Multiple inheritance is not supported.**  A ctype may have
at most one Base field.

### Picking the right mechanism
<a href="#picking-the-right-mechanism"></a>

| You want…                                                | Use                                                     |
|----------------------------------------------------------|---------------------------------------------------------|
| Multiple messages sharing the first N bytes              | [Base](/txt/exe/amc/reftype/Base.md) + `typefld` + `msgtype` |
| Polymorphic processing of a buffer of messages           | [Dispatch](/txt/exe/amc/dispatch.md) over the Base'd types |
| A field that names another record by its pkey            | [Pkey](/txt/exe/amc/reftype/Pkey.md)                   |
| The runtime pointer for that pkey                        | Companion [Upptr](/txt/exe/amc/reftype/Upptr.md), populated via xref |
| A single optional child pointer (no array)               | [Ptr](/txt/exe/amc/reftype/Ptr.md)                     |
| Many children indexed by name / value                    | [Thash](/txt/exe/amc/reftype/Thash.md) / [Atree](/txt/exe/amc/reftype/Atree.md) |
| A virtual-method-like callback                           | [Hook](/txt/exe/amc/hook.md)                            |

### Pitfalls
<a href="#pitfalls"></a>

- **No virtuals means no vtable, but also no "default
  method" semantics.**  Each message type's behavior must be
  written separately; share code via direct function calls.
- **`Castdown` requires `typefld` + `msgtype`.**  Without
  them, amc only emits `CopyOut/CopyIn/Castbase`.
- **Field-strip (`fbase`) breaks `Castbase`.**  Stripping a
  base field from a derived layout invalidates the binary
  compatibility — amc refuses to emit `Castbase` in that
  case.
- **Pkey + Upptr need an xref to bridge them.**  Without an
  xref, the Pkey is a free-floating string and the Upptr
  stays NULL.
- **Don't mix Pkey + Upptr names with the same prefix.**
  Conventionally use the field name from the parent for the
  Pkey (e.g., `target`) and a `p_`-prefixed version for the
  Upptr (`p_target`).
- **Cross-namespace Base requires the base's namespace to
  be included.**  amc walks the cppincl/hdrincl tree.

### See also
<a href="#see-also"></a>

- [Base](/txt/exe/amc/reftype/Base.md)
- [Pkey](/txt/exe/amc/reftype/Pkey.md)
- [Upptr](/txt/exe/amc/reftype/Upptr.md)
- [Ptr](/txt/exe/amc/reftype/Ptr.md)
- [Dispatch](/txt/exe/amc/dispatch.md)
- [Cross-references](/txt/exe/amc/xref.md)
- [Hooks](/txt/exe/amc/hook.md)
- Source: `cpp/amc/base.cpp`, `cpp/amc/ptr.cpp`, `cpp/amc/upptr.cpp`, `cpp/amc/gen.cpp` `gen_lookuppkey`

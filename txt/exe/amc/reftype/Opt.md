## amc Reftype: Opt
<a href="#amc-reftype-opt"></a>

`Opt` is a **0-or-1 trailing field** glued to the end of a
message struct.  If the parent's `lenfld` value indicates there
are enough trailing bytes to hold the `arg` type, the optional
field exists; otherwise it doesn't.  Opt is the wire-format
companion of [Varlen](/txt/exe/amc/reftype/Varlen.md) — same
"data lives past the fixed header" pattern, but exactly zero
or one element instead of many.

```
dmmeta.ctype  ctype:prot.LoginRsp
  dmmeta.field  field:prot.LoginRsp.length  arg:u8  reftype:Val
    dmmeta.lenfld  field:prot.LoginRsp.length  extra:0
  dmmeta.field  field:prot.LoginRsp.reason   arg:algo.RspaceStr64  reftype:Opt
```

Above declares a login response whose `reason` string is only
present when the wire length is at least
`sizeof(LoginRsp) + sizeof(RspaceStr64)`.

### What it generates
<a href="#what-it-generates"></a>

No storage in the parent ctype other than a comment marker
(see `tclass_Opt` in `cpp/amc/opt.cpp`):

```c++
struct LoginRsp {
    u8  length;
    ...
    // <T>    reason[];   optional field
};
```

The optional element lives in the bytes immediately following
`sizeof(LoginRsp)`.  `_Get` returns either a pointer to that
location or NULL based on the parent's lenfld value.

The parent must have a `dmmeta.lenfld`; without one, `_Get`
cannot decide whether the optional is present.

### Opt vs Varlen
<a href="#opt-vs-varlen"></a>

|              | **Opt**                            | **Varlen**                  |
|--------------|------------------------------------|-----------------------------|
| Count        | 0 or 1                             | 0..N                        |
| API          | `_Get` returns `T*` or NULL        | `_Getary` returns span      |
| Last field?  | yes                                | yes                         |
| Element ctor | placement-new via `AllocVarlen`    | typically `memcpy`'d in     |

Both are used for tail content in protocol messages; Opt is
the right choice when the trailing piece is itself a single
typed structure (possibly varlen on its own).  The choice is
exclusive: one ctype cannot carry both an Opt and a Varlen
field, because each begins at the end of the fixed portion --
amc rejects the combination with `amc.trail_overlay`.  Two Opt
fields on one ctype are rejected the same way: a Varlen field
carries the offset at which it begins, so one Varlen can follow
another, and an Opt carries none.

### Ssim inputs
<a href="#ssim-inputs"></a>

Required:

- `dmmeta.field` with `reftype:Opt`.  `arg:` is the optional
  type.
- `dmmeta.lenfld` on the parent's length field — required for
  `_Get` to decide presence.

No spec table.

### Generated functions
<a href="#generated-functions"></a>

Source: `cpp/amc/opt.cpp`.

| Tfunc                  | Generated function                                        | Effect |
|------------------------|-----------------------------------------------------------|--------|
| `Opt.Get`              | `T* <name>_Get(P&)`                                       | Returns pointer to the trailing optional, or NULL if the parent's length is too short.  If the optional itself has a `lenfld`, also validates that the optional's declared length fits. |
| `Opt.Getary`           | `aryptr<u8> <name>_Getary(P&)`                            | Raw byte span over the trailing region. |
| `Opt.Print`            | `void <name>_Print(P&, cstring& out)`                     | If present, dispatch on the optional's `typefld` (for tagged types) or print directly. |
| `Opt.ReadStrptrMaybe`  | `bool <name>_ReadStrptrMaybe(P&, strptr in)`              | Parse the optional from a string and append it to the buffer pointed to by `algo_lib::_db.varlenbuf`. |

### Memory model
<a href="#memory-model"></a>

- The optional shares its allocation with the parent: use the
  pool's `AllocVarlen(extra_bytes)` to reserve space for the
  optional at construction time.
- `_Get` does no allocation; it computes
  `(u8*)&parent + sizeof(parent)` and validates against
  lenfld.
- No destructor invocation — like Varlen, Opt elements must
  not have a destructor.

### Pitfalls
<a href="#pitfalls"></a>

- **lenfld required.**  Opt with no parent lenfld leaves
  `_Get` no way to decide presence.
- **Must be the last field.**  Same rule as Varlen.
- **Cannot coexist with a Varlen field, or with a second Opt
  field.**  Each begins at the end of the fixed portion; amc
  rejects the ctype with `amc.trail_overlay`.
- **Element type's destructor.**  Forbidden — the parent's
  `_Uninit` does not know about the trailing region's typed
  content.
- **No mutation of presence.**  `_Get` returns NULL based on
  the parent's lenfld; flipping presence means rewriting the
  lenfld and the trailing bytes (typically: free the parent
  and allocate a new one).
- **`Opt` of a Varlen** — the optional's own lenfld is
  validated.  Don't bake assumptions about
  `sizeof(Opt arg)` into callers; use `_Get` and check.

### See also
<a href="#see-also"></a>

- [Reftypes index](/txt/exe/amc/reftype.md)
- [Varlen](/txt/exe/amc/reftype/Varlen.md) — 0..N variant
- [Protocols](/txt/exe/amc/proto.md) — message-layout idioms
- [Varlen feature page](/txt/exe/amc/varlen.md)
- Source: `cpp/amc/opt.cpp`
- Tfunc records: `acr 'tfunc:Opt.%'`

### Example
<a href="#example"></a>

A login response that may include a reason string:

```
dmmeta.ctype  ctype:prot.LoginRsp
  dmmeta.field  field:prot.LoginRsp.length  arg:u16  reftype:Val
    dmmeta.lenfld  field:prot.LoginRsp.length  extra:0
  dmmeta.field  field:prot.LoginRsp.code     arg:u8   reftype:Val
  dmmeta.field  field:prot.LoginRsp.reason   arg:algo.RspaceStr64  reftype:Opt
```

In C++:

```c++
prot::LoginRsp *rsp = LoginRsp_AllocVarlen(_db, sizeof(algo::RspaceStr64));
rsp->code = 0;
rsp->length = sizeof(prot::LoginRsp) + sizeof(algo::RspaceStr64);
if (algo::RspaceStr64 *reason = reason_Get(*rsp)) {
    *reason = "ok";
}
```

On the read side, callers check the return of `_Get`:

```c++
prot::LoginRsp *rsp = ...;
if (algo::RspaceStr64 *reason = reason_Get(*rsp)) {
    log("login: "<<*reason);
} else {
    log("login: (no reason given)");
}
```

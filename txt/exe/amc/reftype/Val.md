## amc Reftype: Val
<a href="#amc-reftype-val"></a>

`Val` is the **simplest reftype** — a plain struct member.  An
`arg:T  reftype:Val` field becomes a `T` field in the parent C++
struct, initialized to its declared default.  No allocation,
no indirection, no extra machinery.

The vast majority of fields in amc-generated code are `Val`s.

```ssim
dmmeta.field  field:algo.UnTime.value  arg:i64  reftype:Val  dflt:""  comment:"unix time * 1e9 + nanoseconds"
```

### What it generates
<a href="#what-it-generates"></a>

A single member field is added to the parent ctype
(see `tclass_Val` in `cpp/amc/val.cpp`):

| Field           | Type | Notes |
|-----------------|------|-------|
| `<name>` (or `<name>_be` if `fbigend`) | `T` | The default value, if any, is applied at parent's `_Init` time |

When `dmmeta.fbigend` is set on a Val of an integer type, amc
renames the storage to `<name>_be` so accessors must go through
`_Get` / `_Set` (which call `betoh*` / `htobe*`).  See
[Big-endian fields](/txt/exe/amc/bigendian.md).

### Ssim inputs
<a href="#ssim-inputs"></a>

Required:

- `dmmeta.field` with `reftype:Val`.  `arg:` is the value type.

Optional:

- `dflt:"…"` — initializer expression applied in `_Init`.  May
  reference `*this` (rewritten to `parent`).
- `dmmeta.fbigend` — store big-endian, expose via `_Get/_Set`.
- `dmmeta.cppfunc` — make this a *computed* (field-function)
  Val with no storage; the value is an expression.
- `dmmeta.pmaskfld_member` — when set, `_Set` also flips a bit
  in a presence mask.  See [Presence masks](/txt/exe/amc/pmask.md).

### Generated functions
<a href="#generated-functions"></a>

Source: `cpp/amc/val.cpp`.

| Tfunc          | Generated function                  | When emitted | Effect |
|----------------|-------------------------------------|--------------|--------|
| `Val.Init`     | (folded into parent's `_Init`)      | always, if `dflt` non-empty | Assign the default.  For external types with `initmemset`, does `memset(&field, dflt, sizeof field)`. |
| `Val.Get`      | `T <name>_Get(const P&)`            | `fbigend:Y`  | `be<W>toh(parent.<name>_be)` |
| `Val.Set`      | `void <name>_Set(P&, T rhs)`        | when `fbigend` or `pmaskfld_member` present | Write the value; for big-endian fields swap; for pmask members flip the presence bit. |
| `Val.RowidFind`| `algo::ImrowPtr <name>_RowidFind(int t)` | `reflect:Y` | Reflection helper (returns the address of the single value when `t==0`). |
| `Val.N`        | `i32 <name>_N()` returning 1        | `reflect:Y`  | Reflection helper. |

A regular Val without big-endian / pmask attachments produces
*only* `Init`.  Reads and writes go through the field
directly: `parent.<name>`.

### Memory model
<a href="#memory-model"></a>

- Zero overhead: one field of size `sizeof(T)` in the parent.
- `_Init` is folded into the parent's constructor — no separate
  function call per field.
- A `cppfunc` (computed) Val occupies zero bytes and is
  evaluated on each read.

### Pitfalls
<a href="#pitfalls"></a>

- **`fbigend + fldfunc` is rejected.**  A computed field can't
  also be big-endian — there is no storage to swap.
- **`Val + non-extern finput` is rejected.**  A plain value
  field can't be a row-source for the ssim loader unless the
  loader is extern.
- **`Val` cannot be a cross-reference.**  Use Ptr / Upptr / etc.
  if you need referential semantics.
- **`_Set` is not always generated.**  Without `fbigend` or
  pmask membership, write directly: `parent.<name> = rhs;`.
  No special call needed.
- **`dflt` can reference `*this`** (resolved to `parent`).  Use
  for self-referential defaults; otherwise prefer a literal
  expression to keep the constructor simple.

### See also
<a href="#see-also"></a>

- [Reftypes index](/txt/exe/amc/reftype.md)
- [Big-endian fields](/txt/exe/amc/bigendian.md) — `fbigend` variant
- [Presence masks](/txt/exe/amc/pmask.md) — pmaskfld_member interaction
- [Cppfunc](/txt/exe/amc/reftype.md) — computed-value variant
- Source: `cpp/amc/val.cpp`
- Tfunc records: `acr 'tfunc:Val.%'`

### Example
<a href="#example"></a>

A nanosecond timestamp:

```ssim
dmmeta.ctype  ctype:algo.UnTime
  dmmeta.field  field:algo.UnTime.value  arg:i64  reftype:Val  dflt:""
```

Generates:

```c++
struct UnTime {
    i64 value;
    UnTime();
};

inline UnTime::UnTime() {
    // no init body — dflt is empty, default-init is enough
}
```

A big-endian wire-format field:

```ssim
dmmeta.field  field:net.TcpHdr.seq  arg:u32  reftype:Val
  dmmeta.fbigend  field:net.TcpHdr.seq
```

Generates `seq_be` storage plus `seq_Get` / `seq_Set` accessors
that swap.

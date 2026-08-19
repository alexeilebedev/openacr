## amc Reftype: Smallstr
<a href="#amc-reftype-smallstr"></a>

`Smallstr` declares a **fixed-length inline string**.  The
storage is `length` characters embedded directly in the
struct — no heap allocation, no separate length field — with
amc supplying the read/print/parse glue.  Smallstrs are the
backbone of every wire format (`algo.Smallstr50`,
`algo.RspaceStr8`, `algo.LnumStr10_U64`) and every database
pkey that has to fit in a fixed slot.

```
dmmeta.field   field:algo.LspaceStr5.ch  arg:char  reftype:Smallstr  dflt:""  comment:""
  dmmeta.smallstr  field:algo.LspaceStr5.ch  length:5  strtype:leftpad  pad:"' '"  strict:Y
```

### What it generates
<a href="#what-it-generates"></a>

Storage and accessors are emitted directly into the **parent**
ctype (see `tclass_Smallstr` in `cpp/amc/smallstr.cpp`).
The layout depends on the **strtype** picked from
`dmmeta.strtype`:

| strtype     | Layout                                                                |
|-------------|----------------------------------------------------------------------|
| `rightpad`  | `length` bytes; unused tail filled with `pad`.                       |
| `leftpad`   | `length` bytes; unused head filled with `pad`.                       |
| `rpascal`   | `length + 2` bytes: data, then a trailing `u8` length byte (max length 255). |

`pad` is a C++ character expression (`"' '"`, `"'0'"`, `"0"`).
For numeric strings (paired with `dmmeta.numstr`), the pad
typically is `'0'` to enable left-padded base-10/16 encoding.

### Naming convention
<a href="#naming-convention"></a>

With `strict:Y` (the usual setting), amc enforces a strict ctype
name → behavior mapping:

| Prefix      | strtype  | pad    | Use                                         |
|-------------|----------|--------|---------------------------------------------|
| `Smallstr`  | `rpascal`| —      | Generic pascal string up to `N` chars.      |
| `RspaceStr` | rightpad | `' '`  | Right-space-padded (used in fixed-format APIs). |
| `LspaceStr` | leftpad  | `' '`  | Left-space-padded.                          |
| `LnumStr`   | leftpad  | `'0'`  | Numeric, zero-padded on left.               |
| `RnullStr`  | rightpad | `0`    | Right-NUL-padded.                           |
| `LnullStr`  | leftpad  | `0`    | Left-NUL-padded.                            |

Length suffix in the name must match `length` (e.g.,
`LspaceStr7` ↔ `length:7`).  Numstr base, if any, appears as
`_BaseN`.

### Ssim inputs
<a href="#ssim-inputs"></a>

Required:

- `dmmeta.field` with `reftype:Smallstr`.  `arg:char` for
  byte-oriented strings.
- `dmmeta.smallstr`:
  - `length` — character count (≤ 255 for rpascal).
  - `strtype` — pkey of `dmmeta.strtype` (rightpad / leftpad / rpascal).
  - `pad` — C++ character expression for the pad byte
    (rightpad / leftpad only).
  - `strict:Y/N` — enforce the naming-prefix conventions above.

Optional:

- `dmmeta.numstr` — turn the field into a numeric-string field
  (also generates `_Getnum` / `_SetnumMaybe`); see
  [Strings page](/txt/exe/amc/string.md).

### Generated functions
<a href="#generated-functions"></a>

Source: `cpp/amc/smallstr.cpp`.

| Tfunc                       | Generated function                                  | Effect |
|-----------------------------|-----------------------------------------------------|--------|
| `Smallstr.Init`             | `<name>_Init(P&)` (folded)                          | Padded forms fill the storage with the pad character; rpascal sets the length byte to zero and leaves the character bytes as they were. |
| `Smallstr.N`                | `int <name>_N(const P&)`                            | Current logical length (for rpascal: trailing byte; for padded forms: counts non-pad characters). |
| `Smallstr.Max`              | `int <name>_Max(P&)`                                | Compile-time constant equal to `length`. |
| `Smallstr.Getary`           | `aryptr<char> <name>_Getary(const P&)`              | Span over the storage (excluding the rpascal length byte). |
| `Smallstr.Add`              | `void <name>_Add(P&, char c)`                       | Append `c`, if there's room.  rpascal: bumps the length. |
| `Smallstr.AddStrptr`        | `void <name>_AddStrptr(P&, strptr rhs)`             | Append a span; same rules as `Add`. |
| `Smallstr.SetStrptr`        | `void <name>_SetStrptr(P&, const strptr& rhs)`      | Replace contents; clip if `rhs.n_elems > length`. |
| `Smallstr.AssignStrptr`     | `operator=(const strptr&)` (method)                 | Same as `SetStrptr`. |
| `Smallstr.CtorStrptr`       | `<Parent>::<Parent>(const strptr& rhs)` (method)    | Initialize from a span. |
| `Smallstr.Print`            | `void <name>_Print(P&, cstring& out)`               | Append the visible characters (strips pad on the appropriate side). |
| `Smallstr.ReadStrptrMaybe`  | `bool <name>_ReadStrptrMaybe(P&, strptr in)`        | Store the string and return true when it fits; a longer string leaves the field unchanged, appends errtext and returns false. |

### Memory model
<a href="#memory-model"></a>

- All storage is inline.  `sizeof(<Parent>)` includes `length`
  (or `length + 2` for rpascal) bytes for the string.
- No heap, no growth.  `SetStrptr`, `Add` and `AddStrptr` clip
  what exceeds `length`; `ReadStrptrMaybe` refuses it instead.
- The pad byte is the "no-data" marker — `_N` walks until the
  first/last non-pad to compute the logical length for padded
  strtypes.

### Pitfalls
<a href="#pitfalls"></a>

- **Silent clipping.**  Writing more than `length` characters
  through `SetStrptr`, `Add` or `AddStrptr` truncates — no
  error, no warning.  Validate at the call site if that
  matters.  `ReadStrptrMaybe` reports it instead, by storing
  nothing and returning false, so a caller that drops its
  return value keeps the field's previous contents.
- **An rpascal string carries an uninitialized tail.**  Init
  and the assignments write the length byte and the characters
  the value occupies, and no more, so the bytes past the length
  hold whatever was in that memory.  Copying the whole struct
  onto a wire frame copies them too.  The padded strtypes fill
  their whole storage, on Init and on each assignment.
- **`strict:Y` is opinionated.**  Mis-prefix or mis-suffix a
  ctype name and amc rejects it.  Stick to the catalog or
  use `strict:N`.
- **`rpascal` is limited to 255 chars** (the trailing length
  byte is `u8`).
- **Right-pad with a digit is rejected** for `numstr`
  fields — the pad character must not be confusable with a
  numeral.
- **A signed `numstr` is rejected when `'-'` is its left pad.**
  The left pad is stripped from the end the sign is written to,
  so the sign of a negative value goes with it and the value
  reads back positive.
- **Padded strings have a position-dependent semantic length.**
  Trailing spaces are part of the logical string for
  `rightpad` only if you treat them so; `_N` ignores them.

### See also
<a href="#see-also"></a>

- [Reftypes index](/txt/exe/amc/reftype.md)
- [Strings feature page](/txt/exe/amc/string.md) — numstr, conversion
- [Bitfld](/txt/exe/amc/reftype/Bitfld.md) — packing within an integer
- Source: `cpp/amc/smallstr.cpp`
- Tfunc records: `acr 'tfunc:Smallstr.%'`
- Spec table: `acr 'dmmeta.smallstr:%'`

### Example
<a href="#example"></a>

Standard 50-char pascal string used in dmmeta tables:

```
dmmeta.ctype  ctype:algo.Smallstr50
  dmmeta.field  field:algo.Smallstr50.ch  arg:char  reftype:Smallstr
    dmmeta.smallstr  field:algo.Smallstr50.ch  length:50  strtype:rpascal  pad:""  strict:Y
```

Wire-format-friendly right-padded string:

```
dmmeta.ctype  ctype:algo.RspaceStr8
  dmmeta.field  field:algo.RspaceStr8.ch  arg:char  reftype:Smallstr
    dmmeta.smallstr  field:algo.RspaceStr8.ch  length:8  strtype:rightpad  pad:"' '"  strict:Y
```

Numeric string with base-36 encoding:

```
dmmeta.ctype  ctype:algo.LnumStr11_U64_Base36
  dmmeta.field  field:....ch  arg:char  reftype:Smallstr
    dmmeta.smallstr  ...  length:11  strtype:leftpad  pad:"'0'"  strict:Y
  dmmeta.numstr  field:....ch  numtype:u64  base:36  min_len:11
```

Use:

```c++
algo::RspaceStr8 sym;
sym = "AAPL";       // calls operator= (clips/pads)
prlog(sym);         // Print strips trailing spaces
```

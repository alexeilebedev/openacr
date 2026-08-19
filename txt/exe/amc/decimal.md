## amc Feature: Decimals — Dec / fdec fixed-point
<a href="#amc-feature-decimals-dec-fdec-fixed-point"></a>

Fixed-point arithmetic is the right hammer for money, prices,
quantities, and most ratios where IEEE float's quiet rounding
is a bug.  amc's `dmmeta.fdec` annotation marks an integer
field as carrying a value with an **implied decimal scale**:
the bytes on the wire are an `i64` (or other int), but the
value's true meaning is "integer divided by 10^nplace".

amc generates conversion helpers that bridge integer storage
to/from `double` and decimal text — and a `Print` that emits
the value with the same number of decimal places, so the
round-trip through text is exact.

```
dmmeta.field   field:omx.Order.price  arg:i64  reftype:Val  dflt:""  comment:"price in dollars"
  dmmeta.fdec  field:omx.Order.price  nplace:9  fixedfmt:N
```

The above declares a price field whose integer storage is in
units of 10⁻⁹ — a `price` of `100_000_000_000` means $100.

### Table Of Contents
<a href="#table-of-contents"></a>
&nbsp;&nbsp;&bull;&nbsp;  [Ssim inputs](#ssim-inputs)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Generated functions](#generated-functions)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Fixed-format mode](#fixed-format-mode)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example](#example)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Pitfalls](#pitfalls)<br/>

### Ssim inputs
<a href="#ssim-inputs"></a>

Required:

- `dmmeta.field` with an integer `arg:` — one of the eight
  integer builtins `i8`, `i16`, `i32`, `i64`, `u8`, `u16`,
  `u32`, `u64`.  Any other `arg` carries no numeric range and
  is rejected.  A ctype whose single `Val` field holds one of
  those builtins stands for that builtin, and may be named as
  the `arg` as well.  Such a field stores the ctype, while
  every number the generated functions compute — the scale
  factor, the ends of the range, the integer portion, the value
  a text form parses to — is a number of the builtin, so a
  value crosses between the two at each end.  Reading it out of
  the ctype is the conversion operator `dmmeta.fcast`
  generates.  Constructing the ctype from a value is the
  fieldwise constructor a `dmmeta.cpptype` row with `ctor:Y`
  provides.  amc rejects an `arg` ctype carrying only one of
  the two, naming the one that is missing.  It rejects a ctype
  that reaches the builtin through a second ctype as well,
  because the fieldwise constructor of the outer one takes the
  inner ctype rather than the builtin.
  A `Val` field carries the scaled integer as its own
  member.  A `Bitfld` field carries it inside the word of the
  field it is packed into, in the bits its declared width names,
  and that width is what bounds the value it holds.
- `dmmeta.fdec`:
  - `nplace` — number of implied decimal places.  `nplace:2`
    means storage in cents; `nplace:9` means in nanos.  The
    count starts at 1: a count of zero leaves the generated
    reader with no fractional digit to accumulate.  Its upper
    limit is the smaller of two bounds.  Seventeen is where the
    table of powers of ten amc reads the scale factor from ends,
    so an eighteenth place has no scale factor to substitute.
    The magnitude the field's own storage holds is the other
    bound.  For a `Val` field that magnitude comes from the
    `arg`, because the generated `GetScale` returns the scale as
    that type: an `i32` field accepts at most nine places, since
    the tenth would return `10000000000` from a function
    returning `i32`.  For a `Bitfld` field it comes from the
    declared width instead, because the setter masks the store to
    that width: ten bits of a `u32` word hold 1023 and take three
    places.  amc rejects any other count naming the field, and
    states the range that field's storage allows.  A storage too
    narrow to hold ten — a `Bitfld` of three bits — holds no
    decimal place at all, and the diagnostic says that rather
    than naming an empty range.
  - `fixedfmt` — when true, `Print` always emits exactly
    `nplace` characters after the decimal point (no trimming
    of trailing zeros).

The scale factor amc uses is `10^nplace`, precomputed at
amc time via `algo::I64Power10(nplace)`.

### Generated functions
<a href="#generated-functions"></a>

Source: `cpp/amc/fdec.cpp`.  Function names are scoped under
the field name (e.g., `price_GetDouble`).

| Tfunc                  | Generated function                                            | Effect |
|------------------------|----------------------------------------------------------------|--------|
| `Dec.GetScale`         | `<arg> <Ctype>_GetScale()`                                     | Returns `10^nplace` — the multiplier converting raw → real value.  Named after the ctype and takes no argument, so it exists only for a ctype with one fdec field. |
| `Dec.GetInt`           | `<arg> <name>_GetInt(P&)`                                      | Returns the integer portion: the raw storage divided by `10^nplace`, truncated toward zero. |
| `Dec.GetDouble`        | `double <name>_GetDouble(P&)`                                  | Returns `field / 10^nplace` as a `double`. |
| `Dec.qSetDouble`       | `void <name>_qSetDouble(P&, double rhs)`                       | Stores `i64(rhs * 10^nplace)`.  No overflow check; "q" = quick. |
| `Dec.SetDoubleMaybe`   | `bool <name>_SetDoubleMaybe(P&, double rhs)`                   | Like qSetDouble but returns false if the converted value would overflow the integer storage. |
| `Dec.ReadStrptrMaybe`  | `bool <name>_ReadStrptrMaybe(P&, strptr in)`                   | Parse decimal text (`"3.14"`, `"-0.0001"`) into the integer storage; returns false on parse error or overflow. |
| `Dec.Print`            | `void <name>_Print(P&, cstring& out)`                          | Emit the value with `nplace` decimal places.  Strips trailing zeros unless `fixedfmt:Y`. |

There is no `_Set` / `_Get` for the integer form because the
scaled integer is the field's own storage, reached the same way
as any other field of its reftype.  A `Val` field is the member
itself, so `parent.<name>` reads and writes the scaled integer,
scale included.  A `Bitfld` field has no member of its own, so
the same storage is reached through the `<name>_Get` and
`<name>_Set` accessors amc generates for a packed field.  A
global ctype has no parent object, so its member is reached
through the namespace's own `_db` and its accessors — like every
function in the table above — are called with no argument.

### Fixed-format mode
<a href="#fixed-format-mode"></a>

| `fixedfmt:N` (default) | `fixedfmt:Y`            |
|------------------------|-------------------------|
| `100_000_000_000` → `"100"`              | `100_000_000_000` → `"100.000000000"` |
| `123_450_000_000` → `"123.45"`           | `123_450_000_000` → `"123.450000000"` |

`fixedfmt:Y` is useful for log lines and CSV exports where
each row should have a stable column width.  `fixedfmt:N` is
what most APIs want.

### Pitfalls
<a href="#pitfalls"></a>

- **No automatic overflow check on `qSetDouble`.**  A
  `qSetDouble(x, 1e30)` silently saturates to garbage.  Use
  `SetDoubleMaybe` if you can't trust your input.
- **Mixed `nplace` arithmetic.**  Adding two Dec fields with
  different scales requires manual rescaling — amc does not
  generate cross-type arithmetic helpers.
- **`double` is not exact at large magnitudes.**  Round-tripping
  through `GetDouble` / `qSetDouble` loses precision when
  `value > 2^53`.  For high-precision needs, work in the raw
  integer.
- **Parse rounding is half-toward-zero.**  `ReadStrptrMaybe`
  of `"1.555"` with `nplace:2` yields `155` (not `156`).
  Bank with care.
- **Negative numbers print correctly** in both fixed and
  trim modes; sign goes before the decimal point.
- **`fixedfmt:Y` does not pad the integer part.**  Use a
  Smallstr/Numstr wrapper if you need a fixed total width.

### See also
<a href="#see-also"></a>

- [Strings](/txt/exe/amc/string.md) — `Strfmt:String` Print/Read paths
- [Smallstr / Numstr](/txt/exe/amc/reftype/Smallstr.md) — fixed-width numeric Smallstrs
- [Big-endian fields](/txt/exe/amc/bigendian.md) — Dec is independent of byte order
- Source: `cpp/amc/fdec.cpp`
- Spec table: `acr 'dmmeta.fdec:%'`
- Tfunc records: `acr 'tfunc:Dec.%'`

### Example
<a href="#example"></a>

Money-cents:

```
dmmeta.field   field:billing.Invoice.amount  arg:i64  reftype:Val  dflt:""  comment:"USD"
  dmmeta.fdec  field:billing.Invoice.amount  nplace:2  fixedfmt:N
```

```c++
billing::Invoice inv;
inv.amount = 12345;                     // $123.45 (in cents)
prlog(amount_GetDouble(inv));            // 123.45
amount_qSetDouble(inv, 9.99);            // 999
amount_ReadStrptrMaybe(inv, "1234.56");  // 123456
amount_Print(inv, out);                  // "1234.56"
```

Nanosecond-precision price:

```
dmmeta.field   field:omx.Order.price  arg:i64  reftype:Val  dflt:""
  dmmeta.fdec  field:omx.Order.price  nplace:9  fixedfmt:N
```

Storage range with `nplace:9` is `i64 / 10^9` ≈ ±9.22e9 — more
than enough for any realistic asset price; small enough that
arithmetic stays in 64 bits.

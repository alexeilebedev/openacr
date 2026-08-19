## amc Reftype: Charset
<a href="#amc-reftype-charset"></a>

`Charset` declares a **set of characters** with a fast
`Match` predicate.  The set is described once in ssim using a
regex-style expression and amc compiles it to either a runtime
bitset or a sequence of inline range comparisons — whichever is
cheaper.  Use Charset for tokenizers, command-line argument
parsers, and any other place where "is this character one of
those?" is in the hot path.

```
dmmeta.field    field:algo_lib.FDb.ArgvIdent  arg:algo.Charset  reftype:Charset  dflt:""  comment:""
  dmmeta.charset  field:algo_lib.FDb.ArgvIdent  expr:"[a-zA-Z0-9_]"  charrange:Y  calc:N
```

### What it generates
<a href="#what-it-generates"></a>

Two compilation modes, picked by the `calc` flag on
`dmmeta.charset` (see `tclass_Charset` in
`cpp/amc/charset.cpp`):

1. **`calc:N` (runtime bitset)** — amc adds a storage field of
   type `algo.Charset` (256 bits) to the parent ctype and emits
   a hidden `<name>.Init` macro that calls
   `Charset_ReadStrptrMaybe(parent.<name>, "[expr]")` at parent
   construction time.  `Match(ch)` is a single bit-test.
2. **`calc:Y` (compile-time ranges)** — no storage; amc
   precomputes the character set at code-gen time and emits
   `Match(ch)` as a chain of range comparisons (e.g.,
   `(ch-'a') < 26 || (ch-'A') < 26 || (ch-'0') < 10 || ch == '_'`).
   Branchless and constant-foldable; ideal for sets that
   compile down to one or two ranges.

The expression parser is configured by the `charrange` flag:

- `charrange:Y` — interpret `expr` as a regex-style character
  class (`[a-z0-9_]`, escapes, etc.).
- `charrange:N` — treat `expr` as a literal sequence of
  characters (each character is in the set; no ranges).

### Ssim inputs
<a href="#ssim-inputs"></a>

Required:

- `dmmeta.field` with `reftype:Charset`.  `arg:` is conven­tionally
  `algo.Charset` (the 256-bit storage type), but the field arg
  is largely cosmetic when `calc:Y`.
- `dmmeta.charset`:
  - `expr` — the character-set expression.  Examples:
    `"[A-Za-z0-9_]"`, `"\"\\\\"`, `"\\s"`.
  - `charrange:Y/N` — regex-style vs literal character list.
  - `calc:Y/N` — compile-time inline ranges vs runtime bitset.

The set is constructed at amc time via `algo::Charset_ReadStrptrMaybe`
(when `charrange:Y`) or by bit-setting each character of `expr`
(when `charrange:N`) — see `gen_newfield_charset` at line 36.

### Generated functions
<a href="#generated-functions"></a>

Source: `cpp/amc/charset.cpp`.

| Tfunc            | Generated function          | Effect |
|------------------|-----------------------------|--------|
| `Charset.Match`  | `bool <name>Q(u32 ch)`      | True iff `ch` is in the set.  The function name ends in `Q` by convention (a predicate). |

Note: the function name is `<name>Q`, not `<name>_Match`.  This
matches the project's predicate naming convention.

For `calc:N`, an Init contribution to the parent ctype loads
the bitset from the expression string.  No `tfunc_Charset_Init`
record — amc folds the initializer into the parent's own
`Init`.

### Memory model
<a href="#memory-model"></a>

- **`calc:Y`**: no storage, no Init, no allocation.  The set
  is baked into the generated `MatchQ` body.
- **`calc:N`**: 32-byte `algo.Charset` field in the parent
  ctype, populated at construction.  Cheap to test, cheap to
  store.

### Pitfalls
<a href="#pitfalls"></a>

- **`calc:Y` is locked at amc time.**  If you need to compose
  charsets dynamically (e.g., user-configurable separators),
  use `calc:N`.
- **Only 8-bit characters.**  Both modes use a 256-element
  bitset; non-ASCII / multibyte UTF-8 sequences are matched
  byte-by-byte, not codepoint-by-codepoint.
- **No `_Set` / `_Insert` accessors.**  Charset is read-only
  at runtime — the set is fixed at construction.  Use a
  hand-managed `Bitset` field if you need mutability.
- **Function name is `<name>Q`**, not `<name>_Match`.  The
  predicate naming convention.

### See also
<a href="#see-also"></a>

- [Reftypes index](/txt/exe/amc/reftype.md)
- [Charsets feature page](/txt/exe/amc/charset.md)
- [Bitsets](/txt/exe/amc/bitset.md) — for mutable bit-vector fields
- Source: `cpp/amc/charset.cpp`
- Tfunc records: `acr 'tfunc:Charset.%'`
- Spec table: `acr 'dmmeta.charset:%'`

### Example
<a href="#example"></a>

The argv-identifier charset (what's a valid command-line flag
character):

```
dmmeta.field    field:algo_lib.FDb.ArgvIdent  arg:algo.Charset  reftype:Charset  dflt:""
  dmmeta.charset  field:algo_lib.FDb.ArgvIdent  expr:"[a-zA-Z0-9_]"  charrange:Y  calc:N
```

In C++:

```c++
const char *p = arg;
while (ArgvIdentQ(*p)) p++;        // skip identifier characters
```

A compile-time-inline variant — e.g., recognising whitespace
in a tight scanner loop:

```
dmmeta.field    field:lib_lex.FDb.Ws  arg:algo.Charset  reftype:Charset  dflt:""
  dmmeta.charset  field:lib_lex.FDb.Ws  expr:" \t\n\r"  charrange:N  calc:Y
```

Generates:

```c++
inline bool WsQ(u32 ch) {
    bool ret = false;
    ret |= ch == ' ';
    ret |= ch == '\t';
    ret |= (ch - '\n') < 2;        // \n, \r happen to be consecutive
    return ret;
}
```

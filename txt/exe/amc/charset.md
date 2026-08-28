## amc Feature: Charsets — Charset reftype, Match
<a href="#amc-feature-charsets-charset-reftype-match"></a>

Character classification (is this byte a digit, identifier
character, path separator, …?) shows up in every tokenizer.
amc lets you describe a character set once in a ssim record,
and it compiles a `MatchQ` predicate in one of two forms
depending on the set's structure.  Charsets live as FDb-level
fields (typically on `algo_lib.FDb`) and the names are part
of the public algo_lib API used everywhere from `acr` to the
amc command-line parser.

```ssim
dmmeta.field      field:algo_lib.FDb.IdentChar  arg:algo.Charset  reftype:Charset  dflt:""
  dmmeta.charset  field:algo_lib.FDb.IdentChar  expr:a-zA-Z0-9_  charrange:Y  calc:Y
```

### Table Of Contents
<a href="#table-of-contents"></a>
&nbsp;&nbsp;&bull;&nbsp;  [Compilation modes](#compilation-modes)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Expression syntax](#expression-syntax)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Ssim inputs](#ssim-inputs)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Generated functions](#generated-functions)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Standard charsets in algo_lib](#standard-charsets-in-algo_lib)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example](#example)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Pitfalls](#pitfalls)<br/>

### Compilation modes
<a href="#compilation-modes"></a>

amc compiles a Charset to one of two forms (see
`tclass_Charset` and `GenCalcMatch` in `cpp/amc/charset.cpp`):

| Mode      | `calc` flag | Storage | Body of `<name>Q(ch)` |
|-----------|-------------|---------|------------------------|
| Bitset    | `N` (default) | 32-byte `algo.Charset` field in the parent | One bit test: `ch_GetBit(parent.<name>, ch)`. |
| Inline range | `Y`     | none — no storage in parent             | Series of inline range comparisons folded at compile time. |

For `calc:Y`, amc walks the precomputed bitset at amc time,
finds runs of consecutive 1-bits, and emits one comparison per
run:

```c++
inline bool IdentCharQ(u32 ch) {
    bool ret = false;
    ret |= (ch - '0') < 10;
    ret |= (ch - 'A') < 26;
    ret |= ch == '_';
    ret |= (ch - 'a') < 26;
    return ret;
}
```

The compiler typically lowers each range to a single subtract
+ compare, so a 4-range charset costs ~4 ALU ops.  Use `calc:Y`
for charsets that are dense and static (digits, identifier
chars, whitespace) and `calc:N` for charsets that need to be
loaded from a string at startup or computed at runtime.

### Expression syntax
<a href="#expression-syntax"></a>

The `expr` field is parsed by `algo::Charset_ReadStrptrMaybe`.
`charrange:Y` enables regex-style ranges:

| `expr`        | Meaning                                  |
|---------------|------------------------------------------|
| `a-zA-Z0-9_`  | Letters, digits, and underscore.         |
| `0-9`         | Just decimal digits.                     |
| `\\s`         | Whitespace (`\t\n\r `).  Backslash-escape. |
| `[]{}()`      | Literal brackets (no character class meaning here). |

`charrange:N` treats `expr` as a **literal list of characters**
— every byte of the string is in the set, no ranges.  Useful
when ranges would be ambiguous (sets containing `-`).

The parser also accepts `\\n`, `\\t`, `\\r`, `\\\\`, and
`\\"` escapes.

### Ssim inputs
<a href="#ssim-inputs"></a>

Required:

- `dmmeta.field` with `reftype:Charset`.  `arg:` is
  conventionally `algo.Charset` (the bitset storage struct)
  for `calc:N`.  For `calc:Y` the `arg` is cosmetic — there is
  no storage.
- `dmmeta.charset`:
  - `expr` — the character-set expression.
  - `charrange:Y/N` — regex-style ranges vs. literal list.
  - `calc:Y/N` — compile-time inline ranges vs. runtime bitset.
  - `comment` — free text.

### Generated functions
<a href="#generated-functions"></a>

For every Charset field, amc emits exactly one accessor:

| Name         | Signature                  | Effect |
|--------------|----------------------------|--------|
| `<name>Q`    | `bool <name>Q(u32 ch)`     | True iff `ch` is in the set. |

Note the predicate-naming convention — the function ends in
`Q` rather than starting with `<name>_Match`.

For `calc:N`, a hidden contribution to the parent's `_Init`
loads the bitset from the `expr` string via
`Charset_ReadStrptr*` so the set is ready by the time any
caller reaches `<name>Q`.

### Standard charsets in algo_lib
<a href="#standard-charsets-in-algo_lib"></a>

Every project gets these out of the box (defined in
`ssimfile:dmmeta.charset`):

| Charset            | Members           | Mode | Use |
|--------------------|-------------------|------|-----|
| `AlphaCharQ`       | `a-zA-Z`          | calc | Letters |
| `DigitCharQ`       | `0-9`             | calc | Digits |
| `HexCharQ`         | `0-9a-fA-F`       | calc | Hex digits |
| `IdentStartQ`      | `a-zA-Z_`         | calc | First char of an identifier |
| `IdentCharQ`       | `a-zA-Z0-9_`      | calc | Identifier body |
| `WhiteCharQ`       | `\\t \\n\\r`      | calc | ASCII whitespace |
| `DirSepQ`          | `/\\`             | calc | Path separators |
| `ArgvIdentQ`       | `a-zA-Z0-9_`      | bitset | Command-line flag chars |
| `BashQuotesafeQ`   | `a-zA-Z0-9_^%@./-` | bitset | Chars that need no shell quoting |
| `CmdLineNameBreakQ`| `[]{}()\\t \\r\\n:` | calc | Argv parser separators |

The `Q` naming convention plus the `algo_lib::` namespace mean
that calling `algo_lib::IdentCharQ('a')` from anywhere yields
the right answer without ceremony.

### Pitfalls
<a href="#pitfalls"></a>

- **`calc:Y` is frozen at amc time.**  If you need to mutate
  the set at runtime (e.g., user-configurable separators), use
  `calc:N` and re-`Read` the underlying `algo.Charset`.
- **8-bit only.**  Both modes use a 256-element bitset.
  UTF-8 multibyte sequences are matched byte-by-byte, not by
  codepoint.
- **Predicate names live in the namespace of the parent
  ctype.**  Most charsets live on `algo_lib.FDb`, so they end
  up as `algo_lib::<name>Q`.  Don't fight the convention.
- **Literal `-` in `charrange:Y` expressions** must be either
  first/last or escaped, since `-` is the range marker.  Use
  `charrange:N` if you have many literal punctuation
  characters.
- **Empty sets** are legal; the generated `MatchQ` always
  returns false.

### See also
<a href="#see-also"></a>

- [Charset reftype](/txt/exe/amc/reftype/Charset.md) — the field-level reference
- [Bitsets](/txt/exe/amc/bitset.md) — the underlying machinery for `calc:N`
- [Command-line parsing](/txt/exe/amc/cmdline.md) — `CmdLineNameBreakQ` / `BashQuotesafeQ`
- Source: `cpp/amc/charset.cpp`
- Spec table: `acr 'dmmeta.charset:%'`

### Example
<a href="#example"></a>

Walking an identifier at the cursor:

```c++
const char *p = start;
if (!algo_lib::IdentStartQ(*p)) return false;
while (algo_lib::IdentCharQ(*p)) p++;
```

A custom set for a new project:

```ssim
dmmeta.field      field:foo.FDb.OperatorChar  arg:algo.Charset  reftype:Charset
  dmmeta.charset  field:foo.FDb.OperatorChar  expr:"+-*/%<>="  charrange:N  calc:Y
```

```c++
if (foo::OperatorCharQ(c)) handle_operator(c);
```

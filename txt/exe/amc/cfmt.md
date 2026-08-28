## amc Feature: cfmt — print/read format declarations
<a href="#amc-feature-cfmt-print-read-format-declarations"></a>

`dmmeta.cfmt` is the universal opt-in for **textual
serialization**.  One cfmt row says "for ctype C, generate
a print and/or read function in format F."  amc walks all
cfmt rows and dispatches each one to the appropriate
emitter — String, Json, Argv, Tuple — producing the print and
read functions that strfmt defines.  The String strfmt's pair,
`_Print(out)` and `_ReadStrptrMaybe(in)`, round-trips your
in-memory struct through text.

Most ssim files in the repo are themselves
`printfmt:Tuple`-formatted; the same machinery that loads
your `dmmeta.field` ssim is what handles your application
data.

```ssim
dmmeta.cfmt  cfmt:algo.LineBuf.String   printfmt:Tuple        read:Y print:Y sep:""   genop:N
dmmeta.cfmt  cfmt:net.TcpHdr.String     printfmt:Sep          read:Y print:Y sep:":"  genop:N
dmmeta.cfmt  cfmt:net.MsgList.Json      printfmt:Auto         read:N print:Y sep:""   genop:N
dmmeta.cfmt  cfmt:command.abt.Argv      printfmt:Auto         read:Y print:Y sep:""   genop:N
```

### Table Of Contents
<a href="#table-of-contents"></a>
&nbsp;&nbsp;&bull;&nbsp;  [The cfmt key](#the-cfmt-key)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [printfmt: the layout variant](#printfmt-the-layout-variant)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [The String × Tuple combination](#the-string-tuple-combination)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [The String × Sep combinations](#the-string-sep-combinations)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Json](#json)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Argv and ArgvGnu](#argv-and-argvgnu)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Extern](#extern)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Pitfalls](#pitfalls)<br/>

### The cfmt key
<a href="#the-cfmt-key"></a>

The pkey of a cfmt row is `<ctype>.<strfmt>` — a ctype can
have at most one cfmt per strfmt.  The strfmt is a pkey of
`dmmeta.strfmt`:

| `strfmt`     | Conversion target |
|--------------|--------------------|
| `String`     | Generic text form (`_Print`, `_ReadStrptrMaybe`) — the default. |
| `Tuple`      | Attrs of an already-parsed `algo::Tuple` (`_ReadTupleMaybe`) — read only. |
| `Json`       | JSON node tree (`_FmtJson`) — print only. |
| `Argv`       | Unix command line argv array. |
| `ArgvGnu`    | GNU-style argv (`--name value` instead of `-name value`). |

The `printfmt` field further refines the layout for the
String strfmt; see below.

### printfmt: the layout variant
<a href="#printfmt-the-layout-variant"></a>

For `strfmt:String`, the `printfmt` decides how the fields
are laid out in the resulting string:

| `printfmt`    | Output shape                                                |
|---------------|--------------------------------------------------------------|
| `Tuple`       | `ctype  key1:value1  key2:value2  ...`  — ssim form. |
| `Sep`         | `value1<sep>value2<sep>...`  — one-char-per-gap separator. |
| `CompactSep`  | Declared but not generated; a `print:Y` cfmt using it is rejected. |
| `Raw`         | Concatenated fields with no separator (for single-field wrappers and binary-format displays). |
| `Auto`        | Same shape as `Tuple`, for printing and for reading alike. |
| `Bitset`      | Comma-separated names of set bits (only for `fbitset` fields). |
| `Extern`      | User-implements the body; amc emits only the prototype. |

For non-String strfmts (`Json`, `Argv`, …), only `Extern`
changes what is printed: it hands the body to the user as it
does under String.  Any other value selects the strfmt's own
single layout, so a row prints the same whichever of them it
carries — most rows in the tree carry `Tuple` and the rest
carry `Auto`.

### The String × Tuple combination
<a href="#the-string-tuple-combination"></a>

The default and most-used combination: `strfmt:String  printfmt:Tuple`.
Output looks like a ssim tuple:

```ssim
dmmeta.field  field:dmmeta.Field.field  arg:dmmeta.Field  reftype:Pkey  dflt:""  comment:""
```

Print rules (see [strings.md](/txt/exe/amc/string.md) for
the full list):

- Only fields with reftype `Varlen`, `Val`, `Inlary`,
  `Smallstr`, `Ptr`, `Opt`, `Bitfld`, `RegxSql` are emitted.
- `Substr` / `Cppfunc` (computed) fields are skipped.
- `Pmaskfld` / `Lenfld` / `Typefld` are skipped (driven by
  other field state).
- With a presence mask (`pmask`), absent fields are skipped.
- Bool bitfields over `anonfld` are emitted only when true.
- `Opt` is emitted only if present.

Reading is symmetric — amc parses the `ctype` tag, looks up
the ctype, then walks `key:value` pairs.

### The String × Sep combinations
<a href="#the-string-sep-combinations"></a>

`Sep` is for ctype-as-tuple-of-positional-values output.  The
`sep` attribute is a string of separator characters — one
character between each pair of fields:

```ssim
dmmeta.cfmt  cfmt:net.Ipport.String  printfmt:Sep  read:Y print:Y sep:":"
```

…produces `ip:port` form.  A multi-character sep like `":-"`
means use `:` between fields 1-2 and `-` between fields 2-3.
The number of chars in `sep` must equal `n_fields - 1`.

`CompactSep` names a second separated layout, one that would
emit a default-valued field as an empty string and accept
empty fields on input.  No code generates for it: amc rejects
a `print:Y` cfmt whose printfmt is `CompactSep`, naming the
cfmt and the printfmt.

### Json
<a href="#json"></a>

```ssim
dmmeta.cfmt  cfmt:net.MsgList.Json  printfmt:Auto  read:N  print:Y  sep:""
```

A Json cfmt generates one function,
`lib_json::FNode *<Ctype>_FmtJson(<Ctype> &row, lib_json::FNode *parent)`.
It builds the row's JSON representation under the `parent`
node and returns the node it added.  The Json strfmt has no
reader at all: amc generates a reader for the String strfmt and
for the Tuple strfmt and for no other, so `read:Y` on a Json
cfmt turns no generator on and reports nothing.

The print walk visits the ctype's data fields, and what a field
contributes is decided by its reftype.  A `Val` field becomes a
member of the row's object — a nested object when the field's
own type carries a Json cfmt of its own, and otherwise a string
node holding that type's String print.  A `Tary` or `Inlary`
field becomes an array, whose elements are written the same two
ways.  A `Base` field adds no node of its own, because its
fields print as members of this same walk, and a `Global` field
adds none either, because it is the anchor of the namespace's
singleton instance rather than a value.  A field computed from
other fields — `Cppfunc`, `Substr`, `Falias` — stores nothing
of its own and is skipped.  A `Bitfld` field is skipped for the
same reason: its bits live in the word of the field it is
packed into, and that word is what the walk prints.  A ctype
whose one data field produces a node prints as that node alone
rather than as an object wrapped around it, so a single-field
wrapper reaches the consumer as the value it wraps.

Every other reftype is a reported rejection.  `Thash`,
`Smallstr`, `Ptr`, `Ptrary` and the rest draw `amc.badjsonfld`,
which names the cfmt, the field and the field's reftype, and
the run exits nonzero.  A field the walk cannot represent
therefore stops the build instead of quietly disappearing from
the output.  The two reftypes that do produce nodes draw the
same diagnostic when the type they hold has neither a Json cfmt
nor a String print, since neither way of writing the member is
then available.

A value written through its String print lands in a JSON string
node whatever its type, so a numeric field reaches the consumer
quoted.  For `i64` and `u64` that is what a JavaScript consumer
needs, because a JSON number there is a double and loses
precision past 53 bits; see the
[JS feature page](/txt/exe/amc/js.md) for the `bigint` decoder
that reads them back.

Each field renders through a printer of its own type, and
the type must have one of the two:

- The type's Json cfmt, when it has one with `print:Y` — the
  field becomes whatever that type's `_FmtJson` builds.
- Otherwise the type's String cfmt — the field becomes a
  JSON string holding what `_Print` writes.  A field of type
  `algo.Smallstr20` reaches JSON this way: that ctype has a
  String printer and no Json cfmt.

Which fields render, by reftype:

- `Val` → a value of the field's type.
- `Tary` / `Inlary` → a JSON array whose elements each render
  as a value of the element type.
- `Base`, `Bitfld`, `Global`, and computed fields (`Cppfunc`,
  `Substr`, `Alias`) contribute no node of their own: a
  base's cloned fields render as members of this same object,
  and a bitfield's bits travel inside its printed source
  word.
- Any other reftype — `Thash`, `Smallstr`, `Ptrary`, `Opt`, …
  — is rejected with `amc.badjsonfld`.  A field whose type
  carries neither a Json nor a String printer is rejected the
  same way, rather than calling a function amc never emits.

`i64` and `u64` render as JSON numbers.  The TypeScript side
types them as `bigint`, because JS numbers lose precision
beyond 2^53; see the [JS feature page](/txt/exe/amc/js.md).

### Argv and ArgvGnu
<a href="#argv-and-argvgnu"></a>

```ssim
dmmeta.cfmt  cfmt:command.abt.Argv  printfmt:Auto  read:Y  print:Y  sep:""
```

The Argv strfmt drives the **command-line printer** that
[Exec](/txt/exe/amc/reftype/Exec.md) and
[cmdline](/txt/exe/amc/cmdline.md) rely on to round-trip a
command struct through an `argv[]`.  They are separate
strfmts, not two settings of one: each is the last component
of its own cfmt key, so `cfmt:command.abt.Argv` selects the
amc form and `cfmt:command.bash.ArgvGnu` the GNU form.

The strfmt is what selects the argv printer, so an argv
cfmt's `printfmt` does not choose between those two forms; on
the print side it behaves as the rule above describes for any
non-String strfmt.  It is not inert on the read side, and the
`read:Y` paragraphs below say what it decides there.

The two differ in how one option becomes tokens:

- `Argv` — amc-style: one token, `-name:value`.
- `ArgvGnu` — GNU-style: two tokens, the option and then its
  value.  The dash count follows the length of the option
  name — a one-character name prints as `-x`, anything
  longer as `--name`.

`print:Y` generates the same two printers for either strfmt:
`$Name_PrintArgv`, which appends the tokens to a string, and
`$Name_ToArgv`, which appends them to an `algo::StringAry` as
separate elements.  `ToArgv` also fills in element 0, the
command path: `bin/$Name` for `Argv`, bare `$Name` for
`ArgvGnu`, since a GNU-style callee is usually found on
`PATH` rather than in this repo's `bin`.

An `Argv` cfmt gets a third printer, `$Name_ToCmdline`, which
returns `bin/$Name` followed by the same tokens and passes
down one fewer `-verbose` / `-debug` than the current process
carries.  `ArgvGnu` does not get it, because that convenience
only makes sense when the callee is itself an amc binary.

`read:Y` generates the argv reader on an `Argv` cfmt alone,
and it generates two functions.  The first is `$Name_NArgs`,
the table that says how many words follow each option.  The
second is `$Name_ReadArgv`, the field-aware parser that fills
the struct from an already-split word array, consulting that
table so a bare `-opt` can consume the following word.  The
namespace-level `ReadArgv` that amc's `main()` calls does the
argv-to-word-array conversion and then delegates to
`$Name_ReadArgv`, so a cmdline ctype must carry `read:Y`.
Under `read:N` the parser is never emitted, and amc reports
the namespace's call to it as `amc.badargvread` instead of
leaving the missing symbol to the C++ link.

The parser does not stand alone either.  It stores each value
it reads through `$Name_ReadFieldMaybe`, and it takes the next
positional from `$Name_GetAnon`.  When the ctype carries a
`basecmdline` it reads the base's options through that ctype's
`NArgs` and `ReadFieldMaybe` in the same pass.  Those two
functions come from a `read:Y` cfmt whose `printfmt` is neither
`Raw` nor `Extern`, which is not the condition that emits the
parser: an `Argv` cfmt spelled `printfmt:Raw` yields a parser
with nowhere to store what it reads.  A second `read:Y` cfmt
with an ordinary printfmt supplies them, and so satisfies both
conditions at once.  Every companion the parser would call and
amc does not generate is reported as `amc.badargvread`, naming
the row the ctype is missing.

`read:Y` on an `ArgvGnu` cfmt gets neither of those two.  It
still turns on the read-side pieces that any `read:Y` cfmt
turns on regardless of strfmt — the `$Name_ReadFieldMaybe`
attribute dispatcher, and `$Name_GetAnon` when the ctype has
anonymous fields — so the flag is not inert, just useless:
nothing generated from an `ArgvGnu` cfmt can parse a command
line.  A cmdline ctype whose only argv cfmt is `ArgvGnu` draws
`amc.badargvread` for the same reason a `read:N` `Argv` cfmt
does.

### Extern
<a href="#extern"></a>

When `printfmt:Extern`, amc emits only the forward
declaration of the print/read function and expects you to
implement it.  Useful when amc can't represent the format
(custom XML, binary protocols with non-amc framing, etc.).

### Pitfalls
<a href="#pitfalls"></a>

- **One cfmt per strfmt per ctype.**  Conflicting cfmts for
  the same ctype + strfmt are an error.
- **`sep` length must match `n_fields - 1`** for the Sep
  variant.  amc errors otherwise.
- **`genop:Y`** generates `operator<<` for the Print.
  Without it you must call `_Print` explicitly.
- **String × Tuple is the canonical ssim form.**  Every
  ssimfile is read by this exact codec — don't override it
  unless you really mean to.
- **A Json value printed through a String print is a JSON
  string.**  For `i64` and `u64` that is intentional (JS
  precision); for the narrower types it surprises consumers
  that expect numeric JSON.
- **Argv is opt-out by default for most ctypes.**  You only
  need it for `command.<exe>` ctypes (and ones that are
  used as `Exec`-style arguments).
- **Auto printfmt is conservative.**  It picks Raw only
  for trivial single-field wrappers; everything else gets
  Tuple.

### See also
<a href="#see-also"></a>

- [Strings — String × Tuple, Json, Argv, Sep](/txt/exe/amc/string.md) — deeper walk through the variants
- [Enums](/txt/exe/amc/enum.md) — fconst-driven enum printing
- [Pmask](/txt/exe/amc/pmask.md) — presence-driven field skipping in Print
- [Command-line parsing](/txt/exe/amc/cmdline.md) — consumer of Argv
- [I/O](/txt/exe/amc/io.md) — finput uses the String × Tuple cfmt
- Source: `cpp/amc/print.cpp`, `cpp/amc/read.cpp`, `cpp/amc/cmdline.cpp`
- Spec tables: `acr 'dmmeta.cfmt:%'`, `acr 'strfmt:%'`, `acr 'printfmt:%'`

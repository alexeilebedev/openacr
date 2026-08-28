## amc Feature: Regular expressions — Regx, RegxSql
<a href="#amc-feature-regular-expressions-regx-regxsql"></a>

amc has built-in support for compiled regular expressions as
**typed fields**.  A `reftype:Regx` field stores an
`algo_lib::Regx` (the compiled state); amc emits the `_Init`
/ `_Print` / `_ReadStrptrMaybe` glue and selects the **regex
flavor** based on `dmmeta.fregx.regxtype`.  Four flavors are
supported — each tuned for a different use case.

```ssim
dmmeta.field   field:command.abt.target  arg:dev.Target  reftype:RegxSql  dflt:""
  dmmeta.fregx  field:command.abt.target  regxtype:Sql  partial:N
```

### Table Of Contents
<a href="#table-of-contents"></a>
&nbsp;&nbsp;&bull;&nbsp;  [Regex flavors](#regex-flavors)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Ssim inputs](#ssim-inputs)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Matching API](#matching-api)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Partial vs full matches](#partial-vs-full-matches)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example](#example)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Pitfalls](#pitfalls)<br/>

### Regex flavors
<a href="#regex-flavors"></a>

Four regxtypes are catalogued in `amcdb.regxtype`:

| `regxtype` | Wildcards / specials                                  | Use case                                      |
|------------|-------------------------------------------------------|------------------------------------------------|
| `Sql`      | `%` = any string, `_` = any char; escape with `\\`   | acr queries, pkey patterns, `-target:abt%`     |
| `Acr`      | Full regex syntax with attribute matching (extended) | acr's `-where:key:value` clauses               |
| `Shell`    | Shell-style globs (`*`, `?`, `[abc]`)                 | File-path matchers                             |
| `""` (Normal) | POSIX-ish regex (`.`, `*`, `+`, `[]`, `^`, `$`)    | Free-form text scanning                        |

amc generates the same C++ shape for all four — only the
parser used at `_Init` / `_ReadStrptrMaybe` time differs.
Field types in C++ are all `algo_lib::Regx`; the regxtype is
encoded inside the compiled state.

### Ssim inputs
<a href="#ssim-inputs"></a>

| Record       | Role |
|--------------|------|
| `dmmeta.field` with `reftype:Regx` or `reftype:RegxSql` | Declares the field. |
| `dmmeta.fregx field:<full> regxtype:<kind> partial:Y/N` | Required for every Regx field. |
| `dmmeta.fregx.regxtype` | One of the four flavors. |
| `dmmeta.fregx.partial`  | `Y` = match anywhere in the input; `N` = anchor to the whole input. |

[RegxSql](/txt/exe/amc/reftype/RegxSql.md) is shorthand for
`reftype:Regx + regxtype:Sql` with a hard requirement that
the fregx record exist.  By convention, command-line `target`
/ `cijob` / `citest` selectors use RegxSql for SQL-style
wildcards.

### Matching API
<a href="#matching-api"></a>

amc generates only the storage/init/print/read functions.
**Matching itself is a runtime call** — `algo::Regx_Match`:

```c++
algo_lib::Regx &r = cmd.target;       // compiled Regx field
if (Regx_Match(r, candidate)) {
    ...
}
```

There is no `<name>_Match` accessor because matching is
fundamental to `algo_lib::Regx` and the same call works
regardless of the field's flavor.

The compiled regex carries everything it needs (NFA/DFA
state, match mode, partial flag) — `Regx_Match(r, s)` is the
universal entry point.

### Partial vs full matches
<a href="#partial-vs-full-matches"></a>

`partial:N` (default for pkey-style fields) anchors the match
to the entire input:

```bash
target_ReadStrptrMaybe(cmd, "abt%");     // pattern
Regx_Match(cmd.target, "abt_md");        // true
Regx_Match(cmd.target, "atf_abt");        // false — anchored
```

`partial:Y` allows the match anywhere in the input:

```c++
target_ReadStrptrMaybe(cmd, "abt%");
Regx_Match(cmd.target, "atf_abt_test");  // true
```

For SQL-style patterns, `partial:N` is almost always what you
want — that's how `LIKE 'abt%'` works in SQL: anchored.
`partial:Y` is for "contains" semantics typical of free-form
search inputs.

### Pitfalls
<a href="#pitfalls"></a>

- **`fregx` is mandatory.**  Without a fregx record, amc
  refuses to generate a Regx codec (errors with
  `missing fregx`).
- **Wrong regxtype = surprising matches.**  Using `Normal`
  on what was meant as a SQL pattern silently changes
  meaning (`%` is literal under Normal).  Pick the regxtype
  to match the user's mental model.
- **Compilation is per-Read.**  Every `_ReadStrptrMaybe`
  recompiles the pattern.  Cache externally if you parse
  the same pattern frequently.
- **Errors are silent.**  A malformed pattern leaves the
  Regx in a state that matches nothing.  Check
  `algo_lib::_db.errtext` after `_ReadStrptrMaybe` for
  diagnostics.
- **`partial:Y` makes patterns "contains" by default** —
  fine for grep-like UX, surprising for "exact-match"
  expectations.
- **No `Match` tfunc.**  Match is a runtime function, not a
  generated wrapper.  Don't go looking in `tfunc:Regx.%` for
  one.

### See also
<a href="#see-also"></a>

- [Regx reftype](/txt/exe/amc/reftype/Regx.md) — field-level reference
- [RegxSql reftype](/txt/exe/amc/reftype/RegxSql.md) — SQL-style sibling
- [Command-line parsing](/txt/exe/amc/cmdline.md) — most common consumer
- [Charsets](/txt/exe/amc/charset.md) — for character-class predicates (faster than regex for single-char tests)
- Source: `cpp/amc/regx.cpp`
- Spec table: `acr 'dmmeta.fregx:%'`
- Regxtype catalog: `acr 'regxtype:%'`

### Example
<a href="#example"></a>

A command-line flag that selects build targets:

```ssim
dmmeta.field   field:command.abt.target  arg:dev.Target  reftype:RegxSql  dflt:""
  dmmeta.fregx  field:command.abt.target  regxtype:Sql  partial:N
```

```c++
ind_beg(_db_target_curs, target, _db) {
    if (Regx_Match(cmd.target, target.target)) {
        process(target);
    }
} ind_end;
```

CLI invocation: `abt -target:atf% -build`.

A free-form search filter:

```ssim
dmmeta.field   field:foo.Cfg.namepat  arg:algo.Regx  reftype:Regx
  dmmeta.fregx  field:foo.Cfg.namepat  regxtype:Normal  partial:Y
```

Match results vary based on the regxtype:

| Pattern | Sql matches             | Acr matches             | Normal matches             | Shell matches            |
|---------|-------------------------|-------------------------|----------------------------|--------------------------|
| `abt%`  | starts-with abt         | starts-with abt + attr  | literal `abt%`             | starts-with abt          |
| `abt*`  | literal `abt*`          | literal `abt*`          | `abt` followed by any 0+   | starts-with abt          |
| `a.b`   | literal `a.b`           | (attribute notation)    | `a` + any + `b`            | literal `a.b`            |

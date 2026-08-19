## amc Reftype: Regx
<a href="#amc-reftype-regx"></a>

`Regx` is a **compiled regular-expression field**.  The field's
type in C++ is `algo_lib::Regx`; amc emits a `_Init` body that
compiles a default pattern (if any) at parent construction
time, and a `_ReadStrptrMaybe` / `_Print` pair that re-compile
or print the pattern on demand.

The kind of regex (POSIX-ish vs. SQL-ish) is selected by the
`fregx.regxtype` ssim field — `Normal` (default) or `Sql`.
The companion reftype **`RegxSql`** is the same code generator
with `regxtype` defaulted to `Sql` and an extra
`fregx`-required check.

```
dmmeta.field   field:command.abt.target  arg:dev.Target  reftype:Regx  dflt:""  comment:"Sql Regx of dev::Target"
  dmmeta.fregx  field:command.abt.target  regxtype:Sql  partial:N
```

### What it generates
<a href="#what-it-generates"></a>

One field on the parent (see `tclass_Regx` in
`cpp/amc/regx.cpp`):

| Field        | Type             | Meaning |
|--------------|------------------|---------|
| `<name>`     | `algo_lib::Regx` | compiled regex state |

Init compiles the `dflt:` pattern (if any) at parent
construction time using `Regx_Read<Regxtype>`.  The `partial`
flag from `dmmeta.fregx` selects partial vs. full-string match
semantics (passed as the third argument to
`Regx_Read<Regxtype>`).

### Regx vs RegxSql
<a href="#regx-vs-regxsql"></a>

|              | **Regx**                          | **RegxSql**                  |
|--------------|-----------------------------------|------------------------------|
| `regxtype`   | default `Normal`                  | always `Sql`                  |
| Syntax       | classic regex (`.`, `*`, `+`, `[]`, `^$`, …) | SQL-style (`%`, `_`, escapes) |
| Use case     | text matching                     | acr queries, pkey patterns    |
| Code path    | `tfunc_Regx_*`                    | identical (RegxSql delegates) |

In `amcdb.gen:rewrite_regx`, every `RegxSql` field is rewritten to
plain `Regx` with `regxtype:Sql` before code generation —
which is why the two reftypes share the same tfunc bodies.

### Ssim inputs
<a href="#ssim-inputs"></a>

Required:

- `dmmeta.field` with `reftype:Regx` (or `RegxSql`).  `arg:` is
  the relational type the regex describes (when relational),
  or empty / `algo.Regx` for free-form patterns.
- `dmmeta.fregx`:
  - `regxtype` — `Normal` or `Sql`.  Required by RegxSql, which
    fixes it to `Sql`.
  - `partial:Y/N` — match anywhere (`partial:Y`) vs. anchor to
    the whole string (`partial:N`).

Optional:

- `dflt:"..."` — pre-compile this pattern at construction.

### Generated functions
<a href="#generated-functions"></a>

Source: `cpp/amc/regx.cpp`.

| Tfunc                     | Generated function                                  | Effect |
|---------------------------|-----------------------------------------------------|--------|
| `Regx.Init` / `RegxSql.Init`               | (folded into parent's `_Init`) | Compile `dflt` pattern via `Regx_Read<Regxtype>`. |
| `Regx.Print` / `RegxSql.Print`             | `void <name>_Print(P&, cstring& out)` | `Regx_Print(...)` — emits the source pattern. |
| `Regx.ReadStrptrMaybe` / `RegxSql.ReadStrptrMaybe` | `bool <name>_ReadStrptrMaybe(P&, strptr in)` | Compile `in` into the field via `Regx_Read<Regxtype>`. |

At runtime, matching is done via the algo `Regx_Match(regx, str)`
function — no tfunc generates a typed `_Match`, since Regx
match takes a `strptr`, not a typed value.

### Memory model
<a href="#memory-model"></a>

- `algo_lib::Regx` carries its own owned compilation state
  (an NFA/DFA representation plus error info).  The parent
  doesn't see those details.
- Compilation happens at `Init` and on every
  `_ReadStrptrMaybe`.  No caching across re-compiles.

### Pitfalls
<a href="#pitfalls"></a>

- **`fregx` required.**  amc errors if missing (especially for
  RegxSql).
- **`partial` semantics matter.**  `partial:Y` allows matching
  anywhere in the string (often unintended for pkey-style
  matching); `partial:N` anchors to the whole input.
- **Re-compilation on every Read.**  Setting the same pattern
  repeatedly recompiles each time; cache externally if it
  matters.
- **Errors are deferred.**  `Regx_Read*` does not throw on a
  bad pattern; the regex is left in an invalid state that
  matches nothing.  Check `algo_lib::_db.errtext` if a
  pattern unexpectedly never matches.
- **No typed `Match` accessor.**  Compare directly via
  `Regx_Match(field.<name>, candidate)`.

### See also
<a href="#see-also"></a>

- [Reftypes index](/txt/exe/amc/reftype.md)
- [Regular expressions feature page](/txt/exe/amc/regx.md)
- [Command-line parsing](/txt/exe/amc/cmdline.md) — primary consumer of `RegxSql`
- Source: `cpp/amc/regx.cpp`
- Tfunc records: `acr 'tfunc:Regx.%'` / `acr 'tfunc:RegxSql.%'`
- Spec table: `acr 'dmmeta.fregx:%'`

### Example
<a href="#example"></a>

A command-line flag accepting a SQL-style pattern over targets:

```
dmmeta.field   field:command.abt.target  arg:dev.Target  reftype:Regx
  dmmeta.fregx  field:command.abt.target  regxtype:Sql  partial:N
```

In C++:

```c++
command::abt cmd;
target_ReadStrptrMaybe(cmd, "atf%");      // compile the pattern
ind_beg(_db_target_curs, target, _db) {
    if (Regx_Match(cmd.target, target.target)) {
        ...
    }
} ind_end;
```

Free-form regex over a string field:

```
dmmeta.field   field:foo.Cfg.namepat  arg:algo.Regx  reftype:Regx
  dmmeta.fregx  field:foo.Cfg.namepat  regxtype:Normal  partial:Y
```

## amc Reftype: RegxSql
<a href="#amc-reftype-regxsql"></a>

`RegxSql` is a **SQL-style regex** — same code generator as
[Regx](/txt/exe/amc/reftype/Regx.md), with `regxtype` pinned
to `Sql` and a hard requirement that `dmmeta.fregx` be
present.  In SQL-style regex, `%` is "any sequence" and `_` is
"any single character"; everything else is literal.

```ssim
dmmeta.field   field:abt.FDb.target  arg:abt.FTarget  reftype:RegxSql  dflt:""
  dmmeta.fregx  field:abt.FDb.target  regxtype:Sql  partial:N
```

### Rewrite to Regx
<a href="#rewrite-to-regx"></a>

amc's `amcdb.gen:rewrite_regx` phase rewrites every `RegxSql` field
to plain `Regx` with `regxtype:Sql` before tfunc dispatch.
So by the time code is emitted, RegxSql fields look identical
to Regx-with-Sql fields:

- the `algo_lib::Regx` storage field is the same
- the `_Init`, `_Print`, `_ReadStrptrMaybe` bodies are the
  same — RegxSql's tfuncs simply delegate to Regx's
  (see `cpp/amc/regx.cpp`).

The reftype name exists in `dmmeta.reftype` because user code
(and `dmmeta` records) needs a way to express "this regex is
specifically SQL-style".  Once gen runs, the distinction
disappears.

### Ssim inputs
<a href="#ssim-inputs"></a>

Required:

- `dmmeta.field` with `reftype:RegxSql`.
- `dmmeta.fregx` — mandatory.  `regxtype` is forced to `Sql` if
  unset (amc verifies it before generating code).

Optional:

- `dflt:"..."` — SQL pattern compiled at parent construction.

### Generated functions
<a href="#generated-functions"></a>

Source: `cpp/amc/regx.cpp` (RegxSql's tfuncs delegate to
Regx's).

| Tfunc                       | Generated function                          | Effect |
|-----------------------------|---------------------------------------------|--------|
| `RegxSql.Init`              | (folded)                                    | Compile dflt via `Regx_ReadSql`. |
| `RegxSql.ReadStrptrMaybe`   | `bool <name>_ReadStrptrMaybe(P&, strptr in)`| Compile `in` via `Regx_ReadSql`. |
| `RegxSql.Print`             | `void <name>_Print(P&, cstring& out)`       | Print the source SQL pattern. |

See the [Regx](/txt/exe/amc/reftype/Regx.md) page for the
mechanics — RegxSql is a thin wrapper that forces the SQL
flavor.

### Pitfalls
<a href="#pitfalls"></a>

- **`%` and `_` are the only wildcards.**  Other regex
  metacharacters (`.`, `*`, `[]`, etc.) are literal.
- **`fregx` is mandatory.**  amc fails at code-gen with
  "missing fregx" otherwise.
- **`amcdb.gen:rewrite_regx` does the lifting.**  Looking at the
  in-memory schema after the gen pipeline, you'll see Regx
  fields, not RegxSql.  Use `acr` against the ssim sources
  to see the original RegxSql records.

### See also
<a href="#see-also"></a>

- [Reftypes index](/txt/exe/amc/reftype.md)
- [Regx](/txt/exe/amc/reftype/Regx.md) — companion reftype, full docs
- [Regular expressions feature page](/txt/exe/amc/regx.md)
- Source: `cpp/amc/regx.cpp`
- Tfunc records: `acr 'tfunc:RegxSql.%'`

### Example
<a href="#example"></a>

Most amc CLI tools use RegxSql for their `-target`,
`-readmefile`, etc.  flags:

```ssim
dmmeta.field   field:command.abt.target  arg:dev.Target  reftype:RegxSql
  dmmeta.fregx  field:command.abt.target  regxtype:Sql  partial:N
```

After amc:

```c++
command::abt cmd;
target_ReadStrptrMaybe(cmd, "atf%");      // SQL: prefix match
if (Regx_Match(cmd.target, target.target)) { ... }
```

## amc Reftype: Alias
<a href="#amc-reftype-alias"></a>

`Alias` is a **synonym** for another field — it has no storage
of its own.  Reads delegate to the source field's value; writes
and string-parses go to the source field.  Alias is mostly used
to give command-line flags a short alias for a long flag name
(`-d` → `-debug`), but it works for any field-to-field rename.

```ssim
dmmeta.field   field:algo_lib.Cmdline.v   arg:u8  reftype:Alias  dflt:""  comment:"Alias for verbose"
  dmmeta.falias  field:algo_lib.Cmdline.v   srcfield:algo_lib.Cmdline.verbose
```

### What it generates
<a href="#what-it-generates"></a>

No storage in the parent struct.  Only three accessor functions
that forward to the source field
(`cpp/amc/alias.cpp` — `tclass_Alias` is empty: no fields,
no init).

### Ssim inputs
<a href="#ssim-inputs"></a>

Required:

- `dmmeta.field` with `reftype:Alias`.  `arg:` should match the
  source field's `arg` so the accessor signatures line up.
- `dmmeta.falias` (`field` ↔ `srcfield`) — which existing
  field this alias points at.

No memory, no pool.  The alias is purely a name-level
redirection.

### Generated functions
<a href="#generated-functions"></a>

Source: `cpp/amc/alias.cpp`.

| Tfunc                  | Generated function                                      | Effect |
|------------------------|---------------------------------------------------------|--------|
| `Alias.Get`            | `T <name>_Get(const P&)` (inline)                       | `return parent.<srcfield_path>;` — reads through the source field's effective expression (handles bitflds, base composition, etc.). |
| `Alias.Set`            | `void <name>_Set(P&, T rhs)`                            | Writes `rhs` to the source field via the source field's `AssignExpr`. |
| `Alias.ReadStrptrMaybe`| `bool <name>_ReadStrptrMaybe(P&, strptr in)`            | Parses `in` into the source field by calling its own `ReadStrptrMaybe`. |

The source field's full behavior (bitfield masking, default
values, side effects of `_Set`, etc.) is preserved because the
alias is implemented by composition, not duplication.

### Pitfalls
<a href="#pitfalls"></a>

- **`arg:` must match.**  Alias generates `<srcfield_arg>` for
  the return type of `_Get` and the parameter of `_Set` only
  through `Argtype(field)` — if the alias field's `arg` differs
  from the source's `arg` you'll get a type mismatch at compile
  time, not at amc time.
- **No alias of an alias.**  amc does not recursively follow
  alias chains; pick the underlying field as the `srcfield`.
- **Not for ssim aliasing.**  Alias affects only the C++
  surface.  Both names still appear as separate ssim fields in
  `dmmeta.field`.

### See also
<a href="#see-also"></a>

- [Reftypes index](/txt/exe/amc/reftype.md)
- [Bitfld](/txt/exe/amc/reftype/Bitfld.md) — when you want a sub-field of an existing storage field
- [Command-line parsing](/txt/exe/amc/cmdline.md) — the primary use case
- Source: `cpp/amc/alias.cpp`
- Tfunc records: `acr 'tfunc:Alias.%'`
- Spec table: `acr 'dmmeta.falias:%'`

### Example
<a href="#example"></a>

The short alias for `-verbose` in algo_lib:

```ssim
dmmeta.field   field:algo_lib.Cmdline.v        arg:u8  reftype:Alias  dflt:""  comment:""
dmmeta.field   field:algo_lib.Cmdline.verbose  arg:u8  reftype:Val    dflt:""  comment:""
dmmeta.falias  field:algo_lib.Cmdline.v        srcfield:algo_lib.Cmdline.verbose
```

After this, the argv parser accepts both `-v` and `-verbose`,
and the generated help string shows them as aliased.  In C++:

```c++
v_Set(cmdline, 3);                 // equivalent to verbose_Set(cmdline, 3)
i32 vlevel = v_Get(cmdline);       // reads cmdline.verbose
```

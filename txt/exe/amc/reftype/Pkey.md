## amc Reftype: Pkey
<a href="#amc-reftype-pkey"></a>

`Pkey` is a **text-key reference** to another ssim record's
primary key.  It tells amc "this field holds the pkey of some
record of ctype `arg`".  At amc time, before any code is
generated, amc resolves the Pkey to the underlying type
(usually a `Smallstr` or `Val` of a string type) and **rewrites
the field as a plain `Val`** of that resolved type.

By the time code generation runs, no `Pkey` fields remain —
they have all been replaced.  This means there are zero
`Pkey.*` tfuncs.

```ssim
dmmeta.field  field:dmmeta.Ctype.ctype  arg:dmmeta.Ctype  reftype:Pkey  dflt:""  comment:""
```

After the rewrite, this field looks identical to:

```ssim
dmmeta.field  field:dmmeta.Ctype.ctype  arg:algo.Smallstr50  reftype:Val  dflt:""  comment:""
```

…because `dmmeta.Ctype`'s pkey field has `arg:algo.Smallstr50`.

### Why Pkey exists
<a href="#why-pkey-exists"></a>

Pkey is a **schema-level shorthand**.  When you declare a Pkey
field, you are saying "use whatever type makes a valid pkey for
the target ctype" — so renaming the target's pkey type ripples
to every Pkey reference automatically.  Without Pkey you would
have to repeat the underlying string type at every reference
site.

Pkey also marks the field as semantically "this is a foreign
key", which downstream tools (`acr_check`, the in-memory
database loader, etc.) can use for referential-integrity
checks even though the field is a plain string at runtime.

### What it generates
<a href="#what-it-generates"></a>

`tclass_Pkey` in `cpp/amc/main.cpp` is empty:

```c++
void amc::tclass_Pkey() {
    // does not occur
}
```

It never runs because the gen-pipeline phase `amcdb.gen:lookuppkey`
(in `cpp/amc/gen.cpp`) rewrites every Pkey field's `reftype`,
`arg`, and `p_arg` to point at the resolved underlying type
before code generation begins.  The Pkey *concept* survives in
ssim metadata; the *reftype enum value* never reaches a tfunc.

### Ssim inputs
<a href="#ssim-inputs"></a>

Required:

- `dmmeta.field` with `reftype:Pkey`.  `arg:` is the target
  ctype (whose pkey will be inherited).

That's it.  Pkey takes no parameters.

### Generated functions
<a href="#generated-functions"></a>

**None directly.**  After `amcdb.gen:lookuppkey` runs, the field is
indistinguishable from a `Val` of the resolved type, and you
get whatever `Val` generates: `Get`, `Set`, `Init`, `Print`,
`ReadStrptrMaybe`, etc.

### Pitfalls
<a href="#pitfalls"></a>

- **Schema-only construct.**  Pkey vanishes by the time amc
  emits C++.  Don't look for `Pkey.*` tfuncs in
  `ssimfile:amcdb.tfunc` — none exist.
- **Not an automatic xref.**  Pkey only chooses the storage
  type; it does not generate any lookup or referential-
  integrity check.  Combine with `dmmeta.xref` if you want
  the value to resolve to a pointer at runtime — typically
  this is the role of [Upptr](/txt/exe/amc/reftype/Upptr.md)
  (the resolved pointer is set by another field's xref).
- **Pkey of Pkey** is allowed and chained — amc walks down
  the pkey chain until it hits a concrete reftype.

### See also
<a href="#see-also"></a>

- [Reftypes index](/txt/exe/amc/reftype.md)
- [Val](/txt/exe/amc/reftype/Val.md) — what Pkey becomes after rewrite
- [Upptr](/txt/exe/amc/reftype/Upptr.md) — runtime pointer companion
- [Cross-references](/txt/exe/amc/xref.md) — pkey-driven xref `via:` paths
- Source: `cpp/amc/main.cpp` (empty stub), `cpp/amc/gen.cpp` `gen_lookuppkey`
- Tfunc records: *(none)*

### Example
<a href="#example"></a>

Look at almost any dmmeta table — most `field:<table>.X`
records use `reftype:Pkey`:

```ssim
dmmeta.field  field:dmmeta.Field.field    arg:dmmeta.Field   reftype:Pkey
dmmeta.field  field:dmmeta.Field.arg      arg:dmmeta.Ctype   reftype:Pkey
dmmeta.field  field:dmmeta.Field.reftype  arg:dmmeta.Reftype reftype:Pkey
```

After amc:

```c++
struct Field {
    algo::Smallstr100  field;   // resolved from Pkey via dmmeta.Field's pkey
    algo::Smallstr50   arg;     // resolved from Pkey via dmmeta.Ctype's pkey
    algo::Smallstr50   reftype; // resolved from Pkey via dmmeta.Reftype's pkey
    ...
};
```

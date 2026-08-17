## amc Reftype: Cppstack
<a href="#amc-reftype-cppstack"></a>

`Cppstack` is a **placeholder pool**.  It declares "the default
home" for a ctype that is meant to be allocated on the C++
stack at the call site, not in a heap-backed pool.  Code-gen
elsewhere in amc consults `Cppstack` to find the default name
to use when referring to instances of the ctype (for
auto-generated `_curs` variables, lambda captures, etc.) —
but the reftype itself emits no functions and reserves no
memory.

```
dmmeta.field  field:acr.FDb.print  arg:acr.FPrint  reftype:Cppstack  dflt:""  comment:"Provides default name for variables of this type"
```

After this declaration, generated code referring to "the default
instance of `acr::FPrint`" uses `print` as the variable name,
which keeps the generated symbols stable even though no global
variable named `print` exists.

### What it generates
<a href="#what-it-generates"></a>

**Nothing.**  `tclass_Cppstack` in `cpp/amc/main.cpp` is
intentionally an empty function body labelled "not used".

There are no tfuncs for Cppstack — running
`acr 'tfunc:Cppstack.%'` returns zero records.

What you *do* get is the existence of the field record itself,
which other amc code-gen consults:

- Other generators (e.g., `Pool.AllocXref`) look up the
  default pool for `arg:` to find the canonical instance name.
  Cppstack provides that name without forcing an allocation.
- The dmmeta.reftype flags `isval:Y  cancopy:Y  isnew:Y  inst:Y`
  declare the ctype as a value type with a (notional) default
  instance — enough to satisfy amc's bookkeeping for any xref
  whose parent is a stack-allocated record.

### Ssim inputs
<a href="#ssim-inputs"></a>

Required:

- `dmmeta.field` with `reftype:Cppstack`.  `arg:` is the ctype
  this name belongs to.  The field name is the C++ identifier
  used in generated code.

There is no spec table.  Cppstack takes no parameters.

### Generated functions
<a href="#generated-functions"></a>

None.

### Pitfalls
<a href="#pitfalls"></a>

- **No runtime semantics.**  Cppstack is a code-gen hint, not
  an allocator.  Don't expect `Alloc` / `Delete` / `_N` —
  none of them exist.
- **The field name matters.**  Generated code uses the field
  name verbatim, so changing it (e.g., `print` → `printer`) is
  a renaming across every consumer.
- **Naming convention.**  Field names for `Cppstack` are
  usually short, lowercase, and meant to be valid C++ local
  variable names.

### See also
<a href="#see-also"></a>

- [Reftypes index](/txt/exe/amc/reftype.md)
- [Val](/txt/exe/amc/reftype/Val.md) — plain embedded value
- [Memory pools](/txt/exe/amc/pool.md) — the allocator catalog
- Source: `cpp/amc/main.cpp` (search for `tclass_Cppstack`)

### Example
<a href="#example"></a>

`acr` declares its temporary helper ctypes
(`acr::FPrint`, `acr::FCheck`, `acr::FWrite`, …) with
`Cppstack`.  These structs are used as stack locals in `acr`'s
command implementations, never heap-allocated.  The
declaration teaches amc which symbol to use when emitting
references to "an instance of `acr::FPrint`":

```c++
acr::FPrint print;                 // user stack-allocates
print.target = ...;
acr::Main_Print(print);
```

Generators that walk access paths through `acr::FPrint` (e.g.,
for `acr::FPrint::*` cursors) reference the local as `print`,
matching the field name declared in ssim.

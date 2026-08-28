## amc Backend: tclass / tfunc dispatch
<a href="#amc-backend-tclass-tfunc-dispatch"></a>

A **tclass** is a code-generation strategy for a class of fields
(e.g. all `Pool` fields, all `Llist` fields).  A **tfunc** is a
single function template within a tclass — e.g. `Pool.AllocMaybe`,
`Llist.Insert`.  An **ffunc** opts one specific field into a tfunc
that amc does not emit by default, or marks it user-supplied.  This
page explains how tclass and tfunc records bind to C++ generator
functions, how a field selects them, and how ffunc extends the set
per field.

### Table Of Contents
<a href="#table-of-contents"></a>
&nbsp;&nbsp;&bull;&nbsp;  [Tclass](#tclass)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Tfunc](#tfunc)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Ffunc](#ffunc)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Binding to C++](#binding-to-c)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Per-field dispatch](#per-field-dispatch)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [The genctx parameter](#the-genctx-parameter)<br/>

### Tclass
<a href="#tclass"></a>

Source: `ssimfile:amcdb.tclass`.  A tclass groups the functions that
amc can generate for one kind of field.  Each tclass record has a `step`
hook (its C++ entry point, see [Binding to C++](#binding-to-c)) and a
`c_tfunc` list of the tfuncs it owns.

A tclass is not the same kind of thing as a reftype, even though most
tclasses share a reftype's name.  A reftype is a choice the author of a
`dmmeta.field` row makes; `dmmeta.reftype` is a subset of `amcdb.tclass`,
and each of those 36 tclasses runs for any field of its reftype.  The
remaining tclasses are never named in user-written ssim at all.  They
differ in *scope* — what amc is generating when they run:

|Scope|Genctx|Tclasses|
|---|---|---|
|field|`p_field` set|the 36 reftype tclasses, the 12 option tclasses, `Field`, `Field2`|
|pool field|`p_field` set|`Pool`, reached from the pool reftype's own generator|
|ctype|`p_field` NULL|`Ctype`, `Ctype2`|
|namespace|`p_ctype` and `p_field` NULL|`Ns`|

#### Tclass index
<a href="#tclass-index"></a>

Look a tclass up here to find what turns it on and where its generated
functions are documented.

The 36 reftype tclasses are indexed in
[Reftypes](/txt/exe/amc/reftype.md); each has a page of its own under
`/txt/exe/amc/reftype/` covering its ssim inputs, generated functions,
cursors and field prefixes.

The 12 *option* tclasses run for a field that carries some other record
besides `dmmeta.field`.  The record is the switch: no record, no tclass.

|Tclass|Activated by|Documented in|
|---|---|---|
|`Bitset`|`dmmeta.fbitset` on the field|[Bitsets](/txt/exe/amc/bitset.md)|
|`Cmp`|`dmmeta.fcmp` on the field|[Sorting](/txt/exe/amc/sort.md#fcmp)|
|`Cppfunc`|`dmmeta.cppfunc` on the field|[Hooks](/txt/exe/amc/hook.md#hook-vs-cppfunc-vs-fcb)|
|`Dec`|`dmmeta.fdec` on the field|[Decimals](/txt/exe/amc/decimal.md)|
|`Fcast`|`dmmeta.fcast` on the field|[dmmeta.fcast](/txt/ssimdb/dmmeta/README.md#dmmeta-fcast); example in [Enums](/txt/exe/amc/enum.md)|
|`Fconst`|`dmmeta.fconst` on the field|[Enums](/txt/exe/amc/enum.md)|
|`Io`|`dmmeta.finput`, `dmmeta.foutput` or `dmmeta.gstatic` on the field|[I/O](/txt/exe/amc/io.md)|
|`Numstr`|`dmmeta.numstr` on the field's `dmmeta.smallstr`|[Small strings](/txt/exe/amc/string.md#small-strings)|
|`Pmask`|`dmmeta.pmaskfld` on the field's **ctype** — every field of that ctype gets the tclass|[Presence masks](/txt/exe/amc/pmask.md)|
|`Sort`|`dmmeta.fsort` on the field|[Sorting](/txt/exe/amc/sort.md)|
|`Step`|`dmmeta.fstep` on the field|[Runtime](/txt/exe/amc/runtime.md)|
|`Substr`|`dmmeta.substr` on the field|[Substr](/txt/exe/amc/string.md#substr)|

The remaining tclasses have no switch — they run on every field, ctype or
namespace amc reaches.  `Ctype` is counted among the 36 above because it has
a `dmmeta.reftype` row, but no field is ever declared `reftype:Ctype`, so in
practice it only ever runs per ctype.

|Tclass|Scope and role|Documented in|
|---|---|---|
|`Field`, `Field2`|Every field, before and after everything else.  Owns the lifecycle callbacks a field can opt into — `Cleanup`, `Userinit`, `OnXref`, `OnUnref`, `Cascdel`, `Concat` — and `Field2.ReadStrptrMaybe`|[Ffunc](#ffunc), below|
|`Pool`|A field that can allocate.  Called by the pool reftype's own generator (`Malloc`, `Sbrk`, `Tpool`, `Lpool`, `Blkpool`, `Lary`, `Inlary`) rather than by the field walk, so every pool presents the same `Alloc` / `Delete` surface|[Memory pools](/txt/exe/amc/pool.md)|
|`Ctype`, `Ctype2`|Once per ctype, in two passes.  Owns the struct's own C++ interface: lifecycle, comparison, print/read, wire codecs, constructors|[Ctype](/txt/exe/amc/reftype/Ctype.md)|
|`Ns`|Once per namespace, for a `nstype:protocol` namespace or one with a `globfld`.  Emits `StaticCheck()`, the size and field-offset assertions|[Size assertions](/txt/exe/amc/csize.md)|

### Tfunc
<a href="#tfunc"></a>

Source: `ssimfile:amcdb.tfunc`.  A tfunc is one function template owned
by a tclass; its pkey is `<Tclass>.<Name>` (e.g. `Thash.Find`,
`Atree.Insert`, `Field.Cleanup`).  Each tfunc maps 1:1 to a C++
generator function (see [Binding to C++](#binding-to-c)) which, when
run for a field, may emit a C++ function — or nothing, if the field's
options don't call for it.

Tfunc attributes set defaults on the function the generator produces:

|Attribute|Effect|
|---|---|
|`hasthrow`|The function may throw.  The tfunc is skipped entirely in namespaces compiled without exceptions (`GenThrowQ` false).|
|`leaf`|The function calls nothing that throws.  Combined with `!hasthrow` it makes the generated function `nothrow` (`__attribute__((nothrow))`).|
|`inl`|Generate inline by default.|
|`wur`|Generate `__attribute__((warn_unused_result))`.|
|`pure`|The function may be called fewer times than written (no observable side effects).|
|`ismacro`|Emit as a macro rather than a function.|
|`poolfunc`|The function operates on the pool rather than a single row.|

### Ffunc
<a href="#ffunc"></a>

Source: `ssimfile:dmmeta.ffunc`.  Where a tfunc declares *what functions
can exist* for a whole tclass, an **ffunc** is the per-field counterpart:
it names, for one specific field, a function amc should treat specially.
Its pkey is `<field>.<name>`, where `<name>` is a tfunc name — the same
`<field>.<name>` shape (and `extrn` flag) as `dmmeta.func`.

An ffunc does one of two things, selected by `extrn`:

- **`extrn:Y` — user-supplied.**  amc emits only an `extern` prototype
  and calls it from the generated code; the body is hand-written.  Used
  for lifecycle callbacks the user implements — `Cleanup`, `Userinit`,
  `OnXref`, `OnUnref` — and to *override* a function amc would otherwise
  generate, e.g. `Fbuf.ScanMsg` (custom message framing) or
  `Io.Input` / `Io.InputMaybe` (custom load).  Overriding is powerful but
  dangerous: amc no longer controls that function's behavior.
- **`extrn:N` — generate on request.**  amc generates the body, but only
  because the ffunc opted in.  Used for functions amc does not emit by
  default, e.g. `Thash.FindRemove` (atomic find-and-remove on a hash
  index).

Generators consume an ffunc by calling `amc::FindFfunc(field, name,
mark_used=true)` at the point they emit code for it.  For example
`tfunc_Thash_FindRemove` emits its body only when
`FindFfunc(field,"FindRemove")` is present; `tfunc_Field_Cleanup` emits
the extern prototype only when `FindFfunc(field,"Cleanup")` is present.

There is no table of which names are valid for which reftype.  Validity
follows from generation itself: every emission site marks the ffunc
*used*, and `gen_check_ffunc` reports any ffunc left unused at the end of
the run.  An unused ffunc names a function no generator produced for that
field — a typo, or a name not applicable to the field's reftype (e.g.
`FindRemove` on a `Val`).  This **MarkUsed** check replaced the former
`amcdb.tcb` table (which had enumerated valid reftype × callback pairs).

### Binding to C
<a href="#binding-to-c"></a>

Both tclasses and tfuncs reach their C++ code through a `step` hook whose
value is assigned at startup from a gstatic table (`amc::tfunc_LoadStatic`
/ `amc::tclass_LoadStatic` in `cpp/gen/amc_gen.cpp`).  The C++ symbol is
derived from the record key by convention — `amc::tfunc_<Tclass>_<Name>`
(dots become underscores), e.g. `Pool.AllocMaybe` →
`amc::tfunc_Pool_AllocMaybe`, `Field.Cleanup` → `amc::tfunc_Field_Cleanup`.
Adding a tfunc record therefore requires defining a function of the
matching name; the loader wires the hook automatically.  (See
`StaticHookName` in `cpp/amc/io.cpp`.)

### Per-field dispatch
<a href="#per-field-dispatch"></a>

`gen_ns_tclass_field` walks every ctype's fields; for each field
`GenTclass_Field` (`cpp/amc/tclass.cpp`) calls `GenTclass` for every
tclass the field participates in, in this order:

1. `Field`.
2. `Pmask`, `Dec`, `Bitset`, `Cmp` — those whose record is present.
3. The field's reftype tclass.
4. `Numstr`, `Fconst`, `Substr`, `Cppfunc`, `Fcast`, `Step` — again, those
   whose record is present.
5. `Field2`.
6. `Sort`, then `Io`.  The source marks these two destructive and runs
   them after everything else; do not move them earlier.

The order matters to the generators that read what an earlier tclass
emitted: `Sort` consumes the `Cmp` function of the field it sorts on, and
a reftype generator can rely on `Field` having run but not on `Field2`.

`GenTclass` runs the tclass's own `step`, then calls each owned tfunc's
`step` in turn, skipping `hasthrow` tfuncs when exceptions are off, and
cursor tfuncs that weren't explicitly requested.

The two tclasses with no field context are driven separately:
`gen_ns_tclass_ctype` runs `Ctype` and then `Ctype2` once per ctype, after
the field walk has finished so that ctype sizes are known, and
`gen_ns_tclass_ns` runs `Ns` once for the namespace.

Each tfunc generator decides for itself whether to emit anything, reading
the field's options and ffuncs.  So the set of functions a field gets is
the union of: the unconditional tfuncs of its tclasses, plus the tfuncs
its option records turn on, plus the tfuncs its ffuncs request.

### The genctx parameter
<a href="#the-genctx-parameter"></a>

The current scope is passed to generators through `amc::_db.genctx`
rather than as function arguments: `genctx.p_ns`, `genctx.p_ctype`,
`genctx.p_field` (NULL for ctype/ns-level generators), and
`genctx.p_tfunc` (the tfunc being run).  `ResetVars` repopulates the
Replscope (`$ns`, `$Parname`, `$name`, `$Cpptype`, …) from this context
before each field, and tfunc generators read `*amc::_db.genctx.p_field`
to find the field they are generating for.

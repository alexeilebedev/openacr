## amc Feature: Hooks — callback fields
<a href="#amc-feature-hooks-callback-fields"></a>

A **Hook** is amc's typed function-pointer field — the
explicit, statically-bound replacement for C++ virtual
methods.  Where virtuals tie a function pointer to a class
through inheritance (and pay the indirect-call cost on every
invocation), Hooks let you attach behavior to individual
records, dispatch tables, or compile-time-baked rows.

There are two distinct *usage patterns* for hooks:

1. **Instance hooks** — bind at runtime via `<name>_Set0/1/2`,
   change as state evolves.  Used for per-connection
   callbacks, scheduler callbacks, and any record whose
   behavior depends on its data.

2. **Static hooks** — wired by amc itself from
   [gstatic](/txt/exe/amc/reftype/Global.md#gstatics) tables.
   The function pointer is set once at static-init time and
   never rebinds.  This is how amc's own gen pipeline, the
   markdown sections in `abt_md`, and many "table-driven
   dispatch" patterns work.

The reftype-level mechanics (storage, tfuncs, `_Set0/1/2/Call`)
are documented at [Hook reftype](/txt/exe/amc/reftype/Hook.md);
this page covers the patterns.

### Table Of Contents
<a href="#table-of-contents"></a>
&nbsp;&nbsp;&bull;&nbsp;  [Choosing instance vs static hooks](#choosing-instance-vs-static-hooks)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Instance hook pattern](#instance-hook-pattern)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Static-hook pattern (gstatic-backed)](#static-hook-pattern-gstatic-backed-)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Hook signatures](#hook-signatures)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Hook vs Cppfunc vs Fcb](#hook-vs-cppfunc-vs-fcb)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Pitfalls](#pitfalls)<br/>

### Choosing instance vs static hooks
<a href="#choosing-instance-vs-static-hooks"></a>

| Question                                          | Instance | Static |
|---------------------------------------------------|----------|--------|
| Does the callback change after construction?      | yes      | no     |
| Does the value come from a gstatic ssim table?    | no       | yes    |
| Does each row carry its own function pointer?     | depends  | yes    |
| Is there a runtime `Set*` API?                    | yes      | no     |

If you're declaring a hook on `FDb` so user code can rebind
the callback later (e.g., a "log line received" callback),
that's instance.  If you're declaring a row in a gstatic
table where each row has a function name that resolves
automatically, that's static.

### Instance hook pattern
<a href="#instance-hook-pattern"></a>

Declaration:

```
dmmeta.ctype  ctype:net.FConn
  dmmeta.field  field:net.FConn.on_close  arg:net.FConn  reftype:Hook  dflt:""
    dmmeta.hook   field:net.FConn.on_close
```

`arg:` is the type of the second argument the callback
receives (the conn struct itself, in this case — common
pattern).  The first argument is the context, supplied by
`_Set1`/`_Set2`.

Binding and calling:

```c++
static void on_close_cb(net::FConn& self) {
    prlog("disconnected: " << self.peer);
}

net::FConn conn;
on_close_Set1(conn, conn, on_close_cb);       // bind: 1-arg with context=conn

// later, when something disconnects:
on_close_Call(conn, conn);                     // fires the callback
```

The context (`_ctx`) is stored as a `void*` cast — the
template-form `_Set1<T>` preserves the type for type-safety
at the call site.

### Static-hook pattern (gstatic-backed)
<a href="#static-hook-pattern-gstatic-backed-"></a>

The amc gen pipeline itself is the canonical example.  Every
row of `data/amcdb/gen.ssim` looks like:

```
amcdb.gen  gen:prep_signature  perns:N  comment:"Prepare signatures"
amcdb.gen  gen:select_ns       perns:N  comment:"Select namespaces"
...
```

The `amcdb.Gen` ctype has a `Hook` field named `step`, and
`amc.FDb.gen` is a [gstatic](/txt/exe/amc/io.md#gstatic) pool
populated from `amcdb.gen.ssim`.  amc auto-generates one
extern forward-declaration per row:

```c++
namespace amc {
    void gen_prep_signature();
    void gen_select_ns();
    void gen_dispenum();
    void gen_countxref();
    // ...one per row of amcdb.gen
}
```

The user implements the bodies (`cpp/amc/signature.cpp`,
`cpp/amc/disp/main.cpp`, …).  At gstatic-init time, each row's
`step` Hook is pre-filled with the address of the matching
function name — no `_Set*` call needed.

At runtime, amc just walks the table and calls each row's
`step_Call(...)`:

```c++
ind_beg(amc::_db_gen_curs, gen, amc::_db) {
    step_Call(gen);    // resolves to gen_prep_signature, gen_select_ns, ...
} ind_end;
```

This is **declarative dispatch**: the ssim table is the
registry; adding a new row + writing a function is the
extension point.  No vtables, no virtual inheritance, no
`#include` web — just one ssim row and one C++ function.

The function-name binding is **pkey-derived**: amc converts
the row's pkey (`prep_signature`, `select_ns`, …) into a
C++ identifier prefix and looks up that symbol at link time.
See `Hook_FwdDecl` in `cpp/amc/fwddecl.cpp`.

### Hook signatures
<a href="#hook-signatures"></a>

The `arg:` field of the Hook declaration plus a `Set*` arity
determine the generated signature:

| `Set*` variant   | C++ signature                                        |
|------------------|------------------------------------------------------|
| `Set0`           | `void (*fcn)()`                                      |
| `Set1<T>`        | `void (*fcn)(T&)` — `T` is the user-supplied context |
| `Set2<T>`        | `void (*fcn)(T&, Arg&)` where `Arg` is `field.arg`   |

`Call` matches:

| Variant | Call signature                  |
|---------|---------------------------------|
| `Set0`  | `<name>_Call(P&)` (no arg)      |
| `Set1`  | `<name>_Call(P&)` — passes ctx  |
| `Set2`  | `<name>_Call(P&, Arg)` — passes ctx + arg |

For static hooks (no `_Set*`), `Call` simply invokes
`parent.<name>(...)`.  The static-inlary form skips the NULL
check (pre-resolved by amc).

### Hook vs Cppfunc vs Fcb
<a href="#hook-vs-cppfunc-vs-fcb"></a>

amc has three "user-code-hook" mechanisms; they look similar
but solve different problems:

| Feature      | Use when…                                                | Storage           |
|--------------|----------------------------------------------------------|-------------------|
| **Hook**     | Need a typed function-pointer field, possibly rebound    | Function pointer  |
| **Cppfunc**  | The value of a field is a *computed expression*         | None (computed)   |
| **Fcb**      | Want to fire a callback at insert/remove/update time   | Forward decl only |

Hook is the most general — Cppfunc and Fcb are specialized for
common cases (computed fields and table-change callbacks).

#### Cppfunc — a field that is a computed expression
<a href="#cppfunc-a-field-that-is-a-computed-expression"></a>

A `dmmeta.cppfunc` field occupies no storage.  Its value is whatever the
record's other fields say it is, and amc generates the accessor that
computes it:

```
dmmeta.cppfunc  field:<ns>.Fnode.device  expr:"<ns>::Device_Concat_host_name(host_Get(parent), tempstr() << devtype_Get(parent) << '-' << name_Get(parent))"  print:N  set:N
```

The `expr` is C++ pasted into the body of the getter, and it may refer to
`parent` — the record — so it reaches the other fields through their own
accessors.

|Tfunc|Generated function|Emitted when|Effect|
|---|---|---|---|
|`Cppfunc.Get`|`<Fldtype> <name>_Get(P&)`|always|Returns `expr`.  With an empty `expr` amc emits the prototype only and the body is hand-written, which is how a computed field that needs real code rather than one expression is declared.|
|`Cppfunc.Set`|`void <name>_Set(P&, rhs)`|`set:Y`|Prototype only — always hand-written.  A computed field has nowhere to store a value, so what assignment means is the author's decision.|
|`Cppfunc.Init`|—|never|The generator is empty; a computed field has nothing to initialize.|

A cppfunc field is left out of the record's `Print` unless `print:Y`, since
the fields it is computed from are printed already.

### Pitfalls
<a href="#pitfalls"></a>

- **NULL hook is silently skipped.**  An unbound instance hook
  is a no-op on `Call` — useful for "callback if set" but a
  silent bug for "callback is mandatory".
- **Static-hook function name is pkey-derived.**  Changing the
  pkey of a gstatic row renames the extern; an old C++ body
  with the old name becomes orphaned at link time.
- **Hooks don't replace inheritance for state.**  Hooks
  attach *behavior* to data — for shared *data*, use
  [Base](/txt/exe/amc/reftype/Base.md).
- **Context type erasure.**  `_Set1<T>` templates preserve the
  type for the user, but the storage is `u64`-cast.  Don't
  rebind with mismatched types — undefined behavior at the
  callback.
- **Inlary of hooks is fixed at compile time.**  The
  `[gstatic]` pattern uses an Inlary; you can't add rows at
  runtime.  Use a [Tary](/txt/exe/amc/reftype/Tary.md) of
  hooks for runtime extensibility.

### See also
<a href="#see-also"></a>

- [Hook reftype](/txt/exe/amc/reftype/Hook.md) — field-level reference
- [I/O / gstatic](/txt/exe/amc/io.md#gstatic) — table-driven static hooks
- [Backend / pipeline](/txt/exe/amc/backend/pipeline.md) — amc's own use of hooks
- Source: `cpp/amc/hook.cpp`, `cpp/amc/fwddecl.cpp`
- Spec table: `acr 'dmmeta.hook:%'`

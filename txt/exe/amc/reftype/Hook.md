## amc Reftype: Hook
<a href="#amc-reftype-hook"></a>

`Hook` is a **type-safe function-pointer field**.  It stores a
callback that other code in the same ctype can `_Call`.  Hooks
support 0/1/2-argument signatures and an optional context
pointer for closure-style binding.  They are the only function-
pointer mechanism in amc-generated code — amc deliberately
avoids C++ virtual functions in favor of explicit hooks.

The dominant use of Hook is in **gstatic tables**, where each
row binds compile-time-known function names to dispatch entries
(see [Global / gstatics](/txt/exe/amc/reftype/Global.md#gstatics)).

```ssim
dmmeta.field  field:abt_md.FMdsection.step  arg:abt_md.FFileSection  reftype:Hook  dflt:""  comment:""
  dmmeta.hook   field:abt_md.FMdsection.step
```

### What it generates
<a href="#what-it-generates"></a>

`gen_newfield_hook` (a pre-amc phase, `cpp/amc/hook.cpp`)
manufactures a *function-pointer ctype* (named after the hook
field with `_funcptr` appended) that serves as the typed
storage.  Then `tclass_Hook` adds the actual fields to the
parent:

| Field            | Type             | Always | Meaning                                  |
|------------------|------------------|--------|------------------------------------------|
| `<name>`         | `<HookCpptype>`  | yes    | function pointer, default NULL           |
| `<name>_ctx`     | `u64`            | unless static-only | context pointer (cast to/from `void*`) |

The function-pointer type's signature is derived from:

- `field.arg` — additional argument type (or empty = no extra
  arg).
- The hook's "context" semantics (none / void* / typed via
  template).

A hidden child-init contribution sets `<name> = NULL;` and
clears `_ctx` so the parent ctype's `_Init` includes them.

### Static vs. instance hooks
<a href="#static-vs-instance-hooks"></a>

The `static` flag on the hook (determined by amc from the
context the hook lives in — typically gstatic tables) gates two
behaviors:

- **Static hook** — no `_ctx` field; no `Set0/1/2` tfuncs.  The
  pointer is set once at gstatic-table population time, never
  rebound at runtime.  `_Call` does not pass a context.
- **Instance hook** — has `_ctx`; can be rebound at runtime via
  `Set0/1/2` (or its templated overloads); `_Call` passes the
  context as `void*` (or as a typed reference recovered by the
  user).

For static hooks living in an Inlary (the gstatic pattern),
amc additionally skips the NULL check inside `_Call` because
the function pointer is guaranteed by amc to be set.

### Ssim inputs
<a href="#ssim-inputs"></a>

Required:

- `dmmeta.field` with `reftype:Hook`.  `arg:` is the extra
  argument type (often empty for 0-arg hooks; the ctype
  passed as `arg` for 1-arg/2-arg hooks).
- `dmmeta.hook  field:<field>` — declares the hook.  Without
  this record amc fails with `"hook record required"`.

### Generated functions
<a href="#generated-functions"></a>

Source: `cpp/amc/hook.cpp`.

| Tfunc           | Generated function                                                       | Effect |
|-----------------|--------------------------------------------------------------------------|--------|
| `Hook.Set0`     | `inline void <name>_Set0(P&, void (*fcn)())`                             | Bind a 0-arg function pointer; clears `_ctx`. |
| `Hook.Set1`     | `template<class T> inline void <name>_Set1(P&, T& ctx, void (*fcn)(T&))` | Bind a 1-arg function with typed context. |
| `Hook.Set2`     | `template<class T> inline void <name>_Set2(P&, T& ctx, void (*fcn)(T&, Arg&))` | Bind a 2-arg function. |
| `Hook.Call`     | `inline void <name>_Call(P&, [Arg arg])`                                 | Invoke the hook.  No-op if NULL (unless static-inlary). |

For **static hooks** the `Set*` tfuncs are not generated;
only `Call` exists, and the function pointer is filled in via
the gstatic mechanism.

### Pitfalls
<a href="#pitfalls"></a>

- **NULL hook is silently skipped.**  `_Call` checks the
  pointer (except for the static-inlary case).  If you forgot
  to `Set*` it, nothing happens — no error, no log.  Set
  `dmmeta.fcb` for a `Cleanup` callback if you need
  detection.
- **The context pointer is `void*`-castable.**  The typed
  `Set1` / `Set2` overloads are templates that store the
  context address as `u64` and recover it inside `_Call`.
  Pass the same typed reference you bound with — passing a
  different one is UB.
- **Static hooks have no `Set*` API.**  Once amc generates
  the gstatic table, the pointer is fixed for the binary's
  lifetime.
- **`Hook.Set1/2` are inline templates** — must live in the
  header.  amc emits them inline; do not try to declare them
  out-of-line.
- **Argument compatibility.**  The hook's argument type comes
  from `field.arg`; binding a function whose signature
  doesn't match yields a (template) compile error, not a
  runtime fault.

### See also
<a href="#see-also"></a>

- [Reftypes index](/txt/exe/amc/reftype.md)
- [Hooks feature page](/txt/exe/amc/hook.md)
- [Global / gstatics](/txt/exe/amc/reftype/Global.md#gstatics) — primary use case
- [Inlary](/txt/exe/amc/reftype/Inlary.md) — static-hook storage
- Source: `cpp/amc/hook.cpp`
- Tfunc records: `acr 'tfunc:Hook.%'`
- Spec table: `acr 'dmmeta.hook:%'`

### Example
<a href="#example"></a>

A run-of-the-mill instance hook with one untyped argument:

```ssim
dmmeta.ctype  ctype:atf_amc.FAction
  dmmeta.field  field:atf_amc.FAction.on_complete  arg:atf_amc.FAction  reftype:Hook
    dmmeta.hook  field:atf_amc.FAction.on_complete
```

In C++:

```c++
static void log_completion(atf_amc::FAction& self) {
    prlog("done: " << self.name);
}

atf_amc::FAction act;
on_complete_Set1(act, act, log_completion);    // bind with self as ctx

// later, when the action finishes:
on_complete_Call(act, act);
```

The dominant pattern is **gstatic + hook**: a static table
of rows with hook fields, where amc generates one user-extern
function per row.  amc's own gen table is the canonical
example:

```ssim
dmmeta.ctype    ctype:amcdb.Gen
  dmmeta.field  field:amcdb.Gen.step  arg:""  reftype:Hook
    dmmeta.hook  field:amcdb.Gen.step
dmmeta.gstatic  field:amc.FDb.gen  ns:amc
```

Every row of the `amc.gen` ssimfile binds `step` to a
user-implemented function named after the row's pkey
(`amc::gen_prep_signature`, `amc::gen_ns_dispatch`, …).
At runtime, amc walks the gen table and calls `step_Call(...)`
on each row to drive the pipeline.

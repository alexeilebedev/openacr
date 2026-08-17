## amc Reflection
<a href="#amc-reflection"></a>

Reflection in OpenACR is the **ability of a running process to
inspect its own schema** — the namespaces it has linked in, the
tables those namespaces declare, and the function pointers that
read, write, advance, and print them.  `amc` builds this
introspection layer automatically at code-gen time and registers
it into `algo_lib`'s two reflection tables every time a namespace's
`Init` runs.

This page explains:

- what gets registered into `algo_lib::FDb.imdb` and `imtable`,
- what controls registration (`reflect:Y`, finputs),
- the per-field accessors `amc` emits to support it,
- how live tooling (`acr -t`, `acr_in`, an external trace dump)
  consumes the registration,
- the [Dispsigcheck](/txt/exe/amc/dispatch.md#signatures-dispsig-dispsigcheck)
  startup wiring that flows through the same registration path,
- and the limits of when this beats simply loading the ssim
  description of the universe from disk.

### Table Of Contents
<a href="#table-of-contents"></a>
&nbsp;&nbsp;&bull;&nbsp;  [The two tables: imdb and imtable](#the-two-tables-imdb-and-imtable)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [What gets registered](#what-gets-registered)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [The reflect flag and per-field accessors](#the-reflect-flag-and-per-field-accessors)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Consumers in the tree](#consumers-in-the-tree)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inserting records at runtime](#inserting-records-at-runtime)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Driving the main loop through reflection](#driving-the-main-loop-through-reflection)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Dispsigcheck piggybacks on reflection](#dispsigcheck-piggybacks-on-reflection)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [When to use reflection](#when-to-use-reflection)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Pitfalls](#pitfalls)<br/>

### The two tables: imdb and imtable
<a href="#the-two-tables-imdb-and-imtable"></a>

Reflection state lives entirely on `algo_lib::_db`:

- **`algo_lib::_db.imdb`** — one row per linked namespace that
  has a global `FDb`.  Hashed by name (`ind_imdb`).
- **`algo_lib::_db.imtable`** — one row per **reflected
  global-level field** across all namespaces.  Hashed by
  qualified name (`ind_imtable`, key `"<ns>.<field>"`).

The schema is in `data/dmmeta/`:

```
dmmeta.ctype  ctype:algo.Imdb  comment:"In-memory database descriptor (reflection)"
  field:algo.Imdb.imdb               arg:algo.Smallstr50                reftype:Val  // "<ns>"
  field:algo.Imdb.InsertStrptrMaybe  arg:algo.ImdbInsertStrptrMaybeFcn  reftype:Val  // tuple loader
  field:algo.Imdb.RemoveStrptrMaybe  arg:algo.ImdbRemoveStrptrMaybeFcn  reftype:Val  // tuple deleter
  field:algo.Imdb.Step               arg:algo.ImdbStepFcn               reftype:Val  // single scheduler tick
  field:algo.Imdb.MainLoop           arg:algo.ImdbMainLoopFcn           reftype:Val  // run-until-done
  field:algo.Imdb.GetTrace           arg:algo.ImdbGetTraceFcn           reftype:Val  // copy trace counters
```

```
dmmeta.ctype  ctype:algo.Imtable  comment:"In-memory table descriptor"
  field:algo.Imtable.imtable      arg:algo.Smallstr50         reftype:Val  // "<ns>.<field>"
  field:algo.Imtable.elem_type    arg:dmmeta.Ctype            reftype:Pkey // element ctype name
  field:algo.Imtable.c_RowidFind  arg:algo.ImrowRowidFindFcn  reftype:Val  // address-by-rowid
  field:algo.Imtable.NItems       arg:algo.ImrowNItemsFcn     reftype:Val  // row count
  field:algo.Imtable.Print        arg:algo.ImrowPrintFcn      reftype:Val  // element printer
  field:algo.Imtable.size         arg:i32                     reftype:Val  // sizeof(element)
  field:algo.Imtable.ssimfile     arg:dmmeta.Ssimfile         reftype:Pkey // backing ssimfile, if any
```

The function-pointer fields are typed (`ImdbStepFcn`,
`ImrowRowidFindFcn`, …); the `algo` namespace declares the
signatures so callers can invoke them without casting.

### What gets registered
<a href="#what-gets-registered"></a>

Every namespace whose `dmmeta.field` table contains a
`reftype:Global` row gets a generated `InitReflection()` function
(see `tfunc_Global_InitReflection` in `cpp/amc/global.cpp`).
This function runs once during the namespace's `Init` and:

1. Allocates a row in `algo_lib::_db.imdb`, fills its
   `imdb` / function-pointer fields, and `imdb_XrefMaybe`'s it
   so `ind_imdb_Find(<ns>)` works.
2. For each global field marked `reflect:Y`, builds an
   `algo::Imtable` value with its `_RowidFind` / `_N` / element
   printer / size, and registers it via
   `algo_lib::imtable_InsertMaybe`.

The actual emitted C++ looks like:

```c++
namespace gate { // ...
    static void InitReflection() {
        algo_lib::FImdb &row = algo_lib::imdb_Alloc();
        row.imdb              = "gate";
        row.InsertStrptrMaybe = gate::InsertStrptrMaybe;
        row.RemoveStrptrMaybe = gate::RemoveStrptrMaybe;
        row.Step              = gate::Step;
        row.MainLoop          = gate::MainLoop;
        algo_lib::imdb_XrefMaybe(row);

        algo::Imtable t_session;
        t_session.imtable     = "gate.session";
        t_session.ssimfile    = "gate.session";
        t_session.size        = sizeof(gate::FSession);
        t_session.c_RowidFind = (algo::ImrowRowidFindFcn)session_RowidFind;
        t_session.NItems      = (algo::ImrowNItemsFcn) session_N;
        t_session.Print       = (algo::ImrowPrintFcn)  FSession_Print;
        algo_lib::imtable_InsertMaybe(t_session);
        // ... one block per reflected pool ...
    }
}
```

Whether `InsertStrptrMaybe` / `RemoveStrptrMaybe` are non-NULL
depends on whether the namespace declares any `finput` tables
(`amc::HasFinputsQ`).  A namespace with no finputs gets `NULL`
function pointers — there's nothing to load.  `Step` is non-NULL
only when at least one `fstep` exists in the namespace.

### The reflect flag and per-field accessors
<a href="#the-reflect-flag-and-per-field-accessors"></a>

A pool, Val, or other global field has to be opted into reflection
with `dmmeta.field.reflect:Y`.  When set:

- `tclass_<reftype>` generates `<field>_RowidFind(i32 rowid)`
  (returns `ImrowPtr` to that element) and `<field>_N()`
  (count).  See `cpp/amc/val.cpp` (for Val) and the analogous
  blocks in `lary.cpp`, `tary.cpp`, etc.  Without these
  accessors, the field can't be registered.
- The element ctype must have a string printer (`HasStringPrintQ`)
  for the Imtable's `Print` field to be wired.  Without it,
  `Print` is `NULL` and tools that walk the table can read its
  size and count but not print individual rows.

For example, the auto-generated `<ns>.trace` ctype is always
marked `reflect:Y` so `acr -t <ns>.trace` and an external trace
dump can introspect live counters — see
[Trace counters](/txt/exe/amc/trace.md).

### Consumers in the tree
<a href="#consumers-in-the-tree"></a>

The reflection layer is what lets cross-cutting infrastructure
work without per-namespace plumbing.  The most visible consumers:

- **`algo_lib`'s crash handler** dumps every linked namespace's
  trace counters when the process crashes:
  ```c++
  // cpp/lib/algo/backtrace.cpp
  ind_beg(algo_lib::_db_imdb_curs, imdb, algo_lib::_db) {
      algo_lib::FImtable* it = algo_lib::ind_imtable_Find(
          tempstr() << imdb.imdb << ".trace");
      algo::ImrowPtr row = it && it->c_RowidFind
          ? it->c_RowidFind(0) : algo::ImrowPtr();
      if (it && row && it->Print) {
          tempstr temp;
          it->Print(row, temp);
          // emit non-zero attrs from the printed tuple
      }
  } ind_end;
  ```
  Each namespace contributes one trace row (rowid 0), the
  `Print` printer turns the row into a key/value string, and the
  handler emits only the non-zero counters.
- **An external trace dump** against a running process uses the
  same `imdb` / `imtable` lookups to walk the process's live
  trace state by name.
- **`acr_in <ns> -data`** uses `Imtable.NItems` and `c_RowidFind`
  to enumerate any reflected table.
- **The namespace's own `LoadSsimfileMaybe`** is generated
  alongside `InitReflection` (see
  `tfunc_Global_LoadSsimfileMaybe` in the same file).  When a
  namespace's `LoadTuples` runs at startup, it dispatches to the
  right `<table>_InputMaybe` by reading the ssimfile name out of
  the corresponding `Imtable` row.

### Inserting records at runtime
<a href="#inserting-records-at-runtime"></a>

`Imdb.InsertStrptrMaybe` lets one part of the program hand a
ssim-tuple-formatted string to a namespace and have it routed
through the right per-table loader — exactly as if the line had
been read from a file:

```c++
algo_lib::FImdb &gate = *algo_lib::ind_imdb_Find("gate");
gate.InsertStrptrMaybe(strptr(
    "gate.session  session:abc123  login:alice  start:2025-04-12T10:00:00"
));
```

This is what `acr -insert` does when poking data into an
embedded namespace and what
[Dispsigcheck](#dispsigcheck-piggybacks-on-reflection) uses to
load its signature catalog at startup.  `RemoveStrptrMaybe` is
the mirror — pkey-only.

### Driving the main loop through reflection
<a href="#driving-the-main-loop-through-reflection"></a>

`Imdb.Step` and `Imdb.MainLoop` give an outer process a uniform
way to run any embedded namespace's scheduler.  A multi-tenant
host that links three independent `<ns>::FDb` databases doesn't
need a switch over namespace names — it can iterate
`algo_lib::_db.imdb` and call each `Step` or `MainLoop` in turn.

```c++
ind_beg(algo_lib::_db_imdb_curs, imdb, algo_lib::_db) {
    if (imdb.Step) {
        imdb.Step();
    }
} ind_end;
```

In practice this is rare — most processes have a single
namespace driving their main loop — but it is the mechanism that
makes "host one of these in someone else's main loop" trivial
when needed.

### Dispsigcheck piggybacks on reflection
<a href="#dispsigcheck-piggybacks-on-reflection"></a>

The same `InitReflection` function also seeds
[Dispsigcheck](/txt/exe/amc/dispatch.md#signatures-dispsig-dispsigcheck)
state.  After registering imdb/imtable rows, the generator emits
one `InsertStrptrMaybe` call per dispatch declared in the
namespace:

```c++
algo_lib::InsertStrptrMaybe(
    "dmmeta.Dispsigcheck  dispsig:'gate.In'  signature:'A8F2…'");
```

These records flow into `algo_lib`'s in-memory copy of the
`dispsigcheck` ssimfile.  When peers exchange handshake messages,
each side computes its dispatch signatures from its compiled-in
ssim metadata and compares them — a mismatch (one side rebuilt,
the other not) fires a single clear error instead of a silent
decode garble downstream.

### When to use reflection
<a href="#when-to-use-reflection"></a>

Reflection is powerful but narrow in scope: it describes the
**current process's own linked namespaces** and what those
namespaces can do with strings (load, remove, step).  The OpenACR
codebase prefers the alternative most of the time:

- Need to know what tables some *other* process loads?  Use
  `acr_in <target> -list` or load `dev.finput` for that target —
  no need to interrogate a running binary.
- Need the schema of a ctype anywhere in the system?  Read
  `dmmeta.ctype` / `dmmeta.field` directly.
- Need to manipulate ssim tables that aren't even compiled into
  the current binary?  `acr` operates on the files on disk —
  reflection has no role.

Reflection is the right answer when:

- You're inside a running process and you want to dump or
  manipulate something in *its* memory.  Examples: the crash
  handler dumping trace counters, a trace dump injected into a
  running process, host-embeds-tenant scenarios.
- You want one infrastructure path (the crash handler, an
  HTTP-exposed debug endpoint, an `eval`-like REPL) to work for
  any namespace that gets added later — without per-namespace
  plumbing.

### Pitfalls
<a href="#pitfalls"></a>

- **`reflect:Y` is per-field, not per-namespace.**  Pools that
  should appear in `imtable` need the explicit flag.  Forgetting
  it causes an external trace dump to silently miss the table.
- **No `Print` → row contents unreadable.**  The Imtable's
  `Print` field is NULL if the element ctype lacks a
  `cfmt:String  printfmt:Tuple` declaration.  The table is still
  enumerable and addressable, but tools that show values will
  skip it.
- **`Step` / `MainLoop` may be NULL.**  Always check before
  calling.  A namespace that's just a data container (no fstep,
  no main) won't have them.
- **`GetTrace` is for whole-record snapshot.**  Don't confuse
  with the per-counter access provided by `imtable` lookup on
  `<ns>.trace` — `GetTrace` copies the full struct, the imtable
  path lets you walk individual fields.
- **Registration is at namespace `Init` time.**  Before that,
  `ind_imdb_Find("<ns>")` returns nothing.  This matters in
  early-startup code paths.
- **Element address must remain stable.**  The Imtable's
  `c_RowidFind` returns a raw `ImrowPtr` to the element in
  place.  Reflected pools should use stable-address reftypes
  ([Lary](/txt/exe/amc/reftype/Lary.md),
  [Lpool](/txt/exe/amc/reftype/Lpool.md),
  [Tpool](/txt/exe/amc/reftype/Tpool.md)) — never
  [Tary](/txt/exe/amc/reftype/Tary.md) for reflected data.
- **The list of imtables is not pre-sorted.**  Iterate
  `algo_lib::_db_imtable_curs` if order matters, or filter by
  `imdb.imdb` prefix.

### See also
<a href="#see-also"></a>

- [Runtime](/txt/exe/amc/runtime.md) — the `Step`/`MainLoop`
  functions referenced by Imdb
- [Trace counters](/txt/exe/amc/trace.md) — the most common
  reflection consumer
- [Dispatches](/txt/exe/amc/dispatch.md) — Dispsigcheck rides
  the same registration path
- [I/O](/txt/exe/amc/io.md) — `InsertStrptrMaybe` /
  `LoadTuples` plumbing
- Source: `cpp/amc/global.cpp` (`tfunc_Global_InitReflection`,
  `tfunc_Global_LoadSsimfileMaybe`)
- Schema: `acr 'ctype:algo.Imdb%'`, `acr 'ctype:algo.Imtable'`

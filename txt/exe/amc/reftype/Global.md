## amc Reftype: Global
<a href="#amc-reftype-global"></a>

`Global` is the **root of every access path** in a namespace.
Each amc namespace `<ns>` has exactly one field with
`reftype:Global`, named `_db`, whose `arg` is the namespace's
FDb ctype.  That field declares an extern singleton — actual
storage is `<ns>::_db` at file scope — from which every
[Lary](/txt/exe/amc/reftype/Lary.md), [Tpool](/txt/exe/amc/reftype/Tpool.md),
[Tary](/txt/exe/amc/reftype/Tary.md), [Thash](/txt/exe/amc/reftype/Thash.md),
cross-reference index, and step in the namespace hangs.

Global is also where amc attaches **all the namespace-level
plumbing**: `main`, `Main`, `MainLoop`, `Step`,
`InitReflection`, ssim load/save, command-line dispatch, and
the lookup table that lets `algo_lib` find and operate on
every linked-in namespace at runtime.

```ssim
dmmeta.ctype  ctype:abt.FDb
  dmmeta.field  field:abt.FDb._db  arg:abt.FDb  reftype:Global  dflt:""  comment:""
```

### What it generates
<a href="#what-it-generates"></a>

`tclass_Global` itself is empty (`cpp/amc/global.cpp`);
`Global.Init` is empty too.  All the heavy lifting is in the
tfuncs.  The output falls into four groups:

**1. The singleton itself.**  amc emits
`namespace <ns> { extern FDb _db; }` in the header and
`FDb <ns>::_db;` in the cpp file.  `<ns>::_db` is the runtime
container for the whole namespace.

**2. Program startup.**  When the namespace has
`dmmeta.main`, amc emits `int main(int, char **)` (or
`WinMain` on Windows), which:
- saves `argc/argv` to `algo_lib::_db.argc / argv`,
- calls `<ns>::InitReflection()` and `algo_lib::Init()`,
- parses the command line via `<ns>::ReadArgv`,
- calls user-implemented `<ns>::Main()`,
- on exit, prints `algo_lib::_db.errtext` and returns the
  exit code.

**3. ssim load/save.**  When the namespace has
`dmmeta.finput` records (input tables) or `dmmeta.foutput`
records (output tables), amc generates the loader and saver
families documented in the I/O section below.

**4. The reflection table.**  `InitReflection` registers the
namespace in `algo_lib::_db.imdb`, populating function
pointers for `InsertStrptrMaybe`, `Step`, `MainLoop`,
`GetTrace` so other processes can drive this namespace
generically.

### Ssim inputs
<a href="#ssim-inputs"></a>

Required for every namespace with an in-memory database:

- `dmmeta.field` with `reftype:Global`.  `arg:` is the FDb ctype
  of the namespace, field name is conventionally `_db`.

Optional opt-ins:

- `dmmeta.main` — emits `main()` (else the namespace is library-only).
- `dmmeta.fstep` records — wired into `Step` and `MainLoop`.
- `dmmeta.finput` on FDb pool fields — wires those tables into
  `InsertStrptrMaybe`, the ssim loader, and the `-load` command
  line.
- `dmmeta.foutput` on FDb pool fields — wires those tables into
  `SaveTuples`.
- `dmmeta.gstatic` on FDb pool fields — see "gstatic" below.
- `dmmeta.ccmdline` — generates argv parsing into the command
  ctype.

### Generated functions
<a href="#generated-functions"></a>

Source: `cpp/amc/global.cpp`.

#### Singleton & program entry
<a href="#singleton-program-entry"></a>

| Tfunc                  | Generated function                              | Emitted when |
|------------------------|-------------------------------------------------|--------------|
| `Global.Init`          | (folded into FDb's ctor)                        | always       |
| `Global.main`          | `int main(int argc, char **argv)`               | `dmmeta.main` present |
| `Global.WinMain`       | `int WinMain(...)`                              | Windows GUI builds |
| `Global.Main`          | user-extern `void <ns>::Main()`                 | `dmmeta.main` present |
| `Global.ReadArgv`      | `void <ns>::ReadArgv(strptr help, ...)`         | when ctype has a `cmdline` |
| `Global.InitReflection`| `static void InitReflection()`                  | always; registers ns in `algo_lib::_db.imdb` |

#### Step scheduling
<a href="#step-scheduling"></a>

| Tfunc                  | Generated function                       | Effect |
|------------------------|------------------------------------------|--------|
| `Global.Step`          | `void <ns>::Step()`                      | Calls each `fstep`'s `_Call` in declaration order. |
| `Global.Steps`         | `void <ns>::Steps()`                     | Calls `Step()` on every parent ns's dependency, then this ns. |
| `Global.MainLoop`      | `void <ns>::MainLoop()`                  | Repeatedly calls `Step` until `algo_lib::_db.next_loop` says quiesce.  Drains epoll between cycles. |

#### Ssim I/O
<a href="#ssim-i-o"></a>

| Tfunc                       | Generated function                                              | Effect |
|-----------------------------|------------------------------------------------------------------|--------|
| `Global.LoadTuplesMaybe`    | `bool <ns>::LoadTuplesMaybe(strptr root, bool recursive)`        | Dispatch by `root`: file → one file; `"-"` → stdin; directory → walk `dmmeta.finput`-selected ssimfiles. |
| `Global.LoadTuplesFile`     | `bool <ns>::LoadTuplesFile(strptr fname, bool recursive)`        | Open + LoadTuplesFd; missing files are not errors. |
| `Global.LoadTuplesFd`       | `bool <ns>::LoadTuplesFd(Fildes fd, strptr fname, bool recursive)` | Walks the fd line-by-line, calls `InsertStrptrMaybe` and (when recursive) every parent ns's `InsertStrptrMaybe`. |
| `Global.LoadSsimfileMaybe`  | `bool <ns>::LoadSsimfileMaybe(strptr ssimfile)`                  | Like LoadTuplesFile but resolves `ssimfile` against `algo_lib::_db.cmdline.in`. |
| `Global.InsertStrptrMaybe`  | `bool <ns>::InsertStrptrMaybe(strptr line)`                      | Parse the tuple tag, dispatch by `TableId`, call the appropriate finput's `_InputMaybe`. |
| `Global.RemoveStrptrMaybe`  | `bool <ns>::RemoveStrptrMaybe(strptr line)`                      | Reverse of above; removes records that match the parsed tuple. |
| `Global.SaveTuples`         | `u32 <ns>::SaveTuples(strptr root)`                              | For every `foutput` field, write its table to `<root>/<ssimfile>.ssim`. |

### Gstatics
<a href="#gstatics"></a>

A `dmmeta.gstatic  field:<ns>.FDb.<pool>  ns:<defining-ns>`
record declares that **rows of `<pool>` are baked into the
binary at compile time**.  When amc processes a gstatic field,
it:

1. Collects every tuple of that ctype that any input ssimfile
   provides.
2. Emits a C-style anonymous struct in the namespace
   initializer carrying:
   - the ssim tuple as a string literal, and
   - one function pointer per `Hook` field on the ctype.
3. Generates `Init_<pool>_Static()` that iterates the table
   and calls the corresponding `<pool>_InputMaybe`, populating
   the pool at namespace `Init` time.
4. For each `Hook` field on the ctype, generates a
   user-implementable extern function whose name encodes the
   tuple's pkey — see `cpp/amc/io.cpp` `Gstatic_DataTable`
   and `cpp/amc/fwddecl.cpp` `Hook_FwdDecl`.

That last point makes gstatics the canonical way to attach
**named, statically-dispatched behavior** to a row.  Example:
the amc `gen` table is itself a gstatic — every gen-phase row
has a `step` hook bound at compile time to a function like
`amc::gen_prep_signature`, so registration is purely
declarative.

#### Gstatic vs Finput
<a href="#gstatic-vs-finput"></a>

- **`finput`** — table is loaded *at runtime* from a
  ssimfile.  Rows can change between runs.
- **`gstatic`** — table is *baked into the binary*.  Rows are
  fixed at compile time; hooks bind by name.

They are mutually exclusive on the same field — amc rejects
both with `"Finput and gstatic cannot be specified together"`.

### Pitfalls
<a href="#pitfalls"></a>

- **Only one Global per namespace.**  Conventional name is
  `_db`; the field name appears in many generated symbols
  (`<ns>::_db_<pool>_curs`), so renaming it breaks every
  consumer.
- **Singleton initialization order.**  `<ns>::_db` is a
  file-scope global; its constructor runs at C++ static-init
  time.  Don't reference it from another translation unit's
  static initializer — the order is undefined.
- **`Main()` is user-extern.**  amc generates the forward
  declaration; the user writes the body in `cpp/<ns>/main.cpp`
  (or similar).
- **`LoadTuplesMaybe(dir, recursive:true)`** also loads parent
  namespaces' inputs.  Use `recursive:false` if your process
  only needs its own data.
- **Gstatic hook names encode the pkey.**  `Hook_FwdDecl`
  generates a unique extern for each tuple; if the pkey
  changes, the extern name changes and you have to update the
  C++ implementation.
- **`SaveTuples` is silent on errors.**  Per-file writes use
  `SafeStringToFile`; a failed write is recorded in the return
  count but does not throw.

### See also
<a href="#see-also"></a>

- [Reftypes index](/txt/exe/amc/reftype.md)
- [Runtime / Main loops & steps](/txt/exe/amc/runtime.md)
- [I/O — LoadTuples / SaveTuples / gstatic](/txt/exe/amc/io.md)
- [Reflection — `algo_lib::_db.imdb`](/txt/exe/amc/reflection.md)
- [Backend / pipeline](/txt/exe/amc/backend/pipeline.md)
- Source: `cpp/amc/global.cpp`, `cpp/amc/io.cpp`, `cpp/amc/fwddecl.cpp`
- Tfunc records: `acr 'tfunc:Global.%'`
- Gstatic spec: `acr 'dmmeta.gstatic:%'`

### Example
<a href="#example"></a>

Minimal namespace skeleton:

```ssim
dmmeta.ns         ns:myapp  nstype:exe
dmmeta.ctype      ctype:myapp.FDb
dmmeta.field      field:myapp.FDb._db  arg:myapp.FDb  reftype:Global
dmmeta.main       ns:myapp
dmmeta.ccmdline   cmdline:myapp        basecmdline:algo_lib.Cmdline
```

After running amc and writing a `Main()` in `cpp/myapp/`, you
have a working executable that loads its inputs, parses argv,
calls `Main`, and exits.

Loading data into the namespace at runtime:

```c++
if (!myapp::LoadTuplesMaybe("data", false)) {
    prerr(algo_lib::_db.errtext);
    return 1;
}
```

Saving the in-memory database back to disk:

```c++
u32 nwrite = myapp::SaveTuples("out");
prlog("saved "<<nwrite<<" files");
```

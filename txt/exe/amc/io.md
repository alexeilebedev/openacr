## amc Feature: I/O — LoadTuples / SaveTuples / gstatic
<a href="#amc-feature-i-o-loadtuples-savetuples-gstatic"></a>

amc-generated processes treat **ssim files as their primary
input format**.  An executable's input tables are declared in
ssim, amc generates the parser and the table-load
infrastructure, and the runtime loads everything at startup
from a directory of `.ssim` files.  Three related concepts
cover the full life cycle:

| Concept       | When                              | Where the data lives    |
|---------------|-----------------------------------|--------------------------|
| **finput**    | Loaded from disk at startup       | ssim file in `data/` (or wherever `-in` points) |
| **gstatic**   | Compiled into the binary           | C++ array in the generated `.cpp` |
| **foutput**   | Written back to disk on request   | ssim file in the `-out` directory  |

This page covers all three plus `LoadTuplesFile`,
`LoadTuplesFd`, `LoadSsimfileMaybe`, and
`InsertStrptrMaybe` — the runtime APIs amc generates from
the ssim declarations.

### Table Of Contents
<a href="#table-of-contents"></a>
&nbsp;&nbsp;&bull;&nbsp;  [The ssim load chain](#the-ssim-load-chain)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [finput](#finput)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [foutput](#foutput)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [gstatic](#gstatic)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [ssimfile records](#ssimfile-records)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [InsertStrptrMaybe](#insertstrptrmaybe)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Example](#example)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Pitfalls](#pitfalls)<br/>

### The ssim load chain
<a href="#the-ssim-load-chain"></a>

amc emits a fixed family of loader functions on every
namespace with finputs (see `cpp/amc/global.cpp`):

| Function                                    | Effect |
|---------------------------------------------|--------|
| `bool <ns>::LoadTuplesMaybe(strptr root, bool recursive)` | Top-level entry: branch on `root` being a directory, file, or `"-"` (stdin). |
| `bool <ns>::LoadTuplesFile(strptr fname, bool recursive)` | Open `fname`, hand to `LoadTuplesFd`.  Missing file = success (empty set). |
| `bool <ns>::LoadTuplesFd(Fildes fd, strptr fname, bool recursive)` | Walk the fd line-by-line, parse each line as a tuple, dispatch to `InsertStrptrMaybe`. |
| `bool <ns>::LoadSsimfileMaybe(strptr ssimfile)` | Resolve `ssimfile` (e.g., `dmmeta.field`) against `algo_lib::_db.cmdline.in` and load it. |
| `bool <ns>::InsertStrptrMaybe(strptr line)` | Parse the line's type tag, look up the matching finput, and insert. |
| `bool <ns>::RemoveStrptrMaybe(strptr line)` | Inverse — removes a matching row. |
| `u32  <ns>::SaveTuples(strptr root)`        | For each `foutput`-tagged pool, write `<root>/<ssimfile>.ssim`. |

The `recursive` parameter controls whether parent namespaces'
finputs are also loaded — recursive=true is what `main()`
does at startup, so a process inherits its parent
namespaces' data.

### finput
<a href="#finput"></a>

`dmmeta.finput  field:<ns>.FDb.<pool>  ns:<ns2>  …` declares
that the pool field receives rows of its element type from
ssim input.  Required for amc to wire the table into the
loader.

| Attribute | Meaning |
|-----------|---------|
| `field`   | The FDb pool field that will hold the rows. |
| `ns`      | Defining namespace (typically same as the field's). |
| `update:Y/N` | When `Y`, the generated `InputMaybe` uses `<pool>_UpdateMaybe` instead of `InsertMaybe` — duplicate pkeys update existing rows. |
| `strict:Y/N` | When `Y` (default), a parse error fails `LoadTuplesMaybe`.  When `N`, errors are silently tolerated. |

By default amc generates `<pool>_InputMaybe` to call `InsertMaybe` (or
`UpdateMaybe`).  To take over the input path, declare an
[ffunc](/txt/exe/amc/backend/tclass-tfunc.md#ffunc) `extrn:Y`:

| ffunc | Effect |
|-------|--------|
| `ffunc:<field>.InputMaybe extrn:Y` | amc emits only the `bool <pool>_InputMaybe(P&, elem&)` prototype; you implement it. |
| `ffunc:<field>.Input extrn:Y` | you implement `void <pool>_Input(P&, elem&)`; amc generates `InputMaybe` to call it (wrapped in try/catch where exceptions are on). |

The two are independent — externalize `Input`, `InputMaybe`, neither, or
both.  (A `Val` finput target requires one of these, since `Val` has no
`InsertMaybe`.)  This replaced the former `finput.extrn` column.

The associated ssim file is auto-derived from the element
ctype's `dmmeta.ssimfile` record.  At load time:

1. `<ns>::LoadTuplesMaybe("data", true)` is called by amc's
   `main()`.
2. amc walks each finput in dependency order
   (`_db_c_ssimfile_sorted_curs`), calling
   `LoadTuplesFile` per ssim file.
3. Each line is dispatched by `InsertStrptrMaybe` to the
   matching pool's reader.

`finput` and `gstatic` on the same field are mutually
exclusive — amc errors with `Finput and gstatic cannot be
specified together`.

### foutput
<a href="#foutput"></a>

`dmmeta.foutput  field:<ns>.FDb.<pool>` is the reverse:
register a pool for writing back.  amc generates
`<pool>_SaveSsimfile(fname)` for each foutput, and rolls them
all up into `<ns>::SaveTuples(root)`.

Used by:

- `acr -write` — modify a ssim DB and write it back.
- `acr_my` / `ssim2mysql` — round-trip between ssim and SQL.
- Any tool that mutates its in-memory database and wants to
  persist the change.

The element type must have a `cfmt:String  printfmt:Tuple
print:Y` (i.e., a Print function that emits ssim-tuple form).

### gstatic
<a href="#gstatic"></a>

`dmmeta.gstatic  field:<ns>.FDb.<pool>  ns:<defining-ns>` is
the **compile-time** sibling of finput.  Every row of the
matching ssim file is baked into the **generated C++** as a
static array, and the rows are inserted at namespace `Init`
time — no file I/O at runtime.

The killer feature: when the row type has [Hook](/txt/exe/amc/hook.md)
fields, amc auto-generates one **named extern function** per
row whose name is derived from the pkey.  Linking the binary
fails if any expected function name is missing — the table
of rows is also a declarative dispatch table.

This is how amc's own gen pipeline works
(`amcdb.gen` is a gstatic in `amc.FDb`):

```ssim
amcdb.gen  gen:prep_signature  perns:N  comment:"..."
amcdb.gen  gen:select_ns       perns:N  comment:"..."
...
```

amc generates:

```c++
// auto-generated extern forward decls:
namespace amc {
    void gen_prep_signature();
    void gen_select_ns();
    void gen_dispenum();
    void gen_countxref();
    void gen_detectinst();
    // ...
}

// auto-populated static table at namespace Init:
static struct _t {
    const char *s;
    void (*step)();
} data[] = {
    { "amcdb.gen  gen:prep_signature  ...", &amc::gen_prep_signature },
    { "amcdb.gen  gen:select_ns       ...", &amc::gen_select_ns      },
    // ...
    { NULL, NULL }
};
```

Adding a new row to `amcdb.gen.ssim` and writing the matching
C++ function is the whole extension recipe.  No registration
code, no `#include` plumbing.

| Attribute | Meaning |
|-----------|---------|
| `field`   | FDb pool field that will hold the static rows. |
| `ns`      | Defining namespace (where the hook bodies live). |

### ssimfile records
<a href="#ssimfile-records"></a>

`dmmeta.ssimfile  ssimfile:<full>  ssimns:<nsdb>  ns:<ns>  name:<short>  ctype:<rowtype>`
declares the ssim file itself: its path on disk, its host
namespace, and the ctype of its rows.  Every table loaded by
amc needs one.  amc uses the ssimfile records to:

- Compute the load order (topological sort by pkey
  references in `amcdb.gen:sortssimfile`),
- Resolve the on-disk filename for `LoadSsimfileMaybe`,
- Drive the per-pool `SaveSsimfile`.

### InsertStrptrMaybe
<a href="#insertstrptrmaybe"></a>

The universal "parse one ssim line and insert it into the
right table" entry point.  amc generates one per namespace
with finputs:

```c++
bool <ns>::InsertStrptrMaybe(strptr line) {
    <ns>::TableId table_id(-1);
    value_SetStrptrMaybe(table_id, algo::GetTypeTag(line));
    switch (value_GetEnum(table_id)) {
        case <ns>_TableId_<finput0>_<table0>: {
            <Elem> elem;
            retval = <Elem>_ReadStrptrMaybe(elem, line);
            retval = retval && <finput0>_InputMaybe(elem);
            break;
        }
        // ...one case per finput
    }
    return retval;
}
```

The `TableId` enum is auto-generated from every finput
(see [Dispatch — namespace FieldId / TableId](/txt/exe/amc/dispatch.md#namespace-fieldid-tableid)).
Dispatch is O(1) per line.

### Pitfalls
<a href="#pitfalls"></a>

- **gstatic vs finput is mutually exclusive.**  Pick one per
  field.  amc errors with `Finput and gstatic cannot be
  specified together`.
- **`finput.strict:Y` aborts the whole load on any error.**
  Use `strict:N` for forgiving environments (test fixtures,
  user-edited files).
- **`LoadTuplesFile` treats missing files as success.**
  This is intentional — a process can have optional tables
  that are absent in most environments.  Don't rely on it to
  detect missing required inputs.
- **`LoadTuplesMaybe(dir, recursive:true)` walks parent
  namespaces too.**  Use `false` when your process needs
  only its own data.
- **gstatic hook names are pkey-derived.**  Renaming a
  gstatic row's pkey renames its expected extern C++
  function; the old body becomes orphaned at link time.
- **`SaveTuples` doesn't sort or merge.**  Each `foutput`
  pool produces one file; multiple processes saving the
  same file overwrite each other.
- **`-in` and `-out` are separate.**  Loading from `-in`
  doesn't modify those files; `SaveTuples` writes to
  `-out` independently.  This is why `acr -write` reads,
  modifies, and writes back as three separate steps.

### See also
<a href="#see-also"></a>

- [Global](/txt/exe/amc/reftype/Global.md) — the singleton + main() wiring
- [Strings — cfmt formats](/txt/exe/amc/string.md) — `printfmt:Tuple` is what makes a ctype readable/writable
- [Dispatch — TableId / FieldId](/txt/exe/amc/dispatch.md#namespace-fieldid-tableid)
- [Hook](/txt/exe/amc/hook.md) — used by gstatic for table-driven dispatch
- Source: `cpp/amc/global.cpp`, `cpp/amc/io.cpp`, `cpp/amc/fwddecl.cpp`
- Spec tables: `acr 'dmmeta.finput:%'`, `acr 'dmmeta.foutput:%'`, `acr 'dmmeta.gstatic:%'`, `acr 'dmmeta.ssimfile:%'`

### Example
<a href="#example"></a>

A namespace with finputs, foutput, and a gstatic:

```ssim
dmmeta.ns        ns:myapp  nstype:exe
dmmeta.ctype     ctype:myapp.FDb

dmmeta.field     field:myapp.FDb._db    arg:myapp.FDb  reftype:Global

dmmeta.field     field:myapp.FDb.config arg:myapp.FConfig  reftype:Lary
  dmmeta.finput   field:myapp.FDb.config  ns:myapp

dmmeta.field     field:myapp.FDb.user   arg:myapp.FUser  reftype:Lary
  dmmeta.finput    field:myapp.FDb.user  ns:myapp
  dmmeta.foutput   field:myapp.FDb.user  ns:myapp        # allow saving back

dmmeta.field     field:myapp.FDb.parser  arg:myapp.FParser  reftype:Lary
  dmmeta.gstatic  field:myapp.FDb.parser  ns:myapp        # baked into binary
```

In C++:

```c++
// startup (auto-generated main calls this):
myapp::LoadTuplesMaybe("data", true);

// some code modifies myapp::_db.user...

// before exit, persist user table back to disk:
u32 nfile = myapp::SaveTuples("data");
prlog("wrote " << nfile << " files");
```

The `parser` table is **already populated** at this point —
the gstatic mechanism inserted its rows at namespace
construction.

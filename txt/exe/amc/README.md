## amc - Algo Model Compiler: generate code under include/gen and cpp/gen
<a href="#amc"></a>

`amc` reads the ssim tables under `data/` and writes the C++ that implements
them under `cpp/gen/` and `include/gen/`.  Every executable in the repo is built
from that output, `amc` included.  Run it with no arguments after any change to
the schema, and it brings the generated tree back in line with the tables.

### Syntax
<a href="#syntax"></a>
```usage
amc: Algo Model Compiler: generate code under include/gen and cpp/gen
Usage: amc [[-query:]<string>] [options]
    OPTION        TYPE    DFLT    COMMENT
    -in_dir       string  "data"  Root of input ssim dir
    [query]       string  ""      Query mode: generate code for specified object
    -out_dir      string  "."     Root of output cpp dir
    -proto                        Print prototype
    -showcomment          Y       Show generated comments
    -report               Y       Final report
    -e                            Open matching records in editor
    -derive                       Derive and write the amc-owned tables; generate no source
    -verbose      flag            Verbosity level (0..255); alias -v; cumulative
    -debug        flag            Debug level (0..255); alias -d; cumulative
    -trace        string  ""      Trace expression: category[:filter],...; also payload_lim:N, verbose, debug, timestamps
    -help                         Print help and exit; alias -h
    -version                      Print version and exit
    -signature                    Show signatures and exit; alias -sig
```

### Description
<a href="#description"></a>

#### Regenerating the tree
<a href="#regenerating-the-tree"></a>

A plain `amc` loads every ssimfile under `data/` and generates code for every
namespace.  Each namespace gets three files: `cpp/gen/<ns>_gen.cpp` holds the
function bodies, `include/gen/<ns>_gen.h` the declarations, and
`include/gen/<ns>_gen.inl.h` the inline bodies.  A namespace that holds a
projected ctype also gets a file in each language it is projected into, such as
`ts/gen/<ns>_gen.ts` (see [foreign-language projection](/txt/exe/amc/lang.md)).  `amc` writes a file only
when its contents changed, so an up-to-date tree comes out of a run untouched.

The same run rewrites a set of tables that `amc` owns: `gendb.ctypelen`,
`gendb.dispsig`, `gendb.payloadhdr`, `gendb.msg`, `gendb.msgfield`,
`gendb.tracefld`, `gendb.tracerec` and `gendb.cppsym`.  `amc` computes
their rows from the rest of the schema and writes them through `acr`.  Never
edit these tables by hand, since the next run puts them back.

The run ends with a `report.amc` line that counts what it generated and wrote.
A full run over this tree generates ~2M lines in ~630 files, and
`n_filemod:0` means the tree was already current.  The generated output is
committed, so `git diff` after a run shows what a schema change did to the code.
[acr_ed](/txt/exe/acr_ed/README.md) runs `amc` itself after every `-write`, so a
schema edit made through it needs no separate run.

#### Printing the code for one ctype or function
<a href="#printing-the-code-for-one-ctype-or-function"></a>

Give `amc` a query and it prints generated code to stdout and writes nothing.
The query is `ctype:<regx>`, `func:<regx>`, or a bare regx that searches both.
A ctype prints as its struct declaration, with its constants when it has any.
A function prints as its body, or as its prototype under `-proto`.  A regx with
no dot in it names a namespace, so `amc algo_lib` means `amc algo_lib.%`.

A generated function's key is `<ctype>.<field>.<name>` for a field's function
and `<ctype>..<name>` for the ctype's own.  The `// func:` comment above each
function prints its key, so a broad query shows the keys a narrow one can ask for.

#### Editing the records before a run
<a href="#editing-the-records-before-a-run"></a>

`amc -e <query>` opens the records the query names in `$EDITOR`, through
`acr -e`, together with their whole cross-reference tree.  A bare regx
selects the `ns`, `ctype`, `field` and `dispatch` records it matches.  When you
save and quit, `amc` runs a normal regeneration over the edited tables.  A
failed edit stops `amc` before it generates anything.

#### Deriving the amc-owned tables only
<a href="#deriving-the-amc-owned-tables-only"></a>

`amc -derive` rewrites the tables listed above and generates no source.  A
namespace that compiles one of those tables into its code, as a `gstatic`, reads
the table's file at the start of the run.  A run whose derivation changes the
table therefore generates that namespace from the old rows.  Run `amc -derive`
first and a plain `amc` after it, and the second run reads the new rows.

### See also
<a href="#see-also"></a>

- [Code generation](/txt/openacr/codegen.md) sets `amc` in the pipeline from
  ssim to binary, and [the amc intro](/txt/exe/amc/intro.md) explains why the
  generator exists.
- [The reftype index](/txt/exe/amc/reftype.md) lists every reftype a field can
  have, with one page each for its options and the functions it generates.
- The feature pages describe each kind of code `amc` produces:
  [pools](/txt/exe/amc/pool.md), [xrefs](/txt/exe/amc/xref.md),
  [inheritance](/txt/exe/amc/inheritance.md), [I/O](/txt/exe/amc/io.md),
  [runtime](/txt/exe/amc/runtime.md), [reflection](/txt/exe/amc/reflection.md),
  [command lines](/txt/exe/amc/cmdline.md), [strings](/txt/exe/amc/string.md),
  [enums](/txt/exe/amc/enum.md), [decimals](/txt/exe/amc/decimal.md),
  [bitsets](/txt/exe/amc/bitset.md), [charsets](/txt/exe/amc/charset.md),
  [regular expressions](/txt/exe/amc/regx.md),
  [big-endian fields](/txt/exe/amc/bigendian.md),
  [size assertions](/txt/exe/amc/csize.md), [protocols](/txt/exe/amc/proto.md),
  [dispatches](/txt/exe/amc/dispatch.md), [presence masks](/txt/exe/amc/pmask.md),
  [varlen fields](/txt/exe/amc/varlen.md), [FAST](/txt/exe/amc/fast.md),
  [protobuf](/txt/exe/amc/pbuf.md), [Kafka](/txt/exe/amc/kafka.md),
  [subprocesses](/txt/exe/amc/exec.md), [byte buffers](/txt/exe/amc/fbuf.md),
  [hooks](/txt/exe/amc/hook.md), [trace counters](/txt/exe/amc/trace.md),
  [TypeScript](/txt/exe/amc/js.md) and
  [foreign-language projection](/txt/exe/amc/lang.md).
- The backend pages cover `amc`'s own internals: the
  [pipeline](/txt/exe/amc/backend/pipeline.md), the
  [tclass and tfunc model](/txt/exe/amc/backend/tclass-tfunc.md), the
  [output layout](/txt/exe/amc/backend/output.md),
  [extending amc](/txt/exe/amc/backend/extending.md) and the
  [data model](/txt/exe/amc/backend/data-model.md).
- [amc_vis](/txt/exe/amc_vis/README.md) draws the access paths `amc` generates,
  [amc_gc](/txt/exe/amc_gc/README.md) finds records a target can do without,
  [atf_amc](/txt/exe/atf_amc/README.md) tests the generated code, and
  [toamc](/txt/script/toamc.md) turns hand-written C++ into ssim records.

### Examples
<a href="#examples"></a>

```bash
amc                                     # regenerate everything that changed
amc dmmeta.Ctype                        # print the generated struct for dmmeta.Ctype
amc dmmeta.Ns..Print                    # print the body of one generated function
amc 'amc.FDb.ind_ctype.%' -proto        # list the functions of one index, with comments
amc 'func:dmmeta.Ctype..%' -proto -showcomment:N   # bare prototypes, one per line
amc -e ctype:dmmeta.Ns                  # edit a ctype and its fields, then regenerate
amc -out_dir:                           # load and generate without writing, to check the schema
wt amc -reset -diff -- amc              # run amc in the amc sandbox and show the diff
```

A query prints the code and then the report line:

```
$ amc 'amc.FDb.ind_ctype.%' -proto -showcomment:N
inline bool amc::ind_ctype_EmptyQ() __attribute__((nothrow));
amc::FCtype* amc::ind_ctype_Find(const algo::strptr& key) __attribute__((__warn_unused_result__, nothrow));
amc::FCtype& amc::ind_ctype_FindX(const algo::strptr& key);
amc::FCtype& amc::ind_ctype_GetOrCreate(const algo::strptr& key) __attribute__((nothrow));
...
report.amc  n_cppfile:0  n_cppline:9  n_ctype:5135  n_func:165827  n_xref:2547  n_filemod:0
```

### Caveats
<a href="#caveats"></a>

- Nothing under `cpp/gen`, `include/gen` or `ts/gen` survives the next run, so
  change the record the code came from, or change `amc`.
- `amc` reads `data/` strictly.  An attribute that names no field of its ctype
  stops the run with `unrecognized attr` and the file and line that carry it.
- A generation error suppresses all output, so a failed run leaves the tree as
  it was.  The run then prints `amc.no_output` and exits non-zero.
- When you change `amc` itself, run the new binary in the `amc` sandbox with
  `wt amc -reset -diff -- amc`.  A broken generator run in the main tree
  overwrites its own generated source, and it can then no longer rebuild
  itself.

### Options
<a href="#options"></a>
#### -in_dir -- Root of input ssim dir
<a href="#-in_dir"></a>

The directory `amc` loads the ssim tables from, `data` by default.  Point it at
another dataset to generate from that schema.

#### -query -- Query mode: generate code for specified object
<a href="#-query"></a>

Print the generated code for the ctypes and functions the query names, and
write no files.  The query is `ctype:<regx>`, `func:<regx>` or a bare regx; see
[Printing the code](#printing-the-code-for-one-ctype-or-function).

#### -out_dir -- Root of output cpp dir
<a href="#-out_dir"></a>

The root the generated files and the amc-owned tables are written under, the
current directory by default.  An empty value, `-out_dir:`, writes nothing at
all, which makes a full run a check that the schema generates cleanly.

#### -proto -- Print prototype
<a href="#-proto"></a>

With a query, print each matching function's prototype and leave out its body.
Add `-showcomment:N` for one bare prototype per line.

#### -showcomment -- Show generated comments
<a href="#-showcomment"></a>

With a query, print the comment above each function along with its `// func:`
key.  It is on by default, and `-showcomment:N` strips the comments from the
output.

#### -report -- Final report
<a href="#-report"></a>

Print the `report.amc` line at the end of the run, on by default.  It counts the
C++ files and lines written, the ctypes, functions and xrefs in the model, and
`n_filemod`, the files the run changed.  `-report:N` leaves it out.

#### -e -- Open matching records in editor
<a href="#-e"></a>

Open the records the query names in `$EDITOR`, then regenerate.  A failed edit
stops the run before any code is generated; see
[Editing the records](#editing-the-records-before-a-run).

#### -derive -- Derive and write the amc-owned tables; generate no source
<a href="#-derive"></a>

Rewrite `gendb.ctypelen`, `gendb.dispsig` and the other amc-owned tables, and
write no source.  Run it before a plain `amc` when a namespace compiles one of
those tables into its code; see
[Deriving the tables](#deriving-the-amc-owned-tables-only).

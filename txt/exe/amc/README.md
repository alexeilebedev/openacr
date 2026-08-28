## amc - Algo Model Compiler: generate code under include/gen and cpp/gen
<a href="#amc"></a>

`amc` reads ssim relational tables and emits C++ headers and source
files under `cpp/gen/` and `include/gen/`.  Every other tool in the
repo is built from `amc` output, including `amc` itself.

This document is the entry point of the amc manual.  It is
organized in four sections:

1. **Tool & CLI** — invocation, options, companion tools, the
   `inputs` reference table.
2. **Generated features** — one page per kind of code amc
   produces (enums, presence masks, protocols, dispatches, cross-
   references, strings, runtime steps, …).  Read these to learn
   *what amc can build for you*.
3. **Reftypes reference** — one page per reftype, including every
   option, every variant, and every generated function.  Read
   these to learn *how to spell what you want in ssim*.
4. **The amc backend** — the gen table, the tclass/tfunc dispatch
   model, output-file layout, how to extend amc with a new reftype
   or a new gen phase.  Read these to *modify amc itself*.

Release notes are at the bottom.

For the philosophy behind the generative approach see
[Intro](/txt/exe/amc/intro.md).
For a map of amc's own source modules run `doc ns:amc`.

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
    -trace        regx    ""      Regx of something to trace code generation
    -derive                       Derive and write the amc-owned tables; generate no source
    -verbose      flag            Verbosity level (0..255); alias -v; cumulative
    -debug        flag            Debug level (0..255); alias -d; cumulative
    -help                         Print help and exit; alias -h
    -version                      Print version and exit
    -signature                    Show signatures and exit; alias -sig
```

### 1. Tool & CLI
<a href="#1-tool-cli"></a>

&#128196; [Intro — philosophy & motivation](/txt/exe/amc/intro.md)<br/>

The `Syntax`, `Options`, and `Inputs` sections below are
auto-generated from the ssim schema by `abt_md`.

#### Companion tools
<a href="#companion-tools"></a>

**amc_gc — garbage-collect unused records and includes.**
Rebuilds a specified target while sequentially dropping records
that match `-key`.  Records that can be removed without breaking
the build are reported as dead.  With `-include`, the same logic
applies to `#include` lines instead of ssim records.

**amc_vis — access-path diagrams.**  Walks the field graph and
renders an ASCII or Graphviz `.dot` diagram of the pointer chains
rooted at each global FDb.  `-check` runs dependency consistency
checks without rendering.

**toamc — extract amc schema from C++ source.**  `bin/toamc`
reads a hand-written C++ file and emits the ssim records that
would generate it.  See [/txt/script/toamc.md](/txt/script/toamc.md).

The default invocation prints a one-line report:

    $ amc
    report.amc  n_cppfile:123  n_cppline:258301  n_ctype:970  n_func:22524  n_xref:460  n_filemod:0

`amc` runs at roughly 1M generated LOC per second.  Generated
outputs are versioned in git, so changes can be traced with
`git annotate`.

To experiment with amc itself, run it inside a sandbox so that a
broken generator phase cannot prevent its own recovery:

```bash
wt amc -reset -diff -- amc
```

### 2. Generated features
<a href="#2-generated-features"></a>

What amc generates, organized by feature.  Each page describes a
single capability: what ssim you write, what C++ comes out, the
options and variants, an example, and pitfalls.

#### Core in-memory database
<a href="#core-in-memory-database"></a>
&#128196; [Memory pools — allocators overview](/txt/exe/amc/pool.md)<br/>
&#128196; [Cross-references (xrefs) — group-by / partitioned indexes](/txt/exe/amc/xref.md)<br/>
&#128196; [Inheritance — Base, Castbase/Castdown, Pkey, Upptr](/txt/exe/amc/inheritance.md)<br/>
&#128196; [I/O — LoadTuples, SaveTuples, gstatic](/txt/exe/amc/io.md)<br/>
&#128196; [Runtime — main loops, steps, scheduling](/txt/exe/amc/runtime.md)<br/>
&#128196; [Reflection — imdb, table metadata, FieldId](/txt/exe/amc/reflection.md)<br/>
&#128196; [Command-line parsing — command.X tables](/txt/exe/amc/cmdline.md)<br/>

#### Values, strings, numbers
<a href="#values-strings-numbers"></a>
&#128196; [Strings — strptr, cstring, tempstr, Smallstr, Numstr](/txt/exe/amc/string.md)<br/>
&#128196; [Enums — fconst, gconst, conversion functions](/txt/exe/amc/enum.md)<br/>
&#128196; [Decimals — Dec / fdec fixed-point](/txt/exe/amc/decimal.md)<br/>
&#128196; [Bitsets — fbitset over integers or arrays](/txt/exe/amc/bitset.md)<br/>
&#128196; [Charsets — Charset reftype, Match](/txt/exe/amc/charset.md)<br/>
&#128196; [Regular expressions — Regx, RegxSql](/txt/exe/amc/regx.md)<br/>
&#128196; [Big-endian fields — fbigend get/set](/txt/exe/amc/bigendian.md)<br/>
&#128196; [Compile-time size & offset assertions](/txt/exe/amc/csize.md)<br/>

#### Wire protocols & codecs
<a href="#wire-protocols-codecs"></a>
&#128196; [Protocols — TLV / TV messages, packing, varlen](/txt/exe/amc/proto.md)<br/>
&#128196; [Dispatches — dispatch, dispsig, dispctx, disptrace](/txt/exe/amc/dispatch.md)<br/>
&#128196; [Presence masks — pmask, pmaskfld](/txt/exe/amc/pmask.md)<br/>
&#128196; [Varlen trailing fields — Varlen / Opt / lenfld](/txt/exe/amc/varlen.md)<br/>
&#128196; [FAST presence codec](/txt/exe/amc/fast.md)<br/>
&#128196; [pbuf codec (protobuf)](/txt/exe/amc/pbuf.md)<br/>
&#128196; [Kafka codec](/txt/exe/amc/kafka.md)<br/>

#### I/O & process glue
<a href="#i-o-process-glue"></a>
&#128196; [Subprocess invocation — Exec](/txt/exe/amc/exec.md)<br/>
&#128196; [Fbuf — byte buffer with epoll integration](/txt/exe/amc/fbuf.md)<br/>
&#128196; [Hooks — callback fields](/txt/exe/amc/hook.md)<br/>
&#128196; [Trace counters — ftrace, disptrace, usertracefld](/txt/exe/amc/trace.md)<br/>

#### Frontend bindings
<a href="#frontend-bindings"></a>
&#128196; [JavaScript / TypeScript emission](/txt/exe/amc/js.md)<br/>

### 3. Reftypes reference
<a href="#3-reftypes-reference"></a>

&#128196; [Reftypes index — one-line per reftype](/txt/exe/amc/reftype.md)<br/>

Every reftype has its own page under
`/txt/exe/amc/reftype/` with the full template: flags, ssim
inputs, variants, generated-function table, memory model,
cross-reference behaviour, pitfalls.  See the
[reftypes index](/txt/exe/amc/reftype.md) for navigation.

### 4. The amc backend
<a href="#4-the-amc-backend"></a>

How amc itself is built.  Read these if you are modifying amc or
adding a new reftype.

&#128196; [Pipeline — the gen table, phase by phase](/txt/exe/amc/backend/pipeline.md)<br/>
&#128196; [Tclass / tfunc dispatch model](/txt/exe/amc/backend/tclass-tfunc.md)<br/>
&#128196; [Output layout — per-namespace files, report schema](/txt/exe/amc/backend/output.md)<br/>
&#128196; [Extending amc — add a reftype or a gen phase](/txt/exe/amc/backend/extending.md)<br/>
&#128196; [Internal data model — FCtype, FField, FNs, genctx](/txt/exe/amc/backend/data-model.md)<br/>

### 5. Release notes
<a href="#5-release-notes"></a>

&#128196; [amc release notes](/txt/exe/amc/relnotes.md)<br/>

### Options
<a href="#options"></a>
#### -in_dir -- Root of input ssim dir
<a href="#-in_dir"></a>

Directory containing the ssim input tree.  Defaults to `data`.

#### -query -- Query mode: generate code for specified object
<a href="#-query"></a>

When amc receives a positional argument it runs in **query mode**.
No files are touched: amc simply prints to stdout the generated
code section whose key matches the regex (usually a ctype or
function name).  This is the fastest way to inspect how a single
function is generated.

Example: dump every function generated for amc itself

```bash
amc amc.%
```

#### -out_dir -- Root of output cpp dir
<a href="#-out_dir"></a>

Directory where `cpp/gen/` and `include/gen/` will be written.

#### -proto -- Print prototype
<a href="#-proto"></a>

Used with `-query`; restricts the output to function prototypes.

#### -showcomment -- Show generated comments
<a href="#-showcomment"></a>

Print the generator's comment trail interleaved with the C++.

#### -report -- Final report
<a href="#-report"></a>

Emit the one-line stats report after generation.  See
[backend/output.md](/txt/exe/amc/backend/output.md) for the report
schema.

#### -e -- Open matching records in editor
<a href="#-e"></a>

When combined with a query, open the matching ssim records in
`$EDITOR` (via `acr -e`).  When the edit is applied, amc proceeds with
a normal code generation run; if the edit fails, amc aborts without
regenerating anything.

#### -trace -- Regx of something to trace code generation
<a href="#-trace"></a>

Match a regex against generator phase / function names; matching
runs emit detailed diagnostics.  See
[backend/pipeline.md](/txt/exe/amc/backend/pipeline.md) for the
phase list.

#### -derive -- Derive and write the amc-owned tables; generate no source
<a href="#-derive"></a>

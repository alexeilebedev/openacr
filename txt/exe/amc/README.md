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
For an auto-generated map of source modules see
[Internals](/txt/gen/amc/amc.md).

### Table Of Contents
<a href="#table-of-contents"></a>
<!-- abt_md.toc_beg -->
&nbsp;&nbsp;&bull;&nbsp;  [Internals](#internals)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Syntax](#syntax)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [1. Tool & CLI](#1-tool-cli)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [2. Generated features](#2-generated-features)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [3. Reftypes reference](#3-reftypes-reference)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [4. The amc backend](#4-the-amc-backend)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [5. Release notes](#5-release-notes)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Options](#options)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
&#128196; [amc Feature: Big-endian fields — fbigend](/txt/exe/amc/bigendian.md)<br/>
&#128196; [amc Feature: Bitsets — fbitset over integers or arrays](/txt/exe/amc/bitset.md)<br/>
&#128196; [amc Feature: cfmt — print/read format declarations](/txt/exe/amc/cfmt.md)<br/>
&#128196; [amc Feature: Charsets — Charset reftype, Match](/txt/exe/amc/charset.md)<br/>
&#128196; [amc Feature: Command-line parsing](/txt/exe/amc/cmdline.md)<br/>
&#128196; [amc Feature: Compile-time size & offset assertions](/txt/exe/amc/csize.md)<br/>
&#128196; [amc Feature: Decimals — Dec / fdec fixed-point](/txt/exe/amc/decimal.md)<br/>
&#128196; [amc Feature: Dispatches — dispatch, dispsig, dispctx, disptrace](/txt/exe/amc/dispatch.md)<br/>
&#128196; [amc Enumerated Types](/txt/exe/amc/enum.md)<br/>
&#128196; [amc Subprocess Invocation](/txt/exe/amc/exec.md)<br/>
&#128196; [amc Feature: FAST presence codec](/txt/exe/amc/fast.md)<br/>
&#128196; [amc - fbuf](/txt/exe/amc/fbuf.md)<br/>
&#128196; [amc Feature: Fcmap — bidirectional enum mapping](/txt/exe/amc/fcmap.md)<br/>
&#128196; [fcond: list membership tied to a condition on a field](/txt/exe/amc/fcond.md)<br/>
&#128196; [amc Feature: Hooks — callback fields](/txt/exe/amc/hook.md)<br/>
&#128196; [amc Feature: Inheritance — Base, Castbase/Castdown, Pkey, Upptr](/txt/exe/amc/inheritance.md)<br/>
&#128196; [amc Intro](/txt/exe/amc/intro.md)<br/>
&#128196; [amc Feature: I/O — LoadTuples / SaveTuples / gstatic](/txt/exe/amc/io.md)<br/>
&#128196; [amc Feature: JavaScript / TypeScript emission](/txt/exe/amc/js.md)<br/>
&#128196; [amc Feature: Kafka codec](/txt/exe/amc/kafka.md)<br/>
&#128196; [amc Feature: pbuf codec (protobuf)](/txt/exe/amc/pbuf.md)<br/>
&#128196; [amc Presence Masks](/txt/exe/amc/pmask.md)<br/>
&#128196; [amc Memory Pool](/txt/exe/amc/pool.md)<br/>
&#128196; [amc Protocols](/txt/exe/amc/proto.md)<br/>
&#128196; [amc Reflection](/txt/exe/amc/reflection.md)<br/>
&#128196; [amc Reftype - index](/txt/exe/amc/reftype.md)<br/>
&#128196; [amc Feature: Regular expressions — Regx, RegxSql](/txt/exe/amc/regx.md)<br/>
&#128196; [amc release notes](/txt/exe/amc/relnotes.md)<br/>
&#128196; [amc Runtime](/txt/exe/amc/runtime.md)<br/>
&#128196; [amc Feature: Sorting — fsort and fcmp](/txt/exe/amc/sort.md)<br/>
&#128196; [amc String](/txt/exe/amc/string.md)<br/>
&#128196; [amc Feature: Trace counters — ftrace, disptrace, usertracefld](/txt/exe/amc/trace.md)<br/>
&#128196; [amc Feature: Varlen trailing fields — Varlen / Opt / lenfld](/txt/exe/amc/varlen.md)<br/>
&#128196; [amc Cross-References (xrefs)](/txt/exe/amc/xref.md)<br/>
&#128196; [amc Backend: internal data model](/txt/exe/amc/backend/data-model.md)<br/>
&#128196; [amc Backend: extending amc](/txt/exe/amc/backend/extending.md)<br/>
&#128196; [amc Backend: output layout](/txt/exe/amc/backend/output.md)<br/>
&#128196; [amc Backend: the gen-table pipeline](/txt/exe/amc/backend/pipeline.md)<br/>
&#128196; [amc Backend: tclass / tfunc dispatch](/txt/exe/amc/backend/tclass-tfunc.md)<br/>
&#128196; [amc Reftype: Alias](/txt/exe/amc/reftype/Alias.md)<br/>
&#128196; [amc Reftype: Atree](/txt/exe/amc/reftype/Atree.md)<br/>
&#128196; [amc Reftype: Base](/txt/exe/amc/reftype/Base.md)<br/>
&#128196; [amc Reftype: Bheap](/txt/exe/amc/reftype/Bheap.md)<br/>
&#128196; [amc Reftype: Bitfld](/txt/exe/amc/reftype/Bitfld.md)<br/>
&#128196; [amc Reftype: Blkhash](/txt/exe/amc/reftype/Blkhash.md)<br/>
&#128196; [amc Reftype: Blkpool](/txt/exe/amc/reftype/Blkpool.md)<br/>
&#128196; [amc Reftype: Charset](/txt/exe/amc/reftype/Charset.md)<br/>
&#128196; [amc Reftype: Count](/txt/exe/amc/reftype/Count.md)<br/>
&#128196; [amc Reftype: Cppstack](/txt/exe/amc/reftype/Cppstack.md)<br/>
&#128196; [amc Reftype: Ctype](/txt/exe/amc/reftype/Ctype.md)<br/>
&#128196; [amc Reftype: Delptr](/txt/exe/amc/reftype/Delptr.md)<br/>
&#128196; [amc Reftype: Exec](/txt/exe/amc/reftype/Exec.md)<br/>
&#128196; [amc Reftype: Fbuf](/txt/exe/amc/reftype/Fbuf.md)<br/>
&#128196; [amc Reftype: Global](/txt/exe/amc/reftype/Global.md)<br/>
&#128196; [amc Reftype: Hook](/txt/exe/amc/reftype/Hook.md)<br/>
&#128196; [amc Reftype: Inlary](/txt/exe/amc/reftype/Inlary.md)<br/>
&#128196; [amc Reftype: Lary](/txt/exe/amc/reftype/Lary.md)<br/>
&#128196; [amc Reftype: Llist](/txt/exe/amc/reftype/Llist.md)<br/>
&#128196; [amc Reftype: Lpool](/txt/exe/amc/reftype/Lpool.md)<br/>
&#128196; [amc Reftype: Malloc](/txt/exe/amc/reftype/Malloc.md)<br/>
&#128196; [amc Reftype: Opt](/txt/exe/amc/reftype/Opt.md)<br/>
&#128196; [amc Reftype: Pkey](/txt/exe/amc/reftype/Pkey.md)<br/>
&#128196; [amc Reftype: Ptr](/txt/exe/amc/reftype/Ptr.md)<br/>
&#128196; [amc Reftype: Ptrary](/txt/exe/amc/reftype/Ptrary.md)<br/>
&#128196; [amc Reftype: Regx](/txt/exe/amc/reftype/Regx.md)<br/>
&#128196; [amc Reftype: RegxSql](/txt/exe/amc/reftype/RegxSql.md)<br/>
&#128196; [amc Reftype: Sbrk](/txt/exe/amc/reftype/Sbrk.md)<br/>
&#128196; [amc Reftype: Smallstr](/txt/exe/amc/reftype/Smallstr.md)<br/>
&#128196; [amc Reftype: Tary](/txt/exe/amc/reftype/Tary.md)<br/>
&#128196; [amc Reftype: Thash](/txt/exe/amc/reftype/Thash.md)<br/>
&#128196; [amc Reftype: Tpool](/txt/exe/amc/reftype/Tpool.md)<br/>
&#128196; [amc Reftype: Upptr](/txt/exe/amc/reftype/Upptr.md)<br/>
&#128196; [amc Reftype: Val](/txt/exe/amc/reftype/Val.md)<br/>
&#128196; [amc Reftype: Varlen](/txt/exe/amc/reftype/Varlen.md)<br/>
&#128196; [amc Reftype: ZSListMT](/txt/exe/amc/reftype/ZSListMT.md)<br/>
<!-- abt_md.toc_end -->

### Internals
<a href="#internals"></a>
&#128196; [amc - Internals](/txt/gen/amc/amc.md)<br/>

### Syntax
<a href="#syntax"></a>
```
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
&#128196; [Internals — auto-generated module index](/txt/gen/amc/amc.md)<br/>

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

```
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

```
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

### Inputs
<a href="#inputs"></a>
`amc` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|
|[dmmeta.anonfld](/txt/ssimdb/dmmeta/anonfld.md)|Omit field name where possible (command line, enums, constants)|
|[dmmeta.argvtype](/txt/ssimdb/dmmeta/argvtype.md)|Customize parsing of command lines (rarely used)|
|[dmmeta.basepool](/txt/ssimdb/dmmeta/basepool.md)|Specify pool to be used for allocating elements of a type|
|[dmmeta.bitfld](/txt/ssimdb/dmmeta/bitfld.md)|Specify offset/width of a bitfield within another field|
|[dmmeta.blkhash](/txt/ssimdb/dmmeta/blkhash.md)|Block hash index, required for fields with reftype Blkhash|
|[amcdb.bltin](/txt/ssimdb/amcdb/bltin.md)|Specify properties of a C built-in type|
|[dmmeta.cafter](/txt/ssimdb/dmmeta/cafter.md)|Control amc processing order for unrelated types (used in rare situations)|
|[dmmeta.cascdel](/txt/ssimdb/dmmeta/cascdel.md)|Request cascading delete of referenced rows|
|[dmmeta.ccmdline](/txt/ssimdb/dmmeta/ccmdline.md)|tie cmdline to basecmdline|
|[dmmeta.ccmp](/txt/ssimdb/dmmeta/ccmp.md)|Generate hash function|
|[dmmeta.cdflt](/txt/ssimdb/dmmeta/cdflt.md)|Specify default value for single-value types that lack fields|
|[dmmeta.cextern](/txt/ssimdb/dmmeta/cextern.md)|Externally defined ctype (a struct from system header, or from a c++ library)|
|[dmmeta.cfast](/txt/ssimdb/dmmeta/cfast.md)|FAST properties for ctype|
|[dmmeta.cfmt](/txt/ssimdb/dmmeta/cfmt.md)|Specify options for printing/reading ctypes into multiple formats|
|[dmmeta.cget](/txt/ssimdb/dmmeta/cget.md)|Generate state functions for these ctypes|
|[dmmeta.charset](/txt/ssimdb/dmmeta/charset.md)|Generate functions to determine if a character is a member of a set|
|[dmmeta.chash](/txt/ssimdb/dmmeta/chash.md)|Generate hash function|
|[dmmeta.cjsbltin](/txt/ssimdb/dmmeta/cjsbltin.md)||
|[dmmeta.cjson](/txt/ssimdb/dmmeta/cjson.md)||
|[dmmeta.ckafka](/txt/ssimdb/dmmeta/ckafka.md)||
|[dmmeta.cpbuf](/txt/ssimdb/dmmeta/cpbuf.md)||
|[dmmeta.cppfunc](/txt/ssimdb/dmmeta/cppfunc.md)|Value of field provided by this expression|
|[dmmeta.cpptype](/txt/ssimdb/dmmeta/cpptype.md)|Ask amc for a fieldwise constructor|
|[dmmeta.csize](/txt/ssimdb/dmmeta/csize.md)|Specify size/alignment for built-in C++ types|
|[dmmeta.cstr](/txt/ssimdb/dmmeta/cstr.md)|Specify that type behaves like a string|
|[dmmeta.ctype](/txt/ssimdb/dmmeta/ctype.md)|Struct|
|[dmmeta.dispatch](/txt/ssimdb/dmmeta/dispatch.md)|Generate code for a multi-way branch|
|[dmmeta.dispatch_msg](/txt/ssimdb/dmmeta/dispatch_msg.md)|Add message to a dispatch|
|[dmmeta.dispctx](/txt/ssimdb/dmmeta/dispctx.md)|Use context with dispatch|
|[dmmeta.dispfilter](/txt/ssimdb/dmmeta/dispfilter.md)|Generate filter function on dispatch|
|[dmmeta.disptrace](/txt/ssimdb/dmmeta/disptrace.md)|Generate trace fields (cycles, counts) for all dispatch branches|
|[dmmeta.falias](/txt/ssimdb/dmmeta/falias.md)|Alias field|
|[dmmeta.fbigend](/txt/ssimdb/dmmeta/fbigend.md)|Annotate field as having big-endian storage|
|[dmmeta.fbitset](/txt/ssimdb/dmmeta/fbitset.md)|Generate bitset functions over integer field or array|
|[dmmeta.fbuf](/txt/ssimdb/dmmeta/fbuf.md)|Buffer for reading/writing messages, works with Iohook|
|[dmmeta.fbuftype](/txt/ssimdb/dmmeta/fbuftype.md)|Buffer type|
|[dmmeta.fcast](/txt/ssimdb/dmmeta/fcast.md)|Generate implicit conversion from field to c++ expression|
|[dmmeta.fcmap](/txt/ssimdb/dmmeta/fcmap.md)|Bidirectional mapping between fconst values|
|[dmmeta.fcmp](/txt/ssimdb/dmmeta/fcmp.md)|Request versionsort or case-insensitive sort for field|
|[dmmeta.fcompact](/txt/ssimdb/dmmeta/fcompact.md)|Request compaction upon removal from index|
|[dmmeta.fcond](/txt/ssimdb/dmmeta/fcond.md)|List membership tied to a condition on a field|
|[dmmeta.fconst](/txt/ssimdb/dmmeta/fconst.md)|Specify enum value (integer + string constant) for a field|
|[dmmeta.fcurs](/txt/ssimdb/dmmeta/fcurs.md)|Request generation of custom cursor|
|[dmmeta.fdec](/txt/ssimdb/dmmeta/fdec.md)|Specify that field has an implied # of decimal places and specify formatting options|
|[dmmeta.fdelay](/txt/ssimdb/dmmeta/fdelay.md)|Control elapsed time between execution of a step|
|[dmmeta.ffast](/txt/ssimdb/dmmeta/ffast.md)|FAST properties for field|
|[dmmeta.fflag](/txt/ssimdb/dmmeta/fflag.md)|Options for command-line flags|
|[dmmeta.ffunc](/txt/ssimdb/dmmeta/ffunc.md)|User-selected field function; pkey is <field>.<name> where name is a tfunc|
|[dmmeta.field](/txt/ssimdb/dmmeta/field.md)|Specify field of a struct|
|[dmmeta.finput](/txt/ssimdb/dmmeta/finput.md)|Describe input table of a program|
|[dmmeta.fkafka](/txt/ssimdb/dmmeta/fkafka.md)||
|[dmmeta.fldoffset](/txt/ssimdb/dmmeta/fldoffset.md)|Assert field offset - will result in compile-time error if violated|
|[dmmeta.floadtuples](/txt/ssimdb/dmmeta/floadtuples.md)|Request that process automatically load any input tables on startup|
|[dmmeta.fnoremove](/txt/ssimdb/dmmeta/fnoremove.md)|Omit any functions for removing elements from table; Table is append-only|
|[dmmeta.foutput](/txt/ssimdb/dmmeta/foutput.md)|Generate function to save index/table back to disk|
|[dmmeta.fpbuf](/txt/ssimdb/dmmeta/fpbuf.md)||
|[dmmeta.fpbufcase](/txt/ssimdb/dmmeta/fpbufcase.md)||
|[dmmeta.fprefix](/txt/ssimdb/dmmeta/fprefix.md)|Allowed pairing of field-name prefix and reftype|
|[dmmeta.fregx](/txt/ssimdb/dmmeta/fregx.md)|Specify options for command-line regx field|
|[dmmeta.fsort](/txt/ssimdb/dmmeta/fsort.md)|Generate custom sort function for array field|
|[dmmeta.fstep](/txt/ssimdb/dmmeta/fstep.md)|Generate a main loop step to be executed whenever a field is non-empty|
|[dmmeta.ftrace](/txt/ssimdb/dmmeta/ftrace.md)|Generate counters for a field: allocations, buffer traffic, or step cycles|
|[dmmeta.func](/txt/ssimdb/dmmeta/func.md)||
|[dmmeta.funique](/txt/ssimdb/dmmeta/funique.md)|This field must be unique in the table. Not needed for primary key|
|[dmmeta.fwddecl](/txt/ssimdb/dmmeta/fwddecl.md)|Request forward declaration of a field|
|[dmmeta.gconst](/txt/ssimdb/dmmeta/gconst.md)|Import ssim table columns as fconst for a field|
|[dmmeta.gstatic](/txt/ssimdb/dmmeta/gstatic.md)|Load entries for this table at startup time|
|[dmmeta.gsymbol](/txt/ssimdb/dmmeta/gsymbol.md)|Create C++ symbols from entries in ssim table|
|[dmmeta.hook](/txt/ssimdb/dmmeta/hook.md)|Required on Hook fields|
|[dmmeta.inlary](/txt/ssimdb/dmmeta/inlary.md)|Generate inline array of fixed or variable length (all entries fit within parent struct)|
|[dmmeta.jstype](/txt/ssimdb/dmmeta/jstype.md)||
|[dmmeta.lenfld](/txt/ssimdb/dmmeta/lenfld.md)|Specify which gives length of varlen portion in bytes|
|[dev.license](/txt/ssimdb/dev/license.md)|Source code license|
|[dmmeta.listtype](/txt/ssimdb/dmmeta/listtype.md)|Specify structure of linked list based on field prefix|
|[dmmeta.llist](/txt/ssimdb/dmmeta/llist.md)|Options for Llist field|
|[dmmeta.main](/txt/ssimdb/dmmeta/main.md)|Generate Main function for namespace|
|[dmmeta.msgtype](/txt/ssimdb/dmmeta/msgtype.md)|Specify message type for each eligible message, controls dispatch|
|[dmmeta.nocascdel](/txt/ssimdb/dmmeta/nocascdel.md)|Remove cascade delete for this index|
|[dmmeta.nossimfile](/txt/ssimdb/dmmeta/nossimfile.md)|Indicates that ssimfile does not exist for this ssimdb ctype|
|[dmmeta.ns](/txt/ssimdb/dmmeta/ns.md)|Namespace (for in-memory database, protocol, etc)|
|[dmmeta.nscpp](/txt/ssimdb/dmmeta/nscpp.md)|Generate C++ code for this namespace|
|[dmmeta.nsdb](/txt/ssimdb/dmmeta/nsdb.md)|Annotate ssimdb namespaces|
|[dmmeta.nsinclude](/txt/ssimdb/dmmeta/nsinclude.md)|Explicitly specify a C++ include file for namespace|
|[dmmeta.nsjs](/txt/ssimdb/dmmeta/nsjs.md)|Generate JavaScript code for this namespace|
|[dmmeta.nsproto](/txt/ssimdb/dmmeta/nsproto.md)|Annotate protocol namespace (collection of types, no state)|
|[dmmeta.nsx](/txt/ssimdb/dmmeta/nsx.md)|Control code-generation and exception handling options for process/library|
|[dmmeta.numstr](/txt/ssimdb/dmmeta/numstr.md)|Add functions to read numbers out of a string field|
|[dmmeta.pack](/txt/ssimdb/dmmeta/pack.md)|Request byte-packing of structure fields|
|[dmmeta.pmaskfld](/txt/ssimdb/dmmeta/pmaskfld.md)|Specify which field holds presence mask bits (one for each field in struct)|
|[dmmeta.pmaskfld_member](/txt/ssimdb/dmmeta/pmaskfld_member.md)|Explicit field-to-pmask association|
|[dmmeta.pnew](/txt/ssimdb/dmmeta/pnew.md)|Generate custom constructor (placement new), for use with binary protocols|
|[dmmeta.ptrary](/txt/ssimdb/dmmeta/ptrary.md)|Required for fields with reftype:Ptrary|
|[dmmeta.rowid](/txt/ssimdb/dmmeta/rowid.md)|Initialize field to row id of element|
|[dmmeta.smallstr](/txt/ssimdb/dmmeta/smallstr.md)|Generated fixed-length padded or length-delimited string field|
|[dmmeta.sortfld](/txt/ssimdb/dmmeta/sortfld.md)|Specify what field an index (Bheap,Atree) is sorted on|
|[dmmeta.ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|File with ssim tuples|
|[dmmeta.ssimsort](/txt/ssimdb/dmmeta/ssimsort.md)|Define sort order for ssimfile|
|[dmmeta.ssimvolatile](/txt/ssimdb/dmmeta/ssimvolatile.md)|Types based on ssim files marked this way cannot be cross-referenced|
|[dmmeta.steptype](/txt/ssimdb/dmmeta/steptype.md)|Type of scheduler step|
|[dmmeta.substr](/txt/ssimdb/dmmeta/substr.md)|Specify that the field value is computed from a substring of another field|
|[dev.targdep](/txt/ssimdb/dev/targdep.md)|Dependency between targets|
|[dev.target](/txt/ssimdb/dev/target.md)|Build target|
|[dmmeta.tary](/txt/ssimdb/dmmeta/tary.md)|Indirect linear dynamically alocated array (used for strings, arrays of data, etc)|
|[amcdb.tcurs](/txt/ssimdb/amcdb/tcurs.md)|Cursor template|
|[dmmeta.thash](/txt/ssimdb/dmmeta/thash.md)|Hash index, required for fields with reftype Thash|
|[dmmeta.typefld](/txt/ssimdb/dmmeta/typefld.md)|Specifies which field of a message carries the type|
|[dmmeta.usertracefld](/txt/ssimdb/dmmeta/usertracefld.md)|Add custom user trace fields to process's trace struct|
|[dmmeta.xref](/txt/ssimdb/dmmeta/xref.md)|Specify how to cross-reference (i.e. project, or group-by) one record with another|

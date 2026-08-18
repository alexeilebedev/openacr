## OpenACR
<a href="#openacr"></a>

OpenACR is a meta-programming language for generative systems
programming.  It consists of a set of tools for creating programs and
algorithms, and writing code as data.  OpenACR is published at
[https://github.com/alexeilebedev/openacr](https://github.com/alexeilebedev/openacr).

It is the result of over 15 years of development and production use.
OpenACR was used to build mission-critical financial systems that
handle hundreds of billions of messages daily at microsecond latencies,
financial exchanges, distributed HFT platforms, CI systems, deployment
systems, alerts and monitoring systems.  It was also used to build
itself — over 95% of all the source code in the project is generated
by OpenACR's code generator (amc) from ascii relational tables.

### Table Of Contents
<a href="#table-of-contents"></a>
<!-- abt_md.toc_beg -->
&nbsp;&nbsp;&bull;&nbsp;  [How it fits together](#how-it-fits-together)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Key concepts](#key-concepts)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Tools](#tools)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Main ssimfiles](#main-ssimfiles)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Further reading](#further-reading)<br/>
&#128196; [Acr — Querying and Editing the Ssim Database](/txt/openacr/acr.md)<br/>
&#128196; [Build and Test](/txt/openacr/build.md)<br/>
&#128196; [Code Generation](/txt/openacr/codegen.md)<br/>
&#128196; [Recipes](/txt/openacr/recipes.md)<br/>
&#128196; [Runtime Patterns](/txt/openacr/runtime.md)<br/>
&#128196; [Schema Design and Pitfalls](/txt/openacr/schema.md)<br/>
&#128196; [Setup And Installation](/txt/openacr/setup.md)<br/>
&#128196; [Ssim Fundamentals](/txt/openacr/ssim.md)<br/>
<!-- abt_md.toc_end -->

### How it fits together
<a href="#how-it-fits-together"></a>

The core loop is:

1. **Schema in `data/`** — ssimfiles are the source of truth.
   Each file is a sorted text table of whitespace-separated key:value
   tuples; `acr` queries and modifies them.
2. **`amc` generates code** — it reads every ssimfile and emits
   `cpp/gen/*.cpp` and `include/gen/*.h`.  Never hand-edit those files.
3. **`abt` compiles** — it replaces `make`, reads the schema to find
   source files, and links binaries into `bin/`.
4. **Hand-written C++** in `cpp/<ns>/` calls the generated APIs.  The
   generated code handles all pools, indexes, step dispatch, I/O, and
   serialization; hand-written code contains the business logic.

The typical development cycle:

```
edit data/*.ssim  →  amc  →  abt <target> -install  →  test
       ↑___________acr_ed (edit + amc in one step)___|
```

### Key concepts
<a href="#key-concepts"></a>

**Ssim tuples and ssimfiles** — every piece of configuration, schema,
and build metadata is expressed as a whitespace-separated key:value
tuple stored in a text file under `data/`.  The format is trivially
diffable, greppable, and Git-friendly.  Any ssimfile table can be
round-tripped through MariaDB.  Full coverage:
[Ssim Fundamentals](/txt/openacr/ssim.md) and
[acr reference](/txt/exe/acr/README.md).

**C++ code generation** — `amc` reads the ssim schema and emits a
complete in-memory database for each namespace: pools (Lary, Tpool,
Lpool), hash indexes, linked lists, binary heaps, step dispatch, I/O
buffers, and serializers.  The generated APIs are the primary surface
your hand-written code targets.  Full coverage:
[Code Generation](/txt/openacr/codegen.md) and
[amc reference](/txt/exe/amc/README.md).

**The runtime model** — every OpenACR process runs a single-threaded
cooperative scheduler built around *fsteps*.  A step is a C++ function
associated with a table or timer; the scheduler calls it when the
table is non-empty or the timer fires.  There are no locks; all shared
state is on `FDb`.  Full coverage:
[Runtime Patterns](/txt/openacr/runtime.md).

**Schema design** — choosing the right pool, index, and cross-reference
type determines both the generated API and memory layout.  Common
pitfalls are collected in [Schema Design and Pitfalls](/txt/openacr/schema.md).

### Tools
<a href="#tools"></a>

| Tool | Purpose |
|------|---------|
| [acr](/txt/exe/acr/README.md) | Query / edit the ssim database; transitive closure, regex selection, in-place rewrites, editor and MariaDB integration |
| [acr_ed](/txt/exe/acr_ed/README.md) | Schema editor — creates fields, ctypes, targets, source files; runs `amc` to keep generated code in sync |
| [acr_in](/txt/exe/acr_in/README.md) | Extract the ssimfile inputs (and optionally the data) that a given target reads |
| [amc](/txt/exe/amc/README.md) | Code generator — reads `data/*.ssim` and emits `cpp/gen/*.cpp` / `include/gen/*.h` |
| [src_func](/txt/exe/src_func/README.md) | Source-function index: list, grep, navigate, create missing dispatch handlers |
| [abt](/txt/exe/abt/README.md) | Build tool — replaces make/cmake, regex-selects targets, links and installs |
| [abt_md](/txt/exe/abt_md/README.md) | Markdown linter / regenerator — runs inline commands, refreshes TOCs, checks links |
| [atf_unit](/txt/exe/atf_unit/README.md) | C++ unit-test runner |
| [atf_comp](/txt/exe/atf_comp/README.md) | Component-test runner — captures stdout/stderr from a scripted process invocation, diffs against a reference |
| [atf_ci](/txt/exe/atf_ci/README.md) | CI gate — drives test suites under named `cijob`s (`normalize`, `comp`, `memcheck`, `coverage`) |

### Main ssimfiles
<a href="#main-ssimfiles"></a>

The schema for OpenACR itself lives in a handful of tables under
`data/dmmeta/` and `data/dev/`:

| Table | What it defines |
|-------|----------------|
| `dmmeta.ns` | Namespaces (`lib`, `exe`, `ssimdb`, `protocol`, …) |
| `dmmeta.ctype` | Compound types (records / structs); primary key is `<ns>.<Name>` |
| `dmmeta.field` | Fields of a ctype — type, reftype, default, comment |
| `dmmeta.xref` | In-memory cross-references between ctypes (Thash, Llist, Bheap, Ptrary, …) |
| `dmmeta.thash` | Hash-index definitions for `ind_<x>_Find()` lookups |
| `dmmeta.fstep` | Step associations — which scheduler callback fires for a given index or timer |
| `dmmeta.dispatch_msg` | Auto-routed message handlers (`<ns>.<disp>` → `void ns::Disp_<Msg>(...)`) |
| `dev.target` | Build targets (libs, exes, ssimdbs, protocols) |
| `dev.targdep` | Per-target dependencies |
| `dev.targsrc` | Source files attached to a target |
| `dev.gitfile` | Every file tracked in git |

A complete list of ssim tables: `acr ssimfile`.  Per-table column
docs: `acr field:<ns>.<Ctype>.%`.  Schema dump: `acr %`.  All
ssimfiles are described in detail under
[Ssim Databases](/txt/ssimdb/README.md).

### Further reading
<a href="#further-reading"></a>

- **Setup**: [Setup and Installation](/txt/openacr/setup.md) — install
  dependencies, build, first commands.
- **Tutorials**: [Tutorials](/txt/tut/README.md) — step-by-step
  walkthroughs from hello-world to parallel subprocesses and framed I/O.
- **Per-executable docs**: [Executables](/txt/exe/README.md).
- **Library API**: [Libraries](/txt/lib/README.md).
- **ssimdb schema reference**: [Ssim Databases](/txt/ssimdb/README.md).

---
name: acr
description: Use when working on OpenACR — designing features, reviewing code/plans/ssimfiles, adding generators or schema, understanding architecture, or making any design decision. General-purpose structural thinking guide.
---

# /acr — Think Like Alexei Lebedev

This is not a workflow. It is a way of seeing.

When you look at code, see the nouns that should be tables and the switches that should be loops over records. When you design, name the categories and the verbs first — everything else follows from structure. When you review, ask: what would break if I added one more instance? If the answer is "edit code," the system isn't factorized.

Adapt to context:
- **Designing**: name categories and verbs, propose structures, test with the factorization question
- **Reviewing**: find missing nouns, implicit behavior, violated contracts, unfactored actions
- **Implementing**: add records not code, maintain determinism, preserve uniform contracts
- **Exploring**: `acr <table>` queries any table, `amc_vis` visualizes access paths, `txt/` has 500+ docs

## The Core Insight

amc generates 15–25 lines of C++ per line of ssimfile input. The leverage comes from orthogonal factorization, not magic.

Every independent concern becomes its own axis expressed as a table. Axes combine through composition, not code duplication. 35 reftypes × 11 boolean properties = 35 records and one loop — not 35 switch cases per property check.

**The factorization test:** Can a new capability be added by adding only ssimfile records, with zero code changes? If not, the system isn't factorized yet.

## Principles

### Everything Is Data

If you're adding a code branch, you're doing it wrong. Add a record instead.

`switch(value) { case X: ...; case Y: ...; }` signals a *missing noun* — a table that should exist in `data/dmmeta/`. The switch values are records. The body properties are fields on those records. One loop replaces the switch.

### One Record Per Thing

Enum constants are individual records, not comma-separated lists. Fields are individual records, not nested inside type definitions. Cross-references are explicit records, not implicit relationships.

Ssimfiles are sets of tuples: any concatenation, permutation, or subset is valid. Sub-formats within a field value break this property. When you want to pack multiple values into one field, you need a new table.

### Structure Drives All Effects

Moving a field between tables changes what the entire program can do — without changing any code. Structure implies constraints independent of language: you can't close a window without handling its lines. That's an inference from the structure, not a feature of C++ or Python.

When reviewing data model changes: what was possible before? What's newly possible? What constraints emerged or disappeared?

See `references/concepts.md` for the Smart Window example.

### The System of Names Applies Equally Well to the Description of Itself

`dmmeta.ctype` has a record for `dmmeta.Ctype`. `dmmeta.field` has records for its own fields. Adding a new table type means adding records to the meta-schema, not modifying the parser.

The bootstrap cycle: schema describes amc → amc generates code → code compiles into amc → amc regenerates itself identically. If the cycle doesn't close, something is wrong.

### Singular Everywhere

The table is called `field`, not `fields`. One element of the table is also `field`. The table name is a piece of the element's name: `dmmeta.field` is the table, `dmmeta.field field:amc.FDb.ns` is a record. With plural this system of names would break.

This extends to everything: namespace names, relation names, field names. Singular is not a style preference — it's structural, because the naming system must be composable.

### Categories and Verbs

To understand a system, know what categories (nouns) exist and what operations (verbs) change them.

Before designing anything, name the categories and the verbs. If you can't, the design is muddled. Categories become tables. Verbs become operations on those tables.

### Access Paths, Not Data Structures

In programming, you pick ONE data structure that locks your access pattern. In the database view, you index the same set of records N ways without duplicating data. The set exists independently; indexes are access paths to it.

See `references/concepts.md` for xref composition and the InterfaceMC 8-way indexing example.

### Deterministic Output

Byte-identical output on every run. Entities alphabetically, fields in declaration order. No mutable global state. Generators write to string buffers; a separate writer flushes after generation completes.

This enables `git annotate` on generated code — every line traces to its generating record.

### Uniform Contracts

All generators share the `void()` function pointer interface called through a flat loop over `amcdb.gen` records. 90+ generators, zero special cases. Scope variation (`perns:Y` vs `perns:N`) is a field on the record, not a code branch.

Different signatures per generator = contract violation.

### No Intermediate Representation

Never build AST nodes or IR trees. Generators build strings directly via `<<` operators into string buffers. An IR doubles the debugging surface and adds complexity without benefit — ssimfiles say what to generate; there's nothing to optimize. Strings are transparent; IR trees require tooling to inspect.

See `references/concepts.md` for the full argument.

### Generation/IO Separation

Generators write to string accumulators. A separate writer stage flushes all buffers to disk after generation completes. No file I/O inside generators. This enables testing without I/O and prevents partial files on failure.

### Controlled Vocabularies

Every category of allowed values gets its own table with queryable properties. `dmmeta.nstype`: namespace types. `dmmeta.reftype`: reference types with boolean properties (read `data/dmmeta/reftype.ssim` for current fields). Even field name prefixes are declared as records in `dmmeta.fprefix`, not naming conventions. Validation checks records, not hardcoded lists.

### Referential Integrity as Data

Foreign keys declared via `reftype:Pkey` and `reftype:Upptr` fields. Cross-references are explicit `dmmeta.xref` records with traversal paths. `dmmeta.substr` decomposes composite keys for per-component validation. Invalid data produces zero output.

### Source Provenance

Every loaded record in acr carries its source file and line number (`acr.FRec.p_infile`, `acr.FRec.lineno`). Error messages point to exact ssimfile locations as `file:line`. This makes debugging ssimfiles trivial — integrity violations, missing references, and malformed records all report exactly where the problem is.

### Compiler, Not Framework

amc generates infrastructure that consumers use with full control. It does not impose directory structures, require specific patterns in hand-written code, validate what consumers do with output, or create feedback loops forcing prescribed patterns. The relationship is one-way: compiler produces → consumer uses. Generated code is a superset of functions; consumers pick what they need.

## Coding Patterns

### SESE (Single Entry, Single Exit)

Code flows top to bottom as one continuous line. No early returns mid-function — early return is goto in disguise. Any block of SESE code can be extracted or inlined without surprises.

### Action Factoring

Group by action, not condition:

```cpp
// WRONG: action B duplicated across branches
if (A) { B; C; } else { B; D; }

// RIGHT: factor B out, branch only on what differs
B; if (A) { C; } else { D; }
```

The program corresponds to actions. Conditions compute parameters for those actions.

### Data as Stylesheet

When similar code paths differ only in a value, the value is data and the code is one path. Data supplies parameters; code executes the action once. One loop reading record properties replaces N near-identical code blocks.

## The Missing Noun

The most common design fix. When N hardcoded blocks switch on the same values, those values are a noun missing from the schema.

**You see:** N near-identical code blocks differing in a few values. Each handles one "type."

**You do:** Add a table. The values become records. The differing properties become fields. N blocks collapse into one loop.

**Canonical example:** `dmmeta.reftype`. Val, Upptr, Ptrary, Thash, Bheap are not if-branches — they are records with boolean properties. One loop reads properties. One generator dispatches based on them. Read `data/dmmeta/reftype.ssim` for the full set of reftypes and their properties.

## Anti-Patterns

| If you see... | The fix is... |
|---|---|
| Switch/if-chain on a set of values | Add a table; values are records with properties |
| Repeated code differing in values | Data-driven loop; values become record fields |
| Nested structure in a field value | New table (one record per thing) |
| Generator doing file I/O | Write to buffer; separate writer handles I/O |
| Different generator signatures | Uniform contract; variation is a data field |
| Hand-editing cpp/gen/ or include/gen/ | Run `amc`; generated files are overwritten |
| Code branch in dispatcher | Add record to `amcdb.gen` |
| Hardcoded allowed values | Controlled vocabulary table |
| IR/AST between generator and output | Build strings directly via `<<` |
| Early return mid-function | Refactor to SESE; compute parameters, act once |
| Behavior changes based on naming convention | Explicit field on the record declares the variation |

## Tools and Feedback Loops

| Tool | Use for |
|------|---------|
| `acr <table>` | Query any ssimfile table. `acr dmmeta.field -where:field:amc.FDb.%` |
| `acr -check` | Verify referential integrity across all `data/` |
| `acr_ed -create` | Scaffold ctypes, fields, targets, cross-references |
| `amc` | Regenerate all C++ from schema |
| `abt` / `ai` | Build specific target / full build |
| `normalize` | Full test suite (normalization, component, unit) |
| `amc_vis` | Visualize access path diagrams |
| `git diff cpp/gen/` | Review what `amc` changed |

## References

SKILL.md gives you principles and patterns — enough for most tasks. Load a reference when you need depth:

- `references/concepts.md` — **how mechanisms work.** Load when you need to understand reftypes beyond their names, trace an xref via chain, explain the bootstrap/self-generation cycle, or reason about access path design. Not needed for routine code changes.
- `references/meta-schema.md` — **which table to use.** Load when you need to find the right `dmmeta` table for a task, understand what configuration tables exist for a reftype, or construct `acr` queries. Not needed if you already know the table name.

**In-repo documentation:**
- `txt/exe/amc/README.md` — AMC comprehensive guide
- `txt/exe/amc/reftypes.md` — full reftype system documentation
- `txt/exe/amc/xref.md` — cross-reference system
- `txt/exe/acr/README.md` — ACR database tool guide
- `txt/tut/tut1.md` – `tut9_make.md` — progressive tutorials
- `txt/recipe/` — recipes for common tasks
- `txt/ssim.md` — ssimfile format specification

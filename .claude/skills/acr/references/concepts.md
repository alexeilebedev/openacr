# Concepts

## The Reftype System

Reftypes define how a field relates to its arg type. Each reftype causes amc to generate specific C++ structures and access functions. The reftype is the most important field on `dmmeta.field` — it determines what code amc generates.

**Authoritative sources:** `data/dmmeta/reftype.ssim` for the full set with boolean properties, `txt/exe/amc/reftypes.md` for detailed documentation. The examples below are illustrative, not exhaustive.

### Key Reftypes by Category

**Ownership & Reference:** `Val` (inline value, copy semantics), `Upptr` (upward pointer, child→parent, populated via xref), `Base` (inheritance via field embedding), `Global` (singleton — the FDb pattern)

**Containers (Memory Ownership):** `Tpool` (typed free-list pool), `Lary` (level array, stable pointers — the default allocator), `Tary` (dynamic array, growing), `Inlary` (inline/stack array), `Ptrary` (array of pointers to externally-owned objects)

**Indexes (Cross-Reference Targets):** `Thash` (hash table — Find, GetOrCreate), `Llist` (linked list — configurable via `dmmeta.llist`), `Atree` (AVL tree), `Bheap` (binary heap / priority queue)

**Strings:** `Smallstr` (fixed-size inline, configured via `dmmeta.smallstr`), `RegxSql` (SQL regex pattern)

**Keys & Special:** `Pkey` (string reference to parent table), `Hook` (callback function pointer), `Count` (reference counter), `Bitfld` (bit-level packed field)

### Reftype Properties

Every reftype is a record in `dmmeta.reftype` with boolean properties that determine amc's behavior. For example, `isval` (holds a value vs. reference), `isxref` (derived cross-reference, not stored in ssimfiles), `hasalloc` (manages memory allocation), `cascins` (insertion cascades to children).

Read `data/dmmeta/reftype.ssim` for the complete property set. This is orthogonal factorization in action: adding a new reftype means adding one record with the right boolean properties set. The generators read these properties — no new code branches needed.

### Field Prefix System

Field names encode their reftype via prefix — declared as records in `data/dmmeta/fprefix.ssim`. Selected examples:

| Prefix | Reftype | Meaning |
|--------|---------|---------|
| `p_` | Upptr | Upward pointer (child→parent) |
| `c_` | Ptrary | Cross-reference (child pointer array) |
| `zd_` | Llist | Zero-terminated doubly-linked list |
| `ind_` | Thash | Hash index |
| `bh_` | Bheap | Binary heap |
| `tr_` | Atree | AVL tree |

The prefix tells you at a glance what generated code exists for this field: `ind_ctype` has `Find`, `GetOrCreate`; `zd_proj` has `Insert`, `Remove`, `First`, `Last`, `Next`; `bh_event` has `Insert`, `RemoveFirst`.

Read `data/dmmeta/fprefix.ssim` for the complete set (including additional linked list variants like `zs_`, `cd_`, `cs_`, `zdl_`, etc.). This is "everything is data" applied to naming: the naming system itself is declared in the schema.

## Structure & Access Paths

### The Smart Window

> All effects are connected to structure. Structure is the most important thing in any system and what generates all its manifestations.

Consider a text editor. `Window` has a `file` attribute. Each window shows one file. Lines belong to a window.

Now imagine Smart Window — a window showing lines matching a pattern across ALL files. This feature requires a structural change: move `file` from `Window` to `Line`. Now each line carries its own file reference, and a window can host lines from any file.

But moving that field changes deletion semantics entirely. Before: close a window, its lines are gone. After: delete a line from the view, but the file lives on elsewhere. Like Google Photos — deletion became strange the moment ownership structure changed. You delete, but it just disappears from here, while actually living somewhere else.

No algorithm was modified. No code was rewritten. Structure changed; the entire capability space and constraint set followed. This is why data model review matters more than code review.

### The Field→Ctype→Ns Hierarchy

```
dmmeta.ns (namespace)
  └─ dmmeta.ctype (struct, qualified as ns.Name)
       └─ dmmeta.field (field, qualified as ns.Ctype.fieldname)
```

Primary keys encode the hierarchy. `field:amc.FDb.ind_ns` → namespace `amc`, ctype `FDb`, field `ind_ns`.

`dmmeta.substr` extracts parent components from composite keys for automatic foreign key resolution. This is how referential integrity works without explicit foreign key declarations — the structure of the primary key IS the relationship.

### Cross-References (dmmeta.xref)

A cross-reference is a derived index populated automatically when records are loaded. Not stored in ssimfiles — computed at runtime from the declared relationship.

```
dmmeta.xref  field:abt.FDb.ind_target  inscond:true  via:""
dmmeta.xref  field:abt.FTarget.c_targsrc  inscond:true  via:abt.FDb.ind_target/dev.Targsrc.target
```

Fields:
- **field**: the index field (must use an index reftype: Thash, Llist, Atree, Bheap, Ptrary)
- **inscond**: C++ boolean expression controlling insertion. `true` for unconditional. `row.pid != 0` for conditional.
- **via**: path through tables to reach the parent. Empty string for direct pool→index. Multi-hop for tree structures.

### Via Chains

`via:abt.FDb.ind_target/dev.Targsrc.target` means:
1. Take the record being loaded (a `dev.Targsrc`)
2. Read its `.target` field
3. Look up that value in `FDb.ind_target` (a hash index on FTarget)
4. Insert the record into the found FTarget's child container

Via chains enable automatic population of tree-structured indexes from flat ssimfile records. The loaded data is flat (one line per record); the in-memory structure is a graph of cross-referenced pointers — all derived from xref declarations.

### InterfaceMC — 8-Way Indexing

A packet routing program runs on a machine with multiple network interfaces. For each interface × multicast group combination, there's an `InterfaceMC` record. If 10 interfaces and 100 groups — potentially 1,000 records.

In a programming language, you'd store these in ONE data structure — a hashmap, a vector — and that locks your access pattern. In the database view, InterfaceMC is indexed 8 different ways simultaneously:

- By pool (memory ownership)
- By InfiniBand flag (hardware type filter)
- By read/write mode
- By join events
- By name (hash lookup)
- By interface (child list)
- By multicast group (child list)
- Free list (allocation)

You're a rat in a maze taking shortcuts through corridors. Every shortcut is an access path declared as a `dmmeta.xref` record. The program navigates: global pointer → multicast group → InterfaceMC array → send work request ring buffer → network card. Each hop is a declared cross-reference.

Without multiple access paths, you'd duplicate data or build complex lookup chains in code. With them, each access pattern is a declared index on the same underlying set.

## The FDb Pattern

Every namespace has a `FDb` ctype — the "database" singleton containing all pools, indexes, and global state for that namespace. Accessed via `ns::_db`.

```
dmmeta.ctype  ctype:amc.FDb  comment:"In-memory database for amc"
dmmeta.field  field:amc.FDb.ctype  arg:dmmeta.Ctype  reftype:Tpool  comment:""
dmmeta.field  field:amc.FDb.ind_ctype  arg:amc.FCtype  reftype:Thash  comment:""
```

This declares:
- `amc.FDb.ctype` — a Tpool (memory pool) that owns all FCtype records
- `amc.FDb.ind_ctype` — a Thash (hash index) over those same records

One set of records, two access paths. The pool owns memory; the hash enables O(1) lookup by name. Cross-references tie them together automatically.

## Cursors

Every container reftype generates a cursor (iterator) with a standard interface:

```cpp
ind_beg(ns::parent_child_curs, child, parent) {
    // child is the current element
} ind_end;
```

`ind_beg`/`ind_end` are macros wrapping a standard for-loop. The cursor struct has `Reset`, `ValidQ`, `Next`, and `Access` methods. Cursor type names follow the pattern `{parent_ctype}_{field_name}_curs`.

## The Bootstrap Cycle

1. `data/dmmeta/*.ssim` defines the meta-schema (including records describing amc's own ctypes and fields)
2. `amc` reads these records and generates `cpp/gen/*.cpp` and `include/gen/*.h`
3. `abt` compiles generated code + hand-written `cpp/amc/*.cpp` into the `amc` binary
4. Running the rebuilt `amc` regenerates identical output — the cycle closes

The generated code includes: struct definitions, field accessors, constructors/destructors, memory pool allocators, hash tables, linked lists, trees, cross-reference maintenance, input loading (ssimfile parsing), output serialization, cursor definitions.

All generators run through the same flat dispatch loop reading `amcdb.gen` records. The `perns` field on each gen record determines whether the generator runs once globally or once per namespace.

### Self-Generation and the Fixed Point

amc is not just a self-hosting compiler (like GCC compiling itself). It generates most of its own source code from a schema that describes itself in its own format — `dmmeta.field` has records for the fields of `dmmeta.Field`. This is closer to a quine (a program that outputs its own source) than to a traditional self-hosting compiler.

The generated code (`cpp/gen/`, `include/gen/`) is committed to git. This solves the chicken-and-egg problem: on first clone, the generated C++ is already there — you compile amc from it without needing amc. Once built, amc regenerates identical output. The committed generated code is the fossil record of every stage of amc's evolution — `git log cpp/gen/` traces how the generated layer changed as the schema evolved.

Platform-specific bootstrap scripts in `bin/bootstrap/` handle the very first build step: raw `g++` commands that compile `abt` (the build tool) without needing `abt` itself. From there: `abt` builds `amc` → `amc` regenerates `cpp/gen/` identically → cycle closes.

## No Intermediate Representation — Why

Why amc builds strings directly instead of constructing AST/IR trees:

1. **Second source of truth.** An IR must faithfully represent the output. If it doesn't, you have IR bugs AND output bugs. Two systems to debug instead of one. The IR promises fidelity but can only be verified by... producing the output and comparing. So why not just produce the output?

2. **Optimization illusion.** IRs promise "we can optimize the tree" — reorder nodes, merge duplicates, eliminate dead branches. In practice, code generators emit what the ssimfiles say. There's nothing to optimize. The schema IS the optimization decision. Tree transformations add complexity for zero benefit.

3. **Double complexity.** IR + serializer = two components to build, test, debug, and maintain. The IR needs constructors, visitors, type checking. The serializer needs formatting, indentation, language-specific syntax. String builders need... append.

4. **Opacity vs transparency.** A string buffer is transparent — print it, read it, diff it. An IR tree requires tooling to inspect. You need pretty-printers, visualizers, debug dumps. With strings, the output IS the debug dump.

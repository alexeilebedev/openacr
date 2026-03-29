# acr_nav: What's Next?

## Story So Far

| Milestone | Concept Taught | Key Artifact |
|-----------|---------------|--------------|
| M0 | Vision/design | Master plan |
| M1 | Schema before code | 26 ssimfile records, zero C++ |
| M2 | Code generation + xrefs | amc_vis diagram before any logic |
| M3 | Cursor loops, hash lookups | Batch output mode |
| M4 | Raw terminal, event loop | Single-panel scrollable TUI |
| M5 | Xref traversal, navstack | Two-panel Enter/Backspace navigation |
| M6a | gstatic Hook dispatch | Function pointers replace if-chain |
| M6b | Mode-qualified keybindings | Filter mode, composite key lookup |
| M7a | Component tests | atfdb.comptest, normalize comp |
| M7b | Substr decomposition | Naventry filter state, keybind validation |
| M8 | Keybinding redesign | Vim + normie dual support |
| M11 | TermColor controlled vocabulary | Controlled vocab properties, data as stylesheet |
| M12 | Reverse xref + filter_accept | Access paths: same data, second index |
| M13 | Help overlay, status bar, preview, viewmode, filter UX | Self-describing: docs = data = behavior |
| M14 | Breadcrumb navigation bar | Naventry ctype name, navstack as UI |
| M15 | Help as right-panel viewmode | Controlled vocabulary refinement, startup discoverability |
| M16 | Field detail drilldown + viewmode line-source refactoring | Orthogonal factorization visible; data-driven detailsrc |
| M16.1 | Fix I1-I6: stale names, overlay bug, Ptr, type fix | Naming hygiene, overlay guard, pointer comparison |
| M17 | Inline reftype glossary + detail view enrichment | Controlled vocab properties; on-demand context over always-on noise |
| M18 | Namespace grouping / tree view | Orthogonal axes: left-panel layout independent of right-panel content |

**Current:** ~1150 lines C++, 21 navactions, 37 keybinds, 2 modes, 2 panels, 5 viewmodes (fields/xref/preview/help/detail), 9 navstyles, 36 reftypestyles, ~1449 ctypes / ~5596 fields across 89 namespaces. Namespace-grouped tree view with collapse/expand.

---

## Known Issues

### I7. Detail view is incomprehensible

The detail view (`d` keybind) renders raw ssimfile records — full `dmmeta.field`, `dmmeta.reftype`, `dmmeta.thash`, `dmmeta.xref` lines with every key:value pair. On a typical terminal (80-120 cols), lines truncate mid-field and look like a wall of noise:

```
dmmeta.field  field:abt.FDb.ind_arch  arg:abt.FArch  reftype:Thash  dflt:""  comment:""
dmmeta.reftype  reftype:Thash  isval:N  cascins:N  usebasepool:Y  cancopy:N  isxref:Y  de...
dmmeta.thash  field:abt.FDb.ind_arch  hashfld:dev.Arch.arch  unique:Y  comment:""
dmmeta.xref  field:abt.FDb.ind_arch  inscond:true  via:""
```

A newcomer can't extract meaning from this. Possible fixes: (a) curated projection showing only the most useful fields per record type, (b) vertical key:value layout instead of horizontal ssim, (c) horizontal scrolling, (d) field-level syntax coloring to visually separate keys from values. These aren't mutually exclusive.

---

## Pain Points

### ~~P1. Flat list at scale~~ — Solved (M18: namespace grouping)
1,449 ctypes in a flat alphabetical list across 89 namespaces. Browsing is like reading a phone book. Filter helps, but you need to already know what you're looking for.

### P2. Opaque reftype vocabulary (partially addressed by M17)
Lary, Thash, Ptrary, Upptr, Tpool, Tary, Llist -- the reftype names are terse and carry no visible meaning. M17 added `comment` to `dmmeta.Reftype` and shows it in the detail view (`d` keybind). This helps on-demand but doesn't solve the "what does this mean at a glance?" problem during normal field browsing.

### P3. Tool-switching tax
Typical schema exploration requires five context switches: `acr` to find a type, `amc_vis` for access paths, `acr -t` for xref tree, `src_func` for hand-written code, then an editor. One question ("how does this type work?") scattered across five tools.

### P4. Field metadata hidden
Understanding a single field (its xref wiring, index config, substr decomposition) requires running `acr dmmeta.thash:...`, `acr dmmeta.substr:...`, etc. in separate terminals. The orthogonal factorization that makes the schema elegant also means a field's full story is spread across 5+ tables.

### P5. Silent failures across tools
`acr dmmeta.ctype:nonexistent.Foo` returns empty output, exit 0. Same for `amc_vis nonexistent.Foo`. No "did you mean?", no error. Can't distinguish typo from empty result.

### P6. The F-prefix gap
Generated C++ uses `acr_nav::FNaventry`. Querying `acr dmmeta.field:acr_nav.FNaventry.%` returns nothing -- the real name is `acr_nav.Naventry`. No hint. Trips up everyone who reads generated code first.

### P7. No sense of scale
Browsing ctypes in acr_nav, you can't tell which types have 3 records vs 3000. No record counts, no indication of which tables are the big ones.

### P8. Schema-to-code gap
"What C++ did amc generate for this type?" requires leaving acr_nav. The connection between schema definition and generated code is invisible inside the tool meant to browse the schema.

---

## Ideas by Tier

### Tier 1: High Impact, Natural Fit

These solve the most painful problems, align with OpenACR philosophy, and build on existing architecture.

#### ~~1A. Inline Reftype Glossary~~ — Done (M17)

Implemented: added `comment` field to `dmmeta.Reftype` (all 35 records populated from `amcdb.tclass.comment` with 6 overrides). Reftype comment shown in detail view (`d` keybind) as the full reftype record — on-demand rather than status bar, to avoid displacing keyboard hints with info that's useful once then becomes noise. Also normalized tclass.ssim comments (trailing periods, capitalization) and fixed Delptr typo.

#### ~~1B. Field Detail Drilldown~~ — Done (M16)

Implemented: press `d` on a field to see all its dmmeta metadata (thash, xref, llist, ptrary, substr) gathered from across orthogonal tables. Data-driven via `acr_navdb.detailsrc` — adding new metadata sources costs one record, zero code changes. Also included viewmode line-source refactoring (line/header storage moved from FDb to FViewmode) and `PopViewmode()` helper extraction.

#### ~~1C. Namespace Grouping / Tree View~~ — Done (M18)

Implemented: left panel grouped by namespace with expand/collapse. All namespaces start collapsed (83 headers). Enter on header toggles. LeftItem Tary replaces zd_sel_ctype linked list. FNs.c_ctype Ptrary indexes ctypes by namespace. Extern types (dot-less ctypes like `DIR`, `i32`, `MYSQL`) grouped under "extern" label. Width stable across collapse/expand. Follow-ref auto-expands target namespace. Go-back scans by ctype name (collapse-safe).

#### 1D. Generated Code Preview

New viewmode: "generated". Shows the amc-generated C++ for the selected ctype -- struct definition, access functions, constructors, pool operations. Shell out to `amc <ctype>` and render inline, same pattern as preview mode loading ssimfiles.

**Solves:** P8 (schema-to-code gap), P3 (tool-switching tax).
**Value:** High. Schema and generated code are two views of the same thing -- this makes that visible.
**Size:** Small-Medium. Same architecture as preview mode (shell out, capture output, render in right panel). One new viewmode record.

#### 1E. Record Count Display

For ctypes that have ssimfiles, show the record count next to the ctype name in the left panel. `dmmeta.Ctype (1449)`, `dmmeta.Field (5596)`, `acr_navdb.Keybind (37)`.

**Solves:** P7 (no sense of scale).
**Value:** High ratio of usefulness to effort. Immediate sense of scale -- distinguishes configuration tables (37 keybinds) from data tables (5596 fields) at a glance.
**Size:** Small. Count lines in ssimfiles at startup (or lazily). Only ctypes with a corresponding ssimfile get counts.

#### 1F. Field Name / Comment Search

Filter mode currently searches ctype names. Add a toggle to search field names and comments instead. Results show ctypes containing matching fields, with matches highlighted in the right panel.

**Solves:** Part of P3 (tool-switching tax). "Which ctypes have a field called 'comment'?" currently requires `acr dmmeta.field -where:...`.
**Value:** High. Turns acr_nav into a tool you reach for instead of `acr` for cross-cutting field queries.
**Size:** Medium. Needs a second filter index over fields.

---

### Tier 2: Solid Value, Moderate Effort

These improve the experience meaningfully but solve less acute pain or require more work.

#### 2A. Command Mode

Press `:` for vim-style command input. `:goto <ctype>`, `:ns <regex>`, `:q`. Command dispatch via the same gstatic Hook pattern as navactions.

**Solves:** Direct jump by name is faster than filtering for known targets.
**Value:** Medium. Essential for a daily-driver tool -- experienced users don't want to scroll-and-filter to reach a known type.
**Size:** Medium.
**Design note:** Both filter mode and command mode need free-text capture. The current code handles filter's printable-char capture as a special case. When command mode is designed, factoring `capture_text` into a mode property should be considered.

#### 2B. Access Path Diagram — Static (Inline amc_vis)

New viewmode: "diagram". Shell out to `amc_vis <ctype>`, render output in right panel. Static picture — look but don't touch.

**Solves:** P3 (tool-switching tax for access path visualization).
**Value:** Medium. Removes context-switching to CLI. Cheap to implement.
**Size:** Small. Same shell-out pattern as preview mode.
**Limitation:** Static image. Can't navigate the graph — just a snapshot. amc_vis output can be very wide (150+ columns), needs horizontal scrolling or truncation.

#### 2C. Access Path Diagram — Interactive Graph View

Full interactive graph navigation inside acr_nav. Selected ctype rendered as center node, direct neighbors (field arg ctypes) as surrounding boxes connected by labeled edges (reftype + field name). Tab/Shift-Tab moves between nodes, Enter navigates into a node (push navstack, re-center graph on new ctype), Backspace returns.

**Solves:** P3 (tool-switching tax), and goes further — you can **walk** the access path graph, not just look at a snapshot.
**Value:** High. This is `amc_vis` as an interactive experience, not a batch tool. Combined with headless mode, an agent could navigate the graph programmatically.
**Size:** Medium. What already exists: raw terminal, colors, navstack, viewmode system. What's new: ASCII box drawing (unicode `┌─┐│└─┘` + arrows), simple layout algorithm (center + ring of neighbors, not full amc_vis topological sort), node-to-node cursor movement (Tab order over nodes, no true 2D cursor needed).
**Design note:** Keep layout simple — local neighborhood only (selected ctype + its direct references, 5-15 nodes). Full graph is amc_vis's job. The value is interactivity, not completeness.

#### 2D. Cross-Namespace Dependency View

For the selected ctype's namespace, show upstream dependencies (what it imports via field references) and downstream dependents (what references it). Based on cross-namespace field arg references.

**Solves:** "What namespaces does acr_nav depend on? What depends on dmmeta?" Currently requires multiple `acr -ndown` queries.
**Value:** Medium. Useful for understanding the schema's macro structure.
**Size:** Medium. Needs per-namespace aggregation of field references.

#### 2E. Bookmark / Recent History

Press `m` to bookmark current ctype. Press `'` to see bookmark list. Also show recently-visited types (navstack already tracks history; just needs persistence across sessions or a dedicated view).

**Solves:** Repeatedly navigating to the same handful of types during a dev session.
**Value:** Medium. Most useful during extended exploration sessions.
**Size:** Small-Medium.

---

### Tier 3: Nice to Have, Larger Scope

These are valuable but either serve narrower use cases or require significant effort.

#### 3A. Integrity Check View

New viewmode: "check". Runs `acr -check` and shows only errors related to the selected ctype or namespace. Green indicator when clean.

**Solves:** `acr -check` reports errors as a flat list; hard to find "which errors affect MY ctype?"
**Value:** Low-Medium. Schema is currently clean (zero errors). Most useful when actively editing schema.
**Size:** Medium. Parsing acr -check output, filtering by ctype.

#### 3B. Tutorial / Guided Mode

`:tutorial 1` starts a guided walkthrough inside acr_nav. Highlights specific types, shows explanatory text in the help panel, walks through key concepts interactively.

**Solves:** Tutorials teach concepts but require jumping between docs and terminal.
**Value:** Low. Narrow use case -- significant content authoring effort for infrequent benefit. Better to make the tool self-explanatory through good structure (1C, 1E) and on-demand context (1A, 1B) than to build a separate guided experience.
**Size:** Large.

---

### Standalone: Generalized Headless (_db Pool Dump)

This idea lives outside the tier system because it's an amc-level capability, not an acr_nav feature.

Headless mode in acr_nav serializes curated projections (Screen, PanelState, VisibleField) as ssimfile records. But every amc-generated program already has typed pools in `_db`. If amc could generate a generic "dump all pools" function, any OpenACR program could expose its full in-memory state on demand -- headless mode for free, from the schema.

This is the "freeze a running program and look at its tables" idea turned into a general capability.

**Value:** Very high. Turns every tool into an agent-inspectable program. Programs ARE in-memory databases -- this makes that an operational interface, not just a design philosophy.
**Size:** Large. Requires amc generator work (pool iteration + ssim serialization for all ctypes).

**What generalizes cleanly (output/dump side):**
amc already generates `Print` for every ctype and knows every pool in FDb. A new `amcdb.gen` phase could generate `DumpAllPools()` -- one record in `gen.ssim`, one generator, every program gets it. Passes the factorization test: capability added by adding records, not code.

**What doesn't generalize (input/step side):**
`SendKey` is an acr_nav verb -- meaningful because it's a TUI with keybinds. A batch tool like `acr` has no keystrokes. A server has incoming messages. The "step" verb is program-specific even though the "dump" verb is universal. Don't oversell as "any tool becomes agent-testable" -- only the output side generalizes cleanly. More precise: **any tool becomes agent-inspectable.**

**Curated vs raw:**
Raw pool dumps are the truth -- curated views are opinions that drift. But raw dumps of a real program (thousands of records, runtime artifacts like file descriptors and computed caches) are a firehose. ACR answer: generate the raw dump (free from schema), let programs also define curated views as additional ctypes. Both, not either/or. The curated views are just more records -- they pass the factorization test.

Full pool scan has a practical obstacle: Tpool loses iteration capability (free-list allocator, no scan without a separate access path). The right approach: **dump everything reachable**, not everything allocated. Follow access paths from `_db`, not scan pools. Output as untyped tuples with regex filtering. This is essentially a built-in `acr` for runtime state -- a pretty printer for the entire program. Sidesteps the Tpool problem elegantly because you traverse what's reachable, not what's allocated. Consider forking this for `acr_vis`.

---

## Recommended Sequence

| Milestone | Idea | Rationale |
|-----------|------|-----------|
| ~~M17~~ | ~~1A: Inline reftype glossary~~ | ~~Done~~ |
| ~~M18~~ | ~~1C: Namespace grouping~~ | ~~Done~~ |
| M19 | 1E: Record count display | Small, high-value -- immediate sense of scale with minimal effort |
| M20 | 1D: Generated code preview | Bridges schema and code; shell-out pattern already proven |
| M21 | 1F: Field name/comment search | Turns acr_nav into a replacement for `acr` field queries |
| M22 | 2A: Command mode | Direct-jump efficiency for daily use |
| Later | 2B-2E, Tier 3 | As interest dictates |

**Sequencing rationale:**

- **M17 (reftype glossary)** — Done. Added `dmmeta.Reftype.comment`, surfaced in detail view.
- **M18 (namespace grouping)** — Done. LeftItem Tary, FNs.c_ctype Ptrary, collapse/expand, extern label, stable width.
- **M19 (record counts)** is the cheapest remaining win -- small code change, immediate payoff.
- **M20 (generated code)** builds on the shell-out pattern from preview mode. After structure (M19) makes navigation tractable, seeing the generated code for a type completes the schema-to-code loop.
- **M21 (field search)** adds cross-cutting query capability. Positioned after the structural improvements so the tool already feels capable when search extends it.
- **M22 (command mode)** is the gateway to power-user efficiency. By this point acr_nav has enough features that direct-jump becomes worthwhile.

**Future factoring:** IsHelpMode (4 sites) and IsXrefMode (5 sites) are identity checks that should become data on FViewmode. At 2 modes per axis, the branching is trivial. Refactor when a 3rd mode is added to either axis. Documented in code at their definition sites.

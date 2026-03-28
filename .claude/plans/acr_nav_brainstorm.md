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

**Current:** ~1000 lines C++, 20 navactions, 36 keybinds, 2 modes, 2 panels, 4 viewmodes (fields/xref/preview/help), 9 navstyles, 36 reftypestyles, ~1449 ctypes / ~5804 fields across 89 namespaces.

---

## Pain Points Observed

These come from hands-on experimentation with the full toolset and reviewing tutorials 1-5.

### P1. Flat list at scale
1,449 ctypes in a flat alphabetical list across 89 namespaces. Browsing is like reading a phone book. Filter helps, but you need to already know what you're looking for.

### P2. Vocabulary cliff
Lary, Thash, Ptrary, Upptr, Tpool, Tary, Llist -- the building blocks are opaque to newcomers. Seeing `|Lary ctype-->/` in an amc_vis diagram means nothing without looking up that "Lary" = "level array (pool allocator)." The reftype records have comments, but they're not visible where you need them -- inside acr_nav.

### P3. Tool-switching tax
Typical schema exploration requires five context switches: `acr` to find a type, `amc_vis` for access paths, `acr -t` for xref tree, `src_func` for hand-written code, then an editor. One question ("how does this type work?") scattered across five tools.

### P4. Field metadata hidden
Understanding a single field (its xref wiring, index config, substr decomposition) requires running `acr dmmeta.thash:...`, `acr dmmeta.substr:...`, etc. in separate terminals. The orthogonal factorization that makes the schema elegant also means a field's full story is spread across 5+ tables.

### P5. Silent failures across tools
`acr dmmeta.ctype:nonexistent.Foo` returns empty output, exit 0. Same for `amc_vis nonexistent.Foo`. No "did you mean?", no error. Can't distinguish typo from empty result. Biggest friction for learning.

### P6. The F-prefix gap
Generated C++ uses `acr_nav::FNaventry`. Querying `acr dmmeta.field:acr_nav.FNaventry.%` returns nothing -- the real name is `acr_nav.Naventry`. No hint. Trips up everyone who reads generated code first.

### P7. No sense of scale
Browsing ctypes in acr_nav, you can't tell which types have 3 records vs 3000. No record counts, no indication of which tables are the big ones.

### P8. Schema-to-code gap
"What C++ did amc generate for this type?" requires leaving acr_nav. The connection between schema definition and generated code -- the central lesson of OpenACR -- is invisible inside the tool that's supposed to teach it.

---

## Ideas by Tier

### Tier 1: High Impact, Natural Fit

These solve the most painful problems, align with OpenACR philosophy, and build on existing architecture.

#### 1A. Inline Reftype Glossary

When a field is selected, the status bar shows the reftype's comment from `dmmeta.reftype`. Example: selecting a Thash field shows "Hash table, unique keys." One-line definition, always visible, zero keystrokes.

**Solves:** P2 (vocabulary cliff).
**Teaches:** Controlled vocabularies have queryable properties -- you're seeing data, not hardcoded help text.
**Value:** High. Removes the single biggest barrier for newcomers.
**Size:** Very small. The data already exists in `dmmeta.reftype.comment`. Read one field from an already-loaded table, render one line in the status bar.

#### 1B. Field Detail Drilldown

Select a field in the right panel, press Enter, see all its metadata from every table: xref wiring, index config (thash/llist/ptrary), substr decomposition, fprefix, anonfld, comment. Shows how a single field's properties are factored across multiple independent tables.

**Solves:** P4 (field metadata hidden), P3 (tool-switching tax).
**Teaches:** Orthogonal factorization, made visible. Select `acr_nav.FDb.ind_ctype` and see five records from five tables, each capturing one independent concern about the same field.
**Value:** High. Currently requires `acr dmmeta.thash:...` in a separate terminal.
**Size:** Medium. Requires several new finputs and rendering (new panel or viewmode for field detail).

#### 1C. Namespace Grouping / Tree View

Toggle between flat ctype list and tree view grouped by namespace. Expandable/collapsible namespaces. With 89 namespaces averaging ~16 ctypes each, browsing becomes tractable.

**Solves:** P1 (flat list at scale).
**Teaches:** Orthogonal axes -- view mode vs filter vs selection are independent. Left-panel layout is an axis that doesn't affect right-panel content or filter semantics.
**Value:** High (biggest usability win for large schemas).
**Size:** Medium. Needs per-namespace collapse state and a different iteration pattern for the left panel.

#### 1D. Generated Code Preview

New viewmode: "generated". Shows the amc-generated C++ for the selected ctype -- struct definition, access functions, constructors, pool operations. Shell out to `amc <ctype>` and render inline, same pattern as preview mode loading ssimfiles.

**Solves:** P8 (schema-to-code gap), P3 (tool-switching tax).
**Teaches:** Schema and code are two views of the same thing. The generated code IS the schema made executable.
**Value:** High. Bridges the central conceptual gap for learners.
**Size:** Small-Medium. Same architecture as preview mode (shell out, capture output, render in right panel). One new viewmode record.

---

### Tier 2: Solid Value, Moderate Effort

These improve the experience meaningfully but solve less acute pain or require more work.

#### 2A. Record Count Display

For ctypes that have ssimfiles, show the record count next to the ctype name in the left panel. `dmmeta.Ctype (1449)`, `dmmeta.Field (5804)`, `acr_navdb.Keybind (36)`.

**Solves:** P7 (no sense of scale).
**Teaches:** Tables have different cardinalities. Some are configuration (36 keybinds), some are data (5804 fields). The count tells you what kind of table you're looking at.
**Value:** Medium. Immediate sense of scale, very low effort.
**Size:** Small. Count lines in ssimfiles at startup (or lazily). Only ctypes with a corresponding ssimfile get counts.

#### 2B. Field Name / Comment Search

Filter mode currently searches ctype names. Add a toggle to search field names and comments instead. Results show ctypes containing matching fields, with matches highlighted in the right panel.

**Solves:** Part of P3 (tool-switching tax). "Which ctypes have a field called 'comment'?" currently requires `acr dmmeta.field -where:...`.
**Teaches:** Same filter infrastructure, different search target. Filter axis is orthogonal to what's being searched.
**Value:** Medium. Powerful for exploration once you know the vocabulary.
**Size:** Medium. Needs a second filter index over fields.

#### 2C. Access Path Diagram (Inline amc_vis)

New viewmode: "diagram". Shell out to `amc_vis <ctype>`, render output in right panel.

**Solves:** P3 (tool-switching tax for access path visualization).
**Teaches:** Makes access paths visual inside the tool. Same data, different rendering.
**Value:** Medium. amc_vis exists at CLI, but inline removes context-switching.
**Size:** Small if shelling out, Large if reimplementing. Challenge: amc_vis output can be very wide (150+ columns). Would need horizontal scrolling or truncation.

#### 2D. Cross-Namespace Dependency View

For the selected ctype's namespace, show upstream dependencies (what it imports via field references) and downstream dependents (what references it). Based on cross-namespace field arg references.

**Solves:** "What namespaces does acr_nav depend on? What depends on dmmeta?" Currently requires multiple `acr -ndown` queries.
**Teaches:** Dependencies are implicit in field references. This makes that structure explicit.
**Value:** Medium. Useful for understanding the schema's macro structure.
**Size:** Medium. Needs per-namespace aggregation of field references.

---

### Tier 3: Nice to Have, Larger Scope

These are valuable but either serve narrower use cases or require significant effort.

#### 3A. Command Mode

Press `:` for vim-style command input. `:goto <ctype>`, `:ns <regex>`, `:q`. Command dispatch via the same gstatic Hook pattern as navactions.

**Solves:** Direct jump by name is faster than filtering for known targets.
**Teaches:** Uniform contracts -- same dispatch mechanism scales to a new domain.
**Value:** Medium. Most useful for power users who already know type names.
**Size:** Medium.
**Design note:** Both filter mode and command mode need free-text capture. The current code handles filter's printable-char capture as a special case. When command mode is designed, factoring `capture_text` into a mode property should be considered.

#### 3B. Bookmark / Recent History

Press `m` to bookmark current ctype. Press `'` to see bookmark list. Also show recently-visited types (navstack already tracks history; just needs persistence across sessions or a dedicated view).

**Solves:** Repeatedly navigating to the same handful of types during a dev session.
**Value:** Medium. Most useful during extended exploration sessions.
**Size:** Small-Medium.

#### 3C. Integrity Check View

New viewmode: "check". Runs `acr -check` and shows only errors related to the selected ctype or namespace. Green indicator when clean.

**Solves:** `acr -check` reports errors as a flat list; hard to find "which errors affect MY ctype?"
**Value:** Low-Medium. Schema is currently clean (zero errors). Most useful when actively editing schema.
**Size:** Medium. Parsing acr -check output, filtering by ctype.

#### 3D. Tutorial / Guided Mode

`:tutorial 1` starts a guided walkthrough inside acr_nav. The tool highlights specific types, shows explanatory text in the help panel, walks through key concepts interactively.

**Solves:** Tutorials teach concepts but require jumping between docs and terminal. acr_nav could BE the learning tool.
**Value:** High for onboarding, but narrow audience.
**Size:** Large. Significant content authoring beyond code.

---

### Standalone: Generalized Headless (_db Pool Dump)

This idea lives outside the tier system because it's an amc-level capability, not an acr_nav feature.

Headless mode in acr_nav serializes curated projections (Screen, PanelState, VisibleField) as ssimfile records. But every amc-generated program already has typed pools in `_db`. If amc could generate a generic "dump all pools" function, any OpenACR program could expose its full in-memory state on demand -- headless mode for free, from the schema.

This is the "freeze a running program and look at its tables" idea turned into a general capability.

**Teaches:** The deepest lesson -- programs ARE in-memory databases, and that's not just a design philosophy, it's an operational interface.
**Value:** Very high. Turns every tool into an agent-inspectable program.
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

| Milestone | Idea | Principle Demonstrated |
|-----------|------|----------------------|
| M16 | 1A: Inline reftype glossary | Controlled vocab properties surfaced in context |
| M17 | 1B: Field detail drilldown | Orthogonal factorization, visible |
| M18 | 1C: Namespace grouping | Orthogonal axes (left-panel layout independent of right panel) |
| M19 | 1D: Generated code preview | Schema = code, two views of same thing |
| Later | Tier 2-3 ideas | As interest dictates |

**Sequencing rationale:**

- **M16 (reftype glossary)** is nearly zero effort -- it surfaces existing data in the status bar. Ships fast, removes the vocabulary cliff immediately. Good warmup.
- **M17 (field detail)** loads the most new finputs and is best attempted now that viewmode rendering is clean. Teaches the deepest schema concept.
- **M18 (namespace grouping)** is the biggest usability win but requires more architectural work (left-panel rendering becomes mode-dependent). Better after field detail proves the viewmode pattern further.
- **M19 (generated code)** builds on the shell-out pattern from preview mode. Natural capstone: after seeing fields (M17) and structure (M18), see the generated code that ties them together.

**Future factoring:** IsHelpMode (4 sites) and IsXrefMode (5 sites) are identity checks that should become data on FViewmode. At 2 modes per axis, the branching is trivial. Refactor when a 3rd mode is added to either axis. Documented in code at their definition sites.

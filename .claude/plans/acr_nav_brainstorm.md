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

**Current:** ~1000 lines C++, 20 navactions, 37 keybinds, 2 modes, 2 panels, 4 viewmodes (fields/xref/preview/help), 8 navstyles, 35 reftypestyles, ~1390 ctypes / ~5570 fields.

---

## Ideas

### Field Detail Drilldown

Select a field, see its full metadata: xref wiring, index config (thash/llist/ptrary), substr decomposition, comment. Shows how a single field's properties are factored across multiple independent tables.

**Teaches:** Orthogonal factorization, made visible. Select `acr_nav.FDb.ind_ctype` and see five records from five tables, each capturing one independent concern about the same field.
**Value:** High. Currently requires `acr dmmeta.thash:...` in a separate terminal.
**Size:** Medium. Requires several new finputs and rendering.

---

### Namespace Tree View

Toggle between flat ctype list and tree view grouped by namespace. Expandable/collapsible namespaces. With 1380 ctypes, the flat list is overwhelming.

**Teaches:** Orthogonal axes (view mode vs filter vs selection are independent).
**Value:** High (biggest usability win for large schemas).
**Size:** Medium. Needs per-namespace collapse state and a different iteration pattern for the left panel.

---

### Access Path Diagram (Inline amc_vis)

Display a text-based access path diagram for the selected ctype. Shell out to `amc_vis` and render the output inline -- don't reimplement the layout algorithm.

**Teaches:** Makes access paths visual inside the tool.
**Value:** Medium (amc_vis exists at CLI, but inline removes context-switching).
**Size:** Small if shelling out, Large if reimplementing.

---

### Command Mode

Press `:` for vim-style command input. `:goto <ctype>`, `:ns <regex>`, `:q`. Command dispatch via the same gstatic Hook pattern as navactions.

**Teaches:** Uniform contracts -- same dispatch mechanism scales to a new domain.
**Value:** Medium. Direct jump by name is faster than filtering.
**Size:** Medium.
**Design note:** Both filter mode and command mode need free-text capture. The current code handles filter's printable-char capture as a special case. When command mode is designed, factoring `capture_text` into a mode property should be considered.


---

---

### Generalized Headless: _db Pool Dump for Any Program

Headless mode in acr_nav serializes curated projections (Screen, PanelState, VisibleField) as ssimfile records. But every amc-generated program already has typed pools in `_db`. If amc could generate a generic "dump all pools" function, any OpenACR program could expose its full in-memory state on demand — headless mode for free, from the schema.

This is the "freeze a running program and look at its tables" idea turned into a general capability.

**Teaches:** The deepest lesson — programs ARE in-memory databases, and that's not just a design philosophy, it's an operational interface.
**Value:** Very high. Turns every tool into an agent-inspectable program.
**Size:** Large. Requires amc generator work (pool iteration + ssim serialization for all ctypes).

**What generalizes cleanly (output/dump side):**
amc already generates `Print` for every ctype and knows every pool in FDb. A new `amcdb.gen` phase could generate `DumpAllPools()` — one record in `gen.ssim`, one generator, every program gets it. Passes the factorization test: capability added by adding records, not code.

**What doesn't generalize (input/step side):**
`SendKey` is an acr_nav verb — meaningful because it's a TUI with keybinds. A batch tool like `acr` has no keystrokes. A server has incoming messages. The "step" verb is program-specific even though the "dump" verb is universal. Don't oversell as "any tool becomes agent-testable" — only the output side generalizes cleanly. More precise: **any tool becomes agent-inspectable.**

**Curated vs raw:**
Raw pool dumps are the truth — curated views are opinions that drift. But raw dumps of a real program (thousands of records, runtime artifacts like file descriptors and computed caches) are a firehose. ACR answer: generate the raw dump (free from schema), let programs also define curated views as additional ctypes. Both, not either/or. The curated views are just more records — they pass the factorization test.

---

## Recommended Sequence

| Milestone | Idea | Principle Demonstrated |
|-----------|------|----------------------|
| M16 | Field detail drilldown | Orthogonal factorization, visible |
| Later | Tree view, amc_vis, command mode | As interest dictates |

**Sequencing rationale:** M16 loads the most new finputs and is best attempted now that viewmode rendering is clean.

**Future factoring:** IsHelpMode (4 sites) and IsXrefMode (5 sites) are identity checks that should become data on FViewmode. At 2 modes per axis, the branching is trivial. Refactor when a 3rd mode is added to either axis. Documented in code at their definition sites.

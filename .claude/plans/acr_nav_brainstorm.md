# acr_nav: What's Next?

## Story So Far

10 milestones (M0-M8), each teaching one OpenACR concept:

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
| M9 | Semantic field coloring + resolve hardcoded styles | Controlled vocab properties, data as stylesheet |

**Current:** ~630 lines C++, 16 navactions, 31 keybinds, 2 modes, 2 panels, 8 navstyles, 35 reftypestyles, ~1380 ctypes / ~5500 fields.

**Principles not yet taught by any milestone:** self-describing UI, orthogonal factorization (visible to user), access paths as new capability.

---

## Ideas

### Reverse Xref ("Who Points Here?")

Flip the right panel from "fields OF this ctype" to "fields POINTING TO this ctype." The same `dmmeta.field` records, accessed via a second Ptrary index. One xref record in the schema opens bidirectional graph navigation.

**Teaches:** Access paths, not data structures. Same data, new index, new capability.
**Value:** High. "Who depends on this type?" is essential for impact analysis.
**Size:** Small.

---

### Semantic Field Coloring

Color-code fields by reftype category: navigable references (Upptr/Pkey) vs container indexes (Thash/Llist/Ptrary) vs plain values (Val). The color mapping lives in records, not code.

**Teaches:** Controlled vocabularies carry properties. Attaching a new property to a vocabulary member changes behavior without touching code.
**Value:** High. Visual distinction between navigable links and plain data makes the tool instantly more scannable.
**Size:** Small. Design should consider how styles are represented (the 3 hardcoded ANSI style roles in Render are a small missing noun worth resolving alongside this).

---

### Self-Documenting Help

Press `?` to see all keybindings for the current mode, generated from the `keybind` and `navaction` records. The help is never written by hand -- it IS the data that drives dispatch. The hardcoded status bar hint string should also become generated from records.

**Teaches:** Self-describing. The system's documentation IS its specification. Adding a keybinding automatically updates the help.
**Value:** High. Discoverability is the biggest barrier for TUI tools.
**Size:** Small. Status bar generation is the higher-frequency win; full-screen overlay is an extension.

---

### Ssimfile Content Preview

When viewing a ctype with a backing ssimfile, preview actual records from that file. Navigate to `dmmeta.Reftype`, see the 35 reftype records with their boolean property columns. The meta-schema stops being abstract.

**Teaches:** Self-describing. The ssimfile path is deterministic from the ssimfile pkey -- another teaching moment.
**Value:** High. Eliminates the "exit, cat, re-enter" cycle. Makes acr_nav a complete exploration tool.
**Size:** Medium. Requires loading `dmmeta.ssimfile` and on-demand file reading.
**Design note:** This changes what the right panel displays. How to handle multiple right-panel content types (fields, preview, reverse xrefs) is a design question that should be resolved when this or reverse xref is first implemented -- not prescribed now.

---

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

### Breadcrumb Trail

Display navstack as breadcrumb: `dmmeta.Ctype > dmmeta.Field > algo.cstring`. New view of existing data.

**Teaches:** Modest (same data, new presentation).
**Value:** Medium. Spatial orientation during deep reference chains.
**Size:** Small.
**Prerequisite:** Naventry should store a ctype name explicitly. Currently the ctype is only implied by `sel_row` (an index into the filtered list). If the list changes, the index is meaningless. This is a correctness improvement independent of breadcrumbs.

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

## Recommended Sequence

| Milestone | Idea | Principle Demonstrated |
|-----------|------|----------------------|
| M9 | ~~Semantic field coloring (+ resolve hardcoded styles)~~ DONE | Controlled vocab properties, data as stylesheet |
| M10 | Reverse xref | Access paths: same data, second index |
| M11 | Self-documenting help | Self-describing: docs = data = behavior |
| M12 | Naventry ctype name + breadcrumb | Correctness fix + usability |
| M13 | Ssimfile content preview | Self-describing: see the actual records |
| M14 | Field detail drilldown | Orthogonal factorization, visible |
| Later | Tree view, amc_vis, command mode | As interest dictates |

**Sequencing rationale:** Front-loads small/high-impact ideas. M9-M11 are each single milestones that teach distinct principles. M10 and M13 both add right-panel content types -- whichever lands first will establish the pattern for the other. M14 loads the most new finputs and is best attempted after the tool's rendering model has matured through the earlier milestones.

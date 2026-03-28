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

**Current:** ~1000 lines C++, 20 navactions, 37 keybinds, 2 modes, 2 panels, 4 viewmodes (fields/xref/preview/help), 8 navstyles, 35 reftypestyles, ~1390 ctypes / ~5570 fields.

---

## Ideas

### Hint on Startup

Show help on startup in the right panel (not as a full-screen overlay). Help should always be available as a right panel viewmode via `?`. Simplify the bottom status bar to breadcrumb + minimal hints (1 row).

**Teaches:** Controlled vocabulary refinement. The existing viewmode system should absorb help naturally.
**Value:** High. First-launch discoverability without blocking interaction.
**Size:** Small-Medium.

**UX requirements from prototyping:**
- Help auto-dismisses on any keypress (startup help is informational, not blocking)
- Help is NOT in the Tab cycle — only reachable via `?` toggle
- Help display should split keys into two columns: standard keys (arrows, Enter, PgDown) and shortcuts (j, k, l) — makes it scannable for both audiences
- Help sort order should be logical (movement, navigation, views, search, meta), not alphabetical by action name
- Status bar hints should remain data-driven (from keybind/navaction records), not hardcoded strings


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

---

## Recommended Sequence

| Milestone | Idea | Principle Demonstrated |
|-----------|------|----------------------|
| M15 | Hint on startup | Controlled vocabulary refinement, viewmode factoring |
| M16 | Field detail drilldown | Orthogonal factorization, visible |
| Later | Tree view, amc_vis, command mode | As interest dictates |

**Sequencing rationale:** M15 is a usability win that also forces the viewmode dispatch to be factored properly. M16 loads the most new finputs and is best attempted after viewmode rendering is clean.

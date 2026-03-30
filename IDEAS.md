# acr_nav: What's Next?

## Story So Far

| # | Description |
|---|-------------|
| 0 | **Vision and design.** Project ideas document for acr_nav: interactive terminal browser for ctypes, fields, and xrefs. Milestone plan, constraints, no premature implementation decisions. |
| 1 | **Schema tables.** Create acr_navdb ssimdb with four configuration tables: navaction (11 actions), keybind (11 key mappings), navmode (browse/filter), panel (ctype_list/field_list). 26 ssimfile records, zero C++. |
| 2 | **Exe scaffold and cross-references.** Create acr_nav executable with finputs from dmmeta (ctype, field, ns, reftype) and acr_navdb. Wire cross-references matching amc_vis patterns (c_field, p_ns, p_ctype, p_arg, p_reftype, p_navaction). UI state: navigation stack, filtered ctype list, per-panel scroll/selection, terminal dimensions, mode and panel pointers. |
| 3 | **Batch-mode Main().** Replace Hello World stub with Main() that filters ctypes by namespace regex, populates zd_sel_ctype, prints each selected ctype with its fields showing arg type and reftype via Upptr traversal. |
| 4 | **Single-panel scrollable TUI.** Raw termios input, ANSI rendering, data-driven keybind dispatch via ind_keybind_Find. Batch output preserved as fallback when stdout is not a terminal. j/k vim keybinds for move_down/move_up. |
| 5 | **Two-panel TUI with xref navigation.** Side-by-side layout (40/60 split): left panel ctypes, right panel fields. Enter follows field references via p_arg, Backspace pops navigation stack. Directional panel switching with boundary guards. Right panel resets on ctype change via sel_ct pointer comparison. |
| 6 | **Hook dispatch and filter mode.** Replace DispatchAction string if-chain with gstatic + Hook function-pointer dispatch via navaction handlers, matching amc.FGen.step pattern. Mode-qualify keybind table with composite keys (browse.Up, filter.Escape) so mode-specific behavior is data, not code. Filter mode: type to narrow the ctype list, Escape exits and restores full list. |
| 7 | **Component tests and filter state.** Three batch-mode component tests (BatchSelf, BatchConfig, EmptyResult) via atfdb.comptest. Extract Naventry filter state (cur_filter, filter_regx) from global FDb into per-stack-entry state. Keybind substr validation ensures composite keys reference valid navmode/navaction. |
| 8 | **Keybinding redesign.** Dual-audience keybinds for arrow-key users and vim users. ReadKeyName() recognizes Tab, Space, Ctrl-A through Ctrl-Z, Home, End. 31 keybinds (was 21), 16 navactions (was 12). New actions: go_top, go_bottom, filter_clear (Ctrl-U), filter_append_space. |
| 9 | **Color-code fields by reftype.** Two new ssimfile tables: acr_navdb.navstyle (8 named terminal style records) and acr_navdb.reftypestyle (35 records mapping every reftype to a navstyle via compound key). Navigable references (Upptr/Pkey/Base) cyan, container indexes (Thash/Llist/Ptrary) yellow, plain values default. All hardcoded ANSI escapes replaced with data lookups through EmitStyle helper. |
| 10 | **Structural role color categories.** Refine color scheme from ad-hoc 3-category to 4 categories derived from reftype properties: pools (green), indexes (yellow), upward refs (cyan), values (default). Unfocused selection changed from bold to dim-reverse for visibility. |
| 11 | **TermColor controlled vocabulary.** Replace navstyle's 3 boolean color encoding (fg_red, fg_green, fg_blue) with single fg_color field using new algo.TermColor type: controlled vocabulary of 8 named ANSI bright colors (default/red/green/yellow/blue/magenta/cyan/white). |
| 12 | **Reverse xref view and filter_accept.** Toggle right panel between forward fields and reverse xrefs ("who points here?") via r/Tab. Reverse index built automatically by amc via c_field_arg Ptrary on FCtype. In xref mode, second column shows source ctype; follow_ref navigates to it. Enter in filter mode accepts text and returns to browse mode. Toggle state saved/restored in navstack. |
| 13 | **Help overlay, status bar, preview, viewmode system, filter UX.** Press ? for keybind help generated from keybind/navaction records. Status bar hints built from hint/hint_order fields. Ssimfile content preview (p key) with table-formatted output. Viewmode controlled vocabulary (fields/xref/preview) with data-driven Tab cycling via next field. Column headers for all views. Content-driven left panel width. Filter persistence with status bar indicator, Esc to clear accepted filter. |
| 14 | **Breadcrumb navigation bar.** Display navstack as a bar above status bar showing visited ctype path: `MYSQL > lib_mysql.FDb > lib_mysql.Res`. Appears only at depth > 0, reclaiming content row at depth 0. Naventry.ctype stores ctype name on push. VisibleRows() helper shared by AdjustScroll, page_up, page_down, and Render. |
| 15 | **Help as right-panel viewmode.** Replace full-screen overlay with help rendered in right panel alongside ctype list. Helpgroup controlled vocabulary for logical grouping. has_fields boolean on viewmode for data-driven dispatch. Two-column key layout (standard + shortcut). Startup help auto-dismisses on first keypress; manual ? help is scrollable and restores previous viewmode. |
| 16 | **Field detail drilldown and viewmode refactoring.** Press d on any field to see all dmmeta records about it (thash config, xref wiring, llist setup) gathered from orthogonal tables. Data-driven via acr_navdb.detailsrc: adding a new metadata source costs one record, zero code. Line/header storage moved from FDb to FViewmode, viewmode_stack replaces p_prev_viewmode for nested overlays. |
| 17 | **Inline reftype glossary.** Add comment field to dmmeta.Reftype (35 records populated from amcdb.tclass.comment with 6 overrides for clarity), surfaced in detail view (d keybind). On-demand context for opaque reftype names like Thash, Ptrary, Tpool. |
| 18 | **Namespace grouping and tree view.** Replace flat ctype list with namespace-grouped tree: 83 namespaces start collapsed, Enter toggles expand/collapse. LeftItem Tary replaces zd_sel_ctype linked list. FNs.c_ctype Ptrary indexes ctypes by namespace. Extern types grouped under "extern" label. Follow-ref auto-expands target namespace. |
| 19 | **Record count display.** Show ssimfile record counts next to ctype names (e.g., "Keybind (38)") for immediate sense of scale. Counts computed at startup via FileLine_curs. DecimalDigits helper fixes latent 4-digit cap in namespace header width formula. |
| 20 | **Generated code preview viewmode.** New "codegen" viewmode shows amc-generated C++ struct definition for selected ctype inline in right panel. Press 'c' to toggle or Tab to cycle. SysEval captures `amc '<ctype>'` output with per-ctype caching. |
| 21 | **Field name and comment search.** New filtertarget controlled vocabulary table with Tab cycling in filter mode. Press / then Tab to switch from ctype name search to field name+comment search. Status bar shows `/f:` prefix in field mode. Matching fields highlighted bold in right panel. Filtertarget saved/restored in navstack. CtypeMatchesFilter + FieldMatchesFilter helpers with cached regex on FDb. |

---

## Known Issues

### I1. Filter clear does not restore namespace collapse state
When a filter is applied, namespaces containing matching ctypes are auto-expanded. Clearing the filter (Escape) does not restore the pre-filter collapse state — all auto-expanded namespaces remain expanded. Pre-existing; found via exploratory testing 2026-03-30.

---

## Pain Points

### P2. Opaque reftype vocabulary (partially addressed by M17)
Lary, Thash, Ptrary, Upptr, Tpool, Tary, Llist -- the reftype names are terse and carry no visible meaning. M17 added `comment` to `dmmeta.Reftype` and shows it in the detail view (`d` keybind). This helps on-demand but doesn't solve the "what does this mean at a glance?" problem during normal field browsing.

### P3. Tool-switching tax
Typical schema exploration requires five context switches: `acr` to find a type, `amc_vis` for access paths, `acr -t` for xref tree, `src_func` for hand-written code, then an editor. One question ("how does this type work?") scattered across five tools.

### P5. Silent failures across tools
`acr dmmeta.ctype:nonexistent.Foo` returns empty output, exit 0. Same for `amc_vis nonexistent.Foo`. No "did you mean?", no error. Can't distinguish typo from empty result.

### P6. The F-prefix gap
Generated C++ uses `acr_nav::FNaventry`. Querying `acr dmmeta.field:acr_nav.FNaventry.%` returns nothing -- the real name is `acr_nav.Naventry`. No hint. Trips up everyone who reads generated code first.

---

## Simplification Opportunities

#### S5. Eliminate IsXrefMode via field_source on Viewmode
5 call sites dispatch on `IsXrefMode()` identity check to switch between `c_field` (forward) and `c_field_arg` (reverse xrefs). Add `field_source` property to viewmode record. Makes forward/xref distinction data-driven. Removes ~20 lines + 3 identity-check functions. Adding new field-based viewmodes (e.g., "inherited fields") becomes a record, not code.

#### S6. Extract SaveNavstate/RestoreNavstate helpers
13 fields manually copied between FDb and Naventry in follow_ref/go_back (was 9; grew with right_sel_row, right_scroll_offset, focus_panel). Helpers centralize the copy so adding a 14th field requires editing 1 place per direction. Insurance against future bugs, not urgent.

---

## Ideas by Tier

### Tier 1: High Impact, Natural Fit

These solve the most painful problems, align with OpenACR philosophy, and build on existing architecture.

#### ~~1D. Generated Code Preview~~ (done — M20)

#### ~~1E. Record Count Display~~ (done — M19)

#### ~~1F. Field Name / Comment Search~~ (done — M21)

`acr_navdb.filtertarget` controlled vocabulary table with 2 records: `ctype` (default) and `field` (name + comment). Tab in filter mode cycles targets. Status bar: `/text` vs `/f:text`. Matching fields bold-highlighted in right panel. `CtypeMatchesFilter` + `FieldMatchesFilter` helpers; cached `filter_regx` on FDb. Filtertarget saved/restored in navstack. 4 new tests (FieldFilter, FieldFilterCancel, FieldFilterNavstack, FieldFilterScreen).

**Future filtertarget extensions** (one record + one match branch each):

| Target | Label | Searches against | Use case |
|--------|-------|-----------------|----------|
| `ctype` | (none) | ctype name | Default. "Find types named FDb" |
| `field` | `f:` | field name + comment | V1. "Which types have a `comment` field?" |
| `arg` | `a:` | field argument type | "Which types have `algo.cstring` fields?" |
| `reftype` | `r:` | field reftype | "Which types use Thash indexes?" |
| `all` | `*:` | name + field + comment + arg | Kitchen sink — "find anything mentioning X" |

At 3+ targets, Tab cycling becomes tedious. Two possible evolutions:
- **Prefix typing:** user types `/f:comment` directly; filter parser detects known prefix and sets filtertarget. Tab still works as fallback.
- **Picker popup:** Tab opens a brief target selector instead of blind cycling. Heavier UI but scales to any count.

The `filtertarget` table with `next` cycling chain supports both — the table is the source of truth; the input mechanism is independent.

---

### Tier 2: Solid Value, Moderate Effort

These improve the experience meaningfully but solve less acute pain or require more work.

#### 2A. Syntax highlighting for ssimfiles and codegen

#### 2B. Interactive Access Path Diagram

Interactive graph navigation inside acr_nav. Selected ctype rendered as center node, direct neighbors (field arg ctypes) as surrounding boxes connected by labeled edges (reftype + field name). Tab/Shift-Tab moves between nodes, Enter navigates into a node (push navstack, re-center graph on new ctype), Backspace returns.

**Solves:** P3 (tool-switching tax) — you can **walk** the access path graph, not just look at a snapshot.
**Value:** High. This is `amc_vis` as an interactive experience, not a batch tool. Combined with headless mode, an agent could navigate the graph programmatically.
**Size:** Medium. What already exists: raw terminal, colors, navstack, viewmode system. What's new: ASCII box drawing (unicode `┌─┐│└─┘` + arrows), simple layout algorithm (center + ring of neighbors, not full amc_vis topological sort), node-to-node cursor movement (Tab order over nodes, no true 2D cursor needed).
**Design note:** Keep layout simple — local neighborhood only (selected ctype + its direct references, 5-15 nodes). Full graph is amc_vis's job. The value is interactivity, not completeness. Borrow ideas and code from `amc_vis` (box drawing, layout) as a starting point.

#### 2C. Cross-Namespace Dependency View

For the selected ctype's namespace, show upstream dependencies (what it imports via field references) and downstream dependents (what references it). Based on cross-namespace field arg references.

**Solves:** "What namespaces does acr_nav depend on? What depends on dmmeta?" Currently requires multiple `acr -ndown` queries.
**Value:** Medium. Useful for understanding the schema's macro structure.
**Size:** Medium. Needs per-namespace aggregation of field references.

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

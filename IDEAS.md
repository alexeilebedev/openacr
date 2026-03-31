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
| 22 | **Instant terminal resize.** SIGWINCH handler sets flag, main loop repaints. AdjustScroll clamps selection on shrink. |
| 23 | **Expanded filter targets.** Add arg, reftype, and all filter targets with data-driven match logic. |
| 24 | **Syntax highlighting for line-mode views.** Codegen (C++ keywords, types, strings), detail (ssim keys/values), preview (ssim columns), help (keybind/section styles). Uses existing AddSpan + navstyle infrastructure. |
| 25 | **Cross-namespace dependency overlay.** nsdep viewmode (n key) shows upstream/downstream namespace dependencies for selected ctype's namespace. Overlay on viewmode_stack, data-driven from cross-namespace field references. |
| 26 | **Interactive access path graph viewmode.** New `graph` viewmode (v key) renders amc_vis-style ASCII access path diagram in right panel. Three-column layout: left (Pkey/Upptr parents), center (selected ctype), right (Val creation + Ptr/Ptrary children). Edge labels show "reftype fieldname". Enter walks into neighbor ctype staying in graph mode; Backspace returns via navstack. Edge collection matches amc_vis exactly: GraphSkipQ mirrors DepRefQ, Val fields use first-wins dedup, dep fields use reftype `up` flag for column placement. NavigateToTarget helper extracted from follow_ref eliminates navstack push duplication. Tab cycle: fields→xref→preview→codegen→graph→fields. |

---

## Known Issues

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

## Ideas by Tier

### Tier 2: Solid Value, Moderate Effort

These improve the experience meaningfully but solve less acute pain or require more work.

#### 2A. Syntax highlighting for ssimfiles and codegen -- DONE (M24)

#### 2B. Interactive Access Path Diagram -- DONE (M26)

Implemented as graph viewmode with amc_vis-style three-column ASCII layout, interactive Enter/Backspace navigation, and exact DepRefQ edge matching. See M26 in Story So Far.

#### 2B+. Graph Viewmode Extensions

Ideas for extending the graph viewmode, roughly ordered by feasibility:

**Quick wins:**

1. **Color edges by reftype.** Call `AddSpan` in `LoadGraph` using existing `reftypestyle` -> `navstyle` color mappings. Pool edges green, index edges yellow, upward refs cyan. Infrastructure fully in place -- just never called. Low complexity.

2. **Reftype comment on selected edge.** Show `dmmeta.Reftype.comment` in status bar when cursor highlights an edge line. Extend `GraphNodeAtLine` to also return the FField pointer. Low complexity. Partially addresses P2.

3. **Record count on neighbor nodes.** Append `(N)` after neighbor ctype names, reusing counts already computed at startup. Low complexity.

**Solid additions:**

4. **Depth-2 expansion toggle.** Press `+` to expand selected neighbor inline showing its depth-1 edges indented. `-` collapses. One node at a time. Medium complexity.

5. **Reverse xref edges.** Include `c_field_arg` back-references ("who points at me") as a fourth section. Unifies graph and xref views. Medium complexity.

6. **Node cycling.** Tab/Shift-Tab jumps cursor to next/previous neighbor opener line, skipping close/border lines. Medium complexity.

7. **Horizontal scroll.** Left/Right keys pan wide graphs via `scroll_x` offset. Medium complexity.

**Ambitious:**

8. **Full namespace topology.** Port amc_vis's topological sort layout (`Main_ColLayout` + `Main_RowLayout` with binary heaps) to show all ctypes in a namespace. The complete `amc_vis` inside acr_nav. High complexity, depends on idea 7.

**Not worth pursuing:** Unicode box drawing (compatibility risk, breaks amc_vis parity), 2D spatial cursor (fights line-oriented model; node cycling gives 90% of value at 10% cost).

#### 2C. Cross-Namespace Dependency View -- DONE (M25)

Implemented as nsdep overlay viewmode showing upstream/downstream namespace dependencies.

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

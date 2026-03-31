# acr_nav: What's Next?

## Story So Far


| #   | Description                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                          |
| --- | ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 0   | **Vision and design.** Project ideas document for acr_nav: interactive terminal browser for ctypes, fields, and xrefs. Milestone plan, constraints, no premature implementation decisions.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           |
| 1   | **Schema tables.** Create acr_navdb ssimdb with four configuration tables: navaction (11 actions), keybind (11 key mappings), navmode (browse/filter), panel (ctype_list/field_list). 26 ssimfile records, zero C++.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                 |
| 2   | **Exe scaffold and cross-references.** Create acr_nav executable with finputs from dmmeta (ctype, field, ns, reftype) and acr_navdb. Wire cross-references matching amc_vis patterns (c_field, p_ns, p_ctype, p_arg, p_reftype, p_navaction). UI state: navigation stack, filtered ctype list, per-panel scroll/selection, terminal dimensions, mode and panel pointers.                                                                                                                                                                                                                                                                                                                             |
| 3   | **Batch-mode Main().** Replace Hello World stub with Main() that filters ctypes by namespace regex, populates zd_sel_ctype, prints each selected ctype with its fields showing arg type and reftype via Upptr traversal.                                                                                                                                                                                                                                                                                                                                                                                                                                                                             |
| 4   | **Single-panel scrollable TUI.** Raw termios input, ANSI rendering, data-driven keybind dispatch via ind_keybind_Find. Batch output preserved as fallback when stdout is not a terminal. j/k vim keybinds for move_down/move_up.                                                                                                                                                                                                                                                                                                                                                                                                                                                                     |
| 5   | **Two-panel TUI with xref navigation.** Side-by-side layout (40/60 split): left panel ctypes, right panel fields. Enter follows field references via p_arg, Backspace pops navigation stack. Directional panel switching with boundary guards. Right panel resets on ctype change via sel_ct pointer comparison.                                                                                                                                                                                                                                                                                                                                                                                     |
| 6   | **Hook dispatch and filter mode.** Replace DispatchAction string if-chain with gstatic + Hook function-pointer dispatch via navaction handlers, matching amc.FGen.step pattern. Mode-qualify keybind table with composite keys (browse.Up, filter.Escape) so mode-specific behavior is data, not code. Filter mode: type to narrow the ctype list, Escape exits and restores full list.                                                                                                                                                                                                                                                                                                              |
| 7   | **Component tests and filter state.** Three batch-mode component tests (BatchSelf, BatchConfig, EmptyResult) via atfdb.comptest. Extract Naventry filter state (cur_filter, filter_regx) from global FDb into per-stack-entry state. Keybind substr validation ensures composite keys reference valid navmode/navaction.                                                                                                                                                                                                                                                                                                                                                                             |
| 8   | **Keybinding redesign.** Dual-audience keybinds for arrow-key users and vim users. ReadKeyName() recognizes Tab, Space, Ctrl-A through Ctrl-Z, Home, End. 31 keybinds (was 21), 16 navactions (was 12). New actions: go_top, go_bottom, filter_clear (Ctrl-U), filter_append_space.                                                                                                                                                                                                                                                                                                                                                                                                                  |
| 9   | **Color-code fields by reftype.** Two new ssimfile tables: acr_navdb.navstyle (8 named terminal style records) and acr_navdb.reftypestyle (35 records mapping every reftype to a navstyle via compound key). Navigable references (Upptr/Pkey/Base) cyan, container indexes (Thash/Llist/Ptrary) yellow, plain values default. All hardcoded ANSI escapes replaced with data lookups through EmitStyle helper.                                                                                                                                                                                                                                                                                       |
| 10  | **Structural role color categories.** Refine color scheme from ad-hoc 3-category to 4 categories derived from reftype properties: pools (green), indexes (yellow), upward refs (cyan), values (default). Unfocused selection changed from bold to dim-reverse for visibility.                                                                                                                                                                                                                                                                                                                                                                                                                        |
| 11  | **TermColor controlled vocabulary.** Replace navstyle's 3 boolean color encoding (fg_red, fg_green, fg_blue) with single fg_color field using new algo.TermColor type: controlled vocabulary of 8 named ANSI bright colors (default/red/green/yellow/blue/magenta/cyan/white).                                                                                                                                                                                                                                                                                                                                                                                                                       |
| 12  | **Reverse xref view and filter_accept.** Toggle right panel between forward fields and reverse xrefs ("who points here?") via r/Tab. Reverse index built automatically by amc via c_field_arg Ptrary on FCtype. In xref mode, second column shows source ctype; follow_ref navigates to it. Enter in filter mode accepts text and returns to browse mode. Toggle state saved/restored in navstack.                                                                                                                                                                                                                                                                                                   |
| 13  | **Help overlay, status bar, preview, viewmode system, filter UX.** Press ? for keybind help generated from keybind/navaction records. Status bar hints built from hint/hint_order fields. Ssimfile content preview (p key) with table-formatted output. Viewmode controlled vocabulary (fields/xref/preview) with data-driven Tab cycling via next field. Column headers for all views. Content-driven left panel width. Filter persistence with status bar indicator, Esc to clear accepted filter.                                                                                                                                                                                                 |
| 14  | **Breadcrumb navigation bar.** Display navstack as a bar above status bar showing visited ctype path: `MYSQL > lib_mysql.FDb > lib_mysql.Res`. Appears only at depth > 0, reclaiming content row at depth 0. Naventry.ctype stores ctype name on push. VisibleRows() helper shared by AdjustScroll, page_up, page_down, and Render.                                                                                                                                                                                                                                                                                                                                                                  |
| 15  | **Help as right-panel viewmode.** Replace full-screen overlay with help rendered in right panel alongside ctype list. Helpgroup controlled vocabulary for logical grouping. has_fields boolean on viewmode for data-driven dispatch. Two-column key layout (standard + shortcut). Startup help auto-dismisses on first keypress; manual ? help is scrollable and restores previous viewmode.                                                                                                                                                                                                                                                                                                         |
| 16  | **Field detail drilldown and viewmode refactoring.** Press d on any field to see all dmmeta records about it (thash config, xref wiring, llist setup) gathered from orthogonal tables. Data-driven via acr_navdb.detailsrc: adding a new metadata source costs one record, zero code. Line/header storage moved from FDb to FViewmode, viewmode_stack replaces p_prev_viewmode for nested overlays.                                                                                                                                                                                                                                                                                                  |
| 17  | **Inline reftype glossary.** Add comment field to dmmeta.Reftype (35 records populated from amcdb.tclass.comment with 6 overrides for clarity), surfaced in detail view (d keybind). On-demand context for opaque reftype names like Thash, Ptrary, Tpool.                                                                                                                                                                                                                                                                                                                                                                                                                                           |
| 18  | **Namespace grouping and tree view.** Replace flat ctype list with namespace-grouped tree: 83 namespaces start collapsed, Enter toggles expand/collapse. LeftItem Tary replaces zd_sel_ctype linked list. FNs.c_ctype Ptrary indexes ctypes by namespace. Extern types grouped under "extern" label. Follow-ref auto-expands target namespace.                                                                                                                                                                                                                                                                                                                                                       |
| 19  | **Record count display.** Show ssimfile record counts next to ctype names (e.g., "Keybind (38)") for immediate sense of scale. Counts computed at startup via FileLine_curs. DecimalDigits helper fixes latent 4-digit cap in namespace header width formula.                                                                                                                                                                                                                                                                                                                                                                                                                                        |
| 20  | **Generated code preview viewmode.** New "codegen" viewmode shows amc-generated C++ struct definition for selected ctype inline in right panel. Press 'c' to toggle or Tab to cycle. SysEval captures `amc '<ctype>'` output with per-ctype caching.                                                                                                                                                                                                                                                                                                                                                                                                                                                 |
| 21  | **Field name and comment search.** New filtertarget controlled vocabulary table with Tab cycling in filter mode. Press / then Tab to switch from ctype name search to field name+comment search. Status bar shows `/f:` prefix in field mode. Matching fields highlighted bold in right panel. Filtertarget saved/restored in navstack. CtypeMatchesFilter + FieldMatchesFilter helpers with cached regex on FDb.                                                                                                                                                                                                                                                                                    |
| 22  | **Instant terminal resize.** SIGWINCH handler sets flag, main loop repaints. AdjustScroll clamps selection on shrink.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                |
| 23  | **Expanded filter targets.** Add arg, reftype, and all filter targets with data-driven match logic.                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  |
| 24  | **Syntax highlighting for line-mode views.** Codegen (C++ keywords, types, strings), detail (ssim keys/values), preview (ssim columns), help (keybind/section styles). Uses existing AddSpan + navstyle infrastructure.                                                                                                                                                                                                                                                                                                                                                                                                                                                                              |
| 25  | **Cross-namespace dependency overlay.** nsdep viewmode (n key) shows upstream/downstream namespace dependencies for selected ctype's namespace. Overlay on viewmode_stack, data-driven from cross-namespace field references.                                                                                                                                                                                                                                                                                                                                                                                                                                                                        |
| 26  | **Interactive access path graph viewmode.** New `graph` viewmode (v key) renders amc_vis-style ASCII access path diagram in right panel. Three-column layout: left (Pkey/Upptr parents), center (selected ctype), right (Val creation + Ptr/Ptrary children). Edge labels show "reftype fieldname". Enter walks into neighbor ctype staying in graph mode; Backspace returns via navstack. Edge collection matches amc_vis exactly: GraphSkipQ mirrors DepRefQ, Val fields use first-wins dedup, dep fields use reftype `up` flag for column placement. NavigateToTarget helper extracted from follow_ref eliminates navstack push duplication. Tab cycle: fields→xref→preview→codegen→graph→fields. |


---

## Known Issues

---

## Pain Points

### Solved by TUI (CLI problems that vanish with interactive browsing)


| Pain                                                                         | How acr_nav solves it                                 |
| ---------------------------------------------------------------------------- | ----------------------------------------------------- |
| **Query syntax** — `%` wildcards, `-where`, regex construction               | Live filter as you type, no syntax to learn           |
| **Silent failures** — typo in `acr ctpe:%` returns 0, exit 0, no hint        | Empty filter result is visually obvious and immediate |
| **Output overload** — `acr dmmeta.field` dumps 5894 lines                    | Shows fields for one ctype at a time                  |
| **Context loss** — no state between `acr` invocations                        | Persistent left panel + breadcrumb bar + navstack     |
| **Reference chaining** — new command per hop                                 | Enter follows ref, Backspace returns                  |
| **Flag overload** — `-pretty -cmt -fldfunc -tree` to get useful output       | All views enabled by default                          |
| **Discoverability** — must know table names like `ssimfile`, `reftype` exist | Browsable namespace tree with record counts           |


### Mostly addressed

**P2. Opaque reftype vocabulary.** M9/M10 color categories (pools=green, indexes=yellow, uprefs=cyan). M17 added reftype comments visible via `d`. **Remaining gap:** must press `d` to see explanation. Could show reftype comment in status bar when field is selected (zero-press context).

**P3. Tool-switching tax.** Down from 5 switches to ~1.5. Steps 1-2 fully replaced, step 3 partial. **Remaining:** step 4 (src_func — blocked on userfunc data, see Blocked section), step 5 (editor — out of scope). Further reduction via 1A (record browser) and 1B (transitive closure).

### Open

**P6. F-prefix gap.** `dmmeta.Ctype` becomes `dmmeta::FCtype` in C++. The convention is documented *nowhere* — not in README, tutorials, or guides. Users reading generated code can't find the schema type. **Fix proposed:** idea 1C (~10 lines in filter) + show "C++: FCtype" in panel title.

**P7. Field prefix conventions invisible.** `c_field` = Ptrary, `ind_ns` = Thash, `p_ctype` = Upptr, `zd_inst` = Llist. These prefixes encode the reftype (light Hungarian Notation from `dmmeta.fprefix`), but acr_nav shows the prefix without decoding it. **Idea:** show fprefix meaning in status bar or detail view. Data already exists in `dmmeta.fprefix` — just not loaded.

**P8. Xref via: syntax opaque.** Detail view shows `via:abt.FDb.ind_target/dev.Targsrc.target` as raw text. Should parse into: "look up `Targsrc.target` in global `ind_target` hash." **Idea:** decompose via: path into human-readable steps in detail view.

**P9. "Where do I start?" problem.** 1405 ctypes across 83 namespaces with no guided entry point. No "start here" indicator, no learning progression. Newcomer opens acr_nav and faces an alphabetical wall. **Idea:** startup hint pointing to essentials (dmmeta.Ns → Ctype → Field → Reftype). Or a curated "tour" overlay showing the 5-table bootstrap path.

**P10. Meta-schema circularity.** `dmmeta.Ctype` is a ctype, `dmmeta.Field` describes its fields, but Field is itself a Ctype... The bootstrapping is navigable in acr_nav (Enter/Backspace through the circle) but never explained. Newcomers don't know which table to read first. Related to P9.

---

## Ideas by Tier

All ideas below were validated by critical review against the actual codebase, architecture, and data quality. Problems found are noted inline. Effort estimates are post-validation.

### Tier 1: High Impact, Validated

These fill the biggest remaining gaps in acr_nav's coverage of the tool ecosystem.

#### 1A. Navigable Record Browser (enhanced preview)

The biggest gap. acr_nav browses *schema* (ctypes, fields) but not *data* (actual ssim records). Preview shows records as inert text. Enhancing it with column-level navigation and data-level follow-ref closes the loop: schema -> data -> follow reference -> data at target -> back.

**What it does:**

- Column-level cursor in preview: Left/Right moves between fields within a record row
- Follow-ref at the data level: Enter on a Pkey attribute value navigates to the referenced record in its target ssimfile (e.g., from `arg:dmmeta.Ns` in a field record, jump to `ns:dmmeta` in ns.ssim)
- Status bar shows current column's field name and reftype

**What it replaces:** `acr <table>:<pattern>`, `acr -where key:value`, the common "let me check the actual records" terminal switch.

**Validated problems to address:**

- LoadPreview (line ~494) parses tuples twice but stores only formatted strings. Column boundaries (`col_wid` array) are local variables discarded after formatting. Must persist column boundaries for cursor navigation.
- Column cursor is entirely new UI. `FPanel` has only `sel_row`/`scroll_offset`. Need new `sel_col` state on FDb or FPanel.
- Ssimfile columns don't match 1:1 with `c_field` list -- fldfunc/Substr fields appear in schema but not as ssimfile columns. Solution: match column header names against field names.
- Works for 18% of ctypes (258/1405 have ssimfiles). Same limitation as preview -- acceptable since users primarily browse schema-definition namespaces (dmmeta, dev, atfdb).
- Enhance preview rather than add a separate viewmode. Same data, same rendering, avoids Tab cycle clutter.

**Size:** 250-350 lines C++. Schema change for sel_col. ~3 ssim records.

#### 1B. Interactive Transitive Closure (via acr subprocess)

The most powerful `acr` capability with no acr_nav equivalent. `acr -ndown 2 -t` shows everything reachable within 2 hops.

**What it does:**

- In preview/data mode, select a record. Press `u` (upstream/nup) or `w` (downstream/ndown)
- Shells out to `acr '<ctype>:<pkey>' -ndown:1 -t` and displays results as a line-array overlay
- Each press re-runs with deeper `-ndown`/`-nup` level
- Follow-ref works on any record in the results

**What it replaces:** `acr <record> -nup N`, `acr <record> -ndown N`, `acr <record> -xref -tree`.

**Design note:** In-process reimplementation would require acr's `c_child` index, `EvalAttr`, and lazy record loading (500-800 lines). Shelling out to `acr` via SysEval (same pattern as codegen's `amc` call) gives 80% of the value for 20% of the effort. ~50-100ms per invocation.

**Depends on:** 1A (record-level identity -- knowing the selected record's pkey value).
**Size:** ~80 lines C++ (using SysEval), 3 ssim records.

#### 1C. F-Prefix Resolution (P6)

Tiny change, daily annoyance eliminated. `acr_nav::FNaventry` in generated code -> user types `FNaventry` in filter -> finds `acr_nav.Naventry`.

**What it does:**

- In `CtypeMatchesFilter`, when filter starts with `F` + uppercase letter, also match with the F stripped
- Optionally: show "C++: FNaventry" in the right panel title

**Validated:** Filter uses SQL-style `%pattern%` regex (line ~149). Modification is straightforward. False positive risk is negligible -- no ctype names collide after F-stripping. The F-prefix is a universal amc convention (every ctype gets F-prefixed in generated C++), so the rule applies consistently.

**Size:** ~10 lines C++, 0 ssim records. The most honest estimate in this document.

#### 1D. Clipboard/Yank

Every session ends with transferring a name. Copy to clipboard with one key.

**What it does:**

- `y` key copies context-dependent text via OSC 52 escape sequence
- Left panel: copies ctype name. Right panel fields: copies field path. Other viewmodes: copies ctype name
- Status bar flash: "Copied: dmmeta.Ctype" (needs new `flash_msg` field on FDb)

**Caveats:**

- OSC 52 is blocked by default in tmux, screen, and some terminal setups. VS Code terminal requires explicit `terminal.integrated.allowClipboardAccess`. Will silently fail for some users. No clean cross-platform fallback.
- Context dispatch is 5+ cases (left panel ctype vs namespace header, right panel per-viewmode).
- `flash_msg` infrastructure doesn't exist yet -- needs field, write/read/clear paths in render loop.

**Size:** 40-50 lines C++, 3 ssim records.

---

### Tier 2: Solid Value, Moderate Effort

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

1. **Depth-2 expansion toggle.** Press `+` to expand selected neighbor inline showing its depth-1 edges indented. `-` collapses. One node at a time. Medium complexity.
2. **Reverse xref edges.** Include `c_field_arg` back-references ("who points at me") as a fourth section. Unifies graph and xref views. Medium complexity.
3. **Node cycling.** Tab/Shift-Tab jumps cursor to next/previous neighbor opener line, skipping close/border lines. Medium complexity.
4. **Horizontal scroll.** Left/Right keys pan wide graphs via `scroll_x` offset. Medium complexity.

**Ambitious:**

1. **Full namespace topology.** Port amc_vis's topological sort layout (`Main_ColLayout` + `Main_RowLayout` with binary heaps) to show all ctypes in a namespace. The complete `amc_vis` inside acr_nav. High complexity, depends on idea 7.

**Not worth pursuing:** Unicode box drawing (compatibility risk, breaks amc_vis parity), 2D spatial cursor (fights line-oriented model; node cycling gives 90% of value at 10% cost).

#### 2C. Cross-Namespace Dependency View -- DONE (M25)

Implemented as nsdep overlay viewmode showing upstream/downstream namespace dependencies.

#### 2D. Path Finder ("How do I get from A to B?")

No existing tool answers this. Confirmed: amc_vis shows local neighborhoods, acr -nup/-ndown shows closure from one point, neither finds the route between two arbitrary types.

**What it does:**

- "Path from previous navstack ctype to current ctype" (natural UX -- user navigates to ctype B, presses a key to see path from the ctype they came from)
- BFS over field references using bidirectional Pkey/Upptr edges
- Display: `acr.FCtype -> .p_ns -> acr.FNs -> .ns -> dmmeta.Ns`. Each step navigable via Enter.

**Validated caveats:**

- Graph is sparse with Pkey/Upptr-only edges (~580 edges, 1405 nodes, average degree <0.5). Many cross-namespace queries return "no path found."
- Must use bidirectional edges (if A has Pkey->B, also traverse B->A). Without this, leaf types (Reftype, Nstype) are unreachable.
- Including all reftypes makes the graph dense but produces semantically meaningless paths.
- BFS on 1405 nodes is trivially fast (microseconds).

**Size:** ~60 lines BFS + display. Unique capability, cheap to implement.

#### 2E. Bookmarks / Jump List

The navstack is LIFO -- consumed on pop. No way to return to a "home base" after exploring.

**What it does:**

- `m` to bookmark current ctype, `'` to show bookmarks overlay (both keys currently unbound -- verified)
- Up to 10 bookmarks, session-persistent
- Enter on bookmark navigates there

**Caveat:** Session-only persistence limits value for short sessions. Cross-session persistence would require writing to disk (acr_nav is currently read-only).

**Size:** ~50 lines C++, 4 ssim records.

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

### Blocked / Deferred

These ideas have merit but are blocked on prerequisites or require more groundwork than initially estimated.

#### Source Function View (blocked on userfunc data quality)

Would eliminate step 4 of the 5-tool workflow (P3). Show hand-written functions for the selected ctype.

**Blocking issue:** 85% of `dmmeta.userfunc` records (1244/1461) have truncated primary keys due to `Smallstr50` field limit. Keys like `acr_nav...navaction_cycle_viewmode` have the ctype portion destroyed by truncation. Only ~217 records have parseable key structure. Only ~129 distinct ctypes are covered (9% of 1405 total). Zero dmmeta ctypes have userfuncs.

**Prerequisites:**

1. Fix the schema: change `Userfunc.userfunc` from `Smallstr50` to a larger type. Regenerate all records via `src_func -updateproto`.
2. Or bypass userfunc entirely: shell out to `src_func <ns>.% -proto` and post-filter by ctype name pattern. Slower, no ctype-level filtering in src_func.

#### Computed Field Colors (requires new finput + schema)

Show fldfunc/Substr fields in a distinct color in the field list.

**Why the original "zero code" claim was wrong:** Substr is NOT a reftype -- it's a separate table (`dmmeta.substr`). The rendering pipeline traverses `field -> reftype -> reftypestyle -> navstyle`. You cannot add a `reftypestyle` record for Substr because Substr isn't a reftype. To color Substr fields: (1) add `dmmeta.substr` as a finput, (2) add cross-reference from FField to substr, (3) check `fld->c_substr` in the render path.

**Size:** 3-4 ssim records + schema changes + amc regeneration + render code.

#### Input Dependency View (finput viewmode)

Show `dmmeta.finput` records for a namespace's target -- "what data does this program consume?"

**Issues:** acr_nav doesn't load finput data (needs new finput + FFinput ctype + cross-references). Only meaningful for `exe`/`lib` namespace types -- `ssimdb`/`protocol` namespaces have no finput records. Libraries often have just 1 finput record. Meaningfully different from nsdep (runtime data deps vs schema-level deps) but narrow audience.
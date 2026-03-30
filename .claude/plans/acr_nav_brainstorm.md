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
| M18.1 | Fix 2 headless bugs, add VisibleLine/SetTermSize, rename tests | Headless protocol maturity; data-driven overlay dismiss |
| M18.2 | Add VisibleLeftItem + InputError, close protocol gaps | Left panel readable by agents; explicit error feedback |
| M19 | Record count display in left panel | Ssimfile line counts at startup; DecimalDigits helper; RecordCount test |
| M20 | Generated code preview viewmode | SysEval `amc '<ctype>'`; codegen viewmode; 'c' toggle; per-ctype caching |
| M21 | Field name/comment search | filtertarget controlled vocabulary; Tab cycles search target; CtypeMatchesFilter + FieldMatchesFilter; bold highlight |

**Current:** ~1340 lines C++, 24 navactions, 39 keybinds, 2 modes, 2 panels, 6 viewmodes (fields/xref/preview/codegen/help/detail), 10 navstyles, 36 reftypestyles, 2 filtertargets, ~1400 ctypes / ~5600 fields across 83 namespaces. Namespace-grouped tree view with collapse/expand, record counts for ssimfile-backed ctypes. 22 headless tests. Headless protocol: SendKey, Screenshot, SetTermSize (input); Screen, PanelState, VisibleLeftItem, VisibleField, VisibleLine, InputError (output).

---

## Known Issues

- **I7: Filter keybind not discoverable.** The `/` filter command isn't shown in the status bar, so new users don't know it exists. The status bar should hint at available commands (like vim's bottom line).
- **I8: Control characters in ssimfile values corrupt display.** Selecting `dmmeta.Charset` in preview or detail mode renders garbled output because the `expr` field contains literal control characters (`\t`, `\r`, `\n`). `FormatPreviewRow` (line 318) and `FormatDetailCard` (line 428) write `attr.value` directly to the terminal. Fix: add a `SanitizeForDisplay` helper (1:1 replacement of bytes < 0x20 and DEL with `.`) at those two entry points. Column width calc (line 364) uses `ch_N()` which stays correct under 1:1 replacement.
- **I9: Help not shown on startup.** It also shouldn't dismiss on Up/Down. Only on enter/search/etc. So you start navigation and still see help. Once you pick namespace - help auto dissmisses.
- **I10: acr_nav not showing (x) counts for ssimfile-backed ctypes.** E.g. FNavstyle preview shows (9) but FNavstyle in the left panel shows nothing.
- **I11: enter after filter should also navigate additionally to applying the filter.** When you enter a filter and hit enter, nothing visible changes. It is confusing. It should apply the filter AND navigate / open collapsed namespaces.

---

## Pain Points

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
| ~~M19~~ | ~~1E: Record count display~~ | ~~Done~~ |
| ~~M20~~ | ~~1D: Generated code preview~~ | ~~Done~~ |
| ~~M21~~ | ~~1F: Field name/comment search~~ | ~~Done~~ |
| Later | 2A-2E, Tier 3 | As interest dictates |

**Sequencing rationale:**

- **M17 (reftype glossary)** — Done. Added `dmmeta.Reftype.comment`, surfaced in detail view.
- **M18 (namespace grouping)** — Done. LeftItem Tary, FNs.c_ctype Ptrary, collapse/expand, extern label, stable width.
- **M19 (record counts)** — Done. FSsimfile.n_record counted at startup, displayed as `(N)` in left panel. DecimalDigits helper also fixed latent 4-digit cap in namespace header width.
- **M20 (generated code)** — Done. SysEval to `amc '<ctype>'`, codegen viewmode with `has_fields:N`, `c` toggle key, per-ctype caching via `p_codegen_ctype`.
- **M21 (field search)** — Done. `acr_navdb.filtertarget` table (2 records), Tab cycles in filter mode, `CtypeMatchesFilter`/`FieldMatchesFilter` helpers, bold highlighting, cached regex, navstack save/restore. 4 new tests.

**Future factoring:** IsHelpMode (4 sites) and IsXrefMode (5 sites) are identity checks that should become data on FViewmode. At 2 modes per axis, the branching is trivial. Refactor when a 3rd mode is added to either axis. Documented in code at their definition sites. RightPanelItemCount has per-viewmode lazy-load checks for preview and codegen (2 instances); at 3, refactor to an ensure-content hook on FViewmode.

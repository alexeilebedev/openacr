# OpenACR Project Ideas

Ideas for contributing to OpenACR, ordered by schema design depth.

Design criteria:

1. **Schema design depth** — ssimfile records are the design, C++ is the implementation
2. **"Everything is data"** — behavior controlled by records, not code branches
3. **Practical utility** — solves a real workflow pain; removes complexity rather than adding it
4. **Full development loop** — acr_ed scaffold -> amc generate -> abt build -> normalize test

Design principles (from the OpenACR philosophy):

- Structure first, language second — programs are in-memory databases of typed records
- Orthogonal factorization — independent concerns as separate axes/tables
- Deterministic output — byte-identical results on every run, serial execution
- SESE coding style — single entry, single exit; no early returns
- Ease of use — an order-of-magnitude easier can matter as much as faster

---

## 1. `acr_nav` — TUI Schema Explorer (highest ceiling)

**What:** Interactive terminal tool for browsing ctypes, fields, and xrefs. Two-panel layout: ctype list (left), fields for selected ctype (right). Arrow keys navigate, Enter follows a reference to its target ctype, Backspace goes back. Navigation history as a stack.

**Replaces:** `acr ctype:amc.% | less` -> `acr field:amc.FDb.% | less` -> `acr field:amc.FTarget.% | less` (3+ commands per hop -> 1 keystroke)

**Why it's strong:**

- The program that explores in-memory databases IS ITSELF an in-memory database — recursive demonstration of the core principle
- You can run `amc_vis acr_nav.%` and see the tool's own access path diagram
- UI state modeled as ssimfile-driven schema — all with proper access paths
- Navigation between ctypes follows the same access paths that amc_vis visualizes
- Multiple access paths genuinely needed: ctypes indexed by namespace, by name, by selection state; fields indexed by parent ctype, by reftype
- Data-driven behavior: panel layout as records, key bindings as records — no switch statements
- Practical value: live cross-reference traversal beats piping acr to less
- Directly addresses the ease-of-use bottleneck — newcomers can discover the schema without knowing what to type

**Vision constraints:**

- Read-only browser only (no editing) — keep scope tight
- Schema is the star, not the TUI framework
- Raw ANSI escape sequences following amc_vis patterns — no external dependencies

**Factorization aspiration:** Adding a new panel, keybinding, or color should be a record, not a code change.

**Risk:** Terminal handling complexity drowning out schema design. Mitigation: ruthless scoping — two panels, arrow keys, Enter/Backspace. Nothing else.

**Design questions:** "Show me the schema" — read the ssimfile records before looking at C++. The tool's own data model must be clean. "What can you do here that you can't with acr | less?" — answer: live xref traversal, follow a field to its target ctype instantly.

**Strength: 9/10 if executed well, 3/10 if terminal plumbing takes over.**

---

## 2. ssim_diff — Semantic Diff for Ssimfile Records (lower risk)

**What:** Compare two versions of ssimfile data and show which records were added, removed, or modified — field by field. Unlike git diff (line-level text), this understands ssimfile semantics: records are sets of tuples, order doesn't matter, primary key identifies the record.

**Why it's strong:**

- Demonstrates understanding that ssimfiles are sets, not sequences — the core semantic property
- Schema has genuine depth: records indexed by key, by change type, by table; field-level change tracking
- Output format as data-driven: one loop over format records — no if/else chain
- Practically useful: reviewing data/ changes after schema modifications is a real workflow pain
- Deterministic output (sorted by primary key, always)

**Risk:** Could devolve into string processing with a trivial schema. Mitigation: invest heavily in schema design — field-level change tracking, table-level summaries, format selection as records.

**Design questions:** "Why not sort + comm?" — answer: comm shows line changes, not field changes; can't handle field reordering; no grouping by table or change type. "Is the output deterministic?" — yes, sorted by primary key.

**Strength: 7/10** — solid, achievable, genuine schema design opportunity.

---

## 3. `acr_watch` — Live Schema Change Monitor (reactive feedback)

**What:** Watches `data/` for file changes and displays a live, continuously updated view of what changed, what referential integrity constraints broke, and which build targets need rebuilding. Run it in a second terminal while editing ssimfiles — the consequence of every edit is visible immediately.

**Replaces:** The "edit, save, run `acr -check`, squint at output, repeat" cycle.

**Why it's strong:**

- "Sell removal, not addition" — eliminates the manual check cycle entirely
- The baseline/current comparison is literally two snapshots of the same database, diffed record by record — pure database thinking
- Deterministic: same filesystem state always produces identical output
- Schema has genuine depth: baseline vs current snapshots, change status as controlled vocabulary, integrity results, affected build targets
- Simpler TUI than acr_nav (no interactive input beyond Ctrl-C), lower implementation risk

**Risk:** File watching cross-platform (kqueue on macOS, inotify on Linux). Mitigation: fall back to simple fstat polling.

**Design questions:** "Is the diff deterministic?" — yes, always sorted by primary key. "What's the overhead?" — one fstat per known ssimfile per poll cycle, re-scan only dirty files.

**Strength: 7/10** — strong practical utility, good schema depth, less novel than acr_nav.

---

## 4. `acr_query` — Interactive REPL with Live Preview (ease of use)

**What:** Interactive prompt where you type an acr-style query and matching records appear below in real time, updating on every keystroke. Function keys toggle modes: tree view, cross-reference expansion, meta-information. Think `fzf` for the ssimfile database.

**Replaces:** Guessing table names and iterating with `acr <guess> | less`. Newcomers can explore by typing partial names and seeing what exists.

**Why it's strong:**

- Most directly addresses the ease-of-use bottleneck
- Orthogonal factorization: display format, mode (tree/xref/meta), and query are three independent axes
- Reuses `lib_rl` (already in the codebase) for readline — shows ability to build on existing infrastructure
- Uses same regex engine (`algo::Regx`) as acr — results guaranteed identical

**Risk:** Readline callback integration with live preview rendering. Mitigation: lib_rl already provides the infrastructure.

**Design questions:** "Why not just pipe acr to grep?" — answer: live preview, mode toggling, no subprocess overhead. "Are the results identical to acr?" — yes, same regex engine, same data.

**Strength: 7/10** — strong utility, good orthogonal design, lighter schema than acr_nav.

---

## 5. amc_vis Mermaid Extension (warm-up / complement)

**What:** Extend amc_vis with Mermaid diagram output format. Mermaid renders natively in GitHub markdown, so schema changes become visually reviewable in PRs. Also enrich DOT output with field-level detail and reftype color coding.

**Why it's strong:**

- Shows ability to read and extend existing code — not just greenfield
- Demonstrates understanding of access paths (the deepest OpenACR concept)
- Immediately useful: paste amc_vis output in a PR description, get a rendered diagram
- Low risk, achievable quickly

**Risk:** Too incremental by itself. Best used as a warm-up PR before tackling a main project.

**Design questions:** "Do you understand how the layout algorithm works?" — expect explanation of the binary heap topological sort in Main_RowLayout/Main_ColLayout. "Why Mermaid?" — renders in GitHub PRs, zero tooling needed.

**Strength: 6/10** — shows codebase competence, not design ability. Best as complement.

---

## Comparative Analysis

|                     | Schema depth | Data-driven signal | Practical use | Novelty |
| ------------------- | ------------ | ------------------ | ------------- | ------- |
| **acr_nav**         | Deepest      | Strongest          | High          | Highest |
| **ssim_diff**       | Good         | Strong             | Very high     | Medium  |
| **acr_watch**       | Good         | Strong             | Very high     | Medium  |
| **acr_query**       | Lighter      | Good               | Very high     | Medium  |
| **amc_vis Mermaid** | N/A          | Moderate           | High          | Low     |

## Chosen project: `acr_nav` with integrated query/filter

Build acr_nav incrementally, absorbing acr_query as a filter mode (press `/` to search).
Each milestone teaches one OpenACR concept and produces a working program.

### Milestones

| # | What you build | What you learn |
|---|---------------|----------------|
| **M0** | Nothing — explore existing tools | `acr` queries, `amc_vis`, how tools are modeled as data |
| **M1** | Ssimfile config tables, zero C++ | Schema design, controlled vocabularies, `acr -check` |
| **M2** | Scaffold with `acr_ed`, run `amc` | Meta-schema, reftypes, generated API, `amc_vis acr_nav.%` |
| **M3** | `Main()` that prints ctypes + fields | Cursor loops, hash lookups, Upptr traversal |
| **M4** | Single-panel scrollable TUI | Terminal raw mode, ANSI rendering, event loop |
| **M5** | Two panels + Enter/Backspace navigation | Xref traversal, navigation stack, the core experience |
| **M6a** | gstatic Hook dispatch — replace string if-chain | gstatic, Hook reftype, function-pointer dispatch |
| **M6b** | `/` filter mode + mode-qualified keybindings | algo::Regx, composite key lookup, mode-driven data |
| **M7** | Tests, docs, SESE cleanup | Component tests, normalization, documentation |

### Key principle

**Schema before code.** M1 is pure data design — no C++ at all. By M2, `amc_vis acr_nav.%` already shows the tool's access path diagram before a single line of hand-written C++ exists.

### What to study at each milestone

| Milestone | Study | Why |
|-----------|-------|-----|
| M0 | `amc_vis.cpp`, `acr dmmeta.field:amc_vis.FDb.%` | Pattern to follow |
| M1 | `data/dmmeta/reftype.ssim`, `data/gclidb/gtblact.ssim` | Best examples of data-driven design |
| M2 | `include/gen/amc_vis_gen.h`, `cpp/gen/amc_vis_gen.cpp` | Understand generated code |
| M3 | `include/gen/acr_nav_gen.inl.h` (own generated cursors) | Learn the generated API |
| M4 | `amc_vis.cpp` lines with `\x1b[` (ANSI codes) | Rendering patterns |
| M5 | `data/dmmeta/xref.ssim` (acr_nav entries) | Understand xref wiring |
| M6a | `data/dmmeta/gstatic.ssim`, `data/dmmeta/hook.ssim`, `amc.FGen.step` pattern | gstatic + Hook dispatch |
| M6b | `data/gclidb/gclicmd.ssim`, `cpp/samp_regx/samp_regx.cpp` | mode-qualified keybinds, algo::Regx |
| M7 | `data/atfdb/comptest.ssim`, `test/atf_comp/` | Test patterns |

### Verification (every milestone)

- `acr -check` passes
- `amc` runs clean (M2+)
- `abt -build acr_nav` compiles (M2+)
- `amc_vis acr_nav.%` shows clean access path diagram (M2+)

### Design notes from review

- **switch_panel direction (resolved in M5):** Split `switch_panel` into `switch_panel_left`/`switch_panel_right`. Left at leftmost panel is no-op, Right at rightmost is no-op. Directional semantics scale to N panels without code changes (just add panel records with increasing `position` values and wire keybinds).
- **Keybinding design:** Current keybinds cover arrow keys and vim (j/k), but MacBook keyboards lack PgUp/PgDn. Broader question: what's the right set of navigation actions for a schema browser? Should page_up/page_down exist at all, or is `/` filter (M6) the real fast-navigation answer? Should we support emacs muscle memory (Ctrl-N/Ctrl-P)? Control characters (bytes 1-31) currently need a code change to map to key names — is there a more data-driven way? Needs a design session before adding more keybinds.
- **M5 factorization debt (address in M6+):**
  - `PanelItemCount` and `Render` dispatch on `panel.position` (0 vs 1) — adding a third panel requires code changes. To factorize: each panel record should carry enough data to describe its content source (e.g., a `content_type` field), and rendering should be a single loop over panel records.
  - ~~`DispatchAction` is a string if-chain over action names — function-pointer dispatch (like `amcdb.gen` generators) warranted when action count grows beyond ~10.~~ Resolved in M6a: gstatic + Hook dispatch via `navaction_<key>()` handlers, `step_Call(*keybind->p_navaction)`.
  - `SelectedCtype` walks O(n) through `zd_sel_ctype` linked list per call — cached once per event iteration, but would benefit from a direct index if scaling to very large namespaces.
  - Navigation stack (`Naventry`) stores only `ctype_name`, `scroll_offset`, `sel_row` — no filter state. Following a ref while filtered, then pressing Backspace to go back, won't restore the filter. To fix: add `filter` (cstring) and `navmode` (Smallstr50) fields to `acr_nav.Naventry`.
  - Printable char text input in filter mode is inline code in the event loop, not data-driven dispatch. Justified asymmetry (text input vs action dispatch), but not factorized.
  - Composite keybind keys (`browse.Up`) embed a navmode prefix without referential integrity — `acr -check` won't catch `keybind:typo.Up`. To fix: add a `dmmeta.substr` that decomposes the keybind pkey and validates the mode portion against `navmode.ssim`.

### Other ideas (backlog)

- **ssim_diff** (#2) — strong standalone project if acr_nav stalls
- **acr_watch** (#3) — complementary tool, different workflow
- **amc_vis Mermaid** (#5) — good warm-up PR before or alongside acr_nav

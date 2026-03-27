# Plan: acr_nav — TUI Schema Explorer

## The idea

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

### Integrated: `acr_query` as filter mode

**What:** Interactive prompt where you type an acr-style query and matching records appear below in real time, updating on every keystroke. Function keys toggle modes: tree view, cross-reference expansion, meta-information. Think `fzf` for the ssimfile database.

**Replaces:** Guessing table names and iterating with `acr <guess> | less`. Newcomers can explore by typing partial names and seeing what exists.

**Why it's strong:**

- Most directly addresses the ease-of-use bottleneck
- Orthogonal factorization: display format, mode (tree/xref/meta), and query are three independent axes
- Reuses `lib_rl` (already in the codebase) for readline — shows ability to build on existing infrastructure
- Uses same regex engine (`algo::Regx`) as acr — results guaranteed identical

**Absorbed into acr_nav as:** press `/` to enter filter mode (M6b).

---

## Chosen project: `acr_nav` with integrated query/filter

Build acr_nav incrementally, absorbing acr_query as a filter mode (press `/` to search).
Each milestone teaches one OpenACR concept and produces a working program.

### Milestones

| # | What you build | What you learn | Status |
|---|---------------|----------------|--------|
| **M0** | Nothing — explore existing tools | `acr` queries, `amc_vis`, how tools are modeled as data | Done |
| **M1** | Ssimfile config tables, zero C++ | Schema design, controlled vocabularies, `acr -check` | Done |
| **M2** | Scaffold with `acr_ed`, run `amc` | Meta-schema, reftypes, generated API, `amc_vis acr_nav.%` | Done |
| **M3** | `Main()` that prints ctypes + fields | Cursor loops, hash lookups, Upptr traversal | Done |
| **M4** | Single-panel scrollable TUI | Terminal raw mode, ANSI rendering, event loop | Done |
| **M5** | Two panels + Enter/Backspace navigation | Xref traversal, navigation stack, the core experience | Done |
| **M6a** | gstatic Hook dispatch — replace string if-chain | gstatic, Hook reftype, function-pointer dispatch | Done |
| **M6b** | `/` filter mode + mode-qualified keybindings | algo::Regx, composite key lookup, mode-driven data | Done |
| **M7a** | Component tests + documentation | `atfdb.comptest` infrastructure, full development loop | Done |
| **M7b** | Naventry filter state + keybind substr validation | Schema evolution, `dmmeta.substr` decomposition | Done |
| **M8** | Keybinding redesign — normies + vim | Control char parsing, data-driven dispatch, factorization test | Done |

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
| M7a | `data/atfdb/comptest.ssim`, `test/atf_comp/` | Test patterns |
| M7b | `data/dmmeta/substr.ssim`, `data/dmmeta/field.ssim` | Schema evolution, substr decomposition |
| M8 | Terminal byte-to-name parsing, the factorization boundary between code and data | Where code stops and data begins |

### Verification (every milestone)

- `acr -check` passes
- `amc` runs clean (M2+)
- `abt -build acr_nav` compiles (M2+)
- `amc_vis acr_nav.%` shows clean access path diagram (M2+)

### Open items

(none)

### Known limitations

- **Panel dispatch on position:** `PanelItemCount` and `Render` dispatch on `panel.position` (0 vs 1). N=2 with genuinely different iteration patterns (linked list vs Ptrary) — not a missing noun. Revisit when a third panel type is needed.

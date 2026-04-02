# acr_nav: What's Next?

## Story So Far


| #    | Description                                                                                                                                                                           |
| ---- | ------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| 0    | **Vision and design.** Ideas document, milestone plan, constraints.                                                                                                                   |
| 1    | **Schema tables.** acr_navdb ssimdb: navaction, keybind, navmode, panel. Zero C++.                                                                                                    |
| 2    | **Exe scaffold.** acr_nav executable with dmmeta finputs and cross-references.                                                                                                        |
| 3    | **Batch mode.** Filter ctypes by namespace regex, print fields with arg and reftype.                                                                                                  |
| 4    | **Single-panel TUI.** Scrollable list with vim + arrow key navigation, batch fallback.                                                                                                |
| 5    | **Two-panel layout.** Left: ctypes, right: fields. Enter follows refs, Backspace pops navstack.                                                                                       |
| 6    | **Data-driven dispatch and filter mode.** Hook function pointers, mode-qualified keybinds, live filter.                                                                               |
| 7    | **Component tests.** Three batch-mode tests. Per-navstack filter state.                                                                                                               |
| 8    | **Keybinding redesign.** Vim + arrow keys, special keys (Tab, Ctrl-*, Home/End). 31 keybinds, 16 actions.                                                                             |
| 9–11 | **Reftype color coding.** Fields colored by structural role: pools (green), indexes (yellow), uprefs (cyan), values (default). Data-driven styles via navstyle + reftypestyle tables. |
| 12   | **Reverse xref view.** Toggle to "who points here?" view. Enter accepts filter text.                                                                                                  |
| 13   | **Help, status bar, preview, viewmode system.** ? for help, p for ssimfile preview, Tab cycles viewmodes, filter persistence.                                                         |
| 14   | **Breadcrumb bar.** Navstack path display: `MYSQL > lib_mysql.FDb > lib_mysql.Res`.                                                                                                   |
| 15   | **Help as viewmode.** Help in right panel, grouped sections, startup auto-dismiss.                                                                                                    |
| 16   | **Field detail drilldown.** Press d to see all dmmeta records for a field. Data-driven via detailsrc table.                                                                           |
| 17   | **Reftype glossary.** Inline descriptions for opaque reftypes (Thash, Ptrary, Tpool) in detail view.                                                                                  |
| 18   | **Namespace tree.** Collapsible namespace groups replacing flat list. Follow-ref auto-expands.                                                                                        |
| 19   | **Record counts.** Ssimfile record counts next to ctype names, e.g., "Keybind (38)".                                                                                                  |
| 20   | **Codegen preview.** amc-generated C++ struct shown inline (c key). Per-ctype caching.                                                                                                |
| 21   | **Field search.** Tab in filter mode switches to field name+comment search. Matches highlighted.                                                                                      |
| 22   | **Terminal resize.** SIGWINCH-driven repaint with scroll clamping.                                                                                                                    |
| 23   | **Expanded filters.** Filter by arg, reftype, or all targets.                                                                                                                         |
| 24   | **Syntax highlighting.** C++ keywords in codegen, ssim keys in detail/preview, keybind styles in help.                                                                                |
| 25   | **Namespace dependencies.** nsdep view shows upstream/downstream ns deps. Context-sensitive: auto-activates on namespace headers.                                                     |
| 26   | **Access path graph.** Interactive amc_vis-style diagram (v key). Parents left, children right, navigable.                                                                            |
| 27   | **Graph polish.** Reftype-colored edges, record counts on nodes, edge tooltips in status bar.                                                                                         |
| 28   | **Graph reverse edges.** Bidirectional access paths (forward fields + reverse xrefs), graph syntax highlighting.                                                                      |
| 29   | **Preview follow-ref.** Arrow keys cycle navigable columns in preview, Enter follows to target record.                                                                                |
| 30   | **Unicode graph.** Box-drawing characters (╔║╚╟╢╭╰─▶◀), UTF-8-aware column alignment.                                                                                                 |
| 31   | **Preview horizontal scroll.** All columns navigable via Left/Right. Auto-scroll keeps selected column visible with column-boundary snapping. FK columns cyan, non-FK default.        |
| 32   | **Ns-deps auto-view.** Context-sensitive: activates on namespace headers, restores on ctype rows. Enter jumps to dep namespace. No keybind needed.                                    |


---

## Known Issues

- Toggle functions data-driven: 5 one-liner navaction_toggle_* could collapse via toggle_viewmode field on FNavaction. Good structural win but separate scope.

---

## Pain Points

### Solved by TUI (CLI problems that vanish with interactive browsing)


| Pain                                                                                    | How acr_nav solves it                                                                                                |
| --------------------------------------------------------------------------------------- | -------------------------------------------------------------------------------------------------------------------- |
| **Query syntax** — `%` wildcards, `-where`, regex construction                          | Live filter as you type, no syntax to learn                                                                          |
| **Silent failures** — typo in `acr ctpe:%` returns 0, exit 0, no hint                   | Empty filter result is visually obvious and immediate                                                                |
| **Output overload** — `acr dmmeta.field` dumps 5894 lines                               | Shows fields for one ctype at a time                                                                                 |
| **Context loss** — no state between `acr` invocations                                   | Persistent left panel + breadcrumb bar + navstack                                                                    |
| **Reference chaining** — new command per hop                                            | Enter follows ref, Backspace returns                                                                                 |
| **Flag overload** — `-pretty -cmt -fldfunc -tree` to get useful output                  | All views enabled by default                                                                                         |
| **Discoverability** — must know table names like `ssimfile`, `reftype` exist            | Browsable namespace tree with record counts                                                                          |
| **Opaque reftype vocabulary** — Thash, Ptrary, Upptr mean nothing to newcomers          | Color categories (pool/index/upref/value), `d` for detail, graph edge tooltips                                       |
| **Tool-switching tax** — 5 tools per workflow (acr, acr -t, acr data, src_func, editor) | Down to ~1: schema/fields/xref/preview/codegen/graph/nsdep all inline. Only src_func and editor remain outside scope |


---

## Ideas

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

### Standalone: CLI + Daemon for AI Agents

Inspired by Vercel's agent-browser. Instead of the pipe protocol (`printf '...' | acr_nav -headless`), AI agents call acr_nav as a normal CLI tool with subcommands:

```
acr_nav navigate dmmeta.Field              # jump to ctype
acr_nav navigate dmmeta.Field --view xref  # jump + set view
acr_nav filter dmmeta --target field       # apply filter
acr_nav view xref                          # switch viewmode
acr_nav back                               # pop navstack
acr_nav summary                            # lightweight state (3 lines)
acr_nav screenshot                         # full state dump
acr_nav press j                            # send keystroke
acr_nav close                              # shut down daemon
```

**Architecture:** 3-tier like agent-browser. Thin Rust/C++ CLI client connects via Unix socket to a warm `acr_nav --daemon` process that holds the loaded schema in memory. CLI sends command, prints response, exits. Daemon auto-starts on first command, auto-stops on `close` or timeout.

**Why:** AI agents already know how to call CLI tools. Zero protocol docs needed -- `acr_nav --help` is self-documenting. Exit codes (0=ok, 1=error) replace parsing `ok:Y/N`. Each invocation is atomic -- no subprocess pipe management.

**Coexistence with pipe protocol:** The daemon internally uses the same semantic commands (Navigate, SetFilter, SetView, GoBack, Summary) and the same handler functions as the pipe protocol. atf_comp continues using `printf ... | acr_nav -headless` unchanged. The CLI is a thin translation layer: `acr_nav navigate X` → connect to daemon → send `acr_nav.Navigate ctype:X` → print response → exit.

**Value:** High. Cuts agent token overhead to near zero (no protocol reference, no pipe management, natural CLI).
**Size:** Medium. Daemon lifecycle, Unix socket IPC, CLI argument parsing, auto-start logic. The semantic command handlers from the pipe protocol v2 are reused directly.
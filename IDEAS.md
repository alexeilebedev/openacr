# acr_nav: What's Next?

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
| 9–10 | **Reftype color coding.** Fields colored by structural role: pools (green), indexes (yellow), uprefs (cyan), values (default). Data-driven styles via navstyle + reftypestyle tables. |
| 11   | **Headless mode.** stdin/stdout protocol for agent-driven testing. SendKey + Screenshot commands, structured ssim output (Screen, PanelState, VisibleField, VisibleLeftItem).         |
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
| 33   | **Headless protocol v2.** Semantic commands (Navigate, SetFilter, SetView, GoBack, Summary) replace keystroke-by-keystroke interaction. ~97% output token reduction for agent workflows. |


---

## Known Issues

(none)

---

## Ideas

See [RUNTIME_INSPECT.md](RUNTIME_INSPECT.md) — runtime state inspection for all amc-generated programs.
# acr_nav Exploratory Test Report

**Date:** 2026-04-01
**Scope:** Broad exploratory testing across all features, 8 parallel subagents
**Regression:** 36/36 existing tests pass

## Summary Table

| Area | Tests | Bugs | Gaps | Status |
|------|-------|------|------|--------|
| Graph view & nsdep | 22 | 1 | 2 | Mostly solid |
| Deep navstack & state | 14 | 0 | 0 | Rock solid |
| Filter interactions | 17 | 0 | 2 | Good |
| Preview nav & sel_nav_col | 12 | 0 | 2 | Clean |
| Overlay stacking & detail | 15 | 2 | 1 | Needs fixes |
| Namespace collapse & left panel | 14 | 0 | 0 | Rock solid |
| Pagination & SetTermSize | 15 | 1 | 2 | One real bug |
| Protocol robustness | 28 | 2 | 3 | Solid |
| Xref & cross-validation | 12 | 0 | 1 | Accurate |
| **Total** | **149** | **5** | **13** | |

## Bugs Found

### Bug 1: Help overlay cannot be invoked from namespace (nsdep) rows
**Severity:** Medium
**Area:** Overlay stacking
After startup help is dismissed, pressing `?` while on a namespace header has no effect. `PushOverlay` succeeds momentarily, but `PopOverlayOnCtypeChange()` (lines 439-452 of `cpp/acr_nav/acr_nav.cpp`) immediately clobbers the help overlay by detecting "not in nsdep mode on a namespace row" and auto-activating nsdep. Help works fine on ctype rows.
**Fix direction:** Guard nsdep auto-activation at line 440 with `p_cur_viewmode->is_overlay` check.

### Bug 2: Orphaned overlay stack entries from Bug 1
**Severity:** Low
Each `?` press on a namespace row pushes to `viewmode_stack` without popping, accumulating orphaned entries. These get cleaned by `RemoveAll()` when navigating to a ctype row, but the restored base viewmode may be wrong (uses first orphaned entry's saved viewmode).

### Bug 3: scroll_offset not reduced on terminal grow
**Severity:** Medium
**Area:** Pagination
When the terminal is enlarged via `SetTermSize`, `AdjustScroll` does not reduce `scroll_offset` to fill newly available viewport space. Empty rows appear at the bottom while items remain hidden above.
**Reproduction:**
```
printf 'acr_nav.SetTermSize  term_hei:10  term_wid:80\nacr_nav.SendKey  key:Esc\nacr_nav.SendKey  key:G\nacr_nav.SetTermSize  term_hei:40  term_wid:80\nacr_nav.Screenshot\n' | acr_nav -headless 2>/dev/null
```
**Fix direction:** Add to `AdjustScroll` (after line 1635):
```cpp
int max_offset = i32_Max(0, n_items - visible);
panel.scroll_offset = i32_Min(panel.scroll_offset, max_offset);
```

### Bug 4: Quit does not stop processing buffered input
**Severity:** Low
**Area:** Protocol
After `navaction_quit` sets `_db.running = false`, the inner `LinebufNext` loop continues processing remaining lines from the current `read()` buffer. Keys sent after `q` are still executed.
**Reproduction:**
```
printf 'acr_nav.SendKey  key:Escape\nacr_nav.SendKey  key:q\nacr_nav.SendKey  key:j\nacr_nav.SendKey  key:j\nacr_nav.SendKey  key:j\nacr_nav.Screenshot\n' | acr_nav -headless 2>/dev/null
```

### Bug 5: Whitespace-only lines produce InputError
**Severity:** Low
**Area:** Protocol
Lines containing only spaces/tabs produce InputError instead of being silently ignored like empty lines. The check at line 3175 uses `elems_N(line) == 0` which doesn't match whitespace-only lines.


## Protocol Gaps Found

| # | Gap | Impact |
|---|-----|--------|
| 1 | Empty graph shows no `empty_msg` in headless output (n_items:0 but no VisibleLine) | Low |
| 2 | No "no xrefs" empty message for ctypes with zero incoming references | Low |
| 3 | Empty content `empty_msg` not surfaced in headless output | Low |
| 4 | Filter field in Screen has no target prefix label | Low |
| 5 | VisibleField `match:Y/N` not emitted during active filter mode | Low |
| 6 | No column navigability indicator for preview nav columns | Low |
| 7 | No total column count (`n_nav_col`) in Screen record | Low |
| 8 | `viewmode_stack` depth not exposed in headless output | Low |
| 9 | Key names case-sensitive but near-misses silently ignored | Low |
| 10 | SetTermSize zero/negative silently clamped without warning | Low |
| 11 | Smallstr50 overflow gives generic error, not specific message | Low |
| 12 | `term_wid` doesn't affect VisibleLine content (no truncation) | Low |
| 13 | Duplicate screenshot output per Screenshot command (explicit + EOF) | Low |

## Verified Working (Highlights)

**Graph view:** Toggle with `v`, Unicode box-drawing, Tab cycling (fields->xref->preview->codegen->graph->fields), navigation updates graph, follow_ref from graph works, overlay stacking on graph works.

**Nsdep view:** Auto-activates on namespace headers, shows upstream/downstream deps, auto-switches when moving between ns/ctype rows.

**Navstack:** Tested to depth 20 via circular references. All state perfectly restored on unwind: filter text, filter target, viewmode, scroll_offset, sel_row, collapse state. Breadcrumb accurate at every depth. 10 rapid push/pop cycles — no corruption.

**Filter targets:** All 5 targets (ctype, field, arg, reftype, all) cycle correctly. Each matches against the correct criteria. Zero-match filter shows empty state cleanly. Filter cancel vs accept behaves correctly. Ctrl-U clears text but stays in filter mode.

**Preview nav:** sel_nav_col cycles through columns, Left/Right are linear (not wrapping), Enter follows FK columns, non-FK columns correctly rejected. Column selection preserved across navstack and viewmode changes. preview_nav_pending scrolls to matching record after follow.

**Overlays:** Help/detail toggle, stacking (detail over help), LIFO unwinding, Escape dismiss, need_no_overlay blocking all work correctly on ctype rows.

**Left panel:** Collapse/expand toggle, n_match/n_record cross-validated against acr (all exact matches). Filter + collapse interaction correct. j/k correctly skips collapsed contents.

**Pagination:** Visible rows = term_hei - 3 (title + header + status). Page down/up symmetric. go_top/go_bottom correct. Small terminals (5x30) degrade gracefully. Zero/negative SetTermSize clamped to 1.

**Protocol:** Malformed input → InputError with line number. Unknown keys → ignored. State not corrupted after errors. Output format ordering consistent. n_ctype:1407 and n_field:5694 match ssimfile line counts exactly.

**Xref:** Counts cross-validated for dmmeta.Ctype (64), dmmeta.Field (99), dmmeta.Ns (48), dmmeta.Ssimfile (23) — all exact matches. Reftype styles match acr_navdb.reftypestyle data. Follow from xref works. Pagination of large xref lists works.

## Observations

- Toggle keys (v, x, f, c, p) always return to `fields` when toggling off, not to previous viewmode. Consistent design.
- Viewmode inheritance on follow: following from graph enters child in graph; following from fields/xref enters in fields.
- Focus panel not saved in navstack — on pop, focus goes to content panel. Reasonable UX.
- The `_` character acts as SQL single-char wildcard in filter. Documented SQL glob behavior.
- n_sel_ctype is 1406 (not 1407) with empty filter because ctype:"" (empty-named) exists and doesn't match.
- acr_nav loads 1407 of 1462 total ctypes (scoped by acr_in).

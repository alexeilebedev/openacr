# Headless Mode Comprehensive Test Report

**Date:** 2025-03-29
**Branch:** acr-nav
**Method:** 60+ automated tests via headless protocol across 7 functional areas

## Test Summary

| Area | Tests | Pass | Fail | Notes |
|------|-------|------|------|-------|
| Navigation boundaries | 10 | 10 | 0 | 1 design gap (page-up/down untestable) |
| Filter mode | 14 | 14 | 0 | Solid |
| Viewmode & overlays | 10 | 9 | 1 | Escape doesn't dismiss overlays |
| Navigation stack | 7 | 7 | 0 | Solid through 6+ levels |
| Error handling / protocol | 12 | 7 | 5 | startup_help bug |
| Xref | 8 | 8 | 0 | Counts match `acr` exactly |
| Namespace & content | 9 | 9 | 0 | Solid |

**Pre-existing issue:** All 11 existing `atf_comp` headless tests were failing due to stale expected output (schema grew: `n_ctype:1397->1398`, `n_field:5597->5603`). Re-captured with `atf_comp acr_nav.Headless% -capture`. All 11 now pass.

---

## App Bugs Found

### Bug 1: Unknown/empty keys dismiss startup help overlay

**Location:** `cpp/acr_nav/acr_nav.cpp:1243-1246`

The `startup_help` guard runs unconditionally before keybind lookup:
```cpp
if (acr_nav::_db.startup_help) {
    acr_nav::_db.startup_help = false;
    PopViewmode();
}
```

Any `ProcessKey` call — including empty keys (`key:`), unknown keys (`key:F13`, `key:Unknown`), and malformed SendKey records with missing key field — pops the help overlay as a side effect, even though the key matched no keybind. The viewmode changes from `help` to `fields` spuriously.

**Affected tests:** Empty key value, missing key field, unknown multi-char keys, some control characters.

**Fix:** Guard the pop on keybind match:
```cpp
if (acr_nav::_db.startup_help && keybind) {
    acr_nav::_db.startup_help = false;
    PopViewmode();
}
```
Or move the guard to after the keybind lookup succeeds. The startup_help flag must be cleared (so it only fires once), but `PopViewmode()` should only execute when the key is recognized.

### Bug 2: Escape doesn't dismiss overlays

Escape is bound to `filter_clear` in browse mode. When help or detail overlay is active and no filter is set, Escape does nothing. Users expect Escape to dismiss the topmost overlay.

The only way to dismiss overlays is using the same key that opened them (`?` for help, `d` for detail).

**Fix:** In `navaction_filter_clear` (or a new action), check if an overlay is active and pop it. Alternatively, add a keybind `browse.Escape -> dismiss_overlay` that pops viewmode_stack when in overlay mode, falling through to `filter_clear` when not.

---

## Headless Mode Protocol Gaps

### Critical Gaps

#### Gap 1: No content output for text-based viewmodes (preview, detail, help)

`HeadlessOutput()` emits `VisibleField` records for field-based viewmodes (fields, xref) — one record per field with full metadata (field name, arg, reftype, style, navigable). But for text-based viewmodes (preview, detail, help), only `PanelState.sel_value` is emitted — the text of a single selected row.

The agent is blind to:
- Preview mode: actual ssimfile content, columnar formatting, all rows except selected
- Detail mode: field metadata cards, reftype info, detailsrc records
- Help mode: keybinding help text

To see N lines, the agent must send N Down keys and N Screenshots — 2N commands for one screen.

**Fix:** Add `acr_nav.VisibleLine` output record:
```
acr_nav.VisibleLine  row:0  text:"..."
```
Emit one per visible line in text-based modes, paralleling `VisibleField` for field-based modes. This single addition closes 3 critical gaps.

#### Gap 2: No left panel item list

Only `PanelState.sel_value` for the selected ctype/namespace is emitted. The full list of namespaces, their collapse state, and the ctypes under each are invisible.

The agent cannot verify:
- Whether a namespace is collapsed or expanded
- Which ctypes appear in the filtered list
- The ordering of items

**Fix:** Add `acr_nav.LeftItem` output record:
```
acr_nav.LeftItem  row:0  value:dmmeta  kind:ns  collapsed:Y
acr_nav.LeftItem  row:1  value:dmmeta.Ctype  kind:ctype  collapsed:N
```

#### Gap 3: `term_hei=100000` makes pagination untestable

`HeadlessMain()` sets `term_hei = 100000`, so `VisibleRows()` returns ~99998. Page-up/down (Space, b, PgUp, PgDown) jump 99998 rows, collapsing to Home/End behavior. `scroll_offset` never departs from 0.

The agent cannot test:
- Incremental page scrolling
- `AdjustScroll` viewport clamping logic
- Scroll offset tracking across navigation
- Partial-page behavior near list boundaries

**Fix:** Add `acr_nav.SetTermSize` input record:
```
acr_nav.SetTermSize  width:80  height:24
```
Sets `term_wid` and `term_hei` at runtime, enabling realistic scroll testing. Also closes Gap 5 below.

### Moderate Gaps

#### Gap 4: No error reporting for malformed input

Unrecognized input lines are silently dropped. Valid SendKey with unbound keys is also silent. Makes debugging test scripts harder.

**Fix:** Add error output:
```
acr_nav.Error  line:5  msg:"unrecognized input"
```

#### Gap 5: No terminal resize capability

Agent cannot simulate narrow/wide terminals or test text truncation. Same fix as Gap 3 (`SetTermSize`).

#### Gap 6: Missing cosmetic output

| TUI Element | In Headless? |
|---|---|
| Column headers (Field/Arg/Reftype) | No |
| Panel titles (ctype name, count) | No |
| Empty-state messages (`viewmode.empty_msg`) | No |
| Status bar position indicator (3/42) | No |
| Breadcrumb bar | Yes (via `Screen.breadcrumb`) |

**Fix:** Add `header` field to PanelState. Add `left_title` and `right_title` to Screen.

### Minor Gaps

#### Gap 7: `q` key is no-op in headless mode

The process only exits on EOF. `q` is silently consumed. May be intentional (headless always runs to EOF), but undocumented.

#### Gap 8: No targeted queries

Full screenshot is the only observation mechanism. Minor because screenshots are cheap and easy to parse.

---

## Verified Working (No Issues)

These areas were tested thoroughly and work correctly:

- **Navigation boundaries:** sel_row clamped at 0 and n_items-1, no negative/overflow
- **Panel focus:** Left/Right correctly switch focus, no wrap, no crash at boundaries
- **Filter:** SQL glob matching, case-insensitive, Backspace in filter mode, cancel with Escape, empty filter, no-match state, 500-char filter
- **Filter + navstack:** Follow clears filter, backtrack restores it
- **Navstack:** 6+ levels deep, correct breadcrumb accumulation, correct backtrack, Backspace at depth 0 is no-op
- **Viewmode cycle:** fields -> xref -> preview (3-way cycle, help/detail are overlays)
- **Overlay stacking:** help over detail, dismiss in correct order
- **Xref counts:** exact match with `acr dmmeta.field -where arg:X` queries
- **Xref navigation:** follow xref pushes navstack, resets to fields viewmode
- **Namespace collapse/expand:** Enter toggles, j skips collapsed groups
- **Content panel:** sel_row tracks correctly, resets on ctype change, boundary clamping works
- **VisibleField styles:** match `acr_navdb.reftypestyle` table exactly
- **EOF screenshot:** auto-emitted on stdin close
- **Multiple screenshots:** each produces separate blank-line-delimited block
- **Rapid keys:** 100 sequential keys processed correctly
- **Long filter:** 500 chars handled without crash or truncation
- **Exit codes:** consistently 0 in all scenarios

---

## Recommended Fix Priority

| # | Fix | Closes | Complexity |
|---|-----|--------|------------|
| 1 | Add `VisibleLine` output record | Gaps 1 (preview/detail/help content) | Medium — new output type, emit loop in HeadlessOutput |
| 2 | Add `SetTermSize` input record | Gaps 3, 5 (pagination, resize) | Small — new input type, set term_hei/term_wid |
| 3 | Add `LeftItem` output record | Gap 2 (left panel visibility) | Medium — new output type, emit loop |
| 4 | Fix startup_help guard | Bug 1 (unknown keys pop help) | Small — move PopViewmode after keybind lookup |
| 5 | Escape dismisses overlays | Bug 2 (UX expectation) | Small — check overlay state in filter_clear |
| 6 | Add error output | Gap 4 (silent failures) | Small — else branch + Error record |
| 7 | Add header/title fields | Gap 6 (cosmetic output) | Small — additional fields on existing records |

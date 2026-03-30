# Exploratory Test Report: acr_nav Headless Protocol

**Date:** 2026-03-30
**Branch:** acr-nav
**Method:** 142 automated tests via headless protocol across 4 functional areas, 4 parallel subagents
**Regression:** All 23 existing `atf_comp acr_nav.%` tests pass

## Summary Table

| Area | Tests | Bugs | Gaps | Notes |
|------|-------|------|------|-------|
| SetTermSize & Pagination | 74 | 3 | 2 | Zero/negative values accepted, Visible* not clipped |
| State Combinations | 16 | 4 | 1 | Navstack doesn't save focus or right panel position |
| Field Filter & Codegen | 20 | 1 | 1 | Minor count discrepancy, no match indicator |
| Dismiss/Overlay/Empty States | 32 | 2 | 2 | filter_cancel destroys previous filter |

**Total: 142 tests, 7 unique bugs found (3 overlap), 6 protocol gaps**

After deduplication: **5 distinct bugs, 6 protocol gaps.**

---

## Bugs Found

### Bug 1: filter_cancel destroys previous filter instead of restoring it
**Severity:** High (data loss — user loses carefully typed filter)
**Found by:** State Combinations, Dismiss/Overlay agents (independently confirmed)
**Location:** `cpp/acr_nav/acr_nav.cpp` — `navaction_filter_start()` clears filter immediately via `ch_RemoveAll`, and `navaction_filter_cancel()` clears it again. No pre-filter state is saved.

When a filter is active (e.g., "dmmeta"), pressing `/` to start a new filter clears the old filter immediately. Pressing Escape to cancel the new filter clears it again, leaving `filter:""` instead of restoring "dmmeta". Also resets `filtertarget` to default even if user had set it to "field".

**Reproduction:**
```
j / d m m e t a Enter   # filter="dmmeta", n_sel_ctype reduced
/ x y z Escape           # filter="" instead of "dmmeta"
```

**Fix:** Save `filter` and `filtertarget` in `navaction_filter_start()` before clearing. Restore them in `navaction_filter_cancel()`.

### Bug 2: Content panel sel_row not saved/restored on navstack backtrack
**Severity:** Medium (user loses scroll position in field list)
**Found by:** State Combinations agent
**Location:** `cpp/acr_nav/acr_nav.cpp` — `navaction_follow_ref()` saves `left->sel_row` and `left->scroll_offset` but not `right->sel_row` or `right->scroll_offset`. `navaction_go_back()` never restores these.

When browsing fields at row 5 in the content panel, following a reference, then going back, the content panel resets to row 0.

**Reproduction:**
```
j / dmmeta.Field Enter Enter   # select dmmeta.Field
j Right j j j j j Screenshot   # content sel_row=5
Enter Screenshot               # follow ref, new ctype
Backspace Screenshot           # back — content sel_row=0 (should be 5)
```

### Bug 3: Focus panel not saved/restored on navstack backtrack
**Severity:** Medium (user must manually re-focus content panel after backtrack)
**Found by:** State Combinations agent
**Location:** `cpp/acr_nav/acr_nav.cpp` — `Naventry` struct does not store `p_cur_panel`. Neither `navaction_follow_ref()` nor `navaction_go_back()` handles focus state.

**Reproduction:**
```
j Enter j Right Screenshot   # focus:content
Enter Left Screenshot        # follow ref, focus:ctype_list
Backspace Screenshot         # back — focus:ctype_list (should be content)
```

### Bug 4: Escape (dismiss_or_clear) at navstack depth > 0 resets selection position
**Severity:** Medium (user loses navigation position unexpectedly)
**Found by:** Dismiss/Overlay agent
**Location:** `cpp/acr_nav/acr_nav.cpp` — `navaction_filter_clear()` invokes `BuildLeftItemsReset()` which resets `sel_row` to 0 even when filter is already empty.

At navstack depth > 0, with no overlay and no filter, pressing Escape calls `filter_clear` which rebuilds the left panel and resets `sel_row` to 0, destroying the current position.

**Reproduction:**
```
j / dmmeta.Ctype Enter Enter   # navigate to dmmeta.Ctype
j l j Enter                    # follow ref to dmmeta.Ns at depth 1
Escape Screenshot              # sel_value resets to "" (should stay "dmmeta.Ns")
```

**Fix:** In `navaction_filter_clear`, early return if filter is already empty.

### Bug 5: SetTermSize accepts zero and negative values without validation
**Severity:** Low (degenerate state, no crash)
**Found by:** SetTermSize agent
**Location:** `cpp/acr_nav/acr_nav.cpp` — SetTermSize handler applies values directly without bounds checking.

`term_hei:0`, `term_hei:-1`, `term_wid:0` are silently accepted. At `term_hei:0`, page_size is clamped to 1 (no crash), but the state is degenerate. Missing parameters default to 0.

**Reproduction:**
```
acr_nav.SetTermSize  term_hei:0  term_wid:80
acr_nav.Screenshot
```

**Fix:** Clamp `term_hei` and `term_wid` to minimum of 1 (or 3 for term_hei to account for header/status). Emit InputError for invalid values. Require both parameters.

---

## Protocol Gaps

### Gap 1: Visible* records not clipped to terminal viewport
**Impact:** Medium
SetTermSize affects pagination (page_size, scroll_offset) but does NOT limit the number of `VisibleLeftItem`, `VisibleField`, or `VisibleLine` records emitted. All items are always output regardless of `term_hei`. Headless consumers cannot directly observe what a real user would see — they must compute the visible window from `scroll_offset` and `term_hei`.

### Gap 2: term_wid has no observable effect in headless output
**Impact:** Low
Changing `term_wid` produces no visible difference in any headless output field. Width affects only the TUI rendering layer (truncation, layout split) which is not reflected in the protocol.

### Gap 3: No field-level match indicator in VisibleField during field filter
**Impact:** Low
When `filtertarget:field` is active, `VisibleField` records don't indicate which fields matched the filter. The TUI highlights matching fields, but headless consumers can't determine which fields triggered the match.

### Gap 4: No feedback when toggle operations are silently no-op
**Impact:** Low
When `p` (toggle_preview) is pressed on a ctype with no ssimfile, or `d` (show_detail) when content is empty, the command is silently ignored. No indication that the operation was unavailable.

### Gap 5: Help overlay shows blank key name for Ctrl-U and filter Tab
**Impact:** Low (cosmetic)
Help VisibleLine entries for Ctrl-U and Tab-in-filter-mode show blank key name columns, though descriptions explain the functionality.

### Gap 6: Field filter "%" matches 1307 ctypes instead of expected 1309
**Impact:** Low (minor count discrepancy)
Using `%` as a field filter shows `n_sel_ctype:1307` but `acr dmmeta.field` suggests 1309 ctypes have fields. Two ctypes are missed — possibly ctypes without any fields or with fields whose names/comments don't pass the glob.

---

## Verified Working

### Navigation & Boundaries
- Page_down/up advance/retreat by exactly `term_hei - 2` positions
- Page_size clamped to minimum 1 at tiny terminal sizes
- scroll_offset correctly maintained through all operations
- Home/End (g/G) work correctly at all terminal sizes
- Cursor boundaries clamped at 0 and n_items-1, no overflow
- INT32_MAX values for term_hei/term_wid handled without crash

### Filter
- SQL glob matching, case-insensitive, all special chars work
- Field filter (filtertarget:field) matches names and comments
- Field filter + follow_ref pushes navstack correctly, clears filter
- Field filter + backtrack restores filter text and filtertarget
- Field filter + viewmode switch preserves filter across viewmodes
- Ctrl-U clears filter text (stays in filter mode)
- Backspace in filter mode deletes last character
- Zero-match filter: j, k, l, Enter are safe no-ops

### Viewmodes & Overlays
- Tab cycles: fields -> xref -> preview -> codegen -> fields
- Help/detail are overlays that stack correctly
- Overlay stacking: detail over help, Escape pops in correct order
- Tab/Enter blocked within overlay viewmodes
- j/k navigation works within detail overlay
- p toggles preview on/off, c toggles codegen on/off
- Codegen output matches `amc` byte-for-byte
- Codegen works at any navstack depth

### Navstack
- Viewmode saves and restores correctly at all depths
- Filter text and filtertarget save/restore correctly on backtrack
- Left panel sel_row restores correctly (by ctype name scan)
- Breadcrumb trail accurate at all depths
- Backspace at depth 0 is a no-op

### Escape (dismiss_or_clear)
- Dismisses help overlay first, then detail, then clears filter (correct priority)
- Double Escape: overlay dismissed first, then filter cleared
- Escape with nothing active at depth 0 is a no-op

### Cross-Validation
- Field counts match `acr dmmeta.field` exactly for tested ctypes
- Xref counts match `acr dmmeta.field -where:arg:X` exactly
- Codegen content matches `amc` output exactly

---

## Observations

- Page_down + page_up round-trip: sel_row returns to same value but scroll_offset may differ by 1 (window shifts slightly). Not a bug — reflects asymmetric viewport adjustment.
- Startup help is dismissed by ANY keybind match (and that key's action is also processed). This differs from `?` help overlay which requires Escape or `?` to dismiss.
- Backspace always means "go back" regardless of overlay state. Escape always means "dismiss/clear". This asymmetry is a deliberate design choice.
- Filter start from content panel always moves focus to ctype_list (correct — filters apply to left panel).
- The "Visible" prefix in VisibleLeftItem/VisibleField/VisibleLine is misleading since these emit ALL items, not just viewport-visible ones.

---

## Recommended Fix Priority

| # | Fix | Bug | Complexity |
|---|-----|-----|------------|
| 1 | Save/restore filter state in filter_start/filter_cancel | Bug 1 | Small — save 2 fields before clearing, restore on cancel |
| 2 | Save/restore right panel sel_row + scroll_offset in navstack | Bug 2 | Small — add 2 fields to Naventry, save in follow_ref, restore in go_back |
| 3 | Save/restore p_cur_panel in navstack | Bug 3 | Small — add 1 field to Naventry |
| 4 | Early return in filter_clear when filter already empty | Bug 4 | Trivial — 2-line guard |
| 5 | Validate SetTermSize parameters | Bug 5 | Small — clamp + InputError |

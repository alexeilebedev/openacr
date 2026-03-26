# M5: Two Panels + Enter/Backspace Navigation [DONE]

## Context

acr_nav M1–M4 are complete. The tool has a single-panel scrollable TUI showing a ctype list with up/down/page navigation. M5 adds the **core experience**: two-panel side-by-side layout with xref traversal — left panel shows ctypes, right panel shows the selected ctype's fields. Enter follows a field's `p_arg` reference to its target ctype; Backspace returns via the navigation stack.

Schema mostly designed in M1–M2. One schema change in M5: split `switch_panel` into directional `switch_panel_left`/`switch_panel_right` navactions so Left at leftmost panel is a no-op (not a toggle).

**Factorization debt (M6+):** documented in `txt/ideas.md` "Design notes from review" section. Three items: position-based dispatch in `PanelItemCount`/`Render`, string if-chain in `DispatchAction`, O(n) `SelectedCtype` walk. None triggered by M6 scope.

## Files modified

- `cpp/acr_nav/acr_nav.cpp` — two-panel rendering, navigation dispatch, navstack push/pop
- `data/acr_navdb/navaction.ssim` — replaced `switch_panel` with `switch_panel_left`/`switch_panel_right`
- `data/acr_navdb/keybind.ssim` — Left→switch_panel_left, Right→switch_panel_right

## Implementation steps

### Step 1: Add helper `SelectedCtype()`

Returns the FCtype currently highlighted in the left panel (position 0), or nullptr if list is empty. Walks `zd_sel_ctype` linked list to the selected index.

**Called once per event iteration** — result cached in a local variable and passed to all consumers (Render, DispatchAction, AdjustScroll) to avoid repeated O(n) walks.

### Step 2: Add helper `PanelItemCount()`

Returns item count for the given panel. Uses `position` field (0 = ctype list, 1 = field list) rather than string comparison against panel names. Takes the cached `FCtype*` as a parameter to avoid re-walking the list.

```cpp
static int PanelItemCount(acr_nav::FPanel &panel, acr_nav::FCtype *sel_ct) {
    int ret = 0;
    if (panel.position == 0) {
        ret = acr_nav::zd_sel_ctype_N();
    } else if (panel.position == 1) {
        if (sel_ct) {
            ret = c_field_N(*sel_ct);
        }
    }
    return ret;
}
```

### Step 3: Parameterize `AdjustScroll()`

Change signature to `AdjustScroll(FPanel &panel, int n_items)`. Clamp `sel_row` to valid range within the function. Called for **both** panels after every action dispatch.

### Step 4: Extend `DispatchAction()` with three new actions

Takes cached `sel_ct` and both panel pointers as parameters. Uses `PanelItemCount()` for n_items instead of hardcoded `zd_sel_ctype_N()`.

**`switch_panel_left`/`switch_panel_right`**: Directional focus change — only moves if there is a panel in that direction (compares `position` field). No-op at boundary.

**`follow_ref`**:
- If on position-0 panel: switch focus to position-1 panel.
- If on position-1 panel: get selected field via `c_field_Find(*sel_ct, panel.sel_row)`, follow `fld->p_arg` to target ctype. Guard with `zd_sel_ctype_InLlistQ(*target)` (target may be outside `-ns` filter). Push current left-panel state onto navstack via `navstack_Alloc()` (returns `Naventry&`). Walk `zd_sel_ctype` to find target's index, update left panel's `sel_row`.
- Right panel reset factored out of both branches using `did_follow` flag (action factoring).

**`go_back`**: If `!navstack_EmptyQ()`, read from `navstack_Last()` (returns `Naventry*`, use `->` access), restore left panel's scroll_offset/sel_row, call `navstack_RemoveLast()`, reset right panel.

**Right panel reset on left selection change**: Done in the event loop by comparing left panel's `sel_row` before and after dispatch:
```cpp
int prev_left_sel = left->sel_row;
DispatchAction(...);
if (left->sel_row != prev_left_sel) {
    right->sel_row = 0;
    right->scroll_offset = 0;
}
```

### Step 5: Rewrite `Render()` for two-panel layout

Takes cached `sel_ct` as parameter to avoid re-walking the list.

**Terminal resize**: Call `DetectTerminal()` at the top of each `Render()` call (ioctl is cheap).

**Geometry**: `left_wid = i32_Max(2, term_wid * left->width_pct / 100)`, `right_wid = i32_Max(1, term_wid - left_wid)`. Clamped to prevent UB on very narrow terminals.

**Row 0 — Title bar**: Both panels' titles. Focused panel: `\x1b[7m` (inverse). Unfocused: `\x1b[2;7m` (dim inverse). Left title: `" Types (N)"`. Right title: `" Fields: <ctype> (N)"`. **Truncate** title text to panel width to prevent overflow.

**Rows 1..term_hei-2 — Content**: Build each row by concatenating left cell + `|` separator + right cell.

- Left cell: pre-walk `zd_sel_ctype` to `scroll_offset`, then iterate visible rows. Show ctype name, pad to `left_wid - 1` chars. Highlight selected row: inverse if focused, bold if unfocused.
- Right cell: use `c_field_Find(*sel_ct, right->scroll_offset + row)`. Show field name, arg ctype, reftype in columns. Pad to `right_wid` chars. Highlight selected row: inverse if focused.
- **ANSI-safe rendering**: Build plain text first, pad to exact display width, **then** wrap in escape codes. Every cell ends with `\x1b[0m` to prevent attribute bleed across the separator.

Row pattern: `<left_attr><left_text_padded>\x1b[0m|<right_attr><right_text_padded>\x1b[0m\x1b[K\r\n`

**Last row — Status bar**: `q:quit  Arrows:navigate  Enter:follow  Backspace:back    row/total`

### Step 6: Update `Main()` initialization and event loop

- Cache both panel pointers at initialization (`left`, `right`). Verify both exist with `vrfy`.
- Initialize both panels (sel_row=0, scroll_offset=0).
- Event loop: compute `sel_ct` once, pass to all functions. After dispatch, adjust scroll for both panels:
```cpp
AdjustScroll(*left, zd_sel_ctype_N());
AdjustScroll(*right, sel_ct ? c_field_N(*sel_ct) : 0);
```

## Edge cases

| Scenario | Behavior |
|---|---|
| Right panel focused, ctype has 0 fields | Empty panel, move is no-op |
| Enter on field whose target is outside `-ns` filter | No-op (`zd_sel_ctype_InLlistQ` check) |
| Backspace with empty navstack | No-op |
| Enter on left panel | Switch focus to right panel |
| Very narrow terminal | Content truncated, functional |
| Terminal resize mid-session | Handled — `DetectTerminal()` called every render |

## Verification

1. `abt -build acr_nav` compiles clean
2. `acr_nav -ns:amc` — two panels visible, left shows ctypes, right shows fields of selected ctype
3. Up/Down navigates within focused panel; Left/Right switches panels
4. Enter on right panel follows field reference (navigates to target ctype)
5. Backspace returns to previous ctype with restored scroll position
6. Enter on field whose arg type is outside namespace filter → no-op
7. Backspace with empty stack → no-op
8. `acr_nav | head` — batch mode still works (no regression)

# acr_nav Headless Protocol Reference

## Invocation

```bash
printf 'acr_nav.SendKey  key:j\nacr_nav.Screenshot\n' | acr_nav -headless 2>/dev/null
```

Headless mode activates with `-headless` flag or when stdout is not a TTY. Commands go to stdin as ssimfile tuples, structured state comes out on stdout.

## State Machine

**Modes** (from `acr_navdb.navmode`):
- `browse` — default. Keys dispatch navigation actions.
- `filter` — entered via `/`. Printable characters append to filter string. Enter applies, Escape cancels.

Keybinds are mode-qualified. `ProcessKey` composes `"{navmode}.{key}"` and looks it up in `ind_keybind`. In filter mode, any single printable character (>32) not matching a keybind is appended to the filter string — filter input is via `SendKey key:a`, `SendKey key:b`, etc.

**Viewmodes** (from `acr_navdb.viewmode`):
- `fields` (has_fields:Y) — forward fields of selected ctype. Tab next: xref.
- `xref` (has_fields:Y) — reverse cross-references. Tab next: preview.
- `preview` (has_fields:N) — ssimfile record content. Tab next: codegen.
- `codegen` (has_fields:N) — amc-generated C++ struct. Tab next: graph.
- `graph` (has_fields:N) — interactive access path diagram (amc_vis-style). Tab next: fields.
- `help` (has_fields:N) — keybinding help. Overlay, toggled by `?`.
- `detail` (has_fields:N) — per-field metadata cards. Overlay, toggled by `d`.

`has_fields:Y` viewmodes emit `VisibleField` records. `has_fields:N` emit `VisibleLine` records. Tab cycles the non-overlay chain: fields -> xref -> preview -> codegen -> graph -> fields. Help and detail are overlays pushed/popped on `viewmode_stack` (stacking is possible: `d` during help pushes detail on top).

## Input Records

```
# Send a key event
acr_nav.SendKey  key:<Smallstr50>

# Request state dump
acr_nav.Screenshot

# Set terminal dimensions (affects pagination)
acr_nav.SetTermSize  term_hei:<i32>  term_wid:<i32>
```

## v2 Semantic Commands

Semantic commands return `acr_nav.Ack` followed by a blank line. Use alongside v1 commands.

```
acr_nav.Navigate  ctype:<key>                    # Navigate to ctype (dismisses help, expands ns)
acr_nav.Navigate  ctype:<key>  viewmode:<name>   # Navigate + set viewmode (rejects nsdep, overlays)
acr_nav.SetFilter  filter:<text>  target:<name>  # Apply filter (empty filter clears; target default: ctype)
acr_nav.SetView  viewmode:<name>                 # Switch viewmode (rejects nsdep, detail; accepts help)
acr_nav.GoBack                                   # Pop navstack (Ack ok:N if empty)
acr_nav.Summary                                  # Lightweight state: Screen + 2x PanelState only
```

**Ack output record** (1 per semantic command, terminated by blank line):
```
acr_nav.Ack  ack:<cmd_type_tag>  ok:<Y|N>  ctype:<key>  viewmode:<name>
    navstack_depth:<i32>  msg:<text>
```
Fields after `ok` report current state. `msg` is empty on success, error message on failure.

## Output Records

**Screen** — top-level state (1 per screenshot):
```
acr_nav.Screen  mode:<browse|filter>  focus:<ctype_list|content>  filter:<text>
    navstack_depth:<i32>  n_sel_ctype:<i32>  n_ctype:<i32>  n_field:<i32>
    viewmode:<name>  breadcrumb:<text>  filtertarget:<ctype|field>
```

**PanelState** — per-panel state (2 per screenshot: left then right):
```
acr_nav.PanelState  panel:<ctype_list|content>  sel_row:<i32>
    scroll_offset:<i32>  n_items:<i32>  sel_value:<text>
```

**VisibleLeftItem** — one per left panel row (between left and right PanelState):
```
acr_nav.VisibleLeftItem  row:<i32>  value:<ns_or_ctype_key>  kind:<ns|ctype>
    collapsed:<Y|N>  n_match:<i32>  n_record:<i32>
```

**VisibleField** — one per field in has_fields:Y viewmodes (after right PanelState):
```
acr_nav.VisibleField  row:<i32>  field:<qualified_name>  arg:<ctype>
    reftype:<name>  style:<navstyle>  navigable:<Y|N>
```

**VisibleLine** — one per line in has_fields:N viewmodes (after right PanelState):
```
acr_nav.VisibleLine  row:<i32>  value:<text>
```

**InputError** — emitted inline when input is malformed:
```
acr_nav.InputError  lineno:<i32>  msg:<text>
```

## Output Block Ordering

Each Screenshot produces records in this exact sequence:
1. `Screen` (1)
2. `PanelState` for ctype_list (1)
3. `VisibleLeftItem` (N, one per visible left panel row)
4. `PanelState` for content (1)
5. `VisibleField` (N) or `VisibleLine` (N), depending on viewmode
6. Blank line (block terminator)

`InputError` records are emitted inline when malformed input is encountered.

## Keybinds

Browse mode:
```
/         filter_start          Enter     follow_ref
?         show_help             Escape    dismiss_or_clear
Backspace go_back               Tab,r     cycle_viewmode
j,Down    move_down             c         toggle_codegen
k,Up      move_up               d         show_detail
g,Home    go_top                p         toggle_preview
G,End     go_bottom             q         quit
Space,PgDown  page_down         h,Left    switch_panel_left
b,PgUp    page_up               l,Right   switch_panel_right
```

Filter mode:
```
Enter     filter_accept         Escape    filter_cancel
Backspace filter_backspace      Ctrl-U    filter_clear
Tab       filter_cycle_target   Space     filter_append_space
j,Down    move_down             k,Up      move_up
PgDown    page_down             PgUp      page_up
Left      switch_panel_left     Right     switch_panel_right
(any printable char not above)  -> appended to filter string
```

## Behavioral Notes

- **EOF auto-screenshot**: a final screenshot is auto-emitted when stdin closes, even without an explicit Screenshot command
- **Empty input lines**: treated as ssim field separators, silently ignored (NOT errors)
- **Startup help**: initial viewmode is `help`. First successful keypress (matching a keybind) dismisses it, changing viewmode to `fields`
- **Default term_hei**: headless sets `term_hei=100000` (all items visible, no pagination). Use `SetTermSize` to test scroll behavior at realistic sizes
- **Filter matching**: SQL glob (`%filter%`), case-insensitive. Not regex.
- **Navstack**: follow_ref pushes state (filter, selection, viewmode, scroll). Backspace pops and restores all state.
- **Namespace headers**: Enter on a namespace header toggles collapse/expand (not follow_ref). Collapsed namespaces are skipped by j/k.
- **v2 commands dismiss startup help**: All semantic commands (except Summary) clear the startup help overlay
- **v2 commands cancel filter mode**: If filter mode is active when a semantic command runs, filter is cancelled first
- **Navigate pushes navstack unconditionally**: Navigating to the same ctype you're already on pushes a new navstack entry (consistent with v1 follow_ref)
- **Summary output**: Screen + left PanelState + right PanelState + blank line (4 lines total, ~400 bytes)

## Common Patterns

```
# Navigate and check state (minimal tokens):
acr_nav.Navigate  ctype:dmmeta.Ctype
acr_nav.Summary
# -> Ack shows ok:Y, Summary shows 3 state records

# v1/v2 equivalence test:
# v2: Navigate + Summary -> extract Screen line
# v1: / + type filter + Enter + Screenshot -> extract Screen line
# Compare: viewmode, sel_value, n_sel_ctype must match

# Error handling:
acr_nav.Navigate  ctype:bad.Name
# -> Ack ok:N msg:"ctype not found: bad.Name"

# Full workflow:
acr_nav.Navigate  ctype:dmmeta.Ctype
acr_nav.SetView  viewmode:xref
acr_nav.Navigate  ctype:dmmeta.Field
acr_nav.GoBack
acr_nav.Summary
```

## Cross-Validation

Verify headless output against the ssimfile database:
```bash
# Count fields for a ctype
acr dmmeta.field -where:ctype:dmmeta.Ctype 2>/dev/null | wc -l

# Count reverse references (xrefs)
acr dmmeta.field -where:arg:dmmeta.Ctype 2>/dev/null | wc -l

# Discover acr_nav's full capability surface
acr acr_navdb.% 2>/dev/null
```


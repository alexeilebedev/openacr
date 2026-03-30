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
- `codegen` (has_fields:N) — amc-generated C++ struct. Tab next: fields.
- `help` (has_fields:N) — keybinding help. Overlay, toggled by `?`.
- `detail` (has_fields:N) — per-field metadata cards. Overlay, toggled by `d`.

`has_fields:Y` viewmodes emit `VisibleField` records. `has_fields:N` emit `VisibleLine` records. Tab cycles the non-overlay chain: fields -> xref -> preview -> codegen -> fields. Help and detail are overlays pushed/popped on `viewmode_stack` (stacking is possible: `d` during help pushes detail on top).

## Input Records

```
# Send a key event
acr_nav.SendKey  key:<Smallstr50>

# Request state dump
acr_nav.Screenshot

# Set terminal dimensions (affects pagination)
acr_nav.SetTermSize  term_hei:<i32>  term_wid:<i32>
```

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

## Sample Session

A simple test sending an unknown key and verifying it doesn't change state (first 5 lines of output):

Input: `acr_nav.SendKey  key:F13` then EOF.

Output:
```
acr_nav.Screen  mode:browse  focus:ctype_list  filter:""  navstack_depth:0  n_sel_ctype:1403  n_ctype:1404  n_field:5631  viewmode:help  breadcrumb:""  filtertarget:ctype
acr_nav.PanelState  panel:ctype_list  sel_row:0  scroll_offset:0  n_items:83  sel_value:""
acr_nav.VisibleLeftItem  row:0  value:""  kind:ns  collapsed:Y  n_match:44  n_record:0
acr_nav.VisibleLeftItem  row:1  value:abt  kind:ns  collapsed:Y  n_match:19  n_record:0
...83 VisibleLeftItem rows (one per namespace, all collapsed)...
acr_nav.PanelState  panel:content  sel_row:0  scroll_offset:0  n_items:0  sel_value:""
(blank line)
```

Key observation: `viewmode:help` is preserved — the unknown key did not dismiss the startup help.

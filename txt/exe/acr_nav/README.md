## acr_nav - TUI schema explorer for browsing ctypes, fields, and cross-references
<a href="#acr_nav"></a>
`acr_nav` is an interactive terminal tool for browsing ctypes, fields, and cross-references.
Two-panel layout: ctype list (left), fields for the selected ctype (right).
Navigate with arrow keys, press Enter to follow a field reference to its target ctype,
press Backspace to go back. Press `/` to filter ctypes by name.

When stdout is not a TTY or when `-headless` is specified, `acr_nav` enters headless mode:
it reads structured commands (`acr_nav.SendKey`, `acr_nav.Screenshot`) from stdin and emits
structured screen state records on stdout. This enables agent-driven testing without a terminal.

### Table Of Contents
<a href="#table-of-contents"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Toc -->
&nbsp;&nbsp;&bull;&nbsp;  [Syntax](#syntax)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Key Bindings](#key-bindings)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Design](#design)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Options](#options)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
&#128196; [acr_nav - Internals](/txt/exe/acr_nav/internals.md)<br/>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Toc -->

### Syntax
<a href="#syntax"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Syntax -->
```
acr_nav: TUI schema explorer for browsing ctypes, fields, and cross-references
Usage: acr_nav [options]
    OPTION      TYPE    DFLT    COMMENT
    -headless                   Headless mode: structured I/O for agent testing
    -in         string  "data"  Input directory or filename, - for stdin
    -verbose    flag            Verbosity level (0..255); alias -v; cumulative
    -debug      flag            Debug level (0..255); alias -d; cumulative
    -help                       Print help and exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig

```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Syntax -->

### Key Bindings
<a href="#key-bindings"></a>
Key bindings are data-driven records in `data/acr_navdb/keybind.ssim`.
Query them with `acr acr_navdb.keybind`.

**Browse mode:**

| Key | Action |
|-----|--------|
| Up / k | Move selection up |
| Down / j | Move selection down |
| PgUp | Page up |
| PgDown | Page down |
| Left | Switch to left panel |
| Right | Switch to right panel |
| Enter | Follow field reference to target ctype |
| Backspace | Go back (pop navigation stack) |
| / | Enter filter mode |
| q | Quit |

**Filter mode:**

| Key | Action |
|-----|--------|
| (any printable) | Append to filter string |
| Backspace | Delete last filter character |
| Escape | Cancel filter and return to browse mode |
| Enter | Follow field reference |
| Up / Down | Move selection |
| PgUp / PgDown | Page up / down |
| Left / Right | Switch panel |

### Design
<a href="#design"></a>
The tool's own schema is the design. Run `amc_vis acr_nav.%` to see the access path diagram.
UI state is modeled as ssimfile records:

- `acr_navdb.navmode` — interaction modes (browse, filter)
- `acr_navdb.panel` — panel layout (ctype_list, field_list)
- `acr_navdb.navaction` — navigation actions (move_up, follow_ref, etc.)
- `acr_navdb.keybind` — mode-qualified key bindings dispatched via gstatic Hook

Actions are dispatched through function pointers (`step` Hook on `FNavaction`),
not string if-chains. Adding a new action requires one `navaction` record,
one `keybind` record, and one C++ handler function.

### Options
<a href="#options"></a>

<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Options -->
#### -headless -- Headless mode: structured I/O for agent testing
<a href="#-headless"></a>

#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Options -->

### Inputs
<a href="#inputs"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Inputs -->
`acr_nav` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|
|[dmmeta.ctype](/txt/ssimdb/dmmeta/ctype.md)|Struct|
|[dmmeta.field](/txt/ssimdb/dmmeta/field.md)|Specify field of a struct|
|[acr_navdb.keybind](/txt/ssimdb/acr_navdb/keybind.md)|Key-to-action mapping for acr_nav|
|[acr_navdb.navmode](/txt/ssimdb/acr_navdb/navmode.md)|UI mode for acr_nav|
|[acr_navdb.navstyle](/txt/ssimdb/acr_navdb/navstyle.md)||
|[dmmeta.ns](/txt/ssimdb/dmmeta/ns.md)|Namespace (for in-memory database, protocol, etc)|
|[acr_navdb.panel](/txt/ssimdb/acr_navdb/panel.md)|Panel definition for acr_nav TUI layout|
|[dmmeta.reftype](/txt/ssimdb/dmmeta/reftype.md)|Field type constructor (e.g. reference type)|
|[acr_navdb.reftypestyle](/txt/ssimdb/acr_navdb/reftypestyle.md)||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Inputs -->


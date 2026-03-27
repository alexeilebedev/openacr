# Agent-Terminal: Headless mode for acr_nav

## Context

M9 introduced field coloring and navigation markers for acr_nav. After testing, 4 issues surfaced — all were visually obvious but undetectable by an AI agent that never saw the running program. The agent built plausible code that compiled and passed batch tests, but produced wrong visual results.

The root cause: **no feedback loop between the AI agent and the rendered TUI.** The agent couldn't run acr_nav, look at it, and judge whether the result matched the design intent.

This plan adds a `-headless` mode to acr_nav — the terminal equivalent of Vercel's agent-browser. The agent sends keystrokes, reads structured screen state, and reasons about what it sees. ~270 tokens per screenshot. Full semantic information for judgment.

## The Key Insight

The screen IS an in-memory database state. Every visible element — selected ctype, field names, colors, navigability — already exists as queried data inside Render(). Headless mode makes that data available as ssimfile records instead of ANSI escape sequences.

## Drop `-ns` flag

The `-ns` command-line flag is superseded by the interactive `/` filter and causes the M9 Issue 2 bug (namespace blocking navigation to loaded ctypes). Drop it entirely:

**Remove from schema:**
- `data/dmmeta/field.ssim` — delete `command.acr_nav.ns` record
- `data/dmmeta/anonfld.ssim` — delete `command.acr_nav.ns` record

**Remove from code** (`cpp/acr_nav/acr_nav.cpp`):
- Line 89: delete `bool ns_match = ...` check in ApplyFilter(), change condition to just `if (filter_match)`
- Lines 317-318: delete `algo_lib::Regx_Match(acr_nav::_db.cmdline.ns, ...)` check in follow_ref(), simplify to `fld && fld->p_arg != sel_ct`

**Remove old tests:**
- `data/atfdb/comptest.ssim` — delete 3 records (BatchConfig, BatchSelf, EmptyResult)
- `data/atfdb/targs.ssim` — delete 3 records
- `test/atf_comp/acr_nav.BatchConfig`, `acr_nav.BatchSelf`, `acr_nav.EmptyResult` — delete files

**Remove `BatchOutput()`** — the function at line 109 is deleted (replaced by headless auto-screenshot).

This eliminates M9 Issues 2 and 3 in one stroke — follow_ref no longer checks namespace, and there's no namespace to check.

## Protocol

**Input** — line-oriented ssimfile commands on stdin:
```
acr_nav.send_key  key:j
acr_nav.send_key  key:Enter
acr_nav.screenshot
```

`send_key` feeds into the existing keybind+navaction dispatch — no new dispatch table needed. `screenshot` is the only headless-specific operation. Unrecognized lines are silently ignored (consistent with `InsertStrptrMaybe` convention). `send_key key:q` sets `_db.running=false`, ending the loop.

**Output** — structured records on stdout per screenshot (or on EOF):
```
acr_nav.screen  mode:browse  focus:ctype_list  filter:""  navstack_depth:0  n_sel_ctype:1384  n_ctype:1384  n_field:5503
acr_nav.panel_state  panel:ctype_list  sel_row:0  scroll_offset:0  n_items:1384  sel_value:abt_md.Aline
acr_nav.panel_state  panel:field_list  sel_row:0  scroll_offset:0  n_items:4  sel_value:""
acr_nav.visible_field  row:0  field:abt_md.Aline.aline  arg:algo.Smallstr200  reftype:Val  style:field_val  navigable:N
acr_nav.visible_field  row:1  field:abt_md.Aline.p_abt_md  arg:abt_md.Abt_md  reftype:Upptr  style:field_nav  navigable:Y
```

Blank line terminates each screenshot block. Agent controls when to screenshot (send 5 filter keys, then 1 screenshot = token-efficient). On EOF, always emits a final auto-screenshot.

**Style vocabulary** (for agent reasoning about field colors):
- `field_val` — data value fields (Val, Smallstr, Bitfld, etc.)
- `field_nav` — upward reference fields (Pkey, Upptr, Base, Regx, RegxSql)
- `field_idx` — index/pool fields (Thash, Llist, Bheap, Atree, Ptrary, Lary, Tpool, etc.)

**Note on filter mode:** `filter_cancel` (Escape) clears the filter text. There is no `filter_accept` action. To observe filtered results, take the screenshot *while still in filter mode* (before Escape). The `mode:filter` value in the Screen record tells the agent it's in filter mode.

## How This Catches M9-Style Bugs

| M9 Bug | What agent sees in headless output |
|--------|-------------------------------------|
| Wrong color categories | `style:field_nav` on a Lary field — Lary is a pool, not a nav reference |
| Missing nav markers | `navigable:N` + `style:field_nav` — color says navigable, field says not |
| Navigation blocked | `follow_ref` → screenshot shows `sel_value` unchanged — navigation failed |
| Sub-format in color encoding | Output uses `style:field_val` (single name), not 3 booleans — the format itself avoids sub-formats |

## Implementation Steps

### Step 1: Drop `-ns` flag + BatchOutput + old tests

**Schema deletions:**
- `data/dmmeta/field.ssim` — delete `command.acr_nav.ns` record
- `data/dmmeta/anonfld.ssim` — delete `command.acr_nav.ns` record
- `data/atfdb/comptest.ssim` — delete 3 records (BatchConfig, BatchSelf, EmptyResult)
- `data/atfdb/targs.ssim` — delete 3 records

**Code deletions** (`cpp/acr_nav/acr_nav.cpp`):
- Delete `BatchOutput()` function (lines 109-130)
- Line 89: remove `ns_match` variable and its use in the condition
- Lines 317-318: simplify follow_ref to `fld && fld->p_arg != sel_ct`

**File deletions:**
- `test/atf_comp/acr_nav.BatchConfig`
- `test/atf_comp/acr_nav.BatchSelf`
- `test/atf_comp/acr_nav.EmptyResult`

Run `amc` to regenerate (removes generated ns field code).

### Step 2: Add `-headless` flag (schema only)

Add one field record:
```
dmmeta.field  field:command.acr_nav.headless  arg:bool  reftype:Val  dflt:""  comment:"Headless mode: structured I/O for agent testing"
```

Run `amc` to regenerate.

### Step 3: Add output/input record ctypes (schema only)

Add 3 output ctypes with fields + cfmt (print:Y, printfmt:Tuple):
- `acr_nav.Screen` — mode, focus (`Pkey` → `acr_navdb.Panel`), filter, navstack_depth, n_sel_ctype, n_ctype, n_field
- `acr_nav.PanelState` — panel (`Pkey` → `acr_navdb.Panel`), sel_row, scroll_offset, n_items, sel_value
- `acr_nav.VisibleField` — row, field, arg, reftype, style, navigable

Add 2 input ctypes with cfmt (read:Y, printfmt:Tuple):
- `acr_nav.SendKey` — key
- `acr_nav.Screenshot` — (tag type, no extra fields)

`panel` and `focus` fields are `Pkey` references to `acr_navdb.Panel`.
`Screen` includes `n_ctype` and `n_field` (diagnostic stats).

Run `amc` to regenerate print/read functions.

### Step 4: Extract InitPanels(), ProcessKey(), implement HeadlessMain() and HeadlessOutput()

In `cpp/acr_nav/acr_nav.cpp`:

**`InitPanels()`** (~10 lines) — extract shared panel/mode initialization from Main() lines 586-599: panel lookups (`ind_panel_Find`), vrfy calls, mode setup (`ind_navmode_Find`, `SwitchToBrowse`), initial scroll positions. Excludes `DetectTerminal()`, `EnterRawMode()`, and initial `Render()` which are TUI-specific. Called by both interactive and headless paths.

**`ProcessKey(strptr key_name)`** (~17 lines) — extract the shared dispatch logic from the interactive main loop (lines 605-628). Internally:
1. Compose mode+key, look up keybind, dispatch via `step_Call`
2. Handle filter character accumulation (printable chars in filter mode)
3. Compute prev/current sel_ct internally (`SelectedCtype(*_db.p_left_panel)` before and after dispatch)
4. Reset right panel scroll if ctype changed
5. Call AdjustScroll for both panels
6. Return bool indicating whether state changed

Both the interactive loop and HeadlessMain() call ProcessKey(). Adding a navaction requires zero loop changes.

**Interactive loop becomes:**
```cpp
while (_db.running) {
    tempstr key_name = ReadKeyName();
    if (ProcessKey(key_name)) {
        Render(SelectedCtype(*left), left, right);
    }
}
```

**`HeadlessOutput()`** (~40 lines) — constructs Screen, PanelState, VisibleField records from current state and prints via `prlog`. Emits all fields of the selected ctype (not viewport-constrained). Field style comes from `fld->p_reftype->c_reftypestyle->p_navstyle->navstyle`.

**Navigability check:** `fld->p_arg != sel_ct` (target differs from current ctype). With `-ns` gone, there's no namespace filter to interfere.

**`HeadlessMain()`** (~40 lines):
1. Call `InitPanels()`
2. Set `_db.term_hei = 100000` — disables scroll windowing (AdjustScroll line 242 and page_up/page_down lines 274/285 use `term_hei - 2` for visible rows and page size; default 0 would give `visible=1` and page size `-2`)
3. Read stdin line by line while `_db.running`:
   - Try parsing as SendKey → call `ProcessKey(key)`
   - Try parsing as Screenshot → call `HeadlessOutput()`
   - Otherwise silently ignore (consistent with `InsertStrptrMaybe` pattern)
4. On EOF: always emit a final auto-screenshot

**Modify `Main()`** — replace the batch/interactive split:
```cpp
bool headless = _db.cmdline.headless || !isatty(STDOUT_FILENO);
if (headless) {
    HeadlessMain();
} else {
    InitPanels();
    DetectTerminal();
    EnterRawMode();
    // ... existing interactive code, now using ProcessKey()
    ExitRawMode();
}
```

**Remove `BatchOutput()`** — fully replaced by HeadlessMain + auto-screenshot-on-EOF.

**Files modified:** `cpp/acr_nav/acr_nav.cpp`

### Step 5: Add headless tests

4 new component tests using `send_key key:/` for filtering (not `-ns`):
- `acr_nav.HeadlessInit` — auto-screenshot on empty stdin, verify initial state (all ctypes loaded)
- `acr_nav.HeadlessNav` — navigate down/right, verify panel state changes
- `acr_nav.HeadlessFilter` — `send_key key:/` + type filter text + screenshot (while still in filter mode) + `send_key key:Escape`, verify n_sel_ctype decreased in the screenshot
- `acr_nav.HeadlessFollow` — follow a reference, verify navstack_depth and sel_value

**Files modified:** `data/atfdb/comptest.ssim`, `data/atfdb/targs.ssim`, `test/atf_comp/acr_nav.*`

### Step 6: Update README + build and verify

**Update** `txt/exe/acr_nav/README.md` — hand-written text about pipe behavior (lines 8-9) and `-ns` option needs manual update. Auto-generated sections (Syntax, Options) update via `abt_md` after `amc`.

```bash
amc && ai
acr_nav -headless < /dev/null
# Should output structured records for all loaded ctypes
echo -e "acr_nav.send_key  key:/\nacr_nav.send_key  key:a\nacr_nav.send_key  key:m\nacr_nav.send_key  key:c\nacr_nav.screenshot" | acr_nav -headless
# Should show n_sel_ctype < 1384 (filtered, screenshot taken while in filter mode)
normalize comp
```

## Critical Files

| File | Role |
|------|------|
| `cpp/acr_nav/acr_nav.cpp:578` | Main() — add headless branch, extract InitPanels() + ProcessKey() |
| `cpp/acr_nav/acr_nav.cpp:435` | Render() — reference for what HeadlessOutput mirrors |
| `cpp/acr_nav/acr_nav.cpp:309` | follow_ref — simplify (remove ns check) |
| `cpp/acr_nav/acr_nav.cpp:241` | AdjustScroll — uses term_hei (must be set in headless) |
| `cpp/acr_nav/acr_nav.cpp:80` | ApplyFilter — remove ns check |
| `data/dmmeta/field.ssim` | Schema: delete ns, add headless flag + output record fields |
| `data/dmmeta/ctype.ssim` | Schema: Screen, PanelState, VisibleField, SendKey, Screenshot |
| `data/dmmeta/cfmt.ssim` | Print/read format declarations |
| `data/atfdb/comptest.ssim` | Delete old tests, add new headless tests |
| `txt/exe/acr_nav/README.md` | Update pipe behavior description, remove -ns docs |

## Design Decisions

**Why drop `-ns` entirely?** It's superseded by the interactive `/` filter (which you can change at runtime). It caused M9 Issue 2 (blocking navigation to loaded ctypes). Headless tests use `send_key key:/` to exercise filtering, which also tests the filter path instead of bypassing it.

**Why set term_hei to 100000 in headless?** AdjustScroll (line 242) and page_up/page_down (lines 274/285) use `term_hei - 2`. With default 0, `visible=1` and page size is `-2`, breaking scroll behavior. A large sentinel effectively disables scroll windowing — headless outputs all fields regardless of viewport.

**Why not auto-screenshot after every key?** Token efficiency. Agent entering a 5-char filter needs 5 keys but 1 screenshot. Agent controls when to look.

**Why always auto-screenshot on EOF?** If the agent sends keys but forgets a trailing screenshot, silent exit with no output is surprising. Always emitting a final screenshot is cheap and ensures piped usage always produces output.

**Why extract InitPanels()?** The interactive path (lines 586-599) has 10 lines of panel/mode setup. HeadlessMain needs the same setup minus DetectTerminal/EnterRawMode/Render. Without extraction, both paths duplicate 10 lines that must stay in sync.

**Why does ProcessKey() internalize sel_ct tracking?** The interactive loop (lines 608, 621-625) tracks prev_sel_ct vs sel_ct to reset the right panel on ctype change. If ProcessKey doesn't handle this internally, both callers need the same 5-line post-dispatch block — defeating the extraction.

**Why not an external pty wrapper?** A pty wrapper reverse-engineers ANSI escapes back into structure — lossy, fragile, and can't distinguish semantic styles (field_nav vs coincidental cyan). HeadlessOutput() has direct access to the semantic data.

**Why ssimfile tuples for input?** Consistency with OpenACR patterns. `acr_nav.send_key  key:j` is self-documenting. amc-generated parsers make input handling correct by construction.

**Why not a dispatch table for headless commands?** There are exactly 2 protocol verbs: input (send_key feeds into existing keybind dispatch) and output (screenshot). Two fixed verbs don't form a growing category. If a third verb ever appears, factor then.

## Verification

1. `acr_nav -headless < /dev/null` — auto-screenshot showing all loaded ctypes
2. `echo "acr_nav.screenshot" | acr_nav -headless` — explicit screenshot, same result
3. `echo -e "acr_nav.send_key  key:j\nacr_nav.screenshot" | acr_nav -headless` — should show sel_row:1
4. `echo "acr_nav.send_key  key:j" | acr_nav -headless` — auto-screenshot on EOF shows sel_row:1
5. Filter test: `send_key key:/` + chars + screenshot (in filter mode) — verify n_sel_ctype decreased
6. Follow test: navigate to a field with `navigable:Y`, `send_key key:Enter`, screenshot — verify sel_value changed and navstack_depth:1
7. `normalize comp` — all new tests pass
8. `acr -check` — referential integrity after schema changes

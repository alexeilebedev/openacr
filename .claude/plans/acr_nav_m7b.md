# Plan: acr_nav M7b — Naventry filter state + keybind substr validation [DONE]

## Context

acr_nav M1-M7a are complete. Two schema gaps were identified during implementation:

1. **Naventry loses filter state** — following a ref (Enter) while filtered, then going back (Backspace), doesn't restore the filter. Naventry only stores ctype_name/scroll_offset/sel_row — not filter text or navmode.

2. **Keybind composite keys lack referential integrity** — keys like `browse.Up` embed a navmode prefix (`browse`) without validation against the navmode table. Adding `dmmeta.substr` decomposition lets `acr -check` enforce this.

Both are pure schema evolution — no new features, just making the data model honest.

## Part 1: Naventry filter state

### Schema: add 2 fields to `data/dmmeta/field.ssim`

Insert in sorted position (after `ctype_name`, before `scroll_offset`):

```
dmmeta.field  field:acr_nav.Naventry.filter  arg:algo.cstring  reftype:Val  dflt:""  comment:"Filter text at time of push"
dmmeta.field  field:acr_nav.Naventry.navmode  arg:algo.Smallstr50  reftype:Val  dflt:""  comment:"Navigation mode at time of push"
```

### Code: `cpp/acr_nav/acr_nav.cpp`

**navaction_follow_ref() (~line 289)** — add 2 lines after existing assignments:

```cpp
entry.filter = acr_nav::_db.filter;
entry.navmode = acr_nav::_db.p_cur_mode->navmode;
```

**navaction_go_back() (~line 306)** — restore filter+navmode before sel_row (sel_row indexes the filtered list):

```cpp
void acr_nav::navaction_go_back() {
    if (!acr_nav::navstack_EmptyQ()) {
        acr_nav::Naventry *entry = acr_nav::navstack_Last();
        acr_nav::_db.filter = entry->filter;
        acr_nav::FNavmode *mode = acr_nav::ind_navmode_Find(entry->navmode);
        if (mode) {
            acr_nav::_db.p_cur_mode = mode;
        }
        ApplyFilter();
        acr_nav::_db.p_left_panel->scroll_offset = entry->scroll_offset;
        acr_nav::_db.p_left_panel->sel_row = entry->sel_row;
        acr_nav::navstack_RemoveLast();
    }
}
```

**Critical ordering:** ApplyFilter() before restoring sel_row — sel_row is an index into the filtered list.

## Part 2: Keybind substr validation

### Schema: add 1 field to `data/dmmeta/field.ssim`

Insert in sorted position (between `navaction` and `comment`, since `navm` > `nava`):

```
dmmeta.field  field:acr_navdb.Keybind.navmode  arg:acr_navdb.Navmode  reftype:Pkey  dflt:""  comment:"Navigation mode prefix"
```

### Schema: add 1 substr to `data/dmmeta/substr.ssim`

Insert in sorted position:

```
dmmeta.substr  field:acr_navdb.Keybind.navmode  expr:.RL  srcfield:acr_navdb.Keybind.keybind
```

`.RL` = find dot from right, take left part. `browse.Up` -> `browse`. Pkey reftype validates against navmode table.

No xref needed — runtime keybind dispatch constructs composite keys directly (`cpp/acr_nav/acr_nav.cpp:524`), no need to navigate keybind->navmode.

## Part 3: Test updates

All 3 existing tests need expected output regeneration:

- **BatchSelf**: Naventry goes from 3 to 5 fields; n_field 5501 -> 5504
- **BatchConfig**: Keybind goes from 3 to 4 fields; n_field 5501 -> 5504
- **EmptyResult**: n_field 5501 -> 5504

Regenerate with `atf_comp acr_nav.% -capture`.

## Execution sequence

1. Edit `data/dmmeta/field.ssim` — add 3 field records
2. Edit `data/dmmeta/substr.ssim` — add 1 substr record
3. `amc` — regenerate
4. `acr -check` — validate referential integrity
5. Edit `cpp/acr_nav/acr_nav.cpp` — follow_ref (+2 lines), go_back (rewrite ~8 lines)
6. `abt -build acr_nav` — build
7. `atf_comp acr_nav.% -capture` — regenerate test expectations
8. `atf_comp acr_nav.%` — verify tests pass
9. Update `txt/ideas.md` — mark M7b done, note any remaining open items

## Files to modify

- `data/dmmeta/field.ssim` — 3 new records
- `data/dmmeta/substr.ssim` — 1 new record
- `cpp/acr_nav/acr_nav.cpp` — follow_ref and go_back
- `test/atf_comp/acr_nav.BatchSelf` — regenerate
- `test/atf_comp/acr_nav.BatchConfig` — regenerate
- `test/atf_comp/acr_nav.EmptyResult` — regenerate
- `txt/ideas.md` — update open items

## Verification

- `acr -check` passes (especially keybind navmode validation)
- `amc` runs clean
- `abt -build acr_nav` compiles
- `atf_comp acr_nav.%` — all 3 tests pass
- `amc_vis acr_nav.%` — clean diagram
- Manual: interactive test of filter-then-navigate-then-backspace restoring filter

## Known limitations (out of scope, verified)

- `Naventry.ctype_name` is written (line 290) but never read — `go_back` uses `sel_row` to locate the ctype. Harmless dead data; could be removed or used for robustness in a future change.
- Whether `follow_ref` should switch to browse mode after following from filter mode is a UX design question. M7b's filter save/restore makes the round-trip correct either way.
- `p_cur_panel` does NOT need save/restore — navstack push only happens from the right panel (position==1), so panel focus is always correct after pop. (Verified: not a limitation.)

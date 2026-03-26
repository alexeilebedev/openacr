# acr_nav — TUI Schema Explorer: M1 + M2 Plan

## Context

Building `acr_nav`, an interactive terminal tool for browsing ctypes, fields, and xrefs.
Branch `acr-nav` exists with `txt/ideas.md` committed. M0 exploration is complete.

---

## /acr Review

### Verified against amc_vis

1. **Core wrapper types match amc_vis exactly.** FCtype(Base:dmmeta.Ctype), FField(Base:dmmeta.Field), FReftype — same pattern, same xref wiring. Via paths verified:
   - `FCtype.c_field via ind_ctype/dmmeta.Field.ctype` — identical to amc_vis
   - `FField.p_arg via ind_ctype/dmmeta.Field.arg` — identical to amc_vis
   - `FCtype.p_ns via ind_ns/dmmeta.Ctype.ns` — **new for acr_nav** (amc_vis has no FNs). Valid: dmmeta.Ctype.ns is Pkey substr, expr:.RL

2. **zd_sel_ctype inscond:false** — matches amc_vis.FDb.zd_select exactly.

4. **filter_accept has no keybind** — intentional. Enter→follow_ref in browse mode and Enter→filter_accept in filter mode collide on the same pkey. Mode-dependent keybind dispatch (M6) will resolve this. Until then, C++ code in M3 hardcodes "Enter in filter mode = filter_accept."

3. **Failed xref = silent rejection** — verified in amc_vis_gen.cpp. Fields with unresolvable p_arg are skipped.

### Design decisions

1. **navaction as controlled vocabulary** — 11 actions = 11 records. `acr -check` validates every keybind reference. Foundation for M6 gstatic dispatch.

2. **Panel as data, not code.** Each panel is a record in `acr_navdb.panel` with title, position, width. Per-panel state (scroll_offset, sel_row) lives on FPanel. Adding a third panel = one record, zero code.

3. **Separate acr_navdb ssimdb** — follows gclidb pattern. Four config tables: navaction, keybind, panel, navmode.

4. **navmode as controlled vocabulary.** Browse and filter are records, not a bool. Adding a third mode = one record, zero code.

5. **`/` as keybind key** — ssimfile field values can contain `/`. No "slash" indirection needed.

6. **navaction gstatic dispatch deferred to M6** — tracked in `txt/ideas.md` milestones. The absence of `dmmeta.gstatic field:acr_nav.FDb.navaction` in the schema is self-documenting. M6 adds the gstatic record + userfunc records + C++ handlers.

---

## M1 — Config Tables (Zero C++)

### M1.1: Create acr_navdb ssimdb

```bash
acr_ed -create -target acr_navdb -nstype ssimdb -comment "Configuration tables for acr_nav TUI schema explorer" -write
```

### M1.2: Create navaction controlled vocabulary

```bash
acr_ed -create -ssimfile acr_navdb.navaction -subset algo.Smallstr50 -comment "Controlled vocabulary of navigation actions" -write
```

Populate `data/acr_navdb/navaction.ssim`:

```
acr_navdb.navaction  navaction:filter_accept   comment:"Accept current filter text"
acr_navdb.navaction  navaction:filter_cancel    comment:"Cancel filter input"
acr_navdb.navaction  navaction:filter_start     comment:"Enter filter input mode"
acr_navdb.navaction  navaction:follow_ref       comment:"Follow reference to target ctype"
acr_navdb.navaction  navaction:go_back          comment:"Return to previous ctype"
acr_navdb.navaction  navaction:move_down        comment:"Move selection down"
acr_navdb.navaction  navaction:move_up          comment:"Move selection up"
acr_navdb.navaction  navaction:page_down        comment:"Move selection down one page"
acr_navdb.navaction  navaction:page_up          comment:"Move selection up one page"
acr_navdb.navaction  navaction:quit             comment:"Exit acr_nav"
acr_navdb.navaction  navaction:switch_panel     comment:"Switch focus between panels"
```

### M1.3: Create panel definition table

```bash
acr_ed -create -ssimfile acr_navdb.panel -subset algo.Smallstr50 -comment "Panel definition for acr_nav TUI layout" -write
acr_ed -create -field acr_navdb.Panel.title -arg algo.cstring -comment "Display title" -write
acr_ed -create -field acr_navdb.Panel.position -arg i32 -comment "Left-to-right ordering" -write
acr_ed -create -field acr_navdb.Panel.width_pct -arg i32 -comment "Width as percentage of terminal" -write
```

Populate `data/acr_navdb/panel.ssim`:

```
acr_navdb.panel  panel:ctype_list  title:Types   position:0  width_pct:40  comment:""
acr_navdb.panel  panel:field_list  title:Fields  position:1  width_pct:60  comment:""
```

### M1.4: Create navmode controlled vocabulary

```bash
acr_ed -create -ssimfile acr_navdb.navmode -subset algo.Smallstr50 -comment "UI mode for acr_nav" -write
```

Populate `data/acr_navdb/navmode.ssim`:

```
acr_navdb.navmode  navmode:browse  comment:"Normal navigation mode"
acr_navdb.navmode  navmode:filter  comment:"Filter input mode"
```

### M1.5: Create keybind table

```bash
acr_ed -create -ssimfile acr_navdb.keybind -subset algo.Smallstr50 -comment "Key-to-action mapping for acr_nav" -write
acr_ed -create -field acr_navdb.Keybind.navaction -arg acr_navdb.Navaction -reftype Pkey -comment "Navigation action" -write
```

Populate `data/acr_navdb/keybind.ssim`:

```
acr_navdb.keybind  keybind:/          navaction:filter_start   comment:""
acr_navdb.keybind  keybind:Backspace  navaction:go_back        comment:""
acr_navdb.keybind  keybind:Down       navaction:move_down      comment:""
acr_navdb.keybind  keybind:Enter      navaction:follow_ref     comment:""
acr_navdb.keybind  keybind:Escape     navaction:filter_cancel  comment:""
acr_navdb.keybind  keybind:Left       navaction:switch_panel   comment:""
acr_navdb.keybind  keybind:PgDown     navaction:page_down      comment:""
acr_navdb.keybind  keybind:PgUp       navaction:page_up        comment:""
acr_navdb.keybind  keybind:Right      navaction:switch_panel   comment:""
acr_navdb.keybind  keybind:Up         navaction:move_up        comment:""
acr_navdb.keybind  keybind:q          navaction:quit           comment:""
```

### M1.6: Verify M1

```bash
acr -check          # Every keybind.navaction must exist; all references valid
acr acr_navdb.%     # See all records (11 navaction + 2 panel + 2 navmode + 11 keybind = 26)
```

---

## M2 — Scaffold Exe + Core Schema

### M2.1: Create acr_nav exe target

```bash
acr_ed -create -target acr_nav -comment "TUI schema explorer for browsing ctypes, fields, and cross-references" -write
acr_ed -create -field command.acr_nav.ns -arg dmmeta.Ns -reftype RegxSql -dflt '"%"' -anon -comment "Namespace filter regex" -write
```

### M2.2: Add finput tables from dmmeta

```bash
acr_ed -create -finput -target acr_nav -ssimfile dmmeta.ctype -indexed -write
acr_ed -create -finput -target acr_nav -ssimfile dmmeta.field -indexed -write
acr_ed -create -finput -target acr_nav -ssimfile dmmeta.ns -indexed -write
acr_ed -create -finput -target acr_nav -ssimfile dmmeta.reftype -indexed -write
```

### M2.3: Add finput tables from acr_navdb

```bash
acr_ed -create -finput -target acr_nav -ssimfile acr_navdb.navaction -indexed -write
acr_ed -create -finput -target acr_nav -ssimfile acr_navdb.keybind -indexed -write
acr_ed -create -finput -target acr_nav -ssimfile acr_navdb.panel -indexed -write
acr_ed -create -finput -target acr_nav -ssimfile acr_navdb.navmode -indexed -write
```

### M2.4: Add cross-reference fields

```bash
# FCtype.c_field — collect fields by parent ctype (identical to amc_vis)
acr_ed -create -field acr_nav.FCtype.c_field -arg acr_nav.FField -reftype Ptrary -via acr_nav.FDb.ind_ctype/dmmeta.Field.ctype -xref -write

# FCtype.p_ns — namespace back-pointer
acr_ed -create -field acr_nav.FCtype.p_ns -arg acr_nav.FNs -reftype Upptr -via acr_nav.FDb.ind_ns/dmmeta.Ctype.ns -xref -write

# FField.p_ctype — parent ctype back-pointer (identical to amc_vis)
acr_ed -create -field acr_nav.FField.p_ctype -arg acr_nav.FCtype -reftype Upptr -via acr_nav.FDb.ind_ctype/dmmeta.Field.ctype -xref -write

# FField.p_arg — target ctype for navigation (identical to amc_vis)
acr_ed -create -field acr_nav.FField.p_arg -arg acr_nav.FCtype -reftype Upptr -via acr_nav.FDb.ind_ctype/dmmeta.Field.arg -xref -write

# FField.p_reftype — reftype semantics (identical to amc_vis)
acr_ed -create -field acr_nav.FField.p_reftype -arg acr_nav.FReftype -reftype Upptr -via acr_nav.FDb.ind_reftype/dmmeta.Field.reftype -xref -write

# FKeybind.p_navaction — action for dispatch
acr_ed -create -field acr_nav.FKeybind.p_navaction -arg acr_nav.FNavaction -reftype Upptr -via acr_nav.FDb.ind_navaction/acr_navdb.Keybind.navaction -xref -write
```

Note: FNavmode has no xref fields at M2 — it's a standalone controlled vocabulary. At M6, mode-dependent keybind dispatch may add a keybind→navmode cross-reference.

### M2.5: Add UI state fields

```bash
# Naventry value type for navigation stack (no pkey, no ssimfile)
echo 'dmmeta.ctype ctype:acr_nav.Naventry comment:"Navigation stack entry"' | acr -insert -write
echo 'dmmeta.field field:acr_nav.Naventry.ctype_name arg:algo.cstring reftype:Val dflt:"" comment:"Ctype name"' | acr -insert -write
echo 'dmmeta.field field:acr_nav.Naventry.scroll_offset arg:i32 reftype:Val dflt:"" comment:""' | acr -insert -write
echo 'dmmeta.field field:acr_nav.Naventry.sel_row arg:i32 reftype:Val dflt:"" comment:""' | acr -insert -write

# Navigation stack
acr_ed -create -field acr_nav.FDb.navstack -arg acr_nav.Naventry -reftype Tary -comment "Navigation history" -write

# Filtered ctype list (inscond:false — manually managed)
acr_ed -create -field acr_nav.FDb.zd_sel_ctype -arg acr_nav.FCtype -reftype Llist -inscond false -xref -comment "Visible ctypes after filter" -write

# Per-panel runtime state (on FPanel, not FDb)
acr_ed -create -field acr_nav.FPanel.scroll_offset -arg i32 -comment "Scroll position" -write
acr_ed -create -field acr_nav.FPanel.sel_row -arg i32 -comment "Selected row" -write

# Current panel pointer
acr_ed -create -field acr_nav.FDb.p_cur_panel -arg acr_nav.FPanel -reftype Ptr -comment "Currently focused panel" -write

# Current mode pointer
acr_ed -create -field acr_nav.FDb.p_cur_mode -arg acr_nav.FNavmode -reftype Ptr -comment "Current UI mode (browse/filter)" -write

# Other UI state
acr_ed -create -field acr_nav.FDb.filter -arg algo.cstring -comment "Current filter text" -write
acr_ed -create -field acr_nav.FDb.running -arg bool -dflt true -comment "Event loop control" -write
acr_ed -create -field acr_nav.FDb.term_hei -arg i32 -comment "Terminal height" -write
acr_ed -create -field acr_nav.FDb.term_wid -arg i32 -comment "Terminal width" -write
```

### M2.6: Verify M2

```bash
acr -check                    # Referential integrity
amc                           # Generate code
abt -build -install acr_nav   # Build
amc_vis acr_nav.%             # Access path diagram
```

---

## Expected Access Path Diagram

```
FDb
  |- ctype (Lary) --> FCtype --> c_field (Ptrary) --> FField
  |    |- ind_ctype (Thash)        p_ns (Upptr) --> FNs
  |    +- zd_sel_ctype (Llist)
  |- field (Lary) --> FField --> p_ctype (Upptr) --> FCtype
  |    +- ind_field (Thash)     p_arg (Upptr) --> FCtype
  |                              p_reftype (Upptr) --> FReftype
  |- ns (Lary) --> FNs
  |    +- ind_ns (Thash)
  |- reftype (Lary) --> FReftype
  |    +- ind_reftype (Thash)
  |- panel (Lary) --> FPanel [scroll_offset, sel_row]
  |    +- ind_panel (Thash)
  |- navmode (Lary) --> FNavmode
  |    +- ind_navmode (Thash)
  |- p_cur_panel (Ptr) --> FPanel
  |- p_cur_mode (Ptr) --> FNavmode
  |- navaction (Lary) --> FNavaction
  |    +- ind_navaction (Thash)
  |- keybind (Lary) --> FKeybind --> p_navaction (Upptr) --> FNavaction
  |    +- ind_keybind (Thash)
  +- navstack (Tary) --> Naventry
```

---

## Deferred to M6 (tracked in txt/ideas.md)

- **navaction gstatic dispatch** — add `dmmeta.gstatic` + `dmmeta.userfunc` records + C++ handlers. The absence of the gstatic record in the schema is self-documenting.

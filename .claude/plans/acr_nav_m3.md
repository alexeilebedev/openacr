# acr_nav M3 — Main() that prints ctypes + fields [DONE]

## Context

M1 created config tables (navaction, keybind, panel, navmode) in `data/acr_navdb/`. M2 scaffolded the `acr_nav` exe with finputs from dmmeta + acr_navdb, xrefs (c_field, p_ns, p_ctype, p_arg, p_reftype, p_navaction), and UI state fields (navstack, zd_sel_ctype, panels, filter, etc.). The generated code compiles and runs a "Hello, World!" stub.

M3 is the **first hand-written C++**. It replaces the stub with a batch-mode function that filters ctypes by namespace regex, populates `zd_sel_ctype`, and prints each selected ctype with its fields. This proves the schema works and teaches: **cursor loops, hash lookups, Upptr traversal**.

No TUI yet — that's M4. M3 is pure text output.

---

## File to modify

**`cpp/acr_nav/acr_nav.cpp`** — the only change. No header modifications needed.

---

## Data loading (already done for us)

The generated `main()` (`cpp/gen/acr_nav_gen.cpp:3343`) calls:
1. `FDb_Init()` — initialize all Lary/Thash/Llist structures
2. `ReadArgv()` — parse command line, call `LoadTuplesMaybe(cmd.in, true)` which loads all 9 ssimfiles
3. `Main()` — our entry point; all data loaded, all xrefs wired

By the time `Main()` runs: ~1380 ctypes, ~5500 fields, 85 namespaces, 35 reftypes, 11 navactions, 11 keybinds, 2 panels, 2 navmodes are in memory.

---

## Implementation

Replace the current stub:
```cpp
void acr_nav::Main() {
    prlog("Hello, World!");
    acr_nav::MainLoop();
}
```

With four phases:

### Phase 1 — Populate zd_sel_ctype (cursor loop + regex)

```cpp
ind_beg(acr_nav::_db_ctype_curs, ctype, acr_nav::_db) {
    bool match = Regx_Match(acr_nav::_db.cmdline.ns, ns_Get(ctype));
    if (match) {
        zd_sel_ctype_Insert(ctype);
    }
} ind_end;
```

- `_db.cmdline.ns` is `algo_lib::Regx` with default `"%"` (match all)
- `ns_Get(ctype)` extracts namespace via `Pathcomp(ctype.ctype, ".RL")` → e.g. `"dmmeta"`
- `zd_sel_ctype` has `inscond:false` — must manually insert (no automatic xref insertion)
- Pattern: `amc_vis.cpp:435`

### Phase 2 — Print selected ctypes with fields (nested cursors + Upptr traversal)

```cpp
ind_beg(acr_nav::_db_zd_sel_ctype_curs, ctype, acr_nav::_db) {
    prlog(ctype.ctype << "  (" << c_field_N(ctype) << " fields)");
    ind_beg(acr_nav::ctype_c_field_curs, field, ctype) {
        tempstr out;
        out << "  " << name_Get(field);
        char_PrintNTimes(' ', out, i32_Max(1, 24 - ch_N(out)));  // pad to column 24
        out << field.p_arg->ctype;
        char_PrintNTimes(' ', out, i32_Max(1, 52 - ch_N(out)));  // pad to column 52
        out << field.p_reftype->reftype;
        prlog(out);
    } ind_end;
} ind_end;
```

- Outer: `_db_zd_sel_ctype_curs` walks the doubly-linked selection list
- Inner: `ctype_c_field_curs` walks the Ptrary `c_field`
- `name_Get(field)` → `Pathcomp(field.field, ".RR")` → field name only
- `field.p_arg->ctype` — **Upptr traversal** to target type name
- `field.p_reftype->reftype` — **Upptr traversal** to reftype name
- `char_PrintNTimes` for column alignment (standard algo pattern)

### Phase 3 — Summary report (count functions)

```cpp
prlog("acr_nav.report"
      << Keyval("n_ctype", ctype_N())
      << Keyval("n_field", field_N())
      << Keyval("n_sel_ctype", zd_sel_ctype_N())
      << Keyval("n_keybind", keybind_N())
      << Keyval("n_panel", panel_N())
      << Keyval("n_navaction", navaction_N())
      << Keyval("n_navmode", navmode_N()));
```

Proves all finput chains (M1 config + M2 dmmeta) loaded successfully.

### Note: Remove MainLoop() call

`MainLoop()` is the event loop — only needed from M4 onward. M3 is batch mode.

---

## Expected output

```
$ acr_nav dmmeta
dmmeta.Ctype  (2 fields)
  ctype                   algo.Smallstr100            Val
  comment                 algo.Comment                Val
dmmeta.Field  (5 fields)
  field                   algo.Smallstr100            Val
  arg                     algo.Smallstr100            Val
  reftype                 algo.Smallstr50             Val
  dflt                    algo.CppExpr                Val
  comment                 algo.Comment                Val
...
acr_nav.report  n_ctype:1380  n_field:5498  n_sel_ctype:~200  n_keybind:11  n_panel:2  n_navaction:11  n_navmode:2
```

---

## Verification

```bash
abt -build -install acr_nav     # Must compile clean
acr_nav                          # All namespaces — large output, pipe to head
acr_nav dmmeta | head -40        # Single namespace — readable
acr_nav "acr%"                   # Regex filter — acr, acr_nav, acr_in, etc.
acr_nav nonexistent              # Zero matches — summary shows n_sel_ctype:0
acr -check                       # Referential integrity still passes
```

Spot-check:
- Fields of `dmmeta.Ctype` show `algo.Smallstr100` and `algo.Comment` args
- Summary shows `n_keybind:11 n_panel:2 n_navaction:11 n_navmode:2` (M1 config)
- No crashes on empty selection

---

## Notes

- `char_PrintNTimes(char c, cstring &out, int n)` — verified in `include/algo.h:806`, widely used in codebase including `amc_vis.cpp`
- `i32_Max(1, ...)` ensures at least one space even for long names

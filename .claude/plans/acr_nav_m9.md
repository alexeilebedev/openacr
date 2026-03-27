# Color-code fields by reftype + resolve hardcoded styles

## Context

acr_nav displays fields in the right panel as `[name] [ctype] [reftype]` with no visual distinction by reftype. All 35 reftypes render identically. Meanwhile, 18+ raw ANSI escape sequences are hardcoded throughout `cpp/acr_nav/acr_nav.cpp` for title bars, selection highlights, and the status bar. Both problems violate "everything is data" — style parameters should live in records, not in code.

## Design

Two new ssimfile tables, both in the existing `acr_navdb` namespace:

### Table 1: `acr_navdb.navstyle` — Named terminal styles

A controlled vocabulary of named terminal attribute bundles. Each record carries individual boolean ANSI attributes and RGB foreground bits. One function converts any navstyle record to an ANSI escape sequence — no switch, no code branches.

This table serves both UI chrome styles (title_focus, sel_nofocus, statusbar) and field color categories (field_nav, field_idx, field_val). These share the same attribute schema and are accessed via different paths: UI chrome by name lookup (`ind_navstyle_Find`), field colors by xref chain from FReftype. One noun — "named terminal attribute bundle" — two access paths.

**Design choice — booleans vs bitmask:** The existing `algo::TermStyle` is a u32 bitmask with `fconst` values for red/green/blue/bold. We use individual booleans instead because: (a) self-documenting in ssimfiles (`bold:Y reverse:Y` vs `style:40`), (b) fully contained in acr_navdb — no changes to the shared `algo` protocol namespace, (c) follows the standard /acr pattern for record properties (like reftype.ssim's 11 booleans). Trade-off: adding a new attribute requires a new field rather than a new fconst record.

**Fields:** navstyle (pkey), bold (bool), dim (bool), reverse (bool), fg_red (bool), fg_green (bool), fg_blue (bool), comment

**Records:**

| navstyle | bold | dim | reverse | fg_red | fg_green | fg_blue | comment |
|---|---|---|---|---|---|---|---|
| title_focus | N | N | Y | N | N | N | Focused panel title bar |
| title_nofocus | N | Y | Y | N | N | N | Unfocused panel title bar |
| sel_focus | N | N | Y | N | N | N | Selected row, focused |
| sel_nofocus | Y | N | N | N | N | N | Selected row, unfocused |
| statusbar | N | N | Y | N | N | N | Status bar |
| field_nav | N | N | N | N | Y | Y | Navigable references (cyan) |
| field_idx | N | N | N | Y | Y | N | Container indexes (yellow) |
| field_val | N | N | N | N | N | N | Plain values (default) |

### Table 2: `acr_navdb.reftypestyle` — Reftype-to-style mapping

Maps each of the 35 reftypes to a navstyle. The user's 3 categories:

- **Navigable references → `field_nav` (cyan):** Base, Pkey, Regx, RegxSql, Upptr (all have `up:Y` in reftype.ssim)
- **Container indexes → `field_idx` (yellow):** Atree, Bheap, Llist, Ptrary, Thash. Note: Ptrary has `isxref:N` but is explicitly grouped here as a container per the feature request. The remaining four have `isxref:Y`.
- **Plain values → `field_val` (default):** All remaining 25 reftypes

**Fields:** reftypestyle (Pkey — the primary key, references dmmeta.Reftype), navstyle (Pkey reference to acr_navdb.Navstyle), comment

**Maintenance note:** Adding a new reftype to `dmmeta.reftype` requires a corresponding `reftypestyle` record. `acr -check` will catch missing entries.

## Implementation steps

### Step 1: Create schema records for navstyle table

Use `acr_ed` to scaffold `acr_navdb.navstyle` ctype/ssimfile, add bool fields (bold, dim, reverse, fg_red, fg_green, fg_blue), run `amc`. Write the 8 navstyle data records.

### Step 2: Create schema records for reftypestyle table

Use `acr_ed` to scaffold `acr_navdb.reftypestyle` ctype/ssimfile with navstyle Pkey field, run `amc`. Write the 35 reftypestyle data records (one per reftype).

### Step 3: Add acr_nav-side types, indexes, and xrefs

Create `acr_nav.FNavstyle` and `acr_nav.FReftypestyle` in the acr_nav namespace with:
- `FDb.ind_navstyle` (Thash) — for UI chrome lookups by name
- `FDb.ind_reftypestyle` (Thash) — for loading
- `FReftypestyle.p_navstyle` (Upptr xref → FNavstyle)
- `FReftype.c_reftypestyle` (Ptr xref → FReftypestyle via ind_reftype)

Access path for field rendering: `fld->p_reftype->c_reftypestyle->p_navstyle`

Run `amc` to regenerate.

### Step 4: Write C++ helper + update Render()

In `cpp/acr_nav/acr_nav.cpp`:

**Add helper:**
```cpp
static void EmitStyle(cstring &out, acr_nav::FNavstyle &style) {
    if (style.bold) out << "\x1b[1m";
    if (style.dim) out << "\x1b[2m";
    if (style.reverse) out << "\x1b[7m";
    int color = (style.fg_red ? 1 : 0) | (style.fg_green ? 2 : 0) | (style.fg_blue ? 4 : 0);
    if (color > 0) { out << "\x1b[" << (90 + color) << "m"; }
}
```

**Replace hardcoded escapes in Render():**
- Title bars (lines 440-444, 460-465): look up `title_focus`/`title_nofocus` via `ind_navstyle_Find`
- Selected rows (lines 495-500, 523-528): look up `sel_focus`/`sel_nofocus`
- Status bar (line 533): look up `statusbar`
- Reset codes (`\x1b[0m`) stay as string constants (they are the reset, not a style)

**Add field coloring in right panel (after line 509):**
```cpp
// After building right_cell content, before outputting:
acr_nav::FNavstyle *field_style = nullptr;
if (fld && fld->p_reftype->c_reftypestyle && fld->p_reftype->c_reftypestyle->p_navstyle) {
    field_style = fld->p_reftype->c_reftypestyle->p_navstyle;
}
// Emit selection style, then field color, then cell, then reset
// acr -check guarantees the chain is complete, but null-check p_navstyle defensively
```

Selection style and field color compose via ANSI layering (reverse + cyan = cyan-on-default-background reversed).

### Step 5: Build and test

```bash
amc && abt -install acr_nav
acr_nav   # visually verify: cyan for Upptr/Pkey, yellow for Thash/Llist, default for Val
```

## Key files

| File | Change |
|---|---|
| `data/acr_navdb/navstyle.ssim` | New — 8 style records |
| `data/acr_navdb/reftypestyle.ssim` | New — 35 reftype→style records |
| `data/dmmeta/ctype.ssim` | New ctypes: acr_navdb.Navstyle, acr_navdb.Reftypestyle |
| `data/dmmeta/field.ssim` | New fields for both ctypes + acr_nav xrefs |
| `data/dmmeta/ssimfile.ssim` | Register both ssimfiles |
| `data/dmmeta/xref.ssim` | Xrefs for p_navstyle, c_reftypestyle |
| `cpp/acr_nav/acr_nav.cpp` | EmitStyle helper, Render() rewrite (~40 lines changed) |
| `cpp/gen/acr_nav_gen.cpp` | Regenerated by amc |
| `include/gen/acr_nav_gen.h` | Regenerated by amc |

## Verification

1. `amc && abt -install acr_nav` — builds clean
2. `acr_nav` — visually confirm:
   - Title bars, selection, status bar render identically to before
   - Right panel: Upptr/Pkey fields show cyan, Thash/Llist/Ptrary show yellow, Val/others show default
   - Colors compose with selection highlight (reverse+color)
3. `acr -check` — referential integrity passes
4. `normalize` — no test regressions

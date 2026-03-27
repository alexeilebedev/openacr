# acr_nav field coloring and navigation fixes

## Background

M9 introduced data-driven styling (`navstyle`, `reftypestyle` tables) and field coloring by reftype. After testing, three issues surfaced during review.

## Issue 1: Field color categories should reflect structural role, not navigability

The current 3-category scheme (nav/cyan, index/yellow, plain/white) was based on an ad-hoc grouping. It conflates structure with navigability — e.g. Lary (a pool that owns records) is white while Thash (an index into that same pool) is yellow, yet both point to the same arg ctype.

The reftype table already has boolean properties that cleanly define 4 structural roles:

- **Pool** (owns records): `hasalloc:Y` — Lary, Tpool, Lpool, Blkpool, Malloc, Sbrk
- **Index** (access path into a pool): `isxref:Y` + Ptrary — Thash, Llist, Bheap, Atree, Ptrary
- **Upward reference** (points to parent): `up:Y` — Pkey, Upptr, Base, Regx, RegxSql
- **Data value**: everything else — Val, Smallstr, Bitfld, etc.

The color should communicate the field's structural role in the in-memory database, derived from existing reftype properties.

## Issue 2: Navigability should be shown as a per-field marker, not via color

Almost every field is navigable (its arg ctype can be explored). Color is already used for structural role. Navigability is an orthogonal axis that should use a separate visual channel — e.g. a `>` suffix on fields where `follow_ref` would succeed.

This is a render-time computation (depends on whether the target ctype exists and is reachable), not a static reftype property — so it belongs in the Render() function, not in the reftypestyle data.

## Issue 3: Namespace filter blocks navigation to loaded ctypes

`follow_ref` (line 318 of `acr_nav.cpp`) checks the namespace filter before allowing navigation. This means if you start `acr_nav abt_md`, you cannot follow a field to `algo.cstring` even though that ctype is loaded in memory (`n_ctype:1384`).

The namespace filter should control the left panel browsing list, not the navigation graph. When the user explicitly follows a reference, they should be able to reach any loaded ctype.

## Issue 4: Color encoding uses 3 booleans for a single concept

The `navstyle` table encodes foreground color as three separate boolean fields (`fg_red`, `fg_green`, `fg_blue`). These 3 fields only make sense together — they're a single concept (color) split across 3 fields, which is a sub-format violation. The encoding also limits the palette to 8 ANSI colors.

A single field representing color would be simpler, more self-describing, and easier to extend.

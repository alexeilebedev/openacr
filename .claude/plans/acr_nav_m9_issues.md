# acr_nav open issues

## Background

Remaining issues identified during M9/M10 review.

## Issue 1: Namespace filter blocks navigation to loaded ctypes

`follow_ref` (line 318 of `acr_nav.cpp`) checks the namespace filter before allowing navigation. This means if you start `acr_nav abt_md`, you cannot follow a field to `algo.cstring` even though that ctype is loaded in memory (`n_ctype:1384`).

The namespace filter should control the left panel browsing list, not the navigation graph. When the user explicitly follows a reference, they should be able to reach any loaded ctype.

## Issue 2: Color encoding uses 3 booleans for a single concept

The `navstyle` table encodes foreground color as three separate boolean fields (`fg_red`, `fg_green`, `fg_blue`). These 3 fields only make sense together — they're a single concept (color) split across 3 fields, which is a sub-format violation. The encoding also limits the palette to 8 ANSI colors.

A single field representing color would be simpler, more self-describing, and easier to extend.

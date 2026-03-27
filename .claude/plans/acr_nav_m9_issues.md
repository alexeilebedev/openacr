# acr_nav open issues

## Background

Remaining issues identified during M9/M10 review.

## Issue 1: Color encoding uses 3 booleans for a single concept

The `navstyle` table encodes foreground color as three separate boolean fields (`fg_red`, `fg_green`, `fg_blue`). These 3 fields only make sense together — they're a single concept (color) split across 3 fields, which is a sub-format violation. The encoding also limits the palette to 8 ANSI colors.

A single field representing color would be simpler, more self-describing, and easier to extend.

## src_func: Rules
<a href="#src_func-rules"></a>
`src_func` indexes the functions of the tree, and `-updateproto` rewrites the
`update-hdr` sections of hand-written headers.
[The src_func README](/txt/exe/src_func/README.md) says what each option does.
What follows is the part of its design that the source does not state.

It carries no Steps section, since it reads the tree once and exits.

### An update-hdr rewrite drops only the text it generates
<a href="#an-update-hdr-rewrite-drops-only-the-text-it-generates"></a>
Suppose a header holds `namespace xx { // update-hdr` and, inside that section,
`inline int Bar() { return 1; }`.  The rewrite replaces the section's text with
the prototypes it generates, so the definition is gone.  The header still
compiles, every caller of `Bar` stops compiling, and the run exits 0.

The text the rewrite generates is a banner, which is comments, and prototypes,
which are declarations.  Neither carries a brace that nests code.  So a section
line that carries one is hand-written: a definition, a struct, a namespace, or a
macro that spells a brace.

`-updateproto` therefore refuses such a section and leaves the file unwritten.
It treats every other disagreement about where a section begins or ends the
same way.  A line without a brace that someone typed into the section, such as
a declaration, is still replaced, and the banner says so.

### Invariants
<a href="#invariants"></a>
- `-updateproto` writes a header only when every section in it was located
  without disagreement and the file's braces balance.  A refused file stays
  byte-identical, and the run exits non-zero.
- No section text that carries a nesting brace is ever dropped.  The section
  is refused with `src_func.handwritten_section`.

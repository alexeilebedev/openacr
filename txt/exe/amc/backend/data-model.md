## amc Backend: internal data model
<a href="#amc-backend-internal-data-model"></a>

amc reads ssim tuples into its own in-memory database — the same
database technology it generates code for.  This page documents the
key in-memory types and how the current scope propagates through
the generator.

### Table Of Contents
<a href="#table-of-contents"></a>
&nbsp;&nbsp;&bull;&nbsp;  [Core types](#core-types)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [genctx](#genctx)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Cross-references](#cross-references)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Reading the live in-memory db](#reading-the-live-in-memory-db)<br/>

### Core types
<a href="#core-types"></a>

*To be written.*  amc's main records:
- `amc::FNs` — one per namespace
- `amc::FCtype` — one per struct
- `amc::FField` — one per field
- `amc::FGen` — one per gen-table phase
- `amc::FFunc` — generated C++ function metadata
- `amc::FReftype`, `amc::FTclass`, `amc::FTfunc` — the dispatch
  registry

### genctx
<a href="#genctx"></a>

*To be written.*  `amc::_db.genctx` is the parameter-passing
convention used by tfuncs.  Fields like `p_ctype`, `p_field`,
`p_ns` propagate the current scope so tfuncs don't need to take
explicit arguments.

### Cross-references
<a href="#cross-references"></a>

*To be written.*  Inside amc's own db, every dmmeta record has a
counterpart `F*` ctype plus the xrefs that make navigation cheap.
Example: from an `FField` jump straight to its `FCtype` and its
`FReftype`.

### Reading the live in-memory db
<a href="#reading-the-live-in-memory-db"></a>

*To be written.*  Use `amc -e <pattern>` or `acr -e` to open
matching records.  Use `amc_vis` to draw the access-path diagram.

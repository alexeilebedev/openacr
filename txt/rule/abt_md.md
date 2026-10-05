## abt_md: the rules
<a href="#abt_md-the-rules"></a>

abt_md runs once per pass over `txt/` and holds no shared memory, so it carries
no Steps section; what it carries are the decisions its source cannot state.
The usage is in [/txt/exe/abt_md/README.md](/txt/exe/abt_md/README.md).

### Invariants
<a href="#invariants"></a>

A plain run evaluates every inline command except the tutorials'.  A tutorial is
a readme under `txt/tut`, and three of them build a sample program with
up to a dozen `acr_ed -write` passes each in the cow sandbox, which takes most of a
whole-tree pass.  Their output moves only when a tool they run or a table they
print moves.  `bin/normalize`, the gate every push waits for, runs abt_md
without `-tut`, and the `comp` cijob runs `abt_md -tut` as its `readme_tut`
citest, which fails on a tutorial whose blocks moved.  abt_md is the one place
that names the set; the citest passes the flag and no selection.

A plain run still processes a tutorial.  It regenerates the sections, records
the title, reads the anchors and saves the file, and leaves the output blocks as
they were.  The link check reads every readme at once, so a tutorial skipped
whole would turn every link into it into a broken one.

A page's sections come out in the row order of `dev.mdsection`.  A heading that
matches no row counts as Content, so every hand-written section lands in
Content's slot, between Limitations and Examples.  A tool README reads
Description, then its own sections, then Examples, Caveats and Options, and
Caveats has a row of its own only so that it sorts after Examples.  A new section
that must sit at a fixed place gets a row at that place, inserted with a
fractional `acr.rowid`, and an empty `mdsection_<name>` in
`cpp/abt_md/mdsection.cpp`.

### Decisions
<a href="#decisions"></a>

Recording what each tutorial's commands read, so a pass could skip only a
tutorial whose inputs are unchanged, was built and set aside.  The inputs reach
the blocks through tools that run other tools, through the filter script and
the tables its tool reads, and through every row a block prints, and each of
those is a dependency somebody declares by hand.  A review kept finding the next
undeclared one, and a digest that misses an input keeps a stale block with
nothing reporting it.  Forty seconds in a job nobody waits for costs less than
that table.

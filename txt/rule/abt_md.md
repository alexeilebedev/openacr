## abt_md: the rules
<a href="#abt_md-the-rules"></a>

abt_md runs once per pass over `txt/` and holds no shared memory, so it carries
no Steps section; what it carries are the decisions its source cannot state.
The usage is in [/txt/exe/abt_md/README.md](/txt/exe/abt_md/README.md).

### Invariants
<a href="#invariants"></a>

A plain run evaluates every inline command except the tutorials'.  A tutorial is
a readme marked `tut:Y` in `dev.readmefile`.  Three of the tutorials under
`txt/tut` build a sample program with up to a dozen `acr_ed -write` passes each
in the cow sandbox.  A tutorial of another package may start a server, work
with it and stop it.  Together they take most of a whole-tree pass.
Their output moves only when a tool they run or a table they print moves.
`bin/normalize`, the gate every push waits for, runs abt_md without `-tut`, and
the `compB` cijob runs `abt_md -tut` as its `readme_tut` citest, which fails on
a tutorial whose blocks moved.  The `tut` column is the one place that names the
set; the citest passes the flag and no selection.

A tutorial that starts a server runs in the sandbox, so the server's files under
`temp/` are its own and a server the developer runs is left alone.  The
tutorial stops its server in its last block.

A plain run still processes a tutorial.  It regenerates the sections, records
the title, reads the anchors and saves the file, and leaves the output blocks as
they were.  The link check reads every readme at once, so a tutorial skipped
whole would turn every link into it into a broken one.

Every pipe table in a readme is aligned in its markdown source, so it reads as
a table in a terminal and in a diff as it does rendered.  abt_md reads each
table into an `algo_lib.FTxttbl` and draws it again with `FTxttbl_Markdown`,
which pads every cell to its column's width in characters and draws the
delimiter row to that width, with a colon for a right or centered column.  A
left-justified last column is left unpadded, its text ending where it ends,
so one long cell in it widens no other row.  A
table abt_md generates comes out of the same printer, and the `readme` citest
rejects a committed table that is not aligned.  The cells are cut where
GitHub's table grammar cuts them, at every pipe not escaped with a backslash,
so the rendered table does not change.  A run of pipe lines with no delimiter
row, and everything inside a fenced block, is left as it stands.

A readme marked `narrow` in `dev.readmefile` keeps every table row within 120
characters, and `-check` refuses a wider one, naming the table's first line and
its width.  A row that runs to hundreds of characters reads as one line in a
terminal and in a diff, and alignment pads every row of the table to its widest
cell.  So a narrow readme keeps a table's cells short and puts the long text in
bullets beneath the table.  Every rule file is marked narrow; the rest of the
tree joins the check one file at a time, as its tables are brought to the rule.

A page's sections come out in the row order of `dev.mdsection`.  A heading that
matches no row counts as Content, so every hand-written section lands in
Content's slot, between Limitations and Examples.  A tool README reads
Description, then its own sections, then Examples, Caveats and Options, and
Caveats has a row of its own only so that it sorts after Examples.  A new section
that must sit at a fixed place gets a row at that place, inserted with a
fractional `acr.rowid`, and an empty `mdsection_<name>` in
`cpp/abt_md/mdsection.cpp`.

A section whose `dev.mdsection` row sets `genlist` is generated as a list of
headings, and the prose under each heading is the writer's.  abt_md keeps that
prose by the heading's key across runs, gives a new key a bare heading, and
drops the prose of a key that left the list.  A key added to the table behind
the list shows up on the page undocumented on the next run.

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

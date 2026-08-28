## acr_dm - ACR Diff/Merge
<a href="#acr_dm"></a>

Two branches edit `ssimfile:dmmeta.field`.  One adds a field to a ctype near the top
of the file; the other changes the comment of a field near the bottom.  A line-based
merge sees two regions of changed text, and where those regions come close it stops and
asks a person which text is right.  The question is the wrong one, because a line of an
ssimfile is not a line of text.  It is a record, identified by its key, and the two
branches never touched the same record.

`acr_dm` merges ssimfiles as the sets of records they are.  Two changes to different
attributes of one record merge, and so do a branch that adds an attribute to a table and
a branch that removes another one.  What is left for a person is the case where two files
say different things about one thing: the same attribute of the same record changed to two
values, or one row moved to two places.  That case gets the standard markers
`<<<<<<<`, `=======` and `>>>>>>>`, whose length `-msize` sets.

It is git's merge driver for `*.ssim` in this repo, which is how it is usually run --
see [Git integration](#git-integration).  It merges and does not diff: nothing here
prints the difference between two ssimfiles.

### Syntax
<a href="#syntax"></a>
```usage
acr_dm: ACR Diff/Merge
Usage: acr_dm [[-arg:]<string>] [options]
    OPTION       TYPE    DFLT    COMMENT
    -in          string  "data"  Input directory or filename, - for stdin
    [arg]...     string          Files to merge: older ours theirs...
    -write_ours                  Write result to ours file
    -msize       int     7       Conflict marker size
    -anchor                      Print each row's anchor, the row it was placed after
    -rowid                       Print acr.rowid, each row's position in the merged file
    -verbose     flag            Verbosity level (0..255); alias -v; cumulative
    -debug       flag            Debug level (0..255); alias -d; cumulative
    -help                        Print help and exit; alias -h
    -version                     Print version and exit
    -signature                   Show signatures and exit; alias -sig
```

### How it is called
<a href="#how-it-is-called"></a>

The arguments are files, in the order git hands them over:

- the first is *older*, the common ancestor every other version is derived from;
- the second is *ours*, which during a merge holds the changes on the current branch;
- the third is *theirs*, the head of the integration stream.

A rebase swaps the last two: *ours* is then upstream's content and *theirs* is the
branch's own.  Nothing in the merge depends on which is which, since the only comparison
it makes between two branches is a comparison of keys -- so a rebase produces the file
the merge would have.  The one asymmetry is where the answer goes: the merged file is
written to stdout, and `-write_ours` overwrites *ours* with it, which is the file git
reads back.

### A merge that settles, and one that does not
<a href="#a-merge-that-settles-and-one-that-does-not"></a>

Four files, of which the first is the common ancestor of the rest:

```ssim
inline-command: cat test/acr_dm/file1.ssim
garden.flower  flower:rose  color:red  thorned:Y
garden.flower  flower:dahlia  color:pink  thorned:N
garden.flower  flower:tulip  color:yellow  thorned:N
garden.flower  flower:orchid  color:white  thorned:N
garden.flower  flower:lotus  color:pink  thorned:N
garden.flower  flower:carnation color:red  thorned:N
garden.flower  flower:iris  color:yellow  thorned:N
```

```ssim
inline-command: cat test/acr_dm/file2.ssim
garden.flower  flower:rose  color:yellow  language:romance
garden.flower  flower:tulip  color:yellow  language:friendship
garden.flower  flower:orchid  color:white  language:happiness
garden.flower  flower:lily  color:white  language:sweet
garden.flower  flower:lotus  color:pink  language:purity
garden.flower  flower:iris  color:blue  language:luck
```

```ssim
inline-command: cat test/acr_dm/file3.ssim
garden.flower  flower:rose  color:red  leaf:compound
garden.flower  flower:dahlia  color:pink  leaf:compound
garden.flower  flower:tulip  color:red  leaf:strap
garden.flower  flower:orchid  color:white  leaf:oblong
garden.flower  flower:carnation  color:red  leaf:linear
garden.flower  flower:iris  color:yellow  leaf:sword
garden.flower  flower:daisy  color:orange  leaf:spatula
```

```ssim
inline-command: cat test/acr_dm/file4.ssim
garden.flower  flower:rose  color:red  leaf:compound
garden.flower  flower:dahlia  color:pink  leaf:compound
garden.flower  flower:tulip  color:red  leaf:strap
garden.flower  flower:orchid  color:white  leaf:oblong
garden.flower  flower:carnation  color:red  leaf:linear
garden.flower  flower:iris  color:yellow  leaf:sword
garden.flower  flower:daisy  color:orange  leaf:spatula
garden.flower  flower:lily  color:pink    leaf:bowl
```

Merging **file2** with **file3** settles without a marker.  Both files remove the
`thorned` attribute, which agrees; file2 adds `language` and file3 adds `leaf`, which are
different attributes of the same rows; and the colours each of them changed are colours
the other left alone.

```ssim
inline-command: acr_dm test/acr_dm/file1.ssim test/acr_dm/file2.ssim test/acr_dm/file3.ssim
garden.flower  flower:rose  color:yellow  language:romance  leaf:compound
garden.flower  flower:tulip  color:red  language:friendship  leaf:strap
garden.flower  flower:orchid  color:white  language:happiness  leaf:oblong
garden.flower  flower:lily  color:white  language:sweet
garden.flower  flower:iris  color:blue  language:luck  leaf:sword
garden.flower  flower:daisy  color:orange  leaf:spatula
```

Merging **file2** with **file4** does not settle.  Both add the row `lily`, and they give
it a different colour, so that row is the one thing the two files disagree about and the
only one they are asked about.

```ssim
inline-command: acr_dm test/acr_dm/file1.ssim test/acr_dm/file2.ssim test/acr_dm/file4.ssim; true
garden.flower  flower:rose  color:yellow  language:romance  leaf:compound
garden.flower  flower:tulip  color:red  language:friendship  leaf:strap
garden.flower  flower:orchid  color:white  language:happiness  leaf:oblong
<<<<<<< test/acr_dm/file2.ssim
garden.flower  flower:lily  color:white  language:sweet
=======
garden.flower  flower:lily  color:pink  leaf:bowl
>>>>>>> test/acr_dm/file4.ssim
garden.flower  flower:iris  color:blue  language:luck  leaf:sword
garden.flower  flower:daisy  color:orange  leaf:spatula
```

### A delete and a change are not a conflict
<a href="#a-delete-and-a-change-are-not-a-conflict"></a>

Say one branch deletes the row `garden.flower  flower:rose`, and the other
branch changes that same row's color to blue.  A line-based merge tool stops
here and asks a human, and it is right to: it does not know what the line
means, so it cannot know whether the change mattered.

A set is different.  The key names a role, and deleting the row says that the
role no longer exists.  Once the role is gone, no attribute of it has a value
any more, so there is nothing left for the other branch's change to disagree
with.  The change describes an attribute of a row that is not there.

`acr_dm` therefore takes the delete and prints no marker.  What makes this
sound is that a row has an identity and a known meaning here, which a line of
code does not, and that is what lets the merge settle a case a text merge has
to hand back.

### Where a row sits is part of what the file says
<a href="#where-a-row-sits-is-part-of-what-the-file-says"></a>

`ssimfile:dmmeta.field` sorts by ctype and leaves the order inside a ctype to
the file, so the position of a `dmmeta.field` row is the position of a member in
a generated struct.  Upstream moves `lib_x2net.FRecvWr.p_intfmc` from the top of
its ctype to the bottom.  A branch that has never read that namespace rebases
onto it, and gets the row back at the top: its `_Init` writes the fields in a
different order, its `dmmeta.ctypelen` reports a different length and padding,
and every entry after the move in the field-id enum is renumbered.  Nothing
objects.  `amc` reaches its fixpoint, `acr -check % -x` reports no error, and
`bin/normalize` passes, because each of them asks whether the tree agrees with
the schema as it now stands, and it does — the only copy that says otherwise is
upstream's own.

A row's position is therefore one more thing the two files can disagree about,
alongside each of its attributes.  It merges by the rule the attributes merge
by: where only one file changed it, that file decides, and where both changed it
differently, nobody decides and the file gets a marker.

The base file fixes the order every other file is read against.  A file that
lists the rows it shares with the base in the base file's own order has moved
nothing, and a row that arrives out of that order is a row this file moved.  A
moved row is placed where this file put it, which is to say it joins the run
being built at that point, so a block of rows moved together comes out as one
run in the order the file wrote them.  Two files that move one row to two
different places conflict, and the two sides of that marker block are the same
row printed twice — the text of a row says nothing about where it sits — so the
line above the block names the two positions that disagreed.

Two moves can also ask for a cycle, one file putting `a` after `b` while the
other puts `b` after `a`.  A cycle is refused and reported as a conflict.  The
merged file is a walk from the start of the file, and a row on a cycle is a row
the walk never reaches: it would come out of the merge missing altogether, which
is the one outcome worse than a conflict.

### A line that names no row belongs to the row below it
<a href="#a-line-that-names-no-row-belongs-to-the-row-below-it"></a>

A generated package manifest under `apm/gen/` carries a checksum above each of
the files it names:

```ssim
# SHA1 = DvDKOwBjfK6Qmm2EYNOrdLs63YA=
dev.gitfile  gitfile:cpp/lib_ams/board.cpp
```

That line names no row, so there is nothing to key it by, and a merge of rows
has nowhere to put it.  What the line means is what its position says: it is the
checksum of the row under it.  So it is held as that row's own text, and it
merges the way the row's attributes do — a checksum that changed on one side is
a change and stands, and one that changed on both sides is a conflict.  Lines
below the last row are the file's tail, and they come out after every row there
is.

The rule covers every line the merge cannot key, not only a comment, which is
what keeps a line nobody anticipated from being dropped in silence.

### A failed merge leaves a file that says so
<a href="#a-failed-merge-leaves-a-file-that-says-so"></a>

A merge driver that exits without writing leaves git holding the *ours* file
exactly as it was handed over, and during a rebase that is upstream's content: a
complete, well-formed ssimfile, carrying no marker, holding none of the rows the
branch added, and offering a reader nothing to suggest that anything was
dropped.  Staging it by inspection loses the branch's work, and a resolver that
reads "no markers" as "already merged" loses it with nobody looking.

So a failure writes both sides whole, between markers, and says why on stderr.
The artifact is unusable on purpose: nothing can mistake it for a merged result.

### Algorithm
<a href="#algorithm"></a>

Rows that a file adds one after another form a run, and the run is what gets
placed.  As `acr_dm` reads a file it remembers the last row that file had in
common with what the merge already knew, and a run of new rows hangs off that
row — the run's anchor.  Before the first shared row there is no such row, so a
run at the top of a file hangs off a virtual row standing for the start of the
file.

A row the base file also has, arriving out of the base file's order, is a row
this file moved, and it joins the run being built exactly as an added row does.
So the anchor of a run is written when the run is read, and a row's own place in
a run is written by the one file that put it there.

The output is the runs hanging off the virtual start row, printed in order, each
row followed by the runs hanging off it.  Rows that share an anchor are ordered
by three rules:

- Rows a branch added come before the row the base file had there, which is how
an insertion lands between the two rows it was written between.
- Two runs from different branches are ordered by the key of the first row of
each run.
- Within one run, rows keep the order the branch wrote them in.

Two properties follow.  A branch's run prints without interruption, because the
run is placed as a unit and another branch's run cannot be threaded through it.
And the only comparison ever made between two branches is a comparison of keys,
which cannot tell which branch was passed second — so a rebase, which swaps
*ours* and *theirs*, produces the same file the merge did.

Placing runs rather than rows also keeps the walk over the result shallow.  It
descends once per run nested inside a run, never once per row, so a file that
one branch adds whole is walked at a single level instead of at as many levels
as the file has rows.  A move adds a level and no more: a moved row hangs off a
row its own file left where the base file had it, so the levels a file can add
is one, and a block moved together adds that one level for the whole block.

Use `-anchor` to see the anchor each row was given, and `-rowid` to print each
row's position in the merged file.  `-debug` dumps every row with the files that
showed it, the row it follows, and the files that moved it.

### Git integration
<a href="#git-integration"></a>

`gitconfig-setup` installs `acr_dm` as a merge driver in the local repository's git
config.  The driver is a local setting, so a fresh clone that has never run that command
merges ssimfiles as text, line by line, with no idea that a row's position is a member's
position in a generated struct.  `git config --get merge.acr_dm.driver` prints the
command line when the driver is there.

`.gitattributes` is what selects the driver for a file:

```bash
inline-command: grep acr_dm .gitattributes
*.ssim merge=acr_dm
```

### Options
<a href="#options"></a>
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

Where the ssim database this tool loads for itself comes from.  It is not the files being
merged, which are the positional arguments; the database is what carries
`dmmeta.dispsigcheck`, so that a merge refuses to run under a schema its executable was
not built for.

#### -arg -- Files to merge: older ours theirs...
<a href="#-arg"></a>

The files, in git's own order: the common ancestor, then this branch's version, then the
other one.  More than three are accepted, and each file after the second is one more
branch read against the same ancestor -- a conflict then carries a marker per side rather
than two.

#### -write_ours -- Write result to ours file
<a href="#-write_ours"></a>

Overwrite the second file with the merged result, which is what git reads back from a
merge driver.  Without it the result goes to stdout, which is how a merge is inspected
without touching either input -- and how every example above is run.

#### -msize -- Conflict marker size
<a href="#-msize"></a>

How many characters long each conflict marker is.  Seven is what git writes and what
every tool that reads a conflicted file expects; a longer marker is for a file whose own
content carries a run of seven angle brackets.

#### -anchor -- Print each row's anchor, the row it was placed after
<a href="#-anchor"></a>

Show, beside each row of the merged file, the row it was placed after.  That is the
decision [the algorithm](#algorithm) makes about position, so this is what to read when a
row comes out somewhere unexpected: the anchor says which row it was hung off, and the
file that moved it is the file whose order put it there.

#### -rowid -- Print acr.rowid, each row's position in the merged file
<a href="#-rowid"></a>

Number the rows of the merged file in the order they come out.  A position is a fact
about the output rather than about any input, so this is how two runs are compared -- a
rebase against the merge it swaps the arguments of, for instance, which must produce the
same file.

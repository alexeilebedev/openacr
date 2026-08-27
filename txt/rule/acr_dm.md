## acr_dm: the rules
<a href="#acr_dm-the-rules"></a>

`acr_dm` merges ssimfiles.  Git runs it as the merge driver for `*.ssim`, which
`.gitattributes` selects and `gitconfig-setup` installs, so it is what resolves
`data/` on every merge and every rebase in this repo.  `apm` runs it as a
subprocess to merge the records of a package it is installing or updating.  Its
usage, its algorithm and its worked examples are in
[txt/exe/acr_dm/README.md](/txt/exe/acr_dm/README.md); what follows is the part
of its design that neither the source nor that document states.

It carries no Steps section.  A step inventory describes a module that runs
forever in a topology, and `acr_dm` is a program that reads its inputs once,
prints one file and exits.

### What a merge of ssimfiles is
<a href="#what-a-merge-of-ssimfiles-is"></a>

An ssimfile is a set of rows, and a row is identified by its key.  Everything
else about a row is something each file *says* about it: the value of each of its
attributes, the text of the lines above it, and the row it comes after.  A merge
is one rule applied to every one of those, separately:

- one value, and it stands, unless some file removed it;
- two values with the base file's among them, which is one file changing it, and
the change stands;
- anything else is two files disagreeing, and it goes in the file between
markers.

Stating the rule once, and applying it to a row's position exactly as to its
`comment:`, is what the design is for.  A merge that treats position as a
property of the algorithm rather than as a value the files state will always have
some case where a file's decision about order is quietly discarded, because
nothing in the merge is representing it.

That a delete beats a change is the same rule seen from one end.  The key names a
role, deleting the row says the role no longer exists, and an attribute of a row
that is gone has nothing left to disagree with.

### The invariants
<a href="#the-invariants"></a>

Each one is here because breaking it produces a file that looks correct.  That is
the whole hazard of this tool: its output is a well-formed ssimfile either way,
`amc` regenerates from it, every target builds, and no check downstream asks
whether the file still says what both branches said.

**Nothing an input says is dropped in silence.**  Every line of every input ends
up in the result, or is a removal the merge resolved, or is inside a marker
block.  A line the merge cannot key is not an exception -- it belongs to the row
below it and merges as that row's own text.

**A conflict is visible in the artifact, never only in the exit code.**  Git
hands the driver the *ours* file and keeps whatever the driver leaves there, so a
driver that exits non-zero without writing leaves upstream's content with no
marker in it and none of the branch's rows.  That file reads as a resolved one.
Any refusal to merge therefore writes markers, and a refusal that cannot even
parse its inputs writes both sides whole between markers.

**An input that cannot be read is a failure.**  A missing file would otherwise
read as a file with no rows, which says every row was deleted, and the merge
would write an empty file and exit zero.  An empty file stays legitimate: `apm`
merges one for a package whose incoming version dropped every record it had.

**The result does not depend on which argument slot git put a branch in.**  A
rebase hands over the same three files a merge does with *ours* and *theirs*
swapped, and the two must produce the same file.  So no comparison between two
branches may be by file index; the only thing compared is a key, and a key cannot
tell one branch from the other.  The marker labels are the one exception, and
they name the file in each slot by definition.

**Every row the walk should reach, it reaches.**  The output is a walk of the
tree the rows' anchors form, so a cycle in that tree is rows silently missing
from the merged file.  Anything that would close a cycle is refused and reported
as a conflict instead.

**The walk's depth is bounded by the structure, not by the input.**  Depth is the
nesting of runs plus at most one level per input file.  A million-row file walks
without growing the stack, which is R31 of the codestyle skill and which the
`acr_dm.DeepRun` comptest holds at 100k rows.

**Row order is an interface.**  `dmmeta.ssimsort` sorts `dmmeta.field` by ctype
and by nothing else, so the order of rows within a ctype is the order of members
in a generated struct -- a struct's length, its padding, its `_Init` and its
field-id enum.  A table is order-dependent exactly when `acr` prints an
`acr.rowid` attribute for it.

### Position is a tree, and two simpler designs are not available
<a href="#position-is-a-tree-and-two-simpler-designs-are-not-available"></a>

A row hangs off the row it follows, and the merged file is a walk from a virtual
row standing for the start of the file.  Rows a file adds one after another form
a run, and the run hangs off one anchor as a unit.

Recording each row's predecessor as a value of the row, and merging that value by
the rule above, is the design this one keeps being mistaken for.  It is more
uniform and it does not work: every file states a predecessor for every row, so
the anchors form a chain as long as the file, and the walk recurses once per row.
That is the shape that segfaults around fifty thousand rows, and `dmmeta.field`
is already several thousand.  Runs exist to bound the depth, which is why
position is a tree with a separate conflict rule stated to match, rather than one
more attribute.

Anchoring each row on the row directly above it has the same defect for the same
reason, and it arrives by a different route: it reads naturally and it gets a
branch's additions contiguous for free.  A file that one branch adds whole shares
no row with the base at all, so every row anchors on the one above it.

Which rows a file moved is decided greedily.  The base file fixes the order the
other files are read against, and a row whose base sequence number goes backwards
against the last row this file left alone is a row this file moved.  Moving one
row upward is therefore recorded as the rows it jumped over having moved down.
The output is the file's own order either way, since the flagged rows are placed
in that order; what the imprecision costs is a slightly wider surface for a
contested-move conflict, and what it buys is that inserting rows never looks like
moving anything.

### What is deliberately not merged
<a href="#what-is-deliberately-not-merged"></a>

The position of a row that the base file does not have.  Two files that both add
one row, and put it in different places, get the position of whichever file git
passed first.  Merging that too means recording a predecessor for rows outside
the base file's order, which is the chain the section above rules out, and the
rows whose order is load-bearing -- a ctype's fields, in a table both branches
edit -- are rows the base file has.

### The callers
<a href="#the-callers"></a>

Git passes `%O %A %B` and `-msize %L`, takes the result from the *ours* file
under `-write_ours`, and reads a non-zero exit as a conflict.  The command line
is a local git setting: a clone that never ran `gitconfig-setup` merges ssimfiles
as text, and every hazard this file describes comes back in full, which is why
`git config --get merge.acr_dm.driver` is worth confirming before trusting a
merge in a fresh clone.

`apm` composes a merge of a package's records into its transaction script.  Its
update passes three files and its push passes two, so a two-file merge is a
supported invocation and not a degenerate one.  Push also reads `-rowid` back
through `acr -replace`, which makes the printed position a value another tool
parses: `acr` orders the rows of one sortkey group by rowid, and a number that
only rises keeps each group in the order the merge chose.

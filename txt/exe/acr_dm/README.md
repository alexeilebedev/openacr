## acr_dm - ACR Diff/Merge
<a href="#acr_dm"></a>

ACR Diff/Merge is a tool for merging ssim files.
It is typically installed as a "merge driver" for git, and called upon by git
to merge changes made in ssimfiles. The main insight is that since ssimfile are
sets, they can be merged more efficiently than source files. Changes in nearby
lines are not conflicts, and even changes to different attributes of a record
having the same key are not conflicts either. Only change to the same attribute
are considered a conflict. Thus, the tool can easily merge changes where one version
adds a column, another removes a column.

### Table Of Contents
<a href="#table-of-contents"></a>
<!-- abt_md.toc_beg -->
&nbsp;&nbsp;&bull;&nbsp;  [Internals](#internals)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Limitations](#limitations)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Operation](#operation)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [A delete and a change are not a conflict](#a-delete-and-a-change-are-not-a-conflict)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Algorithm](#algorithm)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Git integration](#git-integration)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Options](#options)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
<!-- abt_md.toc_end -->

### Internals
<a href="#internals"></a>
&#128196; [acr_dm - Internals](/txt/gen/acr_dm/acr_dm.md)<br/>

### Limitations
<a href="#limitations"></a>

At the moment the following limitations exist:

- No diff function, merge only;

### Operation
<a href="#operation"></a>

The tool accepts a list of files. These are the file names to merge.

- The first argument is *older* (archived) version, from which all other
versions are derived;
- second argument is *ours* version, which contains our changes to
integration stream;
- third argument is *theirs* version, which is the head of integration
stream.

In case of rebase, *ours* and *theirs* swap their places: *ours* is the
head of integration stream, and *theirs* is our changes.

There is no fundamental difference between *ours*, and *theirs*, the
only difference that git merge overwrites *ours* file.

The tool loads all input files and merges *ours* and *theirs* input
tuples, writes result to stdout or, if `-write_ours` is specified,
to *ours* file overwriting it.

The tool handles tuples and attribute addition and removal,
detecting conflicts. Conflicts are marked by standard markers
`<<<<<<<`,`=======`,`>>>>>>>`.  Marker size could be adjusted with
`-msize` option.

Here is the example.

Given four files:

```
inline-command: cat test/acr_dm/file1.ssim
garden.flower  flower:rose  color:red  thorned:Y
garden.flower  flower:dahlia  color:pink  thorned:N
garden.flower  flower:tulip  color:yellow  thorned:N
garden.flower  flower:orchid  color:white  thorned:N
garden.flower  flower:lotus  color:pink  thorned:N
garden.flower  flower:carnation color:red  thorned:N
garden.flower  flower:iris  color:yellow  thorned:N
```

```
inline-command: cat test/acr_dm/file2.ssim
garden.flower  flower:rose  color:yellow  language:romance
garden.flower  flower:tulip  color:yellow  language:friendship
garden.flower  flower:orchid  color:white  language:happiness
garden.flower  flower:lily  color:white  language:sweet
garden.flower  flower:lotus  color:pink  language:purity
garden.flower  flower:iris  color:blue  language:luck
```

```
inline-command: cat test/acr_dm/file3.ssim
garden.flower  flower:rose  color:red  leaf:compound
garden.flower  flower:dahlia  color:pink  leaf:compound
garden.flower  flower:tulip  color:red  leaf:strap
garden.flower  flower:orchid  color:white  leaf:oblong
garden.flower  flower:carnation  color:red  leaf:linear
garden.flower  flower:iris  color:yellow  leaf:sword
garden.flower  flower:daisy  color:orange  leaf:spatula
```

```
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

**file1** is the common ancestor.

Let's merge **file2** with **file3**.
Both file2 anad file3 remove the column `thorned`. File2 adds a new column `language`
and file3 adds a new column 'leaf', and there are some changes to the color attribute,
but they don't conflict. The resulting changes can be merged without conflict.

```
inline-command: acr_dm test/acr_dm/file1.ssim test/acr_dm/file2.ssim test/acr_dm/file3.ssim
garden.flower  flower:rose  color:yellow  language:romance  leaf:compound
garden.flower  flower:tulip  color:red  language:friendship  leaf:strap
garden.flower  flower:orchid  color:white  language:happiness  leaf:oblong
garden.flower  flower:lily  color:white  language:sweet
garden.flower  flower:iris  color:blue  language:luck  leaf:sword
garden.flower  flower:daisy  color:orange  leaf:spatula
```

Now let's merge **file2** with **file4**

```
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

Merge results in conflict.

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

### Algorithm
<a href="#algorithm"></a>

Rows that a file adds one after another form a run, and the run is what gets
placed.  As `acr_dm` reads a file it remembers the last row that file had in
common with what the merge already knew, and a run of new rows hangs off that
row — the run's anchor.  Before the first shared row there is no such row, so a
run at the top of a file hangs off a virtual row standing for the start of the
file.

A run is read for the first time in exactly one file, so its anchor is written
once and never revised.

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
as the file has rows.

Use `-anchor` to see the anchor each row was given, and `-rowid` to print each
row's position in the merged file.

### Git integration
<a href="#git-integration"></a>

The tool integrates with local git repository as custom merge driver.

To install, run:
```
gitconfig-setup
```

To choose merge driver for ssim files, **.gitattributes** file is used:

```
inline-command: grep acr_dm .gitattributes
*.ssim merge=acr_dm
```

### Options
<a href="#options"></a>
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

#### -arg -- Files to merge: older ours theirs...
<a href="#-arg"></a>

#### -write_ours -- Write result to ours file
<a href="#-write_ours"></a>

#### -msize -- Conflict marker size
<a href="#-msize"></a>

#### -anchor -- Print each row's anchor, the row it was placed after
<a href="#-anchor"></a>

#### -rowid -- Print acr.rowid, each row's position in the merged file
<a href="#-rowid"></a>

### Inputs
<a href="#inputs"></a>
`acr_dm` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|

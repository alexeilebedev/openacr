## acr_dm - ACR Diff/Merge
<a href="#acr_dm"></a>

`acr_dm` merges ssimfiles as sets of records, each identified by its key.  Two branches
that touch different records, or different attributes of one record, merge without a
conflict.  A person is asked only when two files say different things about one thing,
and the question arrives between the standard markers `<<<<<<<`, `=======` and
`>>>>>>>`.  Git runs it as the merge driver for `*.ssim`, see
[Git integration](#git-integration).

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
    -trace       string  ""      Trace expression: category[:filter],...; also payload_lim:N, verbose, debug, timestamps
    -help                        Print help and exit; alias -h
    -version                     Print version and exit
    -signature                   Show signatures and exit; alias -sig
```

### Description
<a href="#description"></a>

`acr_dm` reads two or more versions of one ssimfile and prints the merged file to
stdout.  With `-write_ours` it writes the result over the second file, which is where
git reads it back.  It exits 1 when the result holds a conflict, and 0 when it does not.
It merges and does not diff, so nothing here prints the difference between two
ssimfiles.

#### How it is called
<a href="#how-it-is-called"></a>

The arguments are files, in the order git hands them over:

- the first is *older*, the common ancestor every other version is derived from;
- the second is *ours*, which during a merge holds the changes on the current branch;
- the third is *theirs*, the head of the integration stream.

A rebase swaps the last two, so *ours* is upstream's content and *theirs* is the
branch's own.  The merge compares branches only by key, so a rebase produces the same
file the merge would.  Each file after the second is one more branch read against the
same ancestor, and `apm` also runs a two-file merge.

#### What merges and what conflicts
<a href="#what-merges-and-what-conflicts"></a>

A merge applies one rule to each thing a file says about a row: the value of each
attribute, the lines above the row, and the row's position.  When only one file changed
it, that file's version stands.  When two files changed it to different values, the row
goes into the result between markers.

A row that one branch deletes and another branch changes is deleted, with no marker.
The key names a record, and a deleted record has no attributes left to disagree about.

A row's position is merged like an attribute, because order matters in some tables.  In
`dmmeta.field` the order of a ctype's fields is the order of members in the generated
struct.  A file that keeps its rows in the ancestor's order has moved nothing.  A row it
places elsewhere is a row it moved, and the merge keeps the move.  Rows moved together
come out together, in the order the file wrote them.

Two files that move one row to two different places conflict.  The two sides of that
marker block are the same row printed twice, so a comment line above the block names the
two positions, as `follows:` and `wanted:`.  Two moves that would form a cycle, one file
putting `a` after `b` and the other `b` after `a`, are also reported as a conflict.

A line that is not a record, such as the `# SHA1 = ...` checksum that `apm -generate`
writes above each `dev.gitfile` row, belongs to the row below it.  It merges as that
row's own text, so a checksum changed on one side stands and one changed on both sides
conflicts.  Lines below the last row come out after every row.

#### When a merge cannot run
<a href="#when-a-merge-cannot-run"></a>

When an input is missing or cannot be read, `acr_dm` prints `acr_dm.mergefail` on stderr
and writes the ours and theirs files whole, between markers.  The base file is not
written, and a missing input leaves its side of the markers empty.  The file it leaves
cannot be mistaken for a merged one.  An empty input is a valid file with no rows.

### Git integration
<a href="#git-integration"></a>

`gitconfig-setup` installs `acr_dm` as a merge driver in the local repository's git
config, and `git config --get merge.acr_dm.driver` prints the command line when it is
there.  `.gitattributes` selects the driver for a file:

```bash
inline-command: grep acr_dm .gitattributes
*.ssim merge=acr_dm
```

### Examples
<a href="#examples"></a>

```bash
# Merge three versions and print the result, leaving the inputs alone
acr_dm older.ssim ours.ssim theirs.ssim

# Merge the way git does, writing the result over ours.ssim
acr_dm -write_ours -msize 7 -- older.ssim ours.ssim theirs.ssim

# Show where the merge placed each row, and its position in the result
acr_dm older.ssim ours.ssim theirs.ssim -anchor -rowid

# Show every row with the files it appeared in, the row it follows and who moved it
acr_dm older.ssim ours.ssim theirs.ssim -debug

# Check that this clone has the merge driver installed
git config --get merge.acr_dm.driver
```

#### A merge that settles, and one that does not
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
`thorned` attribute, so they agree.  File2 adds `language` and file3 adds `leaf`, which
are different attributes of the same rows.  And each file changed only colors the other
left alone.

```ssim
inline-command: acr_dm test/acr_dm/file1.ssim test/acr_dm/file2.ssim test/acr_dm/file3.ssim
garden.flower  flower:rose  color:yellow  language:romance  leaf:compound
garden.flower  flower:tulip  color:red  language:friendship  leaf:strap
garden.flower  flower:orchid  color:white  language:happiness  leaf:oblong
garden.flower  flower:lily  color:white  language:sweet
garden.flower  flower:iris  color:blue  language:luck  leaf:sword
garden.flower  flower:daisy  color:orange  leaf:spatula
```

Merging **file2** with **file4** does not settle.  Both add the row `lily` with a
different color, so that row is the one thing the two files disagree about, and the only
one they are asked about.

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

#### Two branches that move one field
<a href="#two-branches-that-move-one-field"></a>

The ancestor lists the fields `a`, `b`, `c` and `d` of a ctype `ns.C`, in that order.
One branch moves `b` after `c`, and the other moves it after `d`.  The result keeps the
rows neither branch moved, and names the two positions above the conflict:

```bash
$ acr_dm test/acr_dm/movecfl1.ssim test/acr_dm/movecfl2.ssim test/acr_dm/movecfl3.ssim
acr_dm.moveconflict  row:"dmmeta.field  field:ns.C.b"  follows:"dmmeta.field  field:ns.C.c"  wanted:"dmmeta.field  field:ns.C.d"  cycle:N  source:2
dmmeta.field  field:ns.C.a  arg:i32  reftype:Val  dflt:""  comment:""
dmmeta.field  field:ns.C.c  arg:i32  reftype:Val  dflt:""  comment:""
# acr_dm.moveconflict  row:"dmmeta.field  field:ns.C.b"  follows:"dmmeta.field  field:ns.C.c"  wanted:"dmmeta.field  field:ns.C.d"
<<<<<<< test/acr_dm/movecfl2.ssim
dmmeta.field  field:ns.C.b  arg:i32  reftype:Val  dflt:""  comment:""
=======
dmmeta.field  field:ns.C.b  arg:i32  reftype:Val  dflt:""  comment:""
>>>>>>> test/acr_dm/movecfl3.ssim
dmmeta.field  field:ns.C.d  arg:i32  reftype:Val  dflt:""  comment:""
```

Resolve it by keeping one copy of `b` and putting it where it belongs.

### Caveats
<a href="#caveats"></a>

- The merge driver is a setting in the local git config.  A clone that never ran
  `gitconfig-setup` merges ssimfiles as text, line by line, with no idea of keys or of
  field order.
- A clean merge is not a checked merge.  `amc`, `acr -check % -x` and `bin/normalize`
  accept a file whose field order no branch wrote, because each checks the tree against
  itself.  Read the result of a merge that touched the order of `dmmeta.field` rows.
- When two branches add the same new row at different places, the row goes where the
  first of the two files put it, with no conflict.
- A position conflict reads as two identical rows.  The `# acr_dm.moveconflict` line
  above it is what says which positions disagree; delete it along with the markers.

### Options
<a href="#options"></a>
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

The ssim database `acr_dm` loads for itself, `data` by default.  It holds
`dmmeta.dispsigcheck`, so a merge refuses to run under a schema its executable was not
built for.  The files being merged are the positional arguments.

#### -arg -- Files to merge: older ours theirs...
<a href="#-arg"></a>

The files, in git's order: the common ancestor, this branch's version, then the other
one.  More than three are accepted, and each extra file is one more branch read against
the same ancestor, with one marker per side in a conflict.

#### -write_ours -- Write result to ours file
<a href="#-write_ours"></a>

Writes the merged result over the second file, which is what git reads back from a merge
driver.  It takes effect when three or more files are given.  Without it the result goes
to stdout, which is how to inspect a merge without touching an input.

#### -msize -- Conflict marker size
<a href="#-msize"></a>

The number of characters in each conflict marker, 7 by default.  Git passes its own
marker size here, which is longer for a file whose content already holds a run of seven
angle brackets.

#### -anchor -- Print each row's anchor, the row it was placed after
<a href="#-anchor"></a>

Adds `acr_dm.follows:` to each row of the result, naming the row it was placed after.
An empty value is the start of the file.  Read it when a row comes out somewhere
unexpected.

#### -rowid -- Print acr.rowid, each row's position in the merged file
<a href="#-rowid"></a>

Adds `acr.rowid:` to each row of the result, a number that rises through the file.  `apm`
reads it back through `acr -replace` to keep each group of rows in the order the merge
chose.

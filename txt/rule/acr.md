## acr: Rules
<a href="#acr-rules"></a>

`acr` is the query and edit front end for the ssim database.  Every other tool
that changes the schema does it by running `acr`, so the behavior described
here is the behavior the whole toolchain inherits.  Read
[/txt/rule/openacr.md](/txt/rule/openacr.md) beside this file, and
[/txt/exe/acr/README.md](/txt/exe/acr/README.md) for the option-by-option
reference.

### A selection is a pattern, and a pattern is not a scope
<a href="#a-selection-is-a-pattern-and-a-pattern-is-not-a-scope"></a>

A query has two halves, `<ssimfile>:<pkey>`, and both accept `%` as a
wildcard.  What makes `%` dangerous is that it spans separators.  A namespace
key looks hierarchical, so `acr ns:acr%` reads as "the acr namespace and things
under it" -- and it selects `acr_compl`, `acr_dm`, `acr_ed`, `acr_in` and
`acr_my` as well.  Paired with `-del -write` that is five namespaces the
command never named, removed alongside the one it did.

Two rules follow.  A delete is written against the rows it means to remove,
never against a prefix that happens to cover them.  And when a program has
already computed the set -- a list of tuples rather than a pattern -- `-sel`
reads those tuples on stdin and selects exactly the records they name:

```bash
cat rows.acr | acr -sel -del -write -in:<root>   # n_delete equals the lines in rows.acr
```

`-t` deserves its own warning, because its name suggests terseness and it means
`-tree -xref -loose`.  It expands the selection to the whole cross-reference
tree of whatever matched.  **Never combine `-t` with `-del -write`**: the
combination deletes the transitive closure, not the rows.  A scoped delete is
`acr <ssimfile>:<pat> -del -write` without `-t`, which removes the matched rows
plus the rows that directly reference them.

### An unrecognized attribute is ignored, never rejected
<a href="#an-unrecognized-attribute-is-ignored-never-rejected"></a>

`acr` parses a data row against the current ctype, and an attribute naming no
field of that ctype is skipped rather than treated as an error.  That tolerance
is deliberate and it is what makes the schema editable at all: a tuple written
under one version of a ctype still loads under the next, so a schema can move
ahead of the files that hold its data and nothing has to be rewritten in
lockstep.

The same tolerance means a dead attribute is invisible.  Delete
`dmmeta.Ns.license` and `ssimfile:dmmeta.ns` still reads `dmmeta.ns  ns:abt
nstype:exe  license:GPL  comment:"..."`.  `acr -check % -x` passes, `amc`
passes, and `acr ns:abt` prints the row without the attribute -- every check
agrees the edit is complete while the file still carries the values.
`bin/normalize` is what eventually objects, as a `normalize_acr` diff, which is
a full pipeline spent on something the edit should have carried.

So a schema change that removes a field also removes the values.  Reading each
row through the new schema and writing it back is what drops the attribute:

```bash
acr '<ssimfile>:%' -write -print:N
```

`acr_ed -del -field ... -write` does this for you; see
[/txt/rule/acr_ed.md](/txt/rule/acr_ed.md).  Doing the delete with raw `acr` is
what leaves it undone.

### A write serializes the database, not the file it read
<a href="#a-write-serializes-the-database-not-the-file-it-read"></a>

`-write` prints the in-memory database back out.  A row that never entered the
database is therefore not written back, whatever the file on disk says, and
that is the whole of the following surprise.

A doc file is renamed, so the two tables tracking it have to follow.
`bin/update-gitfile` rescans the tree and drops the `dev.gitfile` row naming
the old path.  Deleting the `dev.readmefile` row for that same path is then the
obvious next step, and the report reads `n_select:0  n_delete:0  n_file_mod:0`
-- nothing matched, nothing was written -- while the row is plainly still in
`ssimfile:dev.readmefile`.  Yet the next command that writes that ssimfile for
any reason emits it without the row, and the deletion has happened by accident.

`dev.Readmefile.gitfile` is a `Pkey` into `dev.Gitfile`, and a `Pkey` is
resolved at load.  Once the parent row is gone the child row does not enter the
database at all, so the selection has nothing to match, which is why the delete
reports zero rather than an error.

Two things to take from it.  A delete reporting `n_select:0` after a parent was
removed has neither failed nor succeeded -- it was pre-empted, and the file on
disk is stale until something rewrites it.  And the order of the two deletes is
what makes the outcome legible: remove the child row first, while its parent
still resolves and the report can say `n_delete:1`, then rescan the parent
table.  Reversed, the end state is identical and no command ever claimed to
produce it.

### Read `n_file_mod`, not the echoed edit
<a href="#read-n_file_mod-not-the-echoed-edit"></a>

A refused write still echoes the edit it was going to make, so the output reads
like success: an `acr.insert` line followed by `n_insert:1  n_file_mod:0`, and
the file untouched.  `n_file_mod` is the number that says whether anything
landed.

`-check` is what refuses, and it enforces more than foreign keys.  A row whose
Pkey names a record that does not exist is rejected with `acr.badrefs` naming
the ctype.  An over-long attribute is rejected the same way, with
`acr.attr_too_long` naming the field and its limit.  So a row can be
well-formed, correctly referenced, and still silently not written.  When an
insert matters, grep the file for the key afterwards.

One caveat keeps `-check` honest.  When the referenced key is itself generated
*from* the row being inserted, the check reports a bad ref until `amc` has run.
`dmmeta.usertracefld` is the standing case: its `tracefld` is a Pkey into the
amc-owned `dmmeta.tracefld`, so a new counter reads as a bad ref until `amc`
emits the target row.  Insert, run `amc`, then re-check.

### Insert through `-insert`, never by editing the file
<a href="#insert-through-insert-never-by-editing-the-file"></a>

`acr` rewrites an ssimfile in sorted order, so a row piped in through `-insert`
lands in its correct position and the diff carries that row alone.  A
hand-placed row is the same record in the wrong place, and nothing targeted
objects: `amc`, `acr -check % -x` and `abt_md -check` all pass over a
misordered file.  Only `bin/normalize` notices, as `normalize_acr` plus one
failure per generated artifact that inherited the order.

Row order inside `ssimfile:dmmeta.field` is the exception that is not a
formatting matter at all: it is the member order of the generated struct, so
`acr -insert` respecting it is what keeps layout stable, and
`acr_ed -create -field ... -before <field>` is how a row is placed
deliberately.

### Validation after any edit
<a href="#validation-after-any-edit"></a>

`acr -check %` runs referential integrity over the selection, and `-x`
propagates the check into the `ssimreq` records.  `amc` is the second gate: it
rejects schema the generator cannot emit from.  Both must exit zero.

```bash
acr -check % -x
amc
```

After a git add, remove or rename of a tracked file, run `update-gitfile`
first, then the check.  `acr_ed -write` runs `amc` for you, so an `acr_ed` edit
that succeeded needs no further check.

### Renaming an ssimfile is an ordered edit
<a href="#renaming-an-ssimfile-is-an-ordered-edit"></a>

A bare `acr ssimfile:<x> -rename:<y> -write` renames the record and nothing
else: the data rows keep the old attribute name and the file keeps its old
path.  The rename is four steps in order -- the like-named key field, then the
ssimfile itself **with `-g -x`**, then the ctype, then `amc` -- and `-g -x` is
what rewrites the rows' attribute key and moves the physical file under git.
`acr_ed -ssimfile <old> -rename <new> -write` performs the whole sequence,
which is the reason to use it rather than assembling the steps by hand.

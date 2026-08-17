## Acr — Querying and Editing the Ssim Database
<a href="#acr-querying-and-editing-the-ssim-database"></a>

`acr` is the query and editing front-end for the ssim dataset.  The
full reference — including all options, the operation pipeline, editing
workflows, validation, `acr_in`, and `acr_ed` common patterns — is at:

**[txt/exe/acr/README.md](/txt/exe/acr/README.md)**

This page contains a compact quick-reference cheatsheet.

### Query
<a href="#query"></a>

```bash
acr '<ssimfile>:<pkey>'                  # select by pkey
acr <pkey> -t -cmt                       # transitive closure + comments
acr field -where arg:u8                  # filter on non-primary field
acr field:<ctype>.%                      # all fields of a ctype
acr field:command.<proc>.%               # command-line options for a process
acr targdep:<target>.%                   # dependencies of a target
acr ssimfile                             # list all ssim tables
acr ns                                   # list all namespaces
acr target                               # list build targets
acr xref                                 # in-memory cross-references
acr %                                    # dump everything
```

Both halves of the query accept `%` as a wildcard.  The first
attribute of each tuple is the primary key.

### Inputs and outputs of a target
<a href="#inputs-and-outputs-of-a-target"></a>

```bash
acr_in <target>                          # ssimfiles a target reads
acr_in -r <ssimfile>                     # which targets read this file
acr_in <ns regx> -data                   # actual tuples loaded
```

### Edit
<a href="#edit"></a>

```bash
echo '<ssim tuple>' | acr -insert -write        # insert a record
echo '<full ssim tuple>' | acr -merge -write    # upsert
acr <ssimfile>:<pkey> -del -write               # delete a record
acr <pkey> -rename:<newkey> -write              # rename pkey (data row)
acr field:a.b.c -rename:a.b.d -write            # rename a field
acr -in:<file>                                  # read from a file
```

To rename a **ctype** or **ssimfile**, use `acr_ed` (below), not raw
`acr -rename`.  Renaming an ssimfile is a coordinated, ordered edit —
rename the like-named key field, then the ssimfile *with `-g -x`* (this
rewrites the data rows' attribute key **and moves the physical `.ssim`
file**), then the ctype, then `amc`.  A bare `acr ssimfile:<x>
-rename:<y> -write` renames only the record: it leaves the data rows
under the old attribute name and the file at its old path.

### Delete exactly the rows a list names
<a href="#delete-exactly-the-rows-a-list-names"></a>

A program that computes a set of rows to remove has a list of tuples, not a
pattern.  `-sel` reads tuples on stdin and selects the records they name, so
combining it with `-del` deletes that list and nothing else:

```bash
cat rows.acr | acr -sel -del -write -in:<root>   # n_delete equals the lines in rows.acr
```

Reaching for a key pattern instead is the trap, because a key prefix is not the
scope it looks like.  A node's env is the part of its name before the first dot,
so `nodeacctkey:aws1.%` matches every node under `aws1.sv0` as well — a delete
meant for one environment reaches another.  `%` spans separators, and the only
selection that means "these rows" is the rows.

### Validation — required after any edit
<a href="#validation-required-after-any-edit"></a>

`acr_ed -write` runs `amc` automatically — no further check needed on
success.  After `acr -merge -write`, `acr -insert -write`, or any
manual ssimfile edit, run:

```bash
acr -check % -x       # must exit 0; -x adds ssimreq checks
amc                   # must exit 0
```

After git add/remove/rename of tracked files, run `update-gitfile`
then `acr -check % -x`.

### Deleting a field leaves its values behind
<a href="#deleting-a-field-leaves-its-values-behind"></a>

`acr_ed -del -field amsdb.Proctype.hbtimeout -write` removes the field from
the schema and reruns `amc`.  Afterwards `data/amsdb/proctype.ssim` still
reads `amsdb.proctype  proctype:samp_meng  id:23  ns:samp_meng  ...
hbtimeout:30`.  The `hbtimeout:` attribute names a field that no longer
exists.  `acr -check % -x` passes, `amc` passes, and
`acr amsdb.proctype:samp_meng` prints the row without the attribute — so
every check agrees the edit is complete while the file still carries the
values.

The reason is that a delete edits the schema and nothing else.  `acr`
parses a data row against the current ctype and ignores an attribute it
does not recognize, which is what lets a tuple survive a schema that has
moved on; the same tolerance means a dead attribute is invisible rather
than rejected.  Nothing rewrites a data file until something asks it to.

So a field delete is followed by a rewrite of every ssimfile that holds
rows of the edited ctype:

```bash
acr '<ssimfile>:%' -write     # rewrite the file through the current schema
```

The rewrite drops the dead attributes and re-sorts the rows.  Skipping it
leaves text that reads as data and is not, and it survives every targeted
check; `bin/normalize` is what finally rewrites the file, so the omission
surfaces as a `normalize_acr` diff in CI rather than at the edit.

### Deleting a parent row makes its children unselectable
<a href="#deleting-a-parent-row-makes-its-children-unselectable"></a>

A doc file is renamed, so the two tables that track it have to follow.
`bin/update-gitfile` rescans the tree and drops the `dev.gitfile` row naming
the old path.  The `dev.readmefile` row naming that same path is then deleted
in the obvious way, and the report reads
`n_select:0  n_delete:0  n_file_mod:0` — nothing matched, nothing was
written.  The row is plainly still in `data/dev/readmefile.ssim`.  Yet the
next command that writes that ssimfile for any reason emits it without the
row, and the intended deletion has happened by accident.

The reason is that `dev.Readmefile.gitfile` is a `Pkey` into `dev.Gitfile`.
A `Pkey` is resolved at load, so once the parent row is gone the child row
does not enter the database at all: the selection has nothing to match,
which is why the delete reports zero rather than an error.  The write path
then serializes the database rather than the file it read, so the row that
never loaded is also never written back.

Two consequences worth separating.  A delete that reports `n_select:0` after
a parent was removed has not failed and has not succeeded — it has been
pre-empted, and the file on disk is stale until something rewrites it.  And
the order of the two deletes is what makes the outcome legible: delete the
child row first, while its parent still resolves and the report can say
`n_delete:1`, then rescan the parent table.  Reversed, the end state is the
same and no command ever claimed to produce it.

### acr_ed common operations
<a href="#acr_ed-common-operations"></a>

```bash
acr_ed -create -target <target> -write
acr_ed -create -finput -target <target> -ssimfile <ns>.<name> -write
acr_ed -create -finput -target <target> -ssimfile <ns>.<name> -indexed -write
acr_ed -create -field command.<proc>.<name> -arg <ctype> -dflt '""' -comment "..." -write
acr_ed -create -field <ns>.FCtype.<name> -arg <ctype> -write
acr_ed -create -ctype <ns>.FName -reftype Lary -write
acr_ed -create -ctype <ns>.FName -reftype Tpool -indexed -write
acr_ed -create -field <ns>.FParent.ind_child -arg <ns>.FChild -reftype Thash \
        -hashfld <ns>.FChild.key -xref -via <ns>.FChild.p_parent -write
acr_ed -create -srcfile txt/.../xyz.md -write
acr_ed -del    -srcfile txt/.../xyz.md -write
acr_ed -create -ctype ams.MyMsg -subset ams.MsgHeader -write
acr_ed -ctype <ns>.Old -rename <ns>.New -write          # rename ctype
acr_ed -field <ns>.Ct.old -rename <ns>.Ct.new -write    # rename field
acr_ed -ssimfile <ns>.old -rename <ns>.new -write        # rename ssimfile + key field + ctype + move file
```

`-reftype` is auto-inferred from name prefix: `ind_` → Thash,
`cd_` → Llist, `p_` → Upptr, `c_` → Ptrary.  See `acr fprefix`.

Full `acr_ed` reference:
[/txt/exe/acr_ed/README.md](/txt/exe/acr_ed/README.md).

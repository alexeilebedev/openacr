## Acr — Querying and Editing the Ssim Database
<a href="#acr-querying-and-editing-the-ssim-database"></a>

`acr` is the query and editing front-end for the ssim dataset.  Every table
under `data/` is reachable through it, and every tool that changes the schema
changes it by running `acr`.

This page is a cheatsheet.  Two other documents carry the rest:
[/txt/exe/acr/README.md](/txt/exe/acr/README.md) is the option-by-option
reference, and [/txt/rule/acr.md](/txt/rule/acr.md) states the rules a change
to the database is written against -- what a selection really covers, why a
deleted field leaves its values behind, and what a `-write` actually writes.

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

Both halves of the query accept `%` as a wildcard, and the first attribute of
each tuple is the primary key.  `%` spans separators, so a key prefix covers
more than it appears to; `-t` means `-tree -xref -loose` rather than "terse".
Both of those bite hardest on a delete, and
[/txt/rule/acr.md](/txt/rule/acr.md) says how.

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
cat rows.acr | acr -sel -del -write             # delete exactly the rows listed
acr <pkey> -rename:<newkey> -write              # rename pkey (data row)
acr field:a.b.c -rename:a.b.d -write            # rename a field
acr '<ssimfile>:%' -write -print:N              # rewrite a file through the current schema
acr -in:<file>                                  # read from a file
```

### Validation — required after any edit
<a href="#validation-required-after-any-edit"></a>

```bash
acr -check % -x       # must exit 0; -x adds ssimreq checks
amc                   # must exit 0
```

After a git add, remove or rename of a tracked file, run `update-gitfile`
first.  `acr_ed -write` runs `amc` itself, so an `acr_ed` edit that succeeded
needs no further check.

### Schema edits go through acr_ed
<a href="#schema-edits-go-through-acr_ed"></a>

`acr` edits records.  Creating, deleting or renaming a *schema* entity -- a
ctype, a field, an ssimfile, a target, a source file -- is a coordinated set of
record edits plus an `amc` run, and `acr_ed` is what performs the set.
Renaming an ssimfile, for instance, is four edits in a fixed order, and a bare
`acr ssimfile:<x> -rename:<y> -write` does one of them.

Use `acr` directly for queries, data edits and bulk rewrites.  Use `acr_ed`
whenever the shape of the schema changes.  Its examples and its
option-by-option reference are at
[/txt/exe/acr_ed/README.md](/txt/exe/acr_ed/README.md), and the rules it obeys
are at [/txt/rule/acr_ed.md](/txt/rule/acr_ed.md).

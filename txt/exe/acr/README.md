## acr - Algo Cross-Reference - ssimfile database & update tool
<a href="#acr"></a>

`acr` is the query and editing front-end for the ssim dataset under
`data/`.  It handles selection, mutation, transitive closure, referential
integrity checks, and writes changes back to disk.  Every OpenACR tool
reads its configuration from ssimfiles; `acr` is how those files are
inspected and modified.

### Table Of Contents
<a href="#table-of-contents"></a>
<!-- abt_md.toc_beg -->
&nbsp;&nbsp;&bull;&nbsp;  [Internals](#internals)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Syntax](#syntax)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Description](#description)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Quick reference](#quick-reference)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Querying](#querying)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Editing](#editing)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Validation](#validation)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [acr_in — Target inputs](#acr_in-target-inputs)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [acr_ed — Schema editor](#acr_ed-schema-editor)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Reading Stdin](#reading-stdin)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Sorting & RowIDs](#sorting-rowids)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [See Also](#see-also)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Options](#options)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
&#128196; [ACR release notes](/txt/exe/acr/relnotes.md)<br/>
<!-- abt_md.toc_end -->

### Internals
<a href="#internals"></a>
&#128196; [acr - Internals](/txt/gen/acr/acr.md)<br/>

### Syntax
<a href="#syntax"></a>
```
acr: Algo Cross-Reference - ssimfile database & update tool
Usage: acr [[-query:]<string>] [options]
    OPTION      TYPE    DFLT    COMMENT
    [query]     string  ""      Regx to match record
    -where...   string          Additional key:value pairs to match
    -in         string  "data"  Input directory or filename, - for stdin
    -del                        Delete found item
    -sel                        Read stdin and select records
    -insert                     Read stdin and insert tuples
    -replace                    Read stdin and replace tuples
    -update                     Read stdin and update attributes of existing tuples
    -merge                      Combination of -update and -insert
    -unused                     Only select records which are not referenced.
    -trunc                      Truncate table on first write
    -check                      Run cross-reference check on selection
    -selerr             Y       (with -check): Select error records
    -maxshow    int     100     Limit number of errors per table
    -write                      Write data back to disk.
    -rename     string  ""      Change value of found item
    -nup        int     0       Number of levels to go up
    -ndown      int     0       Number of levels to go down
    -l                          Go down via pkeys only
    -xref                       Short for -nup 100 -ndown 100
    -fldfunc                    Evaluate fldfunc when printing tuple
    -maxgroup   int     25      Max. items per group
    -pretty             Y       Align output in blocks
    -tree                       Print as tree
    -loose                      Allow printing a record before its references (used with -e)
    -my                         Invoke acr_my -e (using acr_my directly is faster)
    -schema     string  "data"  Directory for initializing acr meta-data
    -e                          Open selection in editor, write back when done.
    -t                          Short for -tree -xref -loose
    -g                          Trigger git commands for changes in dev.gitfile table
    -x                          Propagate select/rename/delete to ssimreq records
    -rowid                      Always print acr.rowid attribute
    -cmt                        Print comments for all columns referenced in output
    -report             Y       Show final report
    -print              Y       Print selected records
    -cmd        string  ""      Print script with command execution for each selected row
    -field...   string          Fields to select
    -regxof     string  ""      Single field: output regx of matching field values
    -meta                       Select meta-data for selected records
    -verbose    flag            Verbosity level (0..255); alias -v; cumulative
    -debug      flag            Debug level (0..255); alias -d; cumulative
    -help                       Print help and exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig
```

### Description
<a href="#description"></a>

Acr performs a fixed number of operations in a fixed order.  All
operations can be enabled or controlled with command-line flags.

The order of operations is:

1. Initial selection by `-query` regex.
2. Load files specified by `-in` (default: `data/`).
3. Process any input stream (`-sel`, `-insert`, `-merge`, `-replace`,
   `-update`).
4. Extend selection up (`-nup`).
5. Extend selection down (`-ndown`), optionally restricted by `-l`.
6. Select unused records (`-unused`).
7. Select meta-data (`-meta`) for current records.
8. Delete selected records (`-del`).
9. Check constraints (`-check`); `-x` also traverses `ssimreq` rules.
10. Edit the intermediate transaction in an editor (`-e`) or in
    MariaDB (`-my`).
11. Print results to stdout (`-print`, `-field`, `-regxof`).  Any record
    that is selected, modified, or deleted is printed.  Modified records
    are prefixed `acr.update`; deleted records are prefixed `acr.delete`.
    Print options: `-tree`, `-loose`, `-maxgroup`, `-rowid`, `-fldfunc`,
    `-cmt`.
12. Write to the dataset (`-write`); `-e` implies `-write`.
13. Execute the git script if `-g` is specified, or print it when the
    write did not happen.
14. Print the final report (`-report`): number of updates and modified
    files.

### Quick reference
<a href="#quick-reference"></a>

Common query patterns:

```bash
acr ns                               # list all namespaces
acr target                           # list build targets
acr ssimfile                         # list all ssim tables
acr ctype                            # all ctypes
acr field:<ctype>.%                  # fields of a ctype
acr field:command.<proc>.%           # command-line options for a process
acr field -where arg:u8              # fields by type (-where repeatable)
acr targdep:<target>.%               # target dependencies
acr dispatch_msg                     # dispatch handlers per process
acr xref                             # in-memory xrefs
acr %                                # dump everything
acr <pkey> -t -cmt                   # transitive closure with comments
```

Both halves of the query (`<ssimfile>:<pkey>`) accept `%` as a SQL
wildcard.  The first field in each tuple is the primary key.  If the
type tag is omitted, `%` is assumed — so `acr ctype` is the same as
`acr ctype:%`.

Common edit patterns:

```bash
echo '<ssim tuple>' | acr -insert -write       # insert a record
echo '<full tuple>' | acr -merge -write        # upsert (insert + update)
acr <ssimfile>:<pkey> -del -write              # delete a record
acr <pkey> -rename:<newkey> -write             # rename a pkey
# Field rename uses the FULL field pkey:
acr field:a.b.c -rename:a.b.d -write
```

After any edit run validation (see [Validation](#validation)).

### Querying
<a href="#querying"></a>

#### By primary key and type tag
<a href="#by-primary-key-and-type-tag"></a>

The query argument takes the form `<typetag>:<pkey>` where both halves
are SQL regexes (`%` = wildcard, `|` = alternation, `()` = grouping):

```bash
acr ns:algo_%           # namespaces whose name starts with "algo_"
acr ctype:%FDb          # ctypes whose name ends in "FDb"
acr %:x                 # any record in any table whose pkey is "x"
acr %                   # every record in the dataset
```

If only a type tag is given (no colon), `acr` matches it as a prefix
against both the type tag and the pkey, so `acr ns` and `acr ns:%`
are equivalent.

#### -where — Filtering on non-primary fields
<a href="#-where-filtering-on-non-primary-fields"></a>

`-where` is repeatable; all conditions must match:

```bash
acr field -where arg:algo.cstring           # fields of type cstring
acr field -where reftype:Thash              # all hash-index fields
acr field -where arg:u32 -where reftype:Val # u32 Val fields
```

#### Transitive closure
<a href="#transitive-closure"></a>

`-t` is the most-used option for exploring the schema.  It expands the
selection up and down through all pkey references and prints the result
as an indented tree:

```bash
acr ns:acr -t               # full schema tree for the acr namespace
acr ctype:algo_lib.FDb -t   # all fields, indexes, steps for FDb
```

More fine-grained control:

```bash
acr <query> -nup 2      # follow pkey references 2 levels up
acr <query> -ndown 3    # follow all back-references 3 levels down
acr <query> -xref       # same as -nup 100 -ndown 100
acr <query> -ndown 1 -l # only go down via primary-key references
```

The `-tree` flag turns the flat selected set into a visual tree even
without `-xref`.  `-loose` relaxes the print order so parents need not
appear before children.

#### Fldfunc expansion
<a href="#fldfunc-expansion"></a>

Some fields are computed substrings of the pkey (defined via
`dmmeta.substr`).  By default acr prints only the stored field.
`-fldfunc` also evaluates and prints the computed fields:

```bash
acr field:dmmeta.Field.% -fldfunc
```

This is automatically implied by `-my` (MariaDB integration).

#### Output formatting
<a href="#output-formatting"></a>

```bash
acr ns -pretty          # align output in rectangular blocks
acr ns -cmt             # include field comments on each printed tuple
acr field:<ns>.% -field arg     # print only the arg column, one per line
acr ns -regxof:nstype   # print a single regex matching all nstype values
acr ctype:<ns>.% -meta  # deselect ctypes, select their meta-records instead
```

`-cmd` generates a shell script that assigns every field's value to a
shell variable; pipe to `bash` to act on each row:

```bash
acr ns:acr -cmd 'echo Namespace is $ns'  | bash
```

### Editing
<a href="#editing"></a>

All mutating operations require `-write` to persist changes.

#### Insert, replace, update, merge
<a href="#insert-replace-update-merge"></a>

```bash
# Add a new record (fails silently if pkey already exists)
echo 'dmmeta.ns  ns:myns  nstype:exe  license:GPL  comment:""' \
    | acr -insert -write

# Replace a record entirely (missing fields get defaults)
echo '<full tuple>' | acr -replace -write

# Update only the specified attributes of existing records
echo '<partial tuple>' | acr -update -write

# Insert if new, update non-key attributes if exists (upsert)
echo '<tuple>' | acr -merge -write
```

Bulk updates (many records at once): build a file of tuples and pipe
it in:

```bash
cat changes.ssim | acr -merge -write
```

#### Delete
<a href="#delete"></a>

```bash
acr ns:myns -del -write             # delete one record
acr ns:myns -del -x -write          # also delete ssimreq-dependent records
acr ns:myns -del -x -g -write       # also issue git rm for tracked files
acr % -del -write                   # wipe the entire dataset
```

When deleting a record acr also deletes all records that refer to it
(cascade delete).

Deleting a `dmmeta.field` row removes the field from the schema and leaves its
values in the ssimfile, because `acr` ignores a data attribute that names no
field of the current ctype.  Drop them with a rewrite through the new schema —
`acr '<ssimfile>:%' -write -print:N` — or delete the field through
`acr_ed -del -field <field> -write`, which emits that rewrite itself.  See
[/txt/rule/acr.md](/txt/rule/acr.md).

#### Rename
<a href="#rename"></a>

`-rename` replaces the pkey value of the matched record and cascades to
all referencing records:

```bash
acr ns:old_name -rename:new_name -write
# Fields require the FULL pkey (ctype.fieldname):
acr field:myns.FRec.old_field -rename:myns.FRec.new_field -write
```

If the new pkey already exists (a merge/collision), the original record
is deleted and its children are re-parented.  Combined with `-g`, file
renames are issued to git:

```bash
acr ns:old_ns -rename:new_ns -g -write
```

Renaming an **ssimfile** is more than renaming its record: the data rows keep
the old attribute name and the file keeps its old path unless `-g -x` is given,
and the like-named key field and the ctype have to follow in order.
`acr_ed -ssimfile <old> -rename <new> -write` performs the whole sequence.  See
[/txt/rule/acr.md](/txt/rule/acr.md).

#### Editor workflow (-e)
<a href="#editor-workflow-e-"></a>

`-e` opens the current selection in `$EDITOR` just before writing.
The round-trip is: select → display → edit → write.  Abort by killing
the editor or deleting `temp/acr.ssim`.

```bash
acr ns:myns -t -e        # open the full subtree for editing
acr field:<ns>.% -e      # edit all fields of a namespace
```

`-e` implies `-write`.

The write-back deletes the selection and re-creates it from the buffer,
so a line the editor hands back that acr cannot turn into a record
takes that record with it.  Two ways a line can fail are reported as
errors: a line whose quoting does not close, and a line whose type tag
names no table acr knows.  Either one is named with its file, its line
number and its text, and the run then writes no ssimfile and exits
nonzero, so every file still holds what it held before the session
started.  A table that could not be read refuses the write the same
way and for the same reason.

A line whose primary key attribute was deleted is reported as a
warning rather than an error.  The line still names a type, so acr
knows what kind of record it is, but nothing on it says which record
it is.  Such a line is dropped, and the rest of the session writes, so
the record it came from is no longer in its ssimfile.  Read the record
counts on the final `report.acr` line when a session reports a missing
primary key.

`temp/acr.ssim` carries the buffer for the duration of the session and
is removed when the session ends, including a session that refused the
write.  It is left on disk when the run stops before that point --
the editor exiting nonzero, or a dataset file changing while the
editor was open.

The exit code reports success or failure, as in every other mode: 0
when the edit was applied (whether or not any file changed), nonzero
when it was not (editor failure, a dataset file changed during the
edit, a line reported as an error, a failed write).  The number
of files modified is carried by the `n_file_mod` attribute of the final
`report.acr` line.

#### MariaDB workflow (-my)
<a href="#mariadb-workflow-my-"></a>

`-my` opens the selection in a temporary MariaDB instance.  When the
shell exits the data is written back.  Every ssim namespace maps to a
MariaDB database; each ssimfile maps to a table.  `-my` implies
`-fldfunc` (computed fields appear as regular columns) and `-write`.

```bash
acr ns:dmmeta -my       # browse/edit the dmmeta namespace in SQL
# Or run a one-shot SQL expression:
echo "UPDATE ctype SET comment='Updated' WHERE ns='acr'" \
    | acr -my %
```

#### Git integration (-g)
<a href="#git-integration-g-"></a>

When the selection includes `dev.gitfile` records, `-g` issues `git mv`
and `git rm` commands for any renames or deletions.  Without `-write`
the script is printed to stdout instead of executed.  A script that runs
and returns nonzero fails the run.

```bash
# Rename a namespace and move all its files in git:
acr ns:old_ns -del -x -g -write
```

### Validation
<a href="#validation"></a>

After any manual ssimfile edit (or `acr -insert/merge/update/delete
-write`) run:

```bash
acr -check % -x       # referential integrity + ssimreq; must exit 0
amc                   # code generator; must exit 0
```

`-check` deselects valid records and leaves bad ones so you can see
exactly what's wrong.  `-x` adds `ssimreq` constraints on top of pkey
checks.  With `-check -del` bad records are removed.  With `-check -e`
bad records are opened for editing.

`acr_ed -write` runs `amc` automatically on success — no manual check
needed.  After adding or removing git-tracked files, run:

```bash
update-gitfile        # reconcile dev.gitfile with what git tracks
acr -check % -x      # verify
```

`gstatic` tables (`acr dmmeta.gstatic`) compile into C++ global arrays
via `amc`.  Changes to them take effect only after `amc` + rebuild.

### acr_in — Target inputs
<a href="#acr_in-target-inputs"></a>

`acr_in` is a standalone tool that answers "which ssimfiles does a target
read?".  `acr` itself has no dependency on `acr_in`.

Full reference: [/txt/exe/acr_in/README.md](/txt/exe/acr_in/README.md).

### acr_ed — Schema editor
<a href="#acr_ed-schema-editor"></a>

`acr_ed` is a standalone helper that generates scripts calling `acr` with
`-insert`, `-del` or `-rename`, plus `amc`.  Use it for schema
modifications (new ctypes, fields, targets, source files).  `acr` has no
dependency on `acr_ed` — `acr_ed` just automates the same `acr` + `amc`
calls you could do by hand.

Full reference: [/txt/exe/acr_ed/README.md](/txt/exe/acr_ed/README.md).

### Reading Stdin
<a href="#reading-stdin"></a>

The options `-insert`, `-replace`, `-merge`, `-sel` all enable reading
of stdin for a list of tuples.  Lines in the input stream can override
the setting on the command line.  The following table shows the possible
prefixes:

```
inline-command: acr fconst:acr.ReadMode.read_mode/% -field name,comment
acr.insert	Insert new record only
acr.replace	Replace record with input
acr.update	Merge existing attributes only
acr.merge	Create new record & merge attributes
acr.delete	Delete record
acr.select	Select found record
```

To illustrate, invoking `acr -insert` and then providing the lines
```
acr.delete <tuple>
acr.merge <tuple>
```
performs the corresponding actions regardless of the `-insert` flag.

### Sorting & RowIDs
<a href="#sorting-rowids"></a>

Acr always saves files in sorted order.  Sorting is controlled by the
`ssimsort` table, which is a subset of `ssimfile`.  Sorting is optional.
If `ssimsort` is missing or doesn't specify the primary key of the
table, the set is *order-dependent*.  When sorting is enabled, it can
be done on any fields, including a fldfunc.

To reorder records manually, use `-rowid`:

```bash
acr <pat> -rowid          # prints acr.rowid:<float> per record
# edit the rowid values, then:
acr -merge -write
```

For fields, `acr_ed -create -field ... -before <existing>` is usually
simpler.

### See Also
<a href="#see-also"></a>

* [acr_ed](/txt/exe/acr_ed/README.md) — schema editor; wraps `acr` and runs `amc`
* [acr_my](/txt/exe/acr_my/README.md) — open ssimfiles in MariaDB
* [acr_in](/txt/exe/acr_in/README.md) — extract inputs a target reads
* [mysql2ssim](/txt/exe/mysql2ssim/README.md) — convert MySQL dump to ssimfiles
* [ssim2mysql](/txt/exe/ssim2mysql/README.md) — convert ssimfiles to MySQL
* [amc](/txt/exe/amc/README.md) — code generator driven by the ssim schema
* [Ssim Fundamentals](/txt/openacr/ssim.md) — ssim tuple format, fldfunc, cross-references
* [OpenACR: the rules](/txt/rule/openacr.md)

### Options
<a href="#options"></a>
#### -query -- Regx to match record
<a href="#-query"></a>

This option controls initial record selection.
A SQL-like regular expression of the form `<ssimfile>:<key>` or `<ssimfile.fieldname>:<key>`.
The wildcard character is `%`, and characters `|`, `(`, `)` are also supported.
If `<key>` is omitted, it is assumed to be `%`.
For instance, `acr ctype` prints the ctype table. `acr %:x` prints any record whose primary key is `x`.
And `acr %` prints the entire dataset.

#### -where -- Additional key:value pairs to match
<a href="#-where"></a>

This option can be supplied any number of times. The argument is `<key>:<val>`, both
key and val are regexes. Before adding a record to the selected
set, acr checks that the tuple matches all of `-where`s specified on the command line.

#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

Specify input dataset path.
If -in refers to a directory, then ssimfiles are assumed to follow the standard layout NS/NAME.ssim.
If -in is "-", the dataset is loaded from stdin.

#### -del -- Delete found item
<a href="#-del"></a>

With `-del`, any records that are selected are deleted.
The deletion proceeds recursively, deleting any dependent records.
`-del` works with any number of records. You could delete the entire database with
`acr % -del -write`. When deleting a record, acr also deletes any dependent records.

When deleting a field, acr automatically opens and rewrites the corresponding ssimfile
so that the deleted column disappears.

#### -sel -- Read stdin and select records
<a href="#-sel"></a>

Read tuples from stdin. Any tuples found in input are selected.
Feeding a specific set of initial records to select can be useful as an alternative to trying to
construct a regex query on the command line.

#### -insert -- Read stdin and insert tuples
<a href="#-insert"></a>

Read tuples from stdin, and create new records in the dataset.
Existing records are untouched.

#### -replace -- Read stdin and replace tuples
<a href="#-replace"></a>

Read tuples from stdin. Each new tuple completely overrides the previous tuple with
the same key. Any fields that aren't specified are assigned default values.

#### -update -- Read stdin and update attributes of existing tuples
<a href="#-update"></a>

Read tuples from stdin. No new records will be created. Any attributes
from input records will be used to update existing attributes in the data set.
Attributes not specified on input will retain their original values in the data set.

#### -merge -- Combination of -update and -insert
<a href="#-merge"></a>

Read tuples from stdin.
If a new record is found on input, it is inserted as with insert. When a record being inserted
exists in the dataset, any attributes from the new record replace attributes in the original records.
This behavior is sometimes called 'upsert'.

#### -unused -- Only select records which are not referenced.
<a href="#-unused"></a>

#### -trunc -- Truncate table on first write
<a href="#-trunc"></a>

With this option, when the first change is made to the table, the table is first wiped.

#### -check -- Run cross-reference check on selection
<a href="#-check"></a>

Check the selected set for referential integrity. Any good records
are de-selected, and bad records are left in the selected set.

With `-check -del`, acr deletes any bad records (and any of their dependencies).

With `-check -e`, bad records can be opened for editing.

#### -selerr -- (with -check): Select error records
<a href="#-selerr"></a>

If this option is specified (which is the default), the behavior of `-check` is to de-select
all valid records and leave only bad records as selected. If this option is set to false,
`-check` only displays errors but leaves selection untouched.

#### -maxshow -- Limit number of errors per table
<a href="#-maxshow"></a>

#### -write -- Write data back to disk.
<a href="#-write"></a>

#### -rename -- Change value of found item
<a href="#-rename"></a>

Replace the value of any attribute matching the command-line query
to the specified value. If a collision occurs, or multiple records match selection,
the source record (record being renamed) is deleted, while all of its children (records
reachable with `-ndown`) are still renamed recursively. So this option
can be used to merge any two record trees. In combination with `-g`, this option
can also be used to move git files within the tree. Directories for target files
will be created automatically.

#### -nup -- Number of levels to go up
<a href="#-nup"></a>

Extend initial selection up NUP times.
With each step, we follow Pkey references from any currently selected record and add the target
record to the selected set.

#### -ndown -- Number of levels to go down
<a href="#-ndown"></a>

Extend initial selection down NDOWN times.
With each step, those records that refer to any of the selected records are added to the selected set.

#### -l -- Go down via pkeys only
<a href="#-l"></a>

Left transitive closure.
This option controls how acr processes `-ndown`.
By default, any record referring to a selected record is added to the selected set. With `-l`,
only references from the primary key cause a record to be added to the set.

#### -xref -- Short for -nup 100 -ndown 100
<a href="#-xref"></a>

#### -fldfunc -- Evaluate fldfunc when printing tuple
<a href="#-fldfunc"></a>

Without this flag, only physical fields (those present in the ssimfile) are printed.
With this flag, all substring fields are evaluated and printed.

#### -maxgroup -- Max. items per group
<a href="#-maxgroup"></a>

Number of rows per aligned group (with `-pretty`).

#### -pretty -- Align output in blocks
<a href="#-pretty"></a>

Align output records in rectangular blocks of `-maxgroup` lines. Whitespace is inserted to keep
attributes aligned. Without `-pretty`, tuples are printed with no justification.

#### -tree -- Print as tree
<a href="#-tree"></a>

Invoke an algorithm that constructs a tree out of records such that any records
whose key extends a parent record are shown immediately below and indented.

#### -loose -- Allow printing a record before its references (used with -e)
<a href="#-loose"></a>

By default, acr will not print a record until it prints all records to which this record refers.
With `-loose`, acr will ignore that restriction, resulting in better visual grouping
to the human eye.

#### -my -- Invoke acr_my -e (using acr_my directly is faster)
<a href="#-my"></a>

Launch an instance of mariadb and import the selection as databases.
It then drops you into a mariadb shell, where you can apply needed transformations.
Upon exit, the data is downloaded from the database back to disk. `-my` implies `-write`.
The option `-my` also implies `-fldfunc`, so that all computed fields become available inside MariaDB
as regular columns (but they won't be saved back).

All ssim tables are compatible with MariaDB. The namespace (ns) corresponds to a database in MariaDB,
and ssimfile corresponds to a table.

You can also apply SQL expressions directly from the command line by running `echo "<sql expression>" | acr -my %`.
Any effects of the SQL expression will be visible via `git diff` upon completion of the command.

#### -schema -- Directory for initializing acr meta-data
<a href="#-schema"></a>

Specify where to load acr schema tables (ctype, field, etc).
By default, these tables are loaded from the `"data"` directory.

#### -e -- Open selection in editor, write back when done.
<a href="#-e"></a>

Launch an editor (as specified with the environment
variable EDITOR), right before applying the transaction, allowing you to edit the transaction.
When the editor exits, whatever tuples were in the original selection are deleted from
the ssim files and replaced with the edited output. To abort editing,
kill the editor and remove temporary file `temp/acr.ssim`.

#### -t -- Short for -tree -xref -loose
<a href="#-t"></a>

The initial set of selected records
is extended up and down to include all references, and is then displayed as a tree.

#### -g -- Trigger git commands for changes in dev.gitfile table
<a href="#-g"></a>

Issue `git rm` and `git mv` commands for any changes made to the `dev.gitfile` table.
If you want to rename or delete a namespace, all of its source files, component tests, in one operation,
use `acr ns:XYZ -del -x -g -write`.
If `-write` is not specified, the output script is printed to stdout instead of being executed.
The same happens whenever the ssimfiles do not reach disk -- an input that could not be read,
or a write the filesystem refuses: acr prints the script rather than executing it, matching
the ssimfiles it does not rewrite.
A script that does execute and returns nonzero fails the run, and acr prints the script it ran.
This is what a rename needs: by the time the script runs the ssimfiles already name the
destination, so a refused `git mv` would otherwise leave the worktree on the old path with
acr reporting success.

#### -x -- Propagate select/rename/delete to ssimreq records
<a href="#-x"></a>

This option extends cascade delete, update and delete steps to include any rules
specified in the `ssimreq` table.

#### -rowid -- Always print acr.rowid attribute
<a href="#-rowid"></a>

#### -cmt -- Print comments for all columns referenced in output
<a href="#-cmt"></a>

The -cmt option displays any comments associated with the current selection, which includes comments attached to the displayed fields.

#### -report -- Show final report
<a href="#-report"></a>

Specify whether the final acr report (`report.acr`) is shown.

#### -print -- Print selected records
<a href="#-print"></a>

Specify whether to print selected records. Default is true.

#### -cmd -- Print script with command execution for each selected row
<a href="#-cmd"></a>

The `-cmd` option produces an executable shell script which should be piped to `bash`.
For each record in the final selection, acr outputs variable assignment statements, giving the
shell script access to the values of all field attributes (including any computed fields), the tuple
itself (`acr_tuple`), the type tag (`acr_head`), and the rowid (`acr_rowid`). The script can then use
whatever other Unix tools it needs to.

Since `-cmd` just outputs a script, the output can be consumed with a single process.
One command per output row would have been much slower.

#### -field -- Fields to select
<a href="#-field"></a>

This option can be provided multiple times.
If this option is specified, then only the specified fields are printed, one per line,
instead of the entire tuple.

#### -regxof -- Single field: output regx of matching field values
<a href="#-regxof"></a>

Construct a regular expression matching the values of specified attribute (`-regxof:<FIELDNAME>`)
in the selected set.

#### -meta -- Select meta-data for selected records
<a href="#-meta"></a>

Deselect any selected records and select their meta-data instead.
`-meta` implies `-t`.

### Inputs
<a href="#inputs"></a>
`acr` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|
|[dmmeta.anonfld](/txt/ssimdb/dmmeta/anonfld.md)|Omit field name where possible (command line, enums, constants)|
|[amcdb.bltin](/txt/ssimdb/amcdb/bltin.md)|Specify properties of a C built-in type|
|[dmmeta.cdflt](/txt/ssimdb/dmmeta/cdflt.md)|Specify default value for single-value types that lack fields|
|[dmmeta.cppfunc](/txt/ssimdb/dmmeta/cppfunc.md)|Value of field provided by this expression|
|[dmmeta.ctype](/txt/ssimdb/dmmeta/ctype.md)|Struct|
|[dmmeta.field](/txt/ssimdb/dmmeta/field.md)|Specify field of a struct|
|[dmmeta.funique](/txt/ssimdb/dmmeta/funique.md)|This field must be unique in the table. Not needed for primary key|
|[dmmeta.smallstr](/txt/ssimdb/dmmeta/smallstr.md)|Generated fixed-length padded or length-delimited string field|
|[dmmeta.ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|File with ssim tuples|
|[dmmeta.ssimreq](/txt/ssimdb/dmmeta/ssimreq.md)|Extended constraints for ssim records|
|[dmmeta.ssimsort](/txt/ssimdb/dmmeta/ssimsort.md)|Define sort order for ssimfile|
|[dmmeta.substr](/txt/ssimdb/dmmeta/substr.md)|Specify that the field value is computed from a substring of another field|

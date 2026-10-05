## acr - Algo Cross-Reference - ssimfile database & update tool
<a href="#acr"></a>

`acr` queries and edits the ssim dataset under `data/`.  It selects records by
key, follows the references between them, checks referential integrity, and
writes changes back to the ssimfiles.  Every OpenACR tool reads its
configuration from ssimfiles, and `acr` is how you inspect and change them.

### Syntax
<a href="#syntax"></a>
```usage
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
    -trace      string  ""      Trace expression: category[:filter],...; also payload_lim:N, verbose, debug, timestamps
    -help                       Print help and exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig
```

### Description
<a href="#description"></a>

`acr` loads the dataset lazily, one ssimfile at a time, as the query reaches each
table.  It reads the schema (`dmmeta.ctype`, `dmmeta.field` and the tables
around them) from the directory named by `-schema`, and the data from `-in`.
Without `-write` a run changes nothing on disk, so any editing command doubles
as a dry run that prints the records it would insert, update or delete.

#### Order of operations
<a href="#order-of-operations"></a>

Each run performs the same steps in the same order, and the flags switch the
steps on or tune them:

1. Load the dataset named by `-in`, and read tuples from stdin when `-sel`,
   `-insert`, `-replace`, `-update` or `-merge` is given.
2. Select the records matching the query and every `-where`, and apply `-rename`.
3. Extend the selection up (`-nup`), then down (`-ndown`, restricted by `-l`).
4. Narrow it to unreferenced records (`-unused`), or swap it for the schema
   records that describe it (`-meta`).
5. Mark the selection deleted (`-del`), along with every record that refers to
   a deleted one.
6. Check referential integrity (`-check`, with `ssimreq` rules under `-x`).
7. Open the selection in an editor (`-e`) or in MariaDB (`-my`).
8. Print the result (`-print`, `-field`, `-regxof` or `-cmd`).  Changed records
   carry an `acr.insert`, `acr.update` or `acr.delete` prefix.
9. Write the changed ssimfiles (`-write`).
10. Run the git script (`-g`), or print it when the write did not happen.
11. Print the `report.acr` line (`-report`).

#### Selecting records
<a href="#selecting-records"></a>

The query takes the form `<ssimfile>:<pkey>`, and both halves are SQL-style
patterns.  `%` is the wildcard, and `|` and `()` build alternations.  Leave out
the namespace and `acr` searches every namespace for a table of that name, so
`acr ns` and `acr dmmeta.ns:%` print the same rows.  Leave out the key and `acr`
assumes `%`.  The form `<ssimfile>.<field>:<value>` matches a field other than
the primary key.

`-where` adds a condition on any field, and you can repeat it.  A record is
selected only when it matches the query and every `-where`.

#### Following references
<a href="#following-references"></a>

A field whose type is another table's record is a reference.  `-nup` follows
references from the selection to the records they name, and `-ndown` adds the
records that refer back to the selection.  Each takes a number of levels, and
`-xref` sets both to 100.  `-l` limits the downward walk to records whose
primary key carries the reference.  `acr ctype:dmmeta.Ns -ndown 1` adds ~70
records, among them every field in the tree whose type is `dmmeta.Ns`.  With
`-l` it selects 9: the ctype, its own fields and the records keyed by it.

`-t` is the usual way to explore the schema.  It expands the selection in both
directions and prints it as an indented tree, so `acr ctype:acr.FDb -t` shows
the ctype with its namespace above it and its fields, indexes and steps below.

#### Printing
<a href="#printing"></a>

By default `acr` prints each selected record as a tuple and aligns the tuples of
one table in blocks.  `-field` prints the named columns of each record on one
line, separated by tabs, and `-regxof` prints one pattern that matches every value of a column.
`-cmt` adds a comment block that explains each column of the printed tables.
`-fldfunc` adds the computed fields, the ones `dmmeta.substr` derives from the
key, which the ssimfile does not store.

`-cmd` turns the selection into a shell script.  For each record it assigns
every field to a shell variable of the same name, and then appends your
command.  Pipe the script to `bash` to act on each row.

#### Editing through stdin
<a href="#editing-through-stdin"></a>

`-insert`, `-replace`, `-update` and `-merge` read ssim tuples on stdin and
apply them to the dataset, and `-sel` selects the records they name.  Each
flag sets the default action, and a line can override it with a prefix:

```ssim
inline-command: acr fconst:acr.ReadMode.read_mode/% -field name,comment
acr.insert	Insert new record only
acr.replace	Replace record with input
acr.update	Merge existing attributes only
acr.merge	Create new record & merge attributes
acr.delete	Delete record
acr.select	Select found record
```

So `acr -insert` fed `acr.delete <tuple>` on one line and `acr.merge <tuple>` on
the next deletes the first record and merges the second.  An insert of a key
that already exists changes nothing and counts as `n_ignore` in the report.

`acr` writes each ssimfile in sorted order, so a row inserted this way lands in
its correct place and the diff carries only that row.

#### Deleting
<a href="#deleting"></a>

`-del` deletes the selection, and it also deletes every record that refers to a
deleted record, recursively.  `-x` extends the cascade to the records that
`ssimreq` rules tie to the selection.  `acr ns:<ns> -del -x -g -write`
therefore removes a namespace along with its ctypes, fields, target and source
files.

#### Renaming
<a href="#renaming"></a>

`-rename:<new>` replaces the key of the selected record and updates every
record that refers to it.  A field is renamed by its full key, so write
`acr field:a.B.c -rename:a.B.d`.  When the new key already exists, `acr` deletes
the renamed record and moves its children under the existing one, which merges
the two trees.  With `-x`, `ssimreq` rules carry the rename to the tied records,
and with `-g` a renamed `dev.gitfile` row becomes a `git mv`.

#### Editing in an editor
<a href="#editing-in-an-editor"></a>

`-e` writes the selection to `temp/acr.ssim`, with comments and row ids, and
opens it in `$EDITOR`.  When the editor exits, `acr` deletes the original
selection and reads the buffer back in replace mode, then writes the result.
`-e` implies `-write`, so you can delete, add and change records in one
session.  Abort by exiting the editor with a nonzero status, then remove
`temp/acr.ssim`.

A line that `acr` cannot turn into a record takes its record with it, so the
read-back guards against the common mistakes.  A line whose quoting does not
close, or whose type tag names no table, is an error, reported with its file,
line number and text.  The run then writes no ssimfile and exits nonzero, so
every file keeps what it held before the session.  A table that could not be
read refuses the write the same way.  A line whose primary key attribute was
deleted draws a warning, and the run drops that line and writes the rest.  When
you see that warning, compare the record counts on the final `report.acr` line
with what you expected.

`acr` removes `temp/acr.ssim` when the session ends, including a session that
refused the write.  The file stays when the run stops earlier, because the
editor exited nonzero or a dataset file changed while the editor was open.  The
exit code is 0 when the edit was applied, whether or not any file changed, and
`n_file_mod` on the `report.acr` line counts the files it modified.

#### Editing in MariaDB
<a href="#editing-in-mariadb"></a>

`-my` runs [acr_my](/txt/exe/acr_my/README.md) `-e` with the computed fields
included.  It starts a private MariaDB server, loads the namespaces that the
query names, and drops you into a `mysql` shell where each namespace is a
database and each ssimfile a table.  When the shell exits, `acr_my` saves the
tables back to the ssimfiles and stops the server.  `-my` passes the query to
`acr_my` as a namespace pattern, so write `acr -my dmmeta`, and `%` for every
namespace.  Pipe SQL on stdin to run it without a shell, and read the effect
with `git diff`.

#### Git integration
<a href="#git-integration"></a>

`-g` turns changes to `dev.gitfile` rows into a git script: a deleted row
becomes `git rm --force`, a renamed row becomes `git mv`, and a new row becomes
`touch` plus `git add`.  `acr` runs the script only after its own write went
through.  Otherwise, including every run without `-write`, it prints the script.
A script that runs and returns nonzero fails the run, and `acr` prints the
script it ran.

#### Validation
<a href="#validation"></a>

`-check` tests the selection for referential integrity.  It deselects every
valid record and leaves the bad ones selected, each with a message naming the
field, its bad value and the valid values.  `-x` adds the `ssimreq` rules.  A
check that finds errors makes the run exit nonzero and blocks `-write`, so a
run that inserts and checks writes nothing when the check fails.  `-check -e`
opens the bad records in the editor.  `-check` does not narrow `-del`, which
deletes the whole selection before the check runs.

After an edit made with raw `acr`, run both gates.  After adding, removing or
renaming a git-tracked file, run `update-gitfile` first.

```bash
acr -check % -x       # referential integrity plus ssimreq rules; must exit 0
amc                   # the code generator must also accept the schema
```

#### Sorting and row ids
<a href="#sorting-and-row-ids"></a>

A `dmmeta.ssimsort` row names the field an ssimfile is sorted by.  `acr` writes
the file ordered by that field, and within equal values by row id, which is the
order in which the records were read.  A file with no `ssimsort` row keeps its
row order.  `dmmeta.field` sorts by ctype alone, so the order of fields within a
ctype is the member order of the generated struct.

`-rowid` prints each record's row id as `acr.rowid`, and it leaves the attribute
out where the file sorts by its primary key.  A row id is a number with a
fraction, so you can move a record between two others by giving it a value in
between and feeding the tuple back through `-merge`.  To place a new field,
`acr_ed -create -field ... -before <field>` is simpler.

### See also
<a href="#see-also"></a>

* [acr_ed](/txt/exe/acr_ed/README.md): the schema editor, which composes `acr` edits and runs `amc`
* [acr_in](/txt/exe/acr_in/README.md): lists the ssimfiles and tuples a target reads
* [acr_my](/txt/exe/acr_my/README.md): opens ssimfiles in a private MariaDB server
* [acr_compl](/txt/exe/acr_compl/README.md): bash completion for every tool's command line
* [mysql2ssim](/txt/exe/mysql2ssim/README.md) and [ssim2mysql](/txt/exe/ssim2mysql/README.md): convert between ssim and MySQL
* [amc](/txt/exe/amc/README.md): the code generator driven by the ssim schema
* [Ssim fundamentals](/txt/openacr/ssim.md): the tuple format, computed fields and references
* [OpenACR: the rules](/txt/rule/openacr.md)

### Examples
<a href="#examples"></a>

```bash
acr ns                                  # list every namespace
acr ssimfile:dmmeta.%                   # the tables of the dmmeta namespace
acr field:dmmeta.Ns.%                   # the fields of one ctype
acr field:command.acr.%                 # the command-line options of acr
acr %:acr_in                            # every record, in any table, whose key is acr_in
acr dmmeta.ns.nstype:lib                # namespaces whose nstype is lib
acr field -where arg:u32 -where reftype:Val   # u32 value fields, two conditions
acr 'ns:acr_(in|my)' -field:ns          # print one column, one value per line
acr ns -regxof:nstype                   # a pattern matching every nstype in use
acr ns:acr -cmt                         # the record plus a comment for each column
acr ctype:acr.FDb -t                    # a ctype with its parents and children, as a tree
acr ctype:acr.FDb -meta                 # the schema of the table the record lives in
acr nstype -unused                      # nstypes that no namespace uses
acr field:acr.FDb.% -rowid              # show row ids, to reorder fields
acr ns:acr -cmd 'echo Namespace is $ns' | bash   # run a command per record
```

Editing commands print what they would do until you add `-write`:

```bash
echo 'dmmeta.ns  ns:myns  nstype:exe  license:GPL  comment:""' | acr -insert -write   # add a record
echo 'dmmeta.ns  ns:myns  comment:"New comment"' | acr -update -write   # change one attribute
echo '<tuple>' | acr -merge -write          # insert, or update if the key exists
cat rows.ssim | acr -sel -del -write        # delete exactly the records listed in a file
acr ns:myns -del -write                     # delete a record and everything referring to it
acr ns:myns -del -x -g -write               # also its ssimreq-tied records and git files
acr ns:myns -rename:yourns -write           # rename a key everywhere it is used
acr field:myns.FDb.a -rename:myns.FDb.b -write   # rename a field by its full key
acr ns:myns -t -e                           # edit a namespace's whole tree in $EDITOR
echo "select count(*) from field where arg='u8'" | acr -my dmmeta   # one SQL statement
```

Run a command without `-write` to see its effect first.  Here a rename reports
37 records it would change and writes nothing:

```bash
$ acr ns:acr_my -rename:acr_mysql | tail -2
acr.insert  gendb.dispsig  dispsig:acr_mysql.Input  signature:4af1104b912fd0ce532858d685766efe3c9836a6
report.acr  n_select:37  n_insert:0  n_delete:0  n_ignore:0  n_update:37  n_file_mod:0  n_badline:0
```

### Caveats
<a href="#caveats"></a>

- **Never combine `-t` with `-del -write`.**  `-t` means `-tree -xref -loose`,
  so it expands the selection to the whole cross-reference tree, and the
  delete then removes that tree.  For a scoped delete, run
  `acr <ssimfile>:<pat> -del -write` without `-t`.  It removes the matched rows
  plus the rows that refer to them.
- `%` spans separators.  `acr ns:acr%` selects `acr_compl`, `acr_dm`, `acr_ed`,
  `acr_in` and `acr_my` along with `acr`.  Write a delete against the rows you
  mean, or pipe an exact list of tuples to `-sel`.
- An alternation cannot span a dot.  `acr 'dmmeta.(ns|ctype)'` works, and
  `acr '(dmmeta.ns|dmmeta.ctype)'` selects nothing.
- A query with no colon names a table, never a key.  `acr acr_in` selects
  nothing, and `acr %:acr_in` finds the records whose key is `acr_in`.
- A refused write still echoes the edit, so the output can read like success.
  Read `n_file_mod` on the `report.acr` line to know whether a file changed.
  `-check` refuses a row with a dangling reference (`acr.badrefs`) or an
  over-long attribute (`acr.attr_too_long`).
- Deleting a `dmmeta.field` row leaves the field's values in the ssimfile,
  because `acr` ignores an attribute that names no field.  Rewrite the file
  through the new schema with `acr '<ssimfile>:%' -write -print:N`, or delete the
  field with [acr_ed](/txt/exe/acr_ed/README.md), which does this for you.
- Renaming an ssimfile record leaves the data rows and the file path behind.
  Use `acr_ed -ssimfile <old> -rename <new> -write`, which performs the whole
  sequence.
- Add rows through `-insert` and never by editing a file.  A hand-placed row
  in the wrong position passes `amc` and `acr -check`, and only `bin/normalize`
  objects.
- `-e` refuses to start while `temp/acr.ssim` exists, since the file may be
  another session's buffer or a failed session's backup.  Recover what you need
  from it, then remove it.

See [/txt/rule/acr.md](/txt/rule/acr.md) for the reasoning behind these rules.

### Options
<a href="#options"></a>
#### -query -- Regx to match record
<a href="#-query"></a>

Select the initial records, as `<ssimfile>:<key>` or `<ssimfile>.<field>:<value>`.
Both halves are patterns with `%` as the wildcard and `|` and `()` for
alternation.  A missing key means `%`, and a missing namespace means any
namespace, so `acr ctype` prints the ctype table and `acr %` prints the whole
dataset.

#### -where -- Additional key:value pairs to match
<a href="#-where"></a>

Add a condition `<field>:<value>`, where both halves are patterns.  Repeat the
flag for more conditions; a record is selected only when it matches all of
them and the query.

#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

Name the dataset.  A directory holds ssimfiles in the standard layout
`<ns>/<name>.ssim`, a file holds tuples of any tables, and `-` reads the
dataset from stdin.  With a file, `-write` rewrites that file as a whole.

#### -del -- Delete found item
<a href="#-del"></a>

Delete the selected records, and recursively every record that refers to a
deleted one.  Add `-x` to follow `ssimreq` rules as well, and `-g` to remove
the files of deleted `dev.gitfile` rows.  `-check` does not narrow the
deletion: `acr ns:acr -check -del` deletes what plain `-del` deletes.

#### -sel -- Read stdin and select records
<a href="#-sel"></a>

Read tuples on stdin and select the records they name.  Use it when a program
has already computed the exact set, since a list of tuples cannot overreach the
way a pattern can.

#### -insert -- Read stdin and insert tuples
<a href="#-insert"></a>

Read tuples on stdin and add them as new records.  A tuple whose key already
exists leaves the existing record untouched and counts as `n_ignore`.

#### -replace -- Read stdin and replace tuples
<a href="#-replace"></a>

Read tuples on stdin, and replace each record with the same key by the new
tuple.  Fields the tuple leaves out take their default values.  A tuple with a
new key is inserted.

#### -update -- Read stdin and update attributes of existing tuples
<a href="#-update"></a>

Read tuples on stdin and set the given attributes on existing records.
Attributes the tuple leaves out keep their values, and a tuple whose key does
not exist is ignored.

#### -merge -- Combination of -update and -insert
<a href="#-merge"></a>

Read tuples on stdin.  A tuple with a new key is inserted, and a tuple with an
existing key updates the attributes it carries, which is sometimes called an
upsert.

#### -unused -- Only select records which are not referenced.
<a href="#-unused"></a>

Keep only the selected records that no other record refers to.  Use it to find
dead rows, such as `acr nstype -unused` for namespace types nothing uses.  It
overrides `-nup` and `-ndown`.

#### -trunc -- Truncate table on first write
<a href="#-trunc"></a>

Delete a table's existing records the first time stdin inserts into it.  Pipe
a whole table through `-insert -trunc -write` to replace its contents with the
input.

#### -check -- Run cross-reference check on selection
<a href="#-check"></a>

Check the selection for referential integrity and attribute lengths.  Valid
records are deselected and bad ones stay selected, with a message each.  Errors
make the run exit nonzero and block `-write`.  Combine it with `-e` to edit the
bad records.

#### -selerr -- (with -check): Select error records
<a href="#-selerr"></a>

With `-check`, deselect the valid records and leave the bad ones selected, which
is the default.  Pass `-selerr:N` to report the errors and keep the selection
as it was.

#### -maxshow -- Limit number of errors per table
<a href="#-maxshow"></a>

Cap the number of errors `-check` describes, 100 by default.  Raise it when a
large edit breaks many rows and you want to see all of them.

#### -write -- Write data back to disk.
<a href="#-write"></a>

Write the changed ssimfiles to disk.  Without it every editing command is a dry
run that prints what it would change.  The write does not happen when the run
has already failed, for example on a `-check` error.

#### -rename -- Change value of found item
<a href="#-rename"></a>

Replace the key of the selected record and update every record that refers to
it.  When the new key collides with an existing record, `acr` deletes the
renamed record and moves its children under the existing one, which merges two
record trees.  With `-g`, renamed `dev.gitfile` rows become `git mv` commands.
The flag cannot be combined with reading stdin.

#### -nup -- Number of levels to go up
<a href="#-nup"></a>

Extend the selection up this many levels.  Each level adds the records that the
selected records refer to.

#### -ndown -- Number of levels to go down
<a href="#-ndown"></a>

Extend the selection down this many levels.  Each level adds the records that
refer to a selected record.

#### -l -- Go down via pkeys only
<a href="#-l"></a>

Restrict `-ndown` to records whose primary key, or its leading part, carries
the reference.  Without it any field that refers to a selected record pulls its
row in.

#### -xref -- Short for -nup 100 -ndown 100
<a href="#-xref"></a>

Extend the selection in both directions as far as the references go.  An
explicit `-nup` or `-ndown` overrides its half.

#### -fldfunc -- Evaluate fldfunc when printing tuple
<a href="#-fldfunc"></a>

Print the computed fields as well as the stored ones.  A computed field is a
part of another field, declared by `dmmeta.substr`, and the ssimfile does not
store it.

#### -maxgroup -- Max. items per group
<a href="#-maxgroup"></a>

Set how many rows `-pretty` aligns as one block, 25 by default.

#### -pretty -- Align output in blocks
<a href="#-pretty"></a>

Align the attributes of consecutive records of one table into columns, which is
the default.  Pass `-pretty:N` to print each tuple with single spacing.

#### -tree -- Print as tree
<a href="#-tree"></a>

Print each record below the record it refers to, indented.  `-t` turns it on
along with `-xref` and `-loose`.

#### -loose -- Allow printing a record before its references (used with -e)
<a href="#-loose"></a>

Let a record print before the records it refers to, which groups the output
more naturally.  By default `acr` prints every referenced record first.

#### -my -- Invoke acr_my -e (using acr_my directly is faster)
<a href="#-my"></a>

Open the namespaces named by the query in a private MariaDB server, through
`acr_my -e -fldfunc`.  Each namespace becomes a database and each ssimfile a
table, and the computed fields appear as columns.  The data is saved back when
the shell exits.  `-in` must be a directory.

#### -schema -- Directory for initializing acr meta-data
<a href="#-schema"></a>

Name the directory `acr` reads the schema from: `dmmeta.ctype`, `dmmeta.field`
and the tables around them.  It defaults to `data`, and you change it when
`-in` names a dataset that carries its own schema.

#### -e -- Open selection in editor, write back when done.
<a href="#-e"></a>

Write the selection to `temp/acr.ssim`, open it in `$EDITOR`, and replace the
selection with the buffer when the editor exits.  It implies `-write`, `-cmt`
and `-rowid`.  To abort, exit the editor with a nonzero status and remove
`temp/acr.ssim`.

#### -t -- Short for -tree -xref -loose
<a href="#-t"></a>

Extend the selection up and down through all references and print it as an
aligned tree.  Never combine it with `-del -write`, which would delete the
whole tree.

#### -g -- Trigger git commands for changes in dev.gitfile table
<a href="#-g"></a>

Turn changes to `dev.gitfile` rows into `git rm`, `git mv` and `git add`
commands.  `acr` runs the script only when its own write reached disk, and
prints it otherwise.  A script that fails makes the run fail.  Use
`acr ns:<ns> -del -x -g -write` to remove a namespace and its files in one step.

#### -x -- Propagate select/rename/delete to ssimreq records
<a href="#-x"></a>

Apply the `dmmeta.ssimreq` rules as well as the references.  A rule ties a
record to one in another table, such as a namespace to `cpp/gen/<ns>_gen.cpp`,
so selecting, renaming or deleting the first reaches the second.  With
`-check` it also verifies the rules.

#### -rowid -- Always print acr.rowid attribute
<a href="#-rowid"></a>

Print each record's row id as `acr.rowid`, except in files sorted by their
primary key.  Edit the values and feed the tuples back through `-merge` to
reorder records.

#### -cmt -- Print comments for all columns referenced in output
<a href="#-cmt"></a>

Print a comment block around each table's records.  Before the records it
lists the ctypes involved with their comments, and shows an empty tuple as a
template.  After them it lists each field with its type, reftype and comment.

#### -report -- Show final report
<a href="#-report"></a>

Print the `report.acr` line with the counts of selected, inserted, deleted,
ignored and updated records and of modified files.  It is on by default, and
`-field` and `-regxof` turn it off.

#### -print -- Print selected records
<a href="#-print"></a>

Print the selected records, which is the default.  Pass `-print:N` for a run
whose only purpose is its side effect, such as a rewrite through `-write`.

#### -cmd -- Print script with command execution for each selected row
<a href="#-cmd"></a>

Print a shell script that, for each selected record, assigns every field to a
variable and then runs the given command.  The script also sets `acr_tuple`,
`acr_head` and `acr_rowid`.  Pipe it to `bash`, which runs the whole selection
in one process.

#### -field -- Fields to select
<a href="#-field"></a>

Print the named fields of each record on one line, separated by tabs, in place
of whole tuples.
Repeat the flag or give a comma-separated list.  It turns off the report.

#### -regxof -- Single field: output regx of matching field values
<a href="#-regxof"></a>

Print one pattern that matches every value of the named field in the selection,
such as `(protocol|exe|lib|ssimdb|none)` for `acr ns -regxof:nstype`.

#### -meta -- Select meta-data for selected records
<a href="#-meta"></a>

Replace the selection with the schema records that describe it: the ctype of
each selected table, its fields, and the records keyed by them.  It implies
`-tree` and `-l`, and it reads those records from `-schema`.

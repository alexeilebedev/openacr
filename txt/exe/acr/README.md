## acr - Algo Cross-Reference - ssimfile database & update tool
<a href="#acr"></a>

Acr is a query and editing tool for ssim (super-simple) datasets.
An acr dataset can be a directory, file or stdin.

### Table Of Contents
<a href="#table-of-contents"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Toc -->
&nbsp;&nbsp;&bull;&nbsp;  [Syntax](#syntax)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Description](#description)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Reading Stdin](#reading-stdin)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Sorting & RowIDs](#sorting---rowids)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [See Also](#see-also)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Options](#options)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
&#128196; [acr - Internals](/txt/exe/acr/internals.md)<br/>
&#128196; [ACR release notes](/txt/exe/acr/relnotes.md)<br/>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Toc -->

### Syntax
<a href="#syntax"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Syntax -->
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
    -verbose    int             Verbosity level (0..255); alias -v; cumulative
    -debug      int             Debug level (0..255); alias -d; cumulative
    -help                       Print help and exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig

```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Syntax -->

### Description
<a href="#description"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Description -->

Acr performs a fixed number of operations in a fixed order. All of the operations can be enabled/controlled with
command-line flags.
The order of operations is:
* initial selection according to the command-line regex (-query).
* Loading files (-in)
* Processing any input stream (triggered with -sel, -insert, -merge, -replace, -update). 
* Extending selection up (-nup)
* Extending selection down (-ndown). This option is modified by `-l`.
* Selecting unused records (-unused)
* Selecting meta-data (-meta) for current records
* Deletion of selected records (-del)
* Checking of constraints. Checking and `-ndown` are modified by option `-x`, which traverses `ssimreq` constraints in addition
to pkey refrences.
* Editing of intermediate transaction in an editor (-e), or in MariaDB (-my)
* Printing resulting records to stdout (-print, -field, -regxof). Any records that's selected, modified or deleted is
printed. Modified records are prefixed with `acr.update`, and deleted records with `acr.delete`.
The -print option is modified by `-tree`, `-loose`, `-maxgroup`, `-rowid`, `-fldfunc`, `-cmt`.
* Printing or executing the resulting script if `-g` (git) option is sepcified.
* Writing to dataset or back to file (-write). Option -e implies -write. 
* Printing final report summarizing the number of updates and modified files (-report).

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Description -->

### Reading Stdin
<a href="#reading-stdin"></a>

The options `-insert`, `-replace`, `-merge`, '-sel' all enable reading of stdin
for a list of tuples. The lines in the input stream can override the setting on the command line.
The following table shows the possible prefixes:

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

Performs the corresponding actions.

### Sorting & RowIDs
<a href="#sorting---rowids"></a>

Acr always saves files in sorted order. Sorting is controlled by the `ssimsort`
table, which is a subset of `ssimfile`. Sorting is optional. If `ssimsort` is missing
or doesn't specify the primary key of the table, the set is *order-dependent*.
When sorting is enabled, it can be done on any fields, including a fldfunc.

### See Also
<a href="#see-also"></a>

* [acr_my](/txt/exe/acr_my/README.md)
* [acr_ed](/txt/exe/acr_ed/README.md)
* [mysql2ssim](/txt/exe/mysql2ssim/README.md)
* [ssim2mysql](/txt/exe/ssim2mysql/README.md)

### Options
<a href="#options"></a>

<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Options -->
#### -query -- Regx to match record
<a href="#-query"></a>

This option controls initial record selection.
A SQL-like regular expression of the form `<ssimfile>:<key>` or `<ssimfile.fieldname>:<key>`.
The whildcard character is `%`, and characters '|', '(', ')' are also supported.
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
If -in refers to a directory, then ssimfiles are assumed to follow the standard layout NS/NAME.ssim
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
reachable with `-ndown` are still renamed recursively. So this option
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

Launch an instance of mariadb and imports the selection as databases into mariadb.
It then drops you into a mariadb shell, where you can apply needed transformations.
Upon exit, the data is downloaded from the database back to disk. -my implies -write.
The option `-my` also implies `-fldfunc`, so that all the computed fields become available inside MariaDB
as regular columns (but they won't be saved back).

All ssim tables are compatible with MariaDB. The namespace (ns) corresponds to a database in MariaDB,
and ssimfile corresponds to a table.

You can also apply SQL expressions directly from the command line by running `echo "<sql expression>" | acr -my %`.
Any effects of the SQL expression will be visible via `git diff` upon completion of the command.

#### -schema -- Directory for initializing acr meta-data
<a href="#-schema"></a>

Specify where to load acr schema tables (ctype, field, etc)
By default, these tables are loaded from `"data"` directory.

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

#### -x -- Propagate select/rename/delete to ssimreq records
<a href="#-x"></a>

This option extends cascasde delete, update and delete steps to include any rules
specified in the `ssimreq` table.

#### -rowid -- Always print acr.rowid attribute
<a href="#-rowid"></a>

#### -cmt -- Print comments for all columns referenced in output
<a href="#-cmt"></a>

The -cmt option displays any comments associated with the current selection, which includes comments attached to the displayed
fields.

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
shell script access to the values of all field attributes, (including any computed fields), the tuple itsef (`acr_tuple`)
the type tag (`acr_head`) and the rowid (`acr_rowid`). The script can then use whatever 
other Unix tools it needs to.

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

Deselect any selected records and selects their meta-data instead.
`-meta` implies `-t`.

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Options -->

### Inputs
<a href="#inputs"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Inputs -->
`acr` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dmmeta.ctype](/txt/ssimdb/dmmeta/ctype.md)|Struct|
|[dmmeta.field](/txt/ssimdb/dmmeta/field.md)|Specify field of a struct|
|[dmmeta.substr](/txt/ssimdb/dmmeta/substr.md)|Specify that the field value is computed from a substring of another field|
|[dmmeta.ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|File with ssim tuples|
|[dmmeta.ssimsort](/txt/ssimdb/dmmeta/ssimsort.md)|Define sort order for ssimfile|
|[dmmeta.ssimreq](/txt/ssimdb/dmmeta/ssimreq.md)|Extended constraints for ssim records|
|[dmmeta.smallstr](/txt/ssimdb/dmmeta/smallstr.md)|Generated fixed-length padded or length-delimited string field|
|[dmmeta.funique](/txt/ssimdb/dmmeta/funique.md)|This field must be unique in the table. Not needed for primary key|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|
|[dmmeta.cppfunc](/txt/ssimdb/dmmeta/cppfunc.md)|Value of field provided by this expression|
|[dmmeta.cdflt](/txt/ssimdb/dmmeta/cdflt.md)|Specify default value for single-value types that lack fields|
|[dmmeta.anonfld](/txt/ssimdb/dmmeta/anonfld.md)|Omit field name where possible (command line, enums, constants)|
|[amcdb.bltin](/txt/ssimdb/amcdb/bltin.md)|Specify properties of a C built-in type|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Inputs -->


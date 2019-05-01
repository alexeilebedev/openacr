## acr - Auto Cross Reference

ACR is a query and editing tool for ssim (super-simple) database.
The default data directory "data", can be specified with -in option.

The acr query string has the form <ssimfile>:<key>, with SQL regular expressions allowed.
ACR prints to standard out all tuples matching the query. The entire database can be
dumped with acr %

Acr initializes by reading tables ns, ctype, field, and ssimfile from dmmeta database
to obtain the list of possible ctypes, fields and ssimfiles.
Acr reads other ssim files when necessary, loading each file into memory in its entirety.
Acr does not have a proper storage engine, since it reads and writes entire text files
and always scans all records (no btree here). Acr treats all lines from ssim files as tuples.

Internally, acr scans whatever tuples it loaded, and marks tuples matching the query and as selected.
The selected set can then be optionally extended by following references either ``up'' or ``down''.

To fetch all records referenced from the selected set, use -nup.
To fetch all records which reference any of the records in the selected set, use -ndown
(This usually produces much more output than nup).
The option -xref expands to -nup 100 -ndown 100.
The option -tree groups all records by primary key prefix and indents lines with whitespace.
It doesn't change the relative order of any two records of the same type.
The option -t combines -xref, -pretty and -tree.

Acr -insert reads input from stdin and appends new tuples to the existing database. It will fail
if a duplicate primary key is found.

Acr -replace is similar to -insert, except a duplicate key will result in a record being replaced.
When replacing, acr reserves the order of records.

Acr -del deletes selected records.

Options -insert, -replace, -del require the option -write for changes to take effect.
If -write is not specified, all modifications are performed in-memory only.

The option -e launches an editor (as specified with the environment
variable EDITOR), allowing you to edit the selected set. When the editor
exits, whatever tuples were in the original selection are deleted from
the ssim files and replaced with the edited output. To abort editing,
kill the editor and remove temporary file temp/acr.ssim.

The option -check runs a referential integrity check on all records that match selection.
-check can be combined with -e to edit all broken records.

Running acr -help lists all available options.

The option -my launches an instance of mariadb and imports all namespaces that cover selected records
as databases into mariadb. It then drops you into a mariadb shell, where you can apply needed transformations.
Upon exit, the data is downloaded from the database back to disk. -my implies -write.
Options -mystart, -mystop, -myabort allow a semi-permanent mariadb instance to stay
around longer, and be accessed via socket placed in mysql/mysql.sock.

acr -vis creates a graph representing selected records and writes it to a .dot file.
It then runs graphvis to produce an .svg file.

acr -b starts a built-in web server and then launches a browser that can be used to
edit and cross-reference records using a web interface.

Thanks to a simple storage format (ssim) and a predictable location for each data
item, everything that acr does can be done by hand using grep and vi (or emacs).
Acr simply automates reading, modifying, and writing plain text ssim files.
Any changes produced by acr locally or in the source control system
can also be viewed with "git diff".

### Example: Print all records in Ssim database

    acr %  # A key without value matches a table name.

### Example: Format ssim table
With -cmd option, acr outputs a shell script. The result can be piped to sh for execution.
Essentially, acr output becomes one huge shell script. All the tricks available to bash script,
including print, grep, etc., are available for use.

    acr device -cmd 'echo $device -- $comment' |sh

### Example: Delete unnecessary entries from a dependency table
    Let's say we have a dependency table with two columns. If $a$ depends on $b$, and $b$ depends on $c$,
    it's unnecessary to specify that $a$ depends on $c$. Such entries can be removed with a mariadb one-liner:

    acr -my dmmeta.% -fldfunc << END
    DELETE ac.*
    FROM nsdep AS ab
    INNER JOIN nsdep AS bc ON (ab.parent=bc.ns)
    INNER JOIN nsdep AS ac ON (ab.ns = ac.ns and bc.parent=ac.parent);
    END

### Example: Round-trip custom type through Mariadb without loss

    echo dmmeta.sqltype:ietf.IbLid  expr:"varchar(10)" | acr -insert -write
    echo "" | acr -my ns.%
    git diff

### Example: Recursively rename a key

    acr modtype:A -rename B -write
    amc

### Example: Move field from one ctype to another

    acr field:a.B.c -rename a.C.c -write
    amc

### Example: Recursively delete a key

    acr ctype:acr.FCtype -del -write

### Example: Use SQL statements to refactor code

    $ echo "update llist set haveprev='Y'" | acr -my dmmeta.%

### Example: Specify bool options on command line

    acr % -print:N       # OK
    acr % -print:Y       # database-style -- OK
    acr % -print:true    # OK
    acr % -print         # defaults to true

### Field Funcs

Field funcs are computed fields; they can be used as a field,
their value is not an actual struct member, it is computed. Three fldfunc types are
defined: substrings, bitfields, and externally-defined values.

dmmeta.substr declares a field whose value is a substring of another field.
For explanation of expr, see

    src_func -proto % Pathcmop

Substr fields in ssimfiles are supported by acr: run \bashcommand{acr ctype -fldfunc}
and you all computed fields will be included in the output. This also works with -my
and is especially useful for table joins.

dmmeta.cppfunc declares a field whose value is computed by a user-implemented function.

The dmmeta.bitfld record means that the field value is extracted from the value of another
field by bitwise operations.
See acr bitfld


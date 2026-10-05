## mysql2ssim - mysql -> ssim conversion tool
<a href="#mysql2ssim"></a>
mysql2ssim reads tables from a MariaDB or MySQL database and prints their rows as ssim
tuples.  It can also print a starting ssim schema for tables that have none, or write the
rows straight back into the ssimfiles.  [acr_my](/txt/exe/acr_my/README.md) uses it to
bring edits made in the server back into `data/`, and
[ssim2mysql](/txt/exe/ssim2mysql/README.md) converts the other way.

### Syntax
<a href="#syntax"></a>
```usage
mysql2ssim: mysql -> ssim conversion tool
Usage: mysql2ssim [-url:]<string> [[-tables:]<string>] [options]
    OPTION          TYPE    DFLT    COMMENT
    -writessimfile                  Write to ssimfile directly
    [url]           string          user:pass@host/db or sock:///filename/db
    [tables]        string  ""      comma-separated list of tables. Default is all tables
    -schema                         Generate ssim type definition
    -in             string  "data"  Input directory or filename, - for stdin
    -pretty                         Format output for the screen
    -nologo                         Don't show copyright notice
    -baddbok                        Don't claim if bad database
    -verbose        flag            Verbosity level (0..255); alias -v; cumulative
    -debug          flag            Debug level (0..255); alias -d; cumulative
    -trace          string  ""      Trace expression: category[:filter],...; also payload_lim:N, verbose, debug, timestamps
    -help                           Print help and exit; alias -h
    -version                        Print version and exit
    -signature                      Show signatures and exit; alias -sig
```

### Description
<a href="#description"></a>

The last path component of the URL names the database, and the database name becomes the
namespace of every tuple.  Row data from the table `ns` of database `dmmeta` prints as
`dmmeta.ns` tuples, one per row, with one attribute per non-NULL column.

mysql2ssim has three outputs:

- By default it prints the rows on stdout.  Output starts with two comment lines naming the
  tool, which `-nologo` leaves out.
- With `-schema` it prints a `dmmeta.ctype`, a `dmmeta.ssimfile` and one `dmmeta.field` per
  column for each table.  It maps each SQL type to the nearest ctype and prints a
  `mysql2ssim.conversion_warning` when the mapping loses precision.
- With `-writessimfile` it collects the rows and hands them to `acr -trunc -replace
  -write`.  Each ssimfile it read a table for then holds exactly the rows of that table.

### Examples
<a href="#examples"></a>

```bash
mysql2ssim sock:///tmp/mysql.sock/dmmeta ns -nologo             # print dmmeta.ns rows
mysql2ssim sock:///tmp/mysql.sock/dmmeta ns,ctype -pretty       # two tables, aligned
mysql2ssim user:pass@dbhost/dmmeta -schema -nologo              # schema of every table
mysql2ssim sock:///tmp/mysql.sock/dmmeta ns -writessimfile      # write data/dmmeta/ns.ssim
```

### Caveats
<a href="#caveats"></a>

- `-writessimfile` replaces the whole ssimfile with the table's rows, so rows deleted in
  the server are deleted from the file too.
- A column type it has no mapping for, such as `blob`, stops the run with `cannot convert
  type`.
- The `-schema` output is a starting point to edit.  It carries no reftypes, and `enum`
  and wide `decimal` columns map to approximate types.

### Options
<a href="#options"></a>
#### -writessimfile -- Write to ssimfile directly
<a href="#-writessimfile"></a>

Writes the rows into the ssimfiles under `-in`, through `acr`, and prints no rows.  Each
file the run reads a table for is truncated and refilled.

#### -url -- user:pass@host/db or sock:///filename/db
<a href="#-url"></a>

The server and database to read.  Write `user:pass@host/<db>` for a TCP connection, or
`sock:///<socket path>/<db>` for a unix socket.  The database name is also the namespace
of the output.

#### -tables -- comma-separated list of tables. Default is all tables
<a href="#-tables"></a>

The tables to read, as a comma-separated list such as `ns,ctype`.  With no list,
mysql2ssim reads every table of the database.

#### -schema -- Generate ssim type definition
<a href="#-schema"></a>

Prints ssim schema records for each table in place of its rows.  Use it to start a new
namespace from an existing database.

#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

The data directory that `-writessimfile` writes into, `data` by default.  Other modes
ignore it.

#### -pretty -- Format output for the screen
<a href="#-pretty"></a>

Prints rows as aligned text in blocks of about 100 rows, for reading on screen.  The
output is still one tuple per line.

#### -nologo -- Don't show copyright notice
<a href="#-nologo"></a>

Leaves out the two comment lines that start the output.  Use it when the output feeds
another tool.

#### -baddbok -- Don't claim if bad database
<a href="#-baddbok"></a>

Exits with success and prints nothing when the database does not exist.  acr_my sets it,
since a namespace that was never loaded has no database.

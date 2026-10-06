## ssim2mysql - Ssim -> mysql
<a href="#ssim2mysql"></a>
ssim2mysql turns ssim tuples into SQL statements that load them into MariaDB or MySQL.
It either prints the SQL or runs it against a server.  With `-createdb` it also creates
the database and one table per ssimfile of a namespace.  [acr_my](/txt/exe/acr_my/README.md)
uses it to load ssimfiles into a local server, and [mysql2ssim](/txt/exe/mysql2ssim/README.md)
converts the other way.

### Syntax
<a href="#syntax"></a>
```usage
ssim2mysql: Ssim -> mysql
Usage: ssim2mysql [options]
    OPTION      TYPE    DFLT    COMMENT
    -url        string  ""      URL of mysql server. user:pass@hostb or sock://filename; Empty -> stdout
    -data_dir   string  "data"  Load dmmeta info from this directory
    -maxpacket  int     100000  Max Mysql packet size
    -replace            Y       use REPLACE INTO instead of INSERT INTO
    -trunc                      Truncate target table
    -dry_run                    Print SQL commands to the stdout
    -fldfunc                    create columns for fldfuncs
    -in         string  "-"     Input directory or filename, - for stdin
    -db         string  ""      Optional database name
    -createdb                   Emit CREATE DATABASE code for namespace specified with <db>
    -fkey                       Enable foreign key constraints (uses InnoDB storage engine)
    -verbose    flag            Verbosity level (0..255); alias -v; cumulative
    -debug      flag            Debug level (0..255); alias -d; cumulative
    -trace      string  ""      Trace expression: category[:filter],...; also payload_lim:N, verbose, debug, timestamps
    -help                       Print help and exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig
```

### Description
<a href="#description"></a>

ssim2mysql reads the schema from `-data_dir`: the ctypes, fields and ssimfiles, and
`dmmeta.sqltype`, which maps each ctype to an SQL column type.  It then reads tuples from
`-in` and groups consecutive tuples of one ssimfile into one `REPLACE INTO` statement.  A
tuple for `dmmeta.ns` goes to the table `` `dmmeta`.`ns` ``.

With `-createdb` the output starts by dropping and recreating the database `-db`.  It then
creates a table for each ssimfile of that namespace which exists in `-data_dir`.  The first
field of the ctype becomes the primary key, and each column gets the field's default
value.

With no `-url` the SQL goes to stdout, which is also the way to see what a run would do.

### Examples
<a href="#examples"></a>

```bash
acr ns:acr% | ssim2mysql                          # print REPLACE statements
acr ns:acr | ssim2mysql -replace:N -trunc         # empty the table, then INSERT
acr ns:acr | ssim2mysql -db:dmmeta -createdb      # also print the CREATE TABLE statements
acr ns:% | ssim2mysql -url:sock:///tmp/mysql.sock # load dmmeta.ns into a running server
```

The first command prints:

```
REPLACE INTO `dmmeta`.`ns` (`ns`,`nstype`,`license`,`comment`) VALUES
('acr','exe','Apache','Algo Cross-Reference - ssimfile database & update tool'),
('acr_compl','exe','Apache','ACR shell auto-complete for all targets'),
...
```

### Caveats
<a href="#caveats"></a>

- A tuple whose typetag names no ssimfile, such as `report.acr`, is skipped without a
  message.  A field the table has no column for prints a `WARNING: Column ... not found`
  line and is dropped.
- An enum field stored as its symbol in ssim (`u8` with `dmmeta.fconst` rows) reaches the
  server as that word.  A strict-mode server refuses it for an integer column, so store
  such values as numbers.
- `-fkey` switches the tables to InnoDB and adds a `<table>_rowid` primary key, but the
  output carries no `FOREIGN KEY` constraint.
- A table whose ctype has fewer than two usable fields gets an extra column,
  `extra_column_for_roundtrip`, which mysql2ssim drops again on the way back.

### Options
<a href="#options"></a>
#### -url -- URL of mysql server. user:pass@hostb or sock://filename; Empty -> stdout
<a href="#-url"></a>

The server to run the SQL against.  Write `user:pass@host` for a TCP connection or
`sock://<path>` for a unix socket.  With no `-url` ssim2mysql prints the SQL to stdout,
as `-dry_run` does.

#### -data_dir -- Load dmmeta info from this directory
<a href="#-data_dir"></a>

The directory the schema comes from, `data` by default.  Only ssimfiles that exist in it
get tables and columns, so point it at the tree the tuples came from.

#### -maxpacket -- Max Mysql packet size
<a href="#-maxpacket"></a>

The largest statement the server accepts, in bytes.  ssim2mysql closes the current
statement and starts another once it passes three quarters of this size.  Raise it along
with the server's `max_allowed_packet`.

#### -replace -- use REPLACE INTO instead of INSERT INTO
<a href="#-replace"></a>

On by default, so a row whose key already exists replaces the old row.  Pass
`-replace:N` to emit `INSERT INTO`, which fails on a duplicate key.

#### -trunc -- Truncate target table
<a href="#-trunc"></a>

Emits `TRUNCATE TABLE` before the first row of each table.  After the run the table holds
exactly the rows of the input.

#### -dry_run -- Print SQL commands to the stdout
<a href="#-dry_run"></a>

Prints the SQL and runs nothing, even when `-url` names a server.  Use it to check a load
before running it.

#### -fldfunc -- create columns for fldfuncs
<a href="#-fldfunc"></a>

Adds a column for each field that is computed from part of another field (a
`dmmeta.substr` row).  These columns make joins easier to write.  mysql2ssim cannot bring
them back, so a table loaded this way does not round-trip.

#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

The file of tuples to load, or `-` for stdin, which is the default.

#### -db -- Optional database name
<a href="#-db"></a>

The namespace whose database `-createdb` creates, such as `dmmeta`.  Without `-createdb`
it has no effect, since each statement already names its database.

#### -createdb -- Emit CREATE DATABASE code for namespace specified with <db>
<a href="#-createdb"></a>

Drops and recreates the database named by `-db`, then creates a table for each of its
ssimfiles.  Everything the database held before is lost.

#### -fkey -- Enable foreign key constraints (uses InnoDB storage engine)
<a href="#-fkey"></a>

With `-createdb`, creates InnoDB tables with an auto-increment `<table>_rowid` primary key,
and the first field as a unique key.  Without it the tables are MyISAM.  See the caveat
about the missing constraints.

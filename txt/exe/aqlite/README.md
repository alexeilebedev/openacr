## aqlite - Runs sqlite queries against ssim files
<a href="#aqlite"></a>
aqlite runs an SQL query over ssimfiles.  It opens an in-memory SQLite database, presents
each ssimfile as a table, and prints the rows the query returns as ssim tuples.  Use it for
joins and aggregates that `acr` cannot express.

### Syntax
<a href="#syntax"></a>
```usage
aqlite: Runs sqlite queries against ssim files
Usage: aqlite [-cmd:]<string> [options]
    OPTION      TYPE    DFLT      COMMENT
    -in         string  "data"    Input directory or filename, - for stdin
    -schema     string  "data"    Schema dir
    [cmd]       string            Sql Query to run
    -ns         regx    "dmmeta"  Regx of databases to attach
    -verbose    flag              Verbosity level (0..255); alias -v; cumulative
    -debug      flag              Debug level (0..255); alias -d; cumulative
    -trace      string  ""        Trace expression: category[:filter],...; also payload_lim:N, verbose, debug, timestamps
    -help                         Print help and exit; alias -h
    -version                      Print version and exit
    -signature                    Show signatures and exit; alias -sig
```

### Description
<a href="#description"></a>

Each ssimdb namespace that matches `-ns` becomes an attached SQLite database, and each of
its ssimfiles becomes a table in it.  So `dmmeta.ns` is the table `ns` of the database
`dmmeta`, and a query names it as `dmmeta.ns`.  The columns are the fields of the ctype.

The query may hold several statements separated by `;`.  Each result row prints as one
tuple whose attributes are the result columns.  The typetag is `stmt1` for the first set
of columns, and it counts up each time the column names change.

On an SQL error, aqlite prints SQLite's message on stderr and exits with SQLite's error
code.

### Examples
<a href="#examples"></a>

```bash
aqlite "select ns, nstype from dmmeta.ns where ns like 'acr%'"   # a simple select
aqlite "select nstype, count(*) as n from dmmeta.ns group by nstype" | ssimfilt -t
aqlite -ns:'dmmeta|dev' "select t.target, n.comment from dev.target t
    join dmmeta.ns n on n.ns=t.target where t.target like 'acr_%'"   # join two namespaces
```

The first query prints:

```
stmt1  ns:acr  nstype:exe
stmt1  ns:acr_compl  nstype:exe
stmt1  ns:acr_dm  nstype:exe
...
```

### Caveats
<a href="#caveats"></a>

- Only `dmmeta` is attached by default.  A query on another namespace fails with `no such
  table: dev.target` until `-ns` names it.
- A namespace that has an ssimfile named after an SQL keyword, such as `group`, fails to
  attach with `near "group": syntax error`.  So `-ns:%` can fail where a list of the
  namespaces you need works.
- `INSERT`, `UPDATE` and `DELETE` succeed and change nothing, in memory or on disk.  Use
  [acr](/txt/exe/acr/README.md) to edit ssimfiles.

### Options
<a href="#options"></a>
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

The data directory the tables read their rows from, `data` by default.

#### -schema -- Schema dir
<a href="#-schema"></a>

The directory aqlite loads the schema from, `data` by default.  The schema decides which
namespaces and ssimfiles exist and which columns each table has.

#### -cmd -- Sql Query to run
<a href="#-cmd"></a>

The SQL to run, usually given as the first positional argument.  Quote it for the shell,
and use single quotes for SQL strings inside it.

#### -ns -- Regx of databases to attach
<a href="#-ns"></a>

An SQL-style regx of the ssimdb namespaces to attach, `dmmeta` by default.  Write
`-ns:'dmmeta|dev'` for two namespaces.

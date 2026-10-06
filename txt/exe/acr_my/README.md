## acr_my - ACR <-> MariaDB adaptor
<a href="#acr_my"></a>
`acr_my` loads ssim namespaces into a private MariaDB server, so you can query
and edit them in SQL, and saves the tables back to the ssimfiles when you are
done.  `acr -my` is a shortcut to the same workflow.

### Syntax
<a href="#syntax"></a>
```usage
acr_my: ACR <-> MariaDB adaptor
Usage: acr_my [[-nsdb:]<regx>] [options]
    OPTION      TYPE    DFLT    COMMENT
    [nsdb]      regx    ""      Regx of ssim namespace (dmmeta.nsdb) to select
    -in         string  "data"  Input directory or filename, - for stdin
    -schema     string  "data"  Input directory or filename, - for stdin
    -fldfunc                    Evaluate fldfunc when printing tuple
    -fkey                       Enable foreign key constraints
    -e                          Alias for -start -shell -stop
    -start                      Start local mysql server
    -stop                       Stop local mysql server, saving data
    -abort                      Abort local mysql server, losing data
    -shell                      Connect to local mysql server
    -serv                       Start mysql with TCP/IP service enabled
    -verbose    flag            Verbosity level (0..255); alias -v; cumulative
    -debug      flag            Debug level (0..255); alias -d; cumulative
    -trace      string  ""      Trace expression: category[:filter],...; also payload_lim:N, verbose, debug, timestamps
    -help                       Print help and exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig
```

### Description
<a href="#description"></a>

`acr_my` runs a private MariaDB server under `temp/mysql` in the current
directory, with its log in `temp/mysql_log/mysqld.log`.  The server listens
only on a socket in that directory, so it needs no network and no passwords.

`-start` starts the server and loads the selected namespaces.  Each namespace
becomes a database and each of its ssimfiles a table, loaded through
[ssim2mysql](/txt/exe/ssim2mysql/README.md).  The namespaces you can select are
the rows of `dmmeta.nsdb`, and the positional pattern picks among them.  Every
start reloads the tables from the ssimfiles, replacing what the server held.

`-shell` opens a `mysql` client on the socket, with the last selected namespace
as the current database.  `-stop` saves the selected namespaces back to their
ssimfiles through [mysql2ssim](/txt/exe/mysql2ssim/README.md) and stops the
server.  `-abort` stops the server and discards the changes.  `-e` does start,
shell and stop in one run, and it is what `acr -my` invokes.

`dmmeta.sqltype` maps ctypes to SQL column types, so a table survives the round
trip unchanged.  The `normalize_acr_my` CI test round-trips every namespace
through MariaDB and checks that no ssimfile changes.

### See also
<a href="#see-also"></a>

* [acr](/txt/exe/acr/README.md): `acr -my` opens the namespaces its query names through `acr_my -e`
* [ssim2mysql](/txt/exe/ssim2mysql/README.md) and [mysql2ssim](/txt/exe/mysql2ssim/README.md): the converters `acr_my` runs

### Examples
<a href="#examples"></a>

```bash
acr_my dmmeta -e                  # edit dmmeta in a mysql shell; save when the shell exits
acr_my dmmeta -start              # start the server with dmmeta loaded
acr_my -shell                     # connect; the server keeps running after you exit
acr_my dmmeta -stop               # save dmmeta back to its ssimfiles and stop
acr_my -abort                     # stop and discard every change
acr_my % -start -stop             # round-trip every namespace, as CI does
echo "select count(*) from field where arg='u8'" | acr_my dmmeta -e   # one SQL statement
```

In the shell, the tables carry their ssimfile names, and the namespace names
the database:

```
MariaDB [dmmeta]> select count(*) from dmmeta.field where arg ='u8';
```

### Caveats
<a href="#caveats"></a>

- `-stop` saves only the namespaces its own command line selects.  A bare
  `acr_my -stop` selects nothing, so it stops the server and saves nothing.
  Pass the same pattern you gave `-start`.
- The server lives under `temp/` of the directory you run in, so run every
  `acr_my` step from the same directory, the top of the repo.
- `-fldfunc` adds the computed fields as columns.  Editing them changes
  nothing, since `-stop` drops them on the way back.
- `-serv` opens the server to TCP connections with no authentication.
- `acr_my` refuses to run as root, where `mysqld_safe` cannot bind its socket.

### Options
<a href="#options"></a>
#### -nsdb -- Regx of ssim namespace (dmmeta.nsdb) to select
<a href="#-nsdb"></a>

Select the namespaces to load and save, as a pattern matched against the rows of
`dmmeta.nsdb`.  `dmmeta`, `dmm%` and `dmmeta.%` all select dmmeta, and `%`
selects every namespace.  An empty pattern selects nothing.

#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

Name the dataset to load and save, `data` by default.

#### -schema -- Input directory or filename, - for stdin
<a href="#-schema"></a>

Name where `acr_my` reads `dmmeta.nsdb` and the list of ssimfiles, `data` by
default.

#### -fldfunc -- Evaluate fldfunc when printing tuple
<a href="#-fldfunc"></a>

Load the computed fields as extra columns, so a query can filter on a part of a
key.  `-stop` strips them when it saves.

#### -fkey -- Enable foreign key constraints
<a href="#-fkey"></a>

Create the tables with the InnoDB engine and foreign key constraints, so the
server rejects an edit that breaks a reference.  Without it the tables use
MyISAM and accept any value.

#### -e -- Alias for -start -shell -stop
<a href="#-e"></a>

Start the server, open a shell, and save and stop when the shell exits.

#### -start -- Start local mysql server
<a href="#-start"></a>

Start the server, creating its data directory on first use, and load the
selected namespaces.  It waits up to a minute for the server and prints the end
of the log when the server fails to start.

#### -stop -- Stop local mysql server, saving data
<a href="#-stop"></a>

Save the selected namespaces to their ssimfiles and stop the server.  Give it
the same namespace pattern as `-start`.

#### -abort -- Abort local mysql server, losing data
<a href="#-abort"></a>

Stop the server without saving.  Use it to discard an edit, or to clear a
server left running by an earlier session.

#### -shell -- Connect to local mysql server
<a href="#-shell"></a>

Open a `mysql` client on the server's socket.  Without `-stop` or `-abort` the
server keeps running after the shell exits.

#### -serv -- Start mysql with TCP/IP service enabled
<a href="#-serv"></a>

Start the server listening on TCP as well as on its socket, so a client on
another host or a GUI can connect.  The server checks no passwords.

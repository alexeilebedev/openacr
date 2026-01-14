## acr_my - ACR <-> MariaDB adaptor


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Toc -->
&nbsp;&nbsp;&bull;&nbsp;  [Syntax](#syntax)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Description](#description)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [See also](#see-also)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Examples](#examples)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Options](#options)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
&#128196; [acr_my - Internals](/txt/exe/acr_my/internals.md)<br/>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Toc -->

### Syntax
<a href="#syntax"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Syntax -->
```
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
    -help                       Print help and exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig

```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Syntax -->

### Description
<a href="#description"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Description -->

`acr_md` can be used to operate a local, networkless instance of `mariadb` to work
on acr datasets.

Here are some useful commands:

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Description -->

### See also
<a href="#see-also"></a>

Tools mysql2ssim and ssim2mysql perform conversion to/from ssim schema to SQL, and
acr_my can set up a local MariaDB server process and iteract with
it via a socket.

#### Example: Running a SQL query against an acr dataset from bash:
<a href="#example-running-a-sql-query-against-an-acr-dataset-from-bash"></a>

Any SQL expression can be executed this way. Results are saved back to ssim files.

```
echo "select * from xxx where yyy ='zzz'" | acr -my %.%
```

### Examples
<a href="#examples"></a>

#### Start a local MariaDB server
<a href="#start-a-local-mariadb-server"></a>

$ acr_my -start dmmeta

#### Connect to the local server on the command line
<a href="#connect-to-the-local-server-on-the-command-line"></a>

$ acr_my -shell

From here, you can issue SQL commands:

> select count(*) from dmmeta.field where arg ='u8';

When you exit from the shell, the server keeps running.

#### Stop the server and save changes
<a href="#stop-the-server-and-save-changes"></a>

$ acr_my -stop

#### Stop the server, discarding changes
<a href="#stop-the-server--discarding-changes"></a>

$ acr_my -abort

The table `dmmeta.sqltype` allows ssim2mysql to map ctypes to SQL types so that
round tripping can occur without loss. We check in CI that all acr tables
can be round tripped through MariaDB with no change.

### Options
<a href="#options"></a>

<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Options -->
#### -nsdb -- Regx of ssim namespace (dmmeta.nsdb) to select
<a href="#-nsdb"></a>

#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

#### -schema -- Input directory or filename, - for stdin
<a href="#-schema"></a>

#### -fldfunc -- Evaluate fldfunc when printing tuple
<a href="#-fldfunc"></a>

#### -fkey -- Enable foreign key constraints
<a href="#-fkey"></a>

#### -e -- Alias for -start -shell -stop
<a href="#-e"></a>

#### -start -- Start local mysql server
<a href="#-start"></a>

#### -stop -- Stop local mysql server, saving data
<a href="#-stop"></a>

#### -abort -- Abort local mysql server, losing data
<a href="#-abort"></a>

#### -shell -- Connect to local mysql server
<a href="#-shell"></a>

#### -serv -- Start mysql with TCP/IP service enabled
<a href="#-serv"></a>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Options -->

### Inputs
<a href="#inputs"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Inputs -->
`acr_my` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|
|[dmmeta.nsdb](/txt/ssimdb/dmmeta/nsdb.md)|Annotate ssimdb namespaces|
|[dmmeta.ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|File with ssim tuples|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Inputs -->


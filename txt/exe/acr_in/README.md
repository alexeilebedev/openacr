## acr_in - ACR Input - compute set of ssimfiles or tuples used by a specific target
<a href="#acr_in"></a>
`acr_in` answers which ssimfiles a target reads at startup, and in what order.
It can also print the rows of those files, which gives you a self-contained
input for the target.  Use it to see what a program depends on, to find the
programs that read a table, and to cut a small dataset for a test.

### Syntax
<a href="#syntax"></a>
```usage
acr_in: ACR Input - compute set of ssimfiles or tuples used by a specific target
Usage: acr_in [[-ns:]<regx>] [options]
    OPTION        TYPE    DFLT    COMMENT
    [ns]          regx    ""      Regx of matching namespace
    -data                         List ssimfile contents
    -sigcheck             Y       Output sigcheck records for schema version mismatch detection
    -list                         List ssimfile names
    -t                            (with -list) Tree mode
    -data_dir...  string          Directory with ssimfiles; repeat to layer, empty means data
    -schema       string  "data"
    -related      string  ""      Select only tuples related to specified acr key
    -notssimfile  regx    ""      Exclude ssimfiles matching regx
    -checkable                    Ensure output passes acr -check
    -r            regx    ""      Reverse lookup of target by ssimfile
    -verbose      flag            Verbosity level (0..255); alias -v; cumulative
    -debug        flag            Debug level (0..255); alias -d; cumulative
    -trace        string  ""      Trace expression: category[:filter],...; also payload_lim:N, verbose, debug, timestamps
    -help                         Print help and exit; alias -h
    -version                      Print version and exit
    -signature                    Show signatures and exit; alias -sig
```

### Description
<a href="#description"></a>

A target reads a table because its schema has a `dmmeta.finput` row for it.
[acr_ed](/txt/exe/acr_ed/README.md) `-create -finput` adds such a row.
`acr_in` reads the `dmmeta.finput` rows of the selected targets and of every
library they link through `dev.targdep`, and prints one `dmmeta.ssimfile` row
per table.

```
$ acr_in acr_in -sigcheck:N
dmmeta.ssimfile  ssimfile:dmmeta.ns  ctype:dmmeta.Ns
dmmeta.ssimfile  ssimfile:gendb.dispsig  ctype:gendb.Dispsig
dmmeta.ssimfile  ssimfile:dmmeta.dispsigcheck  ctype:dmmeta.Dispsigcheck
dmmeta.ssimfile  ssimfile:dev.target  ctype:dev.Target
...
```

`acr_in` orders the tables by their `Pkey` references, so each table comes after
the tables it refers to.  `dmmeta.ns` comes before `dmmeta.ctype`, although it
sorts later by name, because a ctype refers to its namespace.  The order depends
only on the schema, so the input computed for a pattern of targets, such as
`%`, loads into any one of those targets without error.

#### Signature checks
<a href="#signature-checks"></a>

By default the output starts with a `dmmeta.dispsigcheck` row for each selected
namespace.  The row carries the signature of the schema the input was cut from.
A program that loads a row whose signature differs from the one it was compiled
with prints `algo_lib.dispsigcheck` and exits.  This catches a stale binary run
against newer data.  `-sigcheck:N` leaves the rows out.

#### Printing the data
<a href="#printing-the-data"></a>

`-data` prints the rows of every listed table in the same order.  The result is
a complete input for the target, and any tool whose `-in` or `-schema` accepts a
file reads it exactly as it reads `data/`.  `-related` narrows the rows to those
reachable from one record, and `-checkable` adds the tables the inputs refer to,
so the result passes `acr -check`.

`-data_dir` names the directory the rows come from.  Repeat it to stack
directories, and `acr_in` searches each one for every table.  A key that two
directories both carry stops the run with `acr_in.duplicate_key`.

#### Reverse lookup
<a href="#reverse-lookup"></a>

`-r` takes a pattern of ssimfiles and prints an `acr_in.nsssimfile` row for each
namespace that reads a matching table, directly or through a library it links.

```
$ acr_in -r dev.gitfile
acr_in.nsssimfile  ns:abt_md  ssimfile:dev.gitfile
acr_in.nsssimfile  ns:acr_ed  ssimfile:dev.gitfile
...
```

### See also
<a href="#see-also"></a>

* [acr](/txt/exe/acr/README.md): query and edit the ssim dataset
* [acr_ed](/txt/exe/acr_ed/README.md): add a `dmmeta.finput` row with `-create -finput`

### Examples
<a href="#examples"></a>

```bash
acr_in acr_in                               # the ssimfiles acr_in reads, in load order
acr_in acr_in -sigcheck:N                   # the same, without the signature rows
acr_in acr_in -t                            # grouped by the namespace that reads each file
acr_in acr_in -checkable -sigcheck:N        # add the tables the inputs refer to
acr_in -r dev.gitfile                       # which namespaces read dev.gitfile
acr_in acr_in -data > temp/acr_in.ssim      # snapshot the rows acr_in loads
acr_in acr_in -schema:temp/acr_in.ssim      # run acr_in against that snapshot
acr_in acr_in -data -related:dmmeta.ns:acr_in   # only the rows reachable from one namespace
acr_in acr_in -notssimfile:dev.%            # leave out the dev tables
```

A snapshot of the whole input is large, and `-related` cuts it down:

```
$ acr_in acr_in -data | wc -l
29891
$ acr_in acr_in -data -related:dmmeta.ns:acr_in -sigcheck:N | head -4
dmmeta.ns  ns:acr_in  nstype:exe  license:Apache  comment:"ACR Input - compute set of ssimfiles or tuples used by a specific target"
gendb.dispsig  dispsig:acr_in.Input  signature:f28ed5579652210d4867ee60ba9d3ae83adf5bd6
dev.target  target:acr_in
dmmeta.ctype  ctype:acr_in.FCtype  comment:""
```

### Caveats
<a href="#caveats"></a>

- The positional argument is a namespace pattern, and an empty one selects
  nothing, so `acr_in` alone prints nothing.  Pass `%` for every target.
- `-r` and a namespace pattern exclude each other, and `acr_in` refuses a
  command line that gives both.
- The plain list names only the tables a target declares.  Its rows can still
  refer to tables outside the list, so `acr -check` fails on the `-data` output
  unless you add `-checkable`.

### Options
<a href="#options"></a>
#### -ns -- Regx of matching namespace
<a href="#-ns"></a>

Select the targets whose inputs to compute, as a pattern of namespace names.
`acr_in` adds every library the selected targets link.  An empty pattern
selects nothing.

#### -data -- List ssimfile contents
<a href="#-data"></a>

Print the rows of each input table, in load order.  The output is a complete
input for the target, which you can save to a file and pass to its `-in` or
`-schema`.  Without `-list`, `-data` prints only the rows.

#### -sigcheck -- Output sigcheck records for schema version mismatch detection
<a href="#-sigcheck"></a>

Print a `dmmeta.dispsigcheck` row with the schema signature of each selected
namespace, which is the default.  A program that loads the output with a
different compiled signature exits with `algo_lib.dispsigcheck`.  `-r` turns the
rows off.

#### -list -- List ssimfile names
<a href="#-list"></a>

Print one `dmmeta.ssimfile` row per input table.  It is the default when
`-data` is absent, and you pass it with `-data` to get both.

#### -t -- (with -list) Tree mode
<a href="#-t"></a>

Group the list by namespace, under a `<ns>:` line each.  A table appears once
for every selected namespace that reads it.

#### -data_dir -- Directory with ssimfiles; repeat to layer, empty means data
<a href="#-data_dir"></a>

Name the directory `-data` reads rows from, `data` by default.  Repeat it to
stack directories: `acr_in` reads each table from every directory that holds
it, and a key found in two of them stops the run.

#### -schema -- 
<a href="#-schema"></a>

Name the directory or file `acr_in` reads its own inputs from: the namespaces,
targets, ctypes, fields and `dmmeta.finput` rows.  It defaults to `data`.

#### -related -- Select only tuples related to specified acr key
<a href="#-related"></a>

With `-data`, print only the rows reachable from the records whose key matches,
written as `<ssimfile>:<key>` such as `dmmeta.ns:acr_in`.  Tables unrelated to
that record stay whole.

#### -notssimfile -- Exclude ssimfiles matching regx
<a href="#-notssimfile"></a>

Leave out the tables whose name matches the pattern, such as `dev.%`.

#### -checkable -- Ensure output passes acr -check
<a href="#-checkable"></a>

Add every table the inputs refer to, recursively, so the output passes
`acr -check`.  `dmmeta.ns` refers to `dmmeta.nstype`, for example, so a target
that reads namespaces gets the nstype table too.

#### -r -- Reverse lookup of target by ssimfile
<a href="#-r"></a>

Take a pattern of ssimfiles and print the namespaces that read a matching
table, including those that read it through a library they link.  It cannot be
combined with a namespace pattern.

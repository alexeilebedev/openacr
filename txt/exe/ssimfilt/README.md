## ssimfilt - Tuple utility
<a href="#ssimfilt"></a>
ssimfilt reads ssim tuples on stdin, keeps the ones that match a filter, and prints them
in the format you choose.  It needs no schema to filter, so it works on the output of any
tool that prints ssim.  Use it to cut columns out of `acr` output, to turn tuples into a
table, CSV or JSON, or to run a shell command once per tuple.

### Syntax
<a href="#syntax"></a>
```usage
ssimfilt: Tuple utility
Usage: ssimfilt [[-typetag:]<regx>] [[-match:]<string>] [options]
    OPTION      TYPE    DFLT    COMMENT
    -in         string  "data"  Input directory or filename, - for stdin
    [typetag]   regx    "%"     (filter) Match typetag. ^=first encountered typetag
    [match]...  string          (filter) Select input tuple if value of key matches value (regx:regx)
    -field...   string          (project) Select fields for output (regx)
    -format     enum    ssim    Output format for selected tuples (ssim|csv|field|cmd|json|stablefld|table|mdtable)
    -t                          Alias for -format:table
    -cmd        string  ""      Command to output
    -f          string  ""      Alias for -field:<f> -format:field
    -verbose    flag            Verbosity level (0..255); alias -v; cumulative
    -debug      flag            Debug level (0..255); alias -d; cumulative
    -trace      string  ""      Trace expression: category[:filter],...; also payload_lim:N, verbose, debug, timestamps
    -help                       Print help and exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig
```

### Description
<a href="#description"></a>

ssimfilt works in two stages.  The filter stage selects tuples, and the projection stage
decides which fields of each selected tuple to print and how.

A tuple passes the filter when its typetag matches the first positional argument and every
`key:value` argument matches too.  `ssimfilt dmmeta.ns nstype:exe` keeps the `dmmeta.ns`
tuples whose `nstype` is `exe`.  Every pattern is an SQL-style regx, where `%` matches any
string and `_` matches one character.  A pattern matches the whole value.

The projection stage prints every field of a selected tuple unless `-field` names some of
them.  `-format` then picks the output: ssim tuples again, bare values, a table, CSV, JSON,
or a bash script.

ssimfilt also reads the schema from the `-in` directory, which is `data` by default.  The
JSON format uses it to print numbers and booleans without quotes, and the `stablefld`
format uses `dev.unstablefld` to find the fields it masks.

### Examples
<a href="#examples"></a>

```bash
acr ns:acr% | ssimfilt -t                          # print acr output as an aligned table
acr ns:acr% | ssimfilt ^ nstype:exe -f ns          # print the name of every exe namespace
acr field:acr.FDb.% | ssimfilt -field:field,arg    # keep two fields of each tuple
acr ns:acr | ssimfilt -format:json                 # one JSON object per tuple
acr ns:acr% | ssimfilt ^ -format:csv > ns.csv      # CSV with a header line
acr field:acr.FDb.% | ssimfilt -cmd 'echo $field/$arg' | bash   # run a command per tuple
```

#### Example: Format ssim input as table
<a href="#example-format-ssim-input-as-table"></a>

```ssim
inline-command: acr field:command.ssimfilt.% | head | ssimfilt ^ -t
FIELD                     ARG           REFTYPE  DFLT    COMMENT
command.ssimfilt.in       algo.cstring  Val      "data"  Input directory or filename, - for stdin
command.ssimfilt.typetag  algo.cstring  RegxSql  "%"     (filter) Match typetag. ^=first encountered typetag
command.ssimfilt.match    algo.cstring  Tary             (filter) Select input tuple if value of key matches value (regx:regx)
command.ssimfilt.field    algo.cstring  Tary             (project) Select fields for output (regx)
command.ssimfilt.format   u8            Val      0       Output format for selected tuples
command.ssimfilt.t        bool          Val      false   Alias for -format:table
command.ssimfilt.cmd      algo.cstring  Val      ""      Command to output
command.ssimfilt.f        algo.cstring  Val      ""      Alias for -field:<f> -format:field

```

#### Example: Extract field
<a href="#example-extract-field"></a>

```bash
inline-command: echo $'blah a:b\nblah a:c' | ssimfilt -field a -format field
b
c
```

#### Example: Convert ssim to Json
<a href="#example-convert-ssim-to-json"></a>

```bash
inline-command: echo $'blah a:b\nblah a:c' | ssimfilt  -format json
{"@type":"blah","a":"b"}
{"@type":"blah","a":"c"}
```

#### Example: Convert ssim to Markdown table
<a href="#example-convert-ssim-to-markdown-table"></a>

```text
inline-command: echo $'blah a:b\nblah a:c' | ssimfilt  -format mdtable
|A|
|---|
|b|
|c|

```

#### Example: Find other uses in documentation
<a href="#example-find-other-uses-in-documentation"></a>

```bash
grep -R 'command.*ssimfilt' txt/
```

### Caveats
<a href="#caveats"></a>

- The first positional argument is the typetag, and a `key:value` pair is only a match
  from the second position on.  `ssimfilt ns:acr` looks for tuples whose typetag is
  `ns:acr` and prints nothing, so write `ssimfilt ^ ns:acr` or `ssimfilt -match:ns:acr`.
- `acr` ends its output with a `report.acr` tuple, and it passes the default `%` filter.
  Use `^` or name the typetag to drop it, or run `acr` with `-report:N`.
- Tuples reach ssimfilt only on stdin.  `-in` names where the schema comes from.
- The `table` and `mdtable` formats start a new table whenever the typetag or the number
  of fields changes, so mixed input prints as several tables.

### Options
<a href="#options"></a>
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

The directory ssimfilt loads its schema from, `data` by default.  The schema affects only
the `json` and `stablefld` formats.  Tuples to filter always come from stdin.

#### -typetag -- (filter) Match typetag. ^=first encountered typetag
<a href="#-typetag"></a>

An input tuple is printed only when its typetag matches this regx.  The special value `^`
locks the filter to the typetag of the first tuple, which keeps the output to one record
type.  The `csv` format needs that, since a CSV file holds one kind of row.

#### -match -- (filter) Select input tuple if value of key matches value (regx:regx)
<a href="#-match"></a>

A pair of regxes, `key:value`, and you can give it several times.  A tuple is selected
when, for every `-match`, each field whose name matches the key has a value that matches
the value.  A tuple with no field matching the key passes that `-match`.

#### -field -- (project) Select fields for output (regx)
<a href="#-field"></a>

Selects the fields to print, by a regx on the field name.  A comma separates alternatives,
so `-field:ns,comment` keeps both fields.  Without `-field` every field is printed.

#### -format -- Output format for selected tuples
<a href="#-format"></a>

Selects the output format:
- `ssim`: print ssim tuples (the default).
- `csv`: print a CSV file.  The first selected tuple fixes the typetag and the header.
- `field`: print the selected field values, one per line.
- `cmd`: print a bash script.  `-cmd` names the command it runs for each tuple.
- `json`: print one JSON object per tuple, with the typetag under `@type`.
- `stablefld`: print each line with the values of `dev.unstablefld` fields replaced by
  `***`.  Filters are ignored, and lines that are not tuples pass through unchanged.
- `table`: print an aligned text table.
- `mdtable`: print a markdown table with a header.

#### -t -- Alias for -format:table
<a href="#-t"></a>

Short for `-format:table`, the usual way to read `acr` output on screen.

#### -cmd -- Command to output
<a href="#-cmd"></a>

Sets `-format:cmd` and names the command to print for each tuple.  Before the command,
ssimfilt prints a bash assignment for every field, plus `$tuple` for the whole line and
`$head` for the typetag.  Pipe the output to `bash` to run it.

```bash
acr field | ssimfilt -cmd 'echo $field/$arg' | bash
```

#### -f -- Alias for -field:<f> -format:field
<a href="#-f"></a>

Prints the values of the named field, one per line.  `-f:ns` is the same as
`-field:ns -format:field`, and a comma list selects several fields.

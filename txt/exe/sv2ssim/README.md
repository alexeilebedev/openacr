## sv2ssim - sv2ssim - Separated Value file processor
<a href="#sv2ssim"></a>
sv2ssim reads a file of separated values, such as CSV or TSV, and converts it.  It can
print the rows as ssim tuples, print them again with another separator, or guess an ssim
schema from the values it sees.  Use it to bring a spreadsheet export or a log into ssim.

### Syntax
<a href="#syntax"></a>
```usage
sv2ssim: sv2ssim - Separated Value file processor
Usage: sv2ssim [-fname:]<string> [options]
    OPTION          TYPE    DFLT    COMMENT
    -in             string  "data"  Input directory or filename, - for stdin
    [fname]         string          Input file, use - for stdin
    -separator      string  ','     Input field separator
    -outseparator   string  ""      Output separator. Default: ssim
    -header                 Y       File has header line
    -ctype          string  ""      Type tag for output tuples
    -ssimfile       string  ""      (with -schema) Create ssimfile definition
    -schema                         (output)Generate schema from input file
    -field          regx    "%"     (output) Print selected fields
    -data                           (output) Convert input file to ssim tuples
    -report                 Y       Print final report
    -prefer_signed                  Prefer signed types when given a choice
    -verbose        flag            Verbosity level (0..255); alias -v; cumulative
    -debug          flag            Debug level (0..255); alias -d; cumulative
    -trace          string  ""      Trace expression: category[:filter],...; also payload_lim:N, verbose, debug, timestamps
    -help                           Print help and exit; alias -h
    -version                        Print version and exit
    -signature                      Show signatures and exit; alias -sig
```

### Description
<a href="#description"></a>

sv2ssim reads the file line by line and skips empty lines.  By default the first line is a
header, and its tokens become the field names.  sv2ssim keeps only letters, digits and
underscores of each name, puts `_` in front of a name that starts with a digit, and adds a
number to a name seen before.  With a comma separator, a token in double quotes may hold
commas and escaped quotes.

Three outputs can be combined in one run:

- `-data` prints each row, as an ssim tuple tagged with `-ctype` or as separated values.
- `-schema` prints a `dmmeta.ctype`, a `dmmeta.cfmt` and one `dmmeta.field` per column.
  It picks each field's type from the widths and values seen in the whole file.
- `-report`, on by default, prints a `sv2ssim.report` line with the line and field counts.

The schema comes from the `dmmeta.svtype` table in `-in`.  A column matches an svtype when
its values fit that type, and when several match, the one with the smallest `maxwid` wins.
A column that matches none is `algo.cstring`.

```ssim
inline-command: acr svtype -report:N
dmmeta.svtype  ctype:algo.Smallstr10   maxwid:10          fixedwid1:0  fixedwid2:0  comment:""
dmmeta.svtype  ctype:algo.Smallstr100  maxwid:100         fixedwid1:0  fixedwid2:0  comment:""
dmmeta.svtype  ctype:algo.Smallstr150  maxwid:150         fixedwid1:0  fixedwid2:0  comment:""
dmmeta.svtype  ctype:algo.Smallstr200  maxwid:200         fixedwid1:0  fixedwid2:0  comment:""
dmmeta.svtype  ctype:algo.Smallstr249  maxwid:249         fixedwid1:0  fixedwid2:0  comment:""
dmmeta.svtype  ctype:algo.Smallstr25   maxwid:25          fixedwid1:0  fixedwid2:0  comment:""
dmmeta.svtype  ctype:algo.Smallstr250  maxwid:250         fixedwid1:0  fixedwid2:0  comment:""
dmmeta.svtype  ctype:algo.Smallstr255  maxwid:255         fixedwid1:0  fixedwid2:0  comment:""
dmmeta.svtype  ctype:algo.Smallstr50   maxwid:10          fixedwid1:0  fixedwid2:0  comment:""
dmmeta.svtype  ctype:algo.cstring      maxwid:1000000000  fixedwid1:0  fixedwid2:0  comment:""
dmmeta.svtype  ctype:bool              maxwid:1           fixedwid1:0  fixedwid2:0  comment:""
dmmeta.svtype  ctype:char              maxwid:1           fixedwid1:0  fixedwid2:0  comment:""
dmmeta.svtype  ctype:double            maxwid:64          fixedwid1:0  fixedwid2:0  comment:""
dmmeta.svtype  ctype:i32               maxwid:31          fixedwid1:0  fixedwid2:0  comment:""
dmmeta.svtype  ctype:i64               maxwid:63          fixedwid1:0  fixedwid2:0  comment:""
dmmeta.svtype  ctype:u32               maxwid:32          fixedwid1:0  fixedwid2:0  comment:""
dmmeta.svtype  ctype:u64               maxwid:64          fixedwid1:0  fixedwid2:0  comment:""
```

### Examples
<a href="#examples"></a>

```bash
sv2ssim data.csv -ctype:a.B -data -report:N              # CSV rows as a.B tuples
sv2ssim data.tsv -separator:$'\t' -ctype:a.B -data       # read a tab-separated file
sv2ssim data.csv -data -outseparator:'|' -report:N       # rewrite with | separators
sv2ssim data.csv -ctype:a.B -ssimfile:a.b -schema        # guess a schema and its ssimfile
# round-trip dmmeta.ns through CSV, keeping two fields
acr ns:acr% | ssimfilt ^ -format:csv | sv2ssim - -ctype:dmmeta.Ns -data -field:'ns|comment'
```

```bash
inline-command: cat test/csv/2.csv; echo; sv2ssim test/csv/2.csv -ctype a.B -schema -data -prefer_signed
"Month", "1958", "1959", "1960"
"JAN",  340,  360,  417
"FEB",  318,  342,  391
"MAR",  362,  406,  419
"APR",  348,  396,  461
"MAY",  363,  420,  472
"JUN",  435,  472,  535
"JUL",  491,  548,  622
"AUG",  505,  559,  606
"SEP",  404,  463,  508
"OCT",  359,  407,  461
"NOV",  310,  362,  390
"DEC",  337,  405,  432

a.B  Month:JAN  _1958:340  _1959:360  _1960:417
a.B  Month:FEB  _1958:318  _1959:342  _1960:391
a.B  Month:MAR  _1958:362  _1959:406  _1960:419
a.B  Month:APR  _1958:348  _1959:396  _1960:461
a.B  Month:MAY  _1958:363  _1959:420  _1960:472
a.B  Month:JUN  _1958:435  _1959:472  _1960:535
a.B  Month:JUL  _1958:491  _1959:548  _1960:622
a.B  Month:AUG  _1958:505  _1959:559  _1960:606
a.B  Month:SEP  _1958:404  _1959:463  _1960:508
a.B  Month:OCT  _1958:359  _1959:407  _1960:461
a.B  Month:NOV  _1958:310  _1959:362  _1960:390
a.B  Month:DEC  _1958:337  _1959:405  _1960:432
sv2ssim.report  n_line:13  n_nonempty_line:13  n_field:4  n_wideline:1
dmmeta.ctype  ctype:a.B  comment:""
dmmeta.cfmt  cfmt:a.B.String  printfmt:Tuple  read:Y  print:Y  sep:""  genop:Y  comment:""
dmmeta.field  field:a.B.Month  arg:algo.Smallstr10  reftype:Val  dflt:""  comment:Month
dmmeta.field  field:a.B._1958  arg:i32  reftype:Val  dflt:""  comment:_1958
dmmeta.field  field:a.B._1959  arg:i32  reftype:Val  dflt:""  comment:_1959
dmmeta.field  field:a.B._1960  arg:i32  reftype:Val  dflt:""  comment:_1960
```

### Caveats
<a href="#caveats"></a>

- Check the types `-schema` picks before using them.  A text column wider than 25
  characters, and a column of decimals, both come out as `u32`, so fix those fields by hand.
- `-outseparator` prints the data rows alone, and the header line is not repeated.
- With a separator other than a comma, double quotes are ordinary characters.
- With a header, a row with more tokens than the header loses the extra ones, and
  `n_wideline` in the report counts such rows.

### Options
<a href="#options"></a>
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

The directory sv2ssim loads `dmmeta.svtype` and the built-in types from, `data` by
default.  The file to convert is `-fname`.

#### -fname -- Input file, use - for stdin
<a href="#-fname"></a>

The file to read, usually given as the first positional argument.  Use `-` to read stdin.

#### -separator -- Input field separator
<a href="#-separator"></a>

The character between fields, a comma by default.  Use `-separator:$'\t'` for a TSV file.
With any separator other than a comma, double quotes lose their meaning.

#### -outseparator -- Output separator. Default: ssim
<a href="#-outseparator"></a>

With `-data`, prints each row as separated values in place of an ssim tuple.  A comma
writes CSV, quoting where needed.  Any other separator is removed from the values before
they are joined.

#### -header -- File has header line
<a href="#-header"></a>

On by default, so the first non-empty line names the fields.  With `-header:N` every line
is data, and the fields are named `field0`, `field1` and so on.

#### -ctype -- Type tag for output tuples
<a href="#-ctype"></a>

The typetag of the tuples `-data` prints, and the ctype `-schema` defines.  `-schema`
requires it.

#### -ssimfile -- (with -schema) Create ssimfile definition
<a href="#-ssimfile"></a>

With `-schema`, also prints a `dmmeta.ssimfile` record that ties this ssimfile name to
`-ctype`.  Use it when the rows are meant to become a new ssimfile.

#### -schema -- (output)Generate schema from input file
<a href="#-schema"></a>

Prints the ctype, its cfmt, and a field per column after reading the whole file.  Each
field's `comment` holds the column name.

#### -field -- (output) Print selected fields
<a href="#-field"></a>

A regx on field names, `%` by default.  Only the matching fields appear in the `-data`
rows and the `-schema` fields.  It matches the cleaned-up names, such as `_1958`.

#### -data -- (output) Convert input file to ssim tuples
<a href="#-data"></a>

Prints every row after the header.  The row is an ssim tuple tagged with `-ctype`, or a
line of separated values when `-outseparator` is set.

#### -report -- Print final report
<a href="#-report"></a>

On by default.  Prints one `sv2ssim.report` line after the data.  Pass `-report:N` when
the output feeds another tool.

#### -prefer_signed -- Prefer signed types when given a choice
<a href="#-prefer_signed"></a>

Types integer columns as signed, `i32` or `i64`, even when no value is negative.  Without
it a column is signed only when it holds a negative value.

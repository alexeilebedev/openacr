## ssim2csv - Ssim -> csv conversion tool
<a href="#ssim2csv"></a>
ssim2csv reads ssim tuples on stdin and writes them out as CSV files, one file per typetag.
Use it to load ssim data into a spreadsheet or another tool that reads CSV.  For a single
CSV stream on stdout, [ssimfilt](/txt/exe/ssimfilt/README.md) `-format:csv` does the same
job.

### Syntax
<a href="#syntax"></a>
```usage
ssim2csv: Ssim -> csv conversion tool
Usage: ssim2csv [options]
    OPTION        TYPE    DFLT  COMMENT
    -expand       string  ""
    -ignoreQuote
    -verbose      flag          Verbosity level (0..255); alias -v; cumulative
    -debug        flag          Debug level (0..255); alias -d; cumulative
    -trace        string  ""    Trace expression: category[:filter],...; also payload_lim:N, verbose, debug, timestamps
    -help                       Print help and exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig
```

### Description
<a href="#description"></a>

Each tuple goes to the file `<typetag>.csv` in the current directory, so `dmmeta.ns` tuples
land in `dmmeta.ns.csv`.  The first tuple of a typetag opens its file, writes a header line
of field names, and prints `open <file>` on stderr.  Every later tuple with that typetag
adds one line.

Each value is enclosed in single quotes.  A single quote, newline or carriage return
inside a value becomes a space.

### Examples
<a href="#examples"></a>

```bash
acr ns:acr% -report:N | ssim2csv                       # write dmmeta.ns.csv
(acr ns:acr% -report:N; acr ctype:acr.FDb -report:N) | ssim2csv   # one file per table
echo 'dmmeta.x a:1 b:"c:2 d:3"' | ssim2csv -expand:b -ignoreQuote  # flatten field b
```

The first command writes this file:

```
'ns','nstype','license','comment'
'acr','exe','Apache','Algo Cross-Reference - ssimfile database & update tool'
'acr_compl','exe','Apache','ACR shell auto-complete for all targets'
...
```

The third one writes `a,b.c,b.d` and `1,2,3` to `dmmeta.x.csv`.

### Caveats
<a href="#caveats"></a>

- Every tuple of one typetag must carry the same fields in the same order.  A tuple that
  differs stops the run with `header mismatch`, and the file keeps only the lines before
  it.  Run the input through `acr` first, which prints every field of a table.
- A run overwrites any `<typetag>.csv` file already in the current directory.
- `acr` ends its output with a `report.acr` tuple, which lands in `report.acr.csv`.  Pass
  `-report:N` to `acr` to leave it out.

### Options
<a href="#options"></a>
#### -expand -- 
<a href="#-expand"></a>

A comma-separated list of fields whose values are tuples themselves.  ssim2csv parses each
such value and prints its parts as separate columns named `<field>.<part>`.  Nested fields
use the dotted name too, so `-expand:b,b.c` expands `b` and then its part `c`.

#### -ignoreQuote -- 
<a href="#-ignorequote"></a>

Prints values without the enclosing single quotes.  Use it when the reader of the file
treats quotes as part of the value.  The characters that ssim2csv turns into spaces stay
spaces.

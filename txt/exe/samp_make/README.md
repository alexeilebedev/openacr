## samp_make - sample program for Makefile management
<a href="#samp_make"></a>

samp_make is the sample program of [Tutorial 9: Transitioning from Makefile to OpenACR
build](/txt/tut/tut09.md).  It shows how the facts a Makefile holds become ssim tables, and
how a program built on those tables prints the Makefile again.  It parses a Makefile into
the `sampdb` tables, and it prints a Makefile from them.

### Syntax
<a href="#syntax"></a>
```usage
samp_make: sample program for Makefile management
Usage: samp_make [options]
    OPTION       TYPE    DFLT                              COMMENT
    -in          string  "data"                            Input directory or filename, - for stdin
    -target      regx    "%"                               Create Makefile for selected targets
    -parse_make                                            Parse extern/gnumake/Simple-Makefile
    -makefile    string  "extern/gnumake/Simple-Makefile"  (with parse_make) makefile to parse
    -write                                                 P(with parse_make) write ssimfiles, otherwise print them
    -verbose     flag                                      Verbosity level (0..255); alias -v; cumulative
    -debug       flag                                      Debug level (0..255); alias -d; cumulative
    -trace       string  ""                                Trace expression: category[:filter],...; also payload_lim:N, verbose, debug, timestamps
    -help                                                  Print help and exit; alias -h
    -version                                               Print version and exit
    -signature                                             Show signatures and exit; alias -sig
```

### Description
<a href="#description"></a>

The `sampdb` namespace holds five tables.  `sampdb.target` lists the targets,
`sampdb.gitfile` the source files, and `sampdb.targsrc` and `sampdb.targdep` say which
sources and which other targets each target depends on.  `sampdb.targrec` holds the
recipe text of each target.  The [sampdb page](/txt/ssimdb/sampdb/README.md) describes
the columns.

With `-parse_make`, samp_make reads a Makefile and prints the records for these tables.
With `-write` as well, it loads them into `data/sampdb` through `acr`.

Without `-parse_make`, it loads the tables from `-in` and prints a Makefile.  The default
target comes first, followed by the targets it depends on, and then the rest.

### Examples
<a href="#examples"></a>

```bash
samp_make -parse_make                     # print the sampdb records for the sample Makefile
samp_make -parse_make -write              # replace the sampdb tables with them
samp_make                                 # print the Makefile back from the tables
samp_make -target:main.o                  # print one rule
```

The last command prints:

```
main.o : defs.h main.c
	cc -c  main.c
```

### Caveats
<a href="#caveats"></a>

- The parser understands the sample Makefile, with rules, prerequisites, recipes and
  continuation lines.  It has no support for variables, pattern rules or includes.
- `-parse_make -write` truncates the `sampdb` tables before loading them, so the tables
  describe only the last Makefile parsed.

### Options
<a href="#options"></a>
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

The data directory samp_make loads the `sampdb` tables from when it prints a Makefile,
`data` by default.

#### -target -- Create Makefile for selected targets
<a href="#-target"></a>

A regx of the targets to print, `%` by default.  Selecting the default target also prints
every target it depends on.  `-target:'clean|insert.o'` prints two rules.

#### -parse_make -- Parse extern/gnumake/Simple-Makefile
<a href="#-parse_make"></a>

Parses the Makefile named by `-makefile` and prints its records, sorted.  Without this
flag samp_make prints a Makefile from the tables.

#### -makefile -- (with parse_make) makefile to parse
<a href="#-makefile"></a>

The Makefile that `-parse_make` reads, `extern/gnumake/Simple-Makefile` by default.

#### -write -- P(with parse_make) write ssimfiles, otherwise print them
<a href="#-write"></a>

With `-parse_make`, writes the records into the `sampdb` ssimfiles through `acr -trunc
-insert -write`.  Without `-parse_make` it has no effect.

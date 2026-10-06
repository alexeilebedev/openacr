## atf_cmdline - Test tool for command line parsing
<a href="#atf_cmdline"></a>
`atf_cmdline` is a test subject for the command-line parser that `amc`
generates for every tool.  It declares one option of each kind the parser
supports and prints the value each one received, so a comptest can check how a
command line is read.  It has no other use.

### Syntax
<a href="#syntax"></a>
```usage
atf_cmdline: Test tool for command line parsing
Usage: atf_cmdline [-astr:]<string> [[-anum:]<int>] [[-adbl:]<double>] -str:<string> [[-amnum:]<int>] [options]
    OPTION      TYPE    DFLT    COMMENT
    -in         string  "data"  Input directory or filename, - for stdin
    -exec                       Execv itself
    [astr]      string          Required anon string
    [anum]      int     0       Anon number
    [adbl]      double  0.0     Anon double
    [aflag]                     Anon flag
    -str        string          Required string
    -num        int     0       Required Number
    -dbl        double  0.0     Required double
    -flag                       Required flag
    -dstr       string  "blah"  Predefined string
    -dnum       int     -33     Predefined number
    -ddbl       double  0.0001  Predefined double
    -dflag              Y       Predefined flag
    -mstr...    string          String array
    -mnum...    int             Number array
    -mdbl...    double          Double array
    [amnum]...  int             Anon number array
    -fconst     enum    high    Fconst for field (high|medium|low)
    -cconst     enum    None    Fconst for arg ctype (January|February|March|April|May|June|July|August|September|October|November|December|None)
    -dregx      regx    "%"     Predefined regx
    -dpkey      string  ""      Predefined pkey
    -verbose    flag            Verbosity level (0..255); alias -v; cumulative
    -debug      flag            Debug level (0..255); alias -d; cumulative
    -trace      string  ""      Trace expression: category[:filter],...; also payload_lim:N, verbose, debug, timestamps
    -help                       Print help and exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig
```

### Description
<a href="#description"></a>

The options are fields of the ctype `command.atf_cmdline`, and together they
cover what a tool's command line can hold.  There are strings, numbers,
doubles and flags, each in a required, an optional and a defaulted form.
Array options repeat, anonymous options are read by position, and the rest
are an enum given by field constants, a month given by the constants of a
ctype, an SQL regex and a key.

`atf_cmdline` parses its command line and prints one `name:value` line per
option, in declaration order.  An array option prints one line per element, as
`mnum.0:1`.  With `-exec` it prints the command line it rebuilt from the
parsed values, then executes itself with that command line, which checks that
printing and parsing agree.

The comptests `atf_comp 'atf_cmdline.%'` drive it; their reference files are
`test/atf_comp/atf_cmdline.*`.

### Examples
<a href="#examples"></a>

```bash
atf_cmdline hello -str:x                  # the smallest valid command line
atf_cmdline hello 3 2.5 Y 7 8 -str:x      # anonymous options by position: astr anum adbl aflag amnum...
atf_cmdline hello -str:x -mnum:1 -mnum:2  # an array option, given twice
atf_cmdline -str:x -- -dash               # after --, a value that starts with a dash is positional
atf_cmdline hello -str:x -exec            # print the rebuilt command line, then run it
atf_comp 'atf_cmdline.%'                  # run the comptests that use it
```

The second command prints, among its lines:

```
astr:hello
anum:3
adbl:2.5
aflag:Y
str:x
...
amnum.0:7
amnum.1:8
```

### Caveats
<a href="#caveats"></a>

- The parser enforces only the string options `-astr` and `-str` as required.  A
  number, double or flag with no default reads as zero or `N` when it is
  missing, whatever its comment says.
- An anonymous option given by name needs the anonymous options before it:
  `-adbl:2` without `-anum` fails with `must be preceded by [-anum]`.

### Options
<a href="#options"></a>
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

The data directory every generated tool accepts.  `atf_cmdline` loads no
tables, but it still fails with `atf_cmdline.load_input` when the path does not
exist.

#### -exec -- Execv itself
<a href="#-exec"></a>

Prints the command line rebuilt from the parsed options, with every option in
`-name:value` form, and executes `atf_cmdline` again with it.  The second run
prints the values, so its output should match a run without `-exec`.

#### -astr -- Required anon string
<a href="#-astr"></a>

The first anonymous option, a required string.  The first word that is not an
option fills it.

#### -anum -- Anon number
<a href="#-anum"></a>

The second anonymous option, an integer that defaults to zero.

#### -adbl -- Anon double
<a href="#-adbl"></a>

The third anonymous option, a double that defaults to zero.

#### -aflag -- Anon flag
<a href="#-aflag"></a>

The fourth anonymous option, a flag.  By position it takes `Y` or `N`.

#### -str -- Required string
<a href="#-str"></a>

A named string with no default.  A command line without it fails with
`Missing value for required argument -str`.

#### -num -- Required Number
<a href="#-num"></a>

A named integer.  The parser reads `0x10` as 16, and a missing value reads as
zero.

#### -dbl -- Required double
<a href="#-dbl"></a>

A named double, which reads as zero when missing.

#### -flag -- Required flag
<a href="#-flag"></a>

A named flag.  `-flag` alone sets it to `Y`, and a missing flag reads as `N`.

#### -dstr -- Predefined string
<a href="#-dstr"></a>

A string that defaults to `blah`.

#### -dnum -- Predefined number
<a href="#-dnum"></a>

An integer that defaults to -33, which checks that a negative default survives.

#### -ddbl -- Predefined double
<a href="#-ddbl"></a>

A double that defaults to 0.0001.

#### -dflag -- Predefined flag
<a href="#-dflag"></a>

A flag that defaults to `Y`, so only `-dflag:N` changes it.

#### -mstr -- String array
<a href="#-mstr"></a>

A string array.  Each `-mstr` adds one element.

#### -mnum -- Number array
<a href="#-mnum"></a>

An integer array.  Each `-mnum` adds one element.

#### -mdbl -- Double array
<a href="#-mdbl"></a>

A double array.  Each `-mdbl` adds one element.

#### -amnum -- Anon number array
<a href="#-amnum"></a>

An anonymous integer array, which takes every positional word after the four
anonymous options.

#### -fconst -- Fconst for field
<a href="#-fconst"></a>

An enum whose values are field constants of this option: `high`, `medium` or
`low`, defaulting to `high`.  Any other value is an error.

#### -cconst -- Fconst for arg ctype
<a href="#-cconst"></a>

A value of the ctype `algo.Month`, which takes the month names that ctype
declares, defaulting to `None`.  Any other value, such as `Smarch`, is an
error.

#### -dregx -- Predefined regx
<a href="#-dregx"></a>

An SQL regex over `dmmeta.ctype` names, defaulting to `%`.  It prints as
`dregx.expr`.

#### -dpkey -- Predefined pkey
<a href="#-dpkey"></a>

A key of `dmmeta.ctype`, defaulting to empty.  The parser accepts a value that
names no ctype, such as `nosuch.Foo`.

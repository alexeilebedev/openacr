## strconv - A simple string utility
<a href="#strconv"></a>
strconv converts one string and prints the result.  It turns a lower_under
name into CamelCase and back, and it extracts a component of a path or key
with the path component expressions the rest of the toolchain uses.  Scripts call
it to derive one name from another.

### Syntax
<a href="#syntax"></a>
```usage
strconv: A simple string utility
Usage: strconv [-str:]<string> [options]
    OPTION         TYPE    DFLT    COMMENT
    [str]          string          String parameter
    -tocamelcase                   Convert string to camel case
    -tolowerunder                  Convert string to lower-under
    -in            string  "data"  Input directory or filename, - for stdin
    -pathcomp      string  ""      Extract path component from string
    -verbose       flag            Verbosity level (0..255); alias -v; cumulative
    -debug         flag            Debug level (0..255); alias -d; cumulative
    -trace         string  ""      Trace expression: category[:filter],...; also payload_lim:N, verbose, debug, timestamps
    -help                          Print help and exit; alias -h
    -version                       Print version and exit
    -signature                     Show signatures and exit; alias -sig
```

### Description
<a href="#description"></a>

strconv takes the string as its positional argument, applies at most one case
conversion, then applies `-pathcomp` when given, and prints what is left.  It
reads no files.

A path component expression is a sequence of steps, and each step is three
characters.  The first is the separator to search for.  The second is `L` to
find its first occurrence or `R` to find its last.  The third is `L` to keep
the text left of that occurrence or `R` to keep the text right of it.  The steps
apply in order, each one to the result of the one before.

### Examples
<a href="#examples"></a>

```bash
strconv -tocamelcase abt_md_file                 # AbtMdFile
strconv -tolowerunder AbtMdFile                  # abt_md_file
strconv dmmeta.Ctype.field -pathcomp:.LL         # dmmeta
strconv dmmeta.Ctype.field -pathcomp:.LR         # Ctype.field
strconv dmmeta.Ctype.field -pathcomp:.RL         # dmmeta.Ctype
strconv dmmeta.Ctype.field -pathcomp:.RR         # field
strconv dmmeta.Ctype.field -pathcomp:.LR.LL      # Ctype
strconv cpp/acr/main.cpp -pathcomp:/RR.LL        # main
strconv -tocamelcase foo_bar -pathcomp:.LL       # FooBar: the conversion runs first
```

### Caveats
<a href="#caveats"></a>

- The output ends in a newline only when stdout is a terminal.  Captured with
  `$(...)` it needs no trimming, and piped to a file it has no newline.
- `-tocamelcase` wins when you pass both conversions.
- The conversions treat the whole string as one identifier.
  `strconv -tolowerunder dmmeta.CtypeLen` prints `dmmeta._ctype_len`, so split
  a key with `-pathcomp` before converting its parts.
- When the separator is absent, `.LL` and `.RR` return the whole string, and
  `.LR` and `.RL` return an empty one.

### Options
<a href="#options"></a>
#### -str -- String parameter
<a href="#-str"></a>

The string to convert, and the one positional argument.  strconv fails when it
is missing, and prints nothing when it is empty.

#### -tocamelcase -- Convert string to camel case
<a href="#-tocamelcase"></a>

Convert a lower_under name to CamelCase, so `ctype_len` becomes `CtypeLen`.
It takes precedence over `-tolowerunder`.

#### -tolowerunder -- Convert string to lower-under
<a href="#-tolowerunder"></a>

Convert a CamelCase name to lower_under, so `CtypeLen` becomes `ctype_len`.
A run of capitals counts as one word, so `URLParser` becomes `urlparser`.

#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

The standard input directory, which strconv never reads.  The flag has no
effect.

#### -pathcomp -- Extract path component from string
<a href="#-pathcomp"></a>

Apply this path component expression to the string after any case
conversion.  `.RR` keeps the text after the last dot, and `/RR.LL` keeps the
file name up to its first dot.

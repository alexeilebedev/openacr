## samp_regx - Test tool for regular expressions
<a href="#samp_regx"></a>
samp_regx is a sample program that exercises the regx engine of `algo_lib`, the one every
openacr tool uses to match its arguments.  Use it to see how a pattern is read in each regx
style, whether it matches a string, and what the engine does step by step.  It can also
grep a file with a pattern.

### Syntax
<a href="#syntax"></a>
```usage
samp_regx: Test tool for regular expressions
Usage: samp_regx [-expr:]<string> [[-string:]<string>] [options]
    OPTION      TYPE    DFLT    COMMENT
    -in         string  "data"  Input directory or filename, - for stdin
    [expr]      string          Expression
    -style      enum    acr     Regx style (default|sql|acr|shell|literal)
    -regxtrace                  Trace regx innards
    -capture                    Use capture groups
    -full               Y       Match full string
    -f                          <string> is a filename, grep the lines
    -match                      Match a string, exit code represnts success
    [string]    string  ""      String to match
    -verbose    flag            Verbosity level (0..255); alias -v; cumulative
    -debug      flag            Debug level (0..255); alias -d; cumulative
    -trace      string  ""      Trace expression: category[:filter],...; also payload_lim:N, verbose, debug, timestamps
    -help                       Print help and exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig
```

### Description
<a href="#description"></a>

samp_regx reads the expression in the chosen style and matches it against the string.  The
styles differ in which characters are special:

| Style | Special characters |
|---|---|
| `default` | the usual regular expression syntax |
| `sql` | `%` is any string and `_` is any one character.  Other characters are literal, except `( ) [ ] \|` |
| `acr` | like `sql`, but `_` is literal |
| `shell` | `*` is any string and `?` is at most one character |
| `literal` | none |

Its output depends on the flags.  `-match` reports the result in the exit code,
`-regxtrace` prints the compiled expression and the match step by step, and `-capture`
prints the ranges of the match and its groups.  With `-f`, samp_regx prints the lines of
a file that match.

### Examples
<a href="#examples"></a>

```bash
samp_regx 'acr_%' acr_ed -style:sql -match && echo yes    # test one string
samp_regx 'acr_%' acr -style:acr -match; echo $?          # 1: _ is literal in acr style
samp_regx 'ns:acr%' data/dmmeta/ns.ssim -f -full:N        # grep a file
samp_regx '(a+)(b)' xaab -style:default -capture -full:N  # print match ranges
samp_regx 'acr_%' acr_ed -style:sql -regxtrace            # watch the engine work
```

The capture example prints the range of the whole match and then of each group:

```
match range: 1 4, 1 3, 3 4
```

### Caveats
<a href="#caveats"></a>

- With none of `-match`, `-regxtrace`, `-capture` or `-f`, samp_regx prints nothing and
  exits 0 whatever the result.
- The default style is `acr`, which is how `acr` reads its patterns, and there `_` is a
  literal underscore.  Tools that read the `sql` style, such as `ssimfilt`, match any
  character with `_`, so test their patterns with `-style:sql`.

### Options
<a href="#options"></a>
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

samp_regx reads no ssim data, so this option has no effect.

#### -expr -- Expression
<a href="#-expr"></a>

The pattern to test, usually given as the first positional argument.  `-style` says how
to read it.

#### -style -- Regx style
<a href="#-style"></a>

The syntax of `-expr`: `default`, `sql`, `acr`, `shell` or `literal`, as the table above
describes.  The default is `acr`.

#### -regxtrace -- Trace regx innards
<a href="#-regxtrace"></a>

Prints how the expression was parsed, the states it compiled to, and each step of the
match with the result.  It works on a single string, and samp_regx refuses it with `-f`.

#### -capture -- Use capture groups
<a href="#-capture"></a>

Records the groups of a `default` style expression and prints the range of the match and
of each group, as start and end offsets.  Combine it with `-full:N` to see where a partial
match lies.

#### -full -- Match full string
<a href="#-full"></a>

On by default, so the pattern must match the whole string.  With `-full:N` a match
anywhere in the string counts, as it does for `grep`.

#### -f -- <string> is a filename, grep the lines
<a href="#-f"></a>

Treats the string argument as a file name and prints every line of the file that matches.
Add `-full:N` unless the pattern describes whole lines.

#### -match -- Match a string, exit code represnts success
<a href="#-match"></a>

Sets the exit code to 0 when the string, or any line with `-f`, matches, and to 1 when
nothing does.  Use it in scripts.

#### -string -- String to match
<a href="#-string"></a>

The string to test, usually given as the second positional argument.  With `-f` it is a
file name.

## src_lim - Refuse a bad line pattern and a stray source file
<a href="#src_lim"></a>
src_lim polices the source tree.  It fails when a source line matches a
forbidden pattern from `dev.badline`, and when a file sits in the tree
without the registration that would build it or include it.  The `normalize`
cijob runs it over the whole tree as `src_lim -strayfile -badline:%`.

### Syntax
<a href="#syntax"></a>
```usage
src_lim: Refuse a bad line pattern and a stray source file
Usage: src_lim [options]
    OPTION      TYPE    DFLT    COMMENT
    -in         string  "data"  Input directory or filename, - for stdin
    -srcfile    regx    "%"     Filter for source files to process
    -strayfile                  Check that every source file is registered and not executable
    -badline    regx    ""      Check badline (acr badline)
    -verbose    flag            Verbosity level (0..255); alias -v; cumulative
    -debug      flag            Debug level (0..255); alias -d; cumulative
    -trace      string  ""      Trace expression: category[:filter],...; also payload_lim:N, verbose, debug, timestamps
    -help                       Print help and exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig
```

### Description
<a href="#description"></a>

src_lim reads `dev.gitfile`, `dev.targsrc` and `dev.badline` from the `-in`
directory.  It runs the checks that its flags select, prints one line per
violation, and exits non-zero when it finds one.  A run with neither
`-strayfile` nor `-badline` checks nothing.

#### Checking lines against dev.badline
<a href="#checking-lines-against-dev-badline"></a>

A `dev.badline` row names a line pattern that must not appear in a tracked
file.  Its `expr` is a regular expression that must match the whole line, its
`gitfile_regx` is an sql regx over the paths it applies to, its `exempt_regx`
names paths it does not apply to, and its `comment` says what to write instead.
`acr dev.badline` lists the rules.

Two kinds of rule share the table.  A code rule forbids an idiom, and its path
regx scopes it to C++ under `cpp/` and `include/` or to TypeScript under `ts/`.
A `secret_%` rule forbids a credential, such as a GitLab or GitHub token, an AWS
access key or a private key, and applies to every tracked file.

```bash
src_lim -badline:'secret_%'                 # look for credentials in every tracked file
acr badline:secret_% -field:exempt_regx     # the paths each secret rule leaves alone
```

`-badline:<regx>` selects rules by name and checks them against every
`dev.gitfile` that matches `-srcfile`, except generated files, `extern/` and
symlinks.  A hit prints the offending line with its location, then a
`src_lim.badline` line naming the rule.

Write `ignore:<rule>` anywhere on a line to exempt that line from that rule,
usually inside a trailing comment.  A file that cannot carry the tag, such as a
test key, is named in the rule's `exempt_regx`, and the rule's comment says why.

#### Checking for stray files
<a href="#checking-for-stray-files"></a>

`-strayfile` runs `abt -listincl` to collect every `#include` in the tree, and
then checks four things:

- every `.cpp` file in `dev.gitfile` has a `dev.targsrc` row
  (`src_lim.stray_srcfile`);
- every include that is not a system header names a versioned file, with
  headers under `build/` exempt (`src_lim.unversioned_include`);
- some source includes each versioned `.h` file outside `extern/`
  (`src_lim.stray_include`);
- no `dev.targsrc` file is executable (`src_lim.badperms`).

It prints a `src_lim.stray_include_1` line with the number of include sites
and versioned files it compared.

### Examples
<a href="#examples"></a>

```bash
src_lim -badline:%                           # check every badline rule over the tree
src_lim -badline:goto -srcfile:cpp/acr/%     # check one rule over acr's sources
src_lim -strayfile                           # find unregistered sources, unused headers and executable sources
src_lim -strayfile -badline:%                # run both, as the normalize cijob does
acr dev.badline:goto                         # show one rule's pattern and comment
```

A `goto` on the second line of a source file prints two lines:

```
cpp/foo/bar.cpp:2:     goto done;
  cpp/foo/bar.cpp:2: : src_lim.badline  badline:goto  comment:"goto is not allowed; restructure control flow"
```

### Caveats
<a href="#caveats"></a>

- `-srcfile` narrows only the C++ half of `-badline`.  src_lim always checks
  the TypeScript files, and `-strayfile` always covers the whole tree.
- `-badline` with no value, or with a pattern that names no rule, checks
  nothing and exits 0.
- An executable source file prints `src_lim.badperms` but leaves the exit code
  at zero.  Read the output to catch it.

### Options
<a href="#options"></a>
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

Read the ssim tables from this directory, which is `data` by default.  Pass an
ssim file, or `-` for stdin, to check against a hand-built set of rows.

#### -srcfile -- Filter for source files to process
<a href="#-srcfile"></a>

Check only the `dev.targsrc` files whose path matches this sql regx, such as
`cpp/acr/%`.  It applies to `-badline` over C++ sources.

#### -strayfile -- Check that every source file is registered and not executable
<a href="#-strayfile"></a>

Run the four checks under [Checking for stray files](#checking-for-stray-files)
over the whole tree.  Use it after adding, renaming or deleting a source file
or a header.

#### -badline -- Check badline (acr badline)
<a href="#-badline"></a>

Check the `dev.badline` rules whose name matches this sql regx.  Pass `%` for
every rule, or one name, such as `goto`, to try a rule you just added.

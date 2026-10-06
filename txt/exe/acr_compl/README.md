## acr_compl - ACR shell auto-complete for all targets
<a href="#acr_compl"></a>
`acr_compl` is the bash completion handler for every OpenACR tool.  It
completes option names and the values that name ssim records, and it can check
a whole command line for options the tool does not have.

### Syntax
<a href="#syntax"></a>
```usage
acr_compl: ACR shell auto-complete for all targets
Usage: acr_compl [options]
    OPTION        TYPE    DFLT    COMMENT
    -data         string  "data"  Source for completions (dir or file or -)
    -schema       string  "data"  Source for schema information
    -line         string  ""      Simulates COMP_LINE (debug)
    -point        string  ""      Simulates COMP_POINT (debug). default: whole line
    -type         string  "9"     Simulates COMP_TYPE (debug)
    -install                      Produce bash commands to install the handler
    -debug_log    string  ""      Log file for debug information, overrides ACR_COMPL_DEBUG_LOG
    -check                        Check command line validity
    -check_batch                  Batch mode: read acr_compl.checkreq from stdin, emit acr_compl.checkerr per failure
    -verbose      flag            Verbosity level (0..255); alias -v; cumulative
    -debug        flag            Debug level (0..255); alias -d; cumulative
    -trace        string  ""      Trace expression: category[:filter],...; also payload_lim:N, verbose, debug, timestamps
    -help                         Print help and exit; alias -h
    -version                      Print version and exit
    -signature                    Show signatures and exit; alias -sig
```

### Description
<a href="#description"></a>

bash runs `acr_compl` each time you press Tab on the command line of an OpenACR
tool.  `acr_compl` reads the tool's options from the `command.<tool>` fields
of the schema.  An option whose type is a table completes to that table's keys,
read from the ssimfiles.  The query argument of `acr` completes to ssimfile
names and then to keys.

#### Completing a command line
<a href="#completing-a-command-line"></a>

`acr_compl` offers the options not yet given, and completes a boolean option
whose default is true to its `:N` form.  A value completes by prefix first.
When no key starts with what you typed, `acr_compl` tries the last word of the
key, then any substring.  A tool that reads rows from a directory named on its
command line, such as `-in:<dir>`, completes from that directory.  Press Tab
twice to list the choices, where an option that needs a value shows its type.

#### Checking a command line
<a href="#checking-a-command-line"></a>

`-check` runs the same parser over a whole line and reports an unknown command
or an unknown option.  It leaves option values unchecked.
`-check_batch` does the same for many lines at once.  It reads
`acr_compl.checkreq` rows on stdin and writes an `acr_compl.checkerr` row for
each line that fails, carrying the request's `id`.  Silence on stdout means
every line passed.  [abt_md](/txt/exe/abt_md/README.md) uses it to check the
command lines in the docs.

### See also
<a href="#see-also"></a>

* [acr](/txt/exe/acr/README.md): query and edit the ssim dataset that supplies the completions

### Examples
<a href="#examples"></a>

```bash
eval "$(acr_compl -install)"              # enable completion in this shell; put it in ~/.bash_profile
acr_compl -line 'acr_in -da'              # what Tab would offer: -data, -data_dir
acr_compl -line 'acr_in -r dev.git'       # a key completion: dev.gitfile, dev.gitinfo
acr_compl -line 'acr_in -da' -type 63     # the double-Tab listing, with value types
acr_compl -line 'acr_in -bogus' -check    # validate a line; exit 1 names the bad word
printf 'acr_compl.checkreq id:1 line:"acr_in -bogus"\n' | acr_compl -check_batch   # validate many lines
```

A failed check names the word and the command:

```
$ acr_compl -line 'acr_in -bogus' -check
acr_compl.check  error:"unknown option"  value:bogus  command:acr_in
```

### Caveats
<a href="#caveats"></a>

- `-install` lists the tools that exist when you run it.  Run it again after
  adding a target, or the new tool has no completion.
- The `complete` command names `acr_compl` the way you invoked it.  Run
  `acr_compl -install` through `PATH`, since a path like `bin/acr_compl` works
  only from the top of the repo.
- Run with no `-line` and outside bash completion, `acr_compl` only prints a
  hint and exits.

### Options
<a href="#options"></a>
#### -data -- Source for completions (dir or file or -)
<a href="#-data"></a>

Name where the keys for value completion come from: a dataset directory, a
file of tuples, or `-` for stdin.  It defaults to `data`.

#### -schema -- Source for schema information
<a href="#-schema"></a>

Name where the tool options and table definitions come from, as a directory, a
file or `-`.  It defaults to `data`.

#### -line -- Simulates COMP_LINE (debug)
<a href="#-line"></a>

Give the command line to complete or check, in place of the `COMP_LINE` that
bash sets.  Use it to see what Tab would offer without a shell.

#### -point -- Simulates COMP_POINT (debug). default: whole line
<a href="#-point"></a>

Give the cursor position within `-line`, counted in characters.  It defaults to
the end of the line.

#### -type -- Simulates COMP_TYPE (debug)
<a href="#-type"></a>

Give the kind of completion bash asked for, as the `COMP_TYPE` number.  `9` is a
single Tab, the default, and `63` is the double-Tab listing, which shows the
value type of each option.

#### -install -- Produce bash commands to install the handler
<a href="#-install"></a>

Print a bash `complete` command that registers `acr_compl` for every tool in the
schema.  Evaluate its output in your shell profile.

#### -debug_log -- Log file for debug information, overrides ACR_COMPL_DEBUG_LOG
<a href="#-debug_log"></a>

Append debug output to this file.  Use it, or the `ACR_COMPL_DEBUG_LOG`
environment variable, to see what happens inside a Tab press, where stderr
would garble the command line.

#### -check -- Check command line validity
<a href="#-check"></a>

Validate the whole of `-line` and print the first error.  The exit code is 1
when the line fails.

#### -check_batch -- Batch mode: read acr_compl.checkreq from stdin, emit acr_compl.checkerr per failure
<a href="#-check_batch"></a>

Read `acr_compl.checkreq` rows (`id`, `line`) on stdin and print an
`acr_compl.checkerr` row (`id`, `err`) for each line that fails.  A row that is
not a `checkreq` fails the run on stderr.  It cannot be combined with `-check`,
`-install`, or a `-data` or `-schema` of `-`, since each of those uses stdin or
stdout.

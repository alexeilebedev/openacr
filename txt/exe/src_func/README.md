## src_func - Access / edit functions
<a href="#src_func"></a>
src_func indexes the functions of every C++ target in the tree and answers
questions about them.  Use it to list functions, to search their prototypes,
bodies and comments, and to open the matches in an editor.  It also writes the
prototype sections of hand-written headers, and it stubs out functions that
`amc` declared and nobody has written yet.

### Syntax
<a href="#syntax"></a>
```usage
src_func: Access / edit functions
Usage: src_func [[-func:]<regx>] [options]
    OPTION          TYPE    DFLT    COMMENT
    -in             string  "data"  Input directory or filename, - for stdin
    -targsrc        regx    "%"     (scan) Limit scanning to these sources only
    -acrkey         regx    "%"     Select function by acr key that caused it
    [func]          regx    "%"     Target.function regex
    -nextfile       string  ""      (action) Print name of next srcfile in targsrc list
    -other                              (with -nextfile), name of previous file
    -list                           (action) List matching functions
    -updateproto                    (action) Update prototypes in headers
    -createmissing                  (action) Create the user functions amc declared and nobody wrote
    -iffy                           (filter) Select functions that may contain errors
    -gen                            (scan) Scan generated files
    -showloc                        (output) Show file location
    -f                              (output) -sortname -showcomment -showbody
    -showstatic             Y       (filter) Allow static functions
    -matchproto     regx    "%"     (filter) Match function prototype
    -matchbody      regx    "%"     (filter) Match function body
    -matchcomment   regx    "%"     (filter) Match function comment
    -showsortkey                    (output) Display function sortkey
    -showcomment                    (output) Display function comment
    -showbody                       (output) Print function body
    -sortname                       (output) Sort functions by name
    -printssim                      (output) Print ssim tuples
    -e                              Edit found functions
    -baddecl                        Report and fail on bad declarations
    -report                         Print final report
    -verbose        flag            Verbosity level (0..255); alias -v; cumulative
    -debug          flag            Debug level (0..255); alias -d; cumulative
    -trace          string  ""      Trace expression: category[:filter],...; also payload_lim:N, verbose, debug, timestamps
    -help                           Print help and exit; alias -h
    -version                        Print version and exit
    -signature                      Show signatures and exit; alias -sig
```

### Description
<a href="#description"></a>

src_func reads `dev.target`, `dev.targsrc`, `gendb.cppsym`,
`gendb.ctypelen` and `dev.gitfile` from the `-in` directory.  It scans the
`.cpp` and `.inl.h` files of every selected `dev.targsrc` row and collects the
functions defined in them.  Generated files, which are the paths under a `gen/`
directory or under `extern/`, stay out of the scan unless you pass `-gen`.

Each function gets a name of the form `<ns>.<function>`, such as `acr.Main`.
The namespace comes from the `ns::` qualifier of the definition, and a static
function takes the name of its target.  The positional argument is an sql regx
over that name, so `acr.%` selects every function in acr and `%.Main` selects
the `Main` of every target.

#### Listing and searching functions
<a href="#listing-and-searching-functions"></a>

With no action flag, src_func lists the selected functions in file order and
prints the first line of each one.  The filter flags narrow the selection.
`-matchproto` tests the first line, `-matchbody` tests the whole body, and
`-matchcomment` tests the comment above the function.  `-acrkey` tests the
schema record that made `amc` declare the function.  The output flags choose
what to print for each match: its location, its comment, its body, or an ssim
tuple.

A function's comment is the run of `//` lines directly above its first line.

#### Opening functions in an editor
<a href="#opening-functions-in-an-editor"></a>

`-e` writes the location of every match to `temp/src_func.txt` and runs
`errlist` on that file.  errlist opens a vim quickfix list when `EDITOR` is
`vim`, and an emacs compilation buffer otherwise, so you can step from one
function to the next.

#### Updating prototypes in headers
<a href="#updating-prototypes-in-headers"></a>

`-updateproto` rewrites the prototype sections of hand-written files.  A
section is a namespace block whose opening line ends in the comment
`// update-hdr`, as `include/acr.h` does.  src_func fills the section with a
prototype for every non-static function of that namespace in the target's
sources, grouped by source file, with each function's comment above it.  Put
`srcfile:<regx>` after the tag to take prototypes only from the matching source
files.

A function that `amc` already declares in a generated header appears in the
section commented out.  src_func replaces the text between the marker and the
closing brace on every run, so you lose any edit made there.  To turn a section into a
hand-written one, delete the word `update-hdr` from its namespace line.

You rarely run `-updateproto` by hand.  The `update-hdr` script runs
[src_hdr](/txt/exe/src_hdr/README.md) over the tree with `-write`, and src_hdr
runs `src_func -updateproto` as its last step.

#### Creating missing functions
<a href="#creating-missing-functions"></a>

A `gendb.cppsym` row with `extrn:Y` is a function that `amc` declared and expects you to
write, such as a message handler or a main function.  `-createmissing` finds
every such function that no scanned source defines.  For each one it asks
`amc` for the prototype and appends an empty stub to one of the target's source
files.  The stub goes to the file that already holds the most functions sharing
its generated prefix or suffix, or to the target's first source when none does.

Run it after `amc`, once a new schema row, such as a `dmmeta.fstep` or a
`dmmeta.dispatch_msg`, has made `amc` declare new functions.  Add `-e` to open
the new stubs in the editor.

#### Checking functions
<a href="#checking-functions"></a>

`-iffy` runs a set of checks on each selected function.  Each finding prints on
its own line with an error name, and the listing that follows holds only the
functions with findings.  The checks flag these things:

- a `cstring` returned by value (`bigret`);
- a `cstring`, a `tempstr`, or a type of 32 bytes or more passed by value
  (`bigarg`, `temparg`);
- a pointer passed by reference (`ptr_byref`);
- a function defined in another target's namespace (`funcns`);
- a `gitfile:<path>` in the comment that names no `dev.gitfile` row
  (`bad_gitfile`);
- an `ind_end` indented differently from its `ind_beg` (`unbalanced_ind_beg`,
  `extra_ind_end`).

Write `ignore:<error>` anywhere in a function's comment to silence that
finding for that function.

`-baddecl` looks for function signatures that span several lines.  src_func
recognizes a function only when its signature starts in the first column and
fits on one line ending in `{`, so these are the functions its index misses.

#### Stepping through a target's source files
<a href="#stepping-through-a-target-s-source-files"></a>

`-nextfile` prints the source file that follows the given one in its target's
list of sources, and `-other` prints the one before it.  The list wraps around
at both ends.  The editor bindings in `conf/emacs.el` use it to step through a
target's files.

### Examples
<a href="#examples"></a>

```bash
src_func acr.%                                    # list every hand-written function of acr
src_func %.Main                                   # list the Main of every target
src_func acr.%Main% -showloc                      # functions whose name contains Main, with file:line
src_func acr.Main -f                              # print acr.Main with its comment and body
src_func acr.% -matchbody:%prerr% -showloc        # acr functions whose body calls prerr
src_func acr.% -matchproto:%FCtype% -showloc      # acr functions that take or return an FCtype
src_func acr.% -matchcomment:%TODO% -showloc      # acr functions with a TODO in their comment
src_func -targsrc:acr/cpp/acr/check.cpp -showloc  # every function of one source file
src_func acr.%ind_ctype% -gen -targsrc:%/gen/%    # the API amc generated for acr's ind_ctype index
src_func -acrkey:main:acr -showloc                # the function amc declared as the main of acr
src_func acr.% -showstatic:N                      # acr's functions minus the static ones
src_func acr.% -iffy                              # report questionable signatures in acr
src_func acr.Main -printssim                      # acr.Main as an ssim tuple
src_func acr.%Check% -e                           # open acr's Check functions in the editor
src_func -targsrc:acr/% -createmissing -e         # stub out and open acr's unwritten user functions
```

A plain listing prints one first line per function:

```
$ src_func acr.%Main%
void acr::Main_Check()
void acr::Main_GitTriggers(bool write_ok)
void acr::Main_ReadIn()
static void Main_RewriteOpts()
...
```

The generated API of a field comes out the same way:

```
$ src_func acr.%ind_ctype% -gen -targsrc:%/gen/%
acr::FRec* acr::ind_ctype_rec_Find(acr::FCtype& parent, const algo::strptr& key)
acr::FRec* acr::ind_ctype_rec_GetOrCreate(acr::FCtype& parent, const algo::strptr& key)
bool acr::ind_ctype_rec_InsertMaybe(acr::FCtype& parent, acr::FRec& row)
void acr::ind_ctype_rec_Remove(acr::FCtype& parent, acr::FRec& row)
```

### Caveats
<a href="#caveats"></a>

- Every regx must match the whole string.  `src_func acr` and
  `-matchbody:prerr` match nothing, and the run exits 0 with empty output.
  Write `acr.%` and `-matchbody:%prerr%`.
- An empty result does not prove a function is absent, because a wrong pattern
  prints the same nothing.  Before you trust it, run the pattern against a case
  you know exists.
- src_func takes one positional argument.  `src_func acr % %pattern%` fails
  with `too many arguments`; pass the pattern through `-matchbody`.
- The scan never reads a plain `.h` file, and it misses a function whose
  signature is split over lines.  `-baddecl` lists the second kind.
- `-gen` adds generated files to the hand-written ones.  Add
  `-targsrc:%/gen/%` to see the generated files alone.
- `-updateproto` and `-createmissing` write nothing when any selected source
  file fails to read, and the run fails.

### Options
<a href="#options"></a>
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

Read the ssim tables from this directory, which is `data` by default.  Pass an
ssim file, or `-` for stdin, to run against a hand-built set of `dev.target`
and `dev.targsrc` rows.

#### -targsrc -- (scan) Limit scanning to these sources only
<a href="#-targsrc"></a>

Scan only the `dev.targsrc` rows whose key matches this sql regx.  The key is
`<target>/<path>`, such as `acr/cpp/acr/check.cpp`, so `acr/%` covers one
target and `%/gen/%` covers the generated files.  It also limits the files
`-updateproto` rewrites and the targets `-createmissing` fills.

#### -acrkey -- Select function by acr key that caused it
<a href="#-acrkey"></a>

Keep functions whose record key matches this sql regx: the `acrkey` of the
function's `gendb.cppsym` row, such as `dmmeta.main:acr` for the main function of
acr.  A function `amc` never declared has an empty key,
which the default `%` matches and any narrower pattern excludes.

#### -func -- Target.function regex
<a href="#-func"></a>

Select functions whose name matches this sql regx, and the one positional
argument.  A name is `<ns>.<function>`, so `acr.%` selects all of acr and
`%.Main` selects the `Main` of every target.

#### -nextfile -- (action) Print name of next srcfile in targsrc list
<a href="#-nextfile"></a>

Print the path of the source file that follows this one in its target's
sources, and exit.  The path may be absolute; src_func finds the repo root by
looking for `.ffroot` in the directories above it, and prefixes that root to
the answer.

#### -other --     (with -nextfile), name of previous file
<a href="#-other"></a>

With `-nextfile`, print the source file before the given one.

#### -list -- (action) List matching functions
<a href="#-list"></a>

List the selected functions.  Listing is the default action, so you need the
flag only beside `-updateproto` or `-baddecl`, which otherwise turn it off.
`-createmissing` turns listing off even when you pass `-list`.

#### -updateproto -- (action) Update prototypes in headers
<a href="#-updateproto"></a>

Rewrite the `update-hdr` sections of the selected hand-written files from the
scanned functions, as described under [Updating prototypes in
headers](#updating-prototypes-in-headers).  It writes files in place and has
no dry run.

#### -createmissing -- (action) Create the user functions amc declared and nobody wrote
<a href="#-createmissing"></a>

Append an empty stub to a source file for every `extrn:Y` `gendb.cppsym` function
that no scanned source defines.  Only the targets that `-targsrc` selects take
part.  It writes files in place and has no dry run.

#### -iffy -- (filter) Select functions that may contain errors
<a href="#-iffy"></a>

Run the checks listed under [Checking functions](#checking-functions), print
each finding, and keep only the functions that fail a check.

#### -gen -- (scan) Scan generated files
<a href="#-gen"></a>

Scan the generated files under `gen/` and `extern/` as well as the
hand-written ones.  Pair it with `-targsrc:%/gen/%` to discover the functions
`amc` generated for a field.

#### -showloc -- (output) Show file location
<a href="#-showloc"></a>

Prefix each listed function with its `file:line:`, which editors and errlist
can jump to.

#### -f -- (output) -sortname -showcomment -showbody
<a href="#-f"></a>

Shorthand for `-sortname -showcomment -showbody`, which prints every match in
name order with its comment and its full body.

#### -showstatic -- (filter) Allow static functions
<a href="#-showstatic"></a>

Include static functions, which is the default.  Pass `-showstatic:N` to keep
only the functions other files can call.

#### -matchproto -- (filter) Match function prototype
<a href="#-matchproto"></a>

Keep functions whose first line matches this sql regx.  The first line holds
the return type, the qualified name and the parameters, so `%FCtype%` finds
the functions that take or return an `FCtype`.

#### -matchbody -- (filter) Match function body
<a href="#-matchbody"></a>

Keep functions whose body, first line included, matches this sql regx.  Put a
`%` on both sides of the text you are looking for.

#### -matchcomment -- (filter) Match function comment
<a href="#-matchcomment"></a>

Keep functions whose comment matches this sql regx.  `-matchcomment:%TODO%`
finds the functions that carry a TODO note.

#### -showsortkey -- (output) Display function sortkey
<a href="#-showsortkey"></a>

Print each function's sort key as a comment line above it.  The key is
`file:line` by default, and it starts with the function name under
`-sortname`.

#### -showcomment -- (output) Display function comment
<a href="#-showcomment"></a>

Print each function's comment above it.

#### -showbody -- (output) Print function body
<a href="#-showbody"></a>

Print each function's whole body in place of its first line.

#### -sortname -- (output) Sort functions by name
<a href="#-sortname"></a>

Sort the listing by function name.  Without the flag, functions come out in
file and line order.

#### -printssim -- (output) Print ssim tuples
<a href="#-printssim"></a>

Print one `src_func.func` tuple per function, with its name, arguments,
static and inline flags, acr key, check results, file, line range and first
line.  The other output flags have no effect with it, and the tuples can be
piped to acr or a script.

#### -e -- Edit found functions
<a href="#-e"></a>

Open the selected functions in an editor through errlist.  With
`-createmissing`, it opens the stubs that run just wrote.

#### -baddecl -- Report and fail on bad declarations
<a href="#-baddecl"></a>

Report every line that looks like a function signature split over several
lines, and fail the run when there is one.  Without the flag, src_func counts
these lines in the report and prints them only under `-verbose`.

#### -report -- Print final report
<a href="#-report"></a>

Print a `report.src_func` line at the end.  It counts the functions and lines
scanned, the static and inline functions, and the non-static, non-inline
functions whose comment is shorter than twenty characters.

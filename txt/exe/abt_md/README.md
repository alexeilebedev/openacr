## abt_md - Tool to generate markdown documentation
<a href="#abt_md"></a>
abt_md keeps the markdown documents of the tree up to date.  It regenerates the
sections of each document that come from the ssim database, reruns the commands
whose output a document quotes, and checks every link, command line and acr key
the documents mention.

### Syntax
<a href="#syntax"></a>
```usage
abt_md: Tool to generate markdown documentation
Usage: abt_md [[-readmefile:]<regx>] [[-section:]<regx>] [options]
    OPTION        TYPE    DFLT    COMMENT
    -in           string  "data"  Input directory or filename, - for stdin
    [readmefile]  regx    "%"     Regx of readme to process/show (empty=all)
    -ns           regx    ""      (overrides -readme) Process readmes for this namespace
    [section]     regx    "%"     Select specific section to process
    -update               Y       (action) Update mode: Re-generate mdfiles
    -check                        (action) Check mode: Check syntax and links
    -link                         (with -print) Print links
    -anchor                       (with -print) Print anchors
    -print                        (action) Query mode: Print .md section without evaluating
    -dry_run                      Do not write changes to disk
    -external                     Check external links as well (may fail if no internet connection)
    -evalcmd              Y       Execute inline-commands
    -tut                          (with -evalcmd) Evaluate the tutorials' inline commands too (txt/tut)
    -verbose      flag            Verbosity level (0..255); alias -v; cumulative
    -debug        flag            Debug level (0..255); alias -d; cumulative
    -trace        string  ""      Trace expression: category[:filter],...; also payload_lim:N, verbose, debug, timestamps
    -help                         Print help and exit; alias -h
    -version                      Print version and exit
    -signature                    Show signatures and exit; alias -sig
```

### Description
<a href="#description"></a>

abt_md processes the files listed in `dev.readmefile`, which holds a row for
every markdown file in `txt/`.  It reads the section templates from
`dev.mdsection`, the tracked files from `dev.gitfile`, and the schema tables it
documents from `dmmeta`.  A tool's help text comes from the generated
`cpp/gen/command_gen.cpp`, so abt_md needs no built binaries.

A plain run regenerates and checks.  It writes each selected file back in
place, and records each document's title as the `comment` of its
`dev.readmefile` row.  When it saves `txt/README.md`, it also copies it to the
top-level `README.md`.

#### Sections
<a href="#sections"></a>

abt_md splits a file into sections at its `##` and `###` headings, and matches
each heading against `dev.mdsection`.  A section whose template names a path
matching the file is generated: the title of a tool's README, its `Syntax`
block, and the `####` heading of each command-line option under `Options`.
Every other section is prose, and abt_md leaves it as it stands.

The text under an option's heading is prose too.  abt_md regenerates the
headings from the `dmmeta.field` rows of the tool's command, and puts back the
text that stood under each flag.

abt_md writes the sections back in the order of `dev.mdsection`: title,
`Syntax`, `Description`, `Limitations`, every other heading in the order it had,
the headings starting with `Example`, and `Options` last.  It also writes the
`<a href>` anchor line under each heading, so write the heading alone.

#### Inline commands
<a href="#inline-commands"></a>

A line in a fenced block that starts with `inline-command: <cmd>` is an inline
command.  abt_md runs the command under bash from the top of the tree, and
replaces the rest of the block with its output.  When the file's
`dev.readmefile` row names a `filter`, the output goes through that program
first, which is how a changing value, such as a time, becomes `***`.

A row with `sandbox:Y` runs its commands in the `abt_md`
[copy-on-write sandbox](/txt/exe/wt/README.md#copy-on-write-sandboxes).  abt_md
resets the sandbox before each such file, and the commands of the file share
it.  So one command can edit the database and a later one can show the result,
and the checkout stays untouched.

The tutorials under `txt/tut` build a sample program with `acr_ed -write`, and
running their commands takes most of a whole-tree pass.  A plain run
regenerates their sections and leaves their output blocks as they are, and
`-tut` runs them as well.

#### Link checks
<a href="#link-checks"></a>

abt_md checks every link between documents, and the anchor it names.  A target
is good when `dev.gitfile` has a row for it.  So a link to a build product, a
gitignored path or a path through a symlinked directory fails, even though it
opens in your checkout.  A relative target resolves from the directory of the
document, and a target starting with `/` resolves from the top of the tree.

#### Command lines a doc shows are checked
<a href="#command-lines-a-doc-shows-are-checked"></a>

abt_md checks the command lines a document shows, so a reader does not find a
wrong flag by running it.  It takes each line of a `bash`, `sh` or unlabeled
fenced block, and each backticked span in prose whose first word names an
executable.  So `` `acr -check` `` is checked, and `` `unique:Y` `` is not.
Only the first command of a line is checked, since what follows a pipe belongs
to another program.  [acr_compl](/txt/exe/acr_compl/README.md) does the
checking, and abt_md reports each unknown option with its file and line.

Write a value the reader supplies as `<like-this>`.  abt_md substitutes a value
for it before the check, so the flags after it are still checked.

#### Acr keys a doc mentions are checked
<a href="#acr-keys-a-doc-mentions-are-checked"></a>

A backticked `<ns>.<ssimfile>:<pkey>` is an acr key, and abt_md reports it when
no record has that key.  So `` `dmmeta.reftype:Val` `` must name a record.  The
bare form `` `reftype:Val` `` is how an attribute appears inside a tuple, and
abt_md leaves it alone, as it does a value carrying `%`.

Qualify a key when the record itself is the subject: `` `dmmeta.ns:acr` `` is a
namespace.  Leave it bare when the sentence states an attribute of another
record: a `dmmeta.field` with `reftype:Val`.

### Examples
<a href="#examples"></a>

```bash
abt_md                                     # regenerate every document and check the tree
abt_md txt/exe/acr/README.md               # regenerate one document
abt_md -ns:acr                             # regenerate every document of the acr namespace
abt_md -evalcmd:N                          # regenerate sections without running inline commands
abt_md -tut                                # run the tutorials' inline commands as well
abt_md -check                              # check links, command lines and keys, writing nothing
abt_md -dry_run -ns:acr                    # regenerate and check the acr documents, saving nothing
abt_md -print -link txt/exe/acr/README.md      # list the links one document holds
abt_md -print -anchor txt/exe/acr/README.md    # list the anchors one document defines
abt_md -print -dry_run txt/exe/acr/README.md -section:%Options   # print one section
abt_md txt/exe/acr/README.md -section:%Syntax  # regenerate one section of one document
abt_md -check -external                    # check the web links too
```

### Caveats
<a href="#caveats"></a>

- `abt_md -check` never regenerates.  It exits zero over a section that has
  drifted from its ssim rows, and the `quickreadme` citest under
  `bin/normalize` then fails on it.  Run plain `abt_md` first, then `-check`.
- `-check` refuses a selection, since a link out of the selection leads to a
  file the run never read.  Use `-update` with a selection, and `-check` over
  the whole tree.
- An inline command's output shows the tree as it was when the command ran, so
  a row you added by hand to try something lands in the document.  An inline-command hunk you did not set out to make
  is that, so revert it.
- Two runs at once overwrite each other's inline command output, because both
  write the same file under `temp/`.  Never start one beside a comptest sweep or
  a `bin/normalize`.
- Run abt_md twice after adding a command-line option.  The first run writes the
  new option's heading without the blank line after it, and the second run adds
  it.
- The text under an option's heading is kept by the flag's name.  Renaming a
  flag drops the text written under the old name.
- abt_md drops a section with no text, so a heading you add stays only once
  something is written under it.  It adds a missing generated section, such as
  `Syntax`, and writes no prose, so a deleted document loses its prose for good.
- A markdown file needs a `dev.readmefile` row before abt_md sees it.  Create
  it with `acr_ed -create -srcfile txt/<path>.md -write`, and remove it with
  `acr_ed -del -srcfile txt/<path>.md -write`.
- A merge conflict in a generated section is never resolved by hand.  Take the
  upstream side of the file, finish the rebase, and run abt_md.  A file whose
  conflict markers span a heading is refused.

### Options
<a href="#options"></a>
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

Read the ssim tables from this directory, `data` by default.  Given a single
file, or `-` for stdin, abt_md records no titles and skips the check that
every `dev.mdsection` path matches a document.

#### -readmefile -- Regx of readme to process/show (empty=all)
<a href="#-readmefile"></a>

Select the documents to process, as a regex over the paths in
`dev.readmefile`.  The default `%` selects every document.  A regex that
matches nothing is an `abt_md.nomatch` error.

#### -ns -- (overrides -readme) Process readmes for this namespace
<a href="#-ns"></a>

Select the documents that belong to the namespaces matching this regex, such as
a tool's README and the other pages under its directory.  When `-ns` is given,
abt_md ignores `-readmefile`.

#### -section -- Select specific section to process
<a href="#-section"></a>

Regenerate only the sections whose heading line matches this regex, and leave
the rest as they are.  The regex matches the whole line, so write
`-section:%Options`.  With `-print -dry_run`, it picks the sections to print.

#### -update -- (action) Update mode: Re-generate mdfiles
<a href="#-update"></a>

Regenerate the selected documents, run their inline commands, and save them.
This is the default.  It runs the checks too, leaving out the link check when
the run has a selection.  `-check` and `-print` turn it off.

#### -check -- (action) Check mode: Check syntax and links
<a href="#-check"></a>

Check the whole tree and write nothing: links and anchors, command lines, acr
keys, and the `dev.mdsection` paths.  It refuses a selection:

```ssim
abt_md.narrow_check  nselect:16  nreadmefile:1284  comment:"-check reads every readme; drop -readmefile and -ns, or use -update to regenerate a selection"
```

To work through the errors one by one, run it under `errlist`:

```bash
errlist abt_md -check
```

#### -link -- (with -print) Print links
<a href="#-link"></a>

With `-print`, list every link in the selected documents as an `abt_md.link`
line carrying its location, text and target.  The document text is not
printed.

#### -anchor -- (with -print) Print anchors
<a href="#-anchor"></a>

With `-print`, list every anchor the selected documents define as an
`abt_md.anchor` line.  The document text is not printed.

#### -print -- (action) Query mode: Print .md section without evaluating
<a href="#-print"></a>

Print the selected documents as abt_md reads them, without regenerating or
saving anything.  Add `-dry_run` and `-section` to print only some sections,
or `-link` or `-anchor` to list those in place of the text.

#### -dry_run -- Do not write changes to disk
<a href="#-dry_run"></a>

Regenerate and check as usual, but save nothing: no document and no title.
Inline commands still run.

#### -external -- Check external links as well (may fail if no internet connection)
<a href="#-external"></a>

Also check every http and https link, by fetching it with `curl --head`.  A
link counts as broken whenever curl returns an error, so a machine with no curl
or no network fails every one of them.  A broken web link raises the exit code
without printing its address, so run with `-v` to see the addresses checked.

#### -evalcmd -- Execute inline-commands
<a href="#-evalcmd"></a>

Run the inline commands, which is the default.  `-evalcmd:N` skips all of them,
the tutorials' included, and leaves each output block as the file holds it.
The generated sections are regenerated either way.

#### -tut -- (with -evalcmd) Evaluate the tutorials' inline commands too (txt/tut)
<a href="#-tut"></a>

Also run the inline commands of the tutorials under `txt/tut`.  Three of them
run up to a dozen `acr_ed -write` passes each in the sandbox, so a plain run
leaves their output blocks alone.  `bin/normalize` runs the plain form, and the
`comp` cijob runs `abt_md -tut` as its `readme_tut` citest, which fails with
`atf_ci.modified_files` when a tutorial's output moved.  Run `abt_md -tut`
after changing a tool a tutorial runs or a table it prints, and commit what it
rewrites.

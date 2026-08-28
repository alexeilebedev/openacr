## abt_md - Tool to generate markdown documentation


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
    -verbose      flag            Verbosity level (0..255); alias -v; cumulative
    -debug        flag            Debug level (0..255); alias -d; cumulative
    -help                         Print help and exit; alias -h
    -version                      Print version and exit
    -signature                    Show signatures and exit; alias -sig
```

### Description
<a href="#description"></a>

abt_md reads and processes files specified in the `dev.readme` table.
The file sections, marked with "##" or deeper, are loaded, sorted
according to the `mdsection` table, and written back out.
Sections are matched against `mdsection` with a pattern, by default `Content` is used.

Any commands specified in the file using `inline-command: ...` are evaluated and the contents
of the section until the next "```" are replaced with the output of the command.

A top-level `README.md` file is created, and the contents of the readme files are either placed
in-line or as links into this top-level file.

`abt_md` can be invoked on a specific file by providing that file name. This is useful
when debugging inline-commands.

The contents of an `md` file can be evaluated inside a sandbox by setting `sandbox:Y`
in the `readme` record. The sandbox is a
[copy-on-write farm](/txt/exe/wt/README.md#copy-on-write-sandboxes) managed by `wt`:
it is reset before each such file, commands run inside it and accumulate state,
and no side effect ever reaches the checkout.

#### Command lines a doc shows are checked
<a href="#command-lines-a-doc-shows-are-checked"></a>

A doc can print a command line whose tool cannot parse it, and a reader finds
out by running it.  So every command line in a processed file is validated:
the lines of a `bash`, `sh` or unlabelled fenced block, and any inline
`` `...` `` span in prose.  A span is taken for a command line when its first
word names an executable — `` `acr -check` `` is one, `` `unique:Y` `` is not —
and only the first command of the line is checked, since what follows a pipe or
a redirect belongs to another program.  The check itself is
[`acr_compl -check_batch`](/txt/exe/acr_compl/README.md), which validates option
names and reports an unknown one against its file and line.

A value the reader has to supply is written `<like-this>`, and the placeholder
is replaced with a value before the line is checked, so flags written after one
are still validated.

#### Acr keys a doc mentions are checked
<a href="#acr-keys-a-doc-mentions-are-checked"></a>

A backticked `<ns>.<ssimfile>:<pkey>` is read as an acr key and is looked up:
`` `dmmeta.reftype:Val` `` names a record, and a qualified key that no record
has is reported.  The namespace is what marks the key.  The bare form
`` `reftype:Val` `` is how an attribute appears inside a tuple — a sentence
saying a field carries `reftype:Val` names no record of its own — so the leaf
form is left alone, and prose can go on naming attributes without qualifying
them.  A value carrying `%` names a set rather than a key, and is not looked
up.

Qualify a key when the record itself is the subject (`` `amsdb.grptype:log`
binds the address ``); leave it bare when the sentence states an attribute of
some other record (``a `dmmeta.field` with `reftype:Count` ``).

### Options
<a href="#options"></a>
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

#### -readmefile -- Regx of readme to process/show (empty=all)
<a href="#-readmefile"></a>

#### -ns -- (overrides -readme) Process readmes for this namespace
<a href="#-ns"></a>

#### -section -- Select specific section to process
<a href="#-section"></a>

#### -update -- (action) Update mode: Re-generate mdfiles
<a href="#-update"></a>

#### -check -- (action) Check mode: Check syntax and links
<a href="#-check"></a>

Example: resolve link errors in .md files:
```bash
errlist abt_md -check
```

A link leaves the document it is written in, so checking links is checking the
whole graph at once: the target of a link out of the selection is a file the run
never read, and the anchor it names is one the run never saw.  `-check` therefore
reads every readme, and it refuses a selection rather than skipping the part it
was asked to do:

```ssim
abt_md.narrow_check  nselect:16  nreadmefile:1284  comment:"-check reads every readme; drop -readmefile and -ns, or use -update to regenerate a selection"
```

`-update` is a different request and still takes a selection, since regenerating
one namespace's documents is the ordinary way to use the tool, and a regeneration
is not a run that was asked to check anything.

A link's target is good when the repo tracks it -- when `dev.gitfile` holds a row
for it -- rather than when it happens to exist on disk.  The two differ for a
build artifact, a gitignored path, and anything reached through a symlinked
directory: all of them resolve in one checkout and none of them is a page anyone
else can open.  `doc` answers by the same rule, so a link that passes here is one
the reader can follow.

#### -link -- (with -print) Print links
<a href="#-link"></a>

#### -anchor -- (with -print) Print anchors
<a href="#-anchor"></a>

#### -print -- (action) Query mode: Print .md section without evaluating
<a href="#-print"></a>

#### -dry_run -- Do not write changes to disk
<a href="#-dry_run"></a>

#### -external -- Check external links as well (may fail if no internet connection)
<a href="#-external"></a>

Every http and https link is fetched with `curl --head`, and the link is
reported as broken whenever that command returns a nonzero status. A status
that came from the server and a status that means curl never ran are not
distinguished, so a host with no curl installed, or with no route to the
network, reports every external link in the tree as broken. Read a failure
from `-external` as a reason to check the address by hand rather than as
proof that it has rotted.

#### -evalcmd -- Execute inline-commands
<a href="#-evalcmd"></a>

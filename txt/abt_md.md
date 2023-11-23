## abt_md: Tool to generate markdown documentation



### Syntax

```
abt_md: Tool to generate markdown documentation
Usage: abt_md [[-readme:]<regx>] [[-section:]<regx>] [options]
    OPTION      TYPE    DFLT    COMMENT
    -in         string  "data"  Input directory or filename, - for stdin
    [readme]    regx    "%"     Regx of readme to process/show (empty=all)
    -ns         regx    ""      (overrides -readme) Process readmes for this namespace
    [section]   regx    "%"     Select specific section to process
    -print                      Print output to screen
    -dry_run                    Do not write changes to disk
    -verbose    int             Verbosity level (0..255); alias -v; cumulative
    -debug      int             Debug level (0..255); alias -d; cumulative
    -help                       Print help and exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig

```

### Description

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
in the `readme` record. Any changes produced by the commands are discarded after the file finishes
processing.

### Inputs

`abt_md` takes the following tables on input:
```
CTYPE                COMMENT
dmmeta.Ns            Namespace (for in-memory database, protocol, etc)
dmmeta.Dispsigcheck  Check signature of input data against executable's version
dmmeta.Ctype         Conceptual type (or C type)
dmmeta.Ssimfile      Ssim tuple name for structure
dev.Readme
dev.Readmens
```


## abt_md - Tool to generate markdown documentation


### Table Of Contents
<a href="#table-of-contents"></a>
* [Syntax](#syntax)
* [Description](#description)
* [Options](#options)
* [Sources](#sources)
* [Inputs](#inputs)

### Syntax
<a href="#syntax"></a>
```
abt_md: Tool to generate markdown documentation
Usage: abt_md [[-readme:]<regx>] [[-section:]<regx>] [options]
    OPTION      TYPE    DFLT    COMMENT
    -in         string  "data"  Input directory or filename, - for stdin
    [readme]    regx    "%"     Regx of readme to process/show (empty=all)
    -ns         regx    ""      (overrides -readme) Process readmes for this namespace
    [section]   regx    "%"     Select specific section to process
    -update             Y       (action) Update mode: Re-generate mdfiles
    -check                      (action) Check mode: Check syntax and links
    -link                       (with -print) Print links
    -anchor                     (with -print) Print anchors
    -print                      (action) Query mode: Print .md section without evaluating
    -dry_run                    Do not write changes to disk
    -verbose    int             Verbosity level (0..255); alias -v; cumulative
    -debug      int             Debug level (0..255); alias -d; cumulative
    -help                       Print help and exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig

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
in the `readme` record. Any changes produced by the commands are discarded after the file finishes
processing.

### Options
<a href="#options"></a>

#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

#### -readme -- Regx of readme to process/show (empty=all)
<a href="#-readme"></a>

#### -ns -- (overrides -readme) Process readmes for this namespace
<a href="#-ns"></a>

#### -section -- Select specific section to process
<a href="#-section"></a>

#### -update -- (action) Update mode: Re-generate mdfiles
<a href="#-update"></a>

#### -check -- (action) Check mode: Check syntax and links
<a href="#-check"></a>

Example: resolve link errors in .md files:
```
errlist abt_md -check
```

#### -link -- (with -print) Print links
<a href="#-link"></a>

#### -anchor -- (with -print) Print anchors
<a href="#-anchor"></a>

#### -print -- (action) Query mode: Print .md section without evaluating
<a href="#-print"></a>

#### -dry_run -- Do not write changes to disk
<a href="#-dry_run"></a>

### Sources
<a href="#sources"></a>
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/abt_md/file_section.cpp](/cpp/abt_md/file_section.cpp)||
|[cpp/abt_md/main.cpp](/cpp/abt_md/main.cpp)||
|[cpp/abt_md/mdsection.cpp](/cpp/abt_md/mdsection.cpp)||
|[cpp/abt_md/sources.cpp](/cpp/abt_md/sources.cpp)||
|[cpp/gen/abt_md_gen.cpp](/cpp/gen/abt_md_gen.cpp)||
|[include/abt_md.h](/include/abt_md.h)||
|[include/gen/abt_md_gen.h](/include/gen/abt_md_gen.h)||
|[include/gen/abt_md_gen.inl.h](/include/gen/abt_md_gen.inl.h)||

### Inputs
<a href="#inputs"></a>
`abt_md` takes the following tables on input:
|ssimfile|comment|
|---|---|
|[dmmeta.nstype](/txt/ssimdb/dmmeta/nstype.md)|Namespace type|
|[dmmeta.ns](/txt/ssimdb/dmmeta/ns.md)|Namespace (for in-memory database, protocol, etc)|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|
|[amcdb.tclass](/txt/ssimdb/amcdb/tclass.md)|AMC template class|
|[dev.targsrc](/txt/ssimdb/dev/targsrc.md)|List of sources for target|
|[dmmeta.reftype](/txt/ssimdb/dmmeta/reftype.md)|Field type constructor (e.g. reference type)|
|[dmmeta.ctype](/txt/ssimdb/dmmeta/ctype.md)|Struct|
|[dmmeta.field](/txt/ssimdb/dmmeta/field.md)|Specify field of a struct|
|[dmmeta.substr](/txt/ssimdb/dmmeta/substr.md)|Specify that the field value is computed from a substring of another field|
|[dmmeta.ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|File with ssim tuples|
|[dev.scriptfile](/txt/ssimdb/dev/scriptfile.md)|Known script file|
|[dev.readmecat](/txt/ssimdb/dev/readmecat.md)|sorted categories of txt/ *.md files presented in dev.readme|
|[dev.readme](/txt/ssimdb/dev/readme.md)|File containing documentation|
|[dmmeta.gconst](/txt/ssimdb/dmmeta/gconst.md)|Import ssim table columns as fconst for a field|
|[dmmeta.fconst](/txt/ssimdb/dmmeta/fconst.md)|Specify enum value (integer + string constant) for a field|
|[atfdb.comptest](/txt/ssimdb/atfdb/comptest.md)||


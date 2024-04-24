## src_hdr - Manage source copyright+license header in source files and scripts


### Table Of Contents
<a href="#table-of-contents"></a>
* [Syntax](#syntax)
* [Options](#options)
* [Sources](#sources)
* [Inputs](#inputs)

### Syntax
<a href="#syntax"></a>
```
src_hdr: Manage source copyright+license header in source files and scripts
Usage: src_hdr [options]
    OPTION             TYPE    DFLT    COMMENT
    -in                string  "data"  Input directory or filename, - for stdin
    -targsrc           regx    ""      Regx of targsrc to update
    -write                             Update files in-place
    -indent                            Indent source files
    -update_copyright                  Update copyright year for current company
    -scriptfile        regx    ""      Regx of scripts to update header
    -verbose           int             Verbosity level (0..255); alias -v; cumulative
    -debug             int             Debug level (0..255); alias -d; cumulative
    -help                              Print help and exit; alias -h
    -version                           Print version and exit
    -signature                         Show signatures and exit; alias -sig

```

### Options
<a href="#options"></a>

#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

#### -targsrc -- Regx of targsrc to update
<a href="#-targsrc"></a>

#### -write -- Update files in-place
<a href="#-write"></a>

#### -indent -- Indent source files
<a href="#-indent"></a>

#### -update_copyright -- Update copyright year for current company
<a href="#-update_copyright"></a>

#### -scriptfile -- Regx of scripts to update header
<a href="#-scriptfile"></a>

### Sources
<a href="#sources"></a>
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/gen/src_hdr_gen.cpp](/cpp/gen/src_hdr_gen.cpp)||
|[cpp/src_hdr/hdr.cpp](/cpp/src_hdr/hdr.cpp)||
|[include/gen/src_hdr_gen.h](/include/gen/src_hdr_gen.h)||
|[include/gen/src_hdr_gen.inl.h](/include/gen/src_hdr_gen.inl.h)||

### Inputs
<a href="#inputs"></a>
`src_hdr` takes the following tables on input:
|ssimfile|comment|
|---|---|
|[dev.license](/txt/ssimdb/dev/license.md)|Source code license|
|[dmmeta.ns](/txt/ssimdb/dmmeta/ns.md)|Namespace (for in-memory database, protocol, etc)|
|[dev.target](/txt/ssimdb/dev/target.md)|Build target|
|[dev.targsrc](/txt/ssimdb/dev/targsrc.md)|List of sources for target|
|[dev.scriptfile](/txt/ssimdb/dev/scriptfile.md)|Known script file|
|[dmmeta.nsx](/txt/ssimdb/dmmeta/nsx.md)|Control code-generation and exception handling options for process/library|
|[dev.copyright](/txt/ssimdb/dev/copyright.md)|Copyrighting entity|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|


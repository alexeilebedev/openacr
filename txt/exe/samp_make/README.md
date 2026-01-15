## samp_make - sample program for Makefile management
<a href="#samp_make"></a>
This program is a sample program covered in [Tutorial on transitioning from Makefile to OpenACR build](/txt/tut/tut9_make.md).
It parses a specified Makefile, produces all requisite ssimfiles, and generates Makefile(s) from its [Inputs](#inputs).

### Table Of Contents
<a href="#table-of-contents"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Toc -->
&nbsp;&nbsp;&bull;&nbsp;  [Syntax](#syntax)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Internals](#internals)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Options](#options)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Sources](#sources)<br/>
&#128196; [samp_make - Internals](/txt/exe/samp_make/internals.md)<br/>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Toc -->

### Syntax
<a href="#syntax"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Syntax -->
```
samp_make: sample program for Makefile management
Usage: samp_make [options]
    OPTION       TYPE    DFLT                              COMMENT
    -in          string  "data"                            Input directory or filename, - for stdin
    -target      regx    "%"                               Create Makefile for selected targets
    -parse_make                                            Parse extern/gnumake/Simple-Makefile
    -makefile    string  "extern/gnumake/Simple-Makefile"  (with parse_make) makefile to parse
    -write                                                 P(with parse_make) write ssimfiles, otherwise print them
    -verbose     flag                                      Verbosity level (0..255); alias -v; cumulative
    -debug       flag                                      Debug level (0..255); alias -d; cumulative
    -help                                                  Print help and exit; alias -h
    -version                                               Print version and exit
    -signature                                             Show signatures and exit; alias -sig

```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Syntax -->

### Internals
<a href="#internals"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Internals -->
for internals description see [samp_make - sample program for Makefile management](/txt/exe/samp_make/internals.md)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Internals -->

### Options
<a href="#options"></a>

<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Options -->
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

#### -target -- Create Makefile for selected targets
<a href="#-target"></a>

#### -parse_make -- Parse extern/gnumake/Simple-Makefile
<a href="#-parse_make"></a>

#### -makefile -- (with parse_make) makefile to parse
<a href="#-makefile"></a>

#### -write -- P(with parse_make) write ssimfiles, otherwise print them
<a href="#-write"></a>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Options -->

### Inputs
<a href="#inputs"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Inputs -->
`samp_make` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|
|[sampdb.gitfile](/txt/ssimdb/sampdb/gitfile.md)|samp_make list of gitfile sources|
|[sampdb.targdep](/txt/ssimdb/sampdb/targdep.md)|samp_make targets dependencies|
|[sampdb.target](/txt/ssimdb/sampdb/target.md)|samp_make targets list|
|[sampdb.targrec](/txt/ssimdb/sampdb/targrec.md)|samp_make recipe for a target|
|[sampdb.targsrc](/txt/ssimdb/sampdb/targsrc.md)|samp_make targets source dependencies|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Inputs -->

### Sources
<a href="#sources"></a>
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/gen/samp_make_gen.cpp](/cpp/gen/samp_make_gen.cpp)||
|[cpp/samp_make/samp_make.cpp](/cpp/samp_make/samp_make.cpp)||
|[include/gen/samp_make_gen.h](/include/gen/samp_make_gen.h)||
|[include/gen/samp_make_gen.inl.h](/include/gen/samp_make_gen.inl.h)||
|[include/samp_make.h](/include/samp_make.h)||


## atf_gcli - test harness for gcli


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Toc -->
&nbsp;&nbsp;&bull;&nbsp;  [Syntax](#syntax)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Options](#options)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Sources](#sources)<br/>
&#128196; [atf_gcli - Internals](/txt/exe/atf_gcli/internals.md)<br/>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Toc -->

### Syntax
<a href="#syntax"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Syntax -->
```
atf_gcli: test harness for gcli
Usage: atf_gcli [options]
    OPTION          TYPE    DFLT    COMMENT
    -in             string  "data"  Input directory or filename, - for stdin
    -gtblacttst     regx    "%"     gcli tests regx
    -id             string  ""      Use this issue for $ISSUE substitutions
    -mr             string  ""      Use this issue for $MR substitutions
    -note           string  ""      Use this issue for $NOTE substitutions
    -capture                        Capture test command output into gverbtout
    -skip_init                      Skip setting local files - already set
    -skip_git_init                  Skip setting local files - already set
    -dry_run                        Print actions, do not perform
    -verbose        int             Verbosity level (0..255); alias -v; cumulative
    -debug          int             Debug level (0..255); alias -d; cumulative
    -help                           Print help and exit; alias -h
    -version                        Print version and exit
    -signature                      Show signatures and exit; alias -sig

```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Syntax -->

### Options
<a href="#options"></a>

<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Options -->
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

#### -gtblacttst -- gcli tests regx
<a href="#-gtblacttst"></a>

#### -id -- Use this issue for $ISSUE substitutions
<a href="#-id"></a>

#### -mr -- Use this issue for $MR substitutions
<a href="#-mr"></a>

#### -note -- Use this issue for $NOTE substitutions
<a href="#-note"></a>

#### -capture -- Capture test command output into gverbtout
<a href="#-capture"></a>

#### -skip_init -- Skip setting local files - already set
<a href="#-skip_init"></a>

#### -skip_git_init -- Skip setting local files - already set
<a href="#-skip_git_init"></a>

#### -dry_run -- Print actions, do not perform
<a href="#-dry_run"></a>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Options -->

### Inputs
<a href="#inputs"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Inputs -->
`atf_gcli` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[gclidb.gclienv](/txt/ssimdb/gclidb/gclienv.md)||
|[gclidb.gtblact](/txt/ssimdb/gclidb/gtblact.md)||
|[gclidb.gtblacttst](/txt/ssimdb/gclidb/gtblacttst.md)||
|[gclidb.gtblacttstout](/txt/ssimdb/gclidb/gtblacttstout.md)||
|[gclidb.gclienvsub](/txt/ssimdb/gclidb/gclienvsub.md)||
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Inputs -->

### Sources
<a href="#sources"></a>
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/atf/atf_gcli.cpp](/cpp/atf/atf_gcli.cpp)||
|[cpp/gen/atf_gcli_gen.cpp](/cpp/gen/atf_gcli_gen.cpp)||
|[include/atf_gcli.h](/include/atf_gcli.h)||
|[include/gen/atf_gcli_gen.h](/include/gen/atf_gcli_gen.h)||
|[include/gen/atf_gcli_gen.inl.h](/include/gen/atf_gcli_gen.inl.h)||


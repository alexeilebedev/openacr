## amsspy - List ams sessions and monitor traffic on host


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- abt_md.toc_beg -->
&nbsp;&nbsp;&bull;&nbsp;  [Internals](#internals)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Syntax](#syntax)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Options](#options)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
<!-- abt_md.toc_end -->

### Internals
<a href="#internals"></a>
&#128196; [amsspy - Internals](/txt/gen/amsspy/amsspy.md)<br/>

### Syntax
<a href="#syntax"></a>
```
amsspy: List ams sessions and monitor traffic on host
Usage: amsspy [options]
    OPTION      TYPE    DFLT    COMMENT
    -in         string  "data"  Input directory or filename, - for stdin
    -session    regx    "%"     Session regex
    -list                       List sessions
    -shm                        List shms
    -spy        regx    ""      Spy on named shared memory segment
    -f          enum    decode  (output) Output format (decode|raw|text|json)
    -clean                      Unlink orphaned /dev/shm ams segments and exit
    -dump                       Dump every segment's header + reader offsets/lag (out-of-band) and exit
    -verbose    flag            Verbosity level (0..255); alias -v; cumulative
    -debug      flag            Debug level (0..255); alias -d; cumulative
    -help                       Print help and exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig
```

### Options
<a href="#options"></a>
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

#### -session -- Session regex
<a href="#-session"></a>

#### -list -- List sessions
<a href="#-list"></a>

#### -shm -- List shms
<a href="#-shm"></a>

#### -spy -- Spy on named shared memory segment
<a href="#-spy"></a>

#### -f -- (output) Output format
<a href="#-f"></a>

#### -clean -- Unlink orphaned /dev/shm ams segments and exit
<a href="#-clean"></a>

#### -dump -- Dump every segment's header + reader offsets/lag (out-of-band) and exit
<a href="#-dump"></a>

### Inputs
<a href="#inputs"></a>
`amsspy` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|

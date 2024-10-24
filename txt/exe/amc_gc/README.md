## amc_gc - Garbage collector for in-memory databases


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Toc -->
&nbsp;&nbsp;&bull;&nbsp;  [Syntax](#syntax)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Description](#description)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Options](#options)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
&#128196; [amc_gc - Internals](/txt/exe/amc_gc/internals.md)<br/>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Toc -->

### Syntax
<a href="#syntax"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Syntax -->
```
amc_gc: Garbage collector for in-memory databases
Usage: amc_gc [options]
    OPTION      TYPE    DFLT    COMMENT
    -target     regx    "%"     Target to test-build
    -key        regx    ""      ACR query selecting records to eliminate, e.g. dmmeta.ctype:amc.%
    -include                    Garbage collect includes for specified target
    -in         string  "data"  Input directory or filename, - for stdin
    -verbose    int             Verbosity level (0..255); alias -v; cumulative
    -debug      int             Debug level (0..255); alias -d; cumulative
    -help                       Print help and exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig

```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Syntax -->

### Description
<a href="#description"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Description -->

`amc_gc` is a tool for removing unused records and thus optimizing a program by simplifying it

`amc_gc` takes a `target` regex and an acr query as an argument. It finds all records
matching the query, then it tries to remove them one by one, and rebuilds the targets
(using abt) in a sandbox dir. If build succeeds with one of the records deleted
it means it wasn't needed in the first place.

Let's illustrate `amc_gc` by creating a new program and inputting a table.

    $ acr_ed -create -target sample -write
    $ acr_ed -create -finput -target sample -ssimfile dmmeta.ns -write

Since the `ns` table is unused, `sample` will compile even if we remove it. This is the
case that `amc_gc` detects, and can remove the table:

    $ amc_gc -target:sample -key:ctype:sample.%
    amc_gc.begin  tot_rec:2  n_cppline:259802  watch_cmd:"watch head -50 temp/amc_gc.build"
    amc_gc.analyze  query:dmmeta.ctype:sample.FDb  eliminate:N  rec_no:1  tot_rec:2  n_del:0  n_cppline:259802  n_cppline_del:0
    amc_gc.analyze  query:dmmeta.ctype:sample.FNs  eliminate:Y  rec_no:2  tot_rec:2  n_del:1  n_cppline:259341  n_cppline_del:461
    report.amc_gc  key:ctype:sample.%  n_match:2  n_del:1  n_cppline:259341  n_cppline_del:461

And indeed, `amc_gc` successfully garbage collects the table.
Let's finish by deleting the unused target

    $ acr_ed -del -target sample -write

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Description -->

### Options
<a href="#options"></a>

<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Options -->
#### -target -- Target to test-build
<a href="#-target"></a>

#### -key -- ACR query selecting records to eliminate, e.g. dmmeta.ctype:amc.%
<a href="#-key"></a>

#### -include -- Garbage collect includes for specified target
<a href="#-include"></a>

#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Options -->

### Inputs
<a href="#inputs"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Inputs -->
`amc_gc` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Inputs -->


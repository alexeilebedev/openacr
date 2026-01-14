## atf_amc - Unit tests for amc (see amctest table)


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Toc -->
&nbsp;&nbsp;&bull;&nbsp;  [Syntax](#syntax)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Options](#options)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Input Messages](#input-messages)<br/>
&#128196; [atf_amc - Internals](/txt/exe/atf_amc/internals.md)<br/>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Toc -->

### Syntax
<a href="#syntax"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Syntax -->
```
atf_amc: Unit tests for amc (see amctest table)
Usage: atf_amc [[-amctest:]<regx>] [options]
    OPTION      TYPE    DFLT    COMMENT
    -in         string  "data"  Input directory or filename, - for stdin
    [amctest]   regx    "%"     SQL regex, selecting test to run
    -dofork             Y       Use fork
    -q                          Quiet mode
    -verbose    flag            Verbosity level (0..255); alias -v; cumulative
    -debug      flag            Debug level (0..255); alias -d; cumulative
    -help                       Print help and exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig

```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Syntax -->

### Options
<a href="#options"></a>

<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Options -->
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

#### -amctest -- SQL regex, selecting test to run
<a href="#-amctest"></a>

#### -dofork -- Use fork
<a href="#-dofork"></a>

#### -q -- Quiet mode
<a href="#-q"></a>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Options -->

### Inputs
<a href="#inputs"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Inputs -->
`atf_amc` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Inputs -->

### Input Messages
<a href="#input-messages"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:InputMessages -->
`atf_amc` Consumes the following messages via atf_amc.Disp ()
|Message|Comment|
|---|---|
|[atf_amc.DispType1](/txt/exe/atf_amc/internals.md#atf_amc-disptype1)|Dispfilter test 1|
|[atf_amc.DispType2](/txt/exe/atf_amc/internals.md#atf_amc-disptype2)|Dispfilter test 2|
|[atf_amc.DispType3](/txt/exe/atf_amc/internals.md#atf_amc-disptype3)|Dispfilter test 3|

`atf_amc` Consumes the following messages via atf_amc.Msgs ()
|Message|Comment|
|---|---|
|[atf_amc.VarlenK](/txt/exe/atf_amc/internals.md#atf_amc-varlenk)|An optional instance of atf_unit.A|

`atf_amc` Consumes the following messages via atf_amc.Ssimfiles ()
|Message|Comment|
|---|---|
|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|Struct|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:InputMessages -->


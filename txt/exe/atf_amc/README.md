## atf_amc - Unit tests for amc (see amctest table)


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- abt_md.toc_beg -->
&nbsp;&nbsp;&bull;&nbsp;  [Internals](#internals)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Options](#options)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Input Messages](#input-messages)<br/>
<!-- abt_md.toc_end -->

### Internals
<a href="#internals"></a>
&#128196; [atf_amc - Internals](/txt/gen/atf_amc/atf_amc.md)<br/>

### Options
<a href="#options"></a>
#### -in -- Input directory or filename, - for stdin
<a href="#-in"></a>

#### -amctest -- SQL regex, selecting test to run
<a href="#-amctest"></a>

#### -dofork -- Use fork
<a href="#-dofork"></a>

#### -q -- Quiet mode
<a href="#-q"></a>

### Inputs
<a href="#inputs"></a>
`atf_amc` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|

### Input Messages
<a href="#input-messages"></a>
`atf_amc` Consumes the following messages via atf_amc.Disp ()
|Message|Comment|
|---|---|
|[atf_amc.DispType1](/txt/gen/atf_amc/atf_amc.md#atf_amc-disptype1)|Dispfilter test 1|
|[atf_amc.DispType2](/txt/gen/atf_amc/atf_amc.md#atf_amc-disptype2)|Dispfilter test 2|
|[atf_amc.DispType3](/txt/gen/atf_amc/atf_amc.md#atf_amc-disptype3)|Dispfilter test 3|

`atf_amc` Consumes the following messages via atf_amc.Disp2 (Dispfilter with match_all:Y over the DispType messages)
|Message|Comment|
|---|---|
|[atf_amc.DispType1](/txt/gen/atf_amc/atf_amc.md#atf_amc-disptype1)|Dispfilter test 1|
|[atf_amc.DispType2](/txt/gen/atf_amc/atf_amc.md#atf_amc-disptype2)|Dispfilter test 2|
|[atf_amc.DispType3](/txt/gen/atf_amc/atf_amc.md#atf_amc-disptype3)|Dispfilter test 3|

`atf_amc` Consumes the following messages via atf_amc.Msgs ()
|Message|Comment|
|---|---|
|[atf_amc.VarlenB](/txt/gen/atf_amc/atf_amc.md#atf_amc-varlenb)|Varlen element: scaled lenfld over a byte-granular payload|
|[atf_amc.VarlenK](/txt/gen/atf_amc/atf_amc.md#atf_amc-varlenk)|An optional instance of atf_unit.A|

`atf_amc` Consumes the following messages via atf_amc.Ssimfiles ()
|Message|Comment|
|---|---|
|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|Struct|

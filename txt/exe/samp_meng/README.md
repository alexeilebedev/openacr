## samp_meng - Sample matching engine


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Toc -->
&nbsp;&nbsp;&bull;&nbsp;  [Syntax](#syntax)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Options](#options)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Input Messages](#input-messages)<br/>
&#128196; [samp_meng - Internals](/txt/exe/samp_meng/internals.md)<br/>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Toc -->

### Syntax
<a href="#syntax"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Syntax -->
```
samp_meng: Sample matching engine
Usage: samp_meng [options]
    OPTION      TYPE    DFLT    COMMENT
    -in         string  "data"  Input directory or filename, - for stdin
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

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Options -->

### Inputs
<a href="#inputs"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Inputs -->
`samp_meng` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Inputs -->

### Input Messages
<a href="#input-messages"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:InputMessages -->
`samp_meng` Consumes the following messages via samp_meng.In ()
|Message|Comment|
|---|---|
|[samp_meng.CancelReqMsg](/txt/exe/samp_meng/internals.md#samp_meng-cancelreqmsg)|From user: cancel request|
|[samp_meng.MassCancelReqMsg](/txt/exe/samp_meng/internals.md#samp_meng-masscancelreqmsg)|From user: cancel mass request|
|[samp_meng.NewOrderReqMsg](/txt/exe/samp_meng/internals.md#samp_meng-neworderreqmsg)|From user: new order request|
|[samp_meng.NewSymbolReqMsg](/txt/exe/samp_meng/internals.md#samp_meng-newsymbolreqmsg)|To matching engine: new symbol request|
|[samp_meng.NewUserReqMsg](/txt/exe/samp_meng/internals.md#samp_meng-newuserreqmsg)|To matching engine: new user request|
|[samp_meng.TextMsg](/txt/exe/samp_meng/internals.md#samp_meng-textmsg)|Debug message|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:InputMessages -->


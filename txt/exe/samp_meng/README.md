## samp_meng - Sample matching engine


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
&#128196; [samp_meng - Internals](/txt/gen/samp_meng/samp_meng.md)<br/>

### Options
<a href="#options"></a>
#### -proc -- <proc_id>[,<prefix>[,<in_grp>,<out_grp>]]
<a href="#-proc"></a>

### Inputs
<a href="#inputs"></a>
`samp_meng` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|

### Input Messages
<a href="#input-messages"></a>
`samp_meng` Consumes the following messages via samp_meng.In ()
|Message|Comment|
|---|---|
|[ams.SampMengCancelReqMsg](/txt/protocol/ams/README.md#ams-sampmengcancelreqmsg)||
|[ams.SampMengMassCancelReqMsg](/txt/protocol/ams/README.md#ams-sampmengmasscancelreqmsg)||
|[ams.SampMengNewOrderReqMsg](/txt/protocol/ams/README.md#ams-sampmengneworderreqmsg)||
|[ams.SampMengNewSymbolReqMsg](/txt/protocol/ams/README.md#ams-sampmengnewsymbolreqmsg)||
|[ams.SampMengNewUserReqMsg](/txt/protocol/ams/README.md#ams-sampmengnewuserreqmsg)||
|[ams.SampMengTextMsg](/txt/protocol/ams/README.md#ams-sampmengtextmsg)||

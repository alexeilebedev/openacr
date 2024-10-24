## ams.StreamHbMsg - Stream heartbeat


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)ams.StreamHbMsg

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|ams.MsgHeader|Base|||
|member|ams.Member|Val||Primary key|
|pos|ams.StreamPos|Val||Read/write position|
|wbudget|u32|Val||Max. offset for writing|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->


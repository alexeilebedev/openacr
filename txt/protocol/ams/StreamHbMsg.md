## txt/protocol/ams/StreamHbMsg.md


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)ams.StreamHbMsg

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|ams.MsgHeader|Base|
|member|ams.Member|Val||Primary key|
|pos|ams.StreamPos|Val||Read/write position|
|wbudget|u32|Val||Max. offset for writing|


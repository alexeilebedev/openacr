## txt/protocol/ams/Seqmsg.md


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)ams.Seqmsg

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|ams.MsgHeader|Base|
|seqmsg_id|ams.SeqmsgId|Val||Message ID|
|tsc|u64|Val||TSC of sender|
|payload|ams.MsgHeader|Opt||Actual Payload|


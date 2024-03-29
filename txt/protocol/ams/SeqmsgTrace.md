## txt/protocol/ams/SeqmsgTrace.md


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)ams.SeqmsgTrace

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|ams.MsgHeader|Base|
|proc_id|ams.ProcId|Val||Proces ID|
|mode|u8|Val|0|0=read, 1=write (2 or more =read)|
|seqmsg_id|ams.SeqmsgId|Val||Message ID|
|tsc|u64|Val||TSC of sender|
|payload|ams.MsgHeader|Opt||Actual Payload|


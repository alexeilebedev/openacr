## ams.Seqmsg - Sequenced stream message


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)ams.Seqmsg

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|ams.MsgHeader|Base|||
|seqmsg_id|ams.SeqmsgId|Val||Message ID|
|tsc|u64|Val||TSC of sender|
|payload|ams.MsgHeader|Opt||Actual Payload|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->


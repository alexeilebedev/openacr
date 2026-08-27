## dmmeta.msg - Message ctype layout summary (amc-derived)


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Msg

* file:[data/dmmeta/msg.ssim](/data/dmmeta/msg.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ctype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Pkey](/txt/exe/amc/reftype/Pkey.md)|||
|payloadhdr|[dmmeta.Payloadhdr](/txt/ssimdb/dmmeta/payloadhdr.md)|[Pkey](/txt/exe/amc/reftype/Pkey.md)||Payload header framing this message|
|type|u64|[Val](/txt/exe/amc/reftype/Val.md)||Msgtype number (evaluated)|
|len|i32|[Val](/txt/exe/amc/reftype/Val.md)||Fixed size of the message in bytes|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

### Related
<a href="#related"></a>
These ssimfiles reference dmmeta.msg

* [dmmeta.msgfield via msg](/txt/ssimdb/dmmeta/msgfield.md) - Message ctype this field belongs to

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/gen/amc/amc.md) as [amc.FMsg](/txt/gen/amc/amc.md#amc-fmsg)

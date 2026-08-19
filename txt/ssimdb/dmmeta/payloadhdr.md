## dmmeta.payloadhdr - Header ctype framing messages: typefld/lenfld layout (amc-derived)


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Payloadhdr

* file:[data/dmmeta/payloadhdr.ssim](/data/dmmeta/payloadhdr.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ctype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)|||
|typeoffset|i32|[Val](/txt/exe/amc/reftype.md#val)||Byte offset of the type field|
|typewidth|i32|[Val](/txt/exe/amc/reftype.md#val)||Byte width of the type field|
|typebig|bool|[Val](/txt/exe/amc/reftype.md#val)||Type field is big-endian|
|lenoffset|i32|[Val](/txt/exe/amc/reftype.md#val)||Byte offset of the length field|
|lenwidth|i32|[Val](/txt/exe/amc/reftype.md#val)||Byte width of the length field|
|lenbig|bool|[Val](/txt/exe/amc/reftype.md#val)||Length field is big-endian|
|lenscale|i32|[Val](/txt/exe/amc/reftype.md#val)|1|Multiplier for length field value|
|lenextra|i32|[Val](/txt/exe/amc/reftype.md#val)||Extra bytes to add to scaled length|
|len|i32|[Val](/txt/exe/amc/reftype.md#val)||Fixed size of the header in bytes|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype.md#val)|||

### Related
<a href="#related"></a>
These ssimfiles reference dmmeta.payloadhdr

* [dmmeta.msg via payloadhdr](/txt/ssimdb/dmmeta/msg.md) - Payload header framing this message

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/gen/amc/amc.md) as [amc.FPayloadhdr](/txt/gen/amc/amc.md#amc-fpayloadhdr)

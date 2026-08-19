## dmmeta.msgfield - Resolved leaf field layout of a message ctype (amc-derived)


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Msgfield

* file:[data/dmmeta/msgfield.ssim](/data/dmmeta/msgfield.ssim)

italicised fields: *msg, name* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|msgfield|[algo.Smallstr100](/txt/protocol/algo/README.md#algo-smallstr100)|[Val](/txt/exe/amc/reftype.md#val)|||
|*msg*|*[dmmeta.Msg](/txt/ssimdb/dmmeta/msg.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*Message ctype this field belongs to<br>/RL of msgfield*|
|*name*|*[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)*|*[Val](/txt/exe/amc/reftype.md#val)*||*Leaf field name (dotted for nested)<br>/RR of msgfield*|
|offset|i32|[Val](/txt/exe/amc/reftype.md#val)||Absolute byte offset in the message|
|arg|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)||Leaf scalar ctype (u8..u64, char, bool, f32, f64)|
|width|i32|[Val](/txt/exe/amc/reftype.md#val)||Byte width; for a char array the array length|
|bigend|bool|[Val](/txt/exe/amc/reftype.md#val)||Field is big-endian|
|varlen|bool|[Val](/txt/exe/amc/reftype.md#val)||Varlen tail: extends to the end of the message|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype.md#val)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/gen/amc/amc.md) as [amc.FMsgfield](/txt/gen/amc/amc.md#amc-fmsgfield)

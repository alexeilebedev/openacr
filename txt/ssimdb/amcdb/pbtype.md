## amcdb.pbtype - Protobuf scalar type: wire type and lib_pb codec binding for the pbuf codec


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)amcdb.Pbtype

* file:[data/amcdb/pbtype.ssim](/data/amcdb/pbtype.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|pbtype|[algo.Smallstr20](/txt/protocol/algo/README.md#algo-smallstr20)|[Val](/txt/exe/amc/reftype/Val.md)||Protobuf type name as written in dmmeta.fpbuf.pb_type|
|wire_type|u8|[Val](/txt/exe/amc/reftype/Val.md)||Protobuf wire type: 0=varint 1=64-bit 2=length-delimited 5=32-bit|
|suffix|[algo.Smallstr20](/txt/protocol/algo/README.md#algo-smallstr20)|[Val](/txt/exe/amc/reftype/Val.md)||suffix appended to the lib_pb Encode/Decode function names|
|encval|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype/Val.md)||Encoder value expression; $fldval is the field access expression|
|decval|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype/Val.md)||Decoder target expression; $fldval is the field access expression|
|nondflt|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype/Val.md)||C++ expression, true when $fldval differs from the proto3 default|
|argtype|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype/Val.md)|""|C++ arg ctype the lib_pb codec functions accept; empty when special-cased (enum, message)|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/gen/amc/amc.md) as [amc.FPbtype](/txt/gen/amc/amc.md#amc-fpbtype)

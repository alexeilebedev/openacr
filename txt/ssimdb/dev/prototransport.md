## dev.prototransport - Protocol/transport binding


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Prototransport

* file:[data/dev/prototransport.ssim](/data/dev/prototransport.ssim)

italicised fields: *netproto, transport* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|prototransport|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype.md#val)|||
|*netproto*|*[dev.Netproto](/txt/ssimdb/dev/netproto.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*<br>/RL of prototransport*|
|*transport*|*[dev.Transport](/txt/ssimdb/dev/transport.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*<br>/RR of prototransport*|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype.md#val)|||

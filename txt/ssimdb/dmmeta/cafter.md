## dmmeta.cafter - Control amc processing order for unrelated types (used in rare situations)


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Cafter

* file:[data/dmmeta/cafter.ssim](/data/dmmeta/cafter.ssim)

italicised fields: *ctype, after* are [**fldfunc**](/txt/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|cafter|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)|||
|*ctype*|*[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*<br>/RL of cafter*|
|*after*|*[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*<br>/RR of cafter*|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [amc](/txt/exe/amc/internals.md) as [amc.FCafter](/txt/exe/amc/internals.md#amc-fcafter)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->


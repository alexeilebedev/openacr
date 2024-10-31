## dmmeta.fcmap - Bidirectional mapping between fconst values


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Fcmap

* file:[data/dmmeta/fcmap.ssim](/data/dmmeta/fcmap.ssim)

italicised fields: *leftField, leftVal, rightVal, rightField* are [**fldfunc**](/txt/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|fcmap|[algo.Smallstr250](/txt/protocol/algo/README.md#algo-smallstr250)|[Val](/txt/exe/amc/reftypes.md#val)||cstring maybe?|
|*leftField*|*[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*<br>=LL/LL of fcmap*|
|*leftVal*|*[algo.Smallstr40](/txt/protocol/algo/README.md#algo-smallstr40)*|*[Val](/txt/exe/amc/reftypes.md#val)*||*<br>=LL/LR of fcmap*|
|*rightVal*|*[algo.Smallstr40](/txt/protocol/algo/README.md#algo-smallstr40)*|*[Val](/txt/exe/amc/reftypes.md#val)*||*<br>=LR/LR of fcmap*|
|*rightField*|*[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*<br>=LR/LL of fcmap*|
|bidir|bool|[Val](/txt/exe/amc/reftypes.md#val)|true||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [amc](/txt/exe/amc/internals.md) as [amc.FFcmap](/txt/exe/amc/internals.md#amc-ffcmap)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->


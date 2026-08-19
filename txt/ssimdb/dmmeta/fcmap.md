## dmmeta.fcmap - Bidirectional mapping between fconst values


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Fcmap

* file:[data/dmmeta/fcmap.ssim](/data/dmmeta/fcmap.ssim)

italicised fields: *leftField, leftVal, rightVal, rightField* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|fcmap|[algo.Smallstr250](/txt/protocol/algo/README.md#algo-smallstr250)|[Val](/txt/exe/amc/reftype.md#val)||cstring maybe?|
|*leftField*|*[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*<br>=LL/LL of fcmap*|
|*leftVal*|*[algo.Smallstr40](/txt/protocol/algo/README.md#algo-smallstr40)*|*[Val](/txt/exe/amc/reftype.md#val)*||*<br>=LL/LR of fcmap*|
|*rightVal*|*[algo.Smallstr40](/txt/protocol/algo/README.md#algo-smallstr40)*|*[Val](/txt/exe/amc/reftype.md#val)*||*<br>=LR/LR of fcmap*|
|*rightField*|*[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*<br>=LR/LL of fcmap*|
|bidir|bool|[Val](/txt/exe/amc/reftype.md#val)|true||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype.md#val)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/gen/amc/amc.md) as [amc.FFcmap](/txt/gen/amc/amc.md#amc-ffcmap)

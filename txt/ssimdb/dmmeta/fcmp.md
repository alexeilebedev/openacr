## dmmeta.fcmp - Request versionsort or case-insensitive sort for field


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Fcmp

* file:[data/dmmeta/fcmp.ssim](/data/dmmeta/fcmp.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)||Target field|
|versionsort|bool|[Val](/txt/exe/amc/reftypes.md#val)||Use version sort (detect embedded integers in string)|
|casesens|bool|[Val](/txt/exe/amc/reftypes.md#val)|true|Comparison is case-sensitive|
|extrn|bool|[Val](/txt/exe/amc/reftypes.md#val)||Whether implementation is external|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [amc](/txt/exe/amc/internals.md) as [amc.FFcmp](/txt/exe/amc/internals.md#amc-ffcmp)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->


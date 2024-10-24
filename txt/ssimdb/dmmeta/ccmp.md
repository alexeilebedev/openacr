## dmmeta.ccmp - Generate hash function


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Ccmp

* file:[data/dmmeta/ccmp.ssim](/data/dmmeta/ccmp.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ctype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)||Target ctype|
|extrn|bool|[Val](/txt/exe/amc/reftypes.md#val)||Whether implementation is external|
|genop|bool|[Val](/txt/exe/amc/reftypes.md#val)||Generate C++ comparison operators (<,>, etc)|
|order|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|minmax|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [amc](/txt/exe/amc/internals.md) as [amc.FCcmp](/txt/exe/amc/internals.md#amc-fccmp)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->


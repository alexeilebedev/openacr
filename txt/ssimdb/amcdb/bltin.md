## amcdb.bltin - Specify properties of a C built-in type


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)amcdb.Bltin

* file:[data/amcdb/bltin.ssim](/data/amcdb/bltin.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ctype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|||
|likeu64|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|bigendok|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|issigned|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [acr](/txt/exe/acr/internals.md) as [acr.FBltin](/txt/exe/acr/internals.md#acr-fbltin)
* [amc](/txt/exe/amc/internals.md) as [amc.FBltin](/txt/exe/amc/internals.md#amc-fbltin)
* [lib_ctype](/txt/lib/lib_ctype/README.md) as [lib_ctype.FBltin](/txt/lib/lib_ctype/README.md#lib_ctype-fbltin)
* [sv2ssim](/txt/exe/sv2ssim/internals.md) as [sv2ssim.FBltin](/txt/exe/sv2ssim/internals.md#sv2ssim-fbltin)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->


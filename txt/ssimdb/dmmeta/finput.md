## dmmeta.finput - Describe input table of a program


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Finput

* file:[data/dmmeta/finput.ssim](/data/dmmeta/finput.ssim)

italicised fields: *ns* are [**fldfunc**](/txt/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)||Target field to read|
|*ns*|*[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*<br>.LL of field*|
|extrn|bool|[Val](/txt/exe/amc/reftypes.md#val)||Call user-provided function|
|update|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|strict|bool|[Val](/txt/exe/amc/reftypes.md#val)|true|Exist process if record contains error|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [abt_md](/txt/exe/abt_md/internals.md) as [abt_md.FFinput](/txt/exe/abt_md/internals.md#abt_md-ffinput)
* [acr_in](/txt/exe/acr_in/internals.md) as [acr_in.FFinput](/txt/exe/acr_in/internals.md#acr_in-ffinput)
* [amc](/txt/exe/amc/internals.md) as [amc.FFinput](/txt/exe/amc/internals.md#amc-ffinput)
* [amc_vis](/txt/exe/amc_vis/internals.md) as [amc_vis.FFinput](/txt/exe/amc_vis/internals.md#amc_vis-ffinput)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->


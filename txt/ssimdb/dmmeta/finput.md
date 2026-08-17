## dmmeta.finput - Describe input table of a program


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Finput

* file:[data/dmmeta/finput.ssim](/data/dmmeta/finput.ssim)

italicised fields: *ns* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)||Target field to read|
|*ns*|*[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*<br>.LL of field*|
|update|bool|[Val](/txt/exe/amc/reftype.md#val)|||
|strict|bool|[Val](/txt/exe/amc/reftype.md#val)|true|Exist process if record contains error|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype.md#val)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [acr_in](/txt/gen/acr_in/acr_in.md) as [acr_in.FFinput](/txt/gen/acr_in/acr_in.md#acr_in-ffinput)
* [amc](/txt/gen/amc/amc.md) as [amc.FFinput](/txt/gen/amc/amc.md#amc-ffinput)
* [amc_vis](/txt/gen/amc_vis/amc_vis.md) as [amc_vis.FFinput](/txt/gen/amc_vis/amc_vis.md#amc_vis-ffinput)

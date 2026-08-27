## dmmeta.foutput - Generate function to save index/table back to disk


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Foutput

* file:[data/dmmeta/foutput.ssim](/data/dmmeta/foutput.ssim)

italicised fields: *ns* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftype/Pkey.md)|||
|*ns*|*[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)*|*[Pkey](/txt/exe/amc/reftype/Pkey.md)*||*<br>.LL of field*|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/gen/amc/amc.md) as [amc.FFoutput](/txt/gen/amc/amc.md#amc-ffoutput)

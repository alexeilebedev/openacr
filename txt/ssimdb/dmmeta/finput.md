## dmmeta.finput - Describe input table of a program


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Finput

* file:[data/dmmeta/finput.ssim](/data/dmmeta/finput.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|Pkey||Target field to read|
|ns|[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)|Pkey||<br>.LL of field|
|extrn|bool|Val||Call user-provided function|
|update|bool|Val|
|strict|bool|Val|true|Exist process if record contains error|
|comment|algo.Comment|Val|

### Used In Executables
<a href="#used-in-executables"></a>
* [acr_in](/txt/exe/acr_in/README.md) as acr_in.FFinput
* [amc](/txt/exe/amc/README.md) as amc.FFinput
* [amc_vis](/txt/exe/amc_vis/README.md) as amc_vis.FFinput


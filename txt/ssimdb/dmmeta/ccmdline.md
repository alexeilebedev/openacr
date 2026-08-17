## dmmeta.ccmdline - tie cmdline to basecmdline


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Ccmdline

* file:[data/dmmeta/ccmdline.ssim](/data/dmmeta/ccmdline.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ctype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)|||
|read|bool|[Val](/txt/exe/amc/reftype.md#val)|true|Read the command line automatically from main?|
|basecmdline|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)||Optional command line to merge with this one|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype.md#val)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [acr_compl](/txt/gen/acr_compl/acr_compl.md) as [acr_compl.FCcmdline](/txt/gen/acr_compl/acr_compl.md#acr_compl-fccmdline)
* [amc](/txt/gen/amc/amc.md) as [amc.FCcmdline](/txt/gen/amc/amc.md#amc-fccmdline)

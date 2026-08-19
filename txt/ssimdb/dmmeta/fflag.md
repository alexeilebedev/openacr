## dmmeta.fflag - Options for command-line flags


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Fflag

* file:[data/dmmeta/fflag.ssim](/data/dmmeta/fflag.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)|||
|cumulative|bool|[Val](/txt/exe/amc/reftype.md#val)||Accumulate on every read|
|emptyval|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|""||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype.md#val)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [acr_compl](/txt/gen/acr_compl/acr_compl.md) as [acr_compl.FFflag](/txt/gen/acr_compl/acr_compl.md#acr_compl-ffflag)
* [amc](/txt/gen/amc/amc.md) as [amc.FFflag](/txt/gen/amc/amc.md#amc-ffflag)

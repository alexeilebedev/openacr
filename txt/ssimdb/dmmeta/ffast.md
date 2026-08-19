## dmmeta.ffast - FAST properties for field


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Ffast

* file:[data/dmmeta/ffast.ssim](/data/dmmeta/ffast.ssim)

italicised fields: *field, ctype, fastinstr* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ffast|[algo.Smallstr100](/txt/protocol/algo/README.md#algo-smallstr100)|[Val](/txt/exe/amc/reftype.md#val)|||
|*field*|*[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*<br>.RL of ffast*|
|*ctype*|*[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*<br>.RL.RL of ffast*|
|*fastinstr*|*[dmmeta.Fastinstr](/txt/ssimdb/dmmeta/fastinstr.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*<br>.RR of ffast*|
|name|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype.md#val)|||
|id|u32|[Val](/txt/exe/amc/reftype.md#val)|||
|pos|u32|[Val](/txt/exe/amc/reftype.md#val)|||
|optional|bool|[Val](/txt/exe/amc/reftype.md#val)|||
|encoding|[dmmeta.Fastenc](/txt/ssimdb/dmmeta/fastenc.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)|||
|op|[dmmeta.Fastop](/txt/ssimdb/dmmeta/fastop.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)|||
|value|[algo.CppExpr](/txt/protocol/algo/CppExpr.md)|[Val](/txt/exe/amc/reftype.md#val)|||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype.md#val)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/gen/amc/amc.md) as [amc.FFfast](/txt/gen/amc/amc.md#amc-fffast)

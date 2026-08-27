## dmmeta.ffast - FAST properties for field


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Ffast

* file:[data/dmmeta/ffast.ssim](/data/dmmeta/ffast.ssim)

italicised fields: *field, ctype, fastinstr* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ffast|[algo.Smallstr100](/txt/protocol/algo/README.md#algo-smallstr100)|[Val](/txt/exe/amc/reftype/Val.md)|||
|*field*|*[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)*|*[Pkey](/txt/exe/amc/reftype/Pkey.md)*||*<br>.RL of ffast*|
|*ctype*|*[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)*|*[Pkey](/txt/exe/amc/reftype/Pkey.md)*||*<br>.RL.RL of ffast*|
|*fastinstr*|*[dmmeta.Fastinstr](/txt/ssimdb/dmmeta/fastinstr.md)*|*[Pkey](/txt/exe/amc/reftype/Pkey.md)*||*<br>.RR of ffast*|
|name|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype/Val.md)|||
|id|u32|[Val](/txt/exe/amc/reftype/Val.md)|||
|pos|u32|[Val](/txt/exe/amc/reftype/Val.md)|||
|optional|bool|[Val](/txt/exe/amc/reftype/Val.md)|||
|encoding|[dmmeta.Fastenc](/txt/ssimdb/dmmeta/fastenc.md)|[Pkey](/txt/exe/amc/reftype/Pkey.md)|||
|op|[dmmeta.Fastop](/txt/ssimdb/dmmeta/fastop.md)|[Pkey](/txt/exe/amc/reftype/Pkey.md)|||
|value|[algo.CppExpr](/txt/protocol/algo/CppExpr.md)|[Val](/txt/exe/amc/reftype/Val.md)|||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/gen/amc/amc.md) as [amc.FFfast](/txt/gen/amc/amc.md#amc-fffast)

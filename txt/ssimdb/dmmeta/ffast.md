## dmmeta.ffast - FAST properties for field


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Ffast

* file:[data/dmmeta/ffast.ssim](/data/dmmeta/ffast.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ffast|algo.Smallstr100|Val|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|Pkey||<br>.RL of ffast|
|ctype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|Pkey||<br>.RL.RL of ffast|
|fastinstr|[dmmeta.Fastinstr](/txt/ssimdb/dmmeta/fastinstr.md)|Pkey||<br>.RR of ffast|
|name|algo.Smallstr50|Val|
|id|u32|Val|
|pos|u32|Val|
|optional|bool|Val|
|encoding|[dmmeta.Fastenc](/txt/ssimdb/dmmeta/fastenc.md)|Pkey|
|op|[dmmeta.Fastop](/txt/ssimdb/dmmeta/fastop.md)|Pkey|
|value|algo.CppExpr|Val|
|comment|algo.Comment|Val|

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/exe/amc/README.md) as amc.FFfast


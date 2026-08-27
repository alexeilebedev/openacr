## dmmeta.cstr - Specify that type behaves like a string


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Cstr

* file:[data/dmmeta/cstr.ssim](/data/dmmeta/cstr.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ctype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Pkey](/txt/exe/amc/reftype/Pkey.md)|||
|strequiv|bool|[Val](/txt/exe/amc/reftype/Val.md)||Use strptr instead of this type when possible|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [acr_ed](/txt/gen/acr_ed/acr_ed.md) as [acr_ed.FCstr](/txt/gen/acr_ed/acr_ed.md#acr_ed-fcstr)
* [amc](/txt/gen/amc/amc.md) as [amc.FCstr](/txt/gen/amc/amc.md#amc-fcstr)

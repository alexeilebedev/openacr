## dmmeta.fcmp - Request versionsort or case-insensitive sort for field


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Fcmp

* file:[data/dmmeta/fcmp.ssim](/data/dmmeta/fcmp.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftype/Pkey.md)||Target field|
|versionsort|bool|[Val](/txt/exe/amc/reftype/Val.md)||Use version sort (detect embedded integers in string)|
|casesens|bool|[Val](/txt/exe/amc/reftype/Val.md)|true|Comparison is case-sensitive|
|extrn|bool|[Val](/txt/exe/amc/reftype/Val.md)||Whether implementation is external|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/gen/amc/amc.md) as [amc.FFcmp](/txt/gen/amc/amc.md#amc-ffcmp)

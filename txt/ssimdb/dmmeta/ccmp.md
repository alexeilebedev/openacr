## dmmeta.ccmp - Generate hash function


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Ccmp

* file:[data/dmmeta/ccmp.ssim](/data/dmmeta/ccmp.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ctype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Pkey](/txt/exe/amc/reftype/Pkey.md)||Target ctype|
|extrn|bool|[Val](/txt/exe/amc/reftype/Val.md)||Whether implementation is external|
|genop|bool|[Val](/txt/exe/amc/reftype/Val.md)||Generate C++ comparison operators (<,>, etc)|
|order|bool|[Val](/txt/exe/amc/reftype/Val.md)|||
|minmax|bool|[Val](/txt/exe/amc/reftype/Val.md)|||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/gen/amc/amc.md) as [amc.FCcmp](/txt/gen/amc/amc.md#amc-fccmp)

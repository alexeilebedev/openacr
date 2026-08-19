## dmmeta.cdflt - Specify default value for single-value types that lack fields


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Cdflt

* file:[data/dmmeta/cdflt.ssim](/data/dmmeta/cdflt.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ctype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)|||
|dflt|[algo.CppExpr](/txt/protocol/algo/CppExpr.md)|[Val](/txt/exe/amc/reftype.md#val)|||
|cppdflt|[algo.CppExpr](/txt/protocol/algo/CppExpr.md)|[Val](/txt/exe/amc/reftype.md#val)|||
|ssimdflt|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype.md#val)|||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype.md#val)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [acr](/txt/gen/acr/acr.md) as [acr.FCdflt](/txt/gen/acr/acr.md#acr-fcdflt)
* [amc](/txt/gen/amc/amc.md) as [amc.FCdflt](/txt/gen/amc/amc.md#amc-fcdflt)
* [lib_ctype](/txt/gen/lib_ctype/lib_ctype.md) as [lib_ctype.FCdflt](/txt/gen/lib_ctype/lib_ctype.md#lib_ctype-fcdflt)

## dmmeta.tary - Indirect linear dynamically alocated array (used for strings, arrays of data, etc)
<a href="#dmmeta-tary"></a>

This record is required on fields with [reftype:Tary](/txt/exe/amc/reftype.md#tary)

### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Tary

* file:[data/dmmeta/tary.ssim](/data/dmmeta/tary.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)|||
|aliased|bool|[Val](/txt/exe/amc/reftype.md#val)||Geneate functions to copy from aryptr|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype.md#val)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/gen/amc/amc.md) as [amc.FTary](/txt/gen/amc/amc.md#amc-ftary)

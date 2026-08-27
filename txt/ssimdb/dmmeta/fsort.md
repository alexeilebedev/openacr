## dmmeta.fsort - Generate custom sort function for array field


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Fsort

* file:[data/dmmeta/fsort.ssim](/data/dmmeta/fsort.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftype/Pkey.md)||Field (array etc) being sorted|
|sorttype|[dmmeta.Sorttype](/txt/ssimdb/dmmeta/sorttype.md)|[Pkey](/txt/exe/amc/reftype/Pkey.md)||Kind of sort to generate|
|sortfld|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftype/Pkey.md)||Field to sort on|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/gen/amc/amc.md) as [amc.FFsort](/txt/gen/amc/amc.md#amc-ffsort)

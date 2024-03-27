## dmmeta.fsort - Generate custom sort function for array field


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Fsort

* file:[data/dmmeta/fsort.ssim](/data/dmmeta/fsort.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|Pkey||Field (array etc) being sorted|
|sorttype|[dmmeta.Sorttype](/txt/ssimdb/dmmeta/sorttype.md)|Pkey||Kind of sort to generate|
|sortfld|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|Pkey||Field to sort on|
|comment|algo.Comment|Val|

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/exe/amc/README.md) as amc.FFsort


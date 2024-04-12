## dmmeta.ssimsort - Define sort order for ssimfile
<a href="#dmmeta-ssimsort"></a>

This record specifies which field the ssimfile is sorted on.
It is optional (i.e. not all ssimfiles are sorted).
A ssimfile is considered fully sorted if the sort field is the primary key.
Otherwise, it's partially sorted. For instance, the `dmmeta.field`
table is only sorted on the `ctype` attribute, since the order of fields
in a structure is not lexicographical.

### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Ssimsort

* file:[data/dmmeta/ssimsort.ssim](/data/dmmeta/ssimsort.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ssimfile|[dmmeta.Ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|Pkey|
|sortfld|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|Pkey|
|comment|algo.Comment|Val|

### Used In Executables
<a href="#used-in-executables"></a>
* [acr](/txt/exe/acr/README.md) as acr.FSsimsort
* [amc](/txt/exe/amc/README.md) as amc.FSsimsort


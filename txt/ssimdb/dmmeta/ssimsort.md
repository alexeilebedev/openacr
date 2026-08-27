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

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ssimfile|[dmmeta.Ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|[Pkey](/txt/exe/amc/reftype/Pkey.md)|||
|sortfld|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftype/Pkey.md)|||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [acr](/txt/gen/acr/acr.md) as [acr.FSsimsort](/txt/gen/acr/acr.md#acr-fssimsort)
* [amc](/txt/gen/amc/amc.md) as [amc.FSsimsort](/txt/gen/amc/amc.md#amc-fssimsort)
* [apm](/txt/gen/apm/apm.md) as [apm.FSsimsort](/txt/gen/apm/apm.md#apm-fssimsort)

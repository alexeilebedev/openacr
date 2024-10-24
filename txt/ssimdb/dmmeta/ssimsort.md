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
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Ssimsort

* file:[data/dmmeta/ssimsort.ssim](/data/dmmeta/ssimsort.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ssimfile|[dmmeta.Ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|||
|sortfld|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [acr](/txt/exe/acr/internals.md) as [acr.FSsimsort](/txt/exe/acr/internals.md#acr-fssimsort)
* [amc](/txt/exe/amc/internals.md) as [amc.FSsimsort](/txt/exe/amc/internals.md#amc-fssimsort)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->


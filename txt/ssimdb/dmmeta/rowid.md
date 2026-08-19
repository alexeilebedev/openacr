## dmmeta.rowid - Initialize field to row id of element
<a href="#dmmeta-rowid"></a>

Use rowid to automatically initialize the field's value to its index in the enclosing array.
This works if the field pool type is `Lary` or `Inlary`..

### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Rowid

* file:[data/dmmeta/rowid.ssim](/data/dmmeta/rowid.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)|||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype.md#val)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/gen/amc/amc.md) as [amc.FRowid](/txt/gen/amc/amc.md#amc-frowid)

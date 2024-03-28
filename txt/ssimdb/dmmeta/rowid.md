## dmmeta.rowid - Initialize field to row id of element
<a href="#dmmeta-rowid"></a>

Use rowid to automatically initialize the field's value to its index in the enclosing array.
This works if the field pool type is `Lary` or `Inlary`..

### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Rowid

* file:[data/dmmeta/rowid.ssim](/data/dmmeta/rowid.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|Pkey|
|comment|algo.Comment|Val|

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/exe/amc/README.md) as amc.FRowid


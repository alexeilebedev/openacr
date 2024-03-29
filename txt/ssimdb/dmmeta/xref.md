## dmmeta.xref - Specify how to cross-reference (i.e. project, or group-by) one record with another
<a href="#dmmeta-xref"></a>

See [xref](/txt/exe/amc/xref.md)

### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Xref

* file:[data/dmmeta/xref.ssim](/data/dmmeta/xref.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|Pkey|
|name|algo.Smallstr50|Val||<br>.RR of field|
|inscond|algo.CppExpr|Val|"true"|Insert condition|
|ns|[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)|Pkey||Ns with which this xref is a member<br>.LL of field|
|via|algo.Smallstr200|Val|
|viafld|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|Pkey||<br>/LL of via|
|keyfld|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|Pkey||<br>/RR of via|

### Subsets
<a href="#subsets"></a>
These ssimfiles are subsets of dmmeta.xref

* [dmmeta.Nocascdel](/txt/ssimdb/dmmeta/nocascdel.md) - Remove cascade delete for this index

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/exe/amc/README.md) as amc.FXref


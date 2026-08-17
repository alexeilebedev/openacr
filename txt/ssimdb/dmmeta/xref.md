## dmmeta.xref - Specify how to cross-reference (i.e. project, or group-by) one record with another
<a href="#dmmeta-xref"></a>

See [xref](/txt/exe/amc/xref.md)

### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Xref

* file:[data/dmmeta/xref.ssim](/data/dmmeta/xref.ssim)

italicised fields: *name, ns, viafld, keyfld* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)|||
|*name*|*[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)*|*[Val](/txt/exe/amc/reftype.md#val)*||*<br>.RR of field*|
|inscond|[algo.CppExpr](/txt/protocol/algo/CppExpr.md)|[Val](/txt/exe/amc/reftype.md#val)|"true"|Insert condition|
|*ns*|*[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*Ns with which this xref is a member<br>.LL of field*|
|via|[algo.Smallstr200](/txt/protocol/algo/README.md#algo-smallstr200)|[Val](/txt/exe/amc/reftype.md#val)|||
|*viafld*|*[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*<br>/LL of via*|
|*keyfld*|*[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*<br>/RR of via*|

### Subsets
<a href="#subsets"></a>
These ssimfiles are subsets of dmmeta.xref

* [dmmeta.Nocascdel](/txt/ssimdb/dmmeta/nocascdel.md) - Remove cascade delete for this index

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/gen/amc/amc.md) as [amc.FXref](/txt/gen/amc/amc.md#amc-fxref)

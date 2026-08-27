## dmmeta.fcond - List membership tied to a condition on a field


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Fcond

* file:[data/dmmeta/fcond.ssim](/data/dmmeta/fcond.ssim)

italicised fields: *field, value* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|fcond|[algo.Smallstr150](/txt/protocol/algo/README.md#algo-smallstr150)|[Val](/txt/exe/amc/reftype/Val.md)||<field>/<condition>|
|*field*|*[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)*|*[Pkey](/txt/exe/amc/reftype/Pkey.md)*||*The field whose condition is watched<br>/LL of fcond*|
|*value*|*[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)*|*[Val](/txt/exe/amc/reftype/Val.md)*||*The condition: a value of a Val field, or an amcdb.tcond name of any other reftype<br>/LR of fcond*|
|ins|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftype/Pkey.md)||Index the record (or via target) enters when the condition begins to hold|
|via|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftype/Pkey.md)|""|Optional Upptr field on the watched ctype; the pointed-to record is inserted instead|
|rem|bool|[Val](/txt/exe/amc/reftype/Val.md)|true|Remove from the index when the condition stops holding|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/gen/amc/amc.md) as [amc.FFcond](/txt/gen/amc/amc.md#amc-ffcond)

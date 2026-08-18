## dmmeta.reftype - Field type constructor (e.g. reference type)
<a href="#dmmeta-reftype"></a>

This table lists all available reftypes ("type constructors").
For more information, see [Reftypes](/txt/exe/amc/reftype.md).

### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Reftype

* file:[data/dmmeta/reftype.ssim](/data/dmmeta/reftype.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|reftype|[amcdb.Tclass](/txt/ssimdb/amcdb/tclass.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)|"Val"||
|isval|bool|[Val](/txt/exe/amc/reftype.md#val)||True if field makes values of target type|
|cascins|bool|[Val](/txt/exe/amc/reftype.md#val)||Field is cascade-insert|
|usebasepool|bool|[Val](/txt/exe/amc/reftype.md#val)||Fields with this type make use of dmmeta.basepool|
|cancopy|bool|[Val](/txt/exe/amc/reftype.md#val)||This type of field can be copied|
|needxref|bool|[Val](/txt/exe/amc/reftype.md#val)||Creating a field of this reftype implies an xref (acr_ed)|
|del|bool|[Val](/txt/exe/amc/reftype.md#val)||Supports random deletion?|
|up|bool|[Val](/txt/exe/amc/reftype.md#val)||This type of field is a reference|
|isnew|bool|[Val](/txt/exe/amc/reftype.md#val)||If set, skip this relation in amc_vis|
|hasalloc|bool|[Val](/txt/exe/amc/reftype.md#val)||Generte Alloc/Delete functions for arg type|
|inst|bool|[Val](/txt/exe/amc/reftype.md#val)||Field creates an instance of arg type (directly or indirectly)|
|varlen|bool|[Val](/txt/exe/amc/reftype.md#val)||This pool supports varlen allocations|

### Related
<a href="#related"></a>
These ssimfiles reference dmmeta.reftype

* [dmmeta.field via reftype](/txt/ssimdb/dmmeta/field.md) - Type constructor 
* [dmmeta.fprefix via reftype](/txt/ssimdb/dmmeta/fprefix.md) - Allowed pairing of field-name prefix and reftype

### Used In Commands
<a href="#used-in-commands"></a>
* [acr_ed -reftype](/txt/exe/acr_ed/README.md) - Reftype (e.g. Val, Thash, Llist, etc)

### Used In Executables
<a href="#used-in-executables"></a>
* [abt_md](/txt/gen/abt_md/abt_md.md) as [abt_md.FReftype](/txt/gen/abt_md/abt_md.md#abt_md-freftype)
* [acr_ed](/txt/gen/acr_ed/acr_ed.md) as [acr_ed.FReftype](/txt/gen/acr_ed/acr_ed.md#acr_ed-freftype)
* [amc](/txt/gen/amc/amc.md) as [amc.FReftype](/txt/gen/amc/amc.md#amc-freftype)
* [amc_vis](/txt/gen/amc_vis/amc_vis.md) as [amc_vis.FReftype](/txt/gen/amc_vis/amc_vis.md#amc_vis-freftype)

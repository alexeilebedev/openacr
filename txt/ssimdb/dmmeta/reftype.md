## dmmeta.reftype - Field type constructor (e.g. reference type)
<a href="#dmmeta-reftype"></a>

This table lists all available reftypes ("type constructors").
For more information, see [Reftypes](/txt/exe/amc/reftypes.md).

### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Reftype

* file:[data/dmmeta/reftype.ssim](/data/dmmeta/reftype.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|reftype|[amcdb.Tclass](/txt/ssimdb/amcdb/tclass.md)|Pkey|"Val"|
|isval|bool|Val||True if field makes values of target type|
|cascins|bool|Val||Field is cascade-insert|
|usebasepool|bool|Val||Fields with this type make use of dmmeta.basepool|
|cancopy|bool|Val||This type of field can be copied|
|isxref|bool|Val||This type of field is an x-ref|
|del|bool|Val||Supports random deletion?|
|up|bool|Val||This type of field is a reference|
|isnew|bool|Val||If set, skip this relation in amc_vis|
|hasalloc|bool|Val||Generte Alloc/Delete functions for arg type|
|inst|bool|Val||Field creates an instance of arg type (directly or indirectly)|
|varlen|bool|Val||This pool supports varlen allocations|

### Related
<a href="#related"></a>
These ssimfiles reference dmmeta.reftype

* [dmmeta.field via reftype](/txt/ssimdb/dmmeta/field.md) - Type constructor 
* [dmmeta.fprefix via reftype](/txt/ssimdb/dmmeta/fprefix.md) - Mapping between field prefix and Reftype

### Used In Commands
<a href="#used-in-commands"></a>
* [acr_ed -pooltype](/txt/exe/acr_ed/README.md) - Pool reftype (Lary,Lpool etc) for finput/ctype 
* [acr_ed -reftype](/txt/exe/acr_ed/README.md) - Reftype (e.g. Val, Thash, Llist, etc)

### Used In Executables
<a href="#used-in-executables"></a>
* [abt_md](/txt/exe/abt_md/README.md) as abt_md.FReftype
* [amc](/txt/exe/amc/README.md) as amc.FReftype
* [amc_vis](/txt/exe/amc_vis/README.md) as amc_vis.FReftype


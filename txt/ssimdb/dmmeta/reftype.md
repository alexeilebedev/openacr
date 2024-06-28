## dmmeta.reftype - Field type constructor (e.g. reference type)
<a href="#dmmeta-reftype"></a>

This table lists all available reftypes ("type constructors").
For more information, see [Reftypes](/txt/exe/amc/reftypes.md).

### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Reftype

* file:[data/dmmeta/reftype.ssim](/data/dmmeta/reftype.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|reftype|[amcdb.Tclass](/txt/ssimdb/amcdb/tclass.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|"Val"||
|isval|bool|[Val](/txt/exe/amc/reftypes.md#val)||True if field makes values of target type|
|cascins|bool|[Val](/txt/exe/amc/reftypes.md#val)||Field is cascade-insert|
|usebasepool|bool|[Val](/txt/exe/amc/reftypes.md#val)||Fields with this type make use of dmmeta.basepool|
|cancopy|bool|[Val](/txt/exe/amc/reftypes.md#val)||This type of field can be copied|
|isxref|bool|[Val](/txt/exe/amc/reftypes.md#val)||This type of field is an x-ref|
|del|bool|[Val](/txt/exe/amc/reftypes.md#val)||Supports random deletion?|
|up|bool|[Val](/txt/exe/amc/reftypes.md#val)||This type of field is a reference|
|isnew|bool|[Val](/txt/exe/amc/reftypes.md#val)||If set, skip this relation in amc_vis|
|hasalloc|bool|[Val](/txt/exe/amc/reftypes.md#val)||Generte Alloc/Delete functions for arg type|
|inst|bool|[Val](/txt/exe/amc/reftypes.md#val)||Field creates an instance of arg type (directly or indirectly)|
|varlen|bool|[Val](/txt/exe/amc/reftypes.md#val)||This pool supports varlen allocations|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Related
<a href="#related"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Related -->
These ssimfiles reference dmmeta.reftype

* [dmmeta.field via reftype](/txt/ssimdb/dmmeta/field.md) - Type constructor 
* [dmmeta.fprefix via reftype](/txt/ssimdb/dmmeta/fprefix.md) - Mapping between field prefix and Reftype 

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Related -->

### Used In Commands
<a href="#used-in-commands"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:CmdlineUses -->

* [acr_ed -pooltype](/txt/exe/acr_ed/README.md) - Pool reftype (Lary,Lpool etc) for finput/ctype 
* [acr_ed -reftype](/txt/exe/acr_ed/README.md) - Reftype (e.g. Val, Thash, Llist, etc) 

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:CmdlineUses -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [abt_md](/txt/exe/abt_md/internals.md) as [abt_md.FReftype](/txt/exe/abt_md/internals.md#abt_md-freftype)
* [amc](/txt/exe/amc/internals.md) as [amc.FReftype](/txt/exe/amc/internals.md#amc-freftype)
* [amc_vis](/txt/exe/amc_vis/internals.md) as [amc_vis.FReftype](/txt/exe/amc_vis/internals.md#amc_vis-freftype)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->


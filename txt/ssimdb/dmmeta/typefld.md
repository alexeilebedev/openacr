## dmmeta.typefld - Specifies which field of a message carries the type
<a href="#dmmeta-typefld"></a>

This record indicates that a field acts as a type field for a message.
It is widely used when describing [protocols](/txt/exe/amc/proto.md)

### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Typefld

* file:[data/dmmeta/typefld.ssim](/data/dmmeta/typefld.ssim)

italicised fields: *ctype* are [**fldfunc**](/txt/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|||
|*ctype*|*[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*Name of length field<br>.RL of field*|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [acr_ed](/txt/exe/acr_ed/internals.md) as [acr_ed.FTypefld](/txt/exe/acr_ed/internals.md#acr_ed-ftypefld)
* [amc](/txt/exe/amc/internals.md) as [amc.FTypefld](/txt/exe/amc/internals.md#amc-ftypefld)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->


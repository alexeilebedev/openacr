## dmmeta.typefld - Specifies which field of a message carries the type
<a href="#dmmeta-typefld"></a>

This record indicates that a field acts as a type field for a message.
It is widely used when describing [protocols](/txt/exe/amc/proto.md)

### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Typefld

* file:[data/dmmeta/typefld.ssim](/data/dmmeta/typefld.ssim)

italicised fields: *ctype* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftype/Pkey.md)|||
|*ctype*|*[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)*|*[Pkey](/txt/exe/amc/reftype/Pkey.md)*||*Name of length field<br>.RL of field*|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [acr_ed](/txt/gen/acr_ed/acr_ed.md) as [acr_ed.FTypefld](/txt/gen/acr_ed/acr_ed.md#acr_ed-ftypefld)
* [amc](/txt/gen/amc/amc.md) as [amc.FTypefld](/txt/gen/amc/amc.md#amc-ftypefld)

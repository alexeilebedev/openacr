## dmmeta.typefld - Specifies which field of a message carries the type
<a href="#dmmeta-typefld"></a>

This record indicates that a field acts as a type field for a message.
It is widely used when describing [protocols](/txt/exe/amc/proto.md)

### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Typefld

* file:[data/dmmeta/typefld.ssim](/data/dmmeta/typefld.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|Pkey|
|ctype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|Pkey||Name of length field<br>.RL of field|
|comment|algo.Comment|Val|

### Used In Executables
<a href="#used-in-executables"></a>
* [acr_ed](/txt/exe/acr_ed/README.md) as acr_ed.FTypefld
* [amc](/txt/exe/amc/README.md) as amc.FTypefld


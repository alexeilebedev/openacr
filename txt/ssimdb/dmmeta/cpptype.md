## dmmeta.cpptype - Specify whether a ctype can be passed by value, and other c++ options


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Cpptype

* file:[data/dmmeta/cpptype.ssim](/data/dmmeta/cpptype.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ctype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|||
|ctor|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|if true, generate non-default constructor from all fields|
|dtor|bool|[Val](/txt/exe/amc/reftypes.md#val)|true|generate non-default destructor|
|cheap_copy|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Pass by value whenever possible|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [acr_ed](/txt/exe/acr_ed/internals.md) as [acr_ed.FCpptype](/txt/exe/acr_ed/internals.md#acr_ed-fcpptype)
* [amc](/txt/exe/amc/internals.md) as [amc.FCpptype](/txt/exe/amc/internals.md#amc-fcpptype)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->


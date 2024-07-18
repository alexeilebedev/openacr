## dmmeta.numstr - Add functions to read numbers out of a string field


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Numstr

* file:[data/dmmeta/numstr.ssim](/data/dmmeta/numstr.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|field|[dmmeta.Smallstr](/txt/ssimdb/dmmeta/smallstr.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)||String type|
|numtype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)||Corresponding numeric type|
|base|i32|[Val](/txt/exe/amc/reftypes.md#val)||Base for conversion|
|min_len|i32|[Val](/txt/exe/amc/reftypes.md#val)||Mininum length of string|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [amc](/txt/exe/amc/internals.md) as [amc.FNumstr](/txt/exe/amc/internals.md#amc-fnumstr)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->


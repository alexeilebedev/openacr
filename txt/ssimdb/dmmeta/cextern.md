## dmmeta.cextern - Externally defined ctype (a struct from system header, or from a c++ library)


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Cextern

* file:[data/dmmeta/cextern.ssim](/data/dmmeta/cextern.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ctype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)||Ctype in question|
|initmemset|bool|[Val](/txt/exe/amc/reftypes.md#val)||Initialize using memset? (set this to Y for all C structs)|
|isstruct|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|plaindata|bool|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [amc](/txt/exe/amc/internals.md) as [amc.FCextern](/txt/exe/amc/internals.md#amc-fcextern)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->


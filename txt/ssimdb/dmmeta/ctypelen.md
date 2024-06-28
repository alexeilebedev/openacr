## dmmeta.ctypelen - Size of Ctype


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Ctypelen

* file:[data/dmmeta/ctypelen.ssim](/data/dmmeta/ctypelen.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ctype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)||Identifies the Ctype|
|len|u32|[Val](/txt/exe/amc/reftypes.md#val)||(calculated) length of the C++ struct in bytes|
|alignment|i32|[Val](/txt/exe/amc/reftypes.md#val)||(calculated) alignment for the struct|
|padbytes|i32|[Val](/txt/exe/amc/reftypes.md#val)||(calculated) total # of pad bytes|
|plaindata|bool|[Val](/txt/exe/amc/reftypes.md#val)||(calculated) this struct can me safely memcpy'ed|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [amc](/txt/exe/amc/internals.md) as [amc.FCtypelen](/txt/exe/amc/internals.md#amc-fctypelen)
* [src_func](/txt/exe/src_func/internals.md) as [src_func.FCtypelen](/txt/exe/src_func/internals.md#src_func-fctypelen)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->


## dmmeta.ctypelen - Size of Ctype


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Ctypelen

* file:[data/dmmeta/ctypelen.ssim](/data/dmmeta/ctypelen.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ctype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Pkey](/txt/exe/amc/reftype/Pkey.md)||Identifies the Ctype|
|len|u32|[Val](/txt/exe/amc/reftype/Val.md)||(calculated) length of the C++ struct in bytes|
|alignment|i32|[Val](/txt/exe/amc/reftype/Val.md)||(calculated) alignment for the struct|
|padbytes|i32|[Val](/txt/exe/amc/reftype/Val.md)||(calculated) total # of pad bytes|
|plaindata|bool|[Val](/txt/exe/amc/reftype/Val.md)||(calculated) can safely call memcpy on this struct|

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/gen/amc/amc.md) as [amc.FCtypelen](/txt/gen/amc/amc.md#amc-fctypelen)
* [src_func](/txt/gen/src_func/src_func.md) as [src_func.FCtypelen](/txt/gen/src_func/src_func.md#src_func-fctypelen)

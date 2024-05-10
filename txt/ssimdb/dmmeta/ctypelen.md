## dmmeta.ctypelen - Size of Ctype


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Ctypelen

* file:[data/dmmeta/ctypelen.ssim](/data/dmmeta/ctypelen.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ctype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|Pkey||Identifies the Ctype|
|len|u32|Val||(calculated) length of the C++ struct in bytes|
|alignment|i32|Val||(calculated) alignment for the struct|
|padbytes|i32|Val||(calculated) total # of pad bytes|
|plaindata|bool|Val||(calculated) this struct can me safely memcpy'ed|

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/exe/amc/README.md) as amc.FCtypelen
* [src_func](/txt/exe/src_func/README.md) as src_func.FCtypelen


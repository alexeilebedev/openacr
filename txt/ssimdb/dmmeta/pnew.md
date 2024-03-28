## dmmeta.pnew - Generate custom constructor (placement new), for use with binary protocols


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Pnew

* file:[data/dmmeta/pnew.ssim](/data/dmmeta/pnew.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|pnew|algo.Smallstr100|Val|
|ns|[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)|Pkey||<br>/LL of pnew|
|ctype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|Pkey||<br>/LR.RL of pnew|
|buftype|[dmmeta.Buftype](/txt/ssimdb/dmmeta/pnewtype.md)|Pkey||<br>/LR.RR of pnew|
|comment|algo.Comment|Val|

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/exe/amc/README.md) as amc.FPnew


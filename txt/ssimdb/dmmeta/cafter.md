## dmmeta.cafter - Control amc processing order for unrelated types (used in rare situations)


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Cafter

* file:[data/dmmeta/cafter.ssim](/data/dmmeta/cafter.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|cafter|algo.Smallstr50|Val|
|ctype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|Pkey||<br>/RL of cafter|
|after|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|Pkey||<br>/RR of cafter|
|comment|algo.Comment|Val|

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/exe/amc/README.md) as amc.FCafter


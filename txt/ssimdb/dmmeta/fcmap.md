## dmmeta.fcmap - Bidirectional mapping between fconst values


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Fcmap

* file:[data/dmmeta/fcmap.ssim](/data/dmmeta/fcmap.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|fcmap|algo.Smallstr250|Val||cstring maybe?|
|leftField|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|Pkey||<br>=LL/LL of fcmap|
|leftVal|algo.Smallstr40|Val||<br>=LL/LR of fcmap|
|rightVal|algo.Smallstr40|Val||<br>=LR/LR of fcmap|
|rightField|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|Pkey||<br>=LR/LL of fcmap|
|bidir|bool|Val|true|
|comment|algo.Comment|Val|

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/exe/amc/README.md) as amc.FFcmap


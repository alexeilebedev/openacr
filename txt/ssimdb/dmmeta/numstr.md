## dmmeta.numstr - Add functions to read numbers out of a string field


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Numstr

* file:[data/dmmeta/numstr.ssim](/data/dmmeta/numstr.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|field|[dmmeta.Smallstr](/txt/ssimdb/dmmeta/smallstr.md)|Pkey||String type|
|numtype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|Pkey||Corresponding numeric type|
|base|i32|Val||Base for conversion|
|min_len|i32|Val||Mininum length of string|

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/exe/amc/README.md) as amc.FNumstr


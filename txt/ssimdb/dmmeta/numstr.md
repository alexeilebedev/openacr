## dmmeta.numstr - Add functions to read numbers out of a string field


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Numstr

* file:[data/dmmeta/numstr.ssim](/data/dmmeta/numstr.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|field|[dmmeta.Smallstr](/txt/ssimdb/dmmeta/smallstr.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)||String type|
|numtype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)||Corresponding numeric type|
|base|i32|[Val](/txt/exe/amc/reftype.md#val)||Base for conversion|
|min_len|i32|[Val](/txt/exe/amc/reftype.md#val)||Mininum length of string|

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/gen/amc/amc.md) as [amc.FNumstr](/txt/gen/amc/amc.md#amc-fnumstr)

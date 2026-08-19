## dmmeta.cextern - Externally defined ctype (a struct from system header, or from a c++ library)


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Cextern

* file:[data/dmmeta/cextern.ssim](/data/dmmeta/cextern.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ctype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)||Ctype in question|
|initmemset|bool|[Val](/txt/exe/amc/reftype.md#val)||Initialize using memset? (set this to Y for all C structs)|
|isstruct|bool|[Val](/txt/exe/amc/reftype.md#val)|||
|plaindata|bool|[Val](/txt/exe/amc/reftype.md#val)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/gen/amc/amc.md) as [amc.FCextern](/txt/gen/amc/amc.md#amc-fcextern)

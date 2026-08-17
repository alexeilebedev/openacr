## gclidb.gtypeh - HTTP heaaderes for user with platform


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)gclidb.Gtypeh

* file:[data/gclidb/gtypeh.ssim](/data/gclidb/gtypeh.ssim)

italicised fields: *gtype, name* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gtypeh|[algo.Smallstr250](/txt/protocol/algo/README.md#algo-smallstr250)|[Val](/txt/exe/amc/reftype.md#val)|||
|*gtype*|*[gclidb.Gtype](/txt/ssimdb/gclidb/gtype.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*<br>.RL of gtypeh*|
|*name*|*[algo.cstring](/txt/protocol/algo/cstring.md)*|*[Val](/txt/exe/amc/reftype.md#val)*||*<br>.RR of gtypeh*|
|header|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [gcli](/txt/gen/gcli/gcli.md) as [gcli.FGtypeh](/txt/gen/gcli/gcli.md#gcli-fgtypeh)

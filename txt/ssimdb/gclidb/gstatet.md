## gclidb.gstatet - Internal


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)gclidb.Gstatet

* file:[data/gclidb/gstatet.ssim](/data/gclidb/gstatet.ssim)

italicised fields: *gtype, gstate* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gstatet|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype.md#val)|||
|*gtype*|*[gclidb.Gtype](/txt/ssimdb/gclidb/gtype.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*<br>.RR of gstatet*|
|*gstate*|*[gclidb.Gstate](/txt/ssimdb/gclidb/gstate.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*<br>.RL of gstatet*|
|state|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [gcli](/txt/gen/gcli/gcli.md) as [gcli.FGstatet](/txt/gen/gcli/gcli.md#gcli-fgstatet)

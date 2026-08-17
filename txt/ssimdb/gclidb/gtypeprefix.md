## gclidb.gtypeprefix - Token prefix mapping to platform


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)gclidb.Gtypeprefix

* file:[data/gclidb/gtypeprefix.ssim](/data/gclidb/gtypeprefix.ssim)

italicised fields: *gtype, prefix* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gtypeprefix|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype.md#val)|||
|*gtype*|*[gclidb.Gtype](/txt/ssimdb/gclidb/gtype.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*<br>.RL of gtypeprefix*|
|*prefix*|*[algo.cstring](/txt/protocol/algo/cstring.md)*|*[Val](/txt/exe/amc/reftype.md#val)*||*<br>.RR of gtypeprefix*|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype.md#val)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [gcli](/txt/gen/gcli/gcli.md) as [gcli.FGtypeprefix](/txt/gen/gcli/gcli.md#gcli-fgtypeprefix)

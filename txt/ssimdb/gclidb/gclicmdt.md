## gclidb.gclicmdt - Internal test


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)gclidb.Gclicmdt

* file:[data/gclidb/gclicmdt.ssim](/data/gclidb/gclicmdt.ssim)

italicised fields: *gclicmd, gtype* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gclicmdt|[algo.Smallstr250](/txt/protocol/algo/README.md#algo-smallstr250)|[Val](/txt/exe/amc/reftype.md#val)|||
|gmethod|[gclidb.Gmethod](/txt/ssimdb/gclidb/gmethod.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)|"GET"||
|*gclicmd*|*[gclidb.Gclicmd](/txt/ssimdb/gclidb/gclicmd.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*<br>.RL of gclicmdt*|
|*gtype*|*[gclidb.Gtype](/txt/ssimdb/gclidb/gtype.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*<br>.RR of gclicmdt*|
|uri|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype.md#val)|||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype.md#val)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [gcli](/txt/gen/gcli/gcli.md) as [gcli.FGclicmdt](/txt/gen/gcli/gcli.md#gcli-fgclicmdt)

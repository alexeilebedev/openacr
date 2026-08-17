## gclidb.mrnote - Issue note


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)gclidb.Mrnote

* file:[data/gclidb/mrnote.ssim](/data/gclidb/mrnote.ssim)

italicised fields: *nid, mr* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|system|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||System or user note (glab only)|
|mrnote|[algo.Smallstr250](/txt/protocol/algo/README.md#algo-smallstr250)|[Val](/txt/exe/amc/reftype.md#val)||Identifier. must be in form issue.id|
|*nid*|*[algo.cstring](/txt/protocol/algo/cstring.md)*|*[Val](/txt/exe/amc/reftype.md#val)*||*Id portion of mr note<br>.RR of mrnote*|
|note|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||Body of mr note|
|author|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||Authtor of mr note|
|*mr*|*[gclidb.Mr](/txt/ssimdb/gclidb/mr.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*Mr portion of mr note<br>.RL of mrnote*|
|resolvable|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|""|Note is part of a resolvable review thread|
|resolved|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|""|Resolvable note has been resolved|

### Used In Executables
<a href="#used-in-executables"></a>
* [gcli](/txt/gen/gcli/gcli.md) as [gcli.FMrnote](/txt/gen/gcli/gcli.md#gcli-fmrnote)

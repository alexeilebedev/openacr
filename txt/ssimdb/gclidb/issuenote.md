## gclidb.issuenote - Issue note


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)gclidb.Issuenote

* file:[data/gclidb/issuenote.ssim](/data/gclidb/issuenote.ssim)

italicised fields: *issue, nid* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|issuenote|[algo.Smallstr250](/txt/protocol/algo/README.md#algo-smallstr250)|[Val](/txt/exe/amc/reftype.md#val)||Identifier. must be in form issue.id|
|*issue*|*[gclidb.Issue](/txt/ssimdb/gclidb/issue.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*Issue portion of issue note<br>.RL of issuenote*|
|*nid*|*[algo.cstring](/txt/protocol/algo/cstring.md)*|*[Val](/txt/exe/amc/reftype.md#val)*||*Id portion of issue note<br>.RR of issuenote*|
|note|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||Body of issue note|
|system|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||System or user note (glab only)|
|author|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||Authtor of issue note|

### Used In Executables
<a href="#used-in-executables"></a>
* [gcli](/txt/gen/gcli/gcli.md) as [gcli.FIssuenote](/txt/gen/gcli/gcli.md#gcli-fissuenote)

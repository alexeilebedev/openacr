## gclidb.issuepipeline - Issue pipeline


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)gclidb.Issuepipeline

* file:[data/gclidb/issuepipeline.ssim](/data/gclidb/issuepipeline.ssim)

italicised fields: *issue, id* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|issuepipeline|[algo.Smallstr250](/txt/protocol/algo/README.md#algo-smallstr250)|[Val](/txt/exe/amc/reftype.md#val)||Identifier. must be in form issue.id|
|*issue*|*[gclidb.Issue](/txt/ssimdb/gclidb/issue.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*Issue portion of issue pipeline<br>.RL of issuepipeline*|
|*id*|*[algo.cstring](/txt/protocol/algo/cstring.md)*|*[Val](/txt/exe/amc/reftype.md#val)*||*Id portion of issue pipeline<br>.RR of issuepipeline*|
|id_in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||Id portion of issue pipeline|
|status|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||Id portion of issue pipeline|

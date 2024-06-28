## gclidb.issuenote - Issue note


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)gclidb.Issuenote

* file:[data/gclidb/issuenote.ssim](/data/gclidb/issuenote.ssim)

italicised fields: *issue, nid* are [**fldfunc**](/txt/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|issuenote|[algo.Smallstr250](/txt/protocol/algo/README.md#algo-smallstr250)|[Val](/txt/exe/amc/reftypes.md#val)||Identifier. must be in form issue.id|
|*issue*|*[gclidb.Issue](/txt/ssimdb/gclidb/issue.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*Issue portion of issue note<br>.RL of issuenote*|
|*nid*|*[algo.cstring](/txt/protocol/algo/cstring.md)*|*[Val](/txt/exe/amc/reftypes.md#val)*||*Id portion of issue note<br>.RR of issuenote*|
|note|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Body of issue note|
|system|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||System or user note (glab only)|
|author|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Authtor of issue note|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [gcli](/txt/exe/gcli/internals.md) as [gcli.FIssuenote](/txt/exe/gcli/internals.md#gcli-fissuenote)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->


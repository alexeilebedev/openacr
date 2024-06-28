## gclidb.mrnote - Issue note


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)gclidb.Mrnote

* file:[data/gclidb/mrnote.ssim](/data/gclidb/mrnote.ssim)

italicised fields: *nid, mr* are [**fldfunc**](/txt/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|system|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||System or user note (glab only)|
|mrnote|[algo.Smallstr250](/txt/protocol/algo/README.md#algo-smallstr250)|[Val](/txt/exe/amc/reftypes.md#val)||Identifier. must be in form issue.id|
|*nid*|*[algo.cstring](/txt/protocol/algo/cstring.md)*|*[Val](/txt/exe/amc/reftypes.md#val)*||*Id portion of mr note<br>.RR of mrnote*|
|note|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Body of mr note|
|author|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Authtor of mr note|
|*mr*|*[gclidb.Mr](/txt/ssimdb/gclidb/mr.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*Mr portion of mr note<br>.RL of mrnote*|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [gcli](/txt/exe/gcli/internals.md) as [gcli.FMrnote](/txt/exe/gcli/internals.md#gcli-fmrnote)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->


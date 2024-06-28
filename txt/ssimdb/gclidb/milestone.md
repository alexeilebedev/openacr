## gclidb.milestone - Gclidb project milestone


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)gclidb.Milestone

* file:[data/gclidb/milestone.ssim](/data/gclidb/milestone.ssim)

italicised fields: *project, iid* are [**fldfunc**](/txt/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|milestone|[algo.Smallstr200](/txt/protocol/algo/README.md#algo-smallstr200)|[Val](/txt/exe/amc/reftypes.md#val)||Milestone id|
|*project*|*[gclidb.Project](/txt/ssimdb/gclidb/project.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*Project portion of milestone<br>.RL of milestone*|
|*iid*|*[algo.cstring](/txt/protocol/algo/cstring.md)*|*[Val](/txt/exe/amc/reftypes.md#val)*||*Iid portion of milestone<br>.RR of milestone*|
|description|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Milestone description|
|title|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Milestone title|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [gcli](/txt/exe/gcli/internals.md) as [gcli.FMilestone](/txt/exe/gcli/internals.md#gcli-fmilestone)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->


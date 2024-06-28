## gclidb.mr - Gitlab project mr


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)gclidb.Mr

* file:[data/gclidb/mr.ssim](/data/gclidb/mr.ssim)

italicised fields: *proj, iid* are [**fldfunc**](/txt/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|mr|[algo.Smallstr150](/txt/protocol/algo/README.md#algo-smallstr150)|[Val](/txt/exe/amc/reftypes.md#val)||Identifier. must be in form project.iid|
|*proj*|*[gclidb.Project](/txt/ssimdb/gclidb/project.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*Project portion of mr<br>.RL of mr*|
|*iid*|*[algo.cstring](/txt/protocol/algo/cstring.md)*|*[Val](/txt/exe/amc/reftypes.md#val)*||*Iid portion of mr<br>.RR of mr*|
|state|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Mr state|
|title|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Mr title|
|description|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Mr description|
|source_branch|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Source branch|
|source_repo|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Source branch|
|reviewer|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Reviewer username|
|author|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Author username|
|pipeline_status|[algo.Smallstr20](/txt/protocol/algo/README.md#algo-smallstr20)|[Val](/txt/exe/amc/reftypes.md#val)||Pipeline status|
|pipeline_id|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Pipeline id|
|draft|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Some draft status|
|node_id|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Node id for graphQL|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Related
<a href="#related"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Related -->
These ssimfiles reference gclidb.mr

* [gclidb.mrjob via mr](/txt/ssimdb/gclidb/mrjob.md) - Mr portion of mr job 
* [gclidb.mrjob via ref](/txt/ssimdb/gclidb/mrjob.md) - Mr portion of mr job 
* [gclidb.mrnote via mr](/txt/ssimdb/gclidb/mrnote.md) - Mr portion of mr note 

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Related -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [gcli](/txt/exe/gcli/internals.md) as [gcli.FMr](/txt/exe/gcli/internals.md#gcli-fmr)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->


## gclidb.mr - Gitlab project mr


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)gclidb.Mr

* file:[data/gclidb/mr.ssim](/data/gclidb/mr.ssim)

italicised fields: *proj, iid* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|mr|[algo.Smallstr150](/txt/protocol/algo/README.md#algo-smallstr150)|[Val](/txt/exe/amc/reftype.md#val)||Identifier. must be in form project.iid|
|*proj*|*[gclidb.Project](/txt/ssimdb/gclidb/project.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*Project portion of mr<br>.RL of mr*|
|*iid*|*[algo.cstring](/txt/protocol/algo/cstring.md)*|*[Val](/txt/exe/amc/reftype.md#val)*||*Iid portion of mr<br>.RR of mr*|
|state|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||Mr state|
|title|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||Mr title|
|description|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||Mr description|
|source_branch|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||Source branch|
|source_repo|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||Source branch|
|reviewer|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||Reviewer username|
|author|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||Author username|
|pipeline_status|[algo.Smallstr20](/txt/protocol/algo/README.md#algo-smallstr20)|[Val](/txt/exe/amc/reftype.md#val)||Pipeline status|
|pipeline_id|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||Pipeline id|
|draft|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||Some draft status|
|node_id|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||Node id for graphQL|
|merge_status|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|""|Detailed merge status (single-mr fetch)|
|behind|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|""|Commits the source branch is behind the target|

### Related
<a href="#related"></a>
These ssimfiles reference gclidb.mr

* [gclidb.mrjob via mr](/txt/ssimdb/gclidb/mrjob.md) - Mr portion of mr job 
* [gclidb.mrjob via ref](/txt/ssimdb/gclidb/mrjob.md) - Mr portion of mr job 
* [gclidb.mrnote via mr](/txt/ssimdb/gclidb/mrnote.md) - Mr portion of mr note

### Used In Executables
<a href="#used-in-executables"></a>
* [gcli](/txt/gen/gcli/gcli.md) as [gcli.FMr](/txt/gen/gcli/gcli.md#gcli-fmr)

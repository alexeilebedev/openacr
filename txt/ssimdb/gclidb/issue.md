## gclidb.issue - Gitlab project issue


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)gclidb.Issue

* file:[data/gclidb/issue.ssim](/data/gclidb/issue.ssim)

italicised fields: *project, iid* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|issue|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype.md#val)||Identifier. must be in form project.iid|
|*project*|*[gclidb.Project](/txt/ssimdb/gclidb/project.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*Project portion of issue<br>.RL of issue*|
|*iid*|*[algo.cstring](/txt/protocol/algo/cstring.md)*|*[Val](/txt/exe/amc/reftype.md#val)*||*Iid portion of issue<br>.RR of issue*|
|assignee|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype.md#val)||User the issue is assigned to|
|labels|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||issue labels - comma-separated list|
|milestone_iid|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||issue milestone, blank if none|
|milestone_num|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||issue milestone, blank if none|
|milestone|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||issue milestone name, blank if none|
|title|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||issue title|
|author|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||issue author|
|state|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||issue state|
|mr|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||mr issue|
|description|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||issue description|

### Related
<a href="#related"></a>
These ssimfiles reference gclidb.issue

* [gclidb.issuenote via issue](/txt/ssimdb/gclidb/issuenote.md) - Issue portion of issue note 
* [gclidb.issuepipeline via issue](/txt/ssimdb/gclidb/issuepipeline.md) - Issue portion of issue pipeline

### Used In Executables
<a href="#used-in-executables"></a>
* [gcli](/txt/gen/gcli/gcli.md) as [gcli.FIssue](/txt/gen/gcli/gcli.md#gcli-fissue)

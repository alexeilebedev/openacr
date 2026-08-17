## gclidb.milestone - Gclidb project milestone


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)gclidb.Milestone

* file:[data/gclidb/milestone.ssim](/data/gclidb/milestone.ssim)

italicised fields: *project, iid* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|milestone|[algo.Smallstr200](/txt/protocol/algo/README.md#algo-smallstr200)|[Val](/txt/exe/amc/reftype.md#val)||Milestone id|
|*project*|*[gclidb.Project](/txt/ssimdb/gclidb/project.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*Project portion of milestone<br>.RL of milestone*|
|*iid*|*[algo.cstring](/txt/protocol/algo/cstring.md)*|*[Val](/txt/exe/amc/reftype.md#val)*||*Iid portion of milestone<br>.RR of milestone*|
|description|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||Milestone description|
|title|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||Milestone title|

### Used In Executables
<a href="#used-in-executables"></a>
* [gcli](/txt/gen/gcli/gcli.md) as [gcli.FMilestone](/txt/gen/gcli/gcli.md#gcli-fmilestone)

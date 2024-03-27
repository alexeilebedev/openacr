## gclidb.milestone - Gclidb project milestone


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)gclidb.Milestone

* file:[data/gclidb/milestone.ssim](/data/gclidb/milestone.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|milestone|algo.Smallstr200|Val||Milestone id|
|project|[gclidb.Project](/txt/ssimdb/gclidb/project.md)|Pkey||Project portion of milestone<br>.RL of milestone|
|iid|algo.cstring|Val||Iid portion of milestone<br>.RR of milestone|
|description|algo.cstring|Val||Milestone description|
|title|algo.cstring|Val||Milestone title|

### Used In Executables
<a href="#used-in-executables"></a>
* [gcli](/txt/exe/gcli/README.md) as gcli.FMilestone


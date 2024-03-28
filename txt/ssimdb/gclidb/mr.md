## gclidb.mr - Gitlab project mr


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)gclidb.Mr

* file:[data/gclidb/mr.ssim](/data/gclidb/mr.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|mr|algo.Smallstr150|Val||Identifier. must be in form project.iid|
|proj|[gclidb.Project](/txt/ssimdb/gclidb/project.md)|Pkey||Project portion of mr<br>.RL of mr|
|iid|algo.cstring|Val||Iid portion of mr<br>.RR of mr|
|state|algo.cstring|Val||Mr state|
|title|algo.cstring|Val||Mr title|
|description|algo.cstring|Val||Mr description|
|source_branch|algo.cstring|Val||Source branch|
|source_repo|algo.cstring|Val||Source branch|
|reviewer|algo.cstring|Val||Reviewer username|
|author|algo.cstring|Val||Author username|
|pipeline_status|algo.Smallstr20|Val||Pipeline status|
|pipeline_id|algo.cstring|Val||Pipeline id|
|draft|algo.cstring|Val||Some draft status|
|node_id|algo.cstring|Val||Node id for graphQL|

### Related
<a href="#related"></a>
These ssimfiles reference gclidb.mr

* [gclidb.mrjob via mr](/txt/ssimdb/gclidb/mrjob.md) - Mr portion of mr job 
* [gclidb.mrjob via ref](/txt/ssimdb/gclidb/mrjob.md) - Mr portion of mr job 
* [gclidb.mrnote via mr](/txt/ssimdb/gclidb/mrnote.md) - Mr portion of mr note

### Used In Executables
<a href="#used-in-executables"></a>
* [gcli](/txt/exe/gcli/README.md) as gcli.FMr


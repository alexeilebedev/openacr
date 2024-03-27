## gclidb.issue - Gitlab project issue


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)gclidb.Issue

* file:[data/gclidb/issue.ssim](/data/gclidb/issue.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|issue|algo.Smallstr50|Val||Identifier. must be in form project.iid|
|project|[gclidb.Project](/txt/ssimdb/gclidb/project.md)|Pkey||Project portion of issue<br>.RL of issue|
|iid|algo.cstring|Val||Iid portion of issue<br>.RR of issue|
|assignee|algo.Smallstr50|Val||User the issue is assigned to|
|labels|algo.cstring|Val||issue labels - comma-separated list|
|milestone_iid|algo.cstring|Val||issue milestone, blank if none|
|milestone_num|algo.cstring|Val||issue milestone, blank if none|
|milestone|algo.cstring|Val||issue milestone name, blank if none|
|title|algo.cstring|Val||issue title|
|author|algo.cstring|Val||issue author|
|state|algo.cstring|Val||issue state|
|mr|algo.cstring|Val||mr issue|
|description|algo.cstring|Val||issue description|

### Related
<a href="#related"></a>
These ssimfiles reference gclidb.issue

* [gclidb.issuenote via issue](/txt/ssimdb/gclidb/issuenote.md) - Issue portion of issue note 
* [gclidb.issuepipeline via issue](/txt/ssimdb/gclidb/issuepipeline.md) - Issue portion of issue pipeline

### Used In Executables
<a href="#used-in-executables"></a>
* [gcli](/txt/exe/gcli/README.md) as gcli.FIssue


## gclidb.mrjob - Mr job


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)gclidb.Mrjob

* file:[data/gclidb/mrjob.ssim](/data/gclidb/mrjob.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|mrjob|algo.Smallstr250|Val||Identifier. must be in form mr.id|
|mr|[gclidb.Mr](/txt/ssimdb/gclidb/mr.md)|Pkey||Mr portion of mr job<br>/RL of mrjob|
|job|algo.cstring|Val||Id portion of mr job<br>/RR of mrjob|
|ref|[gclidb.Mr](/txt/ssimdb/gclidb/mr.md)|Pkey||Mr portion of mr job|
|id|algo.cstring|Val||Job ID|
|name|algo.cstring|Val||Job name|
|pipeline_id|algo.cstring|Val||Pipeline id|
|status|algo.cstring|Val|
|runner|algo.cstring|Val|

### Used In Executables
<a href="#used-in-executables"></a>
* [gcli](/txt/exe/gcli/README.md) as gcli.FMrjob


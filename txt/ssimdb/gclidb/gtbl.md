## gclidb.gtbl - Supported gcli tables


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)gclidb.Gtbl

* file:[data/gclidb/gtbl.ssim](/data/gclidb/gtbl.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gtbl|[algo.Smallstr250](/txt/protocol/algo/README.md#algo-smallstr250)|[Val](/txt/exe/amc/reftype.md#val)|||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype.md#val)|||

### Related
<a href="#related"></a>
These ssimfiles reference gclidb.gtbl

* [gclidb.gtblact via gtbl](/txt/ssimdb/gclidb/gtblact.md) - static combination of gcli tables and actions

### Used In Commands
<a href="#used-in-commands"></a>
* [gcli -selector](/txt/exe/gcli/README.md) - table:key, where table is issue,repo,mr, etc. and key is a regex.

### Used In Executables
<a href="#used-in-executables"></a>
* [gcli](/txt/gen/gcli/gcli.md) as [gcli.FGtbl](/txt/gen/gcli/gcli.md#gcli-fgtbl)

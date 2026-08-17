## gclidb.gclicmd - table of static gcli commands


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)gclidb.Gclicmd

* file:[data/gclidb/gclicmd.ssim](/data/gclidb/gclicmd.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|gclicmd|[algo.Smallstr250](/txt/protocol/algo/README.md#algo-smallstr250)|[Val](/txt/exe/amc/reftype.md#val)||table of static commands|
|gclicmdf2j|[gclidb.Gclicmd](/txt/ssimdb/gclidb/gclicmd.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)||Base for gclicmdf2j|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype.md#val)|||

### Related
<a href="#related"></a>
These ssimfiles reference gclidb.gclicmd

* [gclidb.gclicmd via gclicmdf2j](/txt/ssimdb/gclidb/gclicmd.md) - Base for gclicmdf2j 
* [gclidb.gclicmdf2j via gclicmd](/txt/ssimdb/gclidb/gclicmdf2j.md) - Mapping of internal fields to JSON fields for gitlab/github 
* [gclidb.gclicmdt via gclicmd](/txt/ssimdb/gclidb/gclicmdt.md) - Internal test

### Used In Executables
<a href="#used-in-executables"></a>
* [gcli](/txt/gen/gcli/gcli.md) as [gcli.FGclicmd](/txt/gen/gcli/gcli.md#gcli-fgclicmd)

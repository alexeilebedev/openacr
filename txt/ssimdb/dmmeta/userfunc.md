## dmmeta.userfunc -


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Userfunc

* file:[data/dmmeta/userfunc.ssim](/data/dmmeta/userfunc.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|userfunc|[algo.Smallstr100](/txt/protocol/algo/README.md#algo-smallstr100)|[Val](/txt/exe/amc/reftype.md#val)||Primary key: as wide as dmmeta.Func.func, the key it is copied from|
|acrkey|[algo.Smallstr200](/txt/protocol/algo/README.md#algo-smallstr200)|[Val](/txt/exe/amc/reftype.md#val)|||
|cppname|[algo.Smallstr100](/txt/protocol/algo/README.md#algo-smallstr100)|[Val](/txt/exe/amc/reftype.md#val)|||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype.md#val)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [amc](/txt/gen/amc/amc.md) as [amc.FUserfunc](/txt/gen/amc/amc.md#amc-fuserfunc)
* [src_func](/txt/gen/src_func/src_func.md) as [src_func.FUserfunc](/txt/gen/src_func/src_func.md#src_func-fuserfunc)

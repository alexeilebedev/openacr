## sampdb.targdep - samp_make targets dependencies


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)sampdb.Targdep

* file:[data/sampdb/targdep.ssim](/data/sampdb/targdep.ssim)

italicised fields: *target, parent* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|targdep|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype/Val.md)|||
|*target*|*[sampdb.Target](/txt/ssimdb/sampdb/target.md)*|*[Pkey](/txt/exe/amc/reftype/Pkey.md)*||*Child target<br>.LL of targdep*|
|*parent*|*[sampdb.Target](/txt/ssimdb/sampdb/target.md)*|*[Pkey](/txt/exe/amc/reftype/Pkey.md)*||*Parent target<br>.LR of targdep*|
|rec|bool|[Val](/txt/exe/amc/reftype/Val.md)||use in recipe|
|pre|bool|[Val](/txt/exe/amc/reftype/Val.md)||use in prerequisite|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [samp_make](/txt/gen/samp_make/samp_make.md) as [samp_make.FTargdep](/txt/gen/samp_make/samp_make.md#samp_make-ftargdep)

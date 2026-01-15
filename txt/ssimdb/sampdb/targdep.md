## sampdb.targdep - samp_make targets dependencies


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)sampdb.Targdep

* file:[data/sampdb/targdep.ssim](/data/sampdb/targdep.ssim)

italicised fields: *target, parent* are [**fldfunc**](/txt/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|targdep|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)|||
|*target*|*[sampdb.Target](/txt/ssimdb/sampdb/target.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*Child target<br>.LL of targdep*|
|*parent*|*[sampdb.Target](/txt/ssimdb/sampdb/target.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*Parent target<br>.LR of targdep*|
|rec|bool|[Val](/txt/exe/amc/reftypes.md#val)||use in recipe|
|pre|bool|[Val](/txt/exe/amc/reftypes.md#val)||use in prerequisite|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [samp_make](/txt/exe/samp_make/internals.md) as [samp_make.FTargdep](/txt/exe/samp_make/internals.md#samp_make-ftargdep)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->


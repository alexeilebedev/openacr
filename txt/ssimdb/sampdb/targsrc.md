## sampdb.targsrc - samp_make targets source dependencies


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)sampdb.Targsrc

* file:[data/sampdb/targsrc.ssim](/data/sampdb/targsrc.ssim)

italicised fields: *target, src* are [**fldfunc**](/txt/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|targsrc|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)|||
|pre|bool|[Val](/txt/exe/amc/reftypes.md#val)||use in prerequisite|
|*target*|*[sampdb.Target](/txt/ssimdb/sampdb/target.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*<br>/LL of targsrc*|
|*src*|*[sampdb.Gitfile](/txt/ssimdb/sampdb/gitfile.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*<br>/LR of targsrc*|
|rec|bool|[Val](/txt/exe/amc/reftypes.md#val)||use in recipe|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [samp_make](/txt/exe/samp_make/internals.md) as [samp_make.FTargsrc](/txt/exe/samp_make/internals.md#samp_make-ftargsrc)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->


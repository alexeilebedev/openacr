## sampdb.targsrc - samp_make targets source dependencies


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)sampdb.Targsrc

* file:[data/sampdb/targsrc.ssim](/data/sampdb/targsrc.ssim)

italicised fields: *target, src* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|targsrc|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype.md#val)|||
|pre|bool|[Val](/txt/exe/amc/reftype.md#val)||use in prerequisite|
|*target*|*[sampdb.Target](/txt/ssimdb/sampdb/target.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*<br>/LL of targsrc*|
|*src*|*[sampdb.Gitfile](/txt/ssimdb/sampdb/gitfile.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*<br>/LR of targsrc*|
|rec|bool|[Val](/txt/exe/amc/reftype.md#val)||use in recipe|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype.md#val)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [samp_make](/txt/gen/samp_make/samp_make.md) as [samp_make.FTargsrc](/txt/gen/samp_make/samp_make.md#samp_make-ftargsrc)

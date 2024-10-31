## dev.include - A site where one file includes another


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Include

* file:[data/dev/include.ssim](/data/dev/include.ssim)

italicised fields: *srcfile, filename* are [**fldfunc**](/txt/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|include|[algo.Smallstr200](/txt/protocol/algo/README.md#algo-smallstr200)|[Val](/txt/exe/amc/reftypes.md#val)|||
|*srcfile*|*[dev.Srcfile](/txt/ssimdb/dev/srcfile.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*<br>:LL of include*|
|*filename*|*[dev.Srcfile](/txt/ssimdb/dev/srcfile.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*<br>:LR of include*|
|sys|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [abt](/txt/exe/abt/internals.md) as [abt.FInclude](/txt/exe/abt/internals.md#abt-finclude)
* [src_lim](/txt/exe/src_lim/internals.md) as [src_lim.FInclude](/txt/exe/src_lim/internals.md#src_lim-finclude)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->


## dev.include - A site where one file includes another


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Include

* file:[data/dev/include.ssim](/data/dev/include.ssim)

italicised fields: *srcfile, filename* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|include|[algo.Smallstr200](/txt/protocol/algo/README.md#algo-smallstr200)|[Val](/txt/exe/amc/reftype.md#val)|||
|*srcfile*|*[dev.Srcfile](/txt/ssimdb/dev/srcfile.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*<br>:LL of include*|
|*filename*|*[dev.Srcfile](/txt/ssimdb/dev/srcfile.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*<br>:LR of include*|
|sys|bool|[Val](/txt/exe/amc/reftype.md#val)|||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype.md#val)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [abt](/txt/gen/abt/abt.md) as [abt.FInclude](/txt/gen/abt/abt.md#abt-finclude)
* [src_lim](/txt/gen/src_lim/src_lim.md) as [src_lim.FInclude](/txt/gen/src_lim/src_lim.md#src_lim-finclude)

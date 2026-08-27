## dev.targsrc - List of sources for target


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Targsrc

* file:[data/dev/targsrc.ssim](/data/dev/targsrc.ssim)

italicised fields: *target, src, ext* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|targsrc|[algo.Smallstr100](/txt/protocol/algo/README.md#algo-smallstr100)|[Val](/txt/exe/amc/reftype/Val.md)|||
|*target*|*[dev.Target](/txt/ssimdb/dev/target.md)*|*[Pkey](/txt/exe/amc/reftype/Pkey.md)*||*<br>/LL of targsrc*|
|*src*|*[dev.Gitfile](/txt/ssimdb/dev/gitfile.md)*|*[Pkey](/txt/exe/amc/reftype/Pkey.md)*||*<br>/LR of targsrc*|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype/Val.md)|||
|*ext*|*[algo.Smallstr10](/txt/protocol/algo/README.md#algo-smallstr10)*|*[Val](/txt/exe/amc/reftype/Val.md)*||*<br>.RR of targsrc*|

### Used In Commands
<a href="#used-in-commands"></a>
* [src_func -targsrc](/txt/exe/src_func/README.md) - (scan) Limit scanning to these sources only 
* [src_hdr -targsrc](/txt/exe/src_hdr/README.md) - Regx of targsrc to update

### Used In Executables
<a href="#used-in-executables"></a>
* [abt](/txt/gen/abt/abt.md) as [abt.FTargsrc](/txt/gen/abt/abt.md#abt-ftargsrc)
* [abt_md](/txt/gen/abt_md/abt_md.md) as [abt_md.FTargsrc](/txt/gen/abt_md/abt_md.md#abt_md-ftargsrc)
* [acr_ed](/txt/gen/acr_ed/acr_ed.md) as [acr_ed.FTargsrc](/txt/gen/acr_ed/acr_ed.md#acr_ed-ftargsrc)
* [atf_ci](/txt/gen/atf_ci/atf_ci.md) as [atf_ci.FTargsrc](/txt/gen/atf_ci/atf_ci.md#atf_ci-ftargsrc)
* [atf_cov](/txt/gen/atf_cov/atf_cov.md) as [atf_cov.FTargsrc](/txt/gen/atf_cov/atf_cov.md#atf_cov-ftargsrc)
* [src_func](/txt/gen/src_func/src_func.md) as [src_func.FTargsrc](/txt/gen/src_func/src_func.md#src_func-ftargsrc)
* [src_hdr](/txt/gen/src_hdr/src_hdr.md) as [src_hdr.FTargsrc](/txt/gen/src_hdr/src_hdr.md#src_hdr-ftargsrc)
* [src_lim](/txt/gen/src_lim/src_lim.md) as [src_lim.FTargsrc](/txt/gen/src_lim/src_lim.md#src_lim-ftargsrc)

## dev.targsrc - List of sources for target


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Targsrc

* file:[data/dev/targsrc.ssim](/data/dev/targsrc.ssim)

italicised fields: *target, src, ext* are [**fldfunc**](/txt/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|targsrc|[algo.Smallstr100](/txt/protocol/algo/README.md#algo-smallstr100)|[Val](/txt/exe/amc/reftypes.md#val)|||
|*target*|*[dev.Target](/txt/ssimdb/dev/target.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*<br>/LL of targsrc*|
|*src*|*[dev.Gitfile](/txt/ssimdb/dev/gitfile.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*<br>/LR of targsrc*|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|*ext*|*[algo.Smallstr10](/txt/protocol/algo/README.md#algo-smallstr10)*|*[Val](/txt/exe/amc/reftypes.md#val)*||*<br>.RR of targsrc*|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Commands
<a href="#used-in-commands"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:CmdlineUses -->

* [src_func -targsrc](/txt/exe/src_func/README.md) - Visit these sources (optional) 
* [src_hdr -targsrc](/txt/exe/src_hdr/README.md) - Regx of targsrc to update 

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:CmdlineUses -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [abt](/txt/exe/abt/internals.md) as [abt.FTargsrc](/txt/exe/abt/internals.md#abt-ftargsrc)
* [abt_md](/txt/exe/abt_md/internals.md) as [abt_md.FTargsrc](/txt/exe/abt_md/internals.md#abt_md-ftargsrc)
* [acr_ed](/txt/exe/acr_ed/internals.md) as [acr_ed.FTargsrc](/txt/exe/acr_ed/internals.md#acr_ed-ftargsrc)
* [atf_ci](/txt/exe/atf_ci/internals.md) as [atf_ci.FTargsrc](/txt/exe/atf_ci/internals.md#atf_ci-ftargsrc)
* [atf_cov](/txt/exe/atf_cov/internals.md) as [atf_cov.FTargsrc](/txt/exe/atf_cov/internals.md#atf_cov-ftargsrc)
* [src_func](/txt/exe/src_func/internals.md) as [src_func.FTargsrc](/txt/exe/src_func/internals.md#src_func-ftargsrc)
* [src_hdr](/txt/exe/src_hdr/internals.md) as [src_hdr.FTargsrc](/txt/exe/src_hdr/internals.md#src_hdr-ftargsrc)
* [src_lim](/txt/exe/src_lim/internals.md) as [src_lim.FTargsrc](/txt/exe/src_lim/internals.md#src_lim-ftargsrc)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->


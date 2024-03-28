## dev.targsrc - List of sources for target


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Targsrc

* file:[data/dev/targsrc.ssim](/data/dev/targsrc.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|targsrc|algo.Smallstr100|Val|
|target|[dev.Target](/txt/ssimdb/dev/target.md)|Pkey||<br>/LL of targsrc|
|src|[dev.Gitfile](/txt/ssimdb/dev/gitfile.md)|Pkey||<br>/LR of targsrc|
|comment|algo.Comment|Val|
|ext|algo.Smallstr10|Val||<br>.RR of targsrc|

### Used In Commands
<a href="#used-in-commands"></a>
* [src_func -targsrc](/txt/exe/src_func/README.md) - Visit these sources (optional) 
* [src_hdr -targsrc](/txt/exe/src_hdr/README.md) - Regx of targsrc to update

### Used In Executables
<a href="#used-in-executables"></a>
* [abt](/txt/exe/abt/README.md) as abt.FTargsrc
* [abt_md](/txt/exe/abt_md/README.md) as abt_md.FTargsrc
* [acr_ed](/txt/exe/acr_ed/README.md) as acr_ed.FTargsrc
* [atf_ci](/txt/exe/atf_ci/README.md) as atf_ci.FTargsrc
* [atf_cov](/txt/exe/atf_cov/README.md) as atf_cov.FTargsrc
* [src_func](/txt/exe/src_func/README.md) as src_func.FTargsrc
* [src_hdr](/txt/exe/src_hdr/README.md) as src_hdr.FTargsrc
* [src_lim](/txt/exe/src_lim/README.md) as src_lim.FTargsrc


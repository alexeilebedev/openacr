## dev.badline - Regex of a dubious source code line


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Badline

* file:[data/dev/badline.ssim](/data/dev/badline.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|badline|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype.md#val)|||
|expr|[algo.Smallstr200](/txt/protocol/algo/README.md#algo-smallstr200)|[Val](/txt/exe/amc/reftype.md#val)|||
|targsrc_regx|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype.md#val)|||
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype.md#val)|||

### Used In Commands
<a href="#used-in-commands"></a>
* [src_lim -badline](/txt/exe/src_lim/README.md) - Check badline (acr badline)

### Used In Executables
<a href="#used-in-executables"></a>
* [src_lim](/txt/gen/src_lim/src_lim.md) as [src_lim.FBadline](/txt/gen/src_lim/src_lim.md#src_lim-fbadline)

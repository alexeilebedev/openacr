## dev.pkgkey - Keys belonging to the OpenACR package


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Pkgkey

* file:[data/dev/pkgkey.ssim](/data/dev/pkgkey.ssim)

italicised fields: *package, key* are [**fldfunc**](/txt/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|pkgkey|[algo.Smallstr150](/txt/protocol/algo/README.md#algo-smallstr150)|[Val](/txt/exe/amc/reftypes.md#val)|||
|*package*|*[dev.Package](/txt/ssimdb/dev/package.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*Parent package<br>/LL of pkgkey*|
|*key*|*[algo.Smallstr150](/txt/protocol/algo/README.md#algo-smallstr150)*|*[Val](/txt/exe/amc/reftypes.md#val)*||*Acr query (regx)<br>/LR of pkgkey*|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [apm](/txt/exe/apm/internals.md) as [apm.FPkgkey](/txt/exe/apm/internals.md#apm-fpkgkey)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->


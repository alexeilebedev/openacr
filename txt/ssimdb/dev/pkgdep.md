## dev.pkgdep - OpenACR Package dependency


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Pkgdep

* file:[data/dev/pkgdep.ssim](/data/dev/pkgdep.ssim)

italicised fields: *package, parent* are [**fldfunc**](/txt/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|pkgdep|[algo.Smallstr100](/txt/protocol/algo/README.md#algo-smallstr100)|[Val](/txt/exe/amc/reftypes.md#val)|||
|*package*|*[dev.Package](/txt/ssimdb/dev/package.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*<br>.RL of pkgdep*|
|*parent*|*[dev.Package](/txt/ssimdb/dev/package.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*<br>.RR of pkgdep*|
|soft|bool|[Val](/txt/exe/amc/reftypes.md#val)||Soft dependency: affects package ordering but not forced update|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [apm](/txt/exe/apm/internals.md) as [apm.FPkgdep](/txt/exe/apm/internals.md#apm-fpkgdep)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->


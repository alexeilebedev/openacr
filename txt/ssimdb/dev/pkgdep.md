## dev.pkgdep - OpenACR Package dependency


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Pkgdep

* file:[data/dev/pkgdep.ssim](/data/dev/pkgdep.ssim)

italicised fields: *package, parent* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|pkgdep|[algo.Smallstr100](/txt/protocol/algo/README.md#algo-smallstr100)|[Val](/txt/exe/amc/reftype/Val.md)|||
|*package*|*[dev.Package](/txt/ssimdb/dev/package.md)*|*[Pkey](/txt/exe/amc/reftype/Pkey.md)*||*<br>.RL of pkgdep*|
|*parent*|*[dev.Package](/txt/ssimdb/dev/package.md)*|*[Pkey](/txt/exe/amc/reftype/Pkey.md)*||*<br>.RR of pkgdep*|
|soft|bool|[Val](/txt/exe/amc/reftype/Val.md)||Soft dependency: affects package ordering but not forced update|
|pkgdeptype|[dev.Pkgdeptype](/txt/ssimdb/dev/pkgdeptype.md)|[Pkey](/txt/exe/amc/reftype/Pkey.md)|"require"|How this package stands to its parent|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [apm](/txt/gen/apm/apm.md) as [apm.FPkgdep](/txt/gen/apm/apm.md#apm-fpkgdep)

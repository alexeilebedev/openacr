## dev.pkgdep - OpenACR Package dependency


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Pkgdep

* file:[data/dev/pkgdep.ssim](/data/dev/pkgdep.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|pkgdep|algo.Smallstr100|Val|
|package|[dev.Package](/txt/ssimdb/dev/package.md)|Pkey||<br>.RL of pkgdep|
|parent|[dev.Package](/txt/ssimdb/dev/package.md)|Pkey||<br>.RR of pkgdep|
|soft|bool|Val||Soft dependency: affects package ordering but not forced update|
|comment|algo.Comment|Val|

### Used In Executables
<a href="#used-in-executables"></a>
* [apm](/txt/exe/apm/README.md) as apm.FPkgdep


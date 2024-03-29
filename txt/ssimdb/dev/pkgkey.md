## dev.pkgkey - Keys belonging to the OpenACR package


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Pkgkey

* file:[data/dev/pkgkey.ssim](/data/dev/pkgkey.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|pkgkey|algo.Smallstr150|Val|
|package|[dev.Package](/txt/ssimdb/dev/package.md)|Pkey||Parent package<br>/LL of pkgkey|
|key|algo.Smallstr150|Val||Acr query (regx)<br>/LR of pkgkey|
|comment|algo.Comment|Val|

### Used In Executables
<a href="#used-in-executables"></a>
* [apm](/txt/exe/apm/README.md) as apm.FPkgkey


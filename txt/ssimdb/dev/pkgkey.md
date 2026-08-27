## dev.pkgkey - Keys belonging to the OpenACR package


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Pkgkey

* file:[data/dev/pkgkey.ssim](/data/dev/pkgkey.ssim)

italicised fields: *package, key* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|pkgkey|[algo.Smallstr150](/txt/protocol/algo/README.md#algo-smallstr150)|[Val](/txt/exe/amc/reftype/Val.md)|||
|*package*|*[dev.Package](/txt/ssimdb/dev/package.md)*|*[Pkey](/txt/exe/amc/reftype/Pkey.md)*||*Parent package<br>/LL of pkgkey*|
|*key*|*[algo.Smallstr150](/txt/protocol/algo/README.md#algo-smallstr150)*|*[Val](/txt/exe/amc/reftype/Val.md)*||*Acr query (regx) - named key for clarity<br>/LR of pkgkey*|
|up|bool|[Val](/txt/exe/amc/reftype/Val.md)||perform transitive closure upwards|
|down|bool|[Val](/txt/exe/amc/reftype/Val.md)||perform transitive closure downwards (via the leftmost parent link)|
|exclude|bool|[Val](/txt/exe/amc/reftype/Val.md)||exclude selected records from the package|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

### Used In Executables
<a href="#used-in-executables"></a>
* [apm](/txt/gen/apm/apm.md) as [apm.FPkgkey](/txt/gen/apm/apm.md#apm-fpkgkey)

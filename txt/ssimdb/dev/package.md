## dev.package - OpenACR package


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Package

* file:[data/dev/package.ssim](/data/dev/package.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|package|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype/Val.md)||Package name|
|baseref|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype/Val.md)||Base gitref of installed package|
|origin|[algo.Smallstr200](/txt/protocol/algo/README.md#algo-smallstr200)|[Val](/txt/exe/amc/reftype/Val.md)||Source URL for published package|
|nomention|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype/Val.md)||Regx of words the package contents must not mention; empty disables the check|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype/Val.md)|||

### Subsets
<a href="#subsets"></a>
These ssimfiles are subsets of dev.package

* [atfdb.Cipackage](/txt/ssimdb/atfdb/cipackage.md) -  
* [dev.Pkggen](/txt/ssimdb/dev/pkggen.md) -

### Related
<a href="#related"></a>
These ssimfiles reference dev.package

* [dev.pkgdep via package](/txt/ssimdb/dev/pkgdep.md) - OpenACR Package dependency 
* [dev.pkgdep via parent](/txt/ssimdb/dev/pkgdep.md) - OpenACR Package dependency 
* [dev.pkgkey via package](/txt/ssimdb/dev/pkgkey.md) - Parent package

### Used In Commands
<a href="#used-in-commands"></a>
* [apm -package](/txt/exe/apm/README.md) - Regx of package

### Used In Executables
<a href="#used-in-executables"></a>
* [apm](/txt/gen/apm/apm.md) as [apm.FPackage](/txt/gen/apm/apm.md#apm-fpackage)
* [atf_ci](/txt/gen/atf_ci/atf_ci.md) as [atf_ci.FPackage](/txt/gen/atf_ci/atf_ci.md#atf_ci-fpackage)

## dev.package - OpenACR package


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Package

* file:[data/dev/package.ssim](/data/dev/package.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|package|algo.Smallstr50|Val||Package name|
|baseref|algo.Smallstr50|Val||Base gitref of installed package|
|origin|algo.Smallstr200|Val||Source URL for published package|
|comment|algo.Comment|Val|

### Subsets
<a href="#subsets"></a>
These ssimfiles are subsets of dev.package

* [atfdb.Cipackage](/txt/ssimdb/atfdb/cipackage.md) -

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
* [apm](/txt/exe/apm/README.md) as apm.FPackage


## dev.package - OpenACR package


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Package

* file:[data/dev/package.ssim](/data/dev/package.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|package|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)||Package name|
|baseref|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)||Base gitref of installed package|
|origin|[algo.Smallstr200](/txt/protocol/algo/README.md#algo-smallstr200)|[Val](/txt/exe/amc/reftypes.md#val)||Source URL for published package|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Subsets
<a href="#subsets"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Subsets -->
These ssimfiles are subsets of dev.package

* [atfdb.Cipackage](/txt/ssimdb/atfdb/cipackage.md) -  

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Subsets -->

### Related
<a href="#related"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Related -->
These ssimfiles reference dev.package

* [dev.pkgdep via package](/txt/ssimdb/dev/pkgdep.md) - OpenACR Package dependency 
* [dev.pkgdep via parent](/txt/ssimdb/dev/pkgdep.md) - OpenACR Package dependency 
* [dev.pkgkey via package](/txt/ssimdb/dev/pkgkey.md) - Parent package 

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Related -->

### Used In Commands
<a href="#used-in-commands"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:CmdlineUses -->

* [apm -package](/txt/exe/apm/README.md) - Regx of package 

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:CmdlineUses -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [apm](/txt/exe/apm/internals.md) as [apm.FPackage](/txt/exe/apm/internals.md#apm-fpackage)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->


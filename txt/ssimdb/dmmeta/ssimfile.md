## dmmeta.ssimfile - File with ssim tuples
<a href="#dmmeta-ssimfile"></a>

This record indicates that a ssimfile exists for the given ctype.

### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Ssimfile

* file:[data/dmmeta/ssimfile.ssim](/data/dmmeta/ssimfile.ssim)

italicised fields: *ssimns, ns, name* are [**fldfunc**](/txt/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ssimfile|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)|||
|*ssimns*|*[dmmeta.Nsdb](/txt/ssimdb/dmmeta/nsdb.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*<br>.LL of ssimfile*|
|*ns*|*[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*<br>.LL of ssimfile*|
|*name*|*[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)*|*[Val](/txt/exe/amc/reftypes.md#val)*||*<br>.RR of ssimfile*|
|ctype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Subsets
<a href="#subsets"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Subsets -->
These ssimfiles are subsets of dmmeta.ssimfile

* [dmmeta.Ssimsort](/txt/ssimdb/dmmeta/ssimsort.md) - Define sort order for ssimfile 
* [dmmeta.Ssimvolatile](/txt/ssimdb/dmmeta/ssimvolatile.md) - Types based on ssim files marked this way cannot be cross-referenced 

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Subsets -->

### Related
<a href="#related"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Related -->
These ssimfiles reference dmmeta.ssimfile

* [dmmeta.gsymbol via ssimfile](/txt/ssimdb/dmmeta/gsymbol.md) - Source ssimfile 
* [dmmeta.ssimreq via child_ssimfile](/txt/ssimdb/dmmeta/ssimreq.md) - Extended constraints for ssim records 

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Related -->

### Used In Commands
<a href="#used-in-commands"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:CmdlineUses -->

* [acr_ed -ssimfile](/txt/exe/acr_ed/README.md) - Ssimfile for new ctype 
* [acr_in -notssimfile](/txt/exe/acr_in/README.md) - Exclude ssimfiles matching regx 
* [acr_in -r](/txt/exe/acr_in/README.md) - Reverse lookup of target by ssimfile 

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:CmdlineUses -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [abt_md](/txt/exe/abt_md/internals.md) as [abt_md.FSsimfile](/txt/exe/abt_md/internals.md#abt_md-fssimfile)
* [acr](/txt/exe/acr/internals.md) as [acr.FSsimfile](/txt/exe/acr/internals.md#acr-fssimfile)
* [acr_compl](/txt/exe/acr_compl/internals.md) as [acr_compl.FSsimfile](/txt/exe/acr_compl/internals.md#acr_compl-fssimfile)
* [acr_ed](/txt/exe/acr_ed/internals.md) as [acr_ed.FSsimfile](/txt/exe/acr_ed/internals.md#acr_ed-fssimfile)
* [acr_in](/txt/exe/acr_in/internals.md) as [acr_in.FSsimfile](/txt/exe/acr_in/internals.md#acr_in-fssimfile)
* [acr_my](/txt/exe/acr_my/internals.md) as [acr_my.FSsimfile](/txt/exe/acr_my/internals.md#acr_my-fssimfile)
* [amc](/txt/exe/amc/internals.md) as [amc.FSsimfile](/txt/exe/amc/internals.md#amc-fssimfile)
* [apm](/txt/exe/apm/internals.md) as [apm.FSsimfile](/txt/exe/apm/internals.md#apm-fssimfile)
* [atf_ci](/txt/exe/atf_ci/internals.md) as [atf_ci.FSsimfile](/txt/exe/atf_ci/internals.md#atf_ci-fssimfile)
* [lib_ctype](/txt/lib/lib_ctype/README.md) as [lib_ctype.FSsimfile](/txt/lib/lib_ctype/README.md#lib_ctype-fssimfile)
* [ssim2mysql](/txt/exe/ssim2mysql/internals.md) as [ssim2mysql.FSsimfile](/txt/exe/ssim2mysql/internals.md#ssim2mysql-fssimfile)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->


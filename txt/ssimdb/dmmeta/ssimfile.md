## dmmeta.ssimfile - File with ssim tuples
<a href="#dmmeta-ssimfile"></a>

This record indicates that a ssimfile exists for the given ctype.

### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Ssimfile

* file:[data/dmmeta/ssimfile.ssim](/data/dmmeta/ssimfile.ssim)

italicised fields: *ssimns, ns, name* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ssimfile|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype/Val.md)|||
|*ssimns*|*[dmmeta.Nsdb](/txt/ssimdb/dmmeta/nsdb.md)*|*[Pkey](/txt/exe/amc/reftype/Pkey.md)*||*<br>.LL of ssimfile*|
|*ns*|*[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)*|*[Pkey](/txt/exe/amc/reftype/Pkey.md)*||*<br>.LL of ssimfile*|
|*name*|*[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)*|*[Val](/txt/exe/amc/reftype/Val.md)*||*<br>.RR of ssimfile*|
|ctype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Pkey](/txt/exe/amc/reftype/Pkey.md)|||

### Subsets
<a href="#subsets"></a>
These ssimfiles are subsets of dmmeta.ssimfile

* [dmmeta.Ssimsort](/txt/ssimdb/dmmeta/ssimsort.md) - Define sort order for ssimfile 
* [dmmeta.Ssimvolatile](/txt/ssimdb/dmmeta/ssimvolatile.md) - Types based on ssim files marked this way cannot be cross-referenced

### Related
<a href="#related"></a>
These ssimfiles reference dmmeta.ssimfile

* [dmmeta.gsymbol via ssimfile](/txt/ssimdb/dmmeta/gsymbol.md) - Source ssimfile 
* [dmmeta.ssimreq via child_ssimfile](/txt/ssimdb/dmmeta/ssimreq.md) - Extended constraints for ssim records

### Used In Commands
<a href="#used-in-commands"></a>
* [acr_ed -ssimfile](/txt/exe/acr_ed/README.md) - Ssimfile for new ctype 
* [acr_in -notssimfile](/txt/exe/acr_in/README.md) - Exclude ssimfiles matching regx 
* [acr_in -r](/txt/exe/acr_in/README.md) - Reverse lookup of target by ssimfile

### Used In Executables
<a href="#used-in-executables"></a>
* [abt_md](/txt/gen/abt_md/abt_md.md) as [abt_md.FSsimfile](/txt/gen/abt_md/abt_md.md#abt_md-fssimfile)
* [acr](/txt/gen/acr/acr.md) as [acr.FSsimfile](/txt/gen/acr/acr.md#acr-fssimfile)
* [acr_compl](/txt/gen/acr_compl/acr_compl.md) as [acr_compl.FSsimfile](/txt/gen/acr_compl/acr_compl.md#acr_compl-fssimfile)
* [acr_ed](/txt/gen/acr_ed/acr_ed.md) as [acr_ed.FSsimfile](/txt/gen/acr_ed/acr_ed.md#acr_ed-fssimfile)
* [acr_in](/txt/gen/acr_in/acr_in.md) as [acr_in.FSsimfile](/txt/gen/acr_in/acr_in.md#acr_in-fssimfile)
* [acr_my](/txt/gen/acr_my/acr_my.md) as [acr_my.FSsimfile](/txt/gen/acr_my/acr_my.md#acr_my-fssimfile)
* [amc](/txt/gen/amc/amc.md) as [amc.FSsimfile](/txt/gen/amc/amc.md#amc-fssimfile)
* [apm](/txt/gen/apm/apm.md) as [apm.FSsimfile](/txt/gen/apm/apm.md#apm-fssimfile)
* [atf_ci](/txt/gen/atf_ci/atf_ci.md) as [atf_ci.FSsimfile](/txt/gen/atf_ci/atf_ci.md#atf_ci-fssimfile)
* [lib_ctype](/txt/gen/lib_ctype/lib_ctype.md) as [lib_ctype.FSsimfile](/txt/gen/lib_ctype/lib_ctype.md#lib_ctype-fssimfile)
* [lib_sqlite](/txt/gen/lib_sqlite/lib_sqlite.md) as [lib_sqlite.FSsimfile](/txt/gen/lib_sqlite/lib_sqlite.md#lib_sqlite-fssimfile)
* [ssim2mysql](/txt/gen/ssim2mysql/ssim2mysql.md) as [ssim2mysql.FSsimfile](/txt/gen/ssim2mysql/ssim2mysql.md#ssim2mysql-fssimfile)

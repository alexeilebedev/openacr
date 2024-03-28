## dmmeta.ssimfile - File with ssim tuples
<a href="#dmmeta-ssimfile"></a>

This record indicates that a ssimfile exists for the given ctype.

### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Ssimfile

* file:[data/dmmeta/ssimfile.ssim](/data/dmmeta/ssimfile.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ssimfile|algo.Smallstr50|Val|
|ssimns|[dmmeta.Nsdb](/txt/ssimdb/dmmeta/nsdb.md)|Pkey||<br>.LL of ssimfile|
|ns|[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)|Pkey||<br>.LL of ssimfile|
|name|algo.Smallstr50|Val||<br>.RR of ssimfile|
|ctype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|Pkey|

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
* [abt_md](/txt/exe/abt_md/README.md) as abt_md.FSsimfile
* [acr](/txt/exe/acr/README.md) as acr.FSsimfile
* [acr_compl](/txt/exe/acr_compl/README.md) as acr_compl.FSsimfile
* [acr_ed](/txt/exe/acr_ed/README.md) as acr_ed.FSsimfile
* [acr_in](/txt/exe/acr_in/README.md) as acr_in.FSsimfile
* [acr_my](/txt/exe/acr_my/README.md) as acr_my.FSsimfile
* [amc](/txt/exe/amc/README.md) as amc.FSsimfile
* [apm](/txt/exe/apm/README.md) as apm.FSsimfile
* [atf_ci](/txt/exe/atf_ci/README.md) as atf_ci.FSsimfile
* [lib_ctype](/txt/lib/lib_ctype/README.md) as lib_ctype.FSsimfile
* [ssim2mysql](/txt/exe/ssim2mysql/README.md) as ssim2mysql.FSsimfile


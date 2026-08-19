## dev.target - Build target


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Target

* file:[data/dev/target.ssim](/data/dev/target.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|target|[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)||Primary key - name of target|

### Subsets
<a href="#subsets"></a>
These ssimfiles are subsets of dev.target

* [dev.Covtarget](/txt/ssimdb/dev/covtarget.md) -  
* [dev.Tgtcov](/txt/ssimdb/dev/tgtcov.md) - Captured line coverate information by target

### Related
<a href="#related"></a>
These ssimfiles reference dev.target

* [dev.targdep via target](/txt/ssimdb/dev/targdep.md) - Child target 
* [dev.targdep via parent](/txt/ssimdb/dev/targdep.md) - Parent target 
* [dev.targsrc via target](/txt/ssimdb/dev/targsrc.md) - List of sources for target 
* [dev.targsyslib via target](/txt/ssimdb/dev/targsyslib.md) - Use of system library by target

### Used In Commands
<a href="#used-in-commands"></a>
* [abt -target](/txt/exe/abt/README.md) - Regx of target name 
* [acr_ed -target](/txt/exe/acr_ed/README.md) - Create/Rename/Delete target 
* [amc_gc -target](/txt/exe/amc_gc/README.md) - Target to test-build 
* [atf_fuzz -target](/txt/exe/atf_fuzz/README.md) - Target to fuzz 
* [mdbg -target](/txt/exe/mdbg/README.md) - Executable name 
* [src_func -func](/txt/exe/src_func/README.md) - Target.function regex

### Used In Executables
<a href="#used-in-executables"></a>
* [abt](/txt/gen/abt/abt.md) as [abt.FTarget](/txt/gen/abt/abt.md#abt-ftarget)
* [abt_md](/txt/gen/abt_md/abt_md.md) as [abt_md.FTarget](/txt/gen/abt_md/abt_md.md#abt_md-ftarget)
* [acr_ed](/txt/gen/acr_ed/acr_ed.md) as [acr_ed.FTarget](/txt/gen/acr_ed/acr_ed.md#acr_ed-ftarget)
* [acr_in](/txt/gen/acr_in/acr_in.md) as [acr_in.FTarget](/txt/gen/acr_in/acr_in.md#acr_in-ftarget)
* [amc](/txt/gen/amc/amc.md) as [amc.FTarget](/txt/gen/amc/amc.md#amc-ftarget)
* [atf_cov](/txt/gen/atf_cov/atf_cov.md) as [atf_cov.FTarget](/txt/gen/atf_cov/atf_cov.md#atf_cov-ftarget)
* [atf_fuzz](/txt/gen/atf_fuzz/atf_fuzz.md) as [atf_fuzz.FTarget](/txt/gen/atf_fuzz/atf_fuzz.md#atf_fuzz-ftarget)
* [src_func](/txt/gen/src_func/src_func.md) as [src_func.FTarget](/txt/gen/src_func/src_func.md#src_func-ftarget)
* [src_hdr](/txt/gen/src_hdr/src_hdr.md) as [src_hdr.FTarget](/txt/gen/src_hdr/src_hdr.md#src_hdr-ftarget)

## dev.target - Build target


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Target

* file:[data/dev/target.ssim](/data/dev/target.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|target|[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)|Pkey||Primary key - name of target|
|compat|algo.Smallstr50|Val|"Linux-%.%-%"|Compatibility (regx of builddir)|

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
* [src_func -target](/txt/exe/src_func/README.md) - Visit these targets

### Used In Executables
<a href="#used-in-executables"></a>
* [abt](/txt/exe/abt/README.md) as abt.FTarget
* [acr_ed](/txt/exe/acr_ed/README.md) as acr_ed.FTarget
* [acr_in](/txt/exe/acr_in/README.md) as acr_in.FTarget
* [amc](/txt/exe/amc/README.md) as amc.FTarget
* [atf_cov](/txt/exe/atf_cov/README.md) as atf_cov.FTarget
* [atf_fuzz](/txt/exe/atf_fuzz/README.md) as atf_fuzz.FTarget
* [src_func](/txt/exe/src_func/README.md) as src_func.FTarget
* [src_hdr](/txt/exe/src_hdr/README.md) as src_hdr.FTarget


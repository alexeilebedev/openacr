## dev.target - Build target


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dev.Target

* file:[data/dev/target.ssim](/data/dev/target.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|target|[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)||Primary key - name of target|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Subsets
<a href="#subsets"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Subsets -->
These ssimfiles are subsets of dev.target

* [dev.Covtarget](/txt/ssimdb/dev/covtarget.md) -  
* [dev.Tgtcov](/txt/ssimdb/dev/tgtcov.md) - Captured line coverate information by target 

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Subsets -->

### Related
<a href="#related"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Related -->
These ssimfiles reference dev.target

* [dev.targdep via target](/txt/ssimdb/dev/targdep.md) - Child target 
* [dev.targdep via parent](/txt/ssimdb/dev/targdep.md) - Parent target 
* [dev.targsrc via target](/txt/ssimdb/dev/targsrc.md) - List of sources for target 
* [dev.targsyslib via target](/txt/ssimdb/dev/targsyslib.md) - Use of system library by target 

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Related -->

### Used In Commands
<a href="#used-in-commands"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:CmdlineUses -->

* [abt -target](/txt/exe/abt/README.md) - Regx of target name 
* [acr_ed -target](/txt/exe/acr_ed/README.md) - Create/Rename/Delete target 
* [amc_gc -target](/txt/exe/amc_gc/README.md) - Target to test-build 
* [atf_fuzz -target](/txt/exe/atf_fuzz/README.md) - Target to fuzz 
* [mdbg -target](/txt/exe/mdbg/README.md) - Executable name 
* [src_func -target](/txt/exe/src_func/README.md) - Visit these targets 

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:CmdlineUses -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [abt](/txt/exe/abt/internals.md) as [abt.FTarget](/txt/exe/abt/internals.md#abt-ftarget)
* [abt_md](/txt/exe/abt_md/internals.md) as [abt_md.FTarget](/txt/exe/abt_md/internals.md#abt_md-ftarget)
* [acr_ed](/txt/exe/acr_ed/internals.md) as [acr_ed.FTarget](/txt/exe/acr_ed/internals.md#acr_ed-ftarget)
* [acr_in](/txt/exe/acr_in/internals.md) as [acr_in.FTarget](/txt/exe/acr_in/internals.md#acr_in-ftarget)
* [amc](/txt/exe/amc/internals.md) as [amc.FTarget](/txt/exe/amc/internals.md#amc-ftarget)
* [atf_cov](/txt/exe/atf_cov/internals.md) as [atf_cov.FTarget](/txt/exe/atf_cov/internals.md#atf_cov-ftarget)
* [atf_fuzz](/txt/exe/atf_fuzz/internals.md) as [atf_fuzz.FTarget](/txt/exe/atf_fuzz/internals.md#atf_fuzz-ftarget)
* [src_func](/txt/exe/src_func/internals.md) as [src_func.FTarget](/txt/exe/src_func/internals.md#src_func-ftarget)
* [src_hdr](/txt/exe/src_hdr/internals.md) as [src_hdr.FTarget](/txt/exe/src_hdr/internals.md#src_hdr-ftarget)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->


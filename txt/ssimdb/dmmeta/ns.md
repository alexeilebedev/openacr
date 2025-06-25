## dmmeta.ns - Namespace (for in-memory database, protocol, etc)


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Ns

* file:[data/dmmeta/ns.ssim](/data/dmmeta/ns.ssim)

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ns|[algo.Smallstr16](/txt/protocol/algo/README.md#algo-smallstr16)|[Val](/txt/exe/amc/reftypes.md#val)||Namespace name (primary key)|
|nstype|[dmmeta.Nstype](/txt/ssimdb/dmmeta/nstype.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)||Namespace type|
|license|[dev.License](/txt/ssimdb/dev/license.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)||Associated license|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Subsets
<a href="#subsets"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Subsets -->
These ssimfiles are subsets of dmmeta.ns

* [dev.Target](/txt/ssimdb/dev/target.md) - Build target 
* [dmmeta.Main](/txt/ssimdb/dmmeta/main.md) - Generate Main function for namespace 
* [dmmeta.Nscpp](/txt/ssimdb/dmmeta/nscpp.md) - Generate C++ code for this namespace 
* [dmmeta.Nsdb](/txt/ssimdb/dmmeta/nsdb.md) - Annotate ssimdb namespaces 
* [dmmeta.Nsfast](/txt/ssimdb/dmmeta/nsfast.md) - Whether namespace is FAST 
* [dmmeta.Nsproto](/txt/ssimdb/dmmeta/nsproto.md) - Annotate protocol namespace (collection of types, no state) 
* [dmmeta.Nsversion](/txt/ssimdb/dmmeta/nsversion.md) - Namespace version 
* [dmmeta.Nsx](/txt/ssimdb/dmmeta/nsx.md) - Control code-generation and exception handling options for process/library 

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Subsets -->

### Related
<a href="#related"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Related -->
These ssimfiles reference dmmeta.ns

* [amsdb.proctype via ns](/txt/ssimdb/amsdb/proctype.md) -  
* [atfdb.comptest via target](/txt/ssimdb/atfdb/comptest.md) -  
* [atfdb.unittest via target](/txt/ssimdb/atfdb/unittest.md) -  
* [dmmeta.ctype via ns](/txt/ssimdb/dmmeta/ctype.md) - Parent namespace 
* [dmmeta.dispatch via ns](/txt/ssimdb/dmmeta/dispatch.md) - Parent namespace 
* [dmmeta.dispsig via ns](/txt/ssimdb/dmmeta/dispsig.md) - Cryptographic signature of all dispatches 
* [dmmeta.fcmdline via ns](/txt/ssimdb/dmmeta/fcmdline.md) - Parent namespace 
* [dmmeta.field via ns](/txt/ssimdb/dmmeta/field.md) - Parent namespace 
* [dmmeta.finput via ns](/txt/ssimdb/dmmeta/finput.md) - Describe input table of a program 
* [dmmeta.foutput via ns](/txt/ssimdb/dmmeta/foutput.md) - Generate function to save index/table back to disk 
* [dmmeta.fstep via ns](/txt/ssimdb/dmmeta/fstep.md) - Generate a main loop step to be executed whenever a field is non-empty 
* [dmmeta.func via ns](/txt/ssimdb/dmmeta/func.md) -  
* [dmmeta.fwddecl via ns](/txt/ssimdb/dmmeta/fwddecl.md) - Request forward declaration of a field 
* [dmmeta.gstatic via ns](/txt/ssimdb/dmmeta/gstatic.md) - Load entries for this table at startup time 
* [dmmeta.gsymbol via ns](/txt/ssimdb/dmmeta/gsymbol.md) - Namespace where new symbols will be placed 
* [dmmeta.nsinclude via ns](/txt/ssimdb/dmmeta/nsinclude.md) - Explicitly specify a C++ include file for namespace 
* [dmmeta.pnew via ns](/txt/ssimdb/dmmeta/pnew.md) - Generate custom constructor (placement new), for use with binary protocols 
* [dmmeta.ssimfile via ns](/txt/ssimdb/dmmeta/ssimfile.md) - File with ssim tuples 
* [dmmeta.tracerec via ns](/txt/ssimdb/dmmeta/tracerec.md) - e.g. rl_lib 
* [dmmeta.xref via ns](/txt/ssimdb/dmmeta/xref.md) - Ns with which this xref is a member 

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Related -->

### Used In Commands
<a href="#used-in-commands"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:CmdlineUses -->

* [abt_md -ns](/txt/exe/abt_md/README.md) - (overrides -readme) Process readmes for this namespace 
* [acr_in -ns](/txt/exe/acr_in/README.md) - Regx of matching namespace 
* [apm -ns](/txt/exe/apm/README.md) - Operate on specified namespace only 

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:CmdlineUses -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [abt](/txt/exe/abt/internals.md) as [abt.FNs](/txt/exe/abt/internals.md#abt-fns)
* [abt_md](/txt/exe/abt_md/internals.md) as [abt_md.FNs](/txt/exe/abt_md/internals.md#abt_md-fns)
* [acr_compl](/txt/exe/acr_compl/internals.md) as [acr_compl.FNs](/txt/exe/acr_compl/internals.md#acr_compl-fns)
* [acr_ed](/txt/exe/acr_ed/internals.md) as [acr_ed.FNs](/txt/exe/acr_ed/internals.md#acr_ed-fns)
* [acr_in](/txt/exe/acr_in/internals.md) as [acr_in.FNs](/txt/exe/acr_in/internals.md#acr_in-fns)
* [amc](/txt/exe/amc/internals.md) as [amc.FNs](/txt/exe/amc/internals.md#amc-fns)
* [apm](/txt/exe/apm/internals.md) as [apm.FNs](/txt/exe/apm/internals.md#apm-fns)
* [atf_ci](/txt/exe/atf_ci/internals.md) as [atf_ci.FNs](/txt/exe/atf_ci/internals.md#atf_ci-fns)
* [lib_sqlite](/txt/lib/lib_sqlite/README.md) as [lib_sqlite.FNs](/txt/lib/lib_sqlite/README.md#lib_sqlite-fns)
* [src_hdr](/txt/exe/src_hdr/internals.md) as [src_hdr.FNs](/txt/exe/src_hdr/internals.md#src_hdr-fns)
* [ssim2mysql](/txt/exe/ssim2mysql/internals.md) as [ssim2mysql.FNs](/txt/exe/ssim2mysql/internals.md#ssim2mysql-fns)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->


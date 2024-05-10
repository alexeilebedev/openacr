## dmmeta.ns - Namespace (for in-memory database, protocol, etc)


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Ns

* file:[data/dmmeta/ns.ssim](/data/dmmeta/ns.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ns|algo.Smallstr16|Val||Namespace name (primary key)|
|nstype|[dmmeta.Nstype](/txt/ssimdb/dmmeta/nstype.md)|Pkey||Namespace type|
|license|[dev.License](/txt/ssimdb/dev/license.md)|Pkey||Associated license|
|comment|algo.Comment|Val|

### Subsets
<a href="#subsets"></a>
These ssimfiles are subsets of dmmeta.ns

* [dev.Target](/txt/ssimdb/dev/target.md) - Build target 
* [dmmeta.Main](/txt/ssimdb/dmmeta/main.md) - Generate Main function for namespace 
* [dmmeta.Nscpp](/txt/ssimdb/dmmeta/nscpp.md) - Generate C++ code for this namespace 
* [dmmeta.Nsdb](/txt/ssimdb/dmmeta/nsdb.md) - Annotate ssimdb namespaces 
* [dmmeta.Nsfast](/txt/ssimdb/dmmeta/nsfast.md) - Whether namespace is FAST 
* [dmmeta.Nsproto](/txt/ssimdb/dmmeta/nsproto.md) - Annotate protocol namespace (collection of types, no state) 
* [dmmeta.Nsversion](/txt/ssimdb/dmmeta/nsversion.md) - Namespace version 
* [dmmeta.Nsx](/txt/ssimdb/dmmeta/nsx.md) - Control code-generation and exception handling options for process/library

### Related
<a href="#related"></a>
These ssimfiles reference dmmeta.ns

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

### Used In Commands
<a href="#used-in-commands"></a>
* [abt_md -ns](/txt/exe/abt_md/README.md) - (overrides -readme) Process readmes for this namespace 
* [acr_in -ns](/txt/exe/acr_in/README.md) - Regx of matching namespace 
* [apm -ns](/txt/exe/apm/README.md) - Operate on specified namespace only

### Used In Executables
<a href="#used-in-executables"></a>
* [abt](/txt/exe/abt/README.md) as abt.FNs
* [abt_md](/txt/exe/abt_md/README.md) as abt_md.FNs
* [acr_compl](/txt/exe/acr_compl/README.md) as acr_compl.FNs
* [acr_ed](/txt/exe/acr_ed/README.md) as acr_ed.FNs
* [acr_in](/txt/exe/acr_in/README.md) as acr_in.FNs
* [amc](/txt/exe/amc/README.md) as amc.FNs
* [apm](/txt/exe/apm/README.md) as apm.FNs
* [aqlite](/txt/exe/aqlite/README.md) as aqlite.FNs
* [atf_ci](/txt/exe/atf_ci/README.md) as atf_ci.FNs
* [src_hdr](/txt/exe/src_hdr/README.md) as src_hdr.FNs
* [ssim2mysql](/txt/exe/ssim2mysql/README.md) as ssim2mysql.FNs


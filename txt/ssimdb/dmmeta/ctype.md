## dmmeta.ctype - Struct


### Attributes
<a href="#attributes"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Attributes -->
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Ctype

* file:[data/dmmeta/ctype.ssim](/data/dmmeta/ctype.ssim)

italicised fields: *ns, name* are [**fldfunc**](/txt/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ctype|[algo.Smallstr100](/txt/protocol/algo/README.md#algo-smallstr100)|[Val](/txt/exe/amc/reftypes.md#val)||Identifier. must be ns.typename|
|*ns*|*[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)*|*[Pkey](/txt/exe/amc/reftypes.md#pkey)*||*Parent namespace<br>.RL of ctype*|
|*name*|*[algo.Smallstr100](/txt/protocol/algo/README.md#algo-smallstr100)*|*[Val](/txt/exe/amc/reftypes.md#val)*||*Name sans namespace<br>.RR of ctype*|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Attributes -->

### Subsets
<a href="#subsets"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Subsets -->
These ssimfiles are subsets of dmmeta.ctype

* [amcdb.Bltin](/txt/ssimdb/amcdb/bltin.md) - Specify properties of a C built-in type 
* [dmmeta.Argvtype](/txt/ssimdb/dmmeta/argvtype.md) - Customize parsing of command lines (rarely used) 
* [dmmeta.Ccmp](/txt/ssimdb/dmmeta/ccmp.md) - Generate hash function 
* [dmmeta.Cdflt](/txt/ssimdb/dmmeta/cdflt.md) - Specify default value for single-value types that lack fields 
* [dmmeta.Cextern](/txt/ssimdb/dmmeta/cextern.md) - Externally defined ctype (a struct from system header, or from a c++ library) 
* [dmmeta.Cfast](/txt/ssimdb/dmmeta/cfast.md) - FAST properties for ctype 
* [dmmeta.Cget](/txt/ssimdb/dmmeta/cget.md) - Generate state functions for these ctypes 
* [dmmeta.Chash](/txt/ssimdb/dmmeta/chash.md) - Generate hash function 
* [dmmeta.Cpptype](/txt/ssimdb/dmmeta/cpptype.md) - Specify whether a ctype can be passed by value, and other c++ options 
* [dmmeta.Csize](/txt/ssimdb/dmmeta/csize.md) - Specify size/alignment for built-in C++ types 
* [dmmeta.Cstr](/txt/ssimdb/dmmeta/cstr.md) - Specify that type behaves like a string 
* [dmmeta.Ctypelen](/txt/ssimdb/dmmeta/ctypelen.md) - Size of Ctype 
* [dmmeta.Msgtype](/txt/ssimdb/dmmeta/msgtype.md) - Specify message type for each eligible message, controls dispatch 
* [dmmeta.Nossimfile](/txt/ssimdb/dmmeta/nossimfile.md) - Indicates that ssimfile does not exist for this ssimdb ctype 
* [dmmeta.Pack](/txt/ssimdb/dmmeta/pack.md) - Request byte-packing of structure fields 
* [dmmeta.Sqltype](/txt/ssimdb/dmmeta/sqltype.md) - Mapping of ctype -> SQL expression 
* [dmmeta.Svtype](/txt/ssimdb/dmmeta/svtype.md) - Table for determining ctype from separated value file 

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Subsets -->

### Related
<a href="#related"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Related -->
These ssimfiles reference dmmeta.ctype

* [dmmeta.cafter via ctype](/txt/ssimdb/dmmeta/cafter.md) - Control amc processing order for unrelated types (used in rare situations) 
* [dmmeta.cafter via after](/txt/ssimdb/dmmeta/cafter.md) - Control amc processing order for unrelated types (used in rare situations) 
* [dmmeta.cfmt via ctype](/txt/ssimdb/dmmeta/cfmt.md) - Specify options for printing/reading ctypes into multiple formats 
* [dmmeta.dispatch_msg via ctype](/txt/ssimdb/dmmeta/dispatch_msg.md) - Add message to a dispatch 
* [dmmeta.dispctx via ctype](/txt/ssimdb/dmmeta/dispctx.md) - Use context with dispatch 
* [dmmeta.fcurs via ctype](/txt/ssimdb/dmmeta/fcurs.md) - Request generation of custom cursor 
* [dmmeta.ffast via ctype](/txt/ssimdb/dmmeta/ffast.md) - FAST properties for field 
* [dmmeta.field via ctype](/txt/ssimdb/dmmeta/field.md) - Parent structure 
* [dmmeta.field via arg](/txt/ssimdb/dmmeta/field.md) - Type of field 
* [dmmeta.floadtuples via ctype](/txt/ssimdb/dmmeta/floadtuples.md) - Request that process automatically load any input tables on startup 
* [dmmeta.func via ctype](/txt/ssimdb/dmmeta/func.md) - ** doesn't always work 
* [dmmeta.fwddecl via ctype](/txt/ssimdb/dmmeta/fwddecl.md) - Request forward declaration of a field 
* [dmmeta.gconst via namefldctyp](/txt/ssimdb/dmmeta/gconst.md) - The ctype for this gconst's name field 
* [dmmeta.gsymbol via symboltype](/txt/ssimdb/dmmeta/gsymbol.md) - Create C++ symbols from entries in ssim table 
* [dmmeta.lenfld via ctype](/txt/ssimdb/dmmeta/lenfld.md) - Name of length field 
* [dmmeta.numstr via numtype](/txt/ssimdb/dmmeta/numstr.md) - Corresponding numeric type 
* [dmmeta.pmaskfld via ctype](/txt/ssimdb/dmmeta/pmaskfld.md) - Specify which field holds presence mask bits (one for each field in struct) 
* [dmmeta.pnew via ctype](/txt/ssimdb/dmmeta/pnew.md) - Generate custom constructor (placement new), for use with binary protocols 
* [dmmeta.smallstr via ctype](/txt/ssimdb/dmmeta/smallstr.md) - Generated fixed-length padded or length-delimited string field 
* [dmmeta.ssimfile via ctype](/txt/ssimdb/dmmeta/ssimfile.md) - File with ssim tuples 
* [dmmeta.ssimreq via parent_ctype](/txt/ssimdb/dmmeta/ssimreq.md) - Extended constraints for ssim records 
* [dmmeta.typefld via ctype](/txt/ssimdb/dmmeta/typefld.md) - Name of length field 

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Related -->

### Used In Commands
<a href="#used-in-commands"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:CmdlineUses -->

* [acr_ed -ctype](/txt/exe/acr_ed/README.md) - Create/Rename/Delete ctype 
* [acr_ed -subset](/txt/exe/acr_ed/README.md) - Primary key is a subset of this ctype 
* [acr_ed -subset2](/txt/exe/acr_ed/README.md) - Primary key is also a subset of this ctype 
* [acr_ed -arg](/txt/exe/acr_ed/README.md) - Field type (e.g. u32, etc), (with -ctype) add the base field 
* [amc_vis -ctype](/txt/exe/amc_vis/README.md) - Ctype regexp to compute access path diagram 
* [atf_cmdline -dregx](/txt/exe/atf_cmdline/README.md) - Predefined regx 
* [atf_cmdline -dpkey](/txt/exe/atf_cmdline/README.md) - Predefined pkey 

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:CmdlineUses -->

### Used In Executables
<a href="#used-in-executables"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:ImdbUses -->

* [abt_md](/txt/exe/abt_md/internals.md) as [abt_md.FCtype](/txt/exe/abt_md/internals.md#abt_md-fctype)
* [acr](/txt/exe/acr/internals.md) as [acr.FCtype](/txt/exe/acr/internals.md#acr-fctype)
* [acr_compl](/txt/exe/acr_compl/internals.md) as [acr_compl.FCtype](/txt/exe/acr_compl/internals.md#acr_compl-fctype)
* [acr_ed](/txt/exe/acr_ed/internals.md) as [acr_ed.FCtype](/txt/exe/acr_ed/internals.md#acr_ed-fctype)
* [acr_in](/txt/exe/acr_in/internals.md) as [acr_in.FCtype](/txt/exe/acr_in/internals.md#acr_in-fctype)
* [amc](/txt/exe/amc/internals.md) as [amc.FCtype](/txt/exe/amc/internals.md#amc-fctype)
* [amc_vis](/txt/exe/amc_vis/internals.md) as [amc_vis.FCtype](/txt/exe/amc_vis/internals.md#amc_vis-fctype)
* [apm](/txt/exe/apm/internals.md) as [apm.FCtype](/txt/exe/apm/internals.md#apm-fctype)
* [lib_ctype](/txt/lib/lib_ctype/README.md) as [lib_ctype.FCtype](/txt/lib/lib_ctype/README.md#lib_ctype-fctype)
* [ssim2mysql](/txt/exe/ssim2mysql/internals.md) as [ssim2mysql.FCtype](/txt/exe/ssim2mysql/internals.md#ssim2mysql-fctype)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:ImdbUses -->


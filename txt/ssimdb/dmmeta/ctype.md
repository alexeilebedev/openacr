## dmmeta.ctype - Struct


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Ctype

* file:[data/dmmeta/ctype.ssim](/data/dmmeta/ctype.ssim)

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|ctype|algo.Smallstr100|Val||Identifier. must be ns.typename|
|ns|[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)|Pkey||Parent namespace<br>.RL of ctype|
|name|algo.Smallstr100|Val||Name sans namespace<br>.RR of ctype|
|comment|algo.Comment|Val|

### Subsets
<a href="#subsets"></a>
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

### Related
<a href="#related"></a>
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

### Used In Commands
<a href="#used-in-commands"></a>
* [acr_ed -ctype](/txt/exe/acr_ed/README.md) - Create/Rename/Delete ctype 
* [acr_ed -subset](/txt/exe/acr_ed/README.md) - Primary key is a subset of this ctype 
* [acr_ed -subset2](/txt/exe/acr_ed/README.md) - Primary key is also a subset of this ctype 
* [acr_ed -arg](/txt/exe/acr_ed/README.md) - Field type (e.g. u32, etc), (with -ctype) add the base field 
* [amc_vis -ctype](/txt/exe/amc_vis/README.md) - Ctype regexp to compute access path diagram 
* [atf_cmdline -dregx](/txt/exe/atf_cmdline/README.md) - Predefined regx 
* [atf_cmdline -dpkey](/txt/exe/atf_cmdline/README.md) - Predefined pkey

### Used In Executables
<a href="#used-in-executables"></a>
* [abt_md](/txt/exe/abt_md/README.md) as abt_md.FCtype
* [acr](/txt/exe/acr/README.md) as acr.FCtype
* [acr_compl](/txt/exe/acr_compl/README.md) as acr_compl.FCtype
* [acr_ed](/txt/exe/acr_ed/README.md) as acr_ed.FCtype
* [acr_in](/txt/exe/acr_in/README.md) as acr_in.FCtype
* [amc](/txt/exe/amc/README.md) as amc.FCtype
* [amc_vis](/txt/exe/amc_vis/README.md) as amc_vis.FCtype
* [apm](/txt/exe/apm/README.md) as apm.FCtype
* [lib_ctype](/txt/lib/lib_ctype/README.md) as lib_ctype.FCtype
* [ssim2mysql](/txt/exe/ssim2mysql/README.md) as ssim2mysql.FCtype


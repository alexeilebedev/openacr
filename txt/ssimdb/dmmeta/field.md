## dmmeta.field - Specify field of a struct


### Attributes
<a href="#attributes"></a>
* [ctype:](/txt/ssimdb/dmmeta/ctype.md)dmmeta.Field

* file:[data/dmmeta/field.ssim](/data/dmmeta/field.ssim)

italicised fields: *ctype, ns, name* are [**fldfunc**](/txt/openacr/ssim.md#fldfunc) fields

|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|field|[algo.Smallstr150](/txt/protocol/algo/README.md#algo-smallstr150)|[Val](/txt/exe/amc/reftype.md#val)||Primary key, as ctype.name|
|*ctype*|*[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*Parent structure<br>.RL of field*|
|*ns*|*[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)*|*[Pkey](/txt/exe/amc/reftype.md#pkey)*||*Parent namespace<br>.RL.RL of field*|
|*name*|*[algo.Smallstr100](/txt/protocol/algo/README.md#algo-smallstr100)*|*[Val](/txt/exe/amc/reftype.md#val)*||*Name of the field<br>.RR of field*|
|arg|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)||Type of field|
|reftype|[dmmeta.Reftype](/txt/ssimdb/dmmeta/reftype.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)|"Val"|Type constructor|
|dflt|[algo.CppExpr](/txt/protocol/algo/CppExpr.md)|[Val](/txt/exe/amc/reftype.md#val)||Default value (c++ expression)|
|comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftype.md#val)|||

### Subsets
<a href="#subsets"></a>
These ssimfiles are subsets of dmmeta.field

* [dev.Unstablefld](/txt/ssimdb/dev/unstablefld.md) - Fields that should be stripped from component test output because they contain timestamps etc. 
* [dmmeta.Anonfld](/txt/ssimdb/dmmeta/anonfld.md) - Omit field name where possible (command line, enums, constants) 
* [dmmeta.Basepool](/txt/ssimdb/dmmeta/basepool.md) - Specify pool to be used for allocating elements of a type 
* [dmmeta.Bitfld](/txt/ssimdb/dmmeta/bitfld.md) - Specify offset/width of a bitfield within another field 
* [dmmeta.Blkhash](/txt/ssimdb/dmmeta/blkhash.md) - Block hash index, required for fields with reftype Blkhash 
* [dmmeta.Cascdel](/txt/ssimdb/dmmeta/cascdel.md) - Request cascading delete of referenced rows 
* [dmmeta.Charset](/txt/ssimdb/dmmeta/charset.md) - Generate functions to determine if a character is a member of a set 
* [dmmeta.Cppfunc](/txt/ssimdb/dmmeta/cppfunc.md) - Value of field provided by this expression 
* [dmmeta.Falias](/txt/ssimdb/dmmeta/falias.md) - Alias field 
* [dmmeta.Fbigend](/txt/ssimdb/dmmeta/fbigend.md) - Annotate field as having big-endian storage 
* [dmmeta.Fbitset](/txt/ssimdb/dmmeta/fbitset.md) - Generate bitset functions over integer field or array 
* [dmmeta.Fbuf](/txt/ssimdb/dmmeta/fbuf.md) - Buffer for reading/writing messages, works with Iohook 
* [dmmeta.Fcast](/txt/ssimdb/dmmeta/fcast.md) - Generate implicit conversion from field to c++ expression 
* [dmmeta.Fcmp](/txt/ssimdb/dmmeta/fcmp.md) - Request versionsort or case-insensitive sort for field 
* [dmmeta.Fcompact](/txt/ssimdb/dmmeta/fcompact.md) - Request compaction upon removal from index 
* [dmmeta.Fdec](/txt/ssimdb/dmmeta/fdec.md) - Specify that field has an implied # of decimal places and specify formatting options 
* [dmmeta.Fflag](/txt/ssimdb/dmmeta/fflag.md) - Options for command-line flags 
* [dmmeta.Finput](/txt/ssimdb/dmmeta/finput.md) - Describe input table of a program 
* [dmmeta.Finsertwhen](/txt/ssimdb/dmmeta/finsertwhen.md) - Switch -<flag> to insert-completion mode when <when> flag is also set 
* [dmmeta.Fkafka](/txt/ssimdb/dmmeta/fkafka.md) -  
* [dmmeta.Fldoffset](/txt/ssimdb/dmmeta/fldoffset.md) - Assert field offset - will result in compile-time error if violated 
* [dmmeta.Floadtuples](/txt/ssimdb/dmmeta/floadtuples.md) - Request that process automatically load any input tables on startup 
* [dmmeta.Fnoremove](/txt/ssimdb/dmmeta/fnoremove.md) - Omit any functions for removing elements from table; Table is append-only 
* [dmmeta.Foutput](/txt/ssimdb/dmmeta/foutput.md) - Generate function to save index/table back to disk 
* [dmmeta.Fpbuf](/txt/ssimdb/dmmeta/fpbuf.md) -  
* [dmmeta.Fregx](/txt/ssimdb/dmmeta/fregx.md) - Specify options for command-line regx field 
* [dmmeta.Fsort](/txt/ssimdb/dmmeta/fsort.md) - Generate custom sort function for array field 
* [dmmeta.Fstep](/txt/ssimdb/dmmeta/fstep.md) - Generate a main loop step to be executed whenever a field is non-empty 
* [dmmeta.Ftrace](/txt/ssimdb/dmmeta/ftrace.md) - Generate counters for a field: allocations, buffer traffic, or step cycles 
* [dmmeta.Ftuple](/txt/ssimdb/dmmeta/ftuple.md) -  
* [dmmeta.Funique](/txt/ssimdb/dmmeta/funique.md) - This field must be unique in the table. Not needed for primary key 
* [dmmeta.Gconst](/txt/ssimdb/dmmeta/gconst.md) - Import ssim table columns as fconst for a field 
* [dmmeta.Gstatic](/txt/ssimdb/dmmeta/gstatic.md) - Load entries for this table at startup time 
* [dmmeta.Hook](/txt/ssimdb/dmmeta/hook.md) - Required on Hook fields 
* [dmmeta.Inlary](/txt/ssimdb/dmmeta/inlary.md) - Generate inline array of fixed or variable length (all entries fit within parent struct) 
* [dmmeta.Lenfld](/txt/ssimdb/dmmeta/lenfld.md) - Specify which gives length of varlen portion in bytes 
* [dmmeta.Llist](/txt/ssimdb/dmmeta/llist.md) - Options for Llist field 
* [dmmeta.Pmaskfld](/txt/ssimdb/dmmeta/pmaskfld.md) - Specify which field holds presence mask bits (one for each field in struct) 
* [dmmeta.Ptrary](/txt/ssimdb/dmmeta/ptrary.md) - Required for fields with reftype:Ptrary 
* [dmmeta.Rowid](/txt/ssimdb/dmmeta/rowid.md) - Initialize field to row id of element 
* [dmmeta.Smallstr](/txt/ssimdb/dmmeta/smallstr.md) - Generated fixed-length padded or length-delimited string field 
* [dmmeta.Sortfld](/txt/ssimdb/dmmeta/sortfld.md) - Specify what field an index (Bheap,Atree) is sorted on 
* [dmmeta.Substr](/txt/ssimdb/dmmeta/substr.md) - Specify that the field value is computed from a substring of another field 
* [dmmeta.Tary](/txt/ssimdb/dmmeta/tary.md) - Indirect linear dynamically alocated array (used for strings, arrays of data, etc) 
* [dmmeta.Thash](/txt/ssimdb/dmmeta/thash.md) - Hash index, required for fields with reftype Thash 
* [dmmeta.Typefld](/txt/ssimdb/dmmeta/typefld.md) - Specifies which field of a message carries the type 
* [dmmeta.Xref](/txt/ssimdb/dmmeta/xref.md) - Specify how to cross-reference (i.e. project, or group-by) one record with another

### Related
<a href="#related"></a>
These ssimfiles reference dmmeta.field

* [dmmeta.basepool via base](/txt/ssimdb/dmmeta/basepool.md) - Specify pool to be used for allocating elements of a type 
* [dmmeta.bitfld via srcfield](/txt/ssimdb/dmmeta/bitfld.md) - Specify offset/width of a bitfield within another field 
* [dmmeta.blkhash via hashfld](/txt/ssimdb/dmmeta/blkhash.md) - Key field; blocks are keyed on it with linfld low bits masked off 
* [dmmeta.blkhash via linfld](/txt/ssimdb/dmmeta/blkhash.md) - Dense integer component of the key; a field of hashfld's arg 
* [dmmeta.ccmdline via basecmdline](/txt/ssimdb/dmmeta/ccmdline.md) - Optional command line to merge with this one 
* [dmmeta.falias via srcfield](/txt/ssimdb/dmmeta/falias.md) - Alias field 
* [dmmeta.fbuf via pair](/txt/ssimdb/dmmeta/fbuf.md) - Read buffer this write buffer shares a descriptor with 
* [dmmeta.fcmap via leftField](/txt/ssimdb/dmmeta/fcmap.md) - Bidirectional mapping between fconst values 
* [dmmeta.fcmap via rightField](/txt/ssimdb/dmmeta/fcmap.md) - Bidirectional mapping between fconst values 
* [dmmeta.fcond via field](/txt/ssimdb/dmmeta/fcond.md) - The field whose condition is watched 
* [dmmeta.fcond via ins](/txt/ssimdb/dmmeta/fcond.md) - Index the record (or via target) enters when the condition begins to hold 
* [dmmeta.fcond via via](/txt/ssimdb/dmmeta/fcond.md) - Optional Upptr field on the watched ctype; the pointed-to record is inserted instead 
* [dmmeta.fconst via field](/txt/ssimdb/dmmeta/fconst.md) - Specify enum value (integer + string constant) for a field 
* [dmmeta.fcurs via field](/txt/ssimdb/dmmeta/fcurs.md) - Request generation of custom cursor 
* [dmmeta.ffast via field](/txt/ssimdb/dmmeta/ffast.md) - FAST properties for field 
* [dmmeta.ffunc via field](/txt/ssimdb/dmmeta/ffunc.md) - Field that this function applies to 
* [dmmeta.finsertwhen via when](/txt/ssimdb/dmmeta/finsertwhen.md) - When this flag is also set on the cmdline, use insert-completion for finsertwhen.field 
* [dmmeta.fsort via sortfld](/txt/ssimdb/dmmeta/fsort.md) - Field to sort on 
* [dmmeta.fstep via field](/txt/ssimdb/dmmeta/fstep.md) - Generate a main loop step to be executed whenever a field is non-empty 
* [dmmeta.func via field](/txt/ssimdb/dmmeta/func.md) - ** doesn't always work 
* [dmmeta.gconst via namefld](/txt/ssimdb/dmmeta/gconst.md) - Field which gives the name of the constant 
* [dmmeta.gconst via idfld](/txt/ssimdb/dmmeta/gconst.md) - Optional attribute of namefld that gives numeric ID for gconst 
* [dmmeta.nsx via pool](/txt/ssimdb/dmmeta/nsx.md) - Control code-generation and exception handling options for process/library 
* [dmmeta.pmaskfld_member via field](/txt/ssimdb/dmmeta/pmaskfld_member.md) - Explicit field-to-pmask association 
* [dmmeta.sortfld via sortfld](/txt/ssimdb/dmmeta/sortfld.md) - Field to sort by 
* [dmmeta.ssimreq via parent_field](/txt/ssimdb/dmmeta/ssimreq.md) - Extended constraints for ssim records 
* [dmmeta.ssimsort via sortfld](/txt/ssimdb/dmmeta/ssimsort.md) - Define sort order for ssimfile 
* [dmmeta.substr via srcfield](/txt/ssimdb/dmmeta/substr.md) - Specify that the field value is computed from a substring of another field 
* [dmmeta.thash via hashfld](/txt/ssimdb/dmmeta/thash.md) - Field on which hash function is computed 
* [dmmeta.xref via viafld](/txt/ssimdb/dmmeta/xref.md) - Specify how to cross-reference (i.e. project, or group-by) one record with another 
* [dmmeta.xref via keyfld](/txt/ssimdb/dmmeta/xref.md) - Specify how to cross-reference (i.e. project, or group-by) one record with another 
* [gclidb.gclicmdf2j via field](/txt/ssimdb/gclidb/gclicmdf2j.md) - Mapping of internal fields to JSON fields for gitlab/github

### Used In Commands
<a href="#used-in-commands"></a>
* [acr_ed -field](/txt/exe/acr_ed/README.md) - Create field 
* [acr_ed -before](/txt/exe/acr_ed/README.md) - Place field before this one 
* [acr_ed -srcfield](/txt/exe/acr_ed/README.md) - Source field for bitfld/substr 
* [acr_ed -hashfld](/txt/exe/acr_ed/README.md) - (-reftype:Thash) Hash field 
* [acr_ed -sortfld](/txt/exe/acr_ed/README.md) - (-reftype:Bheap) Sort field 
* [acr_ed -fstep](/txt/exe/acr_ed/README.md) - Add fstep record on existing field (use with -create)

### Used In Executables
<a href="#used-in-executables"></a>
* [abt_md](/txt/gen/abt_md/abt_md.md) as [abt_md.FField](/txt/gen/abt_md/abt_md.md#abt_md-ffield)
* [acr](/txt/gen/acr/acr.md) as [acr.FField](/txt/gen/acr/acr.md#acr-ffield)
* [acr_compl](/txt/gen/acr_compl/acr_compl.md) as [acr_compl.FField](/txt/gen/acr_compl/acr_compl.md#acr_compl-ffield)
* [acr_ed](/txt/gen/acr_ed/acr_ed.md) as [acr_ed.FField](/txt/gen/acr_ed/acr_ed.md#acr_ed-ffield)
* [acr_in](/txt/gen/acr_in/acr_in.md) as [acr_in.FField](/txt/gen/acr_in/acr_in.md#acr_in-ffield)
* [amc](/txt/gen/amc/amc.md) as [amc.FField](/txt/gen/amc/amc.md#amc-ffield)
* [amc_vis](/txt/gen/amc_vis/amc_vis.md) as [amc_vis.FField](/txt/gen/amc_vis/amc_vis.md#amc_vis-ffield)
* [apm](/txt/gen/apm/apm.md) as [apm.FField](/txt/gen/apm/apm.md#apm-ffield)
* [lib_ctype](/txt/gen/lib_ctype/lib_ctype.md) as [lib_ctype.FField](/txt/gen/lib_ctype/lib_ctype.md#lib_ctype-ffield)
* [lib_sqlite](/txt/gen/lib_sqlite/lib_sqlite.md) as [lib_sqlite.FField](/txt/gen/lib_sqlite/lib_sqlite.md#lib_sqlite-ffield)
* [ssim2mysql](/txt/gen/ssim2mysql/ssim2mysql.md) as [ssim2mysql.FField](/txt/gen/ssim2mysql/ssim2mysql.md#ssim2mysql-ffield)

## dmmeta - Tables holding metadata (data model meta)


### Chapters
<a href="#chapters"></a>

<!-- TOC_BEG AUTO -->
* [dmmeta.anonfld - Omit field name where possible (command line, enums, constants)](/txt/ssimdb/dmmeta/anonfld.md)

* [dmmeta.argvtype - Customize parsing of command lines (rarely used)](/txt/ssimdb/dmmeta/argvtype.md)

* [dmmeta.basepool - Specify pool to be used for allocating elements of a type](/txt/ssimdb/dmmeta/basepool.md)

* [dmmeta.bitfld - Specify offset/width of a bitfield within another field](/txt/ssimdb/dmmeta/bitfld.md)

* [dmmeta.cafter - Control amc processing order for unrelated types (used in rare situations)](/txt/ssimdb/dmmeta/cafter.md)

* [dmmeta.cascdel - Request cascading delete of referenced rows](/txt/ssimdb/dmmeta/cascdel.md)

* [dmmeta.ccmp - Generate hash function](/txt/ssimdb/dmmeta/ccmp.md)

* [dmmeta.cdflt - Specify default value for single-value types that lack fields](/txt/ssimdb/dmmeta/cdflt.md)

* [dmmeta.cextern - Externally defined ctype (a struct from system header, or from a c++ library)](/txt/ssimdb/dmmeta/cextern.md)

* [dmmeta.cfast - FAST properties for ctype](/txt/ssimdb/dmmeta/cfast.md)

* [dmmeta.cfmt - Specify options for printing/reading ctypes into multiple formats](/txt/ssimdb/dmmeta/cfmt.md)

* [dmmeta.cget - Generate state functions for these ctypes](/txt/ssimdb/dmmeta/cget.md)

* [dmmeta.charset - Generate functions to determine if a character is a member of a set](/txt/ssimdb/dmmeta/charset.md)

* [dmmeta.chash - Generate hash function](/txt/ssimdb/dmmeta/chash.md)

* [dmmeta.cppfunc - Value of field provided by this expression](/txt/ssimdb/dmmeta/cppfunc.md)

* [dmmeta.cppkeyword - Built-in C++ keyword](/txt/ssimdb/dmmeta/cppkeyword.md)

* [dmmeta.cpptype - Specify whether a ctype can be passed by value, and other c++ options](/txt/ssimdb/dmmeta/cpptype.md)

* [dmmeta.csize - Specify size/alignment for built-in C++ types](/txt/ssimdb/dmmeta/csize.md)

* [dmmeta.cstr - Specify that type behaves like a string](/txt/ssimdb/dmmeta/cstr.md)

* [dmmeta.ctype - Struct](/txt/ssimdb/dmmeta/ctype.md)

* [dmmeta.ctypelen - Size of Ctype](/txt/ssimdb/dmmeta/ctypelen.md)

* [dmmeta.dispatch - Generate code for a multi-way branch](/txt/ssimdb/dmmeta/dispatch.md)

* [dmmeta.dispatch_msg - Add message to a dispatch](/txt/ssimdb/dmmeta/dispatch_msg.md)

* [dmmeta.dispctx - Use context with dispatch](/txt/ssimdb/dmmeta/dispctx.md)

* [dmmeta.dispfilter - Generate filter function on dispatch](/txt/ssimdb/dmmeta/dispfilter.md)

* [dmmeta.dispsig - Cryptographic signature of all dispatches](/txt/ssimdb/dmmeta/dispsig.md)

* [dmmeta.dispsigcheck - Check signature of input data against executable's version](/txt/ssimdb/dmmeta/dispsigcheck.md)

* [dmmeta.disptrace - Generate trace fields (cycles, counts) for all dispatch branches](/txt/ssimdb/dmmeta/disptrace.md)

* [dmmeta.falias - Alias field](/txt/ssimdb/dmmeta/falias.md)

* [dmmeta.fastenc -](/txt/ssimdb/dmmeta/fastenc.md)

* [dmmeta.fastinstr -](/txt/ssimdb/dmmeta/fastinstr.md)

* [dmmeta.fastop -](/txt/ssimdb/dmmeta/fastop.md)

* [dmmeta.fbase - Customize imported Base fields](/txt/ssimdb/dmmeta/fbase.md)

* [dmmeta.fbigend - Annotate field as having big-endian storage](/txt/ssimdb/dmmeta/fbigend.md)

* [dmmeta.fbitset - Generate bitset functions over integer field or array](/txt/ssimdb/dmmeta/fbitset.md)

* [dmmeta.fbuf - Buffer for reading/writing messages, works with Iohook](/txt/ssimdb/dmmeta/fbuf.md)

* [dmmeta.fbufdir - Buffer direction (in,out)](/txt/ssimdb/dmmeta/fbufdir.md)

* [dmmeta.fbuftype - Buffer type](/txt/ssimdb/dmmeta/fbuftype.md)

* [dmmeta.fcast - Generate implicit conversion from field to c++ expression](/txt/ssimdb/dmmeta/fcast.md)

* [dmmeta.fcleanup - Request user-implemented function to be called at Uninit time for a field](/txt/ssimdb/dmmeta/fcleanup.md)

* [dmmeta.fcmap - Bidirectional mapping between fconst values](/txt/ssimdb/dmmeta/fcmap.md)

* [dmmeta.fcmdline - Annotate field that holds process command line](/txt/ssimdb/dmmeta/fcmdline.md)

* [dmmeta.fcmp - Request versionsort or case-insensitive sort for field](/txt/ssimdb/dmmeta/fcmp.md)

* [dmmeta.fcompact - Request compaction upon removal from index](/txt/ssimdb/dmmeta/fcompact.md)

* [dmmeta.fconst - Specify enum value (integer + string constant) for a field](/txt/ssimdb/dmmeta/fconst.md)

* [dmmeta.fcurs - Request generation of custom cursor](/txt/ssimdb/dmmeta/fcurs.md)

* [dmmeta.fdec - Specify that field has an implied # of decimal places and specify formatting options](/txt/ssimdb/dmmeta/fdec.md)

* [dmmeta.fdelay - Control elapsed time between execution of a step](/txt/ssimdb/dmmeta/fdelay.md)

* [dmmeta.ffast - FAST properties for field](/txt/ssimdb/dmmeta/ffast.md)

* [dmmeta.fflag - Options for command-line flags](/txt/ssimdb/dmmeta/fflag.md)

* [dmmeta.field - Specify field of a struct](/txt/ssimdb/dmmeta/field.md)

* [dmmeta.findrem - Request generation of FindRemove function](/txt/ssimdb/dmmeta/findrem.md)

* [dmmeta.finput - Describe input table of a program](/txt/ssimdb/dmmeta/finput.md)

* [dmmeta.fldoffset - Assert field offset - will result in compile-time error if violated](/txt/ssimdb/dmmeta/fldoffset.md)

* [dmmeta.floadtuples - Request that process automatically load any input tables on startup](/txt/ssimdb/dmmeta/floadtuples.md)

* [dmmeta.fnoremove - Omit any functions for removing elements from table; Table is append-only](/txt/ssimdb/dmmeta/fnoremove.md)

* [dmmeta.foutput - Generate function to save index/table back to disk](/txt/ssimdb/dmmeta/foutput.md)

* [dmmeta.fprefix - Mapping between field prefix and Reftype](/txt/ssimdb/dmmeta/fprefix.md)

* [dmmeta.fregx - Specify options for command-line regx field](/txt/ssimdb/dmmeta/fregx.md)

* [dmmeta.fsort - Generate custom sort function for array field](/txt/ssimdb/dmmeta/fsort.md)

* [dmmeta.fstep - Generate a main loop step to be executed whenever a field is non-empty](/txt/ssimdb/dmmeta/fstep.md)

* [dmmeta.ftrace - Generate cycle/step counting fields for a step](/txt/ssimdb/dmmeta/ftrace.md)

* [dmmeta.ftuple -](/txt/ssimdb/dmmeta/ftuple.md)

* [dmmeta.func -](/txt/ssimdb/dmmeta/func.md)

* [dmmeta.funique - This field must be unique in the table. Not needed for primary key](/txt/ssimdb/dmmeta/funique.md)

* [dmmeta.fuserinit - Add user-defined initialization function for field (see fcleanup)](/txt/ssimdb/dmmeta/fuserinit.md)

* [dmmeta.fwddecl - Request forward declaration of a field](/txt/ssimdb/dmmeta/fwddecl.md)

* [dmmeta.gconst - Import ssim table columns as fconst for a field](/txt/ssimdb/dmmeta/gconst.md)

* [dmmeta.gstatic - Load entries for this table at startup time](/txt/ssimdb/dmmeta/gstatic.md)

* [dmmeta.gsymbol - Create C++ symbols from entries in ssim table](/txt/ssimdb/dmmeta/gsymbol.md)

* [dmmeta.hashtype - Hash function type](/txt/ssimdb/dmmeta/hashtype.md)

* [dmmeta.hook - Required on Hook fields](/txt/ssimdb/dmmeta/hook.md)

* [dmmeta.inlary - Generate inline array of fixed or variable length (all entries fit within parent struct)](/txt/ssimdb/dmmeta/inlary.md)

* [dmmeta.lenfld - Specify which gives length of varlen portion in bytes](/txt/ssimdb/dmmeta/lenfld.md)

* [dmmeta.listtype - Specify structure of linked list based on field prefix](/txt/ssimdb/dmmeta/listtype.md)

* [dmmeta.llist - Options for Llist field](/txt/ssimdb/dmmeta/llist.md)

* [dmmeta.logcat - Log category](/txt/ssimdb/dmmeta/logcat.md)

* [dmmeta.main - Generate Main function for namespace](/txt/ssimdb/dmmeta/main.md)

* [dmmeta.msgtype - Specify message type for each eligible message, controls dispatch](/txt/ssimdb/dmmeta/msgtype.md)

* [dmmeta.nocascdel - Remove cascade delete for this index](/txt/ssimdb/dmmeta/nocascdel.md)

* [dmmeta.nossimfile - Indicates that ssimfile does not exist for this ssimdb ctype](/txt/ssimdb/dmmeta/nossimfile.md)

* [dmmeta.noxref - Explicitly specify that no x-ref exists between tables (don't use unless forced)](/txt/ssimdb/dmmeta/noxref.md)

* [dmmeta.ns - Namespace (for in-memory database, protocol, etc)](/txt/ssimdb/dmmeta/ns.md)

* [dmmeta.nscpp - Generate C++ code for this namespace](/txt/ssimdb/dmmeta/nscpp.md)

* [dmmeta.nsdb - Annotate ssimdb namespaces](/txt/ssimdb/dmmeta/nsdb.md)

* [dmmeta.nsfast - Whether namespace is FAST](/txt/ssimdb/dmmeta/nsfast.md)

* [dmmeta.nsinclude - Explicitly specify a C++ include file for namespace](/txt/ssimdb/dmmeta/nsinclude.md)

* [dmmeta.nsproto - Annotate protocol namespace (collection of types, no state)](/txt/ssimdb/dmmeta/nsproto.md)

* [dmmeta.nstype - Namespace type](/txt/ssimdb/dmmeta/nstype.md)

* [dmmeta.nsversion - Namespace version](/txt/ssimdb/dmmeta/nsversion.md)

* [dmmeta.nsx - Control code-generation and exception handling options for process/library](/txt/ssimdb/dmmeta/nsx.md)

* [dmmeta.numstr - Add functions to read numbers out of a string field](/txt/ssimdb/dmmeta/numstr.md)

* [dmmeta.pack - Request byte-packing of structure fields](/txt/ssimdb/dmmeta/pack.md)

* [dmmeta.pmaskfld - Specify which field holds presence mask bits (one for each field in struct)](/txt/ssimdb/dmmeta/pmaskfld.md)

* [dmmeta.pmaskfld_member - Explicit field-to-pmask association](/txt/ssimdb/dmmeta/pmaskfld_member.md)

* [dmmeta.pnew - Generate custom constructor (placement new), for use with binary protocols](/txt/ssimdb/dmmeta/pnew.md)

* [dmmeta.pnewtype - Type of fbuf](/txt/ssimdb/dmmeta/pnewtype.md)

* [dmmeta.printfmt - Print format](/txt/ssimdb/dmmeta/printfmt.md)

* [dmmeta.ptrary - Required for fields with reftype:Ptrary](/txt/ssimdb/dmmeta/ptrary.md)

* [dmmeta.reftype - Field type constructor (e.g. reference type)](/txt/ssimdb/dmmeta/reftype.md)

* [dmmeta.rowid - Initialize field to row id of element](/txt/ssimdb/dmmeta/rowid.md)

* [dmmeta.smallstr - Generated fixed-length padded or length-delimited string field](/txt/ssimdb/dmmeta/smallstr.md)

* [dmmeta.sortfld - Specify what field an index (Bheap,Atree) is sorted on](/txt/ssimdb/dmmeta/sortfld.md)

* [dmmeta.sorttype - Sort type](/txt/ssimdb/dmmeta/sorttype.md)

* [dmmeta.sqltype - Mapping of ctype -> SQL expression](/txt/ssimdb/dmmeta/sqltype.md)

* [dmmeta.ssimfile - File with ssim tuples](/txt/ssimdb/dmmeta/ssimfile.md)

* [dmmeta.ssimreq - Extended constraints for ssim records](/txt/ssimdb/dmmeta/ssimreq.md)

* [dmmeta.ssimsort - Define sort order for ssimfile](/txt/ssimdb/dmmeta/ssimsort.md)

* [dmmeta.ssimvolatile - Types based on ssim files marked this way cannot be cross-referenced](/txt/ssimdb/dmmeta/ssimvolatile.md)

* [dmmeta.steptype - Type of scheduler step](/txt/ssimdb/dmmeta/steptype.md)

* [dmmeta.strfmt - Cfmt conversion type (misnomer, not necessarily a string)](/txt/ssimdb/dmmeta/strfmt.md)

* [dmmeta.strtype - Type of small string](/txt/ssimdb/dmmeta/strtype.md)

* [dmmeta.substr - Specify that the field value is computed from a substring of another field](/txt/ssimdb/dmmeta/substr.md)

* [dmmeta.svtype - Table for determining ctype from separated value file](/txt/ssimdb/dmmeta/svtype.md)

* [dmmeta.tary - Indirect linear dynamically alocated array (used for strings, arrays of data, etc)](/txt/ssimdb/dmmeta/tary.md)

* [dmmeta.thash - Hash index, required for fields with reftype Thash](/txt/ssimdb/dmmeta/thash.md)

* [dmmeta.tracefld - Trace fields](/txt/ssimdb/dmmeta/tracefld.md)

* [dmmeta.tracerec - Trace record](/txt/ssimdb/dmmeta/tracerec.md)

* [dmmeta.typefld - Specifies which field of a message carries the type](/txt/ssimdb/dmmeta/typefld.md)

* [dmmeta.usertracefld - Add custom user trace fields to process's trace struct](/txt/ssimdb/dmmeta/usertracefld.md)

* [dmmeta.xref - Specify how to cross-reference (i.e. project, or group-by) one record with another](/txt/ssimdb/dmmeta/xref.md)

<!-- TOC_END AUTO -->

### Table Of Contents
<a href="#table-of-contents"></a>
* [Chapters](#chapters)


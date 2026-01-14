## dmmeta - Tables holding metadata (data model meta)


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Toc -->
<!-- dev.mdmark  mdmark:TOC  state:BEG_AUTO  param:Toc -->
&#128196; [dmmeta.anonfld - Omit field name where possible (command line, enums, constants)](/txt/ssimdb/dmmeta/anonfld.md)<br/>
&#128196; [dmmeta.argvtype - Customize parsing of command lines (rarely used)](/txt/ssimdb/dmmeta/argvtype.md)<br/>
&#128196; [dmmeta.basepool - Specify pool to be used for allocating elements of a type](/txt/ssimdb/dmmeta/basepool.md)<br/>
&#128196; [dmmeta.bitfld - Specify offset/width of a bitfield within another field](/txt/ssimdb/dmmeta/bitfld.md)<br/>
&#128196; [dmmeta.cafter - Control amc processing order for unrelated types (used in rare situations)](/txt/ssimdb/dmmeta/cafter.md)<br/>
&#128196; [dmmeta.cascdel - Request cascading delete of referenced rows](/txt/ssimdb/dmmeta/cascdel.md)<br/>
&#128196; [dmmeta.ccmp - Generate hash function](/txt/ssimdb/dmmeta/ccmp.md)<br/>
&#128196; [dmmeta.cdflt - Specify default value for single-value types that lack fields](/txt/ssimdb/dmmeta/cdflt.md)<br/>
&#128196; [dmmeta.cextern - Externally defined ctype (a struct from system header, or from a c++ library)](/txt/ssimdb/dmmeta/cextern.md)<br/>
&#128196; [dmmeta.cfast - FAST properties for ctype](/txt/ssimdb/dmmeta/cfast.md)<br/>
&#128196; [dmmeta.cfmt - Specify options for printing/reading ctypes into multiple formats](/txt/ssimdb/dmmeta/cfmt.md)<br/>
&#128196; [dmmeta.cget - Generate state functions for these ctypes](/txt/ssimdb/dmmeta/cget.md)<br/>
&#128196; [dmmeta.charset - Generate functions to determine if a character is a member of a set](/txt/ssimdb/dmmeta/charset.md)<br/>
&#128196; [dmmeta.chash - Generate hash function](/txt/ssimdb/dmmeta/chash.md)<br/>
&#128196; [dmmeta.ckafka -](/txt/ssimdb/dmmeta/ckafka.md)<br/>
&#128196; [dmmeta.cppfunc - Value of field provided by this expression](/txt/ssimdb/dmmeta/cppfunc.md)<br/>
&#128196; [dmmeta.cppkeyword - Built-in C++ keyword](/txt/ssimdb/dmmeta/cppkeyword.md)<br/>
&#128196; [dmmeta.cpptype - Specify whether a ctype can be passed by value, and other c++ options](/txt/ssimdb/dmmeta/cpptype.md)<br/>
&#128196; [dmmeta.csize - Specify size/alignment for built-in C++ types](/txt/ssimdb/dmmeta/csize.md)<br/>
&#128196; [dmmeta.cstr - Specify that type behaves like a string](/txt/ssimdb/dmmeta/cstr.md)<br/>
&#128196; [dmmeta.ctype - Struct](/txt/ssimdb/dmmeta/ctype.md)<br/>
&#128196; [dmmeta.ctypelen - Size of Ctype](/txt/ssimdb/dmmeta/ctypelen.md)<br/>
&#128196; [dmmeta.dispatch - Generate code for a multi-way branch](/txt/ssimdb/dmmeta/dispatch.md)<br/>
&#128196; [dmmeta.dispatch_msg - Add message to a dispatch](/txt/ssimdb/dmmeta/dispatch_msg.md)<br/>
&#128196; [dmmeta.dispctx - Use context with dispatch](/txt/ssimdb/dmmeta/dispctx.md)<br/>
&#128196; [dmmeta.dispfilter - Generate filter function on dispatch](/txt/ssimdb/dmmeta/dispfilter.md)<br/>
&#128196; [dmmeta.dispsig - Cryptographic signature of all dispatches](/txt/ssimdb/dmmeta/dispsig.md)<br/>
&#128196; [dmmeta.dispsigcheck - Check signature of input data against executable's version](/txt/ssimdb/dmmeta/dispsigcheck.md)<br/>
&#128196; [dmmeta.disptrace - Generate trace fields (cycles, counts) for all dispatch branches](/txt/ssimdb/dmmeta/disptrace.md)<br/>
&#128196; [dmmeta.falias - Alias field](/txt/ssimdb/dmmeta/falias.md)<br/>
&#128196; [dmmeta.fastenc -](/txt/ssimdb/dmmeta/fastenc.md)<br/>
&#128196; [dmmeta.fastinstr -](/txt/ssimdb/dmmeta/fastinstr.md)<br/>
&#128196; [dmmeta.fastop -](/txt/ssimdb/dmmeta/fastop.md)<br/>
&#128196; [dmmeta.fbase - Customize imported Base fields](/txt/ssimdb/dmmeta/fbase.md)<br/>
&#128196; [dmmeta.fbigend - Annotate field as having big-endian storage](/txt/ssimdb/dmmeta/fbigend.md)<br/>
&#128196; [dmmeta.fbitset - Generate bitset functions over integer field or array](/txt/ssimdb/dmmeta/fbitset.md)<br/>
&#128196; [dmmeta.fbuf - Buffer for reading/writing messages, works with Iohook](/txt/ssimdb/dmmeta/fbuf.md)<br/>
&#128196; [dmmeta.fbufdir - Buffer direction (in,out)](/txt/ssimdb/dmmeta/fbufdir.md)<br/>
&#128196; [dmmeta.fbufiotype - Underlying IO type](/txt/ssimdb/dmmeta/fbufiotype.md)<br/>
&#128196; [dmmeta.fbuftype - Buffer type](/txt/ssimdb/dmmeta/fbuftype.md)<br/>
&#128196; [dmmeta.fcast - Generate implicit conversion from field to c++ expression](/txt/ssimdb/dmmeta/fcast.md)<br/>
&#128196; [dmmeta.fcleanup - Request user-implemented function to be called at Uninit time for a field](/txt/ssimdb/dmmeta/fcleanup.md)<br/>
&#128196; [dmmeta.fcmap - Bidirectional mapping between fconst values](/txt/ssimdb/dmmeta/fcmap.md)<br/>
&#128196; [dmmeta.fcmdline - Annotate field that holds process command line](/txt/ssimdb/dmmeta/fcmdline.md)<br/>
&#128196; [dmmeta.fcmp - Request versionsort or case-insensitive sort for field](/txt/ssimdb/dmmeta/fcmp.md)<br/>
&#128196; [dmmeta.fcompact - Request compaction upon removal from index](/txt/ssimdb/dmmeta/fcompact.md)<br/>
&#128196; [dmmeta.fconst - Specify enum value (integer + string constant) for a field](/txt/ssimdb/dmmeta/fconst.md)<br/>
&#128196; [dmmeta.fcurs - Request generation of custom cursor](/txt/ssimdb/dmmeta/fcurs.md)<br/>
&#128196; [dmmeta.fdec - Specify that field has an implied # of decimal places and specify formatting options](/txt/ssimdb/dmmeta/fdec.md)<br/>
&#128196; [dmmeta.fdelay - Control elapsed time between execution of a step](/txt/ssimdb/dmmeta/fdelay.md)<br/>
&#128196; [dmmeta.ffast - FAST properties for field](/txt/ssimdb/dmmeta/ffast.md)<br/>
&#128196; [dmmeta.fflag - Options for command-line flags](/txt/ssimdb/dmmeta/fflag.md)<br/>
&#128196; [dmmeta.field - Specify field of a struct](/txt/ssimdb/dmmeta/field.md)<br/>
&#128196; [dmmeta.findrem - Request generation of FindRemove function](/txt/ssimdb/dmmeta/findrem.md)<br/>
&#128196; [dmmeta.finput - Describe input table of a program](/txt/ssimdb/dmmeta/finput.md)<br/>
&#128196; [dmmeta.fkafka -](/txt/ssimdb/dmmeta/fkafka.md)<br/>
&#128196; [dmmeta.fldoffset - Assert field offset - will result in compile-time error if violated](/txt/ssimdb/dmmeta/fldoffset.md)<br/>
&#128196; [dmmeta.floadtuples - Request that process automatically load any input tables on startup](/txt/ssimdb/dmmeta/floadtuples.md)<br/>
&#128196; [dmmeta.fnoremove - Omit any functions for removing elements from table; Table is append-only](/txt/ssimdb/dmmeta/fnoremove.md)<br/>
&#128196; [dmmeta.foutput - Generate function to save index/table back to disk](/txt/ssimdb/dmmeta/foutput.md)<br/>
&#128196; [dmmeta.fprefix - Mapping between field prefix and Reftype](/txt/ssimdb/dmmeta/fprefix.md)<br/>
&#128196; [dmmeta.fregx - Specify options for command-line regx field](/txt/ssimdb/dmmeta/fregx.md)<br/>
&#128196; [dmmeta.fsort - Generate custom sort function for array field](/txt/ssimdb/dmmeta/fsort.md)<br/>
&#128196; [dmmeta.fstep - Generate a main loop step to be executed whenever a field is non-empty](/txt/ssimdb/dmmeta/fstep.md)<br/>
&#128196; [dmmeta.ftrace - Generate cycle/step counting fields for a step](/txt/ssimdb/dmmeta/ftrace.md)<br/>
&#128196; [dmmeta.ftuple -](/txt/ssimdb/dmmeta/ftuple.md)<br/>
&#128196; [dmmeta.func -](/txt/ssimdb/dmmeta/func.md)<br/>
&#128196; [dmmeta.funique - This field must be unique in the table. Not needed for primary key](/txt/ssimdb/dmmeta/funique.md)<br/>
&#128196; [dmmeta.fuserinit - Add user-defined initialization function for field (see fcleanup)](/txt/ssimdb/dmmeta/fuserinit.md)<br/>
&#128196; [dmmeta.fwddecl - Request forward declaration of a field](/txt/ssimdb/dmmeta/fwddecl.md)<br/>
&#128196; [dmmeta.gconst - Import ssim table columns as fconst for a field](/txt/ssimdb/dmmeta/gconst.md)<br/>
&#128196; [dmmeta.gstatic - Load entries for this table at startup time](/txt/ssimdb/dmmeta/gstatic.md)<br/>
&#128196; [dmmeta.gsymbol - Create C++ symbols from entries in ssim table](/txt/ssimdb/dmmeta/gsymbol.md)<br/>
&#128196; [dmmeta.hashtype - Hash function type](/txt/ssimdb/dmmeta/hashtype.md)<br/>
&#128196; [dmmeta.hook - Required on Hook fields](/txt/ssimdb/dmmeta/hook.md)<br/>
&#128196; [dmmeta.inlary - Generate inline array of fixed or variable length (all entries fit within parent struct)](/txt/ssimdb/dmmeta/inlary.md)<br/>
&#128196; [dmmeta.jstype -](/txt/ssimdb/dmmeta/jstype.md)<br/>
&#128196; [dmmeta.kafka_type_kind -](/txt/ssimdb/dmmeta/kafka_type_kind.md)<br/>
&#128196; [dmmeta.lenfld - Specify which gives length of varlen portion in bytes](/txt/ssimdb/dmmeta/lenfld.md)<br/>
&#128196; [dmmeta.listtype - Specify structure of linked list based on field prefix](/txt/ssimdb/dmmeta/listtype.md)<br/>
&#128196; [dmmeta.llist - Options for Llist field](/txt/ssimdb/dmmeta/llist.md)<br/>
&#128196; [dmmeta.logcat - Log category](/txt/ssimdb/dmmeta/logcat.md)<br/>
&#128196; [dmmeta.main - Generate Main function for namespace](/txt/ssimdb/dmmeta/main.md)<br/>
&#128196; [dmmeta.msgtype - Specify message type for each eligible message, controls dispatch](/txt/ssimdb/dmmeta/msgtype.md)<br/>
&#128196; [dmmeta.nocascdel - Remove cascade delete for this index](/txt/ssimdb/dmmeta/nocascdel.md)<br/>
&#128196; [dmmeta.nossimfile - Indicates that ssimfile does not exist for this ssimdb ctype](/txt/ssimdb/dmmeta/nossimfile.md)<br/>
&#128196; [dmmeta.noxref - Explicitly specify that no x-ref exists between tables (don't use unless forced)](/txt/ssimdb/dmmeta/noxref.md)<br/>
&#128196; [dmmeta.ns - Namespace (for in-memory database, protocol, etc)](/txt/ssimdb/dmmeta/ns.md)<br/>
&#128196; [dmmeta.nscpp - Generate C++ code for this namespace](/txt/ssimdb/dmmeta/nscpp.md)<br/>
&#128196; [dmmeta.nsdb - Annotate ssimdb namespaces](/txt/ssimdb/dmmeta/nsdb.md)<br/>
&#128196; [dmmeta.nsfast - Whether namespace is FAST](/txt/ssimdb/dmmeta/nsfast.md)<br/>
&#128196; [dmmeta.nsinclude - Explicitly specify a C++ include file for namespace](/txt/ssimdb/dmmeta/nsinclude.md)<br/>
&#128196; [dmmeta.nsjs - Generate JavaScript code for this namespace](/txt/ssimdb/dmmeta/nsjs.md)<br/>
&#128196; [dmmeta.nsproto - Annotate protocol namespace (collection of types, no state)](/txt/ssimdb/dmmeta/nsproto.md)<br/>
&#128196; [dmmeta.nstype - Namespace type](/txt/ssimdb/dmmeta/nstype.md)<br/>
&#128196; [dmmeta.nsversion - Namespace version](/txt/ssimdb/dmmeta/nsversion.md)<br/>
&#128196; [dmmeta.nsx - Control code-generation and exception handling options for process/library](/txt/ssimdb/dmmeta/nsx.md)<br/>
&#128196; [dmmeta.numstr - Add functions to read numbers out of a string field](/txt/ssimdb/dmmeta/numstr.md)<br/>
&#128196; [dmmeta.pack - Request byte-packing of structure fields](/txt/ssimdb/dmmeta/pack.md)<br/>
&#128196; [dmmeta.pmaskfld - Specify which field holds presence mask bits (one for each field in struct)](/txt/ssimdb/dmmeta/pmaskfld.md)<br/>
&#128196; [dmmeta.pmaskfld_member - Explicit field-to-pmask association](/txt/ssimdb/dmmeta/pmaskfld_member.md)<br/>
&#128196; [dmmeta.pnew - Generate custom constructor (placement new), for use with binary protocols](/txt/ssimdb/dmmeta/pnew.md)<br/>
&#128196; [dmmeta.pnewtype - Type of fbuf](/txt/ssimdb/dmmeta/pnewtype.md)<br/>
&#128196; [dmmeta.printfmt - Print format](/txt/ssimdb/dmmeta/printfmt.md)<br/>
&#128196; [dmmeta.ptrary - Required for fields with reftype:Ptrary](/txt/ssimdb/dmmeta/ptrary.md)<br/>
&#128196; [dmmeta.reftype - Field type constructor (e.g. reference type)](/txt/ssimdb/dmmeta/reftype.md)<br/>
&#128196; [dmmeta.rowid - Initialize field to row id of element](/txt/ssimdb/dmmeta/rowid.md)<br/>
&#128196; [dmmeta.smallstr - Generated fixed-length padded or length-delimited string field](/txt/ssimdb/dmmeta/smallstr.md)<br/>
&#128196; [dmmeta.sortfld - Specify what field an index (Bheap,Atree) is sorted on](/txt/ssimdb/dmmeta/sortfld.md)<br/>
&#128196; [dmmeta.sorttype - Sort type](/txt/ssimdb/dmmeta/sorttype.md)<br/>
&#128196; [dmmeta.sqltype - Mapping of ctype -> SQL expression](/txt/ssimdb/dmmeta/sqltype.md)<br/>
&#128196; [dmmeta.ssimfile - File with ssim tuples](/txt/ssimdb/dmmeta/ssimfile.md)<br/>
&#128196; [dmmeta.ssimreq - Extended constraints for ssim records](/txt/ssimdb/dmmeta/ssimreq.md)<br/>
&#128196; [dmmeta.ssimsort - Define sort order for ssimfile](/txt/ssimdb/dmmeta/ssimsort.md)<br/>
&#128196; [dmmeta.ssimvolatile - Types based on ssim files marked this way cannot be cross-referenced](/txt/ssimdb/dmmeta/ssimvolatile.md)<br/>
&#128196; [dmmeta.steptype - Type of scheduler step](/txt/ssimdb/dmmeta/steptype.md)<br/>
&#128196; [dmmeta.strfmt - Cfmt conversion type (misnomer, not necessarily a string)](/txt/ssimdb/dmmeta/strfmt.md)<br/>
&#128196; [dmmeta.strtype - Type of small string](/txt/ssimdb/dmmeta/strtype.md)<br/>
&#128196; [dmmeta.substr - Specify that the field value is computed from a substring of another field](/txt/ssimdb/dmmeta/substr.md)<br/>
&#128196; [dmmeta.svtype - Table for determining ctype from separated value file](/txt/ssimdb/dmmeta/svtype.md)<br/>
&#128196; [dmmeta.tary - Indirect linear dynamically alocated array (used for strings, arrays of data, etc)](/txt/ssimdb/dmmeta/tary.md)<br/>
&#128196; [dmmeta.thash - Hash index, required for fields with reftype Thash](/txt/ssimdb/dmmeta/thash.md)<br/>
&#128196; [dmmeta.tracefld - Trace fields](/txt/ssimdb/dmmeta/tracefld.md)<br/>
&#128196; [dmmeta.tracerec - Trace record](/txt/ssimdb/dmmeta/tracerec.md)<br/>
&#128196; [dmmeta.typefld - Specifies which field of a message carries the type](/txt/ssimdb/dmmeta/typefld.md)<br/>
&#128196; [dmmeta.userfunc -](/txt/ssimdb/dmmeta/userfunc.md)<br/>
&#128196; [dmmeta.usertracefld - Add custom user trace fields to process's trace struct](/txt/ssimdb/dmmeta/usertracefld.md)<br/>
&#128196; [dmmeta.xref - Specify how to cross-reference (i.e. project, or group-by) one record with another](/txt/ssimdb/dmmeta/xref.md)<br/>

<!-- dev.mdmark  mdmark:TOC  state:END_AUTO  param:Toc -->

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Toc -->


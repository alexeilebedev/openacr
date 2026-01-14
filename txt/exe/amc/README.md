## amc - Algo Model Compiler: generate code under include/gen and cpp/gen
<a href="#amc"></a>
For the philosophy behind the generative approach, and an explanation
of concepts, see [Intro](/txt/exe/amc/intro.md)

### Table Of Contents
<a href="#table-of-contents"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Toc -->
&nbsp;&nbsp;&bull;&nbsp;  [Syntax](#syntax)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Introduction](#introduction)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Main Input Tables](#main-input-tables)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Size and field offset assertions](#size-and-field-offset-assertions)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Options](#options)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
&#128196; [amc Enumerated Types](/txt/exe/amc/enum.md)<br/>
&#128196; [amc Subprocess Invocation](/txt/exe/amc/exec.md)<br/>
&#128196; [amc - fbuf](/txt/exe/amc/fbuf.md)<br/>
&#128196; [amc - Internals](/txt/exe/amc/internals.md)<br/>
&#128196; [amc Intro](/txt/exe/amc/intro.md)<br/>
&#128196; [amc Presence Masks](/txt/exe/amc/pmask.md)<br/>
&#128196; [amc Memory Pools](/txt/exe/amc/pools.md)<br/>
&#128196; [amc Protocols](/txt/exe/amc/proto.md)<br/>
&#128196; [amc Reflection](/txt/exe/amc/reflection.md)<br/>
&#128196; [amc Reftypes](/txt/exe/amc/reftypes.md)<br/>
&#128196; [amc release notes](/txt/exe/amc/relnotes.md)<br/>
&#128196; [amc Runtime](/txt/exe/amc/runtime.md)<br/>
&#128196; [amc Strings](/txt/exe/amc/strings.md)<br/>
&#128196; [amc Cross-References (xrefs)](/txt/exe/amc/xref.md)<br/>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Toc -->

### Syntax
<a href="#syntax"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Syntax -->
```
amc: Algo Model Compiler: generate code under include/gen and cpp/gen
Usage: amc [[-query:]<string>] [options]
    OPTION        TYPE    DFLT    COMMENT
    -in_dir       string  "data"  Root of input ssim dir
    [query]       string  ""      Query mode: generate code for specified object
    -out_dir      string  "."     Root of output cpp dir
    -proto                        Print prototype
    -showcomment          Y       Show generated comments
    -report               Y       Final report
    -e                            Open matching records in editor
    -trace        regx    ""      Regx of something to trace code generation
    -verbose      flag            Verbosity level (0..255); alias -v; cumulative
    -debug        flag            Debug level (0..255); alias -d; cumulative
    -help                         Print help and exit; alias -h
    -version                      Print version and exit
    -signature                    Show signatures and exit; alias -sig

```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Syntax -->

### Introduction
<a href="#introduction"></a>

Amc is an extensible generator of source code from relational tables.

Amc reads ssim tables, creates C++ in-memory database code as determined by these tables,
and outputs .h and .cpp files.
By default, ssim tables are loaded from directory "data", and output
is written to directories `cpp/gen` and `include/gen`. With option `-in_dir`,
the location of the input dataset can be specified. 
With option `-out_dir`, the location of output directory can be specified. The default
is `.`.

To run `amc`, just type `amc`. All of its inputs come from ssimfiles, so no arguments
are necessary. The default invocation prints something like this:

    $ amc
    report.amc  n_cppfile:123  n_cppline:258301  n_ctype:970  n_func:22524  n_xref:460  n_filemod:0

With respect to generator performance, `amc` process generates about 1M LOC per second.
Of course, this performance is not reflected in
the final executable, which means that adding new checks or generator features to amc is effectively free.

Amc outputs are versioned in git, so we can trace the origin
of any change (with git annotate), and continue to make changes to amc
without breaking existing code.

To run amc in a sandbox and see what it produced, use
```
    sandbox amc -reset -diff -- amc
```

This can be very useful when modifying amc itself, since introducing an error into a generator
phase may break compiling of amc and thus prevent fixing of such error without resorting to
`git reset --hard`.

### Main Input Tables
<a href="#main-input-tables"></a>

The main tables Amc uses as input are ns, ctype and field.
Ns maps to a C++ namespace.
Ctype corresponds to a struct.
Field corresponds to a struct member or some derived value.

By adding rows to these three tables, you can generate code for libraries, executables and protocols.
The remaining tables are used to elaborate and customize the ns/ctype/field skeleton.

### Size and field offset assertions
<a href="#size-and-field-offset-assertions"></a>

`amc` offers several mechanisms that help cross-reference a protocol description
that has been converted to ssim records with the intended original.
They are
* [csize](/txt/ssimdb/dmmeta/csize.md)
* [fldoffset](/txt/ssimdb/dmmeta/fldoffset.md)
* [ctypelen](/txt/ssimdb/dmmeta/ctypelen.md)

### Options
<a href="#options"></a>

<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Options -->
#### -in_dir -- Root of input ssim dir
<a href="#-in_dir"></a>

#### -query -- Query mode: generate code for specified object
<a href="#-query"></a>

When amc is given this argument, it runs in query mode.
Instead of modifying source files it simply prints to stdout all internal code
sections whose key matches the specified regex (typically it's a ctype name or a function name).
This is the fastest way to check how a certain function is generated

Example: output all functions which are used to compile amc itself
    amc amc.%

#### -out_dir -- Root of output cpp dir
<a href="#-out_dir"></a>

#### -proto -- Print prototype
<a href="#-proto"></a>
Used with `-query`, limits the output to just function prototypes

#### -showcomment -- Show generated comments
<a href="#-showcomment"></a>

#### -report -- Final report
<a href="#-report"></a>

#### -e -- Open matching records in editor
<a href="#-e"></a>

#### -trace -- Regx of something to trace code generation
<a href="#-trace"></a>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Options -->

### Inputs
<a href="#inputs"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Inputs -->
`amc` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|
|[dmmeta.anonfld](/txt/ssimdb/dmmeta/anonfld.md)|Omit field name where possible (command line, enums, constants)|
|[dmmeta.argvtype](/txt/ssimdb/dmmeta/argvtype.md)|Customize parsing of command lines (rarely used)|
|[dmmeta.basepool](/txt/ssimdb/dmmeta/basepool.md)|Specify pool to be used for allocating elements of a type|
|[dmmeta.bitfld](/txt/ssimdb/dmmeta/bitfld.md)|Specify offset/width of a bitfield within another field|
|[amcdb.bltin](/txt/ssimdb/amcdb/bltin.md)|Specify properties of a C built-in type|
|[dmmeta.cafter](/txt/ssimdb/dmmeta/cafter.md)|Control amc processing order for unrelated types (used in rare situations)|
|[dmmeta.cascdel](/txt/ssimdb/dmmeta/cascdel.md)|Request cascading delete of referenced rows|
|[dmmeta.ccmp](/txt/ssimdb/dmmeta/ccmp.md)|Generate hash function|
|[dmmeta.cdflt](/txt/ssimdb/dmmeta/cdflt.md)|Specify default value for single-value types that lack fields|
|[dmmeta.cextern](/txt/ssimdb/dmmeta/cextern.md)|Externally defined ctype (a struct from system header, or from a c++ library)|
|[dmmeta.cfast](/txt/ssimdb/dmmeta/cfast.md)|FAST properties for ctype|
|[dmmeta.cfmt](/txt/ssimdb/dmmeta/cfmt.md)|Specify options for printing/reading ctypes into multiple formats|
|[dmmeta.cget](/txt/ssimdb/dmmeta/cget.md)|Generate state functions for these ctypes|
|[dmmeta.charset](/txt/ssimdb/dmmeta/charset.md)|Generate functions to determine if a character is a member of a set|
|[dmmeta.chash](/txt/ssimdb/dmmeta/chash.md)|Generate hash function|
|[dmmeta.ckafka](/txt/ssimdb/dmmeta/ckafka.md)||
|[dmmeta.cppfunc](/txt/ssimdb/dmmeta/cppfunc.md)|Value of field provided by this expression|
|[dmmeta.cpptype](/txt/ssimdb/dmmeta/cpptype.md)|Specify whether a ctype can be passed by value, and other c++ options|
|[dmmeta.csize](/txt/ssimdb/dmmeta/csize.md)|Specify size/alignment for built-in C++ types|
|[dmmeta.cstr](/txt/ssimdb/dmmeta/cstr.md)|Specify that type behaves like a string|
|[dmmeta.ctype](/txt/ssimdb/dmmeta/ctype.md)|Struct|
|[dmmeta.dispatch](/txt/ssimdb/dmmeta/dispatch.md)|Generate code for a multi-way branch|
|[dmmeta.dispatch_msg](/txt/ssimdb/dmmeta/dispatch_msg.md)|Add message to a dispatch|
|[dmmeta.dispctx](/txt/ssimdb/dmmeta/dispctx.md)|Use context with dispatch|
|[dmmeta.dispfilter](/txt/ssimdb/dmmeta/dispfilter.md)|Generate filter function on dispatch|
|[dmmeta.disptrace](/txt/ssimdb/dmmeta/disptrace.md)|Generate trace fields (cycles, counts) for all dispatch branches|
|[dmmeta.falias](/txt/ssimdb/dmmeta/falias.md)|Alias field|
|[dmmeta.fbase](/txt/ssimdb/dmmeta/fbase.md)|Customize imported Base fields|
|[dmmeta.fbigend](/txt/ssimdb/dmmeta/fbigend.md)|Annotate field as having big-endian storage|
|[dmmeta.fbitset](/txt/ssimdb/dmmeta/fbitset.md)|Generate bitset functions over integer field or array|
|[dmmeta.fbuf](/txt/ssimdb/dmmeta/fbuf.md)|Buffer for reading/writing messages, works with Iohook|
|[dmmeta.fbuftype](/txt/ssimdb/dmmeta/fbuftype.md)|Buffer type|
|[dmmeta.fcast](/txt/ssimdb/dmmeta/fcast.md)|Generate implicit conversion from field to c++ expression|
|[dmmeta.fcleanup](/txt/ssimdb/dmmeta/fcleanup.md)|Request user-implemented function to be called at Uninit time for a field|
|[dmmeta.fcmap](/txt/ssimdb/dmmeta/fcmap.md)|Bidirectional mapping between fconst values|
|[dmmeta.fcmdline](/txt/ssimdb/dmmeta/fcmdline.md)|Annotate field that holds process command line|
|[dmmeta.fcmp](/txt/ssimdb/dmmeta/fcmp.md)|Request versionsort or case-insensitive sort for field|
|[dmmeta.fcompact](/txt/ssimdb/dmmeta/fcompact.md)|Request compaction upon removal from index|
|[dmmeta.fconst](/txt/ssimdb/dmmeta/fconst.md)|Specify enum value (integer + string constant) for a field|
|[dmmeta.fcurs](/txt/ssimdb/dmmeta/fcurs.md)|Request generation of custom cursor|
|[dmmeta.fdec](/txt/ssimdb/dmmeta/fdec.md)|Specify that field has an implied # of decimal places and specify formatting options|
|[dmmeta.fdelay](/txt/ssimdb/dmmeta/fdelay.md)|Control elapsed time between execution of a step|
|[dmmeta.ffast](/txt/ssimdb/dmmeta/ffast.md)|FAST properties for field|
|[dmmeta.fflag](/txt/ssimdb/dmmeta/fflag.md)|Options for command-line flags|
|[dmmeta.field](/txt/ssimdb/dmmeta/field.md)|Specify field of a struct|
|[dmmeta.findrem](/txt/ssimdb/dmmeta/findrem.md)|Request generation of FindRemove function|
|[dmmeta.finput](/txt/ssimdb/dmmeta/finput.md)|Describe input table of a program|
|[dmmeta.fkafka](/txt/ssimdb/dmmeta/fkafka.md)||
|[dmmeta.fldoffset](/txt/ssimdb/dmmeta/fldoffset.md)|Assert field offset - will result in compile-time error if violated|
|[dmmeta.floadtuples](/txt/ssimdb/dmmeta/floadtuples.md)|Request that process automatically load any input tables on startup|
|[dmmeta.fnoremove](/txt/ssimdb/dmmeta/fnoremove.md)|Omit any functions for removing elements from table; Table is append-only|
|[dmmeta.foutput](/txt/ssimdb/dmmeta/foutput.md)|Generate function to save index/table back to disk|
|[dmmeta.fprefix](/txt/ssimdb/dmmeta/fprefix.md)|Mapping between field prefix and Reftype|
|[dmmeta.fregx](/txt/ssimdb/dmmeta/fregx.md)|Specify options for command-line regx field|
|[dmmeta.fsort](/txt/ssimdb/dmmeta/fsort.md)|Generate custom sort function for array field|
|[dmmeta.fstep](/txt/ssimdb/dmmeta/fstep.md)|Generate a main loop step to be executed whenever a field is non-empty|
|[dmmeta.ftrace](/txt/ssimdb/dmmeta/ftrace.md)|Generate cycle/step counting fields for a step|
|[dmmeta.func](/txt/ssimdb/dmmeta/func.md)||
|[dmmeta.funique](/txt/ssimdb/dmmeta/funique.md)|This field must be unique in the table. Not needed for primary key|
|[dmmeta.fuserinit](/txt/ssimdb/dmmeta/fuserinit.md)|Add user-defined initialization function for field (see fcleanup)|
|[dmmeta.fwddecl](/txt/ssimdb/dmmeta/fwddecl.md)|Request forward declaration of a field|
|[dmmeta.gconst](/txt/ssimdb/dmmeta/gconst.md)|Import ssim table columns as fconst for a field|
|[dmmeta.gstatic](/txt/ssimdb/dmmeta/gstatic.md)|Load entries for this table at startup time|
|[dmmeta.gsymbol](/txt/ssimdb/dmmeta/gsymbol.md)|Create C++ symbols from entries in ssim table|
|[dmmeta.hook](/txt/ssimdb/dmmeta/hook.md)|Required on Hook fields|
|[dmmeta.inlary](/txt/ssimdb/dmmeta/inlary.md)|Generate inline array of fixed or variable length (all entries fit within parent struct)|
|[dmmeta.jstype](/txt/ssimdb/dmmeta/jstype.md)||
|[dmmeta.lenfld](/txt/ssimdb/dmmeta/lenfld.md)|Specify which gives length of varlen portion in bytes|
|[dev.license](/txt/ssimdb/dev/license.md)|Source code license|
|[dmmeta.listtype](/txt/ssimdb/dmmeta/listtype.md)|Specify structure of linked list based on field prefix|
|[dmmeta.llist](/txt/ssimdb/dmmeta/llist.md)|Options for Llist field|
|[dmmeta.main](/txt/ssimdb/dmmeta/main.md)|Generate Main function for namespace|
|[dmmeta.msgtype](/txt/ssimdb/dmmeta/msgtype.md)|Specify message type for each eligible message, controls dispatch|
|[dmmeta.nocascdel](/txt/ssimdb/dmmeta/nocascdel.md)|Remove cascade delete for this index|
|[dmmeta.nossimfile](/txt/ssimdb/dmmeta/nossimfile.md)|Indicates that ssimfile does not exist for this ssimdb ctype|
|[dmmeta.noxref](/txt/ssimdb/dmmeta/noxref.md)|Explicitly specify that no x-ref exists between tables (don't use unless forced)|
|[dmmeta.ns](/txt/ssimdb/dmmeta/ns.md)|Namespace (for in-memory database, protocol, etc)|
|[dmmeta.nscpp](/txt/ssimdb/dmmeta/nscpp.md)|Generate C++ code for this namespace|
|[dmmeta.nsdb](/txt/ssimdb/dmmeta/nsdb.md)|Annotate ssimdb namespaces|
|[dmmeta.nsinclude](/txt/ssimdb/dmmeta/nsinclude.md)|Explicitly specify a C++ include file for namespace|
|[dmmeta.nsjs](/txt/ssimdb/dmmeta/nsjs.md)|Generate JavaScript code for this namespace|
|[dmmeta.nsproto](/txt/ssimdb/dmmeta/nsproto.md)|Annotate protocol namespace (collection of types, no state)|
|[dmmeta.nsx](/txt/ssimdb/dmmeta/nsx.md)|Control code-generation and exception handling options for process/library|
|[dmmeta.numstr](/txt/ssimdb/dmmeta/numstr.md)|Add functions to read numbers out of a string field|
|[dmmeta.pack](/txt/ssimdb/dmmeta/pack.md)|Request byte-packing of structure fields|
|[dmmeta.pmaskfld](/txt/ssimdb/dmmeta/pmaskfld.md)|Specify which field holds presence mask bits (one for each field in struct)|
|[dmmeta.pmaskfld_member](/txt/ssimdb/dmmeta/pmaskfld_member.md)|Explicit field-to-pmask association|
|[dmmeta.pnew](/txt/ssimdb/dmmeta/pnew.md)|Generate custom constructor (placement new), for use with binary protocols|
|[dmmeta.ptrary](/txt/ssimdb/dmmeta/ptrary.md)|Required for fields with reftype:Ptrary|
|[dmmeta.rowid](/txt/ssimdb/dmmeta/rowid.md)|Initialize field to row id of element|
|[dmmeta.smallstr](/txt/ssimdb/dmmeta/smallstr.md)|Generated fixed-length padded or length-delimited string field|
|[dmmeta.sortfld](/txt/ssimdb/dmmeta/sortfld.md)|Specify what field an index (Bheap,Atree) is sorted on|
|[dmmeta.ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|File with ssim tuples|
|[dmmeta.ssimsort](/txt/ssimdb/dmmeta/ssimsort.md)|Define sort order for ssimfile|
|[dmmeta.ssimvolatile](/txt/ssimdb/dmmeta/ssimvolatile.md)|Types based on ssim files marked this way cannot be cross-referenced|
|[dmmeta.steptype](/txt/ssimdb/dmmeta/steptype.md)|Type of scheduler step|
|[dmmeta.substr](/txt/ssimdb/dmmeta/substr.md)|Specify that the field value is computed from a substring of another field|
|[dev.targdep](/txt/ssimdb/dev/targdep.md)|Dependency between targets|
|[dev.target](/txt/ssimdb/dev/target.md)|Build target|
|[dmmeta.tary](/txt/ssimdb/dmmeta/tary.md)|Indirect linear dynamically alocated array (used for strings, arrays of data, etc)|
|[amcdb.tcurs](/txt/ssimdb/amcdb/tcurs.md)|Cursor template|
|[dmmeta.thash](/txt/ssimdb/dmmeta/thash.md)|Hash index, required for fields with reftype Thash|
|[dmmeta.typefld](/txt/ssimdb/dmmeta/typefld.md)|Specifies which field of a message carries the type|
|[dmmeta.usertracefld](/txt/ssimdb/dmmeta/usertracefld.md)|Add custom user trace fields to process's trace struct|
|[dmmeta.xref](/txt/ssimdb/dmmeta/xref.md)|Specify how to cross-reference (i.e. project, or group-by) one record with another|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Inputs -->


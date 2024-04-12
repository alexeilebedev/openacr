## amc - Algo Model Compiler: generate code under include/gen and cpp/gen
<a href="#amc"></a>
For the philosophy behind the generative approach, and an explanation
of concepts, see [Introduction To AMC](/txt/exe/amc/intro.md)

### Chapters
<a href="#chapters"></a>

* [AMC Enumerated Types](/txt/exe/amc/enum.md)

* [AMC Subprocess Invocation](/txt/exe/amc/exec.md)

* [AMC Intro](/txt/exe/amc/intro.md)

* [AMC Memory Pools](/txt/exe/amc/pools.md)

* [AMC Protocols](/txt/exe/amc/proto.md)

* [AMC Reflection](/txt/exe/amc/reflection.md)

* [AMC Reftypes](/txt/exe/amc/reftypes.md)

* [AMC release notes](/txt/exe/amc/relnotes.md)

* [AMC Runtime](/txt/exe/amc/runtime.md)

* [AMC Strings](/txt/exe/amc/strings.md)

* [AMC Cross-References (xrefs)](/txt/exe/amc/xref.md)

### Table Of Contents
<a href="#table-of-contents"></a>
* [Chapters](#chapters)
* [Syntax](#syntax)
* [Options](#options)
* [Introduction](#introduction)
* [Main Input Tables](#main-input-tables)
* [Size and field offset assertions](#size-and-field-offset-assertions)
* [Sources](#sources)
* [Inputs](#inputs)

### Syntax
<a href="#syntax"></a>
```
amc: Algo Model Compiler: generate code under include/gen and cpp/gen
Usage: amc [[-query:]<string>] [options]
    OPTION      TYPE    DFLT    COMMENT
    -in_dir     string  "data"  Root of input ssim dir
    [query]     string  ""      Query mode: generate code for specified object
    -out_dir    string  "."     Root of output cpp dir
    -proto                      Print prototype
    -report             Y       Final report
    -e                          Open matching records in editor
    -trace      regx    ""      Regx of something to trace code generation
    -verbose    int             Verbosity level (0..255); alias -v; cumulative
    -debug      int             Debug level (0..255); alias -d; cumulative
    -help                       Print help and exit; alias -h
    -version                    Print version and exit
    -signature                  Show signatures and exit; alias -sig

```

### Options
<a href="#options"></a>

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

#### -report -- Final report
<a href="#-report"></a>

#### -e -- Open matching records in editor
<a href="#-e"></a>

#### -trace -- Regx of something to trace code generation
<a href="#-trace"></a>

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

### Sources
<a href="#sources"></a>
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/amc/alias.cpp](/cpp/amc/alias.cpp)||
|[cpp/amc/avl.cpp](/cpp/amc/avl.cpp)||
|[cpp/amc/base.cpp](/cpp/amc/base.cpp)||
|[cpp/amc/bheap.cpp](/cpp/amc/bheap.cpp)|Binary heaps|
|[cpp/amc/bitfld.cpp](/cpp/amc/bitfld.cpp)||
|[cpp/amc/bitset.cpp](/cpp/amc/bitset.cpp)||
|[cpp/amc/blkpool.cpp](/cpp/amc/blkpool.cpp)||
|[cpp/amc/cget.cpp](/cpp/amc/cget.cpp)||
|[cpp/amc/charset.cpp](/cpp/amc/charset.cpp)|Charset functions|
|[cpp/amc/checkxref.cpp](/cpp/amc/checkxref.cpp)||
|[cpp/amc/cmp.cpp](/cpp/amc/cmp.cpp)||
|[cpp/amc/concat.cpp](/cpp/amc/concat.cpp)||
|[cpp/amc/copypriv.cpp](/cpp/amc/copypriv.cpp)||
|[cpp/amc/count.cpp](/cpp/amc/count.cpp)||
|[cpp/amc/cppfunc.cpp](/cpp/amc/cppfunc.cpp)||
|[cpp/amc/ctype.cpp](/cpp/amc/ctype.cpp)|Ctype code generators|
|[cpp/amc/delptr.cpp](/cpp/amc/delptr.cpp)||
|[cpp/amc/dflt.cpp](/cpp/amc/dflt.cpp)|Field default|
|[cpp/amc/disp/call.cpp](/cpp/amc/disp/call.cpp)||
|[cpp/amc/disp/casetype.cpp](/cpp/amc/disp/casetype.cpp)||
|[cpp/amc/disp/filter.cpp](/cpp/amc/disp/filter.cpp)|Dispatch filter|
|[cpp/amc/disp/main.cpp](/cpp/amc/disp/main.cpp)|Dispatch|
|[cpp/amc/disp/msg.cpp](/cpp/amc/disp/msg.cpp)||
|[cpp/amc/disp/print.cpp](/cpp/amc/disp/print.cpp)||
|[cpp/amc/disp/read.cpp](/cpp/amc/disp/read.cpp)||
|[cpp/amc/enum.cpp](/cpp/amc/enum.cpp)||
|[cpp/amc/exec.cpp](/cpp/amc/exec.cpp)|reftype Exec|
|[cpp/amc/fast.cpp](/cpp/amc/fast.cpp)||
|[cpp/amc/fbuf.cpp](/cpp/amc/fbuf.cpp)|Byte buffer|
|[cpp/amc/fcast.cpp](/cpp/amc/fcast.cpp)||
|[cpp/amc/fcmap.cpp](/cpp/amc/fcmap.cpp)|Fcmap generation|
|[cpp/amc/fconst.cpp](/cpp/amc/fconst.cpp)|Constants, string <-> int conversion|
|[cpp/amc/fdec.cpp](/cpp/amc/fdec.cpp)||
|[cpp/amc/field.cpp](/cpp/amc/field.cpp)||
|[cpp/amc/func.cpp](/cpp/amc/func.cpp)|C++ function output|
|[cpp/amc/fwddecl.cpp](/cpp/amc/fwddecl.cpp)||
|[cpp/amc/gen.cpp](/cpp/amc/gen.cpp)||
|[cpp/amc/global.cpp](/cpp/amc/global.cpp)||
|[cpp/amc/gsymbol.cpp](/cpp/amc/gsymbol.cpp)|Generate strings from tables|
|[cpp/amc/hook.cpp](/cpp/amc/hook.cpp)||
|[cpp/amc/include.cpp](/cpp/amc/include.cpp)||
|[cpp/amc/inlary.cpp](/cpp/amc/inlary.cpp)||
|[cpp/amc/io.cpp](/cpp/amc/io.cpp)|I/O functions|
|[cpp/amc/lary.cpp](/cpp/amc/lary.cpp)||
|[cpp/amc/llist.cpp](/cpp/amc/llist.cpp)|Linked lists|
|[cpp/amc/lpool.cpp](/cpp/amc/lpool.cpp)||
|[cpp/amc/main.cpp](/cpp/amc/main.cpp)|Main driver|
|[cpp/amc/malloc.cpp](/cpp/amc/malloc.cpp)||
|[cpp/amc/msgcurs.cpp](/cpp/amc/msgcurs.cpp)|Message scanning cursor|
|[cpp/amc/nstype.cpp](/cpp/amc/nstype.cpp)||
|[cpp/amc/numstr.cpp](/cpp/amc/numstr.cpp)|Small strings|
|[cpp/amc/opt.cpp](/cpp/amc/opt.cpp)||
|[cpp/amc/outfile.cpp](/cpp/amc/outfile.cpp)||
|[cpp/amc/pmask.cpp](/cpp/amc/pmask.cpp)||
|[cpp/amc/pnew.cpp](/cpp/amc/pnew.cpp)|Custom constructors|
|[cpp/amc/pool.cpp](/cpp/amc/pool.cpp)||
|[cpp/amc/print.cpp](/cpp/amc/print.cpp)|Print ctype to string|
|[cpp/amc/protocol.cpp](/cpp/amc/protocol.cpp)||
|[cpp/amc/ptr.cpp](/cpp/amc/ptr.cpp)||
|[cpp/amc/ptrary.cpp](/cpp/amc/ptrary.cpp)||
|[cpp/amc/query.cpp](/cpp/amc/query.cpp)||
|[cpp/amc/read.cpp](/cpp/amc/read.cpp)|Read ctype from string|
|[cpp/amc/regx.cpp](/cpp/amc/regx.cpp)|Small strings|
|[cpp/amc/sbrk.cpp](/cpp/amc/sbrk.cpp)||
|[cpp/amc/signature.cpp](/cpp/amc/signature.cpp)||
|[cpp/amc/size.cpp](/cpp/amc/size.cpp)|Compute struct sizes|
|[cpp/amc/smallstr.cpp](/cpp/amc/smallstr.cpp)|Small strings|
|[cpp/amc/sort.cpp](/cpp/amc/sort.cpp)|Comparison & Sorting routines|
|[cpp/amc/step.cpp](/cpp/amc/step.cpp)||
|[cpp/amc/struct.cpp](/cpp/amc/struct.cpp)||
|[cpp/amc/substr.cpp](/cpp/amc/substr.cpp)||
|[cpp/amc/tableid.cpp](/cpp/amc/tableid.cpp)|Per-namespace enum of tables|
|[cpp/amc/tary.cpp](/cpp/amc/tary.cpp)||
|[cpp/amc/tclass.cpp](/cpp/amc/tclass.cpp)||
|[cpp/amc/thash.cpp](/cpp/amc/thash.cpp)|Hash tables|
|[cpp/amc/tpool.cpp](/cpp/amc/tpool.cpp)||
|[cpp/amc/trace.cpp](/cpp/amc/trace.cpp)|Generate trace code|
|[cpp/amc/upptr.cpp](/cpp/amc/upptr.cpp)||
|[cpp/amc/val.cpp](/cpp/amc/val.cpp)||
|[cpp/amc/varlen.cpp](/cpp/amc/varlen.cpp)||
|[cpp/gen/amc_gen.cpp](/cpp/gen/amc_gen.cpp)||
|[include/amc.h](/include/amc.h)|Main header|
|[include/gen/amc_gen.h](/include/gen/amc_gen.h)||
|[include/gen/amc_gen.inl.h](/include/gen/amc_gen.inl.h)||

### Inputs
<a href="#inputs"></a>
`amc` takes the following tables on input:
|ssimfile|comment|
|---|---|
|[dev.license](/txt/ssimdb/dev/license.md)|Source code license|
|[dmmeta.ns](/txt/ssimdb/dmmeta/ns.md)|Namespace (for in-memory database, protocol, etc)|
|[dmmeta.ctype](/txt/ssimdb/dmmeta/ctype.md)|Struct|
|[dmmeta.field](/txt/ssimdb/dmmeta/field.md)|Specify field of a struct|
|[dmmeta.xref](/txt/ssimdb/dmmeta/xref.md)|Specify how to cross-reference (i.e. project, or group-by) one record with another|
|[dmmeta.usertracefld](/txt/ssimdb/dmmeta/usertracefld.md)|Add custom user trace fields to process's trace struct|
|[dmmeta.typefld](/txt/ssimdb/dmmeta/typefld.md)|Specifies which field of a message carries the type|
|[dmmeta.thash](/txt/ssimdb/dmmeta/thash.md)|Hash index, required for fields with reftype Thash|
|[amcdb.tcurs](/txt/ssimdb/amcdb/tcurs.md)|Cursor template|
|[dmmeta.tary](/txt/ssimdb/dmmeta/tary.md)|Indirect linear dynamically alocated array (used for strings, arrays of data, etc)|
|[dev.target](/txt/ssimdb/dev/target.md)|Build target|
|[dev.targdep](/txt/ssimdb/dev/targdep.md)|Dependency between targets|
|[dmmeta.substr](/txt/ssimdb/dmmeta/substr.md)|Specify that the field value is computed from a substring of another field|
|[dmmeta.nsdb](/txt/ssimdb/dmmeta/nsdb.md)|Annotate ssimdb namespaces|
|[dmmeta.ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|File with ssim tuples|
|[dmmeta.ssimvolatile](/txt/ssimdb/dmmeta/ssimvolatile.md)|Types based on ssim files marked this way cannot be cross-referenced|
|[dmmeta.ssimsort](/txt/ssimdb/dmmeta/ssimsort.md)|Define sort order for ssimfile|
|[dmmeta.sortfld](/txt/ssimdb/dmmeta/sortfld.md)|Specify what field an index (Bheap,Atree) is sorted on|
|[dmmeta.smallstr](/txt/ssimdb/dmmeta/smallstr.md)|Generated fixed-length padded or length-delimited string field|
|[dmmeta.rowid](/txt/ssimdb/dmmeta/rowid.md)|Initialize field to row id of element|
|[dmmeta.ptrary](/txt/ssimdb/dmmeta/ptrary.md)|Required for fields with reftype:Ptrary|
|[dmmeta.pnew](/txt/ssimdb/dmmeta/pnew.md)|Generate custom constructor (placement new), for use with binary protocols|
|[dmmeta.pmaskfld](/txt/ssimdb/dmmeta/pmaskfld.md)|Specify which field holds presence mask bits (one for each field in struct)|
|[dmmeta.pmaskfld_member](/txt/ssimdb/dmmeta/pmaskfld_member.md)|Explicit field-to-pmask association|
|[dmmeta.pack](/txt/ssimdb/dmmeta/pack.md)|Request byte-packing of structure fields|
|[dmmeta.numstr](/txt/ssimdb/dmmeta/numstr.md)|Add functions to read numbers out of a string field|
|[dmmeta.nsx](/txt/ssimdb/dmmeta/nsx.md)|Control code-generation and exception handling options for process/library|
|[dmmeta.nsproto](/txt/ssimdb/dmmeta/nsproto.md)|Annotate protocol namespace (collection of types, no state)|
|[dmmeta.nsinclude](/txt/ssimdb/dmmeta/nsinclude.md)|Explicitly specify a C++ include file for namespace|
|[dmmeta.nscpp](/txt/ssimdb/dmmeta/nscpp.md)|Generate C++ code for this namespace|
|[dmmeta.noxref](/txt/ssimdb/dmmeta/noxref.md)|Explicitly specify that no x-ref exists between tables (don't use unless forced)|
|[dmmeta.nossimfile](/txt/ssimdb/dmmeta/nossimfile.md)|Indicates that ssimfile does not exist for this ssimdb ctype|
|[dmmeta.nocascdel](/txt/ssimdb/dmmeta/nocascdel.md)|Remove cascade delete for this index|
|[dmmeta.msgtype](/txt/ssimdb/dmmeta/msgtype.md)|Specify message type for each eligible message, controls dispatch|
|[dmmeta.main](/txt/ssimdb/dmmeta/main.md)|Generate Main function for namespace|
|[dmmeta.fprefix](/txt/ssimdb/dmmeta/fprefix.md)|Mapping between field prefix and Reftype|
|[dmmeta.listtype](/txt/ssimdb/dmmeta/listtype.md)|Specify structure of linked list based on field prefix|
|[dmmeta.llist](/txt/ssimdb/dmmeta/llist.md)|Options for Llist field|
|[dmmeta.lenfld](/txt/ssimdb/dmmeta/lenfld.md)|Specify which gives length of varlen portion in bytes|
|[dmmeta.inlary](/txt/ssimdb/dmmeta/inlary.md)|Generate inline array of fixed or variable length (all entries fit within parent struct)|
|[dmmeta.hook](/txt/ssimdb/dmmeta/hook.md)|Required on Hook fields|
|[dmmeta.gsymbol](/txt/ssimdb/dmmeta/gsymbol.md)|Create C++ symbols from entries in ssim table|
|[dmmeta.gstatic](/txt/ssimdb/dmmeta/gstatic.md)|Load entries for this table at startup time|
|[dmmeta.gconst](/txt/ssimdb/dmmeta/gconst.md)|Import ssim table columns as fconst for a field|
|[dmmeta.fwddecl](/txt/ssimdb/dmmeta/fwddecl.md)|Request forward declaration of a field|
|[dmmeta.fuserinit](/txt/ssimdb/dmmeta/fuserinit.md)|Add user-defined initialization function for field (see fcleanup)|
|[dmmeta.funique](/txt/ssimdb/dmmeta/funique.md)|This field must be unique in the table. Not needed for primary key|
|[dmmeta.func](/txt/ssimdb/dmmeta/func.md)||
|[dmmeta.ftrace](/txt/ssimdb/dmmeta/ftrace.md)|Generate cycle/step counting fields for a step|
|[dmmeta.fstep](/txt/ssimdb/dmmeta/fstep.md)|Generate a main loop step to be executed whenever a field is non-empty|
|[dmmeta.fsort](/txt/ssimdb/dmmeta/fsort.md)|Generate custom sort function for array field|
|[dmmeta.fregx](/txt/ssimdb/dmmeta/fregx.md)|Specify options for command-line regx field|
|[dmmeta.foutput](/txt/ssimdb/dmmeta/foutput.md)|Generate function to save index/table back to disk|
|[dmmeta.fnoremove](/txt/ssimdb/dmmeta/fnoremove.md)|Omit any functions for removing elements from table; Table is append-only|
|[dmmeta.floadtuples](/txt/ssimdb/dmmeta/floadtuples.md)|Request that process automatically load any input tables on startup|
|[dmmeta.fldoffset](/txt/ssimdb/dmmeta/fldoffset.md)|Assert field offset - will result in compile-time error if violated|
|[dmmeta.finput](/txt/ssimdb/dmmeta/finput.md)|Describe input table of a program|
|[dmmeta.findrem](/txt/ssimdb/dmmeta/findrem.md)|Request generation of FindRemove function|
|[dmmeta.fflag](/txt/ssimdb/dmmeta/fflag.md)|Options for command-line flags|
|[dmmeta.ffast](/txt/ssimdb/dmmeta/ffast.md)|FAST properties for field|
|[dmmeta.fdelay](/txt/ssimdb/dmmeta/fdelay.md)|Control elapsed time between execution of a step|
|[dmmeta.fdec](/txt/ssimdb/dmmeta/fdec.md)|Specify that field has an implied # of decimal places and specify formatting options|
|[dmmeta.fcurs](/txt/ssimdb/dmmeta/fcurs.md)|Request generation of custom cursor|
|[dmmeta.fconst](/txt/ssimdb/dmmeta/fconst.md)|Specify enum value (integer + string constant) for a field|
|[dmmeta.fcompact](/txt/ssimdb/dmmeta/fcompact.md)|Request compaction upon removal from index|
|[dmmeta.fcmp](/txt/ssimdb/dmmeta/fcmp.md)|Request versionsort or case-insensitive sort for field|
|[dmmeta.fcmdline](/txt/ssimdb/dmmeta/fcmdline.md)|Annotate field that holds process command line|
|[dmmeta.fcmap](/txt/ssimdb/dmmeta/fcmap.md)|Bidirectional mapping between fconst values|
|[dmmeta.fcleanup](/txt/ssimdb/dmmeta/fcleanup.md)|Request user-implemented function to be called at Uninit time for a field|
|[dmmeta.fcast](/txt/ssimdb/dmmeta/fcast.md)|Generate implicit conversion from field to c++ expression|
|[dmmeta.fbuf](/txt/ssimdb/dmmeta/fbuf.md)|Buffer for reading/writing messages, works with Iohook|
|[dmmeta.fbitset](/txt/ssimdb/dmmeta/fbitset.md)|Generate bitset functions over integer field or array|
|[dmmeta.fbigend](/txt/ssimdb/dmmeta/fbigend.md)|Annotate field as having big-endian storage|
|[dmmeta.fbase](/txt/ssimdb/dmmeta/fbase.md)|Customize imported Base fields|
|[dmmeta.falias](/txt/ssimdb/dmmeta/falias.md)|Alias field|
|[dmmeta.dispatch](/txt/ssimdb/dmmeta/dispatch.md)|Generate code for a multi-way branch|
|[dmmeta.disptrace](/txt/ssimdb/dmmeta/disptrace.md)|Generate trace fields (cycles, counts) for all dispatch branches|
|[dmmeta.dispfilter](/txt/ssimdb/dmmeta/dispfilter.md)|Generate filter function on dispatch|
|[dmmeta.dispctx](/txt/ssimdb/dmmeta/dispctx.md)|Use context with dispatch|
|[dmmeta.dispatch_msg](/txt/ssimdb/dmmeta/dispatch_msg.md)|Add message to a dispatch|
|[dmmeta.cstr](/txt/ssimdb/dmmeta/cstr.md)|Specify that type behaves like a string|
|[dmmeta.csize](/txt/ssimdb/dmmeta/csize.md)|Specify size/alignment for built-in C++ types|
|[dmmeta.cpptype](/txt/ssimdb/dmmeta/cpptype.md)|Specify whether a ctype can be passed by value, and other c++ options|
|[dmmeta.cppfunc](/txt/ssimdb/dmmeta/cppfunc.md)|Value of field provided by this expression|
|[dmmeta.chash](/txt/ssimdb/dmmeta/chash.md)|Generate hash function|
|[dmmeta.charset](/txt/ssimdb/dmmeta/charset.md)|Generate functions to determine if a character is a member of a set|
|[dmmeta.cget](/txt/ssimdb/dmmeta/cget.md)|Generate state functions for these ctypes|
|[dmmeta.cfmt](/txt/ssimdb/dmmeta/cfmt.md)|Specify options for printing/reading ctypes into multiple formats|
|[dmmeta.cfast](/txt/ssimdb/dmmeta/cfast.md)|FAST properties for ctype|
|[dmmeta.cextern](/txt/ssimdb/dmmeta/cextern.md)|Externally defined ctype (a struct from system header, or from a c++ library)|
|[dmmeta.cdflt](/txt/ssimdb/dmmeta/cdflt.md)|Specify default value for single-value types that lack fields|
|[dmmeta.ccmp](/txt/ssimdb/dmmeta/ccmp.md)|Generate hash function|
|[dmmeta.cascdel](/txt/ssimdb/dmmeta/cascdel.md)|Request cascading delete of referenced rows|
|[dmmeta.cafter](/txt/ssimdb/dmmeta/cafter.md)|Control amc processing order for unrelated types (used in rare situations)|
|[amcdb.bltin](/txt/ssimdb/amcdb/bltin.md)|Specify properties of a C built-in type|
|[dmmeta.bitfld](/txt/ssimdb/dmmeta/bitfld.md)|Specify offset/width of a bitfield within another field|
|[dmmeta.basepool](/txt/ssimdb/dmmeta/basepool.md)|Specify pool to be used for allocating elements of a type|
|[dmmeta.argvtype](/txt/ssimdb/dmmeta/argvtype.md)|Customize parsing of command lines (rarely used)|
|[dmmeta.anonfld](/txt/ssimdb/dmmeta/anonfld.md)|Omit field name where possible (command line, enums, constants)|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|


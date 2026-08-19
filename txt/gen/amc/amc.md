## amc - Internals


### Table Of Contents
<a href="#table-of-contents"></a>
* [Description](#description)
* [Sources](#sources)
* [Dependencies](#dependencies)
* [In Memory DB](#in-memory-db)

### Description
<a href="#description"></a>
for usage, see [amc - Algo Model Compiler: generate code under include/gen and cpp/gen](/txt/exe/amc/README.md)

### Sources
<a href="#sources"></a>
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/amc/alias.cpp](/cpp/amc/alias.cpp)|Alias field type|
|[cpp/amc/avl.cpp](/cpp/amc/avl.cpp)|AVL tree|
|[cpp/amc/base.cpp](/cpp/amc/base.cpp)|Base reftype|
|[cpp/amc/bheap.cpp](/cpp/amc/bheap.cpp)|Binary heaps|
|[cpp/amc/bitfld.cpp](/cpp/amc/bitfld.cpp)|Bit fields|
|[cpp/amc/bitset.cpp](/cpp/amc/bitset.cpp)|Bit sets|
|[cpp/amc/blkhash.cpp](/cpp/amc/blkhash.cpp)||
|[cpp/amc/blkpool.cpp](/cpp/amc/blkpool.cpp)|Block pool|
|[cpp/amc/cget.cpp](/cpp/amc/cget.cpp)|Getters / Setters|
|[cpp/amc/charset.cpp](/cpp/amc/charset.cpp)|Charset functions|
|[cpp/amc/checkxref.cpp](/cpp/amc/checkxref.cpp)|X-reference checker|
|[cpp/amc/cmdline.cpp](/cpp/amc/cmdline.cpp)||
|[cpp/amc/cmp.cpp](/cpp/amc/cmp.cpp)|Comparison functions|
|[cpp/amc/concat.cpp](/cpp/amc/concat.cpp)|Pkey constructor|
|[cpp/amc/count.cpp](/cpp/amc/count.cpp)|Count reftype|
|[cpp/amc/cppfunc.cpp](/cpp/amc/cppfunc.cpp)|Cppfunc reftype|
|[cpp/amc/ctype.cpp](/cpp/amc/ctype.cpp)|Ctype code generators|
|[cpp/amc/delptr.cpp](/cpp/amc/delptr.cpp)|Delptr reftype|
|[cpp/amc/dflt.cpp](/cpp/amc/dflt.cpp)|Field default|
|[cpp/amc/disp/call.cpp](/cpp/amc/disp/call.cpp)|Dispatch call|
|[cpp/amc/disp/casetype.cpp](/cpp/amc/disp/casetype.cpp)|Dispatch casetype generator|
|[cpp/amc/disp/del.cpp](/cpp/amc/disp/del.cpp)||
|[cpp/amc/disp/filter.cpp](/cpp/amc/disp/filter.cpp)|Dispatch filter|
|[cpp/amc/disp/kafka_decode.cpp](/cpp/amc/disp/kafka_decode.cpp)||
|[cpp/amc/disp/kafka_encode.cpp](/cpp/amc/disp/kafka_encode.cpp)||
|[cpp/amc/disp/main.cpp](/cpp/amc/disp/main.cpp)|Dispatch main|
|[cpp/amc/disp/msg.cpp](/cpp/amc/disp/msg.cpp)|Dispatch on message|
|[cpp/amc/disp/print.cpp](/cpp/amc/disp/print.cpp)|Dispatch print|
|[cpp/amc/disp/read.cpp](/cpp/amc/disp/read.cpp)|Dispatch read from string|
|[cpp/amc/enum.cpp](/cpp/amc/enum.cpp)|Enumerated types|
|[cpp/amc/exec.cpp](/cpp/amc/exec.cpp)|Exec reftype|
|[cpp/amc/fast.cpp](/cpp/amc/fast.cpp)|Fixfast generator|
|[cpp/amc/fbuf.cpp](/cpp/amc/fbuf.cpp)|Byte buffer|
|[cpp/amc/fcast.cpp](/cpp/amc/fcast.cpp)|Implicit casts|
|[cpp/amc/fcmap.cpp](/cpp/amc/fcmap.cpp)|Fcmap generation (enum<->enum conversion)|
|[cpp/amc/fcond.cpp](/cpp/amc/fcond.cpp)||
|[cpp/amc/fconst.cpp](/cpp/amc/fconst.cpp)|Constants, string <-> int conversion|
|[cpp/amc/fdec.cpp](/cpp/amc/fdec.cpp)|Decimal types|
|[cpp/amc/field.cpp](/cpp/amc/field.cpp)|Generic field generator|
|[cpp/amc/func.cpp](/cpp/amc/func.cpp)|C++ function output|
|[cpp/amc/fwddecl.cpp](/cpp/amc/fwddecl.cpp)|Forward declarations|
|[cpp/amc/gen.cpp](/cpp/amc/gen.cpp)|AMC Generators (amcdb.gen)|
|[cpp/amc/global.cpp](/cpp/amc/global.cpp)|Global (FDb)|
|[cpp/amc/gsymbol.cpp](/cpp/amc/gsymbol.cpp)|Generate strings from tables|
|[cpp/amc/hook.cpp](/cpp/amc/hook.cpp)|Hook (function pointer)|
|[cpp/amc/include.cpp](/cpp/amc/include.cpp)|Manage includes for generated files|
|[cpp/amc/inlary.cpp](/cpp/amc/inlary.cpp)|Inline array|
|[cpp/amc/io.cpp](/cpp/amc/io.cpp)|I/O functions|
|[cpp/amc/js.cpp](/cpp/amc/js.cpp)||
|[cpp/amc/kafka.cpp](/cpp/amc/kafka.cpp)||
|[cpp/amc/lary.cpp](/cpp/amc/lary.cpp)|Level array with permanent pointers|
|[cpp/amc/llist.cpp](/cpp/amc/llist.cpp)|Linked lists|
|[cpp/amc/lpool.cpp](/cpp/amc/lpool.cpp)|Variable-length free pool|
|[cpp/amc/main.cpp](/cpp/amc/main.cpp)|Main driver|
|[cpp/amc/malloc.cpp](/cpp/amc/malloc.cpp)|Malloc allocator|
|[cpp/amc/msgcurs.cpp](/cpp/amc/msgcurs.cpp)|Message scanning cursor|
|[cpp/amc/msgschema.cpp](/cpp/amc/msgschema.cpp)||
|[cpp/amc/numstr.cpp](/cpp/amc/numstr.cpp)|Small strings|
|[cpp/amc/opt.cpp](/cpp/amc/opt.cpp)|Opt reftype|
|[cpp/amc/outfile.cpp](/cpp/amc/outfile.cpp)|Output functions|
|[cpp/amc/pbuf.cpp](/cpp/amc/pbuf.cpp)|Protobuf wire codec|
|[cpp/amc/pmask.cpp](/cpp/amc/pmask.cpp)|Presence masks|
|[cpp/amc/pnew.cpp](/cpp/amc/pnew.cpp)|Custom constructors|
|[cpp/amc/pool.cpp](/cpp/amc/pool.cpp)|Generic pool functions|
|[cpp/amc/print.cpp](/cpp/amc/print.cpp)|Print ctype to string|
|[cpp/amc/protocol.cpp](/cpp/amc/protocol.cpp)|Protocol functions|
|[cpp/amc/ptr.cpp](/cpp/amc/ptr.cpp)|Ptr reftype|
|[cpp/amc/ptrary.cpp](/cpp/amc/ptrary.cpp)|Ptrary reftype|
|[cpp/amc/query.cpp](/cpp/amc/query.cpp)|Query mode|
|[cpp/amc/read.cpp](/cpp/amc/read.cpp)|Read ctype from string|
|[cpp/amc/regx.cpp](/cpp/amc/regx.cpp)|Small strings|
|[cpp/amc/sbrk.cpp](/cpp/amc/sbrk.cpp)|Sbrk allocator|
|[cpp/amc/signature.cpp](/cpp/amc/signature.cpp)|Signature calculation for protocols|
|[cpp/amc/size.cpp](/cpp/amc/size.cpp)|Compute struct sizes|
|[cpp/amc/smallstr.cpp](/cpp/amc/smallstr.cpp)|Small strings|
|[cpp/amc/sort.cpp](/cpp/amc/sort.cpp)|Comparison & Sorting routines|
|[cpp/amc/step.cpp](/cpp/amc/step.cpp)|Step functions|
|[cpp/amc/struct.cpp](/cpp/amc/struct.cpp)|C++ struct output|
|[cpp/amc/substr.cpp](/cpp/amc/substr.cpp)|Substr fields|
|[cpp/amc/tableid.cpp](/cpp/amc/tableid.cpp)|Per-namespace enum of tables|
|[cpp/amc/tary.cpp](/cpp/amc/tary.cpp)|Tary (vector) reftype|
|[cpp/amc/tclass.cpp](/cpp/amc/tclass.cpp)|Driver for tfuncs|
|[cpp/amc/thash.cpp](/cpp/amc/thash.cpp)|Hash tables|
|[cpp/amc/tpool.cpp](/cpp/amc/tpool.cpp)|Tpool refetype (fixed-length freelist)|
|[cpp/amc/trace.cpp](/cpp/amc/trace.cpp)|Generate trace code|
|[cpp/amc/upptr.cpp](/cpp/amc/upptr.cpp)|Upptr reftype|
|[cpp/amc/val.cpp](/cpp/amc/val.cpp)|Val reftype|
|[cpp/amc/varlen.cpp](/cpp/amc/varlen.cpp)|Varlen reftype|
|[cpp/gen/amc_gen.cpp](/cpp/gen/amc_gen.cpp)||
|[include/amc.h](/include/amc.h)|Main header|
|[include/gen/amc_gen.h](/include/gen/amc_gen.h)||
|[include/gen/amc_gen.inl.h](/include/gen/amc_gen.inl.h)||

### Dependencies
<a href="#dependencies"></a>
The build target depends on the following libraries
|Target|Comment|
|---|---|
|[algo_lib](/txt/lib/algo_lib/README.md)|Support library for all executables|
|[lib_amcdb](/txt/lib/lib_amcdb/README.md)|Library used by amc|

### In Memory DB
<a href="#in-memory-db"></a>
`amc` generated code creates the tables below.
All allocations are done through global `amc::_db` [amc.FDb](#amc-fdb) structure
|Ctype|Ssimfile|Create|Access|
|---|---|---|---|
|[amc.BltinId](#amc-bltinid)||
|[amc.CppkeywordId](#amc-cppkeywordid)||
|[amc.Enumstr](#amc-enumstr)||
|[amc.FAnonfld](#amc-fanonfld)|[dmmeta.anonfld](/txt/ssimdb/dmmeta/anonfld.md)|FDb.anonfld (Lary)|anonfld (Lary, by rowid)|
||||FField.c_anonfld (Ptr)|
|[amc.FArgvtype](#amc-fargvtype)|[dmmeta.argvtype](/txt/ssimdb/dmmeta/argvtype.md)|FDb.argvtype (Lary)|argvtype (Lary, by rowid)|
||||FCtype.c_argvtype (Ptr)|
|[amc.FBasepool](#amc-fbasepool)|[dmmeta.basepool](/txt/ssimdb/dmmeta/basepool.md)|FDb.basepool (Lary)|basepool (Lary, by rowid)|
||||FField.c_basepool (Ptr)|
|[amc.FBitfld](#amc-fbitfld)|[dmmeta.bitfld](/txt/ssimdb/dmmeta/bitfld.md)|FDb.bitfld (Lary)|bitfld (Lary, by rowid)|
||||FField.c_bitfld (Ptr)|
||||FField.bh_bitfld (Bheap)|
|[amc.FBlkhash](#amc-fblkhash)|[dmmeta.blkhash](/txt/ssimdb/dmmeta/blkhash.md)|FDb.blkhash (Lary)|blkhash (Lary, by rowid)|
||||FField.c_blkhash (Ptr)|
|[amc.FBltin](#amc-fbltin)|[amcdb.bltin](/txt/ssimdb/amcdb/bltin.md)|FDb.bltin (Lary)|bltin (Lary, by rowid)|ind_bltin (Thash, hash field ctype)|
||||FCtype.c_bltin (Ptr)|
|[amc.FCafter](#amc-fcafter)|[dmmeta.cafter](/txt/ssimdb/dmmeta/cafter.md)|FDb.cafter (Lary)|cafter (Lary, by rowid)|
||||FCtype.zd_cafter (Llist)|
|[amc.FCascdel](#amc-fcascdel)|[dmmeta.cascdel](/txt/ssimdb/dmmeta/cascdel.md)|FDb.cascdel (Lary)|cascdel (Lary, by rowid)|
||||FField.c_cascdel (Ptr)|
|[amc.FCcmdline](#amc-fccmdline)|[dmmeta.ccmdline](/txt/ssimdb/dmmeta/ccmdline.md)|FDb.ccmdline (Lary)|ccmdline (Lary, by rowid)|
||||FCtype.c_ccmdline (Ptr)|
|[amc.FCcmp](#amc-fccmp)|[dmmeta.ccmp](/txt/ssimdb/dmmeta/ccmp.md)|FDb.ccmp (Lary)|ccmp (Lary, by rowid)|ind_ccmp (Thash, hash field ctype)|
||||FCtype.c_ccmp (Ptr)|
|[amc.FCdflt](#amc-fcdflt)|[dmmeta.cdflt](/txt/ssimdb/dmmeta/cdflt.md)|FDb.cdflt (Lary)|cdflt (Lary, by rowid)|
||||FCtype.c_cdflt (Ptr)|
|[amc.FCextern](#amc-fcextern)|[dmmeta.cextern](/txt/ssimdb/dmmeta/cextern.md)|FDb.cextern (Lary)|cextern (Lary, by rowid)|
||||FCtype.c_cextern (Ptr)|
|[amc.FCfast](#amc-fcfast)|[dmmeta.cfast](/txt/ssimdb/dmmeta/cfast.md)|FDb.cfast (Lary)|cfast (Lary, by rowid)|
||||FCtype.c_cfast (Ptr)|
|[amc.FCfmt](#amc-fcfmt)|[dmmeta.cfmt](/txt/ssimdb/dmmeta/cfmt.md)|FDb.cfmt (Lary)|ind_cfmt (Thash, hash field cfmt)|cfmt (Lary, by rowid)|
||||FCtype.zs_cfmt (Llist)|
|[amc.FCget](#amc-fcget)|[dmmeta.cget](/txt/ssimdb/dmmeta/cget.md)|FDb.cget (Lary)|cget (Lary, by rowid)|
|[amc.FCharset](#amc-fcharset)|[dmmeta.charset](/txt/ssimdb/dmmeta/charset.md)|FDb.charset (Lary)|charset (Lary, by rowid)|
||||FField.c_charset (Ptr)|
|[amc.FChash](#amc-fchash)|[dmmeta.chash](/txt/ssimdb/dmmeta/chash.md)|FDb.chash (Lary)|chash (Lary, by rowid)|ind_chash (Thash, hash field ctype)|
||||FCtype.c_chash (Ptr)|
|[amc.FCjsbltin](#amc-fcjsbltin)|[dmmeta.cjsbltin](/txt/ssimdb/dmmeta/cjsbltin.md)|FDb.cjsbltin (Lary)|cjsbltin (Lary, by rowid)|
||||FCtype.c_cjsbltin (Ptr)|
|[amc.FCjson](#amc-fcjson)|[dmmeta.cjson](/txt/ssimdb/dmmeta/cjson.md)|FDb.cjson (Lary)|cjson (Lary, by rowid)|
||||FCtype.c_cjson (Ptr)|
|[amc.FCkafka](#amc-fckafka)|[dmmeta.ckafka](/txt/ssimdb/dmmeta/ckafka.md)|FDb.ckafka (Lary)|ckafka (Lary, by rowid)|
||||FCtype.c_ckafka (Ptr)|
|[amc.FCpbuf](#amc-fcpbuf)|[dmmeta.cpbuf](/txt/ssimdb/dmmeta/cpbuf.md)|FDb.cpbuf (Lary)|cpbuf (Lary, by rowid)|
||||FCtype.c_cpbuf (Ptr)|
|[amc.FCppfunc](#amc-fcppfunc)|[dmmeta.cppfunc](/txt/ssimdb/dmmeta/cppfunc.md)|FDb.cppfunc (Lary)|cppfunc (Lary, by rowid)|
||||FField.c_cppfunc (Ptr)|
|[amc.FCpptype](#amc-fcpptype)|[dmmeta.cpptype](/txt/ssimdb/dmmeta/cpptype.md)|FDb.cpptype (Lary)|cpptype (Lary, by rowid)|ind_cpptype (Thash, hash field ctype)|
||||FCtype.c_cpptype (Ptr)|
|[amc.FCsize](#amc-fcsize)|[dmmeta.csize](/txt/ssimdb/dmmeta/csize.md)|FDb.csize (Lary)|csize (Lary, by rowid)|
||||FCtype.c_csize (Ptr)|
|[amc.FCstr](#amc-fcstr)|[dmmeta.cstr](/txt/ssimdb/dmmeta/cstr.md)|FDb.cstr (Lary)|cstr (Lary, by rowid)|
||||FCtype.c_cstr (Ptr)|
|[amc.FCtype](#amc-fctype)|[dmmeta.ctype](/txt/ssimdb/dmmeta/ctype.md)|FDb.ctype (Lary)|ctype (Lary, by rowid)|ind_ctype (Thash, hash field ctype)|c_ctype_sorted (Ptrary)|zsl_ctype_pack_tran (Llist)|zs_sig_visit (Llist)|c_u64 (Ptr)|
||||FCafter.p_after (Upptr)|
||||FCcmdline.p_ctype (Upptr)|
||||FCfmt.p_ctype (Upptr)|
||||FCget.p_ctype (Upptr)|
||||FCpptype.p_ctype (Upptr)|
||||FCtype.c_parent (Ptrary)|
||||FDispatch.p_ctype_hdr (Upptr)|
||||FDispatch.p_casetype (Upptr)|
||||FDispatchmsg.p_ctype (Upptr)|
||||FDispctx.p_ctype (Upptr)|
||||FField.p_ctype (Upptr)|
||||FField.p_arg (Upptr)|
||||FGsymbol.p_symboltype (Upptr)|
||||FHook.p_funcptr (Upptr)|
||||FMsgtype.p_ctype (Upptr)|
||||FNs.c_ctype (Ptrary)|
||||FNumstr.p_numtype (Upptr)|
||||FPack.p_ctype (Upptr)|
||||FPnew.p_ctype (Upptr)|
||||FSsimfile.p_ctype (Upptr)|
||||FTypefld.p_ctype (Upptr)|
||||FXref.p_ctype (Upptr)|
||||Genctx.p_ctype (Upptr)|
||||Genpnew.p_ctype (Upptr)|
|[amc.FCtypelen](#amc-fctypelen)|[dmmeta.ctypelen](/txt/ssimdb/dmmeta/ctypelen.md)|FDb.ctypelen (Lary)|ctypelen (Lary, by rowid)|
||||FCtype.c_ctypelen (Ptr)|
|[amc.FJsbltin](#amc-fjsbltin)|[dmmeta.jsbltin](/txt/ssimdb/dmmeta/jsbltin.md)|FDb.jsbltin (Inlary)|**static**|ind_jsbltin (Thash, hash field jsbltin)|
||||FCjsbltin.p_jsbltin (Upptr)|
||||FCjson.p_jsbltin (Upptr)|
|[amc.Genctx](#amc-genctx)||
|[amc.FTclass](#amc-ftclass)|[amcdb.tclass](/txt/ssimdb/amcdb/tclass.md)|FDb.tclass (Inlary)|**static**|ind_tclass (Thash, hash field tclass)|
||||FReftype.p_tclass (Upptr)|
|[amc.FDb](#amc-fdb)||FDb._db (Global)|
|[amc.FDispatch](#amc-fdispatch)|[dmmeta.dispatch](/txt/ssimdb/dmmeta/dispatch.md)|FDb.dispatch (Lary)|dispatch (Lary, by rowid)|ind_dispatch (Thash, hash field dispatch)|
||||FDispatchmsg.p_dispatch (Upptr)|
||||FDisptrace.p_dispatch (Upptr)|
||||FNs.c_dispatch (Ptrary)|
|[amc.FDispatchmsg](#amc-fdispatchmsg)|[dmmeta.dispatch_msg](/txt/ssimdb/dmmeta/dispatch_msg.md)|FDb.dispatch_msg (Lary)|dispatch_msg (Lary, by rowid)|
||||FDispatch.c_dispatch_msg (Ptrary)|
|[amc.FDispctx](#amc-fdispctx)|[dmmeta.dispctx](/txt/ssimdb/dmmeta/dispctx.md)|FDb.dispctx (Lary)|dispctx (Lary, by rowid)|
||||FDispatch.c_dispctx (Ptr)|
|[amc.FDispfilter](#amc-fdispfilter)|[dmmeta.dispfilter](/txt/ssimdb/dmmeta/dispfilter.md)|FDb.dispfilter (Lary)|dispfilter (Lary, by rowid)|
||||FDispatch.c_dispfilter (Ptr)|
|[amc.FDispsig](#amc-fdispsig)|[dmmeta.dispsig](/txt/ssimdb/dmmeta/dispsig.md)|FDb.dispsig (Lary)|dispsig (Lary, by rowid)|
||||FNs.c_dispsig (Ptrary)|
|[amc.FDisptrace](#amc-fdisptrace)|[dmmeta.disptrace](/txt/ssimdb/dmmeta/disptrace.md)|FDb.disptrace (Lary)|disptrace (Lary, by rowid)|
||||FDispatch.c_disptrace (Ptr)|
|[amc.FEnumstr](#amc-fenumstr)||FDb.enumstr (Lary)|enumstr (Lary, by rowid)|ind_enumstr (Thash, hash field enumstr)|
||||FEnumstrLen.bh_enumstr (Bheap)|
|[amc.FEnumstrLen](#amc-fenumstrlen)||FDb.enumstr_len (Lary)|enumstr_len (Lary, by rowid)|bh_enumstr_len (Bheap, sort field len)|ind_enumstr_len (Thash, hash field len)|
|[amc.FFalias](#amc-ffalias)|[dmmeta.falias](/txt/ssimdb/dmmeta/falias.md)|FDb.falias (Lary)|falias (Lary, by rowid)|
||||FField.c_falias (Ptr)|
|[amc.FFbigend](#amc-ffbigend)|[dmmeta.fbigend](/txt/ssimdb/dmmeta/fbigend.md)|FDb.fbigend (Lary)|fbigend (Lary, by rowid)|
||||FField.c_fbigend (Ptr)|
|[amc.FFbitset](#amc-ffbitset)|[dmmeta.fbitset](/txt/ssimdb/dmmeta/fbitset.md)|FDb.fbitset (Lary)|fbitset (Lary, by rowid)|ind_fbitset (Thash, hash field field)|
||||FField.c_fbitset (Ptr)|
|[amc.FFbuf](#amc-ffbuf)|[dmmeta.fbuf](/txt/ssimdb/dmmeta/fbuf.md)|FDb.fbuf (Lary)|fbuf (Lary, by rowid)|ind_fbuf (Thash, hash field field)|
||||FField.c_fbuf (Ptr)|
|[amc.FFbuftype](#amc-ffbuftype)|[dmmeta.fbuftype](/txt/ssimdb/dmmeta/fbuftype.md)|FDb.fbuftype (Lary)|fbuftype (Lary, by rowid)|ind_fbuftype (Thash, hash field fbuftype)|
||||FFbuf.p_fbuftype (Upptr)|
|[amc.FFcast](#amc-ffcast)|[dmmeta.fcast](/txt/ssimdb/dmmeta/fcast.md)|FDb.fcast (Lary)|fcast (Lary, by rowid)|
||||FField.c_fcast (Ptr)|
|[amc.FFcmap](#amc-ffcmap)|[dmmeta.fcmap](/txt/ssimdb/dmmeta/fcmap.md)|FDb.fcmap (Lary)|fcmap (Lary, by rowid)|ind_fcmap (Thash, hash field fcmap)|
||||FField.zs_fcmap (Llist)|
|[amc.FFcmp](#amc-ffcmp)|[dmmeta.fcmp](/txt/ssimdb/dmmeta/fcmp.md)|FDb.fcmp (Lary)|fcmp (Lary, by rowid)|
||||FField.c_fcmp (Ptr)|
|[amc.FFcompact](#amc-ffcompact)|[dmmeta.fcompact](/txt/ssimdb/dmmeta/fcompact.md)|FDb.fcompact (Lary)|fcompact (Lary, by rowid)|
||||FField.c_fcompact (Ptr)|
|[amc.FFcond](#amc-ffcond)|[dmmeta.fcond](/txt/ssimdb/dmmeta/fcond.md)|FDb.fcond (Lary)|fcond (Lary, by rowid)|
||||FField.c_fcond (Ptrary)|
|[amc.FFconst](#amc-ffconst)|[dmmeta.fconst](/txt/ssimdb/dmmeta/fconst.md)|FDb.fconst (Lary)|fconst (Lary, by rowid)|ind_fconst (Thash, hash field fconst)|ind_fconst_int (Thash, hash field cpp_value)|
||||FEnumstr.c_fconst (Ptrary)|
||||FField.c_fconst (Ptrary)|
||||FField.c_inkey_fconst (Ptr)|
|[amc.FFcurs](#amc-ffcurs)|[dmmeta.fcurs](/txt/ssimdb/dmmeta/fcurs.md)|FDb.fcurs (Lary)|fcurs (Lary, by rowid)|ind_fcurs (Thash, hash field fcurs)|
||||FCtype.c_fcurs (Ptrary)|
|[amc.FFdec](#amc-ffdec)|[dmmeta.fdec](/txt/ssimdb/dmmeta/fdec.md)|FDb.fdec (Lary)|fdec (Lary, by rowid)|ind_fdec (Thash, hash field field)|
||||FField.c_fdec (Ptr)|
|[amc.FFdelay](#amc-ffdelay)|[dmmeta.fdelay](/txt/ssimdb/dmmeta/fdelay.md)|FDb.fdelay (Lary)|fdelay (Lary, by rowid)|
||||FFstep.c_fdelay (Ptr)|
|[amc.FFfast](#amc-fffast)|[dmmeta.ffast](/txt/ssimdb/dmmeta/ffast.md)|FDb.ffast (Lary)|ffast (Lary, by rowid)|
||||FCtype.c_ffast (Ptrary)|
||||FField.c_ffast_field (Ptr)|
||||FField.c_ffast_length (Ptr)|
||||FField.c_ffast_mantissa (Ptr)|
||||FField.c_ffast (Ptrary)|
|[amc.FFflag](#amc-ffflag)|[dmmeta.fflag](/txt/ssimdb/dmmeta/fflag.md)|FDb.fflag (Lary)|fflag (Lary, by rowid)|ind_fflag (Thash, hash field field)|
||||FField.c_fflag (Ptr)|
|[amc.FFfunc](#amc-fffunc)|[dmmeta.ffunc](/txt/ssimdb/dmmeta/ffunc.md)|FDb.ffunc (Lary)|ffunc (Lary, by rowid)|ind_ffunc (Thash, hash field ffunc)|
||||FField.zs_ffunc (Llist)|
|[amc.FField](#amc-ffield)|[dmmeta.field](/txt/ssimdb/dmmeta/field.md)|FDb.field (Lary)|field (Lary, by rowid)|ind_field (Thash, hash field field)|c_malloc (Ptr)|zs_ordkeyfield (Llist)|
||||FBasepool.p_field (Upptr)|
||||FBasepool.p_base (Upptr)|
||||FBitfld.p_srcfield (Upptr)|
||||FBitfld.p_field (Upptr)|
||||FBlkhash.p_hashfld (Upptr)|
||||FBlkhash.p_linfld (Upptr)|
||||FBlkhash.p_field (Upptr)|
||||FCascdel.p_field (Upptr)|
||||FCcmdline.p_basecmdline (Upptr)|
||||FCtype.c_field (Ptrary)|
||||FCtype.c_optfld (Ptr)|
||||FCtype.c_datafld (Ptrary)|
||||FCtype.zd_inst (Llist)|
||||FCtype.zd_access (Llist)|
||||FCtype.c_pkeyfield (Ptr)|
||||FCtype.zd_varlenfld (Llist)|
||||FFalias.p_srcfield (Upptr)|
||||FFbigend.p_field (Upptr)|
||||FFbitset.p_field (Upptr)|
||||FFbuf.p_pair (Upptr)|
||||FFcmap.p_leftField (Upptr)|
||||FFcmap.p_rightField (Upptr)|
||||FFcond.p_ins (Upptr)|
||||FFconst.p_field (Upptr)|
||||FFcurs.p_field (Upptr)|
||||FFdec.p_field (Upptr)|
||||FFfast.p_field (Upptr)|
||||FFinput.p_field (Upptr)|
||||FFldoffset.p_field (Upptr)|
||||FFloadtuples.p_field (Upptr)|
||||FFpbufcase.p_field (Upptr)|
||||FFsort.p_field (Upptr)|
||||FFsort.p_sortfld (Upptr)|
||||FFstep.p_field (Upptr)|
||||FGconst.p_namefld (Upptr)|
||||FGconst.p_field (Upptr)|
||||FGstatic.p_field (Upptr)|
||||FHook.p_field (Upptr)|
||||FLenfld.p_field (Upptr)|
||||FNs.c_globfld (Ptr)|
||||FNsx.p_pool (Upptr)|
||||FPmaskfld.p_field (Upptr)|
||||FPmaskfldMember.p_field (Upptr)|
||||FPtrary.p_field (Upptr)|
||||FSmallstr.p_field (Upptr)|
||||FSortfld.p_sortfld (Upptr)|
||||FSsimsort.p_sortfld (Upptr)|
||||FSubstr.p_field (Upptr)|
||||FSubstr.p_srcfield (Upptr)|
||||FTary.p_field (Upptr)|
||||FThash.p_hashfld (Upptr)|
||||FThash.p_field (Upptr)|
||||FTypefld.p_field (Upptr)|
||||FXref.p_field (Upptr)|
||||FXref.p_viafld (Upptr)|
||||FXref.p_keyfld (Upptr)|
||||Genctx.p_field (Upptr)|
||||Genpnew.p_optfld (Upptr)|
||||Genpnew.p_varlenfld (Upptr)|
|[amc.FFinput](#amc-ffinput)|[dmmeta.finput](/txt/ssimdb/dmmeta/finput.md)|FDb.finput (Lary)|finput (Lary, by rowid)|
||||FField.c_finput (Ptr)|
|[amc.FFkafka](#amc-ffkafka)|[dmmeta.fkafka](/txt/ssimdb/dmmeta/fkafka.md)|FDb.fkafka (Lary)|fkafka (Lary, by rowid)|
||||FField.c_fkafka (Ptr)|
|[amc.FFldoffset](#amc-ffldoffset)|[dmmeta.fldoffset](/txt/ssimdb/dmmeta/fldoffset.md)|FDb.fldoffset (Lary)|fldoffset (Lary, by rowid)|
||||FField.c_fldoffset (Ptr)|
|[amc.FFloadtuples](#amc-ffloadtuples)|[dmmeta.floadtuples](/txt/ssimdb/dmmeta/floadtuples.md)|FDb.floadtuples (Lary)|floadtuples (Lary, by rowid)|
||||FCtype.c_floadtuples (Ptrary)|
|[amc.FFnoremove](#amc-ffnoremove)|[dmmeta.fnoremove](/txt/ssimdb/dmmeta/fnoremove.md)|FDb.fnoremove (Lary)|fnoremove (Lary, by rowid)|
||||FField.c_fnoremove (Ptr)|
|[amc.FFoutput](#amc-ffoutput)|[dmmeta.foutput](/txt/ssimdb/dmmeta/foutput.md)|FDb.foutput (Lary)|foutput (Lary, by rowid)|
||||FField.c_foutput (Ptr)|
||||FNs.c_foutput (Ptrary)|
|[amc.FFpbuf](#amc-ffpbuf)|[dmmeta.fpbuf](/txt/ssimdb/dmmeta/fpbuf.md)|FDb.fpbuf (Lary)|fpbuf (Lary, by rowid)|
||||FField.c_fpbuf (Ptr)|
|[amc.FFpbufcase](#amc-ffpbufcase)|[dmmeta.fpbufcase](/txt/ssimdb/dmmeta/fpbufcase.md)|FDb.fpbufcase (Lary)|fpbufcase (Lary, by rowid)|
||||FCtype.c_fpbufcase (Ptrary)|
||||FField.c_fpbufcase (Ptr)|
|[amc.FFprefix](#amc-ffprefix)|[dmmeta.fprefix](/txt/ssimdb/dmmeta/fprefix.md)|FDb.fprefix (Lary)|fprefix (Lary, by rowid)|
||||FReftype.zs_fprefix (Llist)|
|[amc.FFregx](#amc-ffregx)|[dmmeta.fregx](/txt/ssimdb/dmmeta/fregx.md)|FDb.fregx (Lary)|fregx (Lary, by rowid)|
||||FField.c_fregx (Ptr)|
|[amc.FFsort](#amc-ffsort)|[dmmeta.fsort](/txt/ssimdb/dmmeta/fsort.md)|FDb.fsort (Lary)|fsort (Lary, by rowid)|
||||FField.c_fsort (Ptr)|
|[amc.FFstep](#amc-ffstep)|[dmmeta.fstep](/txt/ssimdb/dmmeta/fstep.md)|FDb.fstep (Lary)|fstep (Lary, by rowid)|ind_fstep (Thash, hash field fstep)|
||||FField.c_fstep (Ptr)|
||||FNs.c_fstep (Ptrary)|
|[amc.FFtrace](#amc-fftrace)|[dmmeta.ftrace](/txt/ssimdb/dmmeta/ftrace.md)|FDb.ftrace (Lary)|ftrace (Lary, by rowid)|
||||FField.c_ftrace (Ptr)|
|[amc.FFunc](#amc-ffunc)|[dmmeta.func](/txt/ssimdb/dmmeta/func.md)|FDb.func (Lary)|func (Lary, by rowid)|ind_func (Thash, hash field func)|cd_temp_func (Llist)|
||||FCtype.c_init (Ptr)|
||||FCtype.c_ffunc (Ptrary)|
||||FFcmap.c_convfunc (Ptr)|
||||FField.c_ffunc (Ptrary)|
||||FNs.c_func (Ptrary)|
||||Genpnew.p_func (Upptr)|
|[amc.FFunique](#amc-ffunique)|[dmmeta.funique](/txt/ssimdb/dmmeta/funique.md)|FDb.funique (Lary)|funique (Lary, by rowid)|
||||FField.c_funique (Ptr)|
|[amc.FFwddecl](#amc-ffwddecl)|[dmmeta.fwddecl](/txt/ssimdb/dmmeta/fwddecl.md)|FDb.fwddecl (Lary)|fwddecl (Lary, by rowid)|ind_fwddecl (Thash, hash field fwddecl)|
||||FNs.c_fwddecl (Ptrary)|
|[amc.FGconst](#amc-fgconst)|[dmmeta.gconst](/txt/ssimdb/dmmeta/gconst.md)|FDb.gconst (Lary)|gconst (Lary, by rowid)|
||||FField.c_gconst (Ptr)|
|[amc.FGen](#amc-fgen)|[amcdb.gen](/txt/ssimdb/amcdb/gen.md)|FDb.gen (Lary)|**static**|gen (Lary, by rowid)|zs_gen_perns (Llist)|
|[amc.FGenXref](#amc-fgenxref)||
|[amc.FGenXrefSeen](#amc-fgenxrefseen)||FGenXref.seen (Lary)|
||||FGenXref.ind_seen (Thash)|
|[amc.FGstatic](#amc-fgstatic)|[dmmeta.gstatic](/txt/ssimdb/dmmeta/gstatic.md)|FDb.gstatic (Lary)|gstatic (Lary, by rowid)|
||||FField.c_gstatic (Ptr)|
||||FNs.c_gstatic (Ptrary)|
|[amc.FGsymbol](#amc-fgsymbol)|[dmmeta.gsymbol](/txt/ssimdb/dmmeta/gsymbol.md)|FDb.gsymbol (Lary)|gsymbol (Lary, by rowid)|
||||FNs.c_gsymbol (Ptrary)|
|[amc.FHook](#amc-fhook)|[dmmeta.hook](/txt/ssimdb/dmmeta/hook.md)|FDb.hook (Lary)|hook (Lary, by rowid)|
||||FField.c_hook (Ptr)|
|[amc.FInlary](#amc-finlary)|[dmmeta.inlary](/txt/ssimdb/dmmeta/inlary.md)|FDb.inlary (Lary)|inlary (Lary, by rowid)|ind_inlary (Thash, hash field field)|
||||FField.c_inlary (Ptr)|
|[amc.FJstype](#amc-fjstype)|[dmmeta.jstype](/txt/ssimdb/dmmeta/jstype.md)|FDb.jstype (Lary)|jstype (Lary, by rowid)|
||||FCtype.c_jstype (Ptr)|
|[amc.FLenfld](#amc-flenfld)|[dmmeta.lenfld](/txt/ssimdb/dmmeta/lenfld.md)|FDb.lenfld (Lary)|lenfld (Lary, by rowid)|
||||FCtype.c_lenfld (Ptr)|
||||FField.c_lenfld (Ptr)|
|[amc.FLicense](#amc-flicense)|[dev.license](/txt/ssimdb/dev/license.md)|FDb.license (Lary)|license (Lary, by rowid)|ind_license (Thash, hash field license)|
||||FNs.p_license (Upptr)|
|[amc.FListtype](#amc-flisttype)|[dmmeta.listtype](/txt/ssimdb/dmmeta/listtype.md)|FDb.listtype (Lary)|listtype (Lary, by rowid)|ind_listtype (Thash, hash field listtype)|
||||FLlist.p_listtype (Upptr)|
|[amc.FLlist](#amc-fllist)|[dmmeta.llist](/txt/ssimdb/dmmeta/llist.md)|FDb.llist (Lary)|llist (Lary, by rowid)|
||||FField.c_llist (Ptr)|
|[amc.FMain](#amc-fmain)|[dmmeta.main](/txt/ssimdb/dmmeta/main.md)|FDb.main (Lary)|main (Lary, by rowid)|ind_main (Thash, hash field ns)|
||||FNs.c_main (Ptr)|
|[amc.FMsg](#amc-fmsg)|[dmmeta.msg](/txt/ssimdb/dmmeta/msg.md)|FDb.msg (Lary)|msg (Lary, by rowid)|ind_msg_typekey (Thash, hash field typekey)|
||||FCtype.c_msg (Ptr)|
|[amc.FMsgfield](#amc-fmsgfield)|[dmmeta.msgfield](/txt/ssimdb/dmmeta/msgfield.md)|FDb.msgfield (Lary)|msgfield (Lary, by rowid)|
|[amc.FMsgtype](#amc-fmsgtype)|[dmmeta.msgtype](/txt/ssimdb/dmmeta/msgtype.md)|FDb.msgtype (Lary)|msgtype (Lary, by rowid)|
||||FCtype.c_msgtype (Ptr)|
|[amc.FNocascdel](#amc-fnocascdel)|[dmmeta.nocascdel](/txt/ssimdb/dmmeta/nocascdel.md)|FDb.nocascdel (Lary)|nocascdel (Lary, by rowid)|
||||FXref.c_nocascdel (Ptr)|
|[amc.FNossimfile](#amc-fnossimfile)|[dmmeta.nossimfile](/txt/ssimdb/dmmeta/nossimfile.md)|FDb.nossimfile (Lary)|nossimfile (Lary, by rowid)|
||||FCtype.c_nossimfile (Ptr)|
|[amc.FNs](#amc-fns)|[dmmeta.ns](/txt/ssimdb/dmmeta/ns.md)|FDb.ns (Lary)|ns (Lary, by rowid)|ind_ns (Thash, hash field ns)|c_ns_sorted (Ptrary)|c_curns (Ptr)|c_ns (Ptr)|
||||FCtype.p_ns (Upptr)|
||||FDispatch.p_ns (Upptr)|
||||FFunc.p_ns (Upptr)|
||||FNs.c_parentns (Ptrary)|
||||FNs.c_cppincl (Ptrary)|
||||FNs.c_hdrincl (Ptrary)|
||||FOutfile.p_ns (Upptr)|
||||FPnew.p_ns (Upptr)|
||||FTarget.p_ns (Upptr)|
||||Genctx.p_ns (Upptr)|
|[amc.FNscpp](#amc-fnscpp)|[dmmeta.nscpp](/txt/ssimdb/dmmeta/nscpp.md)|FDb.nscpp (Lary)|nscpp (Lary, by rowid)|
||||FNs.c_nscpp (Ptr)|
|[amc.FNsdb](#amc-fnsdb)|[dmmeta.nsdb](/txt/ssimdb/dmmeta/nsdb.md)|FDb.nsdb (Lary)|nsdb (Lary, by rowid)|
||||FNs.c_nsdb (Ptr)|
|[amc.FNsinclude](#amc-fnsinclude)|[dmmeta.nsinclude](/txt/ssimdb/dmmeta/nsinclude.md)|FDb.nsinclude (Lary)|nsinclude (Lary, by rowid)|
||||FNs.c_nsinclude (Ptrary)|
|[amc.FNsjs](#amc-fnsjs)|[dmmeta.nsjs](/txt/ssimdb/dmmeta/nsjs.md)|FDb.nsjs (Lary)|nsjs (Lary, by rowid)|
||||FNs.c_nsjs (Ptr)|
|[amc.FNsproto](#amc-fnsproto)|[dmmeta.nsproto](/txt/ssimdb/dmmeta/nsproto.md)|FDb.nsproto (Lary)|nsproto (Lary, by rowid)|
||||FNs.c_nsproto (Ptr)|
|[amc.FNsx](#amc-fnsx)|[dmmeta.nsx](/txt/ssimdb/dmmeta/nsx.md)|FDb.nsx (Lary)|nsx (Lary, by rowid)|
||||FNs.c_nsx (Ptr)|
|[amc.FNumstr](#amc-fnumstr)|[dmmeta.numstr](/txt/ssimdb/dmmeta/numstr.md)|FDb.numstr (Lary)|numstr (Lary, by rowid)|
||||FSmallstr.c_numstr (Ptr)|
|[amc.FOutfile](#amc-foutfile)||FDb.outfile (Tpool)|
||||FNs.c_outfile (Ptrary)|
|[amc.FPack](#amc-fpack)|[dmmeta.pack](/txt/ssimdb/dmmeta/pack.md)|FDb.pack (Lary)|pack (Lary, by rowid)|ind_pack (Thash, hash field ctype)|
||||FCtype.c_pack (Ptr)|
|[amc.FPayloadhdr](#amc-fpayloadhdr)|[dmmeta.payloadhdr](/txt/ssimdb/dmmeta/payloadhdr.md)|FDb.payloadhdr (Lary)|payloadhdr (Lary, by rowid)|
||||FCtype.c_payloadhdr (Ptr)|
|[amc.FPbsyntax](#amc-fpbsyntax)|[dmmeta.pbsyntax](/txt/ssimdb/dmmeta/pbsyntax.md)|FDb.pbsyntax (Lary)|**static**|pbsyntax (Lary, by rowid)|ind_pbsyntax (Thash, hash field pbsyntax)|
|[amc.FPbtype](#amc-fpbtype)|[amcdb.pbtype](/txt/ssimdb/amcdb/pbtype.md)|FDb.pbtype (Lary)|**static**|pbtype (Lary, by rowid)|ind_pbtype (Thash, hash field pbtype)|
|[amc.FPmaskfld](#amc-fpmaskfld)|[dmmeta.pmaskfld](/txt/ssimdb/dmmeta/pmaskfld.md)|FDb.pmaskfld (Lary)|pmaskfld (Lary, by rowid)|ind_pmaskfld (Thash, hash field field)|
||||FCtype.c_pmaskfld (Ptrary)|
||||FField.c_pmaskfld (Ptr)|
||||FPmaskfldMember.p_pmaskfld (Upptr)|
|[amc.FPmaskfldMember](#amc-fpmaskfldmember)|[dmmeta.pmaskfld_member](/txt/ssimdb/dmmeta/pmaskfld_member.md)|FDb.pmaskfld_member (Lary)|pmaskfld_member (Lary, by rowid)|
||||FField.c_pmaskfld_member (Ptrary)|
||||FPmaskfld.c_pmaskfld_member (Ptrary)|
|[amc.FPnew](#amc-fpnew)|[dmmeta.pnew](/txt/ssimdb/dmmeta/pnew.md)|FDb.pnew (Lary)|pnew (Lary, by rowid)|ind_pnew (Thash, hash field pnew)|
||||FCtype.c_pnew (Ptrary)|
||||FNs.c_pnew (Ptrary)|
|[amc.FPtrary](#amc-fptrary)|[dmmeta.ptrary](/txt/ssimdb/dmmeta/ptrary.md)|FDb.ptrary (Lary)|ptrary (Lary, by rowid)|
||||FField.c_ptrary (Ptr)|
|[amc.FReftype](#amc-freftype)|[dmmeta.reftype](/txt/ssimdb/dmmeta/reftype.md)|FDb.reftype (Lary)|**static**|reftype (Lary, by rowid)|ind_reftype (Thash, hash field reftype)|
||||FField.p_reftype (Upptr)|
|[amc.FRowid](#amc-frowid)|[dmmeta.rowid](/txt/ssimdb/dmmeta/rowid.md)|FDb.rowid (Lary)|rowid (Lary, by rowid)|ind_rowid (Thash, hash field field)|
||||FField.c_rowid (Ptr)|
|[amc.FSmallstr](#amc-fsmallstr)|[dmmeta.smallstr](/txt/ssimdb/dmmeta/smallstr.md)|FDb.smallstr (Lary)|smallstr (Lary, by rowid)|ind_smallstr (Thash, hash field field)|
||||FField.c_smallstr (Ptr)|
|[amc.FSortfld](#amc-fsortfld)|[dmmeta.sortfld](/txt/ssimdb/dmmeta/sortfld.md)|FDb.sortfld (Lary)|sortfld (Lary, by rowid)|
||||FField.c_sortfld (Ptr)|
|[amc.FSsimfile](#amc-fssimfile)|[dmmeta.ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|FDb.ssimfile (Lary)|ssimfile (Lary, by rowid)|ind_ssimfile (Thash, hash field ssimfile)|c_ssimfile_sorted (Ptrary)|zd_ssimfile_todo (Llist)|
||||FCtype.c_ssimfile (Ptr)|
||||FGsymbol.p_ssimfile (Upptr)|
||||FSsimsort.p_ssimfile (Upptr)|
|[amc.FSsimsort](#amc-fssimsort)|[dmmeta.ssimsort](/txt/ssimdb/dmmeta/ssimsort.md)|FDb.ssimsort (Lary)|ssimsort (Lary, by rowid)|
|[amc.FSsimvolatile](#amc-fssimvolatile)|[dmmeta.ssimvolatile](/txt/ssimdb/dmmeta/ssimvolatile.md)|FDb.ssimvolatile (Lary)|ssimvolatile (Lary, by rowid)|
||||FSsimfile.c_ssimvolatile (Ptr)|
|[amc.FStatictuple](#amc-fstatictuple)||FDb.static_tuple (Lary)|static_tuple (Lary, by rowid)|
||||FCtype.c_static (Ptrary)|
|[amc.FSteptype](#amc-fsteptype)|[dmmeta.steptype](/txt/ssimdb/dmmeta/steptype.md)|FDb.steptype (Lary)|steptype (Lary, by rowid)|ind_steptype (Thash, hash field steptype)|
||||FFstep.p_steptype (Upptr)|
|[amc.FSubstr](#amc-fsubstr)|[dmmeta.substr](/txt/ssimdb/dmmeta/substr.md)|FDb.substr (Lary)|substr (Lary, by rowid)|zd_substr_params (Llist)|c_substr_field (Ptrary)|
||||FField.c_substr (Ptr)|
|[amc.FTargdep](#amc-ftargdep)|[dev.targdep](/txt/ssimdb/dev/targdep.md)|FDb.targdep (Lary)|targdep (Lary, by rowid)|
||||FTarget.c_targdep (Ptrary)|
|[amc.FTarget](#amc-ftarget)|[dev.target](/txt/ssimdb/dev/target.md)|FDb.target (Lary)|target (Lary, by rowid)|ind_target (Thash, hash field target)|
||||FNs.c_target (Ptr)|
||||FTargdep.p_target (Upptr)|
||||FTargdep.p_parent (Upptr)|
|[amc.FTary](#amc-ftary)|[dmmeta.tary](/txt/ssimdb/dmmeta/tary.md)|FDb.tary (Lary)|tary (Lary, by rowid)|ind_tary (Thash, hash field field)|
||||FField.c_tary (Ptr)|
|[amc.FTcond](#amc-ftcond)|[amcdb.tcond](/txt/ssimdb/amcdb/tcond.md)|FDb.tcond (Lary)|**static**|tcond (Lary, by rowid)|ind_tcond (Thash, hash field tcond)|
||||FTclass.c_tcond (Ptrary)|
|[amc.FTcurs](#amc-ftcurs)|[amcdb.tcurs](/txt/ssimdb/amcdb/tcurs.md)|FDb.tcurs (Lary)|tcurs (Lary, by rowid)|
||||FTfunc.c_tcurs (Ptr)|
|[amc.FTfunc](#amc-ftfunc)|[amcdb.tfunc](/txt/ssimdb/amcdb/tfunc.md)|FDb.tfunc (Lary)|**static**|tfunc (Lary, by rowid)|ind_tfunc (Thash, hash field tfunc)|
||||FTclass.c_tfunc (Ptrary)|
||||Genctx.p_tfunc (Upptr)|
|[amc.FThash](#amc-fthash)|[dmmeta.thash](/txt/ssimdb/dmmeta/thash.md)|FDb.thash (Lary)|thash (Lary, by rowid)|
||||FField.c_thash (Ptr)|
|[amc.FTracefld](#amc-ftracefld)|[dmmeta.tracefld](/txt/ssimdb/dmmeta/tracefld.md)|FDb.tracefld (Lary)|tracefld (Lary, by rowid)|
|[amc.FTracerec](#amc-ftracerec)|[dmmeta.tracerec](/txt/ssimdb/dmmeta/tracerec.md)|FDb.tracerec (Lary)|tracerec (Lary, by rowid)|
|[amc.FTypefld](#amc-ftypefld)|[dmmeta.typefld](/txt/ssimdb/dmmeta/typefld.md)|FDb.typefld (Lary)|typefld (Lary, by rowid)|
||||FCtype.c_typefld (Ptr)|
||||FField.c_typefld (Ptr)|
|[amc.FUserfunc](#amc-fuserfunc)|[dmmeta.userfunc](/txt/ssimdb/dmmeta/userfunc.md)|FDb.userfunc (Lary)|userfunc (Lary, by rowid)|
|[amc.FUsertracefld](#amc-fusertracefld)|[dmmeta.usertracefld](/txt/ssimdb/dmmeta/usertracefld.md)|FDb.usertracefld (Lary)|usertracefld (Lary, by rowid)|
|[amc.FXref](#amc-fxref)|[dmmeta.xref](/txt/ssimdb/dmmeta/xref.md)|FDb.xref (Lary)|xref (Lary, by rowid)|ind_xref (Thash, hash field field)|
||||FCtype.zs_xref (Llist)|
||||FField.c_xref (Ptr)|
||||FField.zd_xref_keyfld (Llist)|
|[amc.Funcarg](#amc-funcarg)||FFunc.funcarg (Tary)|
|[amc.Genpnew](#amc-genpnew)||
|[amc.Pnewtype](#amc-pnewtype)||

#### amc.BltinId
<a href="#amc-bltinid"></a>

#### amc.BltinId Fields
<a href="#amc-bltinid-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.BltinId.value|u8|[Val](/txt/exe/amc/reftype.md#val)|||

#### Struct BltinId
<a href="#struct-bltinid"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct BltinId { // amc.BltinId
    u8   value;   //   0
    // func:amc.BltinId.value.Cast
    inline               operator amc_BltinIdEnum() const __attribute__((nothrow));
    // func:amc.BltinId..Ctor
    inline               BltinId() __attribute__((nothrow));
    // func:amc.BltinId..FieldwiseCtor
    explicit inline               BltinId(u8 in_value) __attribute__((nothrow));
    // func:amc.BltinId..EnumCtor
    inline               BltinId(amc_BltinIdEnum arg) __attribute__((nothrow));
};
```

#### amc.CppkeywordId
<a href="#amc-cppkeywordid"></a>

#### amc.CppkeywordId Fields
<a href="#amc-cppkeywordid-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.CppkeywordId.id|u32|[Val](/txt/exe/amc/reftype.md#val)|||

#### Struct CppkeywordId
<a href="#struct-cppkeywordid"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct CppkeywordId { // amc.CppkeywordId
    u32   id;   //   0
    // func:amc.CppkeywordId..Ctor
    inline               CppkeywordId() __attribute__((nothrow));
    // func:amc.CppkeywordId..EnumCtor
    inline               CppkeywordId(amc_CppkeywordIdEnum arg) __attribute__((nothrow));
};
```

#### amc.Enumstr - Key
<a href="#amc-enumstr"></a>

#### amc.Enumstr Fields
<a href="#amc-enumstr-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.Enumstr.len|i32|[Val](/txt/exe/amc/reftype.md#val)|||
|amc.Enumstr.str|[algo.RnullStr8](/txt/protocol/algo/README.md#algo-rnullstr8)|[Val](/txt/exe/amc/reftype.md#val)|||

#### Struct Enumstr
<a href="#struct-enumstr"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct Enumstr { // amc.Enumstr: Key
    i32               len;   //   0
    algo::RnullStr8   str;   //
    // func:amc.Enumstr..EqOp
    inline bool          operator ==(const amc::Enumstr &rhs) const __attribute__((nothrow));
    // func:amc.Enumstr..NeOp
    inline bool          operator !=(const amc::Enumstr &rhs) const __attribute__((nothrow));
    // func:amc.Enumstr..LtOp
    inline bool          operator <(const amc::Enumstr &rhs) const __attribute__((nothrow));
    // func:amc.Enumstr..GtOp
    inline bool          operator >(const amc::Enumstr &rhs) const __attribute__((nothrow));
    // func:amc.Enumstr..LeOp
    inline bool          operator <=(const amc::Enumstr &rhs) const __attribute__((nothrow));
    // func:amc.Enumstr..GeOp
    inline bool          operator >=(const amc::Enumstr &rhs) const __attribute__((nothrow));
    // func:amc.Enumstr..Ctor
    inline               Enumstr() __attribute__((nothrow));
    // func:amc.Enumstr..FieldwiseCtor
    explicit inline               Enumstr(i32 in_len, const algo::strptr& in_str) __attribute__((nothrow));
};
```

#### amc.FAnonfld - Omit field name where possible (command line, enums, constants)
<a href="#amc-fanonfld"></a>

#### amc.FAnonfld Fields
<a href="#amc-fanonfld-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FAnonfld.msghdr|[dmmeta.Anonfld](/txt/ssimdb/dmmeta/anonfld.md)|[Base](/txt/ssimdb/dmmeta/anonfld.md)|||
|amc.FAnonfld.anon_idx|i32|[Val](/txt/exe/amc/reftype.md#val)|0|Index of anonymous field (if any)|

#### Struct FAnonfld
<a href="#struct-fanonfld"></a>
*Note:* field ``amc.FAnonfld.msghdr`` has reftype ``base`` so the fields of [dmmeta.Anonfld](/txt/ssimdb/dmmeta/anonfld.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FAnonfld { // amc.FAnonfld
    algo::Smallstr150   field;      //
    algo::cstring       comment;    //
    i32                 anon_idx;   //   0  Index of anonymous field (if any)
    // func:amc.FAnonfld..AssignOp
    inline amc::FAnonfld& operator =(const amc::FAnonfld &rhs) = delete;
    // func:amc.FAnonfld..CopyCtor
    inline               FAnonfld(const amc::FAnonfld &rhs) = delete;
private:
    // func:amc.FAnonfld..Ctor
    inline               FAnonfld() __attribute__((nothrow));
    // func:amc.FAnonfld..Dtor
    inline               ~FAnonfld() __attribute__((nothrow));
    friend amc::FAnonfld&       anonfld_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FAnonfld*       anonfld_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 anonfld_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FArgvtype - Customize parsing of command lines (rarely used)
<a href="#amc-fargvtype"></a>

#### amc.FArgvtype Fields
<a href="#amc-fargvtype-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FArgvtype.base|[dmmeta.Argvtype](/txt/ssimdb/dmmeta/argvtype.md)|[Base](/txt/ssimdb/dmmeta/argvtype.md)|||

#### Struct FArgvtype
<a href="#struct-fargvtype"></a>
*Note:* field ``amc.FArgvtype.base`` has reftype ``base`` so the fields of [dmmeta.Argvtype](/txt/ssimdb/dmmeta/argvtype.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FArgvtype { // amc.FArgvtype
    algo::Smallstr100   ctype;      //
    algo::Smallstr50    argvtype;   //
    algo::cstring       comment;    //
    // func:amc.FArgvtype..AssignOp
    inline amc::FArgvtype& operator =(const amc::FArgvtype &rhs) = delete;
    // func:amc.FArgvtype..CopyCtor
    inline               FArgvtype(const amc::FArgvtype &rhs) = delete;
private:
    // func:amc.FArgvtype..Ctor
    inline               FArgvtype() __attribute__((nothrow));
    // func:amc.FArgvtype..Dtor
    inline               ~FArgvtype() __attribute__((nothrow));
    friend amc::FArgvtype&      argvtype_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FArgvtype*      argvtype_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 argvtype_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FBasepool - Specify pool to be used for allocating elements of a type
<a href="#amc-fbasepool"></a>

#### amc.FBasepool Fields
<a href="#amc-fbasepool-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FBasepool.msghdr|[dmmeta.Basepool](/txt/ssimdb/dmmeta/basepool.md)|[Base](/txt/ssimdb/dmmeta/basepool.md)|||
|amc.FBasepool.p_field|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|amc.FBasepool.p_base|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||

#### Struct FBasepool
<a href="#struct-fbasepool"></a>
*Note:* field ``amc.FBasepool.msghdr`` has reftype ``base`` so the fields of [dmmeta.Basepool](/txt/ssimdb/dmmeta/basepool.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FBasepool { // amc.FBasepool
    algo::Smallstr150   field;     //
    algo::Smallstr150   base;      //
    amc::FField*        p_field;   // reference to parent row
    amc::FField*        p_base;    // reference to parent row
    // x-reference on amc.FBasepool.p_field prevents copy
    // x-reference on amc.FBasepool.p_base prevents copy
    // func:amc.FBasepool..AssignOp
    inline amc::FBasepool& operator =(const amc::FBasepool &rhs) = delete;
    // x-reference on amc.FBasepool.p_field prevents copy
    // x-reference on amc.FBasepool.p_base prevents copy
    // func:amc.FBasepool..CopyCtor
    inline               FBasepool(const amc::FBasepool &rhs) = delete;
private:
    // func:amc.FBasepool..Ctor
    inline               FBasepool() __attribute__((nothrow));
    // func:amc.FBasepool..Dtor
    inline               ~FBasepool() __attribute__((nothrow));
    friend amc::FBasepool&      basepool_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FBasepool*      basepool_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 basepool_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FBitfld - Specify offset/width of a bitfield within another field
<a href="#amc-fbitfld"></a>

#### amc.FBitfld Fields
<a href="#amc-fbitfld-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FBitfld.msghdr|[dmmeta.Bitfld](/txt/ssimdb/dmmeta/bitfld.md)|[Base](/txt/ssimdb/dmmeta/bitfld.md)|||
|amc.FBitfld.p_srcfield|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|amc.FBitfld.p_field|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||

#### Struct FBitfld
<a href="#struct-fbitfld"></a>
*Note:* field ``amc.FBitfld.msghdr`` has reftype ``base`` so the fields of [dmmeta.Bitfld](/txt/ssimdb/dmmeta/bitfld.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FBitfld { // amc.FBitfld
    algo::Smallstr150   field;                 //
    i32                 offset;                //   0  Offset, in bits, within parent field
    i32                 width;                 //   0  Width, in bits, within parent field.
    algo::Smallstr150   srcfield;              //
    algo::cstring       comment;               //
    amc::FField*        p_srcfield;            // reference to parent row
    amc::FField*        p_field;               // reference to parent row
    i32                 field_bh_bitfld_idx;   // index in heap; -1 means not-in-heap
    // x-reference on amc.FBitfld.p_srcfield prevents copy
    // x-reference on amc.FBitfld.p_field prevents copy
    // func:amc.FBitfld..AssignOp
    inline amc::FBitfld& operator =(const amc::FBitfld &rhs) = delete;
    // x-reference on amc.FBitfld.p_srcfield prevents copy
    // x-reference on amc.FBitfld.p_field prevents copy
    // func:amc.FBitfld..CopyCtor
    inline               FBitfld(const amc::FBitfld &rhs) = delete;
private:
    // func:amc.FBitfld..Ctor
    inline               FBitfld() __attribute__((nothrow));
    // func:amc.FBitfld..Dtor
    inline               ~FBitfld() __attribute__((nothrow));
    friend amc::FBitfld&        bitfld_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FBitfld*        bitfld_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 bitfld_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FBlkhash - Block hash index, required for fields with reftype Blkhash
<a href="#amc-fblkhash"></a>

#### amc.FBlkhash Fields
<a href="#amc-fblkhash-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FBlkhash.msghdr|[dmmeta.Blkhash](/txt/ssimdb/dmmeta/blkhash.md)|[Base](/txt/ssimdb/dmmeta/blkhash.md)|||
|amc.FBlkhash.p_hashfld|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|amc.FBlkhash.p_linfld|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|amc.FBlkhash.p_field|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||

#### Struct FBlkhash
<a href="#struct-fblkhash"></a>
*Note:* field ``amc.FBlkhash.msghdr`` has reftype ``base`` so the fields of [dmmeta.Blkhash](/txt/ssimdb/dmmeta/blkhash.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FBlkhash { // amc.FBlkhash
    algo::Smallstr150   field;       //
    algo::Smallstr150   hashfld;     // Key field; blocks are keyed on it with linfld low bits masked off
    algo::Smallstr150   linfld;      // Dense integer component of the key; a field of hashfld's arg
    u8                  linbits;     //   0  Block holds 2^linbits slots; low linfld bits select the slot directly
    algo::cstring       comment;     //
    amc::FField*        p_hashfld;   // reference to parent row
    amc::FField*        p_linfld;    // reference to parent row
    amc::FField*        p_field;     // reference to parent row
    // x-reference on amc.FBlkhash.p_hashfld prevents copy
    // x-reference on amc.FBlkhash.p_linfld prevents copy
    // x-reference on amc.FBlkhash.p_field prevents copy
    // func:amc.FBlkhash..AssignOp
    inline amc::FBlkhash& operator =(const amc::FBlkhash &rhs) = delete;
    // x-reference on amc.FBlkhash.p_hashfld prevents copy
    // x-reference on amc.FBlkhash.p_linfld prevents copy
    // x-reference on amc.FBlkhash.p_field prevents copy
    // func:amc.FBlkhash..CopyCtor
    inline               FBlkhash(const amc::FBlkhash &rhs) = delete;
private:
    // func:amc.FBlkhash..Ctor
    inline               FBlkhash() __attribute__((nothrow));
    // func:amc.FBlkhash..Dtor
    inline               ~FBlkhash() __attribute__((nothrow));
    friend amc::FBlkhash&       blkhash_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FBlkhash*       blkhash_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 blkhash_RemoveAll() __attribute__((nothrow));
    friend void                 blkhash_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FBltin - Specify properties of a C built-in type
<a href="#amc-fbltin"></a>

#### amc.FBltin Fields
<a href="#amc-fbltin-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FBltin.msghdr|[amcdb.Bltin](/txt/ssimdb/amcdb/bltin.md)|[Base](/txt/ssimdb/amcdb/bltin.md)|||

#### Struct FBltin
<a href="#struct-fbltin"></a>
*Note:* field ``amc.FBltin.msghdr`` has reftype ``base`` so the fields of [amcdb.Bltin](/txt/ssimdb/amcdb/bltin.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FBltin { // amc.FBltin
    algo::Smallstr100   ctype;               //
    bool                likeu64;             //   false
    bool                bigendok;            //   false
    bool                issigned;            //   false
    algo::cstring       comment;             //
    amc::FBltin*        ind_bltin_next;      // hash next
    u32                 ind_bltin_hashval;   // hash value
    // func:amc.FBltin..AssignOp
    inline amc::FBltin&  operator =(const amc::FBltin &rhs) = delete;
    // func:amc.FBltin..CopyCtor
    inline               FBltin(const amc::FBltin &rhs) = delete;
private:
    // func:amc.FBltin..Ctor
    inline               FBltin() __attribute__((nothrow));
    // func:amc.FBltin..Dtor
    inline               ~FBltin() __attribute__((nothrow));
    friend amc::FBltin&         bltin_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FBltin*         bltin_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 bltin_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FCafter - Control amc processing order for unrelated types (used in rare situations)
<a href="#amc-fcafter"></a>

#### amc.FCafter Fields
<a href="#amc-fcafter-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FCafter.msghdr|[dmmeta.Cafter](/txt/ssimdb/dmmeta/cafter.md)|[Base](/txt/ssimdb/dmmeta/cafter.md)|||
|amc.FCafter.p_after|[amc.FCtype](/txt/gen/amc/amc.md#amc-fctype)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||

#### Struct FCafter
<a href="#struct-fcafter"></a>
*Note:* field ``amc.FCafter.msghdr`` has reftype ``base`` so the fields of [dmmeta.Cafter](/txt/ssimdb/dmmeta/cafter.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FCafter { // amc.FCafter
    algo::Smallstr50   cafter;                 //
    algo::cstring      comment;                //
    amc::FCtype*       p_after;                // reference to parent row
    amc::FCafter*      ctype_zd_cafter_next;   // zslist link; -1 means not-in-list
    amc::FCafter*      ctype_zd_cafter_prev;   // previous element
    // x-reference on amc.FCafter.p_after prevents copy
    // func:amc.FCafter..AssignOp
    inline amc::FCafter& operator =(const amc::FCafter &rhs) = delete;
    // x-reference on amc.FCafter.p_after prevents copy
    // func:amc.FCafter..CopyCtor
    inline               FCafter(const amc::FCafter &rhs) = delete;
private:
    // func:amc.FCafter..Ctor
    inline               FCafter() __attribute__((nothrow));
    // func:amc.FCafter..Dtor
    inline               ~FCafter() __attribute__((nothrow));
    friend amc::FCafter&        cafter_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FCafter*        cafter_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 cafter_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FCascdel - Request cascading delete of referenced rows
<a href="#amc-fcascdel"></a>

#### amc.FCascdel Fields
<a href="#amc-fcascdel-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FCascdel.msghdr|[dmmeta.Cascdel](/txt/ssimdb/dmmeta/cascdel.md)|[Base](/txt/ssimdb/dmmeta/cascdel.md)|||
|amc.FCascdel.p_field|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||

#### Struct FCascdel
<a href="#struct-fcascdel"></a>
*Note:* field ``amc.FCascdel.msghdr`` has reftype ``base`` so the fields of [dmmeta.Cascdel](/txt/ssimdb/dmmeta/cascdel.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FCascdel { // amc.FCascdel
    algo::Smallstr150   field;     //
    algo::cstring       comment;   //
    amc::FField*        p_field;   // reference to parent row
    // x-reference on amc.FCascdel.p_field prevents copy
    // func:amc.FCascdel..AssignOp
    inline amc::FCascdel& operator =(const amc::FCascdel &rhs) = delete;
    // x-reference on amc.FCascdel.p_field prevents copy
    // func:amc.FCascdel..CopyCtor
    inline               FCascdel(const amc::FCascdel &rhs) = delete;
private:
    // func:amc.FCascdel..Ctor
    inline               FCascdel() __attribute__((nothrow));
    // func:amc.FCascdel..Dtor
    inline               ~FCascdel() __attribute__((nothrow));
    friend amc::FCascdel&       cascdel_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FCascdel*       cascdel_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 cascdel_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FCcmdline - tie cmdline to basecmdline
<a href="#amc-fccmdline"></a>

#### amc.FCcmdline Fields
<a href="#amc-fccmdline-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FCcmdline.base|[dmmeta.Ccmdline](/txt/ssimdb/dmmeta/ccmdline.md)|[Base](/txt/ssimdb/dmmeta/ccmdline.md)|||
|amc.FCcmdline.p_ctype|[amc.FCtype](/txt/gen/amc/amc.md#amc-fctype)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|amc.FCcmdline.p_basecmdline|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||

#### Struct FCcmdline
<a href="#struct-fccmdline"></a>
*Note:* field ``amc.FCcmdline.base`` has reftype ``base`` so the fields of [dmmeta.Ccmdline](/txt/ssimdb/dmmeta/ccmdline.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FCcmdline { // amc.FCcmdline
    algo::Smallstr100   ctype;           //
    bool                read;            //   true  Read the command line automatically from main?
    algo::Smallstr150   basecmdline;     // Optional command line to merge with this one
    algo::cstring       comment;         //
    amc::FCtype*        p_ctype;         // reference to parent row
    amc::FField*        p_basecmdline;   // reference to parent row
    // x-reference on amc.FCcmdline.p_ctype prevents copy
    // x-reference on amc.FCcmdline.p_basecmdline prevents copy
    // func:amc.FCcmdline..AssignOp
    inline amc::FCcmdline& operator =(const amc::FCcmdline &rhs) = delete;
    // x-reference on amc.FCcmdline.p_ctype prevents copy
    // x-reference on amc.FCcmdline.p_basecmdline prevents copy
    // func:amc.FCcmdline..CopyCtor
    inline               FCcmdline(const amc::FCcmdline &rhs) = delete;
private:
    // func:amc.FCcmdline..Ctor
    inline               FCcmdline() __attribute__((nothrow));
    // func:amc.FCcmdline..Dtor
    inline               ~FCcmdline() __attribute__((nothrow));
    friend amc::FCcmdline&      ccmdline_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FCcmdline*      ccmdline_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 ccmdline_RemoveAll() __attribute__((nothrow));
    friend void                 ccmdline_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FCcmp - Generate hash function
<a href="#amc-fccmp"></a>

#### amc.FCcmp Fields
<a href="#amc-fccmp-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FCcmp.msghdr|[dmmeta.Ccmp](/txt/ssimdb/dmmeta/ccmp.md)|[Base](/txt/ssimdb/dmmeta/ccmp.md)|||

#### Struct FCcmp
<a href="#struct-fccmp"></a>
*Note:* field ``amc.FCcmp.msghdr`` has reftype ``base`` so the fields of [dmmeta.Ccmp](/txt/ssimdb/dmmeta/ccmp.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FCcmp { // amc.FCcmp
    algo::Smallstr100   ctype;              // Target ctype
    bool                extrn;              //   false  Whether implementation is external
    bool                genop;              //   false  Generate C++ comparison operators (<,>, etc)
    bool                order;              //   false
    bool                minmax;             //   false
    algo::cstring       comment;            //
    amc::FCcmp*         ind_ccmp_next;      // hash next
    u32                 ind_ccmp_hashval;   // hash value
    // func:amc.FCcmp..AssignOp
    inline amc::FCcmp&   operator =(const amc::FCcmp &rhs) = delete;
    // func:amc.FCcmp..CopyCtor
    inline               FCcmp(const amc::FCcmp &rhs) = delete;
private:
    // func:amc.FCcmp..Ctor
    inline               FCcmp() __attribute__((nothrow));
    // func:amc.FCcmp..Dtor
    inline               ~FCcmp() __attribute__((nothrow));
    friend amc::FCcmp&          ccmp_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FCcmp*          ccmp_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 ccmp_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FCdflt - Specify default value for single-value types that lack fields
<a href="#amc-fcdflt"></a>

#### amc.FCdflt Fields
<a href="#amc-fcdflt-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FCdflt.base|[dmmeta.Cdflt](/txt/ssimdb/dmmeta/cdflt.md)|[Base](/txt/ssimdb/dmmeta/cdflt.md)|||

#### Struct FCdflt
<a href="#struct-fcdflt"></a>
*Note:* field ``amc.FCdflt.base`` has reftype ``base`` so the fields of [dmmeta.Cdflt](/txt/ssimdb/dmmeta/cdflt.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FCdflt { // amc.FCdflt
    algo::Smallstr100   ctype;      //
    algo::CppExpr       dflt;       //
    algo::CppExpr       cppdflt;    //
    algo::Smallstr50    ssimdflt;   //
    algo::cstring       comment;    //
    // func:amc.FCdflt..AssignOp
    inline amc::FCdflt&  operator =(const amc::FCdflt &rhs) = delete;
    // func:amc.FCdflt..CopyCtor
    inline               FCdflt(const amc::FCdflt &rhs) = delete;
private:
    // func:amc.FCdflt..Ctor
    inline               FCdflt() __attribute__((nothrow));
    // func:amc.FCdflt..Dtor
    inline               ~FCdflt() __attribute__((nothrow));
    friend amc::FCdflt&         cdflt_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FCdflt*         cdflt_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 cdflt_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FCextern - Externally defined ctype (a struct from system header, or from a c++ library)
<a href="#amc-fcextern"></a>

#### amc.FCextern Fields
<a href="#amc-fcextern-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FCextern.msghdr|[dmmeta.Cextern](/txt/ssimdb/dmmeta/cextern.md)|[Base](/txt/ssimdb/dmmeta/cextern.md)|||

#### Struct FCextern
<a href="#struct-fcextern"></a>
*Note:* field ``amc.FCextern.msghdr`` has reftype ``base`` so the fields of [dmmeta.Cextern](/txt/ssimdb/dmmeta/cextern.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FCextern { // amc.FCextern
    algo::Smallstr100   ctype;        // Ctype in question
    bool                initmemset;   //   false  Initialize using memset? (set this to Y for all C structs)
    bool                isstruct;     //   false
    bool                plaindata;    //   false
    // func:amc.FCextern..AssignOp
    inline amc::FCextern& operator =(const amc::FCextern &rhs) = delete;
    // func:amc.FCextern..CopyCtor
    inline               FCextern(const amc::FCextern &rhs) = delete;
private:
    // func:amc.FCextern..Ctor
    inline               FCextern() __attribute__((nothrow));
    // func:amc.FCextern..Dtor
    inline               ~FCextern() __attribute__((nothrow));
    friend amc::FCextern&       cextern_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FCextern*       cextern_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 cextern_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FCfast - FAST properties for ctype
<a href="#amc-fcfast"></a>

#### amc.FCfast Fields
<a href="#amc-fcfast-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FCfast.base|[dmmeta.Cfast](/txt/ssimdb/dmmeta/cfast.md)|[Base](/txt/ssimdb/dmmeta/cfast.md)|||

#### Struct FCfast
<a href="#struct-fcfast"></a>
*Note:* field ``amc.FCfast.base`` has reftype ``base`` so the fields of [dmmeta.Cfast](/txt/ssimdb/dmmeta/cfast.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FCfast { // amc.FCfast
    algo::Smallstr100   ctype;      //
    u32                 id;         //   0
    algo::Smallstr50    encoding;   //
    bool                reset;      //   false
    algo::cstring       comment;    //
    // func:amc.FCfast..AssignOp
    inline amc::FCfast&  operator =(const amc::FCfast &rhs) = delete;
    // func:amc.FCfast..CopyCtor
    inline               FCfast(const amc::FCfast &rhs) = delete;
private:
    // func:amc.FCfast..Ctor
    inline               FCfast() __attribute__((nothrow));
    // func:amc.FCfast..Dtor
    inline               ~FCfast() __attribute__((nothrow));
    friend amc::FCfast&         cfast_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FCfast*         cfast_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 cfast_RemoveAll() __attribute__((nothrow));
    friend void                 cfast_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FCfmt - Specify options for printing/reading ctypes into multiple formats
<a href="#amc-fcfmt"></a>

#### amc.FCfmt Fields
<a href="#amc-fcfmt-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FCfmt.msghdr|[dmmeta.Cfmt](/txt/ssimdb/dmmeta/cfmt.md)|[Base](/txt/ssimdb/dmmeta/cfmt.md)|||
|amc.FCfmt.p_ctype|[amc.FCtype](/txt/gen/amc/amc.md#amc-fctype)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||

#### Struct FCfmt
<a href="#struct-fcfmt"></a>
*Note:* field ``amc.FCfmt.msghdr`` has reftype ``base`` so the fields of [dmmeta.Cfmt](/txt/ssimdb/dmmeta/cfmt.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FCfmt { // amc.FCfmt
    algo::Smallstr100   cfmt;                 //
    algo::Smallstr50    printfmt;             //
    bool                read;                 //   false
    bool                print;                //   false
    algo::Smallstr20    sep;                  //
    bool                genop;                //   false
    algo::cstring       comment;              //
    amc::FCtype*        p_ctype;              // reference to parent row
    amc::FCfmt*         ctype_zs_cfmt_next;   // zslist link; -1 means not-in-list
    amc::FCfmt*         ind_cfmt_next;        // hash next
    u32                 ind_cfmt_hashval;     // hash value
    // x-reference on amc.FCfmt.p_ctype prevents copy
    // func:amc.FCfmt..AssignOp
    amc::FCfmt&          operator =(const amc::FCfmt &rhs) = delete;
    // x-reference on amc.FCfmt.p_ctype prevents copy
    // func:amc.FCfmt..CopyCtor
    FCfmt(const amc::FCfmt &rhs) = delete;
private:
    // func:amc.FCfmt..Ctor
    inline               FCfmt() __attribute__((nothrow));
    // func:amc.FCfmt..Dtor
    inline               ~FCfmt() __attribute__((nothrow));
    friend amc::FCfmt&          cfmt_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FCfmt*          cfmt_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 cfmt_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FCget - Generate state functions for these ctypes
<a href="#amc-fcget"></a>

#### amc.FCget Fields
<a href="#amc-fcget-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FCget.base|[dmmeta.Cget](/txt/ssimdb/dmmeta/cget.md)|[Base](/txt/ssimdb/dmmeta/cget.md)|||
|amc.FCget.p_ctype|[amc.FCtype](/txt/gen/amc/amc.md#amc-fctype)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||

#### Struct FCget
<a href="#struct-fcget"></a>
*Note:* field ``amc.FCget.base`` has reftype ``base`` so the fields of [dmmeta.Cget](/txt/ssimdb/dmmeta/cget.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FCget { // amc.FCget
    algo::Smallstr100   ctype;     //
    algo::cstring       comment;   //
    amc::FCtype*        p_ctype;   // reference to parent row
private:
    // func:amc.FCget..Ctor
    inline               FCget() __attribute__((nothrow));
    friend amc::FCget&          cget_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FCget*          cget_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 cget_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FCharset - Generate functions to determine if a character is a member of a set
<a href="#amc-fcharset"></a>

#### amc.FCharset Fields
<a href="#amc-fcharset-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FCharset.base|[dmmeta.Charset](/txt/ssimdb/dmmeta/charset.md)|[Base](/txt/ssimdb/dmmeta/charset.md)|||
|amc.FCharset.chars|[algo.Charset](/txt/protocol/algo/Charset.md)|[Val](/txt/exe/amc/reftype.md#val)|||

#### Struct FCharset
<a href="#struct-fcharset"></a>
*Note:* field ``amc.FCharset.base`` has reftype ``base`` so the fields of [dmmeta.Charset](/txt/ssimdb/dmmeta/charset.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FCharset { // amc.FCharset
    algo::Smallstr150   field;       //
    algo::Smallstr100   expr;        //
    bool                charrange;   //   false
    bool                calc;        //   false
    algo::cstring       comment;     //
    algo::Charset       chars;       //
    // func:amc.FCharset..AssignOp
    inline amc::FCharset& operator =(const amc::FCharset &rhs) = delete;
    // func:amc.FCharset..CopyCtor
    inline               FCharset(const amc::FCharset &rhs) = delete;
private:
    // func:amc.FCharset..Ctor
    inline               FCharset() __attribute__((nothrow));
    // func:amc.FCharset..Dtor
    inline               ~FCharset() __attribute__((nothrow));
    friend amc::FCharset&       charset_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FCharset*       charset_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 charset_RemoveAll() __attribute__((nothrow));
    friend void                 charset_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FChash - Generate hash function
<a href="#amc-fchash"></a>

#### amc.FChash Fields
<a href="#amc-fchash-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FChash.msghdr|[dmmeta.Chash](/txt/ssimdb/dmmeta/chash.md)|[Base](/txt/ssimdb/dmmeta/chash.md)|||

#### Struct FChash
<a href="#struct-fchash"></a>
*Note:* field ``amc.FChash.msghdr`` has reftype ``base`` so the fields of [dmmeta.Chash](/txt/ssimdb/dmmeta/chash.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FChash { // amc.FChash
    algo::Smallstr100   ctype;               // Target ctype
    algo::Smallstr50    hashtype;            // Hash type
    algo::cstring       comment;             //
    amc::FChash*        ind_chash_next;      // hash next
    u32                 ind_chash_hashval;   // hash value
    // func:amc.FChash..AssignOp
    inline amc::FChash&  operator =(const amc::FChash &rhs) = delete;
    // func:amc.FChash..CopyCtor
    inline               FChash(const amc::FChash &rhs) = delete;
private:
    // func:amc.FChash..Ctor
    inline               FChash() __attribute__((nothrow));
    // func:amc.FChash..Dtor
    inline               ~FChash() __attribute__((nothrow));
    friend amc::FChash&         chash_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FChash*         chash_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 chash_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FCjsbltin
<a href="#amc-fcjsbltin"></a>

#### amc.FCjsbltin Fields
<a href="#amc-fcjsbltin-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FCjsbltin.base|[dmmeta.Cjsbltin](/txt/ssimdb/dmmeta/cjsbltin.md)|[Base](/txt/ssimdb/dmmeta/cjsbltin.md)|||
|amc.FCjsbltin.p_jsbltin|[amc.FJsbltin](/txt/gen/amc/amc.md#amc-fjsbltin)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||

#### Struct FCjsbltin
<a href="#struct-fcjsbltin"></a>
*Note:* field ``amc.FCjsbltin.base`` has reftype ``base`` so the fields of [dmmeta.Cjsbltin](/txt/ssimdb/dmmeta/cjsbltin.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FCjsbltin { // amc.FCjsbltin
    algo::Smallstr100   ctype;       //
    algo::Smallstr50    jsbltin;     //
    algo::cstring       comment;     //
    amc::FJsbltin*      p_jsbltin;   // reference to parent row
    // x-reference on amc.FCjsbltin.p_jsbltin prevents copy
    // func:amc.FCjsbltin..AssignOp
    inline amc::FCjsbltin& operator =(const amc::FCjsbltin &rhs) = delete;
    // x-reference on amc.FCjsbltin.p_jsbltin prevents copy
    // func:amc.FCjsbltin..CopyCtor
    inline               FCjsbltin(const amc::FCjsbltin &rhs) = delete;
private:
    // func:amc.FCjsbltin..Ctor
    inline               FCjsbltin() __attribute__((nothrow));
    // func:amc.FCjsbltin..Dtor
    inline               ~FCjsbltin() __attribute__((nothrow));
    friend amc::FCjsbltin&      cjsbltin_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FCjsbltin*      cjsbltin_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 cjsbltin_RemoveAll() __attribute__((nothrow));
    friend void                 cjsbltin_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FCjson
<a href="#amc-fcjson"></a>

#### amc.FCjson Fields
<a href="#amc-fcjson-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FCjson.base|[dmmeta.Cjson](/txt/ssimdb/dmmeta/cjson.md)|[Base](/txt/ssimdb/dmmeta/cjson.md)|||
|amc.FCjson.p_jsbltin|[amc.FJsbltin](/txt/gen/amc/amc.md#amc-fjsbltin)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||

#### Struct FCjson
<a href="#struct-fcjson"></a>
*Note:* field ``amc.FCjson.base`` has reftype ``base`` so the fields of [dmmeta.Cjson](/txt/ssimdb/dmmeta/cjson.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FCjson { // amc.FCjson
    algo::Smallstr100   ctype;       //
    algo::Smallstr100   jsbltin;     //
    algo::cstring       comment;     //
    amc::FJsbltin*      p_jsbltin;   // reference to parent row
    // x-reference on amc.FCjson.p_jsbltin prevents copy
    // func:amc.FCjson..AssignOp
    inline amc::FCjson&  operator =(const amc::FCjson &rhs) = delete;
    // x-reference on amc.FCjson.p_jsbltin prevents copy
    // func:amc.FCjson..CopyCtor
    inline               FCjson(const amc::FCjson &rhs) = delete;
private:
    // func:amc.FCjson..Ctor
    inline               FCjson() __attribute__((nothrow));
    // func:amc.FCjson..Dtor
    inline               ~FCjson() __attribute__((nothrow));
    friend amc::FCjson&         cjson_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FCjson*         cjson_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 cjson_RemoveAll() __attribute__((nothrow));
    friend void                 cjson_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FCkafka
<a href="#amc-fckafka"></a>

#### amc.FCkafka Fields
<a href="#amc-fckafka-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FCkafka.base|[dmmeta.Ckafka](/txt/ssimdb/dmmeta/ckafka.md)|[Base](/txt/ssimdb/dmmeta/ckafka.md)|||

#### Struct FCkafka
<a href="#struct-fckafka"></a>
*Note:* field ``amc.FCkafka.base`` has reftype ``base`` so the fields of [dmmeta.Ckafka](/txt/ssimdb/dmmeta/ckafka.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FCkafka { // amc.FCkafka
    algo::Smallstr100   ctype;               //
    algo::Smallstr50    kind;                //
    algo::cstring       root;                //
    algo::Smallstr10    valid_versions;      //
    algo::Smallstr10    flexible_versions;   //
    algo::cstring       comment;             //
    // func:amc.FCkafka..AssignOp
    inline amc::FCkafka& operator =(const amc::FCkafka &rhs) = delete;
    // func:amc.FCkafka..CopyCtor
    inline               FCkafka(const amc::FCkafka &rhs) = delete;
private:
    // func:amc.FCkafka..Ctor
    inline               FCkafka() __attribute__((nothrow));
    // func:amc.FCkafka..Dtor
    inline               ~FCkafka() __attribute__((nothrow));
    friend amc::FCkafka&        ckafka_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FCkafka*        ckafka_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 ckafka_RemoveAll() __attribute__((nothrow));
    friend void                 ckafka_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FCpbuf
<a href="#amc-fcpbuf"></a>

#### amc.FCpbuf Fields
<a href="#amc-fcpbuf-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FCpbuf.base|[dmmeta.Cpbuf](/txt/ssimdb/dmmeta/cpbuf.md)|[Base](/txt/ssimdb/dmmeta/cpbuf.md)|||

#### Struct FCpbuf
<a href="#struct-fcpbuf"></a>
*Note:* field ``amc.FCpbuf.base`` has reftype ``base`` so the fields of [dmmeta.Cpbuf](/txt/ssimdb/dmmeta/cpbuf.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FCpbuf { // amc.FCpbuf
    algo::Smallstr100   ctype;     //
    algo::Smallstr50    syntax;    // Protobuf syntax variant this message follows
    algo::cstring       comment;   //
    // func:amc.FCpbuf..AssignOp
    inline amc::FCpbuf&  operator =(const amc::FCpbuf &rhs) = delete;
    // func:amc.FCpbuf..CopyCtor
    inline               FCpbuf(const amc::FCpbuf &rhs) = delete;
private:
    // func:amc.FCpbuf..Ctor
    inline               FCpbuf() __attribute__((nothrow));
    // func:amc.FCpbuf..Dtor
    inline               ~FCpbuf() __attribute__((nothrow));
    friend amc::FCpbuf&         cpbuf_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FCpbuf*         cpbuf_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 cpbuf_RemoveAll() __attribute__((nothrow));
    friend void                 cpbuf_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FCppfunc - Value of field provided by this expression
<a href="#amc-fcppfunc"></a>

#### amc.FCppfunc Fields
<a href="#amc-fcppfunc-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FCppfunc.msghdr|[dmmeta.Cppfunc](/txt/ssimdb/dmmeta/cppfunc.md)|[Base](/txt/ssimdb/dmmeta/cppfunc.md)|||

#### Struct FCppfunc
<a href="#struct-fcppfunc"></a>
*Note:* field ``amc.FCppfunc.msghdr`` has reftype ``base`` so the fields of [dmmeta.Cppfunc](/txt/ssimdb/dmmeta/cppfunc.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FCppfunc { // amc.FCppfunc
    algo::Smallstr150   field;   //
    algo::CppExpr       expr;    //
    bool                print;   //   false
    bool                set;     //   false
    // func:amc.FCppfunc..AssignOp
    inline amc::FCppfunc& operator =(const amc::FCppfunc &rhs) = delete;
    // func:amc.FCppfunc..CopyCtor
    inline               FCppfunc(const amc::FCppfunc &rhs) = delete;
private:
    // func:amc.FCppfunc..Ctor
    inline               FCppfunc() __attribute__((nothrow));
    // func:amc.FCppfunc..Dtor
    inline               ~FCppfunc() __attribute__((nothrow));
    friend amc::FCppfunc&       cppfunc_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FCppfunc*       cppfunc_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 cppfunc_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FCpptype - Ask amc for a fieldwise constructor
<a href="#amc-fcpptype"></a>

#### amc.FCpptype Fields
<a href="#amc-fcpptype-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FCpptype.msghdr|[dmmeta.Cpptype](/txt/ssimdb/dmmeta/cpptype.md)|[Base](/txt/ssimdb/dmmeta/cpptype.md)|||
|amc.FCpptype.p_ctype|[amc.FCtype](/txt/gen/amc/amc.md#amc-fctype)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||

#### Struct FCpptype
<a href="#struct-fcpptype"></a>
*Note:* field ``amc.FCpptype.msghdr`` has reftype ``base`` so the fields of [dmmeta.Cpptype](/txt/ssimdb/dmmeta/cpptype.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FCpptype { // amc.FCpptype
    algo::Smallstr100   ctype;                 //
    bool                ctor;                  //   false  if true, generate non-default constructor from all fields
    amc::FCtype*        p_ctype;               // reference to parent row
    amc::FCpptype*      ind_cpptype_next;      // hash next
    u32                 ind_cpptype_hashval;   // hash value
    // x-reference on amc.FCpptype.p_ctype prevents copy
    // func:amc.FCpptype..AssignOp
    inline amc::FCpptype& operator =(const amc::FCpptype &rhs) = delete;
    // x-reference on amc.FCpptype.p_ctype prevents copy
    // func:amc.FCpptype..CopyCtor
    inline               FCpptype(const amc::FCpptype &rhs) = delete;
private:
    // func:amc.FCpptype..Ctor
    inline               FCpptype() __attribute__((nothrow));
    // func:amc.FCpptype..Dtor
    inline               ~FCpptype() __attribute__((nothrow));
    friend amc::FCpptype&       cpptype_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FCpptype*       cpptype_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 cpptype_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FCsize - Specify size/alignment for built-in C++ types
<a href="#amc-fcsize"></a>

#### amc.FCsize Fields
<a href="#amc-fcsize-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FCsize.msghdr|[dmmeta.Csize](/txt/ssimdb/dmmeta/csize.md)|[Base](/txt/ssimdb/dmmeta/csize.md)|||

#### Struct FCsize
<a href="#struct-fcsize"></a>
*Note:* field ``amc.FCsize.msghdr`` has reftype ``base`` so the fields of [dmmeta.Csize](/txt/ssimdb/dmmeta/csize.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FCsize { // amc.FCsize
    algo::Smallstr100   ctype;       //
    u32                 size;        //   0
    u32                 alignment;   //   1
    algo::cstring       comment;     //
    // func:amc.FCsize..AssignOp
    inline amc::FCsize&  operator =(const amc::FCsize &rhs) = delete;
    // func:amc.FCsize..CopyCtor
    inline               FCsize(const amc::FCsize &rhs) = delete;
private:
    // func:amc.FCsize..Ctor
    inline               FCsize() __attribute__((nothrow));
    // func:amc.FCsize..Dtor
    inline               ~FCsize() __attribute__((nothrow));
    friend amc::FCsize&         csize_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FCsize*         csize_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 csize_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FCstr - Specify that type behaves like a string
<a href="#amc-fcstr"></a>

#### amc.FCstr Fields
<a href="#amc-fcstr-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FCstr.msghdr|[dmmeta.Cstr](/txt/ssimdb/dmmeta/cstr.md)|[Base](/txt/ssimdb/dmmeta/cstr.md)|||

#### Struct FCstr
<a href="#struct-fcstr"></a>
*Note:* field ``amc.FCstr.msghdr`` has reftype ``base`` so the fields of [dmmeta.Cstr](/txt/ssimdb/dmmeta/cstr.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FCstr { // amc.FCstr
    algo::Smallstr100   ctype;      //
    bool                strequiv;   //   false  Use strptr instead of this type when possible
    algo::cstring       comment;    //
    // func:amc.FCstr..AssignOp
    inline amc::FCstr&   operator =(const amc::FCstr &rhs) = delete;
    // func:amc.FCstr..CopyCtor
    inline               FCstr(const amc::FCstr &rhs) = delete;
private:
    // func:amc.FCstr..Ctor
    inline               FCstr() __attribute__((nothrow));
    // func:amc.FCstr..Dtor
    inline               ~FCstr() __attribute__((nothrow));
    friend amc::FCstr&          cstr_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FCstr*          cstr_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 cstr_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FCtype - Struct
<a href="#amc-fctype"></a>

#### amc.FCtype Fields
<a href="#amc-fctype-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FCtype.msghdr|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Base](/txt/ssimdb/dmmeta/ctype.md)|||
|amc.FCtype.zs_cfmt|[amc.FCfmt](/txt/gen/amc/amc.md#amc-fcfmt)|[Llist](/txt/exe/amc/reftype.md#llist)|||
|amc.FCtype.cpp_type|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||type name to use in c++|
|amc.FCtype.body|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||Struct contents|
|amc.FCtype.sizecheck|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||Size check assertions|
|amc.FCtype.c_init|[amc.FFunc](/txt/gen/amc/amc.md#amc-ffunc)|[Ptr](/txt/exe/amc/reftype.md#ptr)||Pointer to constructor|
|amc.FCtype.p_ns|[amc.FNs](/txt/gen/amc/amc.md#amc-fns)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|amc.FCtype.c_bltin|[amc.FBltin](/txt/gen/amc/amc.md#amc-fbltin)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FCtype.c_field|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Ptrary](/txt/exe/amc/reftype.md#ptrary)|||
|amc.FCtype.c_msgtype|[amc.FMsgtype](/txt/gen/amc/amc.md#amc-fmsgtype)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FCtype.c_optfld|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FCtype.c_static|[amc.FStatictuple](/txt/gen/amc/amc.md#amc-fstatictuple)|[Ptrary](/txt/exe/amc/reftype.md#ptrary)|||
|amc.FCtype.c_cpptype|[amc.FCpptype](/txt/gen/amc/amc.md#amc-fcpptype)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FCtype.c_parent|[amc.FCtype](/txt/gen/amc/amc.md#amc-fctype)|[Ptrary](/txt/exe/amc/reftype.md#ptrary)|||
|amc.FCtype.c_ssimfile|[amc.FSsimfile](/txt/gen/amc/amc.md#amc-fssimfile)|[Ptr](/txt/exe/amc/reftype.md#ptr)||Ssimfile associated with this ctype|
|amc.FCtype.c_pack|[amc.FPack](/txt/gen/amc/amc.md#amc-fpack)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FCtype.c_lenfld|[amc.FLenfld](/txt/gen/amc/amc.md#amc-flenfld)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FCtype.c_pmaskfld|[amc.FPmaskfld](/txt/gen/amc/amc.md#amc-fpmaskfld)|[Ptrary](/txt/exe/amc/reftype.md#ptrary)||List of pmask fields in this ctype|
|amc.FCtype.c_typefld|[amc.FTypefld](/txt/gen/amc/amc.md#amc-ftypefld)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FCtype.c_datafld|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Ptrary](/txt/exe/amc/reftype.md#ptrary)||Physical fields (non-fldfunc)|
|amc.FCtype.zd_inst|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Llist](/txt/exe/amc/reftype.md#llist)||All the places where this ctype is instantiated (collected manually)|
|amc.FCtype.zs_xref|[amc.FXref](/txt/gen/amc/amc.md#amc-fxref)|[Llist](/txt/exe/amc/reftype.md#llist)||List of xrefs where this ctype is a child (collected manually)|
|amc.FCtype.c_chash|[amc.FChash](/txt/gen/amc/amc.md#amc-fchash)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FCtype.c_ccmp|[amc.FCcmp](/txt/gen/amc/amc.md#amc-fccmp)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FCtype.c_cstr|[amc.FCstr](/txt/gen/amc/amc.md#amc-fcstr)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FCtype.c_cextern|[amc.FCextern](/txt/gen/amc/amc.md#amc-fcextern)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FCtype.signature|[algo.Sha1sig](/txt/protocol/algo/Sha1sig.md)|[Val](/txt/exe/amc/reftype.md#val)|||
|amc.FCtype.c_ffunc|[amc.FFunc](/txt/gen/amc/amc.md#amc-ffunc)|[Ptrary](/txt/exe/amc/reftype.md#ptrary)|||
|amc.FCtype.zd_cafter|[amc.FCafter](/txt/gen/amc/amc.md#amc-fcafter)|[Llist](/txt/exe/amc/reftype.md#llist)|||
|amc.FCtype.c_csize|[amc.FCsize](/txt/gen/amc/amc.md#amc-fcsize)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FCtype.zd_access|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Llist](/txt/exe/amc/reftype.md#llist)||All access paths to this field (via field.arg)|
|amc.FCtype.c_cdflt|[amc.FCdflt](/txt/gen/amc/amc.md#amc-fcdflt)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FCtype.c_argvtype|[amc.FArgvtype](/txt/gen/amc/amc.md#amc-fargvtype)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FCtype.c_floadtuples|[amc.FFloadtuples](/txt/gen/amc/amc.md#amc-ffloadtuples)|[Ptrary](/txt/exe/amc/reftype.md#ptrary)||tuple sources this command loads at startup|
|amc.FCtype.c_pkeyfield|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Ptr](/txt/exe/amc/reftype.md#ptr)||Field corresponding to pkey of this type|
|amc.FCtype.c_fcurs|[amc.FFcurs](/txt/gen/amc/amc.md#amc-ffcurs)|[Ptrary](/txt/exe/amc/reftype.md#ptrary)|||
|amc.FCtype.c_ctypelen|[amc.FCtypelen](/txt/gen/amc/amc.md#amc-fctypelen)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FCtype.size_unknown|bool|[Val](/txt/exe/amc/reftype.md#val)|||
|amc.FCtype.size_locked|bool|[Val](/txt/exe/amc/reftype.md#val)|||
|amc.FCtype.topo_visited|bool|[Val](/txt/exe/amc/reftype.md#val)|false|Temporary|
|amc.FCtype.enum_visited|bool|[Val](/txt/exe/amc/reftype.md#val)|false|Temporary|
|amc.FCtype.fields_cloned|bool|[Val](/txt/exe/amc/reftype.md#val)||True if fields from c_cbase have been cloned.|
|amc.FCtype.original|bool|[Val](/txt/exe/amc/reftype.md#val)||True if this ctype comes from disk|
|amc.FCtype.plaindata|bool|[Val](/txt/exe/amc/reftype.md#val)|||
|amc.FCtype.has_dtor|bool|[Val](/txt/exe/amc/reftype.md#val)||Computed: the ctype owns something a destructor must release or unlink|
|amc.FCtype.cheap_copy|bool|[Val](/txt/exe/amc/reftype.md#val)||Computed: type is plaindata and fits in MaxCheapCopySize bytes, so pass it by value|
|amc.FCtype.declared_size|i32|[Val](/txt/exe/amc/reftype.md#val)||Computed at gen:cheapcopy: schema-declared byte size, 0 when the schema does not determine it|
|amc.FCtype.declared_alignment|i32|[Val](/txt/exe/amc/reftype.md#val)|1|Computed at gen:cheapcopy: alignment implied by declared_size's members|
|amc.FCtype.alignment|u32|[Val](/txt/exe/amc/reftype.md#val)|1||
|amc.FCtype.n_padbytes|i32|[Val](/txt/exe/amc/reftype.md#val)|||
|amc.FCtype.totsize_byte|u32|[Val](/txt/exe/amc/reftype.md#val)||Total size in bytes of all fields|
|amc.FCtype.n_xref|i32|[Val](/txt/exe/amc/reftype.md#val)|||
|amc.FCtype.next_anon_idx|i32|[Val](/txt/exe/amc/reftype.md#val)|0|sequence to enumerate positional fields|
|amc.FCtype.c_nossimfile|[amc.FNossimfile](/txt/gen/amc/amc.md#amc-fnossimfile)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FCtype.topo_idx|i32|[Val](/txt/exe/amc/reftype.md#val)||Index of ctype in array c_ctype (after topological sort)|
|amc.FCtype.c_cfast|[amc.FCfast](/txt/gen/amc/amc.md#amc-fcfast)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FCtype.c_ffast|[amc.FFfast](/txt/gen/amc/amc.md#amc-fffast)|[Ptrary](/txt/exe/amc/reftype.md#ptrary)|||
|amc.FCtype.in_copy_priv|bool|[Val](/txt/exe/amc/reftype.md#val)|||
|amc.FCtype.zd_varlenfld|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Llist](/txt/exe/amc/reftype.md#llist)|||
|amc.FCtype.c_jstype|[amc.FJstype](/txt/gen/amc/amc.md#amc-fjstype)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FCtype.c_ckafka|[amc.FCkafka](/txt/gen/amc/amc.md#amc-fckafka)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FCtype.c_cjsbltin|[amc.FCjsbltin](/txt/gen/amc/amc.md#amc-fcjsbltin)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FCtype.c_cjson|[amc.FCjson](/txt/gen/amc/amc.md#amc-fcjson)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FCtype.c_ccmdline|[amc.FCcmdline](/txt/gen/amc/amc.md#amc-fccmdline)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FCtype.c_cpbuf|[amc.FCpbuf](/txt/gen/amc/amc.md#amc-fcpbuf)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FCtype.c_fpbufcase|[amc.FFpbufcase](/txt/gen/amc/amc.md#amc-ffpbufcase)|[Ptrary](/txt/exe/amc/reftype.md#ptrary)|||
|amc.FCtype.in_compute_size|bool|[Val](/txt/exe/amc/reftype.md#val)||ComputeCtypeSize is walking this ctype's body (cycle detection)|
|amc.FCtype.c_payloadhdr|[amc.FPayloadhdr](/txt/gen/amc/amc.md#amc-fpayloadhdr)|[Ptr](/txt/exe/amc/reftype.md#ptr)||Derived payload header of this header ctype|
|amc.FCtype.c_msg|[amc.FMsg](/txt/gen/amc/amc.md#amc-fmsg)|[Ptr](/txt/exe/amc/reftype.md#ptr)||Derived message layout of this msg ctype|
|amc.FCtype.c_pnew|[amc.FPnew](/txt/gen/amc/amc.md#amc-fpnew)|[Ptrary](/txt/exe/amc/reftype.md#ptrary)|||

#### Struct FCtype
<a href="#struct-fctype"></a>
*Note:* field ``amc.FCtype.msghdr`` has reftype ``base`` so the fields of [dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FCtype { // amc.FCtype
    algo::Smallstr100     ctype;                      // Identifier. must be ns.typename
    algo::cstring         comment;                    //
    amc::FCfmt*           zs_cfmt_head;               // zero-terminated singly linked list
    amc::FCfmt*           zs_cfmt_tail;               // pointer to last element
    algo::cstring         cpp_type;                   // type name to use in c++
    algo::cstring         body;                       // Struct contents
    algo::cstring         sizecheck;                  // Size check assertions
    amc::FFunc*           c_init;                     // Pointer to constructor. optional pointer
    amc::FNs*             p_ns;                       // reference to parent row
    amc::FBltin*          c_bltin;                    // optional pointer
    amc::FField**         c_field_elems;              // array of pointers
    u64                   c_field_n;                  // current size
    u64                   c_field_max;                // capacity of allocated array
    amc::FMsgtype*        c_msgtype;                  // optional pointer
    amc::FField*          c_optfld;                   // optional pointer
    amc::FStatictuple**   c_static_elems;             // array of pointers
    u64                   c_static_n;                 // current size
    u64                   c_static_max;               // capacity of allocated array
    amc::FCpptype*        c_cpptype;                  // optional pointer
    amc::FCtype**         c_parent_elems;             // array of pointers
    u64                   c_parent_n;                 // current size
    u64                   c_parent_max;               // capacity of allocated array
    amc::FSsimfile*       c_ssimfile;                 // Ssimfile associated with this ctype. optional pointer
    amc::FPack*           c_pack;                     // optional pointer
    amc::FLenfld*         c_lenfld;                   // optional pointer
    amc::FPmaskfld**      c_pmaskfld_elems;           // array of pointers
    u64                   c_pmaskfld_n;               // current size
    u64                   c_pmaskfld_max;             // capacity of allocated array
    amc::FTypefld*        c_typefld;                  // optional pointer
    amc::FField**         c_datafld_elems;            // array of pointers
    u64                   c_datafld_n;                // current size
    u64                   c_datafld_max;              // capacity of allocated array
    amc::FField*          zd_inst_head;               // zero-terminated doubly linked list
    i32                   zd_inst_n;                  // zero-terminated doubly linked list
    amc::FField*          zd_inst_tail;               // pointer to last element
    amc::FXref*           zs_xref_head;               // zero-terminated singly linked list
    i32                   zs_xref_n;                  // zero-terminated singly linked list
    amc::FXref*           zs_xref_tail;               // pointer to last element
    amc::FChash*          c_chash;                    // optional pointer
    amc::FCcmp*           c_ccmp;                     // optional pointer
    amc::FCstr*           c_cstr;                     // optional pointer
    amc::FCextern*        c_cextern;                  // optional pointer
    algo::Sha1sig         signature;                  //
    amc::FFunc**          c_ffunc_elems;              // array of pointers
    u64                   c_ffunc_n;                  // current size
    u64                   c_ffunc_max;                // capacity of allocated array
    amc::FCafter*         zd_cafter_head;             // zero-terminated doubly linked list
    i32                   zd_cafter_n;                // zero-terminated doubly linked list
    amc::FCafter*         zd_cafter_tail;             // pointer to last element
    amc::FCsize*          c_csize;                    // optional pointer
    amc::FField*          zd_access_head;             // zero-terminated doubly linked list
    i32                   zd_access_n;                // zero-terminated doubly linked list
    amc::FField*          zd_access_tail;             // pointer to last element
    amc::FCdflt*          c_cdflt;                    // optional pointer
    amc::FArgvtype*       c_argvtype;                 // optional pointer
    amc::FFloadtuples**   c_floadtuples_elems;        // array of pointers
    u64                   c_floadtuples_n;            // current size
    u64                   c_floadtuples_max;          // capacity of allocated array
    amc::FField*          c_pkeyfield;                // Field corresponding to pkey of this type. optional pointer
    amc::FFcurs**         c_fcurs_elems;              // array of pointers
    u64                   c_fcurs_n;                  // current size
    u64                   c_fcurs_max;                // capacity of allocated array
    amc::FCtypelen*       c_ctypelen;                 // optional pointer
    bool                  size_unknown;               //   false
    bool                  size_locked;                //   false
    bool                  topo_visited;               //   false  Temporary
    bool                  enum_visited;               //   false  Temporary
    bool                  fields_cloned;              //   false  True if fields from c_cbase have been cloned.
    bool                  original;                   //   false  True if this ctype comes from disk
    bool                  plaindata;                  //   false
    bool                  has_dtor;                   //   false  Computed: the ctype owns something a destructor must release or unlink
    bool                  cheap_copy;                 //   false  Computed: type is plaindata and fits in MaxCheapCopySize bytes, so pass it by value
    i32                   declared_size;              //   0  Computed at gen:cheapcopy: schema-declared byte size, 0 when the schema does not determine it
    i32                   declared_alignment;         //   1  Computed at gen:cheapcopy: alignment implied by declared_size's members
    u32                   alignment;                  //   1
    i32                   n_padbytes;                 //   0
    u32                   totsize_byte;               //   0  Total size in bytes of all fields
    i32                   n_xref;                     //   0
    i32                   next_anon_idx;              //   0  sequence to enumerate positional fields
    amc::FNossimfile*     c_nossimfile;               // optional pointer
    i32                   topo_idx;                   //   0  Index of ctype in array c_ctype (after topological sort)
    amc::FCfast*          c_cfast;                    // optional pointer
    amc::FFfast**         c_ffast_elems;              // array of pointers
    u64                   c_ffast_n;                  // current size
    u64                   c_ffast_max;                // capacity of allocated array
    bool                  in_copy_priv;               //   false
    amc::FField*          zd_varlenfld_head;          // zero-terminated doubly linked list
    i32                   zd_varlenfld_n;             // zero-terminated doubly linked list
    amc::FField*          zd_varlenfld_tail;          // pointer to last element
    amc::FJstype*         c_jstype;                   // optional pointer
    amc::FCkafka*         c_ckafka;                   // optional pointer
    amc::FCjsbltin*       c_cjsbltin;                 // optional pointer
    amc::FCjson*          c_cjson;                    // optional pointer
    amc::FCcmdline*       c_ccmdline;                 // optional pointer
    amc::FCpbuf*          c_cpbuf;                    // optional pointer
    amc::FFpbufcase**     c_fpbufcase_elems;          // array of pointers
    u64                   c_fpbufcase_n;              // current size
    u64                   c_fpbufcase_max;            // capacity of allocated array
    bool                  in_compute_size;            //   false  ComputeCtypeSize is walking this ctype's body (cycle detection)
    amc::FPayloadhdr*     c_payloadhdr;               // Derived payload header of this header ctype. optional pointer
    amc::FMsg*            c_msg;                      // Derived message layout of this msg ctype. optional pointer
    amc::FPnew**          c_pnew_elems;               // array of pointers
    u64                   c_pnew_n;                   // current size
    u64                   c_pnew_max;                 // capacity of allocated array
    bool                  ns_c_ctype_in_ary;          //   false  membership flag
    amc::FCtype*          ind_ctype_next;             // hash next
    u32                   ind_ctype_hashval;          // hash value
    amc::FCtype*          zsl_ctype_pack_tran_next;   // zslist link; -1 means not-in-list
    amc::FCtype*          zs_sig_visit_next;          // zslist link; -1 means not-in-list
    // reftype Llist of amc.FCtype.zs_cfmt prohibits copy
    // x-reference on amc.FCtype.p_ns prevents copy
    // x-reference on amc.FCtype.c_bltin prevents copy
    // reftype Ptrary of amc.FCtype.c_field prohibits copy
    // x-reference on amc.FCtype.c_msgtype prevents copy
    // x-reference on amc.FCtype.c_optfld prevents copy
    // reftype Ptrary of amc.FCtype.c_static prohibits copy
    // x-reference on amc.FCtype.c_cpptype prevents copy
    // reftype Ptrary of amc.FCtype.c_parent prohibits copy
    // x-reference on amc.FCtype.c_ssimfile prevents copy
    // x-reference on amc.FCtype.c_pack prevents copy
    // x-reference on amc.FCtype.c_lenfld prevents copy
    // reftype Ptrary of amc.FCtype.c_pmaskfld prohibits copy
    // x-reference on amc.FCtype.c_typefld prevents copy
    // reftype Ptrary of amc.FCtype.c_datafld prohibits copy
    // reftype Llist of amc.FCtype.zd_inst prohibits copy
    // reftype Llist of amc.FCtype.zs_xref prohibits copy
    // x-reference on amc.FCtype.c_chash prevents copy
    // x-reference on amc.FCtype.c_ccmp prevents copy
    // x-reference on amc.FCtype.c_cstr prevents copy
    // x-reference on amc.FCtype.c_cextern prevents copy
    // reftype Ptrary of amc.FCtype.c_ffunc prohibits copy
    // reftype Llist of amc.FCtype.zd_cafter prohibits copy
    // x-reference on amc.FCtype.c_csize prevents copy
    // reftype Llist of amc.FCtype.zd_access prohibits copy
    // x-reference on amc.FCtype.c_cdflt prevents copy
    // x-reference on amc.FCtype.c_argvtype prevents copy
    // reftype Ptrary of amc.FCtype.c_floadtuples prohibits copy
    // reftype Ptrary of amc.FCtype.c_fcurs prohibits copy
    // x-reference on amc.FCtype.c_ctypelen prevents copy
    // x-reference on amc.FCtype.c_nossimfile prevents copy
    // x-reference on amc.FCtype.c_cfast prevents copy
    // reftype Ptrary of amc.FCtype.c_ffast prohibits copy
    // reftype Llist of amc.FCtype.zd_varlenfld prohibits copy
    // x-reference on amc.FCtype.c_jstype prevents copy
    // x-reference on amc.FCtype.c_ckafka prevents copy
    // x-reference on amc.FCtype.c_cjsbltin prevents copy
    // x-reference on amc.FCtype.c_cjson prevents copy
    // x-reference on amc.FCtype.c_ccmdline prevents copy
    // x-reference on amc.FCtype.c_cpbuf prevents copy
    // reftype Ptrary of amc.FCtype.c_fpbufcase prohibits copy
    // x-reference on amc.FCtype.c_payloadhdr prevents copy
    // x-reference on amc.FCtype.c_msg prevents copy
    // reftype Ptrary of amc.FCtype.c_pnew prohibits copy
    // func:amc.FCtype..AssignOp
    amc::FCtype&         operator =(const amc::FCtype &rhs) = delete;
    // reftype Llist of amc.FCtype.zs_cfmt prohibits copy
    // x-reference on amc.FCtype.p_ns prevents copy
    // x-reference on amc.FCtype.c_bltin prevents copy
    // reftype Ptrary of amc.FCtype.c_field prohibits copy
    // x-reference on amc.FCtype.c_msgtype prevents copy
    // x-reference on amc.FCtype.c_optfld prevents copy
    // reftype Ptrary of amc.FCtype.c_static prohibits copy
    // x-reference on amc.FCtype.c_cpptype prevents copy
    // reftype Ptrary of amc.FCtype.c_parent prohibits copy
    // x-reference on amc.FCtype.c_ssimfile prevents copy
    // x-reference on amc.FCtype.c_pack prevents copy
    // x-reference on amc.FCtype.c_lenfld prevents copy
    // reftype Ptrary of amc.FCtype.c_pmaskfld prohibits copy
    // x-reference on amc.FCtype.c_typefld prevents copy
    // reftype Ptrary of amc.FCtype.c_datafld prohibits copy
    // reftype Llist of amc.FCtype.zd_inst prohibits copy
    // reftype Llist of amc.FCtype.zs_xref prohibits copy
    // x-reference on amc.FCtype.c_chash prevents copy
    // x-reference on amc.FCtype.c_ccmp prevents copy
    // x-reference on amc.FCtype.c_cstr prevents copy
    // x-reference on amc.FCtype.c_cextern prevents copy
    // reftype Ptrary of amc.FCtype.c_ffunc prohibits copy
    // reftype Llist of amc.FCtype.zd_cafter prohibits copy
    // x-reference on amc.FCtype.c_csize prevents copy
    // reftype Llist of amc.FCtype.zd_access prohibits copy
    // x-reference on amc.FCtype.c_cdflt prevents copy
    // x-reference on amc.FCtype.c_argvtype prevents copy
    // reftype Ptrary of amc.FCtype.c_floadtuples prohibits copy
    // reftype Ptrary of amc.FCtype.c_fcurs prohibits copy
    // x-reference on amc.FCtype.c_ctypelen prevents copy
    // x-reference on amc.FCtype.c_nossimfile prevents copy
    // x-reference on amc.FCtype.c_cfast prevents copy
    // reftype Ptrary of amc.FCtype.c_ffast prohibits copy
    // reftype Llist of amc.FCtype.zd_varlenfld prohibits copy
    // x-reference on amc.FCtype.c_jstype prevents copy
    // x-reference on amc.FCtype.c_ckafka prevents copy
    // x-reference on amc.FCtype.c_cjsbltin prevents copy
    // x-reference on amc.FCtype.c_cjson prevents copy
    // x-reference on amc.FCtype.c_ccmdline prevents copy
    // x-reference on amc.FCtype.c_cpbuf prevents copy
    // reftype Ptrary of amc.FCtype.c_fpbufcase prohibits copy
    // x-reference on amc.FCtype.c_payloadhdr prevents copy
    // x-reference on amc.FCtype.c_msg prevents copy
    // reftype Ptrary of amc.FCtype.c_pnew prohibits copy
    // func:amc.FCtype..CopyCtor
    FCtype(const amc::FCtype &rhs) = delete;
private:
    // func:amc.FCtype..Ctor
    inline               FCtype() __attribute__((nothrow));
    // func:amc.FCtype..Dtor
    inline               ~FCtype() __attribute__((nothrow));
    friend amc::FCtype&         ctype_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FCtype*         ctype_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 ctype_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FCtypelen - Size of Ctype
<a href="#amc-fctypelen"></a>

#### amc.FCtypelen Fields
<a href="#amc-fctypelen-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FCtypelen.base|[dmmeta.Ctypelen](/txt/ssimdb/dmmeta/ctypelen.md)|[Base](/txt/ssimdb/dmmeta/ctypelen.md)|||

#### Struct FCtypelen
<a href="#struct-fctypelen"></a>
*Note:* field ``amc.FCtypelen.base`` has reftype ``base`` so the fields of [dmmeta.Ctypelen](/txt/ssimdb/dmmeta/ctypelen.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FCtypelen { // amc.FCtypelen
    algo::Smallstr100   ctype;       // Identifies the Ctype
    u32                 len;         //   0  (calculated) length of the C++ struct in bytes
    i32                 alignment;   //   0  (calculated) alignment for the struct
    i32                 padbytes;    //   0  (calculated) total # of pad bytes
    bool                plaindata;   //   false  (calculated) can safely call memcpy on this struct
    // func:amc.FCtypelen..AssignOp
    inline amc::FCtypelen& operator =(const amc::FCtypelen &rhs) = delete;
    // func:amc.FCtypelen..CopyCtor
    inline               FCtypelen(const amc::FCtypelen &rhs) = delete;
private:
    // func:amc.FCtypelen..Ctor
    inline               FCtypelen() __attribute__((nothrow));
    // func:amc.FCtypelen..Dtor
    inline               ~FCtypelen() __attribute__((nothrow));
    friend amc::FCtypelen&      ctypelen_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FCtypelen*      ctypelen_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 ctypelen_RemoveAll() __attribute__((nothrow));
    friend void                 ctypelen_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FJsbltin
<a href="#amc-fjsbltin"></a>

#### amc.FJsbltin Fields
<a href="#amc-fjsbltin-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FJsbltin.base|[dmmeta.Jsbltin](/txt/ssimdb/dmmeta/jsbltin.md)|[Base](/txt/ssimdb/dmmeta/jsbltin.md)|||

#### Struct FJsbltin
<a href="#struct-fjsbltin"></a>
*Note:* field ``amc.FJsbltin.base`` has reftype ``base`` so the fields of [dmmeta.Jsbltin](/txt/ssimdb/dmmeta/jsbltin.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FJsbltin { // amc.FJsbltin
    algo::Smallstr50   jsbltin;               //
    algo::Smallstr50   cons;                  //
    algo::Smallstr50   dflt;                  //
    algo::cstring      comment;               //
    amc::FJsbltin*     ind_jsbltin_next;      // hash next
    u32                ind_jsbltin_hashval;   // hash value
    // func:amc.FJsbltin..AssignOp
    inline amc::FJsbltin& operator =(const amc::FJsbltin &rhs) = delete;
    // func:amc.FJsbltin..Ctor
    inline               FJsbltin() __attribute__((nothrow));
    // func:amc.FJsbltin..Dtor
    inline               ~FJsbltin() __attribute__((nothrow));
    // func:amc.FJsbltin..CopyCtor
    inline               FJsbltin(const amc::FJsbltin &rhs) = delete;
};
```

#### amc.Genctx
<a href="#amc-genctx"></a>

#### amc.Genctx Fields
<a href="#amc-genctx-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.Genctx.R|[algo_lib.Replscope](/txt/gen/algo_lib/algo_lib.md#algo_lib-replscope)|[Val](/txt/exe/amc/reftype.md#val)|||
|amc.Genctx.p_ns|[amc.FNs](/txt/gen/amc/amc.md#amc-fns)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|amc.Genctx.p_ctype|[amc.FCtype](/txt/gen/amc/amc.md#amc-fctype)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|amc.Genctx.p_field|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|amc.Genctx.p_tfunc|[amc.FTfunc](/txt/gen/amc/amc.md#amc-ftfunc)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||

#### Struct Genctx
<a href="#struct-genctx"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct Genctx { // amc.Genctx
    algo_lib::Replscope   R;         //
    amc::FNs*             p_ns;      // reference to parent row
    amc::FCtype*          p_ctype;   // reference to parent row
    amc::FField*          p_field;   // reference to parent row
    amc::FTfunc*          p_tfunc;   // reference to parent row
    // func:amc.Genctx..Ctor
    inline               Genctx() __attribute__((nothrow));
};
```

#### amc.FTclass - AMC template class
<a href="#amc-ftclass"></a>

#### amc.FTclass Fields
<a href="#amc-ftclass-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FTclass.msghdr|[amcdb.Tclass](/txt/ssimdb/amcdb/tclass.md)|[Base](/txt/ssimdb/amcdb/tclass.md)|||
|amc.FTclass.c_tfunc|[amc.FTfunc](/txt/gen/amc/amc.md#amc-ftfunc)|[Ptrary](/txt/exe/amc/reftype.md#ptrary)|||
|amc.FTclass.step||[Hook](/txt/exe/amc/reftype.md#hook)|||
|amc.FTclass.c_tcond|[amc.FTcond](/txt/gen/amc/amc.md#amc-ftcond)|[Ptrary](/txt/exe/amc/reftype.md#ptrary)|||

#### Struct FTclass
<a href="#struct-ftclass"></a>
*Note:* field ``amc.FTclass.msghdr`` has reftype ``base`` so the fields of [amcdb.Tclass](/txt/ssimdb/amcdb/tclass.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FTclass { // amc.FTclass
    algo::Smallstr50        tclass;               //
    algo::cstring           comment;              //
    amc::FTfunc**           c_tfunc_elems;        // array of pointers
    u64                     c_tfunc_n;            // current size
    u64                     c_tfunc_max;          // capacity of allocated array
    amc::tclass_step_hook   step;                 //   NULL  Pointer to a function
    amc::FTcond**           c_tcond_elems;        // array of pointers
    u64                     c_tcond_n;            // current size
    u64                     c_tcond_max;          // capacity of allocated array
    amc::FTclass*           ind_tclass_next;      // hash next
    u32                     ind_tclass_hashval;   // hash value
    // reftype Ptrary of amc.FTclass.c_tfunc prohibits copy
    // reftype Hook of amc.FTclass.step prohibits copy
    // reftype Ptrary of amc.FTclass.c_tcond prohibits copy
    // func:amc.FTclass..AssignOp
    inline amc::FTclass& operator =(const amc::FTclass &rhs) = delete;
    // func:amc.FTclass..Ctor
    inline               FTclass() __attribute__((nothrow));
    // func:amc.FTclass..Dtor
    inline               ~FTclass() __attribute__((nothrow));
    // reftype Ptrary of amc.FTclass.c_tfunc prohibits copy
    // reftype Hook of amc.FTclass.step prohibits copy
    // reftype Ptrary of amc.FTclass.c_tcond prohibits copy
    // func:amc.FTclass..CopyCtor
    inline               FTclass(const amc::FTclass &rhs) = delete;
};
```

#### amc.FDb - In-memory database for amc
<a href="#amc-fdb"></a>

#### amc.FDb Fields
<a href="#amc-fdb-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FDb.lpool|u8|[Lpool](/txt/exe/amc/reftype.md#lpool)||private memory pool|
|amc.FDb.fsort|[amc.FFsort](/txt/gen/amc/amc.md#amc-ffsort)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb._db|[amc.FDb](/txt/gen/amc/amc.md#amc-fdb)|[Global](/txt/exe/amc/reftype.md#global)|||
|amc.FDb.ind_cfmt|[amc.FCfmt](/txt/gen/amc/amc.md#amc-fcfmt)|[Thash](/txt/exe/amc/reftype.md#thash)|||
|amc.FDb.dispfilter|[amc.FDispfilter](/txt/gen/amc/amc.md#amc-fdispfilter)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.usertracefld|[amc.FUsertracefld](/txt/gen/amc/amc.md#amc-fusertracefld)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.cfmt|[amc.FCfmt](/txt/gen/amc/amc.md#amc-fcfmt)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.cmdline|[command.amc](/txt/protocol/command/README.md#command-amc)|[Val](/txt/exe/amc/reftype.md#val)|||
|amc.FDb.dispatch|[amc.FDispatch](/txt/gen/amc/amc.md#amc-fdispatch)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.dispatch_msg|[amc.FDispatchmsg](/txt/gen/amc/amc.md#amc-fdispatchmsg)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.ctype|[amc.FCtype](/txt/gen/amc/amc.md#amc-fctype)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.field|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.basepool|[amc.FBasepool](/txt/gen/amc/amc.md#amc-fbasepool)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.llist|[amc.FLlist](/txt/gen/amc/amc.md#amc-fllist)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.anonfld|[amc.FAnonfld](/txt/gen/amc/amc.md#amc-fanonfld)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.xref|[amc.FXref](/txt/gen/amc/amc.md#amc-fxref)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.ns|[amc.FNs](/txt/gen/amc/amc.md#amc-fns)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.pnew|[amc.FPnew](/txt/gen/amc/amc.md#amc-fpnew)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.fldoffset|[amc.FFldoffset](/txt/gen/amc/amc.md#amc-ffldoffset)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.typefld|[amc.FTypefld](/txt/gen/amc/amc.md#amc-ftypefld)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.lenfld|[amc.FLenfld](/txt/gen/amc/amc.md#amc-flenfld)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.bltin|[amc.FBltin](/txt/gen/amc/amc.md#amc-fbltin)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.static_tuple|[amc.FStatictuple](/txt/gen/amc/amc.md#amc-fstatictuple)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.msgtype|[amc.FMsgtype](/txt/gen/amc/amc.md#amc-fmsgtype)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.gconst|[amc.FGconst](/txt/gen/amc/amc.md#amc-fgconst)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.gstatic|[amc.FGstatic](/txt/gen/amc/amc.md#amc-fgstatic)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.thash|[amc.FThash](/txt/gen/amc/amc.md#amc-fthash)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.jsbltin|[amc.FJsbltin](/txt/gen/amc/amc.md#amc-fjsbltin)|[Inlary](/txt/exe/amc/reftype.md#inlary)|||
|amc.FDb.outfile|[amc.FOutfile](/txt/gen/amc/amc.md#amc-foutfile)|[Tpool](/txt/exe/amc/reftype.md#tpool)|||
|amc.FDb.func|[amc.FFunc](/txt/gen/amc/amc.md#amc-ffunc)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.smallstr|[amc.FSmallstr](/txt/gen/amc/amc.md#amc-fsmallstr)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.numstr|[amc.FNumstr](/txt/gen/amc/amc.md#amc-fnumstr)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.main|[amc.FMain](/txt/gen/amc/amc.md#amc-fmain)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.reftype|[amc.FReftype](/txt/gen/amc/amc.md#amc-freftype)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.ind_bltin|[amc.FBltin](/txt/gen/amc/amc.md#amc-fbltin)|[Thash](/txt/exe/amc/reftype.md#thash)|||
|amc.FDb.ind_ctype|[amc.FCtype](/txt/gen/amc/amc.md#amc-fctype)|[Thash](/txt/exe/amc/reftype.md#thash)|||
|amc.FDb.ind_dispatch|[amc.FDispatch](/txt/gen/amc/amc.md#amc-fdispatch)|[Thash](/txt/exe/amc/reftype.md#thash)|||
|amc.FDb.ind_func|[amc.FFunc](/txt/gen/amc/amc.md#amc-ffunc)|[Thash](/txt/exe/amc/reftype.md#thash)|||
|amc.FDb.ind_field|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Thash](/txt/exe/amc/reftype.md#thash)|||
|amc.FDb.ind_ns|[amc.FNs](/txt/gen/amc/amc.md#amc-fns)|[Thash](/txt/exe/amc/reftype.md#thash)|||
|amc.FDb.ind_pnew|[amc.FPnew](/txt/gen/amc/amc.md#amc-fpnew)|[Thash](/txt/exe/amc/reftype.md#thash)|||
|amc.FDb.ind_xref|[amc.FXref](/txt/gen/amc/amc.md#amc-fxref)|[Thash](/txt/exe/amc/reftype.md#thash)|||
|amc.FDb.cpptype|[amc.FCpptype](/txt/gen/amc/amc.md#amc-fcpptype)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.ind_cpptype|[amc.FCpptype](/txt/gen/amc/amc.md#amc-fcpptype)|[Thash](/txt/exe/amc/reftype.md#thash)|||
|amc.FDb.inlary|[amc.FInlary](/txt/gen/amc/amc.md#amc-finlary)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.ind_inlary|[amc.FInlary](/txt/gen/amc/amc.md#amc-finlary)|[Thash](/txt/exe/amc/reftype.md#thash)|||
|amc.FDb.tary|[amc.FTary](/txt/gen/amc/amc.md#amc-ftary)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.ind_tary|[amc.FTary](/txt/gen/amc/amc.md#amc-ftary)|[Thash](/txt/exe/amc/reftype.md#thash)|||
|amc.FDb.cppfunc|[amc.FCppfunc](/txt/gen/amc/amc.md#amc-fcppfunc)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.rowid|[amc.FRowid](/txt/gen/amc/amc.md#amc-frowid)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.ind_rowid|[amc.FRowid](/txt/gen/amc/amc.md#amc-frowid)|[Thash](/txt/exe/amc/reftype.md#thash)|||
|amc.FDb.cascdel|[amc.FCascdel](/txt/gen/amc/amc.md#amc-fcascdel)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.substr|[amc.FSubstr](/txt/gen/amc/amc.md#amc-fsubstr)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.bitfld|[amc.FBitfld](/txt/gen/amc/amc.md#amc-fbitfld)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.ssimfile|[amc.FSsimfile](/txt/gen/amc/amc.md#amc-fssimfile)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.ind_ssimfile|[amc.FSsimfile](/txt/gen/amc/amc.md#amc-fssimfile)|[Thash](/txt/exe/amc/reftype.md#thash)|||
|amc.FDb.pack|[amc.FPack](/txt/gen/amc/amc.md#amc-fpack)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.ind_pack|[amc.FPack](/txt/gen/amc/amc.md#amc-fpack)|[Thash](/txt/exe/amc/reftype.md#thash)|||
|amc.FDb.ind_smallstr|[amc.FSmallstr](/txt/gen/amc/amc.md#amc-fsmallstr)|[Thash](/txt/exe/amc/reftype.md#thash)|||
|amc.FDb.ptrary|[amc.FPtrary](/txt/gen/amc/amc.md#amc-fptrary)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.c_ctype_sorted|[amc.FCtype](/txt/gen/amc/amc.md#amc-fctype)|[Ptrary](/txt/exe/amc/reftype.md#ptrary)|||
|amc.FDb.enumstr|[amc.FEnumstr](/txt/gen/amc/amc.md#amc-fenumstr)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.enumstr_len|[amc.FEnumstrLen](/txt/gen/amc/amc.md#amc-fenumstrlen)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.bh_enumstr_len|[amc.FEnumstrLen](/txt/gen/amc/amc.md#amc-fenumstrlen)|[Bheap](/txt/exe/amc/reftype.md#bheap)||List of FEnumstrLen by length|
|amc.FDb.ind_enumstr_len|[amc.FEnumstrLen](/txt/gen/amc/amc.md#amc-fenumstrlen)|[Thash](/txt/exe/amc/reftype.md#thash)||Index of FEnumstrLen by length|
|amc.FDb.ind_enumstr|[amc.FEnumstr](/txt/gen/amc/amc.md#amc-fenumstr)|[Thash](/txt/exe/amc/reftype.md#thash)||Index of FEnumstr by string value|
|amc.FDb.fbitset|[amc.FFbitset](/txt/gen/amc/amc.md#amc-ffbitset)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.ind_fbitset|[amc.FFbitset](/txt/gen/amc/amc.md#amc-ffbitset)|[Thash](/txt/exe/amc/reftype.md#thash)|||
|amc.FDb.fdec|[amc.FFdec](/txt/gen/amc/amc.md#amc-ffdec)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.ind_fdec|[amc.FFdec](/txt/gen/amc/amc.md#amc-ffdec)|[Thash](/txt/exe/amc/reftype.md#thash)|||
|amc.FDb.ind_reftype|[amc.FReftype](/txt/gen/amc/amc.md#amc-freftype)|[Thash](/txt/exe/amc/reftype.md#thash)|||
|amc.FDb.fconst|[amc.FFconst](/txt/gen/amc/amc.md#amc-ffconst)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.ind_fconst|[amc.FFconst](/txt/gen/amc/amc.md#amc-ffconst)|[Thash](/txt/exe/amc/reftype.md#thash)|||
|amc.FDb.c_ns_sorted|[amc.FNs](/txt/gen/amc/amc.md#amc-fns)|[Ptrary](/txt/exe/amc/reftype.md#ptrary)|||
|amc.FDb.finput|[amc.FFinput](/txt/gen/amc/amc.md#amc-ffinput)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.foutput|[amc.FFoutput](/txt/gen/amc/amc.md#amc-ffoutput)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.fbuf|[amc.FFbuf](/txt/gen/amc/amc.md#amc-ffbuf)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.ind_fbuf|[amc.FFbuf](/txt/gen/amc/amc.md#amc-ffbuf)|[Thash](/txt/exe/amc/reftype.md#thash)|||
|amc.FDb.chash|[amc.FChash](/txt/gen/amc/amc.md#amc-fchash)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.ind_chash|[amc.FChash](/txt/gen/amc/amc.md#amc-fchash)|[Thash](/txt/exe/amc/reftype.md#thash)|||
|amc.FDb.ccmp|[amc.FCcmp](/txt/gen/amc/amc.md#amc-fccmp)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.ind_ccmp|[amc.FCcmp](/txt/gen/amc/amc.md#amc-fccmp)|[Thash](/txt/exe/amc/reftype.md#thash)|||
|amc.FDb.fbigend|[amc.FFbigend](/txt/gen/amc/amc.md#amc-ffbigend)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.zsl_ctype_pack_tran|[amc.FCtype](/txt/gen/amc/amc.md#amc-fctype)|[Llist](/txt/exe/amc/reftype.md#llist)||Transitive closure on packed ctype|
|amc.FDb.cstr|[amc.FCstr](/txt/gen/amc/amc.md#amc-fcstr)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.listtype|[amc.FListtype](/txt/gen/amc/amc.md#amc-flisttype)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.ind_listtype|[amc.FListtype](/txt/gen/amc/amc.md#amc-flisttype)|[Thash](/txt/exe/amc/reftype.md#thash)|||
|amc.FDb.fstep|[amc.FFstep](/txt/gen/amc/amc.md#amc-ffstep)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.cextern|[amc.FCextern](/txt/gen/amc/amc.md#amc-fcextern)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.fdelay|[amc.FFdelay](/txt/gen/amc/amc.md#amc-ffdelay)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.disptrace|[amc.FDisptrace](/txt/gen/amc/amc.md#amc-fdisptrace)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.ind_fstep|[amc.FFstep](/txt/gen/amc/amc.md#amc-ffstep)|[Thash](/txt/exe/amc/reftype.md#thash)|||
|amc.FDb.tracefld|[amc.FTracefld](/txt/gen/amc/amc.md#amc-ftracefld)|[Lary](/txt/exe/amc/reftype.md#lary)||Word trace is already taken...|
|amc.FDb.tracerec|[amc.FTracerec](/txt/gen/amc/amc.md#amc-ftracerec)|[Lary](/txt/exe/amc/reftype.md#lary)||Word trace is already taken...|
|amc.FDb.dispsig|[amc.FDispsig](/txt/gen/amc/amc.md#amc-fdispsig)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.zs_sig_visit|[amc.FCtype](/txt/gen/amc/amc.md#amc-fctype)|[Llist](/txt/exe/amc/reftype.md#llist)|||
|amc.FDb.target|[amc.FTarget](/txt/gen/amc/amc.md#amc-ftarget)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.ind_target|[amc.FTarget](/txt/gen/amc/amc.md#amc-ftarget)|[Thash](/txt/exe/amc/reftype.md#thash)|||
|amc.FDb.targdep|[amc.FTargdep](/txt/gen/amc/amc.md#amc-ftargdep)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.c_malloc|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FDb.dispctx|[amc.FDispctx](/txt/gen/amc/amc.md#amc-fdispctx)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.pmaskfld|[amc.FPmaskfld](/txt/gen/amc/amc.md#amc-fpmaskfld)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.genctx|[amc.Genctx](/txt/gen/amc/amc.md#amc-genctx)|[Val](/txt/exe/amc/reftype.md#val)|||
|amc.FDb.fwddecl|[amc.FFwddecl](/txt/gen/amc/amc.md#amc-ffwddecl)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.ind_fwddecl|[amc.FFwddecl](/txt/gen/amc/amc.md#amc-ffwddecl)|[Thash](/txt/exe/amc/reftype.md#thash)|||
|amc.FDb.tfunc|[amc.FTfunc](/txt/gen/amc/amc.md#amc-ftfunc)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.ind_tfunc|[amc.FTfunc](/txt/gen/amc/amc.md#amc-ftfunc)|[Thash](/txt/exe/amc/reftype.md#thash)|||
|amc.FDb.gen|[amc.FGen](/txt/gen/amc/amc.md#amc-fgen)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.c_curns|[amc.FNs](/txt/gen/amc/amc.md#amc-fns)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FDb.fregx|[amc.FFregx](/txt/gen/amc/amc.md#amc-ffregx)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.tclass|[amc.FTclass](/txt/gen/amc/amc.md#amc-ftclass)|[Inlary](/txt/exe/amc/reftype.md#inlary)|||
|amc.FDb.ind_tclass|[amc.FTclass](/txt/gen/amc/amc.md#amc-ftclass)|[Thash](/txt/exe/amc/reftype.md#thash)|||
|amc.FDb.fcmp|[amc.FFcmp](/txt/gen/amc/amc.md#amc-ffcmp)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.fcast|[amc.FFcast](/txt/gen/amc/amc.md#amc-ffcast)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.nocascdel|[amc.FNocascdel](/txt/gen/amc/amc.md#amc-fnocascdel)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.cafter|[amc.FCafter](/txt/gen/amc/amc.md#amc-fcafter)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.csize|[amc.FCsize](/txt/gen/amc/amc.md#amc-fcsize)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.nsx|[amc.FNsx](/txt/gen/amc/amc.md#amc-fnsx)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.report|[report.amc](/txt/protocol/report/README.md#report-amc)|[Val](/txt/exe/amc/reftype.md#val)||Final report|
|amc.FDb.fcompact|[amc.FFcompact](/txt/gen/amc/amc.md#amc-ffcompact)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.fcurs|[amc.FFcurs](/txt/gen/amc/amc.md#amc-ffcurs)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.cdflt|[amc.FCdflt](/txt/gen/amc/amc.md#amc-fcdflt)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.argvtype|[amc.FArgvtype](/txt/gen/amc/amc.md#amc-fargvtype)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.ind_main|[amc.FMain](/txt/gen/amc/amc.md#amc-fmain)|[Thash](/txt/exe/amc/reftype.md#thash)|||
|amc.FDb.floadtuples|[amc.FFloadtuples](/txt/gen/amc/amc.md#amc-ffloadtuples)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.fcmap|[amc.FFcmap](/txt/gen/amc/amc.md#amc-ffcmap)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.lim_ind_func|u32|[Val](/txt/exe/amc/reftype.md#val)||Limit for ind_func table, used to enforce function creation|
|amc.FDb.zs_ordkeyfield|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Llist](/txt/exe/amc/reftype.md#llist)|||
|amc.FDb.lockfile|[algo_lib.FLockfile](/txt/gen/algo_lib/algo_lib.md#algo_lib-flockfile)|[Val](/txt/exe/amc/reftype.md#val)||Prevent concurrent execution|
|amc.FDb.nsproto|[amc.FNsproto](/txt/gen/amc/amc.md#amc-fnsproto)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.nsdb|[amc.FNsdb](/txt/gen/amc/amc.md#amc-fnsdb)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.zd_substr_params|[amc.FSubstr](/txt/gen/amc/amc.md#amc-fsubstr)|[Llist](/txt/exe/amc/reftype.md#llist)|||
|amc.FDb.ind_fconst_int|[amc.FFconst](/txt/gen/amc/amc.md#amc-ffconst)|[Thash](/txt/exe/amc/reftype.md#thash)||Used only during generation|
|amc.FDb.fprefix|[amc.FFprefix](/txt/gen/amc/amc.md#amc-ffprefix)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.ftrace|[amc.FFtrace](/txt/gen/amc/amc.md#amc-fftrace)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.c_ns|[amc.FNs](/txt/gen/amc/amc.md#amc-fns)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FDb.fnoremove|[amc.FFnoremove](/txt/gen/amc/amc.md#amc-ffnoremove)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.c_substr_field|[amc.FSubstr](/txt/gen/amc/amc.md#amc-fsubstr)|[Ptrary](/txt/exe/amc/reftype.md#ptrary)|||
|amc.FDb.ctypelen|[amc.FCtypelen](/txt/gen/amc/amc.md#amc-fctypelen)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.c_u64|[amc.FCtype](/txt/gen/amc/amc.md#amc-fctype)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FDb.ind_fcmap|[amc.FFcmap](/txt/gen/amc/amc.md#amc-ffcmap)|[Thash](/txt/exe/amc/reftype.md#thash)|||
|amc.FDb.nossimfile|[amc.FNossimfile](/txt/gen/amc/amc.md#amc-fnossimfile)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.gsymbol|[amc.FGsymbol](/txt/gen/amc/amc.md#amc-fgsymbol)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.sortfld|[amc.FSortfld](/txt/gen/amc/amc.md#amc-fsortfld)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.cget|[amc.FCget](/txt/gen/amc/amc.md#amc-fcget)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.cd_temp_func|[amc.FFunc](/txt/gen/amc/amc.md#amc-ffunc)|[Llist](/txt/exe/amc/reftype.md#llist)|||
|amc.FDb.zs_gen_perns|[amc.FGen](/txt/gen/amc/amc.md#amc-fgen)|[Llist](/txt/exe/amc/reftype.md#llist)|||
|amc.FDb.hook|[amc.FHook](/txt/gen/amc/amc.md#amc-fhook)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.charset|[amc.FCharset](/txt/gen/amc/amc.md#amc-fcharset)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.ValidRnumPad|[algo.Charset](/txt/protocol/algo/Charset.md)|[Charset](/txt/exe/amc/reftype.md#charset)|||
|amc.FDb.nsinclude|[amc.FNsinclude](/txt/gen/amc/amc.md#amc-fnsinclude)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.ssimvolatile|[amc.FSsimvolatile](/txt/gen/amc/amc.md#amc-fssimvolatile)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.funique|[amc.FFunique](/txt/gen/amc/amc.md#amc-ffunique)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.ind_fcurs|[amc.FFcurs](/txt/gen/amc/amc.md#amc-ffcurs)|[Thash](/txt/exe/amc/reftype.md#thash)|||
|amc.FDb.tcurs|[amc.FTcurs](/txt/gen/amc/amc.md#amc-ftcurs)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.cur_gen|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|||
|amc.FDb.has_ams_fwd_declare|bool|[Val](/txt/exe/amc/reftype.md#val)|||
|amc.FDb.nscpp|[amc.FNscpp](/txt/gen/amc/amc.md#amc-fnscpp)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.fflag|[amc.FFflag](/txt/gen/amc/amc.md#amc-ffflag)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.ind_fflag|[amc.FFflag](/txt/gen/amc/amc.md#amc-ffflag)|[Thash](/txt/exe/amc/reftype.md#thash)|||
|amc.FDb.falias|[amc.FFalias](/txt/gen/amc/amc.md#amc-ffalias)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.copyright|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|||
|amc.FDb.license|[amc.FLicense](/txt/gen/amc/amc.md#amc-flicense)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.ind_license|[amc.FLicense](/txt/gen/amc/amc.md#amc-flicense)|[Thash](/txt/exe/amc/reftype.md#thash)|||
|amc.FDb.c_ssimfile_sorted|[amc.FSsimfile](/txt/gen/amc/amc.md#amc-fssimfile)|[Ptrary](/txt/exe/amc/reftype.md#ptrary)||Global list of ssimfiles topologically sorted by pkey|
|amc.FDb.zd_ssimfile_todo|[amc.FSsimfile](/txt/gen/amc/amc.md#amc-fssimfile)|[Llist](/txt/exe/amc/reftype.md#llist)||Temporary, for sorting ssimfiles|
|amc.FDb.cfast|[amc.FCfast](/txt/gen/amc/amc.md#amc-fcfast)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.ffast|[amc.FFfast](/txt/gen/amc/amc.md#amc-fffast)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.pmaskfld_member|[amc.FPmaskfldMember](/txt/gen/amc/amc.md#amc-fpmaskfldmember)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.ind_pmaskfld|[amc.FPmaskfld](/txt/gen/amc/amc.md#amc-fpmaskfld)|[Thash](/txt/exe/amc/reftype.md#thash)|||
|amc.FDb.ssimsort|[amc.FSsimsort](/txt/gen/amc/amc.md#amc-fssimsort)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.fbuftype|[amc.FFbuftype](/txt/gen/amc/amc.md#amc-ffbuftype)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.ind_fbuftype|[amc.FFbuftype](/txt/gen/amc/amc.md#amc-ffbuftype)|[Thash](/txt/exe/amc/reftype.md#thash)|||
|amc.FDb.nsjs|[amc.FNsjs](/txt/gen/amc/amc.md#amc-fnsjs)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.jstype|[amc.FJstype](/txt/gen/amc/amc.md#amc-fjstype)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.ckafka|[amc.FCkafka](/txt/gen/amc/amc.md#amc-fckafka)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.fkafka|[amc.FFkafka](/txt/gen/amc/amc.md#amc-ffkafka)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.userfunc|[amc.FUserfunc](/txt/gen/amc/amc.md#amc-fuserfunc)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.steptype|[amc.FSteptype](/txt/gen/amc/amc.md#amc-fsteptype)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.ind_steptype|[amc.FSteptype](/txt/gen/amc/amc.md#amc-fsteptype)|[Thash](/txt/exe/amc/reftype.md#thash)|||
|amc.FDb.cjsbltin|[amc.FCjsbltin](/txt/gen/amc/amc.md#amc-fcjsbltin)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.cjson|[amc.FCjson](/txt/gen/amc/amc.md#amc-fcjson)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.ind_jsbltin|[amc.FJsbltin](/txt/gen/amc/amc.md#amc-fjsbltin)|[Thash](/txt/exe/amc/reftype.md#thash)|||
|amc.FDb.ccmdline|[amc.FCcmdline](/txt/gen/amc/amc.md#amc-fccmdline)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.cpbuf|[amc.FCpbuf](/txt/gen/amc/amc.md#amc-fcpbuf)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.fpbuf|[amc.FFpbuf](/txt/gen/amc/amc.md#amc-ffpbuf)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.fpbufcase|[amc.FFpbufcase](/txt/gen/amc/amc.md#amc-ffpbufcase)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.ffunc|[amc.FFfunc](/txt/gen/amc/amc.md#amc-fffunc)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.ind_ffunc|[amc.FFfunc](/txt/gen/amc/amc.md#amc-fffunc)|[Thash](/txt/exe/amc/reftype.md#thash)|||
|amc.FDb.blkhash|[amc.FBlkhash](/txt/gen/amc/amc.md#amc-fblkhash)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.sideload_noted|bool|[Val](/txt/exe/amc/reftype.md#val)|false|The side-load fallback notice has been printed|
|amc.FDb.dataroot|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||Root for side-loaded table data; -in_dir when it names a directory, else data|
|amc.FDb.dataroot_dflt|bool|[Val](/txt/exe/amc/reftype.md#val)|false|-in_dir names no directory; side tables load from the default data root|
|amc.FDb.n_generr|i32|[Val](/txt/exe/amc/reftype.md#val)|-1|exit_code captured when output begins (-1 before): generation errors, excluding write failures|
|amc.FDb.fcond|[amc.FFcond](/txt/gen/amc/amc.md#amc-ffcond)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.tcond|[amc.FTcond](/txt/gen/amc/amc.md#amc-ftcond)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.ind_tcond|[amc.FTcond](/txt/gen/amc/amc.md#amc-ftcond)|[Thash](/txt/exe/amc/reftype.md#thash)|||
|amc.FDb.payloadhdr|[amc.FPayloadhdr](/txt/gen/amc/amc.md#amc-fpayloadhdr)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.msg|[amc.FMsg](/txt/gen/amc/amc.md#amc-fmsg)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.msgfield|[amc.FMsgfield](/txt/gen/amc/amc.md#amc-fmsgfield)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.ind_msg_typekey|[amc.FMsg](/txt/gen/amc/amc.md#amc-fmsg)|[Thash](/txt/exe/amc/reftype.md#thash)||Msgtype uniqueness per payload header|
|amc.FDb.pbtype|[amc.FPbtype](/txt/gen/amc/amc.md#amc-fpbtype)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.ind_pbtype|[amc.FPbtype](/txt/gen/amc/amc.md#amc-fpbtype)|[Thash](/txt/exe/amc/reftype.md#thash)|||
|amc.FDb.pbsyntax|[amc.FPbsyntax](/txt/gen/amc/amc.md#amc-fpbsyntax)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|amc.FDb.ind_pbsyntax|[amc.FPbsyntax](/txt/gen/amc/amc.md#amc-fpbsyntax)|[Thash](/txt/exe/amc/reftype.md#thash)|||

#### Struct FDb
<a href="#struct-fdb"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FDb { // amc.FDb: In-memory database for amc
    lpool_Lpblock*              lpool_free[36];                             // Lpool levels
    lpool_Lpblk*                lpool_blk[11];                              // Dedicated blks with free space, per class
    amc::FFsort*                fsort_lary[36];                             // level array
    i64                         fsort_n;                                    // number of elements in array
    amc::FCfmt**                ind_cfmt_buckets_elems;                     // pointer to bucket array
    i32                         ind_cfmt_buckets_n;                         // number of elements in bucket array
    i32                         ind_cfmt_n;                                 // number of elements in the hash table
    amc::FDispfilter*           dispfilter_lary[36];                        // level array
    i64                         dispfilter_n;                               // number of elements in array
    amc::FUsertracefld*         usertracefld_lary[36];                      // level array
    i64                         usertracefld_n;                             // number of elements in array
    amc::FCfmt*                 cfmt_lary[36];                              // level array
    i64                         cfmt_n;                                     // number of elements in array
    command::amc                cmdline;                                    //
    amc::FDispatch*             dispatch_lary[36];                          // level array
    i64                         dispatch_n;                                 // number of elements in array
    amc::FDispatchmsg*          dispatch_msg_lary[36];                      // level array
    i64                         dispatch_msg_n;                             // number of elements in array
    amc::FCtype*                ctype_lary[36];                             // level array
    i64                         ctype_n;                                    // number of elements in array
    amc::FField*                field_lary[36];                             // level array
    i64                         field_n;                                    // number of elements in array
    amc::FBasepool*             basepool_lary[36];                          // level array
    i64                         basepool_n;                                 // number of elements in array
    amc::FLlist*                llist_lary[36];                             // level array
    i64                         llist_n;                                    // number of elements in array
    amc::FAnonfld*              anonfld_lary[36];                           // level array
    i64                         anonfld_n;                                  // number of elements in array
    amc::FXref*                 xref_lary[36];                              // level array
    i64                         xref_n;                                     // number of elements in array
    amc::FNs*                   ns_lary[36];                                // level array
    i64                         ns_n;                                       // number of elements in array
    amc::FPnew*                 pnew_lary[36];                              // level array
    i64                         pnew_n;                                     // number of elements in array
    amc::FFldoffset*            fldoffset_lary[36];                         // level array
    i64                         fldoffset_n;                                // number of elements in array
    amc::FTypefld*              typefld_lary[36];                           // level array
    i64                         typefld_n;                                  // number of elements in array
    amc::FLenfld*               lenfld_lary[36];                            // level array
    i64                         lenfld_n;                                   // number of elements in array
    amc::FBltin*                bltin_lary[36];                             // level array
    i64                         bltin_n;                                    // number of elements in array
    amc::FStatictuple*          static_tuple_lary[36];                      // level array
    i64                         static_tuple_n;                             // number of elements in array
    amc::FMsgtype*              msgtype_lary[36];                           // level array
    i64                         msgtype_n;                                  // number of elements in array
    amc::FGconst*               gconst_lary[36];                            // level array
    i64                         gconst_n;                                   // number of elements in array
    amc::FGstatic*              gstatic_lary[36];                           // level array
    i64                         gstatic_n;                                  // number of elements in array
    amc::FThash*                thash_lary[36];                             // level array
    i64                         thash_n;                                    // number of elements in array
    alignas(amc::FJsbltin) u8   jsbltin_data[sizeof(amc::FJsbltin) * 10];   // place for data
    i32                         jsbltin_n;                                  // number of elems current in existence
    enum { jsbltin_max = 10 };
    u64                         outfile_blocksize;                          // # bytes per block
    amc::FOutfile*              outfile_free;                               //
    amc::FFunc*                 func_lary[36];                              // level array
    i64                         func_n;                                     // number of elements in array
    amc::FSmallstr*             smallstr_lary[36];                          // level array
    i64                         smallstr_n;                                 // number of elements in array
    amc::FNumstr*               numstr_lary[36];                            // level array
    i64                         numstr_n;                                   // number of elements in array
    amc::FMain*                 main_lary[36];                              // level array
    i64                         main_n;                                     // number of elements in array
    amc::FReftype*              reftype_lary[36];                           // level array
    i64                         reftype_n;                                  // number of elements in array
    amc::FBltin**               ind_bltin_buckets_elems;                    // pointer to bucket array
    i32                         ind_bltin_buckets_n;                        // number of elements in bucket array
    i32                         ind_bltin_n;                                // number of elements in the hash table
    amc::FCtype**               ind_ctype_buckets_elems;                    // pointer to bucket array
    i32                         ind_ctype_buckets_n;                        // number of elements in bucket array
    i32                         ind_ctype_n;                                // number of elements in the hash table
    amc::FDispatch**            ind_dispatch_buckets_elems;                 // pointer to bucket array
    i32                         ind_dispatch_buckets_n;                     // number of elements in bucket array
    i32                         ind_dispatch_n;                             // number of elements in the hash table
    amc::FFunc**                ind_func_buckets_elems;                     // pointer to bucket array
    i32                         ind_func_buckets_n;                         // number of elements in bucket array
    i32                         ind_func_n;                                 // number of elements in the hash table
    amc::FField**               ind_field_buckets_elems;                    // pointer to bucket array
    i32                         ind_field_buckets_n;                        // number of elements in bucket array
    i32                         ind_field_n;                                // number of elements in the hash table
    amc::FNs**                  ind_ns_buckets_elems;                       // pointer to bucket array
    i32                         ind_ns_buckets_n;                           // number of elements in bucket array
    i32                         ind_ns_n;                                   // number of elements in the hash table
    amc::FPnew**                ind_pnew_buckets_elems;                     // pointer to bucket array
    i32                         ind_pnew_buckets_n;                         // number of elements in bucket array
    i32                         ind_pnew_n;                                 // number of elements in the hash table
    amc::FXref**                ind_xref_buckets_elems;                     // pointer to bucket array
    i32                         ind_xref_buckets_n;                         // number of elements in bucket array
    i32                         ind_xref_n;                                 // number of elements in the hash table
    amc::FCpptype*              cpptype_lary[36];                           // level array
    i64                         cpptype_n;                                  // number of elements in array
    amc::FCpptype**             ind_cpptype_buckets_elems;                  // pointer to bucket array
    i32                         ind_cpptype_buckets_n;                      // number of elements in bucket array
    i32                         ind_cpptype_n;                              // number of elements in the hash table
    amc::FInlary*               inlary_lary[36];                            // level array
    i64                         inlary_n;                                   // number of elements in array
    amc::FInlary**              ind_inlary_buckets_elems;                   // pointer to bucket array
    i32                         ind_inlary_buckets_n;                       // number of elements in bucket array
    i32                         ind_inlary_n;                               // number of elements in the hash table
    amc::FTary*                 tary_lary[36];                              // level array
    i64                         tary_n;                                     // number of elements in array
    amc::FTary**                ind_tary_buckets_elems;                     // pointer to bucket array
    i32                         ind_tary_buckets_n;                         // number of elements in bucket array
    i32                         ind_tary_n;                                 // number of elements in the hash table
    amc::FCppfunc*              cppfunc_lary[36];                           // level array
    i64                         cppfunc_n;                                  // number of elements in array
    amc::FRowid*                rowid_lary[36];                             // level array
    i64                         rowid_n;                                    // number of elements in array
    amc::FRowid**               ind_rowid_buckets_elems;                    // pointer to bucket array
    i32                         ind_rowid_buckets_n;                        // number of elements in bucket array
    i32                         ind_rowid_n;                                // number of elements in the hash table
    amc::FCascdel*              cascdel_lary[36];                           // level array
    i64                         cascdel_n;                                  // number of elements in array
    amc::FSubstr*               substr_lary[36];                            // level array
    i64                         substr_n;                                   // number of elements in array
    amc::FBitfld*               bitfld_lary[36];                            // level array
    i64                         bitfld_n;                                   // number of elements in array
    amc::FSsimfile*             ssimfile_lary[36];                          // level array
    i64                         ssimfile_n;                                 // number of elements in array
    amc::FSsimfile**            ind_ssimfile_buckets_elems;                 // pointer to bucket array
    i32                         ind_ssimfile_buckets_n;                     // number of elements in bucket array
    i32                         ind_ssimfile_n;                             // number of elements in the hash table
    amc::FPack*                 pack_lary[36];                              // level array
    i64                         pack_n;                                     // number of elements in array
    amc::FPack**                ind_pack_buckets_elems;                     // pointer to bucket array
    i32                         ind_pack_buckets_n;                         // number of elements in bucket array
    i32                         ind_pack_n;                                 // number of elements in the hash table
    amc::FSmallstr**            ind_smallstr_buckets_elems;                 // pointer to bucket array
    i32                         ind_smallstr_buckets_n;                     // number of elements in bucket array
    i32                         ind_smallstr_n;                             // number of elements in the hash table
    amc::FPtrary*               ptrary_lary[36];                            // level array
    i64                         ptrary_n;                                   // number of elements in array
    amc::FCtype**               c_ctype_sorted_elems;                       // array of pointers
    u64                         c_ctype_sorted_n;                           // current size
    u64                         c_ctype_sorted_max;                         // capacity of allocated array
    amc::FEnumstr*              enumstr_lary[36];                           // level array
    i64                         enumstr_n;                                  // number of elements in array
    amc::FEnumstrLen*           enumstr_len_lary[36];                       // level array
    i64                         enumstr_len_n;                              // number of elements in array
    amc::FEnumstrLen**          bh_enumstr_len_elems;                       // binary heap by len
    i32                         bh_enumstr_len_n;                           // number of elements in the heap
    i32                         bh_enumstr_len_max;                         // max elements in bh_enumstr_len_elems
    amc::FEnumstrLen**          ind_enumstr_len_buckets_elems;              // pointer to bucket array
    i32                         ind_enumstr_len_buckets_n;                  // number of elements in bucket array
    i32                         ind_enumstr_len_n;                          // number of elements in the hash table
    amc::FEnumstr**             ind_enumstr_buckets_elems;                  // pointer to bucket array
    i32                         ind_enumstr_buckets_n;                      // number of elements in bucket array
    i32                         ind_enumstr_n;                              // number of elements in the hash table
    amc::FFbitset*              fbitset_lary[36];                           // level array
    i64                         fbitset_n;                                  // number of elements in array
    amc::FFbitset**             ind_fbitset_buckets_elems;                  // pointer to bucket array
    i32                         ind_fbitset_buckets_n;                      // number of elements in bucket array
    i32                         ind_fbitset_n;                              // number of elements in the hash table
    amc::FFdec*                 fdec_lary[36];                              // level array
    i64                         fdec_n;                                     // number of elements in array
    amc::FFdec**                ind_fdec_buckets_elems;                     // pointer to bucket array
    i32                         ind_fdec_buckets_n;                         // number of elements in bucket array
    i32                         ind_fdec_n;                                 // number of elements in the hash table
    amc::FReftype**             ind_reftype_buckets_elems;                  // pointer to bucket array
    i32                         ind_reftype_buckets_n;                      // number of elements in bucket array
    i32                         ind_reftype_n;                              // number of elements in the hash table
    amc::FFconst*               fconst_lary[36];                            // level array
    i64                         fconst_n;                                   // number of elements in array
    amc::FFconst**              ind_fconst_buckets_elems;                   // pointer to bucket array
    i32                         ind_fconst_buckets_n;                       // number of elements in bucket array
    i32                         ind_fconst_n;                               // number of elements in the hash table
    amc::FNs**                  c_ns_sorted_elems;                          // array of pointers
    u64                         c_ns_sorted_n;                              // current size
    u64                         c_ns_sorted_max;                            // capacity of allocated array
    amc::FFinput*               finput_lary[36];                            // level array
    i64                         finput_n;                                   // number of elements in array
    amc::FFoutput*              foutput_lary[36];                           // level array
    i64                         foutput_n;                                  // number of elements in array
    amc::FFbuf*                 fbuf_lary[36];                              // level array
    i64                         fbuf_n;                                     // number of elements in array
    amc::FFbuf**                ind_fbuf_buckets_elems;                     // pointer to bucket array
    i32                         ind_fbuf_buckets_n;                         // number of elements in bucket array
    i32                         ind_fbuf_n;                                 // number of elements in the hash table
    amc::FChash*                chash_lary[36];                             // level array
    i64                         chash_n;                                    // number of elements in array
    amc::FChash**               ind_chash_buckets_elems;                    // pointer to bucket array
    i32                         ind_chash_buckets_n;                        // number of elements in bucket array
    i32                         ind_chash_n;                                // number of elements in the hash table
    amc::FCcmp*                 ccmp_lary[36];                              // level array
    i64                         ccmp_n;                                     // number of elements in array
    amc::FCcmp**                ind_ccmp_buckets_elems;                     // pointer to bucket array
    i32                         ind_ccmp_buckets_n;                         // number of elements in bucket array
    i32                         ind_ccmp_n;                                 // number of elements in the hash table
    amc::FFbigend*              fbigend_lary[36];                           // level array
    i64                         fbigend_n;                                  // number of elements in array
    amc::FCtype*                zsl_ctype_pack_tran_head;                   // zero-terminated singly linked list
    amc::FCstr*                 cstr_lary[36];                              // level array
    i64                         cstr_n;                                     // number of elements in array
    amc::FListtype*             listtype_lary[36];                          // level array
    i64                         listtype_n;                                 // number of elements in array
    amc::FListtype**            ind_listtype_buckets_elems;                 // pointer to bucket array
    i32                         ind_listtype_buckets_n;                     // number of elements in bucket array
    i32                         ind_listtype_n;                             // number of elements in the hash table
    amc::FFstep*                fstep_lary[36];                             // level array
    i64                         fstep_n;                                    // number of elements in array
    amc::FCextern*              cextern_lary[36];                           // level array
    i64                         cextern_n;                                  // number of elements in array
    amc::FFdelay*               fdelay_lary[36];                            // level array
    i64                         fdelay_n;                                   // number of elements in array
    amc::FDisptrace*            disptrace_lary[36];                         // level array
    i64                         disptrace_n;                                // number of elements in array
    amc::FFstep**               ind_fstep_buckets_elems;                    // pointer to bucket array
    i32                         ind_fstep_buckets_n;                        // number of elements in bucket array
    i32                         ind_fstep_n;                                // number of elements in the hash table
    amc::FTracefld*             tracefld_lary[36];                          // level array
    i64                         tracefld_n;                                 // number of elements in array
    amc::FTracerec*             tracerec_lary[36];                          // level array
    i64                         tracerec_n;                                 // number of elements in array
    amc::FDispsig*              dispsig_lary[36];                           // level array
    i64                         dispsig_n;                                  // number of elements in array
    amc::FCtype*                zs_sig_visit_head;                          // zero-terminated singly linked list
    amc::FCtype*                zs_sig_visit_tail;                          // pointer to last element
    amc::FTarget*               target_lary[36];                            // level array
    i64                         target_n;                                   // number of elements in array
    amc::FTarget**              ind_target_buckets_elems;                   // pointer to bucket array
    i32                         ind_target_buckets_n;                       // number of elements in bucket array
    i32                         ind_target_n;                               // number of elements in the hash table
    amc::FTargdep*              targdep_lary[36];                           // level array
    i64                         targdep_n;                                  // number of elements in array
    amc::FField*                c_malloc;                                   // optional pointer
    amc::FDispctx*              dispctx_lary[36];                           // level array
    i64                         dispctx_n;                                  // number of elements in array
    amc::FPmaskfld*             pmaskfld_lary[36];                          // level array
    i64                         pmaskfld_n;                                 // number of elements in array
    amc::Genctx                 genctx;                                     //
    amc::FFwddecl*              fwddecl_lary[36];                           // level array
    i64                         fwddecl_n;                                  // number of elements in array
    amc::FFwddecl**             ind_fwddecl_buckets_elems;                  // pointer to bucket array
    i32                         ind_fwddecl_buckets_n;                      // number of elements in bucket array
    i32                         ind_fwddecl_n;                              // number of elements in the hash table
    amc::FTfunc*                tfunc_lary[36];                             // level array
    i64                         tfunc_n;                                    // number of elements in array
    amc::FTfunc**               ind_tfunc_buckets_elems;                    // pointer to bucket array
    i32                         ind_tfunc_buckets_n;                        // number of elements in bucket array
    i32                         ind_tfunc_n;                                // number of elements in the hash table
    amc::FGen*                  gen_lary[36];                               // level array
    i64                         gen_n;                                      // number of elements in array
    amc::FNs*                   c_curns;                                    // optional pointer
    amc::FFregx*                fregx_lary[36];                             // level array
    i64                         fregx_n;                                    // number of elements in array
    alignas(amc::FTclass) u8    tclass_data[sizeof(amc::FTclass) * 53];     // place for data
    i32                         tclass_n;                                   // number of elems current in existence
    enum { tclass_max = 53 };
    amc::FTclass**              ind_tclass_buckets_elems;                   // pointer to bucket array
    i32                         ind_tclass_buckets_n;                       // number of elements in bucket array
    i32                         ind_tclass_n;                               // number of elements in the hash table
    amc::FFcmp*                 fcmp_lary[36];                              // level array
    i64                         fcmp_n;                                     // number of elements in array
    amc::FFcast*                fcast_lary[36];                             // level array
    i64                         fcast_n;                                    // number of elements in array
    amc::FNocascdel*            nocascdel_lary[36];                         // level array
    i64                         nocascdel_n;                                // number of elements in array
    amc::FCafter*               cafter_lary[36];                            // level array
    i64                         cafter_n;                                   // number of elements in array
    amc::FCsize*                csize_lary[36];                             // level array
    i64                         csize_n;                                    // number of elements in array
    amc::FNsx*                  nsx_lary[36];                               // level array
    i64                         nsx_n;                                      // number of elements in array
    report::amc                 report;                                     // Final report
    amc::FFcompact*             fcompact_lary[36];                          // level array
    i64                         fcompact_n;                                 // number of elements in array
    amc::FFcurs*                fcurs_lary[36];                             // level array
    i64                         fcurs_n;                                    // number of elements in array
    amc::FCdflt*                cdflt_lary[36];                             // level array
    i64                         cdflt_n;                                    // number of elements in array
    amc::FArgvtype*             argvtype_lary[36];                          // level array
    i64                         argvtype_n;                                 // number of elements in array
    amc::FMain**                ind_main_buckets_elems;                     // pointer to bucket array
    i32                         ind_main_buckets_n;                         // number of elements in bucket array
    i32                         ind_main_n;                                 // number of elements in the hash table
    amc::FFloadtuples*          floadtuples_lary[36];                       // level array
    i64                         floadtuples_n;                              // number of elements in array
    amc::FFcmap*                fcmap_lary[36];                             // level array
    i64                         fcmap_n;                                    // number of elements in array
    u32                         lim_ind_func;                               //   0  Limit for ind_func table, used to enforce function creation
    amc::FField*                zs_ordkeyfield_head;                        // zero-terminated singly linked list
    amc::FField*                zs_ordkeyfield_tail;                        // pointer to last element
    algo_lib::FLockfile         lockfile;                                   // Prevent concurrent execution
    amc::FNsproto*              nsproto_lary[36];                           // level array
    i64                         nsproto_n;                                  // number of elements in array
    amc::FNsdb*                 nsdb_lary[36];                              // level array
    i64                         nsdb_n;                                     // number of elements in array
    amc::FSubstr*               zd_substr_params_head;                      // zero-terminated doubly linked list
    i32                         zd_substr_params_n;                         // zero-terminated doubly linked list
    amc::FSubstr*               zd_substr_params_tail;                      // pointer to last element
    amc::FFconst**              ind_fconst_int_buckets_elems;               // pointer to bucket array
    i32                         ind_fconst_int_buckets_n;                   // number of elements in bucket array
    i32                         ind_fconst_int_n;                           // number of elements in the hash table
    amc::FFprefix*              fprefix_lary[36];                           // level array
    i64                         fprefix_n;                                  // number of elements in array
    amc::FFtrace*               ftrace_lary[36];                            // level array
    i64                         ftrace_n;                                   // number of elements in array
    amc::FNs*                   c_ns;                                       // optional pointer
    amc::FFnoremove*            fnoremove_lary[36];                         // level array
    i64                         fnoremove_n;                                // number of elements in array
    amc::FSubstr**              c_substr_field_elems;                       // array of pointers
    u64                         c_substr_field_n;                           // current size
    u64                         c_substr_field_max;                         // capacity of allocated array
    amc::FCtypelen*             ctypelen_lary[36];                          // level array
    i64                         ctypelen_n;                                 // number of elements in array
    amc::FCtype*                c_u64;                                      // optional pointer
    amc::FFcmap**               ind_fcmap_buckets_elems;                    // pointer to bucket array
    i32                         ind_fcmap_buckets_n;                        // number of elements in bucket array
    i32                         ind_fcmap_n;                                // number of elements in the hash table
    amc::FNossimfile*           nossimfile_lary[36];                        // level array
    i64                         nossimfile_n;                               // number of elements in array
    amc::FGsymbol*              gsymbol_lary[36];                           // level array
    i64                         gsymbol_n;                                  // number of elements in array
    amc::FSortfld*              sortfld_lary[36];                           // level array
    i64                         sortfld_n;                                  // number of elements in array
    amc::FCget*                 cget_lary[36];                              // level array
    i64                         cget_n;                                     // number of elements in array
    amc::FFunc*                 cd_temp_func_head;                          // zero-terminated doubly linked list
    i32                         cd_temp_func_n;                             // zero-terminated doubly linked list
    amc::FGen*                  zs_gen_perns_head;                          // zero-terminated singly linked list
    amc::FGen*                  zs_gen_perns_tail;                          // pointer to last element
    amc::FHook*                 hook_lary[36];                              // level array
    i64                         hook_n;                                     // number of elements in array
    amc::FCharset*              charset_lary[36];                           // level array
    i64                         charset_n;                                  // number of elements in array
    algo::Charset               ValidRnumPad;                               //
    amc::FNsinclude*            nsinclude_lary[36];                         // level array
    i64                         nsinclude_n;                                // number of elements in array
    amc::FSsimvolatile*         ssimvolatile_lary[36];                      // level array
    i64                         ssimvolatile_n;                             // number of elements in array
    amc::FFunique*              funique_lary[36];                           // level array
    i64                         funique_n;                                  // number of elements in array
    amc::FFcurs**               ind_fcurs_buckets_elems;                    // pointer to bucket array
    i32                         ind_fcurs_buckets_n;                        // number of elements in bucket array
    i32                         ind_fcurs_n;                                // number of elements in the hash table
    amc::FTcurs*                tcurs_lary[36];                             // level array
    i64                         tcurs_n;                                    // number of elements in array
    algo::cstring               cur_gen;                                    //
    bool                        has_ams_fwd_declare;                        //   false
    amc::FNscpp*                nscpp_lary[36];                             // level array
    i64                         nscpp_n;                                    // number of elements in array
    amc::FFflag*                fflag_lary[36];                             // level array
    i64                         fflag_n;                                    // number of elements in array
    amc::FFflag**               ind_fflag_buckets_elems;                    // pointer to bucket array
    i32                         ind_fflag_buckets_n;                        // number of elements in bucket array
    i32                         ind_fflag_n;                                // number of elements in the hash table
    amc::FFalias*               falias_lary[36];                            // level array
    i64                         falias_n;                                   // number of elements in array
    algo::cstring               copyright;                                  //
    amc::FLicense*              license_lary[36];                           // level array
    i64                         license_n;                                  // number of elements in array
    amc::FLicense**             ind_license_buckets_elems;                  // pointer to bucket array
    i32                         ind_license_buckets_n;                      // number of elements in bucket array
    i32                         ind_license_n;                              // number of elements in the hash table
    amc::FSsimfile**            c_ssimfile_sorted_elems;                    // array of pointers
    u64                         c_ssimfile_sorted_n;                        // current size
    u64                         c_ssimfile_sorted_max;                      // capacity of allocated array
    amc::FSsimfile*             zd_ssimfile_todo_head;                      // zero-terminated doubly linked list
    i32                         zd_ssimfile_todo_n;                         // zero-terminated doubly linked list
    amc::FSsimfile*             zd_ssimfile_todo_tail;                      // pointer to last element
    amc::FCfast*                cfast_lary[36];                             // level array
    i64                         cfast_n;                                    // number of elements in array
    amc::FFfast*                ffast_lary[36];                             // level array
    i64                         ffast_n;                                    // number of elements in array
    amc::FPmaskfldMember*       pmaskfld_member_lary[36];                   // level array
    i64                         pmaskfld_member_n;                          // number of elements in array
    amc::FPmaskfld**            ind_pmaskfld_buckets_elems;                 // pointer to bucket array
    i32                         ind_pmaskfld_buckets_n;                     // number of elements in bucket array
    i32                         ind_pmaskfld_n;                             // number of elements in the hash table
    amc::FSsimsort*             ssimsort_lary[36];                          // level array
    i64                         ssimsort_n;                                 // number of elements in array
    amc::FFbuftype*             fbuftype_lary[36];                          // level array
    i64                         fbuftype_n;                                 // number of elements in array
    amc::FFbuftype**            ind_fbuftype_buckets_elems;                 // pointer to bucket array
    i32                         ind_fbuftype_buckets_n;                     // number of elements in bucket array
    i32                         ind_fbuftype_n;                             // number of elements in the hash table
    amc::FNsjs*                 nsjs_lary[36];                              // level array
    i64                         nsjs_n;                                     // number of elements in array
    amc::FJstype*               jstype_lary[36];                            // level array
    i64                         jstype_n;                                   // number of elements in array
    amc::FCkafka*               ckafka_lary[36];                            // level array
    i64                         ckafka_n;                                   // number of elements in array
    amc::FFkafka*               fkafka_lary[36];                            // level array
    i64                         fkafka_n;                                   // number of elements in array
    amc::FUserfunc*             userfunc_lary[36];                          // level array
    i64                         userfunc_n;                                 // number of elements in array
    amc::FSteptype*             steptype_lary[36];                          // level array
    i64                         steptype_n;                                 // number of elements in array
    amc::FSteptype**            ind_steptype_buckets_elems;                 // pointer to bucket array
    i32                         ind_steptype_buckets_n;                     // number of elements in bucket array
    i32                         ind_steptype_n;                             // number of elements in the hash table
    amc::FCjsbltin*             cjsbltin_lary[36];                          // level array
    i64                         cjsbltin_n;                                 // number of elements in array
    amc::FCjson*                cjson_lary[36];                             // level array
    i64                         cjson_n;                                    // number of elements in array
    amc::FJsbltin**             ind_jsbltin_buckets_elems;                  // pointer to bucket array
    i32                         ind_jsbltin_buckets_n;                      // number of elements in bucket array
    i32                         ind_jsbltin_n;                              // number of elements in the hash table
    amc::FCcmdline*             ccmdline_lary[36];                          // level array
    i64                         ccmdline_n;                                 // number of elements in array
    amc::FCpbuf*                cpbuf_lary[36];                             // level array
    i64                         cpbuf_n;                                    // number of elements in array
    amc::FFpbuf*                fpbuf_lary[36];                             // level array
    i64                         fpbuf_n;                                    // number of elements in array
    amc::FFpbufcase*            fpbufcase_lary[36];                         // level array
    i64                         fpbufcase_n;                                // number of elements in array
    amc::FFfunc*                ffunc_lary[36];                             // level array
    i64                         ffunc_n;                                    // number of elements in array
    amc::FFfunc**               ind_ffunc_buckets_elems;                    // pointer to bucket array
    i32                         ind_ffunc_buckets_n;                        // number of elements in bucket array
    i32                         ind_ffunc_n;                                // number of elements in the hash table
    amc::FBlkhash*              blkhash_lary[36];                           // level array
    i64                         blkhash_n;                                  // number of elements in array
    bool                        sideload_noted;                             //   false  The side-load fallback notice has been printed
    algo::cstring               dataroot;                                   // Root for side-loaded table data; -in_dir when it names a directory, else data
    bool                        dataroot_dflt;                              //   false  -in_dir names no directory; side tables load from the default data root
    i32                         n_generr;                                   //   -1  exit_code captured when output begins (-1 before): generation errors, excluding write failures
    amc::FFcond*                fcond_lary[36];                             // level array
    i64                         fcond_n;                                    // number of elements in array
    amc::FTcond*                tcond_lary[36];                             // level array
    i64                         tcond_n;                                    // number of elements in array
    amc::FTcond**               ind_tcond_buckets_elems;                    // pointer to bucket array
    i32                         ind_tcond_buckets_n;                        // number of elements in bucket array
    i32                         ind_tcond_n;                                // number of elements in the hash table
    amc::FPayloadhdr*           payloadhdr_lary[36];                        // level array
    i64                         payloadhdr_n;                               // number of elements in array
    amc::FMsg*                  msg_lary[36];                               // level array
    i64                         msg_n;                                      // number of elements in array
    amc::FMsgfield*             msgfield_lary[36];                          // level array
    i64                         msgfield_n;                                 // number of elements in array
    amc::FMsg**                 ind_msg_typekey_buckets_elems;              // pointer to bucket array
    i32                         ind_msg_typekey_buckets_n;                  // number of elements in bucket array
    i32                         ind_msg_typekey_n;                          // number of elements in the hash table
    amc::FPbtype*               pbtype_lary[36];                            // level array
    i64                         pbtype_n;                                   // number of elements in array
    amc::FPbtype**              ind_pbtype_buckets_elems;                   // pointer to bucket array
    i32                         ind_pbtype_buckets_n;                       // number of elements in bucket array
    i32                         ind_pbtype_n;                               // number of elements in the hash table
    amc::FPbsyntax*             pbsyntax_lary[36];                          // level array
    i64                         pbsyntax_n;                                 // number of elements in array
    amc::FPbsyntax**            ind_pbsyntax_buckets_elems;                 // pointer to bucket array
    i32                         ind_pbsyntax_buckets_n;                     // number of elements in bucket array
    i32                         ind_pbsyntax_n;                             // number of elements in the hash table
    amc::trace                  trace;                                      //
};
```

#### amc.FDispatch - Generate code for a multi-way branch
<a href="#amc-fdispatch"></a>

#### amc.FDispatch Fields
<a href="#amc-fdispatch-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FDispatch.msghdr|[dmmeta.Dispatch](/txt/ssimdb/dmmeta/dispatch.md)|[Base](/txt/ssimdb/dmmeta/dispatch.md)|||
|amc.FDispatch.dyn|bool|[Val](/txt/exe/amc/reftype.md#val)||Use dynamic memory allocation: new, delete instead of  ByteAry|
|amc.FDispatch.kafka|bool|[Val](/txt/exe/amc/reftype.md#val)||generate kafka codec|
|amc.FDispatch.p_ctype_hdr|[amc.FCtype](/txt/gen/amc/amc.md#amc-fctype)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|amc.FDispatch.p_casetype|[amc.FCtype](/txt/gen/amc/amc.md#amc-fctype)|[Upptr](/txt/exe/amc/reftype.md#upptr)||Optional casetype|
|amc.FDispatch.c_dispfilter|[amc.FDispfilter](/txt/gen/amc/amc.md#amc-fdispfilter)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FDispatch.p_ns|[amc.FNs](/txt/gen/amc/amc.md#amc-fns)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|amc.FDispatch.c_dispatch_msg|[amc.FDispatchmsg](/txt/gen/amc/amc.md#amc-fdispatchmsg)|[Ptrary](/txt/exe/amc/reftype.md#ptrary)|||
|amc.FDispatch.c_disptrace|[amc.FDisptrace](/txt/gen/amc/amc.md#amc-fdisptrace)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FDispatch.signature|[algo.Sha1sig](/txt/protocol/algo/Sha1sig.md)|[Val](/txt/exe/amc/reftype.md#val)|||
|amc.FDispatch.c_dispctx|[amc.FDispctx](/txt/gen/amc/amc.md#amc-fdispctx)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||

#### Struct FDispatch
<a href="#struct-fdispatch"></a>
*Note:* field ``amc.FDispatch.msghdr`` has reftype ``base`` so the fields of [dmmeta.Dispatch](/txt/ssimdb/dmmeta/dispatch.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FDispatch { // amc.FDispatch
    amc::FDispatch*       ind_dispatch_next;      // hash next
    u32                   ind_dispatch_hashval;   // hash value
    algo::Smallstr50      dispatch;               // Primary key (ns.name)
    bool                  unk;                    //   false  Want default case?
    bool                  read;                   //   false  Generate read function
    bool                  print;                  //   false  Generate print function
    bool                  haslen;                 //   false  Include length in dispatch function
    bool                  call;                   //   false  Generate call to user-defined function
    bool                  strict;                 //   false  Only dispatch if length matches exactly
    algo::cstring         comment;                //
    bool                  dyn;                    //   false  Use dynamic memory allocation: new, delete instead of  ByteAry
    bool                  kafka;                  //   false  generate kafka codec
    amc::FCtype*          p_ctype_hdr;            // reference to parent row
    amc::FCtype*          p_casetype;             // reference to parent row
    amc::FDispfilter*     c_dispfilter;           // optional pointer
    amc::FNs*             p_ns;                   // reference to parent row
    amc::FDispatchmsg**   c_dispatch_msg_elems;   // array of pointers
    u64                   c_dispatch_msg_n;       // current size
    u64                   c_dispatch_msg_max;     // capacity of allocated array
    amc::FDisptrace*      c_disptrace;            // optional pointer
    algo::Sha1sig         signature;              //
    amc::FDispctx*        c_dispctx;              // optional pointer
    bool                  ns_c_dispatch_in_ary;   //   false  membership flag
    // x-reference on amc.FDispatch.c_dispfilter prevents copy
    // x-reference on amc.FDispatch.p_ns prevents copy
    // reftype Ptrary of amc.FDispatch.c_dispatch_msg prohibits copy
    // x-reference on amc.FDispatch.c_disptrace prevents copy
    // x-reference on amc.FDispatch.c_dispctx prevents copy
    // func:amc.FDispatch..AssignOp
    amc::FDispatch&      operator =(const amc::FDispatch &rhs) = delete;
    // x-reference on amc.FDispatch.c_dispfilter prevents copy
    // x-reference on amc.FDispatch.p_ns prevents copy
    // reftype Ptrary of amc.FDispatch.c_dispatch_msg prohibits copy
    // x-reference on amc.FDispatch.c_disptrace prevents copy
    // x-reference on amc.FDispatch.c_dispctx prevents copy
    // func:amc.FDispatch..CopyCtor
    FDispatch(const amc::FDispatch &rhs) = delete;
private:
    // func:amc.FDispatch..Ctor
    inline               FDispatch() __attribute__((nothrow));
    // func:amc.FDispatch..Dtor
    inline               ~FDispatch() __attribute__((nothrow));
    friend amc::FDispatch&      dispatch_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FDispatch*      dispatch_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 dispatch_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FDispatchmsg - Add message to a dispatch
<a href="#amc-fdispatchmsg"></a>

#### amc.FDispatchmsg Fields
<a href="#amc-fdispatchmsg-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FDispatchmsg.msghdr|[dmmeta.DispatchMsg](/txt/ssimdb/dmmeta/dispatch_msg.md)|[Base](/txt/ssimdb/dmmeta/dispatch_msg.md)|||
|amc.FDispatchmsg.p_ctype|[amc.FCtype](/txt/gen/amc/amc.md#amc-fctype)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|amc.FDispatchmsg.p_dispatch|[amc.FDispatch](/txt/gen/amc/amc.md#amc-fdispatch)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|amc.FDispatchmsg.selector|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|||

#### Struct FDispatchmsg
<a href="#struct-fdispatchmsg"></a>
*Note:* field ``amc.FDispatchmsg.msghdr`` has reftype ``base`` so the fields of [dmmeta.DispatchMsg](/txt/ssimdb/dmmeta/dispatch_msg.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FDispatchmsg { // amc.FDispatchmsg
    algo::Smallstr100   dispatch_msg;                     //
    algo::cstring       comment;                          //
    amc::FCtype*        p_ctype;                          // reference to parent row
    amc::FDispatch*     p_dispatch;                       // reference to parent row
    algo::cstring       selector;                         //
    bool                dispatch_c_dispatch_msg_in_ary;   //   false  membership flag
    // x-reference on amc.FDispatchmsg.p_ctype prevents copy
    // x-reference on amc.FDispatchmsg.p_dispatch prevents copy
    // func:amc.FDispatchmsg..AssignOp
    inline amc::FDispatchmsg& operator =(const amc::FDispatchmsg &rhs) = delete;
    // x-reference on amc.FDispatchmsg.p_ctype prevents copy
    // x-reference on amc.FDispatchmsg.p_dispatch prevents copy
    // func:amc.FDispatchmsg..CopyCtor
    inline               FDispatchmsg(const amc::FDispatchmsg &rhs) = delete;
private:
    // func:amc.FDispatchmsg..Ctor
    inline               FDispatchmsg() __attribute__((nothrow));
    // func:amc.FDispatchmsg..Dtor
    inline               ~FDispatchmsg() __attribute__((nothrow));
    friend amc::FDispatchmsg&   dispatch_msg_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FDispatchmsg*   dispatch_msg_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 dispatch_msg_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FDispctx - Use context with dispatch
<a href="#amc-fdispctx"></a>

#### amc.FDispctx Fields
<a href="#amc-fdispctx-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FDispctx.msghdr|[dmmeta.Dispctx](/txt/ssimdb/dmmeta/dispctx.md)|[Base](/txt/ssimdb/dmmeta/dispctx.md)|||
|amc.FDispctx.p_ctype|[amc.FCtype](/txt/gen/amc/amc.md#amc-fctype)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||

#### Struct FDispctx
<a href="#struct-fdispctx"></a>
*Note:* field ``amc.FDispctx.msghdr`` has reftype ``base`` so the fields of [dmmeta.Dispctx](/txt/ssimdb/dmmeta/dispctx.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FDispctx { // amc.FDispctx
    algo::Smallstr50    dispatch;   //
    algo::Smallstr100   ctype;      //
    algo::cstring       comment;    //
    amc::FCtype*        p_ctype;    // reference to parent row
    // x-reference on amc.FDispctx.p_ctype prevents copy
    // func:amc.FDispctx..AssignOp
    inline amc::FDispctx& operator =(const amc::FDispctx &rhs) = delete;
    // x-reference on amc.FDispctx.p_ctype prevents copy
    // func:amc.FDispctx..CopyCtor
    inline               FDispctx(const amc::FDispctx &rhs) = delete;
private:
    // func:amc.FDispctx..Ctor
    inline               FDispctx() __attribute__((nothrow));
    // func:amc.FDispctx..Dtor
    inline               ~FDispctx() __attribute__((nothrow));
    friend amc::FDispctx&       dispctx_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FDispctx*       dispctx_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 dispctx_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FDispfilter - Generate filter function on dispatch
<a href="#amc-fdispfilter"></a>

#### amc.FDispfilter Fields
<a href="#amc-fdispfilter-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FDispfilter.msghdr|[dmmeta.Dispfilter](/txt/ssimdb/dmmeta/dispfilter.md)|[Base](/txt/ssimdb/dmmeta/dispfilter.md)|||

#### Struct FDispfilter
<a href="#struct-fdispfilter"></a>
*Note:* field ``amc.FDispfilter.msghdr`` has reftype ``base`` so the fields of [dmmeta.Dispfilter](/txt/ssimdb/dmmeta/dispfilter.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FDispfilter { // amc.FDispfilter
    algo::Smallstr50   dispatch;    //
    bool               match_all;   //   false
    algo::cstring      comment;     //
    // func:amc.FDispfilter..AssignOp
    inline amc::FDispfilter& operator =(const amc::FDispfilter &rhs) = delete;
    // func:amc.FDispfilter..CopyCtor
    inline               FDispfilter(const amc::FDispfilter &rhs) = delete;
private:
    // func:amc.FDispfilter..Ctor
    inline               FDispfilter() __attribute__((nothrow));
    // func:amc.FDispfilter..Dtor
    inline               ~FDispfilter() __attribute__((nothrow));
    friend amc::FDispfilter&    dispfilter_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FDispfilter*    dispfilter_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 dispfilter_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FDispsig - Cryptographic signature of all dispatches
<a href="#amc-fdispsig"></a>

#### amc.FDispsig Fields
<a href="#amc-fdispsig-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FDispsig.msghdr|[dmmeta.Dispsig](/txt/ssimdb/dmmeta/dispsig.md)|[Base](/txt/ssimdb/dmmeta/dispsig.md)|||

#### Struct FDispsig
<a href="#struct-fdispsig"></a>
*Note:* field ``amc.FDispsig.msghdr`` has reftype ``base`` so the fields of [dmmeta.Dispsig](/txt/ssimdb/dmmeta/dispsig.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FDispsig { // amc.FDispsig
    algo::Smallstr50   dispsig;               //
    algo::Sha1sig      signature;             //
    bool               ns_c_dispsig_in_ary;   //   false  membership flag
    // func:amc.FDispsig..AssignOp
    inline amc::FDispsig& operator =(const amc::FDispsig &rhs) = delete;
    // func:amc.FDispsig..CopyCtor
    inline               FDispsig(const amc::FDispsig &rhs) = delete;
private:
    // func:amc.FDispsig..Ctor
    inline               FDispsig() __attribute__((nothrow));
    // func:amc.FDispsig..Dtor
    inline               ~FDispsig() __attribute__((nothrow));
    friend amc::FDispsig&       dispsig_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FDispsig*       dispsig_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 dispsig_RemoveAll() __attribute__((nothrow));
    friend void                 dispsig_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FDisptrace - Generate trace fields (cycles, counts) for all dispatch branches
<a href="#amc-fdisptrace"></a>

#### amc.FDisptrace Fields
<a href="#amc-fdisptrace-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FDisptrace.msghdr|[dmmeta.Disptrace](/txt/ssimdb/dmmeta/disptrace.md)|[Base](/txt/ssimdb/dmmeta/disptrace.md)|||
|amc.FDisptrace.p_dispatch|[amc.FDispatch](/txt/gen/amc/amc.md#amc-fdispatch)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||

#### Struct FDisptrace
<a href="#struct-fdisptrace"></a>
*Note:* field ``amc.FDisptrace.msghdr`` has reftype ``base`` so the fields of [dmmeta.Disptrace](/txt/ssimdb/dmmeta/disptrace.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FDisptrace { // amc.FDisptrace
    algo::Smallstr50   dispatch;     //
    bool               cycle;        //   false
    algo::cstring      comment;      //
    amc::FDispatch*    p_dispatch;   // reference to parent row
    // x-reference on amc.FDisptrace.p_dispatch prevents copy
    // func:amc.FDisptrace..AssignOp
    inline amc::FDisptrace& operator =(const amc::FDisptrace &rhs) = delete;
    // x-reference on amc.FDisptrace.p_dispatch prevents copy
    // func:amc.FDisptrace..CopyCtor
    inline               FDisptrace(const amc::FDisptrace &rhs) = delete;
private:
    // func:amc.FDisptrace..Ctor
    inline               FDisptrace() __attribute__((nothrow));
    // func:amc.FDisptrace..Dtor
    inline               ~FDisptrace() __attribute__((nothrow));
    friend amc::FDisptrace&     disptrace_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FDisptrace*     disptrace_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 disptrace_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FEnumstr - All values of a given length
<a href="#amc-fenumstr"></a>

#### amc.FEnumstr Fields
<a href="#amc-fenumstr-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FEnumstr.enumstr|[amc.Enumstr](/txt/gen/amc/amc.md#amc-enumstr)|[Val](/txt/exe/amc/reftype.md#val)|||
|amc.FEnumstr.c_fconst|[amc.FFconst](/txt/gen/amc/amc.md#amc-ffconst)|[Ptrary](/txt/exe/amc/reftype.md#ptrary)||for fconst use|

#### Struct FEnumstr
<a href="#struct-fenumstr"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FEnumstr { // amc.FEnumstr: All values of a given length
    amc::FEnumstr*   ind_enumstr_next;             // hash next
    u32              ind_enumstr_hashval;          // hash value
    amc::Enumstr     enumstr;                      //
    amc::FFconst**   c_fconst_elems;               // array of pointers
    u64              c_fconst_n;                   // current size
    u64              c_fconst_max;                 // capacity of allocated array
    i32              enumstr_len_bh_enumstr_idx;   // index in heap; -1 means not-in-heap
    // reftype Ptrary of amc.FEnumstr.c_fconst prohibits copy
    // func:amc.FEnumstr..AssignOp
    inline amc::FEnumstr& operator =(const amc::FEnumstr &rhs) = delete;
    // reftype Ptrary of amc.FEnumstr.c_fconst prohibits copy
    // func:amc.FEnumstr..CopyCtor
    inline               FEnumstr(const amc::FEnumstr &rhs) = delete;
private:
    // func:amc.FEnumstr..Ctor
    inline               FEnumstr() __attribute__((nothrow));
    // func:amc.FEnumstr..Dtor
    inline               ~FEnumstr() __attribute__((nothrow));
    friend amc::FEnumstr&       enumstr_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FEnumstr*       enumstr_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 enumstr_RemoveAll() __attribute__((nothrow));
    friend void                 enumstr_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FEnumstrLen
<a href="#amc-fenumstrlen"></a>

#### amc.FEnumstrLen Fields
<a href="#amc-fenumstrlen-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FEnumstrLen.len|i32|[Val](/txt/exe/amc/reftype.md#val)|||
|amc.FEnumstrLen.bh_enumstr|[amc.FEnumstr](/txt/gen/amc/amc.md#amc-fenumstr)|[Bheap](/txt/exe/amc/reftype.md#bheap)||List of FEnumstr by string value|

#### Struct FEnumstrLen
<a href="#struct-fenumstrlen"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FEnumstrLen { // amc.FEnumstrLen
    i32                 bh_enumstr_len_idx;        // index in heap; -1 means not-in-heap
    amc::FEnumstrLen*   ind_enumstr_len_next;      // hash next
    u32                 ind_enumstr_len_hashval;   // hash value
    i32                 len;                       //   0
    amc::FEnumstr**     bh_enumstr_elems;          // binary heap by str
    i32                 bh_enumstr_n;              // number of elements in the heap
    i32                 bh_enumstr_max;            // max elements in bh_enumstr_elems
    // reftype Bheap of amc.FEnumstrLen.bh_enumstr prohibits copy
    // func:amc.FEnumstrLen..AssignOp
    inline amc::FEnumstrLen& operator =(const amc::FEnumstrLen &rhs) = delete;
    // reftype Bheap of amc.FEnumstrLen.bh_enumstr prohibits copy
    // func:amc.FEnumstrLen..CopyCtor
    inline               FEnumstrLen(const amc::FEnumstrLen &rhs) = delete;
private:
    // func:amc.FEnumstrLen..Ctor
    inline               FEnumstrLen() __attribute__((nothrow));
    // func:amc.FEnumstrLen..Dtor
    inline               ~FEnumstrLen() __attribute__((nothrow));
    friend amc::FEnumstrLen&    enumstr_len_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FEnumstrLen*    enumstr_len_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 enumstr_len_RemoveAll() __attribute__((nothrow));
    friend void                 enumstr_len_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FFalias - Alias field
<a href="#amc-ffalias"></a>

#### amc.FFalias Fields
<a href="#amc-ffalias-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FFalias.base|[dmmeta.Falias](/txt/ssimdb/dmmeta/falias.md)|[Base](/txt/ssimdb/dmmeta/falias.md)|||
|amc.FFalias.p_srcfield|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||

#### Struct FFalias
<a href="#struct-ffalias"></a>
*Note:* field ``amc.FFalias.base`` has reftype ``base`` so the fields of [dmmeta.Falias](/txt/ssimdb/dmmeta/falias.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FFalias { // amc.FFalias
    algo::Smallstr150   field;        //
    algo::Smallstr150   srcfield;     //
    algo::cstring       comment;      //
    amc::FField*        p_srcfield;   // reference to parent row
    // x-reference on amc.FFalias.p_srcfield prevents copy
    // func:amc.FFalias..AssignOp
    inline amc::FFalias& operator =(const amc::FFalias &rhs) = delete;
    // x-reference on amc.FFalias.p_srcfield prevents copy
    // func:amc.FFalias..CopyCtor
    inline               FFalias(const amc::FFalias &rhs) = delete;
private:
    // func:amc.FFalias..Ctor
    inline               FFalias() __attribute__((nothrow));
    // func:amc.FFalias..Dtor
    inline               ~FFalias() __attribute__((nothrow));
    friend amc::FFalias&        falias_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FFalias*        falias_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 falias_RemoveAll() __attribute__((nothrow));
    friend void                 falias_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FFbigend - Annotate field as having big-endian storage
<a href="#amc-ffbigend"></a>

#### amc.FFbigend Fields
<a href="#amc-ffbigend-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FFbigend.msghdr|[dmmeta.Fbigend](/txt/ssimdb/dmmeta/fbigend.md)|[Base](/txt/ssimdb/dmmeta/fbigend.md)|||
|amc.FFbigend.p_field|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||

#### Struct FFbigend
<a href="#struct-ffbigend"></a>
*Note:* field ``amc.FFbigend.msghdr`` has reftype ``base`` so the fields of [dmmeta.Fbigend](/txt/ssimdb/dmmeta/fbigend.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FFbigend { // amc.FFbigend
    algo::Smallstr150   field;     //
    algo::cstring       comment;   //
    amc::FField*        p_field;   // reference to parent row
    // x-reference on amc.FFbigend.p_field prevents copy
    // func:amc.FFbigend..AssignOp
    inline amc::FFbigend& operator =(const amc::FFbigend &rhs) = delete;
    // x-reference on amc.FFbigend.p_field prevents copy
    // func:amc.FFbigend..CopyCtor
    inline               FFbigend(const amc::FFbigend &rhs) = delete;
private:
    // func:amc.FFbigend..Ctor
    inline               FFbigend() __attribute__((nothrow));
    // func:amc.FFbigend..Dtor
    inline               ~FFbigend() __attribute__((nothrow));
    friend amc::FFbigend&       fbigend_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FFbigend*       fbigend_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 fbigend_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FFbitset - Generate bitset functions over integer field or array
<a href="#amc-ffbitset"></a>

#### amc.FFbitset Fields
<a href="#amc-ffbitset-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FFbitset.msghdr|[dmmeta.Fbitset](/txt/ssimdb/dmmeta/fbitset.md)|[Base](/txt/ssimdb/dmmeta/fbitset.md)|||
|amc.FFbitset.p_field|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||

#### Struct FFbitset
<a href="#struct-ffbitset"></a>
*Note:* field ``amc.FFbitset.msghdr`` has reftype ``base`` so the fields of [dmmeta.Fbitset](/txt/ssimdb/dmmeta/fbitset.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FFbitset { // amc.FFbitset
    amc::FFbitset*      ind_fbitset_next;      // hash next
    u32                 ind_fbitset_hashval;   // hash value
    algo::Smallstr150   field;                 //
    algo::cstring       comment;               //
    amc::FField*        p_field;               // reference to parent row
    // x-reference on amc.FFbitset.p_field prevents copy
    // func:amc.FFbitset..AssignOp
    inline amc::FFbitset& operator =(const amc::FFbitset &rhs) = delete;
    // x-reference on amc.FFbitset.p_field prevents copy
    // func:amc.FFbitset..CopyCtor
    inline               FFbitset(const amc::FFbitset &rhs) = delete;
private:
    // func:amc.FFbitset..Ctor
    inline               FFbitset() __attribute__((nothrow));
    // func:amc.FFbitset..Dtor
    inline               ~FFbitset() __attribute__((nothrow));
    friend amc::FFbitset&       fbitset_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FFbitset*       fbitset_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 fbitset_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FFbuf - Buffer for reading/writing messages, works with Iohook
<a href="#amc-ffbuf"></a>

#### amc.FFbuf Fields
<a href="#amc-ffbuf-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FFbuf.msghdr|[dmmeta.Fbuf](/txt/ssimdb/dmmeta/fbuf.md)|[Base](/txt/ssimdb/dmmeta/fbuf.md)|||
|amc.FFbuf.p_fbuftype|[amc.FFbuftype](/txt/gen/amc/amc.md#amc-ffbuftype)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|amc.FFbuf.p_pair|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||

#### Struct FFbuf
<a href="#struct-ffbuf"></a>
*Note:* field ``amc.FFbuf.msghdr`` has reftype ``base`` so the fields of [dmmeta.Fbuf](/txt/ssimdb/dmmeta/fbuf.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FFbuf { // amc.FFbuf
    amc::FFbuf*         ind_fbuf_next;      // hash next
    u32                 ind_fbuf_hashval;   // hash value
    algo::Smallstr150   field;              //
    u32                 max;                //   0  Size of buffer in bytes
    algo::Smallstr50    fbuftype;           // Type of buffer
    algo::Smallstr50    iotype;             //   "standard"
    algo::Smallstr150   pair;               //   ""  Read buffer this write buffer shares a descriptor with
    algo::cstring       comment;            //
    amc::FFbuftype*     p_fbuftype;         // reference to parent row
    amc::FField*        p_pair;             // reference to parent row
    // x-reference on amc.FFbuf.p_fbuftype prevents copy
    // x-reference on amc.FFbuf.p_pair prevents copy
    // func:amc.FFbuf..AssignOp
    amc::FFbuf&          operator =(const amc::FFbuf &rhs) = delete;
    // x-reference on amc.FFbuf.p_fbuftype prevents copy
    // x-reference on amc.FFbuf.p_pair prevents copy
    // func:amc.FFbuf..CopyCtor
    FFbuf(const amc::FFbuf &rhs) = delete;
private:
    // func:amc.FFbuf..Ctor
    inline               FFbuf() __attribute__((nothrow));
    // func:amc.FFbuf..Dtor
    inline               ~FFbuf() __attribute__((nothrow));
    friend amc::FFbuf&          fbuf_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FFbuf*          fbuf_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 fbuf_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FFbuftype - Buffer type
<a href="#amc-ffbuftype"></a>

#### amc.FFbuftype Fields
<a href="#amc-ffbuftype-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FFbuftype.base|[dmmeta.Fbuftype](/txt/ssimdb/dmmeta/fbuftype.md)|[Base](/txt/ssimdb/dmmeta/fbuftype.md)|||

#### Struct FFbuftype
<a href="#struct-ffbuftype"></a>
*Note:* field ``amc.FFbuftype.base`` has reftype ``base`` so the fields of [dmmeta.Fbuftype](/txt/ssimdb/dmmeta/fbuftype.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FFbuftype { // amc.FFbuftype
    amc::FFbuftype*    ind_fbuftype_next;      // hash next
    u32                ind_fbuftype_hashval;   // hash value
    algo::Smallstr50   fbuftype;               //
    bool               skipbytes;              //   false  Has skipbytes function?
    algo::cstring      comment;                //
    // func:amc.FFbuftype..AssignOp
    inline amc::FFbuftype& operator =(const amc::FFbuftype &rhs) = delete;
    // func:amc.FFbuftype..CopyCtor
    inline               FFbuftype(const amc::FFbuftype &rhs) = delete;
private:
    // func:amc.FFbuftype..Ctor
    inline               FFbuftype() __attribute__((nothrow));
    // func:amc.FFbuftype..Dtor
    inline               ~FFbuftype() __attribute__((nothrow));
    friend amc::FFbuftype&      fbuftype_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FFbuftype*      fbuftype_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 fbuftype_RemoveAll() __attribute__((nothrow));
    friend void                 fbuftype_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FFcast - Generate implicit conversion from field to c++ expression
<a href="#amc-ffcast"></a>

#### amc.FFcast Fields
<a href="#amc-ffcast-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FFcast.msghdr|[dmmeta.Fcast](/txt/ssimdb/dmmeta/fcast.md)|[Base](/txt/ssimdb/dmmeta/fcast.md)|||

#### Struct FFcast
<a href="#struct-ffcast"></a>
*Note:* field ``amc.FFcast.msghdr`` has reftype ``base`` so the fields of [dmmeta.Fcast](/txt/ssimdb/dmmeta/fcast.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FFcast { // amc.FFcast
    algo::Smallstr150   field;     //
    algo::Smallstr100   expr;      //
    algo::cstring       comment;   //
    // func:amc.FFcast..AssignOp
    inline amc::FFcast&  operator =(const amc::FFcast &rhs) = delete;
    // func:amc.FFcast..CopyCtor
    inline               FFcast(const amc::FFcast &rhs) = delete;
private:
    // func:amc.FFcast..Ctor
    inline               FFcast() __attribute__((nothrow));
    // func:amc.FFcast..Dtor
    inline               ~FFcast() __attribute__((nothrow));
    friend amc::FFcast&         fcast_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FFcast*         fcast_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 fcast_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FFcmap - Bidirectional mapping between fconst values
<a href="#amc-ffcmap"></a>

#### amc.FFcmap Fields
<a href="#amc-ffcmap-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FFcmap.base|[dmmeta.Fcmap](/txt/ssimdb/dmmeta/fcmap.md)|[Base](/txt/ssimdb/dmmeta/fcmap.md)|||
|amc.FFcmap.p_leftField|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|amc.FFcmap.p_rightField|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|amc.FFcmap.c_convfunc|[amc.FFunc](/txt/gen/amc/amc.md#amc-ffunc)|[Ptr](/txt/exe/amc/reftype.md#ptr)||Generated conversion func|

#### Struct FFcmap
<a href="#struct-ffcmap"></a>
*Note:* field ``amc.FFcmap.base`` has reftype ``base`` so the fields of [dmmeta.Fcmap](/txt/ssimdb/dmmeta/fcmap.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FFcmap { // amc.FFcmap
    amc::FFcmap*        ind_fcmap_next;        // hash next
    u32                 ind_fcmap_hashval;     // hash value
    algo::Smallstr250   fcmap;                 // cstring maybe?
    bool                bidir;                 //   true
    algo::cstring       comment;               //
    amc::FField*        p_leftField;           // reference to parent row
    amc::FField*        p_rightField;          // reference to parent row
    amc::FFunc*         c_convfunc;            // Generated conversion func. optional pointer
    amc::FFcmap*        field_zs_fcmap_next;   // zslist link; -1 means not-in-list
    // x-reference on amc.FFcmap.p_leftField prevents copy
    // x-reference on amc.FFcmap.p_rightField prevents copy
    // func:amc.FFcmap..AssignOp
    amc::FFcmap&         operator =(const amc::FFcmap &rhs) = delete;
    // x-reference on amc.FFcmap.p_leftField prevents copy
    // x-reference on amc.FFcmap.p_rightField prevents copy
    // func:amc.FFcmap..CopyCtor
    FFcmap(const amc::FFcmap &rhs) = delete;
private:
    // func:amc.FFcmap..Ctor
    inline               FFcmap() __attribute__((nothrow));
    // func:amc.FFcmap..Dtor
    inline               ~FFcmap() __attribute__((nothrow));
    friend amc::FFcmap&         fcmap_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FFcmap*         fcmap_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 fcmap_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FFcmp - Request versionsort or case-insensitive sort for field
<a href="#amc-ffcmp"></a>

#### amc.FFcmp Fields
<a href="#amc-ffcmp-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FFcmp.msghdr|[dmmeta.Fcmp](/txt/ssimdb/dmmeta/fcmp.md)|[Base](/txt/ssimdb/dmmeta/fcmp.md)|||

#### Struct FFcmp
<a href="#struct-ffcmp"></a>
*Note:* field ``amc.FFcmp.msghdr`` has reftype ``base`` so the fields of [dmmeta.Fcmp](/txt/ssimdb/dmmeta/fcmp.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FFcmp { // amc.FFcmp
    algo::Smallstr150   field;         // Target field
    bool                versionsort;   //   false  Use version sort (detect embedded integers in string)
    bool                casesens;      //   true  Comparison is case-sensitive
    bool                extrn;         //   false  Whether implementation is external
    algo::cstring       comment;       //
    // func:amc.FFcmp..AssignOp
    inline amc::FFcmp&   operator =(const amc::FFcmp &rhs) = delete;
    // func:amc.FFcmp..CopyCtor
    inline               FFcmp(const amc::FFcmp &rhs) = delete;
private:
    // func:amc.FFcmp..Ctor
    inline               FFcmp() __attribute__((nothrow));
    // func:amc.FFcmp..Dtor
    inline               ~FFcmp() __attribute__((nothrow));
    friend amc::FFcmp&          fcmp_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FFcmp*          fcmp_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 fcmp_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FFcompact - Request compaction upon removal from index
<a href="#amc-ffcompact"></a>

#### amc.FFcompact Fields
<a href="#amc-ffcompact-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FFcompact.base|[dmmeta.Fcompact](/txt/ssimdb/dmmeta/fcompact.md)|[Base](/txt/ssimdb/dmmeta/fcompact.md)|||

#### Struct FFcompact
<a href="#struct-ffcompact"></a>
*Note:* field ``amc.FFcompact.base`` has reftype ``base`` so the fields of [dmmeta.Fcompact](/txt/ssimdb/dmmeta/fcompact.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FFcompact { // amc.FFcompact
    algo::Smallstr150   field;     //
    algo::cstring       comment;   //
    // func:amc.FFcompact..AssignOp
    inline amc::FFcompact& operator =(const amc::FFcompact &rhs) = delete;
    // func:amc.FFcompact..CopyCtor
    inline               FFcompact(const amc::FFcompact &rhs) = delete;
private:
    // func:amc.FFcompact..Ctor
    inline               FFcompact() __attribute__((nothrow));
    // func:amc.FFcompact..Dtor
    inline               ~FFcompact() __attribute__((nothrow));
    friend amc::FFcompact&      fcompact_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FFcompact*      fcompact_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 fcompact_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FFcond - List membership tied to a condition on a field
<a href="#amc-ffcond"></a>

#### amc.FFcond Fields
<a href="#amc-ffcond-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FFcond.base|[dmmeta.Fcond](/txt/ssimdb/dmmeta/fcond.md)|[Base](/txt/ssimdb/dmmeta/fcond.md)|||
|amc.FFcond.p_ins|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)||The list field this row inserts into / removes from|

#### Struct FFcond
<a href="#struct-ffcond"></a>
*Note:* field ``amc.FFcond.base`` has reftype ``base`` so the fields of [dmmeta.Fcond](/txt/ssimdb/dmmeta/fcond.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FFcond { // amc.FFcond
    algo::Smallstr150   fcond;                  // <field>/<condition>
    algo::Smallstr150   ins;                    // Index the record (or via target) enters when the condition begins to hold
    algo::Smallstr150   via;                    //   ""  Optional Upptr field on the watched ctype; the pointed-to record is inserted instead
    bool                rem;                    //   true  Remove from the index when the condition stops holding
    algo::cstring       comment;                //
    amc::FField*        p_ins;                  // reference to parent row
    bool                field_c_fcond_in_ary;   //   false  membership flag
    // x-reference on amc.FFcond.p_ins prevents copy
    // func:amc.FFcond..AssignOp
    amc::FFcond&         operator =(const amc::FFcond &rhs) = delete;
    // x-reference on amc.FFcond.p_ins prevents copy
    // func:amc.FFcond..CopyCtor
    FFcond(const amc::FFcond &rhs) = delete;
private:
    // func:amc.FFcond..Ctor
    inline               FFcond() __attribute__((nothrow));
    // func:amc.FFcond..Dtor
    inline               ~FFcond() __attribute__((nothrow));
    friend amc::FFcond&         fcond_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FFcond*         fcond_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 fcond_RemoveAll() __attribute__((nothrow));
    friend void                 fcond_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FFconst - Specify enum value (integer + string constant) for a field
<a href="#amc-ffconst"></a>

#### amc.FFconst Fields
<a href="#amc-ffconst-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FFconst.msghdr|[dmmeta.Fconst](/txt/ssimdb/dmmeta/fconst.md)|[Base](/txt/ssimdb/dmmeta/fconst.md)|||
|amc.FFconst.p_field|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|amc.FFconst.cpp_value|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|||
|amc.FFconst.int_val|i64|[Val](/txt/exe/amc/reftype.md#val)||integer value|
|amc.FFconst.cpp_name|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||symbol to use in cpp file|

#### Struct FFconst
<a href="#struct-ffconst"></a>
*Note:* field ``amc.FFconst.msghdr`` has reftype ``base`` so the fields of [dmmeta.Fconst](/txt/ssimdb/dmmeta/fconst.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FFconst { // amc.FFconst
    amc::FFconst*       ind_fconst_next;          // hash next
    u32                 ind_fconst_hashval;       // hash value
    amc::FFconst*       ind_fconst_int_next;      // hash next
    u32                 ind_fconst_int_hashval;   // hash value
    algo::Smallstr150   fconst;                   //
    algo::CppExpr       value;                    //
    algo::cstring       comment;                  //
    amc::FField*        p_field;                  // reference to parent row
    algo::cstring       cpp_value;                //
    i64                 int_val;                  //   0  integer value
    algo::cstring       cpp_name;                 // symbol to use in cpp file
    bool                field_c_fconst_in_ary;    //   false  membership flag
    // x-reference on amc.FFconst.p_field prevents copy
    // func:amc.FFconst..AssignOp
    amc::FFconst&        operator =(const amc::FFconst &rhs) = delete;
    // x-reference on amc.FFconst.p_field prevents copy
    // func:amc.FFconst..CopyCtor
    FFconst(const amc::FFconst &rhs) = delete;
private:
    // func:amc.FFconst..Ctor
    inline               FFconst() __attribute__((nothrow));
    // func:amc.FFconst..Dtor
    inline               ~FFconst() __attribute__((nothrow));
    friend amc::FFconst&        fconst_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FFconst*        fconst_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 fconst_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FFcurs - Request generation of custom cursor
<a href="#amc-ffcurs"></a>

#### amc.FFcurs Fields
<a href="#amc-ffcurs-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FFcurs.base|[dmmeta.Fcurs](/txt/ssimdb/dmmeta/fcurs.md)|[Base](/txt/ssimdb/dmmeta/fcurs.md)|||
|amc.FFcurs.p_field|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||

#### Struct FFcurs
<a href="#struct-ffcurs"></a>
*Note:* field ``amc.FFcurs.base`` has reftype ``base`` so the fields of [dmmeta.Fcurs](/txt/ssimdb/dmmeta/fcurs.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FFcurs { // amc.FFcurs
    amc::FFcurs*       ind_fcurs_next;         // hash next
    u32                ind_fcurs_hashval;      // hash value
    algo::Smallstr50   fcurs;                  //
    algo::cstring      comment;                //
    amc::FField*       p_field;                // reference to parent row
    bool               ctype_c_fcurs_in_ary;   //   false  membership flag
    // x-reference on amc.FFcurs.p_field prevents copy
    // func:amc.FFcurs..AssignOp
    inline amc::FFcurs&  operator =(const amc::FFcurs &rhs) = delete;
    // x-reference on amc.FFcurs.p_field prevents copy
    // func:amc.FFcurs..CopyCtor
    inline               FFcurs(const amc::FFcurs &rhs) = delete;
private:
    // func:amc.FFcurs..Ctor
    inline               FFcurs() __attribute__((nothrow));
    // func:amc.FFcurs..Dtor
    inline               ~FFcurs() __attribute__((nothrow));
    friend amc::FFcurs&         fcurs_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FFcurs*         fcurs_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 fcurs_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FFdec - Specify that field has an implied # of decimal places and specify formatting options
<a href="#amc-ffdec"></a>

#### amc.FFdec Fields
<a href="#amc-ffdec-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FFdec.msghdr|[dmmeta.Fdec](/txt/ssimdb/dmmeta/fdec.md)|[Base](/txt/ssimdb/dmmeta/fdec.md)|||
|amc.FFdec.p_field|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|amc.FFdec.nummin|u64|[Val](/txt/exe/amc/reftype.md#val)|||
|amc.FFdec.nummax|u64|[Val](/txt/exe/amc/reftype.md#val)|||
|amc.FFdec.issigned|bool|[Val](/txt/exe/amc/reftype.md#val)|||

#### Struct FFdec
<a href="#struct-ffdec"></a>
*Note:* field ``amc.FFdec.msghdr`` has reftype ``base`` so the fields of [dmmeta.Fdec](/txt/ssimdb/dmmeta/fdec.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FFdec { // amc.FFdec
    amc::FFdec*         ind_fdec_next;      // hash next
    u32                 ind_fdec_hashval;   // hash value
    algo::Smallstr150   field;              // Target field
    i32                 nplace;             //   0  Number of implied decimal places: 1..17, and 10^nplace must fit the field storage
    bool                fixedfmt;           //   false  Print exactly SCALE chars after decimal point
    algo::cstring       comment;            //
    amc::FField*        p_field;            // reference to parent row
    u64                 nummin;             //   0
    u64                 nummax;             //   0
    bool                issigned;           //   false
    // x-reference on amc.FFdec.p_field prevents copy
    // func:amc.FFdec..AssignOp
    inline amc::FFdec&   operator =(const amc::FFdec &rhs) = delete;
    // x-reference on amc.FFdec.p_field prevents copy
    // func:amc.FFdec..CopyCtor
    inline               FFdec(const amc::FFdec &rhs) = delete;
private:
    // func:amc.FFdec..Ctor
    inline               FFdec() __attribute__((nothrow));
    // func:amc.FFdec..Dtor
    inline               ~FFdec() __attribute__((nothrow));
    friend amc::FFdec&          fdec_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FFdec*          fdec_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 fdec_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FFdelay - Control elapsed time between execution of a step
<a href="#amc-ffdelay"></a>

#### amc.FFdelay Fields
<a href="#amc-ffdelay-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FFdelay.msghdr|[dmmeta.Fdelay](/txt/ssimdb/dmmeta/fdelay.md)|[Base](/txt/ssimdb/dmmeta/fdelay.md)|||

#### Struct FFdelay
<a href="#struct-ffdelay"></a>
*Note:* field ``amc.FFdelay.msghdr`` has reftype ``base`` so the fields of [dmmeta.Fdelay](/txt/ssimdb/dmmeta/fdelay.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FFdelay { // amc.FFdelay
    algo::Smallstr150   fstep;     //
    algo::I64Dec9       delay;     // Delay between steps in seconds
    bool                scale;     //   false  Does delay apply to the whole list?
    algo::cstring       comment;   //
    // func:amc.FFdelay..AssignOp
    inline amc::FFdelay& operator =(const amc::FFdelay &rhs) = delete;
    // func:amc.FFdelay..CopyCtor
    inline               FFdelay(const amc::FFdelay &rhs) = delete;
private:
    // func:amc.FFdelay..Ctor
    inline               FFdelay() __attribute__((nothrow));
    // func:amc.FFdelay..Dtor
    inline               ~FFdelay() __attribute__((nothrow));
    friend amc::FFdelay&        fdelay_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FFdelay*        fdelay_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 fdelay_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FFfast - FAST properties for field
<a href="#amc-fffast"></a>

#### amc.FFfast Fields
<a href="#amc-fffast-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FFfast.base|[dmmeta.Ffast](/txt/ssimdb/dmmeta/ffast.md)|[Base](/txt/ssimdb/dmmeta/ffast.md)|||
|amc.FFfast.p_field|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||

#### Struct FFfast
<a href="#struct-fffast"></a>
*Note:* field ``amc.FFfast.base`` has reftype ``base`` so the fields of [dmmeta.Ffast](/txt/ssimdb/dmmeta/ffast.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FFfast { // amc.FFfast
    algo::Smallstr100   ffast;                  //
    algo::Smallstr50    name;                   //
    u32                 id;                     //   0
    u32                 pos;                    //   0
    bool                optional;               //   false
    algo::Smallstr50    encoding;               //
    algo::Smallstr50    op;                     //
    algo::CppExpr       value;                  //
    algo::cstring       comment;                //
    amc::FField*        p_field;                // reference to parent row
    bool                ctype_c_ffast_in_ary;   //   false  membership flag
    bool                field_c_ffast_in_ary;   //   false  membership flag
    // x-reference on amc.FFfast.p_field prevents copy
    // func:amc.FFfast..AssignOp
    amc::FFfast&         operator =(const amc::FFfast &rhs) = delete;
    // x-reference on amc.FFfast.p_field prevents copy
    // func:amc.FFfast..CopyCtor
    FFfast(const amc::FFfast &rhs) = delete;
private:
    // func:amc.FFfast..Ctor
    inline               FFfast() __attribute__((nothrow));
    // func:amc.FFfast..Dtor
    inline               ~FFfast() __attribute__((nothrow));
    friend amc::FFfast&         ffast_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FFfast*         ffast_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 ffast_RemoveAll() __attribute__((nothrow));
    friend void                 ffast_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FFflag - Options for command-line flags
<a href="#amc-ffflag"></a>

#### amc.FFflag Fields
<a href="#amc-ffflag-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FFflag.base|[dmmeta.Fflag](/txt/ssimdb/dmmeta/fflag.md)|[Base](/txt/ssimdb/dmmeta/fflag.md)|||

#### Struct FFflag
<a href="#struct-ffflag"></a>
*Note:* field ``amc.FFflag.base`` has reftype ``base`` so the fields of [dmmeta.Fflag](/txt/ssimdb/dmmeta/fflag.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FFflag { // amc.FFflag
    amc::FFflag*        ind_fflag_next;      // hash next
    u32                 ind_fflag_hashval;   // hash value
    algo::Smallstr150   field;               //
    bool                cumulative;          //   false  Accumulate on every read
    algo::cstring       emptyval;            //   ""
    algo::cstring       comment;             //
    // func:amc.FFflag..AssignOp
    inline amc::FFflag&  operator =(const amc::FFflag &rhs) = delete;
    // func:amc.FFflag..CopyCtor
    inline               FFflag(const amc::FFflag &rhs) = delete;
private:
    // func:amc.FFflag..Ctor
    inline               FFflag() __attribute__((nothrow));
    // func:amc.FFflag..Dtor
    inline               ~FFflag() __attribute__((nothrow));
    friend amc::FFflag&         fflag_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FFflag*         fflag_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 fflag_RemoveAll() __attribute__((nothrow));
    friend void                 fflag_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FFfunc - User-selected field function; pkey is <field>.<name> where name is a tfunc
<a href="#amc-fffunc"></a>

#### amc.FFfunc Fields
<a href="#amc-fffunc-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FFfunc.base|[dmmeta.Ffunc](/txt/ssimdb/dmmeta/ffunc.md)|[Base](/txt/ssimdb/dmmeta/ffunc.md)|||
|amc.FFfunc.used|bool|[Val](/txt/exe/amc/reftype.md#val)|false|set when amc emits code for this ffunc|

#### Struct FFfunc
<a href="#struct-fffunc"></a>
*Note:* field ``amc.FFfunc.base`` has reftype ``base`` so the fields of [dmmeta.Ffunc](/txt/ssimdb/dmmeta/ffunc.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FFfunc { // amc.FFfunc
    amc::FFfunc*        ind_ffunc_next;        // hash next
    u32                 ind_ffunc_hashval;     // hash value
    algo::Smallstr150   ffunc;                 //
    bool                extrn;                 //   false  Y: user supplies the function body (extern); N: amc generates it
    algo::cstring       comment;               //
    bool                used;                  //   false  set when amc emits code for this ffunc
    amc::FFfunc*        field_zs_ffunc_next;   // zslist link; -1 means not-in-list
    // func:amc.FFfunc..AssignOp
    inline amc::FFfunc&  operator =(const amc::FFfunc &rhs) = delete;
    // func:amc.FFfunc..CopyCtor
    inline               FFfunc(const amc::FFfunc &rhs) = delete;
private:
    // func:amc.FFfunc..Ctor
    inline               FFfunc() __attribute__((nothrow));
    // func:amc.FFfunc..Dtor
    inline               ~FFfunc() __attribute__((nothrow));
    friend amc::FFfunc&         ffunc_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FFfunc*         ffunc_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 ffunc_RemoveAll() __attribute__((nothrow));
    friend void                 ffunc_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FField - Specify field of a struct
<a href="#amc-ffield"></a>

#### amc.FField Fields
<a href="#amc-ffield-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FField.msghdr|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Base](/txt/ssimdb/dmmeta/field.md)|||
|amc.FField.c_fsort|[amc.FFsort](/txt/gen/amc/amc.md#amc-ffsort)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FField.c_fbitset|[amc.FFbitset](/txt/gen/amc/amc.md#amc-ffbitset)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FField.c_smallstr|[amc.FSmallstr](/txt/gen/amc/amc.md#amc-fsmallstr)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FField.c_ffunc|[amc.FFunc](/txt/gen/amc/amc.md#amc-ffunc)|[Ptrary](/txt/exe/amc/reftype.md#ptrary)|||
|amc.FField.cpp_type|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||C++ expression to use in declaration|
|amc.FField.p_ctype|[amc.FCtype](/txt/gen/amc/amc.md#amc-fctype)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|amc.FField.p_arg|[amc.FCtype](/txt/gen/amc/amc.md#amc-fctype)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|amc.FField.c_xref|[amc.FXref](/txt/gen/amc/amc.md#amc-fxref)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FField.c_anonfld|[amc.FAnonfld](/txt/gen/amc/amc.md#amc-fanonfld)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FField.c_fldoffset|[amc.FFldoffset](/txt/gen/amc/amc.md#amc-ffldoffset)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FField.c_basepool|[amc.FBasepool](/txt/gen/amc/amc.md#amc-fbasepool)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FField.c_llist|[amc.FLlist](/txt/gen/amc/amc.md#amc-fllist)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FField.c_thash|[amc.FThash](/txt/gen/amc/amc.md#amc-fthash)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FField.c_inlary|[amc.FInlary](/txt/gen/amc/amc.md#amc-finlary)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FField.c_tary|[amc.FTary](/txt/gen/amc/amc.md#amc-ftary)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FField.c_rowid|[amc.FRowid](/txt/gen/amc/amc.md#amc-frowid)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FField.c_cascdel|[amc.FCascdel](/txt/gen/amc/amc.md#amc-fcascdel)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FField.rowid|double|[Val](/txt/exe/amc/reftype.md#val)|||
|amc.FField.child_rowid|double|[Val](/txt/exe/amc/reftype.md#val)||Rowid increment for next child|
|amc.FField.c_gstatic|[amc.FGstatic](/txt/gen/amc/amc.md#amc-fgstatic)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FField.c_ptrary|[amc.FPtrary](/txt/gen/amc/amc.md#amc-fptrary)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FField.c_pmaskfld|[amc.FPmaskfld](/txt/gen/amc/amc.md#amc-fpmaskfld)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FField.c_typefld|[amc.FTypefld](/txt/gen/amc/amc.md#amc-ftypefld)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FField.c_fdec|[amc.FFdec](/txt/gen/amc/amc.md#amc-ffdec)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FField.p_reftype|[amc.FReftype](/txt/gen/amc/amc.md#amc-freftype)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|amc.FField.c_fconst|[amc.FFconst](/txt/gen/amc/amc.md#amc-ffconst)|[Ptrary](/txt/exe/amc/reftype.md#ptrary)|||
|amc.FField.c_finput|[amc.FFinput](/txt/gen/amc/amc.md#amc-ffinput)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FField.c_foutput|[amc.FFoutput](/txt/gen/amc/amc.md#amc-ffoutput)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FField.c_fbuf|[amc.FFbuf](/txt/gen/amc/amc.md#amc-ffbuf)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FField.c_fbigend|[amc.FFbigend](/txt/gen/amc/amc.md#amc-ffbigend)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FField.c_fstep|[amc.FFstep](/txt/gen/amc/amc.md#amc-ffstep)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FField.c_fregx|[amc.FFregx](/txt/gen/amc/amc.md#amc-ffregx)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FField.c_fcmp|[amc.FFcmp](/txt/gen/amc/amc.md#amc-ffcmp)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FField.c_fcast|[amc.FFcast](/txt/gen/amc/amc.md#amc-ffcast)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FField.c_gconst|[amc.FGconst](/txt/gen/amc/amc.md#amc-fgconst)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FField.c_bitfld|[amc.FBitfld](/txt/gen/amc/amc.md#amc-fbitfld)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FField.c_cppfunc|[amc.FCppfunc](/txt/gen/amc/amc.md#amc-fcppfunc)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FField.c_substr|[amc.FSubstr](/txt/gen/amc/amc.md#amc-fsubstr)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FField.c_fcompact|[amc.FFcompact](/txt/gen/amc/amc.md#amc-ffcompact)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FField.zd_xref_keyfld|[amc.FXref](/txt/gen/amc/amc.md#amc-fxref)|[Llist](/txt/exe/amc/reftype.md#llist)|||
|amc.FField.zs_fcmap|[amc.FFcmap](/txt/gen/amc/amc.md#amc-ffcmap)|[Llist](/txt/exe/amc/reftype.md#llist)|||
|amc.FField.c_inkey_fconst|[amc.FFconst](/txt/gen/amc/amc.md#amc-ffconst)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FField.c_fnoremove|[amc.FFnoremove](/txt/gen/amc/amc.md#amc-ffnoremove)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FField.c_sortfld|[amc.FSortfld](/txt/gen/amc/amc.md#amc-fsortfld)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FField.c_ftrace|[amc.FFtrace](/txt/gen/amc/amc.md#amc-fftrace)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FField.reflect|bool|[Val](/txt/exe/amc/reftype.md#val)||Want reflection|
|amc.FField.ctype_read|bool|[Val](/txt/exe/amc/reftype.md#val)||Field is readable using its ctype's ReadStrptrMaybe function|
|amc.FField.has_substr|bool|[Val](/txt/exe/amc/reftype.md#val)|||
|amc.FField.need_firstchanged|bool|[Val](/txt/exe/amc/reftype.md#val)|||
|amc.FField.do_trace|bool|[Val](/txt/exe/amc/reftype.md#val)||Enable tracing|
|amc.FField.processed|bool|[Val](/txt/exe/amc/reftype.md#val)|||
|amc.FField.synthetic|bool|[Val](/txt/exe/amc/reftype.md#val)|false||
|amc.FField.visit|bool|[Val](/txt/exe/amc/reftype.md#val)|false|Temporary flag|
|amc.FField.c_hook|[amc.FHook](/txt/gen/amc/amc.md#amc-fhook)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FField.c_charset|[amc.FCharset](/txt/gen/amc/amc.md#amc-fcharset)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FField.bh_bitfld|[amc.FBitfld](/txt/gen/amc/amc.md#amc-fbitfld)|[Bheap](/txt/exe/amc/reftype.md#bheap)|||
|amc.FField.c_funique|[amc.FFunique](/txt/gen/amc/amc.md#amc-ffunique)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FField.c_fflag|[amc.FFflag](/txt/gen/amc/amc.md#amc-ffflag)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FField.c_falias|[amc.FFalias](/txt/gen/amc/amc.md#amc-ffalias)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FField.c_ffast_field|[amc.FFfast](/txt/gen/amc/amc.md#amc-fffast)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FField.c_ffast_length|[amc.FFfast](/txt/gen/amc/amc.md#amc-fffast)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FField.c_ffast_mantissa|[amc.FFfast](/txt/gen/amc/amc.md#amc-fffast)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FField.c_ffast|[amc.FFfast](/txt/gen/amc/amc.md#amc-fffast)|[Ptrary](/txt/exe/amc/reftype.md#ptrary)|||
|amc.FField.c_pmaskfld_member|[amc.FPmaskfldMember](/txt/gen/amc/amc.md#amc-fpmaskfldmember)|[Ptrary](/txt/exe/amc/reftype.md#ptrary)|||
|amc.FField.c_lenfld|[amc.FLenfld](/txt/gen/amc/amc.md#amc-flenfld)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FField.c_fkafka|[amc.FFkafka](/txt/gen/amc/amc.md#amc-ffkafka)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FField.c_fpbuf|[amc.FFpbuf](/txt/gen/amc/amc.md#amc-ffpbuf)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FField.c_fpbufcase|[amc.FFpbufcase](/txt/gen/amc/amc.md#amc-ffpbufcase)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FField.zs_ffunc|[amc.FFfunc](/txt/gen/amc/amc.md#amc-fffunc)|[Llist](/txt/exe/amc/reftype.md#llist)||Singly-linked list of ffunc registrations on this field|
|amc.FField.c_blkhash|[amc.FBlkhash](/txt/gen/amc/amc.md#amc-fblkhash)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FField.c_fcond|[amc.FFcond](/txt/gen/amc/amc.md#amc-ffcond)|[Ptrary](/txt/exe/amc/reftype.md#ptrary)||fcond rows watching this field|

#### Struct FField
<a href="#struct-ffield"></a>
*Note:* field ``amc.FField.msghdr`` has reftype ``base`` so the fields of [dmmeta.Field](/txt/ssimdb/dmmeta/field.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FField { // amc.FField
    amc::FField*             ctype_zd_inst_next;        // zslist link; -1 means not-in-list
    amc::FField*             ctype_zd_inst_prev;        // previous element
    amc::FField*             ctype_zd_access_next;      // zslist link; -1 means not-in-list
    amc::FField*             ctype_zd_access_prev;      // previous element
    amc::FField*             ctype_zd_varlenfld_next;   // zslist link; -1 means not-in-list
    amc::FField*             ctype_zd_varlenfld_prev;   // previous element
    amc::FField*             ind_field_next;            // hash next
    u32                      ind_field_hashval;         // hash value
    amc::FField*             zs_ordkeyfield_next;       // zslist link; -1 means not-in-list
    algo::Smallstr150        field;                     // Primary key, as ctype.name
    algo::Smallstr100        arg;                       // Type of field
    algo::Smallstr50         reftype;                   //   "Val"  Type constructor
    algo::CppExpr            dflt;                      // Default value (c++ expression)
    algo::cstring            comment;                   //
    amc::FFsort*             c_fsort;                   // optional pointer
    amc::FFbitset*           c_fbitset;                 // optional pointer
    amc::FSmallstr*          c_smallstr;                // optional pointer
    amc::FFunc**             c_ffunc_elems;             // array of pointers
    u64                      c_ffunc_n;                 // current size
    u64                      c_ffunc_max;               // capacity of allocated array
    algo::cstring            cpp_type;                  // C++ expression to use in declaration
    amc::FCtype*             p_ctype;                   // reference to parent row
    amc::FCtype*             p_arg;                     // reference to parent row
    amc::FXref*              c_xref;                    // optional pointer
    amc::FAnonfld*           c_anonfld;                 // optional pointer
    amc::FFldoffset*         c_fldoffset;               // optional pointer
    amc::FBasepool*          c_basepool;                // optional pointer
    amc::FLlist*             c_llist;                   // optional pointer
    amc::FThash*             c_thash;                   // optional pointer
    amc::FInlary*            c_inlary;                  // optional pointer
    amc::FTary*              c_tary;                    // optional pointer
    amc::FRowid*             c_rowid;                   // optional pointer
    amc::FCascdel*           c_cascdel;                 // optional pointer
    double                   rowid;                     //   0.0
    double                   child_rowid;               //   0.0  Rowid increment for next child
    amc::FGstatic*           c_gstatic;                 // optional pointer
    amc::FPtrary*            c_ptrary;                  // optional pointer
    amc::FPmaskfld*          c_pmaskfld;                // optional pointer
    amc::FTypefld*           c_typefld;                 // optional pointer
    amc::FFdec*              c_fdec;                    // optional pointer
    amc::FReftype*           p_reftype;                 // reference to parent row
    amc::FFconst**           c_fconst_elems;            // array of pointers
    u64                      c_fconst_n;                // current size
    u64                      c_fconst_max;              // capacity of allocated array
    amc::FFinput*            c_finput;                  // optional pointer
    amc::FFoutput*           c_foutput;                 // optional pointer
    amc::FFbuf*              c_fbuf;                    // optional pointer
    amc::FFbigend*           c_fbigend;                 // optional pointer
    amc::FFstep*             c_fstep;                   // optional pointer
    amc::FFregx*             c_fregx;                   // optional pointer
    amc::FFcmp*              c_fcmp;                    // optional pointer
    amc::FFcast*             c_fcast;                   // optional pointer
    amc::FGconst*            c_gconst;                  // optional pointer
    amc::FBitfld*            c_bitfld;                  // optional pointer
    amc::FCppfunc*           c_cppfunc;                 // optional pointer
    amc::FSubstr*            c_substr;                  // optional pointer
    amc::FFcompact*          c_fcompact;                // optional pointer
    amc::FXref*              zd_xref_keyfld_head;       // zero-terminated doubly linked list
    i32                      zd_xref_keyfld_n;          // zero-terminated doubly linked list
    amc::FXref*              zd_xref_keyfld_tail;       // pointer to last element
    amc::FFcmap*             zs_fcmap_head;             // zero-terminated singly linked list
    amc::FFcmap*             zs_fcmap_tail;             // pointer to last element
    amc::FFconst*            c_inkey_fconst;            // optional pointer
    amc::FFnoremove*         c_fnoremove;               // optional pointer
    amc::FSortfld*           c_sortfld;                 // optional pointer
    amc::FFtrace*            c_ftrace;                  // optional pointer
    bool                     reflect;                   //   false  Want reflection
    bool                     ctype_read;                //   false  Field is readable using its ctype's ReadStrptrMaybe function
    bool                     has_substr;                //   false
    bool                     need_firstchanged;         //   false
    bool                     do_trace;                  //   false  Enable tracing
    bool                     processed;                 //   false
    bool                     synthetic;                 //   false
    bool                     visit;                     //   false  Temporary flag
    amc::FHook*              c_hook;                    // optional pointer
    amc::FCharset*           c_charset;                 // optional pointer
    amc::FBitfld**           bh_bitfld_elems;           // binary heap by offset
    i32                      bh_bitfld_n;               // number of elements in the heap
    i32                      bh_bitfld_max;             // max elements in bh_bitfld_elems
    amc::FFunique*           c_funique;                 // optional pointer
    amc::FFflag*             c_fflag;                   // optional pointer
    amc::FFalias*            c_falias;                  // optional pointer
    amc::FFfast*             c_ffast_field;             // optional pointer
    amc::FFfast*             c_ffast_length;            // optional pointer
    amc::FFfast*             c_ffast_mantissa;          // optional pointer
    amc::FFfast**            c_ffast_elems;             // array of pointers
    u64                      c_ffast_n;                 // current size
    u64                      c_ffast_max;               // capacity of allocated array
    amc::FPmaskfldMember**   c_pmaskfld_member_elems;   // array of pointers
    u64                      c_pmaskfld_member_n;       // current size
    u64                      c_pmaskfld_member_max;     // capacity of allocated array
    amc::FLenfld*            c_lenfld;                  // optional pointer
    amc::FFkafka*            c_fkafka;                  // optional pointer
    amc::FFpbuf*             c_fpbuf;                   // optional pointer
    amc::FFpbufcase*         c_fpbufcase;               // optional pointer
    amc::FFfunc*             zs_ffunc_head;             // zero-terminated singly linked list
    amc::FFfunc*             zs_ffunc_tail;             // pointer to last element
    amc::FBlkhash*           c_blkhash;                 // optional pointer
    amc::FFcond**            c_fcond_elems;             // array of pointers
    u64                      c_fcond_n;                 // current size
    u64                      c_fcond_max;               // capacity of allocated array
    bool                     ctype_c_datafld_in_ary;    //   false  membership flag
    // x-reference on amc.FField.c_fsort prevents copy
    // x-reference on amc.FField.c_fbitset prevents copy
    // x-reference on amc.FField.c_smallstr prevents copy
    // reftype Ptrary of amc.FField.c_ffunc prohibits copy
    // x-reference on amc.FField.p_ctype prevents copy
    // x-reference on amc.FField.p_arg prevents copy
    // x-reference on amc.FField.c_xref prevents copy
    // x-reference on amc.FField.c_anonfld prevents copy
    // x-reference on amc.FField.c_fldoffset prevents copy
    // x-reference on amc.FField.c_basepool prevents copy
    // x-reference on amc.FField.c_llist prevents copy
    // x-reference on amc.FField.c_thash prevents copy
    // x-reference on amc.FField.c_inlary prevents copy
    // x-reference on amc.FField.c_tary prevents copy
    // x-reference on amc.FField.c_rowid prevents copy
    // x-reference on amc.FField.c_cascdel prevents copy
    // x-reference on amc.FField.c_gstatic prevents copy
    // x-reference on amc.FField.c_ptrary prevents copy
    // x-reference on amc.FField.c_pmaskfld prevents copy
    // x-reference on amc.FField.c_typefld prevents copy
    // x-reference on amc.FField.c_fdec prevents copy
    // x-reference on amc.FField.p_reftype prevents copy
    // reftype Ptrary of amc.FField.c_fconst prohibits copy
    // x-reference on amc.FField.c_finput prevents copy
    // x-reference on amc.FField.c_foutput prevents copy
    // x-reference on amc.FField.c_fbuf prevents copy
    // x-reference on amc.FField.c_fbigend prevents copy
    // x-reference on amc.FField.c_fstep prevents copy
    // x-reference on amc.FField.c_fregx prevents copy
    // x-reference on amc.FField.c_fcmp prevents copy
    // x-reference on amc.FField.c_fcast prevents copy
    // x-reference on amc.FField.c_gconst prevents copy
    // x-reference on amc.FField.c_bitfld prevents copy
    // x-reference on amc.FField.c_cppfunc prevents copy
    // x-reference on amc.FField.c_substr prevents copy
    // x-reference on amc.FField.c_fcompact prevents copy
    // reftype Llist of amc.FField.zd_xref_keyfld prohibits copy
    // reftype Llist of amc.FField.zs_fcmap prohibits copy
    // x-reference on amc.FField.c_fnoremove prevents copy
    // x-reference on amc.FField.c_sortfld prevents copy
    // x-reference on amc.FField.c_ftrace prevents copy
    // x-reference on amc.FField.c_hook prevents copy
    // x-reference on amc.FField.c_charset prevents copy
    // reftype Bheap of amc.FField.bh_bitfld prohibits copy
    // x-reference on amc.FField.c_funique prevents copy
    // x-reference on amc.FField.c_fflag prevents copy
    // x-reference on amc.FField.c_falias prevents copy
    // x-reference on amc.FField.c_ffast_field prevents copy
    // x-reference on amc.FField.c_ffast_length prevents copy
    // x-reference on amc.FField.c_ffast_mantissa prevents copy
    // reftype Ptrary of amc.FField.c_ffast prohibits copy
    // reftype Ptrary of amc.FField.c_pmaskfld_member prohibits copy
    // x-reference on amc.FField.c_lenfld prevents copy
    // x-reference on amc.FField.c_fkafka prevents copy
    // x-reference on amc.FField.c_fpbuf prevents copy
    // x-reference on amc.FField.c_fpbufcase prevents copy
    // reftype Llist of amc.FField.zs_ffunc prohibits copy
    // x-reference on amc.FField.c_blkhash prevents copy
    // reftype Ptrary of amc.FField.c_fcond prohibits copy
    // func:amc.FField..AssignOp
    amc::FField&         operator =(const amc::FField &rhs) = delete;
    // x-reference on amc.FField.c_fsort prevents copy
    // x-reference on amc.FField.c_fbitset prevents copy
    // x-reference on amc.FField.c_smallstr prevents copy
    // reftype Ptrary of amc.FField.c_ffunc prohibits copy
    // x-reference on amc.FField.p_ctype prevents copy
    // x-reference on amc.FField.p_arg prevents copy
    // x-reference on amc.FField.c_xref prevents copy
    // x-reference on amc.FField.c_anonfld prevents copy
    // x-reference on amc.FField.c_fldoffset prevents copy
    // x-reference on amc.FField.c_basepool prevents copy
    // x-reference on amc.FField.c_llist prevents copy
    // x-reference on amc.FField.c_thash prevents copy
    // x-reference on amc.FField.c_inlary prevents copy
    // x-reference on amc.FField.c_tary prevents copy
    // x-reference on amc.FField.c_rowid prevents copy
    // x-reference on amc.FField.c_cascdel prevents copy
    // x-reference on amc.FField.c_gstatic prevents copy
    // x-reference on amc.FField.c_ptrary prevents copy
    // x-reference on amc.FField.c_pmaskfld prevents copy
    // x-reference on amc.FField.c_typefld prevents copy
    // x-reference on amc.FField.c_fdec prevents copy
    // x-reference on amc.FField.p_reftype prevents copy
    // reftype Ptrary of amc.FField.c_fconst prohibits copy
    // x-reference on amc.FField.c_finput prevents copy
    // x-reference on amc.FField.c_foutput prevents copy
    // x-reference on amc.FField.c_fbuf prevents copy
    // x-reference on amc.FField.c_fbigend prevents copy
    // x-reference on amc.FField.c_fstep prevents copy
    // x-reference on amc.FField.c_fregx prevents copy
    // x-reference on amc.FField.c_fcmp prevents copy
    // x-reference on amc.FField.c_fcast prevents copy
    // x-reference on amc.FField.c_gconst prevents copy
    // x-reference on amc.FField.c_bitfld prevents copy
    // x-reference on amc.FField.c_cppfunc prevents copy
    // x-reference on amc.FField.c_substr prevents copy
    // x-reference on amc.FField.c_fcompact prevents copy
    // reftype Llist of amc.FField.zd_xref_keyfld prohibits copy
    // reftype Llist of amc.FField.zs_fcmap prohibits copy
    // x-reference on amc.FField.c_fnoremove prevents copy
    // x-reference on amc.FField.c_sortfld prevents copy
    // x-reference on amc.FField.c_ftrace prevents copy
    // x-reference on amc.FField.c_hook prevents copy
    // x-reference on amc.FField.c_charset prevents copy
    // reftype Bheap of amc.FField.bh_bitfld prohibits copy
    // x-reference on amc.FField.c_funique prevents copy
    // x-reference on amc.FField.c_fflag prevents copy
    // x-reference on amc.FField.c_falias prevents copy
    // x-reference on amc.FField.c_ffast_field prevents copy
    // x-reference on amc.FField.c_ffast_length prevents copy
    // x-reference on amc.FField.c_ffast_mantissa prevents copy
    // reftype Ptrary of amc.FField.c_ffast prohibits copy
    // reftype Ptrary of amc.FField.c_pmaskfld_member prohibits copy
    // x-reference on amc.FField.c_lenfld prevents copy
    // x-reference on amc.FField.c_fkafka prevents copy
    // x-reference on amc.FField.c_fpbuf prevents copy
    // x-reference on amc.FField.c_fpbufcase prevents copy
    // reftype Llist of amc.FField.zs_ffunc prohibits copy
    // x-reference on amc.FField.c_blkhash prevents copy
    // reftype Ptrary of amc.FField.c_fcond prohibits copy
    // func:amc.FField..CopyCtor
    FField(const amc::FField &rhs) = delete;
private:
    // func:amc.FField..Ctor
    inline               FField() __attribute__((nothrow));
    // func:amc.FField..Dtor
    inline               ~FField() __attribute__((nothrow));
    friend amc::FField&         field_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FField*         field_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 field_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FFinput - Describe input table of a program
<a href="#amc-ffinput"></a>

#### amc.FFinput Fields
<a href="#amc-ffinput-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FFinput.msghdr|[dmmeta.Finput](/txt/ssimdb/dmmeta/finput.md)|[Base](/txt/ssimdb/dmmeta/finput.md)|||
|amc.FFinput.p_field|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||

#### Struct FFinput
<a href="#struct-ffinput"></a>
*Note:* field ``amc.FFinput.msghdr`` has reftype ``base`` so the fields of [dmmeta.Finput](/txt/ssimdb/dmmeta/finput.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FFinput { // amc.FFinput
    algo::Smallstr150   field;     // Target field to read
    bool                update;    //   false
    bool                strict;    //   true  Exist process if record contains error
    algo::cstring       comment;   //
    amc::FField*        p_field;   // reference to parent row
    // x-reference on amc.FFinput.p_field prevents copy
    // func:amc.FFinput..AssignOp
    inline amc::FFinput& operator =(const amc::FFinput &rhs) = delete;
    // x-reference on amc.FFinput.p_field prevents copy
    // func:amc.FFinput..CopyCtor
    inline               FFinput(const amc::FFinput &rhs) = delete;
private:
    // func:amc.FFinput..Ctor
    inline               FFinput() __attribute__((nothrow));
    // func:amc.FFinput..Dtor
    inline               ~FFinput() __attribute__((nothrow));
    friend amc::FFinput&        finput_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FFinput*        finput_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 finput_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FFkafka
<a href="#amc-ffkafka"></a>

#### amc.FFkafka Fields
<a href="#amc-ffkafka-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FFkafka.base|[dmmeta.Fkafka](/txt/ssimdb/dmmeta/fkafka.md)|[Base](/txt/ssimdb/dmmeta/fkafka.md)|||

#### Struct FFkafka
<a href="#struct-ffkafka"></a>
*Note:* field ``amc.FFkafka.base`` has reftype ``base`` so the fields of [dmmeta.Fkafka](/txt/ssimdb/dmmeta/fkafka.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FFkafka { // amc.FFkafka
    algo::Smallstr150   field;               //
    algo::Smallstr10    versions;            //
    algo::Smallstr10    nullable_versions;   //
    algo::Smallstr10    tagged_versions;     //
    u64                 tag;                 //   0
    algo::cstring       comment;             //
    // func:amc.FFkafka..AssignOp
    inline amc::FFkafka& operator =(const amc::FFkafka &rhs) = delete;
    // func:amc.FFkafka..CopyCtor
    inline               FFkafka(const amc::FFkafka &rhs) = delete;
private:
    // func:amc.FFkafka..Ctor
    inline               FFkafka() __attribute__((nothrow));
    // func:amc.FFkafka..Dtor
    inline               ~FFkafka() __attribute__((nothrow));
    friend amc::FFkafka&        fkafka_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FFkafka*        fkafka_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 fkafka_RemoveAll() __attribute__((nothrow));
    friend void                 fkafka_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FFldoffset - Assert field offset - will result in compile-time error if violated
<a href="#amc-ffldoffset"></a>

#### amc.FFldoffset Fields
<a href="#amc-ffldoffset-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FFldoffset.msghdr|[dmmeta.Fldoffset](/txt/ssimdb/dmmeta/fldoffset.md)|[Base](/txt/ssimdb/dmmeta/fldoffset.md)|||
|amc.FFldoffset.p_field|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||

#### Struct FFldoffset
<a href="#struct-ffldoffset"></a>
*Note:* field ``amc.FFldoffset.msghdr`` has reftype ``base`` so the fields of [dmmeta.Fldoffset](/txt/ssimdb/dmmeta/fldoffset.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FFldoffset { // amc.FFldoffset
    algo::Smallstr150   field;     //
    i32                 offset;    //   0
    amc::FField*        p_field;   // reference to parent row
    // x-reference on amc.FFldoffset.p_field prevents copy
    // func:amc.FFldoffset..AssignOp
    inline amc::FFldoffset& operator =(const amc::FFldoffset &rhs) = delete;
    // x-reference on amc.FFldoffset.p_field prevents copy
    // func:amc.FFldoffset..CopyCtor
    inline               FFldoffset(const amc::FFldoffset &rhs) = delete;
private:
    // func:amc.FFldoffset..Ctor
    inline               FFldoffset() __attribute__((nothrow));
    // func:amc.FFldoffset..Dtor
    inline               ~FFldoffset() __attribute__((nothrow));
    friend amc::FFldoffset&     fldoffset_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FFldoffset*     fldoffset_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 fldoffset_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FFloadtuples - Request that process automatically load any input tables on startup
<a href="#amc-ffloadtuples"></a>

#### amc.FFloadtuples Fields
<a href="#amc-ffloadtuples-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FFloadtuples.base|[dmmeta.Floadtuples](/txt/ssimdb/dmmeta/floadtuples.md)|[Base](/txt/ssimdb/dmmeta/floadtuples.md)|||
|amc.FFloadtuples.p_field|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||

#### Struct FFloadtuples
<a href="#struct-ffloadtuples"></a>
*Note:* field ``amc.FFloadtuples.base`` has reftype ``base`` so the fields of [dmmeta.Floadtuples](/txt/ssimdb/dmmeta/floadtuples.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FFloadtuples { // amc.FFloadtuples
    algo::Smallstr150   field;                        //
    bool                autoscan;                     //   true  acr_compl scans this location for completion candidates
    bool                optional;                     //   false  a layer beyond the primary source: absent means skip, not fail
    algo::cstring       comment;                      //
    amc::FField*        p_field;                      // reference to parent row
    bool                ctype_c_floadtuples_in_ary;   //   false  membership flag
    // x-reference on amc.FFloadtuples.p_field prevents copy
    // func:amc.FFloadtuples..AssignOp
    inline amc::FFloadtuples& operator =(const amc::FFloadtuples &rhs) = delete;
    // x-reference on amc.FFloadtuples.p_field prevents copy
    // func:amc.FFloadtuples..CopyCtor
    inline               FFloadtuples(const amc::FFloadtuples &rhs) = delete;
private:
    // func:amc.FFloadtuples..Ctor
    inline               FFloadtuples() __attribute__((nothrow));
    // func:amc.FFloadtuples..Dtor
    inline               ~FFloadtuples() __attribute__((nothrow));
    friend amc::FFloadtuples&   floadtuples_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FFloadtuples*   floadtuples_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 floadtuples_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FFnoremove - Omit any functions for removing elements from table; Table is append-only
<a href="#amc-ffnoremove"></a>

#### amc.FFnoremove Fields
<a href="#amc-ffnoremove-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FFnoremove.base|[dmmeta.Fnoremove](/txt/ssimdb/dmmeta/fnoremove.md)|[Base](/txt/ssimdb/dmmeta/fnoremove.md)|||

#### Struct FFnoremove
<a href="#struct-ffnoremove"></a>
*Note:* field ``amc.FFnoremove.base`` has reftype ``base`` so the fields of [dmmeta.Fnoremove](/txt/ssimdb/dmmeta/fnoremove.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FFnoremove { // amc.FFnoremove
    algo::Smallstr150   field;     //
    algo::cstring       comment;   //
    // func:amc.FFnoremove..AssignOp
    inline amc::FFnoremove& operator =(const amc::FFnoremove &rhs) = delete;
    // func:amc.FFnoremove..CopyCtor
    inline               FFnoremove(const amc::FFnoremove &rhs) = delete;
private:
    // func:amc.FFnoremove..Ctor
    inline               FFnoremove() __attribute__((nothrow));
    // func:amc.FFnoremove..Dtor
    inline               ~FFnoremove() __attribute__((nothrow));
    friend amc::FFnoremove&     fnoremove_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FFnoremove*     fnoremove_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 fnoremove_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FFoutput - Generate function to save index/table back to disk
<a href="#amc-ffoutput"></a>

#### amc.FFoutput Fields
<a href="#amc-ffoutput-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FFoutput.msghdr|[dmmeta.Foutput](/txt/ssimdb/dmmeta/foutput.md)|[Base](/txt/ssimdb/dmmeta/foutput.md)|||

#### Struct FFoutput
<a href="#struct-ffoutput"></a>
*Note:* field ``amc.FFoutput.msghdr`` has reftype ``base`` so the fields of [dmmeta.Foutput](/txt/ssimdb/dmmeta/foutput.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FFoutput { // amc.FFoutput
    algo::Smallstr150   field;                 //
    algo::cstring       comment;               //
    bool                ns_c_foutput_in_ary;   //   false  membership flag
    // func:amc.FFoutput..AssignOp
    inline amc::FFoutput& operator =(const amc::FFoutput &rhs) = delete;
    // func:amc.FFoutput..CopyCtor
    inline               FFoutput(const amc::FFoutput &rhs) = delete;
private:
    // func:amc.FFoutput..Ctor
    inline               FFoutput() __attribute__((nothrow));
    // func:amc.FFoutput..Dtor
    inline               ~FFoutput() __attribute__((nothrow));
    friend amc::FFoutput&       foutput_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FFoutput*       foutput_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 foutput_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FFpbuf
<a href="#amc-ffpbuf"></a>

#### amc.FFpbuf Fields
<a href="#amc-ffpbuf-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FFpbuf.base|[dmmeta.Fpbuf](/txt/ssimdb/dmmeta/fpbuf.md)|[Base](/txt/ssimdb/dmmeta/fpbuf.md)|||

#### Struct FFpbuf
<a href="#struct-ffpbuf"></a>
*Note:* field ``amc.FFpbuf.base`` has reftype ``base`` so the fields of [dmmeta.Fpbuf](/txt/ssimdb/dmmeta/fpbuf.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FFpbuf { // amc.FFpbuf
    algo::Smallstr150   field;          //
    u32                 field_number;   //   0
    algo::Smallstr20    pb_type;        //   ""
    bool                packed;         //   false
    algo::cstring       comment;        //
    // func:amc.FFpbuf..AssignOp
    inline amc::FFpbuf&  operator =(const amc::FFpbuf &rhs) = delete;
    // func:amc.FFpbuf..CopyCtor
    inline               FFpbuf(const amc::FFpbuf &rhs) = delete;
private:
    // func:amc.FFpbuf..Ctor
    inline               FFpbuf() __attribute__((nothrow));
    // func:amc.FFpbuf..Dtor
    inline               ~FFpbuf() __attribute__((nothrow));
    friend amc::FFpbuf&         fpbuf_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FFpbuf*         fpbuf_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 fpbuf_RemoveAll() __attribute__((nothrow));
    friend void                 fpbuf_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FFpbufcase
<a href="#amc-ffpbufcase"></a>

#### amc.FFpbufcase Fields
<a href="#amc-ffpbufcase-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FFpbufcase.base|[dmmeta.Fpbufcase](/txt/ssimdb/dmmeta/fpbufcase.md)|[Base](/txt/ssimdb/dmmeta/fpbufcase.md)|||
|amc.FFpbufcase.p_field|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||

#### Struct FFpbufcase
<a href="#struct-ffpbufcase"></a>
*Note:* field ``amc.FFpbufcase.base`` has reftype ``base`` so the fields of [dmmeta.Fpbufcase](/txt/ssimdb/dmmeta/fpbufcase.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FFpbufcase { // amc.FFpbufcase
    algo::Smallstr150   fpbufcase;                  //
    algo::cstring       comment;                    //
    amc::FField*        p_field;                    // reference to parent row
    bool                ctype_c_fpbufcase_in_ary;   //   false  membership flag
    // x-reference on amc.FFpbufcase.p_field prevents copy
    // func:amc.FFpbufcase..AssignOp
    inline amc::FFpbufcase& operator =(const amc::FFpbufcase &rhs) = delete;
    // x-reference on amc.FFpbufcase.p_field prevents copy
    // func:amc.FFpbufcase..CopyCtor
    inline               FFpbufcase(const amc::FFpbufcase &rhs) = delete;
private:
    // func:amc.FFpbufcase..Ctor
    inline               FFpbufcase() __attribute__((nothrow));
    // func:amc.FFpbufcase..Dtor
    inline               ~FFpbufcase() __attribute__((nothrow));
    friend amc::FFpbufcase&     fpbufcase_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FFpbufcase*     fpbufcase_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 fpbufcase_RemoveAll() __attribute__((nothrow));
    friend void                 fpbufcase_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FFprefix - Allowed pairing of field-name prefix and reftype
<a href="#amc-ffprefix"></a>

#### amc.FFprefix Fields
<a href="#amc-ffprefix-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FFprefix.base|[dmmeta.Fprefix](/txt/ssimdb/dmmeta/fprefix.md)|[Base](/txt/ssimdb/dmmeta/fprefix.md)|||

#### Struct FFprefix
<a href="#struct-ffprefix"></a>
*Note:* field ``amc.FFprefix.base`` has reftype ``base`` so the fields of [dmmeta.Fprefix](/txt/ssimdb/dmmeta/fprefix.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FFprefix { // amc.FFprefix
    algo::Smallstr50   fprefix;                   // prefix.reftype pairing
    bool               require;                   //   true  Reftype must use one of its registered prefixes
    bool               dflt;                      //   true  Reftype acr_ed infers for this prefix; at most one per prefix
    algo::cstring      comment;                   //
    amc::FFprefix*     reftype_zs_fprefix_next;   // zslist link; -1 means not-in-list
    // func:amc.FFprefix..AssignOp
    inline amc::FFprefix& operator =(const amc::FFprefix &rhs) = delete;
    // func:amc.FFprefix..CopyCtor
    inline               FFprefix(const amc::FFprefix &rhs) = delete;
private:
    // func:amc.FFprefix..Ctor
    inline               FFprefix() __attribute__((nothrow));
    // func:amc.FFprefix..Dtor
    inline               ~FFprefix() __attribute__((nothrow));
    friend amc::FFprefix&       fprefix_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FFprefix*       fprefix_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 fprefix_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FFregx - Specify options for command-line regx field
<a href="#amc-ffregx"></a>

#### amc.FFregx Fields
<a href="#amc-ffregx-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FFregx.msghdr|[dmmeta.Fregx](/txt/ssimdb/dmmeta/fregx.md)|[Base](/txt/ssimdb/dmmeta/fregx.md)|||

#### Struct FFregx
<a href="#struct-ffregx"></a>
*Note:* field ``amc.FFregx.msghdr`` has reftype ``base`` so the fields of [dmmeta.Fregx](/txt/ssimdb/dmmeta/fregx.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FFregx { // amc.FFregx
    algo::Smallstr150   field;      //
    bool                partial;    //   false
    algo::Smallstr50    regxtype;   //
    algo::cstring       comment;    //
    // func:amc.FFregx..AssignOp
    inline amc::FFregx&  operator =(const amc::FFregx &rhs) = delete;
    // func:amc.FFregx..CopyCtor
    inline               FFregx(const amc::FFregx &rhs) = delete;
private:
    // func:amc.FFregx..Ctor
    inline               FFregx() __attribute__((nothrow));
    // func:amc.FFregx..Dtor
    inline               ~FFregx() __attribute__((nothrow));
    friend amc::FFregx&         fregx_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FFregx*         fregx_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 fregx_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FFsort - Generate custom sort function for array field
<a href="#amc-ffsort"></a>

#### amc.FFsort Fields
<a href="#amc-ffsort-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FFsort.msghdr|[dmmeta.Fsort](/txt/ssimdb/dmmeta/fsort.md)|[Base](/txt/ssimdb/dmmeta/fsort.md)|||
|amc.FFsort.p_field|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|amc.FFsort.p_sortfld|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||

#### Struct FFsort
<a href="#struct-ffsort"></a>
*Note:* field ``amc.FFsort.msghdr`` has reftype ``base`` so the fields of [dmmeta.Fsort](/txt/ssimdb/dmmeta/fsort.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FFsort { // amc.FFsort
    algo::Smallstr150   field;       // Field (array etc) being sorted
    algo::Smallstr50    sorttype;    // Kind of sort to generate
    algo::Smallstr150   sortfld;     // Field to sort on
    algo::cstring       comment;     //
    amc::FField*        p_field;     // reference to parent row
    amc::FField*        p_sortfld;   // reference to parent row
    // x-reference on amc.FFsort.p_field prevents copy
    // x-reference on amc.FFsort.p_sortfld prevents copy
    // func:amc.FFsort..AssignOp
    inline amc::FFsort&  operator =(const amc::FFsort &rhs) = delete;
    // x-reference on amc.FFsort.p_field prevents copy
    // x-reference on amc.FFsort.p_sortfld prevents copy
    // func:amc.FFsort..CopyCtor
    inline               FFsort(const amc::FFsort &rhs) = delete;
private:
    // func:amc.FFsort..Ctor
    inline               FFsort() __attribute__((nothrow));
    // func:amc.FFsort..Dtor
    inline               ~FFsort() __attribute__((nothrow));
    friend amc::FFsort&         fsort_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FFsort*         fsort_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 fsort_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FFstep - Generate a main loop step to be executed whenever a field is non-empty
<a href="#amc-ffstep"></a>

#### amc.FFstep Fields
<a href="#amc-ffstep-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FFstep.msghdr|[dmmeta.Fstep](/txt/ssimdb/dmmeta/fstep.md)|[Base](/txt/ssimdb/dmmeta/fstep.md)|||
|amc.FFstep.c_fdelay|[amc.FFdelay](/txt/gen/amc/amc.md#amc-ffdelay)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FFstep.p_field|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|amc.FFstep.p_steptype|[amc.FSteptype](/txt/gen/amc/amc.md#amc-fsteptype)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||

#### Struct FFstep
<a href="#struct-ffstep"></a>
*Note:* field ``amc.FFstep.msghdr`` has reftype ``base`` so the fields of [dmmeta.Fstep](/txt/ssimdb/dmmeta/fstep.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FFstep { // amc.FFstep
    amc::FFstep*        ind_fstep_next;      // hash next
    u32                 ind_fstep_hashval;   // hash value
    algo::Smallstr150   fstep;               //
    algo::Smallstr50    steptype;            //
    algo::cstring       comment;             //
    amc::FFdelay*       c_fdelay;            // optional pointer
    amc::FField*        p_field;             // reference to parent row
    amc::FSteptype*     p_steptype;          // reference to parent row
    bool                ns_c_fstep_in_ary;   //   false  membership flag
    // x-reference on amc.FFstep.c_fdelay prevents copy
    // x-reference on amc.FFstep.p_field prevents copy
    // x-reference on amc.FFstep.p_steptype prevents copy
    // func:amc.FFstep..AssignOp
    amc::FFstep&         operator =(const amc::FFstep &rhs) = delete;
    // x-reference on amc.FFstep.c_fdelay prevents copy
    // x-reference on amc.FFstep.p_field prevents copy
    // x-reference on amc.FFstep.p_steptype prevents copy
    // func:amc.FFstep..CopyCtor
    FFstep(const amc::FFstep &rhs) = delete;
private:
    // func:amc.FFstep..Ctor
    inline               FFstep() __attribute__((nothrow));
    // func:amc.FFstep..Dtor
    inline               ~FFstep() __attribute__((nothrow));
    friend amc::FFstep&         fstep_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FFstep*         fstep_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 fstep_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FFtrace - Generate counters for a field: allocations, buffer traffic, or step cycles
<a href="#amc-fftrace"></a>

#### amc.FFtrace Fields
<a href="#amc-fftrace-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FFtrace.base|[dmmeta.Ftrace](/txt/ssimdb/dmmeta/ftrace.md)|[Base](/txt/ssimdb/dmmeta/ftrace.md)|||

#### Struct FFtrace
<a href="#struct-fftrace"></a>
*Note:* field ``amc.FFtrace.base`` has reftype ``base`` so the fields of [dmmeta.Ftrace](/txt/ssimdb/dmmeta/ftrace.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FFtrace { // amc.FFtrace
    algo::Smallstr150   field;     //
    algo::cstring       comment;   //
    // func:amc.FFtrace..AssignOp
    inline amc::FFtrace& operator =(const amc::FFtrace &rhs) = delete;
    // func:amc.FFtrace..CopyCtor
    inline               FFtrace(const amc::FFtrace &rhs) = delete;
private:
    // func:amc.FFtrace..Ctor
    inline               FFtrace() __attribute__((nothrow));
    // func:amc.FFtrace..Dtor
    inline               ~FFtrace() __attribute__((nothrow));
    friend amc::FFtrace&        ftrace_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FFtrace*        ftrace_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 ftrace_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FFunc
<a href="#amc-ffunc"></a>

#### amc.FFunc Fields
<a href="#amc-ffunc-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FFunc.msghdr|[dmmeta.Func](/txt/ssimdb/dmmeta/func.md)|[Base](/txt/ssimdb/dmmeta/func.md)|||
|amc.FFunc.p_ns|[amc.FNs](/txt/gen/amc/amc.md#amc-fns)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|amc.FFunc.body|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||Body of function|
|amc.FFunc.prepcond|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||Preprocessor #if condition|
|amc.FFunc.proto|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||Prototype|
|amc.FFunc.nonnull|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||List of non-null attributes|
|amc.FFunc.initializer|[algo.StringAry](/txt/protocol/algo/README.md#algo-stringary)|[Val](/txt/exe/amc/reftype.md#val)||Initializer list (constructors only)|
|amc.FFunc.ret|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||Return type (c++ expression)|
|amc.FFunc.acrkey|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||Acr key due to which function was created|
|amc.FFunc.funcarg|[amc.Funcarg](/txt/gen/amc/amc.md#amc-funcarg)|[Tary](/txt/exe/amc/reftype.md#tary)||Array of declared function arguments|
|amc.FFunc.printed|bool|[Val](/txt/exe/amc/reftype.md#val)||Already printed? (internal)|
|amc.FFunc.throws|bool|[Val](/txt/exe/amc/reftype.md#val)||Throws exceptions|
|amc.FFunc.nothrow|bool|[Val](/txt/exe/amc/reftype.md#val)||Definitely doesn't throw exception|
|amc.FFunc.member|bool|[Val](/txt/exe/amc/reftype.md#val)|false|Function is a member (print inside struct)|
|amc.FFunc.disable|bool|[Val](/txt/exe/amc/reftype.md#val)|false|Function is enabled (if not, don't print)|
|amc.FFunc.deprecate|bool|[Val](/txt/exe/amc/reftype.md#val)||Make deprecated?|
|amc.FFunc.globns|bool|[Val](/txt/exe/amc/reftype.md#val)||Place function outside of namespace when printing|
|amc.FFunc.ismacro|bool|[Val](/txt/exe/amc/reftype.md#val)||Always inline.|
|amc.FFunc.isalloc|bool|[Val](/txt/exe/amc/reftype.md#val)||Function returns a new value|
|amc.FFunc.isconst|bool|[Val](/txt/exe/amc/reftype.md#val)||Member function is const|
|amc.FFunc.glob|bool|[Val](/txt/exe/amc/reftype.md#val)||Function is not attached to a field (EXPLAIN!!)|
|amc.FFunc.wur|bool|[Val](/txt/exe/amc/reftype.md#val)||Warn on unused result|
|amc.FFunc.priv|bool|[Val](/txt/exe/amc/reftype.md#val)||Private to translateion unit (static)|
|amc.FFunc.pure|bool|[Val](/txt/exe/amc/reftype.md#val)||OK to call fewer times than specified in text|
|amc.FFunc.isprivate|bool|[Val](/txt/exe/amc/reftype.md#val)||Applies to member functions only -- private|
|amc.FFunc.finalized|bool|[Val](/txt/exe/amc/reftype.md#val)||Don't add any more code to me|
|amc.FFunc.isexplicit|bool|[Val](/txt/exe/amc/reftype.md#val)||Is explicit constructor (add 'explicit')|
|amc.FFunc.istmpl|bool|[Val](/txt/exe/amc/reftype.md#val)||Function is a template|
|amc.FFunc.deleted|bool|[Val](/txt/exe/amc/reftype.md#val)||Function is marked as deleted|
|amc.FFunc.inl|bool|[Val](/txt/exe/amc/reftype.md#val)||Make inline?|

#### Struct FFunc
<a href="#struct-ffunc"></a>
*Note:* field ``amc.FFunc.msghdr`` has reftype ``base`` so the fields of [dmmeta.Func](/txt/ssimdb/dmmeta/func.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FFunc { // amc.FFunc
    amc::FFunc*         ind_func_next;       // hash next
    u32                 ind_func_hashval;    // hash value
    amc::FFunc*         cd_temp_func_next;   // zslist link; -1 means not-in-list
    amc::FFunc*         cd_temp_func_prev;   // previous element
    algo::Smallstr100   func;                // Primary key
    bool                extrn;               //   false  Implemented externally by user
    algo::cstring       comment;             // Description
    amc::FNs*           p_ns;                // reference to parent row
    algo::cstring       body;                // Body of function
    algo::cstring       prepcond;            // Preprocessor #if condition
    algo::cstring       proto;               // Prototype
    algo::cstring       nonnull;             // List of non-null attributes
    algo::StringAry     initializer;         // Initializer list (constructors only)
    algo::cstring       ret;                 // Return type (c++ expression)
    algo::cstring       acrkey;              // Acr key due to which function was created
    amc::Funcarg*       funcarg_elems;       // pointer to elements
    u64                 funcarg_n;           // number of elements in array
    u64                 funcarg_max;         // max. capacity of array before realloc
    bool                printed;             //   false  Already printed? (internal)
    bool                throws;              //   false  Throws exceptions
    bool                nothrow;             //   false  Definitely doesn't throw exception
    bool                member;              //   false  Function is a member (print inside struct)
    bool                disable;             //   false  Function is enabled (if not, don't print)
    bool                deprecate;           //   false  Make deprecated?
    bool                globns;              //   false  Place function outside of namespace when printing
    bool                ismacro;             //   false  Always inline.
    bool                isalloc;             //   false  Function returns a new value
    bool                isconst;             //   false  Member function is const
    bool                glob;                //   false  Function is not attached to a field (EXPLAIN!!)
    bool                wur;                 //   false  Warn on unused result
    bool                priv;                //   false  Private to translateion unit (static)
    bool                pure;                //   false  OK to call fewer times than specified in text
    bool                isprivate;           //   false  Applies to member functions only -- private
    bool                finalized;           //   false  Don't add any more code to me
    bool                isexplicit;          //   false  Is explicit constructor (add 'explicit')
    bool                istmpl;              //   false  Function is a template
    bool                deleted;             //   false  Function is marked as deleted
    bool                inl;                 //   false  Make inline?
    bool                ns_c_func_in_ary;    //   false  membership flag
    // x-reference on amc.FFunc.p_ns prevents copy
    // func:amc.FFunc..AssignOp
    amc::FFunc&          operator =(const amc::FFunc &rhs) = delete;
    // x-reference on amc.FFunc.p_ns prevents copy
    // func:amc.FFunc..CopyCtor
    FFunc(const amc::FFunc &rhs) = delete;
private:
    // func:amc.FFunc..Ctor
    inline               FFunc() __attribute__((nothrow));
    // func:amc.FFunc..Dtor
    inline               ~FFunc() __attribute__((nothrow));
    friend amc::FFunc&          func_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FFunc*          func_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 func_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FFunique - This field must be unique in the table. Not needed for primary key
<a href="#amc-ffunique"></a>

#### amc.FFunique Fields
<a href="#amc-ffunique-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FFunique.base|[dmmeta.Funique](/txt/ssimdb/dmmeta/funique.md)|[Base](/txt/ssimdb/dmmeta/funique.md)|||

#### Struct FFunique
<a href="#struct-ffunique"></a>
*Note:* field ``amc.FFunique.base`` has reftype ``base`` so the fields of [dmmeta.Funique](/txt/ssimdb/dmmeta/funique.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FFunique { // amc.FFunique
    algo::Smallstr150   field;     //
    algo::cstring       comment;   //
    // func:amc.FFunique..AssignOp
    inline amc::FFunique& operator =(const amc::FFunique &rhs) = delete;
    // func:amc.FFunique..CopyCtor
    inline               FFunique(const amc::FFunique &rhs) = delete;
private:
    // func:amc.FFunique..Ctor
    inline               FFunique() __attribute__((nothrow));
    // func:amc.FFunique..Dtor
    inline               ~FFunique() __attribute__((nothrow));
    friend amc::FFunique&       funique_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FFunique*       funique_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 funique_RemoveAll() __attribute__((nothrow));
    friend void                 funique_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FFwddecl - Request forward declaration of a field
<a href="#amc-ffwddecl"></a>

#### amc.FFwddecl Fields
<a href="#amc-ffwddecl-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FFwddecl.msghdr|[dmmeta.Fwddecl](/txt/ssimdb/dmmeta/fwddecl.md)|[Base](/txt/ssimdb/dmmeta/fwddecl.md)|||

#### Struct FFwddecl
<a href="#struct-ffwddecl"></a>
*Note:* field ``amc.FFwddecl.msghdr`` has reftype ``base`` so the fields of [dmmeta.Fwddecl](/txt/ssimdb/dmmeta/fwddecl.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FFwddecl { // amc.FFwddecl
    amc::FFwddecl*      ind_fwddecl_next;      // hash next
    u32                 ind_fwddecl_hashval;   // hash value
    algo::Smallstr100   fwddecl;               //
    algo::cstring       comment;               //
    bool                ns_c_fwddecl_in_ary;   //   false  membership flag
    // func:amc.FFwddecl..AssignOp
    inline amc::FFwddecl& operator =(const amc::FFwddecl &rhs) = delete;
    // func:amc.FFwddecl..CopyCtor
    inline               FFwddecl(const amc::FFwddecl &rhs) = delete;
private:
    // func:amc.FFwddecl..Ctor
    inline               FFwddecl() __attribute__((nothrow));
    // func:amc.FFwddecl..Dtor
    inline               ~FFwddecl() __attribute__((nothrow));
    friend amc::FFwddecl&       fwddecl_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FFwddecl*       fwddecl_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 fwddecl_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FGconst - Import ssim table columns as fconst for a field
<a href="#amc-fgconst"></a>

#### amc.FGconst Fields
<a href="#amc-fgconst-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FGconst.msghdr|[dmmeta.Gconst](/txt/ssimdb/dmmeta/gconst.md)|[Base](/txt/ssimdb/dmmeta/gconst.md)|||
|amc.FGconst.p_namefld|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|amc.FGconst.p_field|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||

#### Struct FGconst
<a href="#struct-fgconst"></a>
*Note:* field ``amc.FGconst.msghdr`` has reftype ``base`` so the fields of [dmmeta.Gconst](/txt/ssimdb/dmmeta/gconst.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FGconst { // amc.FGconst
    algo::Smallstr150   field;       // Field with which to associate constants
    algo::Smallstr150   namefld;     // Field which gives the name of the constant
    algo::Smallstr150   idfld;       // Optional attribute of namefld that gives numeric ID for gconst
    bool                wantenum;    //   true
    amc::FField*        p_namefld;   // reference to parent row
    amc::FField*        p_field;     // reference to parent row
    // x-reference on amc.FGconst.p_namefld prevents copy
    // x-reference on amc.FGconst.p_field prevents copy
    // func:amc.FGconst..AssignOp
    inline amc::FGconst& operator =(const amc::FGconst &rhs) = delete;
    // x-reference on amc.FGconst.p_namefld prevents copy
    // x-reference on amc.FGconst.p_field prevents copy
    // func:amc.FGconst..CopyCtor
    inline               FGconst(const amc::FGconst &rhs) = delete;
private:
    // func:amc.FGconst..Ctor
    inline               FGconst() __attribute__((nothrow));
    // func:amc.FGconst..Dtor
    inline               ~FGconst() __attribute__((nothrow));
    friend amc::FGconst&        gconst_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FGconst*        gconst_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 gconst_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FGen
<a href="#amc-fgen"></a>

#### amc.FGen Fields
<a href="#amc-fgen-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FGen.msghdr|[amcdb.Gen](/txt/ssimdb/amcdb/gen.md)|[Base](/txt/ssimdb/amcdb/gen.md)|||
|amc.FGen.cycle_total|u64|[Val](/txt/exe/amc/reftype.md#val)|||
|amc.FGen.step||[Hook](/txt/exe/amc/reftype.md#hook)|||

#### Struct FGen
<a href="#struct-fgen"></a>
*Note:* field ``amc.FGen.msghdr`` has reftype ``base`` so the fields of [amcdb.Gen](/txt/ssimdb/amcdb/gen.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FGen { // amc.FGen
    amc::FGen*           zs_gen_perns_next;   // zslist link; -1 means not-in-list
    algo::Smallstr50     gen;                 //
    bool                 perns;               //   false
    algo::cstring        comment;             //
    u64                  cycle_total;         //   0
    amc::gen_step_hook   step;                //   NULL  Pointer to a function
    // reftype Hook of amc.FGen.step prohibits copy
    // func:amc.FGen..AssignOp
    inline amc::FGen&    operator =(const amc::FGen &rhs) = delete;
    // reftype Hook of amc.FGen.step prohibits copy
    // func:amc.FGen..CopyCtor
    inline               FGen(const amc::FGen &rhs) = delete;
private:
    // func:amc.FGen..Ctor
    inline               FGen() __attribute__((nothrow));
    // func:amc.FGen..Dtor
    inline               ~FGen() __attribute__((nothrow));
    friend amc::FGen&           gen_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FGen*           gen_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 gen_RemoveAll() __attribute__((nothrow));
    friend void                 gen_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FGenXref
<a href="#amc-fgenxref"></a>

#### amc.FGenXref Fields
<a href="#amc-fgenxref-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FGenXref.ind_seen|[amc.FGenXrefSeen](/txt/gen/amc/amc.md#amc-fgenxrefseen)|[Thash](/txt/exe/amc/reftype.md#thash)|||
|amc.FGenXref.seen|[amc.FGenXrefSeen](/txt/gen/amc/amc.md#amc-fgenxrefseen)|[Lary](/txt/exe/amc/reftype.md#lary)|||

#### Struct FGenXref
<a href="#struct-fgenxref"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FGenXref { // amc.FGenXref
    amc::FGenXrefSeen**   ind_seen_buckets_elems;   // pointer to bucket array
    i32                   ind_seen_buckets_n;       // number of elements in bucket array
    i32                   ind_seen_n;               // number of elements in the hash table
    amc::FGenXrefSeen*    seen_lary[36];            // level array
    i64                   seen_n;                   // number of elements in array
    // func:amc.FGenXref..Ctor
    inline               FGenXref() __attribute__((nothrow));
    // func:amc.FGenXref..Dtor
    inline               ~FGenXref() __attribute__((nothrow));
};
```

#### amc.FGenXrefSeen
<a href="#amc-fgenxrefseen"></a>

#### amc.FGenXrefSeen Fields
<a href="#amc-fgenxrefseen-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FGenXrefSeen.value|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|||
|amc.FGenXrefSeen.via|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)||Saved via|
|amc.FGenXrefSeen.xreffld|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)||Saved xreffld|

#### Struct FGenXrefSeen
<a href="#struct-fgenxrefseen"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FGenXrefSeen { // amc.FGenXrefSeen
    amc::FGenXrefSeen*   parent_ind_seen_next;      // hash next
    u32                  parent_ind_seen_hashval;   // hash value
    algo::cstring        value;                     //
    algo::Smallstr150    via;                       // Saved via
    algo::Smallstr150    xreffld;                   // Saved xreffld
private:
    // func:amc.FGenXrefSeen..Ctor
    inline               FGenXrefSeen() __attribute__((nothrow));
    friend amc::FGenXrefSeen&   seen_Alloc(amc::FGenXref& parent) __attribute__((__warn_unused_result__, nothrow));
    friend amc::FGenXrefSeen*   seen_AllocMaybe(amc::FGenXref& parent) __attribute__((__warn_unused_result__, nothrow));
    friend void                 seen_RemoveAll(amc::FGenXref& parent) __attribute__((nothrow));
    friend void                 seen_RemoveLast(amc::FGenXref& parent) __attribute__((nothrow));
};
```

#### amc.FGstatic - Load entries for this table at startup time
<a href="#amc-fgstatic"></a>

#### amc.FGstatic Fields
<a href="#amc-fgstatic-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FGstatic.msghdr|[dmmeta.Gstatic](/txt/ssimdb/dmmeta/gstatic.md)|[Base](/txt/ssimdb/dmmeta/gstatic.md)|||
|amc.FGstatic.p_field|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|amc.FGstatic.rowid|u32|[Val](/txt/exe/amc/reftype.md#val)|||

#### Struct FGstatic
<a href="#struct-fgstatic"></a>
*Note:* field ``amc.FGstatic.msghdr`` has reftype ``base`` so the fields of [dmmeta.Gstatic](/txt/ssimdb/dmmeta/gstatic.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FGstatic { // amc.FGstatic
    algo::Smallstr150   field;                 //
    algo::cstring       comment;               //
    amc::FField*        p_field;               // reference to parent row
    u32                 rowid;                 //   0
    bool                ns_c_gstatic_in_ary;   //   false  membership flag
    // x-reference on amc.FGstatic.p_field prevents copy
    // func:amc.FGstatic..AssignOp
    inline amc::FGstatic& operator =(const amc::FGstatic &rhs) = delete;
    // x-reference on amc.FGstatic.p_field prevents copy
    // func:amc.FGstatic..CopyCtor
    inline               FGstatic(const amc::FGstatic &rhs) = delete;
private:
    // func:amc.FGstatic..Ctor
    inline               FGstatic() __attribute__((nothrow));
    // func:amc.FGstatic..Dtor
    inline               ~FGstatic() __attribute__((nothrow));
    friend amc::FGstatic&       gstatic_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FGstatic*       gstatic_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 gstatic_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FGsymbol - Create C++ symbols from entries in ssim table
<a href="#amc-fgsymbol"></a>

#### amc.FGsymbol Fields
<a href="#amc-fgsymbol-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FGsymbol.base|[dmmeta.Gsymbol](/txt/ssimdb/dmmeta/gsymbol.md)|[Base](/txt/ssimdb/dmmeta/gsymbol.md)|||
|amc.FGsymbol.p_ssimfile|[amc.FSsimfile](/txt/gen/amc/amc.md#amc-fssimfile)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|amc.FGsymbol.p_symboltype|[amc.FCtype](/txt/gen/amc/amc.md#amc-fctype)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||

#### Struct FGsymbol
<a href="#struct-fgsymbol"></a>
*Note:* field ``amc.FGsymbol.base`` has reftype ``base`` so the fields of [dmmeta.Gsymbol](/txt/ssimdb/dmmeta/gsymbol.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FGsymbol { // amc.FGsymbol
    algo::Smallstr50    gsymbol;               //
    algo::Smallstr100   inc;                   // Full regex (sql % wildcard) matched against the row key value
    algo::Smallstr100   symboltype;            //
    algo::cstring       comment;               //
    amc::FSsimfile*     p_ssimfile;            // reference to parent row
    amc::FCtype*        p_symboltype;          // reference to parent row
    bool                ns_c_gsymbol_in_ary;   //   false  membership flag
    // x-reference on amc.FGsymbol.p_ssimfile prevents copy
    // x-reference on amc.FGsymbol.p_symboltype prevents copy
    // func:amc.FGsymbol..AssignOp
    amc::FGsymbol&       operator =(const amc::FGsymbol &rhs) = delete;
    // x-reference on amc.FGsymbol.p_ssimfile prevents copy
    // x-reference on amc.FGsymbol.p_symboltype prevents copy
    // func:amc.FGsymbol..CopyCtor
    FGsymbol(const amc::FGsymbol &rhs) = delete;
private:
    // func:amc.FGsymbol..Ctor
    inline               FGsymbol() __attribute__((nothrow));
    // func:amc.FGsymbol..Dtor
    inline               ~FGsymbol() __attribute__((nothrow));
    friend amc::FGsymbol&       gsymbol_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FGsymbol*       gsymbol_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 gsymbol_RemoveAll() __attribute__((nothrow));
    friend void                 gsymbol_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FHook - Required on Hook fields
<a href="#amc-fhook"></a>

#### amc.FHook Fields
<a href="#amc-fhook-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FHook.base|[dmmeta.Hook](/txt/ssimdb/dmmeta/hook.md)|[Base](/txt/ssimdb/dmmeta/hook.md)|||
|amc.FHook.p_funcptr|[amc.FCtype](/txt/gen/amc/amc.md#amc-fctype)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|amc.FHook.p_field|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||

#### Struct FHook
<a href="#struct-fhook"></a>
*Note:* field ``amc.FHook.base`` has reftype ``base`` so the fields of [dmmeta.Hook](/txt/ssimdb/dmmeta/hook.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FHook { // amc.FHook
    algo::Smallstr150   field;       // The field
    amc::FCtype*        p_funcptr;   // reference to parent row
    amc::FField*        p_field;     // reference to parent row
    // x-reference on amc.FHook.p_field prevents copy
    // func:amc.FHook..AssignOp
    inline amc::FHook&   operator =(const amc::FHook &rhs) = delete;
    // x-reference on amc.FHook.p_field prevents copy
    // func:amc.FHook..CopyCtor
    inline               FHook(const amc::FHook &rhs) = delete;
private:
    // func:amc.FHook..Ctor
    inline               FHook() __attribute__((nothrow));
    // func:amc.FHook..Dtor
    inline               ~FHook() __attribute__((nothrow));
    friend amc::FHook&          hook_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FHook*          hook_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 hook_RemoveAll() __attribute__((nothrow));
    friend void                 hook_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FInlary - Generate inline array of fixed or variable length (all entries fit within parent struct)
<a href="#amc-finlary"></a>

#### amc.FInlary Fields
<a href="#amc-finlary-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FInlary.msghdr|[dmmeta.Inlary](/txt/ssimdb/dmmeta/inlary.md)|[Base](/txt/ssimdb/dmmeta/inlary.md)|||

#### Struct FInlary
<a href="#struct-finlary"></a>
*Note:* field ``amc.FInlary.msghdr`` has reftype ``base`` so the fields of [dmmeta.Inlary](/txt/ssimdb/dmmeta/inlary.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FInlary { // amc.FInlary
    amc::FInlary*       ind_inlary_next;      // hash next
    u32                 ind_inlary_hashval;   // hash value
    algo::Smallstr150   field;                //
    i32                 min;                  //   0  Min. elements to preallocate
    i32                 max;                  //   0  Max. elements
    algo::cstring       comment;              //
    // func:amc.FInlary..AssignOp
    inline amc::FInlary& operator =(const amc::FInlary &rhs) = delete;
    // func:amc.FInlary..CopyCtor
    inline               FInlary(const amc::FInlary &rhs) = delete;
private:
    // func:amc.FInlary..Ctor
    inline               FInlary() __attribute__((nothrow));
    // func:amc.FInlary..Dtor
    inline               ~FInlary() __attribute__((nothrow));
    friend amc::FInlary&        inlary_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FInlary*        inlary_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 inlary_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FJstype
<a href="#amc-fjstype"></a>

#### amc.FJstype Fields
<a href="#amc-fjstype-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FJstype.base|[dmmeta.Jstype](/txt/ssimdb/dmmeta/jstype.md)|[Base](/txt/ssimdb/dmmeta/jstype.md)|||
|amc.FJstype.body|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|||
|amc.FJstype.ctor|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|||
|amc.FJstype.args|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|||
|amc.FJstype.funcs|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|||

#### Struct FJstype
<a href="#struct-fjstype"></a>
*Note:* field ``amc.FJstype.base`` has reftype ``base`` so the fields of [dmmeta.Jstype](/txt/ssimdb/dmmeta/jstype.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FJstype { // amc.FJstype
    algo::Smallstr100   ctype;     //
    algo::cstring       comment;   //
    algo::cstring       body;      //
    algo::cstring       ctor;      //
    algo::cstring       args;      //
    algo::cstring       funcs;     //
    // func:amc.FJstype..AssignOp
    inline amc::FJstype& operator =(const amc::FJstype &rhs) = delete;
    // func:amc.FJstype..CopyCtor
    inline               FJstype(const amc::FJstype &rhs) = delete;
private:
    // func:amc.FJstype..Ctor
    inline               FJstype() __attribute__((nothrow));
    // func:amc.FJstype..Dtor
    inline               ~FJstype() __attribute__((nothrow));
    friend amc::FJstype&        jstype_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FJstype*        jstype_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 jstype_RemoveAll() __attribute__((nothrow));
    friend void                 jstype_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FLenfld - Specify which gives length of varlen portion in bytes
<a href="#amc-flenfld"></a>

#### amc.FLenfld Fields
<a href="#amc-flenfld-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FLenfld.msghdr|[dmmeta.Lenfld](/txt/ssimdb/dmmeta/lenfld.md)|[Base](/txt/ssimdb/dmmeta/lenfld.md)|||
|amc.FLenfld.p_field|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||

#### Struct FLenfld
<a href="#struct-flenfld"></a>
*Note:* field ``amc.FLenfld.msghdr`` has reftype ``base`` so the fields of [dmmeta.Lenfld](/txt/ssimdb/dmmeta/lenfld.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FLenfld { // amc.FLenfld
    algo::Smallstr150   field;     // Name of length field
    i32                 extra;     //   0  Extra #bytes to add
    i32                 scale;     //   1  Multiplier for length field value
    amc::FField*        p_field;   // reference to parent row
    // x-reference on amc.FLenfld.p_field prevents copy
    // func:amc.FLenfld..AssignOp
    inline amc::FLenfld& operator =(const amc::FLenfld &rhs) = delete;
    // x-reference on amc.FLenfld.p_field prevents copy
    // func:amc.FLenfld..CopyCtor
    inline               FLenfld(const amc::FLenfld &rhs) = delete;
private:
    // func:amc.FLenfld..Ctor
    inline               FLenfld() __attribute__((nothrow));
    // func:amc.FLenfld..Dtor
    inline               ~FLenfld() __attribute__((nothrow));
    friend amc::FLenfld&        lenfld_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FLenfld*        lenfld_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 lenfld_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FLicense - Source code license
<a href="#amc-flicense"></a>

#### amc.FLicense Fields
<a href="#amc-flicense-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FLicense.base|[dev.License](/txt/ssimdb/dev/license.md)|[Base](/txt/ssimdb/dev/license.md)|||
|amc.FLicense.text|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|||

#### Struct FLicense
<a href="#struct-flicense"></a>
*Note:* field ``amc.FLicense.base`` has reftype ``base`` so the fields of [dev.License](/txt/ssimdb/dev/license.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FLicense { // amc.FLicense
    amc::FLicense*     ind_license_next;      // hash next
    u32                ind_license_hashval;   // hash value
    algo::Smallstr50   license;               //
    algo::cstring      comment;               //
    algo::cstring      text;                  //
    // func:amc.FLicense..AssignOp
    inline amc::FLicense& operator =(const amc::FLicense &rhs) = delete;
    // func:amc.FLicense..CopyCtor
    inline               FLicense(const amc::FLicense &rhs) = delete;
private:
    // func:amc.FLicense..Ctor
    inline               FLicense() __attribute__((nothrow));
    // func:amc.FLicense..Dtor
    inline               ~FLicense() __attribute__((nothrow));
    friend amc::FLicense&       license_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FLicense*       license_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 license_RemoveAll() __attribute__((nothrow));
    friend void                 license_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FListtype - Specify structure of linked list based on field prefix
<a href="#amc-flisttype"></a>

#### amc.FListtype Fields
<a href="#amc-flisttype-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FListtype.msghdr|[dmmeta.Listtype](/txt/ssimdb/dmmeta/listtype.md)|[Base](/txt/ssimdb/dmmeta/listtype.md)|||

#### Struct FListtype
<a href="#struct-flisttype"></a>
*Note:* field ``amc.FListtype.msghdr`` has reftype ``base`` so the fields of [dmmeta.Listtype](/txt/ssimdb/dmmeta/listtype.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FListtype { // amc.FListtype
    amc::FListtype*   ind_listtype_next;      // hash next
    u32               ind_listtype_hashval;   // hash value
    algo::Smallstr5   listtype;               // Field-name prefix naming this list shape
    bool              circular;               //   false  Circular list
    bool              haveprev;               //   false  Previous link
    bool              instail;                //   false  Queue
    algo::cstring     comment;                //
    // func:amc.FListtype..AssignOp
    inline amc::FListtype& operator =(const amc::FListtype &rhs) = delete;
    // func:amc.FListtype..CopyCtor
    inline               FListtype(const amc::FListtype &rhs) = delete;
private:
    // func:amc.FListtype..Ctor
    inline               FListtype() __attribute__((nothrow));
    // func:amc.FListtype..Dtor
    inline               ~FListtype() __attribute__((nothrow));
    friend amc::FListtype&      listtype_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FListtype*      listtype_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 listtype_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FLlist - Options for Llist field
<a href="#amc-fllist"></a>

#### amc.FLlist Fields
<a href="#amc-fllist-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FLlist.msghdr|[dmmeta.Llist](/txt/ssimdb/dmmeta/llist.md)|[Base](/txt/ssimdb/dmmeta/llist.md)|||
|amc.FLlist.p_listtype|[amc.FListtype](/txt/gen/amc/amc.md#amc-flisttype)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||

#### Struct FLlist
<a href="#struct-fllist"></a>
*Note:* field ``amc.FLlist.msghdr`` has reftype ``base`` so the fields of [dmmeta.Llist](/txt/ssimdb/dmmeta/llist.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FLlist { // amc.FLlist
    algo::Smallstr150   field;        // Index in question
    bool                havetail;     //   true  Generate 'tail' field
    bool                havecount;    //   false  Generate count of elements
    algo::cstring       comment;      //
    amc::FListtype*     p_listtype;   // reference to parent row
    // x-reference on amc.FLlist.p_listtype prevents copy
    // func:amc.FLlist..AssignOp
    inline amc::FLlist&  operator =(const amc::FLlist &rhs) = delete;
    // x-reference on amc.FLlist.p_listtype prevents copy
    // func:amc.FLlist..CopyCtor
    inline               FLlist(const amc::FLlist &rhs) = delete;
private:
    // func:amc.FLlist..Ctor
    inline               FLlist() __attribute__((nothrow));
    // func:amc.FLlist..Dtor
    inline               ~FLlist() __attribute__((nothrow));
    friend amc::FLlist&         llist_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FLlist*         llist_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 llist_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FMain - Generate Main function for namespace
<a href="#amc-fmain"></a>

#### amc.FMain Fields
<a href="#amc-fmain-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FMain.msghdr|[dmmeta.Main](/txt/ssimdb/dmmeta/main.md)|[Base](/txt/ssimdb/dmmeta/main.md)|||

#### Struct FMain
<a href="#struct-fmain"></a>
*Note:* field ``amc.FMain.msghdr`` has reftype ``base`` so the fields of [dmmeta.Main](/txt/ssimdb/dmmeta/main.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FMain { // amc.FMain
    amc::FMain*        ind_main_next;      // hash next
    u32                ind_main_hashval;   // hash value
    algo::Smallstr16   ns;                 //
    bool               ismodule;           //   false
    // func:amc.FMain..AssignOp
    inline amc::FMain&   operator =(const amc::FMain &rhs) = delete;
    // func:amc.FMain..CopyCtor
    inline               FMain(const amc::FMain &rhs) = delete;
private:
    // func:amc.FMain..Ctor
    inline               FMain() __attribute__((nothrow));
    // func:amc.FMain..Dtor
    inline               ~FMain() __attribute__((nothrow));
    friend amc::FMain&          main_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FMain*          main_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 main_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FMsg - Derived message layout row
<a href="#amc-fmsg"></a>

#### amc.FMsg Fields
<a href="#amc-fmsg-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FMsg.base|[dmmeta.Msg](/txt/ssimdb/dmmeta/msg.md)|[Base](/txt/ssimdb/dmmeta/msg.md)|||
|amc.FMsg.typekey|[algo.Smallstr100](/txt/protocol/algo/README.md#algo-smallstr100)|[Val](/txt/exe/amc/reftype.md#val)||Dup-detect key: payloadhdr/type|

#### Struct FMsg
<a href="#struct-fmsg"></a>
*Note:* field ``amc.FMsg.base`` has reftype ``base`` so the fields of [dmmeta.Msg](/txt/ssimdb/dmmeta/msg.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FMsg { // amc.FMsg: Derived message layout row
    amc::FMsg*          ind_msg_typekey_next;      // hash next
    u32                 ind_msg_typekey_hashval;   // hash value
    algo::Smallstr100   ctype;                     //
    algo::Smallstr100   payloadhdr;                // Payload header framing this message
    u64                 type;                      //   0  Msgtype number (evaluated)
    i32                 len;                       //   0  Fixed size of the message in bytes
    algo::cstring       comment;                   //
    algo::Smallstr100   typekey;                   // Dup-detect key: payloadhdr/type
    // func:amc.FMsg..AssignOp
    inline amc::FMsg&    operator =(const amc::FMsg &rhs) = delete;
    // func:amc.FMsg..CopyCtor
    inline               FMsg(const amc::FMsg &rhs) = delete;
private:
    // func:amc.FMsg..Ctor
    inline               FMsg() __attribute__((nothrow));
    // func:amc.FMsg..Dtor
    inline               ~FMsg() __attribute__((nothrow));
    friend amc::FMsg&           msg_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FMsg*           msg_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 msg_RemoveAll() __attribute__((nothrow));
    friend void                 msg_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FMsgfield - Derived leaf field layout row
<a href="#amc-fmsgfield"></a>

#### amc.FMsgfield Fields
<a href="#amc-fmsgfield-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FMsgfield.base|[dmmeta.Msgfield](/txt/ssimdb/dmmeta/msgfield.md)|[Base](/txt/ssimdb/dmmeta/msgfield.md)|||

#### Struct FMsgfield
<a href="#struct-fmsgfield"></a>
*Note:* field ``amc.FMsgfield.base`` has reftype ``base`` so the fields of [dmmeta.Msgfield](/txt/ssimdb/dmmeta/msgfield.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FMsgfield { // amc.FMsgfield: Derived leaf field layout row
    algo::Smallstr100   msgfield;   //
    i32                 offset;     //   0  Absolute byte offset in the message
    algo::Smallstr100   arg;        // Leaf scalar ctype (u8..u64, char, bool, f32, f64)
    i32                 width;      //   0  Byte width; for a char array the array length
    bool                bigend;     //   false  Field is big-endian
    bool                varlen;     //   false  Varlen tail: extends to the end of the message
    algo::cstring       comment;    //
private:
    // func:amc.FMsgfield..Ctor
    inline               FMsgfield() __attribute__((nothrow));
    friend amc::FMsgfield&      msgfield_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FMsgfield*      msgfield_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 msgfield_RemoveAll() __attribute__((nothrow));
    friend void                 msgfield_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FMsgtype - Specify message type for each eligible message, controls dispatch
<a href="#amc-fmsgtype"></a>

#### amc.FMsgtype Fields
<a href="#amc-fmsgtype-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FMsgtype.msghdr|[dmmeta.Msgtype](/txt/ssimdb/dmmeta/msgtype.md)|[Base](/txt/ssimdb/dmmeta/msgtype.md)|||
|amc.FMsgtype.p_ctype|[amc.FCtype](/txt/gen/amc/amc.md#amc-fctype)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||

#### Struct FMsgtype
<a href="#struct-fmsgtype"></a>
*Note:* field ``amc.FMsgtype.msghdr`` has reftype ``base`` so the fields of [dmmeta.Msgtype](/txt/ssimdb/dmmeta/msgtype.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FMsgtype { // amc.FMsgtype
    algo::Smallstr100   ctype;     //
    algo::CppExpr       type;      //
    amc::FCtype*        p_ctype;   // reference to parent row
    // x-reference on amc.FMsgtype.p_ctype prevents copy
    // func:amc.FMsgtype..AssignOp
    inline amc::FMsgtype& operator =(const amc::FMsgtype &rhs) = delete;
    // x-reference on amc.FMsgtype.p_ctype prevents copy
    // func:amc.FMsgtype..CopyCtor
    inline               FMsgtype(const amc::FMsgtype &rhs) = delete;
private:
    // func:amc.FMsgtype..Ctor
    inline               FMsgtype() __attribute__((nothrow));
    // func:amc.FMsgtype..Dtor
    inline               ~FMsgtype() __attribute__((nothrow));
    friend amc::FMsgtype&       msgtype_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FMsgtype*       msgtype_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 msgtype_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FNocascdel - Remove cascade delete for this index
<a href="#amc-fnocascdel"></a>

#### amc.FNocascdel Fields
<a href="#amc-fnocascdel-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FNocascdel.msghdr|[dmmeta.Nocascdel](/txt/ssimdb/dmmeta/nocascdel.md)|[Base](/txt/ssimdb/dmmeta/nocascdel.md)|||

#### Struct FNocascdel
<a href="#struct-fnocascdel"></a>
*Note:* field ``amc.FNocascdel.msghdr`` has reftype ``base`` so the fields of [dmmeta.Nocascdel](/txt/ssimdb/dmmeta/nocascdel.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FNocascdel { // amc.FNocascdel
    algo::Smallstr150   xref;      //
    algo::cstring       comment;   //
    // func:amc.FNocascdel..AssignOp
    inline amc::FNocascdel& operator =(const amc::FNocascdel &rhs) = delete;
    // func:amc.FNocascdel..CopyCtor
    inline               FNocascdel(const amc::FNocascdel &rhs) = delete;
private:
    // func:amc.FNocascdel..Ctor
    inline               FNocascdel() __attribute__((nothrow));
    // func:amc.FNocascdel..Dtor
    inline               ~FNocascdel() __attribute__((nothrow));
    friend amc::FNocascdel&     nocascdel_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FNocascdel*     nocascdel_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 nocascdel_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FNossimfile - Indicates that ssimfile does not exist for this ssimdb ctype
<a href="#amc-fnossimfile"></a>

#### amc.FNossimfile Fields
<a href="#amc-fnossimfile-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FNossimfile.base|[dmmeta.Nossimfile](/txt/ssimdb/dmmeta/nossimfile.md)|[Base](/txt/ssimdb/dmmeta/nossimfile.md)|||

#### Struct FNossimfile
<a href="#struct-fnossimfile"></a>
*Note:* field ``amc.FNossimfile.base`` has reftype ``base`` so the fields of [dmmeta.Nossimfile](/txt/ssimdb/dmmeta/nossimfile.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FNossimfile { // amc.FNossimfile
    algo::Smallstr100   ctype;     //
    algo::cstring       comment;   //
    // func:amc.FNossimfile..AssignOp
    inline amc::FNossimfile& operator =(const amc::FNossimfile &rhs) = delete;
    // func:amc.FNossimfile..CopyCtor
    inline               FNossimfile(const amc::FNossimfile &rhs) = delete;
private:
    // func:amc.FNossimfile..Ctor
    inline               FNossimfile() __attribute__((nothrow));
    // func:amc.FNossimfile..Dtor
    inline               ~FNossimfile() __attribute__((nothrow));
    friend amc::FNossimfile&    nossimfile_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FNossimfile*    nossimfile_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 nossimfile_RemoveAll() __attribute__((nothrow));
    friend void                 nossimfile_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FNs - Namespace (for in-memory database, protocol, etc)
<a href="#amc-fns"></a>

#### amc.FNs Fields
<a href="#amc-fns-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FNs.msghdr|[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)|[Base](/txt/ssimdb/dmmeta/ns.md)|||
|amc.FNs.nhash|i32|[Val](/txt/exe/amc/reftype.md#val)|0|number of hash functions generated (hack)|
|amc.FNs.curstext|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|||
|amc.FNs.c_ctype|[amc.FCtype](/txt/gen/amc/amc.md#amc-fctype)|[Ptrary](/txt/exe/amc/reftype.md#ptrary)|||
|amc.FNs.c_func|[amc.FFunc](/txt/gen/amc/amc.md#amc-ffunc)|[Ptrary](/txt/exe/amc/reftype.md#ptrary)|||
|amc.FNs.c_dispatch|[amc.FDispatch](/txt/gen/amc/amc.md#amc-fdispatch)|[Ptrary](/txt/exe/amc/reftype.md#ptrary)|||
|amc.FNs.c_globfld|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FNs.c_gstatic|[amc.FGstatic](/txt/gen/amc/amc.md#amc-fgstatic)|[Ptrary](/txt/exe/amc/reftype.md#ptrary)|||
|amc.FNs.c_main|[amc.FMain](/txt/gen/amc/amc.md#amc-fmain)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FNs.hdr|[algo.cstring](/txt/protocol/algo/cstring.md)|[Ptr](/txt/exe/amc/reftype.md#ptr)||Temporary hack -- pointer to hdr contents|
|amc.FNs.inl|[algo.cstring](/txt/protocol/algo/cstring.md)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FNs.cpp|[algo.cstring](/txt/protocol/algo/cstring.md)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FNs.include|[algo.cstring](/txt/protocol/algo/cstring.md)|[Tary](/txt/exe/amc/reftype.md#tary)|||
|amc.FNs.signature|[algo.Sha1sig](/txt/protocol/algo/Sha1sig.md)|[Val](/txt/exe/amc/reftype.md#val)|||
|amc.FNs.signature_input|[algo.Sha1sig](/txt/protocol/algo/Sha1sig.md)|[Val](/txt/exe/amc/reftype.md#val)|||
|amc.FNs.c_dispsig|[amc.FDispsig](/txt/gen/amc/amc.md#amc-fdispsig)|[Ptrary](/txt/exe/amc/reftype.md#ptrary)|||
|amc.FNs.c_parentns|[amc.FNs](/txt/gen/amc/amc.md#amc-fns)|[Ptrary](/txt/exe/amc/reftype.md#ptrary)||Namespaces implied by targdep|
|amc.FNs.c_cppincl|[amc.FNs](/txt/gen/amc/amc.md#amc-fns)|[Ptrary](/txt/exe/amc/reftype.md#ptrary)||List of .cpp includes|
|amc.FNs.c_hdrincl|[amc.FNs](/txt/gen/amc/amc.md#amc-fns)|[Ptrary](/txt/exe/amc/reftype.md#ptrary)||List of .h includes|
|amc.FNs.c_fwddecl|[amc.FFwddecl](/txt/gen/amc/amc.md#amc-ffwddecl)|[Ptrary](/txt/exe/amc/reftype.md#ptrary)|||
|amc.FNs.select|bool|[Val](/txt/exe/amc/reftype.md#val)|||
|amc.FNs.c_nsx|[amc.FNsx](/txt/gen/amc/amc.md#amc-fnsx)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FNs.c_target|[amc.FTarget](/txt/gen/amc/amc.md#amc-ftarget)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FNs.c_pnew|[amc.FPnew](/txt/gen/amc/amc.md#amc-fpnew)|[Ptrary](/txt/exe/amc/reftype.md#ptrary)|||
|amc.FNs.c_nsproto|[amc.FNsproto](/txt/gen/amc/amc.md#amc-fnsproto)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FNs.c_nsdb|[amc.FNsdb](/txt/gen/amc/amc.md#amc-fnsdb)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FNs.c_outfile|[amc.FOutfile](/txt/gen/amc/amc.md#amc-foutfile)|[Ptrary](/txt/exe/amc/reftype.md#ptrary)|||
|amc.FNs.c_foutput|[amc.FFoutput](/txt/gen/amc/amc.md#amc-ffoutput)|[Ptrary](/txt/exe/amc/reftype.md#ptrary)|||
|amc.FNs.c_fstep|[amc.FFstep](/txt/gen/amc/amc.md#amc-ffstep)|[Ptrary](/txt/exe/amc/reftype.md#ptrary)|||
|amc.FNs.c_gsymbol|[amc.FGsymbol](/txt/gen/amc/amc.md#amc-fgsymbol)|[Ptrary](/txt/exe/amc/reftype.md#ptrary)|||
|amc.FNs.c_nsinclude|[amc.FNsinclude](/txt/gen/amc/amc.md#amc-fnsinclude)|[Ptrary](/txt/exe/amc/reftype.md#ptrary)|||
|amc.FNs.c_nscpp|[amc.FNscpp](/txt/gen/amc/amc.md#amc-fnscpp)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FNs.p_license|[amc.FLicense](/txt/gen/amc/amc.md#amc-flicense)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|amc.FNs.c_nsjs|[amc.FNsjs](/txt/gen/amc/amc.md#amc-fnsjs)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||

#### Struct FNs
<a href="#struct-fns"></a>
*Note:* field ``amc.FNs.msghdr`` has reftype ``base`` so the fields of [dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FNs { // amc.FNs
    amc::FNs*           ind_ns_next;         // hash next
    u32                 ind_ns_hashval;      // hash value
    algo::Smallstr16    ns;                  // Namespace name (primary key)
    algo::Smallstr50    nstype;              // Namespace type
    algo::Smallstr50    license;             // Associated license
    algo::cstring       comment;             //
    i32                 nhash;               //   0  number of hash functions generated (hack)
    algo::cstring       curstext;            //
    amc::FCtype**       c_ctype_elems;       // array of pointers
    u64                 c_ctype_n;           // current size
    u64                 c_ctype_max;         // capacity of allocated array
    amc::FFunc**        c_func_elems;        // array of pointers
    u64                 c_func_n;            // current size
    u64                 c_func_max;          // capacity of allocated array
    amc::FDispatch**    c_dispatch_elems;    // array of pointers
    u64                 c_dispatch_n;        // current size
    u64                 c_dispatch_max;      // capacity of allocated array
    amc::FField*        c_globfld;           // optional pointer
    amc::FGstatic**     c_gstatic_elems;     // array of pointers
    u64                 c_gstatic_n;         // current size
    u64                 c_gstatic_max;       // capacity of allocated array
    amc::FMain*         c_main;              // optional pointer
    algo::cstring*      hdr;                 // Temporary hack -- pointer to hdr contents. optional pointer
    algo::cstring*      inl;                 // optional pointer
    algo::cstring*      cpp;                 // optional pointer
    algo::cstring*      include_elems;       // pointer to elements
    u64                 include_n;           // number of elements in array
    u64                 include_max;         // max. capacity of array before realloc
    algo::Sha1sig       signature;           //
    algo::Sha1sig       signature_input;     //
    amc::FDispsig**     c_dispsig_elems;     // array of pointers
    u64                 c_dispsig_n;         // current size
    u64                 c_dispsig_max;       // capacity of allocated array
    amc::FNs**          c_parentns_elems;    // array of pointers
    u64                 c_parentns_n;        // current size
    u64                 c_parentns_max;      // capacity of allocated array
    amc::FNs**          c_cppincl_elems;     // array of pointers
    u64                 c_cppincl_n;         // current size
    u64                 c_cppincl_max;       // capacity of allocated array
    amc::FNs**          c_hdrincl_elems;     // array of pointers
    u64                 c_hdrincl_n;         // current size
    u64                 c_hdrincl_max;       // capacity of allocated array
    amc::FFwddecl**     c_fwddecl_elems;     // array of pointers
    u64                 c_fwddecl_n;         // current size
    u64                 c_fwddecl_max;       // capacity of allocated array
    bool                select;              //   false
    amc::FNsx*          c_nsx;               // optional pointer
    amc::FTarget*       c_target;            // optional pointer
    amc::FPnew**        c_pnew_elems;        // array of pointers
    u64                 c_pnew_n;            // current size
    u64                 c_pnew_max;          // capacity of allocated array
    amc::FNsproto*      c_nsproto;           // optional pointer
    amc::FNsdb*         c_nsdb;              // optional pointer
    amc::FOutfile**     c_outfile_elems;     // array of pointers
    u64                 c_outfile_n;         // current size
    u64                 c_outfile_max;       // capacity of allocated array
    amc::FFoutput**     c_foutput_elems;     // array of pointers
    u64                 c_foutput_n;         // current size
    u64                 c_foutput_max;       // capacity of allocated array
    amc::FFstep**       c_fstep_elems;       // array of pointers
    u64                 c_fstep_n;           // current size
    u64                 c_fstep_max;         // capacity of allocated array
    amc::FGsymbol**     c_gsymbol_elems;     // array of pointers
    u64                 c_gsymbol_n;         // current size
    u64                 c_gsymbol_max;       // capacity of allocated array
    amc::FNsinclude**   c_nsinclude_elems;   // array of pointers
    u64                 c_nsinclude_n;       // current size
    u64                 c_nsinclude_max;     // capacity of allocated array
    amc::FNscpp*        c_nscpp;             // optional pointer
    amc::FLicense*      p_license;           // reference to parent row
    amc::FNsjs*         c_nsjs;              // optional pointer
    // reftype Ptrary of amc.FNs.c_ctype prohibits copy
    // reftype Ptrary of amc.FNs.c_func prohibits copy
    // reftype Ptrary of amc.FNs.c_dispatch prohibits copy
    // x-reference on amc.FNs.c_globfld prevents copy
    // reftype Ptrary of amc.FNs.c_gstatic prohibits copy
    // x-reference on amc.FNs.c_main prevents copy
    // reftype Ptrary of amc.FNs.c_dispsig prohibits copy
    // reftype Ptrary of amc.FNs.c_parentns prohibits copy
    // reftype Ptrary of amc.FNs.c_cppincl prohibits copy
    // reftype Ptrary of amc.FNs.c_hdrincl prohibits copy
    // reftype Ptrary of amc.FNs.c_fwddecl prohibits copy
    // x-reference on amc.FNs.c_nsx prevents copy
    // x-reference on amc.FNs.c_target prevents copy
    // reftype Ptrary of amc.FNs.c_pnew prohibits copy
    // x-reference on amc.FNs.c_nsproto prevents copy
    // x-reference on amc.FNs.c_nsdb prevents copy
    // reftype Ptrary of amc.FNs.c_outfile prohibits copy
    // reftype Ptrary of amc.FNs.c_foutput prohibits copy
    // reftype Ptrary of amc.FNs.c_fstep prohibits copy
    // reftype Ptrary of amc.FNs.c_gsymbol prohibits copy
    // reftype Ptrary of amc.FNs.c_nsinclude prohibits copy
    // x-reference on amc.FNs.c_nscpp prevents copy
    // x-reference on amc.FNs.p_license prevents copy
    // x-reference on amc.FNs.c_nsjs prevents copy
    // func:amc.FNs..AssignOp
    amc::FNs&            operator =(const amc::FNs &rhs) = delete;
    // reftype Ptrary of amc.FNs.c_ctype prohibits copy
    // reftype Ptrary of amc.FNs.c_func prohibits copy
    // reftype Ptrary of amc.FNs.c_dispatch prohibits copy
    // x-reference on amc.FNs.c_globfld prevents copy
    // reftype Ptrary of amc.FNs.c_gstatic prohibits copy
    // x-reference on amc.FNs.c_main prevents copy
    // reftype Ptrary of amc.FNs.c_dispsig prohibits copy
    // reftype Ptrary of amc.FNs.c_parentns prohibits copy
    // reftype Ptrary of amc.FNs.c_cppincl prohibits copy
    // reftype Ptrary of amc.FNs.c_hdrincl prohibits copy
    // reftype Ptrary of amc.FNs.c_fwddecl prohibits copy
    // x-reference on amc.FNs.c_nsx prevents copy
    // x-reference on amc.FNs.c_target prevents copy
    // reftype Ptrary of amc.FNs.c_pnew prohibits copy
    // x-reference on amc.FNs.c_nsproto prevents copy
    // x-reference on amc.FNs.c_nsdb prevents copy
    // reftype Ptrary of amc.FNs.c_outfile prohibits copy
    // reftype Ptrary of amc.FNs.c_foutput prohibits copy
    // reftype Ptrary of amc.FNs.c_fstep prohibits copy
    // reftype Ptrary of amc.FNs.c_gsymbol prohibits copy
    // reftype Ptrary of amc.FNs.c_nsinclude prohibits copy
    // x-reference on amc.FNs.c_nscpp prevents copy
    // x-reference on amc.FNs.p_license prevents copy
    // x-reference on amc.FNs.c_nsjs prevents copy
    // func:amc.FNs..CopyCtor
    FNs(const amc::FNs &rhs) = delete;
private:
    // func:amc.FNs..Ctor
    inline               FNs() __attribute__((nothrow));
    // func:amc.FNs..Dtor
    inline               ~FNs() __attribute__((nothrow));
    friend amc::FNs&            ns_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FNs*            ns_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 ns_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FNscpp - Generate C++ code for this namespace
<a href="#amc-fnscpp"></a>

#### amc.FNscpp Fields
<a href="#amc-fnscpp-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FNscpp.base|[dmmeta.Nscpp](/txt/ssimdb/dmmeta/nscpp.md)|[Base](/txt/ssimdb/dmmeta/nscpp.md)|||

#### Struct FNscpp
<a href="#struct-fnscpp"></a>
*Note:* field ``amc.FNscpp.base`` has reftype ``base`` so the fields of [dmmeta.Nscpp](/txt/ssimdb/dmmeta/nscpp.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FNscpp { // amc.FNscpp
    algo::Smallstr16   ns;        //
    algo::cstring      comment;   //
    // func:amc.FNscpp..AssignOp
    inline amc::FNscpp&  operator =(const amc::FNscpp &rhs) = delete;
    // func:amc.FNscpp..CopyCtor
    inline               FNscpp(const amc::FNscpp &rhs) = delete;
private:
    // func:amc.FNscpp..Ctor
    inline               FNscpp() __attribute__((nothrow));
    // func:amc.FNscpp..Dtor
    inline               ~FNscpp() __attribute__((nothrow));
    friend amc::FNscpp&         nscpp_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FNscpp*         nscpp_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 nscpp_RemoveAll() __attribute__((nothrow));
    friend void                 nscpp_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FNsdb - Annotate ssimdb namespaces
<a href="#amc-fnsdb"></a>

#### amc.FNsdb Fields
<a href="#amc-fnsdb-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FNsdb.base|[dmmeta.Nsdb](/txt/ssimdb/dmmeta/nsdb.md)|[Base](/txt/ssimdb/dmmeta/nsdb.md)|||

#### Struct FNsdb
<a href="#struct-fnsdb"></a>
*Note:* field ``amc.FNsdb.base`` has reftype ``base`` so the fields of [dmmeta.Nsdb](/txt/ssimdb/dmmeta/nsdb.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FNsdb { // amc.FNsdb
    algo::Smallstr16   ns;        //
    algo::cstring      comment;   //
    // func:amc.FNsdb..AssignOp
    inline amc::FNsdb&   operator =(const amc::FNsdb &rhs) = delete;
    // func:amc.FNsdb..CopyCtor
    inline               FNsdb(const amc::FNsdb &rhs) = delete;
private:
    // func:amc.FNsdb..Ctor
    inline               FNsdb() __attribute__((nothrow));
    // func:amc.FNsdb..Dtor
    inline               ~FNsdb() __attribute__((nothrow));
    friend amc::FNsdb&          nsdb_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FNsdb*          nsdb_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 nsdb_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FNsinclude - Explicitly specify a C++ include file for namespace
<a href="#amc-fnsinclude"></a>

#### amc.FNsinclude Fields
<a href="#amc-fnsinclude-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FNsinclude.base|[dmmeta.Nsinclude](/txt/ssimdb/dmmeta/nsinclude.md)|[Base](/txt/ssimdb/dmmeta/nsinclude.md)|||

#### Struct FNsinclude
<a href="#struct-fnsinclude"></a>
*Note:* field ``amc.FNsinclude.base`` has reftype ``base`` so the fields of [dmmeta.Nsinclude](/txt/ssimdb/dmmeta/nsinclude.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FNsinclude { // amc.FNsinclude
    algo::Smallstr50   nsinclude;               //
    bool               sys;                     //   false
    algo::cstring      comment;                 //
    bool               ns_c_nsinclude_in_ary;   //   false  membership flag
    // func:amc.FNsinclude..AssignOp
    inline amc::FNsinclude& operator =(const amc::FNsinclude &rhs) = delete;
    // func:amc.FNsinclude..CopyCtor
    inline               FNsinclude(const amc::FNsinclude &rhs) = delete;
private:
    // func:amc.FNsinclude..Ctor
    inline               FNsinclude() __attribute__((nothrow));
    // func:amc.FNsinclude..Dtor
    inline               ~FNsinclude() __attribute__((nothrow));
    friend amc::FNsinclude&     nsinclude_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FNsinclude*     nsinclude_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 nsinclude_RemoveAll() __attribute__((nothrow));
    friend void                 nsinclude_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FNsjs - Generate JavaScript code for this namespace
<a href="#amc-fnsjs"></a>

#### amc.FNsjs Fields
<a href="#amc-fnsjs-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FNsjs.base|[dmmeta.Nsjs](/txt/ssimdb/dmmeta/nsjs.md)|[Base](/txt/ssimdb/dmmeta/nsjs.md)|||
|amc.FNsjs.imports|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|||
|amc.FNsjs.filebody|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|||

#### Struct FNsjs
<a href="#struct-fnsjs"></a>
*Note:* field ``amc.FNsjs.base`` has reftype ``base`` so the fields of [dmmeta.Nsjs](/txt/ssimdb/dmmeta/nsjs.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FNsjs { // amc.FNsjs
    algo::Smallstr16   ns;           //
    bool               typescript;   //   false  Generate typescript
    bool               ifaceonly;    //   false  Project ctypes as interfaces named after the ctype; emit no runtime class
    algo::cstring      comment;      //
    algo::cstring      imports;      //
    algo::cstring      filebody;     //
    // func:amc.FNsjs..AssignOp
    inline amc::FNsjs&   operator =(const amc::FNsjs &rhs) = delete;
    // func:amc.FNsjs..CopyCtor
    inline               FNsjs(const amc::FNsjs &rhs) = delete;
private:
    // func:amc.FNsjs..Ctor
    inline               FNsjs() __attribute__((nothrow));
    // func:amc.FNsjs..Dtor
    inline               ~FNsjs() __attribute__((nothrow));
    friend amc::FNsjs&          nsjs_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FNsjs*          nsjs_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 nsjs_RemoveAll() __attribute__((nothrow));
    friend void                 nsjs_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FNsproto - Annotate protocol namespace (collection of types, no state)
<a href="#amc-fnsproto"></a>

#### amc.FNsproto Fields
<a href="#amc-fnsproto-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FNsproto.base|[dmmeta.Nsproto](/txt/ssimdb/dmmeta/nsproto.md)|[Base](/txt/ssimdb/dmmeta/nsproto.md)|||

#### Struct FNsproto
<a href="#struct-fnsproto"></a>
*Note:* field ``amc.FNsproto.base`` has reftype ``base`` so the fields of [dmmeta.Nsproto](/txt/ssimdb/dmmeta/nsproto.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FNsproto { // amc.FNsproto
    algo::Smallstr16   ns;        //
    algo::cstring      comment;   //
    // func:amc.FNsproto..AssignOp
    inline amc::FNsproto& operator =(const amc::FNsproto &rhs) = delete;
    // func:amc.FNsproto..CopyCtor
    inline               FNsproto(const amc::FNsproto &rhs) = delete;
private:
    // func:amc.FNsproto..Ctor
    inline               FNsproto() __attribute__((nothrow));
    // func:amc.FNsproto..Dtor
    inline               ~FNsproto() __attribute__((nothrow));
    friend amc::FNsproto&       nsproto_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FNsproto*       nsproto_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 nsproto_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FNsx - Control code-generation and exception handling options for process/library
<a href="#amc-fnsx"></a>

#### amc.FNsx Fields
<a href="#amc-fnsx-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FNsx.base|[dmmeta.Nsx](/txt/ssimdb/dmmeta/nsx.md)|[Base](/txt/ssimdb/dmmeta/nsx.md)|||
|amc.FNsx.p_pool|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||

#### Struct FNsx
<a href="#struct-fnsx"></a>
*Note:* field ``amc.FNsx.base`` has reftype ``base`` so the fields of [dmmeta.Nsx](/txt/ssimdb/dmmeta/nsx.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FNsx { // amc.FNsx
    algo::Smallstr16    ns;                    //
    bool                genthrow;              //   false
    bool                correct_getorcreate;   //   false
    algo::Smallstr150   pool;                  //
    bool                sortxref;              //   false
    bool                pack;                  //   false
    algo::cstring       comment;               //
    amc::FField*        p_pool;                // reference to parent row
    // x-reference on amc.FNsx.p_pool prevents copy
    // func:amc.FNsx..AssignOp
    inline amc::FNsx&    operator =(const amc::FNsx &rhs) = delete;
    // x-reference on amc.FNsx.p_pool prevents copy
    // func:amc.FNsx..CopyCtor
    inline               FNsx(const amc::FNsx &rhs) = delete;
private:
    // func:amc.FNsx..Ctor
    inline               FNsx() __attribute__((nothrow));
    // func:amc.FNsx..Dtor
    inline               ~FNsx() __attribute__((nothrow));
    friend amc::FNsx&           nsx_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FNsx*           nsx_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 nsx_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FNumstr - Add functions to read numbers out of a string field
<a href="#amc-fnumstr"></a>

#### amc.FNumstr Fields
<a href="#amc-fnumstr-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FNumstr.msghdr|[dmmeta.Numstr](/txt/ssimdb/dmmeta/numstr.md)|[Base](/txt/ssimdb/dmmeta/numstr.md)|||
|amc.FNumstr.p_numtype|[amc.FCtype](/txt/gen/amc/amc.md#amc-fctype)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|amc.FNumstr.nummin|u64|[Val](/txt/exe/amc/reftype.md#val)|||
|amc.FNumstr.nummax|u64|[Val](/txt/exe/amc/reftype.md#val)|||
|amc.FNumstr.issigned|bool|[Val](/txt/exe/amc/reftype.md#val)|||

#### Struct FNumstr
<a href="#struct-fnumstr"></a>
*Note:* field ``amc.FNumstr.msghdr`` has reftype ``base`` so the fields of [dmmeta.Numstr](/txt/ssimdb/dmmeta/numstr.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FNumstr { // amc.FNumstr
    algo::Smallstr150   field;       // String type
    algo::Smallstr100   numtype;     // Corresponding numeric type
    i32                 base;        //   0  Base for conversion
    i32                 min_len;     //   0  Mininum length of string
    amc::FCtype*        p_numtype;   // reference to parent row
    u64                 nummin;      //   0
    u64                 nummax;      //   0
    bool                issigned;    //   false
    // x-reference on amc.FNumstr.p_numtype prevents copy
    // func:amc.FNumstr..AssignOp
    inline amc::FNumstr& operator =(const amc::FNumstr &rhs) = delete;
    // x-reference on amc.FNumstr.p_numtype prevents copy
    // func:amc.FNumstr..CopyCtor
    inline               FNumstr(const amc::FNumstr &rhs) = delete;
private:
    // func:amc.FNumstr..Ctor
    inline               FNumstr() __attribute__((nothrow));
    // func:amc.FNumstr..Dtor
    inline               ~FNumstr() __attribute__((nothrow));
    friend amc::FNumstr&        numstr_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FNumstr*        numstr_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 numstr_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FOutfile - AMC's output file (generated file under include/gen or cpp/gen)
<a href="#amc-foutfile"></a>

#### amc.FOutfile Fields
<a href="#amc-foutfile-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|*amc.FOutfile.ns (/RR_RL)*|[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)|||
|amc.FOutfile.text|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|||
|amc.FOutfile.p_ns|[amc.FNs](/txt/gen/amc/amc.md#amc-fns)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|amc.FOutfile.ssim|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||Ssim content|
|amc.FOutfile.prev_head|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||Helper|
|amc.FOutfile.outfile|[dev.Gitfile](/txt/ssimdb/dev/gitfile.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)|||

#### Struct FOutfile
<a href="#struct-foutfile"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FOutfile { // amc.FOutfile: AMC's output file (generated file under include/gen or cpp/gen)
    amc::FOutfile*      outfile_next;          // Pointer to next free element int tpool
    algo::cstring       text;                  //
    amc::FNs*           p_ns;                  // reference to parent row
    algo::cstring       ssim;                  // Ssim content
    algo::cstring       prev_head;             // Helper
    algo::Smallstr200   outfile;               //
    bool                ns_c_outfile_in_ary;   //   false  membership flag
    // x-reference on amc.FOutfile.p_ns prevents copy
    // func:amc.FOutfile..AssignOp
    inline amc::FOutfile& operator =(const amc::FOutfile &rhs) = delete;
    // x-reference on amc.FOutfile.p_ns prevents copy
    // func:amc.FOutfile..CopyCtor
    inline               FOutfile(const amc::FOutfile &rhs) = delete;
private:
    // func:amc.FOutfile..Ctor
    inline               FOutfile() __attribute__((nothrow));
    // func:amc.FOutfile..Dtor
    inline               ~FOutfile() __attribute__((nothrow));
    friend amc::FOutfile&       outfile_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FOutfile*       outfile_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 outfile_Delete(amc::FOutfile &row) __attribute__((nothrow));
};
```

#### amc.FPack - Request byte-packing of structure fields
<a href="#amc-fpack"></a>

#### amc.FPack Fields
<a href="#amc-fpack-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FPack.msghdr|[dmmeta.Pack](/txt/ssimdb/dmmeta/pack.md)|[Base](/txt/ssimdb/dmmeta/pack.md)|||
|amc.FPack.p_ctype|[amc.FCtype](/txt/gen/amc/amc.md#amc-fctype)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||

#### Struct FPack
<a href="#struct-fpack"></a>
*Note:* field ``amc.FPack.msghdr`` has reftype ``base`` so the fields of [dmmeta.Pack](/txt/ssimdb/dmmeta/pack.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FPack { // amc.FPack
    amc::FPack*         ind_pack_next;      // hash next
    u32                 ind_pack_hashval;   // hash value
    algo::Smallstr100   ctype;              // Target ctype
    algo::cstring       comment;            //
    amc::FCtype*        p_ctype;            // reference to parent row
    // x-reference on amc.FPack.p_ctype prevents copy
    // func:amc.FPack..AssignOp
    inline amc::FPack&   operator =(const amc::FPack &rhs) = delete;
    // x-reference on amc.FPack.p_ctype prevents copy
    // func:amc.FPack..CopyCtor
    inline               FPack(const amc::FPack &rhs) = delete;
private:
    // func:amc.FPack..Ctor
    inline               FPack() __attribute__((nothrow));
    // func:amc.FPack..Dtor
    inline               ~FPack() __attribute__((nothrow));
    friend amc::FPack&          pack_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FPack*          pack_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 pack_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FPayloadhdr - Derived payload header row
<a href="#amc-fpayloadhdr"></a>

#### amc.FPayloadhdr Fields
<a href="#amc-fpayloadhdr-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FPayloadhdr.base|[dmmeta.Payloadhdr](/txt/ssimdb/dmmeta/payloadhdr.md)|[Base](/txt/ssimdb/dmmeta/payloadhdr.md)|||

#### Struct FPayloadhdr
<a href="#struct-fpayloadhdr"></a>
*Note:* field ``amc.FPayloadhdr.base`` has reftype ``base`` so the fields of [dmmeta.Payloadhdr](/txt/ssimdb/dmmeta/payloadhdr.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FPayloadhdr { // amc.FPayloadhdr: Derived payload header row
    algo::Smallstr100   ctype;        //
    i32                 typeoffset;   //   0  Byte offset of the type field
    i32                 typewidth;    //   0  Byte width of the type field
    bool                typebig;      //   false  Type field is big-endian
    i32                 lenoffset;    //   0  Byte offset of the length field
    i32                 lenwidth;     //   0  Byte width of the length field
    bool                lenbig;       //   false  Length field is big-endian
    i32                 lenscale;     //   1  Multiplier for length field value
    i32                 lenextra;     //   0  Extra bytes to add to scaled length
    i32                 len;          //   0  Fixed size of the header in bytes
    algo::cstring       comment;      //
    // func:amc.FPayloadhdr..AssignOp
    amc::FPayloadhdr&    operator =(const amc::FPayloadhdr &rhs) = delete;
    // func:amc.FPayloadhdr..CopyCtor
    FPayloadhdr(const amc::FPayloadhdr &rhs) = delete;
private:
    // func:amc.FPayloadhdr..Ctor
    inline               FPayloadhdr() __attribute__((nothrow));
    // func:amc.FPayloadhdr..Dtor
    inline               ~FPayloadhdr() __attribute__((nothrow));
    friend amc::FPayloadhdr&    payloadhdr_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FPayloadhdr*    payloadhdr_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 payloadhdr_RemoveAll() __attribute__((nothrow));
    friend void                 payloadhdr_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FPbsyntax - Protobuf syntax variant selecting default-value and presence semantics
<a href="#amc-fpbsyntax"></a>

#### amc.FPbsyntax Fields
<a href="#amc-fpbsyntax-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FPbsyntax.base|[dmmeta.Pbsyntax](/txt/ssimdb/dmmeta/pbsyntax.md)|[Base](/txt/ssimdb/dmmeta/pbsyntax.md)|||

#### Struct FPbsyntax
<a href="#struct-fpbsyntax"></a>
*Note:* field ``amc.FPbsyntax.base`` has reftype ``base`` so the fields of [dmmeta.Pbsyntax](/txt/ssimdb/dmmeta/pbsyntax.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FPbsyntax { // amc.FPbsyntax
    amc::FPbsyntax*    ind_pbsyntax_next;      // hash next
    u32                ind_pbsyntax_hashval;   // hash value
    algo::Smallstr50   pbsyntax;               //
    bool               implicit_presence;      //   false  A scalar equal to its zero value is not written to the wire
    algo::cstring      comment;                //
    // func:amc.FPbsyntax..AssignOp
    inline amc::FPbsyntax& operator =(const amc::FPbsyntax &rhs) = delete;
    // func:amc.FPbsyntax..CopyCtor
    inline               FPbsyntax(const amc::FPbsyntax &rhs) = delete;
private:
    // func:amc.FPbsyntax..Ctor
    inline               FPbsyntax() __attribute__((nothrow));
    // func:amc.FPbsyntax..Dtor
    inline               ~FPbsyntax() __attribute__((nothrow));
    friend amc::FPbsyntax&      pbsyntax_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FPbsyntax*      pbsyntax_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 pbsyntax_RemoveAll() __attribute__((nothrow));
    friend void                 pbsyntax_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FPbtype - Protobuf scalar type: wire type and lib_pb codec binding for the pbuf codec
<a href="#amc-fpbtype"></a>

#### amc.FPbtype Fields
<a href="#amc-fpbtype-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FPbtype.base|[amcdb.Pbtype](/txt/ssimdb/amcdb/pbtype.md)|[Base](/txt/ssimdb/amcdb/pbtype.md)|||

#### Struct FPbtype
<a href="#struct-fpbtype"></a>
*Note:* field ``amc.FPbtype.base`` has reftype ``base`` so the fields of [amcdb.Pbtype](/txt/ssimdb/amcdb/pbtype.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FPbtype { // amc.FPbtype
    amc::FPbtype*      ind_pbtype_next;      // hash next
    u32                ind_pbtype_hashval;   // hash value
    algo::Smallstr20   pbtype;               // Protobuf type name as written in dmmeta.fpbuf.pb_type
    u8                 wire_type;            //   0  Protobuf wire type: 0=varint 1=64-bit 2=length-delimited 5=32-bit
    algo::Smallstr20   suffix;               // suffix appended to the lib_pb Encode/Decode function names
    algo::Smallstr50   encval;               // Encoder value expression; $fldval is the field access expression
    algo::Smallstr50   decval;               // Decoder target expression; $fldval is the field access expression
    algo::Smallstr50   nondflt;              // C++ expression, true when $fldval differs from the proto3 default
    algo::Smallstr50   argtype;              //   ""  C++ arg ctype the lib_pb codec functions accept; empty when special-cased (enum, message)
    algo::cstring      comment;              //
    // func:amc.FPbtype..AssignOp
    inline amc::FPbtype& operator =(const amc::FPbtype &rhs) = delete;
    // func:amc.FPbtype..CopyCtor
    inline               FPbtype(const amc::FPbtype &rhs) = delete;
private:
    // func:amc.FPbtype..Ctor
    inline               FPbtype() __attribute__((nothrow));
    // func:amc.FPbtype..Dtor
    inline               ~FPbtype() __attribute__((nothrow));
    friend amc::FPbtype&        pbtype_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FPbtype*        pbtype_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 pbtype_RemoveAll() __attribute__((nothrow));
    friend void                 pbtype_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FPmaskfld - Specify which field holds presence mask bits (one for each field in struct)
<a href="#amc-fpmaskfld"></a>

#### amc.FPmaskfld Fields
<a href="#amc-fpmaskfld-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FPmaskfld.msghdr|[dmmeta.Pmaskfld](/txt/ssimdb/dmmeta/pmaskfld.md)|[Base](/txt/ssimdb/dmmeta/pmaskfld.md)|||
|amc.FPmaskfld.p_field|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|amc.FPmaskfld.nextbit|u32|[Val](/txt/exe/amc/reftype.md#val)||Next bit to allocate|
|amc.FPmaskfld.c_pmaskfld_member|[amc.FPmaskfldMember](/txt/gen/amc/amc.md#amc-fpmaskfldmember)|[Ptrary](/txt/exe/amc/reftype.md#ptrary)|||
|amc.FPmaskfld.funcname|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftype.md#val)|||

#### Struct FPmaskfld
<a href="#struct-fpmaskfld"></a>
*Note:* field ``amc.FPmaskfld.msghdr`` has reftype ``base`` so the fields of [dmmeta.Pmaskfld](/txt/ssimdb/dmmeta/pmaskfld.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FPmaskfld { // amc.FPmaskfld
    amc::FPmaskfld*          ind_pmaskfld_next;         // hash next
    u32                      ind_pmaskfld_hashval;      // hash value
    algo::Smallstr150        field;                     //
    bool                     filter_print;              //   true  Omit non-present fields when printing
    algo::cstring            comment;                   //
    amc::FField*             p_field;                   // reference to parent row
    u32                      nextbit;                   //   0  Next bit to allocate
    amc::FPmaskfldMember**   c_pmaskfld_member_elems;   // array of pointers
    u64                      c_pmaskfld_member_n;       // current size
    u64                      c_pmaskfld_member_max;     // capacity of allocated array
    algo::Smallstr50         funcname;                  //
    bool                     ctype_c_pmaskfld_in_ary;   //   false  membership flag
    // x-reference on amc.FPmaskfld.p_field prevents copy
    // reftype Ptrary of amc.FPmaskfld.c_pmaskfld_member prohibits copy
    // func:amc.FPmaskfld..AssignOp
    amc::FPmaskfld&      operator =(const amc::FPmaskfld &rhs) = delete;
    // x-reference on amc.FPmaskfld.p_field prevents copy
    // reftype Ptrary of amc.FPmaskfld.c_pmaskfld_member prohibits copy
    // func:amc.FPmaskfld..CopyCtor
    FPmaskfld(const amc::FPmaskfld &rhs) = delete;
private:
    // func:amc.FPmaskfld..Ctor
    inline               FPmaskfld() __attribute__((nothrow));
    // func:amc.FPmaskfld..Dtor
    inline               ~FPmaskfld() __attribute__((nothrow));
    friend amc::FPmaskfld&      pmaskfld_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FPmaskfld*      pmaskfld_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 pmaskfld_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FPmaskfldMember - Explicit field-to-pmask association
<a href="#amc-fpmaskfldmember"></a>

#### amc.FPmaskfldMember Fields
<a href="#amc-fpmaskfldmember-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FPmaskfldMember.base|[dmmeta.PmaskfldMember](/txt/ssimdb/dmmeta/pmaskfld_member.md)|[Base](/txt/ssimdb/dmmeta/pmaskfld_member.md)|||
|amc.FPmaskfldMember.bit|u32|[Val](/txt/exe/amc/reftype.md#val)|||
|amc.FPmaskfldMember.p_field|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|amc.FPmaskfldMember.p_pmaskfld|[amc.FPmaskfld](/txt/gen/amc/amc.md#amc-fpmaskfld)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||

#### Struct FPmaskfldMember
<a href="#struct-fpmaskfldmember"></a>
*Note:* field ``amc.FPmaskfldMember.base`` has reftype ``base`` so the fields of [dmmeta.PmaskfldMember](/txt/ssimdb/dmmeta/pmaskfld_member.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FPmaskfldMember { // amc.FPmaskfldMember
    algo::Smallstr250   pmaskfld_member;                     //
    algo::cstring       comment;                             //
    u32                 bit;                                 //   0
    amc::FField*        p_field;                             // reference to parent row
    amc::FPmaskfld*     p_pmaskfld;                          // reference to parent row
    bool                field_c_pmaskfld_member_in_ary;      //   false  membership flag
    bool                pmaskfld_c_pmaskfld_member_in_ary;   //   false  membership flag
    // x-reference on amc.FPmaskfldMember.p_field prevents copy
    // x-reference on amc.FPmaskfldMember.p_pmaskfld prevents copy
    // func:amc.FPmaskfldMember..AssignOp
    amc::FPmaskfldMember& operator =(const amc::FPmaskfldMember &rhs) = delete;
    // x-reference on amc.FPmaskfldMember.p_field prevents copy
    // x-reference on amc.FPmaskfldMember.p_pmaskfld prevents copy
    // func:amc.FPmaskfldMember..CopyCtor
    FPmaskfldMember(const amc::FPmaskfldMember &rhs) = delete;
private:
    // func:amc.FPmaskfldMember..Ctor
    inline               FPmaskfldMember() __attribute__((nothrow));
    // func:amc.FPmaskfldMember..Dtor
    inline               ~FPmaskfldMember() __attribute__((nothrow));
    friend amc::FPmaskfldMember& pmaskfld_member_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FPmaskfldMember* pmaskfld_member_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 pmaskfld_member_RemoveAll() __attribute__((nothrow));
    friend void                 pmaskfld_member_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FPnew - Generate custom constructor (placement new), for use with binary protocols
<a href="#amc-fpnew"></a>

#### amc.FPnew Fields
<a href="#amc-fpnew-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FPnew.msghdr|[dmmeta.Pnew](/txt/ssimdb/dmmeta/pnew.md)|[Base](/txt/ssimdb/dmmeta/pnew.md)|||
|amc.FPnew.p_ctype|[amc.FCtype](/txt/gen/amc/amc.md#amc-fctype)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|amc.FPnew.p_ns|[amc.FNs](/txt/gen/amc/amc.md#amc-fns)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||

#### Struct FPnew
<a href="#struct-fpnew"></a>
*Note:* field ``amc.FPnew.msghdr`` has reftype ``base`` so the fields of [dmmeta.Pnew](/txt/ssimdb/dmmeta/pnew.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FPnew { // amc.FPnew
    amc::FPnew*         ind_pnew_next;         // hash next
    u32                 ind_pnew_hashval;      // hash value
    algo::Smallstr100   pnew;                  //
    algo::cstring       comment;               //
    amc::FCtype*        p_ctype;               // reference to parent row
    amc::FNs*           p_ns;                  // reference to parent row
    bool                ctype_c_pnew_in_ary;   //   false  membership flag
    bool                ns_c_pnew_in_ary;      //   false  membership flag
    // x-reference on amc.FPnew.p_ctype prevents copy
    // x-reference on amc.FPnew.p_ns prevents copy
    // func:amc.FPnew..AssignOp
    amc::FPnew&          operator =(const amc::FPnew &rhs) = delete;
    // x-reference on amc.FPnew.p_ctype prevents copy
    // x-reference on amc.FPnew.p_ns prevents copy
    // func:amc.FPnew..CopyCtor
    FPnew(const amc::FPnew &rhs) = delete;
private:
    // func:amc.FPnew..Ctor
    inline               FPnew() __attribute__((nothrow));
    // func:amc.FPnew..Dtor
    inline               ~FPnew() __attribute__((nothrow));
    friend amc::FPnew&          pnew_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FPnew*          pnew_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 pnew_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FPtrary - Required for fields with reftype:Ptrary
<a href="#amc-fptrary"></a>

#### amc.FPtrary Fields
<a href="#amc-fptrary-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FPtrary.msghdr|[dmmeta.Ptrary](/txt/ssimdb/dmmeta/ptrary.md)|[Base](/txt/ssimdb/dmmeta/ptrary.md)|||
|amc.FPtrary.p_field|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||

#### Struct FPtrary
<a href="#struct-fptrary"></a>
*Note:* field ``amc.FPtrary.msghdr`` has reftype ``base`` so the fields of [dmmeta.Ptrary](/txt/ssimdb/dmmeta/ptrary.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FPtrary { // amc.FPtrary
    algo::Smallstr150   field;      //
    bool                unique;     //   false  Search for and ignore duplicates
    bool                heaplike;   //   false
    amc::FField*        p_field;    // reference to parent row
    // x-reference on amc.FPtrary.p_field prevents copy
    // func:amc.FPtrary..AssignOp
    inline amc::FPtrary& operator =(const amc::FPtrary &rhs) = delete;
    // x-reference on amc.FPtrary.p_field prevents copy
    // func:amc.FPtrary..CopyCtor
    inline               FPtrary(const amc::FPtrary &rhs) = delete;
private:
    // func:amc.FPtrary..Ctor
    inline               FPtrary() __attribute__((nothrow));
    // func:amc.FPtrary..Dtor
    inline               ~FPtrary() __attribute__((nothrow));
    friend amc::FPtrary&        ptrary_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FPtrary*        ptrary_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 ptrary_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FReftype - Field type constructor (e.g. reference type)
<a href="#amc-freftype"></a>

#### amc.FReftype Fields
<a href="#amc-freftype-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FReftype.msghdr|[dmmeta.Reftype](/txt/ssimdb/dmmeta/reftype.md)|[Base](/txt/ssimdb/dmmeta/reftype.md)|||
|amc.FReftype.rowid|i32|[Val](/txt/exe/amc/reftype.md#val)|||
|amc.FReftype.p_tclass|[amc.FTclass](/txt/gen/amc/amc.md#amc-ftclass)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|amc.FReftype.zs_fprefix|[amc.FFprefix](/txt/gen/amc/amc.md#amc-ffprefix)|[Llist](/txt/exe/amc/reftype.md#llist)|||

#### Struct FReftype
<a href="#struct-freftype"></a>
*Note:* field ``amc.FReftype.msghdr`` has reftype ``base`` so the fields of [dmmeta.Reftype](/txt/ssimdb/dmmeta/reftype.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FReftype { // amc.FReftype
    amc::FReftype*     ind_reftype_next;      // hash next
    u32                ind_reftype_hashval;   // hash value
    algo::Smallstr50   reftype;               //   "Val"
    bool               isval;                 //   false  True if field makes values of target type
    bool               cascins;               //   false  Field is cascade-insert
    bool               usebasepool;           //   false  Fields with this type make use of dmmeta.basepool
    bool               cancopy;               //   false  This type of field can be copied
    bool               needxref;              //   false  Creating a field of this reftype implies an xref (acr_ed)
    bool               del;                   //   false  Supports random deletion?
    bool               up;                    //   false  This type of field is a reference
    bool               isnew;                 //   false  If set, skip this relation in amc_vis
    bool               hasalloc;              //   false  Generte Alloc/Delete functions for arg type
    bool               inst;                  //   false  Field creates an instance of arg type (directly or indirectly)
    bool               varlen;                //   false  This pool supports varlen allocations
    i32                rowid;                 //   0
    amc::FTclass*      p_tclass;              // reference to parent row
    amc::FFprefix*     zs_fprefix_head;       // zero-terminated singly linked list
    amc::FFprefix*     zs_fprefix_tail;       // pointer to last element
    // x-reference on amc.FReftype.p_tclass prevents copy
    // reftype Llist of amc.FReftype.zs_fprefix prohibits copy
    // func:amc.FReftype..AssignOp
    amc::FReftype&       operator =(const amc::FReftype &rhs) = delete;
    // x-reference on amc.FReftype.p_tclass prevents copy
    // reftype Llist of amc.FReftype.zs_fprefix prohibits copy
    // func:amc.FReftype..CopyCtor
    FReftype(const amc::FReftype &rhs) = delete;
private:
    // func:amc.FReftype..Ctor
    inline               FReftype() __attribute__((nothrow));
    // func:amc.FReftype..Dtor
    inline               ~FReftype() __attribute__((nothrow));
    friend amc::FReftype&       reftype_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FReftype*       reftype_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 reftype_RemoveAll() __attribute__((nothrow));
    friend void                 reftype_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FRowid - Initialize field to row id of element
<a href="#amc-frowid"></a>

#### amc.FRowid Fields
<a href="#amc-frowid-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FRowid.msghdr|[dmmeta.Rowid](/txt/ssimdb/dmmeta/rowid.md)|[Base](/txt/ssimdb/dmmeta/rowid.md)|||

#### Struct FRowid
<a href="#struct-frowid"></a>
*Note:* field ``amc.FRowid.msghdr`` has reftype ``base`` so the fields of [dmmeta.Rowid](/txt/ssimdb/dmmeta/rowid.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FRowid { // amc.FRowid
    amc::FRowid*        ind_rowid_next;      // hash next
    u32                 ind_rowid_hashval;   // hash value
    algo::Smallstr150   field;               //
    algo::cstring       comment;             //
    // func:amc.FRowid..AssignOp
    inline amc::FRowid&  operator =(const amc::FRowid &rhs) = delete;
    // func:amc.FRowid..CopyCtor
    inline               FRowid(const amc::FRowid &rhs) = delete;
private:
    // func:amc.FRowid..Ctor
    inline               FRowid() __attribute__((nothrow));
    // func:amc.FRowid..Dtor
    inline               ~FRowid() __attribute__((nothrow));
    friend amc::FRowid&         rowid_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FRowid*         rowid_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 rowid_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FSmallstr - Generated fixed-length padded or length-delimited string field
<a href="#amc-fsmallstr"></a>

#### amc.FSmallstr Fields
<a href="#amc-fsmallstr-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FSmallstr.msghdr|[dmmeta.Smallstr](/txt/ssimdb/dmmeta/smallstr.md)|[Base](/txt/ssimdb/dmmeta/smallstr.md)|||
|amc.FSmallstr.p_field|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|amc.FSmallstr.c_numstr|[amc.FNumstr](/txt/gen/amc/amc.md#amc-fnumstr)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||

#### Struct FSmallstr
<a href="#struct-fsmallstr"></a>
*Note:* field ``amc.FSmallstr.msghdr`` has reftype ``base`` so the fields of [dmmeta.Smallstr](/txt/ssimdb/dmmeta/smallstr.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FSmallstr { // amc.FSmallstr
    amc::FSmallstr*     ind_smallstr_next;      // hash next
    u32                 ind_smallstr_hashval;   // hash value
    algo::Smallstr150   field;                  //
    i32                 length;                 //   0  Maximum characters in the string
    algo::Smallstr50    strtype;                // Data format for string
    algo::CppExpr       pad;                    // Pad character (if applicable)
    bool                strict;                 //   false
    amc::FField*        p_field;                // reference to parent row
    amc::FNumstr*       c_numstr;               // optional pointer
    // x-reference on amc.FSmallstr.p_field prevents copy
    // x-reference on amc.FSmallstr.c_numstr prevents copy
    // func:amc.FSmallstr..AssignOp
    inline amc::FSmallstr& operator =(const amc::FSmallstr &rhs) = delete;
    // x-reference on amc.FSmallstr.p_field prevents copy
    // x-reference on amc.FSmallstr.c_numstr prevents copy
    // func:amc.FSmallstr..CopyCtor
    inline               FSmallstr(const amc::FSmallstr &rhs) = delete;
private:
    // func:amc.FSmallstr..Ctor
    inline               FSmallstr() __attribute__((nothrow));
    // func:amc.FSmallstr..Dtor
    inline               ~FSmallstr() __attribute__((nothrow));
    friend amc::FSmallstr&      smallstr_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FSmallstr*      smallstr_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 smallstr_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FSortfld - Specify what field an index (Bheap,Atree) is sorted on
<a href="#amc-fsortfld"></a>

#### amc.FSortfld Fields
<a href="#amc-fsortfld-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FSortfld.msghdr|[dmmeta.Sortfld](/txt/ssimdb/dmmeta/sortfld.md)|[Base](/txt/ssimdb/dmmeta/sortfld.md)|||
|amc.FSortfld.p_sortfld|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||

#### Struct FSortfld
<a href="#struct-fsortfld"></a>
*Note:* field ``amc.FSortfld.msghdr`` has reftype ``base`` so the fields of [dmmeta.Sortfld](/txt/ssimdb/dmmeta/sortfld.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FSortfld { // amc.FSortfld
    algo::Smallstr150   field;       //
    algo::Smallstr150   sortfld;     // Field to sort by
    amc::FField*        p_sortfld;   // reference to parent row
    // x-reference on amc.FSortfld.p_sortfld prevents copy
    // func:amc.FSortfld..AssignOp
    inline amc::FSortfld& operator =(const amc::FSortfld &rhs) = delete;
    // x-reference on amc.FSortfld.p_sortfld prevents copy
    // func:amc.FSortfld..CopyCtor
    inline               FSortfld(const amc::FSortfld &rhs) = delete;
private:
    // func:amc.FSortfld..Ctor
    inline               FSortfld() __attribute__((nothrow));
    // func:amc.FSortfld..Dtor
    inline               ~FSortfld() __attribute__((nothrow));
    friend amc::FSortfld&       sortfld_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FSortfld*       sortfld_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 sortfld_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FSsimfile - File with ssim tuples
<a href="#amc-fssimfile"></a>

#### amc.FSsimfile Fields
<a href="#amc-fssimfile-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FSsimfile.msghdr|[dmmeta.Ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|[Base](/txt/ssimdb/dmmeta/ssimfile.md)|||
|amc.FSsimfile.ssim|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||Ssim content|
|amc.FSsimfile.p_ctype|[amc.FCtype](/txt/gen/amc/amc.md#amc-fctype)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|amc.FSsimfile.c_ssimvolatile|[amc.FSsimvolatile](/txt/gen/amc/amc.md#amc-fssimvolatile)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FSsimfile.topovisit|bool|[Val](/txt/exe/amc/reftype.md#val)|||
|amc.FSsimfile.topoindex|i32|[Val](/txt/exe/amc/reftype.md#val)|||
|amc.FSsimfile.input_select|bool|[Val](/txt/exe/amc/reftype.md#val)|||

#### Struct FSsimfile
<a href="#struct-fssimfile"></a>
*Note:* field ``amc.FSsimfile.msghdr`` has reftype ``base`` so the fields of [dmmeta.Ssimfile](/txt/ssimdb/dmmeta/ssimfile.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FSsimfile { // amc.FSsimfile
    amc::FSsimfile*       ind_ssimfile_next;          // hash next
    u32                   ind_ssimfile_hashval;       // hash value
    amc::FSsimfile*       zd_ssimfile_todo_next;      // zslist link; -1 means not-in-list
    amc::FSsimfile*       zd_ssimfile_todo_prev;      // previous element
    algo::Smallstr50      ssimfile;                   //
    algo::Smallstr100     ctype;                      //
    algo::cstring         ssim;                       // Ssim content
    amc::FCtype*          p_ctype;                    // reference to parent row
    amc::FSsimvolatile*   c_ssimvolatile;             // optional pointer
    bool                  topovisit;                  //   false
    i32                   topoindex;                  //   0
    bool                  input_select;               //   false
    bool                  c_ssimfile_sorted_in_ary;   //   false  membership flag
    // x-reference on amc.FSsimfile.p_ctype prevents copy
    // x-reference on amc.FSsimfile.c_ssimvolatile prevents copy
    // func:amc.FSsimfile..AssignOp
    amc::FSsimfile&      operator =(const amc::FSsimfile &rhs) = delete;
    // x-reference on amc.FSsimfile.p_ctype prevents copy
    // x-reference on amc.FSsimfile.c_ssimvolatile prevents copy
    // func:amc.FSsimfile..CopyCtor
    FSsimfile(const amc::FSsimfile &rhs) = delete;
private:
    // func:amc.FSsimfile..Ctor
    inline               FSsimfile() __attribute__((nothrow));
    // func:amc.FSsimfile..Dtor
    inline               ~FSsimfile() __attribute__((nothrow));
    friend amc::FSsimfile&      ssimfile_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FSsimfile*      ssimfile_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 ssimfile_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FSsimsort - Define sort order for ssimfile
<a href="#amc-fssimsort"></a>

#### amc.FSsimsort Fields
<a href="#amc-fssimsort-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FSsimsort.base|[dmmeta.Ssimsort](/txt/ssimdb/dmmeta/ssimsort.md)|[Base](/txt/ssimdb/dmmeta/ssimsort.md)|||
|amc.FSsimsort.p_ssimfile|[amc.FSsimfile](/txt/gen/amc/amc.md#amc-fssimfile)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|amc.FSsimsort.p_sortfld|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||

#### Struct FSsimsort
<a href="#struct-fssimsort"></a>
*Note:* field ``amc.FSsimsort.base`` has reftype ``base`` so the fields of [dmmeta.Ssimsort](/txt/ssimdb/dmmeta/ssimsort.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FSsimsort { // amc.FSsimsort
    algo::Smallstr50    ssimfile;     //
    algo::Smallstr150   sortfld;      //
    algo::cstring       comment;      //
    amc::FSsimfile*     p_ssimfile;   // reference to parent row
    amc::FField*        p_sortfld;    // reference to parent row
private:
    // func:amc.FSsimsort..Ctor
    inline               FSsimsort() __attribute__((nothrow));
    friend amc::FSsimsort&      ssimsort_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FSsimsort*      ssimsort_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 ssimsort_RemoveAll() __attribute__((nothrow));
    friend void                 ssimsort_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FSsimvolatile - Types based on ssim files marked this way cannot be cross-referenced
<a href="#amc-fssimvolatile"></a>

#### amc.FSsimvolatile Fields
<a href="#amc-fssimvolatile-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FSsimvolatile.base|[dmmeta.Ssimvolatile](/txt/ssimdb/dmmeta/ssimvolatile.md)|[Base](/txt/ssimdb/dmmeta/ssimvolatile.md)|||

#### Struct FSsimvolatile
<a href="#struct-fssimvolatile"></a>
*Note:* field ``amc.FSsimvolatile.base`` has reftype ``base`` so the fields of [dmmeta.Ssimvolatile](/txt/ssimdb/dmmeta/ssimvolatile.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FSsimvolatile { // amc.FSsimvolatile
    algo::Smallstr50   ssimfile;   //
    algo::cstring      comment;    //
    // func:amc.FSsimvolatile..AssignOp
    inline amc::FSsimvolatile& operator =(const amc::FSsimvolatile &rhs) = delete;
    // func:amc.FSsimvolatile..CopyCtor
    inline               FSsimvolatile(const amc::FSsimvolatile &rhs) = delete;
private:
    // func:amc.FSsimvolatile..Ctor
    inline               FSsimvolatile() __attribute__((nothrow));
    // func:amc.FSsimvolatile..Dtor
    inline               ~FSsimvolatile() __attribute__((nothrow));
    friend amc::FSsimvolatile&  ssimvolatile_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FSsimvolatile*  ssimvolatile_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 ssimvolatile_RemoveAll() __attribute__((nothrow));
    friend void                 ssimvolatile_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FStatictuple
<a href="#amc-fstatictuple"></a>

#### amc.FStatictuple Fields
<a href="#amc-fstatictuple-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FStatictuple.ctype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Pkey](/txt/exe/amc/reftype.md#pkey)|||
|amc.FStatictuple.tuple|[algo.Tuple](/txt/protocol/algo/Tuple.md)|[Val](/txt/exe/amc/reftype.md#val)|||
|amc.FStatictuple.cppident|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|||

#### Struct FStatictuple
<a href="#struct-fstatictuple"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FStatictuple { // amc.FStatictuple
    algo::Smallstr100   ctype;                   //
    algo::Tuple         tuple;                   //
    algo::cstring       cppident;                //
    bool                ctype_c_static_in_ary;   //   false  membership flag
    // func:amc.FStatictuple..AssignOp
    inline amc::FStatictuple& operator =(const amc::FStatictuple &rhs) = delete;
    // func:amc.FStatictuple..CopyCtor
    inline               FStatictuple(const amc::FStatictuple &rhs) = delete;
private:
    // func:amc.FStatictuple..Ctor
    inline               FStatictuple() __attribute__((nothrow));
    // func:amc.FStatictuple..Dtor
    inline               ~FStatictuple() __attribute__((nothrow));
    friend amc::FStatictuple&   static_tuple_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FStatictuple*   static_tuple_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 static_tuple_RemoveAll() __attribute__((nothrow));
    friend void                 static_tuple_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FSteptype - Type of scheduler step
<a href="#amc-fsteptype"></a>

#### amc.FSteptype Fields
<a href="#amc-fsteptype-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FSteptype.base|[dmmeta.Steptype](/txt/ssimdb/dmmeta/steptype.md)|[Base](/txt/ssimdb/dmmeta/steptype.md)|||

#### Struct FSteptype
<a href="#struct-fsteptype"></a>
*Note:* field ``amc.FSteptype.base`` has reftype ``base`` so the fields of [dmmeta.Steptype](/txt/ssimdb/dmmeta/steptype.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FSteptype { // amc.FSteptype
    amc::FSteptype*    ind_steptype_next;      // hash next
    u32                ind_steptype_hashval;   // hash value
    algo::Smallstr50   steptype;               //
    algo::cstring      comment;                //
    // func:amc.FSteptype..AssignOp
    inline amc::FSteptype& operator =(const amc::FSteptype &rhs) = delete;
    // func:amc.FSteptype..CopyCtor
    inline               FSteptype(const amc::FSteptype &rhs) = delete;
private:
    // func:amc.FSteptype..Ctor
    inline               FSteptype() __attribute__((nothrow));
    // func:amc.FSteptype..Dtor
    inline               ~FSteptype() __attribute__((nothrow));
    friend amc::FSteptype&      steptype_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FSteptype*      steptype_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 steptype_RemoveAll() __attribute__((nothrow));
    friend void                 steptype_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FSubstr - Specify that the field value is computed from a substring of another field
<a href="#amc-fsubstr"></a>

#### amc.FSubstr Fields
<a href="#amc-fsubstr-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FSubstr.msghdr|[dmmeta.Substr](/txt/ssimdb/dmmeta/substr.md)|[Base](/txt/ssimdb/dmmeta/substr.md)|||
|amc.FSubstr.p_field|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|amc.FSubstr.p_srcfield|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|amc.FSubstr.range|u64|[Val](/txt/exe/amc/reftype.md#val)|||

#### Struct FSubstr
<a href="#struct-fsubstr"></a>
*Note:* field ``amc.FSubstr.msghdr`` has reftype ``base`` so the fields of [dmmeta.Substr](/txt/ssimdb/dmmeta/substr.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FSubstr { // amc.FSubstr
    amc::FSubstr*       zd_substr_params_next;   // zslist link; -1 means not-in-list
    amc::FSubstr*       zd_substr_params_prev;   // previous element
    algo::Smallstr150   field;                   //
    algo::CppExpr       expr;                    //
    algo::Smallstr150   srcfield;                //
    amc::FField*        p_field;                 // reference to parent row
    amc::FField*        p_srcfield;              // reference to parent row
    u64                 range;                   //   0
    bool                c_substr_field_in_ary;   //   false  membership flag
    // x-reference on amc.FSubstr.p_field prevents copy
    // x-reference on amc.FSubstr.p_srcfield prevents copy
    // func:amc.FSubstr..AssignOp
    inline amc::FSubstr& operator =(const amc::FSubstr &rhs) = delete;
    // x-reference on amc.FSubstr.p_field prevents copy
    // x-reference on amc.FSubstr.p_srcfield prevents copy
    // func:amc.FSubstr..CopyCtor
    inline               FSubstr(const amc::FSubstr &rhs) = delete;
private:
    // func:amc.FSubstr..Ctor
    inline               FSubstr() __attribute__((nothrow));
    // func:amc.FSubstr..Dtor
    inline               ~FSubstr() __attribute__((nothrow));
    friend amc::FSubstr&        substr_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FSubstr*        substr_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 substr_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FTargdep - Dependency between targets
<a href="#amc-ftargdep"></a>

#### amc.FTargdep Fields
<a href="#amc-ftargdep-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FTargdep.msghdr|[dev.Targdep](/txt/ssimdb/dev/targdep.md)|[Base](/txt/ssimdb/dev/targdep.md)|||
|amc.FTargdep.p_target|[amc.FTarget](/txt/gen/amc/amc.md#amc-ftarget)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|amc.FTargdep.p_parent|[amc.FTarget](/txt/gen/amc/amc.md#amc-ftarget)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||

#### Struct FTargdep
<a href="#struct-ftargdep"></a>
*Note:* field ``amc.FTargdep.msghdr`` has reftype ``base`` so the fields of [dev.Targdep](/txt/ssimdb/dev/targdep.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FTargdep { // amc.FTargdep
    algo::Smallstr50   targdep;                   // Primary key - target.parent
    algo::cstring      comment;                   //
    amc::FTarget*      p_target;                  // reference to parent row
    amc::FTarget*      p_parent;                  // reference to parent row
    bool               target_c_targdep_in_ary;   //   false  membership flag
    // x-reference on amc.FTargdep.p_target prevents copy
    // x-reference on amc.FTargdep.p_parent prevents copy
    // func:amc.FTargdep..AssignOp
    inline amc::FTargdep& operator =(const amc::FTargdep &rhs) = delete;
    // x-reference on amc.FTargdep.p_target prevents copy
    // x-reference on amc.FTargdep.p_parent prevents copy
    // func:amc.FTargdep..CopyCtor
    inline               FTargdep(const amc::FTargdep &rhs) = delete;
private:
    // func:amc.FTargdep..Ctor
    inline               FTargdep() __attribute__((nothrow));
    // func:amc.FTargdep..Dtor
    inline               ~FTargdep() __attribute__((nothrow));
    friend amc::FTargdep&       targdep_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FTargdep*       targdep_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 targdep_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FTarget - Build target
<a href="#amc-ftarget"></a>

#### amc.FTarget Fields
<a href="#amc-ftarget-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FTarget.msghdr|[dev.Target](/txt/ssimdb/dev/target.md)|[Base](/txt/ssimdb/dev/target.md)|||
|amc.FTarget.c_targdep|[amc.FTargdep](/txt/gen/amc/amc.md#amc-ftargdep)|[Ptrary](/txt/exe/amc/reftype.md#ptrary)|||
|amc.FTarget.p_ns|[amc.FNs](/txt/gen/amc/amc.md#amc-fns)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||

#### Struct FTarget
<a href="#struct-ftarget"></a>
*Note:* field ``amc.FTarget.msghdr`` has reftype ``base`` so the fields of [dev.Target](/txt/ssimdb/dev/target.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FTarget { // amc.FTarget
    amc::FTarget*      ind_target_next;      // hash next
    u32                ind_target_hashval;   // hash value
    algo::Smallstr16   target;               // Primary key - name of target
    amc::FTargdep**    c_targdep_elems;      // array of pointers
    u64                c_targdep_n;          // current size
    u64                c_targdep_max;        // capacity of allocated array
    amc::FNs*          p_ns;                 // reference to parent row
    // reftype Ptrary of amc.FTarget.c_targdep prohibits copy
    // x-reference on amc.FTarget.p_ns prevents copy
    // func:amc.FTarget..AssignOp
    inline amc::FTarget& operator =(const amc::FTarget &rhs) = delete;
    // reftype Ptrary of amc.FTarget.c_targdep prohibits copy
    // x-reference on amc.FTarget.p_ns prevents copy
    // func:amc.FTarget..CopyCtor
    inline               FTarget(const amc::FTarget &rhs) = delete;
private:
    // func:amc.FTarget..Ctor
    inline               FTarget() __attribute__((nothrow));
    // func:amc.FTarget..Dtor
    inline               ~FTarget() __attribute__((nothrow));
    friend amc::FTarget&        target_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FTarget*        target_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 target_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FTary - Indirect linear dynamically alocated array (used for strings, arrays of data, etc)
<a href="#amc-ftary"></a>

#### amc.FTary Fields
<a href="#amc-ftary-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FTary.msghdr|[dmmeta.Tary](/txt/ssimdb/dmmeta/tary.md)|[Base](/txt/ssimdb/dmmeta/tary.md)|||
|amc.FTary.p_field|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||

#### Struct FTary
<a href="#struct-ftary"></a>
*Note:* field ``amc.FTary.msghdr`` has reftype ``base`` so the fields of [dmmeta.Tary](/txt/ssimdb/dmmeta/tary.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FTary { // amc.FTary
    amc::FTary*         ind_tary_next;      // hash next
    u32                 ind_tary_hashval;   // hash value
    algo::Smallstr150   field;              //
    bool                aliased;            //   false  Geneate functions to copy from aryptr
    algo::cstring       comment;            //
    amc::FField*        p_field;            // reference to parent row
    // x-reference on amc.FTary.p_field prevents copy
    // func:amc.FTary..AssignOp
    inline amc::FTary&   operator =(const amc::FTary &rhs) = delete;
    // x-reference on amc.FTary.p_field prevents copy
    // func:amc.FTary..CopyCtor
    inline               FTary(const amc::FTary &rhs) = delete;
private:
    // func:amc.FTary..Ctor
    inline               FTary() __attribute__((nothrow));
    // func:amc.FTary..Dtor
    inline               ~FTary() __attribute__((nothrow));
    friend amc::FTary&          tary_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FTary*          tary_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 tary_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FTcond - Condition a tclass reports, which an fcond binds to an index
<a href="#amc-ftcond"></a>

#### amc.FTcond Fields
<a href="#amc-ftcond-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FTcond.base|[amcdb.Tcond](/txt/ssimdb/amcdb/tcond.md)|[Base](/txt/ssimdb/amcdb/tcond.md)|||

#### Struct FTcond
<a href="#struct-ftcond"></a>
*Note:* field ``amc.FTcond.base`` has reftype ``base`` so the fields of [amcdb.Tcond](/txt/ssimdb/amcdb/tcond.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FTcond { // amc.FTcond
    amc::FTcond*       ind_tcond_next;          // hash next
    u32                ind_tcond_hashval;       // hash value
    algo::Smallstr50   tcond;                   //
    algo::cstring      comment;                 //
    bool               tclass_c_tcond_in_ary;   //   false  membership flag
    // func:amc.FTcond..AssignOp
    inline amc::FTcond&  operator =(const amc::FTcond &rhs) = delete;
    // func:amc.FTcond..CopyCtor
    inline               FTcond(const amc::FTcond &rhs) = delete;
private:
    // func:amc.FTcond..Ctor
    inline               FTcond() __attribute__((nothrow));
    // func:amc.FTcond..Dtor
    inline               ~FTcond() __attribute__((nothrow));
    friend amc::FTcond&         tcond_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FTcond*         tcond_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 tcond_RemoveAll() __attribute__((nothrow));
    friend void                 tcond_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FTcurs - Cursor template
<a href="#amc-ftcurs"></a>

#### amc.FTcurs Fields
<a href="#amc-ftcurs-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FTcurs.base|[amcdb.Tcurs](/txt/ssimdb/amcdb/tcurs.md)|[Base](/txt/ssimdb/amcdb/tcurs.md)|||

#### Struct FTcurs
<a href="#struct-ftcurs"></a>
*Note:* field ``amc.FTcurs.base`` has reftype ``base`` so the fields of [amcdb.Tcurs](/txt/ssimdb/amcdb/tcurs.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FTcurs { // amc.FTcurs
    algo::Smallstr50   tfunc;     //
    bool               dflt;      //   false  Generate cursor by default? (if not, require fcurs)
    algo::cstring      comment;   //
    // func:amc.FTcurs..AssignOp
    inline amc::FTcurs&  operator =(const amc::FTcurs &rhs) = delete;
    // func:amc.FTcurs..CopyCtor
    inline               FTcurs(const amc::FTcurs &rhs) = delete;
private:
    // func:amc.FTcurs..Ctor
    inline               FTcurs() __attribute__((nothrow));
    // func:amc.FTcurs..Dtor
    inline               ~FTcurs() __attribute__((nothrow));
    friend amc::FTcurs&         tcurs_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FTcurs*         tcurs_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 tcurs_RemoveAll() __attribute__((nothrow));
    friend void                 tcurs_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FTfunc - AMC template function
<a href="#amc-ftfunc"></a>

#### amc.FTfunc Fields
<a href="#amc-ftfunc-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FTfunc.msghdr|[amcdb.Tfunc](/txt/ssimdb/amcdb/tfunc.md)|[Base](/txt/ssimdb/amcdb/tfunc.md)|||
|amc.FTfunc.step||[Hook](/txt/exe/amc/reftype.md#hook)|||
|amc.FTfunc.c_tcurs|[amc.FTcurs](/txt/gen/amc/amc.md#amc-ftcurs)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||

#### Struct FTfunc
<a href="#struct-ftfunc"></a>
*Note:* field ``amc.FTfunc.msghdr`` has reftype ``base`` so the fields of [amcdb.Tfunc](/txt/ssimdb/amcdb/tfunc.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FTfunc { // amc.FTfunc
    amc::FTfunc*           ind_tfunc_next;          // hash next
    u32                    ind_tfunc_hashval;       // hash value
    algo::Smallstr50       tfunc;                   //
    bool                   hasthrow;                //   false  Important defaults for new tfuncs to work
    bool                   leaf;                    //   true  Important defaults for new tfuncs to work
    bool                   poolfunc;                //   false
    bool                   inl;                     //   false  Function is typically inline
    bool                   wur;                     //   false  Warn unused result
    bool                   pure;                    //   false  OK to call fewer times than specified in text
    bool                   ismacro;                 //   false
    algo::cstring          comment;                 //
    amc::tfunc_step_hook   step;                    //   NULL  Pointer to a function
    amc::FTcurs*           c_tcurs;                 // optional pointer
    bool                   tclass_c_tfunc_in_ary;   //   false  membership flag
    // reftype Hook of amc.FTfunc.step prohibits copy
    // x-reference on amc.FTfunc.c_tcurs prevents copy
    // func:amc.FTfunc..AssignOp
    amc::FTfunc&         operator =(const amc::FTfunc &rhs) = delete;
    // reftype Hook of amc.FTfunc.step prohibits copy
    // x-reference on amc.FTfunc.c_tcurs prevents copy
    // func:amc.FTfunc..CopyCtor
    FTfunc(const amc::FTfunc &rhs) = delete;
private:
    // func:amc.FTfunc..Ctor
    inline               FTfunc() __attribute__((nothrow));
    // func:amc.FTfunc..Dtor
    inline               ~FTfunc() __attribute__((nothrow));
    friend amc::FTfunc&         tfunc_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FTfunc*         tfunc_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 tfunc_RemoveAll() __attribute__((nothrow));
    friend void                 tfunc_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FThash - Hash index, required for fields with reftype Thash
<a href="#amc-fthash"></a>

#### amc.FThash Fields
<a href="#amc-fthash-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FThash.msghdr|[dmmeta.Thash](/txt/ssimdb/dmmeta/thash.md)|[Base](/txt/ssimdb/dmmeta/thash.md)|||
|amc.FThash.p_hashfld|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|amc.FThash.p_field|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||

#### Struct FThash
<a href="#struct-fthash"></a>
*Note:* field ``amc.FThash.msghdr`` has reftype ``base`` so the fields of [dmmeta.Thash](/txt/ssimdb/dmmeta/thash.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FThash { // amc.FThash
    algo::Smallstr150   field;       //
    algo::Smallstr150   hashfld;     // Field on which hash function is computed
    bool                unique;      //   true  Enforce uniqueness of member elements
    algo::cstring       comment;     //
    amc::FField*        p_hashfld;   // reference to parent row
    amc::FField*        p_field;     // reference to parent row
    // x-reference on amc.FThash.p_hashfld prevents copy
    // x-reference on amc.FThash.p_field prevents copy
    // func:amc.FThash..AssignOp
    inline amc::FThash&  operator =(const amc::FThash &rhs) = delete;
    // x-reference on amc.FThash.p_hashfld prevents copy
    // x-reference on amc.FThash.p_field prevents copy
    // func:amc.FThash..CopyCtor
    inline               FThash(const amc::FThash &rhs) = delete;
private:
    // func:amc.FThash..Ctor
    inline               FThash() __attribute__((nothrow));
    // func:amc.FThash..Dtor
    inline               ~FThash() __attribute__((nothrow));
    friend amc::FThash&         thash_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FThash*         thash_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 thash_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FTracefld - Trace fields
<a href="#amc-ftracefld"></a>

#### amc.FTracefld Fields
<a href="#amc-ftracefld-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FTracefld.msghdr|[dmmeta.Tracefld](/txt/ssimdb/dmmeta/tracefld.md)|[Base](/txt/ssimdb/dmmeta/tracefld.md)|||

#### Struct FTracefld
<a href="#struct-ftracefld"></a>
*Note:* field ``amc.FTracefld.msghdr`` has reftype ``base`` so the fields of [dmmeta.Tracefld](/txt/ssimdb/dmmeta/tracefld.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FTracefld { // amc.FTracefld
    algo::Smallstr100   tracefld;   //
    algo::cstring       comment;    //
private:
    // func:amc.FTracefld..Ctor
    inline               FTracefld() __attribute__((nothrow));
    friend amc::FTracefld&      tracefld_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FTracefld*      tracefld_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 tracefld_RemoveAll() __attribute__((nothrow));
    friend void                 tracefld_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FTracerec - Trace record
<a href="#amc-ftracerec"></a>

#### amc.FTracerec Fields
<a href="#amc-ftracerec-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FTracerec.msghdr|[dmmeta.Tracerec](/txt/ssimdb/dmmeta/tracerec.md)|[Base](/txt/ssimdb/dmmeta/tracerec.md)|||

#### Struct FTracerec
<a href="#struct-ftracerec"></a>
*Note:* field ``amc.FTracerec.msghdr`` has reftype ``base`` so the fields of [dmmeta.Tracerec](/txt/ssimdb/dmmeta/tracerec.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FTracerec { // amc.FTracerec
    algo::Smallstr50   tracerec;   // e.g. rl_lib.trace
    algo::cstring      comment;    //
private:
    // func:amc.FTracerec..Ctor
    inline               FTracerec() __attribute__((nothrow));
    friend amc::FTracerec&      tracerec_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FTracerec*      tracerec_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 tracerec_RemoveAll() __attribute__((nothrow));
    friend void                 tracerec_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FTypefld - Specifies which field of a message carries the type
<a href="#amc-ftypefld"></a>

#### amc.FTypefld Fields
<a href="#amc-ftypefld-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FTypefld.msghdr|[dmmeta.Typefld](/txt/ssimdb/dmmeta/typefld.md)|[Base](/txt/ssimdb/dmmeta/typefld.md)|||
|amc.FTypefld.p_field|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|amc.FTypefld.p_ctype|[amc.FCtype](/txt/gen/amc/amc.md#amc-fctype)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||

#### Struct FTypefld
<a href="#struct-ftypefld"></a>
*Note:* field ``amc.FTypefld.msghdr`` has reftype ``base`` so the fields of [dmmeta.Typefld](/txt/ssimdb/dmmeta/typefld.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FTypefld { // amc.FTypefld
    algo::Smallstr150   field;     //
    algo::cstring       comment;   //
    amc::FField*        p_field;   // reference to parent row
    amc::FCtype*        p_ctype;   // reference to parent row
    // x-reference on amc.FTypefld.p_field prevents copy
    // x-reference on amc.FTypefld.p_ctype prevents copy
    // func:amc.FTypefld..AssignOp
    inline amc::FTypefld& operator =(const amc::FTypefld &rhs) = delete;
    // x-reference on amc.FTypefld.p_field prevents copy
    // x-reference on amc.FTypefld.p_ctype prevents copy
    // func:amc.FTypefld..CopyCtor
    inline               FTypefld(const amc::FTypefld &rhs) = delete;
private:
    // func:amc.FTypefld..Ctor
    inline               FTypefld() __attribute__((nothrow));
    // func:amc.FTypefld..Dtor
    inline               ~FTypefld() __attribute__((nothrow));
    friend amc::FTypefld&       typefld_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FTypefld*       typefld_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 typefld_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FUserfunc
<a href="#amc-fuserfunc"></a>

#### amc.FUserfunc Fields
<a href="#amc-fuserfunc-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FUserfunc.base|[dmmeta.Userfunc](/txt/ssimdb/dmmeta/userfunc.md)|[Base](/txt/ssimdb/dmmeta/userfunc.md)|||

#### Struct FUserfunc
<a href="#struct-fuserfunc"></a>
*Note:* field ``amc.FUserfunc.base`` has reftype ``base`` so the fields of [dmmeta.Userfunc](/txt/ssimdb/dmmeta/userfunc.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FUserfunc { // amc.FUserfunc
    algo::Smallstr100   userfunc;   // Primary key: as wide as dmmeta.Func.func, the key it is copied from
    algo::Smallstr200   acrkey;     //
    algo::Smallstr100   cppname;    //
    algo::cstring       comment;    //
private:
    // func:amc.FUserfunc..Ctor
    inline               FUserfunc() __attribute__((nothrow));
    friend amc::FUserfunc&      userfunc_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FUserfunc*      userfunc_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 userfunc_RemoveAll() __attribute__((nothrow));
    friend void                 userfunc_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FUsertracefld - Add custom user trace fields to process's trace struct
<a href="#amc-fusertracefld"></a>

#### amc.FUsertracefld Fields
<a href="#amc-fusertracefld-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FUsertracefld.msghdr|[dmmeta.Usertracefld](/txt/ssimdb/dmmeta/usertracefld.md)|[Base](/txt/ssimdb/dmmeta/usertracefld.md)|||

#### Struct FUsertracefld
<a href="#struct-fusertracefld"></a>
*Note:* field ``amc.FUsertracefld.msghdr`` has reftype ``base`` so the fields of [dmmeta.Usertracefld](/txt/ssimdb/dmmeta/usertracefld.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FUsertracefld { // amc.FUsertracefld
    algo::Smallstr100   tracefld;   //
    algo::cstring       comment;    //
private:
    // func:amc.FUsertracefld..Ctor
    inline               FUsertracefld() __attribute__((nothrow));
    friend amc::FUsertracefld&  usertracefld_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FUsertracefld*  usertracefld_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 usertracefld_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FXref - Specify how to cross-reference (i.e. project, or group-by) one record with another
<a href="#amc-fxref"></a>

#### amc.FXref Fields
<a href="#amc-fxref-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FXref.msghdr|[dmmeta.Xref](/txt/ssimdb/dmmeta/xref.md)|[Base](/txt/ssimdb/dmmeta/xref.md)|||
|amc.FXref.p_field|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)||Xref field|
|amc.FXref.p_ctype|[amc.FCtype](/txt/gen/amc/amc.md#amc-fctype)|[Upptr](/txt/exe/amc/reftype.md#upptr)||p_field->p_ctype|
|amc.FXref.c_nocascdel|[amc.FNocascdel](/txt/gen/amc/amc.md#amc-fnocascdel)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|amc.FXref.p_viafld|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|amc.FXref.p_keyfld|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||

#### Struct FXref
<a href="#struct-fxref"></a>
*Note:* field ``amc.FXref.msghdr`` has reftype ``base`` so the fields of [dmmeta.Xref](/txt/ssimdb/dmmeta/xref.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FXref { // amc.FXref
    amc::FXref*         ctype_zs_xref_next;          // zslist link; -1 means not-in-list
    amc::FXref*         ind_xref_next;               // hash next
    u32                 ind_xref_hashval;            // hash value
    amc::FXref*         field_zd_xref_keyfld_next;   // zslist link; -1 means not-in-list
    amc::FXref*         field_zd_xref_keyfld_prev;   // previous element
    algo::Smallstr150   field;                       //
    algo::CppExpr       inscond;                     //   "true"  Insert condition
    algo::Smallstr200   via;                         //
    amc::FField*        p_field;                     // reference to parent row
    amc::FCtype*        p_ctype;                     // reference to parent row
    amc::FNocascdel*    c_nocascdel;                 // optional pointer
    amc::FField*        p_viafld;                    // reference to parent row
    amc::FField*        p_keyfld;                    // reference to parent row
    // x-reference on amc.FXref.p_field prevents copy
    // x-reference on amc.FXref.c_nocascdel prevents copy
    // x-reference on amc.FXref.p_viafld prevents copy
    // x-reference on amc.FXref.p_keyfld prevents copy
    // func:amc.FXref..AssignOp
    amc::FXref&          operator =(const amc::FXref &rhs) = delete;
    // x-reference on amc.FXref.p_field prevents copy
    // x-reference on amc.FXref.c_nocascdel prevents copy
    // x-reference on amc.FXref.p_viafld prevents copy
    // x-reference on amc.FXref.p_keyfld prevents copy
    // func:amc.FXref..CopyCtor
    FXref(const amc::FXref &rhs) = delete;
private:
    // func:amc.FXref..Ctor
    inline               FXref() __attribute__((nothrow));
    // func:amc.FXref..Dtor
    inline               ~FXref() __attribute__((nothrow));
    friend amc::FXref&          xref_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FXref*          xref_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 xref_RemoveLast() __attribute__((nothrow));
};
```

#### amc.Funcarg
<a href="#amc-funcarg"></a>

#### amc.Funcarg Fields
<a href="#amc-funcarg-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.Funcarg.type|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||C++ expression for declaration|
|amc.Funcarg.name|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||Argument name|
|amc.Funcarg.dflt|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||Initializer|
|amc.Funcarg.retval|bool|[Val](/txt/exe/amc/reftype.md#val)||Is return value?|
|amc.Funcarg.initialized|bool|[Val](/txt/exe/amc/reftype.md#val)|||
|amc.Funcarg.indent|bool|[Val](/txt/exe/amc/reftype.md#val)||Formatting option for prototype|
|amc.Funcarg.typearg|bool|[Val](/txt/exe/amc/reftype.md#val)|||

#### Struct Funcarg
<a href="#struct-funcarg"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct Funcarg { // amc.Funcarg
    algo::cstring   type;          // C++ expression for declaration
    algo::cstring   name;          // Argument name
    algo::cstring   dflt;          // Initializer
    bool            retval;        //   false  Is return value?
    bool            initialized;   //   false
    bool            indent;        //   false  Formatting option for prototype
    bool            typearg;       //   false
    // func:amc.Funcarg..Ctor
    inline               Funcarg() __attribute__((nothrow));
};
```

#### amc.Genpnew
<a href="#amc-genpnew"></a>

#### amc.Genpnew Fields
<a href="#amc-genpnew-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.Genpnew.p_func|[amc.FFunc](/txt/gen/amc/amc.md#amc-ffunc)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|amc.Genpnew.R|[algo_lib.Replscope](/txt/gen/algo_lib/algo_lib.md#algo_lib-replscope)|[Val](/txt/exe/amc/reftype.md#val)|||
|amc.Genpnew.preamble|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|||
|amc.Genpnew.postamble|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|||
|amc.Genpnew.req_pack|bool|[Val](/txt/exe/amc/reftype.md#val)|||
|amc.Genpnew.p_ctype|[amc.FCtype](/txt/gen/amc/amc.md#amc-fctype)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|amc.Genpnew.p_optfld|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|amc.Genpnew.p_varlenfld|[amc.FField](/txt/gen/amc/amc.md#amc-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|amc.Genpnew.optnolen|bool|[Val](/txt/exe/amc/reftype.md#val)|||
|amc.Genpnew.hasret|bool|[Val](/txt/exe/amc/reftype.md#val)|true||

#### Struct Genpnew
<a href="#struct-genpnew"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct Genpnew { // amc.Genpnew
    amc::FFunc*           p_func;        // reference to parent row
    algo_lib::Replscope   R;             //
    algo::cstring         preamble;      //
    algo::cstring         postamble;     //
    bool                  req_pack;      //   false
    amc::FCtype*          p_ctype;       // reference to parent row
    amc::FField*          p_optfld;      // reference to parent row
    amc::FField*          p_varlenfld;   // reference to parent row
    bool                  optnolen;      //   false
    bool                  hasret;        //   true
    // func:amc.Genpnew..Ctor
    inline               Genpnew() __attribute__((nothrow));
};
```

#### amc.Pnewtype
<a href="#amc-pnewtype"></a>

#### amc.Pnewtype Fields
<a href="#amc-pnewtype-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.Pnewtype.value|u8|[Val](/txt/exe/amc/reftype.md#val)|||

#### Struct Pnewtype
<a href="#struct-pnewtype"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct Pnewtype { // amc.Pnewtype
    u8   value;   //   0
    // func:amc.Pnewtype.value.Cast
    inline               operator amc_PnewtypeEnum() const __attribute__((nothrow));
    // func:amc.Pnewtype..Ctor
    inline               Pnewtype() __attribute__((nothrow));
    // func:amc.Pnewtype..FieldwiseCtor
    explicit inline               Pnewtype(u8 in_value) __attribute__((nothrow));
    // func:amc.Pnewtype..EnumCtor
    inline               Pnewtype(amc_PnewtypeEnum arg) __attribute__((nothrow));
};
```

### Tests
<a href="#tests"></a>
The following component tests are defined for `amc`.
These can be executed with `atf_comp <comptest> -v`
|Comptest|Comment|
|---|---|
|[amc.ArgvAccessor](/test/atf_comp/amc.ArgvAccessor)|which argv accessors each of the two argv strfmts generates on its print and read side|
|[amc.ArgvField](/test/atf_comp/amc.ArgvField)|Which field of a command ctype draws an argv token, and through which accessor|
|[amc.ArgvGlobal](/test/atf_comp/amc.ArgvGlobal)|PrintArgv/ToArgv call a field's own Print with the parent argument collapsed for a global|
|[amc.ArgvGnu](/test/atf_comp/amc.ArgvGnu)|ArgvGnu tokens: -x for a one-char name, --name plus a separate value|
|[amc.BadArgvRead](/test/atf_comp/amc.BadArgvRead)|the argv reader and its companions are emitted under gates that must agree|
|[amc.BadBigendDeadend](/test/atf_comp/amc.BadBigendDeadend)|fbigend on a type whose bltin row withholds bigendok is rejected|
|[amc.BadBigendNosize](/test/atf_comp/amc.BadBigendNosize)|fbigend type missing its csize row: diagnostic names the row|
|[amc.BadBigendReftype](/test/atf_comp/amc.BadBigendReftype)|non-Val fbigend rejection accumulates; run continues to exit|
|[amc.BadBigendU128](/test/atf_comp/amc.BadBigendU128)|No byteswap primitive beyond 64 bits; u128 fbigend rejected|
|[amc.BadBitfldReftype](/test/atf_comp/amc.BadBitfldReftype)|Reftype Bitfld and the dmmeta.bitfld record must imply each other|
|[amc.BadBitsetElem](/test/atf_comp/amc.BadBitsetElem)|Bitset element must be an unsigned integer builtin|
|[amc.BadBitsetNosize](/test/atf_comp/amc.BadBitsetNosize)|Bitset element missing its csize row: diagnostic names the row|
|[amc.BadBitsetSigned](/test/atf_comp/amc.BadBitsetSigned)|Signed bitset element compiles but sign-extends; rejected|
|[amc.BadBitsetWidth](/test/atf_comp/amc.BadBitsetWidth)|Bitset element width must be a power of two the accessors can index|
|[amc.BadCascdelNopool](/test/atf_comp/amc.BadCascdelNopool)|cascdel of a ctype with no instance pool is rejected: nothing can delete the rows|
|[amc.BadCascdelXref](/test/atf_comp/amc.BadCascdelXref)|cascdel Ptrary/Llist deletes until empty; requires the field to be an xref|
|[amc.BadCcmpGlobal](/test/atf_comp/amc.BadCcmpGlobal)|ccmp on a global ctype is rejected: one instance, nothing to compare|
|[amc.BadCfmtPrint](/test/atf_comp/amc.BadCfmtPrint)|a print:Y cfmt with an unsupported strfmt would ship an empty Print body|
|[amc.BadCfmtRead](/test/atf_comp/amc.BadCfmtRead)|a read:Y cfmt with no read path would ship an empty reader that reports success|
|[amc.BadChashGlobal](/test/atf_comp/amc.BadChashGlobal)|chash on a global ctype is rejected: one instance, nothing to hash|
|[amc.BadCompactSep](/test/atf_comp/amc.BadCompactSep)|printfmt:CompactSep has neither a printer nor a reader: both emitters reject it|
|[amc.BadCopyctorInit](/test/atf_comp/amc.BadCopyctorInit)|Init body naming the record in an uninlinable shape is rejected|
|[amc.BadCsizeAlignment](/test/atf_comp/amc.BadCsizeAlignment)|csize alignment must be a power of two, at most 16|
|[amc.BadFbufCondGlobal](/test/atf_comp/amc.BadFbufCondGlobal)|a buffer condition on a global fbuf has no row to insert|
|[amc.BadFbufElem](/test/atf_comp/amc.BadFbufElem)|Bytebuf/Linebuf take a one-byte element; their length is a byte count|
|[amc.BadFcmpGlobal](/test/atf_comp/amc.BadFcmpGlobal)|fcmp on a global ctype's field is rejected: one instance, nothing to compare|
|[amc.BadFcondIns](/test/atf_comp/amc.BadFcondIns)|a condition ins index must be an Llist or Bheap: one row in, one row out|
|[amc.BadFconstRange](/test/atf_comp/amc.BadFconstRange)|an fconst value the field's store cannot hold, over every end of the rule|
|[amc.BadFdecBitwidth](/test/atf_comp/amc.BadFdecBitwidth)|a zero Bitfld width bounds no decimal range|
|[amc.BadFdecBitwidthNeg](/test/atf_comp/amc.BadFdecBitwidthNeg)|a negative Bitfld width bounds no decimal range|
|[amc.BadFdecNplace](/test/atf_comp/amc.BadFdecNplace)|fdec nplace with no consistent decimal arithmetic, and an arg holding no decimal place|
|[amc.BadFstepAtree](/test/atf_comp/amc.BadFstepAtree)|TimeHookRecur fstep on an Atree would compile and never fire|
|[amc.BadFstepBheap](/test/atf_comp/amc.BadFstepBheap)|InlineOnce/TimeHookOnce fstep requires a Bheap field|
|[amc.BadFstepFdelay](/test/atf_comp/amc.BadFstepFdelay)|an fdelay row applies only to a recurring fstep|
|[amc.BadFstepFirst](/test/atf_comp/amc.BadFstepFirst)|TimeHookRecur fstep requires a First-generating step field|
|[amc.BadFstepGlobal](/test/atf_comp/amc.BadFstepGlobal)|fstep field must be a global (FDb) field|
|[amc.BadFstepInlary](/test/atf_comp/amc.BadFstepInlary)|an fstep on a fixed Inlary has no emptiness test|
|[amc.BadFstepReftype](/test/atf_comp/amc.BadFstepReftype)|an fstep field must offer an emptiness test for the step loop condition|
|[amc.BadFstepScale](/test/atf_comp/amc.BadFstepScale)|fdelay scale:Y requires a counted step field|
|[amc.BadFstepZslistmt](/test/atf_comp/amc.BadFstepZslistmt)|a ZSListMT fstep requires steptype Inline or InlineRecur|
|[amc.BadGconstBadline](/test/atf_comp/amc.BadGconstBadline)|a gconst value table line that is not a tuple reports the file and line|
|[amc.BadGconstChar](/test/atf_comp/amc.BadGconstChar)|a gconst char value must be a single character; offenders are reported, the valid row still emits|
|[amc.BadGconstCtype](/test/atf_comp/amc.BadGconstCtype)|gconst idfld must be a field of the namefld ctype|
|[amc.BadGconstCtypeChar](/test/atf_comp/amc.BadGconstCtypeChar)|one run reports every bad gconst idfld; a char-typed gconst does not abort the scan|
|[amc.BadGconstDup](/test/atf_comp/amc.BadGconstDup)|duplicate name in a gconst value table is an error|
|[amc.BadGconstHeadonly](/test/atf_comp/amc.BadGconstHeadonly)|head-only line in a gconst value table is an error, not a skipped index|
|[amc.BadGconstIdfld](/test/atf_comp/amc.BadGconstIdfld)|gconst idfld must resolve to a known field|
|[amc.BadGlobalInst](/test/atf_comp/amc.BadGlobalInst)|a Global instance must be the ctype's only instance|
|[amc.BadInfinityPool](/test/atf_comp/amc.BadInfinityPool)|a varlen ctype instanced in a fixed-slot pool is rejected|
|[amc.BadInlaryFnoremove](/test/atf_comp/amc.BadInlaryFnoremove)|a variable inlary needs the RemoveAll that fnoremove suppresses|
|[amc.BadInlaryMin](/test/atf_comp/amc.BadInlaryMin)|a variable char/u8 inlary cannot have min>0|
|[amc.BadInlaryMinmax](/test/atf_comp/amc.BadInlaryMinmax)|an inlary with min>max cannot preallocate its floor|
|[amc.BadJsonFld](/test/atf_comp/amc.BadJsonFld)|a Json cfmt field no FmtJson branch covers is rejected, not silently dropped|
|[amc.BadJstypeWire](/test/atf_comp/amc.BadJstypeWire)|every field of a packed jstype ctype must have a TypeScript wire form|
|[amc.BadLenfldBitfld](/test/atf_comp/amc.BadLenfldBitfld)|a Bitfld lenfld stores masked to its width; range checks use the width-limited max|
|[amc.BadLenfldExtra](/test/atf_comp/amc.BadLenfldExtra)|a positive extra beyond the lenfld type range can frame no message|
|[amc.BadLenfldJstype](/test/atf_comp/amc.BadLenfldJstype)|the TS Encode is a store site; size+extra must fit the lenfld range|
|[amc.BadLenfldMinFrame](/test/atf_comp/amc.BadLenfldMinFrame)|a store carrier's fixed size + extra must fit the lenfld range|
|[amc.BadLenfldMsgtype](/test/atf_comp/amc.BadLenfldMsgtype)|a message ctype stores its length; size+extra must divide by scale|
|[amc.BadLenfldPnew](/test/atf_comp/amc.BadLenfldPnew)|a fixed-only pnew ctype stores its fixed size as the total; size+extra must divide by scale|
|[amc.BadLenfldScale](/test/atf_comp/amc.BadLenfldScale)|Pooled ctype fixed size + extra must be a multiple of lenfld scale|
|[amc.BadLenfldType](/test/atf_comp/amc.BadLenfldType)|a lenfld arg must resolve to an integer type with a numeric range|
|[amc.BadLenfldZeroScale](/test/atf_comp/amc.BadLenfldZeroScale)|all scale:0 lenflds are reported in one run|
|[amc.BadMinmax](/test/atf_comp/amc.BadMinmax)|ccmp minmax:Y requires order:Y and genop:Y or a builtin|
|[amc.BadMissingTcurs](/test/atf_comp/amc.BadMissingTcurs)|a requested cursor whose amcdb.tcurs row is absent is named, not a Replscope throw|
|[amc.BadNumstrBase](/test/atf_comp/amc.BadNumstrBase)|numstr base must be 2..36, 95, or 256|
|[amc.BadNumstrMinlen](/test/atf_comp/amc.BadNumstrMinlen)|numstr min_len must fit the string, the pad budget, and leave the sign a slot|
|[amc.BadNumstrNumtype](/test/atf_comp/amc.BadNumstrNumtype)|numstr numtype and fdec arg must resolve to an integer bltin|
|[amc.BadNumstrPad](/test/atf_comp/amc.BadNumstrPad)|numstr pad must not read back as a digit of the base|
|[amc.BadNumstrSignedBase](/test/atf_comp/amc.BadNumstrSignedBase)|signed numstr cannot use base 95 or 256, where '-' is a digit|
|[amc.BadOptDtor](/test/atf_comp/amc.BadOptDtor)|a Varlen/Opt element type that owns a destructor is refused at generation time|
|[amc.BadPbufArg](/test/atf_comp/amc.BadPbufArg)|fpbuf arg and reftype must match what the pb_type codec compiles against|
|[amc.BadPbufFieldNumber](/test/atf_comp/amc.BadPbufFieldNumber)|fpbuf field numbers: no duplicates within a ctype, in [1, 2^29-1]|
|[amc.BadPbufNoCodec](/test/atf_comp/amc.BadPbufNoCodec)|an fpbuf row must produce wire code: cpbuf on the ctype, no Base field|
|[amc.BadPbufOneof](/test/atf_comp/amc.BadPbufOneof)|the variants of a oneof must share one presence mask|
|[amc.BadPbufPacked](/test/atf_comp/amc.BadPbufPacked)|fpbuf packed applies only to a repeated field of a varint or fixed pb_type|
|[amc.BadPbufStore](/test/atf_comp/amc.BadPbufStore)|an fpbuf singular field must have a Set the decoder can store through|
|[amc.BadPbufSyntax](/test/atf_comp/amc.BadPbufSyntax)|cpbuf syntax must name a dmmeta.pbsyntax row; proto2 and proto3 emit different presence guards|
|[amc.BadPbufType](/test/atf_comp/amc.BadPbufType)|an fpbuf pb_type must name an amcdb.pbtype row|
|[amc.BadPmaskMember](/test/atf_comp/amc.BadPmaskMember)|an explicit pmask member row must name a bit-capable field of the pmask's own ctype|
|[amc.BadPmaskWidth](/test/atf_comp/amc.BadPmaskWidth)|pmask member count must fit the pmask field bit width|
|[amc.BadSizeCycle](/test/atf_comp/amc.BadSizeCycle)|By-value ctype cycle reports every defect it finds, and does not hang|
|[amc.BadSizeOverflow](/test/atf_comp/amc.BadSizeOverflow)|Inlary total size beyond i32 is a clean error|
|[amc.BadSmallstrToobig](/test/atf_comp/amc.BadSmallstrToobig)|rpascal smallstr length must fit the one-byte length count|
|[amc.BadVarlenLast](/test/atf_comp/amc.BadVarlenLast)|a fixed field after a varlen field is rejected: varlen writes would clobber it|
|[amc.BadVarlenOpt](/test/atf_comp/amc.BadVarlenOpt)|a ctype may claim the end of its fixed portion once; a second claim is rejected|
|[amc.BitfldReadRange](/test/atf_comp/amc.BitfldReadRange)|which bitfield reads carry a store-range condition, by arg type and width|
|[amc.CopyctorInit](/test/atf_comp/amc.CopyctorInit)|the copy constructor inlines a field Init macro, retargeting the record it names|
|[amc.CursUnrequested](/test/atf_comp/amc.CursUnrequested)|a cursor tfunc without its tcurs row generates only on an fcurs request|
|[amc.DfltRetarget](/test/atf_comp/amc.DfltRetarget)|*this in a field default retargets; a this-prefixed identifier survives|
|[amc.DispfilterFieldPrint](/test/atf_comp/amc.DispfilterFieldPrint)|dispatch-filter regx match prints a field through its own Print function|
|[amc.DispfilterMatchAll](/test/atf_comp/amc.DispfilterMatchAll)|match_all dispatch filter builds every message-side expression from the message field|
|[amc.EditFail](/test/atf_comp/amc.EditFail)|a failed acr -e fails the amc run before codegen|
|[amc.FastPmaskName](/test/atf_comp/amc.FastPmaskName)|the FAST codec names every presence accessor from the pmask row, not from the word pmask|
|[amc.FconstBitfldWidth](/test/atf_comp/amc.FconstBitfldWidth)|the bitfield widths an fconst store range is computed from|
|[amc.FconstGlobal](/test/atf_comp/amc.FconstGlobal)|fconst accessors of a global ctype's field call each other parentless|
|[amc.FdecGetScale](/test/atf_comp/amc.FdecGetScale)|ctype-named GetScale only for a single-fdec ctype|
|[amc.FdecGlobalBitfld](/test/atf_comp/amc.FdecGlobalBitfld)|fdec accessors of a global bitfld field take no parent argument|
|[amc.FstepScaleBlkhash](/test/atf_comp/amc.FstepScaleBlkhash)|fdelay scale:Y on a Blkhash or Lary step field generates cleanly|
|[amc.GconstIndir](/test/atf_comp/amc.GconstIndir)|gconst value data side-loads from the -in_dir root|
|[amc.GconstLoadFail](/test/atf_comp/amc.GconstLoadFail)|a gconst value table that fails to load reports the file and fails the run|
|[amc.GstaticBadLine](/test/atf_comp/amc.GstaticBadLine)|a gstatic input line that is not a tuple reports the file and line|
|[amc.GstaticLoadFail](/test/atf_comp/amc.GstaticLoadFail)|a gstatic table that fails to load reports the file and fails the run|
|[amc.GsymbolBadline](/test/atf_comp/amc.GsymbolBadline)|a gsymbol table line that is not a tuple reports the file and line|
|[amc.GsymbolLoadFail](/test/atf_comp/amc.GsymbolLoadFail)|a gsymbol table that fails to load reports the file and fails the run|
|[amc.GsymbolSideload](/test/atf_comp/amc.GsymbolSideload)|gsymbol side-load from the data/ fallback root prints the amc.sideload notice|
|[amc.JsFixedFrame](/test/atf_comp/amc.JsFixedFrame)|a fixed-size frame length is a literal: no lenfld range guard is emitted|
|[amc.JsonAry](/test/atf_comp/amc.JsonAry)|how a JSON field renders: by its type Json cfmt, as a string, as an array, or as the record itself|
|[amc.JsonBaseBitfld](/test/atf_comp/amc.JsonBaseBitfld)|Base and Bitfld fields pass the JSON-cfmt field check without printing|
|[amc.JsonFld](/test/atf_comp/amc.JsonFld)|How each field of a Json cfmt reaches its node: FmtJson, string print, or an array of either|
|[amc.JsonGlobal](/test/atf_comp/amc.JsonGlobal)|a Global field carries no data and passes the Json-cfmt field check|
|[amc.KafkaBitfld](/test/atf_comp/amc.KafkaBitfld)|kafka codec routes bitfld access through Get/Set accessors|
|[amc.KafkaPmaskName](/test/atf_comp/amc.KafkaPmaskName)|kafka codec presence checks use the pmask-derived accessor name|
|[amc.LenfldBitfld](/test/atf_comp/amc.LenfldBitfld)|a Bitfld lenfld has no TS store site; the schema stays legal|
|[amc.LenfldNarrow](/test/atf_comp/amc.LenfldNarrow)|C++ stores of a runtime total guard the lenfld range like the TS encoder|
|[amc.MinmaxNative](/test/atf_comp/amc.MinmaxNative)|ccmp minmax:Y on a builtin or extrn:Y ctype generates cleanly|
|[amc.MissingLlist](/test/atf_comp/amc.MissingLlist)|Llist field without dmmeta.llist record is a clean error|
|[amc.MissingPack](/test/atf_comp/amc.MissingPack)|a packed message with an unpacked base header names the pack row to insert|
|[amc.MissingPtrary](/test/atf_comp/amc.MissingPtrary)|Ptrary field without dmmeta.ptrary record is a clean error|
|[amc.NumstrGlobal](/test/atf_comp/amc.NumstrGlobal)|numstr on a global: Geti64 and GetnumDflt call Getnum parentless|
|[amc.OptGlobalPrint](/test/atf_comp/amc.OptGlobalPrint)|Opt on a global: infinity_pool error, no bare (void); in Print|
|[amc.OutfileWriteFail](/test/atf_comp/amc.OutfileWriteFail)|a failed output-file write fails the run naming the path|
|[amc.PbufBitfld](/test/atf_comp/amc.PbufBitfld)|pbuf codec routes bitfld access through Get/Set accessors|
|[amc.PbufBitfldNondflt](/test/atf_comp/amc.PbufBitfldNondflt)|pbuf codec guards a no-pmask bitfld with the proto3 default-value check|
|[amc.PbufPmaskName](/test/atf_comp/amc.PbufPmaskName)|pbuf codec presence guard uses the pmask-derived accessor name|
|[amc.PbufRepeatedPmask](/test/atf_comp/amc.PbufRepeatedPmask)|decode of a repeated pmask member marks the field present|
|[amc.PbufStore](/test/atf_comp/amc.PbufStore)|pbuf decode store path: in place, temporary, or Alloc, per field shape|
|[amc.PmaskGiantField](/test/atf_comp/amc.PmaskGiantField)|A pmask field wider than i32 bits is legal; bit bookkeeping must not wrap|
|[amc.PmaskGlobal](/test/atf_comp/amc.PmaskGlobal)|pmask on a global: bitset accessors and print filter take no parent|
|[amc.PoolInsertScale](/test/atf_comp/amc.PoolInsertScale)|InsertMaybe validates an untrusted length word in i64 domain; no mod-2^N wrap|
|[amc.PoolVarlenExtern](/test/atf_comp/amc.PoolVarlenExtern)|Delete of a varlen pool arg without lenfld: extern count, well-formed length statement|
|[amc.PrintGlobalTuple](/test/atf_comp/amc.PrintGlobalTuple)|Tuple print of a global ctype reads fields through the row argument|
|[amc.QueryNocpp](/test/atf_comp/amc.QueryNocpp)|A match in a namespace with no C++ output reports amc.query_nocpp and exits nonzero|
|[amc.ReadGlobalTuple](/test/atf_comp/amc.ReadGlobalTuple)|Tuple read of a global ctype: prefixed dispatcher, parentless field readers|
|[amc.SchemaClosure](/test/atf_comp/amc.SchemaClosure)|amc generates every namespace from its own schema closure; too heavy under valgrind|
|[amc.SideloadNossimfile](/test/atf_comp/amc.SideloadNossimfile)|a gconst and a gstatic whose ctype reaches no ssimfile at all|
|[amc.StdinUniverse](/test/atf_comp/amc.StdinUniverse)|universe piped in with -in_dir:- and its default side-load root|
|[amc.TableWriteAcrFail](/test/atf_comp/amc.TableWriteAcrFail)|a failed table-write acr fails the run|

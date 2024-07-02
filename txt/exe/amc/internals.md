## amc - Internals


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Toc -->
* [Description](#description)
* [Sources](#sources)
* [Dependencies](#dependencies)
* [In Memory DB](#in-memory-db)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Toc -->

### Description
<a href="#description"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Description -->
for usage, see [amc - Algo Model Compiler: generate code under include/gen and cpp/gen](/txt/exe/amc/README.md)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Description -->

### Sources
<a href="#sources"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Sources -->
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
|[cpp/amc/blkpool.cpp](/cpp/amc/blkpool.cpp)|Block pool|
|[cpp/amc/cget.cpp](/cpp/amc/cget.cpp)|Getters / Setters|
|[cpp/amc/charset.cpp](/cpp/amc/charset.cpp)|Charset functions|
|[cpp/amc/checkxref.cpp](/cpp/amc/checkxref.cpp)|X-reference checker|
|[cpp/amc/cmp.cpp](/cpp/amc/cmp.cpp)|Comparison functions|
|[cpp/amc/concat.cpp](/cpp/amc/concat.cpp)|Pkey constructor|
|[cpp/amc/count.cpp](/cpp/amc/count.cpp)|Count reftype|
|[cpp/amc/cppfunc.cpp](/cpp/amc/cppfunc.cpp)|Cppfunc reftype|
|[cpp/amc/ctype.cpp](/cpp/amc/ctype.cpp)|Ctype code generators|
|[cpp/amc/delptr.cpp](/cpp/amc/delptr.cpp)|Delptr reftype|
|[cpp/amc/dflt.cpp](/cpp/amc/dflt.cpp)|Field default|
|[cpp/amc/disp/call.cpp](/cpp/amc/disp/call.cpp)|Dispatch call|
|[cpp/amc/disp/casetype.cpp](/cpp/amc/disp/casetype.cpp)|Dispatch casetype generator|
|[cpp/amc/disp/filter.cpp](/cpp/amc/disp/filter.cpp)|Dispatch filter|
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
|[cpp/amc/fconst.cpp](/cpp/amc/fconst.cpp)|Constants, string <-> int conversion|
|[cpp/amc/fdec.cpp](/cpp/amc/fdec.cpp)|Decimal types|
|[cpp/amc/field.cpp](/cpp/amc/field.cpp)|Generic field genrator|
|[cpp/amc/func.cpp](/cpp/amc/func.cpp)|C++ function output|
|[cpp/amc/fwddecl.cpp](/cpp/amc/fwddecl.cpp)|Forward declarations|
|[cpp/amc/gen.cpp](/cpp/amc/gen.cpp)|AMC Generators (amcdb.gen)|
|[cpp/amc/global.cpp](/cpp/amc/global.cpp)|Global (FDb)|
|[cpp/amc/gsymbol.cpp](/cpp/amc/gsymbol.cpp)|Generate strings from tables|
|[cpp/amc/hook.cpp](/cpp/amc/hook.cpp)|Hook (function pointer)|
|[cpp/amc/include.cpp](/cpp/amc/include.cpp)|Manage includes for generated files|
|[cpp/amc/inlary.cpp](/cpp/amc/inlary.cpp)|Inline array|
|[cpp/amc/io.cpp](/cpp/amc/io.cpp)|I/O functions|
|[cpp/amc/lary.cpp](/cpp/amc/lary.cpp)|Level array with permanent pointers|
|[cpp/amc/llist.cpp](/cpp/amc/llist.cpp)|Linked lists|
|[cpp/amc/lpool.cpp](/cpp/amc/lpool.cpp)|Variable-length free pool|
|[cpp/amc/main.cpp](/cpp/amc/main.cpp)|Main driver|
|[cpp/amc/malloc.cpp](/cpp/amc/malloc.cpp)|Malloc allocator|
|[cpp/amc/msgcurs.cpp](/cpp/amc/msgcurs.cpp)|Message scanning cursor|
|[cpp/amc/nstype.cpp](/cpp/amc/nstype.cpp)|Namespace types|
|[cpp/amc/numstr.cpp](/cpp/amc/numstr.cpp)|Small strings|
|[cpp/amc/opt.cpp](/cpp/amc/opt.cpp)|Opt reftype|
|[cpp/amc/outfile.cpp](/cpp/amc/outfile.cpp)|Ouptut functions|
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

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Sources -->

### Dependencies
<a href="#dependencies"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Dependencies -->
The build target depends on the following libraries
|Target|Comment|
|---|---|
|[algo_lib](/txt/lib/algo_lib/README.md)|Support library for all executables|
|[lib_amcdb](/txt/lib/lib_amcdb/README.md)|Library used by amc|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Dependencies -->

### In Memory DB
<a href="#in-memory-db"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Imdb -->
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
|[amc.FBltin](#amc-fbltin)|[amcdb.bltin](/txt/ssimdb/amcdb/bltin.md)|FDb.bltin (Lary)|bltin (Lary, by rowid)|ind_bltin (Thash, hash field ctype)|
||||FCtype.c_bltin (Ptr)|
|[amc.FCafter](#amc-fcafter)|[dmmeta.cafter](/txt/ssimdb/dmmeta/cafter.md)|FDb.cafter (Lary)|cafter (Lary, by rowid)|
||||FCtype.zd_cafter (Llist)|
|[amc.FCascdel](#amc-fcascdel)|[dmmeta.cascdel](/txt/ssimdb/dmmeta/cascdel.md)|FDb.cascdel (Lary)|cascdel (Lary, by rowid)|
||||FField.c_cascdel (Ptr)|
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
|[amc.FCtypelen](#amc-fctypelen)|[dmmeta.ctypelen](/txt/ssimdb/dmmeta/ctypelen.md)|FDb.ctypelen (Lary)|ctypelen (Lary, by rowid)|c_ctypelen (Ptrary)|
||||FCtype.c_ctypelen (Ptr)|
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
|[amc.FDispsig](#amc-fdispsig)|[dmmeta.dispsig](/txt/ssimdb/dmmeta/dispsig.md)|FDb.dispsig (Lary)|dispsig (Lary, by rowid)|c_dispsig_sorted (Ptrary)|
||||FNs.c_dispsig (Ptrary)|
|[amc.FDisptrace](#amc-fdisptrace)|[dmmeta.disptrace](/txt/ssimdb/dmmeta/disptrace.md)|FDb.disptrace (Lary)|disptrace (Lary, by rowid)|
||||FDispatch.c_disptrace (Ptr)|
|[amc.FEnumstr](#amc-fenumstr)||FDb.enumstr (Lary)|enumstr (Lary, by rowid)|ind_enumstr (Thash, hash field enumstr)|
||||FEnumstrLen.bh_enumstr (Bheap)|
|[amc.FEnumstrLen](#amc-fenumstrlen)||FDb.enumstr_len (Lary)|enumstr_len (Lary, by rowid)|bh_enumstr_len (Bheap, sort field len)|ind_enumstr_len (Thash, hash field len)|
|[amc.FFalias](#amc-ffalias)|[dmmeta.falias](/txt/ssimdb/dmmeta/falias.md)|FDb.falias (Lary)|falias (Lary, by rowid)|
||||FField.c_falias (Ptr)|
|[amc.FFbase](#amc-ffbase)|[dmmeta.fbase](/txt/ssimdb/dmmeta/fbase.md)|FDb.fbase (Lary)|fbase (Lary, by rowid)|
||||FField.c_fbase (Ptr)|
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
|[amc.FFcleanup](#amc-ffcleanup)|[dmmeta.fcleanup](/txt/ssimdb/dmmeta/fcleanup.md)|FDb.fcleanup (Lary)|fcleanup (Lary, by rowid)|
||||FField.c_fcleanup (Ptr)|
|[amc.FFcmap](#amc-ffcmap)|[dmmeta.fcmap](/txt/ssimdb/dmmeta/fcmap.md)|FDb.fcmap (Lary)|fcmap (Lary, by rowid)|ind_fcmap (Thash, hash field fcmap)|
||||FField.zs_fcmap (Llist)|
|[amc.FFcmdline](#amc-ffcmdline)|[dmmeta.fcmdline](/txt/ssimdb/dmmeta/fcmdline.md)|FDb.fcmdline (Lary)|fcmdline (Lary, by rowid)|
||||FNs.c_fcmdline (Ptr)|
|[amc.FFcmp](#amc-ffcmp)|[dmmeta.fcmp](/txt/ssimdb/dmmeta/fcmp.md)|FDb.fcmp (Lary)|fcmp (Lary, by rowid)|
||||FField.c_fcmp (Ptr)|
|[amc.FFcompact](#amc-ffcompact)|[dmmeta.fcompact](/txt/ssimdb/dmmeta/fcompact.md)|FDb.fcompact (Lary)|fcompact (Lary, by rowid)|
||||FField.c_fcompact (Ptr)|
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
|[amc.FField](#amc-ffield)|[dmmeta.field](/txt/ssimdb/dmmeta/field.md)|FDb.field (Lary)|field (Lary, by rowid)|ind_field (Thash, hash field field)|c_malloc (Ptr)|zs_ordkeyfield (Llist)|c_tempfield (Ptrary)|
||||FBasepool.p_field (Upptr)|
||||FBasepool.p_base (Upptr)|
||||FBitfld.p_srcfield (Upptr)|
||||FBitfld.p_field (Upptr)|
||||FCascdel.p_field (Upptr)|
||||FCtype.c_field (Ptrary)|
||||FCtype.c_varlenfld (Ptr)|
||||FCtype.c_optfld (Ptr)|
||||FCtype.c_datafld (Ptrary)|
||||FCtype.zd_inst (Llist)|
||||FCtype.zd_access (Llist)|
||||FCtype.c_pkeyfield (Ptr)|
||||FFalias.p_srcfield (Upptr)|
||||FFbigend.p_field (Upptr)|
||||FFbitset.p_field (Upptr)|
||||FFbuf.p_insready (Upptr)|
||||FFbuf.p_inseof (Upptr)|
||||FFcleanup.p_field (Upptr)|
||||FFcmap.p_leftField (Upptr)|
||||FFcmap.p_rightField (Upptr)|
||||FFcmdline.p_field (Upptr)|
||||FFcmdline.p_basecmdline (Upptr)|
||||FFconst.p_field (Upptr)|
||||FFcurs.p_field (Upptr)|
||||FFdec.p_field (Upptr)|
||||FFfast.p_field (Upptr)|
||||FFinput.p_field (Upptr)|
||||FFldoffset.p_field (Upptr)|
||||FFloadtuples.p_field (Upptr)|
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
|[amc.FFindrem](#amc-ffindrem)|[dmmeta.findrem](/txt/ssimdb/dmmeta/findrem.md)|FDb.findrem (Lary)|findrem (Lary, by rowid)|
||||FField.c_findrem (Ptr)|
|[amc.FFinput](#amc-ffinput)|[dmmeta.finput](/txt/ssimdb/dmmeta/finput.md)|FDb.finput (Lary)|finput (Lary, by rowid)|
||||FField.c_finput (Ptr)|
|[amc.FFldoffset](#amc-ffldoffset)|[dmmeta.fldoffset](/txt/ssimdb/dmmeta/fldoffset.md)|FDb.fldoffset (Lary)|fldoffset (Lary, by rowid)|
||||FField.c_fldoffset (Ptr)|
|[amc.FFloadtuples](#amc-ffloadtuples)|[dmmeta.floadtuples](/txt/ssimdb/dmmeta/floadtuples.md)|FDb.floadtuples (Lary)|floadtuples (Lary, by rowid)|
||||FCtype.c_floadtuples (Ptr)|
|[amc.FFnoremove](#amc-ffnoremove)|[dmmeta.fnoremove](/txt/ssimdb/dmmeta/fnoremove.md)|FDb.fnoremove (Lary)|fnoremove (Lary, by rowid)|
||||FField.c_fnoremove (Ptr)|
|[amc.FFoutput](#amc-ffoutput)|[dmmeta.foutput](/txt/ssimdb/dmmeta/foutput.md)|FDb.foutput (Lary)|foutput (Lary, by rowid)|
||||FField.c_foutput (Ptr)|
||||FNs.c_foutput (Ptrary)|
|[amc.FFprefix](#amc-ffprefix)|[dmmeta.fprefix](/txt/ssimdb/dmmeta/fprefix.md)|FDb.fprefix (Lary)|fprefix (Lary, by rowid)|ind_prefix (Thash, hash field fprefix)|
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
|[amc.FFuserinit](#amc-ffuserinit)|[dmmeta.fuserinit](/txt/ssimdb/dmmeta/fuserinit.md)|FDb.fuserinit (Lary)|fuserinit (Lary, by rowid)|
||||FField.c_fuserinit (Ptr)|
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
|[amc.FLenfld](#amc-flenfld)|[dmmeta.lenfld](/txt/ssimdb/dmmeta/lenfld.md)|FDb.lenfld (Lary)|lenfld (Lary, by rowid)|
||||FCtype.c_lenfld (Ptr)|
|[amc.FLicense](#amc-flicense)|[dev.license](/txt/ssimdb/dev/license.md)|FDb.license (Lary)|license (Lary, by rowid)|ind_license (Thash, hash field license)|
||||FNs.p_license (Upptr)|
|[amc.FListtype](#amc-flisttype)|[dmmeta.listtype](/txt/ssimdb/dmmeta/listtype.md)|FDb.listtype (Lary)|listtype (Lary, by rowid)|ind_listtype (Thash, hash field listtype)|
||||FLlist.p_listtype (Upptr)|
|[amc.FLlist](#amc-fllist)|[dmmeta.llist](/txt/ssimdb/dmmeta/llist.md)|FDb.llist (Lary)|llist (Lary, by rowid)|
||||FField.c_llist (Ptr)|
|[amc.FMain](#amc-fmain)|[dmmeta.main](/txt/ssimdb/dmmeta/main.md)|FDb.main (Lary)|main (Lary, by rowid)|ind_main (Thash, hash field ns)|
||||FNs.c_main (Ptr)|
|[amc.FMsgtype](#amc-fmsgtype)|[dmmeta.msgtype](/txt/ssimdb/dmmeta/msgtype.md)|FDb.msgtype (Lary)|msgtype (Lary, by rowid)|
||||FCtype.c_msgtype (Ptr)|
|[amc.FNocascdel](#amc-fnocascdel)|[dmmeta.nocascdel](/txt/ssimdb/dmmeta/nocascdel.md)|FDb.nocascdel (Lary)|nocascdel (Lary, by rowid)|
||||FXref.c_nocascdel (Ptr)|
|[amc.FNossimfile](#amc-fnossimfile)|[dmmeta.nossimfile](/txt/ssimdb/dmmeta/nossimfile.md)|FDb.nossimfile (Lary)|nossimfile (Lary, by rowid)|
||||FCtype.c_nossimfile (Ptr)|
|[amc.FNoxref](#amc-fnoxref)|[dmmeta.noxref](/txt/ssimdb/dmmeta/noxref.md)|FDb.noxref (Lary)|noxref (Lary, by rowid)|
||||FField.c_noxref (Ptr)|
|[amc.FNs](#amc-fns)|[dmmeta.ns](/txt/ssimdb/dmmeta/ns.md)|FDb.ns (Lary)|ns (Lary, by rowid)|ind_ns (Thash, hash field ns)|c_ns_sorted (Ptrary)|c_curns (Ptr)|c_ns (Ptr)|
||||FCtype.p_ns (Upptr)|
||||FDispatch.p_ns (Upptr)|
||||FFcmdline.p_ns (Upptr)|
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
|[amc.FPmaskfld](#amc-fpmaskfld)|[dmmeta.pmaskfld](/txt/ssimdb/dmmeta/pmaskfld.md)|FDb.pmaskfld (Lary)|pmaskfld (Lary, by rowid)|ind_pmaskfld (Thash, hash field field)|
||||FCtype.c_pmaskfld (Ptrary)|
||||FField.c_pmaskfld (Ptr)|
||||FPmaskfldMember.p_pmaskfld (Upptr)|
|[amc.FPmaskfldMember](#amc-fpmaskfldmember)|[dmmeta.pmaskfld_member](/txt/ssimdb/dmmeta/pmaskfld_member.md)|FDb.pmaskfld_member (Lary)|pmaskfld_member (Lary, by rowid)|
||||FField.c_pmaskfld_member (Ptrary)|
||||FPmaskfld.c_pmaskfld_member (Ptrary)|
|[amc.FPnew](#amc-fpnew)|[dmmeta.pnew](/txt/ssimdb/dmmeta/pnew.md)|FDb.pnew (Lary)|pnew (Lary, by rowid)|ind_pnew (Thash, hash field pnew)|
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
|[amc.FUsertracefld](#amc-fusertracefld)|[dmmeta.usertracefld](/txt/ssimdb/dmmeta/usertracefld.md)|FDb.usertracefld (Lary)|usertracefld (Lary, by rowid)|
|[amc.FXref](#amc-fxref)|[dmmeta.xref](/txt/ssimdb/dmmeta/xref.md)|FDb.xref (Lary)|xref (Lary, by rowid)|ind_xref (Thash, hash field field)|
||||FCtype.zs_xref (Llist)|
||||FField.c_xref (Ptr)|
||||FField.zd_xref_keyfld (Llist)|
|[amc.Funcarg](#amc-funcarg)||FFunc.funcarg (Tary)|
|[amc.Genpnew](#amc-genpnew)||
|[amc.Pnewtype](#amc-pnewtype)||

#### amc.BltinId - 
<a href="#amc-bltinid"></a>

#### amc.BltinId Fields
<a href="#amc-bltinid-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.BltinId.value|u8|[Val](/txt/exe/amc/reftypes.md#val)|||

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

#### amc.CppkeywordId - 
<a href="#amc-cppkeywordid"></a>

#### amc.CppkeywordId Fields
<a href="#amc-cppkeywordid-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.CppkeywordId.id|u32|[Val](/txt/exe/amc/reftypes.md#val)|||

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
|amc.Enumstr.len|i32|[Val](/txt/exe/amc/reftypes.md#val)|||
|amc.Enumstr.str|[algo.RnullStr8](/txt/protocol/algo/README.md#algo-rnullstr8)|[Val](/txt/exe/amc/reftypes.md#val)|||

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
|amc.FAnonfld.anon_idx|i32|[Val](/txt/exe/amc/reftypes.md#val)|0|Index of anonymous field (if any)|

#### Struct FAnonfld
<a href="#struct-fanonfld"></a>
*Note:* field ``amc.FAnonfld.msghdr`` has reftype ``base`` so the fields of [dmmeta.Anonfld](/txt/ssimdb/dmmeta/anonfld.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FAnonfld { // amc.FAnonfld
    algo::Smallstr100   field;      //
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
    algo::Comment       comment;    //
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
|amc.FBasepool.p_field|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|amc.FBasepool.p_base|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FBasepool
<a href="#struct-fbasepool"></a>
*Note:* field ``amc.FBasepool.msghdr`` has reftype ``base`` so the fields of [dmmeta.Basepool](/txt/ssimdb/dmmeta/basepool.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FBasepool { // amc.FBasepool
    algo::Smallstr100   field;     //
    algo::Smallstr100   base;      //
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
|amc.FBitfld.p_srcfield|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|amc.FBitfld.p_field|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FBitfld
<a href="#struct-fbitfld"></a>
*Note:* field ``amc.FBitfld.msghdr`` has reftype ``base`` so the fields of [dmmeta.Bitfld](/txt/ssimdb/dmmeta/bitfld.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FBitfld { // amc.FBitfld
    algo::Smallstr100   field;           //
    i32                 offset;          //   0  Offset, in bits, within parent field
    i32                 width;           //   0  Width, in bits, within parent field.
    algo::Smallstr100   srcfield;        //
    amc::FField*        p_srcfield;      // reference to parent row
    amc::FField*        p_field;         // reference to parent row
    i32                 bh_bitfld_idx;   // index in heap; -1 means not-in-heap
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
    algo::Smallstr100   ctype;            //
    bool                likeu64;          //   false
    bool                bigendok;         //   false
    bool                issigned;         //   false
    amc::FBltin*        ind_bltin_next;   // hash next
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
|amc.FCafter.p_after|[amc.FCtype](/txt/exe/amc/internals.md#amc-fctype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FCafter
<a href="#struct-fcafter"></a>
*Note:* field ``amc.FCafter.msghdr`` has reftype ``base`` so the fields of [dmmeta.Cafter](/txt/ssimdb/dmmeta/cafter.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FCafter { // amc.FCafter
    algo::Smallstr50   cafter;           //
    amc::FCtype*       p_after;          // reference to parent row
    amc::FCafter*      zd_cafter_next;   // zslist link; -1 means not-in-list
    amc::FCafter*      zd_cafter_prev;   // previous element
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
|amc.FCascdel.p_field|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FCascdel
<a href="#struct-fcascdel"></a>
*Note:* field ``amc.FCascdel.msghdr`` has reftype ``base`` so the fields of [dmmeta.Cascdel](/txt/ssimdb/dmmeta/cascdel.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FCascdel { // amc.FCascdel
    algo::Smallstr100   field;     //
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
    algo::Smallstr100   ctype;           // Target ctype
    bool                extrn;           //   false  Whether implementation is external
    bool                genop;           //   false  Generate C++ comparison operators (<,>, etc)
    bool                order;           //   false
    bool                minmax;          //   false
    amc::FCcmp*         ind_ccmp_next;   // hash next
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
    algo::Smallstr50    jsdflt;     //
    algo::Comment       comment;    //
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
    algo::Comment       comment;    //
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

#### Struct FCfmt
<a href="#struct-fcfmt"></a>
*Note:* field ``amc.FCfmt.msghdr`` has reftype ``base`` so the fields of [dmmeta.Cfmt](/txt/ssimdb/dmmeta/cfmt.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FCfmt { // amc.FCfmt
    algo::Smallstr100   cfmt;            //
    algo::Smallstr50    printfmt;        //
    bool                read;            //   false
    bool                print;           //   false
    algo::Smallstr20    sep;             //
    bool                genop;           //   false
    amc::FCfmt*         zs_cfmt_next;    // zslist link; -1 means not-in-list
    amc::FCfmt*         ind_cfmt_next;   // hash next
    // func:amc.FCfmt..AssignOp
    inline amc::FCfmt&   operator =(const amc::FCfmt &rhs) = delete;
    // func:amc.FCfmt..CopyCtor
    inline               FCfmt(const amc::FCfmt &rhs) = delete;
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
|amc.FCget.p_ctype|[amc.FCtype](/txt/exe/amc/internals.md#amc-fctype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FCget
<a href="#struct-fcget"></a>
*Note:* field ``amc.FCget.base`` has reftype ``base`` so the fields of [dmmeta.Cget](/txt/ssimdb/dmmeta/cget.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FCget { // amc.FCget
    algo::Smallstr100   ctype;     //
    algo::Comment       comment;   //
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
|amc.FCharset.chars|[algo.Charset](/txt/protocol/algo/Charset.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FCharset
<a href="#struct-fcharset"></a>
*Note:* field ``amc.FCharset.base`` has reftype ``base`` so the fields of [dmmeta.Charset](/txt/ssimdb/dmmeta/charset.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FCharset { // amc.FCharset
    algo::Smallstr100   field;       //
    algo::Smallstr100   expr;        //
    bool                charrange;   //   false
    bool                calc;        //   false
    algo::Comment       comment;     //
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
    algo::Smallstr100   ctype;            // Target ctype
    algo::Smallstr50    hashtype;         // Hash type
    amc::FChash*        ind_chash_next;   // hash next
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
    algo::Smallstr100   field;   //
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

#### amc.FCpptype - Specify whether a ctype can be passed by value, and other c++ options
<a href="#amc-fcpptype"></a>

#### amc.FCpptype Fields
<a href="#amc-fcpptype-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FCpptype.msghdr|[dmmeta.Cpptype](/txt/ssimdb/dmmeta/cpptype.md)|[Base](/txt/ssimdb/dmmeta/cpptype.md)|||
|amc.FCpptype.p_ctype|[amc.FCtype](/txt/exe/amc/internals.md#amc-fctype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FCpptype
<a href="#struct-fcpptype"></a>
*Note:* field ``amc.FCpptype.msghdr`` has reftype ``base`` so the fields of [dmmeta.Cpptype](/txt/ssimdb/dmmeta/cpptype.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FCpptype { // amc.FCpptype
    algo::Smallstr100   ctype;              //
    bool                ctor;               //   false  if true, generate non-default constructor from all fields
    bool                dtor;               //   true  generate non-default destructor
    bool                cheap_copy;         //   false  Pass by value whenever possible
    amc::FCtype*        p_ctype;            // reference to parent row
    amc::FCpptype*      ind_cpptype_next;   // hash next
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
|amc.FCtype.zs_cfmt|[amc.FCfmt](/txt/exe/amc/internals.md#amc-fcfmt)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|amc.FCtype.cpp_type|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||type name to use in c++|
|amc.FCtype.body|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Struct contents|
|amc.FCtype.sizecheck|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Size check assertions|
|amc.FCtype.c_init|[amc.FFunc](/txt/exe/amc/internals.md#amc-ffunc)|[Ptr](/txt/exe/amc/reftypes.md#ptr)||Pointer to constructor|
|amc.FCtype.p_ns|[amc.FNs](/txt/exe/amc/internals.md#amc-fns)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|amc.FCtype.c_bltin|[amc.FBltin](/txt/exe/amc/internals.md#amc-fbltin)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FCtype.c_field|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|amc.FCtype.c_msgtype|[amc.FMsgtype](/txt/exe/amc/internals.md#amc-fmsgtype)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FCtype.c_varlenfld|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FCtype.c_optfld|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FCtype.c_static|[amc.FStatictuple](/txt/exe/amc/internals.md#amc-fstatictuple)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|amc.FCtype.c_cpptype|[amc.FCpptype](/txt/exe/amc/internals.md#amc-fcpptype)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FCtype.c_parent|[amc.FCtype](/txt/exe/amc/internals.md#amc-fctype)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|amc.FCtype.c_ssimfile|[amc.FSsimfile](/txt/exe/amc/internals.md#amc-fssimfile)|[Ptr](/txt/exe/amc/reftypes.md#ptr)||Ssimfile associated with this ctype|
|amc.FCtype.c_pack|[amc.FPack](/txt/exe/amc/internals.md#amc-fpack)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FCtype.c_lenfld|[amc.FLenfld](/txt/exe/amc/internals.md#amc-flenfld)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FCtype.c_pmaskfld|[amc.FPmaskfld](/txt/exe/amc/internals.md#amc-fpmaskfld)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)||List of pmask fields in this ctype|
|amc.FCtype.c_typefld|[amc.FTypefld](/txt/exe/amc/internals.md#amc-ftypefld)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FCtype.c_datafld|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)||Physical fields (non-fldfunc)|
|amc.FCtype.zd_inst|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Llist](/txt/exe/amc/reftypes.md#llist)||All the places where this ctype is instantiated (collected manually)|
|amc.FCtype.zs_xref|[amc.FXref](/txt/exe/amc/internals.md#amc-fxref)|[Llist](/txt/exe/amc/reftypes.md#llist)||List of xrefs where this ctype is a child (collected manually)|
|amc.FCtype.c_chash|[amc.FChash](/txt/exe/amc/internals.md#amc-fchash)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FCtype.c_ccmp|[amc.FCcmp](/txt/exe/amc/internals.md#amc-fccmp)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FCtype.c_cstr|[amc.FCstr](/txt/exe/amc/internals.md#amc-fcstr)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FCtype.c_cextern|[amc.FCextern](/txt/exe/amc/internals.md#amc-fcextern)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FCtype.signature|[algo.Sha1sig](/txt/protocol/algo/Sha1sig.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|amc.FCtype.c_ffunc|[amc.FFunc](/txt/exe/amc/internals.md#amc-ffunc)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|amc.FCtype.zd_cafter|[amc.FCafter](/txt/exe/amc/internals.md#amc-fcafter)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|amc.FCtype.c_csize|[amc.FCsize](/txt/exe/amc/internals.md#amc-fcsize)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FCtype.zd_access|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Llist](/txt/exe/amc/reftypes.md#llist)||All access paths to this field (via field.arg)|
|amc.FCtype.c_cdflt|[amc.FCdflt](/txt/exe/amc/internals.md#amc-fcdflt)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FCtype.c_argvtype|[amc.FArgvtype](/txt/exe/amc/internals.md#amc-fargvtype)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FCtype.c_floadtuples|[amc.FFloadtuples](/txt/exe/amc/internals.md#amc-ffloadtuples)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FCtype.c_pkeyfield|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Ptr](/txt/exe/amc/reftypes.md#ptr)||Field corresponding to pkey of this type|
|amc.FCtype.c_fcurs|[amc.FFcurs](/txt/exe/amc/internals.md#amc-ffcurs)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|amc.FCtype.copy_priv_reason|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|amc.FCtype.c_ctypelen|[amc.FCtypelen](/txt/exe/amc/internals.md#amc-fctypelen)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FCtype.size_unknown|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|amc.FCtype.copy_priv_valid|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|amc.FCtype.size_locked|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|amc.FCtype.topo_visited|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Temporary|
|amc.FCtype.enum_visited|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Temporary|
|amc.FCtype.fields_cloned|bool|[Val](/txt/exe/amc/reftypes.md#val)||True if fields from c_cbase have been cloned.|
|amc.FCtype.original|bool|[Val](/txt/exe/amc/reftypes.md#val)||True if this ctype comes from disk|
|amc.FCtype.copy_priv|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|disallow copy ctor / assign op|
|amc.FCtype.plaindata|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|amc.FCtype.alignment|u32|[Val](/txt/exe/amc/reftypes.md#val)|1||
|amc.FCtype.n_padbytes|i32|[Val](/txt/exe/amc/reftypes.md#val)|||
|amc.FCtype.totsize_byte|u32|[Val](/txt/exe/amc/reftypes.md#val)||Total size in bytes of all fields|
|amc.FCtype.n_xref|i32|[Val](/txt/exe/amc/reftypes.md#val)|||
|amc.FCtype.next_anon_idx|i32|[Val](/txt/exe/amc/reftypes.md#val)|0|sequence to enumerate positional fields|
|amc.FCtype.c_nossimfile|[amc.FNossimfile](/txt/exe/amc/internals.md#amc-fnossimfile)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FCtype.topo_idx|i32|[Val](/txt/exe/amc/reftypes.md#val)||Index of ctype in array c_ctype (after topological sort)|
|amc.FCtype.c_cfast|[amc.FCfast](/txt/exe/amc/internals.md#amc-fcfast)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FCtype.c_ffast|[amc.FFfast](/txt/exe/amc/internals.md#amc-fffast)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|amc.FCtype.in_copy_priv|bool|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FCtype
<a href="#struct-fctype"></a>
*Note:* field ``amc.FCtype.msghdr`` has reftype ``base`` so the fields of [dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FCtype { // amc.FCtype
    algo::Smallstr100     ctype;                      // Identifier. must be ns.typename
    algo::Comment         comment;                    //
    amc::FCfmt*           zs_cfmt_head;               // zero-terminated singly linked list
    amc::FCfmt*           zs_cfmt_tail;               // pointer to last element
    algo::cstring         cpp_type;                   // type name to use in c++
    algo::cstring         body;                       // Struct contents
    algo::cstring         sizecheck;                  // Size check assertions
    amc::FFunc*           c_init;                     // Pointer to constructor. optional pointer
    amc::FNs*             p_ns;                       // reference to parent row
    amc::FBltin*          c_bltin;                    // optional pointer
    amc::FField**         c_field_elems;              // array of pointers
    u32                   c_field_n;                  // array of pointers
    u32                   c_field_max;                // capacity of allocated array
    amc::FMsgtype*        c_msgtype;                  // optional pointer
    amc::FField*          c_varlenfld;                // optional pointer
    amc::FField*          c_optfld;                   // optional pointer
    amc::FStatictuple**   c_static_elems;             // array of pointers
    u32                   c_static_n;                 // array of pointers
    u32                   c_static_max;               // capacity of allocated array
    amc::FCpptype*        c_cpptype;                  // optional pointer
    amc::FCtype**         c_parent_elems;             // array of pointers
    u32                   c_parent_n;                 // array of pointers
    u32                   c_parent_max;               // capacity of allocated array
    amc::FSsimfile*       c_ssimfile;                 // Ssimfile associated with this ctype. optional pointer
    amc::FPack*           c_pack;                     // optional pointer
    amc::FLenfld*         c_lenfld;                   // optional pointer
    amc::FPmaskfld**      c_pmaskfld_elems;           // array of pointers
    u32                   c_pmaskfld_n;               // array of pointers
    u32                   c_pmaskfld_max;             // capacity of allocated array
    amc::FTypefld*        c_typefld;                  // optional pointer
    amc::FField**         c_datafld_elems;            // array of pointers
    u32                   c_datafld_n;                // array of pointers
    u32                   c_datafld_max;              // capacity of allocated array
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
    u32                   c_ffunc_n;                  // array of pointers
    u32                   c_ffunc_max;                // capacity of allocated array
    amc::FCafter*         zd_cafter_head;             // zero-terminated doubly linked list
    i32                   zd_cafter_n;                // zero-terminated doubly linked list
    amc::FCafter*         zd_cafter_tail;             // pointer to last element
    amc::FCsize*          c_csize;                    // optional pointer
    amc::FField*          zd_access_head;             // zero-terminated doubly linked list
    i32                   zd_access_n;                // zero-terminated doubly linked list
    amc::FField*          zd_access_tail;             // pointer to last element
    amc::FCdflt*          c_cdflt;                    // optional pointer
    amc::FArgvtype*       c_argvtype;                 // optional pointer
    amc::FFloadtuples*    c_floadtuples;              // optional pointer
    amc::FField*          c_pkeyfield;                // Field corresponding to pkey of this type. optional pointer
    amc::FFcurs**         c_fcurs_elems;              // array of pointers
    u32                   c_fcurs_n;                  // array of pointers
    u32                   c_fcurs_max;                // capacity of allocated array
    algo::cstring         copy_priv_reason;           //
    amc::FCtypelen*       c_ctypelen;                 // optional pointer
    bool                  size_unknown;               //   false
    bool                  copy_priv_valid;            //   false
    bool                  size_locked;                //   false
    bool                  topo_visited;               //   false  Temporary
    bool                  enum_visited;               //   false  Temporary
    bool                  fields_cloned;              //   false  True if fields from c_cbase have been cloned.
    bool                  original;                   //   false  True if this ctype comes from disk
    bool                  copy_priv;                  //   false  disallow copy ctor / assign op
    bool                  plaindata;                  //   false
    u32                   alignment;                  //   1
    i32                   n_padbytes;                 //   0
    u32                   totsize_byte;               //   0  Total size in bytes of all fields
    i32                   n_xref;                     //   0
    i32                   next_anon_idx;              //   0  sequence to enumerate positional fields
    amc::FNossimfile*     c_nossimfile;               // optional pointer
    i32                   topo_idx;                   //   0  Index of ctype in array c_ctype (after topological sort)
    amc::FCfast*          c_cfast;                    // optional pointer
    amc::FFfast**         c_ffast_elems;              // array of pointers
    u32                   c_ffast_n;                  // array of pointers
    u32                   c_ffast_max;                // capacity of allocated array
    bool                  in_copy_priv;               //   false
    bool                  ns_c_ctype_in_ary;          //   false  membership flag
    amc::FCtype*          ind_ctype_next;             // hash next
    amc::FCtype*          zsl_ctype_pack_tran_next;   // zslist link; -1 means not-in-list
    amc::FCtype*          zs_sig_visit_next;          // zslist link; -1 means not-in-list
    // reftype Llist of amc.FCtype.zs_cfmt prohibits copy
    // x-reference on amc.FCtype.p_ns prevents copy
    // x-reference on amc.FCtype.c_bltin prevents copy
    // reftype Ptrary of amc.FCtype.c_field prohibits copy
    // x-reference on amc.FCtype.c_msgtype prevents copy
    // x-reference on amc.FCtype.c_varlenfld prevents copy
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
    // x-reference on amc.FCtype.c_floadtuples prevents copy
    // reftype Ptrary of amc.FCtype.c_fcurs prohibits copy
    // x-reference on amc.FCtype.c_ctypelen prevents copy
    // x-reference on amc.FCtype.c_nossimfile prevents copy
    // x-reference on amc.FCtype.c_cfast prevents copy
    // reftype Ptrary of amc.FCtype.c_ffast prohibits copy
    // func:amc.FCtype..AssignOp
    amc::FCtype&         operator =(const amc::FCtype &rhs) = delete;
    // reftype Llist of amc.FCtype.zs_cfmt prohibits copy
    // x-reference on amc.FCtype.p_ns prevents copy
    // x-reference on amc.FCtype.c_bltin prevents copy
    // reftype Ptrary of amc.FCtype.c_field prohibits copy
    // x-reference on amc.FCtype.c_msgtype prevents copy
    // x-reference on amc.FCtype.c_varlenfld prevents copy
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
    // x-reference on amc.FCtype.c_floadtuples prevents copy
    // reftype Ptrary of amc.FCtype.c_fcurs prohibits copy
    // x-reference on amc.FCtype.c_ctypelen prevents copy
    // x-reference on amc.FCtype.c_nossimfile prevents copy
    // x-reference on amc.FCtype.c_cfast prevents copy
    // reftype Ptrary of amc.FCtype.c_ffast prohibits copy
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
    algo::Smallstr100   ctype;                   // Identifies the Ctype
    u32                 len;                     //   0  (calculated) length of the C++ struct in bytes
    i32                 alignment;               //   0  (calculated) alignment for the struct
    i32                 padbytes;                //   0  (calculated) total # of pad bytes
    bool                plaindata;               //   false  (calculated) this struct can me safely memcpy'ed
    bool                _db_c_ctypelen_in_ary;   //   false  membership flag
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

#### amc.Genctx - 
<a href="#amc-genctx"></a>

#### amc.Genctx Fields
<a href="#amc-genctx-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.Genctx.R|[algo_lib.Replscope](/txt/lib/algo_lib/README.md#algo_lib-replscope)|[Val](/txt/exe/amc/reftypes.md#val)|||
|amc.Genctx.p_ns|[amc.FNs](/txt/exe/amc/internals.md#amc-fns)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|amc.Genctx.p_ctype|[amc.FCtype](/txt/exe/amc/internals.md#amc-fctype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|amc.Genctx.p_field|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|amc.Genctx.p_tfunc|[amc.FTfunc](/txt/exe/amc/internals.md#amc-ftfunc)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

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
|amc.FTclass.c_tfunc|[amc.FTfunc](/txt/exe/amc/internals.md#amc-ftfunc)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|amc.FTclass.step||[Hook](/txt/exe/amc/reftypes.md#hook)|||

#### Struct FTclass
<a href="#struct-ftclass"></a>
*Note:* field ``amc.FTclass.msghdr`` has reftype ``base`` so the fields of [amcdb.Tclass](/txt/ssimdb/amcdb/tclass.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FTclass { // amc.FTclass
    algo::Smallstr50        tclass;            //
    amc::FTfunc**           c_tfunc_elems;     // array of pointers
    u32                     c_tfunc_n;         // array of pointers
    u32                     c_tfunc_max;       // capacity of allocated array
    amc::tclass_step_hook   step;              //   NULL  Pointer to a function
    amc::FTclass*           ind_tclass_next;   // hash next
    // reftype Ptrary of amc.FTclass.c_tfunc prohibits copy
    // reftype Hook of amc.FTclass.step prohibits copy
    // func:amc.FTclass..AssignOp
    inline amc::FTclass& operator =(const amc::FTclass &rhs) = delete;
    // func:amc.FTclass..Ctor
    inline               FTclass() __attribute__((nothrow));
    // func:amc.FTclass..Dtor
    inline               ~FTclass() __attribute__((nothrow));
    // reftype Ptrary of amc.FTclass.c_tfunc prohibits copy
    // reftype Hook of amc.FTclass.step prohibits copy
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
|amc.FDb.lpool|u8|[Lpool](/txt/exe/amc/reftypes.md#lpool)||private memory pool|
|amc.FDb.fsort|[amc.FFsort](/txt/exe/amc/internals.md#amc-ffsort)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.ind_cfmt|[amc.FCfmt](/txt/exe/amc/internals.md#amc-fcfmt)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|amc.FDb.dispfilter|[amc.FDispfilter](/txt/exe/amc/internals.md#amc-fdispfilter)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.usertracefld|[amc.FUsertracefld](/txt/exe/amc/internals.md#amc-fusertracefld)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.cfmt|[amc.FCfmt](/txt/exe/amc/internals.md#amc-fcfmt)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.cmdline|[command.amc](/txt/protocol/command/README.md#command-amc)|[Val](/txt/exe/amc/reftypes.md#val)|||
|amc.FDb.dispatch|[amc.FDispatch](/txt/exe/amc/internals.md#amc-fdispatch)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.dispatch_msg|[amc.FDispatchmsg](/txt/exe/amc/internals.md#amc-fdispatchmsg)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.ctype|[amc.FCtype](/txt/exe/amc/internals.md#amc-fctype)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.field|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.basepool|[amc.FBasepool](/txt/exe/amc/internals.md#amc-fbasepool)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.llist|[amc.FLlist](/txt/exe/amc/internals.md#amc-fllist)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.anonfld|[amc.FAnonfld](/txt/exe/amc/internals.md#amc-fanonfld)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.xref|[amc.FXref](/txt/exe/amc/internals.md#amc-fxref)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.ns|[amc.FNs](/txt/exe/amc/internals.md#amc-fns)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.pnew|[amc.FPnew](/txt/exe/amc/internals.md#amc-fpnew)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.fldoffset|[amc.FFldoffset](/txt/exe/amc/internals.md#amc-ffldoffset)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.typefld|[amc.FTypefld](/txt/exe/amc/internals.md#amc-ftypefld)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.lenfld|[amc.FLenfld](/txt/exe/amc/internals.md#amc-flenfld)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.bltin|[amc.FBltin](/txt/exe/amc/internals.md#amc-fbltin)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.static_tuple|[amc.FStatictuple](/txt/exe/amc/internals.md#amc-fstatictuple)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.msgtype|[amc.FMsgtype](/txt/exe/amc/internals.md#amc-fmsgtype)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.gconst|[amc.FGconst](/txt/exe/amc/internals.md#amc-fgconst)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.gstatic|[amc.FGstatic](/txt/exe/amc/internals.md#amc-fgstatic)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.thash|[amc.FThash](/txt/exe/amc/internals.md#amc-fthash)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.outfile|[amc.FOutfile](/txt/exe/amc/internals.md#amc-foutfile)|[Tpool](/txt/exe/amc/reftypes.md#tpool)|||
|amc.FDb.func|[amc.FFunc](/txt/exe/amc/internals.md#amc-ffunc)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.smallstr|[amc.FSmallstr](/txt/exe/amc/internals.md#amc-fsmallstr)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.numstr|[amc.FNumstr](/txt/exe/amc/internals.md#amc-fnumstr)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.main|[amc.FMain](/txt/exe/amc/internals.md#amc-fmain)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.reftype|[amc.FReftype](/txt/exe/amc/internals.md#amc-freftype)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb._db|[amc.FDb](/txt/exe/amc/internals.md#amc-fdb)|[Global](/txt/exe/amc/reftypes.md#global)|||
|amc.FDb.ind_bltin|[amc.FBltin](/txt/exe/amc/internals.md#amc-fbltin)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|amc.FDb.ind_ctype|[amc.FCtype](/txt/exe/amc/internals.md#amc-fctype)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|amc.FDb.ind_dispatch|[amc.FDispatch](/txt/exe/amc/internals.md#amc-fdispatch)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|amc.FDb.ind_func|[amc.FFunc](/txt/exe/amc/internals.md#amc-ffunc)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|amc.FDb.ind_field|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|amc.FDb.ind_ns|[amc.FNs](/txt/exe/amc/internals.md#amc-fns)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|amc.FDb.ind_pnew|[amc.FPnew](/txt/exe/amc/internals.md#amc-fpnew)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|amc.FDb.ind_xref|[amc.FXref](/txt/exe/amc/internals.md#amc-fxref)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|amc.FDb.cpptype|[amc.FCpptype](/txt/exe/amc/internals.md#amc-fcpptype)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.ind_cpptype|[amc.FCpptype](/txt/exe/amc/internals.md#amc-fcpptype)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|amc.FDb.inlary|[amc.FInlary](/txt/exe/amc/internals.md#amc-finlary)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.ind_inlary|[amc.FInlary](/txt/exe/amc/internals.md#amc-finlary)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|amc.FDb.tary|[amc.FTary](/txt/exe/amc/internals.md#amc-ftary)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.ind_tary|[amc.FTary](/txt/exe/amc/internals.md#amc-ftary)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|amc.FDb.cppfunc|[amc.FCppfunc](/txt/exe/amc/internals.md#amc-fcppfunc)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.rowid|[amc.FRowid](/txt/exe/amc/internals.md#amc-frowid)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.ind_rowid|[amc.FRowid](/txt/exe/amc/internals.md#amc-frowid)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|amc.FDb.cascdel|[amc.FCascdel](/txt/exe/amc/internals.md#amc-fcascdel)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.substr|[amc.FSubstr](/txt/exe/amc/internals.md#amc-fsubstr)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.bitfld|[amc.FBitfld](/txt/exe/amc/internals.md#amc-fbitfld)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.ssimfile|[amc.FSsimfile](/txt/exe/amc/internals.md#amc-fssimfile)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.ind_ssimfile|[amc.FSsimfile](/txt/exe/amc/internals.md#amc-fssimfile)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|amc.FDb.pack|[amc.FPack](/txt/exe/amc/internals.md#amc-fpack)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.ind_pack|[amc.FPack](/txt/exe/amc/internals.md#amc-fpack)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|amc.FDb.ind_smallstr|[amc.FSmallstr](/txt/exe/amc/internals.md#amc-fsmallstr)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|amc.FDb.ptrary|[amc.FPtrary](/txt/exe/amc/internals.md#amc-fptrary)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.c_ctype_sorted|[amc.FCtype](/txt/exe/amc/internals.md#amc-fctype)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|amc.FDb.enumstr|[amc.FEnumstr](/txt/exe/amc/internals.md#amc-fenumstr)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.enumstr_len|[amc.FEnumstrLen](/txt/exe/amc/internals.md#amc-fenumstrlen)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.bh_enumstr_len|[amc.FEnumstrLen](/txt/exe/amc/internals.md#amc-fenumstrlen)|[Bheap](/txt/exe/amc/reftypes.md#bheap)||List of FEnumstrLen by length|
|amc.FDb.ind_enumstr_len|[amc.FEnumstrLen](/txt/exe/amc/internals.md#amc-fenumstrlen)|[Thash](/txt/exe/amc/reftypes.md#thash)||Index of FEnumstrLen by length|
|amc.FDb.ind_enumstr|[amc.FEnumstr](/txt/exe/amc/internals.md#amc-fenumstr)|[Thash](/txt/exe/amc/reftypes.md#thash)||Index of FEnumstr by string value|
|amc.FDb.fbitset|[amc.FFbitset](/txt/exe/amc/internals.md#amc-ffbitset)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.ind_fbitset|[amc.FFbitset](/txt/exe/amc/internals.md#amc-ffbitset)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|amc.FDb.fcleanup|[amc.FFcleanup](/txt/exe/amc/internals.md#amc-ffcleanup)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.fdec|[amc.FFdec](/txt/exe/amc/internals.md#amc-ffdec)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.ind_fdec|[amc.FFdec](/txt/exe/amc/internals.md#amc-ffdec)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|amc.FDb.ind_reftype|[amc.FReftype](/txt/exe/amc/internals.md#amc-freftype)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|amc.FDb.fconst|[amc.FFconst](/txt/exe/amc/internals.md#amc-ffconst)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.ind_fconst|[amc.FFconst](/txt/exe/amc/internals.md#amc-ffconst)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|amc.FDb.c_ns_sorted|[amc.FNs](/txt/exe/amc/internals.md#amc-fns)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|amc.FDb.finput|[amc.FFinput](/txt/exe/amc/internals.md#amc-ffinput)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.foutput|[amc.FFoutput](/txt/exe/amc/internals.md#amc-ffoutput)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.fbuf|[amc.FFbuf](/txt/exe/amc/internals.md#amc-ffbuf)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.ind_fbuf|[amc.FFbuf](/txt/exe/amc/internals.md#amc-ffbuf)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|amc.FDb.chash|[amc.FChash](/txt/exe/amc/internals.md#amc-fchash)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.ind_chash|[amc.FChash](/txt/exe/amc/internals.md#amc-fchash)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|amc.FDb.ccmp|[amc.FCcmp](/txt/exe/amc/internals.md#amc-fccmp)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.ind_ccmp|[amc.FCcmp](/txt/exe/amc/internals.md#amc-fccmp)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|amc.FDb.fbigend|[amc.FFbigend](/txt/exe/amc/internals.md#amc-ffbigend)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.zsl_ctype_pack_tran|[amc.FCtype](/txt/exe/amc/internals.md#amc-fctype)|[Llist](/txt/exe/amc/reftypes.md#llist)||Transitive closure on packed ctype|
|amc.FDb.cstr|[amc.FCstr](/txt/exe/amc/internals.md#amc-fcstr)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.listtype|[amc.FListtype](/txt/exe/amc/internals.md#amc-flisttype)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.ind_listtype|[amc.FListtype](/txt/exe/amc/internals.md#amc-flisttype)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|amc.FDb.fstep|[amc.FFstep](/txt/exe/amc/internals.md#amc-ffstep)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.cextern|[amc.FCextern](/txt/exe/amc/internals.md#amc-fcextern)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.fdelay|[amc.FFdelay](/txt/exe/amc/internals.md#amc-ffdelay)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.disptrace|[amc.FDisptrace](/txt/exe/amc/internals.md#amc-fdisptrace)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.ind_fstep|[amc.FFstep](/txt/exe/amc/internals.md#amc-ffstep)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|amc.FDb.tracefld|[amc.FTracefld](/txt/exe/amc/internals.md#amc-ftracefld)|[Lary](/txt/exe/amc/reftypes.md#lary)||Word trace is already taken...|
|amc.FDb.tracerec|[amc.FTracerec](/txt/exe/amc/internals.md#amc-ftracerec)|[Lary](/txt/exe/amc/reftypes.md#lary)||Word trace is already taken...|
|amc.FDb.dispsig|[amc.FDispsig](/txt/exe/amc/internals.md#amc-fdispsig)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.c_dispsig_sorted|[amc.FDispsig](/txt/exe/amc/internals.md#amc-fdispsig)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|amc.FDb.zs_sig_visit|[amc.FCtype](/txt/exe/amc/internals.md#amc-fctype)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|amc.FDb.target|[amc.FTarget](/txt/exe/amc/internals.md#amc-ftarget)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.ind_target|[amc.FTarget](/txt/exe/amc/internals.md#amc-ftarget)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|amc.FDb.targdep|[amc.FTargdep](/txt/exe/amc/internals.md#amc-ftargdep)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.c_malloc|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FDb.dispctx|[amc.FDispctx](/txt/exe/amc/internals.md#amc-fdispctx)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.pmaskfld|[amc.FPmaskfld](/txt/exe/amc/internals.md#amc-fpmaskfld)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.genctx|[amc.Genctx](/txt/exe/amc/internals.md#amc-genctx)|[Val](/txt/exe/amc/reftypes.md#val)|||
|amc.FDb.fwddecl|[amc.FFwddecl](/txt/exe/amc/internals.md#amc-ffwddecl)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.ind_fwddecl|[amc.FFwddecl](/txt/exe/amc/internals.md#amc-ffwddecl)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|amc.FDb.tfunc|[amc.FTfunc](/txt/exe/amc/internals.md#amc-ftfunc)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.ind_tfunc|[amc.FTfunc](/txt/exe/amc/internals.md#amc-ftfunc)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|amc.FDb.gen|[amc.FGen](/txt/exe/amc/internals.md#amc-fgen)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.c_curns|[amc.FNs](/txt/exe/amc/internals.md#amc-fns)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FDb.fregx|[amc.FFregx](/txt/exe/amc/internals.md#amc-ffregx)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.tclass|[amc.FTclass](/txt/exe/amc/internals.md#amc-ftclass)|[Inlary](/txt/exe/amc/reftypes.md#inlary)|||
|amc.FDb.ind_tclass|[amc.FTclass](/txt/exe/amc/internals.md#amc-ftclass)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|amc.FDb.fcmp|[amc.FFcmp](/txt/exe/amc/internals.md#amc-ffcmp)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.fcast|[amc.FFcast](/txt/exe/amc/internals.md#amc-ffcast)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.noxref|[amc.FNoxref](/txt/exe/amc/internals.md#amc-fnoxref)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.nocascdel|[amc.FNocascdel](/txt/exe/amc/internals.md#amc-fnocascdel)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.cafter|[amc.FCafter](/txt/exe/amc/internals.md#amc-fcafter)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.csize|[amc.FCsize](/txt/exe/amc/internals.md#amc-fcsize)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.nsx|[amc.FNsx](/txt/exe/amc/internals.md#amc-fnsx)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.report|[report.amc](/txt/protocol/report/README.md#report-amc)|[Val](/txt/exe/amc/reftypes.md#val)||Final report|
|amc.FDb.fcompact|[amc.FFcompact](/txt/exe/amc/internals.md#amc-ffcompact)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.findrem|[amc.FFindrem](/txt/exe/amc/internals.md#amc-ffindrem)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.fcurs|[amc.FFcurs](/txt/exe/amc/internals.md#amc-ffcurs)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.cdflt|[amc.FCdflt](/txt/exe/amc/internals.md#amc-fcdflt)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.argvtype|[amc.FArgvtype](/txt/exe/amc/internals.md#amc-fargvtype)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.fcmdline|[amc.FFcmdline](/txt/exe/amc/internals.md#amc-ffcmdline)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.ind_main|[amc.FMain](/txt/exe/amc/internals.md#amc-fmain)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|amc.FDb.floadtuples|[amc.FFloadtuples](/txt/exe/amc/internals.md#amc-ffloadtuples)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.fcmap|[amc.FFcmap](/txt/exe/amc/internals.md#amc-ffcmap)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.lim_ind_func|u32|[Val](/txt/exe/amc/reftypes.md#val)||Limit for ind_func table, used to enforce function creation|
|amc.FDb.zs_ordkeyfield|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|amc.FDb.lockfile|[algo_lib.FLockfile](/txt/lib/algo_lib/README.md#algo_lib-flockfile)|[Val](/txt/exe/amc/reftypes.md#val)||Prevent concurrent execution|
|amc.FDb.nsproto|[amc.FNsproto](/txt/exe/amc/internals.md#amc-fnsproto)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.nsdb|[amc.FNsdb](/txt/exe/amc/internals.md#amc-fnsdb)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.zd_substr_params|[amc.FSubstr](/txt/exe/amc/internals.md#amc-fsubstr)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|amc.FDb.ind_fconst_int|[amc.FFconst](/txt/exe/amc/internals.md#amc-ffconst)|[Thash](/txt/exe/amc/reftypes.md#thash)||Used only during generation|
|amc.FDb.fprefix|[amc.FFprefix](/txt/exe/amc/internals.md#amc-ffprefix)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.ind_prefix|[amc.FFprefix](/txt/exe/amc/internals.md#amc-ffprefix)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|amc.FDb.ftrace|[amc.FFtrace](/txt/exe/amc/internals.md#amc-fftrace)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.c_ns|[amc.FNs](/txt/exe/amc/internals.md#amc-fns)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FDb.fnoremove|[amc.FFnoremove](/txt/exe/amc/internals.md#amc-ffnoremove)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.c_substr_field|[amc.FSubstr](/txt/exe/amc/internals.md#amc-fsubstr)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|amc.FDb.ctypelen|[amc.FCtypelen](/txt/exe/amc/internals.md#amc-fctypelen)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.c_ctypelen|[amc.FCtypelen](/txt/exe/amc/internals.md#amc-fctypelen)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|amc.FDb.c_u64|[amc.FCtype](/txt/exe/amc/internals.md#amc-fctype)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FDb.c_tempfield|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|amc.FDb.fbase|[amc.FFbase](/txt/exe/amc/internals.md#amc-ffbase)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.ind_fcmap|[amc.FFcmap](/txt/exe/amc/internals.md#amc-ffcmap)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|amc.FDb.nossimfile|[amc.FNossimfile](/txt/exe/amc/internals.md#amc-fnossimfile)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.gsymbol|[amc.FGsymbol](/txt/exe/amc/internals.md#amc-fgsymbol)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.sortfld|[amc.FSortfld](/txt/exe/amc/internals.md#amc-fsortfld)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.cget|[amc.FCget](/txt/exe/amc/internals.md#amc-fcget)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.cd_temp_func|[amc.FFunc](/txt/exe/amc/internals.md#amc-ffunc)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|amc.FDb.zs_gen_perns|[amc.FGen](/txt/exe/amc/internals.md#amc-fgen)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|amc.FDb.hook|[amc.FHook](/txt/exe/amc/internals.md#amc-fhook)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.charset|[amc.FCharset](/txt/exe/amc/internals.md#amc-fcharset)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.ValidRnumPad|[algo.Charset](/txt/protocol/algo/Charset.md)|[Charset](/txt/exe/amc/reftypes.md#charset)|||
|amc.FDb.nsinclude|[amc.FNsinclude](/txt/exe/amc/internals.md#amc-fnsinclude)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.ssimvolatile|[amc.FSsimvolatile](/txt/exe/amc/internals.md#amc-fssimvolatile)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.funique|[amc.FFunique](/txt/exe/amc/internals.md#amc-ffunique)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.fuserinit|[amc.FFuserinit](/txt/exe/amc/internals.md#amc-ffuserinit)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.ind_fcurs|[amc.FFcurs](/txt/exe/amc/internals.md#amc-ffcurs)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|amc.FDb.tcurs|[amc.FTcurs](/txt/exe/amc/internals.md#amc-ftcurs)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.cur_gen|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|amc.FDb.has_ams_fwd_declare|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|amc.FDb.nscpp|[amc.FNscpp](/txt/exe/amc/internals.md#amc-fnscpp)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.fflag|[amc.FFflag](/txt/exe/amc/internals.md#amc-ffflag)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.ind_fflag|[amc.FFflag](/txt/exe/amc/internals.md#amc-ffflag)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|amc.FDb.falias|[amc.FFalias](/txt/exe/amc/internals.md#amc-ffalias)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.copyright|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|amc.FDb.license|[amc.FLicense](/txt/exe/amc/internals.md#amc-flicense)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.ind_license|[amc.FLicense](/txt/exe/amc/internals.md#amc-flicense)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|amc.FDb.c_ssimfile_sorted|[amc.FSsimfile](/txt/exe/amc/internals.md#amc-fssimfile)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)||Global list of ssimfiles topologically sorted by pkey|
|amc.FDb.zd_ssimfile_todo|[amc.FSsimfile](/txt/exe/amc/internals.md#amc-fssimfile)|[Llist](/txt/exe/amc/reftypes.md#llist)||Temporary, for sorting ssimfiles|
|amc.FDb.cfast|[amc.FCfast](/txt/exe/amc/internals.md#amc-fcfast)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.ffast|[amc.FFfast](/txt/exe/amc/internals.md#amc-fffast)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.pmaskfld_member|[amc.FPmaskfldMember](/txt/exe/amc/internals.md#amc-fpmaskfldmember)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.ind_pmaskfld|[amc.FPmaskfld](/txt/exe/amc/internals.md#amc-fpmaskfld)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|amc.FDb.ssimsort|[amc.FSsimsort](/txt/exe/amc/internals.md#amc-fssimsort)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.fbuftype|[amc.FFbuftype](/txt/exe/amc/internals.md#amc-ffbuftype)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|amc.FDb.ind_fbuftype|[amc.FFbuftype](/txt/exe/amc/internals.md#amc-ffbuftype)|[Thash](/txt/exe/amc/reftypes.md#thash)|||

#### Struct FDb
<a href="#struct-fdb"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FDb { // amc.FDb: In-memory database for amc
    lpool_Lpblock*          lpool_free[36];                           // Lpool levels
    amc::FFsort*            fsort_lary[32];                           // level array
    i32                     fsort_n;                                  // number of elements in array
    amc::FCfmt**            ind_cfmt_buckets_elems;                   // pointer to bucket array
    i32                     ind_cfmt_buckets_n;                       // number of elements in bucket array
    i32                     ind_cfmt_n;                               // number of elements in the hash table
    amc::FDispfilter*       dispfilter_lary[32];                      // level array
    i32                     dispfilter_n;                             // number of elements in array
    amc::FUsertracefld*     usertracefld_lary[32];                    // level array
    i32                     usertracefld_n;                           // number of elements in array
    amc::FCfmt*             cfmt_lary[32];                            // level array
    i32                     cfmt_n;                                   // number of elements in array
    command::amc            cmdline;                                  //
    amc::FDispatch*         dispatch_lary[32];                        // level array
    i32                     dispatch_n;                               // number of elements in array
    amc::FDispatchmsg*      dispatch_msg_lary[32];                    // level array
    i32                     dispatch_msg_n;                           // number of elements in array
    amc::FCtype*            ctype_lary[32];                           // level array
    i32                     ctype_n;                                  // number of elements in array
    amc::FField*            field_lary[32];                           // level array
    i32                     field_n;                                  // number of elements in array
    amc::FBasepool*         basepool_lary[32];                        // level array
    i32                     basepool_n;                               // number of elements in array
    amc::FLlist*            llist_lary[32];                           // level array
    i32                     llist_n;                                  // number of elements in array
    amc::FAnonfld*          anonfld_lary[32];                         // level array
    i32                     anonfld_n;                                // number of elements in array
    amc::FXref*             xref_lary[32];                            // level array
    i32                     xref_n;                                   // number of elements in array
    amc::FNs*               ns_lary[32];                              // level array
    i32                     ns_n;                                     // number of elements in array
    amc::FPnew*             pnew_lary[32];                            // level array
    i32                     pnew_n;                                   // number of elements in array
    amc::FFldoffset*        fldoffset_lary[32];                       // level array
    i32                     fldoffset_n;                              // number of elements in array
    amc::FTypefld*          typefld_lary[32];                         // level array
    i32                     typefld_n;                                // number of elements in array
    amc::FLenfld*           lenfld_lary[32];                          // level array
    i32                     lenfld_n;                                 // number of elements in array
    amc::FBltin*            bltin_lary[32];                           // level array
    i32                     bltin_n;                                  // number of elements in array
    amc::FStatictuple*      static_tuple_lary[32];                    // level array
    i32                     static_tuple_n;                           // number of elements in array
    amc::FMsgtype*          msgtype_lary[32];                         // level array
    i32                     msgtype_n;                                // number of elements in array
    amc::FGconst*           gconst_lary[32];                          // level array
    i32                     gconst_n;                                 // number of elements in array
    amc::FGstatic*          gstatic_lary[32];                         // level array
    i32                     gstatic_n;                                // number of elements in array
    amc::FThash*            thash_lary[32];                           // level array
    i32                     thash_n;                                  // number of elements in array
    u64                     outfile_blocksize;                        // # bytes per block
    amc::FOutfile*          outfile_free;                             //
    amc::FFunc*             func_lary[32];                            // level array
    i32                     func_n;                                   // number of elements in array
    amc::FSmallstr*         smallstr_lary[32];                        // level array
    i32                     smallstr_n;                               // number of elements in array
    amc::FNumstr*           numstr_lary[32];                          // level array
    i32                     numstr_n;                                 // number of elements in array
    amc::FMain*             main_lary[32];                            // level array
    i32                     main_n;                                   // number of elements in array
    amc::FReftype*          reftype_lary[32];                         // level array
    i32                     reftype_n;                                // number of elements in array
    amc::FBltin**           ind_bltin_buckets_elems;                  // pointer to bucket array
    i32                     ind_bltin_buckets_n;                      // number of elements in bucket array
    i32                     ind_bltin_n;                              // number of elements in the hash table
    amc::FCtype**           ind_ctype_buckets_elems;                  // pointer to bucket array
    i32                     ind_ctype_buckets_n;                      // number of elements in bucket array
    i32                     ind_ctype_n;                              // number of elements in the hash table
    amc::FDispatch**        ind_dispatch_buckets_elems;               // pointer to bucket array
    i32                     ind_dispatch_buckets_n;                   // number of elements in bucket array
    i32                     ind_dispatch_n;                           // number of elements in the hash table
    amc::FFunc**            ind_func_buckets_elems;                   // pointer to bucket array
    i32                     ind_func_buckets_n;                       // number of elements in bucket array
    i32                     ind_func_n;                               // number of elements in the hash table
    amc::FField**           ind_field_buckets_elems;                  // pointer to bucket array
    i32                     ind_field_buckets_n;                      // number of elements in bucket array
    i32                     ind_field_n;                              // number of elements in the hash table
    amc::FNs**              ind_ns_buckets_elems;                     // pointer to bucket array
    i32                     ind_ns_buckets_n;                         // number of elements in bucket array
    i32                     ind_ns_n;                                 // number of elements in the hash table
    amc::FPnew**            ind_pnew_buckets_elems;                   // pointer to bucket array
    i32                     ind_pnew_buckets_n;                       // number of elements in bucket array
    i32                     ind_pnew_n;                               // number of elements in the hash table
    amc::FXref**            ind_xref_buckets_elems;                   // pointer to bucket array
    i32                     ind_xref_buckets_n;                       // number of elements in bucket array
    i32                     ind_xref_n;                               // number of elements in the hash table
    amc::FCpptype*          cpptype_lary[32];                         // level array
    i32                     cpptype_n;                                // number of elements in array
    amc::FCpptype**         ind_cpptype_buckets_elems;                // pointer to bucket array
    i32                     ind_cpptype_buckets_n;                    // number of elements in bucket array
    i32                     ind_cpptype_n;                            // number of elements in the hash table
    amc::FInlary*           inlary_lary[32];                          // level array
    i32                     inlary_n;                                 // number of elements in array
    amc::FInlary**          ind_inlary_buckets_elems;                 // pointer to bucket array
    i32                     ind_inlary_buckets_n;                     // number of elements in bucket array
    i32                     ind_inlary_n;                             // number of elements in the hash table
    amc::FTary*             tary_lary[32];                            // level array
    i32                     tary_n;                                   // number of elements in array
    amc::FTary**            ind_tary_buckets_elems;                   // pointer to bucket array
    i32                     ind_tary_buckets_n;                       // number of elements in bucket array
    i32                     ind_tary_n;                               // number of elements in the hash table
    amc::FCppfunc*          cppfunc_lary[32];                         // level array
    i32                     cppfunc_n;                                // number of elements in array
    amc::FRowid*            rowid_lary[32];                           // level array
    i32                     rowid_n;                                  // number of elements in array
    amc::FRowid**           ind_rowid_buckets_elems;                  // pointer to bucket array
    i32                     ind_rowid_buckets_n;                      // number of elements in bucket array
    i32                     ind_rowid_n;                              // number of elements in the hash table
    amc::FCascdel*          cascdel_lary[32];                         // level array
    i32                     cascdel_n;                                // number of elements in array
    amc::FSubstr*           substr_lary[32];                          // level array
    i32                     substr_n;                                 // number of elements in array
    amc::FBitfld*           bitfld_lary[32];                          // level array
    i32                     bitfld_n;                                 // number of elements in array
    amc::FSsimfile*         ssimfile_lary[32];                        // level array
    i32                     ssimfile_n;                               // number of elements in array
    amc::FSsimfile**        ind_ssimfile_buckets_elems;               // pointer to bucket array
    i32                     ind_ssimfile_buckets_n;                   // number of elements in bucket array
    i32                     ind_ssimfile_n;                           // number of elements in the hash table
    amc::FPack*             pack_lary[32];                            // level array
    i32                     pack_n;                                   // number of elements in array
    amc::FPack**            ind_pack_buckets_elems;                   // pointer to bucket array
    i32                     ind_pack_buckets_n;                       // number of elements in bucket array
    i32                     ind_pack_n;                               // number of elements in the hash table
    amc::FSmallstr**        ind_smallstr_buckets_elems;               // pointer to bucket array
    i32                     ind_smallstr_buckets_n;                   // number of elements in bucket array
    i32                     ind_smallstr_n;                           // number of elements in the hash table
    amc::FPtrary*           ptrary_lary[32];                          // level array
    i32                     ptrary_n;                                 // number of elements in array
    amc::FCtype**           c_ctype_sorted_elems;                     // array of pointers
    u32                     c_ctype_sorted_n;                         // array of pointers
    u32                     c_ctype_sorted_max;                       // capacity of allocated array
    amc::FEnumstr*          enumstr_lary[32];                         // level array
    i32                     enumstr_n;                                // number of elements in array
    amc::FEnumstrLen*       enumstr_len_lary[32];                     // level array
    i32                     enumstr_len_n;                            // number of elements in array
    amc::FEnumstrLen**      bh_enumstr_len_elems;                     // binary heap by len
    i32                     bh_enumstr_len_n;                         // number of elements in the heap
    i32                     bh_enumstr_len_max;                       // max elements in bh_enumstr_len_elems
    amc::FEnumstrLen**      ind_enumstr_len_buckets_elems;            // pointer to bucket array
    i32                     ind_enumstr_len_buckets_n;                // number of elements in bucket array
    i32                     ind_enumstr_len_n;                        // number of elements in the hash table
    amc::FEnumstr**         ind_enumstr_buckets_elems;                // pointer to bucket array
    i32                     ind_enumstr_buckets_n;                    // number of elements in bucket array
    i32                     ind_enumstr_n;                            // number of elements in the hash table
    amc::FFbitset*          fbitset_lary[32];                         // level array
    i32                     fbitset_n;                                // number of elements in array
    amc::FFbitset**         ind_fbitset_buckets_elems;                // pointer to bucket array
    i32                     ind_fbitset_buckets_n;                    // number of elements in bucket array
    i32                     ind_fbitset_n;                            // number of elements in the hash table
    amc::FFcleanup*         fcleanup_lary[32];                        // level array
    i32                     fcleanup_n;                               // number of elements in array
    amc::FFdec*             fdec_lary[32];                            // level array
    i32                     fdec_n;                                   // number of elements in array
    amc::FFdec**            ind_fdec_buckets_elems;                   // pointer to bucket array
    i32                     ind_fdec_buckets_n;                       // number of elements in bucket array
    i32                     ind_fdec_n;                               // number of elements in the hash table
    amc::FReftype**         ind_reftype_buckets_elems;                // pointer to bucket array
    i32                     ind_reftype_buckets_n;                    // number of elements in bucket array
    i32                     ind_reftype_n;                            // number of elements in the hash table
    amc::FFconst*           fconst_lary[32];                          // level array
    i32                     fconst_n;                                 // number of elements in array
    amc::FFconst**          ind_fconst_buckets_elems;                 // pointer to bucket array
    i32                     ind_fconst_buckets_n;                     // number of elements in bucket array
    i32                     ind_fconst_n;                             // number of elements in the hash table
    amc::FNs**              c_ns_sorted_elems;                        // array of pointers
    u32                     c_ns_sorted_n;                            // array of pointers
    u32                     c_ns_sorted_max;                          // capacity of allocated array
    amc::FFinput*           finput_lary[32];                          // level array
    i32                     finput_n;                                 // number of elements in array
    amc::FFoutput*          foutput_lary[32];                         // level array
    i32                     foutput_n;                                // number of elements in array
    amc::FFbuf*             fbuf_lary[32];                            // level array
    i32                     fbuf_n;                                   // number of elements in array
    amc::FFbuf**            ind_fbuf_buckets_elems;                   // pointer to bucket array
    i32                     ind_fbuf_buckets_n;                       // number of elements in bucket array
    i32                     ind_fbuf_n;                               // number of elements in the hash table
    amc::FChash*            chash_lary[32];                           // level array
    i32                     chash_n;                                  // number of elements in array
    amc::FChash**           ind_chash_buckets_elems;                  // pointer to bucket array
    i32                     ind_chash_buckets_n;                      // number of elements in bucket array
    i32                     ind_chash_n;                              // number of elements in the hash table
    amc::FCcmp*             ccmp_lary[32];                            // level array
    i32                     ccmp_n;                                   // number of elements in array
    amc::FCcmp**            ind_ccmp_buckets_elems;                   // pointer to bucket array
    i32                     ind_ccmp_buckets_n;                       // number of elements in bucket array
    i32                     ind_ccmp_n;                               // number of elements in the hash table
    amc::FFbigend*          fbigend_lary[32];                         // level array
    i32                     fbigend_n;                                // number of elements in array
    amc::FCtype*            zsl_ctype_pack_tran_head;                 // zero-terminated singly linked list
    amc::FCstr*             cstr_lary[32];                            // level array
    i32                     cstr_n;                                   // number of elements in array
    amc::FListtype*         listtype_lary[32];                        // level array
    i32                     listtype_n;                               // number of elements in array
    amc::FListtype**        ind_listtype_buckets_elems;               // pointer to bucket array
    i32                     ind_listtype_buckets_n;                   // number of elements in bucket array
    i32                     ind_listtype_n;                           // number of elements in the hash table
    amc::FFstep*            fstep_lary[32];                           // level array
    i32                     fstep_n;                                  // number of elements in array
    amc::FCextern*          cextern_lary[32];                         // level array
    i32                     cextern_n;                                // number of elements in array
    amc::FFdelay*           fdelay_lary[32];                          // level array
    i32                     fdelay_n;                                 // number of elements in array
    amc::FDisptrace*        disptrace_lary[32];                       // level array
    i32                     disptrace_n;                              // number of elements in array
    amc::FFstep**           ind_fstep_buckets_elems;                  // pointer to bucket array
    i32                     ind_fstep_buckets_n;                      // number of elements in bucket array
    i32                     ind_fstep_n;                              // number of elements in the hash table
    amc::FTracefld*         tracefld_lary[32];                        // level array
    i32                     tracefld_n;                               // number of elements in array
    amc::FTracerec*         tracerec_lary[32];                        // level array
    i32                     tracerec_n;                               // number of elements in array
    amc::FDispsig*          dispsig_lary[32];                         // level array
    i32                     dispsig_n;                                // number of elements in array
    amc::FDispsig**         c_dispsig_sorted_elems;                   // array of pointers
    u32                     c_dispsig_sorted_n;                       // array of pointers
    u32                     c_dispsig_sorted_max;                     // capacity of allocated array
    amc::FCtype*            zs_sig_visit_head;                        // zero-terminated singly linked list
    amc::FCtype*            zs_sig_visit_tail;                        // pointer to last element
    amc::FTarget*           target_lary[32];                          // level array
    i32                     target_n;                                 // number of elements in array
    amc::FTarget**          ind_target_buckets_elems;                 // pointer to bucket array
    i32                     ind_target_buckets_n;                     // number of elements in bucket array
    i32                     ind_target_n;                             // number of elements in the hash table
    amc::FTargdep*          targdep_lary[32];                         // level array
    i32                     targdep_n;                                // number of elements in array
    amc::FField*            c_malloc;                                 // optional pointer
    amc::FDispctx*          dispctx_lary[32];                         // level array
    i32                     dispctx_n;                                // number of elements in array
    amc::FPmaskfld*         pmaskfld_lary[32];                        // level array
    i32                     pmaskfld_n;                               // number of elements in array
    amc::Genctx             genctx;                                   //
    amc::FFwddecl*          fwddecl_lary[32];                         // level array
    i32                     fwddecl_n;                                // number of elements in array
    amc::FFwddecl**         ind_fwddecl_buckets_elems;                // pointer to bucket array
    i32                     ind_fwddecl_buckets_n;                    // number of elements in bucket array
    i32                     ind_fwddecl_n;                            // number of elements in the hash table
    amc::FTfunc*            tfunc_lary[32];                           // level array
    i32                     tfunc_n;                                  // number of elements in array
    amc::FTfunc**           ind_tfunc_buckets_elems;                  // pointer to bucket array
    i32                     ind_tfunc_buckets_n;                      // number of elements in bucket array
    i32                     ind_tfunc_n;                              // number of elements in the hash table
    amc::FGen*              gen_lary[32];                             // level array
    i32                     gen_n;                                    // number of elements in array
    amc::FNs*               c_curns;                                  // optional pointer
    amc::FFregx*            fregx_lary[32];                           // level array
    i32                     fregx_n;                                  // number of elements in array
    u128                    tclass_data[sizeu128(amc::FTclass,52)];   // place for data
    i32                     tclass_n;                                 // number of elems current in existence
    enum { tclass_max = 52 };
    amc::FTclass**          ind_tclass_buckets_elems;                 // pointer to bucket array
    i32                     ind_tclass_buckets_n;                     // number of elements in bucket array
    i32                     ind_tclass_n;                             // number of elements in the hash table
    amc::FFcmp*             fcmp_lary[32];                            // level array
    i32                     fcmp_n;                                   // number of elements in array
    amc::FFcast*            fcast_lary[32];                           // level array
    i32                     fcast_n;                                  // number of elements in array
    amc::FNoxref*           noxref_lary[32];                          // level array
    i32                     noxref_n;                                 // number of elements in array
    amc::FNocascdel*        nocascdel_lary[32];                       // level array
    i32                     nocascdel_n;                              // number of elements in array
    amc::FCafter*           cafter_lary[32];                          // level array
    i32                     cafter_n;                                 // number of elements in array
    amc::FCsize*            csize_lary[32];                           // level array
    i32                     csize_n;                                  // number of elements in array
    amc::FNsx*              nsx_lary[32];                             // level array
    i32                     nsx_n;                                    // number of elements in array
    report::amc             report;                                   // Final report
    amc::FFcompact*         fcompact_lary[32];                        // level array
    i32                     fcompact_n;                               // number of elements in array
    amc::FFindrem*          findrem_lary[32];                         // level array
    i32                     findrem_n;                                // number of elements in array
    amc::FFcurs*            fcurs_lary[32];                           // level array
    i32                     fcurs_n;                                  // number of elements in array
    amc::FCdflt*            cdflt_lary[32];                           // level array
    i32                     cdflt_n;                                  // number of elements in array
    amc::FArgvtype*         argvtype_lary[32];                        // level array
    i32                     argvtype_n;                               // number of elements in array
    amc::FFcmdline*         fcmdline_lary[32];                        // level array
    i32                     fcmdline_n;                               // number of elements in array
    amc::FMain**            ind_main_buckets_elems;                   // pointer to bucket array
    i32                     ind_main_buckets_n;                       // number of elements in bucket array
    i32                     ind_main_n;                               // number of elements in the hash table
    amc::FFloadtuples*      floadtuples_lary[32];                     // level array
    i32                     floadtuples_n;                            // number of elements in array
    amc::FFcmap*            fcmap_lary[32];                           // level array
    i32                     fcmap_n;                                  // number of elements in array
    u32                     lim_ind_func;                             //   0  Limit for ind_func table, used to enforce function creation
    amc::FField*            zs_ordkeyfield_head;                      // zero-terminated singly linked list
    amc::FField*            zs_ordkeyfield_tail;                      // pointer to last element
    algo_lib::FLockfile     lockfile;                                 // Prevent concurrent execution
    amc::FNsproto*          nsproto_lary[32];                         // level array
    i32                     nsproto_n;                                // number of elements in array
    amc::FNsdb*             nsdb_lary[32];                            // level array
    i32                     nsdb_n;                                   // number of elements in array
    amc::FSubstr*           zd_substr_params_head;                    // zero-terminated doubly linked list
    i32                     zd_substr_params_n;                       // zero-terminated doubly linked list
    amc::FSubstr*           zd_substr_params_tail;                    // pointer to last element
    amc::FFconst**          ind_fconst_int_buckets_elems;             // pointer to bucket array
    i32                     ind_fconst_int_buckets_n;                 // number of elements in bucket array
    i32                     ind_fconst_int_n;                         // number of elements in the hash table
    amc::FFprefix*          fprefix_lary[32];                         // level array
    i32                     fprefix_n;                                // number of elements in array
    amc::FFprefix**         ind_prefix_buckets_elems;                 // pointer to bucket array
    i32                     ind_prefix_buckets_n;                     // number of elements in bucket array
    i32                     ind_prefix_n;                             // number of elements in the hash table
    amc::FFtrace*           ftrace_lary[32];                          // level array
    i32                     ftrace_n;                                 // number of elements in array
    amc::FNs*               c_ns;                                     // optional pointer
    amc::FFnoremove*        fnoremove_lary[32];                       // level array
    i32                     fnoremove_n;                              // number of elements in array
    amc::FSubstr**          c_substr_field_elems;                     // array of pointers
    u32                     c_substr_field_n;                         // array of pointers
    u32                     c_substr_field_max;                       // capacity of allocated array
    amc::FCtypelen*         ctypelen_lary[32];                        // level array
    i32                     ctypelen_n;                               // number of elements in array
    amc::FCtypelen**        c_ctypelen_elems;                         // array of pointers
    u32                     c_ctypelen_n;                             // array of pointers
    u32                     c_ctypelen_max;                           // capacity of allocated array
    amc::FCtype*            c_u64;                                    // optional pointer
    amc::FField**           c_tempfield_elems;                        // array of pointers
    u32                     c_tempfield_n;                            // array of pointers
    u32                     c_tempfield_max;                          // capacity of allocated array
    amc::FFbase*            fbase_lary[32];                           // level array
    i32                     fbase_n;                                  // number of elements in array
    amc::FFcmap**           ind_fcmap_buckets_elems;                  // pointer to bucket array
    i32                     ind_fcmap_buckets_n;                      // number of elements in bucket array
    i32                     ind_fcmap_n;                              // number of elements in the hash table
    amc::FNossimfile*       nossimfile_lary[32];                      // level array
    i32                     nossimfile_n;                             // number of elements in array
    amc::FGsymbol*          gsymbol_lary[32];                         // level array
    i32                     gsymbol_n;                                // number of elements in array
    amc::FSortfld*          sortfld_lary[32];                         // level array
    i32                     sortfld_n;                                // number of elements in array
    amc::FCget*             cget_lary[32];                            // level array
    i32                     cget_n;                                   // number of elements in array
    amc::FFunc*             cd_temp_func_head;                        // zero-terminated doubly linked list
    i32                     cd_temp_func_n;                           // zero-terminated doubly linked list
    amc::FGen*              zs_gen_perns_head;                        // zero-terminated singly linked list
    amc::FGen*              zs_gen_perns_tail;                        // pointer to last element
    amc::FHook*             hook_lary[32];                            // level array
    i32                     hook_n;                                   // number of elements in array
    amc::FCharset*          charset_lary[32];                         // level array
    i32                     charset_n;                                // number of elements in array
    algo::Charset           ValidRnumPad;                             //
    amc::FNsinclude*        nsinclude_lary[32];                       // level array
    i32                     nsinclude_n;                              // number of elements in array
    amc::FSsimvolatile*     ssimvolatile_lary[32];                    // level array
    i32                     ssimvolatile_n;                           // number of elements in array
    amc::FFunique*          funique_lary[32];                         // level array
    i32                     funique_n;                                // number of elements in array
    amc::FFuserinit*        fuserinit_lary[32];                       // level array
    i32                     fuserinit_n;                              // number of elements in array
    amc::FFcurs**           ind_fcurs_buckets_elems;                  // pointer to bucket array
    i32                     ind_fcurs_buckets_n;                      // number of elements in bucket array
    i32                     ind_fcurs_n;                              // number of elements in the hash table
    amc::FTcurs*            tcurs_lary[32];                           // level array
    i32                     tcurs_n;                                  // number of elements in array
    algo::cstring           cur_gen;                                  //
    bool                    has_ams_fwd_declare;                      //   false
    amc::FNscpp*            nscpp_lary[32];                           // level array
    i32                     nscpp_n;                                  // number of elements in array
    amc::FFflag*            fflag_lary[32];                           // level array
    i32                     fflag_n;                                  // number of elements in array
    amc::FFflag**           ind_fflag_buckets_elems;                  // pointer to bucket array
    i32                     ind_fflag_buckets_n;                      // number of elements in bucket array
    i32                     ind_fflag_n;                              // number of elements in the hash table
    amc::FFalias*           falias_lary[32];                          // level array
    i32                     falias_n;                                 // number of elements in array
    algo::cstring           copyright;                                //
    amc::FLicense*          license_lary[32];                         // level array
    i32                     license_n;                                // number of elements in array
    amc::FLicense**         ind_license_buckets_elems;                // pointer to bucket array
    i32                     ind_license_buckets_n;                    // number of elements in bucket array
    i32                     ind_license_n;                            // number of elements in the hash table
    amc::FSsimfile**        c_ssimfile_sorted_elems;                  // array of pointers
    u32                     c_ssimfile_sorted_n;                      // array of pointers
    u32                     c_ssimfile_sorted_max;                    // capacity of allocated array
    amc::FSsimfile*         zd_ssimfile_todo_head;                    // zero-terminated doubly linked list
    i32                     zd_ssimfile_todo_n;                       // zero-terminated doubly linked list
    amc::FSsimfile*         zd_ssimfile_todo_tail;                    // pointer to last element
    amc::FCfast*            cfast_lary[32];                           // level array
    i32                     cfast_n;                                  // number of elements in array
    amc::FFfast*            ffast_lary[32];                           // level array
    i32                     ffast_n;                                  // number of elements in array
    amc::FPmaskfldMember*   pmaskfld_member_lary[32];                 // level array
    i32                     pmaskfld_member_n;                        // number of elements in array
    amc::FPmaskfld**        ind_pmaskfld_buckets_elems;               // pointer to bucket array
    i32                     ind_pmaskfld_buckets_n;                   // number of elements in bucket array
    i32                     ind_pmaskfld_n;                           // number of elements in the hash table
    amc::FSsimsort*         ssimsort_lary[32];                        // level array
    i32                     ssimsort_n;                               // number of elements in array
    amc::FFbuftype*         fbuftype_lary[32];                        // level array
    i32                     fbuftype_n;                               // number of elements in array
    amc::FFbuftype**        ind_fbuftype_buckets_elems;               // pointer to bucket array
    i32                     ind_fbuftype_buckets_n;                   // number of elements in bucket array
    i32                     ind_fbuftype_n;                           // number of elements in the hash table
    amc::trace              trace;                                    //
};
```

#### amc.FDispatch - Generate code for a multi-way branch
<a href="#amc-fdispatch"></a>

#### amc.FDispatch Fields
<a href="#amc-fdispatch-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FDispatch.msghdr|[dmmeta.Dispatch](/txt/ssimdb/dmmeta/dispatch.md)|[Base](/txt/ssimdb/dmmeta/dispatch.md)|||
|amc.FDispatch.p_ctype_hdr|[amc.FCtype](/txt/exe/amc/internals.md#amc-fctype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|amc.FDispatch.p_casetype|[amc.FCtype](/txt/exe/amc/internals.md#amc-fctype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)||Optional casetype|
|amc.FDispatch.c_dispfilter|[amc.FDispfilter](/txt/exe/amc/internals.md#amc-fdispfilter)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FDispatch.p_ns|[amc.FNs](/txt/exe/amc/internals.md#amc-fns)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|amc.FDispatch.c_dispatch_msg|[amc.FDispatchmsg](/txt/exe/amc/internals.md#amc-fdispatchmsg)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|amc.FDispatch.c_disptrace|[amc.FDisptrace](/txt/exe/amc/internals.md#amc-fdisptrace)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FDispatch.signature|[algo.Sha1sig](/txt/protocol/algo/Sha1sig.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|amc.FDispatch.c_dispctx|[amc.FDispctx](/txt/exe/amc/internals.md#amc-fdispctx)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||

#### Struct FDispatch
<a href="#struct-fdispatch"></a>
*Note:* field ``amc.FDispatch.msghdr`` has reftype ``base`` so the fields of [dmmeta.Dispatch](/txt/ssimdb/dmmeta/dispatch.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FDispatch { // amc.FDispatch
    amc::FDispatch*       ind_dispatch_next;      // hash next
    algo::Smallstr50      dispatch;               // Primary key (ns.name)
    bool                  unk;                    //   false  Want default case?
    bool                  read;                   //   false  Generate read function
    bool                  print;                  //   false  Generate print function
    bool                  haslen;                 //   false  Include length in dispatch function
    bool                  call;                   //   false  Generate call to user-defined function
    bool                  strict;                 //   false  Only dispatch if length matches exactly
    amc::FCtype*          p_ctype_hdr;            // reference to parent row
    amc::FCtype*          p_casetype;             // reference to parent row
    amc::FDispfilter*     c_dispfilter;           // optional pointer
    amc::FNs*             p_ns;                   // reference to parent row
    amc::FDispatchmsg**   c_dispatch_msg_elems;   // array of pointers
    u32                   c_dispatch_msg_n;       // array of pointers
    u32                   c_dispatch_msg_max;     // capacity of allocated array
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
|amc.FDispatchmsg.p_ctype|[amc.FCtype](/txt/exe/amc/internals.md#amc-fctype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|amc.FDispatchmsg.p_dispatch|[amc.FDispatch](/txt/exe/amc/internals.md#amc-fdispatch)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|amc.FDispatchmsg.selector|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FDispatchmsg
<a href="#struct-fdispatchmsg"></a>
*Note:* field ``amc.FDispatchmsg.msghdr`` has reftype ``base`` so the fields of [dmmeta.DispatchMsg](/txt/ssimdb/dmmeta/dispatch_msg.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FDispatchmsg { // amc.FDispatchmsg
    algo::Smallstr100   dispatch_msg;                     //
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
|amc.FDispctx.p_ctype|[amc.FCtype](/txt/exe/amc/internals.md#amc-fctype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FDispctx
<a href="#struct-fdispctx"></a>
*Note:* field ``amc.FDispctx.msghdr`` has reftype ``base`` so the fields of [dmmeta.Dispctx](/txt/ssimdb/dmmeta/dispctx.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FDispctx { // amc.FDispctx
    algo::Smallstr50    dispatch;   //
    algo::Smallstr100   ctype;      //
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
    algo::Smallstr50   dispsig;                       //
    algo::Sha1sig      signature;                     //
    bool               _db_c_dispsig_sorted_in_ary;   //   false  membership flag
    bool               ns_c_dispsig_in_ary;           //   false  membership flag
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
|amc.FDisptrace.p_dispatch|[amc.FDispatch](/txt/exe/amc/internals.md#amc-fdispatch)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FDisptrace
<a href="#struct-fdisptrace"></a>
*Note:* field ``amc.FDisptrace.msghdr`` has reftype ``base`` so the fields of [dmmeta.Disptrace](/txt/ssimdb/dmmeta/disptrace.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FDisptrace { // amc.FDisptrace
    algo::Smallstr50   dispatch;     //
    bool               cycle;        //   false
    algo::Comment      comment;      //
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
|amc.FEnumstr.enumstr|[amc.Enumstr](/txt/exe/amc/internals.md#amc-enumstr)|[Val](/txt/exe/amc/reftypes.md#val)|||
|amc.FEnumstr.c_fconst|[amc.FFconst](/txt/exe/amc/internals.md#amc-ffconst)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)||for fconst use|

#### Struct FEnumstr
<a href="#struct-fenumstr"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FEnumstr { // amc.FEnumstr: All values of a given length
    amc::FEnumstr*   ind_enumstr_next;   // hash next
    amc::Enumstr     enumstr;            //
    amc::FFconst**   c_fconst_elems;     // array of pointers
    u32              c_fconst_n;         // array of pointers
    u32              c_fconst_max;       // capacity of allocated array
    i32              bh_enumstr_idx;     // index in heap; -1 means not-in-heap
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

#### amc.FEnumstrLen - 
<a href="#amc-fenumstrlen"></a>

#### amc.FEnumstrLen Fields
<a href="#amc-fenumstrlen-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FEnumstrLen.len|i32|[Val](/txt/exe/amc/reftypes.md#val)|||
|amc.FEnumstrLen.bh_enumstr|[amc.FEnumstr](/txt/exe/amc/internals.md#amc-fenumstr)|[Bheap](/txt/exe/amc/reftypes.md#bheap)||List of FEnumstr by string value|

#### Struct FEnumstrLen
<a href="#struct-fenumstrlen"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FEnumstrLen { // amc.FEnumstrLen
    i32                 bh_enumstr_len_idx;     // index in heap; -1 means not-in-heap
    amc::FEnumstrLen*   ind_enumstr_len_next;   // hash next
    i32                 len;                    //   0
    amc::FEnumstr**     bh_enumstr_elems;       // binary heap by str
    i32                 bh_enumstr_n;           // number of elements in the heap
    i32                 bh_enumstr_max;         // max elements in bh_enumstr_elems
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
|amc.FFalias.p_srcfield|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FFalias
<a href="#struct-ffalias"></a>
*Note:* field ``amc.FFalias.base`` has reftype ``base`` so the fields of [dmmeta.Falias](/txt/ssimdb/dmmeta/falias.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FFalias { // amc.FFalias
    algo::Smallstr100   field;        //
    algo::Smallstr100   srcfield;     //
    algo::Comment       comment;      //
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

#### amc.FFbase - Customize imported Base fields
<a href="#amc-ffbase"></a>

#### amc.FFbase Fields
<a href="#amc-ffbase-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FFbase.base|[dmmeta.Fbase](/txt/ssimdb/dmmeta/fbase.md)|[Base](/txt/ssimdb/dmmeta/fbase.md)|||

#### Struct FFbase
<a href="#struct-ffbase"></a>
*Note:* field ``amc.FFbase.base`` has reftype ``base`` so the fields of [dmmeta.Fbase](/txt/ssimdb/dmmeta/fbase.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FFbase { // amc.FFbase
    algo::Smallstr100   field;          //
    bool                stripcomment;   //   false
    algo::Comment       comment;        //
    // func:amc.FFbase..AssignOp
    inline amc::FFbase&  operator =(const amc::FFbase &rhs) = delete;
    // func:amc.FFbase..CopyCtor
    inline               FFbase(const amc::FFbase &rhs) = delete;
private:
    // func:amc.FFbase..Ctor
    inline               FFbase() __attribute__((nothrow));
    // func:amc.FFbase..Dtor
    inline               ~FFbase() __attribute__((nothrow));
    friend amc::FFbase&         fbase_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FFbase*         fbase_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 fbase_RemoveAll() __attribute__((nothrow));
    friend void                 fbase_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FFbigend - Annotate field as having big-endian storage
<a href="#amc-ffbigend"></a>

#### amc.FFbigend Fields
<a href="#amc-ffbigend-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FFbigend.msghdr|[dmmeta.Fbigend](/txt/ssimdb/dmmeta/fbigend.md)|[Base](/txt/ssimdb/dmmeta/fbigend.md)|||
|amc.FFbigend.p_field|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FFbigend
<a href="#struct-ffbigend"></a>
*Note:* field ``amc.FFbigend.msghdr`` has reftype ``base`` so the fields of [dmmeta.Fbigend](/txt/ssimdb/dmmeta/fbigend.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FFbigend { // amc.FFbigend
    algo::Smallstr100   field;     //
    algo::Comment       comment;   //
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
|amc.FFbitset.p_field|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FFbitset
<a href="#struct-ffbitset"></a>
*Note:* field ``amc.FFbitset.msghdr`` has reftype ``base`` so the fields of [dmmeta.Fbitset](/txt/ssimdb/dmmeta/fbitset.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FFbitset { // amc.FFbitset
    amc::FFbitset*      ind_fbitset_next;   // hash next
    algo::Smallstr100   field;              //
    amc::FField*        p_field;            // reference to parent row
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
|amc.FFbuf.p_insready|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|amc.FFbuf.p_inseof|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|amc.FFbuf.p_fbuftype|[amc.FFbuftype](/txt/exe/amc/internals.md#amc-ffbuftype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FFbuf
<a href="#struct-ffbuf"></a>
*Note:* field ``amc.FFbuf.msghdr`` has reftype ``base`` so the fields of [dmmeta.Fbuf](/txt/ssimdb/dmmeta/fbuf.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FFbuf { // amc.FFbuf
    amc::FFbuf*         ind_fbuf_next;   // hash next
    algo::Smallstr100   field;           //
    u32                 max;             //   0  Size of buffer in bytes
    algo::Smallstr50    fbuftype;        // Type of buffer
    algo::Smallstr100   insready;        //
    algo::Smallstr100   inseof;          //
    amc::FField*        p_insready;      // reference to parent row
    amc::FField*        p_inseof;        // reference to parent row
    amc::FFbuftype*     p_fbuftype;      // reference to parent row
    // x-reference on amc.FFbuf.p_insready prevents copy
    // x-reference on amc.FFbuf.p_inseof prevents copy
    // x-reference on amc.FFbuf.p_fbuftype prevents copy
    // func:amc.FFbuf..AssignOp
    amc::FFbuf&          operator =(const amc::FFbuf &rhs) = delete;
    // x-reference on amc.FFbuf.p_insready prevents copy
    // x-reference on amc.FFbuf.p_inseof prevents copy
    // x-reference on amc.FFbuf.p_fbuftype prevents copy
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
    amc::FFbuftype*    ind_fbuftype_next;   // hash next
    algo::Smallstr50   fbuftype;            //
    bool               skipbytes;           //   false  Has skipbytes function?
    algo::Comment      comment;             //
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
    algo::Smallstr100   field;   //
    algo::Smallstr100   expr;    //
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

#### amc.FFcleanup - Request user-implemented function to be called at Uninit time for a field
<a href="#amc-ffcleanup"></a>

#### amc.FFcleanup Fields
<a href="#amc-ffcleanup-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FFcleanup.msghdr|[dmmeta.Fcleanup](/txt/ssimdb/dmmeta/fcleanup.md)|[Base](/txt/ssimdb/dmmeta/fcleanup.md)|||
|amc.FFcleanup.p_field|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FFcleanup
<a href="#struct-ffcleanup"></a>
*Note:* field ``amc.FFcleanup.msghdr`` has reftype ``base`` so the fields of [dmmeta.Fcleanup](/txt/ssimdb/dmmeta/fcleanup.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FFcleanup { // amc.FFcleanup
    algo::Smallstr100   field;     //
    amc::FField*        p_field;   // reference to parent row
    // x-reference on amc.FFcleanup.p_field prevents copy
    // func:amc.FFcleanup..AssignOp
    inline amc::FFcleanup& operator =(const amc::FFcleanup &rhs) = delete;
    // x-reference on amc.FFcleanup.p_field prevents copy
    // func:amc.FFcleanup..CopyCtor
    inline               FFcleanup(const amc::FFcleanup &rhs) = delete;
private:
    // func:amc.FFcleanup..Ctor
    inline               FFcleanup() __attribute__((nothrow));
    // func:amc.FFcleanup..Dtor
    inline               ~FFcleanup() __attribute__((nothrow));
    friend amc::FFcleanup&      fcleanup_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FFcleanup*      fcleanup_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 fcleanup_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FFcmap - Bidirectional mapping between fconst values
<a href="#amc-ffcmap"></a>

#### amc.FFcmap Fields
<a href="#amc-ffcmap-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FFcmap.base|[dmmeta.Fcmap](/txt/ssimdb/dmmeta/fcmap.md)|[Base](/txt/ssimdb/dmmeta/fcmap.md)|||
|amc.FFcmap.p_leftField|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|amc.FFcmap.p_rightField|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|amc.FFcmap.c_convfunc|[amc.FFunc](/txt/exe/amc/internals.md#amc-ffunc)|[Ptr](/txt/exe/amc/reftypes.md#ptr)||Generated conversion func|

#### Struct FFcmap
<a href="#struct-ffcmap"></a>
*Note:* field ``amc.FFcmap.base`` has reftype ``base`` so the fields of [dmmeta.Fcmap](/txt/ssimdb/dmmeta/fcmap.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FFcmap { // amc.FFcmap
    amc::FFcmap*        ind_fcmap_next;   // hash next
    algo::Smallstr250   fcmap;            // cstring maybe?
    bool                bidir;            //   true
    algo::Comment       comment;          //
    amc::FField*        p_leftField;      // reference to parent row
    amc::FField*        p_rightField;     // reference to parent row
    amc::FFunc*         c_convfunc;       // Generated conversion func. optional pointer
    amc::FFcmap*        zs_fcmap_next;    // zslist link; -1 means not-in-list
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

#### amc.FFcmdline - Annotate field that holds process command line
<a href="#amc-ffcmdline"></a>

#### amc.FFcmdline Fields
<a href="#amc-ffcmdline-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FFcmdline.base|[dmmeta.Fcmdline](/txt/ssimdb/dmmeta/fcmdline.md)|[Base](/txt/ssimdb/dmmeta/fcmdline.md)|||
|amc.FFcmdline.p_field|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|amc.FFcmdline.p_ns|[amc.FNs](/txt/exe/amc/internals.md#amc-fns)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|amc.FFcmdline.p_basecmdline|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FFcmdline
<a href="#struct-ffcmdline"></a>
*Note:* field ``amc.FFcmdline.base`` has reftype ``base`` so the fields of [dmmeta.Fcmdline](/txt/ssimdb/dmmeta/fcmdline.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FFcmdline { // amc.FFcmdline
    algo::Smallstr100   field;           // Parent field
    bool                read;            //   true  Read the command line automatically from main?
    algo::Smallstr100   basecmdline;     // Optional command line to merge with this one
    algo::Comment       comment;         //
    amc::FField*        p_field;         // reference to parent row
    amc::FNs*           p_ns;            // reference to parent row
    amc::FField*        p_basecmdline;   // reference to parent row
    // x-reference on amc.FFcmdline.p_field prevents copy
    // x-reference on amc.FFcmdline.p_ns prevents copy
    // x-reference on amc.FFcmdline.p_basecmdline prevents copy
    // func:amc.FFcmdline..AssignOp
    inline amc::FFcmdline& operator =(const amc::FFcmdline &rhs) = delete;
    // x-reference on amc.FFcmdline.p_field prevents copy
    // x-reference on amc.FFcmdline.p_ns prevents copy
    // x-reference on amc.FFcmdline.p_basecmdline prevents copy
    // func:amc.FFcmdline..CopyCtor
    inline               FFcmdline(const amc::FFcmdline &rhs) = delete;
private:
    // func:amc.FFcmdline..Ctor
    inline               FFcmdline() __attribute__((nothrow));
    // func:amc.FFcmdline..Dtor
    inline               ~FFcmdline() __attribute__((nothrow));
    friend amc::FFcmdline&      fcmdline_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FFcmdline*      fcmdline_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 fcmdline_RemoveLast() __attribute__((nothrow));
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
    algo::Smallstr100   field;         // Target field
    bool                versionsort;   //   false  Use version sort (detect embedded integers in string)
    bool                casesens;      //   true  Comparison is case-sensitive
    bool                extrn;         //   false  Whether implementation is external
    algo::Comment       comment;       //
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
    algo::Smallstr100   field;     //
    algo::Comment       comment;   //
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

#### amc.FFconst - Specify enum value (integer + string constant) for a field
<a href="#amc-ffconst"></a>

#### amc.FFconst Fields
<a href="#amc-ffconst-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FFconst.msghdr|[dmmeta.Fconst](/txt/ssimdb/dmmeta/fconst.md)|[Base](/txt/ssimdb/dmmeta/fconst.md)|||
|amc.FFconst.p_field|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|amc.FFconst.cpp_value|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|amc.FFconst.int_val|i64|[Val](/txt/exe/amc/reftypes.md#val)||integer value|
|amc.FFconst.cpp_name|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||symbol to use in cpp file|

#### Struct FFconst
<a href="#struct-ffconst"></a>
*Note:* field ``amc.FFconst.msghdr`` has reftype ``base`` so the fields of [dmmeta.Fconst](/txt/ssimdb/dmmeta/fconst.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FFconst { // amc.FFconst
    amc::FFconst*       ind_fconst_next;         // hash next
    amc::FFconst*       ind_fconst_int_next;     // hash next
    algo::Smallstr100   fconst;                  //
    algo::CppExpr       value;                   //
    algo::Comment       comment;                 //
    amc::FField*        p_field;                 // reference to parent row
    algo::cstring       cpp_value;               //
    i64                 int_val;                 //   0  integer value
    algo::cstring       cpp_name;                // symbol to use in cpp file
    bool                field_c_fconst_in_ary;   //   false  membership flag
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
|amc.FFcurs.p_field|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FFcurs
<a href="#struct-ffcurs"></a>
*Note:* field ``amc.FFcurs.base`` has reftype ``base`` so the fields of [dmmeta.Fcurs](/txt/ssimdb/dmmeta/fcurs.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FFcurs { // amc.FFcurs
    amc::FFcurs*       ind_fcurs_next;         // hash next
    algo::Smallstr50   fcurs;                  //
    algo::Comment      comment;                //
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
|amc.FFdec.p_field|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FFdec
<a href="#struct-ffdec"></a>
*Note:* field ``amc.FFdec.msghdr`` has reftype ``base`` so the fields of [dmmeta.Fdec](/txt/ssimdb/dmmeta/fdec.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FFdec { // amc.FFdec
    amc::FFdec*         ind_fdec_next;   // hash next
    algo::Smallstr100   field;           // Target field
    i32                 nplace;          //   0  Number of implied decimal places
    bool                fixedfmt;        //   false  Print exactly SCALE chars after decimal point
    amc::FField*        p_field;         // reference to parent row
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
    algo::Smallstr100   fstep;   //
    algo::I64Dec9       delay;   // Delay between steps in seconds
    bool                scale;   //   false  Does delay apply to the whole list?
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
|amc.FFfast.p_field|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

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
    algo::Comment       comment;                //
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
    amc::FFflag*        ind_fflag_next;   // hash next
    algo::Smallstr100   field;            //
    bool                cumulative;       //   false  Accumulate on every read
    algo::cstring       emptyval;         //   ""
    algo::Comment       comment;          //
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

#### amc.FField - Specify field of a struct
<a href="#amc-ffield"></a>

#### amc.FField Fields
<a href="#amc-ffield-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FField.msghdr|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Base](/txt/ssimdb/dmmeta/field.md)|||
|amc.FField.c_fsort|[amc.FFsort](/txt/exe/amc/internals.md#amc-ffsort)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FField.c_fbitset|[amc.FFbitset](/txt/exe/amc/internals.md#amc-ffbitset)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FField.c_smallstr|[amc.FSmallstr](/txt/exe/amc/internals.md#amc-fsmallstr)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FField.c_ffunc|[amc.FFunc](/txt/exe/amc/internals.md#amc-ffunc)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|amc.FField.cpp_type|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||C++ expression to use in declaration|
|amc.FField.p_ctype|[amc.FCtype](/txt/exe/amc/internals.md#amc-fctype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|amc.FField.p_arg|[amc.FCtype](/txt/exe/amc/internals.md#amc-fctype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|amc.FField.c_xref|[amc.FXref](/txt/exe/amc/internals.md#amc-fxref)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FField.c_anonfld|[amc.FAnonfld](/txt/exe/amc/internals.md#amc-fanonfld)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FField.c_fldoffset|[amc.FFldoffset](/txt/exe/amc/internals.md#amc-ffldoffset)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FField.c_basepool|[amc.FBasepool](/txt/exe/amc/internals.md#amc-fbasepool)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FField.c_llist|[amc.FLlist](/txt/exe/amc/internals.md#amc-fllist)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FField.c_thash|[amc.FThash](/txt/exe/amc/internals.md#amc-fthash)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FField.c_inlary|[amc.FInlary](/txt/exe/amc/internals.md#amc-finlary)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FField.c_tary|[amc.FTary](/txt/exe/amc/internals.md#amc-ftary)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FField.c_rowid|[amc.FRowid](/txt/exe/amc/internals.md#amc-frowid)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FField.c_cascdel|[amc.FCascdel](/txt/exe/amc/internals.md#amc-fcascdel)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FField.rowid|double|[Val](/txt/exe/amc/reftypes.md#val)|||
|amc.FField.child_rowid|double|[Val](/txt/exe/amc/reftypes.md#val)||Rowid increment for next child|
|amc.FField.c_gstatic|[amc.FGstatic](/txt/exe/amc/internals.md#amc-fgstatic)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FField.c_ptrary|[amc.FPtrary](/txt/exe/amc/internals.md#amc-fptrary)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FField.c_pmaskfld|[amc.FPmaskfld](/txt/exe/amc/internals.md#amc-fpmaskfld)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FField.c_typefld|[amc.FTypefld](/txt/exe/amc/internals.md#amc-ftypefld)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FField.c_fcleanup|[amc.FFcleanup](/txt/exe/amc/internals.md#amc-ffcleanup)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FField.c_fdec|[amc.FFdec](/txt/exe/amc/internals.md#amc-ffdec)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FField.p_reftype|[amc.FReftype](/txt/exe/amc/internals.md#amc-freftype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|amc.FField.c_fconst|[amc.FFconst](/txt/exe/amc/internals.md#amc-ffconst)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|amc.FField.c_finput|[amc.FFinput](/txt/exe/amc/internals.md#amc-ffinput)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FField.c_foutput|[amc.FFoutput](/txt/exe/amc/internals.md#amc-ffoutput)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FField.c_fbuf|[amc.FFbuf](/txt/exe/amc/internals.md#amc-ffbuf)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FField.c_fbigend|[amc.FFbigend](/txt/exe/amc/internals.md#amc-ffbigend)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FField.c_fstep|[amc.FFstep](/txt/exe/amc/internals.md#amc-ffstep)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FField.c_fregx|[amc.FFregx](/txt/exe/amc/internals.md#amc-ffregx)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FField.c_fcmp|[amc.FFcmp](/txt/exe/amc/internals.md#amc-ffcmp)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FField.c_fcast|[amc.FFcast](/txt/exe/amc/internals.md#amc-ffcast)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FField.c_gconst|[amc.FGconst](/txt/exe/amc/internals.md#amc-fgconst)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FField.c_bitfld|[amc.FBitfld](/txt/exe/amc/internals.md#amc-fbitfld)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FField.c_noxref|[amc.FNoxref](/txt/exe/amc/internals.md#amc-fnoxref)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FField.c_cppfunc|[amc.FCppfunc](/txt/exe/amc/internals.md#amc-fcppfunc)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FField.c_substr|[amc.FSubstr](/txt/exe/amc/internals.md#amc-fsubstr)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FField.c_fcompact|[amc.FFcompact](/txt/exe/amc/internals.md#amc-ffcompact)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FField.c_findrem|[amc.FFindrem](/txt/exe/amc/internals.md#amc-ffindrem)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FField.zd_xref_keyfld|[amc.FXref](/txt/exe/amc/internals.md#amc-fxref)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|amc.FField.zs_fcmap|[amc.FFcmap](/txt/exe/amc/internals.md#amc-ffcmap)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|amc.FField.c_inkey_fconst|[amc.FFconst](/txt/exe/amc/internals.md#amc-ffconst)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FField.c_fbase|[amc.FFbase](/txt/exe/amc/internals.md#amc-ffbase)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FField.c_fnoremove|[amc.FFnoremove](/txt/exe/amc/internals.md#amc-ffnoremove)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FField.c_sortfld|[amc.FSortfld](/txt/exe/amc/internals.md#amc-fsortfld)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FField.c_ftrace|[amc.FFtrace](/txt/exe/amc/internals.md#amc-fftrace)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FField.reflect|bool|[Val](/txt/exe/amc/reftypes.md#val)||Want reflection|
|amc.FField.ctype_read|bool|[Val](/txt/exe/amc/reftypes.md#val)||Field is readable using its ctype's ReadStrptrMaybe function|
|amc.FField.has_substr|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|amc.FField.need_firstchanged|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|amc.FField.do_trace|bool|[Val](/txt/exe/amc/reftypes.md#val)||Enable tracing|
|amc.FField.processed|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|amc.FField.synthetic|bool|[Val](/txt/exe/amc/reftypes.md#val)|false||
|amc.FField.visit|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Temporary flag|
|amc.FField.c_hook|[amc.FHook](/txt/exe/amc/internals.md#amc-fhook)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FField.c_charset|[amc.FCharset](/txt/exe/amc/internals.md#amc-fcharset)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FField.bh_bitfld|[amc.FBitfld](/txt/exe/amc/internals.md#amc-fbitfld)|[Bheap](/txt/exe/amc/reftypes.md#bheap)|||
|amc.FField.c_funique|[amc.FFunique](/txt/exe/amc/internals.md#amc-ffunique)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FField.c_fuserinit|[amc.FFuserinit](/txt/exe/amc/internals.md#amc-ffuserinit)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FField.c_fflag|[amc.FFflag](/txt/exe/amc/internals.md#amc-ffflag)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FField.c_falias|[amc.FFalias](/txt/exe/amc/internals.md#amc-ffalias)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FField.c_ffast_field|[amc.FFfast](/txt/exe/amc/internals.md#amc-fffast)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FField.c_ffast_length|[amc.FFfast](/txt/exe/amc/internals.md#amc-fffast)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FField.c_ffast_mantissa|[amc.FFfast](/txt/exe/amc/internals.md#amc-fffast)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FField.c_ffast|[amc.FFfast](/txt/exe/amc/internals.md#amc-fffast)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|amc.FField.c_pmaskfld_member|[amc.FPmaskfldMember](/txt/exe/amc/internals.md#amc-fpmaskfldmember)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||

#### Struct FField
<a href="#struct-ffield"></a>
*Note:* field ``amc.FField.msghdr`` has reftype ``base`` so the fields of [dmmeta.Field](/txt/ssimdb/dmmeta/field.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FField { // amc.FField
    amc::FField*             zd_inst_next;              // zslist link; -1 means not-in-list
    amc::FField*             zd_inst_prev;              // previous element
    amc::FField*             zd_access_next;            // zslist link; -1 means not-in-list
    amc::FField*             zd_access_prev;            // previous element
    amc::FField*             ind_field_next;            // hash next
    amc::FField*             zs_ordkeyfield_next;       // zslist link; -1 means not-in-list
    algo::Smallstr100        field;                     // Primary key, as ctype.name
    algo::Smallstr100        arg;                       // Type of field
    algo::Smallstr50         reftype;                   //   "Val"  Type constructor
    algo::CppExpr            dflt;                      // Default value (c++ expression)
    algo::Comment            comment;                   //
    amc::FFsort*             c_fsort;                   // optional pointer
    amc::FFbitset*           c_fbitset;                 // optional pointer
    amc::FSmallstr*          c_smallstr;                // optional pointer
    amc::FFunc**             c_ffunc_elems;             // array of pointers
    u32                      c_ffunc_n;                 // array of pointers
    u32                      c_ffunc_max;               // capacity of allocated array
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
    amc::FFcleanup*          c_fcleanup;                // optional pointer
    amc::FFdec*              c_fdec;                    // optional pointer
    amc::FReftype*           p_reftype;                 // reference to parent row
    amc::FFconst**           c_fconst_elems;            // array of pointers
    u32                      c_fconst_n;                // array of pointers
    u32                      c_fconst_max;              // capacity of allocated array
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
    amc::FNoxref*            c_noxref;                  // optional pointer
    amc::FCppfunc*           c_cppfunc;                 // optional pointer
    amc::FSubstr*            c_substr;                  // optional pointer
    amc::FFcompact*          c_fcompact;                // optional pointer
    amc::FFindrem*           c_findrem;                 // optional pointer
    amc::FXref*              zd_xref_keyfld_head;       // zero-terminated doubly linked list
    i32                      zd_xref_keyfld_n;          // zero-terminated doubly linked list
    amc::FXref*              zd_xref_keyfld_tail;       // pointer to last element
    amc::FFcmap*             zs_fcmap_head;             // zero-terminated singly linked list
    amc::FFcmap*             zs_fcmap_tail;             // pointer to last element
    amc::FFconst*            c_inkey_fconst;            // optional pointer
    amc::FFbase*             c_fbase;                   // optional pointer
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
    amc::FFuserinit*         c_fuserinit;               // optional pointer
    amc::FFflag*             c_fflag;                   // optional pointer
    amc::FFalias*            c_falias;                  // optional pointer
    amc::FFfast*             c_ffast_field;             // optional pointer
    amc::FFfast*             c_ffast_length;            // optional pointer
    amc::FFfast*             c_ffast_mantissa;          // optional pointer
    amc::FFfast**            c_ffast_elems;             // array of pointers
    u32                      c_ffast_n;                 // array of pointers
    u32                      c_ffast_max;               // capacity of allocated array
    amc::FPmaskfldMember**   c_pmaskfld_member_elems;   // array of pointers
    u32                      c_pmaskfld_member_n;       // array of pointers
    u32                      c_pmaskfld_member_max;     // capacity of allocated array
    bool                     ctype_c_datafld_in_ary;    //   false  membership flag
    bool                     _db_c_tempfield_in_ary;    //   false  membership flag
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
    // x-reference on amc.FField.c_fcleanup prevents copy
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
    // x-reference on amc.FField.c_noxref prevents copy
    // x-reference on amc.FField.c_cppfunc prevents copy
    // x-reference on amc.FField.c_substr prevents copy
    // x-reference on amc.FField.c_fcompact prevents copy
    // x-reference on amc.FField.c_findrem prevents copy
    // reftype Llist of amc.FField.zd_xref_keyfld prohibits copy
    // reftype Llist of amc.FField.zs_fcmap prohibits copy
    // x-reference on amc.FField.c_fbase prevents copy
    // x-reference on amc.FField.c_fnoremove prevents copy
    // x-reference on amc.FField.c_sortfld prevents copy
    // x-reference on amc.FField.c_ftrace prevents copy
    // x-reference on amc.FField.c_hook prevents copy
    // x-reference on amc.FField.c_charset prevents copy
    // reftype Bheap of amc.FField.bh_bitfld prohibits copy
    // x-reference on amc.FField.c_funique prevents copy
    // x-reference on amc.FField.c_fuserinit prevents copy
    // x-reference on amc.FField.c_fflag prevents copy
    // x-reference on amc.FField.c_falias prevents copy
    // x-reference on amc.FField.c_ffast_field prevents copy
    // x-reference on amc.FField.c_ffast_length prevents copy
    // x-reference on amc.FField.c_ffast_mantissa prevents copy
    // reftype Ptrary of amc.FField.c_ffast prohibits copy
    // reftype Ptrary of amc.FField.c_pmaskfld_member prohibits copy
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
    // x-reference on amc.FField.c_fcleanup prevents copy
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
    // x-reference on amc.FField.c_noxref prevents copy
    // x-reference on amc.FField.c_cppfunc prevents copy
    // x-reference on amc.FField.c_substr prevents copy
    // x-reference on amc.FField.c_fcompact prevents copy
    // x-reference on amc.FField.c_findrem prevents copy
    // reftype Llist of amc.FField.zd_xref_keyfld prohibits copy
    // reftype Llist of amc.FField.zs_fcmap prohibits copy
    // x-reference on amc.FField.c_fbase prevents copy
    // x-reference on amc.FField.c_fnoremove prevents copy
    // x-reference on amc.FField.c_sortfld prevents copy
    // x-reference on amc.FField.c_ftrace prevents copy
    // x-reference on amc.FField.c_hook prevents copy
    // x-reference on amc.FField.c_charset prevents copy
    // reftype Bheap of amc.FField.bh_bitfld prohibits copy
    // x-reference on amc.FField.c_funique prevents copy
    // x-reference on amc.FField.c_fuserinit prevents copy
    // x-reference on amc.FField.c_fflag prevents copy
    // x-reference on amc.FField.c_falias prevents copy
    // x-reference on amc.FField.c_ffast_field prevents copy
    // x-reference on amc.FField.c_ffast_length prevents copy
    // x-reference on amc.FField.c_ffast_mantissa prevents copy
    // reftype Ptrary of amc.FField.c_ffast prohibits copy
    // reftype Ptrary of amc.FField.c_pmaskfld_member prohibits copy
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

#### amc.FFindrem - Request generation of FindRemove function
<a href="#amc-ffindrem"></a>

#### amc.FFindrem Fields
<a href="#amc-ffindrem-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FFindrem.base|[dmmeta.Findrem](/txt/ssimdb/dmmeta/findrem.md)|[Base](/txt/ssimdb/dmmeta/findrem.md)|||

#### Struct FFindrem
<a href="#struct-ffindrem"></a>
*Note:* field ``amc.FFindrem.base`` has reftype ``base`` so the fields of [dmmeta.Findrem](/txt/ssimdb/dmmeta/findrem.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FFindrem { // amc.FFindrem
    algo::Smallstr100   field;     //
    algo::Comment       comment;   //
    // func:amc.FFindrem..AssignOp
    inline amc::FFindrem& operator =(const amc::FFindrem &rhs) = delete;
    // func:amc.FFindrem..CopyCtor
    inline               FFindrem(const amc::FFindrem &rhs) = delete;
private:
    // func:amc.FFindrem..Ctor
    inline               FFindrem() __attribute__((nothrow));
    // func:amc.FFindrem..Dtor
    inline               ~FFindrem() __attribute__((nothrow));
    friend amc::FFindrem&       findrem_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FFindrem*       findrem_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 findrem_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FFinput - Describe input table of a program
<a href="#amc-ffinput"></a>

#### amc.FFinput Fields
<a href="#amc-ffinput-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FFinput.msghdr|[dmmeta.Finput](/txt/ssimdb/dmmeta/finput.md)|[Base](/txt/ssimdb/dmmeta/finput.md)|||
|amc.FFinput.p_field|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FFinput
<a href="#struct-ffinput"></a>
*Note:* field ``amc.FFinput.msghdr`` has reftype ``base`` so the fields of [dmmeta.Finput](/txt/ssimdb/dmmeta/finput.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FFinput { // amc.FFinput
    algo::Smallstr100   field;     // Target field to read
    bool                extrn;     //   false  Call user-provided function
    bool                update;    //   false
    bool                strict;    //   true  Exist process if record contains error
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

#### amc.FFldoffset - Assert field offset - will result in compile-time error if violated
<a href="#amc-ffldoffset"></a>

#### amc.FFldoffset Fields
<a href="#amc-ffldoffset-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FFldoffset.msghdr|[dmmeta.Fldoffset](/txt/ssimdb/dmmeta/fldoffset.md)|[Base](/txt/ssimdb/dmmeta/fldoffset.md)|||
|amc.FFldoffset.p_field|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FFldoffset
<a href="#struct-ffldoffset"></a>
*Note:* field ``amc.FFldoffset.msghdr`` has reftype ``base`` so the fields of [dmmeta.Fldoffset](/txt/ssimdb/dmmeta/fldoffset.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FFldoffset { // amc.FFldoffset
    algo::Smallstr100   field;     //
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
|amc.FFloadtuples.p_field|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FFloadtuples
<a href="#struct-ffloadtuples"></a>
*Note:* field ``amc.FFloadtuples.base`` has reftype ``base`` so the fields of [dmmeta.Floadtuples](/txt/ssimdb/dmmeta/floadtuples.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FFloadtuples { // amc.FFloadtuples
    algo::Smallstr100   field;     //
    algo::Comment       comment;   //
    amc::FField*        p_field;   // reference to parent row
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
    algo::Smallstr100   field;     //
    algo::Comment       comment;   //
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
    algo::Smallstr100   field;                 //
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

#### amc.FFprefix - Mapping between field prefix and Reftype
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
    amc::FFprefix*     ind_prefix_next;   // hash next
    algo::Smallstr5    fprefix;           //
    algo::Smallstr50   reftype;           //
    algo::Comment      comment;           //
    amc::FFprefix*     zs_fprefix_next;   // zslist link; -1 means not-in-list
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
    algo::Smallstr100   field;      //
    bool                partial;    //   false
    algo::Smallstr50    regxtype;   //
    algo::Comment       comment;    //
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
|amc.FFsort.p_field|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|amc.FFsort.p_sortfld|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FFsort
<a href="#struct-ffsort"></a>
*Note:* field ``amc.FFsort.msghdr`` has reftype ``base`` so the fields of [dmmeta.Fsort](/txt/ssimdb/dmmeta/fsort.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FFsort { // amc.FFsort
    algo::Smallstr100   field;       // Field (array etc) being sorted
    algo::Smallstr50    sorttype;    // Kind of sort to generate
    algo::Smallstr100   sortfld;     // Field to sort on
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
|amc.FFstep.c_fdelay|[amc.FFdelay](/txt/exe/amc/internals.md#amc-ffdelay)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FFstep.p_field|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FFstep
<a href="#struct-ffstep"></a>
*Note:* field ``amc.FFstep.msghdr`` has reftype ``base`` so the fields of [dmmeta.Fstep](/txt/ssimdb/dmmeta/fstep.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FFstep { // amc.FFstep
    amc::FFstep*        ind_fstep_next;      // hash next
    algo::Smallstr100   fstep;               //
    algo::Smallstr50    steptype;            //
    algo::Comment       comment;             //
    amc::FFdelay*       c_fdelay;            // optional pointer
    amc::FField*        p_field;             // reference to parent row
    bool                ns_c_fstep_in_ary;   //   false  membership flag
    // x-reference on amc.FFstep.c_fdelay prevents copy
    // x-reference on amc.FFstep.p_field prevents copy
    // func:amc.FFstep..AssignOp
    amc::FFstep&         operator =(const amc::FFstep &rhs) = delete;
    // x-reference on amc.FFstep.c_fdelay prevents copy
    // x-reference on amc.FFstep.p_field prevents copy
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

#### amc.FFtrace - Generate cycle/step counting fields for a step
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
    algo::Smallstr100   field;     //
    algo::Comment       comment;   //
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

#### amc.FFunc - 
<a href="#amc-ffunc"></a>

#### amc.FFunc Fields
<a href="#amc-ffunc-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FFunc.msghdr|[dmmeta.Func](/txt/ssimdb/dmmeta/func.md)|[Base](/txt/ssimdb/dmmeta/func.md)|||
|amc.FFunc.p_ns|[amc.FNs](/txt/exe/amc/internals.md#amc-fns)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|amc.FFunc.inl|bool|[Val](/txt/exe/amc/reftypes.md#val)||Make inline?|
|amc.FFunc.nonnull|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||List of non-null attributes|
|amc.FFunc.funcarg|[amc.Funcarg](/txt/exe/amc/internals.md#amc-funcarg)|[Tary](/txt/exe/amc/reftypes.md#tary)||Array of declared function arguments|
|amc.FFunc.printed|bool|[Val](/txt/exe/amc/reftypes.md#val)||Already printed? (internal)|
|amc.FFunc.throws|bool|[Val](/txt/exe/amc/reftypes.md#val)||Throws exceptions|
|amc.FFunc.proto|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Prototype|
|amc.FFunc.nothrow|bool|[Val](/txt/exe/amc/reftypes.md#val)||Definitely doesn't throw exception|
|amc.FFunc.oper|bool|[Val](/txt/exe/amc/reftypes.md#val)||Is operator?|
|amc.FFunc.body|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Body of function|
|amc.FFunc.member|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Function is a member (print inside struct)|
|amc.FFunc.disable|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Function is enabled (if not, don't print)|
|amc.FFunc.deprecate|bool|[Val](/txt/exe/amc/reftypes.md#val)||Make deprecated?|
|amc.FFunc.globns|bool|[Val](/txt/exe/amc/reftypes.md#val)||Place function outside of namespace when printing|
|amc.FFunc.ismacro|bool|[Val](/txt/exe/amc/reftypes.md#val)||Always inline.|
|amc.FFunc.isalloc|bool|[Val](/txt/exe/amc/reftypes.md#val)||Function returns a new value|
|amc.FFunc.isconst|bool|[Val](/txt/exe/amc/reftypes.md#val)||Member function is const|
|amc.FFunc.glob|bool|[Val](/txt/exe/amc/reftypes.md#val)||Function is not attached to a field (EXPLAIN!!)|
|amc.FFunc.wur|bool|[Val](/txt/exe/amc/reftypes.md#val)||Warn on unused result|
|amc.FFunc.priv|bool|[Val](/txt/exe/amc/reftypes.md#val)||Private to translateion unit (static)|
|amc.FFunc.ret|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Return type (c++ expression)|
|amc.FFunc.pure|bool|[Val](/txt/exe/amc/reftypes.md#val)||OK to call fewer times than specified in text|
|amc.FFunc.isprivate|bool|[Val](/txt/exe/amc/reftypes.md#val)||Applies to member functions only -- private|
|amc.FFunc.finalized|bool|[Val](/txt/exe/amc/reftypes.md#val)||Don't add any more code to me|
|amc.FFunc.isexplicit|bool|[Val](/txt/exe/amc/reftypes.md#val)||Is explicit constructor (add 'explicit')|
|amc.FFunc.istmpl|bool|[Val](/txt/exe/amc/reftypes.md#val)||Function is a template|
|amc.FFunc.deleted|bool|[Val](/txt/exe/amc/reftypes.md#val)||Function is marked as deleted|
|amc.FFunc.prepcond|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Preprocessor #if condition|
|amc.FFunc.initializer|[algo.StringAry](/txt/protocol/algo/README.md#algo-stringary)|[Val](/txt/exe/amc/reftypes.md#val)||Initializer list (constructors only)|

#### Struct FFunc
<a href="#struct-ffunc"></a>
*Note:* field ``amc.FFunc.msghdr`` has reftype ``base`` so the fields of [dmmeta.Func](/txt/ssimdb/dmmeta/func.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FFunc { // amc.FFunc
    amc::FFunc*         ind_func_next;       // hash next
    amc::FFunc*         cd_temp_func_next;   // zslist link; -1 means not-in-list
    amc::FFunc*         cd_temp_func_prev;   // previous element
    algo::Smallstr100   func;                // Primary key
    bool                extrn;               //   false  Implemented externally by user
    algo::cstring       comment;             // Description
    amc::FNs*           p_ns;                // reference to parent row
    bool                inl;                 //   false  Make inline?
    algo::cstring       nonnull;             // List of non-null attributes
    amc::Funcarg*       funcarg_elems;       // pointer to elements
    u32                 funcarg_n;           // number of elements in array
    u32                 funcarg_max;         // max. capacity of array before realloc
    bool                printed;             //   false  Already printed? (internal)
    bool                throws;              //   false  Throws exceptions
    algo::cstring       proto;               // Prototype
    bool                nothrow;             //   false  Definitely doesn't throw exception
    bool                oper;                //   false  Is operator?
    algo::cstring       body;                // Body of function
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
    algo::cstring       ret;                 // Return type (c++ expression)
    bool                pure;                //   false  OK to call fewer times than specified in text
    bool                isprivate;           //   false  Applies to member functions only -- private
    bool                finalized;           //   false  Don't add any more code to me
    bool                isexplicit;          //   false  Is explicit constructor (add 'explicit')
    bool                istmpl;              //   false  Function is a template
    bool                deleted;             //   false  Function is marked as deleted
    algo::cstring       prepcond;            // Preprocessor #if condition
    algo::StringAry     initializer;         // Initializer list (constructors only)
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
    algo::Smallstr100   field;     //
    algo::Comment       comment;   //
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

#### amc.FFuserinit - Add user-defined initialization function for field (see fcleanup)
<a href="#amc-ffuserinit"></a>

#### amc.FFuserinit Fields
<a href="#amc-ffuserinit-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FFuserinit.base|[dmmeta.Fuserinit](/txt/ssimdb/dmmeta/fuserinit.md)|[Base](/txt/ssimdb/dmmeta/fuserinit.md)|||

#### Struct FFuserinit
<a href="#struct-ffuserinit"></a>
*Note:* field ``amc.FFuserinit.base`` has reftype ``base`` so the fields of [dmmeta.Fuserinit](/txt/ssimdb/dmmeta/fuserinit.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FFuserinit { // amc.FFuserinit
    algo::Smallstr100   field;     //
    algo::Comment       comment;   //
    // func:amc.FFuserinit..AssignOp
    inline amc::FFuserinit& operator =(const amc::FFuserinit &rhs) = delete;
    // func:amc.FFuserinit..CopyCtor
    inline               FFuserinit(const amc::FFuserinit &rhs) = delete;
private:
    // func:amc.FFuserinit..Ctor
    inline               FFuserinit() __attribute__((nothrow));
    // func:amc.FFuserinit..Dtor
    inline               ~FFuserinit() __attribute__((nothrow));
    friend amc::FFuserinit&     fuserinit_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FFuserinit*     fuserinit_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 fuserinit_RemoveAll() __attribute__((nothrow));
    friend void                 fuserinit_RemoveLast() __attribute__((nothrow));
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
    algo::Smallstr100   fwddecl;               //
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
|amc.FGconst.p_namefld|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|amc.FGconst.p_field|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FGconst
<a href="#struct-fgconst"></a>
*Note:* field ``amc.FGconst.msghdr`` has reftype ``base`` so the fields of [dmmeta.Gconst](/txt/ssimdb/dmmeta/gconst.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FGconst { // amc.FGconst
    algo::Smallstr100   field;       // Field with which to associate constants
    algo::Smallstr100   namefld;     // Field which gives the name of the constant
    algo::Smallstr100   idfld;       // Optional attribute of namefld that gives numeric ID for gconst
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

#### amc.FGen - 
<a href="#amc-fgen"></a>

#### amc.FGen Fields
<a href="#amc-fgen-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FGen.msghdr|[amcdb.Gen](/txt/ssimdb/amcdb/gen.md)|[Base](/txt/ssimdb/amcdb/gen.md)|||
|amc.FGen.cycle_total|u64|[Val](/txt/exe/amc/reftypes.md#val)|||
|amc.FGen.step||[Hook](/txt/exe/amc/reftypes.md#hook)|||

#### Struct FGen
<a href="#struct-fgen"></a>
*Note:* field ``amc.FGen.msghdr`` has reftype ``base`` so the fields of [amcdb.Gen](/txt/ssimdb/amcdb/gen.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FGen { // amc.FGen
    amc::FGen*           zs_gen_perns_next;   // zslist link; -1 means not-in-list
    algo::Smallstr50     gen;                 //
    bool                 perns;               //   false
    algo::Comment        comment;             //
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

#### amc.FGenXref - 
<a href="#amc-fgenxref"></a>

#### amc.FGenXref Fields
<a href="#amc-fgenxref-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FGenXref.ind_seen|[amc.FGenXrefSeen](/txt/exe/amc/internals.md#amc-fgenxrefseen)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|amc.FGenXref.seen|[amc.FGenXrefSeen](/txt/exe/amc/internals.md#amc-fgenxrefseen)|[Lary](/txt/exe/amc/reftypes.md#lary)|||

#### Struct FGenXref
<a href="#struct-fgenxref"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FGenXref { // amc.FGenXref
    amc::FGenXrefSeen**   ind_seen_buckets_elems;   // pointer to bucket array
    i32                   ind_seen_buckets_n;       // number of elements in bucket array
    i32                   ind_seen_n;               // number of elements in the hash table
    amc::FGenXrefSeen*    seen_lary[32];            // level array
    i32                   seen_n;                   // number of elements in array
    // func:amc.FGenXref..Ctor
    inline               FGenXref() __attribute__((nothrow));
    // func:amc.FGenXref..Dtor
    inline               ~FGenXref() __attribute__((nothrow));
};
```

#### amc.FGenXrefSeen - 
<a href="#amc-fgenxrefseen"></a>

#### amc.FGenXrefSeen Fields
<a href="#amc-fgenxrefseen-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FGenXrefSeen.value|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|amc.FGenXrefSeen.via|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)||Saved via|
|amc.FGenXrefSeen.xreffld|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)||Saved xreffld|

#### Struct FGenXrefSeen
<a href="#struct-fgenxrefseen"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FGenXrefSeen { // amc.FGenXrefSeen
    amc::FGenXrefSeen*   ind_seen_next;   // hash next
    algo::cstring        value;           //
    algo::Smallstr100    via;             // Saved via
    algo::Smallstr100    xreffld;         // Saved xreffld
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
|amc.FGstatic.p_field|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|amc.FGstatic.rowid|u32|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FGstatic
<a href="#struct-fgstatic"></a>
*Note:* field ``amc.FGstatic.msghdr`` has reftype ``base`` so the fields of [dmmeta.Gstatic](/txt/ssimdb/dmmeta/gstatic.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FGstatic { // amc.FGstatic
    algo::Smallstr100   field;                 //
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
|amc.FGsymbol.p_ssimfile|[amc.FSsimfile](/txt/exe/amc/internals.md#amc-fssimfile)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|amc.FGsymbol.p_symboltype|[amc.FCtype](/txt/exe/amc/internals.md#amc-fctype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FGsymbol
<a href="#struct-fgsymbol"></a>
*Note:* field ``amc.FGsymbol.base`` has reftype ``base`` so the fields of [dmmeta.Gsymbol](/txt/ssimdb/dmmeta/gsymbol.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FGsymbol { // amc.FGsymbol
    algo::Smallstr50    gsymbol;               //
    algo::Smallstr100   inc;                   // Regex to filter rows
    algo::Smallstr100   symboltype;            //
    algo::Comment       comment;               //
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
|amc.FHook.p_funcptr|[amc.FCtype](/txt/exe/amc/internals.md#amc-fctype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|amc.FHook.p_field|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FHook
<a href="#struct-fhook"></a>
*Note:* field ``amc.FHook.base`` has reftype ``base`` so the fields of [dmmeta.Hook](/txt/ssimdb/dmmeta/hook.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FHook { // amc.FHook
    algo::Smallstr100   field;       // The field
    algo::Comment       comment;     //
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
    amc::FInlary*       ind_inlary_next;   // hash next
    algo::Smallstr100   field;             //
    i32                 min;               //   0  Min. elements to preallocate
    i32                 max;               //   0  Max. elements
    algo::Comment       comment;           //
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

#### amc.FLenfld - Specify which gives length of varlen portion in bytes
<a href="#amc-flenfld"></a>

#### amc.FLenfld Fields
<a href="#amc-flenfld-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FLenfld.msghdr|[dmmeta.Lenfld](/txt/ssimdb/dmmeta/lenfld.md)|[Base](/txt/ssimdb/dmmeta/lenfld.md)|||
|amc.FLenfld.p_field|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FLenfld
<a href="#struct-flenfld"></a>
*Note:* field ``amc.FLenfld.msghdr`` has reftype ``base`` so the fields of [dmmeta.Lenfld](/txt/ssimdb/dmmeta/lenfld.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FLenfld { // amc.FLenfld
    algo::Smallstr100   field;     // Name of length field
    i32                 extra;     //   0  Extra #bytes to add
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
|amc.FLicense.text|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FLicense
<a href="#struct-flicense"></a>
*Note:* field ``amc.FLicense.base`` has reftype ``base`` so the fields of [dev.License](/txt/ssimdb/dev/license.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FLicense { // amc.FLicense
    amc::FLicense*     ind_license_next;   // hash next
    algo::Smallstr50   license;            //
    algo::Comment      comment;            //
    algo::cstring      text;               //
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
    amc::FListtype*   ind_listtype_next;   // hash next
    algo::Smallstr5   listtype;            //
    bool              circular;            //   false  Circular list
    bool              haveprev;            //   false  Previous link
    bool              instail;             //   false  Queue
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
|amc.FLlist.p_listtype|[amc.FListtype](/txt/exe/amc/internals.md#amc-flisttype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FLlist
<a href="#struct-fllist"></a>
*Note:* field ``amc.FLlist.msghdr`` has reftype ``base`` so the fields of [dmmeta.Llist](/txt/ssimdb/dmmeta/llist.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FLlist { // amc.FLlist
    algo::Smallstr100   field;        // Index in question
    bool                havetail;     //   true  Generate 'tail' field
    bool                havecount;    //   false  Generate count of elements
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
    amc::FMain*        ind_main_next;   // hash next
    algo::Smallstr16   ns;              //
    bool               ismodule;        //   false
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

#### amc.FMsgtype - Specify message type for each eligible message, controls dispatch
<a href="#amc-fmsgtype"></a>

#### amc.FMsgtype Fields
<a href="#amc-fmsgtype-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FMsgtype.msghdr|[dmmeta.Msgtype](/txt/ssimdb/dmmeta/msgtype.md)|[Base](/txt/ssimdb/dmmeta/msgtype.md)|||
|amc.FMsgtype.p_ctype|[amc.FCtype](/txt/exe/amc/internals.md#amc-fctype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

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
    algo::Smallstr100   xref;   //
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
    algo::Comment       comment;   //
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

#### amc.FNoxref - Explicitly specify that no x-ref exists between tables (don't use unless forced)
<a href="#amc-fnoxref"></a>

#### amc.FNoxref Fields
<a href="#amc-fnoxref-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FNoxref.msghdr|[dmmeta.Noxref](/txt/ssimdb/dmmeta/noxref.md)|[Base](/txt/ssimdb/dmmeta/noxref.md)|||

#### Struct FNoxref
<a href="#struct-fnoxref"></a>
*Note:* field ``amc.FNoxref.msghdr`` has reftype ``base`` so the fields of [dmmeta.Noxref](/txt/ssimdb/dmmeta/noxref.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FNoxref { // amc.FNoxref
    algo::Smallstr100   field;   //
    // func:amc.FNoxref..AssignOp
    inline amc::FNoxref& operator =(const amc::FNoxref &rhs) = delete;
    // func:amc.FNoxref..CopyCtor
    inline               FNoxref(const amc::FNoxref &rhs) = delete;
private:
    // func:amc.FNoxref..Ctor
    inline               FNoxref() __attribute__((nothrow));
    // func:amc.FNoxref..Dtor
    inline               ~FNoxref() __attribute__((nothrow));
    friend amc::FNoxref&        noxref_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend amc::FNoxref*        noxref_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 noxref_RemoveLast() __attribute__((nothrow));
};
```

#### amc.FNs - Namespace (for in-memory database, protocol, etc)
<a href="#amc-fns"></a>

#### amc.FNs Fields
<a href="#amc-fns-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FNs.msghdr|[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)|[Base](/txt/ssimdb/dmmeta/ns.md)|||
|amc.FNs.nhash|i32|[Val](/txt/exe/amc/reftypes.md#val)|0|number of hash functions generated (hack)|
|amc.FNs.curstext|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|amc.FNs.c_ctype|[amc.FCtype](/txt/exe/amc/internals.md#amc-fctype)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|amc.FNs.c_func|[amc.FFunc](/txt/exe/amc/internals.md#amc-ffunc)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|amc.FNs.c_dispatch|[amc.FDispatch](/txt/exe/amc/internals.md#amc-fdispatch)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|amc.FNs.c_globfld|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FNs.c_gstatic|[amc.FGstatic](/txt/exe/amc/internals.md#amc-fgstatic)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|amc.FNs.c_main|[amc.FMain](/txt/exe/amc/internals.md#amc-fmain)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FNs.hdr|[algo.cstring](/txt/protocol/algo/cstring.md)|[Ptr](/txt/exe/amc/reftypes.md#ptr)||Temporary hack -- pointer to hdr contents|
|amc.FNs.inl|[algo.cstring](/txt/protocol/algo/cstring.md)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FNs.cpp|[algo.cstring](/txt/protocol/algo/cstring.md)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FNs.include|[algo.cstring](/txt/protocol/algo/cstring.md)|[Tary](/txt/exe/amc/reftypes.md#tary)|||
|amc.FNs.signature|[algo.Sha1sig](/txt/protocol/algo/Sha1sig.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|amc.FNs.signature_input|[algo.Sha1sig](/txt/protocol/algo/Sha1sig.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|amc.FNs.c_dispsig|[amc.FDispsig](/txt/exe/amc/internals.md#amc-fdispsig)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|amc.FNs.c_parentns|[amc.FNs](/txt/exe/amc/internals.md#amc-fns)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)||Namespaces implied by targdep|
|amc.FNs.c_cppincl|[amc.FNs](/txt/exe/amc/internals.md#amc-fns)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)||List of .cpp includes|
|amc.FNs.c_hdrincl|[amc.FNs](/txt/exe/amc/internals.md#amc-fns)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)||List of .h includes|
|amc.FNs.c_fwddecl|[amc.FFwddecl](/txt/exe/amc/internals.md#amc-ffwddecl)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|amc.FNs.select|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|amc.FNs.c_nsx|[amc.FNsx](/txt/exe/amc/internals.md#amc-fnsx)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FNs.c_target|[amc.FTarget](/txt/exe/amc/internals.md#amc-ftarget)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FNs.c_pnew|[amc.FPnew](/txt/exe/amc/internals.md#amc-fpnew)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|amc.FNs.c_fcmdline|[amc.FFcmdline](/txt/exe/amc/internals.md#amc-ffcmdline)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FNs.c_nsproto|[amc.FNsproto](/txt/exe/amc/internals.md#amc-fnsproto)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FNs.c_nsdb|[amc.FNsdb](/txt/exe/amc/internals.md#amc-fnsdb)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FNs.c_outfile|[amc.FOutfile](/txt/exe/amc/internals.md#amc-foutfile)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|amc.FNs.c_foutput|[amc.FFoutput](/txt/exe/amc/internals.md#amc-ffoutput)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|amc.FNs.c_fstep|[amc.FFstep](/txt/exe/amc/internals.md#amc-ffstep)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|amc.FNs.c_gsymbol|[amc.FGsymbol](/txt/exe/amc/internals.md#amc-fgsymbol)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|amc.FNs.c_nsinclude|[amc.FNsinclude](/txt/exe/amc/internals.md#amc-fnsinclude)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|amc.FNs.c_nscpp|[amc.FNscpp](/txt/exe/amc/internals.md#amc-fnscpp)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FNs.p_license|[amc.FLicense](/txt/exe/amc/internals.md#amc-flicense)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FNs
<a href="#struct-fns"></a>
*Note:* field ``amc.FNs.msghdr`` has reftype ``base`` so the fields of [dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FNs { // amc.FNs
    amc::FNs*           ind_ns_next;         // hash next
    algo::Smallstr16    ns;                  // Namespace name (primary key)
    algo::Smallstr50    nstype;              // Namespace type
    algo::Smallstr50    license;             // Associated license
    algo::Comment       comment;             //
    i32                 nhash;               //   0  number of hash functions generated (hack)
    algo::cstring       curstext;            //
    amc::FCtype**       c_ctype_elems;       // array of pointers
    u32                 c_ctype_n;           // array of pointers
    u32                 c_ctype_max;         // capacity of allocated array
    amc::FFunc**        c_func_elems;        // array of pointers
    u32                 c_func_n;            // array of pointers
    u32                 c_func_max;          // capacity of allocated array
    amc::FDispatch**    c_dispatch_elems;    // array of pointers
    u32                 c_dispatch_n;        // array of pointers
    u32                 c_dispatch_max;      // capacity of allocated array
    amc::FField*        c_globfld;           // optional pointer
    amc::FGstatic**     c_gstatic_elems;     // array of pointers
    u32                 c_gstatic_n;         // array of pointers
    u32                 c_gstatic_max;       // capacity of allocated array
    amc::FMain*         c_main;              // optional pointer
    algo::cstring*      hdr;                 // Temporary hack -- pointer to hdr contents. optional pointer
    algo::cstring*      inl;                 // optional pointer
    algo::cstring*      cpp;                 // optional pointer
    algo::cstring*      include_elems;       // pointer to elements
    u32                 include_n;           // number of elements in array
    u32                 include_max;         // max. capacity of array before realloc
    algo::Sha1sig       signature;           //
    algo::Sha1sig       signature_input;     //
    amc::FDispsig**     c_dispsig_elems;     // array of pointers
    u32                 c_dispsig_n;         // array of pointers
    u32                 c_dispsig_max;       // capacity of allocated array
    amc::FNs**          c_parentns_elems;    // array of pointers
    u32                 c_parentns_n;        // array of pointers
    u32                 c_parentns_max;      // capacity of allocated array
    amc::FNs**          c_cppincl_elems;     // array of pointers
    u32                 c_cppincl_n;         // array of pointers
    u32                 c_cppincl_max;       // capacity of allocated array
    amc::FNs**          c_hdrincl_elems;     // array of pointers
    u32                 c_hdrincl_n;         // array of pointers
    u32                 c_hdrincl_max;       // capacity of allocated array
    amc::FFwddecl**     c_fwddecl_elems;     // array of pointers
    u32                 c_fwddecl_n;         // array of pointers
    u32                 c_fwddecl_max;       // capacity of allocated array
    bool                select;              //   false
    amc::FNsx*          c_nsx;               // optional pointer
    amc::FTarget*       c_target;            // optional pointer
    amc::FPnew**        c_pnew_elems;        // array of pointers
    u32                 c_pnew_n;            // array of pointers
    u32                 c_pnew_max;          // capacity of allocated array
    amc::FFcmdline*     c_fcmdline;          // optional pointer
    amc::FNsproto*      c_nsproto;           // optional pointer
    amc::FNsdb*         c_nsdb;              // optional pointer
    amc::FOutfile**     c_outfile_elems;     // array of pointers
    u32                 c_outfile_n;         // array of pointers
    u32                 c_outfile_max;       // capacity of allocated array
    amc::FFoutput**     c_foutput_elems;     // array of pointers
    u32                 c_foutput_n;         // array of pointers
    u32                 c_foutput_max;       // capacity of allocated array
    amc::FFstep**       c_fstep_elems;       // array of pointers
    u32                 c_fstep_n;           // array of pointers
    u32                 c_fstep_max;         // capacity of allocated array
    amc::FGsymbol**     c_gsymbol_elems;     // array of pointers
    u32                 c_gsymbol_n;         // array of pointers
    u32                 c_gsymbol_max;       // capacity of allocated array
    amc::FNsinclude**   c_nsinclude_elems;   // array of pointers
    u32                 c_nsinclude_n;       // array of pointers
    u32                 c_nsinclude_max;     // capacity of allocated array
    amc::FNscpp*        c_nscpp;             // optional pointer
    amc::FLicense*      p_license;           // reference to parent row
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
    // x-reference on amc.FNs.c_fcmdline prevents copy
    // x-reference on amc.FNs.c_nsproto prevents copy
    // x-reference on amc.FNs.c_nsdb prevents copy
    // reftype Ptrary of amc.FNs.c_outfile prohibits copy
    // reftype Ptrary of amc.FNs.c_foutput prohibits copy
    // reftype Ptrary of amc.FNs.c_fstep prohibits copy
    // reftype Ptrary of amc.FNs.c_gsymbol prohibits copy
    // reftype Ptrary of amc.FNs.c_nsinclude prohibits copy
    // x-reference on amc.FNs.c_nscpp prevents copy
    // x-reference on amc.FNs.p_license prevents copy
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
    // x-reference on amc.FNs.c_fcmdline prevents copy
    // x-reference on amc.FNs.c_nsproto prevents copy
    // x-reference on amc.FNs.c_nsdb prevents copy
    // reftype Ptrary of amc.FNs.c_outfile prohibits copy
    // reftype Ptrary of amc.FNs.c_foutput prohibits copy
    // reftype Ptrary of amc.FNs.c_fstep prohibits copy
    // reftype Ptrary of amc.FNs.c_gsymbol prohibits copy
    // reftype Ptrary of amc.FNs.c_nsinclude prohibits copy
    // x-reference on amc.FNs.c_nscpp prevents copy
    // x-reference on amc.FNs.p_license prevents copy
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
    algo::Comment      comment;   //
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
    algo::Comment      comment;   //
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
    algo::Comment      comment;                 //
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
    algo::Comment      comment;   //
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
|amc.FNsx.p_pool|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FNsx
<a href="#struct-fnsx"></a>
*Note:* field ``amc.FNsx.base`` has reftype ``base`` so the fields of [dmmeta.Nsx](/txt/ssimdb/dmmeta/nsx.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FNsx { // amc.FNsx
    algo::Smallstr16    ns;                    //
    bool                genthrow;              //   false
    bool                correct_getorcreate;   //   false
    algo::Smallstr100   pool;                  //
    bool                sortxref;              //   false
    bool                pack;                  //   false
    algo::Comment       comment;               //
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
|amc.FNumstr.p_numtype|[amc.FCtype](/txt/exe/amc/internals.md#amc-fctype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|amc.FNumstr.nummin|u64|[Val](/txt/exe/amc/reftypes.md#val)|||
|amc.FNumstr.nummax|u64|[Val](/txt/exe/amc/reftypes.md#val)|||
|amc.FNumstr.issigned|bool|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FNumstr
<a href="#struct-fnumstr"></a>
*Note:* field ``amc.FNumstr.msghdr`` has reftype ``base`` so the fields of [dmmeta.Numstr](/txt/ssimdb/dmmeta/numstr.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FNumstr { // amc.FNumstr
    algo::Smallstr100   field;       // String type
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
|*amc.FOutfile.ns (/RR_RL)*|[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|||
|amc.FOutfile.text|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|amc.FOutfile.p_ns|[amc.FNs](/txt/exe/amc/internals.md#amc-fns)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|amc.FOutfile.ssim|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Ssim content|
|amc.FOutfile.prev_head|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Helper|
|amc.FOutfile.outfile|[dev.Gitfile](/txt/ssimdb/dev/gitfile.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|||

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
|amc.FPack.p_ctype|[amc.FCtype](/txt/exe/amc/internals.md#amc-fctype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FPack
<a href="#struct-fpack"></a>
*Note:* field ``amc.FPack.msghdr`` has reftype ``base`` so the fields of [dmmeta.Pack](/txt/ssimdb/dmmeta/pack.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FPack { // amc.FPack
    amc::FPack*         ind_pack_next;   // hash next
    algo::Smallstr100   ctype;           // Target ctype
    amc::FCtype*        p_ctype;         // reference to parent row
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

#### amc.FPmaskfld - Specify which field holds presence mask bits (one for each field in struct)
<a href="#amc-fpmaskfld"></a>

#### amc.FPmaskfld Fields
<a href="#amc-fpmaskfld-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FPmaskfld.msghdr|[dmmeta.Pmaskfld](/txt/ssimdb/dmmeta/pmaskfld.md)|[Base](/txt/ssimdb/dmmeta/pmaskfld.md)|||
|amc.FPmaskfld.p_field|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|amc.FPmaskfld.nextbit|u32|[Val](/txt/exe/amc/reftypes.md#val)||Next bit to allocate|
|amc.FPmaskfld.c_pmaskfld_member|[amc.FPmaskfldMember](/txt/exe/amc/internals.md#amc-fpmaskfldmember)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|amc.FPmaskfld.funcname|[algo.Smallstr50](/txt/protocol/algo/README.md#algo-smallstr50)|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FPmaskfld
<a href="#struct-fpmaskfld"></a>
*Note:* field ``amc.FPmaskfld.msghdr`` has reftype ``base`` so the fields of [dmmeta.Pmaskfld](/txt/ssimdb/dmmeta/pmaskfld.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FPmaskfld { // amc.FPmaskfld
    amc::FPmaskfld*          ind_pmaskfld_next;         // hash next
    algo::Smallstr100        field;                     //
    bool                     filter_print;              //   true  Omit non-present fields when printing
    algo::Comment            comment;                   //
    amc::FField*             p_field;                   // reference to parent row
    u32                      nextbit;                   //   0  Next bit to allocate
    amc::FPmaskfldMember**   c_pmaskfld_member_elems;   // array of pointers
    u32                      c_pmaskfld_member_n;       // array of pointers
    u32                      c_pmaskfld_member_max;     // capacity of allocated array
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
|amc.FPmaskfldMember.bit|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|amc.FPmaskfldMember.p_field|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|amc.FPmaskfldMember.p_pmaskfld|[amc.FPmaskfld](/txt/exe/amc/internals.md#amc-fpmaskfld)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FPmaskfldMember
<a href="#struct-fpmaskfldmember"></a>
*Note:* field ``amc.FPmaskfldMember.base`` has reftype ``base`` so the fields of [dmmeta.PmaskfldMember](/txt/ssimdb/dmmeta/pmaskfld_member.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FPmaskfldMember { // amc.FPmaskfldMember
    algo::Smallstr100   pmaskfld_member;                     //
    algo::Comment       comment;                             //
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
|amc.FPnew.p_ctype|[amc.FCtype](/txt/exe/amc/internals.md#amc-fctype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|amc.FPnew.p_ns|[amc.FNs](/txt/exe/amc/internals.md#amc-fns)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FPnew
<a href="#struct-fpnew"></a>
*Note:* field ``amc.FPnew.msghdr`` has reftype ``base`` so the fields of [dmmeta.Pnew](/txt/ssimdb/dmmeta/pnew.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FPnew { // amc.FPnew
    amc::FPnew*         ind_pnew_next;      // hash next
    algo::Smallstr100   pnew;               //
    amc::FCtype*        p_ctype;            // reference to parent row
    amc::FNs*           p_ns;               // reference to parent row
    bool                ns_c_pnew_in_ary;   //   false  membership flag
    // x-reference on amc.FPnew.p_ctype prevents copy
    // x-reference on amc.FPnew.p_ns prevents copy
    // func:amc.FPnew..AssignOp
    inline amc::FPnew&   operator =(const amc::FPnew &rhs) = delete;
    // x-reference on amc.FPnew.p_ctype prevents copy
    // x-reference on amc.FPnew.p_ns prevents copy
    // func:amc.FPnew..CopyCtor
    inline               FPnew(const amc::FPnew &rhs) = delete;
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
|amc.FPtrary.p_field|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FPtrary
<a href="#struct-fptrary"></a>
*Note:* field ``amc.FPtrary.msghdr`` has reftype ``base`` so the fields of [dmmeta.Ptrary](/txt/ssimdb/dmmeta/ptrary.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FPtrary { // amc.FPtrary
    algo::Smallstr100   field;     //
    bool                unique;    //   false  Search for and ignore duplicates
    amc::FField*        p_field;   // reference to parent row
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
|amc.FReftype.rowid|i32|[Val](/txt/exe/amc/reftypes.md#val)|||
|amc.FReftype.p_tclass|[amc.FTclass](/txt/exe/amc/internals.md#amc-ftclass)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|amc.FReftype.zs_fprefix|[amc.FFprefix](/txt/exe/amc/internals.md#amc-ffprefix)|[Llist](/txt/exe/amc/reftypes.md#llist)|||

#### Struct FReftype
<a href="#struct-freftype"></a>
*Note:* field ``amc.FReftype.msghdr`` has reftype ``base`` so the fields of [dmmeta.Reftype](/txt/ssimdb/dmmeta/reftype.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FReftype { // amc.FReftype
    amc::FReftype*     ind_reftype_next;   // hash next
    algo::Smallstr50   reftype;            //   "Val"
    bool               isval;              //   false  True if field makes values of target type
    bool               cascins;            //   false  Field is cascade-insert
    bool               usebasepool;        //   false  Fields with this type make use of dmmeta.basepool
    bool               cancopy;            //   false  This type of field can be copied
    bool               isxref;             //   false  This type of field is an x-ref
    bool               del;                //   false  Supports random deletion?
    bool               up;                 //   false  This type of field is a reference
    bool               isnew;              //   false  If set, skip this relation in amc_vis
    bool               hasalloc;           //   false  Generte Alloc/Delete functions for arg type
    bool               inst;               //   false  Field creates an instance of arg type (directly or indirectly)
    bool               varlen;             //   false  This pool supports varlen allocations
    i32                rowid;              //   0
    amc::FTclass*      p_tclass;           // reference to parent row
    amc::FFprefix*     zs_fprefix_head;    // zero-terminated singly linked list
    amc::FFprefix*     zs_fprefix_tail;    // pointer to last element
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
    amc::FRowid*        ind_rowid_next;   // hash next
    algo::Smallstr100   field;            //
    algo::Comment       comment;          //
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
|amc.FSmallstr.p_field|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|amc.FSmallstr.c_numstr|[amc.FNumstr](/txt/exe/amc/internals.md#amc-fnumstr)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||

#### Struct FSmallstr
<a href="#struct-fsmallstr"></a>
*Note:* field ``amc.FSmallstr.msghdr`` has reftype ``base`` so the fields of [dmmeta.Smallstr](/txt/ssimdb/dmmeta/smallstr.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FSmallstr { // amc.FSmallstr
    amc::FSmallstr*     ind_smallstr_next;   // hash next
    algo::Smallstr100   field;               //
    i32                 length;              //   0  Maximum characters in the string
    algo::Smallstr50    strtype;             // Data format for string
    algo::CppExpr       pad;                 // Pad character (if applicable)
    bool                strict;              //   false
    amc::FField*        p_field;             // reference to parent row
    amc::FNumstr*       c_numstr;            // optional pointer
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
|amc.FSortfld.p_sortfld|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FSortfld
<a href="#struct-fsortfld"></a>
*Note:* field ``amc.FSortfld.msghdr`` has reftype ``base`` so the fields of [dmmeta.Sortfld](/txt/ssimdb/dmmeta/sortfld.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FSortfld { // amc.FSortfld
    algo::Smallstr100   field;       //
    algo::Smallstr100   sortfld;     // Field to sort by
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
|amc.FSsimfile.ssim|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Ssim content|
|amc.FSsimfile.p_ctype|[amc.FCtype](/txt/exe/amc/internals.md#amc-fctype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|amc.FSsimfile.c_ssimvolatile|[amc.FSsimvolatile](/txt/exe/amc/internals.md#amc-fssimvolatile)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FSsimfile.topovisit|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|amc.FSsimfile.topoindex|i32|[Val](/txt/exe/amc/reftypes.md#val)|||
|amc.FSsimfile.input_select|bool|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FSsimfile
<a href="#struct-fssimfile"></a>
*Note:* field ``amc.FSsimfile.msghdr`` has reftype ``base`` so the fields of [dmmeta.Ssimfile](/txt/ssimdb/dmmeta/ssimfile.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FSsimfile { // amc.FSsimfile
    amc::FSsimfile*       ind_ssimfile_next;              // hash next
    amc::FSsimfile*       zd_ssimfile_todo_next;          // zslist link; -1 means not-in-list
    amc::FSsimfile*       zd_ssimfile_todo_prev;          // previous element
    algo::Smallstr50      ssimfile;                       //
    algo::Smallstr100     ctype;                          //
    algo::cstring         ssim;                           // Ssim content
    amc::FCtype*          p_ctype;                        // reference to parent row
    amc::FSsimvolatile*   c_ssimvolatile;                 // optional pointer
    bool                  topovisit;                      //   false
    i32                   topoindex;                      //   0
    bool                  input_select;                   //   false
    bool                  _db_c_ssimfile_sorted_in_ary;   //   false  membership flag
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
|amc.FSsimsort.p_ssimfile|[amc.FSsimfile](/txt/exe/amc/internals.md#amc-fssimfile)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|amc.FSsimsort.p_sortfld|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FSsimsort
<a href="#struct-fssimsort"></a>
*Note:* field ``amc.FSsimsort.base`` has reftype ``base`` so the fields of [dmmeta.Ssimsort](/txt/ssimdb/dmmeta/ssimsort.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FSsimsort { // amc.FSsimsort
    algo::Smallstr50    ssimfile;     //
    algo::Smallstr100   sortfld;      //
    algo::Comment       comment;      //
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
    algo::Comment      comment;    //
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

#### amc.FStatictuple - 
<a href="#amc-fstatictuple"></a>

#### amc.FStatictuple Fields
<a href="#amc-fstatictuple-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FStatictuple.ctype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|||
|amc.FStatictuple.tuple|[algo.Tuple](/txt/protocol/algo/Tuple.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|amc.FStatictuple.cppident|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

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

#### amc.FSubstr - Specify that the field value is computed from a substring of another field
<a href="#amc-fsubstr"></a>

#### amc.FSubstr Fields
<a href="#amc-fsubstr-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.FSubstr.msghdr|[dmmeta.Substr](/txt/ssimdb/dmmeta/substr.md)|[Base](/txt/ssimdb/dmmeta/substr.md)|||
|amc.FSubstr.p_field|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|amc.FSubstr.p_srcfield|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|amc.FSubstr.range|u64|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FSubstr
<a href="#struct-fsubstr"></a>
*Note:* field ``amc.FSubstr.msghdr`` has reftype ``base`` so the fields of [dmmeta.Substr](/txt/ssimdb/dmmeta/substr.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FSubstr { // amc.FSubstr
    amc::FSubstr*       zd_substr_params_next;       // zslist link; -1 means not-in-list
    amc::FSubstr*       zd_substr_params_prev;       // previous element
    algo::Smallstr100   field;                       //
    algo::CppExpr       expr;                        //
    algo::Smallstr100   srcfield;                    //
    amc::FField*        p_field;                     // reference to parent row
    amc::FField*        p_srcfield;                  // reference to parent row
    u64                 range;                       //   0
    bool                _db_c_substr_field_in_ary;   //   false  membership flag
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
|amc.FTargdep.p_target|[amc.FTarget](/txt/exe/amc/internals.md#amc-ftarget)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|amc.FTargdep.p_parent|[amc.FTarget](/txt/exe/amc/internals.md#amc-ftarget)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FTargdep
<a href="#struct-ftargdep"></a>
*Note:* field ``amc.FTargdep.msghdr`` has reftype ``base`` so the fields of [dev.Targdep](/txt/ssimdb/dev/targdep.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FTargdep { // amc.FTargdep
    algo::Smallstr50   targdep;                   // Primary key - target.parent
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
|amc.FTarget.c_targdep|[amc.FTargdep](/txt/exe/amc/internals.md#amc-ftargdep)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|amc.FTarget.p_ns|[amc.FNs](/txt/exe/amc/internals.md#amc-fns)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FTarget
<a href="#struct-ftarget"></a>
*Note:* field ``amc.FTarget.msghdr`` has reftype ``base`` so the fields of [dev.Target](/txt/ssimdb/dev/target.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FTarget { // amc.FTarget
    amc::FTarget*      ind_target_next;   // hash next
    algo::Smallstr16   target;            // Primary key - name of target
    amc::FTargdep**    c_targdep_elems;   // array of pointers
    u32                c_targdep_n;       // array of pointers
    u32                c_targdep_max;     // capacity of allocated array
    amc::FNs*          p_ns;              // reference to parent row
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
|amc.FTary.p_field|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FTary
<a href="#struct-ftary"></a>
*Note:* field ``amc.FTary.msghdr`` has reftype ``base`` so the fields of [dmmeta.Tary](/txt/ssimdb/dmmeta/tary.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FTary { // amc.FTary
    amc::FTary*         ind_tary_next;   // hash next
    algo::Smallstr100   field;           //
    bool                aliased;         //   false  Geneate functions to copy from aryptr
    amc::FField*        p_field;         // reference to parent row
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
    algo::Comment      comment;   //
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
|amc.FTfunc.step||[Hook](/txt/exe/amc/reftypes.md#hook)|||
|amc.FTfunc.c_tcurs|[amc.FTcurs](/txt/exe/amc/internals.md#amc-ftcurs)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||

#### Struct FTfunc
<a href="#struct-ftfunc"></a>
*Note:* field ``amc.FTfunc.msghdr`` has reftype ``base`` so the fields of [amcdb.Tfunc](/txt/ssimdb/amcdb/tfunc.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FTfunc { // amc.FTfunc
    amc::FTfunc*           ind_tfunc_next;          // hash next
    algo::Smallstr50       tfunc;                   //
    bool                   hasthrow;                //   false  Important defaults for new tfuncs to work
    bool                   leaf;                    //   true  Important defaults for new tfuncs to work
    bool                   poolfunc;                //   false
    bool                   inl;                     //   false  Function is typically inline
    bool                   wur;                     //   false  Warn unused result
    bool                   pure;                    //   false  OK to call fewer times than specified in text
    bool                   ismacro;                 //   false
    algo::Comment          comment;                 //
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
|amc.FThash.p_hashfld|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|amc.FThash.p_field|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FThash
<a href="#struct-fthash"></a>
*Note:* field ``amc.FThash.msghdr`` has reftype ``base`` so the fields of [dmmeta.Thash](/txt/ssimdb/dmmeta/thash.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FThash { // amc.FThash
    algo::Smallstr100   field;       //
    algo::Smallstr100   hashfld;     // Field on which hash function is computed
    bool                unique;      //   true  Enforce uniqueness of member elements
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
    algo::Comment       comment;    //
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
    algo::Comment      comment;    //
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
|amc.FTypefld.p_field|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|amc.FTypefld.p_ctype|[amc.FCtype](/txt/exe/amc/internals.md#amc-fctype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FTypefld
<a href="#struct-ftypefld"></a>
*Note:* field ``amc.FTypefld.msghdr`` has reftype ``base`` so the fields of [dmmeta.Typefld](/txt/ssimdb/dmmeta/typefld.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FTypefld { // amc.FTypefld
    algo::Smallstr100   field;     //
    algo::Comment       comment;   //
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
    algo::Comment       comment;    //
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
|amc.FXref.p_field|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)||Xref field|
|amc.FXref.p_ctype|[amc.FCtype](/txt/exe/amc/internals.md#amc-fctype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)||p_field->p_ctype|
|amc.FXref.c_nocascdel|[amc.FNocascdel](/txt/exe/amc/internals.md#amc-fnocascdel)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|amc.FXref.p_viafld|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|amc.FXref.p_keyfld|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FXref
<a href="#struct-fxref"></a>
*Note:* field ``amc.FXref.msghdr`` has reftype ``base`` so the fields of [dmmeta.Xref](/txt/ssimdb/dmmeta/xref.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/amc_gen.h](/include/gen/amc_gen.h)
```
struct FXref { // amc.FXref
    amc::FXref*         zs_xref_next;          // zslist link; -1 means not-in-list
    amc::FXref*         ind_xref_next;         // hash next
    amc::FXref*         zd_xref_keyfld_next;   // zslist link; -1 means not-in-list
    amc::FXref*         zd_xref_keyfld_prev;   // previous element
    algo::Smallstr100   field;                 //
    algo::CppExpr       inscond;               //   "true"  Insert condition
    algo::Smallstr200   via;                   //
    amc::FField*        p_field;               // reference to parent row
    amc::FCtype*        p_ctype;               // reference to parent row
    amc::FNocascdel*    c_nocascdel;           // optional pointer
    amc::FField*        p_viafld;              // reference to parent row
    amc::FField*        p_keyfld;              // reference to parent row
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

#### amc.Funcarg - 
<a href="#amc-funcarg"></a>

#### amc.Funcarg Fields
<a href="#amc-funcarg-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.Funcarg.type|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||C++ expression for declaration|
|amc.Funcarg.name|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Argument name|
|amc.Funcarg.dflt|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Initializer|
|amc.Funcarg.retval|bool|[Val](/txt/exe/amc/reftypes.md#val)||Is return value?|
|amc.Funcarg.initialized|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|amc.Funcarg.indent|bool|[Val](/txt/exe/amc/reftypes.md#val)||Formatting option for prototype|
|amc.Funcarg.typearg|bool|[Val](/txt/exe/amc/reftypes.md#val)|||

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

#### amc.Genpnew - 
<a href="#amc-genpnew"></a>

#### amc.Genpnew Fields
<a href="#amc-genpnew-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.Genpnew.p_func|[amc.FFunc](/txt/exe/amc/internals.md#amc-ffunc)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|amc.Genpnew.R|[algo_lib.Replscope](/txt/lib/algo_lib/README.md#algo_lib-replscope)|[Val](/txt/exe/amc/reftypes.md#val)|||
|amc.Genpnew.preamble|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|amc.Genpnew.postamble|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|amc.Genpnew.req_pack|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|amc.Genpnew.p_ctype|[amc.FCtype](/txt/exe/amc/internals.md#amc-fctype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|amc.Genpnew.p_optfld|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|amc.Genpnew.p_varlenfld|[amc.FField](/txt/exe/amc/internals.md#amc-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|amc.Genpnew.optnolen|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|amc.Genpnew.hasret|bool|[Val](/txt/exe/amc/reftypes.md#val)|true||

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

#### amc.Pnewtype - 
<a href="#amc-pnewtype"></a>

#### amc.Pnewtype Fields
<a href="#amc-pnewtype-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|amc.Pnewtype.value|u8|[Val](/txt/exe/amc/reftypes.md#val)|||

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

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Imdb -->


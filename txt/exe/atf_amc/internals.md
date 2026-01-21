## atf_amc - Internals


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
for usage, see [atf_amc - Unit tests for amc (see amctest table)](/txt/exe/atf_amc/README.md)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Description -->

### Sources
<a href="#sources"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Sources -->
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/atf_amc/atree.cpp](/cpp/atf_amc/atree.cpp)||
|[cpp/atf_amc/bheap.cpp](/cpp/atf_amc/bheap.cpp)||
|[cpp/atf_amc/bigend.cpp](/cpp/atf_amc/bigend.cpp)||
|[cpp/atf_amc/bitfld.cpp](/cpp/atf_amc/bitfld.cpp)||
|[cpp/atf_amc/bitset.cpp](/cpp/atf_amc/bitset.cpp)||
|[cpp/atf_amc/cascdel.cpp](/cpp/atf_amc/cascdel.cpp)||
|[cpp/atf_amc/cdlist.cpp](/cpp/atf_amc/cdlist.cpp)||
|[cpp/atf_amc/cleanup.cpp](/cpp/atf_amc/cleanup.cpp)||
|[cpp/atf_amc/cmp.cpp](/cpp/atf_amc/cmp.cpp)||
|[cpp/atf_amc/cslist.cpp](/cpp/atf_amc/cslist.cpp)||
|[cpp/atf_amc/delptr.cpp](/cpp/atf_amc/delptr.cpp)||
|[cpp/atf_amc/dispatch.cpp](/cpp/atf_amc/dispatch.cpp)||
|[cpp/atf_amc/exec.cpp](/cpp/atf_amc/exec.cpp)||
|[cpp/atf_amc/fbuf.cpp](/cpp/atf_amc/fbuf.cpp)||
|[cpp/atf_amc/fconst.cpp](/cpp/atf_amc/fconst.cpp)||
|[cpp/atf_amc/fstep.cpp](/cpp/atf_amc/fstep.cpp)||
|[cpp/atf_amc/gsymbol.cpp](/cpp/atf_amc/gsymbol.cpp)||
|[cpp/atf_amc/hook.cpp](/cpp/atf_amc/hook.cpp)||
|[cpp/atf_amc/inlary.cpp](/cpp/atf_amc/inlary.cpp)||
|[cpp/atf_amc/lary.cpp](/cpp/atf_amc/lary.cpp)||
|[cpp/atf_amc/lineiter.cpp](/cpp/atf_amc/lineiter.cpp)||
|[cpp/atf_amc/lpool.cpp](/cpp/atf_amc/lpool.cpp)||
|[cpp/atf_amc/main.cpp](/cpp/atf_amc/main.cpp)||
|[cpp/atf_amc/msgcurs.cpp](/cpp/atf_amc/msgcurs.cpp)||
|[cpp/atf_amc/numstr.cpp](/cpp/atf_amc/numstr.cpp)||
|[cpp/atf_amc/opt.cpp](/cpp/atf_amc/opt.cpp)||
|[cpp/atf_amc/pmask.cpp](/cpp/atf_amc/pmask.cpp)||
|[cpp/atf_amc/ptrary.cpp](/cpp/atf_amc/ptrary.cpp)||
|[cpp/atf_amc/readstr.cpp](/cpp/atf_amc/readstr.cpp)||
|[cpp/atf_amc/sort.cpp](/cpp/atf_amc/sort.cpp)||
|[cpp/atf_amc/strconv.cpp](/cpp/atf_amc/strconv.cpp)||
|[cpp/atf_amc/tary.cpp](/cpp/atf_amc/tary.cpp)||
|[cpp/atf_amc/thash.cpp](/cpp/atf_amc/thash.cpp)||
|[cpp/atf_amc/varlen.cpp](/cpp/atf_amc/varlen.cpp)||
|[cpp/atf_amc/zdlist.cpp](/cpp/atf_amc/zdlist.cpp)||
|[cpp/atf_amc/zslist.cpp](/cpp/atf_amc/zslist.cpp)||
|[cpp/gen/atf_amc_gen.cpp](/cpp/gen/atf_amc_gen.cpp)||
|[include/atf_amc.h](/include/atf_amc.h)||
|[include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)||
|[include/gen/atf_amc_gen.inl.h](/include/gen/atf_amc_gen.inl.h)||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Sources -->

### Dependencies
<a href="#dependencies"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Dependencies -->
The build target depends on the following libraries
|Target|Comment|
|---|---|
|[algo_lib](/txt/lib/algo_lib/README.md)|Support library for all executables|
|[lib_exec](/txt/lib/lib_exec/README.md)|Build and run a dag of subprocesses with N parallel jobs|
|[lib_json](/txt/lib/lib_json/README.md)|Full json support library|
|[lib_prot](/txt/lib/lib_prot/README.md)|Library covering all protocols|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Dependencies -->

### In Memory DB
<a href="#in-memory-db"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Imdb -->
`atf_amc` generated code creates the tables below.
All allocations are done through global `atf_amc::_db` [atf_amc.FDb](#atf_amc-fdb) structure
|Ctype|Ssimfile|Create|Access|
|---|---|---|---|
|[atf_amc.AmcCleanup2](#atf_amc-amccleanup2)||
|[atf_amc.AmcSubstr1](#atf_amc-amcsubstr1)||
|[atf_amc.BitfldType1](#atf_amc-bitfldtype1)||
|[atf_amc.BitfldType2](#atf_amc-bitfldtype2)||
|[atf_amc.BitfldU128](#atf_amc-bitfldu128)||
|[atf_amc.BitfldU16](#atf_amc-bitfldu16)||
|[atf_amc.Bitset](#atf_amc-bitset)||
|[atf_amc.Bytebuf](#atf_amc-bytebuf)||FDb.bytebuf (Cppstack)|
|[atf_amc.BytebufDyn](#atf_amc-bytebufdyn)||FDb.bytebuf_dyn (Cppstack)|
|[atf_amc.Cstr](#atf_amc-cstr)||FPerfSortString.orig (Tary)|FPerfSortString.sorted1 (Tary)|
|[atf_amc.Cstring](#atf_amc-cstring)||
||||FCstring.msghdr (Base)|
|[atf_amc.Ctype1Attr](#atf_amc-ctype1attr)||
|[atf_amc.Ctype2Attr](#atf_amc-ctype2attr)||
|[atf_amc.Ctype2AttrAnon](#atf_amc-ctype2attranon)||
|[atf_amc.DelType1](#atf_amc-deltype1)||
|[atf_amc.DispType1](#atf_amc-disptype1)||
|[atf_amc.DispType2](#atf_amc-disptype2)||
|[atf_amc.DispType3](#atf_amc-disptype3)||
|[atf_amc.FAmctest](#atf_amc-famctest)|[atfdb.amctest](/txt/ssimdb/atfdb/amctest.md)|FDb.amctest (Lary)|**static**|amctest (Lary, by rowid)|
|[atf_amc.FAvl](#atf_amc-favl)||FDb.avl (Lary)|avl (Lary, by rowid)|tr_avl (Atree, sort field n)|
|[atf_amc.FCascdel](#atf_amc-fcascdel)||FDb.cascdel (Tpool)|
||||FCascdel.p_parent (Upptr)|
||||FCascdel.child_ptr (Ptr)|
||||FCascdel.c_child_ptrary (Ptrary)|
||||FCascdel.ind_child_thash (Thash)|
||||FCascdel.bh_child_bheap (Bheap)|
||||FCascdel.zd_childlist (Llist)|
||||FCascdel.tr_child_atree (Atree)|
|[atf_amc.FCstring](#atf_amc-fcstring)||FDb.cstring (Lary)|cstring (Lary, by rowid)|ind_cstring (Thash, hash field str)|
|[atf_amc.FListtype](#atf_amc-flisttype)|[dmmeta.listtype](/txt/ssimdb/dmmeta/listtype.md)|FDb.listtype (Inlary)|**static**|
||||FListtype.step (Hook)|
|[atf_amc.FDb](#atf_amc-fdb)||FDb._db (Global)|
|[atf_amc.FHashableLinear](#atf_amc-fhashablelinear)||FDb.hashable_linear (Tpool)|ind_linear (Thash, hash field key)|
|[atf_amc.TypeG](#atf_amc-typeg)||OptAlloc.typeg (Opt)|OptG.typeg (Opt)|FOptG.typeg (Opt)|
|[atf_amc.FOptG](#atf_amc-foptg)||FDb.optg (Lpool)|
|[atf_amc.FPerfSortString](#atf_amc-fperfsortstring)||
|[atf_amc.FThashElem](#atf_amc-fthashelem)||FDb.thash_elem (Lary)|ind_thash_elem (Thash, hash field key)|thash_elem (Lary, by rowid)|
|[atf_amc.FTimeEntry](#atf_amc-ftimeentry)||FDb.time_entry (Tpool)|bh_time_entry (Bheap, sort field time)|
|[atf_amc.FTypeC](#atf_amc-ftypec)||FTypeA.typec (Inlary)|bh_typec (Bheap, sort field typec)|zdl_h_typec (Llist)|zd_t_typec (Llist)|zsl_h_typec (Llist)|zs_t_typec (Llist)|zs_mt_typec (ZSListMT)|zdl_hnt_typec (Llist)|cdl_h_typec (Llist)|cd_t_typec (Llist)|csl_h_typec (Llist)|cs_t_typec (Llist)|
|[atf_amc.FTypeA](#atf_amc-ftypea)||FDb.typea (Lary)|typea (Lary, by rowid)|ind_typea (Thash, hash field typea)|
|[atf_amc.FTypeB](#atf_amc-ftypeb)||FDb.typeb (Tpool)|
||||FTypeA.zdl_typeb (Llist)|
||||FTypeA.bh_typeb (Bheap)|
|[atf_amc.FTypeD](#atf_amc-ftyped)||FDb.typed (Tpool)|zd_typed (Llist)|zs_t_typed (Llist)|cd_typed (Llist)|
|[atf_amc.FTypeK](#atf_amc-ftypek)||c_typek (Ptrary)|
|[atf_amc.FTypeL](#atf_amc-ftypel)||c_typel (Ptrary)|
|[atf_amc.FTypeM](#atf_amc-ftypem)||c_typem (Ptrary)|
|[atf_amc.FTypeS](#atf_amc-ftypes)||FDb.types (Lary)|types (Lary, by rowid)|ind_types (Thash, hash field types)|
|[atf_amc.FTypeT](#atf_amc-ftypet)||FDb.typet (Lary)|typet (Lary, by rowid)|
||||FTypeS.zdl_typet (Llist)|
|[atf_amc.TypeA](#atf_amc-typea)||FUnitSort.tary (Tary)|FUnitSort.fixary (Inlary)|
||||FTypeA.msghdr (Base)|
||||FUnitSort.c_ptrary (Ptrary)|
|[atf_amc.FUnitSort](#atf_amc-funitsort)||
|[atf_amc.Hashable](#atf_amc-hashable)||FDb.hashable (Tpool)|ind_hashable (Thash, hash field key)|
|[atf_amc.Hooktype](#atf_amc-hooktype)||
||||Hooktype.callback (Hook)|
|[atf_amc.InlaryPrint](#atf_amc-inlaryprint)||
|[atf_amc.Lary32](#atf_amc-lary32)||
|[atf_amc.Linebuf](#atf_amc-linebuf)||FDb.linebuf (Cppstack)|
|[atf_amc.MsgHdrLT](#atf_amc-msghdrlt)||MsgLTO.o (Opt)|MsgLTV.v (Varlen)|Msgbuf.out_extra (Fbuf)|Msgbuf.in_extra (Fbuf)|
||||MsgLTA.base (Base)|
||||MsgLTB.base (Base)|
||||MsgLTO.base (Base)|
||||MsgLTV.base (Base)|
||||MsgHdrLT_curs.msg (Ptr)|
|[atf_amc.MsgType](#atf_amc-msgtype)||
|[atf_amc.MsgLength](#atf_amc-msglength)||
|[atf_amc.MsgHeader](#atf_amc-msgheader)||Msgbuf.in_buf (Fbuf)|Seqmsg.payload (Opt)|Varlen2mMsg.m1 (Varlen)|Varlen2mMsg.m2 (Varlen)|Varlen2mMsg.m3 (Varlen)|
||||Seqmsg.msghdr (Base)|
||||Text.msghdr (Base)|
||||Varlen2Msg.base (Base)|
||||Varlen2aMsg.base (Base)|
||||Varlen2mMsg.base (Base)|
||||Varlen2vMsg.base (Base)|
||||VarlenMsg.base (Base)|
||||MsgHeader_curs.msg (Ptr)|
|[atf_amc.MsgLTA](#atf_amc-msglta)||
|[atf_amc.MsgLTB](#atf_amc-msgltb)||
|[atf_amc.MsgLTO](#atf_amc-msglto)||
|[atf_amc.MsgLTV](#atf_amc-msgltv)||
|[atf_amc.Msgbuf](#atf_amc-msgbuf)||FDb.msgbuf (Cppstack)|cd_in_msg (Llist)|
|[atf_amc.NetBitfld1](#atf_amc-netbitfld1)||
|[atf_amc.OptAlloc](#atf_amc-optalloc)||FDb.optalloc (Lpool)|
||||OptAlloc_curs.msg (Ptr)|
|[atf_amc.OptG](#atf_amc-optg)||OptOptG.optg (Opt)|
||||FOptG.optg (Base)|
||||OptG_curs.msg (Ptr)|
|[atf_amc.OptOptG](#atf_amc-optoptg)||
||||OptOptG_curs.msg (Ptr)|
|[atf_amc.PmaskMultiple](#atf_amc-pmaskmultiple)||
|[atf_amc.PmaskU128](#atf_amc-pmasku128)||
|[atf_amc.PmaskU32](#atf_amc-pmasku32)||
|[atf_amc.PmaskU555](#atf_amc-pmasku555)||
|[atf_amc.PooledBE64](#atf_amc-pooledbe64)||FDb.pooledbe64 (Tpool)|
|[atf_amc.RnullStr6_U32](#atf_amc-rnullstr6_u32)||
|[atf_amc.RpasU32Str6](#atf_amc-rpasu32str6)||
|[atf_amc.Sep1](#atf_amc-sep1)||
|[atf_amc.Seqmsg](#atf_amc-seqmsg)||
|[atf_amc.SortedStr](#atf_amc-sortedstr)||
|[atf_amc.Strkey](#atf_amc-strkey)||FDb.strkey (Tpool)|ind_strkey (Thash, hash field strkey)|
|[atf_amc.TaryU32](#atf_amc-taryu32)||
|[atf_amc.TaryU8](#atf_amc-taryu8)||
|[atf_amc.TestRegx1](#atf_amc-testregx1)||
|[atf_amc.TestType](#atf_amc-testtype)||
|[atf_amc.Text](#atf_amc-text)||
|[atf_amc.TypeB](#atf_amc-typeb)||
||||FTypeB.msghdr (Base)|
|[atf_amc.TypeBE16](#atf_amc-typebe16)||
|[atf_amc.TypeBE32](#atf_amc-typebe32)||
|[atf_amc.TypeBE32en](#atf_amc-typebe32en)||
|[atf_amc.TypeBE64](#atf_amc-typebe64)||
|[atf_amc.TypeBE64dflt](#atf_amc-typebe64dflt)||
|[atf_amc.TypeBE64sf](#atf_amc-typebe64sf)||
|[atf_amc.TypeC](#atf_amc-typec)||
||||FTypeC.msghdr (Base)|
||||FTypeD.msghdr (Base)|
|[atf_amc.TypeH](#atf_amc-typeh)||VarlenH.typeh (Varlen)|
|[atf_amc.TypeS](#atf_amc-types)||
||||FTypeS.msghdr (Base)|
|[atf_amc.TypeT](#atf_amc-typet)||
||||FTypeT.msghdr (Base)|
|[atf_amc.TypeTVal](#atf_amc-typetval)||
|[atf_amc.Typefconst](#atf_amc-typefconst)||
|[atf_amc.Varlen2Msg](#atf_amc-varlen2msg)||
|[atf_amc.Varlen2aMsg](#atf_amc-varlen2amsg)||
|[atf_amc.Varlen2mMsg](#atf_amc-varlen2mmsg)||
|[atf_amc.VarlenK](#atf_amc-varlenk)||Varlen2vMsg.v1 (Varlen)|Varlen2vMsg.v2 (Varlen)|Varlen2vMsg.v3 (Varlen)|VarlenMsg.k (Varlen)|
||||VarlenK_curs.msg (Ptr)|
|[atf_amc.Varlen2vMsg](#atf_amc-varlen2vmsg)||
|[atf_amc.VarlenAlloc](#atf_amc-varlenalloc)||FDb.varlenalloc (Lpool)|
||||VarlenAlloc_curs.msg (Ptr)|
|[atf_amc.VarlenExtern](#atf_amc-varlenextern)||FDb.varlen_extern (Lpool)|
||||VarlenExtern_curs.msg (Ptr)|
|[atf_amc.VarlenH](#atf_amc-varlenh)||
||||VarlenH_curs.msg (Ptr)|
|[atf_amc.VarlenMsg](#atf_amc-varlenmsg)||

#### atf_amc.AmcCleanup2 - 
<a href="#atf_amc-amccleanup2"></a>

#### atf_amc.AmcCleanup2 Fields
<a href="#atf_amc-amccleanup2-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.AmcCleanup2.field1|i32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.AmcCleanup2.field2|i32|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct AmcCleanup2
<a href="#struct-amccleanup2"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct AmcCleanup2 { // atf_amc.AmcCleanup2
    i32   field1;   //   0
    i32   field2;   //   0
    // func:atf_amc.AmcCleanup2..Ctor
    inline               AmcCleanup2() __attribute__((nothrow));
    // func:atf_amc.AmcCleanup2..Dtor
    inline               ~AmcCleanup2() __attribute__((nothrow));
};
```

#### atf_amc.AmcSubstr1 - 
<a href="#atf_amc-amcsubstr1"></a>

#### atf_amc.AmcSubstr1 Fields
<a href="#atf_amc-amcsubstr1-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.AmcSubstr1.strval|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|*atf_amc.AmcSubstr1.boolval (.LR)*|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|*atf_amc.AmcSubstr1.intval (.LR)*|i32|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct AmcSubstr1
<a href="#struct-amcsubstr1"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct AmcSubstr1 { // atf_amc.AmcSubstr1
    algo::cstring   strval;   //
    // func:atf_amc.AmcSubstr1..Ctor
    inline               AmcSubstr1() __attribute__((nothrow));
};
```

#### atf_amc.BitfldType1 - Test type for testing reading/printing bitfields to tuple
<a href="#atf_amc-bitfldtype1"></a>

#### atf_amc.BitfldType1 Fields
<a href="#atf_amc-bitfldtype1-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.BitfldType1.value|u64|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.BitfldType1.bit1|u64|[Bitfld](/txt/exe/amc/reftypes.md#bitfld)|||
|atf_amc.BitfldType1.bits5|u64|[Bitfld](/txt/exe/amc/reftypes.md#bitfld)|||

#### Struct BitfldType1
<a href="#struct-bitfldtype1"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct BitfldType1 { // atf_amc.BitfldType1: Test type for testing reading/printing bitfields to tuple
    u64   value;   //   0
    // func:atf_amc.BitfldType1..Ctor
    inline               BitfldType1() __attribute__((nothrow));
};
```

#### atf_amc.BitfldType2 - Test type 2 for testing reading/printing bitfields to tuple
<a href="#atf_amc-bitfldtype2"></a>

#### atf_amc.BitfldType2 Fields
<a href="#atf_amc-bitfldtype2-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.BitfldType2.value|u64|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.BitfldType2.bit0|bool|[Bitfld](/txt/exe/amc/reftypes.md#bitfld)|||
|atf_amc.BitfldType2.bit1|bool|[Bitfld](/txt/exe/amc/reftypes.md#bitfld)|||
|atf_amc.BitfldType2.freebool|bool|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct BitfldType2
<a href="#struct-bitfldtype2"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct BitfldType2 { // atf_amc.BitfldType2: Test type 2 for testing reading/printing bitfields to tuple
    u64    value;      //   0
    bool   freebool;   //   false
    // func:atf_amc.BitfldType2..Ctor
    inline               BitfldType2() __attribute__((nothrow));
};
```

#### atf_amc.BitfldU128 - Test of bitfield functionality
<a href="#atf_amc-bitfldu128"></a>

#### atf_amc.BitfldU128 Fields
<a href="#atf_amc-bitfldu128-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.BitfldU128.value|u128|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.BitfldU128.bits1_65|u64|[Bitfld](/txt/exe/amc/reftypes.md#bitfld)|||
|atf_amc.BitfldU128.bits65_128|u64|[Bitfld](/txt/exe/amc/reftypes.md#bitfld)|||

#### Struct BitfldU128
<a href="#struct-bitfldu128"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct BitfldU128 { // atf_amc.BitfldU128: Test of bitfield functionality
    u128   value;   //   0
    // func:atf_amc.BitfldU128..Ctor
    inline               BitfldU128() __attribute__((nothrow));
};
```

#### atf_amc.BitfldU16 - Test of bitfield functionality
<a href="#atf_amc-bitfldu16"></a>

#### atf_amc.BitfldU16 Fields
<a href="#atf_amc-bitfldu16-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.BitfldU16.value|u16|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.BitfldU16.bits0_4|u8|[Bitfld](/txt/exe/amc/reftypes.md#bitfld)|||
|atf_amc.BitfldU16.bits8_12|u8|[Bitfld](/txt/exe/amc/reftypes.md#bitfld)|||

#### Struct BitfldU16
<a href="#struct-bitfldu16"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct BitfldU16 { // atf_amc.BitfldU16: Test of bitfield functionality
    u16   value;   //   0
    // func:atf_amc.BitfldU16..Ctor
    inline               BitfldU16() __attribute__((nothrow));
};
```

#### atf_amc.Bitset - Test bitset
<a href="#atf_amc-bitset"></a>

#### atf_amc.Bitset Fields
<a href="#atf_amc-bitset-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.Bitset.fld1|u16|[Inlary](/txt/exe/amc/reftypes.md#inlary)||Bitset array|
|atf_amc.Bitset.fld8|u8|[Val](/txt/exe/amc/reftypes.md#val)||Bitset field|
|atf_amc.Bitset.fld64|u64|[Val](/txt/exe/amc/reftypes.md#val)||Bitset field|
|atf_amc.Bitset.fld128|u128|[Val](/txt/exe/amc/reftypes.md#val)||Bitset field 128 bits|

#### Struct Bitset
<a href="#struct-bitset"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct Bitset { // atf_amc.Bitset: Test bitset
    u16    fld1_elems[4];   //   0  fixed array
    u8     fld8;            //   0  Bitset field
    u64    fld64;           //   0  Bitset field
    u128   fld128;          //   0  Bitset field 128 bits
    // func:atf_amc.Bitset..Ctor
    inline               Bitset() __attribute__((nothrow));
};
```

#### atf_amc.Bytebuf - 
<a href="#atf_amc-bytebuf"></a>

#### atf_amc.Bytebuf Fields
<a href="#atf_amc-bytebuf-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.Bytebuf.in|char|[Fbuf](/txt/exe/amc/reftypes.md#fbuf)||Streaming buffer|

#### Struct Bytebuf
<a href="#struct-bytebuf"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct Bytebuf { // atf_amc.Bytebuf
    u8*             in_elems;          //   NULL  pointer to elements of indirect array
    u32             in_max;            //   0  current length of allocated array
    i32             in_start;          // beginning of valid bytes (in bytes)
    i32             in_end;            // end of valid bytes (in bytes)
    i32             in_msglen;         // current message length
    algo::Errcode   in_err;            // system error code
    bool            in_eof;            // no more data will be written to buffer
    bool            in_msgvalid;       // current message is valid
    bool            in_epoll_enable;   // use epoll?
    // func:atf_amc.Bytebuf..Ctor
    inline               Bytebuf() __attribute__((nothrow));
    // func:atf_amc.Bytebuf..Dtor
    inline               ~Bytebuf() __attribute__((nothrow));
};
```

#### atf_amc.BytebufDyn - 
<a href="#atf_amc-bytebufdyn"></a>

#### atf_amc.BytebufDyn Fields
<a href="#atf_amc-bytebufdyn-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.BytebufDyn.in|char|[Fbuf](/txt/exe/amc/reftypes.md#fbuf)||Streaming buffer|

#### Struct BytebufDyn
<a href="#struct-bytebufdyn"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct BytebufDyn { // atf_amc.BytebufDyn
    u8*             in_elems;          //   NULL  pointer to elements of indirect array
    u32             in_max;            //   0  current length of allocated array
    i32             in_start;          // beginning of valid bytes (in bytes)
    i32             in_end;            // end of valid bytes (in bytes)
    i32             in_msglen;         // current message length
    algo::Errcode   in_err;            // system error code
    bool            in_eof;            // no more data will be written to buffer
    bool            in_msgvalid;       // current message is valid
    bool            in_epoll_enable;   // use epoll?
    // func:atf_amc.BytebufDyn..Ctor
    inline               BytebufDyn() __attribute__((nothrow));
    // func:atf_amc.BytebufDyn..Dtor
    inline               ~BytebufDyn() __attribute__((nothrow));
};
```

#### atf_amc.Cstr - 
<a href="#atf_amc-cstr"></a>

#### atf_amc.Cstr Fields
<a href="#atf_amc-cstr-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.Cstr.val|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct Cstr
<a href="#struct-cstr"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct Cstr { // atf_amc.Cstr
    algo::cstring   val;   //
    // func:atf_amc.Cstr.val.Cast
    inline               operator algo::strptr() const __attribute__((nothrow));
    // func:atf_amc.Cstr..EqOp
    inline bool          operator ==(const atf_amc::Cstr &rhs) const __attribute__((nothrow));
    // func:atf_amc.Cstr..NeOp
    inline bool          operator !=(const atf_amc::Cstr &rhs) const __attribute__((nothrow));
    // func:atf_amc.Cstr..LtOp
    inline bool          operator <(const atf_amc::Cstr &rhs) const __attribute__((nothrow));
    // func:atf_amc.Cstr..GtOp
    inline bool          operator >(const atf_amc::Cstr &rhs) const __attribute__((nothrow));
    // func:atf_amc.Cstr..LeOp
    inline bool          operator <=(const atf_amc::Cstr &rhs) const __attribute__((nothrow));
    // func:atf_amc.Cstr..GeOp
    inline bool          operator >=(const atf_amc::Cstr &rhs) const __attribute__((nothrow));
    // func:atf_amc.Cstr..Ctor
    inline               Cstr() __attribute__((nothrow));
    // func:atf_amc.Cstr..FieldwiseCtor
    explicit inline               Cstr(const algo::strptr& in_val) __attribute__((nothrow));
};
```

#### atf_amc.Cstring - 
<a href="#atf_amc-cstring"></a>

#### atf_amc.Cstring Fields
<a href="#atf_amc-cstring-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.Cstring.str|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct Cstring
<a href="#struct-cstring"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct Cstring { // atf_amc.Cstring
    algo::cstring   str;   //
    // func:atf_amc.Cstring..Ctor
    inline               Cstring() __attribute__((nothrow));
};
```

#### atf_amc.Ctype1Attr - 
<a href="#atf_amc-ctype1attr"></a>

#### atf_amc.Ctype1Attr Fields
<a href="#atf_amc-ctype1attr-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.Ctype1Attr.attr1|u32|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct Ctype1Attr
<a href="#struct-ctype1attr"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct Ctype1Attr { // atf_amc.Ctype1Attr
    u32   attr1;   //   0
    // func:atf_amc.Ctype1Attr.attr1.Cast
    inline               operator u32() const __attribute__((nothrow));
    // func:atf_amc.Ctype1Attr..EqOp
    inline bool          operator ==(const atf_amc::Ctype1Attr &rhs) const __attribute__((nothrow));
    // func:atf_amc.Ctype1Attr..NeOp
    inline bool          operator !=(const atf_amc::Ctype1Attr &rhs) const __attribute__((nothrow));
    // func:atf_amc.Ctype1Attr..LtOp
    inline bool          operator <(const atf_amc::Ctype1Attr &rhs) const __attribute__((nothrow));
    // func:atf_amc.Ctype1Attr..GtOp
    inline bool          operator >(const atf_amc::Ctype1Attr &rhs) const __attribute__((nothrow));
    // func:atf_amc.Ctype1Attr..LeOp
    inline bool          operator <=(const atf_amc::Ctype1Attr &rhs) const __attribute__((nothrow));
    // func:atf_amc.Ctype1Attr..GeOp
    inline bool          operator >=(const atf_amc::Ctype1Attr &rhs) const __attribute__((nothrow));
    // func:atf_amc.Ctype1Attr..Ctor
    inline               Ctype1Attr() __attribute__((nothrow));
    // func:atf_amc.Ctype1Attr..FieldwiseCtor
    explicit inline               Ctype1Attr(u32 in_attr1) __attribute__((nothrow));
};
```

#### atf_amc.Ctype2Attr - 
<a href="#atf_amc-ctype2attr"></a>

#### atf_amc.Ctype2Attr Fields
<a href="#atf_amc-ctype2attr-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.Ctype2Attr.attr1|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.Ctype2Attr.attr2|u32|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct Ctype2Attr
<a href="#struct-ctype2attr"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct Ctype2Attr { // atf_amc.Ctype2Attr
    u32   attr1;   //   0
    u32   attr2;   //   0
    // func:atf_amc.Ctype2Attr..EqOp
    inline bool          operator ==(const atf_amc::Ctype2Attr &rhs) const __attribute__((nothrow));
    // func:atf_amc.Ctype2Attr..NeOp
    inline bool          operator !=(const atf_amc::Ctype2Attr &rhs) const __attribute__((nothrow));
    // func:atf_amc.Ctype2Attr..LtOp
    inline bool          operator <(const atf_amc::Ctype2Attr &rhs) const __attribute__((nothrow));
    // func:atf_amc.Ctype2Attr..GtOp
    inline bool          operator >(const atf_amc::Ctype2Attr &rhs) const __attribute__((nothrow));
    // func:atf_amc.Ctype2Attr..LeOp
    inline bool          operator <=(const atf_amc::Ctype2Attr &rhs) const __attribute__((nothrow));
    // func:atf_amc.Ctype2Attr..GeOp
    inline bool          operator >=(const atf_amc::Ctype2Attr &rhs) const __attribute__((nothrow));
    // func:atf_amc.Ctype2Attr..Ctor
    inline               Ctype2Attr() __attribute__((nothrow));
    // func:atf_amc.Ctype2Attr..FieldwiseCtor
    explicit inline               Ctype2Attr(u32 in_attr1, u32 in_attr2) __attribute__((nothrow));
};
```

#### atf_amc.Ctype2AttrAnon - 
<a href="#atf_amc-ctype2attranon"></a>

#### atf_amc.Ctype2AttrAnon Fields
<a href="#atf_amc-ctype2attranon-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.Ctype2AttrAnon.attr1|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.Ctype2AttrAnon.attr2|u32|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct Ctype2AttrAnon
<a href="#struct-ctype2attranon"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct Ctype2AttrAnon { // atf_amc.Ctype2AttrAnon
    u32   attr1;   //   0
    u32   attr2;   //   0
    // func:atf_amc.Ctype2AttrAnon..Ctor
    inline               Ctype2AttrAnon() __attribute__((nothrow));
    // func:atf_amc.Ctype2AttrAnon..FieldwiseCtor
    explicit inline               Ctype2AttrAnon(u32 in_attr1, u32 in_attr2) __attribute__((nothrow));
};
```

#### atf_amc.DelType1 - Delptr test 1
<a href="#atf_amc-deltype1"></a>

#### atf_amc.DelType1 Fields
<a href="#atf_amc-deltype1-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.DelType1.u32val|u32|[Delptr](/txt/exe/amc/reftypes.md#delptr)|34||

#### Struct DelType1
<a href="#struct-deltype1"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct DelType1 { // atf_amc.DelType1: Delptr test 1
    u32*   u32val;   // Private pointer to value
    // func:atf_amc.DelType1..Ctor
    inline               DelType1() __attribute__((nothrow));
    // func:atf_amc.DelType1..Dtor
    inline               ~DelType1() __attribute__((nothrow));
};
```

#### atf_amc.DispType1 - Dispfilter test 1
<a href="#atf_amc-disptype1"></a>

#### atf_amc.DispType1 Fields
<a href="#atf_amc-disptype1-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.DispType1.strval|[algo.Smallstr20](/txt/protocol/algo/README.md#algo-smallstr20)|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.DispType1.strval2|[algo.Smallstr20](/txt/protocol/algo/README.md#algo-smallstr20)|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.DispType1.dateval|[algo.UnTime](/txt/protocol/algo/UnTime.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct DispType1
<a href="#struct-disptype1"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct DispType1 { // atf_amc.DispType1: Dispfilter test 1
    algo::Smallstr20   strval;    //
    algo::Smallstr20   strval2;   //
    algo::UnTime       dateval;   //
    // func:atf_amc.DispType1..Ctor
    inline               DispType1() __attribute__((nothrow));
};
```

#### atf_amc.DispType2 - Dispfilter test 2
<a href="#atf_amc-disptype2"></a>

#### atf_amc.DispType2 Fields
<a href="#atf_amc-disptype2-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.DispType2.intval|u32|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct DispType2
<a href="#struct-disptype2"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct DispType2 { // atf_amc.DispType2: Dispfilter test 2
    u32   intval;   //   0
    // func:atf_amc.DispType2..Ctor
    inline               DispType2() __attribute__((nothrow));
};
```

#### atf_amc.DispType3 - Dispfilter test 3
<a href="#atf_amc-disptype3"></a>

#### atf_amc.DispType3 Fields
<a href="#atf_amc-disptype3-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.DispType3.intval|u32|[Val](/txt/exe/amc/reftypes.md#val)||Repeats type in DispType2|
|atf_amc.DispType3.strval|[algo.Smallstr20](/txt/protocol/algo/README.md#algo-smallstr20)|[Val](/txt/exe/amc/reftypes.md#val)||Repeats type in DispType1|

#### Struct DispType3
<a href="#struct-disptype3"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct DispType3 { // atf_amc.DispType3: Dispfilter test 3
    u32                intval;   //   0  Repeats type in DispType2
    algo::Smallstr20   strval;   // Repeats type in DispType1
    // func:atf_amc.DispType3..Ctor
    inline               DispType3() __attribute__((nothrow));
};
```

#### atf_amc.FAmctest - Test function
<a href="#atf_amc-famctest"></a>

#### atf_amc.FAmctest Fields
<a href="#atf_amc-famctest-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.FAmctest.msghdr|[atfdb.Amctest](/txt/ssimdb/atfdb/amctest.md)|[Base](/txt/ssimdb/atfdb/amctest.md)|||
|atf_amc.FAmctest.select|bool|[Val](/txt/exe/amc/reftypes.md#val)||Select test for running?|
|atf_amc.FAmctest.success|bool|[Val](/txt/exe/amc/reftypes.md#val)||Did it succeed?|
|atf_amc.FAmctest.step||[Hook](/txt/exe/amc/reftypes.md#hook)|||
|atf_amc.FAmctest.c_syscmd|[lib_exec.FSyscmd](/txt/lib/lib_exec/README.md#lib_exec-fsyscmd)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||

#### Struct FAmctest
<a href="#struct-famctest"></a>
*Note:* field ``atf_amc.FAmctest.msghdr`` has reftype ``base`` so the fields of [atfdb.Amctest](/txt/ssimdb/atfdb/amctest.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct FAmctest { // atf_amc.FAmctest: Test function
    algo::Smallstr50             amctest;    //
    algo::Comment                comment;    //
    bool                         select;     //   false  Select test for running?
    bool                         success;    //   false  Did it succeed?
    atf_amc::amctest_step_hook   step;       //   NULL  Pointer to a function
    lib_exec::FSyscmd*           c_syscmd;   // optional pointer
private:
    // func:atf_amc.FAmctest..Ctor
    inline               FAmctest() __attribute__((nothrow));
    friend atf_amc::FAmctest&   amctest_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend atf_amc::FAmctest*   amctest_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 amctest_RemoveAll() __attribute__((nothrow));
    friend void                 amctest_RemoveLast() __attribute__((nothrow));
};
```

#### atf_amc.FAvl - 
<a href="#atf_amc-favl"></a>

#### atf_amc.FAvl Fields
<a href="#atf_amc-favl-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.FAvl.n|i32|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FAvl
<a href="#struct-favl"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct FAvl { // atf_amc.FAvl
    i32              n;              //   0
    atf_amc::FAvl*   tr_avl_up;      // pointer to parent
    atf_amc::FAvl*   tr_avl_left;    // Left child
    atf_amc::FAvl*   tr_avl_right;   // Right child
    i32              tr_avl_depth;   // Depth
    // func:atf_amc.FAvl..AssignOp
    inline atf_amc::FAvl& operator =(const atf_amc::FAvl &rhs) = delete;
    // func:atf_amc.FAvl..CopyCtor
    inline               FAvl(const atf_amc::FAvl &rhs) = delete;
private:
    // func:atf_amc.FAvl..Ctor
    inline               FAvl() __attribute__((nothrow));
    // func:atf_amc.FAvl..Dtor
    inline               ~FAvl() __attribute__((nothrow));
    friend atf_amc::FAvl&       avl_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend atf_amc::FAvl*       avl_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 avl_RemoveAll() __attribute__((nothrow));
    friend void                 avl_RemoveLast() __attribute__((nothrow));
};
```

#### atf_amc.FCascdel - Test type for cascade delete
<a href="#atf_amc-fcascdel"></a>

#### atf_amc.FCascdel Fields
<a href="#atf_amc-fcascdel-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.FCascdel.p_parent|[atf_amc.FCascdel](/txt/exe/atf_amc/internals.md#atf_amc-fcascdel)|[Upptr](/txt/exe/amc/reftypes.md#upptr)||Parent for xref|
|atf_amc.FCascdel.key|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.FCascdel.type|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.FCascdel.child_ptr|[atf_amc.FCascdel](/txt/exe/atf_amc/internals.md#atf_amc-fcascdel)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|atf_amc.FCascdel.c_child_ptrary|[atf_amc.FCascdel](/txt/exe/atf_amc/internals.md#atf_amc-fcascdel)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|atf_amc.FCascdel.ind_child_thash|[atf_amc.FCascdel](/txt/exe/atf_amc/internals.md#atf_amc-fcascdel)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|atf_amc.FCascdel.bh_child_bheap|[atf_amc.FCascdel](/txt/exe/atf_amc/internals.md#atf_amc-fcascdel)|[Bheap](/txt/exe/amc/reftypes.md#bheap)|||
|atf_amc.FCascdel.zd_childlist|[atf_amc.FCascdel](/txt/exe/atf_amc/internals.md#atf_amc-fcascdel)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|atf_amc.FCascdel.tr_child_atree|[atf_amc.FCascdel](/txt/exe/atf_amc/internals.md#atf_amc-fcascdel)|[Atree](/txt/exe/amc/reftypes.md#atree)|||

#### Struct FCascdel
<a href="#struct-fcascdel"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct FCascdel { // atf_amc.FCascdel: Test type for cascade delete
    atf_amc::FCascdel*    p_parent;                          // reference to parent row
    u32                   key;                               //   0
    u32                   type;                              //   0
    atf_amc::FCascdel*    child_ptr;                         // optional pointer
    atf_amc::FCascdel**   c_child_ptrary_elems;              // array of pointers
    u32                   c_child_ptrary_n;                  // array of pointers
    u32                   c_child_ptrary_max;                // capacity of allocated array
    atf_amc::FCascdel**   ind_child_thash_buckets_elems;     // pointer to bucket array
    i32                   ind_child_thash_buckets_n;         // number of elements in bucket array
    i32                   ind_child_thash_n;                 // number of elements in the hash table
    atf_amc::FCascdel*    cascdel_ind_child_thash_next;      // hash next
    u32                   cascdel_ind_child_thash_hashval;   // hash value
    atf_amc::FCascdel**   bh_child_bheap_elems;              // binary heap by key
    i32                   bh_child_bheap_n;                  // number of elements in the heap
    i32                   bh_child_bheap_max;                // max elements in bh_child_bheap_elems
    i32                   cascdel_bh_child_bheap_idx;        // index in heap; -1 means not-in-heap
    atf_amc::FCascdel*    zd_childlist_head;                 // zero-terminated doubly linked list
    i32                   zd_childlist_n;                    // zero-terminated doubly linked list
    atf_amc::FCascdel*    zd_childlist_tail;                 // pointer to last element
    atf_amc::FCascdel*    cascdel_zd_childlist_next;         // zslist link; -1 means not-in-list
    atf_amc::FCascdel*    cascdel_zd_childlist_prev;         // previous element
    atf_amc::FCascdel*    tr_child_atree_root;               // Root of the tree
    i32                   tr_child_atree_n;                  // number of elements in the tree
    atf_amc::FCascdel*    cascdel_tr_child_atree_up;         // pointer to parent
    atf_amc::FCascdel*    cascdel_tr_child_atree_left;       // Left child
    atf_amc::FCascdel*    cascdel_tr_child_atree_right;      // Right child
    i32                   cascdel_tr_child_atree_depth;      // Depth
    bool                  cascdel_c_child_ptrary_in_ary;     //   false  membership flag
    atf_amc::FCascdel*    cascdel_next;                      // Pointer to next free element int tpool
    // x-reference on atf_amc.FCascdel.child_ptr prevents copy
    // reftype Ptrary of atf_amc.FCascdel.c_child_ptrary prohibits copy
    // reftype Thash of atf_amc.FCascdel.ind_child_thash prohibits copy
    // reftype Bheap of atf_amc.FCascdel.bh_child_bheap prohibits copy
    // reftype Llist of atf_amc.FCascdel.zd_childlist prohibits copy
    // reftype Atree of atf_amc.FCascdel.tr_child_atree prohibits copy
    // func:atf_amc.FCascdel..AssignOp
    atf_amc::FCascdel&   operator =(const atf_amc::FCascdel &rhs) = delete;
    // x-reference on atf_amc.FCascdel.child_ptr prevents copy
    // reftype Ptrary of atf_amc.FCascdel.c_child_ptrary prohibits copy
    // reftype Thash of atf_amc.FCascdel.ind_child_thash prohibits copy
    // reftype Bheap of atf_amc.FCascdel.bh_child_bheap prohibits copy
    // reftype Llist of atf_amc.FCascdel.zd_childlist prohibits copy
    // reftype Atree of atf_amc.FCascdel.tr_child_atree prohibits copy
    // func:atf_amc.FCascdel..CopyCtor
    FCascdel(const atf_amc::FCascdel &rhs) = delete;
private:
    // func:atf_amc.FCascdel..Ctor
    inline               FCascdel() __attribute__((nothrow));
    // func:atf_amc.FCascdel..Dtor
    inline               ~FCascdel() __attribute__((nothrow));
    friend atf_amc::FCascdel&   cascdel_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend atf_amc::FCascdel*   cascdel_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 cascdel_Delete(atf_amc::FCascdel &row) __attribute__((nothrow));
};
```

#### atf_amc.FCstring - 
<a href="#atf_amc-fcstring"></a>

#### atf_amc.FCstring Fields
<a href="#atf_amc-fcstring-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.FCstring.msghdr|[atf_amc.Cstring](/txt/exe/atf_amc/internals.md#atf_amc-cstring)|[Base](#atf_amc-cstring-fields)|||
#### atf_amc.Cstring Fields
<a href="#atf_amc-cstring-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.Cstring.str|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FCstring
<a href="#struct-fcstring"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct FCstring { // atf_amc.FCstring
    algo::cstring        str;                   //
    atf_amc::FCstring*   ind_cstring_next;      // hash next
    u32                  ind_cstring_hashval;   // hash value
    // func:atf_amc.FCstring..AssignOp
    inline atf_amc::FCstring& operator =(const atf_amc::FCstring &rhs) = delete;
    // func:atf_amc.FCstring..CopyCtor
    inline               FCstring(const atf_amc::FCstring &rhs) = delete;
private:
    // func:atf_amc.FCstring..Ctor
    inline               FCstring() __attribute__((nothrow));
    // func:atf_amc.FCstring..Dtor
    inline               ~FCstring() __attribute__((nothrow));
    friend atf_amc::FCstring&   cstring_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend atf_amc::FCstring*   cstring_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 cstring_RemoveAll() __attribute__((nothrow));
    friend void                 cstring_RemoveLast() __attribute__((nothrow));
};
```

#### atf_amc.FListtype - Specify structure of linked list based on field prefix
<a href="#atf_amc-flisttype"></a>

#### atf_amc.FListtype Fields
<a href="#atf_amc-flisttype-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.FListtype.base|[dmmeta.Listtype](/txt/ssimdb/dmmeta/listtype.md)|[Base](/txt/ssimdb/dmmeta/listtype.md)|||
|atf_amc.FListtype.step|[atf_amc.FListtype](/txt/exe/atf_amc/internals.md#atf_amc-flisttype)|[Hook](/txt/exe/amc/reftypes.md#hook)|||
|atf_amc.FListtype.seen|bool|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FListtype
<a href="#struct-flisttype"></a>
*Note:* field ``atf_amc.FListtype.base`` has reftype ``base`` so the fields of [dmmeta.Listtype](/txt/ssimdb/dmmeta/listtype.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct FListtype { // atf_amc.FListtype
    algo::Smallstr5               listtype;   //
    bool                          circular;   //   false  Circular list
    bool                          haveprev;   //   false  Previous link
    bool                          instail;    //   false  Queue
    algo::Comment                 comment;    //
    atf_amc::listtype_step_hook   step;       //   NULL  Pointer to a function
    bool                          seen;       //   false
    // func:atf_amc.FListtype..Ctor
    inline               FListtype() __attribute__((nothrow));
};
```

#### atf_amc.FDb - In-memory database for atf_amc
<a href="#atf_amc-fdb"></a>

#### atf_amc.FDb Fields
<a href="#atf_amc-fdb-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.FDb.cmdline|[command.atf_amc](/txt/protocol/command/README.md#command-atf_amc)|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.FDb.bh_typec|[atf_amc.FTypeC](/txt/exe/atf_amc/internals.md#atf_amc-ftypec)|[Bheap](/txt/exe/amc/reftypes.md#bheap)|||
|atf_amc.FDb.zdl_h_typec|[atf_amc.FTypeC](/txt/exe/atf_amc/internals.md#atf_amc-ftypec)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|atf_amc.FDb.zd_t_typec|[atf_amc.FTypeC](/txt/exe/atf_amc/internals.md#atf_amc-ftypec)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|atf_amc.FDb.zd_typed|[atf_amc.FTypeD](/txt/exe/atf_amc/internals.md#atf_amc-ftyped)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|atf_amc.FDb.zs_t_typed|[atf_amc.FTypeD](/txt/exe/atf_amc/internals.md#atf_amc-ftyped)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|atf_amc.FDb.cd_typed|[atf_amc.FTypeD](/txt/exe/atf_amc/internals.md#atf_amc-ftyped)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|atf_amc.FDb.zsl_h_typec|[atf_amc.FTypeC](/txt/exe/atf_amc/internals.md#atf_amc-ftypec)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|atf_amc.FDb.zs_t_typec|[atf_amc.FTypeC](/txt/exe/atf_amc/internals.md#atf_amc-ftypec)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|atf_amc.FDb.zs_mt_typec|[atf_amc.FTypeC](/txt/exe/atf_amc/internals.md#atf_amc-ftypec)|[ZSListMT](/txt/exe/amc/reftypes.md#zslistmt)||dummy (instance name), for Ptr type gen|
|atf_amc.FDb.zdl_hnt_typec|[atf_amc.FTypeC](/txt/exe/atf_amc/internals.md#atf_amc-ftypec)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|atf_amc.FDb.cdl_h_typec|[atf_amc.FTypeC](/txt/exe/atf_amc/internals.md#atf_amc-ftypec)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|atf_amc.FDb.cd_t_typec|[atf_amc.FTypeC](/txt/exe/atf_amc/internals.md#atf_amc-ftypec)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|atf_amc.FDb.csl_h_typec|[atf_amc.FTypeC](/txt/exe/atf_amc/internals.md#atf_amc-ftypec)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|atf_amc.FDb.cs_t_typec|[atf_amc.FTypeC](/txt/exe/atf_amc/internals.md#atf_amc-ftypec)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|atf_amc.FDb.typea|[atf_amc.FTypeA](/txt/exe/atf_amc/internals.md#atf_amc-ftypea)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|atf_amc.FDb.types|[atf_amc.FTypeS](/txt/exe/atf_amc/internals.md#atf_amc-ftypes)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|atf_amc.FDb.ind_types|[atf_amc.FTypeS](/txt/exe/atf_amc/internals.md#atf_amc-ftypes)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|atf_amc.FDb.ind_typea|[atf_amc.FTypeA](/txt/exe/atf_amc/internals.md#atf_amc-ftypea)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|atf_amc.FDb._db|[atf_amc.FDb](/txt/exe/atf_amc/internals.md#atf_amc-fdb)|[Global](/txt/exe/amc/reftypes.md#global)|||
|atf_amc.FDb.cs_h_count|i32|[Val](/txt/exe/amc/reftypes.md#val)||FirstChanged trigger fire count|
|atf_amc.FDb.cs_t_count|i32|[Val](/txt/exe/amc/reftypes.md#val)||FirstChanged trigger fire count|
|atf_amc.FDb.zs_h_count|i32|[Val](/txt/exe/amc/reftypes.md#val)||FirstChanged trigger fire count|
|atf_amc.FDb.zs_t_count|i32|[Val](/txt/exe/amc/reftypes.md#val)||FirstChanged trigger fire count|
|atf_amc.FDb.zs_mt_count|i32|[Val](/txt/exe/amc/reftypes.md#val)||FirstChanged trigger fire count|
|atf_amc.FDb.bh_count|i32|[Val](/txt/exe/amc/reftypes.md#val)||FirstChanged trigger fire count|
|atf_amc.FDb.typet|[atf_amc.FTypeT](/txt/exe/atf_amc/internals.md#atf_amc-ftypet)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|atf_amc.FDb.cstring|[atf_amc.FCstring](/txt/exe/atf_amc/internals.md#atf_amc-fcstring)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|atf_amc.FDb.ind_cstring|[atf_amc.FCstring](/txt/exe/atf_amc/internals.md#atf_amc-fcstring)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|atf_amc.FDb.msgbuf|[atf_amc.Msgbuf](/txt/exe/atf_amc/internals.md#atf_amc-msgbuf)|[Cppstack](/txt/exe/amc/reftypes.md#cppstack)||Provides default name for variables of this type|
|atf_amc.FDb.bytebuf|[atf_amc.Bytebuf](/txt/exe/atf_amc/internals.md#atf_amc-bytebuf)|[Cppstack](/txt/exe/amc/reftypes.md#cppstack)||Provides default name for variables of this type|
|atf_amc.FDb.linebuf|[atf_amc.Linebuf](/txt/exe/atf_amc/internals.md#atf_amc-linebuf)|[Cppstack](/txt/exe/amc/reftypes.md#cppstack)||Provides default name for variables of this type|
|atf_amc.FDb.cd_in_msg|[atf_amc.Msgbuf](/txt/exe/atf_amc/internals.md#atf_amc-msgbuf)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|atf_amc.FDb.typeb|[atf_amc.FTypeB](/txt/exe/atf_amc/internals.md#atf_amc-ftypeb)|[Tpool](/txt/exe/amc/reftypes.md#tpool)|||
|atf_amc.FDb.pooledbe64|[atf_amc.PooledBE64](/txt/exe/atf_amc/internals.md#atf_amc-pooledbe64)|[Tpool](/txt/exe/amc/reftypes.md#tpool)|||
|atf_amc.FDb.varlen_extern|[atf_amc.VarlenExtern](/txt/exe/atf_amc/internals.md#atf_amc-varlenextern)|[Lpool](/txt/exe/amc/reftypes.md#lpool)|||
|atf_amc.FDb.amctest|[atf_amc.FAmctest](/txt/exe/atf_amc/internals.md#atf_amc-famctest)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|atf_amc.FDb.cascdel|[atf_amc.FCascdel](/txt/exe/atf_amc/internals.md#atf_amc-fcascdel)|[Tpool](/txt/exe/amc/reftypes.md#tpool)||Pool of cascdel records|
|atf_amc.FDb.cascdel_check|[algo_lib.Bitset](/txt/lib/algo_lib/README.md#algo_lib-bitset)|[Val](/txt/exe/amc/reftypes.md#val)||Bitset of FCascdel records in existence|
|atf_amc.FDb.cascdel_next_key|u32|[Val](/txt/exe/amc/reftypes.md#val)||Next sequence number for FCascdel record|
|atf_amc.FDb.optalloc|[atf_amc.OptAlloc](/txt/exe/atf_amc/internals.md#atf_amc-optalloc)|[Lpool](/txt/exe/amc/reftypes.md#lpool)|||
|atf_amc.FDb.varlenalloc|[atf_amc.VarlenAlloc](/txt/exe/atf_amc/internals.md#atf_amc-varlenalloc)|[Lpool](/txt/exe/amc/reftypes.md#lpool)|||
|atf_amc.FDb.optg|[atf_amc.FOptG](/txt/exe/atf_amc/internals.md#atf_amc-foptg)|[Lpool](/txt/exe/amc/reftypes.md#lpool)|||
|atf_amc.FDb.c_typek|[atf_amc.FTypeK](/txt/exe/atf_amc/internals.md#atf_amc-ftypek)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|atf_amc.FDb.c_typel|[atf_amc.FTypeL](/txt/exe/atf_amc/internals.md#atf_amc-ftypel)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|atf_amc.FDb.c_typem|[atf_amc.FTypeM](/txt/exe/atf_amc/internals.md#atf_amc-ftypem)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|atf_amc.FDb.avl|[atf_amc.FAvl](/txt/exe/atf_amc/internals.md#atf_amc-favl)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|atf_amc.FDb.tr_avl|[atf_amc.FAvl](/txt/exe/atf_amc/internals.md#atf_amc-favl)|[Atree](/txt/exe/amc/reftypes.md#atree)|||
|atf_amc.FDb.listtype|[atf_amc.FListtype](/txt/exe/atf_amc/internals.md#atf_amc-flisttype)|[Inlary](/txt/exe/amc/reftypes.md#inlary)|||
|atf_amc.FDb.dofork|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.FDb.typed|[atf_amc.FTypeD](/txt/exe/atf_amc/internals.md#atf_amc-ftyped)|[Tpool](/txt/exe/amc/reftypes.md#tpool)|||
|atf_amc.FDb.bytebuf_dyn|[atf_amc.BytebufDyn](/txt/exe/atf_amc/internals.md#atf_amc-bytebufdyn)|[Cppstack](/txt/exe/amc/reftypes.md#cppstack)||Provides default name for variables of this type|
|atf_amc.FDb.ind_thash_elem|[atf_amc.FThashElem](/txt/exe/atf_amc/internals.md#atf_amc-fthashelem)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|atf_amc.FDb.thash_elem|[atf_amc.FThashElem](/txt/exe/atf_amc/internals.md#atf_amc-fthashelem)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|atf_amc.FDb.hashable|[atf_amc.Hashable](/txt/exe/atf_amc/internals.md#atf_amc-hashable)|[Tpool](/txt/exe/amc/reftypes.md#tpool)|||
|atf_amc.FDb.ind_hashable|[atf_amc.Hashable](/txt/exe/atf_amc/internals.md#atf_amc-hashable)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|atf_amc.FDb.time_entry|[atf_amc.FTimeEntry](/txt/exe/atf_amc/internals.md#atf_amc-ftimeentry)|[Tpool](/txt/exe/amc/reftypes.md#tpool)|||
|atf_amc.FDb.bh_time_entry|[atf_amc.FTimeEntry](/txt/exe/atf_amc/internals.md#atf_amc-ftimeentry)|[Bheap](/txt/exe/amc/reftypes.md#bheap)|||
|atf_amc.FDb.timehook_recur_counter|u64|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.FDb.timehook_once_counter|u64|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.FDb.ind_linear|[atf_amc.FHashableLinear](/txt/exe/atf_amc/internals.md#atf_amc-fhashablelinear)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|atf_amc.FDb.hashable_linear|[atf_amc.FHashableLinear](/txt/exe/atf_amc/internals.md#atf_amc-fhashablelinear)|[Tpool](/txt/exe/amc/reftypes.md#tpool)|||
|atf_amc.FDb.strkey|[atf_amc.Strkey](/txt/exe/atf_amc/internals.md#atf_amc-strkey)|[Tpool](/txt/exe/amc/reftypes.md#tpool)|||
|atf_amc.FDb.ind_strkey|[atf_amc.Strkey](/txt/exe/atf_amc/internals.md#atf_amc-strkey)|[Thash](/txt/exe/amc/reftypes.md#thash)|||

#### Struct FDb
<a href="#struct-fdb"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct FDb { // atf_amc.FDb: In-memory database for atf_amc
    command::atf_amc             cmdline;                                          //
    atf_amc::FTypeC**            bh_typec_elems;                                   // binary heap by typec
    i32                          bh_typec_n;                                       // number of elements in the heap
    i32                          bh_typec_max;                                     // max elements in bh_typec_elems
    atf_amc::FTypeC*             zdl_h_typec_head;                                 // zero-terminated doubly linked list
    i32                          zdl_h_typec_n;                                    // zero-terminated doubly linked list
    atf_amc::FTypeC*             zdl_h_typec_tail;                                 // pointer to last element
    atf_amc::FTypeC*             zd_t_typec_head;                                  // zero-terminated doubly linked list
    i32                          zd_t_typec_n;                                     // zero-terminated doubly linked list
    atf_amc::FTypeC*             zd_t_typec_tail;                                  // pointer to last element
    atf_amc::FTypeD*             zd_typed_head;                                    // zero-terminated doubly linked list
    i32                          zd_typed_n;                                       // zero-terminated doubly linked list
    atf_amc::FTypeD*             zd_typed_tail;                                    // pointer to last element
    atf_amc::FTypeD*             zs_t_typed_head;                                  // zero-terminated singly linked list
    i32                          zs_t_typed_n;                                     // zero-terminated singly linked list
    atf_amc::FTypeD*             zs_t_typed_tail;                                  // pointer to last element
    atf_amc::FTypeD*             cd_typed_head;                                    // zero-terminated doubly linked list
    i32                          cd_typed_n;                                       // zero-terminated doubly linked list
    atf_amc::FTypeC*             zsl_h_typec_head;                                 // zero-terminated singly linked list
    i32                          zsl_h_typec_n;                                    // zero-terminated singly linked list
    atf_amc::FTypeC*             zsl_h_typec_tail;                                 // pointer to last element
    atf_amc::FTypeC*             zs_t_typec_head;                                  // zero-terminated singly linked list
    i32                          zs_t_typec_n;                                     // zero-terminated singly linked list
    atf_amc::FTypeC*             zs_t_typec_tail;                                  // pointer to last element

    atf_amc::FTypeC*             zs_mt_typec_head;                                 // ZSListMT - zero-terminated singly linked list -- read side
    atf_amc::FTypeC*             zs_mt_typec_mt;                                   // elements inserted here from another thread -- write side
    atf_amc::FTypeC*             zdl_hnt_typec_head;                               // zero-terminated doubly linked list
    i32                          zdl_hnt_typec_n;                                  // zero-terminated doubly linked list
    atf_amc::FTypeC*             cdl_h_typec_head;                                 // zero-terminated doubly linked list
    i32                          cdl_h_typec_n;                                    // zero-terminated doubly linked list
    atf_amc::FTypeC*             cd_t_typec_head;                                  // zero-terminated doubly linked list
    i32                          cd_t_typec_n;                                     // zero-terminated doubly linked list
    atf_amc::FTypeC*             csl_h_typec_head;                                 // zero-terminated singly linked list
    i32                          csl_h_typec_n;                                    // zero-terminated singly linked list
    atf_amc::FTypeC*             csl_h_typec_tail;                                 // pointer to last element
    atf_amc::FTypeC*             cs_t_typec_head;                                  // zero-terminated singly linked list
    i32                          cs_t_typec_n;                                     // zero-terminated singly linked list
    atf_amc::FTypeC*             cs_t_typec_tail;                                  // pointer to last element
    atf_amc::FTypeA*             typea_lary[32];                                   // level array
    i32                          typea_n;                                          // number of elements in array
    atf_amc::FTypeS*             types_lary[32];                                   // level array
    i32                          types_n;                                          // number of elements in array
    atf_amc::FTypeS**            ind_types_buckets_elems;                          // pointer to bucket array
    i32                          ind_types_buckets_n;                              // number of elements in bucket array
    i32                          ind_types_n;                                      // number of elements in the hash table
    atf_amc::FTypeA**            ind_typea_buckets_elems;                          // pointer to bucket array
    i32                          ind_typea_buckets_n;                              // number of elements in bucket array
    i32                          ind_typea_n;                                      // number of elements in the hash table
    i32                          cs_h_count;                                       //   0  FirstChanged trigger fire count
    i32                          cs_t_count;                                       //   0  FirstChanged trigger fire count
    i32                          zs_h_count;                                       //   0  FirstChanged trigger fire count
    i32                          zs_t_count;                                       //   0  FirstChanged trigger fire count
    i32                          zs_mt_count;                                      //   0  FirstChanged trigger fire count
    i32                          bh_count;                                         //   0  FirstChanged trigger fire count
    atf_amc::FTypeT*             typet_lary[32];                                   // level array
    i32                          typet_n;                                          // number of elements in array
    atf_amc::FCstring*           cstring_lary[32];                                 // level array
    i32                          cstring_n;                                        // number of elements in array
    atf_amc::FCstring**          ind_cstring_buckets_elems;                        // pointer to bucket array
    i32                          ind_cstring_buckets_n;                            // number of elements in bucket array
    i32                          ind_cstring_n;                                    // number of elements in the hash table
    atf_amc::Msgbuf*             cd_in_msg_head;                                   // zero-terminated doubly linked list
    u64                          typeb_blocksize;                                  // # bytes per block
    atf_amc::FTypeB*             typeb_free;                                       //
    u64                          pooledbe64_blocksize;                             // # bytes per block
    atf_amc::PooledBE64*         pooledbe64_free;                                  //
    i64                          varlen_extern_n;                                  // Number of elements
    varlen_extern_Lpblock*       varlen_extern_free[36];                           // Lpool levels
    atf_amc::FAmctest*           amctest_lary[32];                                 // level array
    i32                          amctest_n;                                        // number of elements in array
    u64                          cascdel_blocksize;                                // # bytes per block
    atf_amc::FCascdel*           cascdel_free;                                     //
    algo_lib::Bitset             cascdel_check;                                    // Bitset of FCascdel records in existence
    u32                          cascdel_next_key;                                 //   0  Next sequence number for FCascdel record
    i64                          optalloc_n;                                       // Number of elements
    optalloc_Lpblock*            optalloc_free[36];                                // Lpool levels
    i64                          varlenalloc_n;                                    // Number of elements
    varlenalloc_Lpblock*         varlenalloc_free[36];                             // Lpool levels
    i64                          optg_n;                                           // Number of elements
    optg_Lpblock*                optg_free[36];                                    // Lpool levels
    atf_amc::FTypeK**            c_typek_elems;                                    // array of pointers
    u32                          c_typek_n;                                        // array of pointers
    u32                          c_typek_max;                                      // capacity of allocated array
    atf_amc::FTypeL**            c_typel_elems;                                    // array of pointers
    u32                          c_typel_n;                                        // array of pointers
    u32                          c_typel_max;                                      // capacity of allocated array
    atf_amc::FTypeM**            c_typem_elems;                                    // array of pointers
    u32                          c_typem_n;                                        // array of pointers
    u32                          c_typem_max;                                      // capacity of allocated array
    atf_amc::FAvl*               avl_lary[32];                                     // level array
    i32                          avl_n;                                            // number of elements in array
    atf_amc::FAvl*               tr_avl_root;                                      // Root of the tree
    i32                          tr_avl_n;                                         // number of elements in the tree
    u128                         listtype_data[sizeu128(atf_amc::FListtype,10)];   // place for data
    i32                          listtype_n;                                       // number of elems current in existence
    enum { listtype_max = 10 };
    bool                         dofork;                                           //   false
    u64                          typed_blocksize;                                  // # bytes per block
    atf_amc::FTypeD*             typed_free;                                       //
    atf_amc::FThashElem**        ind_thash_elem_buckets_elems;                     // pointer to bucket array
    i32                          ind_thash_elem_buckets_n;                         // number of elements in bucket array
    i32                          ind_thash_elem_n;                                 // number of elements in the hash table
    atf_amc::FThashElem*         thash_elem_lary[32];                              // level array
    i32                          thash_elem_n;                                     // number of elements in array
    u64                          hashable_blocksize;                               // # bytes per block
    atf_amc::Hashable*           hashable_free;                                    //
    atf_amc::Hashable**          ind_hashable_buckets_elems;                       // pointer to bucket array
    i32                          ind_hashable_buckets_n;                           // number of elements in bucket array
    i32                          ind_hashable_n;                                   // number of elements in the hash table
    u64                          time_entry_blocksize;                             // # bytes per block
    atf_amc::FTimeEntry*         time_entry_free;                                  //
    atf_amc::FTimeEntry**        bh_time_entry_elems;                              // binary heap by time
    i32                          bh_time_entry_n;                                  // number of elements in the heap
    i32                          bh_time_entry_max;                                // max elements in bh_time_entry_elems
    algo_lib::FTimehook          th_bh_time_entry;                                 // atf_amc.FDb.bh_time_entry                                   fstep time hook for atf_amc.FDb.bh_time_entry
    u64                          timehook_recur_counter;                           //   0
    u64                          timehook_once_counter;                            //   0
    atf_amc::FHashableLinear**   ind_linear_buckets_elems;                         // pointer to bucket array
    i32                          ind_linear_buckets_n;                             // number of elements in bucket array
    i32                          ind_linear_n;                                     // number of elements in the hash table
    u64                          hashable_linear_blocksize;                        // # bytes per block
    atf_amc::FHashableLinear*    hashable_linear_free;                             //
    u64                          strkey_blocksize;                                 // # bytes per block
    atf_amc::Strkey*             strkey_free;                                      //
    atf_amc::Strkey**            ind_strkey_buckets_elems;                         // pointer to bucket array
    i32                          ind_strkey_buckets_n;                             // number of elements in bucket array
    i32                          ind_strkey_n;                                     // number of elements in the hash table
    atf_amc::trace               trace;                                            //
};
```

#### atf_amc.FHashableLinear - 
<a href="#atf_amc-fhashablelinear"></a>

#### atf_amc.FHashableLinear Fields
<a href="#atf_amc-fhashablelinear-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.FHashableLinear.key|[algo.U32LinearKey](/txt/protocol/algo/README.md#algo-u32linearkey)|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FHashableLinear
<a href="#struct-fhashablelinear"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct FHashableLinear { // atf_amc.FHashableLinear
    atf_amc::FHashableLinear*   ind_linear_next;        // hash next
    u32                         ind_linear_hashval;     // hash value
    atf_amc::FHashableLinear*   hashable_linear_next;   // Pointer to next free element int tpool
    algo::U32LinearKey          key;                    //
    // func:atf_amc.FHashableLinear..AssignOp
    inline atf_amc::FHashableLinear& operator =(const atf_amc::FHashableLinear &rhs) = delete;
    // func:atf_amc.FHashableLinear..CopyCtor
    inline               FHashableLinear(const atf_amc::FHashableLinear &rhs) = delete;
private:
    // func:atf_amc.FHashableLinear..Ctor
    inline               FHashableLinear() __attribute__((nothrow));
    // func:atf_amc.FHashableLinear..Dtor
    inline               ~FHashableLinear() __attribute__((nothrow));
    friend atf_amc::FHashableLinear& hashable_linear_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend atf_amc::FHashableLinear* hashable_linear_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 hashable_linear_Delete(atf_amc::FHashableLinear &row) __attribute__((nothrow));
};
```

#### atf_amc.TypeG - 
<a href="#atf_amc-typeg"></a>

#### atf_amc.TypeG Fields
<a href="#atf_amc-typeg-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.TypeG.typeg|i32|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct TypeG
<a href="#struct-typeg"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
#pragma pack(push,1)
struct TypeG { // atf_amc.TypeG
    i32   typeg;   //   0
    // func:atf_amc.TypeG..EqOp
    inline bool          operator ==(const atf_amc::TypeG &rhs) const __attribute__((nothrow));
    // func:atf_amc.TypeG..NeOp
    inline bool          operator !=(const atf_amc::TypeG &rhs) const __attribute__((nothrow));
    // func:atf_amc.TypeG..LtOp
    inline bool          operator <(const atf_amc::TypeG &rhs) const __attribute__((nothrow));
    // func:atf_amc.TypeG..GtOp
    inline bool          operator >(const atf_amc::TypeG &rhs) const __attribute__((nothrow));
    // func:atf_amc.TypeG..LeOp
    inline bool          operator <=(const atf_amc::TypeG &rhs) const __attribute__((nothrow));
    // func:atf_amc.TypeG..GeOp
    inline bool          operator >=(const atf_amc::TypeG &rhs) const __attribute__((nothrow));
    // func:atf_amc.TypeG..Ctor
    inline               TypeG() __attribute__((nothrow));
};
```

#### atf_amc.FOptG - Something derived from OptG
<a href="#atf_amc-foptg"></a>

#### atf_amc.FOptG Fields
<a href="#atf_amc-foptg-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.FOptG.optg|[atf_amc.OptG](/txt/exe/atf_amc/internals.md#atf_amc-optg)|[Base](#atf_amc-optg-fields)|||
#### atf_amc.OptG Fields
<a href="#atf_amc-optg-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.OptG.length|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.OptG.typeg|[atf_amc.TypeG](/txt/exe/atf_amc/internals.md#atf_amc-typeg)|[Opt](/txt/exe/amc/reftypes.md#opt)|||

#### Struct FOptG
<a href="#struct-foptg"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct FOptG { // atf_amc.FOptG: Something derived from OptG
    u32                    length;    //   0
    // atf_amc::TypeG      typeg[];                optional field
private:
    // func:atf_amc.FOptG..Ctor
    inline               FOptG() __attribute__((nothrow));
    friend atf_amc::FOptG&      optg_AllocExtra(void *extra, i32 nbyte_extra) __attribute__((__warn_unused_result__, nothrow));
    friend atf_amc::FOptG*      optg_AllocExtraMaybe(void *extra, i32 nbyte_extra) __attribute__((__warn_unused_result__, nothrow));
    friend void                 optg_Delete(atf_amc::FOptG &row) __attribute__((nothrow));
};
```

#### atf_amc.FPerfSortString - Function to test string sorting
<a href="#atf_amc-fperfsortstring"></a>

#### atf_amc.FPerfSortString Fields
<a href="#atf_amc-fperfsortstring-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.FPerfSortString.orig|[atf_amc.Cstr](/txt/exe/atf_amc/internals.md#atf_amc-cstr)|[Tary](/txt/exe/amc/reftypes.md#tary)|||
|atf_amc.FPerfSortString.sorted1|[atf_amc.Cstr](/txt/exe/atf_amc/internals.md#atf_amc-cstr)|[Tary](/txt/exe/amc/reftypes.md#tary)|||

#### Struct FPerfSortString
<a href="#struct-fperfsortstring"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct FPerfSortString { // atf_amc.FPerfSortString: Function to test string sorting
    atf_amc::Cstr*   orig_elems;      // pointer to elements
    u32              orig_n;          // number of elements in array
    u32              orig_max;        // max. capacity of array before realloc
    atf_amc::Cstr*   sorted1_elems;   // pointer to elements
    u32              sorted1_n;       // number of elements in array
    u32              sorted1_max;     // max. capacity of array before realloc
    // func:atf_amc.FPerfSortString..AssignOp
    atf_amc::FPerfSortString& operator =(const atf_amc::FPerfSortString &rhs) __attribute__((nothrow));
    // func:atf_amc.FPerfSortString..Ctor
    inline               FPerfSortString() __attribute__((nothrow));
    // func:atf_amc.FPerfSortString..Dtor
    inline               ~FPerfSortString() __attribute__((nothrow));
    // func:atf_amc.FPerfSortString..CopyCtor
    FPerfSortString(const atf_amc::FPerfSortString &rhs) __attribute__((nothrow));
};
```

#### atf_amc.FThashElem - 
<a href="#atf_amc-fthashelem"></a>

#### atf_amc.FThashElem Fields
<a href="#atf_amc-fthashelem-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.FThashElem.key|u64|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FThashElem
<a href="#struct-fthashelem"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct FThashElem { // atf_amc.FThashElem
    atf_amc::FThashElem*   ind_thash_elem_next;      // hash next
    u32                    ind_thash_elem_hashval;   // hash value
    u64                    key;                      //   0
    // func:atf_amc.FThashElem..AssignOp
    inline atf_amc::FThashElem& operator =(const atf_amc::FThashElem &rhs) = delete;
    // func:atf_amc.FThashElem..CopyCtor
    inline               FThashElem(const atf_amc::FThashElem &rhs) = delete;
private:
    // func:atf_amc.FThashElem..Ctor
    inline               FThashElem() __attribute__((nothrow));
    // func:atf_amc.FThashElem..Dtor
    inline               ~FThashElem() __attribute__((nothrow));
    friend atf_amc::FThashElem& thash_elem_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend atf_amc::FThashElem* thash_elem_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 thash_elem_RemoveAll() __attribute__((nothrow));
    friend void                 thash_elem_RemoveLast() __attribute__((nothrow));
};
```

#### atf_amc.FTimeEntry - 
<a href="#atf_amc-ftimeentry"></a>

#### atf_amc.FTimeEntry Fields
<a href="#atf_amc-ftimeentry-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.FTimeEntry.time|[algo.SchedTime](/txt/protocol/algo/SchedTime.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FTimeEntry
<a href="#struct-ftimeentry"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct FTimeEntry { // atf_amc.FTimeEntry
    atf_amc::FTimeEntry*   time_entry_next;     // Pointer to next free element int tpool
    i32                    bh_time_entry_idx;   // index in heap; -1 means not-in-heap
    algo::SchedTime        time;                //
    // func:atf_amc.FTimeEntry..AssignOp
    inline atf_amc::FTimeEntry& operator =(const atf_amc::FTimeEntry &rhs) = delete;
    // func:atf_amc.FTimeEntry..CopyCtor
    inline               FTimeEntry(const atf_amc::FTimeEntry &rhs) = delete;
private:
    // func:atf_amc.FTimeEntry..Ctor
    inline               FTimeEntry() __attribute__((nothrow));
    // func:atf_amc.FTimeEntry..Dtor
    inline               ~FTimeEntry() __attribute__((nothrow));
    friend atf_amc::FTimeEntry& time_entry_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend atf_amc::FTimeEntry* time_entry_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 time_entry_Delete(atf_amc::FTimeEntry &row) __attribute__((nothrow));
};
```

#### atf_amc.FTypeC - 
<a href="#atf_amc-ftypec"></a>

#### atf_amc.FTypeC Fields
<a href="#atf_amc-ftypec-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.FTypeC.msghdr|[atf_amc.TypeC](/txt/exe/atf_amc/internals.md#atf_amc-typec)|[Base](#atf_amc-typec-fields)|||
#### atf_amc.TypeC Fields
<a href="#atf_amc-typec-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.TypeC.typec|i32|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FTypeC
<a href="#struct-ftypec"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct FTypeC { // atf_amc.FTypeC
    i32                bh_typec_idx;         // index in heap; -1 means not-in-heap
    atf_amc::FTypeC*   zdl_h_typec_next;     // zslist link; -1 means not-in-list
    atf_amc::FTypeC*   zdl_h_typec_prev;     // previous element
    atf_amc::FTypeC*   zd_t_typec_next;      // zslist link; -1 means not-in-list
    atf_amc::FTypeC*   zd_t_typec_prev;      // previous element
    atf_amc::FTypeC*   zsl_h_typec_next;     // zslist link; -1 means not-in-list
    atf_amc::FTypeC*   zs_t_typec_next;      // zslist link; -1 means not-in-list
    atf_amc::FTypeC*   zs_mt_typec_next;     // zslist link; -1 means not-in-list
    atf_amc::FTypeC*   zdl_hnt_typec_next;   // zslist link; -1 means not-in-list
    atf_amc::FTypeC*   zdl_hnt_typec_prev;   // previous element
    atf_amc::FTypeC*   cdl_h_typec_next;     // zslist link; -1 means not-in-list
    atf_amc::FTypeC*   cdl_h_typec_prev;     // previous element
    atf_amc::FTypeC*   cd_t_typec_next;      // zslist link; -1 means not-in-list
    atf_amc::FTypeC*   cd_t_typec_prev;      // previous element
    atf_amc::FTypeC*   csl_h_typec_next;     // zslist link; -1 means not-in-list
    atf_amc::FTypeC*   cs_t_typec_next;      // zslist link; -1 means not-in-list
    i32                typec;                //   0
    // func:atf_amc.FTypeC..AssignOp
    inline atf_amc::FTypeC& operator =(const atf_amc::FTypeC &rhs) = delete;
    // func:atf_amc.FTypeC..Ctor
    inline               FTypeC() __attribute__((nothrow));
    // func:atf_amc.FTypeC..Dtor
    inline               ~FTypeC() __attribute__((nothrow));
    // func:atf_amc.FTypeC..CopyCtor
    inline               FTypeC(const atf_amc::FTypeC &rhs) = delete;
};
```

#### atf_amc.FTypeA - 
<a href="#atf_amc-ftypea"></a>

#### atf_amc.FTypeA Fields
<a href="#atf_amc-ftypea-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.FTypeA.msghdr|[atf_amc.TypeA](/txt/exe/atf_amc/internals.md#atf_amc-typea)|[Base](#atf_amc-typea-fields)|||
|atf_amc.FTypeA.typec|[atf_amc.FTypeC](/txt/exe/atf_amc/internals.md#atf_amc-ftypec)|[Inlary](/txt/exe/amc/reftypes.md#inlary)|||
|atf_amc.FTypeA.zdl_typeb|[atf_amc.FTypeB](/txt/exe/atf_amc/internals.md#atf_amc-ftypeb)|[Llist](/txt/exe/amc/reftypes.md#llist)||List of TypeBs|
|atf_amc.FTypeA.rowid|i32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.FTypeA.bh_typeb|[atf_amc.FTypeB](/txt/exe/atf_amc/internals.md#atf_amc-ftypeb)|[Bheap](/txt/exe/amc/reftypes.md#bheap)|||
#### atf_amc.TypeA Fields
<a href="#atf_amc-typea-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.TypeA.typea|i32|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FTypeA
<a href="#struct-ftypea"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct FTypeA { // atf_amc.FTypeA
    atf_amc::FTypeA*    ind_typea_next;                             // hash next
    u32                 ind_typea_hashval;                          // hash value
    i32                 typea;                                      //   0
    u128                typec_data[sizeu128(atf_amc::FTypeC,10)];   // place for data
    i32                 typec_n;                                    // number of elems current in existence
    enum { typec_max = 10 };
    atf_amc::FTypeB*    zdl_typeb_head;                             // zero-terminated doubly linked list
    i32                 zdl_typeb_n;                                // zero-terminated doubly linked list
    atf_amc::FTypeB*    zdl_typeb_tail;                             // pointer to last element
    i32                 rowid;                                      //   0
    atf_amc::FTypeB**   bh_typeb_elems;                             // binary heap by j
    i32                 bh_typeb_n;                                 // number of elements in the heap
    i32                 bh_typeb_max;                               // max elements in bh_typeb_elems
    // reftype Llist of atf_amc.FTypeA.zdl_typeb prohibits copy
    // reftype Bheap of atf_amc.FTypeA.bh_typeb prohibits copy
    // func:atf_amc.FTypeA..AssignOp
    inline atf_amc::FTypeA& operator =(const atf_amc::FTypeA &rhs) = delete;
    // reftype Llist of atf_amc.FTypeA.zdl_typeb prohibits copy
    // reftype Bheap of atf_amc.FTypeA.bh_typeb prohibits copy
    // func:atf_amc.FTypeA..CopyCtor
    inline               FTypeA(const atf_amc::FTypeA &rhs) = delete;
private:
    // func:atf_amc.FTypeA..Ctor
    inline               FTypeA() __attribute__((nothrow));
    // func:atf_amc.FTypeA..Dtor
    inline               ~FTypeA() __attribute__((nothrow));
    friend atf_amc::FTypeA&     typea_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend atf_amc::FTypeA*     typea_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 typea_RemoveAll() __attribute__((nothrow));
    friend void                 typea_RemoveLast() __attribute__((nothrow));
};
```

#### atf_amc.FTypeB - 
<a href="#atf_amc-ftypeb"></a>

#### atf_amc.FTypeB Fields
<a href="#atf_amc-ftypeb-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.FTypeB.msghdr|[atf_amc.TypeB](/txt/exe/atf_amc/internals.md#atf_amc-typeb)|[Base](#atf_amc-typeb-fields)|||
#### atf_amc.TypeB Fields
<a href="#atf_amc-typeb-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.TypeB.typea|[atf_amc.TypeA](/txt/exe/atf_amc/internals.md#atf_amc-typea)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|||
|atf_amc.TypeB.j|i32|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FTypeB
<a href="#struct-ftypeb"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct FTypeB { // atf_amc.FTypeB
    atf_amc::FTypeB*   typeb_next;             // Pointer to next free element int tpool
    atf_amc::FTypeB*   typea_zdl_typeb_next;   // zslist link; -1 means not-in-list
    atf_amc::FTypeB*   typea_zdl_typeb_prev;   // previous element
    i32                typea_bh_typeb_idx;     // index in heap; -1 means not-in-heap
    i32                typea;                  //   0
    i32                j;                      //   0
    // func:atf_amc.FTypeB..AssignOp
    inline atf_amc::FTypeB& operator =(const atf_amc::FTypeB &rhs) = delete;
    // func:atf_amc.FTypeB..CopyCtor
    inline               FTypeB(const atf_amc::FTypeB &rhs) = delete;
private:
    // func:atf_amc.FTypeB..Ctor
    inline               FTypeB() __attribute__((nothrow));
    // func:atf_amc.FTypeB..Dtor
    inline               ~FTypeB() __attribute__((nothrow));
    friend atf_amc::FTypeB&     typeb_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend atf_amc::FTypeB*     typeb_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 typeb_Delete(atf_amc::FTypeB &row) __attribute__((nothrow));
};
```

#### atf_amc.FTypeD - 
<a href="#atf_amc-ftyped"></a>

#### atf_amc.FTypeD Fields
<a href="#atf_amc-ftyped-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.FTypeD.msghdr|[atf_amc.TypeC](/txt/exe/atf_amc/internals.md#atf_amc-typec)|[Base](#atf_amc-typec-fields)|||
#### atf_amc.TypeC Fields
<a href="#atf_amc-typec-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.TypeC.typec|i32|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FTypeD
<a href="#struct-ftyped"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct FTypeD { // atf_amc.FTypeD
    atf_amc::FTypeD*   zd_typed_next;     // zslist link; -1 means not-in-list
    atf_amc::FTypeD*   zd_typed_prev;     // previous element
    atf_amc::FTypeD*   zs_t_typed_next;   // zslist link; -1 means not-in-list
    atf_amc::FTypeD*   cd_typed_next;     // zslist link; -1 means not-in-list
    atf_amc::FTypeD*   cd_typed_prev;     // previous element
    atf_amc::FTypeD*   typed_next;        // Pointer to next free element int tpool
    i32                typec;             //   0
    // func:atf_amc.FTypeD..AssignOp
    inline atf_amc::FTypeD& operator =(const atf_amc::FTypeD &rhs) = delete;
    // func:atf_amc.FTypeD..CopyCtor
    inline               FTypeD(const atf_amc::FTypeD &rhs) = delete;
private:
    // func:atf_amc.FTypeD..Ctor
    inline               FTypeD() __attribute__((nothrow));
    // func:atf_amc.FTypeD..Dtor
    inline               ~FTypeD() __attribute__((nothrow));
    friend atf_amc::FTypeD&     typed_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend atf_amc::FTypeD*     typed_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 typed_Delete(atf_amc::FTypeD &row) __attribute__((nothrow));
};
```

#### atf_amc.FTypeK - Test element for Ptrary
<a href="#atf_amc-ftypek"></a>

#### atf_amc.FTypeK Fields
<a href="#atf_amc-ftypek-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.FTypeK.value|u32|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FTypeK
<a href="#struct-ftypek"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct FTypeK { // atf_amc.FTypeK: Test element for Ptrary
    u32    value;            //   0
    bool   c_typek_in_ary;   //   false  membership flag
    // func:atf_amc.FTypeK..Ctor
    inline               FTypeK() __attribute__((nothrow));
};
```

#### atf_amc.FTypeL - Test element for heap-like Ptrary
<a href="#atf_amc-ftypel"></a>

#### atf_amc.FTypeL Fields
<a href="#atf_amc-ftypel-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.FTypeL.value|u32|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FTypeL
<a href="#struct-ftypel"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct FTypeL { // atf_amc.FTypeL: Test element for heap-like Ptrary
    u32   value;         //   0
    i32   c_typel_idx;   //   -1  Array index (-1 = not in array)
    // func:atf_amc.FTypeL..Ctor
    inline               FTypeL() __attribute__((nothrow));
};
```

#### atf_amc.FTypeM - Test element for non-unique Ptrary
<a href="#atf_amc-ftypem"></a>

#### atf_amc.FTypeM Fields
<a href="#atf_amc-ftypem-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.FTypeM.value|u32|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FTypeM
<a href="#struct-ftypem"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct FTypeM { // atf_amc.FTypeM: Test element for non-unique Ptrary
    u32   value;   //   0
    // func:atf_amc.FTypeM..Ctor
    inline               FTypeM() __attribute__((nothrow));
};
```

#### atf_amc.FTypeS - 
<a href="#atf_amc-ftypes"></a>

#### atf_amc.FTypeS Fields
<a href="#atf_amc-ftypes-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.FTypeS.msghdr|[atf_amc.TypeS](/txt/exe/atf_amc/internals.md#atf_amc-types)|[Base](#atf_amc-types-fields)|||
|atf_amc.FTypeS.zdl_typet|[atf_amc.FTypeT](/txt/exe/atf_amc/internals.md#atf_amc-ftypet)|[Llist](/txt/exe/amc/reftypes.md#llist)||List of TypeTs|
#### atf_amc.TypeS Fields
<a href="#atf_amc-types-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.TypeS.types|i32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.TypeS.comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FTypeS
<a href="#struct-ftypes"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct FTypeS { // atf_amc.FTypeS
    atf_amc::FTypeS*   ind_types_next;      // hash next
    u32                ind_types_hashval;   // hash value
    i32                types;               //   0
    algo::Comment      comment;             //
    atf_amc::FTypeT*   zdl_typet_head;      // zero-terminated doubly linked list
    i32                zdl_typet_n;         // zero-terminated doubly linked list
    atf_amc::FTypeT*   zdl_typet_tail;      // pointer to last element
    // reftype Llist of atf_amc.FTypeS.zdl_typet prohibits copy
    // func:atf_amc.FTypeS..AssignOp
    inline atf_amc::FTypeS& operator =(const atf_amc::FTypeS &rhs) = delete;
    // reftype Llist of atf_amc.FTypeS.zdl_typet prohibits copy
    // func:atf_amc.FTypeS..CopyCtor
    inline               FTypeS(const atf_amc::FTypeS &rhs) = delete;
private:
    // func:atf_amc.FTypeS..Ctor
    inline               FTypeS() __attribute__((nothrow));
    // func:atf_amc.FTypeS..Dtor
    inline               ~FTypeS() __attribute__((nothrow));
    friend atf_amc::FTypeS&     types_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend atf_amc::FTypeS*     types_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 types_RemoveAll() __attribute__((nothrow));
    friend void                 types_RemoveLast() __attribute__((nothrow));
};
```

#### atf_amc.FTypeT - Refers to TypeS
<a href="#atf_amc-ftypet"></a>

#### atf_amc.FTypeT Fields
<a href="#atf_amc-ftypet-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.FTypeT.msghdr|[atf_amc.TypeT](/txt/exe/atf_amc/internals.md#atf_amc-typet)|[Base](#atf_amc-typet-fields)|||
#### atf_amc.TypeT Fields
<a href="#atf_amc-typet-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.TypeT.types|[atf_amc.TypeS](/txt/exe/atf_amc/internals.md#atf_amc-types)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|||
|atf_amc.TypeT.j|i32|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FTypeT
<a href="#struct-ftypet"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct FTypeT { // atf_amc.FTypeT
    atf_amc::FTypeT*   types_zdl_typet_next;   // zslist link; -1 means not-in-list
    atf_amc::FTypeT*   types_zdl_typet_prev;   // previous element
    i32                types;                  //   0
    i32                j;                      //   0
    // func:atf_amc.FTypeT..AssignOp
    inline atf_amc::FTypeT& operator =(const atf_amc::FTypeT &rhs) = delete;
    // func:atf_amc.FTypeT..CopyCtor
    inline               FTypeT(const atf_amc::FTypeT &rhs) = delete;
private:
    // func:atf_amc.FTypeT..Ctor
    inline               FTypeT() __attribute__((nothrow));
    // func:atf_amc.FTypeT..Dtor
    inline               ~FTypeT() __attribute__((nothrow));
    friend atf_amc::FTypeT&     typet_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend atf_amc::FTypeT*     typet_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 typet_RemoveAll() __attribute__((nothrow));
    friend void                 typet_RemoveLast() __attribute__((nothrow));
};
```

#### atf_amc.TypeA - 
<a href="#atf_amc-typea"></a>

#### atf_amc.TypeA Fields
<a href="#atf_amc-typea-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.TypeA.typea|i32|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct TypeA
<a href="#struct-typea"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct TypeA { // atf_amc.TypeA
    i32   typea;   //   0
    // func:atf_amc.TypeA..EqOp
    inline bool          operator ==(const atf_amc::TypeA &rhs) const __attribute__((nothrow));
    // func:atf_amc.TypeA..NeOp
    inline bool          operator !=(const atf_amc::TypeA &rhs) const __attribute__((nothrow));
    // func:atf_amc.TypeA..LtOp
    inline bool          operator <(const atf_amc::TypeA &rhs) const __attribute__((nothrow));
    // func:atf_amc.TypeA..GtOp
    inline bool          operator >(const atf_amc::TypeA &rhs) const __attribute__((nothrow));
    // func:atf_amc.TypeA..LeOp
    inline bool          operator <=(const atf_amc::TypeA &rhs) const __attribute__((nothrow));
    // func:atf_amc.TypeA..GeOp
    inline bool          operator >=(const atf_amc::TypeA &rhs) const __attribute__((nothrow));
    // func:atf_amc.TypeA..Ctor
    inline               TypeA() __attribute__((nothrow));
};
```

#### atf_amc.FUnitSort - 
<a href="#atf_amc-funitsort"></a>

#### atf_amc.FUnitSort Fields
<a href="#atf_amc-funitsort-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.FUnitSort.tary|[atf_amc.TypeA](/txt/exe/atf_amc/internals.md#atf_amc-typea)|[Tary](/txt/exe/amc/reftypes.md#tary)|||
|atf_amc.FUnitSort.c_ptrary|[atf_amc.TypeA](/txt/exe/atf_amc/internals.md#atf_amc-typea)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|atf_amc.FUnitSort.fixary|[atf_amc.TypeA](/txt/exe/atf_amc/internals.md#atf_amc-typea)|[Inlary](/txt/exe/amc/reftypes.md#inlary)|||

#### Struct FUnitSort
<a href="#struct-funitsort"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct FUnitSort { // atf_amc.FUnitSort
    atf_amc::TypeA*    tary_elems;          // pointer to elements
    u32                tary_n;              // number of elements in array
    u32                tary_max;            // max. capacity of array before realloc
    atf_amc::TypeA**   c_ptrary_elems;      // array of pointers
    u32                c_ptrary_n;          // array of pointers
    u32                c_ptrary_max;        // capacity of allocated array
    atf_amc::TypeA     fixary_elems[100];   // fixed array
    // reftype Ptrary of atf_amc.FUnitSort.c_ptrary prohibits copy
    // func:atf_amc.FUnitSort..AssignOp
    atf_amc::FUnitSort&  operator =(const atf_amc::FUnitSort &rhs) = delete;
    // func:atf_amc.FUnitSort..Ctor
    inline               FUnitSort() __attribute__((nothrow));
    // func:atf_amc.FUnitSort..Dtor
    inline               ~FUnitSort() __attribute__((nothrow));
    // reftype Ptrary of atf_amc.FUnitSort.c_ptrary prohibits copy
    // func:atf_amc.FUnitSort..CopyCtor
    FUnitSort(const atf_amc::FUnitSort &rhs) = delete;
};
```

#### atf_amc.Hashable - 
<a href="#atf_amc-hashable"></a>

#### atf_amc.Hashable Fields
<a href="#atf_amc-hashable-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.Hashable.hashable|i32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.Hashable.key|[algo.U64Ary](/txt/protocol/algo/README.md#algo-u64ary)|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct Hashable
<a href="#struct-hashable"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct Hashable { // atf_amc.Hashable
    atf_amc::Hashable*   hashable_next;          // Pointer to next free element int tpool
    atf_amc::Hashable*   ind_hashable_next;      // hash next
    u32                  ind_hashable_hashval;   // hash value
    i32                  hashable;               //   0
    algo::U64Ary         key;                    //
    // func:atf_amc.Hashable..AssignOp
    inline atf_amc::Hashable& operator =(const atf_amc::Hashable &rhs) = delete;
    // func:atf_amc.Hashable..CopyCtor
    inline               Hashable(const atf_amc::Hashable &rhs) = delete;
private:
    // func:atf_amc.Hashable..Ctor
    inline               Hashable() __attribute__((nothrow));
    // func:atf_amc.Hashable..Dtor
    inline               ~Hashable() __attribute__((nothrow));
    friend atf_amc::Hashable&   hashable_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend atf_amc::Hashable*   hashable_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 hashable_Delete(atf_amc::Hashable &row) __attribute__((nothrow));
};
```

#### atf_amc.Hooktype - 
<a href="#atf_amc-hooktype"></a>

#### atf_amc.Hooktype Fields
<a href="#atf_amc-hooktype-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.Hooktype.callback|[atf_amc.Hooktype](/txt/exe/atf_amc/internals.md#atf_amc-hooktype)|[Hook](/txt/exe/amc/reftypes.md#hook)|||
|atf_amc.Hooktype.value|i32|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct Hooktype
<a href="#struct-hooktype"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct Hooktype { // atf_amc.Hooktype
    atf_amc::parent_callback_hook   callback;       //   NULL  Pointer to a function
    u64                             callback_ctx;   //   0  Callback context
    i32                             value;          //   0
    // func:atf_amc.Hooktype..Ctor
    inline               Hooktype() __attribute__((nothrow));
};
```

#### atf_amc.InlaryPrint - 
<a href="#atf_amc-inlaryprint"></a>

#### atf_amc.InlaryPrint Fields
<a href="#atf_amc-inlaryprint-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.InlaryPrint.fixary|u32|[Inlary](/txt/exe/amc/reftypes.md#inlary)|||
|atf_amc.InlaryPrint.inlary|u32|[Inlary](/txt/exe/amc/reftypes.md#inlary)|||

#### Struct InlaryPrint
<a href="#struct-inlaryprint"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct InlaryPrint { // atf_amc.InlaryPrint
    u32    fixary_elems[3];                 //   0  fixed array
    u128   inlary_data[sizeu128(u32,10)];   // place for data
    i32    inlary_n;                        // number of elems current in existence
    enum { inlary_max = 10 };
    // func:atf_amc.InlaryPrint..AssignOp
    inline atf_amc::InlaryPrint& operator =(const atf_amc::InlaryPrint &rhs) __attribute__((nothrow));
    // func:atf_amc.InlaryPrint..Ctor
    inline               InlaryPrint() __attribute__((nothrow));
    // func:atf_amc.InlaryPrint..Dtor
    inline               ~InlaryPrint() __attribute__((nothrow));
    // func:atf_amc.InlaryPrint..CopyCtor
    inline               InlaryPrint(const atf_amc::InlaryPrint &rhs) __attribute__((nothrow));
};
```

#### atf_amc.Lary32 - 
<a href="#atf_amc-lary32"></a>

#### atf_amc.Lary32 Fields
<a href="#atf_amc-lary32-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.Lary32.lary|u32|[Lary](/txt/exe/amc/reftypes.md#lary)|||

#### Struct Lary32
<a href="#struct-lary32"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct Lary32 { // atf_amc.Lary32
    u32*   lary_lary[32];   // level array
    i32    lary_n;          // number of elements in array
    // func:atf_amc.Lary32..Ctor
    inline               Lary32() __attribute__((nothrow));
    // func:atf_amc.Lary32..Dtor
    inline               ~Lary32() __attribute__((nothrow));
};
```

#### atf_amc.Linebuf - 
<a href="#atf_amc-linebuf"></a>

#### atf_amc.Linebuf Fields
<a href="#atf_amc-linebuf-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.Linebuf.in|char|[Fbuf](/txt/exe/amc/reftypes.md#fbuf)|'\n'|Delimited buffer|

#### Struct Linebuf
<a href="#struct-linebuf"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct Linebuf { // atf_amc.Linebuf
    u8*             in_elems;          //   NULL  pointer to elements of indirect array
    u32             in_max;            //   0  current length of allocated array
    i32             in_start;          // beginning of valid bytes (in bytes)
    i32             in_end;            // end of valid bytes (in bytes)
    i32             in_msglen;         // current message length
    algo::Errcode   in_err;            // system error code
    bool            in_eof;            // no more data will be written to buffer
    bool            in_msgvalid;       // current message is valid
    bool            in_epoll_enable;   // use epoll?
    // func:atf_amc.Linebuf..Ctor
    inline               Linebuf() __attribute__((nothrow));
    // func:atf_amc.Linebuf..Dtor
    inline               ~Linebuf() __attribute__((nothrow));
};
```

#### atf_amc.MsgHdrLT - length-type header
<a href="#atf_amc-msghdrlt"></a>

#### atf_amc.MsgHdrLT Fields
<a href="#atf_amc-msghdrlt-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.MsgHdrLT.len|u8|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.MsgHdrLT.type|char|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct MsgHdrLT
<a href="#struct-msghdrlt"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
#pragma pack(push,1)
struct MsgHdrLT { // atf_amc.MsgHdrLT: length-type header
    u8     len;    //   0
    char   type;   //   0
    // func:atf_amc.MsgHdrLT..Ctor
    inline               MsgHdrLT() __attribute__((nothrow));
    // func:atf_amc.MsgHdrLT..FieldwiseCtor
    explicit inline               MsgHdrLT(u8 in_len, char in_type) __attribute__((nothrow));
};
```

#### atf_amc.MsgType - 
<a href="#atf_amc-msgtype"></a>

#### atf_amc.MsgType Fields
<a href="#atf_amc-msgtype-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.MsgType.value|u16|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct MsgType
<a href="#struct-msgtype"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
#pragma pack(push,1)
struct MsgType { // atf_amc.MsgType
    u16   value;   //   0
    // func:atf_amc.MsgType.value.Cast
    inline               operator atf_amc_MsgTypeEnum() const __attribute__((nothrow));
    // func:atf_amc.MsgType..EqOp
    inline bool          operator ==(const atf_amc::MsgType &rhs) const __attribute__((nothrow));
    // func:atf_amc.MsgType..NeOp
    inline bool          operator !=(const atf_amc::MsgType &rhs) const __attribute__((nothrow));
    // func:atf_amc.MsgType..LtOp
    inline bool          operator <(const atf_amc::MsgType &rhs) const __attribute__((nothrow));
    // func:atf_amc.MsgType..GtOp
    inline bool          operator >(const atf_amc::MsgType &rhs) const __attribute__((nothrow));
    // func:atf_amc.MsgType..LeOp
    inline bool          operator <=(const atf_amc::MsgType &rhs) const __attribute__((nothrow));
    // func:atf_amc.MsgType..GeOp
    inline bool          operator >=(const atf_amc::MsgType &rhs) const __attribute__((nothrow));
    // define enum comparison operator to avoid ambiguity
    // func:atf_amc.MsgType..EqEnum
    inline bool          operator ==(atf_amc_MsgTypeEnum rhs) const __attribute__((nothrow));
    // func:atf_amc.MsgType..Ctor
    inline               MsgType() __attribute__((nothrow));
    // func:atf_amc.MsgType..FieldwiseCtor
    explicit inline               MsgType(u16 in_value) __attribute__((nothrow));
    // func:atf_amc.MsgType..EnumCtor
    inline               MsgType(atf_amc_MsgTypeEnum arg) __attribute__((nothrow));
};
```

#### atf_amc.MsgLength - 
<a href="#atf_amc-msglength"></a>

#### atf_amc.MsgLength Fields
<a href="#atf_amc-msglength-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.MsgLength.value|u16|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct MsgLength
<a href="#struct-msglength"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
#pragma pack(push,1)
struct MsgLength { // atf_amc.MsgLength
    u16   value;   //   0
    // func:atf_amc.MsgLength.value.Cast
    inline               operator u16() const __attribute__((nothrow));
    // func:atf_amc.MsgLength..Ctor
    inline               MsgLength() __attribute__((nothrow));
    // func:atf_amc.MsgLength..FieldwiseCtor
    explicit inline               MsgLength(u16 in_value) __attribute__((nothrow));
};
```

#### atf_amc.MsgHeader - 
<a href="#atf_amc-msgheader"></a>

#### atf_amc.MsgHeader Fields
<a href="#atf_amc-msgheader-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.MsgHeader.type|[atf_amc.MsgType](/txt/exe/atf_amc/internals.md#atf_amc-msgtype)|[Val](/txt/exe/amc/reftypes.md#val)||Message type|
|atf_amc.MsgHeader.length|[atf_amc.MsgLength](/txt/exe/atf_amc/internals.md#atf_amc-msglength)|[Val](/txt/exe/amc/reftypes.md#val)||Total message length, including this header|

#### Struct MsgHeader
<a href="#struct-msgheader"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
#pragma pack(push,1)
struct MsgHeader { // atf_amc.MsgHeader
    atf_amc::MsgType     type;     // Message type
    atf_amc::MsgLength   length;   // Total message length, including this header
    // func:atf_amc.MsgHeader..EqOp
    inline bool          operator ==(const atf_amc::MsgHeader &rhs) const __attribute__((nothrow));
    // func:atf_amc.MsgHeader..NeOp
    inline bool          operator !=(const atf_amc::MsgHeader &rhs) const __attribute__((nothrow));
    // func:atf_amc.MsgHeader..Ctor
    inline               MsgHeader() __attribute__((nothrow));
    // func:atf_amc.MsgHeader..FieldwiseCtor
    explicit inline               MsgHeader(atf_amc::MsgType in_type, atf_amc::MsgLength in_length) __attribute__((nothrow));
};
```

#### atf_amc.MsgLTA - length-type header
<a href="#atf_amc-msglta"></a>

#### atf_amc.MsgLTA Fields
<a href="#atf_amc-msglta-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.MsgLTA.base|[atf_amc.MsgHdrLT](/txt/exe/atf_amc/internals.md#atf_amc-msghdrlt)|[Base](#atf_amc-msghdrlt-fields)|||
|atf_amc.MsgLTA.a|[algo.RspaceStr4](/txt/protocol/algo/README.md#algo-rspacestr4)|[Val](/txt/exe/amc/reftypes.md#val)|||
#### atf_amc.MsgHdrLT Fields
<a href="#atf_amc-msghdrlt-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.MsgHdrLT.len|u8|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.MsgHdrLT.type|char|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct MsgLTA
<a href="#struct-msglta"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
#pragma pack(push,1)
struct MsgLTA { // atf_amc.MsgLTA
    u8                 len;    //   ssizeof(parent) + (-2)
    char               type;   //   'A'
    algo::RspaceStr4   a;      //
    // func:atf_amc.MsgLTA..Ctor
    inline               MsgLTA() __attribute__((nothrow));
    // func:atf_amc.MsgLTA..FieldwiseCtor
    explicit inline               MsgLTA(const algo::strptr& in_a) __attribute__((nothrow));
};
```

#### atf_amc.MsgLTB - length-type header
<a href="#atf_amc-msgltb"></a>

#### atf_amc.MsgLTB Fields
<a href="#atf_amc-msgltb-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.MsgLTB.base|[atf_amc.MsgHdrLT](/txt/exe/atf_amc/internals.md#atf_amc-msghdrlt)|[Base](#atf_amc-msghdrlt-fields)|||
|atf_amc.MsgLTB.b|[algo.RspaceStr6](/txt/protocol/algo/README.md#algo-rspacestr6)|[Val](/txt/exe/amc/reftypes.md#val)|||
#### atf_amc.MsgHdrLT Fields
<a href="#atf_amc-msghdrlt-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.MsgHdrLT.len|u8|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.MsgHdrLT.type|char|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct MsgLTB
<a href="#struct-msgltb"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
#pragma pack(push,1)
struct MsgLTB { // atf_amc.MsgLTB
    u8                 len;    //   ssizeof(parent) + (-2)
    char               type;   //   'B'
    algo::RspaceStr6   b;      //
    // func:atf_amc.MsgLTB..Ctor
    inline               MsgLTB() __attribute__((nothrow));
    // func:atf_amc.MsgLTB..FieldwiseCtor
    explicit inline               MsgLTB(const algo::strptr& in_b) __attribute__((nothrow));
};
```

#### atf_amc.MsgLTO - length-type header
<a href="#atf_amc-msglto"></a>

#### atf_amc.MsgLTO Fields
<a href="#atf_amc-msglto-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.MsgLTO.base|[atf_amc.MsgHdrLT](/txt/exe/atf_amc/internals.md#atf_amc-msghdrlt)|[Base](#atf_amc-msghdrlt-fields)|||
|atf_amc.MsgLTO.o|[atf_amc.MsgHdrLT](/txt/exe/atf_amc/internals.md#atf_amc-msghdrlt)|[Opt](/txt/exe/amc/reftypes.md#opt)|||
#### atf_amc.MsgHdrLT Fields
<a href="#atf_amc-msghdrlt-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.MsgHdrLT.len|u8|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.MsgHdrLT.type|char|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct MsgLTO
<a href="#struct-msglto"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
#pragma pack(push,1)
struct MsgLTO { // atf_amc.MsgLTO
    u8                        len;    //   ssizeof(parent) + (-2)
    char                      type;   //   'O'
    // atf_amc::MsgHdrLT      o[];                                      optional field
    // func:atf_amc.MsgLTO..Ctor
    inline               MsgLTO() __attribute__((nothrow));
};
```

#### atf_amc.MsgLTV - length-type header
<a href="#atf_amc-msgltv"></a>

#### atf_amc.MsgLTV Fields
<a href="#atf_amc-msgltv-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.MsgLTV.base|[atf_amc.MsgHdrLT](/txt/exe/atf_amc/internals.md#atf_amc-msghdrlt)|[Base](#atf_amc-msghdrlt-fields)|||
|atf_amc.MsgLTV.v|[atf_amc.MsgHdrLT](/txt/exe/atf_amc/internals.md#atf_amc-msghdrlt)|[Varlen](/txt/exe/amc/reftypes.md#varlen)|||
#### atf_amc.MsgHdrLT Fields
<a href="#atf_amc-msghdrlt-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.MsgHdrLT.len|u8|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.MsgHdrLT.type|char|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct MsgLTV
<a href="#struct-msgltv"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
#pragma pack(push,1)
struct MsgLTV { // atf_amc.MsgLTV
    u8     len;    //   ssizeof(parent) + (-2)
    char   type;   //   'V'
    // var-length field atf_amc.MsgLTV.v starts here. access it with v_Addr
    // func:atf_amc.MsgLTV..Ctor
    inline               MsgLTV() __attribute__((nothrow));
};
```

#### atf_amc.Msgbuf - 
<a href="#atf_amc-msgbuf"></a>

#### atf_amc.Msgbuf Fields
<a href="#atf_amc-msgbuf-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.Msgbuf.in_buf|[atf_amc.MsgHeader](/txt/exe/atf_amc/internals.md#atf_amc-msgheader)|[Fbuf](/txt/exe/amc/reftypes.md#fbuf)||Message-based buffer|
|atf_amc.Msgbuf.in_custom|char|[Fbuf](/txt/exe/amc/reftypes.md#fbuf)||Message-based buffer|
|atf_amc.Msgbuf.out_extra|[atf_amc.MsgHdrLT](/txt/exe/atf_amc/internals.md#atf_amc-msghdrlt)|[Fbuf](/txt/exe/amc/reftypes.md#fbuf)||Message-based buffer|
|atf_amc.Msgbuf.in_extra|[atf_amc.MsgHdrLT](/txt/exe/atf_amc/internals.md#atf_amc-msghdrlt)|[Fbuf](/txt/exe/amc/reftypes.md#fbuf)||Message-based buffer|

#### Struct Msgbuf
<a href="#struct-msgbuf"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct Msgbuf { // atf_amc.Msgbuf
    atf_amc::Msgbuf*    cd_in_msg_next;           // zslist link; -1 means not-in-list
    atf_amc::Msgbuf*    cd_in_msg_prev;           // previous element
    u8*                 in_buf_elems;             //   NULL  pointer to elements of indirect array
    u32                 in_buf_max;               //   0  current length of allocated array
    i32                 in_buf_start;             // beginning of valid bytes (in bytes)
    i32                 in_buf_end;               // end of valid bytes (in bytes)
    i32                 in_buf_msglen;            // current message length
    algo::Errcode       in_buf_err;               // system error code
    algo_lib::FIohook   in_buf_iohook;            // edge-triggered hook for the buffer
    bool                in_buf_eof;               // no more data will be written to buffer
    bool                in_buf_msgvalid;          // current message is valid
    bool                in_buf_epoll_enable;      // use epoll?
    u8*                 in_custom_elems;          //   NULL  pointer to elements of indirect array
    u32                 in_custom_max;            //   0  current length of allocated array
    i32                 in_custom_start;          // beginning of valid bytes (in bytes)
    i32                 in_custom_end;            // end of valid bytes (in bytes)
    i32                 in_custom_msglen;         // current message length
    algo::Errcode       in_custom_err;            // system error code
    algo_lib::FIohook   in_custom_iohook;         // edge-triggered hook for the buffer
    bool                in_custom_eof;            // no more data will be written to buffer
    bool                in_custom_msgvalid;       // current message is valid
    bool                in_custom_epoll_enable;   // use epoll?
    u8*                 out_extra_elems;          //   NULL  pointer to elements of indirect array
    u32                 out_extra_max;            //   0  current length of allocated array
    i32                 out_extra_start;          // beginning of valid bytes (in bytes)
    i32                 out_extra_end;            // end of valid bytes (in bytes)
    i32                 out_extra_msglen;         // current message length
    algo::Errcode       out_extra_err;            // system error code
    bool                out_extra_eof;            // no more data will be written to buffer
    bool                out_extra_msgvalid;       // current message is valid
    bool                out_extra_epoll_enable;   // use epoll?
    u8*                 in_extra_elems;           //   NULL  pointer to elements of indirect array
    u32                 in_extra_max;             //   0  current length of allocated array
    i32                 in_extra_start;           // beginning of valid bytes (in bytes)
    i32                 in_extra_end;             // end of valid bytes (in bytes)
    i32                 in_extra_msglen;          // current message length
    algo::Errcode       in_extra_err;             // system error code
    bool                in_extra_eof;             // no more data will be written to buffer
    bool                in_extra_msgvalid;        // current message is valid
    bool                in_extra_epoll_enable;    // use epoll?
    // field atf_amc.Msgbuf.in_buf prevents copy
    // field atf_amc.Msgbuf.in_custom prevents copy
    // field atf_amc.Msgbuf.out_extra prevents copy
    // field atf_amc.Msgbuf.in_extra prevents copy
    // func:atf_amc.Msgbuf..AssignOp
    inline atf_amc::Msgbuf& operator =(const atf_amc::Msgbuf &rhs) = delete;
    // func:atf_amc.Msgbuf..Ctor
    inline               Msgbuf() __attribute__((nothrow));
    // func:atf_amc.Msgbuf..Dtor
    inline               ~Msgbuf() __attribute__((nothrow));
    // field atf_amc.Msgbuf.in_buf prevents copy
    // field atf_amc.Msgbuf.in_custom prevents copy
    // field atf_amc.Msgbuf.out_extra prevents copy
    // field atf_amc.Msgbuf.in_extra prevents copy
    // func:atf_amc.Msgbuf..CopyCtor
    inline               Msgbuf(const atf_amc::Msgbuf &rhs) = delete;
};
```

#### atf_amc.NetBitfld1 - 
<a href="#atf_amc-netbitfld1"></a>

#### atf_amc.NetBitfld1 Fields
<a href="#atf_amc-netbitfld1-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.NetBitfld1.value|u16|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.NetBitfld1.bits0_4|u8|[Bitfld](/txt/exe/amc/reftypes.md#bitfld)|||
|atf_amc.NetBitfld1.bits8_12|u8|[Bitfld](/txt/exe/amc/reftypes.md#bitfld)|||

#### Struct NetBitfld1
<a href="#struct-netbitfld1"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct NetBitfld1 { // atf_amc.NetBitfld1
    u16   value_be;   //   0
    // func:atf_amc.NetBitfld1..Ctor
    inline               NetBitfld1() __attribute__((nothrow));
};
```

#### atf_amc.OptAlloc - An optional instance of atf_unit.A
<a href="#atf_amc-optalloc"></a>

#### atf_amc.OptAlloc Fields
<a href="#atf_amc-optalloc-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.OptAlloc.length|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.OptAlloc.typeg|[atf_amc.TypeG](/txt/exe/atf_amc/internals.md#atf_amc-typeg)|[Opt](/txt/exe/amc/reftypes.md#opt)|||

#### Struct OptAlloc
<a href="#struct-optalloc"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
#pragma pack(push,1)
struct OptAlloc { // atf_amc.OptAlloc: An optional instance of atf_unit.A
    u32                    length;    //   0
    // atf_amc::TypeG      typeg[];                optional field
private:
    // func:atf_amc.OptAlloc..Ctor
    inline               OptAlloc() __attribute__((nothrow));
    friend atf_amc::OptAlloc&   optalloc_AllocExtra(void *extra, i32 nbyte_extra) __attribute__((__warn_unused_result__, nothrow));
    friend atf_amc::OptAlloc*   optalloc_AllocExtraMaybe(void *extra, i32 nbyte_extra) __attribute__((__warn_unused_result__, nothrow));
    friend void                 optalloc_Delete(atf_amc::OptAlloc &row) __attribute__((nothrow));
};
```

#### atf_amc.OptG - An optional instance of atf_unit.A
<a href="#atf_amc-optg"></a>

#### atf_amc.OptG Fields
<a href="#atf_amc-optg-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.OptG.length|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.OptG.typeg|[atf_amc.TypeG](/txt/exe/atf_amc/internals.md#atf_amc-typeg)|[Opt](/txt/exe/amc/reftypes.md#opt)|||

#### Struct OptG
<a href="#struct-optg"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
#pragma pack(push,1)
struct OptG { // atf_amc.OptG: An optional instance of atf_unit.A
    u32                    length;    //   0
    // atf_amc::TypeG      typeg[];                optional field
    // func:atf_amc.OptG..Ctor
    inline               OptG() __attribute__((nothrow));
};
```

#### atf_amc.OptOptG - An optional instance of atf_unit.Optg (recursive)
<a href="#atf_amc-optoptg"></a>

#### atf_amc.OptOptG Fields
<a href="#atf_amc-optoptg-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.OptOptG.length|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.OptOptG.optg|[atf_amc.OptG](/txt/exe/atf_amc/internals.md#atf_amc-optg)|[Opt](/txt/exe/amc/reftypes.md#opt)|||

#### Struct OptOptG
<a href="#struct-optoptg"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
#pragma pack(push,1)
struct OptOptG { // atf_amc.OptOptG: An optional instance of atf_unit.Optg (recursive)
    u32                   length;   //   0
    // atf_amc::OptG      optg[];                optional field
    // func:atf_amc.OptOptG..Ctor
    inline               OptOptG() __attribute__((nothrow));
};
```

#### atf_amc.PmaskMultiple - Ctype with 3 pmasks. One has an explicit set of fields
<a href="#atf_amc-pmaskmultiple"></a>

#### atf_amc.PmaskMultiple Fields
<a href="#atf_amc-pmaskmultiple-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.PmaskMultiple.present|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskMultiple.assigned|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskMultiple.nullable|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskMultiple.value1|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskMultiple.value2|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskMultiple.value3|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskMultiple.value4|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskMultiple.value5|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskMultiple.value6|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskMultiple.value7|u32|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct PmaskMultiple
<a href="#struct-pmaskmultiple"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
#pragma pack(push,1)
struct PmaskMultiple { // atf_amc.PmaskMultiple: Ctype with 3 pmasks. One has an explicit set of fields
    u32   present;    //   0
    u32   assigned;   //   0
    u32   nullable;   //   0
    u32   value1;     //   0
    u32   value2;     //   0
    u32   value3;     //   0
    u32   value4;     //   0
    u32   value5;     //   0
    u32   value6;     //   0
    u32   value7;     //   0
    // func:atf_amc.PmaskMultiple..Ctor
    inline               PmaskMultiple() __attribute__((nothrow));
};
```

#### atf_amc.PmaskU128 - A field controlled by a bitmask.
<a href="#atf_amc-pmasku128"></a>

#### atf_amc.PmaskU128 Fields
<a href="#atf_amc-pmasku128-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.PmaskU128.pmask|u128|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value2|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value3|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value21|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value22|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value23|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value24|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value25|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value26|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value20|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value28|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value29|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value30|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value31|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value32|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value27|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value19|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value18|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value4|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value5|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value6|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value7|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value8|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value9|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value69|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value11|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value12|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value13|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value14|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value15|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value16|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value33|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value10|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value17|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value35|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value55|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value56|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value57|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value58|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value59|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value60|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value54|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value62|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value63|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value64|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value65|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value66|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value67|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value68|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value61|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value34|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value52|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value36|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value37|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value38|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value39|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value40|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value41|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value42|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value53|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value44|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value45|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value46|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value47|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value48|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value49|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value50|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value51|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value43|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value70|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU128.value71|u32|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct PmaskU128
<a href="#struct-pmasku128"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
#pragma pack(push,1)
struct PmaskU128 { // atf_amc.PmaskU128: A field controlled by a bitmask.
    u128   pmask;     //   0
    u32    value;     //   0
    u32    value2;    //   0
    u32    value3;    //   0
    u32    value21;   //   0
    u32    value22;   //   0
    u32    value23;   //   0
    u32    value24;   //   0
    u32    value25;   //   0
    u32    value26;   //   0
    u32    value20;   //   0
    u32    value28;   //   0
    u32    value29;   //   0
    u32    value30;   //   0
    u32    value31;   //   0
    u32    value32;   //   0
    u32    value27;   //   0
    u32    value19;   //   0
    u32    value18;   //   0
    u32    value4;    //   0
    u32    value5;    //   0
    u32    value6;    //   0
    u32    value7;    //   0
    u32    value8;    //   0
    u32    value9;    //   0
    u32    value69;   //   0
    u32    value11;   //   0
    u32    value12;   //   0
    u32    value13;   //   0
    u32    value14;   //   0
    u32    value15;   //   0
    u32    value16;   //   0
    u32    value33;   //   0
    u32    value10;   //   0
    u32    value17;   //   0
    u32    value35;   //   0
    u32    value55;   //   0
    u32    value56;   //   0
    u32    value57;   //   0
    u32    value58;   //   0
    u32    value59;   //   0
    u32    value60;   //   0
    u32    value54;   //   0
    u32    value62;   //   0
    u32    value63;   //   0
    u32    value64;   //   0
    u32    value65;   //   0
    u32    value66;   //   0
    u32    value67;   //   0
    u32    value68;   //   0
    u32    value61;   //   0
    u32    value34;   //   0
    u32    value52;   //   0
    u32    value36;   //   0
    u32    value37;   //   0
    u32    value38;   //   0
    u32    value39;   //   0
    u32    value40;   //   0
    u32    value41;   //   0
    u32    value42;   //   0
    u32    value53;   //   0
    u32    value44;   //   0
    u32    value45;   //   0
    u32    value46;   //   0
    u32    value47;   //   0
    u32    value48;   //   0
    u32    value49;   //   0
    u32    value50;   //   0
    u32    value51;   //   0
    u32    value43;   //   0
    u32    value70;   //   0
    u32    value71;   //   0
    // func:atf_amc.PmaskU128..Ctor
    inline               PmaskU128() __attribute__((nothrow));
};
```

#### atf_amc.PmaskU32 - A field controlled by a bitmask.
<a href="#atf_amc-pmasku32"></a>

#### atf_amc.PmaskU32 Fields
<a href="#atf_amc-pmasku32-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.PmaskU32.pmask|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU32.value|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU32.value2|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU32.value3|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU32.value4|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU32.value5|u32|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct PmaskU32
<a href="#struct-pmasku32"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
#pragma pack(push,1)
struct PmaskU32 { // atf_amc.PmaskU32: A field controlled by a bitmask.
    u32   pmask;    //   0
    u32   value;    //   0
    u32   value2;   //   0
    u32   value3;   //   0
    u32   value4;   //   0
    u32   value5;   //   0
    // func:atf_amc.PmaskU32..Ctor
    inline               PmaskU32() __attribute__((nothrow));
};
```

#### atf_amc.PmaskU555 - Check pmask with 555 bits
<a href="#atf_amc-pmasku555"></a>

#### atf_amc.PmaskU555 Fields
<a href="#atf_amc-pmasku555-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.PmaskU555.value|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.PmaskU555.pmask|u64|[Inlary](/txt/exe/amc/reftypes.md#inlary)|||

#### Struct PmaskU555
<a href="#struct-pmasku555"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
#pragma pack(push,1)
struct PmaskU555 { // atf_amc.PmaskU555: Check pmask with 555 bits
    u32   value;             //   0
    u64   pmask_elems[10];   //   0  fixed array
    // func:atf_amc.PmaskU555..Ctor
    inline               PmaskU555() __attribute__((nothrow));
};
```

#### atf_amc.PooledBE64 - 
<a href="#atf_amc-pooledbe64"></a>

#### atf_amc.PooledBE64 Fields
<a href="#atf_amc-pooledbe64-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.PooledBE64.value|u64|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct PooledBE64
<a href="#struct-pooledbe64"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct PooledBE64 { // atf_amc.PooledBE64
    atf_amc::PooledBE64*   pooledbe64_next;   // Pointer to next free element int tpool
    u64                    value_be;          //   0
    // func:atf_amc.PooledBE64..EnumCtor
    inline               PooledBE64(atf_amc_PooledBE64_value_Enum arg) __attribute__((nothrow));
private:
    // func:atf_amc.PooledBE64..Ctor
    inline               PooledBE64() __attribute__((nothrow));
    friend atf_amc::PooledBE64& pooledbe64_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend atf_amc::PooledBE64* pooledbe64_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 pooledbe64_Delete(atf_amc::PooledBE64 &row) __attribute__((nothrow));
};
```

#### atf_amc.RnullStr6_U32 - number stored as ascii digits, right pad with NUL
<a href="#atf_amc-rnullstr6_u32"></a>

#### atf_amc.RnullStr6_U32 Fields
<a href="#atf_amc-rnullstr6_u32-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.RnullStr6_U32.ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### Struct RnullStr6_U32
<a href="#struct-rnullstr6_u32"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct RnullStr6_U32 { // atf_amc.RnullStr6_U32: number stored as ascii digits, right pad with NUL
    enum { ch_max = 6 };
    u8 ch[6];
    // Copy from strptr (operator=)
    // func:atf_amc.RnullStr6_U32.ch.AssignStrptr
    inline void          operator =(const algo::strptr &str) __attribute__((nothrow));
    // func:atf_amc.RnullStr6_U32.ch.CtorStrptr
    inline               RnullStr6_U32(const algo::strptr &rhs) __attribute__((nothrow));
    // func:atf_amc.RnullStr6_U32..EqOp
    inline bool          operator ==(const atf_amc::RnullStr6_U32 &rhs) const __attribute__((nothrow));
    // func:atf_amc.RnullStr6_U32..NeOp
    inline bool          operator !=(const atf_amc::RnullStr6_U32 &rhs) const __attribute__((nothrow));
    // func:atf_amc.RnullStr6_U32..LtOp
    inline bool          operator <(const atf_amc::RnullStr6_U32 &rhs) const __attribute__((nothrow));
    // func:atf_amc.RnullStr6_U32..GtOp
    inline bool          operator >(const atf_amc::RnullStr6_U32 &rhs) const __attribute__((nothrow));
    // func:atf_amc.RnullStr6_U32..LeOp
    inline bool          operator <=(const atf_amc::RnullStr6_U32 &rhs) const __attribute__((nothrow));
    // func:atf_amc.RnullStr6_U32..GeOp
    inline bool          operator >=(const atf_amc::RnullStr6_U32 &rhs) const __attribute__((nothrow));
    // func:atf_amc.RnullStr6_U32..EqOpAryptr
    inline bool          operator ==(const algo::aryptr<char> &rhs) const __attribute__((nothrow));
    // func:atf_amc.RnullStr6_U32..AssignOp
    inline atf_amc::RnullStr6_U32& operator =(const atf_amc::RnullStr6_U32 &rhs) __attribute__((nothrow));
    // func:atf_amc.RnullStr6_U32..Ctor
    inline               RnullStr6_U32() __attribute__((nothrow));
    // func:atf_amc.RnullStr6_U32..CopyCtor
    inline               RnullStr6_U32(const atf_amc::RnullStr6_U32 &rhs) __attribute__((nothrow));
};
```

#### atf_amc.RpasU32Str6 - number stored as ascii digits, right pad with NUL
<a href="#atf_amc-rpasu32str6"></a>

#### atf_amc.RpasU32Str6 Fields
<a href="#atf_amc-rpasu32str6-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.RpasU32Str6.ch|char|[Smallstr](/txt/exe/amc/reftypes.md#smallstr)|||

#### Struct RpasU32Str6
<a href="#struct-rpasu32str6"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct RpasU32Str6 { // atf_amc.RpasU32Str6: number stored as ascii digits, right pad with NUL
    enum { ch_max = 6 };
    u8 ch[6+1];
    u8 n_ch;

    // Copy from strptr (operator=)
    // func:atf_amc.RpasU32Str6.ch.AssignStrptr
    inline void          operator =(const algo::strptr &str) __attribute__((nothrow));
    // func:atf_amc.RpasU32Str6.ch.CtorStrptr
    inline               RpasU32Str6(const algo::strptr &rhs) __attribute__((nothrow));
    // func:atf_amc.RpasU32Str6..EqOp
    inline bool          operator ==(const atf_amc::RpasU32Str6 &rhs) const __attribute__((nothrow));
    // func:atf_amc.RpasU32Str6..NeOp
    inline bool          operator !=(const atf_amc::RpasU32Str6 &rhs) const __attribute__((nothrow));
    // func:atf_amc.RpasU32Str6..LtOp
    inline bool          operator <(const atf_amc::RpasU32Str6 &rhs) const __attribute__((nothrow));
    // func:atf_amc.RpasU32Str6..GtOp
    inline bool          operator >(const atf_amc::RpasU32Str6 &rhs) const __attribute__((nothrow));
    // func:atf_amc.RpasU32Str6..LeOp
    inline bool          operator <=(const atf_amc::RpasU32Str6 &rhs) const __attribute__((nothrow));
    // func:atf_amc.RpasU32Str6..GeOp
    inline bool          operator >=(const atf_amc::RpasU32Str6 &rhs) const __attribute__((nothrow));
    // func:atf_amc.RpasU32Str6..EqOpAryptr
    inline bool          operator ==(const algo::aryptr<char> &rhs) const __attribute__((nothrow));
    // func:atf_amc.RpasU32Str6..AssignOp
    inline atf_amc::RpasU32Str6& operator =(const atf_amc::RpasU32Str6 &rhs) __attribute__((nothrow));
    // func:atf_amc.RpasU32Str6..Ctor
    inline               RpasU32Str6() __attribute__((nothrow));
    // func:atf_amc.RpasU32Str6..CopyCtor
    inline               RpasU32Str6(const atf_amc::RpasU32Str6 &rhs) __attribute__((nothrow));
};
```

#### atf_amc.Sep1 - Check printing/reading of types with separators
<a href="#atf_amc-sep1"></a>

#### atf_amc.Sep1 Fields
<a href="#atf_amc-sep1-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.Sep1.val1|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.Sep1.val2|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.Sep1.val3|u32|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct Sep1
<a href="#struct-sep1"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct Sep1 { // atf_amc.Sep1: Check printing/reading of types with separators
    u32   val1;   //   0
    u32   val2;   //   0
    u32   val3;   //   0
    // func:atf_amc.Sep1..EqOp
    inline bool          operator ==(const atf_amc::Sep1 &rhs) const __attribute__((nothrow));
    // func:atf_amc.Sep1..NeOp
    inline bool          operator !=(const atf_amc::Sep1 &rhs) const __attribute__((nothrow));
    // func:atf_amc.Sep1..LtOp
    inline bool          operator <(const atf_amc::Sep1 &rhs) const __attribute__((nothrow));
    // func:atf_amc.Sep1..GtOp
    inline bool          operator >(const atf_amc::Sep1 &rhs) const __attribute__((nothrow));
    // func:atf_amc.Sep1..LeOp
    inline bool          operator <=(const atf_amc::Sep1 &rhs) const __attribute__((nothrow));
    // func:atf_amc.Sep1..GeOp
    inline bool          operator >=(const atf_amc::Sep1 &rhs) const __attribute__((nothrow));
    // func:atf_amc.Sep1..Ctor
    inline               Sep1() __attribute__((nothrow));
    // func:atf_amc.Sep1..FieldwiseCtor
    explicit inline               Sep1(u32 in_val1, u32 in_val2, u32 in_val3) __attribute__((nothrow));
};
```

#### atf_amc.Seqmsg - Sequenced message for stream
<a href="#atf_amc-seqmsg"></a>

#### atf_amc.Seqmsg Fields
<a href="#atf_amc-seqmsg-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.Seqmsg.msghdr|[atf_amc.MsgHeader](/txt/exe/atf_amc/internals.md#atf_amc-msgheader)|[Base](#atf_amc-msgheader-fields)|||
|atf_amc.Seqmsg.payload|[atf_amc.MsgHeader](/txt/exe/atf_amc/internals.md#atf_amc-msgheader)|[Opt](/txt/exe/amc/reftypes.md#opt)||Message header for the payload|
#### atf_amc.MsgHeader Fields
<a href="#atf_amc-msgheader-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.MsgHeader.type|[atf_amc.MsgType](/txt/exe/atf_amc/internals.md#atf_amc-msgtype)|[Val](/txt/exe/amc/reftypes.md#val)||Message type|
|atf_amc.MsgHeader.length|[atf_amc.MsgLength](/txt/exe/atf_amc/internals.md#atf_amc-msglength)|[Val](/txt/exe/amc/reftypes.md#val)||Total message length, including this header|

#### Struct Seqmsg
<a href="#struct-seqmsg"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
#pragma pack(push,1)
struct Seqmsg { // atf_amc.Seqmsg: Sequenced message for stream
    atf_amc::MsgType           type;        //   0x0905  Message type
    atf_amc::MsgLength         length;      //   ssizeof(parent) + (0)  Total message length, including this header
    // atf_amc::MsgHeader      payload[];                                                                               optional field
    // func:atf_amc.Seqmsg..Ctor
    inline               Seqmsg() __attribute__((nothrow));
};
```

#### atf_amc.SortedStr - 
<a href="#atf_amc-sortedstr"></a>

#### atf_amc.SortedStr Fields
<a href="#atf_amc-sortedstr-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.SortedStr.novs|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.SortedStr.vs|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct SortedStr
<a href="#struct-sortedstr"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct SortedStr { // atf_amc.SortedStr
    algo::cstring   novs;   //
    algo::cstring   vs;     //
    // func:atf_amc.SortedStr..EqOp
    inline bool          operator ==(const atf_amc::SortedStr &rhs) const __attribute__((nothrow));
    // func:atf_amc.SortedStr..NeOp
    inline bool          operator !=(const atf_amc::SortedStr &rhs) const __attribute__((nothrow));
    // func:atf_amc.SortedStr..LtOp
    inline bool          operator <(const atf_amc::SortedStr &rhs) const __attribute__((nothrow));
    // func:atf_amc.SortedStr..GtOp
    inline bool          operator >(const atf_amc::SortedStr &rhs) const __attribute__((nothrow));
    // func:atf_amc.SortedStr..LeOp
    inline bool          operator <=(const atf_amc::SortedStr &rhs) const __attribute__((nothrow));
    // func:atf_amc.SortedStr..GeOp
    inline bool          operator >=(const atf_amc::SortedStr &rhs) const __attribute__((nothrow));
    // func:atf_amc.SortedStr..Ctor
    inline               SortedStr() __attribute__((nothrow));
    // func:atf_amc.SortedStr..FieldwiseCtor
    explicit inline               SortedStr(const algo::strptr& in_novs, const algo::strptr& in_vs) __attribute__((nothrow));
};
```

#### atf_amc.Strkey - 
<a href="#atf_amc-strkey"></a>

#### atf_amc.Strkey Fields
<a href="#atf_amc-strkey-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.Strkey.strkey|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct Strkey
<a href="#struct-strkey"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct Strkey { // atf_amc.Strkey
    atf_amc::Strkey*   strkey_next;          // Pointer to next free element int tpool
    atf_amc::Strkey*   ind_strkey_next;      // hash next
    u32                ind_strkey_hashval;   // hash value
    algo::cstring      strkey;               //
    // func:atf_amc.Strkey..AssignOp
    inline atf_amc::Strkey& operator =(const atf_amc::Strkey &rhs) = delete;
    // func:atf_amc.Strkey..CopyCtor
    inline               Strkey(const atf_amc::Strkey &rhs) = delete;
private:
    // func:atf_amc.Strkey..Ctor
    inline               Strkey() __attribute__((nothrow));
    // func:atf_amc.Strkey..Dtor
    inline               ~Strkey() __attribute__((nothrow));
    friend atf_amc::Strkey&     strkey_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend atf_amc::Strkey*     strkey_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 strkey_Delete(atf_amc::Strkey &row) __attribute__((nothrow));
};
```

#### atf_amc.TaryU32 - 
<a href="#atf_amc-taryu32"></a>

#### atf_amc.TaryU32 Fields
<a href="#atf_amc-taryu32-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.TaryU32.tary_u32|u32|[Tary](/txt/exe/amc/reftypes.md#tary)|44||

#### Struct TaryU32
<a href="#struct-taryu32"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct TaryU32 { // atf_amc.TaryU32
    u32*   tary_u32_elems;   // pointer to elements
    u32    tary_u32_n;       // number of elements in array
    u32    tary_u32_max;     // max. capacity of array before realloc
    // Copy from aryptr (operator=)
    // func:atf_amc.TaryU32.tary_u32.AssignAryptr
    inline void          operator =(const algo::aryptr<u32> &rhs) __attribute__((nothrow));
    // func:atf_amc.TaryU32.tary_u32.CtorAryptr
    explicit inline               TaryU32(const algo::aryptr<u32> &rhs) __attribute__((nothrow));
    // func:atf_amc.TaryU32..AssignOp
    atf_amc::TaryU32&    operator =(const atf_amc::TaryU32 &rhs) __attribute__((nothrow));
    // func:atf_amc.TaryU32..Ctor
    inline               TaryU32() __attribute__((nothrow));
    // func:atf_amc.TaryU32..Dtor
    inline               ~TaryU32() __attribute__((nothrow));
    // func:atf_amc.TaryU32..CopyCtor
    TaryU32(const atf_amc::TaryU32 &rhs) __attribute__((nothrow));
};
```

#### atf_amc.TaryU8 - 
<a href="#atf_amc-taryu8"></a>

#### atf_amc.TaryU8 Fields
<a href="#atf_amc-taryu8-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.TaryU8.ary|u8|[Tary](/txt/exe/amc/reftypes.md#tary)|44||

#### Struct TaryU8
<a href="#struct-taryu8"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct TaryU8 { // atf_amc.TaryU8
    u8*   ary_elems;   // pointer to elements
    u32   ary_n;       // number of elements in array
    u32   ary_max;     // max. capacity of array before realloc
    // Copy from aryptr (operator=)
    // func:atf_amc.TaryU8.ary.AssignAryptr
    inline void          operator =(const algo::aryptr<u8> &rhs) __attribute__((nothrow));
    // func:atf_amc.TaryU8.ary.CtorAryptr
    explicit inline               TaryU8(const algo::aryptr<u8> &rhs) __attribute__((nothrow));
    // func:atf_amc.TaryU8..AssignOp
    atf_amc::TaryU8&     operator =(const atf_amc::TaryU8 &rhs) __attribute__((nothrow));
    // func:atf_amc.TaryU8..Ctor
    inline               TaryU8() __attribute__((nothrow));
    // func:atf_amc.TaryU8..Dtor
    inline               ~TaryU8() __attribute__((nothrow));
    // func:atf_amc.TaryU8..CopyCtor
    TaryU8(const atf_amc::TaryU8 &rhs) __attribute__((nothrow));
};
```

#### atf_amc.TestRegx1 - Test automatic reading of Regx
<a href="#atf_amc-testregx1"></a>

#### atf_amc.TestRegx1 Fields
<a href="#atf_amc-testregx1-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.TestRegx1.val|[dev.Target](/txt/ssimdb/dev/target.md)|[RegxSql](/txt/exe/amc/reftypes.md#regxsql)|||

#### Struct TestRegx1
<a href="#struct-testregx1"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct TestRegx1 { // atf_amc.TestRegx1: Test automatic reading of Regx
    algo_lib::Regx   val;   // Sql Regx of dev::Target
    // func:atf_amc.TestRegx1..Ctor
    inline               TestRegx1() __attribute__((nothrow));
};
```

#### atf_amc.TestType - 
<a href="#atf_amc-testtype"></a>

#### atf_amc.TestType Fields
<a href="#atf_amc-testtype-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|

#### Struct TestType
<a href="#struct-testtype"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct TestType { // atf_amc.TestType
    // func:atf_amc.TestType..Ctor
    inline               TestType() __attribute__((nothrow));
};
```

#### atf_amc.Text - 
<a href="#atf_amc-text"></a>

#### atf_amc.Text Fields
<a href="#atf_amc-text-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.Text.msghdr|[atf_amc.MsgHeader](/txt/exe/atf_amc/internals.md#atf_amc-msgheader)|[Base](#atf_amc-msgheader-fields)|||
|atf_amc.Text.text|char|[Varlen](/txt/exe/amc/reftypes.md#varlen)|||
#### atf_amc.MsgHeader Fields
<a href="#atf_amc-msgheader-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.MsgHeader.type|[atf_amc.MsgType](/txt/exe/atf_amc/internals.md#atf_amc-msgtype)|[Val](/txt/exe/amc/reftypes.md#val)||Message type|
|atf_amc.MsgHeader.length|[atf_amc.MsgLength](/txt/exe/atf_amc/internals.md#atf_amc-msglength)|[Val](/txt/exe/amc/reftypes.md#val)||Total message length, including this header|

#### Struct Text
<a href="#struct-text"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
#pragma pack(push,1)
struct Text { // atf_amc.Text
    atf_amc::MsgType     type;     //   0x0412  Message type
    atf_amc::MsgLength   length;   //   ssizeof(parent) + (0)  Total message length, including this header
    // var-length field atf_amc.Text.text starts here. access it with text_Addr
    // func:atf_amc.Text..Ctor
    inline               Text() __attribute__((nothrow));
};
```

#### atf_amc.TypeB - 
<a href="#atf_amc-typeb"></a>

#### atf_amc.TypeB Fields
<a href="#atf_amc-typeb-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.TypeB.typea|[atf_amc.TypeA](/txt/exe/atf_amc/internals.md#atf_amc-typea)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|||
|atf_amc.TypeB.j|i32|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct TypeB
<a href="#struct-typeb"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct TypeB { // atf_amc.TypeB
    i32   typea;   //   0
    i32   j;       //   0
    // func:atf_amc.TypeB..EqOp
    inline bool          operator ==(const atf_amc::TypeB &rhs) const __attribute__((nothrow));
    // func:atf_amc.TypeB..NeOp
    inline bool          operator !=(const atf_amc::TypeB &rhs) const __attribute__((nothrow));
    // func:atf_amc.TypeB..LtOp
    inline bool          operator <(const atf_amc::TypeB &rhs) const __attribute__((nothrow));
    // func:atf_amc.TypeB..GtOp
    inline bool          operator >(const atf_amc::TypeB &rhs) const __attribute__((nothrow));
    // func:atf_amc.TypeB..LeOp
    inline bool          operator <=(const atf_amc::TypeB &rhs) const __attribute__((nothrow));
    // func:atf_amc.TypeB..GeOp
    inline bool          operator >=(const atf_amc::TypeB &rhs) const __attribute__((nothrow));
    // func:atf_amc.TypeB..Ctor
    inline               TypeB() __attribute__((nothrow));
};
```

#### atf_amc.TypeBE16 - 
<a href="#atf_amc-typebe16"></a>

#### atf_amc.TypeBE16 Fields
<a href="#atf_amc-typebe16-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.TypeBE16.value|u16|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct TypeBE16
<a href="#struct-typebe16"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct TypeBE16 { // atf_amc.TypeBE16
    u16   value_be;   //   0
    // func:atf_amc.TypeBE16..Ctor
    inline               TypeBE16() __attribute__((nothrow));
};
```

#### atf_amc.TypeBE32 - 
<a href="#atf_amc-typebe32"></a>

#### atf_amc.TypeBE32 Fields
<a href="#atf_amc-typebe32-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.TypeBE32.value|u32|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct TypeBE32
<a href="#struct-typebe32"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct TypeBE32 { // atf_amc.TypeBE32
    u32   value_be;   //   0
    // func:atf_amc.TypeBE32..Ctor
    inline               TypeBE32() __attribute__((nothrow));
};
```

#### atf_amc.TypeBE32en - 
<a href="#atf_amc-typebe32en"></a>

#### atf_amc.TypeBE32en Fields
<a href="#atf_amc-typebe32en-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.TypeBE32en.value|u32|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct TypeBE32en
<a href="#struct-typebe32en"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct TypeBE32en { // atf_amc.TypeBE32en
    u32   value_be;   //   0
    // func:atf_amc.TypeBE32en.value.Cast
    inline               operator atf_amc_TypeBE32en_value_Enum() const __attribute__((nothrow));
    // func:atf_amc.TypeBE32en..Ctor
    inline               TypeBE32en() __attribute__((nothrow));
    // func:atf_amc.TypeBE32en..FieldwiseCtor
    explicit inline               TypeBE32en(u32 in_value) __attribute__((nothrow));
    // func:atf_amc.TypeBE32en..EnumCtor
    inline               TypeBE32en(atf_amc_TypeBE32en_value_Enum arg) __attribute__((nothrow));
};
```

#### atf_amc.TypeBE64 - 
<a href="#atf_amc-typebe64"></a>

#### atf_amc.TypeBE64 Fields
<a href="#atf_amc-typebe64-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.TypeBE64.value|u64|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct TypeBE64
<a href="#struct-typebe64"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct TypeBE64 { // atf_amc.TypeBE64
    u64   value_be;   //   0
    // func:atf_amc.TypeBE64.value.Cast
    inline               operator u64() const __attribute__((nothrow));
    // func:atf_amc.TypeBE64..EqOp
    inline bool          operator ==(const atf_amc::TypeBE64 &rhs) const __attribute__((nothrow));
    // func:atf_amc.TypeBE64..NeOp
    inline bool          operator !=(const atf_amc::TypeBE64 &rhs) const __attribute__((nothrow));
    // func:atf_amc.TypeBE64..LtOp
    inline bool          operator <(const atf_amc::TypeBE64 &rhs) const __attribute__((nothrow));
    // func:atf_amc.TypeBE64..GtOp
    inline bool          operator >(const atf_amc::TypeBE64 &rhs) const __attribute__((nothrow));
    // func:atf_amc.TypeBE64..LeOp
    inline bool          operator <=(const atf_amc::TypeBE64 &rhs) const __attribute__((nothrow));
    // func:atf_amc.TypeBE64..GeOp
    inline bool          operator >=(const atf_amc::TypeBE64 &rhs) const __attribute__((nothrow));
    // func:atf_amc.TypeBE64..Ctor
    inline               TypeBE64() __attribute__((nothrow));
    // func:atf_amc.TypeBE64..FieldwiseCtor
    explicit inline               TypeBE64(u64 in_value) __attribute__((nothrow));
};
```

#### atf_amc.TypeBE64dflt - 
<a href="#atf_amc-typebe64dflt"></a>

#### atf_amc.TypeBE64dflt Fields
<a href="#atf_amc-typebe64dflt-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.TypeBE64dflt.value|u64|[Val](/txt/exe/amc/reftypes.md#val)|0xfedcba9876543210||

#### Struct TypeBE64dflt
<a href="#struct-typebe64dflt"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct TypeBE64dflt { // atf_amc.TypeBE64dflt
    u64   value_be;   //   0xfedcba9876543210
    // func:atf_amc.TypeBE64dflt.value.Cast
    inline               operator u64() const __attribute__((nothrow));
    // func:atf_amc.TypeBE64dflt..Ctor
    inline               TypeBE64dflt() __attribute__((nothrow));
    // func:atf_amc.TypeBE64dflt..FieldwiseCtor
    explicit inline               TypeBE64dflt(u64 in_value) __attribute__((nothrow));
};
```

#### atf_amc.TypeBE64sf - 
<a href="#atf_amc-typebe64sf"></a>

#### atf_amc.TypeBE64sf Fields
<a href="#atf_amc-typebe64sf-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.TypeBE64sf.value|u64|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.TypeBE64sf.bit63|u64|[Bitfld](/txt/exe/amc/reftypes.md#bitfld)|||
|atf_amc.TypeBE64sf.bits62_61|u64|[Bitfld](/txt/exe/amc/reftypes.md#bitfld)|||
|atf_amc.TypeBE64sf.bits60_58|u64|[Bitfld](/txt/exe/amc/reftypes.md#bitfld)|||
|atf_amc.TypeBE64sf.bits57_53|u64|[Bitfld](/txt/exe/amc/reftypes.md#bitfld)|||
|atf_amc.TypeBE64sf.bits52_45|u64|[Bitfld](/txt/exe/amc/reftypes.md#bitfld)|||
|atf_amc.TypeBE64sf.bits44_32|u64|[Bitfld](/txt/exe/amc/reftypes.md#bitfld)|||
|atf_amc.TypeBE64sf.bits31_11|u64|[Bitfld](/txt/exe/amc/reftypes.md#bitfld)|||
|atf_amc.TypeBE64sf.bits10_0|u64|[Bitfld](/txt/exe/amc/reftypes.md#bitfld)|||

#### Struct TypeBE64sf
<a href="#struct-typebe64sf"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct TypeBE64sf { // atf_amc.TypeBE64sf
    u64   value_be;   //   0
    // func:atf_amc.TypeBE64sf..Ctor
    inline               TypeBE64sf() __attribute__((nothrow));
};
```

#### atf_amc.TypeC - 
<a href="#atf_amc-typec"></a>

#### atf_amc.TypeC Fields
<a href="#atf_amc-typec-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.TypeC.typec|i32|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct TypeC
<a href="#struct-typec"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct TypeC { // atf_amc.TypeC
    i32   typec;   //   0
    // func:atf_amc.TypeC..EqOp
    inline bool          operator ==(const atf_amc::TypeC &rhs) const __attribute__((nothrow));
    // func:atf_amc.TypeC..NeOp
    inline bool          operator !=(const atf_amc::TypeC &rhs) const __attribute__((nothrow));
    // func:atf_amc.TypeC..LtOp
    inline bool          operator <(const atf_amc::TypeC &rhs) const __attribute__((nothrow));
    // func:atf_amc.TypeC..GtOp
    inline bool          operator >(const atf_amc::TypeC &rhs) const __attribute__((nothrow));
    // func:atf_amc.TypeC..LeOp
    inline bool          operator <=(const atf_amc::TypeC &rhs) const __attribute__((nothrow));
    // func:atf_amc.TypeC..GeOp
    inline bool          operator >=(const atf_amc::TypeC &rhs) const __attribute__((nothrow));
    // func:atf_amc.TypeC..Ctor
    inline               TypeC() __attribute__((nothrow));
};
```

#### atf_amc.TypeH - 
<a href="#atf_amc-typeh"></a>

#### atf_amc.TypeH Fields
<a href="#atf_amc-typeh-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.TypeH.typeh|i32|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct TypeH
<a href="#struct-typeh"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct TypeH { // atf_amc.TypeH
    i32   typeh;   //   0
    // func:atf_amc.TypeH..EqOp
    inline bool          operator ==(const atf_amc::TypeH &rhs) const __attribute__((nothrow));
    // func:atf_amc.TypeH..NeOp
    inline bool          operator !=(const atf_amc::TypeH &rhs) const __attribute__((nothrow));
    // func:atf_amc.TypeH..LtOp
    inline bool          operator <(const atf_amc::TypeH &rhs) const __attribute__((nothrow));
    // func:atf_amc.TypeH..GtOp
    inline bool          operator >(const atf_amc::TypeH &rhs) const __attribute__((nothrow));
    // func:atf_amc.TypeH..LeOp
    inline bool          operator <=(const atf_amc::TypeH &rhs) const __attribute__((nothrow));
    // func:atf_amc.TypeH..GeOp
    inline bool          operator >=(const atf_amc::TypeH &rhs) const __attribute__((nothrow));
    // func:atf_amc.TypeH..Ctor
    inline               TypeH() __attribute__((nothrow));
};
```

#### atf_amc.TypeS - 
<a href="#atf_amc-types"></a>

#### atf_amc.TypeS Fields
<a href="#atf_amc-types-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.TypeS.types|i32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.TypeS.comment|[algo.Comment](/txt/protocol/algo/Comment.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct TypeS
<a href="#struct-types"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct TypeS { // atf_amc.TypeS
    i32             types;     //   0
    algo::Comment   comment;   //
    // func:atf_amc.TypeS..Ctor
    inline               TypeS() __attribute__((nothrow));
};
```

#### atf_amc.TypeT - Refers to TypeS
<a href="#atf_amc-typet"></a>

#### atf_amc.TypeT Fields
<a href="#atf_amc-typet-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.TypeT.types|[atf_amc.TypeS](/txt/exe/atf_amc/internals.md#atf_amc-types)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|||
|atf_amc.TypeT.j|i32|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct TypeT
<a href="#struct-typet"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct TypeT { // atf_amc.TypeT: Refers to TypeS
    i32   types;   //   0
    i32   j;       //   0
    // func:atf_amc.TypeT..Ctor
    inline               TypeT() __attribute__((nothrow));
};
```

#### atf_amc.TypeTVal - Refers to TypeS as Val
<a href="#atf_amc-typetval"></a>

#### atf_amc.TypeTVal Fields
<a href="#atf_amc-typetval-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.TypeTVal.types|[atf_amc.TypeS](/txt/exe/atf_amc/internals.md#atf_amc-types)|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.TypeTVal.j|i32|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct TypeTVal
<a href="#struct-typetval"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct TypeTVal { // atf_amc.TypeTVal: Refers to TypeS as Val
    atf_amc::TypeS   types;   //
    i32              j;       //   0
    // func:atf_amc.TypeTVal..Ctor
    inline               TypeTVal() __attribute__((nothrow));
};
```

#### atf_amc.Typefconst - 
<a href="#atf_amc-typefconst"></a>

#### atf_amc.Typefconst Fields
<a href="#atf_amc-typefconst-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.Typefconst.value|u32|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct Typefconst
<a href="#struct-typefconst"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct Typefconst { // atf_amc.Typefconst
    u32   value;   //   0
    // func:atf_amc.Typefconst.value.Cast
    inline               operator atf_amc_Typefconst_value_Enum() const __attribute__((nothrow));
    // func:atf_amc.Typefconst..Ctor
    inline               Typefconst() __attribute__((nothrow));
    // func:atf_amc.Typefconst..FieldwiseCtor
    explicit inline               Typefconst(u32 in_value) __attribute__((nothrow));
    // func:atf_amc.Typefconst..EnumCtor
    inline               Typefconst(atf_amc_Typefconst_value_Enum arg) __attribute__((nothrow));
};
```

#### atf_amc.Varlen2Msg - 
<a href="#atf_amc-varlen2msg"></a>

#### atf_amc.Varlen2Msg Fields
<a href="#atf_amc-varlen2msg-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.Varlen2Msg.base|[atf_amc.MsgHeader](/txt/exe/atf_amc/internals.md#atf_amc-msgheader)|[Base](#atf_amc-msgheader-fields)|||
|atf_amc.Varlen2Msg.s1|char|[Varlen](/txt/exe/amc/reftypes.md#varlen)|||
|atf_amc.Varlen2Msg.s2|char|[Varlen](/txt/exe/amc/reftypes.md#varlen)|||
|atf_amc.Varlen2Msg.s3|char|[Varlen](/txt/exe/amc/reftypes.md#varlen)|||
#### atf_amc.MsgHeader Fields
<a href="#atf_amc-msgheader-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.MsgHeader.type|[atf_amc.MsgType](/txt/exe/atf_amc/internals.md#atf_amc-msgtype)|[Val](/txt/exe/amc/reftypes.md#val)||Message type|
|atf_amc.MsgHeader.length|[atf_amc.MsgLength](/txt/exe/atf_amc/internals.md#atf_amc-msglength)|[Val](/txt/exe/amc/reftypes.md#val)||Total message length, including this header|

#### Struct Varlen2Msg
<a href="#struct-varlen2msg"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
#pragma pack(push,1)
struct Varlen2Msg { // atf_amc.Varlen2Msg
    atf_amc::MsgType     type;     //   0x1001  Message type
    atf_amc::MsgLength   length;   //   ssizeof(parent) + (0)  Total message length, including this header
    atf_amc::MsgLength   s1_end;   // end of s1 field
    // var-length field atf_amc.Varlen2Msg.s1 starts here. access it with s1_Addr
    atf_amc::MsgLength   s2_end;   // end of s2 field
    // var-length field atf_amc.Varlen2Msg.s2 starts here. access it with s2_Addr
    // var-length field atf_amc.Varlen2Msg.s3 starts here. access it with s3_Addr
    // func:atf_amc.Varlen2Msg..Ctor
    inline               Varlen2Msg() __attribute__((nothrow));
};
```

#### atf_amc.Varlen2aMsg - 
<a href="#atf_amc-varlen2amsg"></a>

#### atf_amc.Varlen2aMsg Fields
<a href="#atf_amc-varlen2amsg-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.Varlen2aMsg.base|[atf_amc.MsgHeader](/txt/exe/atf_amc/internals.md#atf_amc-msgheader)|[Base](#atf_amc-msgheader-fields)|||
|atf_amc.Varlen2aMsg.u1|u32|[Varlen](/txt/exe/amc/reftypes.md#varlen)|||
|atf_amc.Varlen2aMsg.u2|u32|[Varlen](/txt/exe/amc/reftypes.md#varlen)|||
|atf_amc.Varlen2aMsg.u3|u32|[Varlen](/txt/exe/amc/reftypes.md#varlen)|||
#### atf_amc.MsgHeader Fields
<a href="#atf_amc-msgheader-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.MsgHeader.type|[atf_amc.MsgType](/txt/exe/atf_amc/internals.md#atf_amc-msgtype)|[Val](/txt/exe/amc/reftypes.md#val)||Message type|
|atf_amc.MsgHeader.length|[atf_amc.MsgLength](/txt/exe/atf_amc/internals.md#atf_amc-msglength)|[Val](/txt/exe/amc/reftypes.md#val)||Total message length, including this header|

#### Struct Varlen2aMsg
<a href="#struct-varlen2amsg"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
#pragma pack(push,1)
struct Varlen2aMsg { // atf_amc.Varlen2aMsg
    atf_amc::MsgType     type;     //   0x1002  Message type
    atf_amc::MsgLength   length;   //   ssizeof(parent) + (0)  Total message length, including this header
    atf_amc::MsgLength   u1_end;   // end of u1 field
    // var-length field atf_amc.Varlen2aMsg.u1 starts here. access it with u1_Addr
    atf_amc::MsgLength   u2_end;   // end of u2 field
    // var-length field atf_amc.Varlen2aMsg.u2 starts here. access it with u2_Addr
    // var-length field atf_amc.Varlen2aMsg.u3 starts here. access it with u3_Addr
    // func:atf_amc.Varlen2aMsg..Ctor
    inline               Varlen2aMsg() __attribute__((nothrow));
};
```

#### atf_amc.Varlen2mMsg - 
<a href="#atf_amc-varlen2mmsg"></a>

#### atf_amc.Varlen2mMsg Fields
<a href="#atf_amc-varlen2mmsg-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.Varlen2mMsg.base|[atf_amc.MsgHeader](/txt/exe/atf_amc/internals.md#atf_amc-msgheader)|[Base](#atf_amc-msgheader-fields)|||
|atf_amc.Varlen2mMsg.m1|[atf_amc.MsgHeader](/txt/exe/atf_amc/internals.md#atf_amc-msgheader)|[Varlen](/txt/exe/amc/reftypes.md#varlen)|||
|atf_amc.Varlen2mMsg.m2|[atf_amc.MsgHeader](/txt/exe/atf_amc/internals.md#atf_amc-msgheader)|[Varlen](/txt/exe/amc/reftypes.md#varlen)|||
|atf_amc.Varlen2mMsg.m3|[atf_amc.MsgHeader](/txt/exe/atf_amc/internals.md#atf_amc-msgheader)|[Varlen](/txt/exe/amc/reftypes.md#varlen)|||
#### atf_amc.MsgHeader Fields
<a href="#atf_amc-msgheader-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.MsgHeader.type|[atf_amc.MsgType](/txt/exe/atf_amc/internals.md#atf_amc-msgtype)|[Val](/txt/exe/amc/reftypes.md#val)||Message type|
|atf_amc.MsgHeader.length|[atf_amc.MsgLength](/txt/exe/atf_amc/internals.md#atf_amc-msglength)|[Val](/txt/exe/amc/reftypes.md#val)||Total message length, including this header|

#### Struct Varlen2mMsg
<a href="#struct-varlen2mmsg"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
#pragma pack(push,1)
struct Varlen2mMsg { // atf_amc.Varlen2mMsg
    atf_amc::MsgType     type;     //   0x1004  Message type
    atf_amc::MsgLength   length;   //   ssizeof(parent) + (0)  Total message length, including this header
    atf_amc::MsgLength   m1_end;   // end of m1 field
    // var-length field atf_amc.Varlen2mMsg.m1 starts here. access it with m1_Addr
    atf_amc::MsgLength   m2_end;   // end of m2 field
    // var-length field atf_amc.Varlen2mMsg.m2 starts here. access it with m2_Addr
    // var-length field atf_amc.Varlen2mMsg.m3 starts here. access it with m3_Addr
    // func:atf_amc.Varlen2mMsg..Ctor
    inline               Varlen2mMsg() __attribute__((nothrow));
};
```

#### atf_amc.VarlenK - An optional instance of atf_unit.A
<a href="#atf_amc-varlenk"></a>

#### atf_amc.VarlenK Fields
<a href="#atf_amc-varlenk-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.VarlenK.length|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.VarlenK.i|u32|[Varlen](/txt/exe/amc/reftypes.md#varlen)|||

#### Struct VarlenK
<a href="#struct-varlenk"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
#pragma pack(push,1)
struct VarlenK { // atf_amc.VarlenK: An optional instance of atf_unit.A
    u32   length;   //   0
    // var-length field atf_amc.VarlenK.i starts here. access it with i_Addr
    // func:atf_amc.VarlenK..Ctor
    inline               VarlenK() __attribute__((nothrow));
};
```

#### atf_amc.Varlen2vMsg - 
<a href="#atf_amc-varlen2vmsg"></a>

#### atf_amc.Varlen2vMsg Fields
<a href="#atf_amc-varlen2vmsg-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.Varlen2vMsg.base|[atf_amc.MsgHeader](/txt/exe/atf_amc/internals.md#atf_amc-msgheader)|[Base](#atf_amc-msgheader-fields)|||
|atf_amc.Varlen2vMsg.v1|[atf_amc.VarlenK](/txt/exe/atf_amc/internals.md#atf_amc-varlenk)|[Varlen](/txt/exe/amc/reftypes.md#varlen)|||
|atf_amc.Varlen2vMsg.v2|[atf_amc.VarlenK](/txt/exe/atf_amc/internals.md#atf_amc-varlenk)|[Varlen](/txt/exe/amc/reftypes.md#varlen)|||
|atf_amc.Varlen2vMsg.v3|[atf_amc.VarlenK](/txt/exe/atf_amc/internals.md#atf_amc-varlenk)|[Varlen](/txt/exe/amc/reftypes.md#varlen)|||
#### atf_amc.MsgHeader Fields
<a href="#atf_amc-msgheader-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.MsgHeader.type|[atf_amc.MsgType](/txt/exe/atf_amc/internals.md#atf_amc-msgtype)|[Val](/txt/exe/amc/reftypes.md#val)||Message type|
|atf_amc.MsgHeader.length|[atf_amc.MsgLength](/txt/exe/atf_amc/internals.md#atf_amc-msglength)|[Val](/txt/exe/amc/reftypes.md#val)||Total message length, including this header|

#### Struct Varlen2vMsg
<a href="#struct-varlen2vmsg"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
#pragma pack(push,1)
struct Varlen2vMsg { // atf_amc.Varlen2vMsg
    atf_amc::MsgType     type;     //   0x1003  Message type
    atf_amc::MsgLength   length;   //   ssizeof(parent) + (0)  Total message length, including this header
    atf_amc::MsgLength   v1_end;   // end of v1 field
    // var-length field atf_amc.Varlen2vMsg.v1 starts here. access it with v1_Addr
    atf_amc::MsgLength   v2_end;   // end of v2 field
    // var-length field atf_amc.Varlen2vMsg.v2 starts here. access it with v2_Addr
    // var-length field atf_amc.Varlen2vMsg.v3 starts here. access it with v3_Addr
    // func:atf_amc.Varlen2vMsg..Ctor
    inline               Varlen2vMsg() __attribute__((nothrow));
};
```

#### atf_amc.VarlenAlloc - An optional instance of atf_unit.A
<a href="#atf_amc-varlenalloc"></a>

#### atf_amc.VarlenAlloc Fields
<a href="#atf_amc-varlenalloc-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.VarlenAlloc.length|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.VarlenAlloc.elem|i32|[Varlen](/txt/exe/amc/reftypes.md#varlen)|||

#### Struct VarlenAlloc
<a href="#struct-varlenalloc"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
#pragma pack(push,1)
struct VarlenAlloc { // atf_amc.VarlenAlloc: An optional instance of atf_unit.A
    u32   length;   //   0
    // var-length field atf_amc.VarlenAlloc.elem starts here. access it with elem_Addr
private:
    // func:atf_amc.VarlenAlloc..Ctor
    inline               VarlenAlloc() __attribute__((nothrow));
    friend atf_amc::VarlenAlloc& varlenalloc_Alloc(i32 n_varfld) __attribute__((__warn_unused_result__, nothrow));
    friend inline atf_amc::VarlenAlloc* varlenalloc_AllocMaybe(i32 n_varfld) __attribute__((__warn_unused_result__, nothrow));
    friend atf_amc::VarlenAlloc& varlenalloc_AllocExtra(void *extra, i32 nbyte_extra) __attribute__((__warn_unused_result__, nothrow));
    friend atf_amc::VarlenAlloc* varlenalloc_AllocExtraMaybe(void *extra, i32 nbyte_extra) __attribute__((__warn_unused_result__, nothrow));
    friend inline atf_amc::VarlenAlloc* varlenalloc_AllocVarlenMaybe(algo::aryptr<i32> elem) __attribute__((__warn_unused_result__, nothrow));
    friend inline atf_amc::VarlenAlloc& varlenalloc_AllocVarlen(algo::aryptr<i32> elem) __attribute__((__warn_unused_result__));
    friend void                 varlenalloc_Delete(atf_amc::VarlenAlloc &row) __attribute__((nothrow));
};
```

#### atf_amc.VarlenExtern - A varlen array at the end of a struct, but lenfld is external
<a href="#atf_amc-varlenextern"></a>

#### atf_amc.VarlenExtern Fields
<a href="#atf_amc-varlenextern-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.VarlenExtern.length|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.VarlenExtern.n_elems|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.VarlenExtern.varlen|u32|[Varlen](/txt/exe/amc/reftypes.md#varlen)|||

#### Struct VarlenExtern
<a href="#struct-varlenextern"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
#pragma pack(push,1)
struct VarlenExtern { // atf_amc.VarlenExtern: A varlen array at the end of a struct, but lenfld is external
    u32   n_elems;   //   0
    // var-length field atf_amc.VarlenExtern.varlen starts here. access it with varlen_Addr
private:
    // func:atf_amc.VarlenExtern..Ctor
    inline               VarlenExtern() __attribute__((nothrow));
    friend atf_amc::VarlenExtern& varlen_extern_Alloc(i32 n_varfld) __attribute__((__warn_unused_result__, nothrow));
    friend inline atf_amc::VarlenExtern* varlen_extern_AllocMaybe(i32 n_varfld) __attribute__((__warn_unused_result__, nothrow));
    friend atf_amc::VarlenExtern& varlen_extern_AllocExtra(void *extra, i32 nbyte_extra) __attribute__((__warn_unused_result__, nothrow));
    friend atf_amc::VarlenExtern* varlen_extern_AllocExtraMaybe(void *extra, i32 nbyte_extra) __attribute__((__warn_unused_result__, nothrow));
    friend inline atf_amc::VarlenExtern* varlen_extern_AllocVarlenMaybe(algo::aryptr<u32> varlen) __attribute__((__warn_unused_result__, nothrow));
    friend inline atf_amc::VarlenExtern& varlen_extern_AllocVarlen(algo::aryptr<u32> varlen) __attribute__((__warn_unused_result__));
    friend void                 varlen_extern_Delete(atf_amc::VarlenExtern &row) __attribute__((nothrow));
};
```

#### atf_amc.VarlenH - 
<a href="#atf_amc-varlenh"></a>

#### atf_amc.VarlenH Fields
<a href="#atf_amc-varlenh-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.VarlenH.length|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_amc.VarlenH.typeh|[atf_amc.TypeH](/txt/exe/atf_amc/internals.md#atf_amc-typeh)|[Varlen](/txt/exe/amc/reftypes.md#varlen)|||

#### Struct VarlenH
<a href="#struct-varlenh"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
struct VarlenH { // atf_amc.VarlenH
    u32   length;   //   0
    // var-length field atf_amc.VarlenH.typeh starts here. access it with typeh_Addr
    // func:atf_amc.VarlenH..Ctor
    inline               VarlenH() __attribute__((nothrow));
};
```

#### atf_amc.VarlenMsg - 
<a href="#atf_amc-varlenmsg"></a>

#### atf_amc.VarlenMsg Fields
<a href="#atf_amc-varlenmsg-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.VarlenMsg.base|[atf_amc.MsgHeader](/txt/exe/atf_amc/internals.md#atf_amc-msgheader)|[Base](#atf_amc-msgheader-fields)|||
|atf_amc.VarlenMsg.k|[atf_amc.VarlenK](/txt/exe/atf_amc/internals.md#atf_amc-varlenk)|[Varlen](/txt/exe/amc/reftypes.md#varlen)|||
#### atf_amc.MsgHeader Fields
<a href="#atf_amc-msgheader-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_amc.MsgHeader.type|[atf_amc.MsgType](/txt/exe/atf_amc/internals.md#atf_amc-msgtype)|[Val](/txt/exe/amc/reftypes.md#val)||Message type|
|atf_amc.MsgHeader.length|[atf_amc.MsgLength](/txt/exe/atf_amc/internals.md#atf_amc-msglength)|[Val](/txt/exe/amc/reftypes.md#val)||Total message length, including this header|

#### Struct VarlenMsg
<a href="#struct-varlenmsg"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_amc_gen.h](/include/gen/atf_amc_gen.h)
```
#pragma pack(push,1)
struct VarlenMsg { // atf_amc.VarlenMsg
    atf_amc::MsgType     type;     //   0x1000  Message type
    atf_amc::MsgLength   length;   //   ssizeof(parent) + (0)  Total message length, including this header
    // var-length field atf_amc.VarlenMsg.k starts here. access it with k_Addr
    // func:atf_amc.VarlenMsg..Ctor
    inline               VarlenMsg() __attribute__((nothrow));
};
```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Imdb -->


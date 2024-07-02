## acr - Internals


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Toc -->
* [Description](#description)
* [Sources](#sources)
* [Dependencies](#dependencies)
* [In Memory DB](#in-memory-db)
* [Tests](#tests)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Toc -->

### Description
<a href="#description"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Description -->
for usage, see [acr - Algo Cross-Reference - ssimfile database & update tool](/txt/exe/acr/README.md)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Description -->

### Sources
<a href="#sources"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Sources -->
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/acr/check.cpp](/cpp/acr/check.cpp)|Check constraints & referential integrity|
|[cpp/acr/createrec.cpp](/cpp/acr/createrec.cpp)|Create record|
|[cpp/acr/err.cpp](/cpp/acr/err.cpp)|Show errors / suggestions|
|[cpp/acr/eval.cpp](/cpp/acr/eval.cpp)|Evaluate attributes|
|[cpp/acr/git.cpp](/cpp/acr/git.cpp)|Git triggers|
|[cpp/acr/load.cpp](/cpp/acr/load.cpp)|Load files|
|[cpp/acr/main.cpp](/cpp/acr/main.cpp)|Main file|
|[cpp/acr/print.cpp](/cpp/acr/print.cpp)|Code for output|
|[cpp/acr/query.cpp](/cpp/acr/query.cpp)|Run query|
|[cpp/acr/select.cpp](/cpp/acr/select.cpp)|Selection of records|
|[cpp/acr/verb.cpp](/cpp/acr/verb.cpp)|Command-line verbs|
|[cpp/acr/write.cpp](/cpp/acr/write.cpp)|Write files|
|[cpp/gen/acr_gen.cpp](/cpp/gen/acr_gen.cpp)||
|[include/acr.h](/include/acr.h)|Header file|
|[include/gen/acr_gen.h](/include/gen/acr_gen.h)||
|[include/gen/acr_gen.inl.h](/include/gen/acr_gen.inl.h)||

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
`acr` generated code creates the tables below.
All allocations are done through global `acr::_db` [acr.FDb](#acr-fdb) structure
|Ctype|Ssimfile|Create|Access|
|---|---|---|---|
|[acr.AttrRegx](#acr-attrregx)||FQuery.where (Tary)|
|[acr.CtypeTopoKey](#acr-ctypetopokey)||
|[acr.Err](#acr-err)||
||||FErr.base (Base)|
|[acr.FAnonfld](#acr-fanonfld)|[dmmeta.anonfld](/txt/ssimdb/dmmeta/anonfld.md)|FDb.anonfld (Lary)|anonfld (Lary, by rowid)|
|[acr.FBltin](#acr-fbltin)|[amcdb.bltin](/txt/ssimdb/amcdb/bltin.md)|FDb.bltin (Lary)|bltin (Lary, by rowid)|
||||FCtype.c_bltin (Ptr)|
|[acr.FCdflt](#acr-fcdflt)|[dmmeta.cdflt](/txt/ssimdb/dmmeta/cdflt.md)|FDb.cdflt (Lary)|cdflt (Lary, by rowid)|
||||FCtype.c_cdflt (Ptr)|
|[acr.FCheck](#acr-fcheck)||FDb.check (Cppstack)|
|[acr.FCppfunc](#acr-fcppfunc)|[dmmeta.cppfunc](/txt/ssimdb/dmmeta/cppfunc.md)|FDb.cppfunc (Lary)|cppfunc (Lary, by rowid)|
|[acr.FCtype](#acr-fctype)|[dmmeta.ctype](/txt/ssimdb/dmmeta/ctype.md)|FDb.ctype (Lary)|ctype (Lary, by rowid)|ind_ctype (Thash, hash field ctype)|zd_sel_ctype (Llist)|bh_ctype_topo (Bheap, sort field topokey)|c_field_ctype (Ptr)|c_ssimfile_ctype (Ptr)|c_ctype_front (Ptrary)|
||||Err.ctype (Ptr)|
||||FCtype.c_child (Ptrary)|
||||FField.p_ctype (Upptr)|
||||FField.p_arg (Upptr)|
||||FQuery.c_ctype (Ptrary)|
||||FRec.p_ctype (Upptr)|
||||FSsimfile.p_ctype (Upptr)|
||||FSsimreq.p_ctype (Upptr)|
||||FErr.ctype (Ptr)|
|[acr.FDb](#acr-fdb)||FDb._db (Global)|
|[acr.FErr](#acr-ferr)||FDb.err (Tpool)|zd_all_err (Llist)|
|[acr.FEvalattr](#acr-fevalattr)||
|[acr.FField](#acr-ffield)|[dmmeta.field](/txt/ssimdb/dmmeta/field.md)|FDb.field (Lary)|field (Lary, by rowid)|ind_field (Thash, hash field field)|
||||Err.fld (Ptr)|
||||FCtype.c_field (Ptrary)|
||||FCtype.zd_arg (Llist)|
||||FEvalattr.field (Ptr)|
||||FQuery.c_field (Ptrary)|
||||FSsimreq.p_parent_field (Upptr)|
||||FErr.fld (Ptr)|
|[acr.FFile](#acr-ffile)||FDb.file (Lary)|file (Lary, by rowid)|ind_file (Thash, hash field file)|
||||FRec.p_outfile (Upptr)|
||||FRec.p_infile (Upptr)|
||||FSsimfile.c_file (Ptr)|
|[acr.FFunique](#acr-ffunique)|[dmmeta.funique](/txt/ssimdb/dmmeta/funique.md)|FDb.funique (Lary)|funique (Lary, by rowid)|
|[acr.FPdep](#acr-fpdep)||FDb.pdep (Tpool)|zd_pdep (Llist)|
||||FPline.zd_child (Llist)|
|[acr.RecSortkey](#acr-recsortkey)||
|[acr.PlineKey](#acr-plinekey)||
|[acr.FPline](#acr-fpline)||FDb.pline (Tpool)|zd_pline (Llist)|bh_pline (Bheap, sort field key)|
||||FPdep.p_parent (Upptr)|
||||FPdep.p_child (Upptr)|
||||FPrint.c_pline (Ptrary)|
||||FRec.c_pline (Ptr)|
|[acr.FPrint](#acr-fprint)||FDb.print (Cppstack)|
||||FPrintAttr.p_print (Upptr)|
|[acr.FPrintAttr](#acr-fprintattr)||FDb.printattr (Malloc)|
||||FPrint.ind_printattr (Thash)|
|[acr.Queryop](#acr-queryop)||
|[acr.FQuery](#acr-fquery)||FDb.query (Tpool)|zs_query (Llist)|
|[acr.FRec](#acr-frec)||FDb.rec (Tpool)|zd_all_selrec (Llist)|c_ssimreq_rec (Ptrary)|
||||Err.rec (Ptr)|
||||FCheck.c_bad_rec (Ptrary)|
||||FCtype.zd_ctype_rec (Llist)|
||||FCtype.ind_ctype_rec (Thash)|
||||FCtype.zd_ctype_selrec (Llist)|
||||FFile.zd_frec (Llist)|
||||FPline.p_rec (Upptr)|
||||FQuery.c_rec (Ptrary)|
||||FWrite.c_cmtrec (Ptrary)|
||||FErr.rec (Ptr)|
|[acr.FSmallstr](#acr-fsmallstr)|[dmmeta.smallstr](/txt/ssimdb/dmmeta/smallstr.md)|FDb.smallstr (Lary)|smallstr (Lary, by rowid)|
|[acr.FSortkey](#acr-fsortkey)||FDb.sortkey (Lary)|sortkey (Lary, by rowid)|ind_sortkey (Thash, hash field sortkey)|
|[acr.FSsimfile](#acr-fssimfile)|[dmmeta.ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|FDb.ssimfile (Lary)|ssimfile (Lary, by rowid)|ind_ssimfile (Thash, hash field ssimfile)|
||||FCtype.c_ssimfile (Ptr)|
||||FSsimreq.p_child_ssimfile (Upptr)|
|[acr.FSsimreq](#acr-fssimreq)|[dmmeta.ssimreq](/txt/ssimdb/dmmeta/ssimreq.md)|FDb.ssimreq (Lary)|ssimreq (Lary, by rowid)|
||||FCtype.c_ssimreq (Ptrary)|
|[acr.FSsimsort](#acr-fssimsort)|[dmmeta.ssimsort](/txt/ssimdb/dmmeta/ssimsort.md)|FDb.ssimsort (Lary)|ssimsort (Lary, by rowid)|ind_ssimsort (Thash, hash field ssimfile)|
||||FSsimfile.c_ssimsort (Ptr)|
|[acr.FSubstr](#acr-fsubstr)|[dmmeta.substr](/txt/ssimdb/dmmeta/substr.md)|FDb.substr (Lary)|substr (Lary, by rowid)|
||||FField.c_substr (Ptr)|
|[acr.FTempkey](#acr-ftempkey)||FDb.tempkey (Lary)|tempkey (Lary, by rowid)|ind_tempkey (Thash, hash field tempkey)|
|[acr.FUniqueattr](#acr-funiqueattr)||FDb.uniqueattr (Tpool)|ind_uniqueattr (Thash, hash field uniqueattr)|
|[acr.FWrite](#acr-fwrite)||FDb.write (Cppstack)|
|[acr.ReadMode](#acr-readmode)||

#### acr.AttrRegx - Filters that must match input key/value pairs
<a href="#acr-attrregx"></a>

#### acr.AttrRegx Fields
<a href="#acr-attrregx-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr.AttrRegx.name|[algo.cstring](/txt/protocol/algo/cstring.md)|[Regx](/txt/exe/amc/reftypes.md#regx)|||
|acr.AttrRegx.value|[algo.cstring](/txt/protocol/algo/cstring.md)|[Regx](/txt/exe/amc/reftypes.md#regx)|||

#### Struct AttrRegx
<a href="#struct-attrregx"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_gen.h](/include/gen/acr_gen.h)
```
struct AttrRegx { // acr.AttrRegx: Filters that must match input key/value pairs
    algo_lib::Regx   name;    // Acr Regx
    algo_lib::Regx   value;   // Acr Regx
    // func:acr.AttrRegx..Ctor
    inline               AttrRegx() __attribute__((nothrow));
};
```

#### acr.CtypeTopoKey - Key for sorting print-line records
<a href="#acr-ctypetopokey"></a>

#### acr.CtypeTopoKey Fields
<a href="#acr-ctypetopokey-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr.CtypeTopoKey.alldep|i32|[Val](/txt/exe/amc/reftypes.md#val)||# Unresolved references|
|acr.CtypeTopoKey.rowid|i32|[Val](/txt/exe/amc/reftypes.md#val)||Rowid of original record|

#### Struct CtypeTopoKey
<a href="#struct-ctypetopokey"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_gen.h](/include/gen/acr_gen.h)
```
struct CtypeTopoKey { // acr.CtypeTopoKey: Key for sorting print-line records
    i32   alldep;   //   0  # Unresolved references
    i32   rowid;    //   0  Rowid of original record
    // func:acr.CtypeTopoKey..EqOp
    inline bool          operator ==(const acr::CtypeTopoKey &rhs) const __attribute__((nothrow));
    // func:acr.CtypeTopoKey..NeOp
    inline bool          operator !=(const acr::CtypeTopoKey &rhs) const __attribute__((nothrow));
    // func:acr.CtypeTopoKey..LtOp
    inline bool          operator <(const acr::CtypeTopoKey &rhs) const __attribute__((nothrow));
    // func:acr.CtypeTopoKey..GtOp
    inline bool          operator >(const acr::CtypeTopoKey &rhs) const __attribute__((nothrow));
    // func:acr.CtypeTopoKey..LeOp
    inline bool          operator <=(const acr::CtypeTopoKey &rhs) const __attribute__((nothrow));
    // func:acr.CtypeTopoKey..GeOp
    inline bool          operator >=(const acr::CtypeTopoKey &rhs) const __attribute__((nothrow));
    // func:acr.CtypeTopoKey..Ctor
    inline               CtypeTopoKey() __attribute__((nothrow));
};
```

#### acr.Err - 
<a href="#acr-err"></a>

#### acr.Err Fields
<a href="#acr-err-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr.Err.ctype|[acr.FCtype](/txt/exe/acr/internals.md#acr-fctype)|[Ptr](/txt/exe/amc/reftypes.md#ptr)||Parent ctype|
|acr.Err.id|u32|[Val](/txt/exe/amc/reftypes.md#val)|0|ID|
|acr.Err.text|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Error text|
|acr.Err.rec|[acr.FRec](/txt/exe/acr/internals.md#acr-frec)|[Ptr](/txt/exe/amc/reftypes.md#ptr)||Parent record|
|acr.Err.fld|[acr.FField](/txt/exe/acr/internals.md#acr-ffield)|[Ptr](/txt/exe/amc/reftypes.md#ptr)||Parent field|

#### Struct Err
<a href="#struct-err"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_gen.h](/include/gen/acr_gen.h)
```
struct Err { // acr.Err
    acr::FCtype*    ctype;   // Parent ctype. optional pointer
    u32             id;      //   0  ID
    algo::cstring   text;    // Error text
    acr::FRec*      rec;     // Parent record. optional pointer
    acr::FField*    fld;     // Parent field. optional pointer
    // func:acr.Err..Ctor
    inline               Err() __attribute__((nothrow));
};
```

#### acr.FAnonfld - Omit field name where possible (command line, enums, constants)
<a href="#acr-fanonfld"></a>

#### acr.FAnonfld Fields
<a href="#acr-fanonfld-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr.FAnonfld.base|[dmmeta.Anonfld](/txt/ssimdb/dmmeta/anonfld.md)|[Base](/txt/ssimdb/dmmeta/anonfld.md)|||

#### Struct FAnonfld
<a href="#struct-fanonfld"></a>
*Note:* field ``acr.FAnonfld.base`` has reftype ``base`` so the fields of [dmmeta.Anonfld](/txt/ssimdb/dmmeta/anonfld.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_gen.h](/include/gen/acr_gen.h)
```
struct FAnonfld { // acr.FAnonfld
    algo::Smallstr100   field;   //
private:
    // func:acr.FAnonfld..Ctor
    inline               FAnonfld() __attribute__((nothrow));
    friend acr::FAnonfld&       anonfld_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr::FAnonfld*       anonfld_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 anonfld_RemoveLast() __attribute__((nothrow));
};
```

#### acr.FBltin - Specify properties of a C built-in type
<a href="#acr-fbltin"></a>

#### acr.FBltin Fields
<a href="#acr-fbltin-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr.FBltin.base|[amcdb.Bltin](/txt/ssimdb/amcdb/bltin.md)|[Base](/txt/ssimdb/amcdb/bltin.md)|||

#### Struct FBltin
<a href="#struct-fbltin"></a>
*Note:* field ``acr.FBltin.base`` has reftype ``base`` so the fields of [amcdb.Bltin](/txt/ssimdb/amcdb/bltin.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_gen.h](/include/gen/acr_gen.h)
```
struct FBltin { // acr.FBltin
    algo::Smallstr100   ctype;      //
    bool                likeu64;    //   false
    bool                bigendok;   //   false
    bool                issigned;   //   false
    // func:acr.FBltin..AssignOp
    inline acr::FBltin&  operator =(const acr::FBltin &rhs) = delete;
    // func:acr.FBltin..CopyCtor
    inline               FBltin(const acr::FBltin &rhs) = delete;
private:
    // func:acr.FBltin..Ctor
    inline               FBltin() __attribute__((nothrow));
    // func:acr.FBltin..Dtor
    inline               ~FBltin() __attribute__((nothrow));
    friend acr::FBltin&         bltin_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr::FBltin*         bltin_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 bltin_RemoveLast() __attribute__((nothrow));
};
```

#### acr.FCdflt - Specify default value for single-value types that lack fields
<a href="#acr-fcdflt"></a>

#### acr.FCdflt Fields
<a href="#acr-fcdflt-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr.FCdflt.base|[dmmeta.Cdflt](/txt/ssimdb/dmmeta/cdflt.md)|[Base](/txt/ssimdb/dmmeta/cdflt.md)|||

#### Struct FCdflt
<a href="#struct-fcdflt"></a>
*Note:* field ``acr.FCdflt.base`` has reftype ``base`` so the fields of [dmmeta.Cdflt](/txt/ssimdb/dmmeta/cdflt.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_gen.h](/include/gen/acr_gen.h)
```
struct FCdflt { // acr.FCdflt
    algo::Smallstr100   ctype;      //
    algo::CppExpr       dflt;       //
    algo::CppExpr       cppdflt;    //
    algo::Smallstr50    ssimdflt;   //
    algo::Smallstr50    jsdflt;     //
    // func:acr.FCdflt..AssignOp
    inline acr::FCdflt&  operator =(const acr::FCdflt &rhs) = delete;
    // func:acr.FCdflt..CopyCtor
    inline               FCdflt(const acr::FCdflt &rhs) = delete;
private:
    // func:acr.FCdflt..Ctor
    inline               FCdflt() __attribute__((nothrow));
    // func:acr.FCdflt..Dtor
    inline               ~FCdflt() __attribute__((nothrow));
    friend acr::FCdflt&         cdflt_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr::FCdflt*         cdflt_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 cdflt_RemoveLast() __attribute__((nothrow));
};
```

#### acr.FCheck - Function to check for consistency
<a href="#acr-fcheck"></a>

#### acr.FCheck Fields
<a href="#acr-fcheck-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr.FCheck.c_bad_rec|[acr.FRec](/txt/exe/acr/internals.md#acr-frec)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|acr.FCheck.n_record|u32|[Val](/txt/exe/amc/reftypes.md#val)|||
|acr.FCheck.n_err|i32|[Val](/txt/exe/amc/reftypes.md#val)|||
|acr.FCheck.ary_name|[algo.cstring](/txt/protocol/algo/cstring.md)|[Tary](/txt/exe/amc/reftypes.md#tary)|||

#### Struct FCheck
<a href="#struct-fcheck"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_gen.h](/include/gen/acr_gen.h)
```
struct FCheck { // acr.FCheck: Function to check for consistency
    acr::FRec**      c_bad_rec_elems;   // array of pointers
    u32              c_bad_rec_n;       // array of pointers
    u32              c_bad_rec_max;     // capacity of allocated array
    u32              n_record;          //   0
    i32              n_err;             //   0
    algo::cstring*   ary_name_elems;    // pointer to elements
    u32              ary_name_n;        // number of elements in array
    u32              ary_name_max;      // max. capacity of array before realloc
    // reftype Ptrary of acr.FCheck.c_bad_rec prohibits copy
    // func:acr.FCheck..AssignOp
    acr::FCheck&         operator =(const acr::FCheck &rhs) = delete;
    // func:acr.FCheck..Ctor
    inline               FCheck() __attribute__((nothrow));
    // func:acr.FCheck..Dtor
    inline               ~FCheck() __attribute__((nothrow));
    // reftype Ptrary of acr.FCheck.c_bad_rec prohibits copy
    // func:acr.FCheck..CopyCtor
    FCheck(const acr::FCheck &rhs) = delete;
};
```

#### acr.FCppfunc - Value of field provided by this expression
<a href="#acr-fcppfunc"></a>

#### acr.FCppfunc Fields
<a href="#acr-fcppfunc-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr.FCppfunc.base|[dmmeta.Cppfunc](/txt/ssimdb/dmmeta/cppfunc.md)|[Base](/txt/ssimdb/dmmeta/cppfunc.md)|||

#### Struct FCppfunc
<a href="#struct-fcppfunc"></a>
*Note:* field ``acr.FCppfunc.base`` has reftype ``base`` so the fields of [dmmeta.Cppfunc](/txt/ssimdb/dmmeta/cppfunc.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_gen.h](/include/gen/acr_gen.h)
```
struct FCppfunc { // acr.FCppfunc
    algo::Smallstr100   field;   //
    algo::CppExpr       expr;    //
    bool                print;   //   false
    bool                set;     //   false
private:
    // func:acr.FCppfunc..Ctor
    inline               FCppfunc() __attribute__((nothrow));
    friend acr::FCppfunc&       cppfunc_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr::FCppfunc*       cppfunc_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 cppfunc_RemoveAll() __attribute__((nothrow));
    friend void                 cppfunc_RemoveLast() __attribute__((nothrow));
};
```

#### acr.FCtype - Struct
<a href="#acr-fctype"></a>

#### acr.FCtype Fields
<a href="#acr-fctype-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr.FCtype.base|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Base](/txt/ssimdb/dmmeta/ctype.md)|||
|acr.FCtype.c_field|[acr.FField](/txt/exe/acr/internals.md#acr-ffield)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|acr.FCtype.c_cdflt|[acr.FCdflt](/txt/exe/acr/internals.md#acr-fcdflt)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|acr.FCtype.c_ssimfile|[acr.FSsimfile](/txt/exe/acr/internals.md#acr-fssimfile)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|acr.FCtype.zd_ctype_rec|[acr.FRec](/txt/exe/acr/internals.md#acr-frec)|[Llist](/txt/exe/amc/reftypes.md#llist)||List of all records with this ctype|
|acr.FCtype.ind_ctype_rec|[acr.FRec](/txt/exe/acr/internals.md#acr-frec)|[Thash](/txt/exe/amc/reftypes.md#thash)||Index of records by primary key|
|acr.FCtype.zd_ctype_selrec|[acr.FRec](/txt/exe/acr/internals.md#acr-frec)|[Llist](/txt/exe/amc/reftypes.md#llist)||List of selected records|
|acr.FCtype.n_insert|i32|[Val](/txt/exe/amc/reftypes.md#val)|0|Number of tuples inserted|
|acr.FCtype.rank|i32|[Val](/txt/exe/amc/reftypes.md#val)|false|Topological sort rank|
|acr.FCtype.c_child|[acr.FCtype](/txt/exe/acr/internals.md#acr-fctype)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)||all tables that reference this table|
|acr.FCtype.show_rowid|bool|[Val](/txt/exe/amc/reftypes.md#val)||True if records of this type require printing rowid (not fully ordered)|
|acr.FCtype.next_rowid|u64|[Val](/txt/exe/amc/reftypes.md#val)|0|Rowid of next created record for this type|
|acr.FCtype.numeric|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|acr.FCtype.c_bltin|[acr.FBltin](/txt/exe/acr/internals.md#acr-fbltin)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|acr.FCtype.zd_arg|[acr.FField](/txt/exe/acr/internals.md#acr-ffield)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|acr.FCtype.rowid|i32|[Val](/txt/exe/amc/reftypes.md#val)||Row id of ctype from its original ctype.ssim table|
|acr.FCtype.topokey|[acr.CtypeTopoKey](/txt/exe/acr/internals.md#acr-ctypetopokey)|[Val](/txt/exe/amc/reftypes.md#val)|||
|acr.FCtype.cmt_printed|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|acr.FCtype.mark_sel|bool|[Val](/txt/exe/amc/reftypes.md#val)||Ctype tree scheduled to be selected (with -meta)|
|acr.FCtype.c_ssimreq|[acr.FSsimreq](/txt/exe/acr/internals.md#acr-fssimreq)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||

#### Struct FCtype
<a href="#struct-fctype"></a>
*Note:* field ``acr.FCtype.base`` has reftype ``base`` so the fields of [dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_gen.h](/include/gen/acr_gen.h)
```
struct FCtype { // acr.FCtype
    algo::Smallstr100   ctype;                         // Identifier. must be ns.typename
    algo::Comment       comment;                       //
    acr::FField**       c_field_elems;                 // array of pointers
    u32                 c_field_n;                     // array of pointers
    u32                 c_field_max;                   // capacity of allocated array
    acr::FCdflt*        c_cdflt;                       // optional pointer
    acr::FSsimfile*     c_ssimfile;                    // optional pointer
    acr::FRec*          zd_ctype_rec_head;             // zero-terminated doubly linked list
    acr::FRec*          zd_ctype_rec_tail;             // pointer to last element
    acr::FRec**         ind_ctype_rec_buckets_elems;   // pointer to bucket array
    i32                 ind_ctype_rec_buckets_n;       // number of elements in bucket array
    i32                 ind_ctype_rec_n;               // number of elements in the hash table
    acr::FRec*          zd_ctype_selrec_head;          // zero-terminated doubly linked list
    acr::FRec*          zd_ctype_selrec_tail;          // pointer to last element
    i32                 n_insert;                      //   0  Number of tuples inserted
    i32                 rank;                          //   false  Topological sort rank
    acr::FCtype**       c_child_elems;                 // array of pointers
    u32                 c_child_n;                     // array of pointers
    u32                 c_child_max;                   // capacity of allocated array
    bool                show_rowid;                    //   false  True if records of this type require printing rowid (not fully ordered)
    u64                 next_rowid;                    //   0  Rowid of next created record for this type
    bool                numeric;                       //   false
    acr::FBltin*        c_bltin;                       // optional pointer
    acr::FField*        zd_arg_head;                   // zero-terminated doubly linked list
    i32                 zd_arg_n;                      // zero-terminated doubly linked list
    acr::FField*        zd_arg_tail;                   // pointer to last element
    i32                 rowid;                         //   0  Row id of ctype from its original ctype.ssim table
    acr::CtypeTopoKey   topokey;                       //
    bool                cmt_printed;                   //   false
    bool                mark_sel;                      //   false  Ctype tree scheduled to be selected (with -meta)
    acr::FSsimreq**     c_ssimreq_elems;               // array of pointers
    u32                 c_ssimreq_n;                   // array of pointers
    u32                 c_ssimreq_max;                 // capacity of allocated array
    bool                _db_c_ctype_front_in_ary;      //   false  membership flag
    acr::FCtype*        ind_ctype_next;                // hash next
    acr::FCtype*        zd_sel_ctype_next;             // zslist link; -1 means not-in-list
    acr::FCtype*        zd_sel_ctype_prev;             // previous element
    i32                 bh_ctype_topo_idx;             // index in heap; -1 means not-in-heap
    // reftype Ptrary of acr.FCtype.c_field prohibits copy
    // x-reference on acr.FCtype.c_cdflt prevents copy
    // x-reference on acr.FCtype.c_ssimfile prevents copy
    // reftype Llist of acr.FCtype.zd_ctype_rec prohibits copy
    // reftype Thash of acr.FCtype.ind_ctype_rec prohibits copy
    // reftype Llist of acr.FCtype.zd_ctype_selrec prohibits copy
    // reftype Ptrary of acr.FCtype.c_child prohibits copy
    // x-reference on acr.FCtype.c_bltin prevents copy
    // reftype Llist of acr.FCtype.zd_arg prohibits copy
    // reftype Ptrary of acr.FCtype.c_ssimreq prohibits copy
    // func:acr.FCtype..AssignOp
    acr::FCtype&         operator =(const acr::FCtype &rhs) = delete;
    // reftype Ptrary of acr.FCtype.c_field prohibits copy
    // x-reference on acr.FCtype.c_cdflt prevents copy
    // x-reference on acr.FCtype.c_ssimfile prevents copy
    // reftype Llist of acr.FCtype.zd_ctype_rec prohibits copy
    // reftype Thash of acr.FCtype.ind_ctype_rec prohibits copy
    // reftype Llist of acr.FCtype.zd_ctype_selrec prohibits copy
    // reftype Ptrary of acr.FCtype.c_child prohibits copy
    // x-reference on acr.FCtype.c_bltin prevents copy
    // reftype Llist of acr.FCtype.zd_arg prohibits copy
    // reftype Ptrary of acr.FCtype.c_ssimreq prohibits copy
    // func:acr.FCtype..CopyCtor
    FCtype(const acr::FCtype &rhs) = delete;
private:
    // func:acr.FCtype..Ctor
    inline               FCtype() __attribute__((nothrow));
    // func:acr.FCtype..Dtor
    inline               ~FCtype() __attribute__((nothrow));
    friend acr::FCtype&         ctype_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr::FCtype*         ctype_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 ctype_RemoveLast() __attribute__((nothrow));
};
```

#### acr.FDb - In-memory database for acr
<a href="#acr-fdb"></a>

#### acr.FDb Fields
<a href="#acr-fdb-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr.FDb.pline|[acr.FPline](/txt/exe/acr/internals.md#acr-fpline)|[Tpool](/txt/exe/amc/reftypes.md#tpool)|||
|acr.FDb.pdep|[acr.FPdep](/txt/exe/acr/internals.md#acr-fpdep)|[Tpool](/txt/exe/amc/reftypes.md#tpool)|||
|acr.FDb.zd_pline|[acr.FPline](/txt/exe/acr/internals.md#acr-fpline)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|acr.FDb.zd_pdep|[acr.FPdep](/txt/exe/acr/internals.md#acr-fpdep)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|acr.FDb.cmdline|[command.acr](/txt/protocol/command/README.md#command-acr)|[Val](/txt/exe/amc/reftypes.md#val)||command line|
|acr.FDb.ctype|[acr.FCtype](/txt/exe/acr/internals.md#acr-fctype)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr.FDb.err|[acr.FErr](/txt/exe/acr/internals.md#acr-ferr)|[Tpool](/txt/exe/amc/reftypes.md#tpool)|||
|acr.FDb.anonfld|[acr.FAnonfld](/txt/exe/acr/internals.md#acr-fanonfld)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr.FDb.cdflt|[acr.FCdflt](/txt/exe/acr/internals.md#acr-fcdflt)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr.FDb.rec|[acr.FRec](/txt/exe/acr/internals.md#acr-frec)|[Tpool](/txt/exe/amc/reftypes.md#tpool)|||
|acr.FDb.uniqueattr|[acr.FUniqueattr](/txt/exe/acr/internals.md#acr-funiqueattr)|[Tpool](/txt/exe/amc/reftypes.md#tpool)|||
|acr.FDb.query|[acr.FQuery](/txt/exe/acr/internals.md#acr-fquery)|[Tpool](/txt/exe/amc/reftypes.md#tpool)|||
|acr.FDb.field|[acr.FField](/txt/exe/acr/internals.md#acr-ffield)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr.FDb.file|[acr.FFile](/txt/exe/acr/internals.md#acr-ffile)|[Lary](/txt/exe/amc/reftypes.md#lary)||List of all loaded files|
|acr.FDb._db|[acr.FDb](/txt/exe/acr/internals.md#acr-fdb)|[Global](/txt/exe/amc/reftypes.md#global)|||
|acr.FDb.err_seq|u32|[Val](/txt/exe/amc/reftypes.md#val)|0||
|acr.FDb.check_failed|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|acr.FDb.ind_ctype|[acr.FCtype](/txt/exe/acr/internals.md#acr-fctype)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|acr.FDb.ind_field|[acr.FField](/txt/exe/acr/internals.md#acr-ffield)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|acr.FDb.ind_file|[acr.FFile](/txt/exe/acr/internals.md#acr-ffile)|[Thash](/txt/exe/amc/reftypes.md#thash)||Index of files by primary key|
|acr.FDb.zd_all_selrec|[acr.FRec](/txt/exe/acr/internals.md#acr-frec)|[Llist](/txt/exe/amc/reftypes.md#llist)||List of selected records|
|acr.FDb.zd_all_err|[acr.FErr](/txt/exe/acr/internals.md#acr-ferr)|[Llist](/txt/exe/amc/reftypes.md#llist)||List of errors|
|acr.FDb.zd_sel_ctype|[acr.FCtype](/txt/exe/acr/internals.md#acr-fctype)|[Llist](/txt/exe/amc/reftypes.md#llist)||List of selected ctypess|
|acr.FDb.bh_pline|[acr.FPline](/txt/exe/acr/internals.md#acr-fpline)|[Bheap](/txt/exe/amc/reftypes.md#bheap)||List of records to print|
|acr.FDb.substr|[acr.FSubstr](/txt/exe/acr/internals.md#acr-fsubstr)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr.FDb.ssimfile|[acr.FSsimfile](/txt/exe/acr/internals.md#acr-fssimfile)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr.FDb.tempkey|[acr.FTempkey](/txt/exe/acr/internals.md#acr-ftempkey)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr.FDb.ind_tempkey|[acr.FTempkey](/txt/exe/acr/internals.md#acr-ftempkey)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|acr.FDb.ind_ssimfile|[acr.FSsimfile](/txt/exe/acr/internals.md#acr-fssimfile)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|acr.FDb.print|[acr.FPrint](/txt/exe/acr/internals.md#acr-fprint)|[Cppstack](/txt/exe/amc/reftypes.md#cppstack)||Provides default name for variables of this type|
|acr.FDb.printattr|[acr.FPrintAttr](/txt/exe/acr/internals.md#acr-fprintattr)|[Malloc](/txt/exe/amc/reftypes.md#malloc)||Allocate via malloc|
|acr.FDb.zs_query|[acr.FQuery](/txt/exe/acr/internals.md#acr-fquery)|[Llist](/txt/exe/amc/reftypes.md#llist)||List of queries remaining to be performed|
|acr.FDb.check|[acr.FCheck](/txt/exe/acr/internals.md#acr-fcheck)|[Cppstack](/txt/exe/amc/reftypes.md#cppstack)||Provides default name for variables of this type|
|acr.FDb.ssimsort|[acr.FSsimsort](/txt/exe/acr/internals.md#acr-fssimsort)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr.FDb.ind_ssimsort|[acr.FSsimsort](/txt/exe/acr/internals.md#acr-fssimsort)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|acr.FDb.write|[acr.FWrite](/txt/exe/acr/internals.md#acr-fwrite)|[Cppstack](/txt/exe/amc/reftypes.md#cppstack)||Provides default name for variables of this type|
|acr.FDb.n_file_written|u32|[Val](/txt/exe/amc/reftypes.md#val)|0|Number of files written|
|acr.FDb.report|[report.acr](/txt/protocol/report/README.md#report-acr)|[Val](/txt/exe/amc/reftypes.md#val)||Final report|
|acr.FDb.smallstr|[acr.FSmallstr](/txt/exe/acr/internals.md#acr-fsmallstr)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr.FDb.funique|[acr.FFunique](/txt/exe/acr/internals.md#acr-ffunique)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr.FDb.ind_uniqueattr|[acr.FUniqueattr](/txt/exe/acr/internals.md#acr-funiqueattr)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|acr.FDb.bltin|[acr.FBltin](/txt/exe/acr/internals.md#acr-fbltin)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr.FDb.ctype_rank_computed|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|acr.FDb.file_input|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|acr.FDb.bh_ctype_topo|[acr.FCtype](/txt/exe/acr/internals.md#acr-fctype)|[Bheap](/txt/exe/amc/reftypes.md#bheap)|||
|acr.FDb.cppfunc|[acr.FCppfunc](/txt/exe/acr/internals.md#acr-fcppfunc)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr.FDb.ssimreq|[acr.FSsimreq](/txt/exe/acr/internals.md#acr-fssimreq)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr.FDb.c_ssimreq_rec|[acr.FRec](/txt/exe/acr/internals.md#acr-frec)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|acr.FDb.c_field_ctype|[acr.FCtype](/txt/exe/acr/internals.md#acr-fctype)|[Ptr](/txt/exe/amc/reftypes.md#ptr)||ctype describing dmmeta.Field|
|acr.FDb.c_ssimfile_ctype|[acr.FCtype](/txt/exe/acr/internals.md#acr-fctype)|[Ptr](/txt/exe/amc/reftypes.md#ptr)||ctype describing dmmeta.Ssimfile|
|acr.FDb.c_ctype_front|[acr.FCtype](/txt/exe/acr/internals.md#acr-fctype)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)||Down front (for -ndown)|
|acr.FDb.sortkey|[acr.FSortkey](/txt/exe/acr/internals.md#acr-fsortkey)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr.FDb.ind_sortkey|[acr.FSortkey](/txt/exe/acr/internals.md#acr-fsortkey)|[Thash](/txt/exe/amc/reftypes.md#thash)|||

#### Struct FDb
<a href="#struct-fdb"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_gen.h](/include/gen/acr_gen.h)
```
struct FDb { // acr.FDb: In-memory database for acr
    u64                  pline_blocksize;                // # bytes per block
    acr::FPline*         pline_free;                     //
    u64                  pdep_blocksize;                 // # bytes per block
    acr::FPdep*          pdep_free;                      //
    acr::FPline*         zd_pline_head;                  // zero-terminated doubly linked list
    acr::FPline*         zd_pline_tail;                  // pointer to last element
    acr::FPdep*          zd_pdep_head;                   // zero-terminated doubly linked list
    acr::FPdep*          zd_pdep_tail;                   // pointer to last element
    command::acr         cmdline;                        // command line
    acr::FCtype*         ctype_lary[32];                 // level array
    i32                  ctype_n;                        // number of elements in array
    u64                  err_blocksize;                  // # bytes per block
    acr::FErr*           err_free;                       //
    acr::FAnonfld*       anonfld_lary[32];               // level array
    i32                  anonfld_n;                      // number of elements in array
    acr::FCdflt*         cdflt_lary[32];                 // level array
    i32                  cdflt_n;                        // number of elements in array
    u64                  rec_blocksize;                  // # bytes per block
    acr::FRec*           rec_free;                       //
    u64                  uniqueattr_blocksize;           // # bytes per block
    acr::FUniqueattr*    uniqueattr_free;                //
    u64                  query_blocksize;                // # bytes per block
    acr::FQuery*         query_free;                     //
    acr::FField*         field_lary[32];                 // level array
    i32                  field_n;                        // number of elements in array
    acr::FFile*          file_lary[32];                  // level array
    i32                  file_n;                         // number of elements in array
    u32                  err_seq;                        //   0
    bool                 check_failed;                   //   false
    acr::FCtype**        ind_ctype_buckets_elems;        // pointer to bucket array
    i32                  ind_ctype_buckets_n;            // number of elements in bucket array
    i32                  ind_ctype_n;                    // number of elements in the hash table
    acr::FField**        ind_field_buckets_elems;        // pointer to bucket array
    i32                  ind_field_buckets_n;            // number of elements in bucket array
    i32                  ind_field_n;                    // number of elements in the hash table
    acr::FFile**         ind_file_buckets_elems;         // pointer to bucket array
    i32                  ind_file_buckets_n;             // number of elements in bucket array
    i32                  ind_file_n;                     // number of elements in the hash table
    acr::FRec*           zd_all_selrec_head;             // zero-terminated doubly linked list
    i32                  zd_all_selrec_n;                // zero-terminated doubly linked list
    acr::FRec*           zd_all_selrec_tail;             // pointer to last element
    acr::FErr*           zd_all_err_head;                // zero-terminated doubly linked list
    i32                  zd_all_err_n;                   // zero-terminated doubly linked list
    acr::FErr*           zd_all_err_tail;                // pointer to last element
    acr::FCtype*         zd_sel_ctype_head;              // zero-terminated doubly linked list
    acr::FCtype*         zd_sel_ctype_tail;              // pointer to last element
    acr::FPline**        bh_pline_elems;                 // binary heap by key
    i32                  bh_pline_n;                     // number of elements in the heap
    i32                  bh_pline_max;                   // max elements in bh_pline_elems
    acr::FSubstr*        substr_lary[32];                // level array
    i32                  substr_n;                       // number of elements in array
    acr::FSsimfile*      ssimfile_lary[32];              // level array
    i32                  ssimfile_n;                     // number of elements in array
    acr::FTempkey*       tempkey_lary[32];               // level array
    i32                  tempkey_n;                      // number of elements in array
    acr::FTempkey**      ind_tempkey_buckets_elems;      // pointer to bucket array
    i32                  ind_tempkey_buckets_n;          // number of elements in bucket array
    i32                  ind_tempkey_n;                  // number of elements in the hash table
    acr::FSsimfile**     ind_ssimfile_buckets_elems;     // pointer to bucket array
    i32                  ind_ssimfile_buckets_n;         // number of elements in bucket array
    i32                  ind_ssimfile_n;                 // number of elements in the hash table
    acr::FQuery*         zs_query_head;                  // zero-terminated singly linked list
    acr::FQuery*         zs_query_tail;                  // pointer to last element
    acr::FSsimsort*      ssimsort_lary[32];              // level array
    i32                  ssimsort_n;                     // number of elements in array
    acr::FSsimsort**     ind_ssimsort_buckets_elems;     // pointer to bucket array
    i32                  ind_ssimsort_buckets_n;         // number of elements in bucket array
    i32                  ind_ssimsort_n;                 // number of elements in the hash table
    u32                  n_file_written;                 //   0  Number of files written
    report::acr          report;                         // Final report
    acr::FSmallstr*      smallstr_lary[32];              // level array
    i32                  smallstr_n;                     // number of elements in array
    acr::FFunique*       funique_lary[32];               // level array
    i32                  funique_n;                      // number of elements in array
    acr::FUniqueattr**   ind_uniqueattr_buckets_elems;   // pointer to bucket array
    i32                  ind_uniqueattr_buckets_n;       // number of elements in bucket array
    i32                  ind_uniqueattr_n;               // number of elements in the hash table
    acr::FBltin*         bltin_lary[32];                 // level array
    i32                  bltin_n;                        // number of elements in array
    bool                 ctype_rank_computed;            //   false
    bool                 file_input;                     //   false
    acr::FCtype**        bh_ctype_topo_elems;            // binary heap by topokey
    i32                  bh_ctype_topo_n;                // number of elements in the heap
    i32                  bh_ctype_topo_max;              // max elements in bh_ctype_topo_elems
    acr::FCppfunc*       cppfunc_lary[32];               // level array
    i32                  cppfunc_n;                      // number of elements in array
    acr::FSsimreq*       ssimreq_lary[32];               // level array
    i32                  ssimreq_n;                      // number of elements in array
    acr::FRec**          c_ssimreq_rec_elems;            // array of pointers
    u32                  c_ssimreq_rec_n;                // array of pointers
    u32                  c_ssimreq_rec_max;              // capacity of allocated array
    acr::FCtype*         c_field_ctype;                  // ctype describing dmmeta.Field. optional pointer
    acr::FCtype*         c_ssimfile_ctype;               // ctype describing dmmeta.Ssimfile. optional pointer
    acr::FCtype**        c_ctype_front_elems;            // array of pointers
    u32                  c_ctype_front_n;                // array of pointers
    u32                  c_ctype_front_max;              // capacity of allocated array
    acr::FSortkey*       sortkey_lary[32];               // level array
    i32                  sortkey_n;                      // number of elements in array
    acr::FSortkey**      ind_sortkey_buckets_elems;      // pointer to bucket array
    i32                  ind_sortkey_buckets_n;          // number of elements in bucket array
    i32                  ind_sortkey_n;                  // number of elements in the hash table
    acr::trace           trace;                          //
};
```

#### acr.FErr - 
<a href="#acr-ferr"></a>

#### acr.FErr Fields
<a href="#acr-ferr-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr.FErr.base|[acr.Err](/txt/exe/acr/internals.md#acr-err)|[Base](#acr-err-fields)|||
#### acr.Err Fields
<a href="#acr-err-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr.Err.ctype|[acr.FCtype](/txt/exe/acr/internals.md#acr-fctype)|[Ptr](/txt/exe/amc/reftypes.md#ptr)||Parent ctype|
|acr.Err.id|u32|[Val](/txt/exe/amc/reftypes.md#val)|0|ID|
|acr.Err.text|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Error text|
|acr.Err.rec|[acr.FRec](/txt/exe/acr/internals.md#acr-frec)|[Ptr](/txt/exe/amc/reftypes.md#ptr)||Parent record|
|acr.Err.fld|[acr.FField](/txt/exe/acr/internals.md#acr-ffield)|[Ptr](/txt/exe/amc/reftypes.md#ptr)||Parent field|

#### Struct FErr
<a href="#struct-ferr"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_gen.h](/include/gen/acr_gen.h)
```
struct FErr { // acr.FErr
    acr::FErr*      err_next;          // Pointer to next free element int tpool
    acr::FErr*      zd_all_err_next;   // zslist link; -1 means not-in-list
    acr::FErr*      zd_all_err_prev;   // previous element
    acr::FCtype*    ctype;             // Parent ctype. optional pointer
    u32             id;                //   0  ID
    algo::cstring   text;              // Error text
    acr::FRec*      rec;               // Parent record. optional pointer
    acr::FField*    fld;               // Parent field. optional pointer
    // func:acr.FErr..AssignOp
    inline acr::FErr&    operator =(const acr::FErr &rhs) = delete;
    // func:acr.FErr..CopyCtor
    inline               FErr(const acr::FErr &rhs) = delete;
private:
    // func:acr.FErr..Ctor
    inline               FErr() __attribute__((nothrow));
    // func:acr.FErr..Dtor
    inline               ~FErr() __attribute__((nothrow));
    friend acr::FErr&           err_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr::FErr*           err_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 err_Delete(acr::FErr &row) __attribute__((nothrow));
};
```

#### acr.FEvalattr - 
<a href="#acr-fevalattr"></a>

#### acr.FEvalattr Fields
<a href="#acr-fevalattr-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr.FEvalattr.pick_dflt|bool|[Val](/txt/exe/amc/reftypes.md#val)||(in) If attr not found, choose default?|
|acr.FEvalattr.normalize|bool|[Val](/txt/exe/amc/reftypes.md#val)||(in) Normalize input value if possible|
|acr.FEvalattr.field|[acr.FField](/txt/exe/acr/internals.md#acr-ffield)|[Ptr](/txt/exe/amc/reftypes.md#ptr)||(in) field|
|acr.FEvalattr.attr|[algo.Attr](/txt/protocol/algo/Attr.md)|[Ptr](/txt/exe/amc/reftypes.md#ptr)||(out) Tuple attribute|
|acr.FEvalattr.value|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||(out) Resulting value|
|acr.FEvalattr.val_range|[algo.i32_Range](/txt/protocol/algo/README.md#algo-i32_range)|[Val](/txt/exe/amc/reftypes.md#val)||(out) Range in attribute corresponding to value|

#### Struct FEvalattr
<a href="#struct-fevalattr"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_gen.h](/include/gen/acr_gen.h)
```
struct FEvalattr { // acr.FEvalattr
    bool              pick_dflt;   //   false  (in) If attr not found, choose default?
    bool              normalize;   //   false  (in) Normalize input value if possible
    acr::FField*      field;       // (in) field. optional pointer
    algo::Attr*       attr;        // (out) Tuple attribute. optional pointer
    algo::cstring     value;       // (out) Resulting value
    algo::i32_Range   val_range;   // (out) Range in attribute corresponding to value
    // func:acr.FEvalattr..Ctor
    inline               FEvalattr() __attribute__((nothrow));
    // func:acr.FEvalattr..FieldwiseCtor
    explicit inline               FEvalattr(bool in_pick_dflt, bool in_normalize, acr::FField* in_field, algo::Attr* in_attr, const algo::strptr& in_value, const algo::i32_Range& in_val_range) __attribute__((nothrow));
};
```

#### acr.FField - Specify field of a struct
<a href="#acr-ffield"></a>

#### acr.FField Fields
<a href="#acr-ffield-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr.FField.base|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Base](/txt/ssimdb/dmmeta/field.md)|||
|acr.FField.p_ctype|[acr.FCtype](/txt/exe/acr/internals.md#acr-fctype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|acr.FField.p_arg|[acr.FCtype](/txt/exe/acr/internals.md#acr-fctype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|acr.FField.c_substr|[acr.FSubstr](/txt/exe/acr/internals.md#acr-fsubstr)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|acr.FField.max_attr_len|u32|[Val](/txt/exe/amc/reftypes.md#val)||Column width in chars|
|acr.FField.col_width|u32|[Val](/txt/exe/amc/reftypes.md#val)||Column width in chars|
|acr.FField.isfldfunc|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|acr.FField.anon|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|acr.FField.unique|bool|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FField
<a href="#struct-ffield"></a>
*Note:* field ``acr.FField.base`` has reftype ``base`` so the fields of [dmmeta.Field](/txt/ssimdb/dmmeta/field.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_gen.h](/include/gen/acr_gen.h)
```
struct FField { // acr.FField
    acr::FField*        zd_arg_next;            // zslist link; -1 means not-in-list
    acr::FField*        zd_arg_prev;            // previous element
    acr::FField*        ind_field_next;         // hash next
    algo::Smallstr100   field;                  // Primary key, as ctype.name
    algo::Smallstr100   arg;                    // Type of field
    algo::Smallstr50    reftype;                //   "Val"  Type constructor
    algo::CppExpr       dflt;                   // Default value (c++ expression)
    algo::Comment       comment;                //
    acr::FCtype*        p_ctype;                // reference to parent row
    acr::FCtype*        p_arg;                  // reference to parent row
    acr::FSubstr*       c_substr;               // optional pointer
    u32                 max_attr_len;           //   0  Column width in chars
    u32                 col_width;              //   0  Column width in chars
    bool                isfldfunc;              //   false
    bool                anon;                   //   false
    bool                unique;                 //   false
    bool                ctype_c_field_in_ary;   //   false  membership flag
    // x-reference on acr.FField.p_ctype prevents copy
    // x-reference on acr.FField.p_arg prevents copy
    // x-reference on acr.FField.c_substr prevents copy
    // func:acr.FField..AssignOp
    acr::FField&         operator =(const acr::FField &rhs) = delete;
    // x-reference on acr.FField.p_ctype prevents copy
    // x-reference on acr.FField.p_arg prevents copy
    // x-reference on acr.FField.c_substr prevents copy
    // func:acr.FField..CopyCtor
    FField(const acr::FField &rhs) = delete;
private:
    // func:acr.FField..Ctor
    inline               FField() __attribute__((nothrow));
    // func:acr.FField..Dtor
    inline               ~FField() __attribute__((nothrow));
    friend acr::FField&         field_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr::FField*         field_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 field_RemoveLast() __attribute__((nothrow));
};
```

#### acr.FFile - 
<a href="#acr-ffile"></a>

#### acr.FFile Fields
<a href="#acr-ffile-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr.FFile.file|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Primary key|
|acr.FFile.filename|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Non-empty if it's a real file|
|acr.FFile.ephemeral|bool|[Val](/txt/exe/amc/reftypes.md#val)||Do not save back|
|acr.FFile.sticky|bool|[Val](/txt/exe/amc/reftypes.md#val)||Records loaded from file are written back to file|
|acr.FFile.lineno|u32|[Val](/txt/exe/amc/reftypes.md#val)|1|Current line number|
|acr.FFile.zd_frec|[acr.FRec](/txt/exe/acr/internals.md#acr-frec)|[Llist](/txt/exe/amc/reftypes.md#llist)||List of records in file order|
|acr.FFile.modtime|[algo.UnTime](/txt/protocol/algo/UnTime.md)|[Val](/txt/exe/amc/reftypes.md#val)||File modification time at time of loading|
|acr.FFile.autoloaded|bool|[Val](/txt/exe/amc/reftypes.md#val)||File was pulled in implicitly: loaded records are not 'inserted'|

#### Struct FFile
<a href="#struct-ffile"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_gen.h](/include/gen/acr_gen.h)
```
struct FFile { // acr.FFile
    acr::FFile*     ind_file_next;   // hash next
    algo::cstring   file;            // Primary key
    algo::cstring   filename;        // Non-empty if it's a real file
    bool            ephemeral;       //   false  Do not save back
    bool            sticky;          //   false  Records loaded from file are written back to file
    u32             lineno;          //   1  Current line number
    acr::FRec*      zd_frec_head;    // zero-terminated doubly linked list
    acr::FRec*      zd_frec_tail;    // pointer to last element
    algo::UnTime    modtime;         // File modification time at time of loading
    bool            autoloaded;      //   false  File was pulled in implicitly: loaded records are not 'inserted'
    // reftype Llist of acr.FFile.zd_frec prohibits copy
    // func:acr.FFile..AssignOp
    inline acr::FFile&   operator =(const acr::FFile &rhs) = delete;
    // reftype Llist of acr.FFile.zd_frec prohibits copy
    // func:acr.FFile..CopyCtor
    inline               FFile(const acr::FFile &rhs) = delete;
private:
    // func:acr.FFile..Ctor
    inline               FFile() __attribute__((nothrow));
    // func:acr.FFile..Dtor
    inline               ~FFile() __attribute__((nothrow));
    friend acr::FFile&          file_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr::FFile*          file_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 file_RemoveAll() __attribute__((nothrow));
    friend void                 file_RemoveLast() __attribute__((nothrow));
};
```

#### acr.FFunique - This field must be unique in the table. Not needed for primary key
<a href="#acr-ffunique"></a>

#### acr.FFunique Fields
<a href="#acr-ffunique-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr.FFunique.base|[dmmeta.Funique](/txt/ssimdb/dmmeta/funique.md)|[Base](/txt/ssimdb/dmmeta/funique.md)|||

#### Struct FFunique
<a href="#struct-ffunique"></a>
*Note:* field ``acr.FFunique.base`` has reftype ``base`` so the fields of [dmmeta.Funique](/txt/ssimdb/dmmeta/funique.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_gen.h](/include/gen/acr_gen.h)
```
struct FFunique { // acr.FFunique
    algo::Smallstr100   field;   //
private:
    // func:acr.FFunique..Ctor
    inline               FFunique() __attribute__((nothrow));
    friend acr::FFunique&       funique_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr::FFunique*       funique_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 funique_RemoveLast() __attribute__((nothrow));
};
```

#### acr.FPdep - Dependency between two print-line records
<a href="#acr-fpdep"></a>

#### acr.FPdep Fields
<a href="#acr-fpdep-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr.FPdep.p_parent|[acr.FPline](/txt/exe/acr/internals.md#acr-fpline)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|acr.FPdep.p_child|[acr.FPline](/txt/exe/acr/internals.md#acr-fpline)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|acr.FPdep.weight|i32|[Val](/txt/exe/amc/reftypes.md#val)|||
|acr.FPdep.lindex|i32|[Val](/txt/exe/amc/reftypes.md#val)||Index of child referencing attr|

#### Struct FPdep
<a href="#struct-fpdep"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_gen.h](/include/gen/acr_gen.h)
```
struct FPdep { // acr.FPdep: Dependency between two print-line records
    acr::FPdep*    pdep_next;       // Pointer to next free element int tpool
    acr::FPdep*    zd_pdep_next;    // zslist link; -1 means not-in-list
    acr::FPdep*    zd_pdep_prev;    // previous element
    acr::FPline*   p_parent;        // reference to parent row
    acr::FPline*   p_child;         // reference to parent row
    i32            weight;          //   0
    i32            lindex;          //   0  Index of child referencing attr
    acr::FPdep*    zd_child_next;   // zslist link; -1 means not-in-list
    acr::FPdep*    zd_child_prev;   // previous element
    // func:acr.FPdep..AssignOp
    inline acr::FPdep&   operator =(const acr::FPdep &rhs) = delete;
    // func:acr.FPdep..CopyCtor
    inline               FPdep(const acr::FPdep &rhs) = delete;
private:
    // func:acr.FPdep..Ctor
    inline               FPdep() __attribute__((nothrow));
    // func:acr.FPdep..Dtor
    inline               ~FPdep() __attribute__((nothrow));
    friend acr::FPdep&          pdep_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr::FPdep*          pdep_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 pdep_Delete(acr::FPdep &row) __attribute__((nothrow));
};
```

#### acr.RecSortkey - One record
<a href="#acr-recsortkey"></a>

#### acr.RecSortkey Fields
<a href="#acr-recsortkey-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr.RecSortkey.ctype|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)||Type prefix|
|acr.RecSortkey.num|double|[Val](/txt/exe/amc/reftypes.md#val)||Numeric key (if present)|
|acr.RecSortkey.str|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||String key (sort key)|
|acr.RecSortkey.rowid|float|[Val](/txt/exe/amc/reftypes.md#val)||row id|

#### Struct RecSortkey
<a href="#struct-recsortkey"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_gen.h](/include/gen/acr_gen.h)
```
struct RecSortkey { // acr.RecSortkey: One record
    algo::Smallstr100   ctype;   // Type prefix
    double              num;     //   0.0  Numeric key (if present)
    algo::cstring       str;     // String key (sort key)
    float               rowid;   //   0.f  row id
    // func:acr.RecSortkey..EqOp
    inline bool          operator ==(const acr::RecSortkey &rhs) const __attribute__((nothrow));
    // func:acr.RecSortkey..NeOp
    inline bool          operator !=(const acr::RecSortkey &rhs) const __attribute__((nothrow));
    // func:acr.RecSortkey..LtOp
    inline bool          operator <(const acr::RecSortkey &rhs) const __attribute__((nothrow));
    // func:acr.RecSortkey..GtOp
    inline bool          operator >(const acr::RecSortkey &rhs) const __attribute__((nothrow));
    // func:acr.RecSortkey..LeOp
    inline bool          operator <=(const acr::RecSortkey &rhs) const __attribute__((nothrow));
    // func:acr.RecSortkey..GeOp
    inline bool          operator >=(const acr::RecSortkey &rhs) const __attribute__((nothrow));
    // func:acr.RecSortkey..Ctor
    inline               RecSortkey() __attribute__((nothrow));
};
```

#### acr.PlineKey - Key for sorting print-line records
<a href="#acr-plinekey"></a>

#### acr.PlineKey Fields
<a href="#acr-plinekey-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr.PlineKey.alldep|i32|[Val](/txt/exe/amc/reftypes.md#val)||# Unresolved references|
|acr.PlineKey.negdepth|i32|[Val](/txt/exe/amc/reftypes.md#val)||Minus tree depth for depth-first print|
|acr.PlineKey.ctype_rank|i32|[Val](/txt/exe/amc/reftypes.md#val)||Topological key for ctype|
|acr.PlineKey.sortkey|[acr.RecSortkey](/txt/exe/acr/internals.md#acr-recsortkey)|[Val](/txt/exe/amc/reftypes.md#val)||Sort key of original record|

#### Struct PlineKey
<a href="#struct-plinekey"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_gen.h](/include/gen/acr_gen.h)
```
struct PlineKey { // acr.PlineKey: Key for sorting print-line records
    i32               alldep;       //   0  # Unresolved references
    i32               negdepth;     //   0  Minus tree depth for depth-first print
    i32               ctype_rank;   //   0  Topological key for ctype
    acr::RecSortkey   sortkey;      // Sort key of original record
    // func:acr.PlineKey..EqOp
    inline bool          operator ==(const acr::PlineKey &rhs) const __attribute__((nothrow));
    // func:acr.PlineKey..NeOp
    inline bool          operator !=(const acr::PlineKey &rhs) const __attribute__((nothrow));
    // func:acr.PlineKey..LtOp
    inline bool          operator <(const acr::PlineKey &rhs) const __attribute__((nothrow));
    // func:acr.PlineKey..GtOp
    inline bool          operator >(const acr::PlineKey &rhs) const __attribute__((nothrow));
    // func:acr.PlineKey..LeOp
    inline bool          operator <=(const acr::PlineKey &rhs) const __attribute__((nothrow));
    // func:acr.PlineKey..GeOp
    inline bool          operator >=(const acr::PlineKey &rhs) const __attribute__((nothrow));
    // func:acr.PlineKey..Ctor
    inline               PlineKey() __attribute__((nothrow));
};
```

#### acr.FPline - Print-line
<a href="#acr-fpline"></a>

#### acr.FPline Fields
<a href="#acr-fpline-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr.FPline.key|[acr.PlineKey](/txt/exe/acr/internals.md#acr-plinekey)|[Val](/txt/exe/amc/reftypes.md#val)||Sort key|
|acr.FPline.p_rec|[acr.FRec](/txt/exe/acr/internals.md#acr-frec)|[Upptr](/txt/exe/amc/reftypes.md#upptr)||Underlying record|
|acr.FPline.zd_child|[acr.FPdep](/txt/exe/acr/internals.md#acr-fpdep)|[Llist](/txt/exe/amc/reftypes.md#llist)||List of child records|

#### Struct FPline
<a href="#struct-fpline"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_gen.h](/include/gen/acr_gen.h)
```
struct FPline { // acr.FPline: Print-line
    acr::FPline*    pline_next;      // Pointer to next free element int tpool
    acr::FPline*    zd_pline_next;   // zslist link; -1 means not-in-list
    acr::FPline*    zd_pline_prev;   // previous element
    i32             bh_pline_idx;    // index in heap; -1 means not-in-heap
    acr::PlineKey   key;             // Sort key
    acr::FRec*      p_rec;           // reference to parent row
    acr::FPdep*     zd_child_head;   // zero-terminated doubly linked list
    acr::FPdep*     zd_child_tail;   // pointer to last element
    // reftype Llist of acr.FPline.zd_child prohibits copy
    // func:acr.FPline..AssignOp
    inline acr::FPline&  operator =(const acr::FPline &rhs) = delete;
    // reftype Llist of acr.FPline.zd_child prohibits copy
    // func:acr.FPline..CopyCtor
    inline               FPline(const acr::FPline &rhs) = delete;
private:
    // func:acr.FPline..Ctor
    inline               FPline() __attribute__((nothrow));
    // func:acr.FPline..Dtor
    inline               ~FPline() __attribute__((nothrow));
    friend acr::FPline&         pline_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr::FPline*         pline_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 pline_Delete(acr::FPline &row) __attribute__((nothrow));
};
```

#### acr.FPrint - Print function
<a href="#acr-fprint"></a>

#### acr.FPrint Fields
<a href="#acr-fprint-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr.FPrint.ind_printattr|[acr.FPrintAttr](/txt/exe/acr/internals.md#acr-fprintattr)|[Thash](/txt/exe/amc/reftypes.md#thash)||Index of printed attributes (to get width)|
|acr.FPrint.c_pline|[acr.FPline](/txt/exe/acr/internals.md#acr-fpline)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)||Current print group|
|acr.FPrint.pretty|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Block alignment|
|acr.FPrint.tree|bool|[Val](/txt/exe/amc/reftypes.md#val)|false||
|acr.FPrint.fldfunc|bool|[Val](/txt/exe/amc/reftypes.md#val)|false||
|acr.FPrint.maxgroup|i32|[Val](/txt/exe/amc/reftypes.md#val)|25||
|acr.FPrint.cmt|bool|[Val](/txt/exe/amc/reftypes.md#val)|false||
|acr.FPrint.rowid|bool|[Val](/txt/exe/amc/reftypes.md#val)|true|Show row ids of records when printing|
|acr.FPrint.out|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Output|
|acr.FPrint.fstdout|bool|[Val](/txt/exe/amc/reftypes.md#val)|true|Dump to stdout?|
|acr.FPrint.loose|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Loose referential integrity|
|acr.FPrint.showstatus|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|show record status (acr.insert etc)|
|acr.FPrint.skip_line|i32|[Val](/txt/exe/amc/reftypes.md#val)|||
|acr.FPrint.this_depth|i32|[Val](/txt/exe/amc/reftypes.md#val)|||
|acr.FPrint.prev_depth|i32|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FPrint
<a href="#struct-fprint"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_gen.h](/include/gen/acr_gen.h)
```
struct FPrint { // acr.FPrint: Print function
    acr::FPrintAttr**   ind_printattr_buckets_elems;   // pointer to bucket array
    i32                 ind_printattr_buckets_n;       // number of elements in bucket array
    i32                 ind_printattr_n;               // number of elements in the hash table
    acr::FPline**       c_pline_elems;                 // array of pointers
    u32                 c_pline_n;                     // array of pointers
    u32                 c_pline_max;                   // capacity of allocated array
    bool                pretty;                        //   false  Block alignment
    bool                tree;                          //   false
    bool                fldfunc;                       //   false
    i32                 maxgroup;                      //   25
    bool                cmt;                           //   false
    bool                rowid;                         //   true  Show row ids of records when printing
    algo::cstring       out;                           // Output
    bool                fstdout;                       //   true  Dump to stdout?
    bool                loose;                         //   false  Loose referential integrity
    bool                showstatus;                    //   false  show record status (acr.insert etc)
    i32                 skip_line;                     //   0
    i32                 this_depth;                    //   0
    i32                 prev_depth;                    //   0
    // func:acr.FPrint..Ctor
    inline               FPrint() __attribute__((nothrow));
    // func:acr.FPrint..Dtor
    inline               ~FPrint() __attribute__((nothrow));
};
```

#### acr.FPrintAttr - 
<a href="#acr-fprintattr"></a>

#### acr.FPrintAttr Fields
<a href="#acr-fprintattr-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr.FPrintAttr.p_print|[acr.FPrint](/txt/exe/acr/internals.md#acr-fprint)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|acr.FPrintAttr.field|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|0||
|acr.FPrintAttr.width|u32|[Val](/txt/exe/amc/reftypes.md#val)|0|Name width|

#### Struct FPrintAttr
<a href="#struct-fprintattr"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_gen.h](/include/gen/acr_gen.h)
```
struct FPrintAttr { // acr.FPrintAttr
    acr::FPrintAttr*    ind_printattr_next;   // hash next
    acr::FPrint*        p_print;              // reference to parent row
    algo::Smallstr100   field;                //   0
    u32                 width;                //   0  Name width
    // func:acr.FPrintAttr..AssignOp
    inline acr::FPrintAttr& operator =(const acr::FPrintAttr &rhs) = delete;
    // func:acr.FPrintAttr..CopyCtor
    inline               FPrintAttr(const acr::FPrintAttr &rhs) = delete;
private:
    // func:acr.FPrintAttr..Ctor
    inline               FPrintAttr() __attribute__((nothrow));
    // func:acr.FPrintAttr..Dtor
    inline               ~FPrintAttr() __attribute__((nothrow));
    friend acr::FPrintAttr&     printattr_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr::FPrintAttr*     printattr_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 printattr_Delete(acr::FPrintAttr &row) __attribute__((nothrow));
};
```

#### acr.Queryop - Operation to perform
<a href="#acr-queryop"></a>

#### acr.Queryop Fields
<a href="#acr-queryop-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr.Queryop.value|u8|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct Queryop
<a href="#struct-queryop"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_gen.h](/include/gen/acr_gen.h)
```
struct Queryop { // acr.Queryop: Operation to perform
    u8   value;   //   0
    // func:acr.Queryop.value.Cast
    inline               operator acr_Queryop_value_Enum() const __attribute__((nothrow));
    // func:acr.Queryop..Ctor
    inline               Queryop() __attribute__((nothrow));
    // func:acr.Queryop..FieldwiseCtor
    explicit inline               Queryop(u8 in_value) __attribute__((nothrow));
    // func:acr.Queryop..EnumCtor
    inline               Queryop(acr_Queryop_value_Enum arg) __attribute__((nothrow));
};
```

#### acr.FQuery - 
<a href="#acr-fquery"></a>

#### acr.FQuery Fields
<a href="#acr-fquery-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr.FQuery.queryop|[acr.Queryop](/txt/exe/acr/internals.md#acr-queryop)|[Val](/txt/exe/amc/reftypes.md#val)||Type of operation to perform|
|acr.FQuery.query|[acr.AttrRegx](/txt/exe/acr/internals.md#acr-attrregx)|[Val](/txt/exe/amc/reftypes.md#val)||Field name (SQL regex as string)|
|acr.FQuery.where|[acr.AttrRegx](/txt/exe/acr/internals.md#acr-attrregx)|[Tary](/txt/exe/amc/reftypes.md#tary)||Additional key/value args|
|acr.FQuery.new_val|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|New value, used with rename|
|acr.FQuery.ssimfile|[algo.cstring](/txt/protocol/algo/cstring.md)|[Regx](/txt/exe/amc/reftypes.md#regx)|""|Full ssimfile name (SQL regex as string)|
|acr.FQuery.comment|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|""|Debug info|
|acr.FQuery.c_ctype|[acr.FCtype](/txt/exe/acr/internals.md#acr-fctype)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)||Array of ctypes to check|
|acr.FQuery.c_field|[acr.FField](/txt/exe/acr/internals.md#acr-ffield)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|acr.FQuery.c_rec|[acr.FRec](/txt/exe/acr/internals.md#acr-frec)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|acr.FQuery.n_regx_match|i32|[Val](/txt/exe/amc/reftypes.md#val)||Number of regex matches executed|
|acr.FQuery.n_visit_field|i32|[Val](/txt/exe/amc/reftypes.md#val)||Number of fields visited|
|acr.FQuery.n_visit_ctype|i32|[Val](/txt/exe/amc/reftypes.md#val)||Number of ctypes visited|
|acr.FQuery.n_mod_rec|i32|[Val](/txt/exe/amc/reftypes.md#val)||Number of records modified|

#### Struct FQuery
<a href="#struct-fquery"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_gen.h](/include/gen/acr_gen.h)
```
struct FQuery { // acr.FQuery
    acr::FQuery*     query_next;      // Pointer to next free element int tpool
    acr::FQuery*     zs_query_next;   // zslist link; -1 means not-in-list
    acr::Queryop     queryop;         // Type of operation to perform
    acr::AttrRegx    query;           // Field name (SQL regex as string)
    acr::AttrRegx*   where_elems;     // pointer to elements
    u32              where_n;         // number of elements in array
    u32              where_max;       // max. capacity of array before realloc
    algo::cstring    new_val;         //   ""  New value, used with rename
    algo_lib::Regx   ssimfile;        //   ""  Acr Regx
    algo::cstring    comment;         //   ""  Debug info
    acr::FCtype**    c_ctype_elems;   // array of pointers
    u32              c_ctype_n;       // array of pointers
    u32              c_ctype_max;     // capacity of allocated array
    acr::FField**    c_field_elems;   // array of pointers
    u32              c_field_n;       // array of pointers
    u32              c_field_max;     // capacity of allocated array
    acr::FRec**      c_rec_elems;     // array of pointers
    u32              c_rec_n;         // array of pointers
    u32              c_rec_max;       // capacity of allocated array
    i32              n_regx_match;    //   0  Number of regex matches executed
    i32              n_visit_field;   //   0  Number of fields visited
    i32              n_visit_ctype;   //   0  Number of ctypes visited
    i32              n_mod_rec;       //   0  Number of records modified
    // reftype Ptrary of acr.FQuery.c_ctype prohibits copy
    // reftype Ptrary of acr.FQuery.c_field prohibits copy
    // reftype Ptrary of acr.FQuery.c_rec prohibits copy
    // func:acr.FQuery..AssignOp
    acr::FQuery&         operator =(const acr::FQuery &rhs) = delete;
    // reftype Ptrary of acr.FQuery.c_ctype prohibits copy
    // reftype Ptrary of acr.FQuery.c_field prohibits copy
    // reftype Ptrary of acr.FQuery.c_rec prohibits copy
    // func:acr.FQuery..CopyCtor
    FQuery(const acr::FQuery &rhs) = delete;
private:
    // func:acr.FQuery..Ctor
    inline               FQuery() __attribute__((nothrow));
    // func:acr.FQuery..Dtor
    inline               ~FQuery() __attribute__((nothrow));
    friend acr::FQuery&         query_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr::FQuery*         query_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 query_Delete(acr::FQuery &row) __attribute__((nothrow));
};
```

#### acr.FRec - 
<a href="#acr-frec"></a>

#### acr.FRec Fields
<a href="#acr-frec-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr.FRec.pkey|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Record key (arbitrary string)|
|acr.FRec.oldpkey|[algo.cstring](/txt/protocol/algo/cstring.md)|[Delptr](/txt/exe/amc/reftypes.md#delptr)||Old pkey (if renamed)|
|acr.FRec.tuple|[algo.Tuple](/txt/protocol/algo/Tuple.md)|[Val](/txt/exe/amc/reftypes.md#val)||Data item|
|acr.FRec.del|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Delete?|
|acr.FRec.mod|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Record was modified?|
|acr.FRec.metasel|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Select for meta-data|
|acr.FRec.isnew|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|Inserted newly|
|acr.FRec.seldist|i32|[Val](/txt/exe/amc/reftypes.md#val)||Distance to selection. >0 is up, <0 is down|
|acr.FRec.sortkey|[acr.RecSortkey](/txt/exe/acr/internals.md#acr-recsortkey)|[Val](/txt/exe/amc/reftypes.md#val)||Output sort key|
|acr.FRec.c_pline|[acr.FPline](/txt/exe/acr/internals.md#acr-fpline)|[Ptr](/txt/exe/amc/reftypes.md#ptr)||Optional pline|
|acr.FRec.p_outfile|[acr.FFile](/txt/exe/acr/internals.md#acr-ffile)|[Upptr](/txt/exe/amc/reftypes.md#upptr)||Record's OUTPUT file (may differ from input)|
|acr.FRec.p_infile|[acr.FFile](/txt/exe/acr/internals.md#acr-ffile)|[Upptr](/txt/exe/amc/reftypes.md#upptr)||File from which record was loaded|
|acr.FRec.p_ctype|[acr.FCtype](/txt/exe/acr/internals.md#acr-fctype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|acr.FRec.lineno|i32|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FRec
<a href="#struct-frec"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_gen.h](/include/gen/acr_gen.h)
```
struct FRec { // acr.FRec
    acr::FRec*        zd_ctype_rec_next;          // zslist link; -1 means not-in-list
    acr::FRec*        zd_ctype_rec_prev;          // previous element
    acr::FRec*        ind_ctype_rec_next;         // hash next
    acr::FRec*        zd_ctype_selrec_next;       // zslist link; -1 means not-in-list
    acr::FRec*        zd_ctype_selrec_prev;       // previous element
    acr::FRec*        rec_next;                   // Pointer to next free element int tpool
    acr::FRec*        zd_all_selrec_next;         // zslist link; -1 means not-in-list
    acr::FRec*        zd_all_selrec_prev;         // previous element
    acr::FRec*        zd_frec_next;               // zslist link; -1 means not-in-list
    acr::FRec*        zd_frec_prev;               // previous element
    algo::cstring     pkey;                       // Record key (arbitrary string)
    algo::cstring*    oldpkey;                    // Private pointer to value
    algo::Tuple       tuple;                      // Data item
    bool              del;                        //   false  Delete?
    bool              mod;                        //   false  Record was modified?
    bool              metasel;                    //   false  Select for meta-data
    bool              isnew;                      //   false  Inserted newly
    i32               seldist;                    //   0  Distance to selection. >0 is up, <0 is down
    acr::RecSortkey   sortkey;                    // Output sort key
    acr::FPline*      c_pline;                    // Optional pline. optional pointer
    acr::FFile*       p_outfile;                  // reference to parent row
    acr::FFile*       p_infile;                   // reference to parent row
    acr::FCtype*      p_ctype;                    // reference to parent row
    i32               lineno;                     //   0
    bool              _db_c_ssimreq_rec_in_ary;   //   false  membership flag
    // reftype Delptr of acr.FRec.oldpkey prohibits copy
    // x-reference on acr.FRec.c_pline prevents copy
    // func:acr.FRec..AssignOp
    acr::FRec&           operator =(const acr::FRec &rhs) = delete;
    // reftype Delptr of acr.FRec.oldpkey prohibits copy
    // x-reference on acr.FRec.c_pline prevents copy
    // func:acr.FRec..CopyCtor
    FRec(const acr::FRec &rhs) = delete;
private:
    // func:acr.FRec..Ctor
    inline               FRec() __attribute__((nothrow));
    // func:acr.FRec..Dtor
    inline               ~FRec() __attribute__((nothrow));
    friend acr::FRec&           rec_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr::FRec*           rec_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 rec_Delete(acr::FRec &row) __attribute__((nothrow));
};
```

#### acr.FSmallstr - Generated fixed-length padded or length-delimited string field
<a href="#acr-fsmallstr"></a>

#### acr.FSmallstr Fields
<a href="#acr-fsmallstr-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr.FSmallstr.base|[dmmeta.Smallstr](/txt/ssimdb/dmmeta/smallstr.md)|[Base](/txt/ssimdb/dmmeta/smallstr.md)|||

#### Struct FSmallstr
<a href="#struct-fsmallstr"></a>
*Note:* field ``acr.FSmallstr.base`` has reftype ``base`` so the fields of [dmmeta.Smallstr](/txt/ssimdb/dmmeta/smallstr.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_gen.h](/include/gen/acr_gen.h)
```
struct FSmallstr { // acr.FSmallstr
    algo::Smallstr100   field;     //
    i32                 length;    //   0  Maximum characters in the string
    algo::Smallstr50    strtype;   // Data format for string
    algo::CppExpr       pad;       // Pad character (if applicable)
    bool                strict;    //   false
private:
    // func:acr.FSmallstr..Ctor
    inline               FSmallstr() __attribute__((nothrow));
    friend acr::FSmallstr&      smallstr_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr::FSmallstr*      smallstr_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 smallstr_RemoveLast() __attribute__((nothrow));
};
```

#### acr.FSortkey - Keep track of next rowid for each sortkey
<a href="#acr-fsortkey"></a>

#### acr.FSortkey Fields
<a href="#acr-fsortkey-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr.FSortkey.sortkey|[acr.RecSortkey](/txt/exe/acr/internals.md#acr-recsortkey)|[Val](/txt/exe/amc/reftypes.md#val)||Sort key|
|acr.FSortkey.next_rowid|double|[Val](/txt/exe/amc/reftypes.md#val)|0||

#### Struct FSortkey
<a href="#struct-fsortkey"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_gen.h](/include/gen/acr_gen.h)
```
struct FSortkey { // acr.FSortkey: Keep track of next rowid for each sortkey
    acr::FSortkey*    ind_sortkey_next;   // hash next
    acr::RecSortkey   sortkey;            // Sort key
    double            next_rowid;         //   0
    // func:acr.FSortkey..AssignOp
    inline acr::FSortkey& operator =(const acr::FSortkey &rhs) = delete;
    // func:acr.FSortkey..CopyCtor
    inline               FSortkey(const acr::FSortkey &rhs) = delete;
private:
    // func:acr.FSortkey..Ctor
    inline               FSortkey() __attribute__((nothrow));
    // func:acr.FSortkey..Dtor
    inline               ~FSortkey() __attribute__((nothrow));
    friend acr::FSortkey&       sortkey_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr::FSortkey*       sortkey_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 sortkey_RemoveAll() __attribute__((nothrow));
    friend void                 sortkey_RemoveLast() __attribute__((nothrow));
};
```

#### acr.FSsimfile - One full table
<a href="#acr-fssimfile"></a>

#### acr.FSsimfile Fields
<a href="#acr-fssimfile-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr.FSsimfile.base|[dmmeta.Ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|[Base](/txt/ssimdb/dmmeta/ssimfile.md)|||
|acr.FSsimfile.c_file|[acr.FFile](/txt/exe/acr/internals.md#acr-ffile)|[Ptr](/txt/exe/amc/reftypes.md#ptr)||optional!|
|acr.FSsimfile.p_ctype|[acr.FCtype](/txt/exe/acr/internals.md#acr-fctype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|acr.FSsimfile.c_ssimsort|[acr.FSsimsort](/txt/exe/acr/internals.md#acr-fssimsort)|[Ptr](/txt/exe/amc/reftypes.md#ptr)||Optional sort order|

#### Struct FSsimfile
<a href="#struct-fssimfile"></a>
*Note:* field ``acr.FSsimfile.base`` has reftype ``base`` so the fields of [dmmeta.Ssimfile](/txt/ssimdb/dmmeta/ssimfile.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_gen.h](/include/gen/acr_gen.h)
```
struct FSsimfile { // acr.FSsimfile: One full table
    acr::FSsimfile*     ind_ssimfile_next;   // hash next
    algo::Smallstr50    ssimfile;            //
    algo::Smallstr100   ctype;               //
    acr::FFile*         c_file;              // optional!. optional pointer
    acr::FCtype*        p_ctype;             // reference to parent row
    acr::FSsimsort*     c_ssimsort;          // Optional sort order. optional pointer
    // x-reference on acr.FSsimfile.p_ctype prevents copy
    // x-reference on acr.FSsimfile.c_ssimsort prevents copy
    // func:acr.FSsimfile..AssignOp
    inline acr::FSsimfile& operator =(const acr::FSsimfile &rhs) = delete;
    // x-reference on acr.FSsimfile.p_ctype prevents copy
    // x-reference on acr.FSsimfile.c_ssimsort prevents copy
    // func:acr.FSsimfile..CopyCtor
    inline               FSsimfile(const acr::FSsimfile &rhs) = delete;
private:
    // func:acr.FSsimfile..Ctor
    inline               FSsimfile() __attribute__((nothrow));
    // func:acr.FSsimfile..Dtor
    inline               ~FSsimfile() __attribute__((nothrow));
    friend acr::FSsimfile&      ssimfile_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr::FSsimfile*      ssimfile_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 ssimfile_RemoveLast() __attribute__((nothrow));
};
```

#### acr.FSsimreq - Extended constraints for ssim records
<a href="#acr-fssimreq"></a>

#### acr.FSsimreq Fields
<a href="#acr-fssimreq-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr.FSsimreq.base|[dmmeta.Ssimreq](/txt/ssimdb/dmmeta/ssimreq.md)|[Base](/txt/ssimdb/dmmeta/ssimreq.md)|||
|acr.FSsimreq.p_ctype|[acr.FCtype](/txt/exe/acr/internals.md#acr-fctype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|acr.FSsimreq.p_child_ssimfile|[acr.FSsimfile](/txt/exe/acr/internals.md#acr-fssimfile)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|acr.FSsimreq.p_parent_field|[acr.FField](/txt/exe/acr/internals.md#acr-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|acr.FSsimreq.regx_value|[algo.cstring](/txt/protocol/algo/cstring.md)|[RegxSql](/txt/exe/amc/reftypes.md#regxsql)|||

#### Struct FSsimreq
<a href="#struct-fssimreq"></a>
*Note:* field ``acr.FSsimreq.base`` has reftype ``base`` so the fields of [dmmeta.Ssimreq](/txt/ssimdb/dmmeta/ssimreq.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_gen.h](/include/gen/acr_gen.h)
```
struct FSsimreq { // acr.FSsimreq
    algo::Smallstr100   ssimreq;                  //
    algo::Smallstr100   parent;                   //
    bool                reqchild;                 //   false
    bool                bidir;                    //   false
    algo::Comment       comment;                  //
    acr::FCtype*        p_ctype;                  // reference to parent row
    acr::FSsimfile*     p_child_ssimfile;         // reference to parent row
    acr::FField*        p_parent_field;           // reference to parent row
    algo_lib::Regx      regx_value;               // Sql Regx
    bool                ctype_c_ssimreq_in_ary;   //   false  membership flag
    // x-reference on acr.FSsimreq.p_ctype prevents copy
    // x-reference on acr.FSsimreq.p_child_ssimfile prevents copy
    // x-reference on acr.FSsimreq.p_parent_field prevents copy
    // func:acr.FSsimreq..AssignOp
    acr::FSsimreq&       operator =(const acr::FSsimreq &rhs) = delete;
    // x-reference on acr.FSsimreq.p_ctype prevents copy
    // x-reference on acr.FSsimreq.p_child_ssimfile prevents copy
    // x-reference on acr.FSsimreq.p_parent_field prevents copy
    // func:acr.FSsimreq..CopyCtor
    FSsimreq(const acr::FSsimreq &rhs) = delete;
private:
    // func:acr.FSsimreq..Ctor
    inline               FSsimreq() __attribute__((nothrow));
    // func:acr.FSsimreq..Dtor
    inline               ~FSsimreq() __attribute__((nothrow));
    friend acr::FSsimreq&       ssimreq_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr::FSsimreq*       ssimreq_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 ssimreq_RemoveAll() __attribute__((nothrow));
    friend void                 ssimreq_RemoveLast() __attribute__((nothrow));
};
```

#### acr.FSsimsort - Define sort order for ssimfile
<a href="#acr-fssimsort"></a>

#### acr.FSsimsort Fields
<a href="#acr-fssimsort-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr.FSsimsort.base|[dmmeta.Ssimsort](/txt/ssimdb/dmmeta/ssimsort.md)|[Base](/txt/ssimdb/dmmeta/ssimsort.md)|||

#### Struct FSsimsort
<a href="#struct-fssimsort"></a>
*Note:* field ``acr.FSsimsort.base`` has reftype ``base`` so the fields of [dmmeta.Ssimsort](/txt/ssimdb/dmmeta/ssimsort.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_gen.h](/include/gen/acr_gen.h)
```
struct FSsimsort { // acr.FSsimsort
    acr::FSsimsort*     ind_ssimsort_next;   // hash next
    algo::Smallstr50    ssimfile;            //
    algo::Smallstr100   sortfld;             //
    // func:acr.FSsimsort..AssignOp
    inline acr::FSsimsort& operator =(const acr::FSsimsort &rhs) = delete;
    // func:acr.FSsimsort..CopyCtor
    inline               FSsimsort(const acr::FSsimsort &rhs) = delete;
private:
    // func:acr.FSsimsort..Ctor
    inline               FSsimsort() __attribute__((nothrow));
    // func:acr.FSsimsort..Dtor
    inline               ~FSsimsort() __attribute__((nothrow));
    friend acr::FSsimsort&      ssimsort_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr::FSsimsort*      ssimsort_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 ssimsort_RemoveLast() __attribute__((nothrow));
};
```

#### acr.FSubstr - Specify that the field value is computed from a substring of another field
<a href="#acr-fsubstr"></a>

#### acr.FSubstr Fields
<a href="#acr-fsubstr-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr.FSubstr.base|[dmmeta.Substr](/txt/ssimdb/dmmeta/substr.md)|[Base](/txt/ssimdb/dmmeta/substr.md)|||

#### Struct FSubstr
<a href="#struct-fsubstr"></a>
*Note:* field ``acr.FSubstr.base`` has reftype ``base`` so the fields of [dmmeta.Substr](/txt/ssimdb/dmmeta/substr.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_gen.h](/include/gen/acr_gen.h)
```
struct FSubstr { // acr.FSubstr
    algo::Smallstr100   field;      //
    algo::CppExpr       expr;       //
    algo::Smallstr100   srcfield;   //
    // func:acr.FSubstr..AssignOp
    inline acr::FSubstr& operator =(const acr::FSubstr &rhs) = delete;
    // func:acr.FSubstr..CopyCtor
    inline               FSubstr(const acr::FSubstr &rhs) = delete;
private:
    // func:acr.FSubstr..Ctor
    inline               FSubstr() __attribute__((nothrow));
    // func:acr.FSubstr..Dtor
    inline               ~FSubstr() __attribute__((nothrow));
    friend acr::FSubstr&        substr_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr::FSubstr*        substr_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 substr_RemoveLast() __attribute__((nothrow));
};
```

#### acr.FTempkey - 
<a href="#acr-ftempkey"></a>

#### acr.FTempkey Fields
<a href="#acr-ftempkey-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr.FTempkey.tempkey|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FTempkey
<a href="#struct-ftempkey"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_gen.h](/include/gen/acr_gen.h)
```
struct FTempkey { // acr.FTempkey
    acr::FTempkey*   ind_tempkey_next;   // hash next
    algo::cstring    tempkey;            //
    // func:acr.FTempkey..AssignOp
    inline acr::FTempkey& operator =(const acr::FTempkey &rhs) = delete;
    // func:acr.FTempkey..CopyCtor
    inline               FTempkey(const acr::FTempkey &rhs) = delete;
private:
    // func:acr.FTempkey..Ctor
    inline               FTempkey() __attribute__((nothrow));
    // func:acr.FTempkey..Dtor
    inline               ~FTempkey() __attribute__((nothrow));
    friend acr::FTempkey&       tempkey_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr::FTempkey*       tempkey_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 tempkey_RemoveAll() __attribute__((nothrow));
    friend void                 tempkey_RemoveLast() __attribute__((nothrow));
};
```

#### acr.FUniqueattr - 
<a href="#acr-funiqueattr"></a>

#### acr.FUniqueattr Fields
<a href="#acr-funiqueattr-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr.FUniqueattr.uniqueattr|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Data|

#### Struct FUniqueattr
<a href="#struct-funiqueattr"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_gen.h](/include/gen/acr_gen.h)
```
struct FUniqueattr { // acr.FUniqueattr
    acr::FUniqueattr*   uniqueattr_next;       // Pointer to next free element int tpool
    acr::FUniqueattr*   ind_uniqueattr_next;   // hash next
    algo::cstring       uniqueattr;            // Data
    // func:acr.FUniqueattr..AssignOp
    inline acr::FUniqueattr& operator =(const acr::FUniqueattr &rhs) = delete;
    // func:acr.FUniqueattr..CopyCtor
    inline               FUniqueattr(const acr::FUniqueattr &rhs) = delete;
private:
    // func:acr.FUniqueattr..Ctor
    inline               FUniqueattr() __attribute__((nothrow));
    // func:acr.FUniqueattr..Dtor
    inline               ~FUniqueattr() __attribute__((nothrow));
    friend acr::FUniqueattr&    uniqueattr_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr::FUniqueattr*    uniqueattr_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 uniqueattr_Delete(acr::FUniqueattr &row) __attribute__((nothrow));
};
```

#### acr.FWrite - Print function
<a href="#acr-fwrite"></a>

#### acr.FWrite Fields
<a href="#acr-fwrite-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr.FWrite.c_cmtrec|[acr.FRec](/txt/exe/acr/internals.md#acr-frec)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)||Temporary array for write|

#### Struct FWrite
<a href="#struct-fwrite"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_gen.h](/include/gen/acr_gen.h)
```
struct FWrite { // acr.FWrite: Print function
    acr::FRec**   c_cmtrec_elems;   // array of pointers
    u32           c_cmtrec_n;       // array of pointers
    u32           c_cmtrec_max;     // capacity of allocated array
    // func:acr.FWrite..Ctor
    inline               FWrite() __attribute__((nothrow));
    // func:acr.FWrite..Dtor
    inline               ~FWrite() __attribute__((nothrow));
};
```

#### acr.ReadMode - 
<a href="#acr-readmode"></a>

#### acr.ReadMode Fields
<a href="#acr-readmode-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr.ReadMode.read_mode|u8|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct ReadMode
<a href="#struct-readmode"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_gen.h](/include/gen/acr_gen.h)
```
struct ReadMode { // acr.ReadMode
    u8   read_mode;   //   0
    // func:acr.ReadMode.read_mode.Cast
    inline               operator acr_ReadModeEnum() const __attribute__((nothrow));
    // func:acr.ReadMode..Ctor
    inline               ReadMode() __attribute__((nothrow));
    // func:acr.ReadMode..FieldwiseCtor
    explicit inline               ReadMode(u8 in_read_mode) __attribute__((nothrow));
    // func:acr.ReadMode..EnumCtor
    inline               ReadMode(acr_ReadModeEnum arg) __attribute__((nothrow));
};
```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Imdb -->

### Tests
<a href="#tests"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Tests -->
The following component tests are defined for `acr`.
These can be executed with `atf_comp <comptest> -v`
|Comptest|Comment|
|---|---|
|[acr.BadInsert](/test/atf_comp/acr.BadInsert)|Duplicate insert is ignored|
|[acr.BadNs](/test/atf_comp/acr.BadNs)|Insert a bad record and check that it's detected|
|[acr.BadPkey](/test/atf_comp/acr.BadPkey)|Warning about missing pkey - not error|
|[acr.BadReftype](/test/atf_comp/acr.BadReftype)|Invalid reftype detected|
|[acr.CascDel](/test/atf_comp/acr.CascDel)|-del recursively deletes dependent records|
|[acr.CascDel2](/test/atf_comp/acr.CascDel2)|Insert records & cascade delete them -- no change|
|[acr.DelField](/test/atf_comp/acr.DelField)|A field is deleted|
|[acr.DelRecord](/test/atf_comp/acr.DelRecord)|A record is deleted|
|[acr.DeleteReinsert](/test/atf_comp/acr.DeleteReinsert)|A record is deleted and re-inserted|
|[acr.Fields](/test/atf_comp/acr.Fields)|Select fields|
|[acr.FieldsComma](/test/atf_comp/acr.FieldsComma)|Select fields with comma separator|
|[acr.GitTrigger1](/test/atf_comp/acr.GitTrigger1)|Test -g option|
|[acr.Insert](/test/atf_comp/acr.Insert)|Insert a few records|
|[acr.InsertDelete](/test/atf_comp/acr.InsertDelete)|Insert & delete record, nothing happens|
|[acr.Merge](/test/atf_comp/acr.Merge)|Merging|
|[acr.Meta1](/test/atf_comp/acr.Meta1)|Select meta-information|
|[acr.Meta2](/test/atf_comp/acr.Meta2)|Select meta-information|
|[acr.Meta3](/test/atf_comp/acr.Meta3)|Select meta-information|
|[acr.NullTrunc](/test/atf_comp/acr.NullTrunc)|Trunc with reinsertion has no effect|
|[acr.QueryCtype](/test/atf_comp/acr.QueryCtype)|Select one record|
|[acr.RenameCollision](/test/atf_comp/acr.RenameCollision)|Rename with collision|
|[acr.RenameField](/test/atf_comp/acr.RenameField)|-rename -field renames affected attributes in dataset|
|[acr.RenameRecord](/test/atf_comp/acr.RenameRecord)|Rename field and propagate recursively through structured keys|
|[acr.Replace](/test/atf_comp/acr.Replace)|A record is replaced|
|[acr.Select](/test/atf_comp/acr.Select)|A record is selected|
|[acr.SelectStdin](/test/atf_comp/acr.SelectStdin)|A record is selected by reading stdin|
|[acr.SelectTree](/test/atf_comp/acr.SelectTree)|Tree selection|
|[acr.TooManyArgs](/test/atf_comp/acr.TooManyArgs)|Acr command line input error|
|[acr.Trunc](/test/atf_comp/acr.Trunc)|Truncate table|
|[acr.UpdateBad](/test/atf_comp/acr.UpdateBad)|Update fails|
|[acr.UpdateGood](/test/atf_comp/acr.UpdateGood)|Update succeeds|
|[acr.Where](/test/atf_comp/acr.Where)|Test -where option|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Tests -->


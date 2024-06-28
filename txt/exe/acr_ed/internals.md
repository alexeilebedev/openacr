## acr_ed - Internals


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
for usage, see [acr_ed - Script generator for common dev tasks](/txt/exe/acr_ed/README.md)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Description -->

### Sources
<a href="#sources"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Sources -->
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/acr_ed/ctype.cpp](/cpp/acr_ed/ctype.cpp)|Create, delete, rename ctype|
|[cpp/acr_ed/field.cpp](/cpp/acr_ed/field.cpp)|Create, delete, rename field|
|[cpp/acr_ed/finput.cpp](/cpp/acr_ed/finput.cpp)||
|[cpp/acr_ed/main.cpp](/cpp/acr_ed/main.cpp)||
|[cpp/acr_ed/srcfile.cpp](/cpp/acr_ed/srcfile.cpp)|Create, delete, rename source file|
|[cpp/acr_ed/ssimfile.cpp](/cpp/acr_ed/ssimfile.cpp)|Create, delete, rename ssim file|
|[cpp/acr_ed/target.cpp](/cpp/acr_ed/target.cpp)|Create, delete, rename target|
|[cpp/acr_ed/unittest.cpp](/cpp/acr_ed/unittest.cpp)|Create, delete, rename unit test|
|[cpp/gen/acr_ed_gen.cpp](/cpp/gen/acr_ed_gen.cpp)||
|[include/acr_ed.h](/include/acr_ed.h)||
|[include/gen/acr_ed_gen.h](/include/gen/acr_ed_gen.h)||
|[include/gen/acr_ed_gen.inl.h](/include/gen/acr_ed_gen.inl.h)||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Sources -->

### Dependencies
<a href="#dependencies"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Dependencies -->
The build target depends on the following libraries
|Target|Comment|
|---|---|
|[algo_lib](/txt/lib/algo_lib/README.md)|Support library for all executables|
|[lib_prot](/txt/lib/lib_prot/README.md)|Library covering all protocols|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Dependencies -->

### In Memory DB
<a href="#in-memory-db"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Imdb -->
`acr_ed` generated code creates the tables below.
All allocations are done through global `acr_ed::_db` [acr_ed.FDb](#acr_ed-fdb) structure
|Ctype|Ssimfile|Create|Access|
|---|---|---|---|
|[acr_ed.FCfmt](#acr_ed-fcfmt)|[dmmeta.cfmt](/txt/ssimdb/dmmeta/cfmt.md)|FDb.cfmt (Lary)|cfmt (Lary, by rowid)|
||||FCtype.c_cfmt (Ptrary)|
|[acr_ed.FCpptype](#acr_ed-fcpptype)|[dmmeta.cpptype](/txt/ssimdb/dmmeta/cpptype.md)|FDb.cpptype (Lary)|cpptype (Lary, by rowid)|
||||FCtype.c_cpptype (Ptr)|
|[acr_ed.FCstr](#acr_ed-fcstr)|[dmmeta.cstr](/txt/ssimdb/dmmeta/cstr.md)|FDb.cstr (Lary)|cstr (Lary, by rowid)|
||||FCtype.c_cstr (Ptr)|
|[acr_ed.FCtype](#acr_ed-fctype)|[dmmeta.ctype](/txt/ssimdb/dmmeta/ctype.md)|FDb.ctype (Lary)|ctype (Lary, by rowid)|ind_ctype (Thash, hash field ctype)|
||||FField.p_ctype (Upptr)|
||||FField.p_arg (Upptr)|
||||FSsimfile.p_ctype (Upptr)|
|[acr_ed.FEdaction](#acr_ed-fedaction)|[dev.edaction](/txt/ssimdb/dev/edaction.md)|FDb.edaction (Inlary)|**static**|ind_edaction (Thash, hash field edaction)|
|[acr_ed.FDb](#acr_ed-fdb)||FDb._db (Global)|
|[acr_ed.FField](#acr_ed-ffield)|[dmmeta.field](/txt/ssimdb/dmmeta/field.md)|FDb.field (Lary)|field (Lary, by rowid)|ind_field (Thash, hash field field)|
||||FCtype.c_field (Ptrary)|
|[acr_ed.FFprefix](#acr_ed-ffprefix)|[dmmeta.fprefix](/txt/ssimdb/dmmeta/fprefix.md)|FDb.fprefix (Lary)|fprefix (Lary, by rowid)|ind_fprefix (Thash, hash field fprefix)|
|[acr_ed.FGitfile](#acr_ed-fgitfile)|[dev.gitfile](/txt/ssimdb/dev/gitfile.md)|FDb.gitfile (Lary)|gitfile (Lary, by rowid)|
|[acr_ed.FListtype](#acr_ed-flisttype)|[dmmeta.listtype](/txt/ssimdb/dmmeta/listtype.md)|FDb.listtype (Lary)|listtype (Lary, by rowid)|ind_listtype (Thash, hash field listtype)|
|[acr_ed.FNs](#acr_ed-fns)|[dmmeta.ns](/txt/ssimdb/dmmeta/ns.md)|FDb.ns (Lary)|ns (Lary, by rowid)|ind_ns (Thash, hash field ns)|
||||FCtype.p_ns (Upptr)|
||||FField.p_ns (Upptr)|
||||FSsimfile.p_ns (Upptr)|
||||FTarget.p_ns (Upptr)|
|[acr_ed.FNsdb](#acr_ed-fnsdb)|[dmmeta.nsdb](/txt/ssimdb/dmmeta/nsdb.md)|FDb.nsdb (Lary)|nsdb (Lary, by rowid)|ind_nsdb (Thash, hash field ns)|
|[acr_ed.FPack](#acr_ed-fpack)|[dmmeta.pack](/txt/ssimdb/dmmeta/pack.md)|FDb.pack (Lary)|pack (Lary, by rowid)|
||||FCtype.c_pack (Ptr)|
|[acr_ed.FSbpath](#acr_ed-fsbpath)|[dev.sbpath](/txt/ssimdb/dev/sbpath.md)|FDb.sbpath (Lary)|sbpath (Lary, by rowid)|
|[acr_ed.FSsimfile](#acr_ed-fssimfile)|[dmmeta.ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|FDb.ssimfile (Lary)|ssimfile (Lary, by rowid)|ind_ssimfile (Thash, hash field ssimfile)|
||||FCtype.c_ssimfile (Ptr)|
|[acr_ed.FTarget](#acr_ed-ftarget)|[dev.target](/txt/ssimdb/dev/target.md)|FDb.target (Lary)|target (Lary, by rowid)|ind_target (Thash, hash field target)|
||||FTargsrc.p_target (Upptr)|
|[acr_ed.FTargsrc](#acr_ed-ftargsrc)|[dev.targsrc](/txt/ssimdb/dev/targsrc.md)|FDb.targsrc (Lary)|targsrc (Lary, by rowid)|
||||FTarget.zd_targsrc (Llist)|
|[acr_ed.FTypefld](#acr_ed-ftypefld)|[dmmeta.typefld](/txt/ssimdb/dmmeta/typefld.md)|FDb.typefld (Lary)|typefld (Lary, by rowid)|
||||FCtype.c_typefld (Ptr)|

#### acr_ed.FCfmt - Specify options for printing/reading ctypes into multiple formats
<a href="#acr_ed-fcfmt"></a>

#### acr_ed.FCfmt Fields
<a href="#acr_ed-fcfmt-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_ed.FCfmt.base|[dmmeta.Cfmt](/txt/ssimdb/dmmeta/cfmt.md)|[Base](/txt/ssimdb/dmmeta/cfmt.md)|||

#### Struct FCfmt
<a href="#struct-fcfmt"></a>
*Note:* field ``acr_ed.FCfmt.base`` has reftype ``base`` so the fields of [dmmeta.Cfmt](/txt/ssimdb/dmmeta/cfmt.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_ed_gen.h](/include/gen/acr_ed_gen.h)
```
struct FCfmt { // acr_ed.FCfmt
    algo::Smallstr100   cfmt;       //
    algo::Smallstr50    printfmt;   //
    bool                read;       //   false
    bool                print;      //   false
    algo::Smallstr20    sep;        //
    bool                genop;      //   false
    algo::Comment       comment;    //
    // func:acr_ed.FCfmt..AssignOp
    acr_ed::FCfmt&       operator =(const acr_ed::FCfmt &rhs) = delete;
    // func:acr_ed.FCfmt..CopyCtor
    FCfmt(const acr_ed::FCfmt &rhs) = delete;
private:
    // func:acr_ed.FCfmt..Ctor
    inline               FCfmt() __attribute__((nothrow));
    // func:acr_ed.FCfmt..Dtor
    inline               ~FCfmt() __attribute__((nothrow));
    friend acr_ed::FCfmt&       cfmt_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_ed::FCfmt*       cfmt_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 cfmt_RemoveAll() __attribute__((nothrow));
    friend void                 cfmt_RemoveLast() __attribute__((nothrow));
};
```

#### acr_ed.FCpptype - Specify whether a ctype can be passed by value, and other c++ options
<a href="#acr_ed-fcpptype"></a>

#### acr_ed.FCpptype Fields
<a href="#acr_ed-fcpptype-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_ed.FCpptype.base|[dmmeta.Cpptype](/txt/ssimdb/dmmeta/cpptype.md)|[Base](/txt/ssimdb/dmmeta/cpptype.md)|||

#### Struct FCpptype
<a href="#struct-fcpptype"></a>
*Note:* field ``acr_ed.FCpptype.base`` has reftype ``base`` so the fields of [dmmeta.Cpptype](/txt/ssimdb/dmmeta/cpptype.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_ed_gen.h](/include/gen/acr_ed_gen.h)
```
struct FCpptype { // acr_ed.FCpptype
    algo::Smallstr100   ctype;        //
    bool                ctor;         //   false  if true, generate non-default constructor from all fields
    bool                dtor;         //   true  generate non-default destructor
    bool                cheap_copy;   //   false  Pass by value whenever possible
    // func:acr_ed.FCpptype..AssignOp
    inline acr_ed::FCpptype& operator =(const acr_ed::FCpptype &rhs) = delete;
    // func:acr_ed.FCpptype..CopyCtor
    inline               FCpptype(const acr_ed::FCpptype &rhs) = delete;
private:
    // func:acr_ed.FCpptype..Ctor
    inline               FCpptype() __attribute__((nothrow));
    // func:acr_ed.FCpptype..Dtor
    inline               ~FCpptype() __attribute__((nothrow));
    friend acr_ed::FCpptype&    cpptype_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_ed::FCpptype*    cpptype_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 cpptype_RemoveAll() __attribute__((nothrow));
    friend void                 cpptype_RemoveLast() __attribute__((nothrow));
};
```

#### acr_ed.FCstr - Specify that type behaves like a string
<a href="#acr_ed-fcstr"></a>

#### acr_ed.FCstr Fields
<a href="#acr_ed-fcstr-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_ed.FCstr.msghdr|[dmmeta.Cstr](/txt/ssimdb/dmmeta/cstr.md)|[Base](/txt/ssimdb/dmmeta/cstr.md)|||

#### Struct FCstr
<a href="#struct-fcstr"></a>
*Note:* field ``acr_ed.FCstr.msghdr`` has reftype ``base`` so the fields of [dmmeta.Cstr](/txt/ssimdb/dmmeta/cstr.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_ed_gen.h](/include/gen/acr_ed_gen.h)
```
struct FCstr { // acr_ed.FCstr
    algo::Smallstr100   ctype;      //
    bool                strequiv;   //   false  Use strptr instead of this type when possible
    algo::Comment       comment;    //
    // func:acr_ed.FCstr..AssignOp
    inline acr_ed::FCstr& operator =(const acr_ed::FCstr &rhs) = delete;
    // func:acr_ed.FCstr..CopyCtor
    inline               FCstr(const acr_ed::FCstr &rhs) = delete;
private:
    // func:acr_ed.FCstr..Ctor
    inline               FCstr() __attribute__((nothrow));
    // func:acr_ed.FCstr..Dtor
    inline               ~FCstr() __attribute__((nothrow));
    friend acr_ed::FCstr&       cstr_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_ed::FCstr*       cstr_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 cstr_RemoveLast() __attribute__((nothrow));
};
```

#### acr_ed.FCtype - Struct
<a href="#acr_ed-fctype"></a>

#### acr_ed.FCtype Fields
<a href="#acr_ed-fctype-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_ed.FCtype.msghdr|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Base](/txt/ssimdb/dmmeta/ctype.md)|||
|acr_ed.FCtype.c_field|[acr_ed.FField](/txt/exe/acr_ed/internals.md#acr_ed-ffield)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|acr_ed.FCtype.c_cstr|[acr_ed.FCstr](/txt/exe/acr_ed/internals.md#acr_ed-fcstr)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|acr_ed.FCtype.p_ns|[acr_ed.FNs](/txt/exe/acr_ed/internals.md#acr_ed-fns)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|acr_ed.FCtype.c_ssimfile|[acr_ed.FSsimfile](/txt/exe/acr_ed/internals.md#acr_ed-fssimfile)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|acr_ed.FCtype.c_pack|[acr_ed.FPack](/txt/exe/acr_ed/internals.md#acr_ed-fpack)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|acr_ed.FCtype.c_typefld|[acr_ed.FTypefld](/txt/exe/acr_ed/internals.md#acr_ed-ftypefld)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|acr_ed.FCtype.c_cpptype|[acr_ed.FCpptype](/txt/exe/acr_ed/internals.md#acr_ed-fcpptype)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|acr_ed.FCtype.c_cfmt|[acr_ed.FCfmt](/txt/exe/acr_ed/internals.md#acr_ed-fcfmt)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||

#### Struct FCtype
<a href="#struct-fctype"></a>
*Note:* field ``acr_ed.FCtype.msghdr`` has reftype ``base`` so the fields of [dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_ed_gen.h](/include/gen/acr_ed_gen.h)
```
struct FCtype { // acr_ed.FCtype
    algo::Smallstr100    ctype;            // Identifier. must be ns.typename
    algo::Comment        comment;          //
    acr_ed::FField**     c_field_elems;    // array of pointers
    u32                  c_field_n;        // array of pointers
    u32                  c_field_max;      // capacity of allocated array
    acr_ed::FCstr*       c_cstr;           // optional pointer
    acr_ed::FNs*         p_ns;             // reference to parent row
    acr_ed::FSsimfile*   c_ssimfile;       // optional pointer
    acr_ed::FPack*       c_pack;           // optional pointer
    acr_ed::FTypefld*    c_typefld;        // optional pointer
    acr_ed::FCpptype*    c_cpptype;        // optional pointer
    acr_ed::FCfmt**      c_cfmt_elems;     // array of pointers
    u32                  c_cfmt_n;         // array of pointers
    u32                  c_cfmt_max;       // capacity of allocated array
    acr_ed::FCtype*      ind_ctype_next;   // hash next
    // reftype Ptrary of acr_ed.FCtype.c_field prohibits copy
    // x-reference on acr_ed.FCtype.c_cstr prevents copy
    // x-reference on acr_ed.FCtype.p_ns prevents copy
    // x-reference on acr_ed.FCtype.c_ssimfile prevents copy
    // x-reference on acr_ed.FCtype.c_pack prevents copy
    // x-reference on acr_ed.FCtype.c_typefld prevents copy
    // x-reference on acr_ed.FCtype.c_cpptype prevents copy
    // reftype Ptrary of acr_ed.FCtype.c_cfmt prohibits copy
    // func:acr_ed.FCtype..AssignOp
    acr_ed::FCtype&      operator =(const acr_ed::FCtype &rhs) = delete;
    // reftype Ptrary of acr_ed.FCtype.c_field prohibits copy
    // x-reference on acr_ed.FCtype.c_cstr prevents copy
    // x-reference on acr_ed.FCtype.p_ns prevents copy
    // x-reference on acr_ed.FCtype.c_ssimfile prevents copy
    // x-reference on acr_ed.FCtype.c_pack prevents copy
    // x-reference on acr_ed.FCtype.c_typefld prevents copy
    // x-reference on acr_ed.FCtype.c_cpptype prevents copy
    // reftype Ptrary of acr_ed.FCtype.c_cfmt prohibits copy
    // func:acr_ed.FCtype..CopyCtor
    FCtype(const acr_ed::FCtype &rhs) = delete;
private:
    // func:acr_ed.FCtype..Ctor
    inline               FCtype() __attribute__((nothrow));
    // func:acr_ed.FCtype..Dtor
    inline               ~FCtype() __attribute__((nothrow));
    friend acr_ed::FCtype&      ctype_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_ed::FCtype*      ctype_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 ctype_RemoveLast() __attribute__((nothrow));
};
```

#### acr_ed.FEdaction - acr_ed editing action (only one can exist at a time)
<a href="#acr_ed-fedaction"></a>

#### acr_ed.FEdaction Fields
<a href="#acr_ed-fedaction-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_ed.FEdaction.base|[dev.Edaction](/txt/ssimdb/dev/edaction.md)|[Base](/txt/ssimdb/dev/edaction.md)|||
|acr_ed.FEdaction.step||[Hook](/txt/exe/amc/reftypes.md#hook)|||
|acr_ed.FEdaction.select|bool|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FEdaction
<a href="#struct-fedaction"></a>
*Note:* field ``acr_ed.FEdaction.base`` has reftype ``base`` so the fields of [dev.Edaction](/txt/ssimdb/dev/edaction.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_ed_gen.h](/include/gen/acr_ed_gen.h)
```
struct FEdaction { // acr_ed.FEdaction
    algo::Smallstr50             edaction;            //
    bool                         needamc;             //   false
    algo::Comment                comment;             //
    acr_ed::edaction_step_hook   step;                //   NULL  Pointer to a function
    bool                         select;              //   false
    acr_ed::FEdaction*           ind_edaction_next;   // hash next
    // reftype Hook of acr_ed.FEdaction.step prohibits copy
    // func:acr_ed.FEdaction..AssignOp
    inline acr_ed::FEdaction& operator =(const acr_ed::FEdaction &rhs) = delete;
    // func:acr_ed.FEdaction..Ctor
    inline               FEdaction() __attribute__((nothrow));
    // func:acr_ed.FEdaction..Dtor
    inline               ~FEdaction() __attribute__((nothrow));
    // reftype Hook of acr_ed.FEdaction.step prohibits copy
    // func:acr_ed.FEdaction..CopyCtor
    inline               FEdaction(const acr_ed::FEdaction &rhs) = delete;
};
```

#### acr_ed.FDb - In-memory database for acr_ed
<a href="#acr_ed-fdb"></a>

#### acr_ed.FDb Fields
<a href="#acr_ed-fdb-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_ed.FDb.ns|[acr_ed.FNs](/txt/exe/acr_ed/internals.md#acr_ed-fns)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr_ed.FDb.ind_ns|[acr_ed.FNs](/txt/exe/acr_ed/internals.md#acr_ed-fns)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|acr_ed.FDb._db|[acr_ed.FDb](/txt/exe/acr_ed/internals.md#acr_ed-fdb)|[Global](/txt/exe/amc/reftypes.md#global)|||
|acr_ed.FDb.cmdline|[command.acr_ed](/txt/protocol/command/README.md#command-acr_ed)|[Val](/txt/exe/amc/reftypes.md#val)|||
|acr_ed.FDb.field|[acr_ed.FField](/txt/exe/acr_ed/internals.md#acr_ed-ffield)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr_ed.FDb.ind_field|[acr_ed.FField](/txt/exe/acr_ed/internals.md#acr_ed-ffield)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|acr_ed.FDb.ctype|[acr_ed.FCtype](/txt/exe/acr_ed/internals.md#acr_ed-fctype)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr_ed.FDb.fcontents|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Optional file contents|
|acr_ed.FDb.script|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Script to execute|
|acr_ed.FDb.out_ssim|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Input for acr command|
|acr_ed.FDb.ind_ctype|[acr_ed.FCtype](/txt/exe/acr_ed/internals.md#acr_ed-fctype)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|acr_ed.FDb.ssimfile|[acr_ed.FSsimfile](/txt/exe/acr_ed/internals.md#acr_ed-fssimfile)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr_ed.FDb.ind_ssimfile|[acr_ed.FSsimfile](/txt/exe/acr_ed/internals.md#acr_ed-fssimfile)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|acr_ed.FDb.cstr|[acr_ed.FCstr](/txt/exe/acr_ed/internals.md#acr_ed-fcstr)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr_ed.FDb.vis|[algo.cstring](/txt/protocol/algo/cstring.md)|[Tary](/txt/exe/amc/reftypes.md#tary)|||
|acr_ed.FDb.listtype|[acr_ed.FListtype](/txt/exe/acr_ed/internals.md#acr_ed-flisttype)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr_ed.FDb.ind_listtype|[acr_ed.FListtype](/txt/exe/acr_ed/internals.md#acr_ed-flisttype)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|acr_ed.FDb.fprefix|[acr_ed.FFprefix](/txt/exe/acr_ed/internals.md#acr_ed-ffprefix)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr_ed.FDb.ind_fprefix|[acr_ed.FFprefix](/txt/exe/acr_ed/internals.md#acr_ed-ffprefix)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|acr_ed.FDb.target|[acr_ed.FTarget](/txt/exe/acr_ed/internals.md#acr_ed-ftarget)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr_ed.FDb.ind_target|[acr_ed.FTarget](/txt/exe/acr_ed/internals.md#acr_ed-ftarget)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|acr_ed.FDb.keyfld|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|||
|acr_ed.FDb.viafld|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|||
|acr_ed.FDb.need_amc|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|acr_ed.FDb.targsrc|[acr_ed.FTargsrc](/txt/exe/acr_ed/internals.md#acr_ed-ftargsrc)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr_ed.FDb.abt|[command.abt](/txt/protocol/command/README.md#command-abt)|[Exec](/txt/exe/amc/reftypes.md#exec)|||
|acr_ed.FDb.sbpath|[acr_ed.FSbpath](/txt/exe/acr_ed/internals.md#acr_ed-fsbpath)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr_ed.FDb.pack|[acr_ed.FPack](/txt/exe/acr_ed/internals.md#acr_ed-fpack)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr_ed.FDb.typefld|[acr_ed.FTypefld](/txt/exe/acr_ed/internals.md#acr_ed-ftypefld)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr_ed.FDb.cpptype|[acr_ed.FCpptype](/txt/exe/acr_ed/internals.md#acr_ed-fcpptype)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr_ed.FDb.cfmt|[acr_ed.FCfmt](/txt/exe/acr_ed/internals.md#acr_ed-fcfmt)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr_ed.FDb.nsdb|[acr_ed.FNsdb](/txt/exe/acr_ed/internals.md#acr_ed-fnsdb)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|acr_ed.FDb.ind_nsdb|[acr_ed.FNsdb](/txt/exe/acr_ed/internals.md#acr_ed-fnsdb)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|acr_ed.FDb.could_be_ptr|bool|[Val](/txt/exe/amc/reftypes.md#val)||Used to automatically guess Ptr or Ptrary reftype on new fields|
|acr_ed.FDb.edaction|[acr_ed.FEdaction](/txt/exe/acr_ed/internals.md#acr_ed-fedaction)|[Inlary](/txt/exe/amc/reftypes.md#inlary)|||
|acr_ed.FDb.ind_edaction|[acr_ed.FEdaction](/txt/exe/acr_ed/internals.md#acr_ed-fedaction)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|acr_ed.FDb.gitfile|[acr_ed.FGitfile](/txt/exe/acr_ed/internals.md#acr_ed-fgitfile)|[Lary](/txt/exe/amc/reftypes.md#lary)|||

#### Struct FDb
<a href="#struct-fdb"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_ed_gen.h](/include/gen/acr_ed_gen.h)
```
struct FDb { // acr_ed.FDb: In-memory database for acr_ed
    acr_ed::FNs*          ns_lary[32];                                     // level array
    i32                   ns_n;                                            // number of elements in array
    acr_ed::FNs**         ind_ns_buckets_elems;                            // pointer to bucket array
    i32                   ind_ns_buckets_n;                                // number of elements in bucket array
    i32                   ind_ns_n;                                        // number of elements in the hash table
    command::acr_ed       cmdline;                                         //
    acr_ed::FField*       field_lary[32];                                  // level array
    i32                   field_n;                                         // number of elements in array
    acr_ed::FField**      ind_field_buckets_elems;                         // pointer to bucket array
    i32                   ind_field_buckets_n;                             // number of elements in bucket array
    i32                   ind_field_n;                                     // number of elements in the hash table
    acr_ed::FCtype*       ctype_lary[32];                                  // level array
    i32                   ctype_n;                                         // number of elements in array
    algo::cstring         fcontents;                                       // Optional file contents
    algo::cstring         script;                                          // Script to execute
    algo::cstring         out_ssim;                                        // Input for acr command
    acr_ed::FCtype**      ind_ctype_buckets_elems;                         // pointer to bucket array
    i32                   ind_ctype_buckets_n;                             // number of elements in bucket array
    i32                   ind_ctype_n;                                     // number of elements in the hash table
    acr_ed::FSsimfile*    ssimfile_lary[32];                               // level array
    i32                   ssimfile_n;                                      // number of elements in array
    acr_ed::FSsimfile**   ind_ssimfile_buckets_elems;                      // pointer to bucket array
    i32                   ind_ssimfile_buckets_n;                          // number of elements in bucket array
    i32                   ind_ssimfile_n;                                  // number of elements in the hash table
    acr_ed::FCstr*        cstr_lary[32];                                   // level array
    i32                   cstr_n;                                          // number of elements in array
    algo::cstring*        vis_elems;                                       // pointer to elements
    u32                   vis_n;                                           // number of elements in array
    u32                   vis_max;                                         // max. capacity of array before realloc
    acr_ed::FListtype*    listtype_lary[32];                               // level array
    i32                   listtype_n;                                      // number of elements in array
    acr_ed::FListtype**   ind_listtype_buckets_elems;                      // pointer to bucket array
    i32                   ind_listtype_buckets_n;                          // number of elements in bucket array
    i32                   ind_listtype_n;                                  // number of elements in the hash table
    acr_ed::FFprefix*     fprefix_lary[32];                                // level array
    i32                   fprefix_n;                                       // number of elements in array
    acr_ed::FFprefix**    ind_fprefix_buckets_elems;                       // pointer to bucket array
    i32                   ind_fprefix_buckets_n;                           // number of elements in bucket array
    i32                   ind_fprefix_n;                                   // number of elements in the hash table
    acr_ed::FTarget*      target_lary[32];                                 // level array
    i32                   target_n;                                        // number of elements in array
    acr_ed::FTarget**     ind_target_buckets_elems;                        // pointer to bucket array
    i32                   ind_target_buckets_n;                            // number of elements in bucket array
    i32                   ind_target_n;                                    // number of elements in the hash table
    algo::Smallstr100     keyfld;                                          //
    algo::Smallstr100     viafld;                                          //
    bool                  need_amc;                                        //   false
    acr_ed::FTargsrc*     targsrc_lary[32];                                // level array
    i32                   targsrc_n;                                       // number of elements in array
    algo::cstring         abt_path;                                        //   "bin/abt"  path for executable
    command::abt          abt_cmd;                                         // command line for child process
    algo::cstring         abt_fstdin;                                      // redirect for stdin
    algo::cstring         abt_fstdout;                                     // redirect for stdout
    algo::cstring         abt_fstderr;                                     // redirect for stderr
    pid_t                 abt_pid;                                         //   0  pid of running child process
    i32                   abt_timeout;                                     //   0  optional timeout for child process
    i32                   abt_status;                                      //   0  last exit status of child process
    acr_ed::FSbpath*      sbpath_lary[32];                                 // level array
    i32                   sbpath_n;                                        // number of elements in array
    acr_ed::FPack*        pack_lary[32];                                   // level array
    i32                   pack_n;                                          // number of elements in array
    acr_ed::FTypefld*     typefld_lary[32];                                // level array
    i32                   typefld_n;                                       // number of elements in array
    acr_ed::FCpptype*     cpptype_lary[32];                                // level array
    i32                   cpptype_n;                                       // number of elements in array
    acr_ed::FCfmt*        cfmt_lary[32];                                   // level array
    i32                   cfmt_n;                                          // number of elements in array
    acr_ed::FNsdb*        nsdb_lary[32];                                   // level array
    i32                   nsdb_n;                                          // number of elements in array
    acr_ed::FNsdb**       ind_nsdb_buckets_elems;                          // pointer to bucket array
    i32                   ind_nsdb_buckets_n;                              // number of elements in bucket array
    i32                   ind_nsdb_n;                                      // number of elements in the hash table
    bool                  could_be_ptr;                                    //   false  Used to automatically guess Ptr or Ptrary reftype on new fields
    u128                  edaction_data[sizeu128(acr_ed::FEdaction,18)];   // place for data
    i32                   edaction_n;                                      // number of elems current in existence
    enum { edaction_max = 18 };
    acr_ed::FEdaction**   ind_edaction_buckets_elems;                      // pointer to bucket array
    i32                   ind_edaction_buckets_n;                          // number of elements in bucket array
    i32                   ind_edaction_n;                                  // number of elements in the hash table
    acr_ed::FGitfile*     gitfile_lary[32];                                // level array
    i32                   gitfile_n;                                       // number of elements in array
    acr_ed::trace         trace;                                           //
};
```

#### acr_ed.FField - Specify field of a struct
<a href="#acr_ed-ffield"></a>

#### acr_ed.FField Fields
<a href="#acr_ed-ffield-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_ed.FField.msghdr|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Base](/txt/ssimdb/dmmeta/field.md)|||
|acr_ed.FField.p_ctype|[acr_ed.FCtype](/txt/exe/acr_ed/internals.md#acr_ed-fctype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|acr_ed.FField.p_arg|[acr_ed.FCtype](/txt/exe/acr_ed/internals.md#acr_ed-fctype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|acr_ed.FField.p_ns|[acr_ed.FNs](/txt/exe/acr_ed/internals.md#acr_ed-fns)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FField
<a href="#struct-ffield"></a>
*Note:* field ``acr_ed.FField.msghdr`` has reftype ``base`` so the fields of [dmmeta.Field](/txt/ssimdb/dmmeta/field.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_ed_gen.h](/include/gen/acr_ed_gen.h)
```
struct FField { // acr_ed.FField
    acr_ed::FField*     ind_field_next;         // hash next
    algo::Smallstr100   field;                  // Primary key, as ctype.name
    algo::Smallstr100   arg;                    // Type of field
    algo::Smallstr50    reftype;                //   "Val"  Type constructor
    algo::CppExpr       dflt;                   // Default value (c++ expression)
    algo::Comment       comment;                //
    acr_ed::FCtype*     p_ctype;                // reference to parent row
    acr_ed::FCtype*     p_arg;                  // reference to parent row
    acr_ed::FNs*        p_ns;                   // reference to parent row
    bool                ctype_c_field_in_ary;   //   false  membership flag
    // x-reference on acr_ed.FField.p_ctype prevents copy
    // x-reference on acr_ed.FField.p_arg prevents copy
    // x-reference on acr_ed.FField.p_ns prevents copy
    // func:acr_ed.FField..AssignOp
    acr_ed::FField&      operator =(const acr_ed::FField &rhs) = delete;
    // x-reference on acr_ed.FField.p_ctype prevents copy
    // x-reference on acr_ed.FField.p_arg prevents copy
    // x-reference on acr_ed.FField.p_ns prevents copy
    // func:acr_ed.FField..CopyCtor
    FField(const acr_ed::FField &rhs) = delete;
private:
    // func:acr_ed.FField..Ctor
    inline               FField() __attribute__((nothrow));
    // func:acr_ed.FField..Dtor
    inline               ~FField() __attribute__((nothrow));
    friend acr_ed::FField&      field_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_ed::FField*      field_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 field_RemoveLast() __attribute__((nothrow));
};
```

#### acr_ed.FFprefix - Mapping between field prefix and Reftype
<a href="#acr_ed-ffprefix"></a>

#### acr_ed.FFprefix Fields
<a href="#acr_ed-ffprefix-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_ed.FFprefix.base|[dmmeta.Fprefix](/txt/ssimdb/dmmeta/fprefix.md)|[Base](/txt/ssimdb/dmmeta/fprefix.md)|||

#### Struct FFprefix
<a href="#struct-ffprefix"></a>
*Note:* field ``acr_ed.FFprefix.base`` has reftype ``base`` so the fields of [dmmeta.Fprefix](/txt/ssimdb/dmmeta/fprefix.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_ed_gen.h](/include/gen/acr_ed_gen.h)
```
struct FFprefix { // acr_ed.FFprefix
    acr_ed::FFprefix*   ind_fprefix_next;   // hash next
    algo::Smallstr5     fprefix;            //
    algo::Smallstr50    reftype;            //
    algo::Comment       comment;            //
    // func:acr_ed.FFprefix..AssignOp
    inline acr_ed::FFprefix& operator =(const acr_ed::FFprefix &rhs) = delete;
    // func:acr_ed.FFprefix..CopyCtor
    inline               FFprefix(const acr_ed::FFprefix &rhs) = delete;
private:
    // func:acr_ed.FFprefix..Ctor
    inline               FFprefix() __attribute__((nothrow));
    // func:acr_ed.FFprefix..Dtor
    inline               ~FFprefix() __attribute__((nothrow));
    friend acr_ed::FFprefix&    fprefix_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_ed::FFprefix*    fprefix_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 fprefix_RemoveLast() __attribute__((nothrow));
};
```

#### acr_ed.FGitfile - File managed by git
<a href="#acr_ed-fgitfile"></a>

#### acr_ed.FGitfile Fields
<a href="#acr_ed-fgitfile-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_ed.FGitfile.base|[dev.Gitfile](/txt/ssimdb/dev/gitfile.md)|[Base](/txt/ssimdb/dev/gitfile.md)|||

#### Struct FGitfile
<a href="#struct-fgitfile"></a>
*Note:* field ``acr_ed.FGitfile.base`` has reftype ``base`` so the fields of [dev.Gitfile](/txt/ssimdb/dev/gitfile.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_ed_gen.h](/include/gen/acr_ed_gen.h)
```
struct FGitfile { // acr_ed.FGitfile
    algo::Smallstr200   gitfile;   //
private:
    // func:acr_ed.FGitfile..Ctor
    inline               FGitfile() __attribute__((nothrow));
    friend acr_ed::FGitfile&    gitfile_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_ed::FGitfile*    gitfile_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 gitfile_RemoveAll() __attribute__((nothrow));
    friend void                 gitfile_RemoveLast() __attribute__((nothrow));
};
```

#### acr_ed.FListtype - Specify structure of linked list based on field prefix
<a href="#acr_ed-flisttype"></a>

#### acr_ed.FListtype Fields
<a href="#acr_ed-flisttype-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_ed.FListtype.msghdr|[dmmeta.Listtype](/txt/ssimdb/dmmeta/listtype.md)|[Base](/txt/ssimdb/dmmeta/listtype.md)|||

#### Struct FListtype
<a href="#struct-flisttype"></a>
*Note:* field ``acr_ed.FListtype.msghdr`` has reftype ``base`` so the fields of [dmmeta.Listtype](/txt/ssimdb/dmmeta/listtype.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_ed_gen.h](/include/gen/acr_ed_gen.h)
```
struct FListtype { // acr_ed.FListtype
    acr_ed::FListtype*   ind_listtype_next;   // hash next
    algo::Smallstr5      listtype;            //
    bool                 circular;            //   false  Circular list
    bool                 haveprev;            //   false  Previous link
    bool                 instail;             //   false  Queue
    algo::Comment        comment;             //
    // func:acr_ed.FListtype..AssignOp
    inline acr_ed::FListtype& operator =(const acr_ed::FListtype &rhs) = delete;
    // func:acr_ed.FListtype..CopyCtor
    inline               FListtype(const acr_ed::FListtype &rhs) = delete;
private:
    // func:acr_ed.FListtype..Ctor
    inline               FListtype() __attribute__((nothrow));
    // func:acr_ed.FListtype..Dtor
    inline               ~FListtype() __attribute__((nothrow));
    friend acr_ed::FListtype&   listtype_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_ed::FListtype*   listtype_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 listtype_RemoveLast() __attribute__((nothrow));
};
```

#### acr_ed.FNs - Namespace (for in-memory database, protocol, etc)
<a href="#acr_ed-fns"></a>

#### acr_ed.FNs Fields
<a href="#acr_ed-fns-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_ed.FNs.msghdr|[dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md)|[Base](/txt/ssimdb/dmmeta/ns.md)|||

#### Struct FNs
<a href="#struct-fns"></a>
*Note:* field ``acr_ed.FNs.msghdr`` has reftype ``base`` so the fields of [dmmeta.Ns](/txt/ssimdb/dmmeta/ns.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_ed_gen.h](/include/gen/acr_ed_gen.h)
```
struct FNs { // acr_ed.FNs
    acr_ed::FNs*       ind_ns_next;   // hash next
    algo::Smallstr16   ns;            // Namespace name (primary key)
    algo::Smallstr50   nstype;        // Namespace type
    algo::Smallstr50   license;       // Associated license
    algo::Comment      comment;       //
    // func:acr_ed.FNs..AssignOp
    inline acr_ed::FNs&  operator =(const acr_ed::FNs &rhs) = delete;
    // func:acr_ed.FNs..CopyCtor
    inline               FNs(const acr_ed::FNs &rhs) = delete;
private:
    // func:acr_ed.FNs..Ctor
    inline               FNs() __attribute__((nothrow));
    // func:acr_ed.FNs..Dtor
    inline               ~FNs() __attribute__((nothrow));
    friend acr_ed::FNs&         ns_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_ed::FNs*         ns_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 ns_RemoveLast() __attribute__((nothrow));
};
```

#### acr_ed.FNsdb - Annotate ssimdb namespaces
<a href="#acr_ed-fnsdb"></a>

#### acr_ed.FNsdb Fields
<a href="#acr_ed-fnsdb-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_ed.FNsdb.base|[dmmeta.Nsdb](/txt/ssimdb/dmmeta/nsdb.md)|[Base](/txt/ssimdb/dmmeta/nsdb.md)|||

#### Struct FNsdb
<a href="#struct-fnsdb"></a>
*Note:* field ``acr_ed.FNsdb.base`` has reftype ``base`` so the fields of [dmmeta.Nsdb](/txt/ssimdb/dmmeta/nsdb.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_ed_gen.h](/include/gen/acr_ed_gen.h)
```
struct FNsdb { // acr_ed.FNsdb
    acr_ed::FNsdb*     ind_nsdb_next;   // hash next
    algo::Smallstr16   ns;              //
    algo::Comment      comment;         //
    // func:acr_ed.FNsdb..AssignOp
    inline acr_ed::FNsdb& operator =(const acr_ed::FNsdb &rhs) = delete;
    // func:acr_ed.FNsdb..CopyCtor
    inline               FNsdb(const acr_ed::FNsdb &rhs) = delete;
private:
    // func:acr_ed.FNsdb..Ctor
    inline               FNsdb() __attribute__((nothrow));
    // func:acr_ed.FNsdb..Dtor
    inline               ~FNsdb() __attribute__((nothrow));
    friend acr_ed::FNsdb&       nsdb_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_ed::FNsdb*       nsdb_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 nsdb_RemoveAll() __attribute__((nothrow));
    friend void                 nsdb_RemoveLast() __attribute__((nothrow));
};
```

#### acr_ed.FPack - Request byte-packing of structure fields
<a href="#acr_ed-fpack"></a>

#### acr_ed.FPack Fields
<a href="#acr_ed-fpack-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_ed.FPack.base|[dmmeta.Pack](/txt/ssimdb/dmmeta/pack.md)|[Base](/txt/ssimdb/dmmeta/pack.md)|||

#### Struct FPack
<a href="#struct-fpack"></a>
*Note:* field ``acr_ed.FPack.base`` has reftype ``base`` so the fields of [dmmeta.Pack](/txt/ssimdb/dmmeta/pack.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_ed_gen.h](/include/gen/acr_ed_gen.h)
```
struct FPack { // acr_ed.FPack
    algo::Smallstr100   ctype;     // Target ctype
    algo::Comment       comment;   //
    // func:acr_ed.FPack..AssignOp
    inline acr_ed::FPack& operator =(const acr_ed::FPack &rhs) = delete;
    // func:acr_ed.FPack..CopyCtor
    inline               FPack(const acr_ed::FPack &rhs) = delete;
private:
    // func:acr_ed.FPack..Ctor
    inline               FPack() __attribute__((nothrow));
    // func:acr_ed.FPack..Dtor
    inline               ~FPack() __attribute__((nothrow));
    friend acr_ed::FPack&       pack_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_ed::FPack*       pack_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 pack_RemoveAll() __attribute__((nothrow));
    friend void                 pack_RemoveLast() __attribute__((nothrow));
};
```

#### acr_ed.FSbpath - Extra files to copy into the sandbox
<a href="#acr_ed-fsbpath"></a>

#### acr_ed.FSbpath Fields
<a href="#acr_ed-fsbpath-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_ed.FSbpath.base|[dev.Sbpath](/txt/ssimdb/dev/sbpath.md)|[Base](/txt/ssimdb/dev/sbpath.md)|||

#### Struct FSbpath
<a href="#struct-fsbpath"></a>
*Note:* field ``acr_ed.FSbpath.base`` has reftype ``base`` so the fields of [dev.Sbpath](/txt/ssimdb/dev/sbpath.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_ed_gen.h](/include/gen/acr_ed_gen.h)
```
struct FSbpath { // acr_ed.FSbpath
    algo::Smallstr50   sbpath;    //
    algo::Comment      comment;   //
private:
    // func:acr_ed.FSbpath..Ctor
    inline               FSbpath() __attribute__((nothrow));
    friend acr_ed::FSbpath&     sbpath_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_ed::FSbpath*     sbpath_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 sbpath_RemoveAll() __attribute__((nothrow));
    friend void                 sbpath_RemoveLast() __attribute__((nothrow));
};
```

#### acr_ed.FSsimfile - File with ssim tuples
<a href="#acr_ed-fssimfile"></a>

#### acr_ed.FSsimfile Fields
<a href="#acr_ed-fssimfile-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_ed.FSsimfile.msghdr|[dmmeta.Ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|[Base](/txt/ssimdb/dmmeta/ssimfile.md)|||
|acr_ed.FSsimfile.p_ctype|[acr_ed.FCtype](/txt/exe/acr_ed/internals.md#acr_ed-fctype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|acr_ed.FSsimfile.p_ns|[acr_ed.FNs](/txt/exe/acr_ed/internals.md#acr_ed-fns)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FSsimfile
<a href="#struct-fssimfile"></a>
*Note:* field ``acr_ed.FSsimfile.msghdr`` has reftype ``base`` so the fields of [dmmeta.Ssimfile](/txt/ssimdb/dmmeta/ssimfile.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_ed_gen.h](/include/gen/acr_ed_gen.h)
```
struct FSsimfile { // acr_ed.FSsimfile
    acr_ed::FSsimfile*   ind_ssimfile_next;   // hash next
    algo::Smallstr50     ssimfile;            //
    algo::Smallstr100    ctype;               //
    acr_ed::FCtype*      p_ctype;             // reference to parent row
    acr_ed::FNs*         p_ns;                // reference to parent row
    // x-reference on acr_ed.FSsimfile.p_ctype prevents copy
    // x-reference on acr_ed.FSsimfile.p_ns prevents copy
    // func:acr_ed.FSsimfile..AssignOp
    inline acr_ed::FSsimfile& operator =(const acr_ed::FSsimfile &rhs) = delete;
    // x-reference on acr_ed.FSsimfile.p_ctype prevents copy
    // x-reference on acr_ed.FSsimfile.p_ns prevents copy
    // func:acr_ed.FSsimfile..CopyCtor
    inline               FSsimfile(const acr_ed::FSsimfile &rhs) = delete;
private:
    // func:acr_ed.FSsimfile..Ctor
    inline               FSsimfile() __attribute__((nothrow));
    // func:acr_ed.FSsimfile..Dtor
    inline               ~FSsimfile() __attribute__((nothrow));
    friend acr_ed::FSsimfile&   ssimfile_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_ed::FSsimfile*   ssimfile_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 ssimfile_RemoveLast() __attribute__((nothrow));
};
```

#### acr_ed.FTarget - Build target
<a href="#acr_ed-ftarget"></a>

#### acr_ed.FTarget Fields
<a href="#acr_ed-ftarget-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_ed.FTarget.base|[dev.Target](/txt/ssimdb/dev/target.md)|[Base](/txt/ssimdb/dev/target.md)|||
|acr_ed.FTarget.score|u32|[Val](/txt/exe/amc/reftypes.md#val)||For guessing target from source file|
|acr_ed.FTarget.zd_targsrc|[acr_ed.FTargsrc](/txt/exe/acr_ed/internals.md#acr_ed-ftargsrc)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|acr_ed.FTarget.p_ns|[acr_ed.FNs](/txt/exe/acr_ed/internals.md#acr_ed-fns)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FTarget
<a href="#struct-ftarget"></a>
*Note:* field ``acr_ed.FTarget.base`` has reftype ``base`` so the fields of [dev.Target](/txt/ssimdb/dev/target.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_ed_gen.h](/include/gen/acr_ed_gen.h)
```
struct FTarget { // acr_ed.FTarget
    acr_ed::FTarget*    ind_target_next;   // hash next
    algo::Smallstr16    target;            // Primary key - name of target
    u32                 score;             //   0  For guessing target from source file
    acr_ed::FTargsrc*   zd_targsrc_head;   // zero-terminated doubly linked list
    i32                 zd_targsrc_n;      // zero-terminated doubly linked list
    acr_ed::FTargsrc*   zd_targsrc_tail;   // pointer to last element
    acr_ed::FNs*        p_ns;              // reference to parent row
    // reftype Llist of acr_ed.FTarget.zd_targsrc prohibits copy
    // x-reference on acr_ed.FTarget.p_ns prevents copy
    // func:acr_ed.FTarget..AssignOp
    inline acr_ed::FTarget& operator =(const acr_ed::FTarget &rhs) = delete;
    // reftype Llist of acr_ed.FTarget.zd_targsrc prohibits copy
    // x-reference on acr_ed.FTarget.p_ns prevents copy
    // func:acr_ed.FTarget..CopyCtor
    inline               FTarget(const acr_ed::FTarget &rhs) = delete;
private:
    // func:acr_ed.FTarget..Ctor
    inline               FTarget() __attribute__((nothrow));
    // func:acr_ed.FTarget..Dtor
    inline               ~FTarget() __attribute__((nothrow));
    friend acr_ed::FTarget&     target_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_ed::FTarget*     target_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 target_RemoveLast() __attribute__((nothrow));
};
```

#### acr_ed.FTargsrc - List of sources for target
<a href="#acr_ed-ftargsrc"></a>

#### acr_ed.FTargsrc Fields
<a href="#acr_ed-ftargsrc-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_ed.FTargsrc.base|[dev.Targsrc](/txt/ssimdb/dev/targsrc.md)|[Base](/txt/ssimdb/dev/targsrc.md)|||
|acr_ed.FTargsrc.p_target|[acr_ed.FTarget](/txt/exe/acr_ed/internals.md#acr_ed-ftarget)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FTargsrc
<a href="#struct-ftargsrc"></a>
*Note:* field ``acr_ed.FTargsrc.base`` has reftype ``base`` so the fields of [dev.Targsrc](/txt/ssimdb/dev/targsrc.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_ed_gen.h](/include/gen/acr_ed_gen.h)
```
struct FTargsrc { // acr_ed.FTargsrc
    acr_ed::FTargsrc*   zd_targsrc_next;   // zslist link; -1 means not-in-list
    acr_ed::FTargsrc*   zd_targsrc_prev;   // previous element
    algo::Smallstr100   targsrc;           //
    algo::Comment       comment;           //
    acr_ed::FTarget*    p_target;          // reference to parent row
    // x-reference on acr_ed.FTargsrc.p_target prevents copy
    // func:acr_ed.FTargsrc..AssignOp
    inline acr_ed::FTargsrc& operator =(const acr_ed::FTargsrc &rhs) = delete;
    // x-reference on acr_ed.FTargsrc.p_target prevents copy
    // func:acr_ed.FTargsrc..CopyCtor
    inline               FTargsrc(const acr_ed::FTargsrc &rhs) = delete;
private:
    // func:acr_ed.FTargsrc..Ctor
    inline               FTargsrc() __attribute__((nothrow));
    // func:acr_ed.FTargsrc..Dtor
    inline               ~FTargsrc() __attribute__((nothrow));
    friend acr_ed::FTargsrc&    targsrc_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_ed::FTargsrc*    targsrc_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 targsrc_RemoveAll() __attribute__((nothrow));
    friend void                 targsrc_RemoveLast() __attribute__((nothrow));
};
```

#### acr_ed.FTypefld - Specifies which field of a message carries the type
<a href="#acr_ed-ftypefld"></a>

#### acr_ed.FTypefld Fields
<a href="#acr_ed-ftypefld-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_ed.FTypefld.base|[dmmeta.Typefld](/txt/ssimdb/dmmeta/typefld.md)|[Base](/txt/ssimdb/dmmeta/typefld.md)|||

#### Struct FTypefld
<a href="#struct-ftypefld"></a>
*Note:* field ``acr_ed.FTypefld.base`` has reftype ``base`` so the fields of [dmmeta.Typefld](/txt/ssimdb/dmmeta/typefld.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_ed_gen.h](/include/gen/acr_ed_gen.h)
```
struct FTypefld { // acr_ed.FTypefld
    algo::Smallstr100   field;     //
    algo::Comment       comment;   //
    // func:acr_ed.FTypefld..AssignOp
    inline acr_ed::FTypefld& operator =(const acr_ed::FTypefld &rhs) = delete;
    // func:acr_ed.FTypefld..CopyCtor
    inline               FTypefld(const acr_ed::FTypefld &rhs) = delete;
private:
    // func:acr_ed.FTypefld..Ctor
    inline               FTypefld() __attribute__((nothrow));
    // func:acr_ed.FTypefld..Dtor
    inline               ~FTypefld() __attribute__((nothrow));
    friend acr_ed::FTypefld&    typefld_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_ed::FTypefld*    typefld_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 typefld_RemoveAll() __attribute__((nothrow));
    friend void                 typefld_RemoveLast() __attribute__((nothrow));
};
```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Imdb -->

### Tests
<a href="#tests"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Tests -->
The following component tests are defined for `acr_ed`.
These can be executed with `atf_comp <comptest> -v`
|Comptest|Comment|
|---|---|
|[acr_ed.CreateCtype](/test/atf_comp/acr_ed.CreateCtype)|Create a regular ctype|
|[acr_ed.CreateSrcfileTarget](/test/atf_comp/acr_ed.CreateSrcfileTarget)|Create a source file in a non-standard location for its target|
|[acr_ed.CreateSsimfileBadNs](/test/atf_comp/acr_ed.CreateSsimfileBadNs)|Create a ssimfile for a non-existence namespace|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Tests -->


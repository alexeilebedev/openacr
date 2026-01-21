## lib_ctype - Helpful library for reading/writing ctypes as text tuples


### Table Of Contents
<a href="#table-of-contents"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Toc -->
&nbsp;&nbsp;&bull;&nbsp;  [Functions](#functions)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Inputs](#inputs)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Sources](#sources)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [Dependencies](#dependencies)<br/>
&nbsp;&nbsp;&bull;&nbsp;  [In Memory DB](#in-memory-db)<br/>

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Toc -->

### Functions
<a href="#functions"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Functions -->
Functions exported from this namespace:

```c++
// Retrieve base type for the given ctype
lib_ctype::FCtype *lib_ctype::Basetype(lib_ctype::FCtype &ctype) 
```

```c++
// Check if specified attribute matches the default value for the field.
bool lib_ctype::EqualDefaultQ(algo::Attr *attr, lib_ctype::FField &field) 
```

```c++
// Fill Replscope with attribute values (including substrings) of tuple
// So, if STR is dev.targdep targdep:a.b comment:"blah"
// Then R will be filled with variables $targdep, $target, $parent, $comment
// This can be used to perform $-substitution in strings.
void lib_ctype::FillReplscope(algo_lib::Replscope &R, algo::Tuple &tuple) 
```

```c++
// Same as tuple version of FillReplscope, but also parses the tuple from STR
void lib_ctype::FillReplscope(algo_lib::Replscope &R, strptr str) 
```

```c++
// Find fconst from field and ssim attr value.
lib_ctype::FFconst *lib_ctype::FindFconst(lib_ctype::FField *field, cstring& attr_value) 
```

```c++
// Find field by name, possibly scanning base fields
// Skips dot-suffix first, i.e. attr "abc.4" is looked up as "abc" (used for Varlen fields)
lib_ctype::FField *lib_ctype::FindField(lib_ctype::FCtype &ctype, strptr name) 
```

```c++
// Prepare main schema
void lib_ctype::Init() 
```

```c++
// Match Tuple EXPECT against tuple RESULT.
// EXPECT must be a subset of RESULT for the match to pass.
// Results of comparison, mostly represented as a match distance, are saved in MATCH.
// Search exits early if match.distance exceeds match.maxdist.
void lib_ctype::Match_Tuple(lib_ctype::Match &match, Tuple &expect, Tuple &result) 
```

```c++
// Normalize a string that's supposed to correspond CTYPE
// If CTYPE is NULL, it is guessed from the type tag
// The following actions are done:
// 1. Print correct type tag back
// 2. Remove any attributes from tuple that don't correspond to fields of CTYPE
// 3. Print back any fields in ctype that don't appear in the string
// 4. Optionally skip printing any field that's already equal to the default value (if SKIP_DFLT is specified)
// 5. Recursively call NormalizeTuple on any field that has Tuple print format
// NOTE:
// Ctype from parent field has been removed, as it is wrong for derived types!
tempstr lib_ctype::NormalizeSsimTuple(strptr str, bool skip_dflt) 
```

```c++
// Print attributes from TUPLE,
// which is known to be of ctype CTYPE, to cstring TEXT.
// Output fields in normalized order, respecting Base.
// If SKIP_DFLT is true, do not print fields which happen to match their default.
// Suports Varlen fields.
// Attributes that are themselves tuples are recursively normalized as well.
void lib_ctype::PrintTupleAttrs(cstring& text, algo::Tuple &tuple, lib_ctype::FCtype &ctype, bool skip_dflt) 
```

```c++
// Remove unstable fields from a tuple encoded in STR, print it back and return
// So, if STR is ams.TraceInfo2Msg  tstamp:... field:xxx
// And ams.TraceInfo2Msg.tstamp appears in dmmeta.unstablefld table,
// this attribute will be stripped from the tuple.
tempstr lib_ctype::StabilizeSsimTuple(strptr str) 
```

```c++
// Find ctype from tuple type tag.
// Supports ctype and ssimfile lookups.
lib_ctype::FCtype *lib_ctype::TagToCtype(algo::Tuple &tuple) 
```

```c++
// Find ctype from ctype name
// Supports ctype and ssimfile lookups.
lib_ctype::FCtype *lib_ctype::TagToCtype(strptr name) 
```

```c++
// Check if attribute is a tuple.
// This is basically the same as TupleFieldQ, but returns false for
// empty Opt fields.
bool lib_ctype::TupleAttrQ(lib_ctype::FField &field, strptr val) 
```

```c++
// Check if field is printed as tuple -- i.e. requires recursive parsing
bool lib_ctype::TupleFieldQ(lib_ctype::FField &field) 
```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Functions -->

### Inputs
<a href="#inputs"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Inputs -->
`lib_ctype` takes the following tables on input:
|Ssimfile|Comment|
|---|---|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|
|[amcdb.bltin](/txt/ssimdb/amcdb/bltin.md)|Specify properties of a C built-in type|
|[dmmeta.cdflt](/txt/ssimdb/dmmeta/cdflt.md)|Specify default value for single-value types that lack fields|
|[dmmeta.cfmt](/txt/ssimdb/dmmeta/cfmt.md)|Specify options for printing/reading ctypes into multiple formats|
|[dmmeta.cppfunc](/txt/ssimdb/dmmeta/cppfunc.md)|Value of field provided by this expression|
|[dmmeta.ctype](/txt/ssimdb/dmmeta/ctype.md)|Struct|
|[dmmeta.fconst](/txt/ssimdb/dmmeta/fconst.md)|Specify enum value (integer + string constant) for a field|
|[dmmeta.field](/txt/ssimdb/dmmeta/field.md)|Specify field of a struct|
|[dmmeta.ftuple](/txt/ssimdb/dmmeta/ftuple.md)||
|[dmmeta.sqltype](/txt/ssimdb/dmmeta/sqltype.md)|Mapping of ctype -> SQL expression|
|[dmmeta.ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|File with ssim tuples|
|[dmmeta.substr](/txt/ssimdb/dmmeta/substr.md)|Specify that the field value is computed from a substring of another field|
|[dev.unstablefld](/txt/ssimdb/dev/unstablefld.md)|Fields that should be stripped from component test output because they contain timestamps etc.|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Inputs -->

### Sources
<a href="#sources"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Sources -->
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/gen/lib_ctype_gen.cpp](/cpp/gen/lib_ctype_gen.cpp)||
|[cpp/lib/lib_ctype.cpp](/cpp/lib/lib_ctype.cpp)||
|[include/gen/lib_ctype_gen.h](/include/gen/lib_ctype_gen.h)||
|[include/gen/lib_ctype_gen.inl.h](/include/gen/lib_ctype_gen.inl.h)||
|[include/lib_ctype.h](/include/lib_ctype.h)||

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
`lib_ctype` generated code creates the tables below.
All allocations are done through global `lib_ctype::_db` [lib_ctype.FDb](#lib_ctype-fdb) structure
|Ctype|Ssimfile|Create|Access|
|---|---|---|---|
|[lib_ctype.Cmdline](#lib_ctype-cmdline)||
|[lib_ctype.FBltin](#lib_ctype-fbltin)|[amcdb.bltin](/txt/ssimdb/amcdb/bltin.md)|FDb.bltin (Lary)|bltin (Lary, by rowid)|
||||FCtype.c_bltin (Ptr)|
|[lib_ctype.FCdflt](#lib_ctype-fcdflt)|[dmmeta.cdflt](/txt/ssimdb/dmmeta/cdflt.md)|FDb.cdflt (Lary)|cdflt (Lary, by rowid)|
||||FCtype.c_cdflt (Ptr)|
|[lib_ctype.FCfmt](#lib_ctype-fcfmt)|[dmmeta.cfmt](/txt/ssimdb/dmmeta/cfmt.md)|FDb.cfmt (Lary)|cfmt (Lary, by rowid)|ind_cfmt (Thash, hash field cfmt)|
||||FCtype.c_cfmt (Ptrary)|
|[lib_ctype.FCppfunc](#lib_ctype-fcppfunc)|[dmmeta.cppfunc](/txt/ssimdb/dmmeta/cppfunc.md)|FDb.cppfunc (Lary)|cppfunc (Lary, by rowid)|
||||FField.c_cppfunc (Ptr)|
|[lib_ctype.FCtype](#lib_ctype-fctype)|[dmmeta.ctype](/txt/ssimdb/dmmeta/ctype.md)|FDb.ctype (Lary)|ctype (Lary, by rowid)|ind_ctype (Thash, hash field ctype)|c_axprice (Ptr)|c_bool (Ptr)|
||||FField.p_arg (Upptr)|
||||FSsimfile.p_ctype (Upptr)|
|[lib_ctype.FDb](#lib_ctype-fdb)||FDb._db (Global)|
|[lib_ctype.FFconst](#lib_ctype-ffconst)|[dmmeta.fconst](/txt/ssimdb/dmmeta/fconst.md)|FDb.fconst (Lary)|fconst (Lary, by rowid)|ind_fconst_key (Thash, hash field key)|ind_fconst (Thash, hash field fconst)|
||||FField.zd_fconst (Llist)|
|[lib_ctype.FField](#lib_ctype-ffield)|[dmmeta.field](/txt/ssimdb/dmmeta/field.md)|FDb.field (Lary)|field (Lary, by rowid)|ind_field (Thash, hash field field)|c_fixmsg (Ptr)|
||||FCtype.c_field (Ptrary)|
||||FSubstr.p_field (Upptr)|
|[lib_ctype.FFtuple](#lib_ctype-fftuple)|[dmmeta.ftuple](/txt/ssimdb/dmmeta/ftuple.md)|FDb.ftuple (Lary)|ftuple (Lary, by rowid)|
||||FField.c_ftuple (Ptr)|
|[lib_ctype.FSqltype](#lib_ctype-fsqltype)|[dmmeta.sqltype](/txt/ssimdb/dmmeta/sqltype.md)|FDb.sqltype (Lary)|sqltype (Lary, by rowid)|
||||FCtype.c_sqltype (Ptr)|
|[lib_ctype.FSsimfile](#lib_ctype-fssimfile)|[dmmeta.ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|FDb.ssimfile (Lary)|ssimfile (Lary, by rowid)|ind_ssimfile (Thash, hash field ssimfile)|
|[lib_ctype.FSubstr](#lib_ctype-fsubstr)|[dmmeta.substr](/txt/ssimdb/dmmeta/substr.md)|FDb.substr (Lary)|substr (Lary, by rowid)|
||||FField.c_substr (Ptr)|
||||FField.c_substr_srcfield (Ptrary)|
|[lib_ctype.FUnstablefld](#lib_ctype-funstablefld)|[dev.unstablefld](/txt/ssimdb/dev/unstablefld.md)|FDb.unstablefld (Lary)|unstablefld (Lary, by rowid)|
||||FField.c_unstablefld (Ptr)|
|[lib_ctype.Match](#lib_ctype-match)||

#### lib_ctype.Cmdline - 
<a href="#lib_ctype-cmdline"></a>

#### lib_ctype.Cmdline Fields
<a href="#lib_ctype-cmdline-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_ctype.Cmdline.in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"data"|Input directory or filename, - for stdin|

#### Struct Cmdline
<a href="#struct-cmdline"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_ctype_gen.h](/include/gen/lib_ctype_gen.h)
```
struct Cmdline { // lib_ctype.Cmdline
    algo::cstring   in;   //   "data"  Input directory or filename, - for stdin
    // func:lib_ctype.Cmdline..Ctor
    inline               Cmdline() __attribute__((nothrow));
};
```

#### lib_ctype.FBltin - Specify properties of a C built-in type
<a href="#lib_ctype-fbltin"></a>

#### lib_ctype.FBltin Fields
<a href="#lib_ctype-fbltin-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_ctype.FBltin.base|[amcdb.Bltin](/txt/ssimdb/amcdb/bltin.md)|[Base](/txt/ssimdb/amcdb/bltin.md)|||

#### Struct FBltin
<a href="#struct-fbltin"></a>
*Note:* field ``lib_ctype.FBltin.base`` has reftype ``base`` so the fields of [amcdb.Bltin](/txt/ssimdb/amcdb/bltin.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_ctype_gen.h](/include/gen/lib_ctype_gen.h)
```
struct FBltin { // lib_ctype.FBltin
    algo::Smallstr100   ctype;      //
    bool                likeu64;    //   false
    bool                bigendok;   //   false
    bool                issigned;   //   false
    algo::Comment       comment;    //
    // func:lib_ctype.FBltin..AssignOp
    inline lib_ctype::FBltin& operator =(const lib_ctype::FBltin &rhs) = delete;
    // func:lib_ctype.FBltin..CopyCtor
    inline               FBltin(const lib_ctype::FBltin &rhs) = delete;
private:
    // func:lib_ctype.FBltin..Ctor
    inline               FBltin() __attribute__((nothrow));
    // func:lib_ctype.FBltin..Dtor
    inline               ~FBltin() __attribute__((nothrow));
    friend lib_ctype::FBltin&   bltin_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend lib_ctype::FBltin*   bltin_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 bltin_RemoveAll() __attribute__((nothrow));
    friend void                 bltin_RemoveLast() __attribute__((nothrow));
};
```

#### lib_ctype.FCdflt - Specify default value for single-value types that lack fields
<a href="#lib_ctype-fcdflt"></a>

#### lib_ctype.FCdflt Fields
<a href="#lib_ctype-fcdflt-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_ctype.FCdflt.base|[dmmeta.Cdflt](/txt/ssimdb/dmmeta/cdflt.md)|[Base](/txt/ssimdb/dmmeta/cdflt.md)|||

#### Struct FCdflt
<a href="#struct-fcdflt"></a>
*Note:* field ``lib_ctype.FCdflt.base`` has reftype ``base`` so the fields of [dmmeta.Cdflt](/txt/ssimdb/dmmeta/cdflt.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_ctype_gen.h](/include/gen/lib_ctype_gen.h)
```
struct FCdflt { // lib_ctype.FCdflt
    algo::Smallstr100   ctype;      //
    algo::CppExpr       dflt;       //
    algo::CppExpr       cppdflt;    //
    algo::Smallstr50    ssimdflt;   //
    algo::Smallstr50    jsdflt;     //
    algo::Comment       comment;    //
    // func:lib_ctype.FCdflt..AssignOp
    inline lib_ctype::FCdflt& operator =(const lib_ctype::FCdflt &rhs) = delete;
    // func:lib_ctype.FCdflt..CopyCtor
    inline               FCdflt(const lib_ctype::FCdflt &rhs) = delete;
private:
    // func:lib_ctype.FCdflt..Ctor
    inline               FCdflt() __attribute__((nothrow));
    // func:lib_ctype.FCdflt..Dtor
    inline               ~FCdflt() __attribute__((nothrow));
    friend lib_ctype::FCdflt&   cdflt_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend lib_ctype::FCdflt*   cdflt_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 cdflt_RemoveLast() __attribute__((nothrow));
};
```

#### lib_ctype.FCfmt - Specify options for printing/reading ctypes into multiple formats
<a href="#lib_ctype-fcfmt"></a>

#### lib_ctype.FCfmt Fields
<a href="#lib_ctype-fcfmt-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_ctype.FCfmt.msghdr|[dmmeta.Cfmt](/txt/ssimdb/dmmeta/cfmt.md)|[Base](/txt/ssimdb/dmmeta/cfmt.md)|||

#### Struct FCfmt
<a href="#struct-fcfmt"></a>
*Note:* field ``lib_ctype.FCfmt.msghdr`` has reftype ``base`` so the fields of [dmmeta.Cfmt](/txt/ssimdb/dmmeta/cfmt.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_ctype_gen.h](/include/gen/lib_ctype_gen.h)
```
struct FCfmt { // lib_ctype.FCfmt
    algo::Smallstr100   cfmt;                  //
    algo::Smallstr50    printfmt;              //
    bool                read;                  //   false
    bool                print;                 //   false
    algo::Smallstr20    sep;                   //
    bool                genop;                 //   false
    algo::Comment       comment;               //
    bool                ctype_c_cfmt_in_ary;   //   false  membership flag
    lib_ctype::FCfmt*   ind_cfmt_next;         // hash next
    u32                 ind_cfmt_hashval;      // hash value
    // func:lib_ctype.FCfmt..AssignOp
    lib_ctype::FCfmt&    operator =(const lib_ctype::FCfmt &rhs) = delete;
    // func:lib_ctype.FCfmt..CopyCtor
    FCfmt(const lib_ctype::FCfmt &rhs) = delete;
private:
    // func:lib_ctype.FCfmt..Ctor
    inline               FCfmt() __attribute__((nothrow));
    // func:lib_ctype.FCfmt..Dtor
    inline               ~FCfmt() __attribute__((nothrow));
    friend lib_ctype::FCfmt&    cfmt_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend lib_ctype::FCfmt*    cfmt_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 cfmt_RemoveLast() __attribute__((nothrow));
};
```

#### lib_ctype.FCppfunc - Value of field provided by this expression
<a href="#lib_ctype-fcppfunc"></a>

#### lib_ctype.FCppfunc Fields
<a href="#lib_ctype-fcppfunc-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_ctype.FCppfunc.base|[dmmeta.Cppfunc](/txt/ssimdb/dmmeta/cppfunc.md)|[Base](/txt/ssimdb/dmmeta/cppfunc.md)|||

#### Struct FCppfunc
<a href="#struct-fcppfunc"></a>
*Note:* field ``lib_ctype.FCppfunc.base`` has reftype ``base`` so the fields of [dmmeta.Cppfunc](/txt/ssimdb/dmmeta/cppfunc.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_ctype_gen.h](/include/gen/lib_ctype_gen.h)
```
struct FCppfunc { // lib_ctype.FCppfunc
    algo::Smallstr100   field;   //
    algo::CppExpr       expr;    //
    bool                print;   //   false
    bool                set;     //   false
    // func:lib_ctype.FCppfunc..AssignOp
    inline lib_ctype::FCppfunc& operator =(const lib_ctype::FCppfunc &rhs) = delete;
    // func:lib_ctype.FCppfunc..CopyCtor
    inline               FCppfunc(const lib_ctype::FCppfunc &rhs) = delete;
private:
    // func:lib_ctype.FCppfunc..Ctor
    inline               FCppfunc() __attribute__((nothrow));
    // func:lib_ctype.FCppfunc..Dtor
    inline               ~FCppfunc() __attribute__((nothrow));
    friend lib_ctype::FCppfunc& cppfunc_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend lib_ctype::FCppfunc* cppfunc_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 cppfunc_RemoveAll() __attribute__((nothrow));
    friend void                 cppfunc_RemoveLast() __attribute__((nothrow));
};
```

#### lib_ctype.FCtype - Struct
<a href="#lib_ctype-fctype"></a>

#### lib_ctype.FCtype Fields
<a href="#lib_ctype-fctype-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_ctype.FCtype.msghdr|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Base](/txt/ssimdb/dmmeta/ctype.md)|||
|lib_ctype.FCtype.c_field|[lib_ctype.FField](/txt/lib/lib_ctype/README.md#lib_ctype-ffield)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|lib_ctype.FCtype.printfmt|[dmmeta.Printfmt](/txt/ssimdb/dmmeta/printfmt.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|||
|lib_ctype.FCtype.c_cdflt|[lib_ctype.FCdflt](/txt/lib/lib_ctype/README.md#lib_ctype-fcdflt)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|lib_ctype.FCtype.c_cfmt|[lib_ctype.FCfmt](/txt/lib/lib_ctype/README.md#lib_ctype-fcfmt)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||
|lib_ctype.FCtype.c_bltin|[lib_ctype.FBltin](/txt/lib/lib_ctype/README.md#lib_ctype-fbltin)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|lib_ctype.FCtype.c_sqltype|[lib_ctype.FSqltype](/txt/lib/lib_ctype/README.md#lib_ctype-fsqltype)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||

#### Struct FCtype
<a href="#struct-fctype"></a>
*Note:* field ``lib_ctype.FCtype.msghdr`` has reftype ``base`` so the fields of [dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_ctype_gen.h](/include/gen/lib_ctype_gen.h)
```
struct FCtype { // lib_ctype.FCtype
    algo::Smallstr100      ctype;               // Identifier. must be ns.typename
    algo::Comment          comment;             //
    lib_ctype::FField**    c_field_elems;       // array of pointers
    u32                    c_field_n;           // array of pointers
    u32                    c_field_max;         // capacity of allocated array
    algo::Smallstr50       printfmt;            //
    lib_ctype::FCdflt*     c_cdflt;             // optional pointer
    lib_ctype::FCfmt**     c_cfmt_elems;        // array of pointers
    u32                    c_cfmt_n;            // array of pointers
    u32                    c_cfmt_max;          // capacity of allocated array
    lib_ctype::FBltin*     c_bltin;             // optional pointer
    lib_ctype::FSqltype*   c_sqltype;           // optional pointer
    lib_ctype::FCtype*     ind_ctype_next;      // hash next
    u32                    ind_ctype_hashval;   // hash value
    // reftype Ptrary of lib_ctype.FCtype.c_field prohibits copy
    // x-reference on lib_ctype.FCtype.c_cdflt prevents copy
    // reftype Ptrary of lib_ctype.FCtype.c_cfmt prohibits copy
    // x-reference on lib_ctype.FCtype.c_bltin prevents copy
    // x-reference on lib_ctype.FCtype.c_sqltype prevents copy
    // func:lib_ctype.FCtype..AssignOp
    lib_ctype::FCtype&   operator =(const lib_ctype::FCtype &rhs) = delete;
    // reftype Ptrary of lib_ctype.FCtype.c_field prohibits copy
    // x-reference on lib_ctype.FCtype.c_cdflt prevents copy
    // reftype Ptrary of lib_ctype.FCtype.c_cfmt prohibits copy
    // x-reference on lib_ctype.FCtype.c_bltin prevents copy
    // x-reference on lib_ctype.FCtype.c_sqltype prevents copy
    // func:lib_ctype.FCtype..CopyCtor
    FCtype(const lib_ctype::FCtype &rhs) = delete;
private:
    // func:lib_ctype.FCtype..Ctor
    inline               FCtype() __attribute__((nothrow));
    // func:lib_ctype.FCtype..Dtor
    inline               ~FCtype() __attribute__((nothrow));
    friend lib_ctype::FCtype&   ctype_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend lib_ctype::FCtype*   ctype_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 ctype_RemoveLast() __attribute__((nothrow));
};
```

#### lib_ctype.FDb - In-memory database for lib_ctype
<a href="#lib_ctype-fdb"></a>

#### lib_ctype.FDb Fields
<a href="#lib_ctype-fdb-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_ctype.FDb.fconst|[lib_ctype.FFconst](/txt/lib/lib_ctype/README.md#lib_ctype-ffconst)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|lib_ctype.FDb.ind_fconst_key|[lib_ctype.FFconst](/txt/lib/lib_ctype/README.md#lib_ctype-ffconst)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|lib_ctype.FDb.ind_fconst|[lib_ctype.FFconst](/txt/lib/lib_ctype/README.md#lib_ctype-ffconst)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|lib_ctype.FDb.ssimfile|[lib_ctype.FSsimfile](/txt/lib/lib_ctype/README.md#lib_ctype-fssimfile)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|lib_ctype.FDb.ind_ssimfile|[lib_ctype.FSsimfile](/txt/lib/lib_ctype/README.md#lib_ctype-fssimfile)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|lib_ctype.FDb.ftuple|[lib_ctype.FFtuple](/txt/lib/lib_ctype/README.md#lib_ctype-fftuple)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|lib_ctype.FDb.ctype|[lib_ctype.FCtype](/txt/lib/lib_ctype/README.md#lib_ctype-fctype)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|lib_ctype.FDb.ind_ctype|[lib_ctype.FCtype](/txt/lib/lib_ctype/README.md#lib_ctype-fctype)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|lib_ctype.FDb.field|[lib_ctype.FField](/txt/lib/lib_ctype/README.md#lib_ctype-ffield)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|lib_ctype.FDb.ind_field|[lib_ctype.FField](/txt/lib/lib_ctype/README.md#lib_ctype-ffield)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|lib_ctype.FDb.cdflt|[lib_ctype.FCdflt](/txt/lib/lib_ctype/README.md#lib_ctype-fcdflt)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|lib_ctype.FDb._db|[lib_ctype.FDb](/txt/lib/lib_ctype/README.md#lib_ctype-fdb)|[Global](/txt/exe/amc/reftypes.md#global)|||
|lib_ctype.FDb.cfmt|[lib_ctype.FCfmt](/txt/lib/lib_ctype/README.md#lib_ctype-fcfmt)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|lib_ctype.FDb.ind_cfmt|[lib_ctype.FCfmt](/txt/lib/lib_ctype/README.md#lib_ctype-fcfmt)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|lib_ctype.FDb.cppfunc|[lib_ctype.FCppfunc](/txt/lib/lib_ctype/README.md#lib_ctype-fcppfunc)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|lib_ctype.FDb.substr|[lib_ctype.FSubstr](/txt/lib/lib_ctype/README.md#lib_ctype-fsubstr)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|lib_ctype.FDb.c_axprice|[lib_ctype.FCtype](/txt/lib/lib_ctype/README.md#lib_ctype-fctype)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|lib_ctype.FDb.c_fixmsg|[lib_ctype.FField](/txt/lib/lib_ctype/README.md#lib_ctype-ffield)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|lib_ctype.FDb.c_bool|[lib_ctype.FCtype](/txt/lib/lib_ctype/README.md#lib_ctype-fctype)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|lib_ctype.FDb.unstablefld|[lib_ctype.FUnstablefld](/txt/lib/lib_ctype/README.md#lib_ctype-funstablefld)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|lib_ctype.FDb.bltin|[lib_ctype.FBltin](/txt/lib/lib_ctype/README.md#lib_ctype-fbltin)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|lib_ctype.FDb.sqltype|[lib_ctype.FSqltype](/txt/lib/lib_ctype/README.md#lib_ctype-fsqltype)|[Lary](/txt/exe/amc/reftypes.md#lary)|||

#### Struct FDb
<a href="#struct-fdb"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_ctype_gen.h](/include/gen/lib_ctype_gen.h)
```
struct FDb { // lib_ctype.FDb: In-memory database for lib_ctype
    lib_ctype::FFconst*        fconst_lary[32];                // level array
    i32                        fconst_n;                       // number of elements in array
    lib_ctype::FFconst**       ind_fconst_key_buckets_elems;   // pointer to bucket array
    i32                        ind_fconst_key_buckets_n;       // number of elements in bucket array
    i32                        ind_fconst_key_n;               // number of elements in the hash table
    lib_ctype::FFconst**       ind_fconst_buckets_elems;       // pointer to bucket array
    i32                        ind_fconst_buckets_n;           // number of elements in bucket array
    i32                        ind_fconst_n;                   // number of elements in the hash table
    lib_ctype::FSsimfile*      ssimfile_lary[32];              // level array
    i32                        ssimfile_n;                     // number of elements in array
    lib_ctype::FSsimfile**     ind_ssimfile_buckets_elems;     // pointer to bucket array
    i32                        ind_ssimfile_buckets_n;         // number of elements in bucket array
    i32                        ind_ssimfile_n;                 // number of elements in the hash table
    lib_ctype::FFtuple*        ftuple_lary[32];                // level array
    i32                        ftuple_n;                       // number of elements in array
    lib_ctype::FCtype*         ctype_lary[32];                 // level array
    i32                        ctype_n;                        // number of elements in array
    lib_ctype::FCtype**        ind_ctype_buckets_elems;        // pointer to bucket array
    i32                        ind_ctype_buckets_n;            // number of elements in bucket array
    i32                        ind_ctype_n;                    // number of elements in the hash table
    lib_ctype::FField*         field_lary[32];                 // level array
    i32                        field_n;                        // number of elements in array
    lib_ctype::FField**        ind_field_buckets_elems;        // pointer to bucket array
    i32                        ind_field_buckets_n;            // number of elements in bucket array
    i32                        ind_field_n;                    // number of elements in the hash table
    lib_ctype::FCdflt*         cdflt_lary[32];                 // level array
    i32                        cdflt_n;                        // number of elements in array
    lib_ctype::FCfmt*          cfmt_lary[32];                  // level array
    i32                        cfmt_n;                         // number of elements in array
    lib_ctype::FCfmt**         ind_cfmt_buckets_elems;         // pointer to bucket array
    i32                        ind_cfmt_buckets_n;             // number of elements in bucket array
    i32                        ind_cfmt_n;                     // number of elements in the hash table
    lib_ctype::FCppfunc*       cppfunc_lary[32];               // level array
    i32                        cppfunc_n;                      // number of elements in array
    lib_ctype::FSubstr*        substr_lary[32];                // level array
    i32                        substr_n;                       // number of elements in array
    lib_ctype::FCtype*         c_axprice;                      // optional pointer
    lib_ctype::FField*         c_fixmsg;                       // optional pointer
    lib_ctype::FCtype*         c_bool;                         // optional pointer
    lib_ctype::FUnstablefld*   unstablefld_lary[32];           // level array
    i32                        unstablefld_n;                  // number of elements in array
    lib_ctype::FBltin*         bltin_lary[32];                 // level array
    i32                        bltin_n;                        // number of elements in array
    lib_ctype::FSqltype*       sqltype_lary[32];               // level array
    i32                        sqltype_n;                      // number of elements in array
    lib_ctype::trace           trace;                          //
};
```

#### lib_ctype.FFconst - Specify enum value (integer + string constant) for a field
<a href="#lib_ctype-ffconst"></a>

#### lib_ctype.FFconst Fields
<a href="#lib_ctype-ffconst-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_ctype.FFconst.msghdr|[dmmeta.Fconst](/txt/ssimdb/dmmeta/fconst.md)|[Base](/txt/ssimdb/dmmeta/fconst.md)|||
|lib_ctype.FFconst.key|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FFconst
<a href="#struct-ffconst"></a>
*Note:* field ``lib_ctype.FFconst.msghdr`` has reftype ``base`` so the fields of [dmmeta.Fconst](/txt/ssimdb/dmmeta/fconst.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_ctype_gen.h](/include/gen/lib_ctype_gen.h)
```
struct FFconst { // lib_ctype.FFconst
    lib_ctype::FFconst*   ind_fconst_key_next;      // hash next
    u32                   ind_fconst_key_hashval;   // hash value
    lib_ctype::FFconst*   ind_fconst_next;          // hash next
    u32                   ind_fconst_hashval;       // hash value
    algo::Smallstr100     fconst;                   //
    algo::CppExpr         value;                    //
    algo::Comment         comment;                  //
    algo::cstring         key;                      //
    lib_ctype::FFconst*   field_zd_fconst_next;     // zslist link; -1 means not-in-list
    lib_ctype::FFconst*   field_zd_fconst_prev;     // previous element
    // func:lib_ctype.FFconst..AssignOp
    inline lib_ctype::FFconst& operator =(const lib_ctype::FFconst &rhs) = delete;
    // func:lib_ctype.FFconst..CopyCtor
    inline               FFconst(const lib_ctype::FFconst &rhs) = delete;
private:
    // func:lib_ctype.FFconst..Ctor
    inline               FFconst() __attribute__((nothrow));
    // func:lib_ctype.FFconst..Dtor
    inline               ~FFconst() __attribute__((nothrow));
    friend lib_ctype::FFconst&  fconst_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend lib_ctype::FFconst*  fconst_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 fconst_RemoveLast() __attribute__((nothrow));
};
```

#### lib_ctype.FField - Specify field of a struct
<a href="#lib_ctype-ffield"></a>

#### lib_ctype.FField Fields
<a href="#lib_ctype-ffield-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_ctype.FField.msghdr|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Base](/txt/ssimdb/dmmeta/field.md)|||
|lib_ctype.FField.istuple_computed|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_ctype.FField.c_ftuple|[lib_ctype.FFtuple](/txt/lib/lib_ctype/README.md#lib_ctype-fftuple)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|lib_ctype.FField.p_arg|[lib_ctype.FCtype](/txt/lib/lib_ctype/README.md#lib_ctype-fctype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|lib_ctype.FField.has_fconst|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_ctype.FField.istuple|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|lib_ctype.FField.zd_fconst|[lib_ctype.FFconst](/txt/lib/lib_ctype/README.md#lib_ctype-ffconst)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|lib_ctype.FField.c_cppfunc|[lib_ctype.FCppfunc](/txt/lib/lib_ctype/README.md#lib_ctype-fcppfunc)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|lib_ctype.FField.c_substr|[lib_ctype.FSubstr](/txt/lib/lib_ctype/README.md#lib_ctype-fsubstr)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|lib_ctype.FField.c_unstablefld|[lib_ctype.FUnstablefld](/txt/lib/lib_ctype/README.md#lib_ctype-funstablefld)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|lib_ctype.FField.c_substr_srcfield|[lib_ctype.FSubstr](/txt/lib/lib_ctype/README.md#lib_ctype-fsubstr)|[Ptrary](/txt/exe/amc/reftypes.md#ptrary)|||

#### Struct FField
<a href="#struct-ffield"></a>
*Note:* field ``lib_ctype.FField.msghdr`` has reftype ``base`` so the fields of [dmmeta.Field](/txt/ssimdb/dmmeta/field.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_ctype_gen.h](/include/gen/lib_ctype_gen.h)
```
struct FField { // lib_ctype.FField
    lib_ctype::FField*         ind_field_next;            // hash next
    u32                        ind_field_hashval;         // hash value
    algo::Smallstr100          field;                     // Primary key, as ctype.name
    algo::Smallstr100          arg;                       // Type of field
    algo::Smallstr50           reftype;                   //   "Val"  Type constructor
    algo::CppExpr              dflt;                      // Default value (c++ expression)
    algo::Comment              comment;                   //
    bool                       istuple_computed;          //   false
    lib_ctype::FFtuple*        c_ftuple;                  // optional pointer
    lib_ctype::FCtype*         p_arg;                     // reference to parent row
    bool                       has_fconst;                //   false
    bool                       istuple;                   //   false
    lib_ctype::FFconst*        zd_fconst_head;            // zero-terminated doubly linked list
    i32                        zd_fconst_n;               // zero-terminated doubly linked list
    lib_ctype::FFconst*        zd_fconst_tail;            // pointer to last element
    lib_ctype::FCppfunc*       c_cppfunc;                 // optional pointer
    lib_ctype::FSubstr*        c_substr;                  // optional pointer
    lib_ctype::FUnstablefld*   c_unstablefld;             // optional pointer
    lib_ctype::FSubstr**       c_substr_srcfield_elems;   // array of pointers
    u32                        c_substr_srcfield_n;       // array of pointers
    u32                        c_substr_srcfield_max;     // capacity of allocated array
    bool                       ctype_c_field_in_ary;      //   false  membership flag
    // x-reference on lib_ctype.FField.c_ftuple prevents copy
    // x-reference on lib_ctype.FField.p_arg prevents copy
    // reftype Llist of lib_ctype.FField.zd_fconst prohibits copy
    // x-reference on lib_ctype.FField.c_cppfunc prevents copy
    // x-reference on lib_ctype.FField.c_substr prevents copy
    // x-reference on lib_ctype.FField.c_unstablefld prevents copy
    // reftype Ptrary of lib_ctype.FField.c_substr_srcfield prohibits copy
    // func:lib_ctype.FField..AssignOp
    lib_ctype::FField&   operator =(const lib_ctype::FField &rhs) = delete;
    // x-reference on lib_ctype.FField.c_ftuple prevents copy
    // x-reference on lib_ctype.FField.p_arg prevents copy
    // reftype Llist of lib_ctype.FField.zd_fconst prohibits copy
    // x-reference on lib_ctype.FField.c_cppfunc prevents copy
    // x-reference on lib_ctype.FField.c_substr prevents copy
    // x-reference on lib_ctype.FField.c_unstablefld prevents copy
    // reftype Ptrary of lib_ctype.FField.c_substr_srcfield prohibits copy
    // func:lib_ctype.FField..CopyCtor
    FField(const lib_ctype::FField &rhs) = delete;
private:
    // func:lib_ctype.FField..Ctor
    inline               FField() __attribute__((nothrow));
    // func:lib_ctype.FField..Dtor
    inline               ~FField() __attribute__((nothrow));
    friend lib_ctype::FField&   field_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend lib_ctype::FField*   field_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 field_RemoveLast() __attribute__((nothrow));
};
```

#### lib_ctype.FFtuple - 
<a href="#lib_ctype-fftuple"></a>

#### lib_ctype.FFtuple Fields
<a href="#lib_ctype-fftuple-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_ctype.FFtuple.msghdr|[dmmeta.Ftuple](/txt/ssimdb/dmmeta/ftuple.md)|[Base](/txt/ssimdb/dmmeta/ftuple.md)|||

#### Struct FFtuple
<a href="#struct-fftuple"></a>
*Note:* field ``lib_ctype.FFtuple.msghdr`` has reftype ``base`` so the fields of [dmmeta.Ftuple](/txt/ssimdb/dmmeta/ftuple.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_ctype_gen.h](/include/gen/lib_ctype_gen.h)
```
struct FFtuple { // lib_ctype.FFtuple
    algo::Smallstr100   field;     //
    algo::Comment       comment;   //
    // func:lib_ctype.FFtuple..AssignOp
    inline lib_ctype::FFtuple& operator =(const lib_ctype::FFtuple &rhs) = delete;
    // func:lib_ctype.FFtuple..CopyCtor
    inline               FFtuple(const lib_ctype::FFtuple &rhs) = delete;
private:
    // func:lib_ctype.FFtuple..Ctor
    inline               FFtuple() __attribute__((nothrow));
    // func:lib_ctype.FFtuple..Dtor
    inline               ~FFtuple() __attribute__((nothrow));
    friend lib_ctype::FFtuple&  ftuple_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend lib_ctype::FFtuple*  ftuple_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 ftuple_RemoveLast() __attribute__((nothrow));
};
```

#### lib_ctype.FSqltype - Mapping of ctype -> SQL expression
<a href="#lib_ctype-fsqltype"></a>

#### lib_ctype.FSqltype Fields
<a href="#lib_ctype-fsqltype-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_ctype.FSqltype.base|[dmmeta.Sqltype](/txt/ssimdb/dmmeta/sqltype.md)|[Base](/txt/ssimdb/dmmeta/sqltype.md)|||

#### Struct FSqltype
<a href="#struct-fsqltype"></a>
*Note:* field ``lib_ctype.FSqltype.base`` has reftype ``base`` so the fields of [dmmeta.Sqltype](/txt/ssimdb/dmmeta/sqltype.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_ctype_gen.h](/include/gen/lib_ctype_gen.h)
```
struct FSqltype { // lib_ctype.FSqltype
    algo::Smallstr100   ctype;     //
    algo::Smallstr100   expr;      //
    algo::Comment       comment;   //
    // func:lib_ctype.FSqltype..AssignOp
    inline lib_ctype::FSqltype& operator =(const lib_ctype::FSqltype &rhs) = delete;
    // func:lib_ctype.FSqltype..CopyCtor
    inline               FSqltype(const lib_ctype::FSqltype &rhs) = delete;
private:
    // func:lib_ctype.FSqltype..Ctor
    inline               FSqltype() __attribute__((nothrow));
    // func:lib_ctype.FSqltype..Dtor
    inline               ~FSqltype() __attribute__((nothrow));
    friend lib_ctype::FSqltype& sqltype_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend lib_ctype::FSqltype* sqltype_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 sqltype_RemoveAll() __attribute__((nothrow));
    friend void                 sqltype_RemoveLast() __attribute__((nothrow));
};
```

#### lib_ctype.FSsimfile - File with ssim tuples
<a href="#lib_ctype-fssimfile"></a>

#### lib_ctype.FSsimfile Fields
<a href="#lib_ctype-fssimfile-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_ctype.FSsimfile.msghdr|[dmmeta.Ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|[Base](/txt/ssimdb/dmmeta/ssimfile.md)|||
|lib_ctype.FSsimfile.p_ctype|[lib_ctype.FCtype](/txt/lib/lib_ctype/README.md#lib_ctype-fctype)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FSsimfile
<a href="#struct-fssimfile"></a>
*Note:* field ``lib_ctype.FSsimfile.msghdr`` has reftype ``base`` so the fields of [dmmeta.Ssimfile](/txt/ssimdb/dmmeta/ssimfile.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_ctype_gen.h](/include/gen/lib_ctype_gen.h)
```
struct FSsimfile { // lib_ctype.FSsimfile
    lib_ctype::FSsimfile*   ind_ssimfile_next;      // hash next
    u32                     ind_ssimfile_hashval;   // hash value
    algo::Smallstr50        ssimfile;               //
    algo::Smallstr100       ctype;                  //
    lib_ctype::FCtype*      p_ctype;                // reference to parent row
    // x-reference on lib_ctype.FSsimfile.p_ctype prevents copy
    // func:lib_ctype.FSsimfile..AssignOp
    inline lib_ctype::FSsimfile& operator =(const lib_ctype::FSsimfile &rhs) = delete;
    // x-reference on lib_ctype.FSsimfile.p_ctype prevents copy
    // func:lib_ctype.FSsimfile..CopyCtor
    inline               FSsimfile(const lib_ctype::FSsimfile &rhs) = delete;
private:
    // func:lib_ctype.FSsimfile..Ctor
    inline               FSsimfile() __attribute__((nothrow));
    // func:lib_ctype.FSsimfile..Dtor
    inline               ~FSsimfile() __attribute__((nothrow));
    friend lib_ctype::FSsimfile& ssimfile_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend lib_ctype::FSsimfile* ssimfile_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 ssimfile_RemoveLast() __attribute__((nothrow));
};
```

#### lib_ctype.FSubstr - Specify that the field value is computed from a substring of another field
<a href="#lib_ctype-fsubstr"></a>

#### lib_ctype.FSubstr Fields
<a href="#lib_ctype-fsubstr-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_ctype.FSubstr.base|[dmmeta.Substr](/txt/ssimdb/dmmeta/substr.md)|[Base](/txt/ssimdb/dmmeta/substr.md)|||
|lib_ctype.FSubstr.p_field|[lib_ctype.FField](/txt/lib/lib_ctype/README.md#lib_ctype-ffield)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||

#### Struct FSubstr
<a href="#struct-fsubstr"></a>
*Note:* field ``lib_ctype.FSubstr.base`` has reftype ``base`` so the fields of [dmmeta.Substr](/txt/ssimdb/dmmeta/substr.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_ctype_gen.h](/include/gen/lib_ctype_gen.h)
```
struct FSubstr { // lib_ctype.FSubstr
    algo::Smallstr100    field;                            //
    algo::CppExpr        expr;                             //
    algo::Smallstr100    srcfield;                         //
    lib_ctype::FField*   p_field;                          // reference to parent row
    bool                 field_c_substr_srcfield_in_ary;   //   false  membership flag
    // x-reference on lib_ctype.FSubstr.p_field prevents copy
    // func:lib_ctype.FSubstr..AssignOp
    inline lib_ctype::FSubstr& operator =(const lib_ctype::FSubstr &rhs) = delete;
    // x-reference on lib_ctype.FSubstr.p_field prevents copy
    // func:lib_ctype.FSubstr..CopyCtor
    inline               FSubstr(const lib_ctype::FSubstr &rhs) = delete;
private:
    // func:lib_ctype.FSubstr..Ctor
    inline               FSubstr() __attribute__((nothrow));
    // func:lib_ctype.FSubstr..Dtor
    inline               ~FSubstr() __attribute__((nothrow));
    friend lib_ctype::FSubstr&  substr_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend lib_ctype::FSubstr*  substr_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 substr_RemoveAll() __attribute__((nothrow));
    friend void                 substr_RemoveLast() __attribute__((nothrow));
};
```

#### lib_ctype.FUnstablefld - Fields that should be stripped from component test output because they contain timestamps etc.
<a href="#lib_ctype-funstablefld"></a>

#### lib_ctype.FUnstablefld Fields
<a href="#lib_ctype-funstablefld-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_ctype.FUnstablefld.base|[dev.Unstablefld](/txt/ssimdb/dev/unstablefld.md)|[Base](/txt/ssimdb/dev/unstablefld.md)|||

#### Struct FUnstablefld
<a href="#struct-funstablefld"></a>
*Note:* field ``lib_ctype.FUnstablefld.base`` has reftype ``base`` so the fields of [dev.Unstablefld](/txt/ssimdb/dev/unstablefld.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_ctype_gen.h](/include/gen/lib_ctype_gen.h)
```
struct FUnstablefld { // lib_ctype.FUnstablefld
    algo::Smallstr100   field;     //
    algo::Comment       comment;   //
    // func:lib_ctype.FUnstablefld..AssignOp
    inline lib_ctype::FUnstablefld& operator =(const lib_ctype::FUnstablefld &rhs) = delete;
    // func:lib_ctype.FUnstablefld..CopyCtor
    inline               FUnstablefld(const lib_ctype::FUnstablefld &rhs) = delete;
private:
    // func:lib_ctype.FUnstablefld..Ctor
    inline               FUnstablefld() __attribute__((nothrow));
    // func:lib_ctype.FUnstablefld..Dtor
    inline               ~FUnstablefld() __attribute__((nothrow));
    friend lib_ctype::FUnstablefld& unstablefld_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend lib_ctype::FUnstablefld* unstablefld_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 unstablefld_RemoveAll() __attribute__((nothrow));
    friend void                 unstablefld_RemoveLast() __attribute__((nothrow));
};
```

#### lib_ctype.Match - 
<a href="#lib_ctype-match"></a>

#### lib_ctype.Match Fields
<a href="#lib_ctype-match-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|lib_ctype.Match.distance|i32|[Val](/txt/exe/amc/reftypes.md#val)|0|Mismatch distance (0 = match)|
|lib_ctype.Match.maxdist|i32|[Val](/txt/exe/amc/reftypes.md#val)|1|Don't match beyond this distance|
|lib_ctype.Match.nattr|i32|[Val](/txt/exe/amc/reftypes.md#val)|0|Number of attributes compared|
|lib_ctype.Match.save_unmatched|bool|[Val](/txt/exe/amc/reftypes.md#val)|false|If set, accumulate match errors in 'err'|
|lib_ctype.Match.n_unmatched|i32|[Val](/txt/exe/amc/reftypes.md#val)||Number of unmatched tuples found|
|lib_ctype.Match.err|[algo.Tuple](/txt/protocol/algo/Tuple.md)|[Val](/txt/exe/amc/reftypes.md#val)||List of errors (key:val format)|

#### Struct Match
<a href="#struct-match"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/lib_ctype_gen.h](/include/gen/lib_ctype_gen.h)
```
struct Match { // lib_ctype.Match
    i32           distance;         //   0  Mismatch distance (0 = match)
    i32           maxdist;          //   1  Don't match beyond this distance
    i32           nattr;            //   0  Number of attributes compared
    bool          save_unmatched;   //   false  If set, accumulate match errors in 'err'
    i32           n_unmatched;      //   0  Number of unmatched tuples found
    algo::Tuple   err;              // List of errors (key:val format)
    // func:lib_ctype.Match..Ctor
    inline               Match() __attribute__((nothrow));
};
```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Imdb -->


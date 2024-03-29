## lib_ctype - Helpful library for reading/writing ctypes as text tuples


### Table Of Contents
<a href="#table-of-contents"></a>
* [Ctypes](#ctypes)
* [Functions](#functions)
* [Sources](#sources)
* [Inputs](#inputs)

### Ctypes
<a href="#ctypes"></a>
Other ctypes in this namespace which don't have own readme files

#### lib_ctype.Cmdline - 
<a href="#lib_ctype-cmdline"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|in|algo.cstring|Val|"data"|Input directory or filename, - for stdin|

#### lib_ctype.FBltin - 
<a href="#lib_ctype-fbltin"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[amcdb.Bltin](/txt/ssimdb/amcdb/bltin.md)|Base|

#### lib_ctype.FCdflt - 
<a href="#lib_ctype-fcdflt"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[dmmeta.Cdflt](/txt/ssimdb/dmmeta/cdflt.md)|Base|

#### lib_ctype.FCfmt - 
<a href="#lib_ctype-fcfmt"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|msghdr|[dmmeta.Cfmt](/txt/ssimdb/dmmeta/cfmt.md)|Base|

#### lib_ctype.FCppfunc - 
<a href="#lib_ctype-fcppfunc"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[dmmeta.Cppfunc](/txt/ssimdb/dmmeta/cppfunc.md)|Base|

#### lib_ctype.FCtype - 
<a href="#lib_ctype-fctype"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|msghdr|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|Base|
|c_field|lib_ctype.FField|Ptrary|
|printfmt|[dmmeta.Printfmt](/txt/ssimdb/dmmeta/printfmt.md)|Pkey|
|c_cdflt|lib_ctype.FCdflt|Ptr|
|c_cfmt|lib_ctype.FCfmt|Ptrary|
|c_bltin|lib_ctype.FBltin|Ptr|

#### lib_ctype.FDb - In-memory database for lib_ctype
<a href="#lib_ctype-fdb"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|fconst|lib_ctype.FFconst|Lary|
|ind_fconst_key|lib_ctype.FFconst|Thash|
|ind_fconst|lib_ctype.FFconst|Thash|
|ssimfile|lib_ctype.FSsimfile|Lary|
|ind_ssimfile|lib_ctype.FSsimfile|Thash|
|ftuple|lib_ctype.FFtuple|Lary|
|ctype|lib_ctype.FCtype|Lary|
|ind_ctype|lib_ctype.FCtype|Thash|
|field|lib_ctype.FField|Lary|
|ind_field|lib_ctype.FField|Thash|
|cdflt|lib_ctype.FCdflt|Lary|
|_db|lib_ctype.FDb|Global|
|cfmt|lib_ctype.FCfmt|Lary|
|ind_cfmt|lib_ctype.FCfmt|Thash|
|cppfunc|lib_ctype.FCppfunc|Lary|
|substr|lib_ctype.FSubstr|Lary|
|c_axprice|lib_ctype.FCtype|Ptr|
|c_fixmsg|lib_ctype.FField|Ptr|
|c_bool|lib_ctype.FCtype|Ptr|
|unstablefld|lib_ctype.FUnstablefld|Lary|
|bltin|lib_ctype.FBltin|Lary|

#### lib_ctype.FFconst - 
<a href="#lib_ctype-ffconst"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|msghdr|[dmmeta.Fconst](/txt/ssimdb/dmmeta/fconst.md)|Base|
|key|algo.cstring|Val|

#### lib_ctype.FField - 
<a href="#lib_ctype-ffield"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|msghdr|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|Base|
|istuple_computed|bool|Val|
|c_ftuple|lib_ctype.FFtuple|Ptr|
|p_arg|lib_ctype.FCtype|Upptr|
|has_fconst|bool|Val|
|istuple|bool|Val|
|zd_fconst|lib_ctype.FFconst|Llist|
|c_cppfunc|lib_ctype.FCppfunc|Ptr|
|c_substr|lib_ctype.FSubstr|Ptr|
|c_unstablefld|lib_ctype.FUnstablefld|Ptr|
|c_substr_srcfield|lib_ctype.FSubstr|Ptrary|

#### lib_ctype.FFtuple - 
<a href="#lib_ctype-fftuple"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|msghdr|[dmmeta.Ftuple](/txt/ssimdb/dmmeta/ftuple.md)|Base|

#### lib_ctype.FSsimfile - 
<a href="#lib_ctype-fssimfile"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|msghdr|[dmmeta.Ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|Base|
|p_ctype|lib_ctype.FCtype|Upptr|

#### lib_ctype.FSubstr - 
<a href="#lib_ctype-fsubstr"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[dmmeta.Substr](/txt/ssimdb/dmmeta/substr.md)|Base|
|p_field|lib_ctype.FField|Upptr|

#### lib_ctype.FUnstablefld - 
<a href="#lib_ctype-funstablefld"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|base|[dev.Unstablefld](/txt/ssimdb/dev/unstablefld.md)|Base|

#### lib_ctype.Match - 
<a href="#lib_ctype-match"></a>

|Name|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|distance|i32|Val|0|Mismatch distance (0 = match)|
|maxdist|i32|Val|1|Don't match beyond this distance|
|nattr|i32|Val|0|Number of attributes compared|
|save_unmatched|bool|Val|false|If set, accumulate match errors in 'err'|
|n_unmatched|i32|Val||Number of unmatched tuples found|
|err|algo.Tuple|Val||List of errors (key:val format)|

### Functions
<a href="#functions"></a>
Functions exported from this namespace:

```c++
// Find ctype from tuple type tag.
// Supports ctype and ssimfile lookups.
lib_ctype::FCtype *lib_ctype::TagToCtype(algo::Tuple &tuple) 
```

```c++
// Check if specified attribute matches the default value for the field.
bool lib_ctype::EqualDefaultQ(algo::Attr *attr, lib_ctype::FField &field) 
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
// Retrieve base type for the given ctype
lib_ctype::FCtype *lib_ctype::Basetype(lib_ctype::FCtype &ctype) 
```

```c++
// Find field by name, possibly scanning base fields
// Skips dot-suffix first, i.e. attr "abc.4" is looked up as "abc" (used for Varlen fields)
lib_ctype::FField *lib_ctype::FindField(lib_ctype::FCtype &ctype, strptr name) 
```

```c++
// Check if field is printed as tuple -- i.e. requires recursive parsing
bool lib_ctype::TupleFieldQ(lib_ctype::FField &field) 
```

```c++
// Check if attribute is a tuple.
// This is basically the same as TupleFieldQ, but returns false for
// empty Opt fields.
bool lib_ctype::TupleAttrQ(lib_ctype::FField &field, strptr val) 
```

```c++
// Prepare main schema
void lib_ctype::Init() 
```

```c++
// Find fconst from field and ssim attr value.
lib_ctype::FFconst *lib_ctype::FindFconst(lib_ctype::FField *field, cstring& attr_value) 
```

```c++
// Match Tuple EXPECT against tuple RESULT.
// EXPECT must be a subset of RESULT for the match to pass.
// Results of comparison, mostly represented as a match distance, are saved in MATCH.
// Search exits early if match.distance exceeds match.maxdist.
void lib_ctype::Match_Tuple(lib_ctype::Match &match, Tuple &expect, Tuple &result) 
```

```c++
// Remove unstable fields from a tuple encoded in STR, print it back and return
// So, if STR is ams.TraceInfo2Msg  tstamp:... field:xxx
// And ams.TraceInfo2Msg.tstamp appears in dmmeta.unstablefld table,
// this attribute will be stripped from the tuple.
tempstr lib_ctype::StabilizeSsimTuple(strptr str) 
```

```c++
// Same as tuple version of FillReplscope, but also parses the tuple from STR
void lib_ctype::FillReplscope(algo_lib::Replscope &R, strptr str) 
```

```c++
// Fill Replscope with attribute values (including substrings) of tuple
// So, if STR is dev.targdep targdep:a.b comment:"blah"
// Then R will be filled with variables $targdep, $target, $parent, $comment
// This can be used to perform $-substitution in strings.
void lib_ctype::FillReplscope(algo_lib::Replscope &R, algo::Tuple &tuple) 
```

### Sources
<a href="#sources"></a>
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/gen/lib_ctype_gen.cpp](/cpp/gen/lib_ctype_gen.cpp)||
|[cpp/lib/lib_ctype.cpp](/cpp/lib/lib_ctype.cpp)||
|[include/gen/lib_ctype_gen.h](/include/gen/lib_ctype_gen.h)||
|[include/gen/lib_ctype_gen.inl.h](/include/gen/lib_ctype_gen.inl.h)||
|[include/lib_ctype.h](/include/lib_ctype.h)||

### Inputs
<a href="#inputs"></a>
`lib_ctype` takes the following tables on input:
|ssimfile|comment|
|---|---|
|[dmmeta.ctype](/txt/ssimdb/dmmeta/ctype.md)|Struct|
|[dmmeta.field](/txt/ssimdb/dmmeta/field.md)|Specify field of a struct|
|[dev.unstablefld](/txt/ssimdb/dev/unstablefld.md)|Fields that should be stripped from component test output because they contain timestamps etc.|
|[dmmeta.substr](/txt/ssimdb/dmmeta/substr.md)|Specify that the field value is computed from a substring of another field|
|[dmmeta.ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|File with ssim tuples|
|[dmmeta.ftuple](/txt/ssimdb/dmmeta/ftuple.md)||
|[dmmeta.fconst](/txt/ssimdb/dmmeta/fconst.md)|Specify enum value (integer + string constant) for a field|
|[dmmeta.cppfunc](/txt/ssimdb/dmmeta/cppfunc.md)|Value of field provided by this expression|
|[dmmeta.cfmt](/txt/ssimdb/dmmeta/cfmt.md)|Specify options for printing/reading ctypes into multiple formats|
|[dmmeta.cdflt](/txt/ssimdb/dmmeta/cdflt.md)|Specify default value for single-value types that lack fields|
|[amcdb.bltin](/txt/ssimdb/amcdb/bltin.md)|Specify properties of a C built-in type|
|[dmmeta.dispsigcheck](/txt/ssimdb/dmmeta/dispsigcheck.md)|Check signature of input data against executable's version|


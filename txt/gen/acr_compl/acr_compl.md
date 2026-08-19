## acr_compl - Internals


### Table Of Contents
<a href="#table-of-contents"></a>
* [Description](#description)
* [Sources](#sources)
* [Dependencies](#dependencies)
* [In Memory DB](#in-memory-db)
* [Tests](#tests)

### Description
<a href="#description"></a>
for usage, see [acr_compl - ACR shell auto-complete for all targets](/txt/exe/acr_compl/README.md)

### Sources
<a href="#sources"></a>
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/acr_compl/main.cpp](/cpp/acr_compl/main.cpp)||
|[cpp/acr_compl/shell.cpp](/cpp/acr_compl/shell.cpp)||
|[cpp/gen/acr_compl_gen.cpp](/cpp/gen/acr_compl_gen.cpp)||
|[include/acr_compl.h](/include/acr_compl.h)||
|[include/gen/acr_compl_gen.h](/include/gen/acr_compl_gen.h)||
|[include/gen/acr_compl_gen.inl.h](/include/gen/acr_compl_gen.inl.h)||

### Dependencies
<a href="#dependencies"></a>
The build target depends on the following libraries
|Target|Comment|
|---|---|
|[algo_lib](/txt/lib/algo_lib/README.md)|Support library for all executables|

### In Memory DB
<a href="#in-memory-db"></a>
`acr_compl` generated code creates the tables below.
All allocations are done through global `acr_compl::_db` [acr_compl.FDb](#acr_compl-fdb) structure
|Ctype|Ssimfile|Create|Access|
|---|---|---|---|
|[acr_compl.Badness](#acr_compl-badness)||
|[acr_compl.Completion](#acr_compl-completion)||
||||FCompletion.msghdr (Base)|
|[acr_compl.FAnonfld](#acr_compl-fanonfld)|[dmmeta.anonfld](/txt/ssimdb/dmmeta/anonfld.md)|FDb.anonfld (Lary)|anonfld (Lary, by rowid)|
||||FField.c_anonfld (Ptr)|
|[acr_compl.FArgvtype](#acr_compl-fargvtype)|[dmmeta.argvtype](/txt/ssimdb/dmmeta/argvtype.md)|FDb.argvtype (Lary)|argvtype (Lary, by rowid)|
||||FCtype.c_argvtype (Ptr)|
|[acr_compl.FCcmdline](#acr_compl-fccmdline)|[dmmeta.ccmdline](/txt/ssimdb/dmmeta/ccmdline.md)|FDb.ccmdline (Lary)|ccmdline (Lary, by rowid)|
|[acr_compl.FCmd](#acr_compl-fcmd)||FDb.cmd (Lary)|cmd (Lary, by rowid)|ind_cmd (Thash, hash field cmd)|
|[acr_compl.FCompletion](#acr_compl-fcompletion)||FDb.completion (Lary)|completion (Lary, by rowid)|bh_completion (Bheap, sort field badness)|
|[acr_compl.FComplsource](#acr_compl-fcomplsource)||FDb.complsource (Lary)|complsource (Lary, by rowid)|
|[acr_compl.FCtype](#acr_compl-fctype)|[dmmeta.ctype](/txt/ssimdb/dmmeta/ctype.md)|FDb.ctype (Lary)|ctype (Lary, by rowid)|ind_ctype (Thash, hash field ctype)|p_cmd_ctype (Ptr)|
||||FCcmdline.p_ctype (Upptr)|
||||FCmd.p_ctype (Upptr)|
||||FCmd.p_basectype (Upptr)|
||||FField.p_arg (Upptr)|
||||FField.p_ctype (Upptr)|
||||FSsimfile.p_ctype (Upptr)|
|[acr_compl.FDb](#acr_compl-fdb)||FDb._db (Global)|
|[acr_compl.FFalias](#acr_compl-ffalias)|[dmmeta.falias](/txt/ssimdb/dmmeta/falias.md)|FDb.falias (Lary)|falias (Lary, by rowid)|
||||FField.c_falias (Ptr)|
||||FField.c_falias_srcfield (Ptrary)|
|[acr_compl.FFconst](#acr_compl-ffconst)|[dmmeta.fconst](/txt/ssimdb/dmmeta/fconst.md)|FDb.fconst (Lary)|fconst (Lary, by rowid)|
||||FField.c_fconst (Ptrary)|
|[acr_compl.FFflag](#acr_compl-ffflag)|[dmmeta.fflag](/txt/ssimdb/dmmeta/fflag.md)|FDb.fflag (Lary)|fflag (Lary, by rowid)|
||||FField.c_fflag (Ptr)|
|[acr_compl.FField](#acr_compl-ffield)|[dmmeta.field](/txt/ssimdb/dmmeta/field.md)|FDb.field (Lary)|field (Lary, by rowid)|ind_field (Thash, hash field field)|zd_cmd_field (Llist)|ind_cmd_field_name (Thash, hash field name)|
||||FCcmdline.p_basecmdline (Upptr)|
||||FCompletion.field (Ptr)|
||||FCtype.c_field (Ptrary)|
||||FFalias.p_srcfield (Upptr)|
||||FFalias.p_field (Upptr)|
||||FFloadtuples.p_field (Upptr)|
|[acr_compl.FFinsertwhen](#acr_compl-ffinsertwhen)|[dmmeta.finsertwhen](/txt/ssimdb/dmmeta/finsertwhen.md)|FDb.finsertwhen (Lary)|finsertwhen (Lary, by rowid)|ind_finsertwhen (Thash, hash field finsertwhen)|
|[acr_compl.FFloadtuples](#acr_compl-ffloadtuples)|[dmmeta.floadtuples](/txt/ssimdb/dmmeta/floadtuples.md)|FDb.floadtuples (Lary)|floadtuples (Lary, by rowid)|ind_floadtuples (Thash, hash field field)|
||||FCtype.c_floadtuples (Ptrary)|
|[acr_compl.FSsimfile](#acr_compl-fssimfile)|[dmmeta.ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|FDb.ssimfile (Lary)|ssimfile (Lary, by rowid)|ind_ssimfile (Thash, hash field ssimfile)|
||||FCtype.c_ssimfile (Ptr)|
|[acr_compl.FSubstr](#acr_compl-fsubstr)|[dmmeta.substr](/txt/ssimdb/dmmeta/substr.md)|FDb.substr (Lary)|substr (Lary, by rowid)|
|[acr_compl.Shellqtype](#acr_compl-shellqtype)||
|[acr_compl.checkerr](#acr_compl-checkerr)||
|[acr_compl.checkreq](#acr_compl-checkreq)||

#### acr_compl.Badness - Trading Session
<a href="#acr_compl-badness"></a>

#### acr_compl.Badness Fields
<a href="#acr_compl-badness-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_compl.Badness.badness|u8|[Val](/txt/exe/amc/reftype.md#val)|||
|acr_compl.Badness.strkey|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|||

#### Struct Badness
<a href="#struct-badness"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_compl_gen.h](/include/gen/acr_compl_gen.h)
```
struct Badness { // acr_compl.Badness: Trading Session
    u8              badness;   //   0
    algo::cstring   strkey;    //
    // func:acr_compl.Badness.badness.Cast
    inline               operator acr_compl_BadnessEnum() const __attribute__((nothrow));
    // func:acr_compl.Badness..EqOp
    inline bool          operator ==(const acr_compl::Badness &rhs) const __attribute__((nothrow));
    // func:acr_compl.Badness..NeOp
    inline bool          operator !=(const acr_compl::Badness &rhs) const __attribute__((nothrow));
    // func:acr_compl.Badness..LtOp
    inline bool          operator <(const acr_compl::Badness &rhs) const __attribute__((nothrow));
    // func:acr_compl.Badness..GtOp
    inline bool          operator >(const acr_compl::Badness &rhs) const __attribute__((nothrow));
    // func:acr_compl.Badness..LeOp
    inline bool          operator <=(const acr_compl::Badness &rhs) const __attribute__((nothrow));
    // func:acr_compl.Badness..GeOp
    inline bool          operator >=(const acr_compl::Badness &rhs) const __attribute__((nothrow));
    // func:acr_compl.Badness..Ctor
    inline               Badness() __attribute__((nothrow));
    // func:acr_compl.Badness..FieldwiseCtor
    explicit inline               Badness(u8 in_badness, const algo::strptr& in_strkey) __attribute__((nothrow));
};
```

#### acr_compl.Completion
<a href="#acr_compl-completion"></a>

#### acr_compl.Completion Fields
<a href="#acr_compl-completion-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_compl.Completion.value|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|||
|acr_compl.Completion.nospace|bool|[Val](/txt/exe/amc/reftype.md#val)||do not add space after the value|

#### Struct Completion
<a href="#struct-completion"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_compl_gen.h](/include/gen/acr_compl_gen.h)
```
struct Completion { // acr_compl.Completion
    algo::cstring   value;     //
    bool            nospace;   //   false  do not add space after the value
    // func:acr_compl.Completion..Ctor
    inline               Completion() __attribute__((nothrow));
};
```

#### acr_compl.FAnonfld - Omit field name where possible (command line, enums, constants)
<a href="#acr_compl-fanonfld"></a>

#### acr_compl.FAnonfld Fields
<a href="#acr_compl-fanonfld-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_compl.FAnonfld.msghdr|[dmmeta.Anonfld](/txt/ssimdb/dmmeta/anonfld.md)|[Base](/txt/ssimdb/dmmeta/anonfld.md)|||

#### Struct FAnonfld
<a href="#struct-fanonfld"></a>
*Note:* field ``acr_compl.FAnonfld.msghdr`` has reftype ``base`` so the fields of [dmmeta.Anonfld](/txt/ssimdb/dmmeta/anonfld.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_compl_gen.h](/include/gen/acr_compl_gen.h)
```
struct FAnonfld { // acr_compl.FAnonfld
    algo::Smallstr150   field;     //
    algo::cstring       comment;   //
    // func:acr_compl.FAnonfld..AssignOp
    inline acr_compl::FAnonfld& operator =(const acr_compl::FAnonfld &rhs) = delete;
    // func:acr_compl.FAnonfld..CopyCtor
    inline               FAnonfld(const acr_compl::FAnonfld &rhs) = delete;
private:
    // func:acr_compl.FAnonfld..Ctor
    inline               FAnonfld() __attribute__((nothrow));
    // func:acr_compl.FAnonfld..Dtor
    inline               ~FAnonfld() __attribute__((nothrow));
    friend acr_compl::FAnonfld& anonfld_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_compl::FAnonfld* anonfld_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 anonfld_RemoveLast() __attribute__((nothrow));
};
```

#### acr_compl.FArgvtype - Customize parsing of command lines (rarely used)
<a href="#acr_compl-fargvtype"></a>

#### acr_compl.FArgvtype Fields
<a href="#acr_compl-fargvtype-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_compl.FArgvtype.base|[dmmeta.Argvtype](/txt/ssimdb/dmmeta/argvtype.md)|[Base](/txt/ssimdb/dmmeta/argvtype.md)|||

#### Struct FArgvtype
<a href="#struct-fargvtype"></a>
*Note:* field ``acr_compl.FArgvtype.base`` has reftype ``base`` so the fields of [dmmeta.Argvtype](/txt/ssimdb/dmmeta/argvtype.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_compl_gen.h](/include/gen/acr_compl_gen.h)
```
struct FArgvtype { // acr_compl.FArgvtype
    algo::Smallstr100   ctype;      //
    algo::Smallstr50    argvtype;   //
    algo::cstring       comment;    //
    // func:acr_compl.FArgvtype..AssignOp
    inline acr_compl::FArgvtype& operator =(const acr_compl::FArgvtype &rhs) = delete;
    // func:acr_compl.FArgvtype..CopyCtor
    inline               FArgvtype(const acr_compl::FArgvtype &rhs) = delete;
private:
    // func:acr_compl.FArgvtype..Ctor
    inline               FArgvtype() __attribute__((nothrow));
    // func:acr_compl.FArgvtype..Dtor
    inline               ~FArgvtype() __attribute__((nothrow));
    friend acr_compl::FArgvtype& argvtype_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_compl::FArgvtype* argvtype_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 argvtype_RemoveAll() __attribute__((nothrow));
    friend void                 argvtype_RemoveLast() __attribute__((nothrow));
};
```

#### acr_compl.FCcmdline - tie cmdline to basecmdline
<a href="#acr_compl-fccmdline"></a>

#### acr_compl.FCcmdline Fields
<a href="#acr_compl-fccmdline-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_compl.FCcmdline.base|[dmmeta.Ccmdline](/txt/ssimdb/dmmeta/ccmdline.md)|[Base](/txt/ssimdb/dmmeta/ccmdline.md)|||
|acr_compl.FCcmdline.p_ctype|[acr_compl.FCtype](/txt/gen/acr_compl/acr_compl.md#acr_compl-fctype)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|acr_compl.FCcmdline.p_basecmdline|[acr_compl.FField](/txt/gen/acr_compl/acr_compl.md#acr_compl-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||

#### Struct FCcmdline
<a href="#struct-fccmdline"></a>
*Note:* field ``acr_compl.FCcmdline.base`` has reftype ``base`` so the fields of [dmmeta.Ccmdline](/txt/ssimdb/dmmeta/ccmdline.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_compl_gen.h](/include/gen/acr_compl_gen.h)
```
struct FCcmdline { // acr_compl.FCcmdline
    algo::Smallstr100    ctype;           //
    bool                 read;            //   true  Read the command line automatically from main?
    algo::Smallstr150    basecmdline;     // Optional command line to merge with this one
    algo::cstring        comment;         //
    acr_compl::FCtype*   p_ctype;         // reference to parent row
    acr_compl::FField*   p_basecmdline;   // reference to parent row
private:
    // func:acr_compl.FCcmdline..Ctor
    inline               FCcmdline() __attribute__((nothrow));
    friend acr_compl::FCcmdline& ccmdline_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_compl::FCcmdline* ccmdline_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 ccmdline_RemoveAll() __attribute__((nothrow));
    friend void                 ccmdline_RemoveLast() __attribute__((nothrow));
};
```

#### acr_compl.FCmd - commands that can autocomplete
<a href="#acr_compl-fcmd"></a>

#### acr_compl.FCmd Fields
<a href="#acr_compl-fcmd-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_compl.FCmd.cmd|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|||
|acr_compl.FCmd.p_ctype|[acr_compl.FCtype](/txt/gen/acr_compl/acr_compl.md#acr_compl-fctype)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|acr_compl.FCmd.p_basectype|[acr_compl.FCtype](/txt/gen/acr_compl/acr_compl.md#acr_compl-fctype)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||

#### Struct FCmd
<a href="#struct-fcmd"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_compl_gen.h](/include/gen/acr_compl_gen.h)
```
struct FCmd { // acr_compl.FCmd: commands that can autocomplete
    algo::cstring        cmd;               //
    acr_compl::FCtype*   p_ctype;           // reference to parent row
    acr_compl::FCtype*   p_basectype;       // reference to parent row
    acr_compl::FCmd*     ind_cmd_next;      // hash next
    u32                  ind_cmd_hashval;   // hash value
    // func:acr_compl.FCmd..AssignOp
    inline acr_compl::FCmd& operator =(const acr_compl::FCmd &rhs) = delete;
    // func:acr_compl.FCmd..CopyCtor
    inline               FCmd(const acr_compl::FCmd &rhs) = delete;
private:
    // func:acr_compl.FCmd..Ctor
    inline               FCmd() __attribute__((nothrow));
    // func:acr_compl.FCmd..Dtor
    inline               ~FCmd() __attribute__((nothrow));
    friend acr_compl::FCmd&     cmd_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_compl::FCmd*     cmd_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 cmd_RemoveAll() __attribute__((nothrow));
    friend void                 cmd_RemoveLast() __attribute__((nothrow));
};
```

#### acr_compl.FCompletion
<a href="#acr_compl-fcompletion"></a>

#### acr_compl.FCompletion Fields
<a href="#acr_compl-fcompletion-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_compl.FCompletion.msghdr|[acr_compl.Completion](/txt/gen/acr_compl/acr_compl.md#acr_compl-completion)|[Base](#acr_compl-completion-fields)|||
|acr_compl.FCompletion.badness|[acr_compl.Badness](/txt/gen/acr_compl/acr_compl.md#acr_compl-badness)|[Val](/txt/exe/amc/reftype.md#val)|||
|acr_compl.FCompletion.field|[acr_compl.FField](/txt/gen/acr_compl/acr_compl.md#acr_compl-ffield)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
#### acr_compl.Completion Fields
<a href="#acr_compl-completion-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_compl.Completion.value|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|||
|acr_compl.Completion.nospace|bool|[Val](/txt/exe/amc/reftype.md#val)||do not add space after the value|

#### Struct FCompletion
<a href="#struct-fcompletion"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_compl_gen.h](/include/gen/acr_compl_gen.h)
```
struct FCompletion { // acr_compl.FCompletion
    algo::cstring        value;               //
    bool                 nospace;             //   false  do not add space after the value
    acr_compl::Badness   badness;             //
    acr_compl::FField*   field;               // optional pointer
    i32                  bh_completion_idx;   // index in heap; -1 means not-in-heap
    // func:acr_compl.FCompletion..AssignOp
    inline acr_compl::FCompletion& operator =(const acr_compl::FCompletion &rhs) = delete;
    // func:acr_compl.FCompletion..CopyCtor
    inline               FCompletion(const acr_compl::FCompletion &rhs) = delete;
private:
    // func:acr_compl.FCompletion..Ctor
    inline               FCompletion() __attribute__((nothrow));
    // func:acr_compl.FCompletion..Dtor
    inline               ~FCompletion() __attribute__((nothrow));
    friend acr_compl::FCompletion& completion_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_compl::FCompletion* completion_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 completion_RemoveAll() __attribute__((nothrow));
    friend void                 completion_RemoveLast() __attribute__((nothrow));
};
```

#### acr_compl.FComplsource
<a href="#acr_compl-fcomplsource"></a>

#### acr_compl.FComplsource Fields
<a href="#acr_compl-fcomplsource-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_compl.FComplsource.tuple|[algo.Tuple](/txt/protocol/algo/Tuple.md)|[Val](/txt/exe/amc/reftype.md#val)|||

#### Struct FComplsource
<a href="#struct-fcomplsource"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_compl_gen.h](/include/gen/acr_compl_gen.h)
```
struct FComplsource { // acr_compl.FComplsource
    algo::Tuple   tuple;   //
private:
    // func:acr_compl.FComplsource..Ctor
    inline               FComplsource() __attribute__((nothrow));
    friend acr_compl::FComplsource& complsource_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_compl::FComplsource* complsource_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 complsource_RemoveAll() __attribute__((nothrow));
    friend void                 complsource_RemoveLast() __attribute__((nothrow));
};
```

#### acr_compl.FCtype - Struct
<a href="#acr_compl-fctype"></a>

#### acr_compl.FCtype Fields
<a href="#acr_compl-fctype-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_compl.FCtype.msghdr|[dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md)|[Base](/txt/ssimdb/dmmeta/ctype.md)|||
|acr_compl.FCtype.c_field|[acr_compl.FField](/txt/gen/acr_compl/acr_compl.md#acr_compl-ffield)|[Ptrary](/txt/exe/amc/reftype.md#ptrary)|||
|acr_compl.FCtype.c_ssimfile|[acr_compl.FSsimfile](/txt/gen/acr_compl/acr_compl.md#acr_compl-fssimfile)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|acr_compl.FCtype.c_argvtype|[acr_compl.FArgvtype](/txt/gen/acr_compl/acr_compl.md#acr_compl-fargvtype)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|acr_compl.FCtype.c_floadtuples|[acr_compl.FFloadtuples](/txt/gen/acr_compl/acr_compl.md#acr_compl-ffloadtuples)|[Ptrary](/txt/exe/amc/reftype.md#ptrary)||tuple sources this command loads at startup|

#### Struct FCtype
<a href="#struct-fctype"></a>
*Note:* field ``acr_compl.FCtype.msghdr`` has reftype ``base`` so the fields of [dmmeta.Ctype](/txt/ssimdb/dmmeta/ctype.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_compl_gen.h](/include/gen/acr_compl_gen.h)
```
struct FCtype { // acr_compl.FCtype
    algo::Smallstr100           ctype;                 // Identifier. must be ns.typename
    algo::cstring               comment;               //
    acr_compl::FField**         c_field_elems;         // array of pointers
    u64                         c_field_n;             // current size
    u64                         c_field_max;           // capacity of allocated array
    acr_compl::FSsimfile*       c_ssimfile;            // optional pointer
    acr_compl::FArgvtype*       c_argvtype;            // optional pointer
    acr_compl::FFloadtuples**   c_floadtuples_elems;   // array of pointers
    u64                         c_floadtuples_n;       // current size
    u64                         c_floadtuples_max;     // capacity of allocated array
    acr_compl::FCtype*          ind_ctype_next;        // hash next
    u32                         ind_ctype_hashval;     // hash value
    // reftype Ptrary of acr_compl.FCtype.c_field prohibits copy
    // x-reference on acr_compl.FCtype.c_ssimfile prevents copy
    // x-reference on acr_compl.FCtype.c_argvtype prevents copy
    // reftype Ptrary of acr_compl.FCtype.c_floadtuples prohibits copy
    // func:acr_compl.FCtype..AssignOp
    inline acr_compl::FCtype& operator =(const acr_compl::FCtype &rhs) = delete;
    // reftype Ptrary of acr_compl.FCtype.c_field prohibits copy
    // x-reference on acr_compl.FCtype.c_ssimfile prevents copy
    // x-reference on acr_compl.FCtype.c_argvtype prevents copy
    // reftype Ptrary of acr_compl.FCtype.c_floadtuples prohibits copy
    // func:acr_compl.FCtype..CopyCtor
    inline               FCtype(const acr_compl::FCtype &rhs) = delete;
private:
    // func:acr_compl.FCtype..Ctor
    inline               FCtype() __attribute__((nothrow));
    // func:acr_compl.FCtype..Dtor
    inline               ~FCtype() __attribute__((nothrow));
    friend acr_compl::FCtype&   ctype_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_compl::FCtype*   ctype_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 ctype_RemoveLast() __attribute__((nothrow));
};
```

#### acr_compl.FDb - In-memory database for acr_compl
<a href="#acr_compl-fdb"></a>

#### acr_compl.FDb Fields
<a href="#acr_compl-fdb-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_compl.FDb._db|[acr_compl.FDb](/txt/gen/acr_compl/acr_compl.md#acr_compl-fdb)|[Global](/txt/exe/amc/reftype.md#global)|||
|acr_compl.FDb.cmdline|[command.acr_compl](/txt/protocol/command/README.md#command-acr_compl)|[Val](/txt/exe/amc/reftype.md#val)|||
|acr_compl.FDb.point|i32|[Val](/txt/exe/amc/reftype.md#val)|||
|acr_compl.FDb.line|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|||
|acr_compl.FDb.word|[algo.cstring](/txt/protocol/algo/cstring.md)|[Tary](/txt/exe/amc/reftype.md#tary)|||
|acr_compl.FDb.ctype|[acr_compl.FCtype](/txt/gen/acr_compl/acr_compl.md#acr_compl-fctype)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|acr_compl.FDb.ind_ctype|[acr_compl.FCtype](/txt/gen/acr_compl/acr_compl.md#acr_compl-fctype)|[Thash](/txt/exe/amc/reftype.md#thash)|||
|acr_compl.FDb.field|[acr_compl.FField](/txt/gen/acr_compl/acr_compl.md#acr_compl-ffield)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|acr_compl.FDb.ind_field|[acr_compl.FField](/txt/gen/acr_compl/acr_compl.md#acr_compl-ffield)|[Thash](/txt/exe/amc/reftype.md#thash)|||
|acr_compl.FDb.ssimfile|[acr_compl.FSsimfile](/txt/gen/acr_compl/acr_compl.md#acr_compl-fssimfile)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|acr_compl.FDb.ind_ssimfile|[acr_compl.FSsimfile](/txt/gen/acr_compl/acr_compl.md#acr_compl-fssimfile)|[Thash](/txt/exe/amc/reftype.md#thash)|||
|acr_compl.FDb.completion|[acr_compl.FCompletion](/txt/gen/acr_compl/acr_compl.md#acr_compl-fcompletion)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|acr_compl.FDb.anonfld|[acr_compl.FAnonfld](/txt/gen/acr_compl/acr_compl.md#acr_compl-fanonfld)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|acr_compl.FDb.bh_completion|[acr_compl.FCompletion](/txt/gen/acr_compl/acr_compl.md#acr_compl-fcompletion)|[Bheap](/txt/exe/amc/reftype.md#bheap)|||
|acr_compl.FDb.fconst|[acr_compl.FFconst](/txt/gen/acr_compl/acr_compl.md#acr_compl-ffconst)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|acr_compl.FDb.fflag|[acr_compl.FFflag](/txt/gen/acr_compl/acr_compl.md#acr_compl-ffflag)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|acr_compl.FDb.zd_cmd_field|[acr_compl.FField](/txt/gen/acr_compl/acr_compl.md#acr_compl-ffield)|[Llist](/txt/exe/amc/reftype.md#llist)|||
|acr_compl.FDb.ind_cmd_field_name|[acr_compl.FField](/txt/gen/acr_compl/acr_compl.md#acr_compl-ffield)|[Thash](/txt/exe/amc/reftype.md#thash)|||
|acr_compl.FDb.name|[algo.strptr](/txt/protocol/algo/strptr.md)|[Val](/txt/exe/amc/reftype.md#val)||option name|
|acr_compl.FDb.value|[algo.strptr](/txt/protocol/algo/strptr.md)|[Val](/txt/exe/amc/reftype.md#val)||option value|
|acr_compl.FDb.need_value|bool|[Val](/txt/exe/amc/reftype.md#val)||next word is value|
|acr_compl.FDb.anon_index|i32|[Val](/txt/exe/amc/reftype.md#val)||current index of anon argument|
|acr_compl.FDb.exact|bool|[Val](/txt/exe/amc/reftype.md#val)||whether option name is considered as exact|
|acr_compl.FDb.anon|bool|[Val](/txt/exe/amc/reftype.md#val)|||
|acr_compl.FDb.argvtype|[acr_compl.FArgvtype](/txt/gen/acr_compl/acr_compl.md#acr_compl-fargvtype)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|acr_compl.FDb.falias|[acr_compl.FFalias](/txt/gen/acr_compl/acr_compl.md#acr_compl-ffalias)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|acr_compl.FDb.complsource|[acr_compl.FComplsource](/txt/gen/acr_compl/acr_compl.md#acr_compl-fcomplsource)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|acr_compl.FDb.is_data_dir|bool|[Val](/txt/exe/amc/reftype.md#val)||Input is a directory of ssimfiles|
|acr_compl.FDb.parse_error|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|""|Error from command line parsing|
|acr_compl.FDb.compl_output|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)|""|Collected completion output|
|acr_compl.FDb.finsertwhen|[acr_compl.FFinsertwhen](/txt/gen/acr_compl/acr_compl.md#acr_compl-ffinsertwhen)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|acr_compl.FDb.ind_finsertwhen|[acr_compl.FFinsertwhen](/txt/gen/acr_compl/acr_compl.md#acr_compl-ffinsertwhen)|[Thash](/txt/exe/amc/reftype.md#thash)|||
|acr_compl.FDb.substr|[acr_compl.FSubstr](/txt/gen/acr_compl/acr_compl.md#acr_compl-fsubstr)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|acr_compl.FDb.ccmdline|[acr_compl.FCcmdline](/txt/gen/acr_compl/acr_compl.md#acr_compl-fccmdline)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|acr_compl.FDb.cmd|[acr_compl.FCmd](/txt/gen/acr_compl/acr_compl.md#acr_compl-fcmd)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|acr_compl.FDb.ind_cmd|[acr_compl.FCmd](/txt/gen/acr_compl/acr_compl.md#acr_compl-fcmd)|[Thash](/txt/exe/amc/reftype.md#thash)|||
|acr_compl.FDb.floadtuples|[acr_compl.FFloadtuples](/txt/gen/acr_compl/acr_compl.md#acr_compl-ffloadtuples)|[Lary](/txt/exe/amc/reftype.md#lary)|||
|acr_compl.FDb.ind_floadtuples|[acr_compl.FFloadtuples](/txt/gen/acr_compl/acr_compl.md#acr_compl-ffloadtuples)|[Thash](/txt/exe/amc/reftype.md#thash)|||
|acr_compl.FDb.p_cmd_ctype|[acr_compl.FCtype](/txt/gen/acr_compl/acr_compl.md#acr_compl-fctype)|[Ptr](/txt/exe/amc/reftype.md#ptr)||cmdline ctype of the command being completed|

#### Struct FDb
<a href="#struct-fdb"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_compl_gen.h](/include/gen/acr_compl_gen.h)
```
struct FDb { // acr_compl.FDb: In-memory database for acr_compl
    command::acr_compl          cmdline;                            //
    i32                         point;                              //   0
    algo::cstring               line;                               //
    algo::cstring*              word_elems;                         // pointer to elements
    u64                         word_n;                             // number of elements in array
    u64                         word_max;                           // max. capacity of array before realloc
    acr_compl::FCtype*          ctype_lary[36];                     // level array
    i64                         ctype_n;                            // number of elements in array
    acr_compl::FCtype**         ind_ctype_buckets_elems;            // pointer to bucket array
    i32                         ind_ctype_buckets_n;                // number of elements in bucket array
    i32                         ind_ctype_n;                        // number of elements in the hash table
    acr_compl::FField*          field_lary[36];                     // level array
    i64                         field_n;                            // number of elements in array
    acr_compl::FField**         ind_field_buckets_elems;            // pointer to bucket array
    i32                         ind_field_buckets_n;                // number of elements in bucket array
    i32                         ind_field_n;                        // number of elements in the hash table
    acr_compl::FSsimfile*       ssimfile_lary[36];                  // level array
    i64                         ssimfile_n;                         // number of elements in array
    acr_compl::FSsimfile**      ind_ssimfile_buckets_elems;         // pointer to bucket array
    i32                         ind_ssimfile_buckets_n;             // number of elements in bucket array
    i32                         ind_ssimfile_n;                     // number of elements in the hash table
    acr_compl::FCompletion*     completion_lary[36];                // level array
    i64                         completion_n;                       // number of elements in array
    acr_compl::FAnonfld*        anonfld_lary[36];                   // level array
    i64                         anonfld_n;                          // number of elements in array
    acr_compl::FCompletion**    bh_completion_elems;                // binary heap by badness
    i32                         bh_completion_n;                    // number of elements in the heap
    i32                         bh_completion_max;                  // max elements in bh_completion_elems
    acr_compl::FFconst*         fconst_lary[36];                    // level array
    i64                         fconst_n;                           // number of elements in array
    acr_compl::FFflag*          fflag_lary[36];                     // level array
    i64                         fflag_n;                            // number of elements in array
    acr_compl::FField*          zd_cmd_field_head;                  // zero-terminated doubly linked list
    i32                         zd_cmd_field_n;                     // zero-terminated doubly linked list
    acr_compl::FField*          zd_cmd_field_tail;                  // pointer to last element
    acr_compl::FField**         ind_cmd_field_name_buckets_elems;   // pointer to bucket array
    i32                         ind_cmd_field_name_buckets_n;       // number of elements in bucket array
    i32                         ind_cmd_field_name_n;               // number of elements in the hash table
    algo::strptr                name;                               // option name
    algo::strptr                value;                              // option value
    bool                        need_value;                         //   false  next word is value
    i32                         anon_index;                         //   0  current index of anon argument
    bool                        exact;                              //   false  whether option name is considered as exact
    bool                        anon;                               //   false
    acr_compl::FArgvtype*       argvtype_lary[36];                  // level array
    i64                         argvtype_n;                         // number of elements in array
    acr_compl::FFalias*         falias_lary[36];                    // level array
    i64                         falias_n;                           // number of elements in array
    acr_compl::FComplsource*    complsource_lary[36];               // level array
    i64                         complsource_n;                      // number of elements in array
    bool                        is_data_dir;                        //   false  Input is a directory of ssimfiles
    algo::cstring               parse_error;                        //   ""  Error from command line parsing
    algo::cstring               compl_output;                       //   ""  Collected completion output
    acr_compl::FFinsertwhen*    finsertwhen_lary[36];               // level array
    i64                         finsertwhen_n;                      // number of elements in array
    acr_compl::FFinsertwhen**   ind_finsertwhen_buckets_elems;      // pointer to bucket array
    i32                         ind_finsertwhen_buckets_n;          // number of elements in bucket array
    i32                         ind_finsertwhen_n;                  // number of elements in the hash table
    acr_compl::FSubstr*         substr_lary[36];                    // level array
    i64                         substr_n;                           // number of elements in array
    acr_compl::FCcmdline*       ccmdline_lary[36];                  // level array
    i64                         ccmdline_n;                         // number of elements in array
    acr_compl::FCmd*            cmd_lary[36];                       // level array
    i64                         cmd_n;                              // number of elements in array
    acr_compl::FCmd**           ind_cmd_buckets_elems;              // pointer to bucket array
    i32                         ind_cmd_buckets_n;                  // number of elements in bucket array
    i32                         ind_cmd_n;                          // number of elements in the hash table
    acr_compl::FFloadtuples*    floadtuples_lary[36];               // level array
    i64                         floadtuples_n;                      // number of elements in array
    acr_compl::FFloadtuples**   ind_floadtuples_buckets_elems;      // pointer to bucket array
    i32                         ind_floadtuples_buckets_n;          // number of elements in bucket array
    i32                         ind_floadtuples_n;                  // number of elements in the hash table
    acr_compl::FCtype*          p_cmd_ctype;                        // cmdline ctype of the command being completed. optional pointer
    acr_compl::trace            trace;                              //
};
```

#### acr_compl.FFalias - Alias field
<a href="#acr_compl-ffalias"></a>

#### acr_compl.FFalias Fields
<a href="#acr_compl-ffalias-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_compl.FFalias.base|[dmmeta.Falias](/txt/ssimdb/dmmeta/falias.md)|[Base](/txt/ssimdb/dmmeta/falias.md)|||
|acr_compl.FFalias.p_srcfield|[acr_compl.FField](/txt/gen/acr_compl/acr_compl.md#acr_compl-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|acr_compl.FFalias.p_field|[acr_compl.FField](/txt/gen/acr_compl/acr_compl.md#acr_compl-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||

#### Struct FFalias
<a href="#struct-ffalias"></a>
*Note:* field ``acr_compl.FFalias.base`` has reftype ``base`` so the fields of [dmmeta.Falias](/txt/ssimdb/dmmeta/falias.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_compl_gen.h](/include/gen/acr_compl_gen.h)
```
struct FFalias { // acr_compl.FFalias
    algo::Smallstr150    field;        //
    algo::Smallstr150    srcfield;     //
    algo::cstring        comment;      //
    acr_compl::FField*   p_srcfield;   // reference to parent row
    acr_compl::FField*   p_field;      // reference to parent row
    // x-reference on acr_compl.FFalias.p_srcfield prevents copy
    // x-reference on acr_compl.FFalias.p_field prevents copy
    // func:acr_compl.FFalias..AssignOp
    inline acr_compl::FFalias& operator =(const acr_compl::FFalias &rhs) = delete;
    // x-reference on acr_compl.FFalias.p_srcfield prevents copy
    // x-reference on acr_compl.FFalias.p_field prevents copy
    // func:acr_compl.FFalias..CopyCtor
    inline               FFalias(const acr_compl::FFalias &rhs) = delete;
private:
    // func:acr_compl.FFalias..Ctor
    inline               FFalias() __attribute__((nothrow));
    // func:acr_compl.FFalias..Dtor
    inline               ~FFalias() __attribute__((nothrow));
    friend acr_compl::FFalias&  falias_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_compl::FFalias*  falias_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 falias_RemoveAll() __attribute__((nothrow));
    friend void                 falias_RemoveLast() __attribute__((nothrow));
};
```

#### acr_compl.FFconst - Specify enum value (integer + string constant) for a field
<a href="#acr_compl-ffconst"></a>

#### acr_compl.FFconst Fields
<a href="#acr_compl-ffconst-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_compl.FFconst.base|[dmmeta.Fconst](/txt/ssimdb/dmmeta/fconst.md)|[Base](/txt/ssimdb/dmmeta/fconst.md)|||

#### Struct FFconst
<a href="#struct-ffconst"></a>
*Note:* field ``acr_compl.FFconst.base`` has reftype ``base`` so the fields of [dmmeta.Fconst](/txt/ssimdb/dmmeta/fconst.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_compl_gen.h](/include/gen/acr_compl_gen.h)
```
struct FFconst { // acr_compl.FFconst
    algo::Smallstr150   fconst;                  //
    algo::CppExpr       value;                   //
    algo::cstring       comment;                 //
    bool                field_c_fconst_in_ary;   //   false  membership flag
    // func:acr_compl.FFconst..AssignOp
    inline acr_compl::FFconst& operator =(const acr_compl::FFconst &rhs) = delete;
    // func:acr_compl.FFconst..CopyCtor
    inline               FFconst(const acr_compl::FFconst &rhs) = delete;
private:
    // func:acr_compl.FFconst..Ctor
    inline               FFconst() __attribute__((nothrow));
    // func:acr_compl.FFconst..Dtor
    inline               ~FFconst() __attribute__((nothrow));
    friend acr_compl::FFconst&  fconst_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_compl::FFconst*  fconst_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 fconst_RemoveAll() __attribute__((nothrow));
    friend void                 fconst_RemoveLast() __attribute__((nothrow));
};
```

#### acr_compl.FFflag - Options for command-line flags
<a href="#acr_compl-ffflag"></a>

#### acr_compl.FFflag Fields
<a href="#acr_compl-ffflag-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_compl.FFflag.base|[dmmeta.Fflag](/txt/ssimdb/dmmeta/fflag.md)|[Base](/txt/ssimdb/dmmeta/fflag.md)|||

#### Struct FFflag
<a href="#struct-ffflag"></a>
*Note:* field ``acr_compl.FFflag.base`` has reftype ``base`` so the fields of [dmmeta.Fflag](/txt/ssimdb/dmmeta/fflag.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_compl_gen.h](/include/gen/acr_compl_gen.h)
```
struct FFflag { // acr_compl.FFflag
    algo::Smallstr150   field;        //
    bool                cumulative;   //   false  Accumulate on every read
    algo::cstring       emptyval;     //   ""
    algo::cstring       comment;      //
    // func:acr_compl.FFflag..AssignOp
    inline acr_compl::FFflag& operator =(const acr_compl::FFflag &rhs) = delete;
    // func:acr_compl.FFflag..CopyCtor
    inline               FFflag(const acr_compl::FFflag &rhs) = delete;
private:
    // func:acr_compl.FFflag..Ctor
    inline               FFflag() __attribute__((nothrow));
    // func:acr_compl.FFflag..Dtor
    inline               ~FFflag() __attribute__((nothrow));
    friend acr_compl::FFflag&   fflag_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_compl::FFflag*   fflag_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 fflag_RemoveAll() __attribute__((nothrow));
    friend void                 fflag_RemoveLast() __attribute__((nothrow));
};
```

#### acr_compl.FField - Specify field of a struct
<a href="#acr_compl-ffield"></a>

#### acr_compl.FField Fields
<a href="#acr_compl-ffield-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_compl.FField.msghdr|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Base](/txt/ssimdb/dmmeta/field.md)|||
|acr_compl.FField.p_arg|[acr_compl.FCtype](/txt/gen/acr_compl/acr_compl.md#acr_compl-fctype)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|acr_compl.FField.c_anonfld|[acr_compl.FAnonfld](/txt/gen/acr_compl/acr_compl.md#acr_compl-fanonfld)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|acr_compl.FField.c_fconst|[acr_compl.FFconst](/txt/gen/acr_compl/acr_compl.md#acr_compl-ffconst)|[Ptrary](/txt/exe/amc/reftype.md#ptrary)|||
|acr_compl.FField.seen|bool|[Val](/txt/exe/amc/reftype.md#val)|||
|acr_compl.FField.c_fflag|[acr_compl.FFflag](/txt/gen/acr_compl/acr_compl.md#acr_compl-ffflag)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|acr_compl.FField.p_ctype|[acr_compl.FCtype](/txt/gen/acr_compl/acr_compl.md#acr_compl-fctype)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||
|acr_compl.FField.c_falias|[acr_compl.FFalias](/txt/gen/acr_compl/acr_compl.md#acr_compl-ffalias)|[Ptr](/txt/exe/amc/reftype.md#ptr)|||
|acr_compl.FField.c_falias_srcfield|[acr_compl.FFalias](/txt/gen/acr_compl/acr_compl.md#acr_compl-ffalias)|[Ptrary](/txt/exe/amc/reftype.md#ptrary)|||
|acr_compl.FField.typed_value|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||value supplied for this option on the line being completed|

#### Struct FField
<a href="#struct-ffield"></a>
*Note:* field ``acr_compl.FField.msghdr`` has reftype ``base`` so the fields of [dmmeta.Field](/txt/ssimdb/dmmeta/field.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_compl_gen.h](/include/gen/acr_compl_gen.h)
```
struct FField { // acr_compl.FField
    acr_compl::FField*     ind_field_next;               // hash next
    u32                    ind_field_hashval;            // hash value
    acr_compl::FField*     zd_cmd_field_next;            // zslist link; -1 means not-in-list
    acr_compl::FField*     zd_cmd_field_prev;            // previous element
    acr_compl::FField*     ind_cmd_field_name_next;      // hash next
    u32                    ind_cmd_field_name_hashval;   // hash value
    algo::Smallstr150      field;                        // Primary key, as ctype.name
    algo::Smallstr100      arg;                          // Type of field
    algo::Smallstr50       reftype;                      //   "Val"  Type constructor
    algo::CppExpr          dflt;                         // Default value (c++ expression)
    algo::cstring          comment;                      //
    acr_compl::FCtype*     p_arg;                        // reference to parent row
    acr_compl::FAnonfld*   c_anonfld;                    // optional pointer
    acr_compl::FFconst**   c_fconst_elems;               // array of pointers
    u64                    c_fconst_n;                   // current size
    u64                    c_fconst_max;                 // capacity of allocated array
    bool                   seen;                         //   false
    acr_compl::FFflag*     c_fflag;                      // optional pointer
    acr_compl::FCtype*     p_ctype;                      // reference to parent row
    acr_compl::FFalias*    c_falias;                     // optional pointer
    acr_compl::FFalias**   c_falias_srcfield_elems;      // array of pointers
    u64                    c_falias_srcfield_n;          // current size
    u64                    c_falias_srcfield_max;        // capacity of allocated array
    algo::cstring          typed_value;                  // value supplied for this option on the line being completed
    bool                   ctype_c_field_in_ary;         //   false  membership flag
    // x-reference on acr_compl.FField.p_arg prevents copy
    // x-reference on acr_compl.FField.c_anonfld prevents copy
    // reftype Ptrary of acr_compl.FField.c_fconst prohibits copy
    // x-reference on acr_compl.FField.c_fflag prevents copy
    // x-reference on acr_compl.FField.p_ctype prevents copy
    // x-reference on acr_compl.FField.c_falias prevents copy
    // reftype Ptrary of acr_compl.FField.c_falias_srcfield prohibits copy
    // func:acr_compl.FField..AssignOp
    acr_compl::FField&   operator =(const acr_compl::FField &rhs) = delete;
    // x-reference on acr_compl.FField.p_arg prevents copy
    // x-reference on acr_compl.FField.c_anonfld prevents copy
    // reftype Ptrary of acr_compl.FField.c_fconst prohibits copy
    // x-reference on acr_compl.FField.c_fflag prevents copy
    // x-reference on acr_compl.FField.p_ctype prevents copy
    // x-reference on acr_compl.FField.c_falias prevents copy
    // reftype Ptrary of acr_compl.FField.c_falias_srcfield prohibits copy
    // func:acr_compl.FField..CopyCtor
    FField(const acr_compl::FField &rhs) = delete;
private:
    // func:acr_compl.FField..Ctor
    inline               FField() __attribute__((nothrow));
    // func:acr_compl.FField..Dtor
    inline               ~FField() __attribute__((nothrow));
    friend acr_compl::FField&   field_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_compl::FField*   field_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 field_RemoveLast() __attribute__((nothrow));
};
```

#### acr_compl.FFinsertwhen - Switch -<flag> to insert-completion mode when <when> flag is also set
<a href="#acr_compl-ffinsertwhen"></a>

#### acr_compl.FFinsertwhen Fields
<a href="#acr_compl-ffinsertwhen-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_compl.FFinsertwhen.base|[dmmeta.Finsertwhen](/txt/ssimdb/dmmeta/finsertwhen.md)|[Base](/txt/ssimdb/dmmeta/finsertwhen.md)|||

#### Struct FFinsertwhen
<a href="#struct-ffinsertwhen"></a>
*Note:* field ``acr_compl.FFinsertwhen.base`` has reftype ``base`` so the fields of [dmmeta.Finsertwhen](/txt/ssimdb/dmmeta/finsertwhen.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_compl_gen.h](/include/gen/acr_compl_gen.h)
```
struct FFinsertwhen { // acr_compl.FFinsertwhen
    acr_compl::FFinsertwhen*   ind_finsertwhen_next;      // hash next
    u32                        ind_finsertwhen_hashval;   // hash value
    algo::Smallstr150          finsertwhen;               // Field that gets insert-completion
    algo::Smallstr150          when;                      // When this flag is also set on the cmdline, use insert-completion for finsertwhen.field
    algo::cstring              comment;                   //
    // func:acr_compl.FFinsertwhen..AssignOp
    inline acr_compl::FFinsertwhen& operator =(const acr_compl::FFinsertwhen &rhs) = delete;
    // func:acr_compl.FFinsertwhen..CopyCtor
    inline               FFinsertwhen(const acr_compl::FFinsertwhen &rhs) = delete;
private:
    // func:acr_compl.FFinsertwhen..Ctor
    inline               FFinsertwhen() __attribute__((nothrow));
    // func:acr_compl.FFinsertwhen..Dtor
    inline               ~FFinsertwhen() __attribute__((nothrow));
    friend acr_compl::FFinsertwhen& finsertwhen_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_compl::FFinsertwhen* finsertwhen_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 finsertwhen_RemoveAll() __attribute__((nothrow));
    friend void                 finsertwhen_RemoveLast() __attribute__((nothrow));
};
```

#### acr_compl.FFloadtuples - Request that process automatically load any input tables on startup
<a href="#acr_compl-ffloadtuples"></a>

#### acr_compl.FFloadtuples Fields
<a href="#acr_compl-ffloadtuples-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_compl.FFloadtuples.base|[dmmeta.Floadtuples](/txt/ssimdb/dmmeta/floadtuples.md)|[Base](/txt/ssimdb/dmmeta/floadtuples.md)|||
|acr_compl.FFloadtuples.p_field|[acr_compl.FField](/txt/gen/acr_compl/acr_compl.md#acr_compl-ffield)|[Upptr](/txt/exe/amc/reftype.md#upptr)||the cmdline field naming this source|

#### Struct FFloadtuples
<a href="#struct-ffloadtuples"></a>
*Note:* field ``acr_compl.FFloadtuples.base`` has reftype ``base`` so the fields of [dmmeta.Floadtuples](/txt/ssimdb/dmmeta/floadtuples.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_compl_gen.h](/include/gen/acr_compl_gen.h)
```
struct FFloadtuples { // acr_compl.FFloadtuples
    acr_compl::FFloadtuples*   ind_floadtuples_next;         // hash next
    u32                        ind_floadtuples_hashval;      // hash value
    algo::Smallstr150          field;                        //
    bool                       autoscan;                     //   true  acr_compl scans this location for completion candidates
    bool                       optional;                     //   false  a layer beyond the primary source: absent means skip, not fail
    algo::cstring              comment;                      //
    acr_compl::FField*         p_field;                      // reference to parent row
    bool                       ctype_c_floadtuples_in_ary;   //   false  membership flag
    // x-reference on acr_compl.FFloadtuples.p_field prevents copy
    // func:acr_compl.FFloadtuples..AssignOp
    inline acr_compl::FFloadtuples& operator =(const acr_compl::FFloadtuples &rhs) = delete;
    // x-reference on acr_compl.FFloadtuples.p_field prevents copy
    // func:acr_compl.FFloadtuples..CopyCtor
    inline               FFloadtuples(const acr_compl::FFloadtuples &rhs) = delete;
private:
    // func:acr_compl.FFloadtuples..Ctor
    inline               FFloadtuples() __attribute__((nothrow));
    // func:acr_compl.FFloadtuples..Dtor
    inline               ~FFloadtuples() __attribute__((nothrow));
    friend acr_compl::FFloadtuples& floadtuples_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_compl::FFloadtuples* floadtuples_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 floadtuples_RemoveAll() __attribute__((nothrow));
    friend void                 floadtuples_RemoveLast() __attribute__((nothrow));
};
```

#### acr_compl.FSsimfile - File with ssim tuples
<a href="#acr_compl-fssimfile"></a>

#### acr_compl.FSsimfile Fields
<a href="#acr_compl-fssimfile-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_compl.FSsimfile.msghdr|[dmmeta.Ssimfile](/txt/ssimdb/dmmeta/ssimfile.md)|[Base](/txt/ssimdb/dmmeta/ssimfile.md)|||
|acr_compl.FSsimfile.p_ctype|[acr_compl.FCtype](/txt/gen/acr_compl/acr_compl.md#acr_compl-fctype)|[Upptr](/txt/exe/amc/reftype.md#upptr)|||

#### Struct FSsimfile
<a href="#struct-fssimfile"></a>
*Note:* field ``acr_compl.FSsimfile.msghdr`` has reftype ``base`` so the fields of [dmmeta.Ssimfile](/txt/ssimdb/dmmeta/ssimfile.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_compl_gen.h](/include/gen/acr_compl_gen.h)
```
struct FSsimfile { // acr_compl.FSsimfile
    acr_compl::FSsimfile*   ind_ssimfile_next;      // hash next
    u32                     ind_ssimfile_hashval;   // hash value
    algo::Smallstr50        ssimfile;               //
    algo::Smallstr100       ctype;                  //
    acr_compl::FCtype*      p_ctype;                // reference to parent row
    // x-reference on acr_compl.FSsimfile.p_ctype prevents copy
    // func:acr_compl.FSsimfile..AssignOp
    inline acr_compl::FSsimfile& operator =(const acr_compl::FSsimfile &rhs) = delete;
    // x-reference on acr_compl.FSsimfile.p_ctype prevents copy
    // func:acr_compl.FSsimfile..CopyCtor
    inline               FSsimfile(const acr_compl::FSsimfile &rhs) = delete;
private:
    // func:acr_compl.FSsimfile..Ctor
    inline               FSsimfile() __attribute__((nothrow));
    // func:acr_compl.FSsimfile..Dtor
    inline               ~FSsimfile() __attribute__((nothrow));
    friend acr_compl::FSsimfile& ssimfile_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_compl::FSsimfile* ssimfile_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 ssimfile_RemoveLast() __attribute__((nothrow));
};
```

#### acr_compl.FSubstr - Specify that the field value is computed from a substring of another field
<a href="#acr_compl-fsubstr"></a>

#### acr_compl.FSubstr Fields
<a href="#acr_compl-fsubstr-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_compl.FSubstr.base|[dmmeta.Substr](/txt/ssimdb/dmmeta/substr.md)|[Base](/txt/ssimdb/dmmeta/substr.md)|||

#### Struct FSubstr
<a href="#struct-fsubstr"></a>
*Note:* field ``acr_compl.FSubstr.base`` has reftype ``base`` so the fields of [dmmeta.Substr](/txt/ssimdb/dmmeta/substr.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_compl_gen.h](/include/gen/acr_compl_gen.h)
```
struct FSubstr { // acr_compl.FSubstr
    algo::Smallstr150   field;      //
    algo::CppExpr       expr;       //
    algo::Smallstr150   srcfield;   //
private:
    // func:acr_compl.FSubstr..Ctor
    inline               FSubstr() __attribute__((nothrow));
    friend acr_compl::FSubstr&  substr_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend acr_compl::FSubstr*  substr_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 substr_RemoveAll() __attribute__((nothrow));
    friend void                 substr_RemoveLast() __attribute__((nothrow));
};
```

#### acr_compl.Shellqtype
<a href="#acr_compl-shellqtype"></a>

#### acr_compl.Shellqtype Fields
<a href="#acr_compl-shellqtype-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_compl.Shellqtype.type|u8|[Val](/txt/exe/amc/reftype.md#val)|||

#### Struct Shellqtype
<a href="#struct-shellqtype"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_compl_gen.h](/include/gen/acr_compl_gen.h)
```
struct Shellqtype { // acr_compl.Shellqtype
    u8   type;   //   0
    // func:acr_compl.Shellqtype..Ctor
    inline               Shellqtype() __attribute__((nothrow));
    // func:acr_compl.Shellqtype..FieldwiseCtor
    explicit inline               Shellqtype(u8 in_type) __attribute__((nothrow));
    // func:acr_compl.Shellqtype..EnumCtor
    inline               Shellqtype(acr_compl_ShellqtypeEnum arg) __attribute__((nothrow));
};
```

#### acr_compl.checkerr - Batch check failure
<a href="#acr_compl-checkerr"></a>

#### acr_compl.checkerr Fields
<a href="#acr_compl-checkerr-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_compl.checkerr.id|u32|[Val](/txt/exe/amc/reftype.md#val)||Echoed from checkreq|
|acr_compl.checkerr.err|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||Validation error text|

#### Struct checkerr
<a href="#struct-checkerr"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_compl_gen.h](/include/gen/acr_compl_gen.h)
```
struct checkerr { // acr_compl.checkerr: Batch check failure
    u32             id;    //   0  Echoed from checkreq
    algo::cstring   err;   // Validation error text
    // func:acr_compl.checkerr..Ctor
    inline               checkerr() __attribute__((nothrow));
};
```

#### acr_compl.checkreq - Batch check request
<a href="#acr_compl-checkreq"></a>

#### acr_compl.checkreq Fields
<a href="#acr_compl-checkreq-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|acr_compl.checkreq.id|u32|[Val](/txt/exe/amc/reftype.md#val)||Echoed in checkerr|
|acr_compl.checkreq.line|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftype.md#val)||Command line to validate|

#### Struct checkreq
<a href="#struct-checkreq"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/acr_compl_gen.h](/include/gen/acr_compl_gen.h)
```
struct checkreq { // acr_compl.checkreq: Batch check request
    u32             id;     //   0  Echoed in checkerr
    algo::cstring   line;   // Command line to validate
    // func:acr_compl.checkreq..Ctor
    inline               checkreq() __attribute__((nothrow));
};
```

### Tests
<a href="#tests"></a>
The following component tests are defined for `acr_compl`.
These can be executed with `atf_comp <comptest> -v`
|Comptest|Comment|
|---|---|
|[acr_compl.A01a](/test/atf_comp/acr_compl.A01a)||
|[acr_compl.A01b](/test/atf_comp/acr_compl.A01b)||
|[acr_compl.A01c](/test/atf_comp/acr_compl.A01c)||
|[acr_compl.A02a](/test/atf_comp/acr_compl.A02a)||
|[acr_compl.A02b](/test/atf_comp/acr_compl.A02b)||
|[acr_compl.A02c](/test/atf_comp/acr_compl.A02c)||
|[acr_compl.A03a](/test/atf_comp/acr_compl.A03a)||
|[acr_compl.A03b](/test/atf_comp/acr_compl.A03b)||
|[acr_compl.A03c](/test/atf_comp/acr_compl.A03c)||
|[acr_compl.A04a](/test/atf_comp/acr_compl.A04a)||
|[acr_compl.A04b](/test/atf_comp/acr_compl.A04b)||
|[acr_compl.A04c](/test/atf_comp/acr_compl.A04c)||
|[acr_compl.A05a](/test/atf_comp/acr_compl.A05a)||
|[acr_compl.A05b](/test/atf_comp/acr_compl.A05b)||
|[acr_compl.A05c](/test/atf_comp/acr_compl.A05c)||
|[acr_compl.A06a](/test/atf_comp/acr_compl.A06a)||
|[acr_compl.A06b](/test/atf_comp/acr_compl.A06b)||
|[acr_compl.A06c](/test/atf_comp/acr_compl.A06c)||
|[acr_compl.A07a](/test/atf_comp/acr_compl.A07a)||
|[acr_compl.A07b](/test/atf_comp/acr_compl.A07b)||
|[acr_compl.A07c](/test/atf_comp/acr_compl.A07c)||
|[acr_compl.Acr01](/test/atf_comp/acr_compl.Acr01)||
|[acr_compl.Acr02](/test/atf_comp/acr_compl.Acr02)||
|[acr_compl.Acr03](/test/atf_comp/acr_compl.Acr03)||
|[acr_compl.Acr04](/test/atf_comp/acr_compl.Acr04)||
|[acr_compl.Acr05](/test/atf_comp/acr_compl.Acr05)||
|[acr_compl.Acr06](/test/atf_comp/acr_compl.Acr06)||
|[acr_compl.Acr07](/test/atf_comp/acr_compl.Acr07)||
|[acr_compl.Acr08](/test/atf_comp/acr_compl.Acr08)||
|[acr_compl.Acr09](/test/atf_comp/acr_compl.Acr09)||
|[acr_compl.Acr10](/test/atf_comp/acr_compl.Acr10)||
|[acr_compl.Acr11](/test/atf_comp/acr_compl.Acr11)||
|[acr_compl.Acr12](/test/atf_comp/acr_compl.Acr12)||
|[acr_compl.Acr13](/test/atf_comp/acr_compl.Acr13)||
|[acr_compl.BadExe](/test/atf_comp/acr_compl.BadExe)||
|[acr_compl.BadOpt](/test/atf_comp/acr_compl.BadOpt)||
|[acr_compl.BadOptColon](/test/atf_comp/acr_compl.BadOptColon)||
|[acr_compl.BadOptColonSpace](/test/atf_comp/acr_compl.BadOptColonSpace)||
|[acr_compl.BadOptSpace](/test/atf_comp/acr_compl.BadOptSpace)||
|[acr_compl.Bare](/test/atf_comp/acr_compl.Bare)||
|[acr_compl.CheckBatch](/test/atf_comp/acr_compl.CheckBatch)||
|[acr_compl.CheckBatchAnon](/test/atf_comp/acr_compl.CheckBatchAnon)|each batch line is parsed from scratch; a -- on one line does not disable option checks on the next|
|[acr_compl.CheckBatchBadReq](/test/atf_comp/acr_compl.CheckBatchBadReq)|an unparsed request row is reported and fails the run; an empty line is not a request|
|[acr_compl.CheckBatchCheck](/test/atf_comp/acr_compl.CheckBatchCheck)|-check_batch with -check is rejected: only one validation mode runs|
|[acr_compl.CheckBatchCmd](/test/atf_comp/acr_compl.CheckBatchCmd)|each batch line is checked against its own command; the completion set does not carry over|
|[acr_compl.CheckBatchCompLine](/test/atf_comp/acr_compl.CheckBatchCompLine)|an inherited COMP_LINE does not discard the -check_batch on the command line; env prefix cannot be valgrind-wrapped|
|[acr_compl.CheckBatchInstall](/test/atf_comp/acr_compl.CheckBatchInstall)|-check_batch with -install is rejected: two writers of one response stream|
|[acr_compl.CheckBatchSchemaStdin](/test/atf_comp/acr_compl.CheckBatchSchemaStdin)|-check_batch with -schema:- or -data:- is rejected: two readers of one stdin|
|[acr_compl.CheckMultiOpt](/test/atf_comp/acr_compl.CheckMultiOpt)||
|[acr_compl.CheckUnknownCmd](/test/atf_comp/acr_compl.CheckUnknownCmd)||
|[acr_compl.CheckUnknownOpt](/test/atf_comp/acr_compl.CheckUnknownOpt)||
|[acr_compl.CheckValid](/test/atf_comp/acr_compl.CheckValid)||
|[acr_compl.CheckValidAnon](/test/atf_comp/acr_compl.CheckValidAnon)||
|[acr_compl.CheckValidFlag](/test/atf_comp/acr_compl.CheckValidFlag)||
|[acr_compl.DblColon](/test/atf_comp/acr_compl.DblColon)||
|[acr_compl.DblColonList](/test/atf_comp/acr_compl.DblColonList)||
|[acr_compl.DblSpace](/test/atf_comp/acr_compl.DblSpace)||
|[acr_compl.DblSpaceList](/test/atf_comp/acr_compl.DblSpaceList)||
|[acr_compl.EnumCtypeColon](/test/atf_comp/acr_compl.EnumCtypeColon)||
|[acr_compl.EnumCtypeColonList](/test/atf_comp/acr_compl.EnumCtypeColonList)||
|[acr_compl.EnumCtypeSpace](/test/atf_comp/acr_compl.EnumCtypeSpace)||
|[acr_compl.EnumCtypeSpaceList](/test/atf_comp/acr_compl.EnumCtypeSpaceList)||
|[acr_compl.EnumFieldColon](/test/atf_comp/acr_compl.EnumFieldColon)||
|[acr_compl.EnumFieldColonList](/test/atf_comp/acr_compl.EnumFieldColonList)||
|[acr_compl.EnumFieldSpace](/test/atf_comp/acr_compl.EnumFieldSpace)||
|[acr_compl.EnumFieldSpaceList](/test/atf_comp/acr_compl.EnumFieldSpaceList)||
|[acr_compl.FlagColon](/test/atf_comp/acr_compl.FlagColon)||
|[acr_compl.FlagColonList](/test/atf_comp/acr_compl.FlagColonList)||
|[acr_compl.FlagSpace](/test/atf_comp/acr_compl.FlagSpace)||
|[acr_compl.FlagSpaceList](/test/atf_comp/acr_compl.FlagSpaceList)||
|[acr_compl.InsertWhenLeft](/test/atf_comp/acr_compl.InsertWhenLeft)||
|[acr_compl.InsertWhenLeftPrefix](/test/atf_comp/acr_compl.InsertWhenLeftPrefix)||
|[acr_compl.InsertWhenLookup](/test/atf_comp/acr_compl.InsertWhenLookup)||
|[acr_compl.InsertWhenRight](/test/atf_comp/acr_compl.InsertWhenRight)||
|[acr_compl.Install](/test/atf_comp/acr_compl.Install)||
|[acr_compl.NumColon](/test/atf_comp/acr_compl.NumColon)||
|[acr_compl.NumColonList](/test/atf_comp/acr_compl.NumColonList)||
|[acr_compl.NumSpace](/test/atf_comp/acr_compl.NumSpace)||
|[acr_compl.NumSpaceList](/test/atf_comp/acr_compl.NumSpaceList)||
|[acr_compl.OptCumul](/test/atf_comp/acr_compl.OptCumul)||
|[acr_compl.OptCumulAlias](/test/atf_comp/acr_compl.OptCumulAlias)||
|[acr_compl.OptD](/test/atf_comp/acr_compl.OptD)||
|[acr_compl.OptDList](/test/atf_comp/acr_compl.OptDList)||
|[acr_compl.OptH](/test/atf_comp/acr_compl.OptH)||
|[acr_compl.OptHList](/test/atf_comp/acr_compl.OptHList)||
|[acr_compl.OptNonCumul](/test/atf_comp/acr_compl.OptNonCumul)||
|[acr_compl.OptNonCumulAlias](/test/atf_comp/acr_compl.OptNonCumulAlias)||
|[acr_compl.OptPkeyColon](/test/atf_comp/acr_compl.OptPkeyColon)||
|[acr_compl.OptPkeyColonFull](/test/atf_comp/acr_compl.OptPkeyColonFull)||
|[acr_compl.OptPkeyColonFullList](/test/atf_comp/acr_compl.OptPkeyColonFullList)||
|[acr_compl.OptPkeyColonList](/test/atf_comp/acr_compl.OptPkeyColonList)||
|[acr_compl.OptPkeyColonPrefix](/test/atf_comp/acr_compl.OptPkeyColonPrefix)||
|[acr_compl.OptPkeyColonPrefixList](/test/atf_comp/acr_compl.OptPkeyColonPrefixList)||
|[acr_compl.OptPkeyColonSubstr](/test/atf_comp/acr_compl.OptPkeyColonSubstr)||
|[acr_compl.OptPkeyColonSubstrList](/test/atf_comp/acr_compl.OptPkeyColonSubstrList)||
|[acr_compl.OptPkeySpace](/test/atf_comp/acr_compl.OptPkeySpace)||
|[acr_compl.OptPkeySpaceFull](/test/atf_comp/acr_compl.OptPkeySpaceFull)||
|[acr_compl.OptPkeySpaceFullList](/test/atf_comp/acr_compl.OptPkeySpaceFullList)||
|[acr_compl.OptPkeySpaceList](/test/atf_comp/acr_compl.OptPkeySpaceList)||
|[acr_compl.OptPkeySpacePrefix](/test/atf_comp/acr_compl.OptPkeySpacePrefix)||
|[acr_compl.OptPkeySpacePrefixList](/test/atf_comp/acr_compl.OptPkeySpacePrefixList)||
|[acr_compl.OptPkeySpaceSubstr](/test/atf_comp/acr_compl.OptPkeySpaceSubstr)||
|[acr_compl.OptPkeySpaceSubstrList](/test/atf_comp/acr_compl.OptPkeySpaceSubstrList)||
|[acr_compl.OptRegxColon](/test/atf_comp/acr_compl.OptRegxColon)||
|[acr_compl.OptRegxColonFull](/test/atf_comp/acr_compl.OptRegxColonFull)||
|[acr_compl.OptRegxColonFullList](/test/atf_comp/acr_compl.OptRegxColonFullList)||
|[acr_compl.OptRegxColonList](/test/atf_comp/acr_compl.OptRegxColonList)||
|[acr_compl.OptRegxColonPrefix](/test/atf_comp/acr_compl.OptRegxColonPrefix)||
|[acr_compl.OptRegxColonPrefixList](/test/atf_comp/acr_compl.OptRegxColonPrefixList)||
|[acr_compl.OptRegxColonSubstr](/test/atf_comp/acr_compl.OptRegxColonSubstr)||
|[acr_compl.OptRegxColonSubstrList](/test/atf_comp/acr_compl.OptRegxColonSubstrList)||
|[acr_compl.OptRegxSpace](/test/atf_comp/acr_compl.OptRegxSpace)||
|[acr_compl.OptRegxSpaceFull](/test/atf_comp/acr_compl.OptRegxSpaceFull)||
|[acr_compl.OptRegxSpaceFullList](/test/atf_comp/acr_compl.OptRegxSpaceFullList)||
|[acr_compl.OptRegxSpaceList](/test/atf_comp/acr_compl.OptRegxSpaceList)||
|[acr_compl.OptRegxSpacePrefix](/test/atf_comp/acr_compl.OptRegxSpacePrefix)||
|[acr_compl.OptRegxSpacePrefixList](/test/atf_comp/acr_compl.OptRegxSpacePrefixList)||
|[acr_compl.OptRegxSpaceSubstr](/test/atf_comp/acr_compl.OptRegxSpaceSubstr)||
|[acr_compl.OptRegxSpaceSubstrList](/test/atf_comp/acr_compl.OptRegxSpaceSubstrList)||
|[acr_compl.OptSig](/test/atf_comp/acr_compl.OptSig)||
|[acr_compl.OptSigList](/test/atf_comp/acr_compl.OptSigList)||
|[acr_compl.OptV](/test/atf_comp/acr_compl.OptV)||
|[acr_compl.OptVList](/test/atf_comp/acr_compl.OptVList)||
|[acr_compl.R01a](/test/atf_comp/acr_compl.R01a)||
|[acr_compl.R01b](/test/atf_comp/acr_compl.R01b)||
|[acr_compl.R01c](/test/atf_comp/acr_compl.R01c)||
|[acr_compl.R01d](/test/atf_comp/acr_compl.R01d)||
|[acr_compl.R02a](/test/atf_comp/acr_compl.R02a)||
|[acr_compl.R02b](/test/atf_comp/acr_compl.R02b)||
|[acr_compl.R02c](/test/atf_comp/acr_compl.R02c)||
|[acr_compl.R02d](/test/atf_comp/acr_compl.R02d)||
|[acr_compl.StrColon](/test/atf_comp/acr_compl.StrColon)||
|[acr_compl.StrColonList](/test/atf_comp/acr_compl.StrColonList)||
|[acr_compl.StrSpace](/test/atf_comp/acr_compl.StrSpace)||
|[acr_compl.StrSpaceList](/test/atf_comp/acr_compl.StrSpaceList)||
|[acr_compl.T01](/test/atf_comp/acr_compl.T01)||
|[acr_compl.T02](/test/atf_comp/acr_compl.T02)||
|[acr_compl.T03](/test/atf_comp/acr_compl.T03)||
|[acr_compl.T04](/test/atf_comp/acr_compl.T04)||
|[acr_compl.T05](/test/atf_comp/acr_compl.T05)||
|[acr_compl.T06](/test/atf_comp/acr_compl.T06)||
|[acr_compl.T07](/test/atf_comp/acr_compl.T07)||
|[acr_compl.T08](/test/atf_comp/acr_compl.T08)||
|[acr_compl.T09](/test/atf_comp/acr_compl.T09)||
|[acr_compl.T10](/test/atf_comp/acr_compl.T10)||

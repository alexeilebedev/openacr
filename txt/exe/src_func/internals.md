## src_func - Internals


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
for usage, see [src_func - Access / edit functions](/txt/exe/src_func/README.md)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Description -->

### Sources
<a href="#sources"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Sources -->
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/gen/src_func_gen.cpp](/cpp/gen/src_func_gen.cpp)||
|[cpp/src_func/edit.cpp](/cpp/src_func/edit.cpp)|Implementation of -e|
|[cpp/src_func/fileloc.cpp](/cpp/src_func/fileloc.cpp)|Location in file, for each function|
|[cpp/src_func/header.cpp](/cpp/src_func/header.cpp)|Generate prototypes in header|
|[cpp/src_func/iffy.cpp](/cpp/src_func/iffy.cpp)|Check iffy function prototypes|
|[cpp/src_func/list.cpp](/cpp/src_func/list.cpp)|Print list of functions|
|[cpp/src_func/main.cpp](/cpp/src_func/main.cpp)|Main file|
|[cpp/src_func/nextfile.cpp](/cpp/src_func/nextfile.cpp)|Find next file in target|
|[cpp/src_func/scan.cpp](/cpp/src_func/scan.cpp)|Scan sources to collect functions|
|[include/gen/src_func_gen.h](/include/gen/src_func_gen.h)||
|[include/gen/src_func_gen.inl.h](/include/gen/src_func_gen.inl.h)||
|[include/src_func.h](/include/src_func.h)||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Sources -->

### Dependencies
<a href="#dependencies"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Dependencies -->
The build target depends on the following libraries
|Target|Comment|
|---|---|
|[algo_lib](/txt/lib/algo_lib/README.md)|Support library for all executables|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Dependencies -->

### In Memory DB
<a href="#in-memory-db"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Imdb -->
`src_func` generated code creates the tables below.
All allocations are done through global `src_func::_db` [src_func.FDb](#src_func-fdb) structure
|Ctype|Ssimfile|Create|Access|
|---|---|---|---|
|[src_func.FCtypelen](#src_func-fctypelen)|[dmmeta.ctypelen](/txt/ssimdb/dmmeta/ctypelen.md)|FDb.ctypelen (Lary)|ctypelen (Lary, by rowid)|ind_ctypelen (Thash, hash field ctype)|
|[src_func.FDb](#src_func-fdb)||FDb._db (Global)|
|[src_func.FDispatch](#src_func-fdispatch)|[dmmeta.dispatch](/txt/ssimdb/dmmeta/dispatch.md)|FDb.dispatch (Lary)|dispatch (Lary, by rowid)|
|[src_func.FFstep](#src_func-ffstep)|[dmmeta.fstep](/txt/ssimdb/dmmeta/fstep.md)|FDb.fstep (Lary)|fstep (Lary, by rowid)|
|[src_func.FFunc](#src_func-ffunc)||FDb.func (Lary)|func (Lary, by rowid)|ind_func (Thash, hash field func)|bh_func (Bheap, sort field sortkey)|
||||FTargsrc.zd_func (Llist)|
|[src_func.FGenprefix](#src_func-fgenprefix)||FDb.genprefix (Lary)|genprefix (Lary, by rowid)|ind_genprefix (Thash, hash field genprefix)|
|[src_func.FGstatic](#src_func-fgstatic)|[dmmeta.gstatic](/txt/ssimdb/dmmeta/gstatic.md)|FDb.gstatic (Lary)|gstatic (Lary, by rowid)|
|[src_func.FTarget](#src_func-ftarget)|[dev.target](/txt/ssimdb/dev/target.md)|FDb.target (Lary)|target (Lary, by rowid)|ind_target (Thash, hash field target)|
||||FTargsrc.p_target (Upptr)|
|[src_func.FTargsrc](#src_func-ftargsrc)|[dev.targsrc](/txt/ssimdb/dev/targsrc.md)|FDb.targsrc (Lary)|targsrc (Lary, by rowid)|c_cur_targsrc (Ptr)|
||||FFunc.p_targsrc (Upptr)|
||||FFunc.p_written_to (Upptr)|
||||FTarget.cd_targsrc (Llist)|

#### src_func.FCtypelen - Size of Ctype
<a href="#src_func-fctypelen"></a>

#### src_func.FCtypelen Fields
<a href="#src_func-fctypelen-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|src_func.FCtypelen.base|[dmmeta.Ctypelen](/txt/ssimdb/dmmeta/ctypelen.md)|[Base](/txt/ssimdb/dmmeta/ctypelen.md)|||

#### Struct FCtypelen
<a href="#struct-fctypelen"></a>
*Note:* field ``src_func.FCtypelen.base`` has reftype ``base`` so the fields of [dmmeta.Ctypelen](/txt/ssimdb/dmmeta/ctypelen.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/src_func_gen.h](/include/gen/src_func_gen.h)
```
struct FCtypelen { // src_func.FCtypelen
    algo::Smallstr100      ctype;               // Identifies the Ctype
    u32                    len;                 //   0  (calculated) length of the C++ struct in bytes
    i32                    alignment;           //   0  (calculated) alignment for the struct
    i32                    padbytes;            //   0  (calculated) total # of pad bytes
    bool                   plaindata;           //   false  (calculated) this struct can me safely memcpy'ed
    src_func::FCtypelen*   ind_ctypelen_next;   // hash next
    // func:src_func.FCtypelen..AssignOp
    inline src_func::FCtypelen& operator =(const src_func::FCtypelen &rhs) = delete;
    // func:src_func.FCtypelen..CopyCtor
    inline               FCtypelen(const src_func::FCtypelen &rhs) = delete;
private:
    // func:src_func.FCtypelen..Ctor
    inline               FCtypelen() __attribute__((nothrow));
    // func:src_func.FCtypelen..Dtor
    inline               ~FCtypelen() __attribute__((nothrow));
    friend src_func::FCtypelen& ctypelen_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend src_func::FCtypelen* ctypelen_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 ctypelen_RemoveAll() __attribute__((nothrow));
    friend void                 ctypelen_RemoveLast() __attribute__((nothrow));
};
```

#### src_func.FDb - In-memory database for src_func
<a href="#src_func-fdb"></a>

#### src_func.FDb Fields
<a href="#src_func-fdb-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|src_func.FDb.report|[report.src_func](/txt/protocol/report/README.md#report-src_func)|[Val](/txt/exe/amc/reftypes.md#val)||Final report|
|src_func.FDb.genprefix|[src_func.FGenprefix](/txt/exe/src_func/internals.md#src_func-fgenprefix)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|src_func.FDb._db|[src_func.FDb](/txt/exe/src_func/internals.md#src_func-fdb)|[Global](/txt/exe/amc/reftypes.md#global)|||
|src_func.FDb.cmdline|[command.src_func](/txt/protocol/command/README.md#command-src_func)|[Val](/txt/exe/amc/reftypes.md#val)|||
|src_func.FDb.targsrc|[src_func.FTargsrc](/txt/exe/src_func/internals.md#src_func-ftargsrc)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|src_func.FDb.target|[src_func.FTarget](/txt/exe/src_func/internals.md#src_func-ftarget)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|src_func.FDb.ind_target|[src_func.FTarget](/txt/exe/src_func/internals.md#src_func-ftarget)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|src_func.FDb.func|[src_func.FFunc](/txt/exe/src_func/internals.md#src_func-ffunc)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|src_func.FDb.ind_func|[src_func.FFunc](/txt/exe/src_func/internals.md#src_func-ffunc)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|src_func.FDb.bh_func|[src_func.FFunc](/txt/exe/src_func/internals.md#src_func-ffunc)|[Bheap](/txt/exe/amc/reftypes.md#bheap)|||
|src_func.FDb.dispatch|[src_func.FDispatch](/txt/exe/src_func/internals.md#src_func-fdispatch)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|src_func.FDb.c_cur_targsrc|[src_func.FTargsrc](/txt/exe/src_func/internals.md#src_func-ftargsrc)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|src_func.FDb.cur_line|i32|[Val](/txt/exe/amc/reftypes.md#val)|||
|src_func.FDb.ignore_funcstart|[algo_lib.Regx](/txt/lib/algo_lib/README.md#algo_lib-regx)|[Val](/txt/exe/amc/reftypes.md#val)|||
|src_func.FDb.fstep|[src_func.FFstep](/txt/exe/src_func/internals.md#src_func-ffstep)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|src_func.FDb.gstatic|[src_func.FGstatic](/txt/exe/src_func/internals.md#src_func-fgstatic)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|src_func.FDb.ind_genprefix|[src_func.FGenprefix](/txt/exe/src_func/internals.md#src_func-fgenprefix)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|src_func.FDb.ctypelen|[src_func.FCtypelen](/txt/exe/src_func/internals.md#src_func-fctypelen)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|src_func.FDb.ind_ctypelen|[src_func.FCtypelen](/txt/exe/src_func/internals.md#src_func-fctypelen)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|src_func.FDb.printed_user_impl_notice|bool|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FDb
<a href="#struct-fdb"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/src_func_gen.h](/include/gen/src_func_gen.h)
```
struct FDb { // src_func.FDb: In-memory database for src_func
    report::src_func         report;                        // Final report
    src_func::FGenprefix*    genprefix_lary[32];            // level array
    i32                      genprefix_n;                   // number of elements in array
    command::src_func        cmdline;                       //
    src_func::FTargsrc*      targsrc_lary[32];              // level array
    i32                      targsrc_n;                     // number of elements in array
    src_func::FTarget*       target_lary[32];               // level array
    i32                      target_n;                      // number of elements in array
    src_func::FTarget**      ind_target_buckets_elems;      // pointer to bucket array
    i32                      ind_target_buckets_n;          // number of elements in bucket array
    i32                      ind_target_n;                  // number of elements in the hash table
    src_func::FFunc*         func_lary[32];                 // level array
    i32                      func_n;                        // number of elements in array
    src_func::FFunc**        ind_func_buckets_elems;        // pointer to bucket array
    i32                      ind_func_buckets_n;            // number of elements in bucket array
    i32                      ind_func_n;                    // number of elements in the hash table
    src_func::FFunc**        bh_func_elems;                 // binary heap by sortkey
    i32                      bh_func_n;                     // number of elements in the heap
    i32                      bh_func_max;                   // max elements in bh_func_elems
    src_func::FDispatch*     dispatch_lary[32];             // level array
    i32                      dispatch_n;                    // number of elements in array
    src_func::FTargsrc*      c_cur_targsrc;                 // optional pointer
    i32                      cur_line;                      //   0
    algo_lib::Regx           ignore_funcstart;              //
    src_func::FFstep*        fstep_lary[32];                // level array
    i32                      fstep_n;                       // number of elements in array
    src_func::FGstatic*      gstatic_lary[32];              // level array
    i32                      gstatic_n;                     // number of elements in array
    src_func::FGenprefix**   ind_genprefix_buckets_elems;   // pointer to bucket array
    i32                      ind_genprefix_buckets_n;       // number of elements in bucket array
    i32                      ind_genprefix_n;               // number of elements in the hash table
    src_func::FCtypelen*     ctypelen_lary[32];             // level array
    i32                      ctypelen_n;                    // number of elements in array
    src_func::FCtypelen**    ind_ctypelen_buckets_elems;    // pointer to bucket array
    i32                      ind_ctypelen_buckets_n;        // number of elements in bucket array
    i32                      ind_ctypelen_n;                // number of elements in the hash table
    bool                     printed_user_impl_notice;      //   false
    src_func::trace          trace;                         //
};
```

#### src_func.FDispatch - Generate code for a multi-way branch
<a href="#src_func-fdispatch"></a>

#### src_func.FDispatch Fields
<a href="#src_func-fdispatch-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|src_func.FDispatch.base|[dmmeta.Dispatch](/txt/ssimdb/dmmeta/dispatch.md)|[Base](/txt/ssimdb/dmmeta/dispatch.md)|||

#### Struct FDispatch
<a href="#struct-fdispatch"></a>
*Note:* field ``src_func.FDispatch.base`` has reftype ``base`` so the fields of [dmmeta.Dispatch](/txt/ssimdb/dmmeta/dispatch.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/src_func_gen.h](/include/gen/src_func_gen.h)
```
struct FDispatch { // src_func.FDispatch
    algo::Smallstr50   dispatch;   // Primary key (ns.name)
    bool               unk;        //   false  Want default case?
    bool               read;       //   false  Generate read function
    bool               print;      //   false  Generate print function
    bool               haslen;     //   false  Include length in dispatch function
    bool               call;       //   false  Generate call to user-defined function
    bool               strict;     //   false  Only dispatch if length matches exactly
    algo::Comment      comment;    //
private:
    // func:src_func.FDispatch..Ctor
    inline               FDispatch() __attribute__((nothrow));
    friend src_func::FDispatch& dispatch_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend src_func::FDispatch* dispatch_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 dispatch_RemoveAll() __attribute__((nothrow));
    friend void                 dispatch_RemoveLast() __attribute__((nothrow));
};
```

#### src_func.FFstep - Generate a main loop step to be executed whenever a field is non-empty
<a href="#src_func-ffstep"></a>

#### src_func.FFstep Fields
<a href="#src_func-ffstep-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|src_func.FFstep.base|[dmmeta.Fstep](/txt/ssimdb/dmmeta/fstep.md)|[Base](/txt/ssimdb/dmmeta/fstep.md)|||

#### Struct FFstep
<a href="#struct-ffstep"></a>
*Note:* field ``src_func.FFstep.base`` has reftype ``base`` so the fields of [dmmeta.Fstep](/txt/ssimdb/dmmeta/fstep.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/src_func_gen.h](/include/gen/src_func_gen.h)
```
struct FFstep { // src_func.FFstep
    algo::Smallstr100   fstep;      //
    algo::Smallstr50    steptype;   //
    algo::Comment       comment;    //
private:
    // func:src_func.FFstep..Ctor
    inline               FFstep() __attribute__((nothrow));
    friend src_func::FFstep&    fstep_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend src_func::FFstep*    fstep_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 fstep_RemoveAll() __attribute__((nothrow));
    friend void                 fstep_RemoveLast() __attribute__((nothrow));
};
```

#### src_func.FFunc - 
<a href="#src_func-ffunc"></a>

#### src_func.FFunc Fields
<a href="#src_func-ffunc-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|src_func.FFunc.func|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||First line of function -- primary key|
|src_func.FFunc.args|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Argument list -- everything beyond first open parenthesis|
|src_func.FFunc.sortkey|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|src_func.FFunc.body|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Function body|
|src_func.FFunc.p_targsrc|[src_func.FTargsrc](/txt/exe/src_func/internals.md#src_func-ftargsrc)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|src_func.FFunc.line|i32|[Val](/txt/exe/amc/reftypes.md#val)||Line where function started|
|src_func.FFunc.precomment|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Comments that appeare before function body|
|src_func.FFunc.isstatic|bool|[Val](/txt/exe/amc/reftypes.md#val)||Static?|
|src_func.FFunc.isinline|bool|[Val](/txt/exe/amc/reftypes.md#val)||Inline?|
|src_func.FFunc.amcprot|bool|[Val](/txt/exe/amc/reftypes.md#val)||Looks like a function where amc has already provided prototype?|
|src_func.FFunc.select|bool|[Val](/txt/exe/amc/reftypes.md#val)||Select for further processing|
|src_func.FFunc.iffy|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|src_func.FFunc.mystery|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|src_func.FFunc.p_written_to|[src_func.FTargsrc](/txt/exe/src_func/internals.md#src_func-ftargsrc)|[Upptr](/txt/exe/amc/reftypes.md#upptr)||If non-NULL, header where this function is written|

#### Struct FFunc
<a href="#struct-ffunc"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/src_func_gen.h](/include/gen/src_func_gen.h)
```
struct FFunc { // src_func.FFunc
    src_func::FFunc*      ind_func_next;   // hash next
    i32                   bh_func_idx;     // index in heap; -1 means not-in-heap
    algo::cstring         func;            // First line of function -- primary key
    algo::cstring         args;            // Argument list -- everything beyond first open parenthesis
    algo::cstring         sortkey;         //
    algo::cstring         body;            // Function body
    src_func::FTargsrc*   p_targsrc;       // reference to parent row
    i32                   line;            //   0  Line where function started
    algo::cstring         precomment;      // Comments that appeare before function body
    bool                  isstatic;        //   false  Static?
    bool                  isinline;        //   false  Inline?
    bool                  amcprot;         //   false  Looks like a function where amc has already provided prototype?
    bool                  select;          //   false  Select for further processing
    bool                  iffy;            //   false
    bool                  mystery;         //   false
    src_func::FTargsrc*   p_written_to;    // reference to parent row
    src_func::FFunc*      zd_func_next;    // zslist link; -1 means not-in-list
    src_func::FFunc*      zd_func_prev;    // previous element
    // func:src_func.FFunc..AssignOp
    src_func::FFunc&     operator =(const src_func::FFunc &rhs) = delete;
    // func:src_func.FFunc..CopyCtor
    FFunc(const src_func::FFunc &rhs) = delete;
private:
    // func:src_func.FFunc..Ctor
    inline               FFunc() __attribute__((nothrow));
    // func:src_func.FFunc..Dtor
    inline               ~FFunc() __attribute__((nothrow));
    friend src_func::FFunc&     func_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend src_func::FFunc*     func_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 func_RemoveAll() __attribute__((nothrow));
    friend void                 func_RemoveLast() __attribute__((nothrow));
};
```

#### src_func.FGenprefix - Prefixes of generated functions, skip generating prototypes for those
<a href="#src_func-fgenprefix"></a>

#### src_func.FGenprefix Fields
<a href="#src_func-fgenprefix-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|src_func.FGenprefix.genprefix|[dmmeta.Field](/txt/ssimdb/dmmeta/field.md)|[Pkey](/txt/exe/amc/reftypes.md#pkey)|||

#### Struct FGenprefix
<a href="#struct-fgenprefix"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/src_func_gen.h](/include/gen/src_func_gen.h)
```
struct FGenprefix { // src_func.FGenprefix: Prefixes of generated functions, skip generating prototypes for those
    src_func::FGenprefix*   ind_genprefix_next;   // hash next
    algo::Smallstr100       genprefix;            //
    // func:src_func.FGenprefix..AssignOp
    inline src_func::FGenprefix& operator =(const src_func::FGenprefix &rhs) = delete;
    // func:src_func.FGenprefix..CopyCtor
    inline               FGenprefix(const src_func::FGenprefix &rhs) = delete;
private:
    // func:src_func.FGenprefix..Ctor
    inline               FGenprefix() __attribute__((nothrow));
    // func:src_func.FGenprefix..Dtor
    inline               ~FGenprefix() __attribute__((nothrow));
    friend src_func::FGenprefix& genprefix_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend src_func::FGenprefix* genprefix_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 genprefix_RemoveAll() __attribute__((nothrow));
    friend void                 genprefix_RemoveLast() __attribute__((nothrow));
};
```

#### src_func.FGstatic - Load entries for this table at startup time
<a href="#src_func-fgstatic"></a>

#### src_func.FGstatic Fields
<a href="#src_func-fgstatic-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|src_func.FGstatic.base|[dmmeta.Gstatic](/txt/ssimdb/dmmeta/gstatic.md)|[Base](/txt/ssimdb/dmmeta/gstatic.md)|||

#### Struct FGstatic
<a href="#struct-fgstatic"></a>
*Note:* field ``src_func.FGstatic.base`` has reftype ``base`` so the fields of [dmmeta.Gstatic](/txt/ssimdb/dmmeta/gstatic.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/src_func_gen.h](/include/gen/src_func_gen.h)
```
struct FGstatic { // src_func.FGstatic
    algo::Smallstr100   field;     //
    algo::Comment       comment;   //
private:
    // func:src_func.FGstatic..Ctor
    inline               FGstatic() __attribute__((nothrow));
    friend src_func::FGstatic&  gstatic_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend src_func::FGstatic*  gstatic_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 gstatic_RemoveAll() __attribute__((nothrow));
    friend void                 gstatic_RemoveLast() __attribute__((nothrow));
};
```

#### src_func.FTarget - Build target
<a href="#src_func-ftarget"></a>

#### src_func.FTarget Fields
<a href="#src_func-ftarget-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|src_func.FTarget.base|[dev.Target](/txt/ssimdb/dev/target.md)|[Base](/txt/ssimdb/dev/target.md)|||
|src_func.FTarget.cd_targsrc|[src_func.FTargsrc](/txt/exe/src_func/internals.md#src_func-ftargsrc)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|src_func.FTarget.select|bool|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FTarget
<a href="#struct-ftarget"></a>
*Note:* field ``src_func.FTarget.base`` has reftype ``base`` so the fields of [dev.Target](/txt/ssimdb/dev/target.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/src_func_gen.h](/include/gen/src_func_gen.h)
```
struct FTarget { // src_func.FTarget
    src_func::FTarget*    ind_target_next;   // hash next
    algo::Smallstr16      target;            // Primary key - name of target
    src_func::FTargsrc*   cd_targsrc_head;   // zero-terminated doubly linked list
    i32                   cd_targsrc_n;      // zero-terminated doubly linked list
    bool                  select;            //   false
    // reftype Llist of src_func.FTarget.cd_targsrc prohibits copy
    // func:src_func.FTarget..AssignOp
    inline src_func::FTarget& operator =(const src_func::FTarget &rhs) = delete;
    // reftype Llist of src_func.FTarget.cd_targsrc prohibits copy
    // func:src_func.FTarget..CopyCtor
    inline               FTarget(const src_func::FTarget &rhs) = delete;
private:
    // func:src_func.FTarget..Ctor
    inline               FTarget() __attribute__((nothrow));
    // func:src_func.FTarget..Dtor
    inline               ~FTarget() __attribute__((nothrow));
    friend src_func::FTarget&   target_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend src_func::FTarget*   target_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 target_RemoveLast() __attribute__((nothrow));
};
```

#### src_func.FTargsrc - List of sources for target
<a href="#src_func-ftargsrc"></a>

#### src_func.FTargsrc Fields
<a href="#src_func-ftargsrc-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|src_func.FTargsrc.base|[dev.Targsrc](/txt/ssimdb/dev/targsrc.md)|[Base](/txt/ssimdb/dev/targsrc.md)|||
|src_func.FTargsrc.zd_func|[src_func.FFunc](/txt/exe/src_func/internals.md#src_func-ffunc)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|src_func.FTargsrc.p_target|[src_func.FTarget](/txt/exe/src_func/internals.md#src_func-ftarget)|[Upptr](/txt/exe/amc/reftypes.md#upptr)|||
|src_func.FTargsrc.select|bool|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FTargsrc
<a href="#struct-ftargsrc"></a>
*Note:* field ``src_func.FTargsrc.base`` has reftype ``base`` so the fields of [dev.Targsrc](/txt/ssimdb/dev/targsrc.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/src_func_gen.h](/include/gen/src_func_gen.h)
```
struct FTargsrc { // src_func.FTargsrc
    src_func::FTargsrc*   cd_targsrc_next;   // zslist link; -1 means not-in-list
    src_func::FTargsrc*   cd_targsrc_prev;   // previous element
    algo::Smallstr100     targsrc;           //
    algo::Comment         comment;           //
    src_func::FFunc*      zd_func_head;      // zero-terminated doubly linked list
    i32                   zd_func_n;         // zero-terminated doubly linked list
    src_func::FFunc*      zd_func_tail;      // pointer to last element
    src_func::FTarget*    p_target;          // reference to parent row
    bool                  select;            //   false
    // reftype Llist of src_func.FTargsrc.zd_func prohibits copy
    // x-reference on src_func.FTargsrc.p_target prevents copy
    // func:src_func.FTargsrc..AssignOp
    inline src_func::FTargsrc& operator =(const src_func::FTargsrc &rhs) = delete;
    // reftype Llist of src_func.FTargsrc.zd_func prohibits copy
    // x-reference on src_func.FTargsrc.p_target prevents copy
    // func:src_func.FTargsrc..CopyCtor
    inline               FTargsrc(const src_func::FTargsrc &rhs) = delete;
private:
    // func:src_func.FTargsrc..Ctor
    inline               FTargsrc() __attribute__((nothrow));
    // func:src_func.FTargsrc..Dtor
    inline               ~FTargsrc() __attribute__((nothrow));
    friend src_func::FTargsrc&  targsrc_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend src_func::FTargsrc*  targsrc_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 targsrc_RemoveLast() __attribute__((nothrow));
};
```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Imdb -->

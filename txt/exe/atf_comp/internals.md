## atf_comp - Internals


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
for usage, see [atf_comp - Algo Test Framework - Component test execution](/txt/exe/atf_comp/README.md)

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Description -->

### Sources
<a href="#sources"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Sources -->
The source code license is GPL
The following source files are part of this tool:

|Source File|Comment|
|---|---|
|[cpp/atf_comp/edit.cpp](/cpp/atf_comp/edit.cpp)|Edit comptest|
|[cpp/atf_comp/main.cpp](/cpp/atf_comp/main.cpp)||
|[cpp/atf_comp/read.cpp](/cpp/atf_comp/read.cpp)|Read comptest from input|
|[cpp/gen/atf_comp_gen.cpp](/cpp/gen/atf_comp_gen.cpp)||
|[include/atf_comp.h](/include/atf_comp.h)||
|[include/gen/atf_comp_gen.h](/include/gen/atf_comp_gen.h)||
|[include/gen/atf_comp_gen.inl.h](/include/gen/atf_comp_gen.inl.h)||

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Sources -->

### Dependencies
<a href="#dependencies"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Dependencies -->
The build target depends on the following libraries
|Target|Comment|
|---|---|
|[algo_lib](/txt/lib/algo_lib/README.md)|Support library for all executables|
|[lib_ctype](/txt/lib/lib_ctype/README.md)|Helpful library for reading/writing ctypes as text tuples|
|[lib_prot](/txt/lib/lib_prot/README.md)|Library covering all protocols|

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Dependencies -->

### In Memory DB
<a href="#in-memory-db"></a>
<!-- dev.mdmark  mdmark:MDSECTION  state:BEG_AUTO  param:Imdb -->
`atf_comp` generated code creates the tables below.
All allocations are done through global `atf_comp::_db` [atf_comp.FDb](#atf_comp-fdb) structure
|Ctype|Ssimfile|Create|Access|
|---|---|---|---|
|[atf_comp.FComptest](#atf_comp-fcomptest)|[atfdb.comptest](/txt/ssimdb/atfdb/comptest.md)|FDb.comptest (Lary)|comptest (Lary, by rowid)|ind_comptest (Thash, hash field comptest)|zd_sel_comptest (Llist)|zd_run_comptest (Llist)|zd_out_comptest (Llist)|
|[atf_comp.FDb](#atf_comp-fdb)||FDb._db (Global)|
|[atf_comp.FTargs](#atf_comp-ftargs)|[atfdb.targs](/txt/ssimdb/atfdb/targs.md)|FDb.targs (Tpool)|zd_out_targs (Llist)|
||||FComptest.c_targs (Ptr)|
|[atf_comp.FTfilt](#atf_comp-ftfilt)|[atfdb.tfilt](/txt/ssimdb/atfdb/tfilt.md)|FDb.tfilt (Tpool)|zd_out_tfilt (Llist)|
||||FComptest.c_tfilt (Ptr)|
|[atf_comp.FTmsg](#atf_comp-ftmsg)|[atfdb.tmsg](/txt/ssimdb/atfdb/tmsg.md)|FDb.tmsg (Tpool)|zd_out_tmsg (Llist)|
||||FComptest.zd_tmsg (Llist)|

#### atf_comp.FComptest - 
<a href="#atf_comp-fcomptest"></a>

#### atf_comp.FComptest Fields
<a href="#atf_comp-fcomptest-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_comp.FComptest.base|[atfdb.Comptest](/txt/ssimdb/atfdb/comptest.md)|[Base](/txt/ssimdb/atfdb/comptest.md)|||
|atf_comp.FComptest.file_test_out|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_comp.FComptest.file_test_in|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_comp.FComptest.bash|[command.bash_proc](/txt/protocol/command/README.md#command-bash_proc)|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_comp.FComptest.c_targs|[atf_comp.FTargs](/txt/exe/atf_comp/internals.md#atf_comp-ftargs)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|atf_comp.FComptest.msg_index|i32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_comp.FComptest.c_tfilt|[atf_comp.FTfilt](/txt/exe/atf_comp/internals.md#atf_comp-ftfilt)|[Ptr](/txt/exe/amc/reftypes.md#ptr)|||
|atf_comp.FComptest.start|[algo.SchedTime](/txt/protocol/algo/SchedTime.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_comp.FComptest.finish|[algo.SchedTime](/txt/protocol/algo/SchedTime.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_comp.FComptest.elapsed|double|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_comp.FComptest.thook|[algo_lib.FTimehook](/txt/lib/algo_lib/README.md#algo_lib-ftimehook)|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_comp.FComptest.success|bool|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_comp.FComptest.file_memcheck|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_comp.FComptest.file_callgrind_log|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_comp.FComptest.file_callgrind_out|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_comp.FComptest.covdir|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_comp.FComptest.zd_tmsg|[atf_comp.FTmsg](/txt/exe/atf_comp/internals.md#atf_comp-ftmsg)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|atf_comp.FComptest.need_write|bool|[Val](/txt/exe/amc/reftypes.md#val)||Component test modified during runtime, needs to be written back|
|atf_comp.FComptest.err|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)||Error string|
|atf_comp.FComptest.filter_command|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||

#### Struct FComptest
<a href="#struct-fcomptest"></a>
*Note:* field ``atf_comp.FComptest.base`` has reftype ``base`` so the fields of [atfdb.Comptest](/txt/ssimdb/atfdb/comptest.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_comp_gen.h](/include/gen/atf_comp_gen.h)
```
struct FComptest { // atf_comp.FComptest
    algo::Smallstr50       comptest;               //
    i32                    timeout;                //   10
    bool                   memcheck;               //   true
    u8                     exit_code;              //   0  Exit code to check
    algo::Comment          comment;                //
    algo::cstring          file_test_out;          //
    algo::cstring          file_test_in;           //
    command::bash_proc     bash;                   //
    atf_comp::FTargs*      c_targs;                // optional pointer
    i32                    msg_index;              //   0
    atf_comp::FTfilt*      c_tfilt;                // optional pointer
    algo::SchedTime        start;                  //
    algo::SchedTime        finish;                 //
    double                 elapsed;                //   0.0
    algo_lib::FTimehook    thook;                  //
    bool                   success;                //   false
    algo::cstring          file_memcheck;          //
    algo::cstring          file_callgrind_log;     //
    algo::cstring          file_callgrind_out;     //
    algo::cstring          covdir;                 //
    atf_comp::FTmsg*       zd_tmsg_head;           // zero-terminated doubly linked list
    i32                    zd_tmsg_n;              // zero-terminated doubly linked list
    atf_comp::FTmsg*       zd_tmsg_tail;           // pointer to last element
    bool                   need_write;             //   false  Component test modified during runtime, needs to be written back
    algo::cstring          err;                    // Error string
    algo::cstring          filter_command;         //
    atf_comp::FComptest*   ind_comptest_next;      // hash next
    atf_comp::FComptest*   zd_sel_comptest_next;   // zslist link; -1 means not-in-list
    atf_comp::FComptest*   zd_sel_comptest_prev;   // previous element
    atf_comp::FComptest*   zd_run_comptest_next;   // zslist link; -1 means not-in-list
    atf_comp::FComptest*   zd_run_comptest_prev;   // previous element
    atf_comp::FComptest*   zd_out_comptest_next;   // zslist link; -1 means not-in-list
    atf_comp::FComptest*   zd_out_comptest_prev;   // previous element
    // value field atf_comp.FComptest.bash is not copiable
    // x-reference on atf_comp.FComptest.c_targs prevents copy
    // x-reference on atf_comp.FComptest.c_tfilt prevents copy
    // value field atf_comp.FComptest.thook is not copiable
    // reftype Llist of atf_comp.FComptest.zd_tmsg prohibits copy
    // func:atf_comp.FComptest..AssignOp
    atf_comp::FComptest& operator =(const atf_comp::FComptest &rhs) = delete;
    // value field atf_comp.FComptest.bash is not copiable
    // x-reference on atf_comp.FComptest.c_targs prevents copy
    // x-reference on atf_comp.FComptest.c_tfilt prevents copy
    // value field atf_comp.FComptest.thook is not copiable
    // reftype Llist of atf_comp.FComptest.zd_tmsg prohibits copy
    // func:atf_comp.FComptest..CopyCtor
    FComptest(const atf_comp::FComptest &rhs) = delete;
private:
    // func:atf_comp.FComptest..Ctor
    inline               FComptest() __attribute__((nothrow));
    // func:atf_comp.FComptest..Dtor
    inline               ~FComptest() __attribute__((nothrow));
    friend atf_comp::FComptest& comptest_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend atf_comp::FComptest* comptest_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 comptest_RemoveAll() __attribute__((nothrow));
    friend void                 comptest_RemoveLast() __attribute__((nothrow));
};
```

#### atf_comp.FDb - In-memory database for atf_comp
<a href="#atf_comp-fdb"></a>

#### atf_comp.FDb Fields
<a href="#atf_comp-fdb-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_comp.FDb._db|[atf_comp.FDb](/txt/exe/atf_comp/internals.md#atf_comp-fdb)|[Global](/txt/exe/amc/reftypes.md#global)|||
|atf_comp.FDb.cmdline|[command.atf_comp](/txt/protocol/command/README.md#command-atf_comp)|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_comp.FDb.tempdir|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|"temp/atf_comp"||
|atf_comp.FDb.comptest|[atf_comp.FComptest](/txt/exe/atf_comp/internals.md#atf_comp-fcomptest)|[Lary](/txt/exe/amc/reftypes.md#lary)|||
|atf_comp.FDb.ind_comptest|[atf_comp.FComptest](/txt/exe/atf_comp/internals.md#atf_comp-fcomptest)|[Thash](/txt/exe/amc/reftypes.md#thash)|||
|atf_comp.FDb.compdir|[algo.cstring](/txt/protocol/algo/cstring.md)|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_comp.FDb.report|[report.atf_comp](/txt/protocol/report/README.md#report-atf_comp)|[Val](/txt/exe/amc/reftypes.md#val)||Final report|
|atf_comp.FDb.zd_sel_comptest|[atf_comp.FComptest](/txt/exe/atf_comp/internals.md#atf_comp-fcomptest)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|atf_comp.FDb.targs|[atf_comp.FTargs](/txt/exe/atf_comp/internals.md#atf_comp-ftargs)|[Tpool](/txt/exe/amc/reftypes.md#tpool)|||
|atf_comp.FDb.tmsg|[atf_comp.FTmsg](/txt/exe/atf_comp/internals.md#atf_comp-ftmsg)|[Tpool](/txt/exe/amc/reftypes.md#tpool)|||
|atf_comp.FDb.tfilt|[atf_comp.FTfilt](/txt/exe/atf_comp/internals.md#atf_comp-ftfilt)|[Tpool](/txt/exe/amc/reftypes.md#tpool)|||
|atf_comp.FDb.zd_run_comptest|[atf_comp.FComptest](/txt/exe/atf_comp/internals.md#atf_comp-fcomptest)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|atf_comp.FDb.zd_out_tmsg|[atf_comp.FTmsg](/txt/exe/atf_comp/internals.md#atf_comp-ftmsg)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|atf_comp.FDb.zd_out_comptest|[atf_comp.FComptest](/txt/exe/atf_comp/internals.md#atf_comp-fcomptest)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|atf_comp.FDb.nchange|i32|[Val](/txt/exe/amc/reftypes.md#val)|||
|atf_comp.FDb.zd_out_tfilt|[atf_comp.FTfilt](/txt/exe/atf_comp/internals.md#atf_comp-ftfilt)|[Llist](/txt/exe/amc/reftypes.md#llist)|||
|atf_comp.FDb.zd_out_targs|[atf_comp.FTargs](/txt/exe/atf_comp/internals.md#atf_comp-ftargs)|[Llist](/txt/exe/amc/reftypes.md#llist)|||

#### Struct FDb
<a href="#struct-fdb"></a>
Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_comp_gen.h](/include/gen/atf_comp_gen.h)
```
struct FDb { // atf_comp.FDb: In-memory database for atf_comp
    command::atf_comp       cmdline;                      //
    algo::cstring           tempdir;                      //   "temp/atf_comp"
    atf_comp::FComptest*    comptest_lary[32];            // level array
    i32                     comptest_n;                   // number of elements in array
    atf_comp::FComptest**   ind_comptest_buckets_elems;   // pointer to bucket array
    i32                     ind_comptest_buckets_n;       // number of elements in bucket array
    i32                     ind_comptest_n;               // number of elements in the hash table
    algo::cstring           compdir;                      //
    report::atf_comp        report;                       // Final report
    atf_comp::FComptest*    zd_sel_comptest_head;         // zero-terminated doubly linked list
    i32                     zd_sel_comptest_n;            // zero-terminated doubly linked list
    atf_comp::FComptest*    zd_sel_comptest_tail;         // pointer to last element
    u64                     targs_blocksize;              // # bytes per block
    atf_comp::FTargs*       targs_free;                   //
    u64                     tmsg_blocksize;               // # bytes per block
    atf_comp::FTmsg*        tmsg_free;                    //
    u64                     tfilt_blocksize;              // # bytes per block
    atf_comp::FTfilt*       tfilt_free;                   //
    atf_comp::FComptest*    zd_run_comptest_head;         // zero-terminated doubly linked list
    i32                     zd_run_comptest_n;            // zero-terminated doubly linked list
    atf_comp::FComptest*    zd_run_comptest_tail;         // pointer to last element
    algo::SchedTime         zd_run_comptest_next;         // atf_comp.FDb.zd_run_comptest          Next invocation time
    algo::SchedTime         zd_run_comptest_delay;        // atf_comp.FDb.zd_run_comptest          Delay between invocations
    atf_comp::FTmsg*        zd_out_tmsg_head;             // zero-terminated doubly linked list
    i32                     zd_out_tmsg_n;                // zero-terminated doubly linked list
    atf_comp::FTmsg*        zd_out_tmsg_tail;             // pointer to last element
    atf_comp::FComptest*    zd_out_comptest_head;         // zero-terminated doubly linked list
    i32                     zd_out_comptest_n;            // zero-terminated doubly linked list
    atf_comp::FComptest*    zd_out_comptest_tail;         // pointer to last element
    i32                     nchange;                      //   0
    atf_comp::FTfilt*       zd_out_tfilt_head;            // zero-terminated doubly linked list
    i32                     zd_out_tfilt_n;               // zero-terminated doubly linked list
    atf_comp::FTfilt*       zd_out_tfilt_tail;            // pointer to last element
    atf_comp::FTargs*       zd_out_targs_head;            // zero-terminated doubly linked list
    i32                     zd_out_targs_n;               // zero-terminated doubly linked list
    atf_comp::FTargs*       zd_out_targs_tail;            // pointer to last element
    atf_comp::trace         trace;                        //
};
```

#### atf_comp.FTargs - 
<a href="#atf_comp-ftargs"></a>

#### atf_comp.FTargs Fields
<a href="#atf_comp-ftargs-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_comp.FTargs.base|[atfdb.Targs](/txt/ssimdb/atfdb/targs.md)|[Base](/txt/ssimdb/atfdb/targs.md)|||

#### Struct FTargs
<a href="#struct-ftargs"></a>
*Note:* field ``atf_comp.FTargs.base`` has reftype ``base`` so the fields of [atfdb.Targs](/txt/ssimdb/atfdb/targs.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_comp_gen.h](/include/gen/atf_comp_gen.h)
```
struct FTargs { // atf_comp.FTargs
    atf_comp::FTargs*   targs_next;          // Pointer to next free element int tpool
    atf_comp::FTargs*   zd_out_targs_next;   // zslist link; -1 means not-in-list
    atf_comp::FTargs*   zd_out_targs_prev;   // previous element
    algo::Smallstr50    comptest;            //
    algo::cstring       args;                //
    // func:atf_comp.FTargs..AssignOp
    inline atf_comp::FTargs& operator =(const atf_comp::FTargs &rhs) = delete;
    // func:atf_comp.FTargs..CopyCtor
    inline               FTargs(const atf_comp::FTargs &rhs) = delete;
private:
    // func:atf_comp.FTargs..Ctor
    inline               FTargs() __attribute__((nothrow));
    // func:atf_comp.FTargs..Dtor
    inline               ~FTargs() __attribute__((nothrow));
    friend atf_comp::FTargs&    targs_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend atf_comp::FTargs*    targs_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 targs_Delete(atf_comp::FTargs &row) __attribute__((nothrow));
};
```

#### atf_comp.FTfilt - 
<a href="#atf_comp-ftfilt"></a>

#### atf_comp.FTfilt Fields
<a href="#atf_comp-ftfilt-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_comp.FTfilt.base|[atfdb.Tfilt](/txt/ssimdb/atfdb/tfilt.md)|[Base](/txt/ssimdb/atfdb/tfilt.md)|||

#### Struct FTfilt
<a href="#struct-ftfilt"></a>
*Note:* field ``atf_comp.FTfilt.base`` has reftype ``base`` so the fields of [atfdb.Tfilt](/txt/ssimdb/atfdb/tfilt.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_comp_gen.h](/include/gen/atf_comp_gen.h)
```
struct FTfilt { // atf_comp.FTfilt
    atf_comp::FTfilt*   tfilt_next;          // Pointer to next free element int tpool
    atf_comp::FTfilt*   zd_out_tfilt_next;   // zslist link; -1 means not-in-list
    atf_comp::FTfilt*   zd_out_tfilt_prev;   // previous element
    algo::Smallstr50    comptest;            //
    algo::cstring       filter;              //
    algo::Comment       comment;             //
    // func:atf_comp.FTfilt..AssignOp
    inline atf_comp::FTfilt& operator =(const atf_comp::FTfilt &rhs) = delete;
    // func:atf_comp.FTfilt..CopyCtor
    inline               FTfilt(const atf_comp::FTfilt &rhs) = delete;
private:
    // func:atf_comp.FTfilt..Ctor
    inline               FTfilt() __attribute__((nothrow));
    // func:atf_comp.FTfilt..Dtor
    inline               ~FTfilt() __attribute__((nothrow));
    friend atf_comp::FTfilt&    tfilt_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend atf_comp::FTfilt*    tfilt_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 tfilt_Delete(atf_comp::FTfilt &row) __attribute__((nothrow));
};
```

#### atf_comp.FTmsg - 
<a href="#atf_comp-ftmsg"></a>

#### atf_comp.FTmsg Fields
<a href="#atf_comp-ftmsg-fields"></a>
|Field|[Type](/txt/ssimdb/dmmeta/ctype.md)|[Reftype](/txt/ssimdb/dmmeta/reftype.md)|Default|Comment|
|---|---|---|---|---|
|atf_comp.FTmsg.base|[atfdb.Tmsg](/txt/ssimdb/atfdb/tmsg.md)|[Base](/txt/ssimdb/atfdb/tmsg.md)|||

#### Struct FTmsg
<a href="#struct-ftmsg"></a>
*Note:* field ``atf_comp.FTmsg.base`` has reftype ``base`` so the fields of [atfdb.Tmsg](/txt/ssimdb/atfdb/tmsg.md) above are included into the resulting struct.

Generated by [amc](/txt/exe/amc/README.md) into [include/gen/atf_comp_gen.h](/include/gen/atf_comp_gen.h)
```
struct FTmsg { // atf_comp.FTmsg
    atf_comp::FTmsg*   zd_tmsg_next;       // zslist link; -1 means not-in-list
    atf_comp::FTmsg*   zd_tmsg_prev;       // previous element
    atf_comp::FTmsg*   tmsg_next;          // Pointer to next free element int tpool
    atf_comp::FTmsg*   zd_out_tmsg_next;   // zslist link; -1 means not-in-list
    atf_comp::FTmsg*   zd_out_tmsg_prev;   // previous element
    algo::Smallstr50   tmsg;               //
    bool               istuple;            //   false
    algo::cstring      msg;                //
    // func:atf_comp.FTmsg..AssignOp
    inline atf_comp::FTmsg& operator =(const atf_comp::FTmsg &rhs) = delete;
    // func:atf_comp.FTmsg..CopyCtor
    inline               FTmsg(const atf_comp::FTmsg &rhs) = delete;
private:
    // func:atf_comp.FTmsg..Ctor
    inline               FTmsg() __attribute__((nothrow));
    // func:atf_comp.FTmsg..Dtor
    inline               ~FTmsg() __attribute__((nothrow));
    friend atf_comp::FTmsg&     tmsg_Alloc() __attribute__((__warn_unused_result__, nothrow));
    friend atf_comp::FTmsg*     tmsg_AllocMaybe() __attribute__((__warn_unused_result__, nothrow));
    friend void                 tmsg_Delete(atf_comp::FTmsg &row) __attribute__((nothrow));
};
```

<!-- dev.mdmark  mdmark:MDSECTION  state:END_AUTO  param:Imdb -->

